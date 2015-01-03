#ifndef __UDP_CONNECT_H__
#define __UDP_CONNECT_H__
#include "connect.h"
#include "inet_addr.h"
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
            CUdpTransport(int fd,std::string ip = "",int port = 0):CConnect(fd),m_addr(ip,port)
        {
        }
            int recv(char* buf,int size,std::string& ip,int& port)
            {
                int size = recvfrom(m_fd,buf,sizeof(buf),0,(struct sockaddr*)&m_addr,sizeof(struct sockaddr));
                if(-1 == size)
                {
                    if(error != EAGAIN)
                        ERR_EXIT("recvfrom");
                }
                ip = m_addr.getip();
                port = m_addr.getport();
                return size;
            }
            int send(char* buf,int size)
            {
                int size = sendto(m_fd,buf,size,0,(struct sockaddr*)&m_addr,sizeof(struct sockaddr));
                if(-1 == size)
                {
                    if(error != EAGAIN)
                        ERR_EXIT("sendto");
                }
                return size;
            }
            CUdpTransport& setip(std::string& ip)
            {
                m_addr.setip(ip);
                return *this;
            }
            CUdpTransport& setport(int port)
            {
                m_addr.setport(port);
                return *this;
            }
    };
}
#endif
