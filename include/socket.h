#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <cstring>
#include "inet_addr.h"
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)
namespace MY_NET
{
    class CSocket
    {
        private:
            int m_fd;
        public:
            CSocket(fd):m_fd(fd)
        {
        }
            void bind(const std::string& ip,int port)
            {
                CInetAddr addr(ip,port);
                if(-1 == ::bind(m_fd,(struct sockaddr*)&addr,sizeof(struct sockaddr)))
                    ERR_EXIT("bind");
            }
            void listen(int num)
            {
                if(-1 == ::listen(m_fd,num))
                    ERR_EXIT("listen");
            }
            int accept(std::string& ip(""),int& port = 0)
            {
                CInetAddr client_addr;
                int fd_client = ::accept(m_fd,(struct sockaddr*)&client_addr,sizeof(struct sockaddr));
                if( -1 == fd_client)
                    ERR_EXIT("accept");
                ip = client_addr.getip();
                port = client_addr.getport();
                return fd_client;
            }
            void connect(const std::string& ip,int port)
            {
               CInetAddr server_addr(ip,port);
               if(-1 != ::connect(m_fd,(struct sockaddr*)&server_ip,sizeof(struct sockaddr)))
                   ERR_EXIT("connect");
            }
            ~CSocket()
            {
                close(m_fd);
            }

    };
}
#endif
