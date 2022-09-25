#pragma once
#include "json.h"

void send_json(JSON json);
JSON recv_json();

namespace Glob
{
    extern int cfd;
    extern const char *remote_ip;
    extern const int port;
    extern int message_cnt;
    JSON &user_config();
}
