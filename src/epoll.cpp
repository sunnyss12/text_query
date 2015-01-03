#include "epoll.h"
namespace MY_NET
{
    CEpoll::Epoll(int sockfd,int maxsize):m_sockfd(sockfd)
    {
        m_epollfd = epoll_create(1024);
        if(-1 == m_epollfd)
            ERR_EXIT("epoll_create");
        struct epoll_event my_event;
        memset(&my_event,0,sizeof(my_event));
        my_event.event = EPOLLIN | EPOLLET;
        my_event.data.fd = m_sockfd;
        if(0 != epoll_ctrl(m_epollfd,EPOLL_CTL_ADD,m_sockfd,&my_event))
            ERR_EXIT("epoll_create");
        set_nonblock(m_sockfd);
        m_pevents = new epoll_event[maxsize];
        m_maxsize = maxsize;

    }
    void CEpoll::set_nonblock(int sockfd)
    {
        int old;
        old = fcntl(sockfd,F_GETFL);
        if(-1 == m_epollfd)
            ERR_EXIT("fcntl");
        old |= O_NONBLOCK;
        int ret = fcntl(sockfd,F_SETFL,old);
        if(-1 == ret)
            ERR_EXIT("fcntl");
    }
    void EPoll::wait()
    {
        m_readysize = epoll_wait(m_epollfd,m_events,maxsize,-1);
        if(m_readysize == -1)
            ERR_EXIT("epoll_wait");
    }
    void Epoll::HandleUDPRead()
    {
        char buf[MAX_SIZE] = {0};
        for(int index = 0;index < m_readysize;index++)
        {
            if(m_events[i].events & EPOLLIN)
            {
                int sockfd = events[i].data.fd;
                while(1)
                {
                    CUdpTransport upd(sockfd);
                    memset(buf,0,MAX_SIZE);
                    std::string ip;
                    int port;
                    int ret = udp.recv(ip,port,buf,MAX_SIZE-1);
                    if(ret<0)
                    {
                        if(errno == EAGAIN)
                            break;
                    }
                    m_onMessage(sockfd,ip,port,buf,ret);
                }

            }
        }
    }
    void Epoll::setOnMessage(HandleMessageCallback onMessage)
    {
        m_onMessage = onMessage;
    }
    ~Epoll()
    {
        close(m_epollfd);
        delete m_events;
    }

}

