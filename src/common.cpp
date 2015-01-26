#include "common.h"
bool isFileExist(const char* filepath)
{
    std::fstream fin;
    fin.open(filepath,std::fstream::in);
    if(!fin)
        return false;
    else
        return true;
}

