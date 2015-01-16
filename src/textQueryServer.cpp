#include <iostream>
#include <string>
#include "singleton.h"
#include <ext/hash_map>
#include <fstream>
#include <sstream>
#include <queue>
#include "epoll.h"
#include "thread_pool.h"
#include "socket.h"
#include "config.h"
#include "common.h"
#include "edit_distance.h"
#define MAX_QUE_SIZE 10
extern void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size);
class CTextQueryServer
{
    friend class CSingleton<CTextQueryServer>;
    public:
    CTextQueryServer()
    {
        NM::CConfig* pconfig = NM::CConfig::getInstance();
        pconfig->getconfigInfo("textquery","IP",m_ip);
        pconfig->getconfigInfo("textquery","port",m_port);
        pconfig->getconfigInfo("textquery","threadnum",m_thread_num);
        pconfig->getconfigInfo("textquery","taskcapacity",m_task_capacity);
        m_sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(-1 == m_sockfd)
            ERR_EXIT("socket");
        m_psocket = new MY_NET::CSocket(m_sockfd);
        m_psocket->bind(m_ip,m_port);
        m_pthreadpool = new MY_THREAD::CThreadPool(m_thread_num,m_task_capacity);
        m_pepoll = new MY_NET::CEpoll(m_sockfd,1024);

    }
    static CTextQueryServer* getInstance()
    {
        return CSingleton<CTextQueryServer>::getInstance(); 
    }
    void readdict()
    {
        std::string wordfreqpath;
        NM::CConfig* pconfig = NM::CConfig::getInstance();
        pconfig->getconfigInfo("textquery","wordfreqpath",wordfreqpath);
        std::cout<<"distpath:"<<wordfreqpath<<std::endl;
        std::fstream fin;
        fin.open(wordfreqpath.c_str());
        if(!fin.is_open())
        {
            std::cout<<wordfreqpath<<"open failed"<<std::endl;
            exit(0);
        }
        std::string line;
        std::string word;
        int freq;
        std::stringstream sin;
        int cnt = 0;
        while(std::getline(fin,line))
        {
            sin.clear();
            sin.str(line);
            sin>>word>>freq;
            m_word_freq[word] = freq;
            cnt++;
        }
        std::cout<<"cnt:"<<cnt<<std::endl;
        fin.close();
    }

    MY_THREAD::CThreadPool* getThreadPool()
    {
        return m_pthreadpool;
    }
    __gnu_cxx::hash_map<std::string,int,NM::CMyHash>& getwordfreq()
    {
        return m_word_freq;
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
    ~CTextQueryServer()
    {
        delete m_pepoll;
        delete m_pthreadpool;
        delete m_psocket;
        close(m_sockfd);
    }
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
        bool operator<(const Data& obj)const
        {
            if(m_distance != obj.m_distance)
                return m_distance < obj.m_distance;
            else
                return m_freq>obj.m_freq;
        }

    };
    /*
       struct CMyComp
       {
       bool operator()(std::pair<std::string,int>& left,std::pair<std::string,int>& right)   //<,大；>,小（距离大
       {
       if(left.second != right.second)
       return left.second < right.second;
       else
       return left.
       }

       };
       */
}
class CTextQueryTask:public MY_THREAD::CTask
{
    public:
        CTextQueryTask(int sockfd,const std::string & ip,int port,std::string querystr,int size):m_sockfd(sockfd),m_ip(ip),m_port(port),m_querystr(querystr),m_size(size)
    {
    }
        void execute()
        {
            std::cout<<"execute"<<m_ip<<":"<<m_port<<":"<<m_querystr<<std::endl;
            CTextQueryServer* pserver = CTextQueryServer::getInstance();
            __gnu_cxx::hash_map<std::string,int,NM::CMyHash> hm_wordfreq = pserver->getwordfreq();
            std::cout<<"wordfreq size:"<<hm_wordfreq.size()<<std::endl;
            __gnu_cxx::hash_map<std::string,int,NM::CMyHash>::iterator itr = hm_wordfreq.begin();
            NM::Editdistance ed;
            int distance;
            for(;itr != hm_wordfreq.end();++itr)
            {
                distance = ed(m_querystr,itr->first);
                NM::Data val(itr->first,distance,itr->second);
                if(m_queue.size() < MAX_QUE_SIZE)
                {
                    m_queue.push(val);
                }
                else if(val < m_queue.top())
                {
                    m_queue.pop();
                    m_queue.push(val);
                }

            }

            int sockfd = socket(AF_INET,SOCK_DGRAM,0);
            MY_NET::CUdpTransport udp(sockfd,m_ip,m_port);
            std::string strquery;
            if(!m_queue.size())
            {
                strquery = "cannot find";
                udp.send(strquery.c_str(),strquery.size());
                std::cout<<strquery<<std::endl;
            }
            else
            {
                std::stringstream sin;
                while(m_queue.size())
                {
                    sin<<m_queue.top().m_word;
                    sin<<":";
                    sin<<m_queue.top().m_distance;
                    sin<<":";
                    sin<<m_queue.top().m_freq;
                    std::cout<<m_queue.top().m_word<<":"<<m_queue.top().m_distance<<":"<<m_queue.top().m_freq<<std::endl;
                    m_queue.pop();
                }
                strquery = sin.str();
                udp.send(strquery.c_str(),strquery.size());
            }
            close(sockfd);
        }
    private:
        int m_sockfd;
        const std::string m_ip;
        int m_port;
        std::string m_querystr;
        int m_size;
      //  std::priority_queue<std::pair<std::string,int>,std::vector<std::pair<std::string,int> >,NM::CMyComp> m_queue;
        std::priority_queue<NM::Data,std::vector<NM::Data> >m_queue;
};

void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size)
{
    std::cout<<"ontextQueryMessage"<<std::endl;
    CTextQueryTask* ptask = new CTextQueryTask(sockfd,ip,port,buf,size);
    MY_THREAD::CThreadPool* pthreadpool = CTextQueryServer::getInstance()->getThreadPool();
    if(pthreadpool != NULL)
        pthreadpool->addTask(ptask);
}

int main(int argc,char* argv[])
{
    CTextQueryServer* pserver = CTextQueryServer::getInstance();
    pserver->readdict();
    pserver->start();
}
