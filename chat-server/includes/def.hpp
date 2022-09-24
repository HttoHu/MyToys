#include <string>
extern int end_flag;
void* loop_send(void *);
void* loop_receive(void *);

void send_msg(int clientfd,const std::string &content);

std::string receive_msg(int clientfd);

