#include <iostream>
#include <fstream>
#include <sstream>
#include "textQueryServer.h"
#include "textQueryTask.h"
#include "config.h"
#include "edit_distance.h"
#include "index.h"
#include "common.h"
extern void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size);
NM::CTextQueryServer::CTextQueryServer()
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
NM::CTextQueryServer* NM::CTextQueryServer::getInstance()
{
    return CSingleton<CTextQueryServer>::getInstance(); 
}
void NM::CTextQueryServer::readdict()
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
    while(std::getline(fin,line))
    {
        sin.clear();
        sin.str(line);
        sin>>word>>freq;
        m_word_freq[word] = freq;
    }
    fin.close();
}
void NM::CTextQueryServer::readIndex()
{
    NM::CIndex* pIndex = NM::CIndex::getInstance();  //CIndex构造函数中包含readIndex的功能
}

MY_THREAD::CThreadPool* NM::CTextQueryServer::getThreadPool()
{
    return m_pthreadpool;
}
__gnu_cxx::hash_map<std::string,int,NM::CMyHash>& NM::CTextQueryServer::getwordfreq()
{
    return m_word_freq;
}
void NM::CTextQueryServer::start()
{
    m_pthreadpool->on();
    m_pepoll->setOnMessage(ontextQueryMessage);
    while(1)
    {
        std::cout<<"waitbefore"<<std::endl;
        m_pepoll->wait();
        std::cout<<"waitafter"<<std::endl;
        m_pepoll->HandleUDPRead();
    }
}
NM::CTextQueryServer::~CTextQueryServer()
{
    delete m_pepoll;
    delete m_pthreadpool;
    delete m_psocket;
    close(m_sockfd);
}

void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size)
{
    std::cout<<"ontextQueryMessage"<<std::endl;
    NM::CTextQueryTask* ptask = new NM::CTextQueryTaskWithIndex(sockfd,ip,port,buf,size);
    MY_THREAD::CThreadPool* pthreadpool = NM::CTextQueryServer::getInstance()->getThreadPool();
    if(pthreadpool != NULL)
        pthreadpool->addTask(ptask);
}

int main(int argc,char* argv[])
{
    NM::CTextQueryServer* pserver = NM::CTextQueryServer::getInstance();
    pserver->readdict();
    pserver->readIndex();
    pserver->start();
}
