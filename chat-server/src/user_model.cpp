#include "../includes/user_model.h"
#include <fstream>

namespace User
{
    UserModel::UserModel(JSON json) : src_json(json)
    {
        username = src_json["username"].get_str();
        password = src_json["password"].get_str();
        is_admin = src_json["is_admin"].get_int();
        auto vec = src_json["friends"].get_list();
        for (auto v : vec)
            friends.insert(v.get_str());
    }

    UserModel::UserModel(const std::string &name, const std::string &pass) : username(name), password(pass), is_admin(0)
    {
        src_json.add_pair("username", JSON::val(username));
        src_json.add_pair("password", JSON::val(password));
        src_json.add_pair("is_admin", JSON::val(is_admin));
        src_json.add_pair("friends", JSON("[]"));
    }

    UserTable::UserTable(const std::string &path) : json_path(path)
    {
        file_json = JSON::read_from_file(json_path);

        init_tab();
    }

    int UserTable::add_user(const std::string &username, const std::string &passowrd)
    {
        std::lock_guard<std::shared_mutex> lk(tab_mux);
        if (tab.count(username))
            return -1;
        auto new_user = UserModel(username, passowrd);
        file_json["user_list"].add_pair(username, new_user.get_src_json());
        write_back();
        return 0;
    }

    std::optional<UserModel> UserTable::find_user(const std::string &username)
    {
        std::shared_lock<std::shared_mutex> lk(tab_mux);
        auto it = tab.find(username);
        if (it == tab.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

    void UserTable::init_tab()
    {
        auto list = file_json["user_list"];
        auto mp = list.get_map();

        for (auto [name, content] : mp)
        {
            tab.insert({name, UserModel(content)});
        }
    }

    void UserTable::write_back()
    {
        std::ofstream ofs(json_path);
        if (!ofs.good())
            throw std::runtime_error("open " + json_path + " failed!");
        ofs << file_json.to_string();
    }

    void UserTable::add_friend(const std::string &user, const std::string &new_friend)
    {
        std::lock_guard<std::shared_mutex> lk(tab_mux);
        auto it = tab.find(user);
        if (it == tab.end())
            throw std::range_error("invalid user " + new_friend + "\n");
        if (it->second.friends.count(new_friend))
            throw std::range_error("you have added " + new_friend + "\n");
        
        it->second.friends.insert(new_friend);
        it->second.get_src_json()["friends"].push(JSON::val(new_friend));
        write_back();
        
    }
}