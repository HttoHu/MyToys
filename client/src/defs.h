#pragma once
#include "json.h"
#include <condition_variable>
#include <mutex>

#define VIS_LEN 4096
void send_json(JSON json);
JSON recv_json();

namespace Glob
{
    extern int cfd;
    extern const char *remote_ip;
    extern const int port;
    extern int message_cnt;
    JSON &user_config();

    extern std::condition_variable notifier;
    extern std::mutex message_mutex;
    // ms
    extern int max_wait_time;
    extern bool vis[VIS_LEN];
    extern std::map<std::string, JSON> msg_tab;
    extern std::map<int, JSON> response_tab;
}
