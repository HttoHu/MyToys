#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>

#include <thread>
#include <mutex>

using namespace std;
void err(const std::string &msg)
{
    std::cout << ">: 毁灭性错误 :" << msg << "\n";
    exit(1);
}
namespace config
{
    const char *server_ip = "43.154.150.103";
    const int port = 12345;
    std::string user_name;
}

int sockfd;
// return socketfd
int set_up_connection()
{
    int ret = socket(AF_INET, SOCK_STREAM, 0);
    if (ret == -1)
        return ret;

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(config::port);
    saddr.sin_addr.s_addr = inet_addr(config::server_ip);

    if (connect(ret, (sockaddr *)&saddr, sizeof(saddr)) == -1)
        return -1;

    return ret;
}

struct file_guard
{
    file_guard(int _fd) : fd(_fd) {}
    ~file_guard() { close(fd); }
    int fd;
};
bool read_config(std::string &ret)
{
    int fd = open("./config", O_CREAT | O_RDWR);
    file_guard fg(fd);
    if (fd == -1)
        err("read config failed");
    char buf[512];
    int len = 0;
    if ((len = read(fd, buf, 512)) == -1)
        err("unknown error read config failed");
    if (len < 3)
        return false;
    stringstream ss(buf);
    string user_name;
    ss << user_name;
    ret = user_name;
    return true;
}

void write_config(const std::string &user_name)
{
    int fd = open("./config", O_CREAT | O_RDWR);
    file_guard fg(fd);

    if (fd == -1)
        err("read config failed");
    char wbuf[512];
    for (int i = 0; i < user_name.size(); i++)
        wbuf[i] = user_name[i];
    wbuf[user_name.size()] = '\0';
    write(fd, wbuf, 512);
}

bool send_msg(const std::string &content)
{
    string str = "[" + config::user_name + "]:" + content;
    return write(sockfd, str.c_str(), str.size()) != -1;
}

void listen_chat()
{
    char buf[512];
    int len = 0;
    while ((len = read(sockfd, buf, 512)) != -1)
    {
        cout << buf << "\n";
        buf[len] = 0;
    }
}

int main()
{
    printf("Connecting...\n");
    sockfd = set_up_connection();
    if (sockfd == -1)
        err("Connect failed!");
    do
    {
        std::cout << "输入用户名(至少3个字符,最多15字符):";
        getline(cin, config::user_name);
    } while (config::user_name.size() < 3 || config::user_name.size() > 15);

    std::thread listen_chart_t(listen_chat);

    while (true)
    {
        string input;
        getline(cin, input);
        if (input == "exit")
            break;
        if (!send_msg(input))
            cout << ">: 发送失败\n";
        else
            std::cout << "[我]:" << input << "\n";
    }
    return 0;
}