#pragma once

namespace clardis
{

static const size_t REQ_BUF_MAX_SIZE = 1024;

enum ReqParserStat
{
    REQ_PARSER_STAT_START,
    REQ_PARSER_STAT_NEW_FIELD,
    REQ_PARSER_STAT_PARSING_FIELD,
};

class Client
{
    int fd;
    struct bufferevent *buf_ev;

    char req_buf[REQ_BUF_MAX_SIZE];
    size_t req_buf_curr_sz;

    ReqParserStat req_parser_stat;
    std::vector<std::string> req;
    size_t field_count;
    size_t last_field_remain_len;

    const char *parse_num(const char *p, size_t sz, char prefix, long *pn);
    void proc_req();
    void proc_req_buf();
    void reply_raw(const char *rsp, size_t sz);

public:

    Client(int fd, struct bufferevent *buf_ev);
    ~Client();
    void on_data_arrived();
    void reply_ok();
    void reply_err(const char *msg);
    void reply_wrong_type();
    void reply_nil();
    void reply_int(long n);
    void reply_item_count(long n);
    void reply_str(const char *s, size_t len);
};

}
