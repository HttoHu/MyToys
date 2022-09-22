#include "./server.hpp"
#include <thread>

int main()
{
    try
    {
        using Net::NetStream;
        using Net::Server;
        Server basic_server(4001);
        basic_server.set_connect_callback([](NetStream &ns)
                                          { 
                                            ns << "hello world"; 
                                        });
        basic_server.run_server();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return 0;
}