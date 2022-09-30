#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <thread>
#include "../includes/thread_pool.h"
#include "../includes/message.h"
#include "../includes/server.h"
#include "../includes/user_model.h"
// ============================= decls======================
namespace User
{
    bool check_login(JSON auth, bool check_admin = false);
    UserTable &user_table();
    int get_fd_by_name(const std::string &name);
}
namespace Utils
{
    JSON gen_response_message(int success, const std::string &msg, int no);
}
extern ThreadPool *thread_pool;

void send_json(int fd, JSON json);
// ============================= end decls======================

namespace Message
{

    MessageUnit::MessageUnit(const std::string &_dest, JSON _c, MessageType mt) : dest(_dest), content(_c), time_stmp(clock()), msg_type(mt) {}

    int MessageUnit::time_dif()
    {
        time_t cur = clock();
        return (cur - time_stmp) / CLOCKS_PER_SEC;
    }

    JSON MessageUnit::get_data()
    {
        return content.borrow();
    }

    MessageQueue message_queue;
    // store offline message

    std::shared_mutex user_msgbox_mux;
    std::map<std::string, std::queue<MessageUnit>> user_msgbox;

    // message sender
    void message_sender()
    {
        while (true)
        {
            MessageUnit mu;
            message_queue.wait_and_pop(mu);
            int fd = User::get_fd_by_name(mu.get_dest());
            if (fd == -1)
            {
                std::lock_guard<std::shared_mutex> lk(user_msgbox_mux);
                user_msgbox[mu.get_dest()].push(mu);
                continue;
            }
            send_json(fd, mu.get_data().clone());
        }
    }
    // ==================== interfaces ==========================

    void send_message(const CallBackParams &cp)
    {
        using namespace Utils;
        try
        {
            JSON req(cp.content);
            auto auth = req["auth"];
            int no = req["no"].get_int();
            std::string dest = req["dest"].get_str();
            JSON content = req["message"];
            if (!User::check_login(auth))
            {
                cp.respose(gen_response_message(0, "invalid user", no));
                return;
            }
            auto dest_user = User::user_table().find_user(dest);
            if (!dest_user)
            {
                cp.respose(gen_response_message(0, "no such user " + dest, no));
                return;
            }
            cp.respose(gen_response_message(1, "okay " + dest, no));
            // push to message queue
            JSON send_msg;

            send_msg.add_pair("type", JSON::val("message"));
            send_msg.add_pair("from", auth["username"].clone());
            send_msg.add_pair("message", content.clone());
            message_queue.push(MessageUnit(dest, send_msg.clone(), MessageType::COMMON));
            return;
        }
        catch (std::exception &e)
        {
            cp.respose(gen_response_message(0, "invalid request", 0));
            return;
        }
    }

    /*
        req: basic
        res: content:[]
    */
    void pull_message(const CallBackParams &cp)
    {
        using namespace Utils;
        try
        {
            JSON req(cp.content);
            auto auth = req["auth"];
            int no = req["no"].get_int();
            std::string user = auth["username"].get_str();

            if (!User::check_login(auth))
            {
                cp.respose(gen_response_message(0, "invalid user", no));
                return;
            }
            auto response = gen_response_message(1, "okay", no);

            auto arr_json = JSON::array({});
            {
                std::lock_guard<std::shared_mutex> lk(user_msgbox_mux);
                auto &q = user_msgbox[user];
                while (!q.empty())
                {
                    arr_json.push(q.front().get_data().clone());
                    q.pop();
                }
            }
            response.add_pair("content", arr_json);
            std::cout << "RESPONSE:" << response.to_string() << "\n";
            cp.respose(response);
        }
        catch (std::exception &e)
        {
            cp.respose(gen_response_message(0, "invalid user", 0));
        }
    }

    void import_message()
    {
        add_handler("message-send", send_message);

        add_handler("message-pull_message", pull_message);

        thread_pool->enqueue(message_sender);
    }
}