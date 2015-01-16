#ifndef __DICTION_H__
#define __DICTION_H__
#include <string>
#include <ext/hash_map>
#include <set>
#include "singleton.h"
#include "common.h"
namespace NM
{
    class CDiction                                                          
    {   
        friend class CSingleton<CDiction>;
        public:                                                         
        static CDiction* getInstance();                              
        void make_dict();                                            
        private:                                                        
        CDiction();
        ~CDiction();
        void getstopword();
        void getstopword(std::string& filepath);
        void getwordfreq_dir(std::string& dirpath);
        void getwordfreq_file(std::string& filepath);
        void wordfreq_savetofile();

        std::string m_distdir;//语料库路径                          
        std::string m_datadir;//分词词典路径                        
        std::string m_stopdir;//停用词路径                          
        std::string m_wordfreqpath;//词频存放的文件名               
        __gnu_cxx::hash_map<std::string,int,NM::CMyHash> m_word_freq;
        std::set<std::string> m_set_stop;//或者英文一个停用词，中文一个停用词。英文只需要用英文停用词判断                                                    
    };                                                                   

}
#endif
