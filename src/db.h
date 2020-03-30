#pragma once

#include "util.h"

namespace clardis
{

void init_db();
void do_write_cmd(const std::string &k, std::function<std::string(const std::string *)> write_value);
void do_read_cmd(const std::string &k, std::function<void(const std::string *)> read_value);

}
