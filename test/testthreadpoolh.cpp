#include "thread_pool.h"
#include <iostream>
#include <stdlib.h>
class Cmytask:public MY_THREAD::CTask
{
    public:
        Cmytask(int val = 0):m_val(val)
    {
    }
        void execute()
        {
            std::cout<<"exe val:"<<m_val<<std::endl;
        }
        
    private:
        int m_val;
};
int main(int argc,char* argv[])
{
    MY_THREAD::CThreadPool m_threadpool(5,10);
    m_threadpool.on();
    int cnt = 10;
    while(cnt)
    {
        int val = rand() % 100;
        std::cout<<"add val:"<<val<<std::endl;
        m_threadpool.addTask(new Cmytask(val));
        cnt--;
    }
    while(1);


}
