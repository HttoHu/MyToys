#include <iostream>
#include <functional>

#include "../includes/json.h"

namespace Glob
{
    extern int port;
    extern int max_connections;
    extern int work_threads;
}

namespace Utils
{
    JSON gen_response_message(int success, const std::string &msg, int no)
    {
        return JSON::map({{"success", JSON::val(success)},
                          {"message", JSON::val(msg)},
                          {"type", JSON::val("response")},
                          {"no", JSON::val(no)}});
    }
}

int load_config(const std::string &conf_path)
{
    try
    {
        auto global_configure = JSON::read_from_file(conf_path);
        Glob::port = global_configure["port"].get_int();
        Glob::max_connections = global_configure["max_connections"].get_int();
        Glob::work_threads = global_configure["work_threads"].get_int();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return -1;
    }
    return 0;
}