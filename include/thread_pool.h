#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <vector>
#include "queue.h"
#include "thread.h"
namespace MY_THREAD
{
    class CThreadPoolRun:public CThreadRun
    {
        public:
            CThreadPoolRun(CQueue* pque):m_pque(pque)
        {
        }
            void run()
            {
                while(1)
                {
                   CTask* ptask;
                   if(!m_pque->consume(&ptask))
                       return;
                   ptask->execute();
                   delete ptask;
                }
            }

        private:
            CQueue* m_pque;
    };

    struct CThreadInfo
    {
        pthread_t m_threadId;
    };
    class CThreadPool
    {
        public:
            CThreadPool(int nworkers=0,int ncapacity=0):m_worker(nworkers),m_que(ncapacity),m_run(&m_que),m_stop(true)
        {
            m_threadInfo = new CThreadInfo[nworkers];
        }
            ~CThreadPool()
            {
                delete[] m_threadInfo;
            }
            
            void on()
            {
                if(m_stop == true)
                {
                    m_que.setstop(false);
                    m_stop = false;
                    for(int index=0;index< m_worker.size();++index)
                    {
                        m_threadInfo[index].m_threadId = m_worker[index].start(&m_run);
                    }
                }
            }

            CThreadInfo* getThreadInfo()
            {
                return m_threadInfo;
            }

            void off()
            {
                if(m_stop == false)
                {
                    m_que.setstop(true);
                    m_stop = true;                    
                }
            }
            bool addTask(CTask* ptask) //阻塞函数
            {
                m_que.produce(ptask);
            }
        private:
            std::vector<CThread> m_worker;
            MY_THREAD::CQueue m_que;
            CThreadPoolRun m_run;
            CThreadInfo* m_threadInfo; 
            bool m_stop;

    };
}
#endif
