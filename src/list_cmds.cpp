#include "list_cmds.h"

namespace clardis
{

void proc_cmd_LPUSH(Client *client, const std::string &cmd, const std::vector<std::string> &args)
{
    client->reply_err("this is lpush cmd");
    //todo
}

void proc_cmd_LRANGE(Client *client, const std::string &cmd, const std::vector<std::string> &args)
{
    client->reply_err("this is lrange cmd");
    //todo
}

}
