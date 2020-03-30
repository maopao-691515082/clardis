#pragma once

#include "util.h"
#include "client.h"

namespace clardis
{

void proc_cmd_LPUSH(Client *client, const std::string &cmd, const std::vector<std::string> &args);
void proc_cmd_LRANGE(Client *client, const std::string &cmd, const std::vector<std::string> &args);

}
