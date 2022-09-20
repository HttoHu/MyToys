#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <thread>
#include "net_stream.hpp"
namespace Net
{
    const int BUFF_SIZE = 512;

    class Server;
    using byte = unsigned char;

    class Server
    {
    public:
        Server(int _port) : port(port)
        {
            sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sockfd == -1)
                throw std::runtime_error("create server failed\n");

            sockaddr_in saddr;
            memset(&saddr, 0, sizeof(saddr));

            saddr.sin_addr.s_addr = INADDR_ANY;
            saddr.sin_family = AF_INET;
            saddr.sin_port = port;

            if (bind(sockfd, (sockaddr *)&saddr, sizeof(saddr)) == -1)
            {
                ::close(sockfd);
                throw std::runtime_error("bind failed\n");
            }
            if (listen(sockfd, 1024) == -1)
            {
                ::close(sockfd);
                throw std::runtime_error("bind failed\n");
            }
        }
        void run_server()
        {
            auto looper = [this]
            {
                while (this->sockfd != -1)
                {
                    sockaddr_in caddr;
                    memset(&caddr, 0, sizeof(sockaddr));
                    socklen_t sock_len = 0;
                    int cfd = accept(this->sockfd, (sockaddr *)&caddr, &sock_len);
                    if (cfd == -1)
                        return;
                    NetStream ns(cfd, AddrInfo{caddr.sin_addr.s_addr, caddr.sin_port});

                    this->connect_callback(ns);
                }
            };
            std::thread t(looper);
            on_running = true;
            listener = std::move(t);
        }
        void close()
        {
            ::close(sockfd);
            sockfd = -1;
        }

        void set_connect_callback(std::function<void(NetStream &)> fn)
        {
            connect_callback = fn;
        }
        void set_close_callback(std::function<void(NetStream &)> fn)
        {
            close_callback = fn;
        }

        void on_close(NetStream &ns)
        {
            close_callback(ns);
        }
        ~Server()
        {
            if (on_running)
                listener.join();
            if (sockfd != -1)
                ::close(sockfd);
        }

    private:
        std::function<void(NetStream &)> connect_callback;
        std::function<void(NetStream &)> close_callback;
        bool on_running = false;
        std::thread listener;
        int sockfd = -1;
        int port;
    };
    class Client
    {
    public:
        template <typename T>
        T read_object()
        {
            T ret;
            byte *ptr = &ret;
            int len = sizeof(T);
            int rd_len = 0;
            int cnt = 0;
            while (rd_len < len)
            {
                int cnt = read(sockfd, ptr + rd_len, len - rd_len);
                if (cnt == -1)
                    throw std::runtime_error("bad I/O operaion ::read_object");
                rd_len += cnt;
            }
            return ret;
        }

        template <typename T>
        void write_object(const T &obj)
        {
            write(sockfd, &obj, sizeof(obj));
        }
        void read_buf(int len, byte *buf)
        {
            int cur_len = 0;
            while (cur_len < len)
                cur_len += read(sockfd, buf + cur_len, len - cur_len);
        }
        void write_buf(int len, byte *buf)
        {
            write(sockfd, buf, len);
        }

    private:
        int sockfd;
    };
};