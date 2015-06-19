#include "topk.h"
#include <iostream>
#include <vector>
#include <string>

struct Data
{
    int m_a;
    int m_b;
    Data(int a,int b):m_a(a),m_b(b)
    {
    }
    bool operator>(const Data& obj)const
    {
        if(m_a != obj.m_a)
            return m_a> obj.m_a;
        else
            return m_b > obj.m_b;
    }
};
template<class Iterator,class T>
Iterator find(Iterator begin,Iterator end,const T& value)
{
    while(begin != end && *begin != value)
        ++begin;
    return begin;
}
int main(int argc,char* argv[])
{
   /* NM::CTopk<std::string> topk(3);
    std::vector<std::string> vec;
    vec.push_back("a");
    vec.push_back("g");
    vec.push_back("b");
    vec.push_back("e");
    vec.push_back("c");
    vec.push_back("d");
    vec.push_back("f");
    std::deque<std::string> deq;
    topk(vec.begin(),vec.end(),deq);
    for(int i=0;i<deq.size();i++)
    {
        std::cout<<deq[i]<<std::endl;
    }
    */
    NM::CTopk<Data> topk(3);

    topk.push(Data(4,5));
    topk.push(Data(2,3));
    topk.push(Data(3,1));
    std::deque<Data> deq;
    topk.gettopk(deq);
    for(int i=0;i<deq.size();i++)
    {
        std::cout<<deq[i].m_a<<":"<<deq[i].m_b<<std::endl;
    }
   
}
