#include "udp_transport.h"
#include "socket.h"
#include <iostream>
#include <string>
#include "config.h"
int main(int argc,char* argv[])
{
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    MY_NET::CSocket sock(sockfd);
    NM::CConfig* pconfig = NM::CConfig::getInstance();
    std::string server_ip;
    int server_port;
    pconfig->getconfigInfo("textquery","IP",server_ip);
    pconfig->getconfigInfo("textquery","port",server_port);
    MY_NET::CUdpTransport udp(sockfd,server_ip,server_port);
    char buf[UDP_MAX_SIZE];
    while(memset(buf,0,UDP_MAX_SIZE),fgets(buf,UDP_MAX_SIZE,stdin)!= NULL)
    {
        if(buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1]='\0';
        udp.send(buf,strlen(buf));
        std::cout<<"send:"<<buf<<std::endl;
        udp.recv(buf,UDP_MAX_SIZE);
        std::cout<<"recv:"<<buf<<std::endl;
    }
    close(sockfd);


}
