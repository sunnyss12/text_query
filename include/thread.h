#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <stdexcept>
namespace MY_THREAD
{
    class CThreadRun
    {
        public:
            virtual void  run()=0;
            virtual ~CThreadRun()
            {
            }
    };
    class CThread
    {
        public:
            CThread() //虽然默认构造函数用不到，但是还是有必要写上，因为vector会调默认构造函数。
            {
            }
            ~CThread()
            {
               
            }
            void start(CThreadRun* run)
            {
                m_thread = pthread_create(&m_thread,NULL,thread_run,run);
                if(m_thread)
                   throw std::runtime_error("thread create failed");
            }
        private:
            pthread_t m_thread; 
            CThreadRun* m_run;
            static void* thread_run(void* arg)
            {
                pthread_detach(pthread_self());
                CThreadRun* obj = (CThreadRun*)arg;
                obj->run();
            }
    };
}
#endif

