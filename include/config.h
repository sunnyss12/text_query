#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <string>
#include "singleton.h"
namespace NM
{
    class CConfig
    {
        friend class CSingleton<CConfig>;
        private:
            std::string m_configpath;
            CConfig():m_configpath("./data/config.ini")
        {
        }
        public:
            int getconfigInfo(const std::string& appName,const std::string& keyName,std::string& value);
            int getconfigInfo(const std::string& appName,const std::string& keyName,int& value);
            static CConfig* getInstance()
            {
                return CSingleton<CConfig>::getInstance();
            }
    
    };
}
#endif
