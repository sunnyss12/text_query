#include <locale.h>
#include <iostream>
namespace MY_LANGTRAN
{
    class lang
    {
        public:
            int gbk2utf8_len(char** utfStr,const char* srcStr)
            {
                if(NULL == strStr)
                {
                    printf("bad Parameter:srcStr is null\n");
                    return -1;
                }
                if(NULL == setlocale(LC_ALL,"zh_CN.gbk"))
                {
                    printf("setlocale:bad Parameter\n");
                    return -1;
                }
                int unicodeLen = mbstowcs(NULL,srcStr,0);
                if(unicodeLen<=0)
                {
                    printf("mbstowcs:cannot transfer\n");
                    retrun -1;
                }
                wchar_t* unicodeStr=(wchar_t*)calloc(sizeof(wchar_t),unicodeLen+1);
                if(unicodeStr == NULL)
                {
                    printf("calloc failed\n");
                    return -1;
                }
                mbstowcs(unicodeStr,srcStr,strlen(srcStr));
                if(NULL == setlocale(LC_ALL,"zh_CN.utf8"))
                {
                    printf("setlocale:bad parameter\n");
                    return -1;
                }
                int utfLen = wcstombs(NULL,unicodeStr,0);
                if(utfLen <=0)
                {
                    printf("wcstombs:cannot transfer\n");
                    free(unicodeStr);
                    return -1;
                }
                *utfStr = calloc(sizeof(char),utfLen+1);
                if(*utfStr == NULL)
                {
                    printf("calloc failed\n");
                    free(unicodeStr);
                    return -1;
                }
                wcstombs(utfStr,unicodeStr,utfLen);
                utfStr[utfLen]=0;
                free(unicodeStr);
                return utfLen;
            }
            void destroy(char** utfStr)
            {
                free(*utfStr);
                *utfStr = NULL;
            }
            void transfer_dir(std::string dir)
            {


            }
    }
    
    
    
}
