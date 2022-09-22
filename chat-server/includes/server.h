#pragma once
#include <functional>
#include "./json.h"
struct CallBackParams{
    CallBackParams(const std::string &s,int f):content(s),fd(f){}
    const std::string content;

    int fd;
    // send json to client
    void respose(JSON json)const;
};

using Handler = std::function<void(const CallBackParams &)>;

// -1: failed , 0 :okay
int set_up_connection();

void add_handler(const std::string & name,Handler handler);

int run_server();
