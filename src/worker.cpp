#include "util.h"
#include "worker.h"
#include "client.h"

namespace clardis
{

class Worker
{
    long idx;
    int client_deliver_fd_r, client_deliver_fd_w;
    struct event_base *ev_base;

    static void on_client_data_arrived(struct bufferevent *buf_ev, void *arg)
    {
        Client *client = (Client *)arg;
        client->on_data_arrived();
    }

    static void on_client_ev(struct bufferevent *buf_ev, short what, void *arg)
    {
        Client *client = (Client *)arg;

        if (what & BEV_EVENT_EOF)
        {
            log("client eof");
        }
        if (what & BEV_EVENT_ERROR)
        {
            log("client error");
        }

        delete client;
    }

    static void on_new_client(int fd, short ev, void *arg)
    {
        Worker *worker = (Worker *)arg;

        int client_fd;
        assert(read(worker->client_deliver_fd_r, &client_fd, sizeof(client_fd)) == sizeof(client_fd));
        log("client[%d] recved by worker #%ld", client_fd, worker->idx);

        struct bufferevent *buf_ev = bufferevent_socket_new(worker->ev_base, client_fd, 0);
        assert(buf_ev != nullptr);
        Client *client = new Client(client_fd, buf_ev);
        bufferevent_setcb(buf_ev, Worker::on_client_data_arrived, nullptr, Worker::on_client_ev, client);
        assert(bufferevent_enable(buf_ev, EV_READ) == 0);
    }

    static void *thread_main(void *arg)
    {
        Worker *worker = (Worker *)arg;

        worker->ev_base = event_base_new();

        struct event *ev_new_client = (
            event_new(worker->ev_base, worker->client_deliver_fd_r, EV_READ | EV_PERSIST, Worker::on_new_client, worker));
        assert(ev_new_client != nullptr);
        event_add(ev_new_client, nullptr);

        event_base_dispatch(worker->ev_base);

        return nullptr;
    }

public:

    void init(long idx)
    {
        this->idx = idx;

        int fds[2];
        assert(pipe(fds) == 0);
        this->client_deliver_fd_r = fds[0];
        assert(evutil_make_socket_nonblocking(this->client_deliver_fd_r) == 0);
        this->client_deliver_fd_w = fds[1];
        assert(evutil_make_socket_nonblocking(this->client_deliver_fd_w) == 0);

        pthread_t tid;
        assert(pthread_create(&tid, nullptr, Worker::thread_main, this) == 0);

        log("worker #%ld started", this->idx);
    }

    void deliver_client(int client_fd)
    {
        assert(write(this->client_deliver_fd_w, &client_fd, sizeof(client_fd)) == sizeof(client_fd));
    }
};

static long worker_count;
static Worker *workers;

void init_workers(long count)
{
    worker_count = count;
    assert(worker_count > 0 && worker_count <= 128);
    workers = new Worker[worker_count];
    for (long i = 0; i < worker_count; ++ i)
    {
        workers[i].init(i);
    }
}

void deliver_client(int client_fd)
{
    workers[(long)rand() % worker_count].deliver_client(client_fd);
}

}
