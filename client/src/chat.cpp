#include "defs.h"

namespace UI
{
    JSON cons_header(const std::string &method);
}

namespace Chat
{
    using namespace std::chrono_literals;
    bool request_friend_list()
    {
        int cur_no = Glob::message_cnt;
        JSON req = UI::cons_header("user-get_friends_list");
        send_json(req.borrow());

        auto timeout = std::chrono::steady_clock::now() +
                       5000ms;

        JSON tmp;
        // lock scope
        {
            std::unique_lock<std::mutex> lk(Glob::message_mutex);
            if (!Glob::notifier.wait_until(lk, timeout, [&]() -> bool
                                           { return Glob::vis[cur_no % VIS_LEN]; }))
            {
                return false;
            }

            tmp = Glob::response_tab[cur_no % VIS_LEN];
            Glob::vis[cur_no % VIS_LEN] = 0;
        }
        int status = tmp["success"].get_int();
        if (status)
        {
            Glob::user_config().add_pair("friends", tmp["friends"].clone());
            return true;
        }
        return false;
    }
}