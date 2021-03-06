#ifndef __TEXTQUERYTASK_H__
#define __TEXTQUERYTASK_H__
#include "thread.h"
#include "socket.h"
#include "udp_transport.h"
#include "queue.h"
#include <string>
#include <deque>
#define MAX_QUE_SIZE 10
namespace NM
{
    struct Data
    {
        std::string m_word;
        int m_distance;
        int m_freq;
        public:
        Data(std::string word,int distance,int freq):m_word(word),m_distance(distance),m_freq(freq)
        {
        }
        bool operator>(const Data& obj)const
        {
            if(m_distance != obj.m_distance)
                return m_distance < obj.m_distance;
            else
                return m_freq>obj.m_freq;
        }

    };
    class CTextQueryTask:public MY_THREAD::CTask
    {
        public:
            CTextQueryTask(int sockfd,const std::string& ip,int port,const std::string& querystr,int size):m_sockfd(sockfd),m_ip(ip),m_port(port),m_querystr(querystr),m_size(size)
        {
        }
            virtual ~CTextQueryTask()
            {
            }
            void execute()
            {
                std::cout<<"execute"<<m_ip<<":"<<m_port<<":"<<m_querystr<<std::endl;
                std::deque<NM::Data> deq;
                getQueryInfo(deq);
                sendMessage(deq);
            }
            virtual void getQueryInfo(std::deque<NM::Data>& deq) = 0;
            void sendMessage(std::deque<NM::Data>& deq);
        protected:
            int m_sockfd;
            const std::string m_ip;
            int m_port;
            const std::string m_querystr;
            int m_size;

    };
    class CTextQueryTaskSimple:public CTextQueryTask
    {
        public:
            CTextQueryTaskSimple(int sockfd,const std::string& ip,int port,const std::string& querystr,int size):CTextQueryTask(sockfd,ip,port,querystr,size)
        {
        }
            virtual void getQueryInfo(std::deque<NM::Data>& deq);
    };
    class CTextQueryTaskWithIndex:public CTextQueryTask
    {
        public:
            CTextQueryTaskWithIndex(int sockfd,const std::string& ip,int port,const std::string& querystr,int size):CTextQueryTask(sockfd,ip,port,querystr,size)
        {
        }
            virtual void getQueryInfo(std::deque<NM::Data>& deq);

    };
    class CTextQueryTaskWithCache:public CTextQueryTaskWithIndex
    {
        public:
            CTextQueryTaskWithCache(int sockfd,const std::string& ip,int port,const std::string& querystr,int size):CTextQueryTaskWithIndex(sockfd,ip,port,querystr,size)
        {
        }
            virtual void getQueryInfo(std::deque<NM::Data>& deq);
    };
}

#endif
