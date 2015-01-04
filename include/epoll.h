#ifndef __EPOLL_H__
#define __EPOLL_H__
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include "udp_transport.h"
#include <iostream>
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

typedef void (*HandleMessageCallback)(int sockfd,const std::string& ip,int port,char* buf,int size);
namespace MY_NET
{
    class CEpoll
    {
        public:
            CEpoll(int sockfd,int maxsize):m_sockfd(sockfd),m_maxsize(maxsize)
            {
                m_epollfd = epoll_create(1024);
                if(-1 == m_epollfd)
                    ERR_EXIT("epoll_create");
                struct epoll_event my_event;
                memset(&my_event,0,sizeof(my_event));
                my_event.events = EPOLLIN | EPOLLET;
                my_event.data.fd = m_sockfd;
                if(0 != epoll_ctl(m_epollfd,EPOLL_CTL_ADD,m_sockfd,&my_event))
                    ERR_EXIT("epoll_create");
                set_nonblock(m_sockfd);
                m_events = new epoll_event[maxsize];
            }
            void wait()
            {
                m_readysize = epoll_wait(m_epollfd,m_events,m_maxsize,-1);
                if(m_readysize == -1)
                    ERR_EXIT("epoll_wait");
            }
            void HandleUDPRead()
            {
                char buf[UDP_MAX_SIZE] = {0};
                for(int index = 0;index < m_readysize;index++)
                {
                    if(m_events[index].events & EPOLLIN)
                    {
                        int sockfd = m_events[index].data.fd;
                        while(1)
                        {
                            CUdpTransport udp(sockfd);
                            memset(buf,0,UDP_MAX_SIZE);
                           CInetAddr addr;
                            int ret = udp.recv(buf,UDP_MAX_SIZE-1,&addr);
                            if(ret<0)
                            {
                                if(errno == EAGAIN)
                                    break;
                            }
                            m_onMessage(sockfd,addr.getip(),addr.getport(),buf,ret);
                        }

                    }
                }


            }
            void setOnMessage(HandleMessageCallback onMessage)
            {
                m_onMessage = onMessage;
            }
            ~CEpoll()
            {
                close(m_epollfd);
                delete m_events;
            }

        private:
            int m_sockfd;
            int m_epollfd;
            struct epoll_event* m_events;
            int m_maxsize;
            int m_readysize;
            HandleMessageCallback m_onMessage;
            void set_nonblock(int sockfd)
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

    };
}
#endif
