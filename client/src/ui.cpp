#include <iostream>
#include <algorithm>
#include "json.h"
#include <fstream>
#include <string>
#include <chrono>
#include <unistd.h>
#include "defs.h"
#include "utils.h"
namespace Chat
{
    bool request_friend_list();
    bool send_message(const std::string &msg, const std::string &dest);
    void print_chat_history(const std::string &friend_name);
    void push_chat_message(const std::string &friend_name, const std::string &msg);
    JSON pull_message(const std::string &name);
}
namespace
{
    void prepare_unread_msg(const std::string &name)
    {
        auto json = Chat::pull_message(name);
        if (!json.count() || !json["success"].get_int())
            return;
        auto arr = json["content"].get_list();
        for (auto v : arr)
        {
            std::string msg = "[" + v["from"].get_str() + "]:" + v["message"].get_str();
            std::cout << msg << "\n";
            Chat::push_chat_message(v["from"].get_str(), msg);
        }
    }
}
namespace UI
{
    using namespace std::chrono_literals;

    void clear_screen()
    {
#ifndef CLRSCR
        // std::cout << "\033c";
#endif
    }
    void chat_page(std::string username)
    {
        clear_screen();
        Glob::cur_chat_people = username;
        Chat::print_chat_history(username);
        // notify chat_msg_listenner
        prepare_unread_msg(username);
        Glob::msg_tab_cv.notify_one();

        getchar();

        Guarder gurder([&]()
                       { Glob::cur_chat_people = ""; });
        std::cout << "[我]:";
        while (true)
        {
            std::string send_msg;
            getline(std::cin, send_msg);
            if (send_msg == "#Q")
                return;
            std::cout << "[我]:";

            Chat::push_chat_message(username, "[我]:" + send_msg);
            if (!Chat::send_message(send_msg, Glob::cur_chat_people))
            {
                set_up_connection();
                if (!Chat::send_message(send_msg, Glob::cur_chat_people))
                {
                    std::cout << "[系统消息]:发送失败!\n";
                    continue;
                }
            }
        }
    }
    void friends_page()
    {
        clear_screen();
        std::cout << "获取好友列表中...\n";
        if (!Glob::user_config().has("friends"))
        {
            if (!Chat::request_friend_list())
            {
                std::cout << "获取好友列表失败...\n";
                sleep(2);
                return;
            }
        }

        clear_screen();
        while (true)
        {
            auto friends = Glob::user_config()["friends"].borrow();
            int len = friends.length();
            for (int i = 0; i < len; i++)
            {
                std::cout << i + 1 << ". " << friends[i].get_str() << "\n";
            }
            std::cout << "B. 返回\n";
            std::cout << "[输入数字或B]:";
            std::string input;
            std::cin >> input;
            if (input == "B")
                return;
            int num;
            try
            {
                num = std::stoi(input);
            }
            catch (std::exception &e)
            {
                std::cout << "请输入数字" << std::endl;
                continue;
            }
            if (num > len)
            {
                std::cout << "非法输入\n";
                continue;
            }
            chat_page(friends[num - 1].get_str());
            Backup::backup_chat_history();
        }
    }
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
            {
                std::cout << "\033[33mGood bye~\033[0m";
                // std::cout << Glob::user_config().to_string() << "\n";
                exit(0);
            }
            else if (str == "2")
            {
                clear_screen();
            }
            else if (str == "1")
            {
                friends_page();
            }
        }
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
        // json.add_pair("friends", JSON::array({}));
        ofs << json.to_string();
        ofs.close();
    }
    void init()
    {
        try
        {
            Glob::user_config() = JSON::read_from_file("./config.json");
            Backup::read_chat_history();
        }
        catch (std::exception &e)
        {
            create_config();
            Glob::user_config() = JSON::read_from_file("./config.json");
        }
    }
    JSON cons_header(const std::string &method)
    {
        JSON msg;
        msg.add_pair("auth", Glob::user_config()["auth"].clone());
        msg.add_pair("req", JSON::val(method));
        msg.add_pair("no", JSON::val(Glob::message_cnt++));
        return msg;
    }

    /*  0 -> 账号名密码错误
        1-> 成功
        -1-> 请求超时
    */
    int login()
    {
        int cur_no = Glob::message_cnt;
        JSON msg = cons_header("user-login");
        send_json(msg.borrow());

        auto timeout = std::chrono::steady_clock::now() +
                       5000ms;
        std::unique_lock<std::mutex> lk(Glob::message_mutex);

        if (!Glob::notifier.wait_until(lk, timeout, [&]() -> bool
                                       { return Glob::vis[cur_no % VIS_LEN]; }))
        {
            return -1;
        }

        JSON tmp = Glob::response_tab[cur_no % VIS_LEN];
        Glob::vis[cur_no % VIS_LEN] = 0;
        if (tmp["success"].get_int())
        {
            std::cout << "登录成功!\n";
            return 1;
        }
        else
        {
            std::cout << "登录失败!\n";
            std::cout << tmp.to_string() << "\n";
            return 0;
        }
    }
}