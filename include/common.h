#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdlib.h>
#include <stdio.h>
#define ERR_EXIT(m)\
    do{\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)
#endif
