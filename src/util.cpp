#include "util.h"

namespace clardis
{

bool parse_long(const char *s, long *n)
{
    char *end_ptr = nullptr;
    *n = strtol(s, &end_ptr, 10);
    if (*s == '\0' || *end_ptr != '\0')
    {
        return false;
    }
    return true;
}

long now_us()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void log(const char *fmt, ...)
{
    printf("[%ld] ", now_us());

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("\n");
}

size_t key_hash(const std::string &s)
{
    size_t sz = s.size();
    size_t h = sz;
    for (size_t i = 0; i < sz; ++ i)
    {
        h = (h + (unsigned char)s[i]) * 1000003;
    }
    return h;
}

}
