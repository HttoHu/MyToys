#include <iostream>
#include <pthread.h>
#include <vector>
#include <arpa/inet.h>
#include <set>
#include "../includes/def.hpp"
#include "../includes/net.hpp"

#define LOCAL_IP "0.0.0.0"
#define PORT "12345"

pthread_mutex_t client_mutex;
std::set<int> client_table;
void process_client(int clientfd,Htto::Net::IpAddr client_addr)
{
    using namespace Htto::Net;
    using namespace Htto::Net::Connect;
    using namespace Htto::Net::Tools;
    loop_receive((void*)&clientfd);
    printf("%x:%d 已经离开房间\n",client_addr.ip,client_addr.port);

    client_table.erase(clientfd);
    close(clientfd);
}

struct ClientThreadArg
{
    int clientfd;
    Htto::Net::IpAddr client_addr;
};
void *process_client_wrapper(void * args)
{
    ClientThreadArg arg=*(ClientThreadArg*)args;
    process_client(arg.clientfd,arg.client_addr);
    return nullptr;
}
int main(){
    using namespace Htto::Net;
    using namespace Htto::Net::Connect;
    using namespace Htto::Net::Tools;
    // get socketfd
    int socketfd=Connect::get_socket();
    // init server ip and port
    IpAddr local_add;
    local_add.ip=liter_to_ip(LOCAL_IP);
    local_add.port = htons(string_to_unsigned(PORT));
    // attach address to socketfd
    bind(socketfd,local_add);
    Connect::listen(socketfd,500);
    int clientfd;
    IpAddr client_addr;
    printf("初始化服务器成功!\n");

    std::vector<pthread_t> threads_id;
    while(1)
    {
        clientfd = Connect::accept(socketfd,client_addr);

        printf("%x:%d 已经进入房间\n",client_addr.ip,client_addr.port);

        pthread_t cur_client_thread;
        ClientThreadArg arg={clientfd,client_addr};
        client_table.insert(clientfd);
        pthread_create(&cur_client_thread,nullptr,process_client_wrapper,&arg);
        threads_id.push_back(cur_client_thread);
    }
    for(auto cur_th:threads_id)
    {
        pthread_join(cur_th,nullptr);
    }

    return 0;
}