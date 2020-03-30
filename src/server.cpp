#include "util.h"
#include "server.h"
#include "worker.h"

namespace clardis
{

void start_server()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd >= 0);

    int reuseaddr_on = 1;
    assert(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on)) == 0);

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(6379);

    assert(bind(listen_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) == 0);
    assert(listen(listen_fd, 1024) == 0);

    for (;;)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        assert(client_fd >= 0);
        assert(evutil_make_socket_nonblocking(client_fd) == 0);

        deliver_client(client_fd);
        log("client[%d] delivered", client_fd);
    }
}

}
