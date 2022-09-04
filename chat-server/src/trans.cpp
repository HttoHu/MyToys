#include "../includes/net.hpp"
#include "../includes/def.hpp"
#include <pthread.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <set>
extern std::set<int> client_table;

using namespace Htto::Net;
int end_flag=false;
namespace 
{
    char buf[1024];
}
void* loop_send(void * arg)
{
    end_flag=true;
    int clientfd=*(int *)arg;
    while(end_flag)
    {
        std::string input;
        std::getline(std::cin,input);
        if(input=="#quit")
        {
            end_flag=false;
        }
        if(end_flag)
        {
            send_msg(clientfd,input);
        }
    }
    return nullptr;
}

void* loop_receive(void * arg)
{
    int clientfd=*(int *)arg;  
    while(true)
    {
        std::string info=receive_msg(clientfd);
        if(info=="#quit")
            break;
        
        std::cout<<info<<std::endl;
        for(auto other_client:client_table)
        {
            if(other_client==clientfd)continue;
            send_msg(other_client,info);
        }
    } 
    return nullptr;
}

void send_msg(int clientfd,const std::string &content)
{
    memcpy(buf,content.c_str(),content.size());
    buf[content.size()]='\0';
    if(Connect::write(clientfd,buf,content.size()+1)<0)
        printf("Write Failed\n");
}

std::string receive_msg(int clientfd)
{
    int n=0;
    if((n=Connect::read(clientfd,buf,sizeof(buf)))>0)
    {
        buf[n]='\0';
        return std::string(buf);
    }
    return "#quit";
}
