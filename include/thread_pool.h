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
                   CPtask ptask;
                   if(!m_pque->consume(ptask))
                       return;
                   ptask->execute();
                   ptask.destroy();
                }
            }

        private:
            CQueue* m_pque;

    };

    class CThreadPool
    {
        public:
            CThreadPool(int nworkers=0,int ncapacity=0):m_worker(nworkers),m_que(ncapacity),m_run(&m_que)
        {
        }
            getInstance()
            {

            }
            void on()
            {
                if(m_stop == true)
                {
                    std::vector<CThread>::iterator itr = m_worker.begin();
                    for(;itr != m_worker.end();++itr)
                    {
                        itr->start(&m_run);
                    }
                    m_que.setstop(false);
                    m_stop = false;
                }
            }
            void off()
            {
                if(m_stop == false)
                {
                    m_que.setstop(true);
                    m_stop = true;                    
                }
            }
            bool addTask(CPtask ptask) //阻塞函数
            {
                m_que.produce(ptask);
            }
        private:
            std::vector<CThread> m_worker;
            MY_THREAD::CQueue m_que;
            CThreadPoolRun m_run;
            bool m_stop;

    };
}
#endif
