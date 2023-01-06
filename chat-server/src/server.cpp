#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <cstring>
#include <map>
#include <shared_mutex>
#include <unistd.h>
#include <thread>
#include <errno.h>

#include "../includes/server.h"
#include "../includes/thread_pool.h"
// ============================ decls ===============================
namespace Glob
{
    // server socket fd
    extern int sfd;
    extern int port;
    extern int max_connections;
    extern int work_threads;
}
namespace User
{
    extern std::map<std::string, int> username_fd_tab;
    extern std::map<int, std::string> fd_username_tab;
    extern std::shared_mutex username_fd_tab_mux;
}
// ============================ end decl ===============================
// ============================ defs ===============================
std::map<int, std::mutex> cfd_mux_tab;

ThreadPool *thread_pool = nullptr;

std::map<std::string, Handler> handler_table;

template <typename T>
struct PointerGuard
{
    PointerGuard(T *p, bool is_arr) : pointer(p), is_array(is_arr) {}
    ~PointerGuard()
    {
        if (is_array)
            delete[] pointer;
        else
            delete pointer;
    }
    T *pointer;
    bool is_array;
};

// ============================ defs ===============================
void send_json(int fd, JSON json)
{
    std::string data = json.to_string();
    int len = data.size();
    len = htonl(len);
    std::lock_guard<std::mutex> lk(cfd_mux_tab[fd]);
    write(fd, &len, sizeof(len));
    write(fd, data.c_str(), ntohl(len));
}
void CallBackParams::respose(JSON json) const
{
    send_json(fd, json);
}

// ============================ functions ===============================
int set_up_connection()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int option = true;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, sizeof(int));

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(Glob::port);

    if (bind(fd, (sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        std::cerr << "bind error!\n";
        return -1;
    }
    if (listen(fd, Glob::max_connections) == -1)
    {
        std::cerr << "listen error!\n";
        return -1;
    }
    Glob::sfd = fd;

    thread_pool = new ThreadPool(Glob::work_threads);

    return 0;
}

void add_handler(const std::string &name, Handler handler)
{
    handler_table.insert({name, handler});
}

std::string read_package(int fd)
{
    int len;
    int cnt = 0;
    if ((cnt = read(fd, &len, 4)) == -1)
        throw std::runtime_error("read_package(int) error!\n");
    if (cnt == 0)
        return "";
    cnt = 0;
    len = ntohl(len);
    if (len < 0 || len > 1024 * 1024)
    {
        std::cerr << len << " invalid package size!\n";
        return "";
    }
    std::string ret(len, 0);
    while (cnt < len)
        cnt += read(fd, ret.data() + cnt, len - cnt);
    return ret;
}

void print_handler()
{
    std::cout << "[";
    for (auto [str, _] : handler_table)
        std::cout << str << " ";
    std::cout << "]";
}

int run_server()
{
    int epfd = epoll_create(512);
    epoll_event *events = new epoll_event[Glob::max_connections + 1];
    epoll_event cur_event;

    PointerGuard<epoll_event> pg(events, true);

    cur_event.events = EPOLLIN;
    cur_event.data.fd = Glob::sfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, Glob::sfd, &cur_event);

    while (true)
    {
        int cnt = epoll_wait(epfd, events, Glob::max_connections + 1, -1);
        if (cnt == -1)
        {
            std::cerr << "epoll_wait() error !\n";
            return -1;
        }
        for (int i = 0; i < cnt; i++)
        {
            if (events[i].data.fd == Glob::sfd)
            {
                // client address
                sockaddr_in caddr;
                // prevent errno 22 QAQ
                memset(&caddr, 0, sizeof(caddr));

                socklen_t caddr_len = 0;
                int cfd = accept(Glob::sfd, (sockaddr *)&caddr, &caddr_len);
                caddr.sin_addr.s_addr = caddr.sin_addr.s_addr;

                if (cfd == -1)
                {
                    std::cerr << Glob::sfd << " invalid client! errno! " << errno << "\n";
                    continue;
                }
                std::cout << "rev connect from: " << inet_ntoa((caddr.sin_addr)) << "\n";

                cur_event.events = EPOLLIN;
                cur_event.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &cur_event);
            }
            else
            {
                try
                {
                    int cfd = events[i].data.fd;
                    std::string content = read_package(cfd);
                    if (content.empty())
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);

                        close(cfd);
                        std::cout << "client closed socket " << cfd << "\n";

                        std::lock_guard<std::shared_mutex> lk(User::username_fd_tab_mux);

                        auto it = User::fd_username_tab.find(cfd);
                        if (it == User::fd_username_tab.end())
                            continue;
                        std::cout << "remove user " << it->second << "\n";
                        User::username_fd_tab.erase(it->second);
                        User::fd_username_tab.erase(it);

                        continue;
                    }
                    JSON json(content);
                    std::cout<<"Rev: "<<json.to_string()<<"\n";
                    std::string req_name = json["req"].get_str();

                    auto it = handler_table.find(req_name);
                    if (it == handler_table.end())
                    {
                        std::cout << "No req " << req_name << " handler !\n";
                        continue;
                    }
                    thread_pool->enqueue(it->second, CallBackParams(content, cfd));
                }
                catch (std::exception &e)
                {
                    std::cerr << "\033[30minvalid package\033[0m\n";
                    continue;
                }
            }
        }
    }
}

// ============================end functions ===============================
