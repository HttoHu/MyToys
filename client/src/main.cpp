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