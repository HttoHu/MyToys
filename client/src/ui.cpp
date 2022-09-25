#include <iostream>
#include <algorithm>
#include "json.h"
#include <fstream>
#include <string>
#include <filesystem>
#include "defs.h"

namespace UI
{
    void main_page()
    {
        while (true)
        {
            std::cout << "1. 查看好友列表\n";
            std::cout << "2. 查看消息列表\n";
            std::cout << "Q. 退出\n";
            std::cout << ":";
            std::string str;
            std::cin >> str;
            if (str == "Q")
                exit(1);
            else if (str == "2")
            {
            }
            else if (str == "1")
            {
            }
        }
    }
    void clear_screen()
    {
        std::cout << "\033c";
    }
    std::pair<std::string, std::string> input_username_password()
    {
        std::cout << "用户名:";
        std::string username, password;
        std::cin >> username;
        std::cout << "密码:";
        std::cin >> password;
        return {username, password};
    }
    void create_config()
    {
        std::ofstream ofs("./config.json");
        if (!ofs.good())
        {
            std::cout << "打开（创建）配置文件失败！\n";
            exit(1);
        }
        auto [user, pass] = input_username_password();
        JSON json =
            JSON::map({{"auth", JSON::map({{"username", JSON::val(user)},
                                           {"password", JSON::val(pass)}})}});
        json.add_pair("friends", JSON::array({}));
        ofs << json.to_string();
    }
    void init()
    {
        if (!std::filesystem::exists("./config.json"))
        {
            create_config();
        }
        Glob::user_config() = JSON::read_from_file("./config.json");
        // std::cout << Glob::user_config()["auth"].to_string() << std::endl;
    }
    JSON cons_header(const std::string &method)
    {
        JSON msg;
        // msg.add_pair("auth", Glob::user_config()["auth"]);
        msg.add_pair("req", JSON::val(method));
        msg.add_pair("no", JSON::val(Glob::message_cnt++));
        return msg;
    }
    void login()
    {
        JSON msg;
        msg.add_pair("auth", Glob::user_config()["auth"]);
        msg.add_pair("req", JSON::val("user-login"));
        msg.add_pair("no", JSON::val(Glob::message_cnt++));
        send_json(msg);
    }
}