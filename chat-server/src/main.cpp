#include <iostream>
#include <string>
#include <unistd.h>
// #include "./json.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include "../includes/server.h"
#include "../includes/json.h"

namespace Glob
{
    extern int sfd;
};

int load_config(const std::string &conf_path);

void print_handler();

namespace Test
{
    void import_test();
}
namespace User
{
    void import_user();
}

int run_main(int argc, char *argv[])
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

    Test::import_test();
    User::import_user();

    print_handler();
    std::cout << "\n";
    run_server();
    return 0;
}

int run_test(){
    return 1;
}
int main(int argc, char *argv[])
{
    return run_main(argc,argv);
}
