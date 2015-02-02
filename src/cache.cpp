#include "cache.h"
#include "textQueryServer.h"
#include "singleton.h"
#include "config.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "textQueryTask.h"
using NM::CCache;
CCache::CCache()
{
    m_query_step = 0;
    m_update_step = 1;
    loadCacheFromFile();
}
void NM::CCache::loadCacheFromFile()
{
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    std::string cachepath;
    pconfig->getconfigInfo("textquery","cachepath",cachepath);
    std::fstream fin;
    fin.open(cachepath.c_str());
    if(!fin.is_open())
    {
        std::cout<<cachepath<<"not exist"<<std::endl;
        return;
    }
    std::string line;
    std::string key;
    std::string word;
    int distance;
    int freq;
    std::stringstream sin;
    while(std::getline(fin,line))
    {
        sin.str(line);
        sin.clear();
        sin>>key;
        while(sin>>word>>distance>>freq)
        {
            m_cache_map[m_query_step][key].push_back(CacheData(word,distance,freq));
            m_cache_map[m_update_step][key].push_back(CacheData(word,distance,freq));
        }
    }
    fin.close();
}
bool NM::CCache::searchCacheData(const std::string& word,CacheVecType& queryVec)
{
    bool ret = false;
    m_lock.Lock();
    std::cout<<"search:query id:"<<m_query_step<<std::endl;
    if( m_cache_map[m_query_step].find(word)!= m_cache_map[m_query_step].end())
    {
        NM::CCache::CacheVecType::iterator itr = m_cache_map[m_query_step][word].begin();
        for(;itr != m_cache_map[m_query_step][word].end();++itr)
        {
            queryVec.push_back(*itr);
        }
        ret = true;
    }
    m_lock.unLock();
    return ret;
}
void NM::CCache::addCacheData(const std::string& word,CacheVecType& cacheData)
{
    m_lock.Lock();
    std::cout<<"add:query id:"<<m_query_step<<std::endl;
    m_cache_map[m_query_step][word] = cacheData;
    m_lock.unLock();
}
NM::CCache::CacheMapType& NM::CCache::getUpdateCacheMap()
{
    //std::cout<<"update id:"<<m_update_step<<std::endl;
    return m_cache_map[m_update_step];
}
void NM::CCache::updateCacheData()
{
    NM::CCacheManager* pCacheManager = NM::CCacheManager::getInstance();
    CacheMapType& globalCacheMap = pCacheManager->getglobalCacheMap();
    CacheMapType& updateCacheMap = getUpdateCacheMap();
    for(CacheMapType::iterator itr = globalCacheMap.begin();itr != globalCacheMap.end();++itr)
    {
        updateCacheMap[itr->first] = itr->second;
    }

    m_lock.Lock();
    int tmp = m_query_step;
    m_query_step = m_update_step;
    m_update_step = tmp;
    m_lock.unLock();

}
void NM::CCacheThreadRun::run()
{
    m_stop = false;
    int waittime;
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    pconfig->getconfigInfo("textquery","cacheupdatetime",waittime);
    while(!m_stop)
    {
        updateCache();
        sleep(waittime);
    }
}
void NM::CCacheThreadRun::updateCache()
{
//    std::cout<<"cache updat thread"<<std::endl;
    NM::CCacheManager* pCacheManager = NM::CCacheManager::getInstance();
    NM::CCacheManager::CacheMapType& cacheMap = pCacheManager->getCacheMap();
    NM::CCache::CacheMapType& globalCacheMap = pCacheManager->getglobalCacheMap();
    for(NM::CCacheManager::CacheMapType::iterator itr = cacheMap.begin();itr != cacheMap.end();++itr)
    {
        NM::CCache::CacheMapType& cacheMap = itr->second->getUpdateCacheMap();
        std::string word;
        for(NM::CCache::CacheMapType::iterator cacheMapItr = cacheMap.begin();cacheMapItr != cacheMap.end();++cacheMapItr)
        {
            word = cacheMapItr->first;
            NM::CCache::CacheVecType& cacheVec = cacheMapItr->second;
            globalCacheMap[word] = cacheVec;
        }
    }

    NM::CConfig* pconfig = NM::CConfig::getInstance();
    std::string cachepath;
    pconfig->getconfigInfo("textquery","cachepath",cachepath);
    std::fstream fout;
    fout.open(cachepath.c_str(),std::fstream::out|std::fstream::trunc);
    if(!fout.is_open())
    {
        std::cout<<"cachepath:"<<cachepath<<std::endl;
        exit(0);
    }
    for(NM::CCache::CacheMapType::iterator globalMapItr = globalCacheMap.begin();globalMapItr != globalCacheMap.end();++globalMapItr)
    {
        fout<<globalMapItr->first<<" ";
        NM::CCache::CacheVecType& cacheDataVec = globalMapItr->second;
        for(NM::CCache::CacheVecType::iterator cacheVecItr = cacheDataVec.begin();cacheVecItr != cacheDataVec.end();++cacheVecItr)
        {
            fout<<cacheVecItr->word<<"  "<<cacheVecItr->distance<<"  "<<cacheVecItr->freq<<" ";
        }
        fout<<"\n";
    }
    fout.close();

    for(NM::CCacheManager::CacheMapType::iterator itr = cacheMap.begin();itr != cacheMap.end();++itr)
    {
        itr->second->updateCacheData();
    }
}

void NM::CCacheThreadRun::stop()
{
    m_stop = true;
}
NM::CCacheManager* NM::CCacheManager::getInstance()
{
    return CSingleton<NM::CCacheManager>::getInstance();
}

NM::CCacheManager::CacheMapType& NM::CCacheManager::getCacheMap()
{
    return m_cache_map;
}
NM::CCache::CacheMapType& NM::CCacheManager::getglobalCacheMap()
{
    return m_globalCacheMap;
}
void NM::CCacheManager::start()
{
    m_cacheThread.start(&m_cacheThreadRun);
}
void NM::CCacheManager::stop()
{
    m_cacheThreadRun.stop();
}


