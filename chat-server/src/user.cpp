#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <fstream>

#include "../includes/json.h"
#include "../includes/server.h"
#include "../includes/user_model.h"

namespace User
{
    UserTable &user_table()
    {
        static UserTable usr_tb("./data/user_list.json");
        return usr_tb;
    }

    bool check_login(JSON auth, bool check_admin)
    {
        std::string username = auth["username"].get_str();
        std::string pass = auth["password"].get_str();
        auto user = user_table().find_user(username);
        return user && user->password == pass && (!check_admin || user->is_admin);
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

            if (!check_login(auth, true))
            {
                cp.respose(JSON::map({{"success", JSON::val(0)},{"password",JSON::val("username or password incorrect")}}));
                return;
            }
            if (user_table().find_user(new_username) != std::nullopt)
            {
                cp.respose(JSON::map({{"success", JSON::val(0)},
                                      {"message", JSON::val("user existed!")}}));
                return;
            }
            std::cout << "Good boy!\n";
            cp.respose(JSON::map({{"success", JSON::val(1 + user_table().add_user(new_username, new_password))}}));
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            cp.respose(JSON::map({{"success", JSON::val(-1)},
                                  {"message", JSON::val("request failed!")}}));
        }
    }

    void import_user()
    {
        add_handler("user-create", create_user);
    }
}