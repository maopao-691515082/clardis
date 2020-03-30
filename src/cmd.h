#pragma once

#include "util.h"
#include "client.h"

namespace clardis
{

struct CmdInfo
{
    const char *cmd;
    int arg_count;
    void (*proc_cmd)(Client *client, const std::string &cmd, const std::vector<std::string> &args);
};

void init_cmd_infos();
const CmdInfo *get_cmd_info(const std::string &cmd);

}
