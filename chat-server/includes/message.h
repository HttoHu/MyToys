#pragma once
#include "json.h"
namespace Message
{
    class Message
    {
    public:
    private:
        JSON content;
        std::string from;
        std::string to;
    };
}