#include "util.h"
#include "client.h"
#include "cmd.h"

namespace clardis
{

const char *Client::parse_num(const char *p, size_t sz, char prefix, long *pn)
{
    assert(*p == prefix);
    size_t cr_idx;
    for (cr_idx = 1; cr_idx < sz - 1; ++ cr_idx)
    {
        if (p[cr_idx] == '\r')
        {
            assert(p[cr_idx + 1] == '\n');
            break;
        }
    }
    assert(cr_idx < 32);
    if (cr_idx == sz - 1)
    {
        //数字字段不完整，移动到buf最前面并返回nullptr
        memcpy(this->req_buf, p, sz);
        this->req_buf_curr_sz = sz;
        return nullptr;
    }
    assert(cr_idx < sz - 1);
    char s[32];
    memcpy(s, p + 1, cr_idx - 1);
    s[cr_idx - 1] = '\0';
    assert(parse_long(s, pn));
    return p + cr_idx + 2;
}

void Client::proc_req()
{
    assert(this->req.size() > 0);
    std::string cmd = this->req[0];
    size_t cmd_len = cmd.size();
    for (size_t i = 0; i < cmd_len; ++ i)
    {
        char c = cmd[i];
        if (c >= 'a' && c <= 'z')
        {
            c -= 'a' - 'A';
        }
        cmd[i] = c;
    }
    const CmdInfo *cmd_info = get_cmd_info(cmd);
    if (cmd_info == nullptr)
    {
        this->reply_err("invalid cmd");
        return;
    }
    int arg_count = this->req.size() - 1;
    if ((cmd_info->arg_count > 0 && arg_count != cmd_info->arg_count) || (cmd_info->arg_count <= 0 && arg_count < -cmd_info->arg_count))
    {
        this->reply_err("cmd arg count error");
        return;
    }

    cmd_info->proc_cmd(this, cmd, this->req);
}

void Client::proc_req_buf()
{
    const char *p = this->req_buf;
    size_t sz = this->req_buf_curr_sz;
    while (sz > 0)
    {
        switch (this->req_parser_stat)
        {
            case REQ_PARSER_STAT_START:
            {
                long fc;
                const char *new_p = this->parse_num(p, sz, '*', &fc);
                if (new_p == nullptr)
                {
                    return;
                }
                size_t diff_sz = new_p - p;
                assert(diff_sz <= sz);
                p = new_p;
                sz -= diff_sz;
                assert(fc > 0);
                this->req.clear();
                this->field_count = fc;
                this->req_parser_stat = REQ_PARSER_STAT_NEW_FIELD;
                break;
            }
            case REQ_PARSER_STAT_NEW_FIELD:
            {
                long fl;
                const char *new_p = this->parse_num(p, sz, '$', &fl);
                if (new_p == nullptr)
                {
                    return;
                }
                size_t diff_sz = new_p - p;
                assert(diff_sz <= sz);
                p = new_p;
                sz -= diff_sz;
                assert(fl >= 0);
                fl += 2; //include \r\n
                this->req.push_back(std::string());
                this->last_field_remain_len = fl;
                this->req_parser_stat = REQ_PARSER_STAT_PARSING_FIELD;
                break;
            }
            case REQ_PARSER_STAT_PARSING_FIELD:
            {
                assert(this->req.size() > 0);
                std::string &last_field = this->req[this->req.size() - 1];
                if (sz < this->last_field_remain_len)
                {
                    last_field.append(p, sz);
                    this->last_field_remain_len -= sz;
                    p = nullptr;
                    sz = 0;
                    break;
                }
                last_field.append(p, this->last_field_remain_len);
                size_t last_field_len = last_field.size();
                assert(last_field_len >= 2);
                assert(last_field[last_field_len - 2] == '\r' && last_field[last_field_len - 1] == '\n');
                last_field.resize(last_field_len - 2);
                p += this->last_field_remain_len;
                sz -= this->last_field_remain_len;
                this->last_field_remain_len = 0;
                if (this->req.size() == this->field_count)
                {
                    this->proc_req();
                    this->req_parser_stat = REQ_PARSER_STAT_START;
                }
                else
                {
                    assert(this->req.size() < this->field_count);
                    this->req_parser_stat = REQ_PARSER_STAT_NEW_FIELD;
                }

                break;
            }
            default:
            {
                assert(false);
            }
        }
    }
    this->req_buf_curr_sz = 0;
}

void Client::reply_raw(const char *rsp, size_t sz)
{
    assert(bufferevent_write(this->buf_ev, rsp, sz) == 0);
}

Client::Client(int fd, struct bufferevent *buf_ev)
{
    this->fd        = fd;
    this->buf_ev    = buf_ev;

    this->req_buf_curr_sz = 0;

    this->req_parser_stat = REQ_PARSER_STAT_START;
}

Client::~Client()
{
    bufferevent_free(buf_ev);
    close(this->fd);
}

void Client::on_data_arrived()
{
    for (;;)
    {
        assert(this->req_buf_curr_sz < REQ_BUF_MAX_SIZE);
        size_t sz = bufferevent_read(this->buf_ev, this->req_buf + this->req_buf_curr_sz, REQ_BUF_MAX_SIZE - this->req_buf_curr_sz);
        if (sz <= 0)
        {
            return;
        }
        this->req_buf_curr_sz += sz;
        this->proc_req_buf();
    }
}

void Client::reply_ok()
{
    this->reply_raw("+OK\r\n", 5);
}

void Client::reply_err(const char *msg)
{
    this->reply_raw("-ERR ", 5);
    this->reply_raw(msg, strlen(msg));
    this->reply_raw("\r\n", 2);
}

void Client::reply_wrong_type()
{
    const char *msg = "-WRONGTYPE wrongtype\r\n";
    this->reply_raw(msg, strlen(msg));
}

void Client::reply_nil()
{
    this->reply_raw("$-1\r\n", 5);
}

void Client::reply_int(long n)
{
    char msg[32];
    sprintf(msg, ":%ld\r\n", n);
    this->reply_raw(msg, strlen(msg));
}

void Client::reply_item_count(long n)
{
    char msg[32];
    sprintf(msg, "*%ld\r\n", n);
    this->reply_raw(msg, strlen(msg));
}

void Client::reply_str(const char *s, size_t len)
{
    char msg[32];
    sprintf(msg, "$%zu\r\n", len);
    this->reply_raw(msg, strlen(msg));
    this->reply_raw(s, len);
    this->reply_raw("\r\n", 2);
}

}
