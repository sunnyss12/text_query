#ifndef __CACHE_H__
#define __CACHE_H__
#include <ext/hash_map>
#include <string>
#include "common.h"
#include "thread.h"
#include <vector>
#include "mutex.h"
#include "thread.h"
#include "thread_pool.h"
#include "textQueryServer.h"
#include "config.h"
namespace NM
{
    struct CacheData
    {
        std::string word;
        int distance;
        int freq;
        CacheData(std::string w,int d,int f):word(w),distance(d),freq(f)
        {
        }
    };
    class CCache
    {
        public:
            typedef std::vector<CacheData> CacheVecType;
            typedef __gnu_cxx::hash_map<std::string,CacheVecType,NM::CMyHash> CacheMapType;
            CCache();
            bool searchCacheData(const std::string& word,CacheVecType& queryVec);
            void addCacheData(const std::string& word,CacheVecType& cacheData);
            void updateCacheData();
            CacheMapType& getUpdateCacheMap();
            void loadCacheFromFile();
        private:
            CacheMapType m_cache_map[2]; //关键字、索引vec的下标.使用双缓冲，避免在cacheupate线程和addcache线程加锁，增加性能
            int m_query_step; //标记当前query在用双缓冲的哪一个cache
            int m_update_step;
            MY_THREAD::CMutex m_lock;
    };

    class CCacheThreadRun:public MY_THREAD::CThreadRun
    {
        public:
            void run();
            void updateCache();
            void stop();
        private:
            bool m_stop;


    };
    class CThreadHash
    {
        public:
            pthread_t operator()(pthread_t obj)const
            {
                return obj;
            }
    };
    class CCacheManager
    {
        public:
            CCacheManager()
            {
                NM::CTextQueryServer* ptextQueryServer = NM::CTextQueryServer::getInstance();
                MY_THREAD::CThreadPool* threadPool = ptextQueryServer->getThreadPool();
                MY_THREAD::CThreadInfo* pthreadInfo = threadPool->getThreadInfo();
                NM::CConfig* pconfig = NM::CConfig::getInstance();
                int thread_num;
                pconfig->getconfigInfo("textquery","threadnum",thread_num);
                for(int i=0;i<thread_num;i++)
                {
                    CCache* pcache = new CCache;
                    m_cache_map[pthreadInfo[i].m_threadId] = pcache;
                }
            }
            ~CCacheManager()
            {
                for(CacheMapType::iterator itr = m_cache_map.begin();itr!=m_cache_map.end();++itr)
                {
                    delete itr->second;
                }
            }
            typedef __gnu_cxx::hash_map<pthread_t,CCache*,CThreadHash> CacheMapType;
            static CCacheManager* getInstance();
            CacheMapType& getCacheMap();
            CCache* getCache(pthread_t threadId)
            {
                return m_cache_map[threadId];
            }
            CCache::CacheMapType& getglobalCacheMap();
            void start();
            void stop();
        private:
            CCache:: CacheMapType m_globalCacheMap;
            CacheMapType m_cache_map;
            MY_THREAD::CThread m_cacheThread;
            CCacheThreadRun m_cacheThreadRun;

    };

}
#endif
