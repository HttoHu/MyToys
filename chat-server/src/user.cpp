#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <fstream>

#include "../includes/json.h"
#include "../includes/server.h"

const std::string user_list_path = "./data/user_list.json";

std::mutex file_mux;

JSON &user_info()
{
    static JSON json = JSON::read_from_file("./data/user_list.json");
    return json;
}

namespace User
{
    // basic funcitons
    int_least16_t write_back()
    {
        std::lock_guard<std::mutex> lk(file_mux);
        std::ofstream ofs(user_list_path);
        if (!ofs.good())
        {
            std::cerr << "open file " + user_list_path + " failed!\n";
            return -1;
        }
        ofs << user_info().to_string();
        return 0;
    }
    bool find_user(const std::string &username, JSON &ret)
    {
        JSON user_lists = user_info()["user_list"];
        if (!user_lists.has(username))
            return false;
        ret = user_lists[username];
        return true;
    }
    int add_user(const std::string &username, const std::string &password)
    {
        auto usr_lst = user_info()["user_list"];
        if (usr_lst.has(username))
            return -1;

        JSON user = JSON::map({
            {"password", JSON::val(password)},
            {"friends", JSON::array({})},
            {"is_admin", JSON::val(0)},
        });

        user_info()["user_list"].add_pair(username, user);

        return write_back();
    }

    bool auth_user(const std::string &username, const std::string &password)
    {
        JSON user;
        if (!find_user(username, user))
            return false;
        return user["password"].get_str() == password;
    }
    bool check_admin(JSON auth)
    {
        std::string username = auth["username"].get_str();
        std::string pass = auth["password"].get_str();
        JSON user;
        if (!find_user(username, user))
            return false;
        return user["is_admin"].get_int();
    }

    // interface

    void create_user(const CallBackParams &cp)
    {
        try
        {
            JSON req(cp.content);
            JSON auth = req["auth"];

            std::string username = auth["username"].get_str();
            std::string pass = auth["password"].get_str();

            std::string new_username = req["new-username"].get_str();
            std::string new_password = req["new-password"].get_str();

            JSON user;

            if (!check_admin(auth))
            {
                cp.respose(JSON::map({{"success", JSON::val(-1)}}));
                return;
            }
            if (find_user(new_username, user))
            {
                cp.respose(JSON::map({{"success", JSON::val(-1)}}));
                return;
            }
            cp.respose(JSON::map({{"success", JSON::val(add_user(new_username, new_password))}}));
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    void import_user()
    {
        add_handler("user-create", create_user);
    }
}