#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <pthread.h>
#include <stdexcept>
namespace MY_THREAD
{
    class CCond;
}
namespace MY_THREAD
{
    class CMutex
    {
        private:
            pthread_mutex_t m_mutex;
            CMutex(const CMutex& obj);
            CMutex& operator=(const CMutex& obj);
        public:
            friend class MY_THREAD::CCond;
            CMutex()
            {
                if(pthread_mutex_init(&m_mutex,NULL))
                    throw std::runtime_error("mutex init error");
            }
            void Lock()
            {
                if(pthread_mutex_lock(&m_mutex))
                    throw std::runtime_error("mutex lock error");
            }
            void unLock()
            {
                if(pthread_mutex_unlock(&m_mutex))
                    throw std::runtime_error("mutex unlock error");
            }
            ~CMutex()
            {
                pthread_mutex_destroy(&m_mutex);
            }

    };
}
#endif
