#include <iostream>
#include <string>
#include <unistd.h>
// #include "./json.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include "./server.h"
#include "./json.h"

namespace Glob
{
    extern int sfd;
};

int load_config(const std::string &conf_path);

void hello_handler(CallBackParams cp)
{
    JSON res = JSON::map({
        {"content", JSON::val("Hello World!")},
        {"fd", JSON::val(cp.fd)},
    });
    auto res_str = res.to_string();
    write(cp.fd, res_str.c_str(), res_str.size());
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "please run the server as following format server <config_path>\n";
        return 1;
    }
    std::string conf_path = argv[1];
    if (load_config(conf_path) == -1)
    {
        std::cerr << "invalid config or path!\n";
        return 1;
    }
    if (set_up_connection() == -1)
    {
        std::cerr << "\033[031mset up server failed\n\033[0m";
        return 1;
    }
    else
    {
        std::cerr << "\033[032mset server success\n\033[0m";
    }

    add_handler("hello", hello_handler);

    run_server();
    return 0;
}