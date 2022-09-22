#include "../includes/server.h"

namespace Test
{
    void echo(const CallBackParams &cp)
    {
        JSON req(cp.content);

        std::string ret = "Hello " + req["data"].get_str();

        JSON res = JSON::map({{"data", JSON::val(ret)}});

        std::cout << "Send " << res.to_string() << std::endl;
        
        cp.respose(res);
    }
    void import_test()
    {
        add_handler("test-echo", echo);
    }

}