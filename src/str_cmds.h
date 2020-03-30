#pragma once

#include "util.h"
#include "client.h"

namespace clardis
{

void proc_cmd_SET(Client *client, const std::string &cmd, const std::vector<std::string> &args);
void proc_cmd_GET(Client *client, const std::string &cmd, const std::vector<std::string> &args);

}
