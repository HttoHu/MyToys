#pragma once
#include "json.h"
#include <condition_variable>
#include <mutex>
#include <queue>

#define VIS_LEN 4096
void send_json(JSON json);

JSON recv_json();

void rev_msg();

void chat_msg_listenner();

std::optional<JSON> wait_response(int no);
namespace Chat
{
    void push_chat_message(const std::string &friend_name, const std::string &msg);
}
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

    extern std::string cur_chat_people;
    extern std::mutex msg_tab_mutex;
    extern std::condition_variable msg_tab_cv;

    extern std::map<std::string, std::queue<JSON>> msg_tab;
    extern std::map<int, JSON> response_tab;
}
