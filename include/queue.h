#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "mutex.h"
#include "cond.h"
#include <queue>
namespace MY_THREAD
{
    class CTask
    {
        public:
            virtual void execute() = 0;
            virtual ~CTask()
            {
            }
    };
    class CPtask
    {
        private:
            CTask* m_ptask;
        public:
            CPtask(CTask* ptask = NULL):m_ptask(ptask)
        {
        }
            CTask* operator->()
            {
                return m_ptask;
            }
            CTask& operator*()
            {
                return *m_ptask;
            }
            void destroy()
            {
                delete m_ptask;
            }
       
    };
    class CQueue
    {
        private:
            CMutex m_lock;
            CCond m_pro_cond;
            CCond m_con_cond;
            std::queue<CTask*> m_queue;
            int m_capacity;
            bool m_stop;
        public:
            CQueue(int capacity):m_capacity(capacity),m_lock(),m_pro_cond(m_lock),m_con_cond(m_lock),m_stop(false)
        {
        }
            bool produce(CTask* ptask)
            {
                m_lock.Lock();
                while(m_queue.size() >= m_capacity && !m_stop)
                    m_pro_cond.Wait();
                if(m_stop)
                {
                    m_con_cond.NotifyAll();
                    return false;
                }
                m_queue.push(ptask);
                m_lock.unLock();
                m_con_cond.NotifyAll();
                return true;
            }
            bool consume(CTask** ptask)
            {
                m_lock.Lock();
                while(m_queue.empty() && !m_stop)
                    m_con_cond.Wait();
                if(m_stop)
                {
                    m_pro_cond.NotifyAll();
                    return false;
                }
                *ptask = m_queue.front();
                m_queue.pop();
                m_lock.unLock();
                m_pro_cond.NotifyAll();
                return true;
            }
            void setstop(int stop)
            {
                m_stop = stop;
            }

    };
}
#endif
