#include <iostream>
#include <ext/hash_map>
#include <set>
#include "NLPIR.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include "epoll.h"
#include "thread_pool.h"
#include "socket.h"
#include "config.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include "gbk2utf_8.h"
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
class CTextQueryTask:public MY_THREAD::CTask
{
    public:
        CTextQueryTask(int sockfd,const std::string & ip,int port,char* buf,int size):m_sockfd(sockfd),m_ip(ip),m_port(port),m_buf(buf),m_size(size)
    {
    }
        void execute()
        {
            std::cout<<"execute"<<m_ip<<":"<<m_port<<":"<<std::endl;
            int sockfd = socket(AF_INET,SOCK_DGRAM,0);
            MY_NET::CUdpTransport udp(sockfd,m_ip,m_port);
            udp.send(m_buf,strlen(m_buf));
            close(sockfd);
        }
    private:
        int m_sockfd;
        const std::string m_ip;
        int m_port;
        char* m_buf;
        int m_size;
};

extern void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size);

class textQueryServer
{
    friend class CTextQueryTask;
    public:
    textQueryServer(const std::string& configpath):m_config(configpath)
    {
        m_config.getconfigInfo("textquery","IP",m_ip);
        m_config.getconfigInfo("textquery","port",m_port);
        m_config.getconfigInfo("textquery","distdir",m_distdir);//yuliao dir
        m_config.getconfigInfo("textquery","datadir",m_datadir);//fenci dir
        m_config.getconfigInfo("textquery","stopdir",m_stopdir);//tingliuci dir
        m_config.getconfigInfo("textquery","threadnum",m_thread_num);
        m_config.getconfigInfo("textquery","taskcapacity",m_task_capacity);
        m_config.getconfigInfo("textquery","wordfreqpath",m_wordfreqpath);
        m_sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(-1 == m_sockfd)
            ERR_EXIT("socket");
        m_psocket = new MY_NET::CSocket(m_sockfd);
        m_psocket->bind(m_ip,m_port);
        m_pthreadpool = new MY_THREAD::CThreadPool(5,10);
        m_pepoll = new MY_NET::CEpoll(m_sockfd,1024);
        if(!NLPIR_Init(m_datadir.c_str(),UTF8_CODE))
        {
            std::cout<<"NLPLR Init Fail!"<<std::endl;
            exit(1);
        }

    }

    static MY_THREAD::CThreadPool* getThreadPool()
    {
        return m_pthreadpool;
    }
    void start()
    {
        m_pthreadpool->on();
        std::cout<<"start"<<std::endl;
        m_pepoll->setOnMessage(ontextQueryMessage);
        while(1)
        {
            std::cout<<"waitbefore"<<std::endl;
            m_pepoll->wait();
            std::cout<<"waitafter"<<std::endl;
            m_pepoll->HandleUDPRead();
        }
    }
    ~textQueryServer()
    {
        delete m_pepoll;
        delete m_pthreadpool;
        delete m_psocket;
        close(m_sockfd);
        NLPIR_Exit();

    }
    void getstopword()
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
    void getstopword(std::string& filepath)
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
    void getwordfreq_dir(std::string& dirpath)
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
    void getwordfreq_file(std::string& filepath)
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
    void wordfreq_savetofile()
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

    void init()
    {
        getstopword();
        getwordfreq_dir(m_distdir);
        wordfreq_savetofile();
    }

    private:
    NM::CConfig m_config;
    std::string m_ip;
    int m_port;
    std::string m_distdir;//语料库路径
    std::string m_datadir;//分词词典路径
    std::string m_stopdir;//停用词路径
    std::string m_wordfreqpath;//词频存放的文件名
    int m_thread_num;
    int m_task_capacity;
    int m_sockfd;
    MY_NET::CSocket* m_psocket;
    MY_NET::CEpoll* m_pepoll;
    static MY_THREAD::CThreadPool* m_pthreadpool;
    __gnu_cxx::hash_map<std::string,int,NM::CMyHash> m_word_freq;
    std::set<std::string> m_set_stop;//或者英文一个停用词，中文一个停用词。英文只需要用英文停用词判断

};
MY_THREAD::CThreadPool* textQueryServer::m_pthreadpool = NULL;
void ontextQueryMessage(int sockfd,const std::string& ip,int port,char* buf,int size)
{
    std::cout<<"ontextQueryMessage"<<std::endl;
    CTextQueryTask* ptask = new CTextQueryTask(sockfd,ip,port,buf,size);
    MY_THREAD::CThreadPool* pthreadpool = textQueryServer::getThreadPool();
    if(pthreadpool != NULL)
        pthreadpool->addTask(ptask);
}
int main(int argc,char* argv[])
{
    std::string configpath = argv[1];
    textQueryServer  server(configpath);
    server.init();
    server.start();
}
