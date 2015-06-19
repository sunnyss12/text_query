#include <iostream>
#include "NLPIR.h"
#include <stdlib.h>
#include <stdio.h>
int main(int argc,char* argv[])
{
    if(!NLPIR_Init(argv[1],UTF8_CODE))
    {
        printf("Init Fail!\n");
        exit(1);
    }
    char* pres;
    std::string line;
    while(std::cin >> line)
    {
        pres = (char*)NLPIR_ParagraphProcess(line.c_str(),0);
        std::cout<<pres<<std::endl;
    }
    NLPIR_Exit();
    return 0;
}
