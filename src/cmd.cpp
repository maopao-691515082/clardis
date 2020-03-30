#include "cmd.h"
#include "str_cmds.h"
#include "list_cmds.h"

namespace clardis
{

static const CmdInfo cmd_infos[] = {
    {"SET",     2,  proc_cmd_SET},
    {"GET",     1,  proc_cmd_GET},
    {"LPUSH",   -2, proc_cmd_LPUSH},
    {"LRANGE",  3,  proc_cmd_LRANGE},
};

static std::map<std::string, CmdInfo> cmd_info_map;

void init_cmd_infos()
{
    for (size_t i = 0; i < sizeof(cmd_infos) / sizeof(CmdInfo); ++ i)
    {
        const CmdInfo &cmd_info = cmd_infos[i];
        cmd_info_map[cmd_info.cmd] = cmd_info;
    }
}

const CmdInfo *get_cmd_info(const std::string &cmd)
{
    std::map<std::string, CmdInfo>::iterator it = cmd_info_map.find(cmd);
    return it == cmd_info_map.end() ? nullptr : &it->second;
}

}
