#pragma once
#include <string>
#include <mutex>
#include <shared_mutex>
#include <set>
#include <optional>
#include "./json.h"

extern std::mutex user_json_mux;

namespace User
{
    class UserModel
    {
    public:
        UserModel(JSON json);
        UserModel(const std::string &name, const std::string &pass);

        std::string username;
        std::string password;

        int is_admin;
        std::set<std::string> friends;
        JSON &get_src_json()
        {
            return src_json;
        }

    private:
        JSON src_json;
    };

    class UserTable
    {
    public:
        UserTable(const std::string &file_path);

        int add_user(const std::string &username, const std::string &passowrd);

        std::optional<UserModel> find_user(const std::string &username);

        void add_friend(const std::string &user, const std::string &new_friend);

    private:
        std::string json_path;

        void write_back();
        void init_tab();

        std::shared_mutex tab_mux;
        std::map<std::string, UserModel> tab;

        JSON file_json;
    };

}