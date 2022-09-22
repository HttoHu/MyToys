#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <thread>
namespace Net
{
    class Server;
    struct AddrInfo
    {
        unsigned int ip;
        int port;
    };
    using byte = unsigned char;
    class NetStream
    {
    public:
        NetStream(int fd, AddrInfo info) : client_fd(fd), client_addr(info) {}
        NetStream(const NetStream &) = delete;
        NetStream(NetStream &&ns) : client_fd(ns.client_fd) { ns.client_fd = -1; }

        unsigned int get_ip() const { return client_addr.ip; }
        int get_port() const { return client_addr.port; }

        template <typename T>
        NetStream &operator<<(const T &obj)
        {
            write(client_fd, &obj, sizeof(obj));
            return *this;
        }
        template <typename T>
        NetStream &operator>>(T &obj)
        {
            byte *ptr = &obj;
            int len = sizeof(T);
            int rd_len = 0;
            int cnt = 0;
            while (rd_len < len)
            {
                int cnt = read(client_fd, ptr + rd_len, len - rd_len);
                if (cnt == -1)
                    throw std::runtime_error("bad I/O operaion ::read_object");
                rd_len += cnt;
            }
            return *this;
        }

        void read_buf(int len, byte *buf)
        {
            int cur_len = 0;
            while (cur_len < len)
                cur_len += read(client_fd, buf + cur_len, len - cur_len);
        }

        void write_buf(int len, byte *buf)
        {
            write(client_fd, buf, len);
        }

        void shutdown();

        ~NetStream()
        {
            shutdown();
        }

    private:
        Server *server;
        AddrInfo client_addr;
        int client_fd;
    };

}