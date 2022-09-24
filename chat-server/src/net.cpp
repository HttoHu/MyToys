#include "../includes/net.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
namespace Htto{
    namespace Net{
        
        namespace Connect{
            int get_socket()
            {
                return socket(AF_INET,SOCK_STREAM,0);
            }

            void bind(int socketfd,const IpAddr &addr)
            {
                sockaddr_in sa;
                sa.sin_family=AF_INET;
                sa.sin_port = addr.port;
                sa.sin_addr.s_addr=addr.ip;
                if(bind(socketfd,(sockaddr*)&sa,sizeof(sa))!=0)
                {
                    printf("bind error\n");
                    exit(1);
                }
            }

            void listen(int socketfd,int max_client_num)
            {
                if(::listen(socketfd,max_client_num)!=0)
                    printf("listen error\n"),exit(1);
            }

            int accept(int socketfd,IpAddr & addr)
            {
                sockaddr_in client_addr;
                int len=sizeof(client_addr);
                int ret=::accept(socketfd,(sockaddr*)&client_addr,(socklen_t*)&len);
                addr.ip=client_addr.sin_addr.s_addr;
                addr.port=client_addr.sin_port;

                return ret;
            }

            int read(int socketfd,char *buf,size_t len)
            {
                return recv(socketfd,buf,len,0);
            }

            int write(int socketfd,char *buf,size_t len)
            {
                return send(socketfd,buf,len,0);
            }

            void close(int socketfd)
            {
                ::close(socketfd);
            }
        }
        namespace Tools{
            std::string ip_to_liter(unsigned ip)
            {
                char buf[64];
                
                if(inet_ntop(AF_INET,(in_addr*)&ip,buf,64)==NULL)
                {
                    printf("ip_to_liter: invalid ip address\n");
                    exit(1);
                }
                return std::string(buf);
            }
            unsigned liter_to_ip(const std::string &ip)
            {
                unsigned ret;
                unsigned flag=inet_pton(AF_INET,ip.c_str(),(in_addr*)&ret);
                if(flag<=0)
                {
                    printf("liter_to_ip: invalid ip address\n");
                    exit(1);
                }
                return ret;
            }
            unsigned string_to_unsigned(const std::string &str)
            {
                unsigned res=0;
                for(auto ch:str)
                {
                    if(!isdigit(ch))
                        throw std::logic_error("invalid number");
                        res*=10;
                        res+= ch-'0';
                }
                return res;
            }
            
        }
    }
}