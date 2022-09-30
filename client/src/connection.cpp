#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include "defs.h"
#include <cstring>
#include <unistd.h>
#include <mutex>
#include <chrono>

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

// 收到服务器的所有消息在这里处理
void rev_msg()
{
    while (true)
    {
        auto json = recv_json();
        std::string type = json["type"].get_str();
        // std::cout << json.to_string() << "\n";
        if (type == "response")
        {
            {
                int no = json["no"].get_int();
                Glob::vis[no % VIS_LEN] = 1;
                // std::cout << "Set " << no << "\n";
                std::lock_guard<std::mutex> lk(Glob::message_mutex);
                Glob::response_tab[no % VIS_LEN] = json;
            }
            Glob::notifier.notify_all();
        }
        else
        {
            bool okay = false;
            {
                std::lock_guard<std::mutex> lk(Glob::msg_tab_mutex);
                // sender username
                std::string from = json["from"].get_str();
                Glob::msg_tab[from].push(json);
                okay = Glob::cur_chat_people.size();
            }
            if (okay)
            {
                Glob::msg_tab_cv.notify_all();
            }
        }
    }
}
// 监听收到的聊天消息
void chat_msg_listenner()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(Glob::msg_tab_mutex);
        Glob::msg_tab_cv.wait(lk);
        auto &q = Glob::msg_tab[Glob::cur_chat_people];
        while (!q.empty())
        {
            auto msg = std::move(q.front());
            q.pop();
            std::string out_msg = "[" + Glob::cur_chat_people + "]:" + msg["message"].get_str();
            std::cout << out_msg << std::endl;
            Chat::push_chat_message(Glob::cur_chat_people, out_msg);
        }
    }
}

JSON wait_response(int no)
{
    using namespace std::chrono_literals;

    auto timeout = std::chrono::steady_clock::now() +
                   5000ms;

    JSON ret;
    // lock scope
    {
        std::unique_lock<std::mutex> lk(Glob::message_mutex);
        if (!Glob::notifier.wait_until(lk, timeout, [&]() -> bool
                                       { return Glob::vis[no % VIS_LEN]; }))
        {
            return ret;
        }

        ret = Glob::response_tab[no % VIS_LEN];
        Glob::vis[no % VIS_LEN] = 0;
    }
    return ret;
}
// 监听单个从服务器发来的消息

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