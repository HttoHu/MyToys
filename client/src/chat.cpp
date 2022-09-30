#include "defs.h"
#include <deque>
namespace UI
{
    JSON cons_header(const std::string &method);
}

namespace Chat
{
    using namespace std::chrono_literals;

    std::map<std::string, std::deque<std::string>> chat_history;

    bool request_friend_list()
    {
        int cur_no = Glob::message_cnt;
        JSON req = UI::cons_header("user-get_friends_list");
        send_json(req.borrow());

        auto timeout = std::chrono::steady_clock::now() +
                       5000ms;

        auto ret = wait_response(cur_no);
        if (!ret.count())
            return false;
        int status = ret["success"].get_int();
        if (status)
        {
            Glob::user_config().add_pair("friends", ret["friends"].clone());
            return true;
        }
        return false;
    }
    std::optional<JSON> pull_message(const std::string &name)
    {
        int cur_no = Glob::message_cnt;
        JSON req = UI::cons_header("message-pull_message");
        send_json(req.borrow());
        return wait_response(cur_no);
    }

    bool send_message(const std::string &msg_str, const std::string &dest)
    {
        int cur_no = Glob::message_cnt;
        JSON req = UI::cons_header("message-send");

        req.add_pair("from", Glob::user_config()["auth"]["username"].clone());
        req.add_pair("dest", JSON::val(dest));
        req.add_pair("message", JSON::val(msg_str));

        send_json(req.borrow());

        auto timeout = std::chrono::steady_clock::now() +
                       5000ms;

        auto json = wait_response(cur_no);
        if (!json.count() || !json["success"].get_int())
            return false;
        return true;
    }

    void print_chat_history(const std::string &friend_name)
    {
        auto &q = chat_history[friend_name];
        for (auto msg : q)
        {
            std::cout << msg << "\n";
        }
    }
    void push_chat_message(const std::string &friend_name, const std::string &msg)
    {
        chat_history[friend_name].push_back(msg);
    }
}