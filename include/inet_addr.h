#ifndef __INET_ADDR_H__
#define __INET_ADDR_H__
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
namespace MY_NET
{
    class CInetAddr
    {
        private:
            struct sockaddr_in m_addr;
        public:
            CInetAddr(const std::string& ip = std::string(""),int port = 0)
            {
                memset(&m_addr,0,sizeof(m_addr));
                m_addr.sin_family = AF_INET;
                m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
                m_addr.sin_port = htons(port);
            }
            CInetAddr(struct sockaddr_in& addr):m_addr(addr)
        {
        }
            std::string getip()
            {
                return std::string(inet_ntoa(m_addr.sin_addr));
            }
            int getport()
            {
                return ntohs(m_addr.sin_port);
            }
            void setip(const std::string& ip)
            {
                m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            }
            void setport(const int port)
            {
                m_addr.sin_port = htons(port);
            }

    };
}
#endif
