#include "config.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
void str_trim(std::string& src)
{
    std::string::iterator itr = src.begin();
    while(itr != src.end())
    {
        if(*itr == ' '|| *itr == '\n')
        {
            itr = src.erase(itr);
        }
        else
            itr++;
    }
}
int NM::CConfig::getconfigInfo(const std::string& appName,const std::string& keyName,std::string& value)
{
    std::fstream fin;
    fin.open(m_configpath.c_str());
    if(!fin.is_open())
    {
        std::cout<<"config file"<<m_configpath<<"open failed"<<std::endl;
        exit(0);
    }
    std::string line;
    std::stringstream sout;
    sout<<"["<<appName<<"]";
    bool find_appName = false;
    size_t pos;
    std::string key;
    bool ret = false;
    while(std::getline(fin,line))
    {
        str_trim(line);
        if(!find_appName)
        {
            if(line.compare(sout.str()) == 0)
            {
                find_appName = true;
            }
        }
        else
        {
            if( (pos = line.find("=")) != std::string::npos && pos != line.length())
            {
                key = line.substr(0,pos);
                if(key.compare(keyName) == 0)
                {
                    std::string keyval = line.substr(pos + 1);
                    value.swap(keyval);
                    ret = true;
                    break;
                }

            }

        }

    }
    fin.close();
    return ret;
}


int NM::CConfig::getconfigInfo(const std::string& appName,const std::string& keyName,int& value)
{
    std::string strval;
    getconfigInfo(appName,keyName,strval);
    value = atoi(strval.c_str());
}
