#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
// netinet.h (实际应为 netinet/in.h)
// 内容	                   说明
// struct sockaddr_in	IPv4地址结构体
// struct in_addr	    IP地址结构体
// 字节序转换	         htons(), htonl(), ntohs(), ntohl()
// 协议定义	             IPPROTO_TCP, IPPROTO_UDP
#include<netdb.h>
// netdb.h
// 内容	               说明
// struct hostent	主机信息结构体
// gethostbyname()	通过主机名获取IP地址
// getservbyname()	通过服务名获取端口号
#define MAXBUFFSIZE 256
#define PORT 6666
#define HOST_ADDR "127.0.0.1"


int main()
{
    int sockfd,n;
    char recvbuff[MAXBUFFSIZE];
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){perror("socket create failed.\n");return -1;}

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("connectong......\n");
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){perror("connect server failed.\n");return -1;}

    while( (n =read(sockfd,recvbuff,MAXBUFFSIZE)) >0 )
    {
        recvbuff[n] = 0;
        fputs(recvbuff,stdout);
    }
    if(n<0){printf("Read failed!\n");return -2;}

    return 0;
}


// 客户端流程
// socket() → connect() → read()/write() → close()