#ifndef __CONNECT_H__
#define __CONNECT_H_
#define MAX_SIZE 1024_
namespace MY_NET
{
    class CTransport
    {
        private:
            int m_fd;
        public:
            CTransport(int fd):m_fd(fd)
        {
        }
            virtual int recv(char* buf,int size,std::string& ip = "",int& port = 0) = 0;
            virtual int send(char* buf,int size) = 0;

    };
}
#endif
