#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <unistd.h>
#include <shared_mutex>
#include <fstream>

#include "../includes/json.h"
#include "../includes/server.h"
#include "../includes/user_model.h"
namespace Utils
{
    JSON gen_response_message(int success, const std::string &msg, int no);
}
namespace User
{
    std::shared_mutex username_fd_tab_mux;
    std::map<std::string, int> username_fd_tab;
    std::map<int, std::string> fd_username_tab;

    int get_fd_by_name(const std::string &name)
    {
        std::shared_lock<std::shared_mutex> lk(username_fd_tab_mux);
        auto it = username_fd_tab.find(name);
        if (it == username_fd_tab.end())
            return -1;
        return it->second;
    }

    UserTable &user_table()
    {
        static UserTable usr_tb("./data/user_list.json");
        return usr_tb;
    }

    bool check_login(JSON auth, bool check_admin = false)
    {
        std::string username = auth["username"].get_str();
        std::string pass = auth["password"].get_str();
        auto user = user_table().find_user(username);
        return user && user->password == pass && (!check_admin || user->is_admin);
    }

    // interface
    void login(const CallBackParams &cp)
    {
        using namespace Utils;
        try
        {
            JSON req(cp.content);
            JSON auth = req["auth"];
            int no = req["no"].get_int();
            if (!check_login(auth))
            {
                cp.respose(gen_response_message(0, "login failed!", no));
                return;
            }
            // lock scope
            {
                std::lock_guard<std::shared_mutex> lk(username_fd_tab_mux);
                std::cout << "added " << auth["username"].get_str() << "\n";
                username_fd_tab.insert({auth["username"].get_str(), cp.fd});
                fd_username_tab.insert({cp.fd, auth["username"].get_str()});
            }
            auto ack = gen_response_message(1, "ack!", no);
            cp.respose(ack);
        }
        catch (std::exception &e)
        {
            cp.respose(gen_response_message(0, "invalid request!", 0));
        }
    }

    void create_user(const CallBackParams &cp)
    {
        using namespace Utils;

        try
        {
            JSON req(cp.content);
            JSON auth = req["auth"];
            int no = req["no"].get_int();

            std::string username = auth["username"].get_str();
            std::string pass = auth["password"].get_str();

            std::string new_username = req["new-username"].get_str();
            std::string new_password = req["new-password"].get_str();

            if (!check_login(auth, true))
            {
                cp.respose(JSON::map({{"success", JSON::val(0)}, {"password", JSON::val("username or password incorrect")}}));
                return;
            }
            if (user_table().find_user(new_username) != std::nullopt)
            {
                cp.respose(JSON::map({{"success", JSON::val(0)},
                                      {"message", JSON::val("user existed!")}}));
                return;
            }
            cp.respose(JSON::map({{"success", JSON::val(user_table().add_user(new_username, new_password) != -1)}}));
        }
        catch (std::exception &e)
        {
            cp.respose(JSON::map({{"success", JSON::val(0)},
                                  {"message", JSON::val("request failed!")}}));
        }
    }

    void add_friend(const CallBackParams &cp)
    {
        using namespace Utils;

        try
        {
            JSON req(cp.content);
            // res
            auto auth = req["auth"];
            auto username = auth["username"].get_str();
            auto new_friend = req["new_friend"].get_str();
            int no = req["no"].get_int();

            if (!check_login(auth))
            {
                cp.respose(gen_response_message(0, "invalid user", no));
                return;
            }

            if (user_table().add_friend(username, new_friend) != -1)
                cp.respose(gen_response_message(1, "okay", no));
            else
                cp.respose(gen_response_message(0, "added friend failed", no));
        }
        catch (std::exception &e)
        {
            cp.respose(gen_response_message(0, "invalid request", 0));
            return;
        }
    }

    void get_friend_list(const CallBackParams &cp)
    {
        using namespace Utils;
        try
        {
            JSON req(cp.content);
            // res
            auto auth = req["auth"];
            int no = req["no"].get_int();
            auto username = auth["username"].get_str();

            if (!check_login(auth))
            {
                cp.respose(gen_response_message(0, "invalid user", no));
                return;
            }
            auto usr = user_table().find_user(username);
            auto ret = gen_response_message(1, "okay", no);
            ret.add_pair("friends", usr->get_src_json()["friends"].clone());
            std::cout << ret.to_string() << std::endl;
            cp.respose(ret);
        }
        catch (std::exception &e)
        {
            cp.respose(gen_response_message(0, "invalid request", 0));
        }
    }

    void import_user()
    {
        add_handler("user-create", create_user);
        add_handler("user-add_friend", add_friend);
        add_handler("user-get_friends_list", get_friend_list);
        add_handler("user-login", login);
    }
}