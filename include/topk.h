#ifndef __TOPK_H__
#define __TOPK_H__
#include <queue>
#include <vector>
#include <deque>
#include <functional>
namespace NM
{
    template<class T,class Compare = std::greater<typename std::vector<T>::value_type> >
        class CTopk
        {
            public:
                CTopk(int k):m_k(k)
            {
            }
                template<class InputIterator>
                void operator()(InputIterator first,InputIterator last,std::deque<T>& deq)
                {
                    for(;first != last;++first)
                    {
                        if(m_queue.size() < m_k)
                        {
                            m_queue.push(*first);
                        }
                        else if(m_comp(*first,m_queue.top()))
                        {
                            m_queue.pop();
                            m_queue.push(*first);
                        }

                    }
                    while(!m_queue.empty())
                    {
                        deq.push_front(m_queue.top());
                        m_queue.pop();
                    }

                }

                void push(const T& val)
                {
                    if(m_queue.size() < m_k)
                    {
                        m_queue.push(val);
                    }
                    else if(m_comp(val,m_queue.top()))
                    {
                        m_queue.pop();
                        m_queue.push(val);
                    }
                }
                void gettopk(std::deque<T>& deq)
                {
                    while(!m_queue.empty())
                    {
                        deq.push_front(m_queue.top());
                        m_queue.pop();
                    }
                }


            private:
                std::priority_queue<T,std::vector<T>,Compare > m_queue;
                Compare m_comp;
                int m_k;
        };
}
#endif
