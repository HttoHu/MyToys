#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(4001);

    if (bind(fd, (sockaddr *)&saddr, sizeof(saddr)) == -1)
        return 1;
    if (listen(fd, 10) == -1)
        return 1;

    while (true)
    {
        sockaddr_in tmp;
        socklen_t len;
        int cfd = accept(fd, (sockaddr *)&tmp, &len);
        if (cfd == -1)
        {
            std::cout << "invalid client!";
            continue;
        }
        const char *str = "HELLO WORLD";
        write(cfd, str, strlen(str));
        close(cfd);
    }
    return 0;
}