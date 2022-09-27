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
        std::cout << json.to_string() << std::endl;
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
                std::lock_guard<std::mutex> lk(Glob::message_mutex);
                // sender username
                std::string from = json["from"].get_str();
                Glob::msg_tab.insert({from, json});
            }
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

    int login_res;
    while ((login_res = UI::login()) == 0)
    {
        std::cout << "登录失败[用户名或密码错误]\n";
        UI::create_config();
        UI::init();
    }
    if(login_res == -1)
    {
        std::cout<<"登录失败!,服务器超时\n";
        exit(1);
    }
    UI::main_page();
    std::cout << "Main Closed";
    close(Glob::cfd);
    return 0;
}