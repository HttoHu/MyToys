#include "./net_stream.hpp"
#include "./server.hpp"
namespace Net
{
    void NetStream::shutdown()
    {
        if (client_fd > 0)
        {
            close(client_fd);
            client_fd = -1;
            server->on_close(*this);
        }
    }
}