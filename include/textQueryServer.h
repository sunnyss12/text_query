#ifndef __TEXTQUERYSERVER_H__
#define __TEXTQUERYSERVER_H__
#include "singleton.h"
#include "epoll.h"
#include "thread_pool.h"
#include "socket.h"
#include "common.h"
#include <string>
#include <ext/hash_map>
namespace NM
{
    class CTextQueryServer
    {
        public:
            friend class CSingleton<CTextQueryServer>;
            CTextQueryServer();
            static CTextQueryServer* getInstance();
            void readdict();
            void readIndex();
            MY_THREAD::CThreadPool* getThreadPool();
            __gnu_cxx::hash_map<std::string,int,NM::CMyHash>& getwordfreq();
            void start();
            ~CTextQueryServer();
        private:
            std::string m_ip;
            int m_port;
            int m_thread_num;
            int m_task_capacity;
            int m_sockfd;
            MY_NET::CSocket* m_psocket;
            MY_NET::CEpoll* m_pepoll;
            MY_THREAD::CThreadPool* m_pthreadpool;
            __gnu_cxx::hash_map<std::string,int,NM::CMyHash> m_word_freq;



    };
}
#endif
