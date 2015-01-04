#ifndef __UDP_CONNECT_H__
#define __UDP_CONNECT_H__
#include "transport.h"
#include <errno.h> 
#include "inet_addr.h"
#include <stdio.h>
#include <stdlib.h>
#define UDP_MAX_SIZE 1024
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

namespace MY_NET
{
    class CUdpTransport:public CTransport
    {
        private:
            CInetAddr m_addr;
        public:
            CUdpTransport(int fd,std::string ip = "",int port = 0):CTransport(fd),m_addr(ip,port)
        {
        }
            int recv(char* buf,int size,CInetAddr* paddr = NULL)
            {
                socklen_t len  = sizeof(struct sockaddr);
                int readn = recvfrom(m_fd,buf,size,0,(struct sockaddr*)paddr,&len);
                if(-1 == readn)
                {
                    if(errno != EAGAIN)
                        ERR_EXIT("recvfrom");
                }
                return readn;
            }
            int send(char* buf,int size)
            {
                int sendn = sendto(m_fd,buf,size,0,(struct sockaddr*)&m_addr,sizeof(struct sockaddr));
                if(-1 == sendn)
                {
                    if(errno != EAGAIN)
                        ERR_EXIT("sendto");
                }
                return sendn;
            }
            CUdpTransport& setip(const std::string& ip)
            {
                m_addr.setip(ip);
                return *this;
            }
            CUdpTransport& setport(const int port)
            {
                m_addr.setport(port);
                return *this;
            }
    };
}
#endif
