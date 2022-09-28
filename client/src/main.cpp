#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <thread>

#include "ui.h"
#include "defs.h"
int set_up_connection();

void rev_msg()
{
    while (true)
    {
        auto json = recv_json();

        {
            std::string type = json["type"].get_str();
            if (type == "response")
            {
                {
                    int no = json["no"].get_int();
                    Glob::vis[no % VIS_LEN] = 1;
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
}

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
            std::cout << "[" << Glob::cur_chat_people << "]:" << msg["content"]["content"].get_str() << "\n";
        }
    }
}

int main()
{
    int cnt = 5;

    while (cnt && set_up_connection() == -1)
    {
        std::cout << "连接失败尝试重连中...\n";
        sleep(1);
        cnt--;
    }
    if (cnt == 0)
        return 0;
    UI::init();

    std::thread th(rev_msg);
    std::thread th2(chat_msg_listenner);

    int login_res;
    while ((login_res = UI::login()) == 0)
    {
        std::cout << "登录失败[用户名或密码错误]\n";
        UI::create_config();
        UI::init();
    }
    if (login_res == -1)
    {
        std::cout << "登录失败!,服务器超时\n";
        exit(1);
    }
    UI::main_page();
    std::cout << "Main Closed";
    close(Glob::cfd);
    return 0;
}