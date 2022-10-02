#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <fstream>

#include "json.h"

namespace Chat
{
    extern std::map<std::string, std::deque<std::string>> chat_history;
}
namespace Backup
{
    // chat history file name
    const std::string chat_his_fname = "./chat_history.json";

    void read_chat_history()
    {
        JSON file = JSON::read_from_file(chat_his_fname);
        auto mp = file.get_map();
        for (auto [name, val] : mp)
        {
            std::deque<std::string> q;
            auto lst = val.get_list();
            std::cout<<"Read chat history "<<name<<":"<<val.length()<<" messages\n";
            for (int i = 0; i < lst.size(); i++)
                q.push_back(lst[i].get_str());
            Chat::chat_history.insert({name, q});
        }
    }

    // backup
    bool backup_chat_history()
    {
        JSON json;
        for (auto [name, q] : Chat::chat_history)
        {
            JSON cur_msg_list("[]");

            while (q.size() > 1000)
                q.pop_front();
            
            for (auto str : q)
                cur_msg_list.push(JSON::val(str));
            json.add_pair(name, cur_msg_list.clone());
        }
        std::ofstream ofs(chat_his_fname);
        if (!ofs.good())
            return false;
        ofs << json.to_string() << "\n";
        return true;
    }
}