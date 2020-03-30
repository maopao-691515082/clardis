#include "str_cmds.h"
#include "db.h"

namespace clardis
{

void proc_cmd_SET(Client *client, const std::string &cmd, const std::vector<std::string> &args)
{
    const std::string &k = args[1];
    const std::string &v = args[2];
    do_write_cmd(k, [&](const std::string *old_v) -> std::string {
        return v;
    });
    client->reply_ok();
}

void proc_cmd_GET(Client *client, const std::string &cmd, const std::vector<std::string> &args)
{
    const std::string &k = args[1];
    do_read_cmd(k, [&](const std::string *old_v) {
        if (old_v == nullptr)
        {
            client->reply_nil();
        }
        else
        {
            client->reply_str(old_v->data(), old_v->size());
        }
    });
}

}
