#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <cstdint>

#include <string>
#include <vector>
#include <map>

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

namespace clardis
{

bool parse_long(const char *s, long *n);
long now_us();
void log(const char *fmt, ...);

}
