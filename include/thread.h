#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

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
                if(pthread_create(&m_thread,NULL,thread_run,run))
                    ERR_EXIT("pthread_create");                   
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

