#include <cstring>
#include "defs.h"
namespace Glob
{
    int cfd = 0;
    JSON &user_config()
    {
        static JSON ret;
        return ret;
    }
    const char *remote_ip = "127.0.0.1";
    const int port = 6832;
    int message_cnt = 0;

    std::condition_variable notifier;
    std::mutex message_mutex;

    // ms
    int max_wait_time = 5000;

    bool vis[VIS_LEN];

    std::string cur_chat_people;
    std::map<std::string, std::queue<JSON>> msg_tab;
    std::mutex msg_tab_mutex;
    std::condition_variable msg_tab_cv;

    std::map<int, JSON> response_tab;
}
