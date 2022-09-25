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
}
