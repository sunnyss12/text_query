#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include "mutex.h"
#include <cstdlib>
template <typename T>
class CSingleton
{
    public:
        static T* getInstance()
        {
            Init();
            return m_instance;
        }
    private:
        static T* volatile m_instance;
        static MY_THREAD::CMutex m_lock;
        static void Init()
        {
            if(m_instance == NULL)
            {
                m_lock.Lock();
                if(m_instance == NULL)
                {
                    m_instance = new T;
                    atexit(Destroy);
                }
                m_lock.unLock();
            }

        }
        static void Destroy()
        {
            delete m_instance;
        }
        CSingleton(const CSingleton& obj);
        CSingleton& operator=(const CSingleton& obj);
        CSingleton();
        ~CSingleton();
};
template<typename T>
T* volatile CSingleton<T>::m_instance = NULL;
template<typename T>
MY_THREAD::CMutex CSingleton<T>::m_lock;
#endif
