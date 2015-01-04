#ifndef __CONNECT_H__
#define __CONNECT_H__
#include <string>
#include "inet_addr.h"
namespace MY_NET
{
    class CTransport
    {
        protected:
            int m_fd;
        public:
            CTransport(int fd):m_fd(fd)
        {
        }
            virtual int recv(char* buf,int size,CInetAddr* addr = NULL) = 0;
            virtual int send(char* buf,int size) = 0;
            virtual ~CTransport()
            {
            }

    };
}
#endif
