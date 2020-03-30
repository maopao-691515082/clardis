#include "db.h"

namespace clardis
{

struct DbSlot
{
    pthread_mutex_t wr_lock;

    std::map<std::string, std::string> kvs;
};

static const size_t DB_SLOT_COUNT = 1024 * 1024;

static DbSlot db_slots[DB_SLOT_COUNT];

void init_db()
{
    for (size_t i = 0; i < DB_SLOT_COUNT; ++ i)
    {
        assert(pthread_mutex_init(&db_slots[i].wr_lock, NULL) == 0);
    }
}

static DbSlot &get_slot_by_key(const std::string &k)
{
    size_t db_slot_idx = key_hash(k) % DB_SLOT_COUNT;
    return db_slots[db_slot_idx];
}

void do_write_cmd(const std::string &k, std::function<std::string(const std::string *)> write_value)
{
    DbSlot &slot = get_slot_by_key(k);

    assert(pthread_mutex_lock(&slot.wr_lock) == 0);

    std::map<std::string, std::string>::iterator it = slot.kvs.find(k);
    std::string new_v = write_value(it == slot.kvs.end() ? nullptr : &it->second);
    slot.kvs[k] = new_v;

    assert(pthread_mutex_unlock(&slot.wr_lock) == 0);
}

void do_read_cmd(const std::string &k, std::function<void(const std::string *)> read_value)
{
    DbSlot &slot = get_slot_by_key(k);

    assert(pthread_mutex_lock(&slot.wr_lock) == 0);

    std::map<std::string, std::string>::iterator it = slot.kvs.find(k);
    read_value(it == slot.kvs.end() ? nullptr : &it->second);

    assert(pthread_mutex_unlock(&slot.wr_lock) == 0);
}

}
