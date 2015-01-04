#include "udp_transport.h"
#include "socket.h"
#include <iostream>
#include <string>
int main(int argc,char* argv[])
{
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    MY_NET::CSocket sock(sockfd);
    sock.bind("192.168.1.116",9999);
    MY_NET::CUdpTransport udp(sockfd,std::string(argv[1]),atoi(argv[2]));
    char buf[UDP_MAX_SIZE];
    while(memset(buf,0,UDP_MAX_SIZE),fgets(buf,UDP_MAX_SIZE,stdin)!= NULL)
    {
        udp.send(buf,strlen(buf));
        std::cout<<"send:"<<buf<<std::endl;
        udp.recv(buf,UDP_MAX_SIZE);
        std::cout<<"recv:"<<buf<<std::endl;
    }
    close(sockfd);


}
