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
        std::cout << json.to_string() << "\n";
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
    UI::login();
    std::thread th(rev_msg);

    UI::main_page();
    close(Glob::cfd);
    return 0;
}