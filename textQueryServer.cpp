#include "epoll.h"
#include "thread_pool.h"
#include "socket.h"


class CTextQueryTask:public MY_THREAD::CTask
{
    public:
        CTextQueryTask(int sockfd,std::string & ip,int port,char* buf,int size):m_sockfd(sockfd),m_ip(ip),m_port(port),m_buf(buf),m_size(size)
        {
        }
        void execute()
        {
            CUdpTransport udp(m_sockfd);
            udp.setip(m_ip).setport(m_port);
            udp.send(m_buf,m_size);
        }
    private:
            int m_sockfd;
            std::string m_ip;
            int m_port;
            char* m_buf;
            int m_size;
}
class textQueryServer;
void ontextQueryMessage(int sockfd,std::string& ip,int port,char* buf,int size)
{
    CThreadPool* pthreadpool = textQueryServer::getThreadPool();
    CTextQueryTask* ptask = new CTextQueryTask(sockfd,ip,port,buf,size);
    pthreadpool->addTask(ptask);
    
}
class textQueryServer
{
    public:
        textQueryServer(std::string& ip = "192.168.1.116",int port = 8888,std::string& distdir = "~/src/dis.txt"):m_ip(ip),m_port(port),m_distdir(distdir)
        {
            m_sockfd = socket(AF_INET,SOCK_DGRAM,0);
            m_psocket = new CSocket(m_sockfd);
            m_psocket->bind(m_ip,m_port);
            m_pthreadpool = new CThreadPool(5,10);
            m_pepoll = new CEpoll(m_sockfd,1024);
        }
        static CThreadPool* getThreadPool()
        {
            return m_pthreadpool;
        }
        void setOnMessage(HandleMessageCallback onMessage)
        {
            m_pepoll->setOnMessage(onMessage);
        }
        void start()
        {
            while(1)
            {
                m_pepoll->wait();
                m_pepoll->HandleUDPRead();
            }
        }
    private:
        std::string m_ip;
        int m_port;
        std::string m_distdir;
        int m_sockfd;
        CSocket* m_psocket;
        CEpoll* m_pepoll;
        static CThreadPool* m_pthreadpool;
}
CThreadPool* textQueryServer::m_pthreadpool = NULL;

int main(int argc,char* argv[])
{
    textQueryServer server;
    server.setOnMessage(ontextQueryMessage);
    server.start();
}
