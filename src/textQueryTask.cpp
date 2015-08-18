#include "textQueryServer.h"
#include "textQueryTask.h"
#include "topk.h"
#include "edit_distance.h"
#include "index.h"
#include "cache.h"
#include <sstream>
#include <time.h>
#include <iomanip>
#include <json/json.h>
void NM::CTextQueryTask::sendMessage(std::deque<NM::Data>& deq)
{
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    MY_NET::CUdpTransport udp(sockfd,m_ip,m_port);
    std::string strquery;
    std::stringstream sin;
    std::deque<NM::Data>::iterator itr = deq.begin();
    Json::Value root;
    Json::Value item;
    Json::FastWriter writer;
    for(;itr != deq.end();++itr)
    {
        item["word"] = itr->m_word;
        item["distance"] = itr->m_distance;
        item["freq"] = itr->m_distance;
        root.append(item);
    }
    strquery = writer.write(root);
    if(deq.size() ==0)
        strquery = "no data\n";
    std::cout<<strquery<<std::endl;
    udp.send(strquery.c_str(),strquery.size());
    close(sockfd);
}

void NM::CTextQueryTaskSimple::getQueryInfo(std::deque<NM::Data>& deq)
{
    clock_t begin_t = clock();
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
    clock_t end_t = clock();
    double time = (end_t - begin_t)*1.0/CLOCKS_PER_SEC;
    std::cout<<"time:"<<time<<std::endl;
}

void NM::CTextQueryTaskWithIndex::getQueryInfo(std::deque<NM::Data>& deq)
{
    clock_t clock1 = clock();
    typedef  NM::CIndex::IndexVecType IndexVecType;
    typedef  NM::CIndex::IndexQueryVecType IndexQueryVecType;
    NM::CIndex* pIndex = NM::CIndex::getInstance();
    IndexVecType& indexVec = pIndex->getIndexVec();
    std::set<IndexQueryVecType::size_type> queryIndexSet;
    pIndex->getQueryIndex(m_querystr,queryIndexSet);
    std::set<IndexQueryVecType::size_type>::iterator itr;
    NM::Editdistance ed;
    int distance;
    NM::CTopk<NM::Data> topk(MAX_QUE_SIZE);
    for(itr = queryIndexSet.begin();itr != queryIndexSet.end();++itr)
    {
        distance = ed(m_querystr,indexVec[*itr].first);
        NM::Data val(indexVec[*itr].first,distance,indexVec[*itr].second);
        topk.push(val);
    }
    topk.gettopk(deq);
    clock_t clock4 = clock();
    double time = (clock4 - clock1)*1.0/CLOCKS_PER_SEC;
    //std::cout<<"time:"<<std::fixed<<std::setprecision(6)<<time<<std::endl;
    std::cout<<"time:"<<time<<"clock4:"<<clock4<<"clock1:"<<clock1<<"PER:"<<CLOCKS_PER_SEC<<std::endl;
}

void NM::CTextQueryTaskWithCache::getQueryInfo(std::deque<NM::Data>& deq)
{
    NM::CCacheManager* pcacheManager = NM::CCacheManager::getInstance();
    pthread_t threadId = pthread_self();
    NM::CCache* m_pCache = pcacheManager->getCache(threadId);
    NM::CCache::CacheVecType queryVec;
    NM::CTopk<NM::Data> topk(MAX_QUE_SIZE);
    if(m_pCache->searchCacheData(m_querystr,queryVec))
    {
        std::cout<<"cache find"<<std::endl;
        for(NM::CCache::CacheVecType::iterator itr = queryVec.begin();itr != queryVec.end();++itr)
        {
            topk.push(NM::Data(itr->word,itr->distance,itr->freq));
        }
        topk.gettopk(deq);
        return;
    }
    std::cout<<"cache not find"<<std::endl;
    CTextQueryTaskWithIndex::getQueryInfo(deq);
    for(std::deque<NM::Data>::iterator itr = deq.begin();itr!=deq.end();++itr)
    {
        queryVec.push_back(NM::CacheData(itr->m_word,itr->m_distance,itr->m_freq));
    }
    m_pCache->addCacheData(m_querystr,queryVec);
    
}

