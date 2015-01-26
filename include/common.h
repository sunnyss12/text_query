#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdlib.h>
#include <stdio.h>
#include <ext/hash_map>
#include <fstream>
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

namespace NM
{
 struct CMyHash
    {
        int operator()(const std::string& obj)const
        {
            __gnu_cxx::hash<const char*> tmp_hash;
            return static_cast<int>(tmp_hash(obj.c_str()));
        }
    };
}

bool isFileExist(const char* filepath);
#endif
