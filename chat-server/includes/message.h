#pragma once
#include <queue>
#include "json.h"
namespace Message
{
    class MessageUnit
    {
    public:
    private:
        JSON content;
        time_t time_val;
    };
    using MessageQueue = std::queue<MessageUnit>;
}