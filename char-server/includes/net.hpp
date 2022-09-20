/*
    By Htto 2021
*/
#include <string>
#include <iostream>
namespace Htto{
    namespace Net
    {
        struct IpAddr{
            unsigned ip;
            unsigned port;
        };
        namespace Connect
        {
            int get_socket();
            void bind(int socketfd,const IpAddr &addr);
            void listen(int socketfd,int max_client_num);
            // return clientfd, and store the client address to addr.
            int accept(int socketfd,IpAddr & addr);
            // return the number of received bytes.
            int read(int socketfd,char *buf,size_t len);
            int write(int socketfd,char *buf,size_t len);
            void close(int socketfd);
        }

        namespace Tools{
            std::string ip_to_liter(unsigned ip);
            unsigned liter_to_ip(const std::string &ip);
            unsigned string_to_unsigned(const std::string &str);
        }
    }
}