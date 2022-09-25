#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include "defs.h"
#include <cstring>
#include <unistd.h>
#include <mutex>

int set_up_connection()
{
    Glob::cfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_addr.s_addr = inet_addr(Glob::remote_ip);
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(Glob::port);

    if (connect(Glob::cfd, (sockaddr *)&saddr, sizeof(saddr)) != -1)
    {
        return 0;
    }
    return -1;
}

void send_json(JSON json)
{
    std::string data = json.to_string();
    int len = data.size();
    len = htonl(len);
    write(Glob::cfd, &len, sizeof(len));
    write(Glob::cfd, data.c_str(), ntohl(len));
}

JSON recv_json()
{
    int len;
    char buf[4096];
    read(Glob::cfd, (void *)&len, sizeof(int));
    len = ntohl(len);
    int cnt = 0;
    while (cnt < len)
        cnt += read(Glob::cfd, buf, len - cnt);
    buf[cnt] = 0;
    return JSON(buf);
}