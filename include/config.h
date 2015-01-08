#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <string>
namespace NM
{
    class CConfig
    {
        private:
            std::string m_configpath;
        public:
            CConfig(const std::string& configpath):m_configpath(configpath)
        {
        }
            int getconfigInfo(const std::string& appName,const std::string& keyName,std::string& value);
            int getconfigInfo(const std::string& appName,const std::string& keyName,int& value);
    
    };
}
#endif
