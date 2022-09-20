#pragma once
#include <functional>

struct CallBackParams{
    const std::string content;
    int fd;
};
using Handler = std::function<void(CallBackParams)>;

// -1: failed , 0 :okay
int set_up_connection();

void add_handler(const std::string & name,Handler handler);

int run_server();

int run_server2();