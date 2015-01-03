#ifndef __COND_H__
#define __COND_H__
#include <pthread.h>
#include <stdexcept>
#include "mutex.h"
namespace MY_THREAD
{
    class CCond
    {
        private:
            pthread_cond_t m_cond;
            CMutex& m_mutex;
            CCond(const CCond& obj);
            CCond& operator=(const CCond& obj);
        public:
           CCond(CMutex& mutex):m_mutex(mutex)
        {
            if(pthread_cond_init(&m_cond,NULL))
                throw std::runtime_error("cond init error");
        }
           void Wait()
           {
               if(pthread_cond_wait(&m_cond,&m_mutex.m_mutex))
                   throw std::runtime_error("cond wait error");
           }
           void Notify()
           {
               if(pthread_cond_signal(&m_cond))
                   throw std::runtime_error("cond signal error");
           }
           void NotifyAll()
           {
               if(pthread_cond_broadcast(&m_cond))
                   throw std::runtime_error("cond broadcast error");
           }
           ~CCond()
           {
               pthread_cond_destroy(&m_cond);
           }

            
    };
}

#endif
