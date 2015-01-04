#include "epoll.h"
#include "thread_pool.h"
#include "socket.h"
#include <iostream>
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

class CTextQueryTask:public MY_THREAD::CTask
{
    public:
        CTextQueryTask(int sockfd,const std::string & ip,int port,char* buf,int size):m_sockfd(sockfd),m_ip(ip),m_port(port),m_buf(buf),m_size(size)
        {
        }
        void execute()
        {
            std::cout<<"execute"<<m_ip<<":"<<m_port<<":"<<std::endl;
            int sockfd = socket(AF_INET,SOCK_DGRAM,0);
            MY_NET::CUdpTransport udp(sockfd,m_ip,m_port);
            udp.send(m_buf,strlen(m_buf));
            close(sockfd);
        }
    private:
            int m_sockfd;
            const std::string m_ip;
            int m_port;
            char* m_buf;
            int m_size;
};

class textQueryServer;
extern void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size);

class textQueryServer
{
    public:
        textQueryServer(const std::string& ip = std::string("192.168.1.116"),int port = 8888,const std::string& distdir = std::string("~/src/dis.txt")):m_ip(ip),m_port(port),m_distdir(distdir)
        {
            m_sockfd = socket(AF_INET,SOCK_DGRAM,0);
            if(-1 == m_sockfd)
                ERR_EXIT("socket");
            m_psocket = new MY_NET::CSocket(m_sockfd);
            m_psocket->bind(m_ip,m_port);
            m_pthreadpool = new MY_THREAD::CThreadPool(5,10);
            m_pepoll = new MY_NET::CEpoll(m_sockfd,1024);
        }
        static MY_THREAD::CThreadPool* getThreadPool()
        {
            return m_pthreadpool;
        }
        void start()
        {
            m_pthreadpool->on();
            std::cout<<"start"<<std::endl;
            m_pepoll->setOnMessage(ontextQueryMessage);
            while(1)
            {
                std::cout<<"waitbefore"<<std::endl;
                m_pepoll->wait();
                std::cout<<"waitafter"<<std::endl;
                m_pepoll->HandleUDPRead();
            }
        }
        ~textQueryServer()
        {
            delete m_pepoll;
            delete m_pthreadpool;
            delete m_psocket;
            close(m_sockfd);

        }
    private:
        std::string m_ip;
        int m_port;
        std::string m_distdir;
        int m_sockfd;
        MY_NET::CSocket* m_psocket;
        MY_NET::CEpoll* m_pepoll;
        static MY_THREAD::CThreadPool* m_pthreadpool;
};
MY_THREAD::CThreadPool* textQueryServer::m_pthreadpool = NULL;
void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size)
{
    std::cout<<"ontextQueryMessage"<<std::endl;
    CTextQueryTask* ptask = new CTextQueryTask(sockfd,ip,port,buf,size);
    MY_THREAD::CThreadPool* pthreadpool = textQueryServer::getThreadPool();
    if(pthreadpool != NULL)
        pthreadpool->addTask(ptask);
}
int main(int argc,char* argv[])
{
    textQueryServer server(std::string(argv[1]),atoi(argv[2]));
    server.start();
}
