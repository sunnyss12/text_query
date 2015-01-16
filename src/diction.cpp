#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "common.h"
#include "diction.h"
#include "config.h"
#include "gbk2utf_8.h"
#include "NLPIR.h"
NM::CDiction::CDiction()
{
    CConfig* pconfig = NM::CConfig::getInstance();
    pconfig->getconfigInfo("textquery","distdir",m_distdir);//yuliao dir
    pconfig->getconfigInfo("textquery","datadir",m_datadir);//fenci dir
    pconfig->getconfigInfo("textquery","stopdir",m_stopdir);//tingliuci dir
    pconfig->getconfigInfo("textquery","wordfreqpath",m_wordfreqpath);
    if(!NLPIR_Init(m_datadir.c_str(),UTF8_CODE))
    {
        std::cout<<"NLPLR Init Fail!"<<std::endl;
        exit(1);
    }
}
NM::CDiction::~CDiction()
{
    NLPIR_Exit();
}
NM::CDiction* NM::CDiction::getInstance()
{
    return CSingleton<NM::CDiction>::getInstance();
}
void NM::CDiction::getstopword()
{
    DIR* pdir = opendir(m_stopdir.c_str());
    if(pdir == NULL)
        ERR_EXIT("opendir");
    struct dirent* pdirent;
    std::string filepath;
    while((pdirent = readdir(pdir)) != NULL)
    {
        filepath = pdirent->d_name;
        if(filepath == "." || filepath == "..")
            continue;

        filepath = m_stopdir + "/" + pdirent->d_name;
        getstopword(filepath);
    }
    closedir(pdir);
}
void NM::CDiction::getstopword(std::string& filepath)
{
    std::fstream fin;
    fin.open(filepath.c_str());
    if(!fin.is_open())
    {
        std::cout<<filepath<<"open failed"<<std::endl;
        exit(0);
    }
    std::string stopword;
    while(fin>>stopword)
    {
        m_set_stop.insert(stopword);
    }
    fin.close();

}
void NM::CDiction::getwordfreq_dir(std::string& dirpath)
{
    DIR* pdir = opendir(dirpath.c_str());
    if(pdir == NULL)
        ERR_EXIT("opendir");
    struct dirent* pdirent;
    std::string filepath;
    struct stat statbuf;
    while((pdirent = readdir(pdir)) != NULL)
    {
        filepath = pdirent->d_name;
        if(filepath == "." || filepath == "..")
            continue;
        filepath = dirpath + "/" + pdirent->d_name;
        if( 0 !=stat(filepath.c_str(),&statbuf))
            ERR_EXIT("stat");
        if(S_ISDIR(statbuf.st_mode))
        {
            getwordfreq_dir(filepath);
        }
        else if(S_ISREG(statbuf.st_mode))
        {
            getwordfreq_file(filepath);
        }
    }
    closedir(pdir);
}
void NM::CDiction::getwordfreq_file(std::string& filepath)
{
    std::cout<<"filepath"<<filepath<<std::endl;
    std::fstream fin;
    fin.open(filepath.c_str());
    if(!fin.is_open())
    {
        std::cout<<filepath<<"open failed"<<std::endl;
        exit(0);
    }
    std::string line;
    std::string partiline;
    std::stringstream sin;
    std::string partiword;
    std::string str_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    int pos;
    MY_LANGTRAN::CLang lang;
    char* utfStr = NULL;
    int ret = -1;
    while(std::getline(fin,line))
    {
        if(line.size() == 0)
            continue;
        ret = lang.gbk2utf8(&utfStr,line.c_str());
        if(ret == -1)
            continue;
        line = utfStr;
        lang.destroy(&utfStr);
        pos = 0;
        while( (pos = line.find_first_of(str_upper,pos)) != std::string::npos)
        {
            line[pos] = line[pos] + 'a' - 'A';
            pos++;
        }

        for(int i=0;i<line.size();i++)
            if(ispunct(line[i]))
                line[i]=' ';
        partiline = (char*)NLPIR_ParagraphProcess(line.c_str(),0);
        sin.str(partiline);
        //sin.str(line);
        sin.clear();
        while(sin>>partiword)
        {
            if(m_set_stop.find(partiword) != m_set_stop.end())
                continue;
            //if(partiword.size() == 1 && ispunct(partiword[0]))
            //    continue;
            m_word_freq[partiword]++;
        }
    }
    fin.close();
}
void NM::CDiction::wordfreq_savetofile()
{
    std::fstream fout;
    fout.open(m_wordfreqpath.c_str(),std::fstream::out|std::fstream::trunc);
    if(!fout.is_open())
    {
        std::cout<<m_wordfreqpath<<"cannot failed"<<std::endl;
        exit(0);

    }
    __gnu_cxx::hash_map<std::string,int,NM::CMyHash>::iterator itr = m_word_freq.begin();
    for(;itr!=m_word_freq.end();itr++)
    {
        fout<<std::left<<std::setw(20)<<itr->first<<std::left<<itr->second<<std::endl;
    }
    fout.close();

}

void NM::CDiction::make_dict()
{
    getstopword();
    getwordfreq_dir(m_distdir);
    wordfreq_savetofile();
}


