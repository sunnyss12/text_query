#include "index.h"
#include "singleton.h"
#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>
NM::CIndex* NM::CIndex::getInstance()
{
    return CSingleton<CIndex>::getInstance();
}
void NM::CIndex::loadIndexVecFromDiction()
{
    std::string dictpath;
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    pconfig->getconfigInfo("textquery","wordfreqpath",dictpath);
    std::fstream fin;
    fin.open(dictpath.c_str());
    if(!fin.is_open())
    {
        std::cout<<dictpath<<"open failed"<<std::endl;
        exit(0);
    }
    std::string line;
    std::stringstream sin;
    std::string word;
    int freq;
    while(std::getline(fin,line))
    {
        sin.str(line);
        sin.clear();
        sin>>word>>freq;
        m_index_vec.push_back(std::pair<std::string,int>(word,freq));
    }
    fin.close();
}
void NM::CIndex::buildIndexFromDiction()
{
    std::vector<std::string> letter_vec;
    std::vector<std::string>::iterator itr;
    for(int index = 0;index < m_index_vec.size();index++)
    {
       // if(index == 219023)
         //   std::cout<<m_index_vec[index].first<<std::endl;
        letter_vec.clear();
        splitWordToLetter(m_index_vec[index].first,letter_vec);
        for(itr = letter_vec.begin(); itr != letter_vec.end(); ++itr)
        {
           // if(index == 219023)
             //   std::cout<<*itr<<std::endl;
            m_index_map[*itr].insert(index);
        }
    }
}
void NM::CIndex::splitWordToLetter(const std::string& word,std::vector<std::string>& letter_vec)
{
    int index;
    std::string letter;
    for(index = 0;index<word.length();)
    {
        if(word[index]>0)
        {
            letter = word.substr(index,1);
            letter_vec.push_back(letter);
            index++;
        }
        else if( (word[index] & 0xE0) == 0xC0)
        {
            letter = word.substr(index,2);
            letter_vec.push_back(letter);
            index = index+2;
        }
        else if( (word[index] & 0xF0) == 0xE0)
        {
            letter = word.substr(index,3);
            letter_vec.push_back(letter);
            index = index+3;
        }
        else
        {
            letter = word.substr(index,4);
            letter_vec.push_back(letter);
            index = index + 4;
        }
        
    }
    
}
void NM::CIndex::saveIndexToFile()
{
    std::string indexpath;
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    pconfig->getconfigInfo("textquery","indexpath",indexpath);
    std::fstream fout;
    fout.open(indexpath.c_str(),std::fstream::out|std::fstream::trunc);
    if(!fout.is_open())
    {
        std::cout<<indexpath<<"create failed"<<std::endl;
        exit(0);
    }
    IndexMapType::iterator itr = m_index_map.begin();
    std::string letter;
    std::set<IndexVecType::size_type> index_set;
    std::set<IndexVecType::size_type>::iterator itr_set;
    for(;itr != m_index_map.end(); ++itr)
    {
        letter = itr->first;
        fout<<letter;
        index_set = itr->second;
        itr_set = index_set.begin();
        for(;itr_set != index_set.end();++itr_set)
        {
            fout<<" ";
            fout<<*itr_set;
            fout<<" ";
        }
        fout<<"\n";
    }
    fout.close();
}
void NM::CIndex::loadIndexMapFromIndexFile()
{
    std::cout<<"loadIndexMapFromIndexFile"<<std::endl;
    std::string indexpath;
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    pconfig->getconfigInfo("textquery","indexpath",indexpath);
    std::fstream fin;
    fin.open(indexpath.c_str());
    if(!fin.is_open())
    {
        std::cout<<indexpath<<"open failed"<<std::endl;
        exit(0);
    }
    std::string line;
    std::stringstream sin;
    std::string letter;
    IndexVecType::size_type index;
    while(std::getline(fin,line))
    {
        sin.str(line);
        sin.clear();
        sin>>letter;
        std::set<IndexVecType::size_type> index_set;
        while(sin>>index)
        {
            index_set.insert(index);
        }
        m_index_map[letter] = index_set;
    }
    fin.close();
}
void NM::CIndex::getQueryIndex(const std::string& word,std::set<IndexQueryVecType::size_type>& queryIndex)
{
    std::vector<std::string> letterVec;
    splitWordToLetter(word,letterVec);
    std::vector<std::string>::iterator itrVec;
    NM::CIndex::IndexMapType::iterator itrMap;
    for(itrVec = letterVec.begin();itrVec != letterVec.end();++itrVec)
    {
        itrMap = m_index_map.find(*itrVec);
        if(itrMap != m_index_map.end())
        {
            queryIndex.insert((*itrMap).second.begin(),(*itrMap).second.end());
        }
    }
}
