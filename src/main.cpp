#include "util.h"
#include "cmd.h"
#include "worker.h"
#include "server.h"

namespace clardis
{

int main(int argc, char *argv[])
{
    assert(argc == 2);
    long worker_count;
    assert(parse_long(argv[1], &worker_count));
    init_cmd_infos();
    init_workers(worker_count);
    start_server();

    return 0;
}

}

int main(int argc, char *argv[])
{
    return clardis::main(argc, argv);
}
