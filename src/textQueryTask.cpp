#include "textQueryServer.h"
#include "textQueryTask.h"
#include "topk.h"
#include "edit_distance.h"
#include <sstream>
void NM::CTextQueryTask::getQueryInfo(std::deque<NM::Data>& deq)
{
    CTextQueryServer* pserver = CTextQueryServer::getInstance();
    __gnu_cxx::hash_map<std::string,int,NM::CMyHash> hm_wordfreq = pserver->getwordfreq();
    std::cout<<"wordfreq size:"<<hm_wordfreq.size()<<std::endl;
    __gnu_cxx::hash_map<std::string,int,NM::CMyHash>::iterator itr = hm_wordfreq.begin();
    NM::Editdistance ed;
    int distance;
    NM::CTopk<NM::Data> topk(MAX_QUE_SIZE);
    for(;itr != hm_wordfreq.end();++itr)
    {
        distance = ed(m_querystr,itr->first);
        NM::Data val(itr->first,distance,itr->second);
        topk.push(val);
    }
    topk.gettopk(deq);
}
void NM::CTextQueryTask::sendMessage(std::deque<NM::Data>& deq)
{
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    MY_NET::CUdpTransport udp(sockfd,m_ip,m_port);
    std::string strquery;
    std::stringstream sin;
    std::deque<NM::Data>::iterator itr = deq.begin();
    for(;itr != deq.end();++itr)
    {
        sin<<itr->m_word;
        sin<<":";
        sin<<itr->m_distance;
        sin<<":";
        sin<<itr->m_freq;
        sin<<" ";
    }
    strquery = sin.str();
    std::cout<<strquery<<std::endl;
    udp.send(strquery.c_str(),strquery.size());
    close(sockfd);
}



