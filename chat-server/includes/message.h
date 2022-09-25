#pragma once
#include <queue>
#include "json.h"
#include "task_queue.h"

namespace Message
{
    enum MessageType
    {
        COMMON
    };
    class MessageUnit
    {
    public:
        MessageUnit() = default;

        MessageUnit(const std::string &_dest, JSON _c, MessageType mt);
        int time_dif();
        JSON get_data();
        std::string get_dest() { return dest; }

    private:
        std::string dest;
        JSON content;
        time_t time_stmp;
        MessageType msg_type;
    };
    using MessageQueue = Task::TSqueue<MessageUnit>;
}