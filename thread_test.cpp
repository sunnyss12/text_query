#include <iostream>
#include "thread_pool.h"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
class run1:public MY_THREAD::CThreadRun
{
    public:
        run1(const std::string& str):m_str(str)
        {
        }
        void run()
        {
             std::cout<<"run1"<<m_str<<std::endl;
        }
    private:
        std::string m_str;
};
class run2:public MY_THREAD::CThreadRun
{
    public:
        run2()
        {
        }
        void run()
        {
            std::cout<<"run2"<<std::endl;
        }
};
class CMytask:public MY_THREAD::CTask
{
    public:
        CMytask(int val):m_val(val)
    {
    }
        void execute()
        {
            std::cout<<"execute:"<<m_val<<std::endl;
        }
        ~CMytask()
        {
            std::cout<<"~CMytask"<<std::endl;
        }
    private:
        int m_val;
    
};
class CPro:public MY_THREAD::CThreadRun
{
    private:
        MY_THREAD::CQueue& m_queue;
    public:
        CPro(MY_THREAD::CQueue& que):m_queue(que)
    {
    }
        void run()
        {
            while(1)
            {
                int val = rand()%100;
                m_queue.produce(new CMytask(val));
                std::cout<<"set val:"<<val<<std::endl;
               sleep(rand()%3+1);
            }
        }

};

class CCon:public MY_THREAD::CThreadRun
{
    private:
        MY_THREAD::CQueue& m_queue;
    public:
        CCon(MY_THREAD::CQueue& que):m_queue(que)
    {
    }
        void run()
        {
            while(1)
            {
                MY_THREAD::CPtask ptask;
                m_queue.consume(ptask);
                ptask->execute();
                ptask.destroy();
               sleep(rand()%3+1);
            }
        }
};
void test_thread1()
{
    MY_THREAD::CThread thread1;
    MY_THREAD::CThread thread2;
    run1 r1(std::string("hello"));
    run2 r2;
    thread1.start(&r1);
    thread2.start(&r2);
    
}
void test_thread2()
{
    MY_THREAD::CQueue queue(5);
    CPro pro(queue);
    CCon con(queue);
    std::vector<MY_THREAD::CThread> thd_pro(10);
    std::vector<MY_THREAD::CThread> thd_con(10);
    for(int i=0;i<thd_pro.size();i++)
    {
        thd_pro[i].start(&pro);
    }
    for(int i=0;i<thd_con.size();i++)
    {
        thd_con[i].start(&con);
    }
  
}
void test_threadpool1()
{
    MY_THREAD::CThreadPool thdpool(10,5);
    thdpool.on();
    int val;
    int cnt = 20;
    while(cnt)
    {
        val = rand()%100;
        std::cout<<"produce:"<<val<<std::endl;
        CMytask* ptask = new CMytask(val);
        thdpool.addTask(ptask);
        cnt--;
    }
    while(1);
}
int main(int argc,char* argv[])
{
    test_threadpool1();
}
