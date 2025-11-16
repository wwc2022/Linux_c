#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
// 主要功能：
// IPv4/IPv6地址结构定义
// 字节序转换函数
// 协议常量定义
// 包含的重要结构体和函数：
// 核心结构体：
// c
// // IPv4地址结构体
// struct sockaddr_in {
//     sa_family_t    sin_family;   // 地址族：AF_INET
//     in_port_t      sin_port;     // 16位端口号
//     struct in_addr sin_addr;     // 32位IP地址
//     char           sin_zero[8];  // 填充字段
// };
// // IP地址结构体
// struct in_addr {
//     in_addr_t s_addr;  // 32位IPv4地址
// };
// 字节序转换函数：
// c
// // 主机字节序 → 网络字节序
// uint16_t htons(uint16_t hostshort);  // 端口转换
// uint32_t htonl(uint32_t hostlong);   // IP地址转换

// // 网络字节序 → 主机字节序  
// uint16_t ntohs(uint16_t netshort);
// uint32_t ntohl(uint32_t netlong);
// 协议常量：
// c
// #define IPPROTO_TCP   6   // TCP协议
// #define IPPROTO_UDP  17   // UDP协议
// #define INADDR_ANY   0   // 任意地址
#include<netdb.h>
// 主要功能：
// 主机名解析
// 服务名解析
// 网络数据库查询
// 包含的重要结构体和函数：
// 核心结构体：
// c
// // 主机信息结构体
// struct hostent {
//     char  *h_name;        // 官方主机名
//     char **h_aliases;     // 别名列表
//     int    h_addrtype;    // 地址类型：AF_INET
//     int    h_length;      // 地址长度
//     char **h_addr_list;   // 地址列表
// };
// // 服务信息结构体  
// struct servent {
//     char  *s_name;        // 官方服务名
//     char **s_aliases;     // 别名列表
//     int    s_port;        // 端口号（网络字节序）
//     char  *s_proto;       // 协议名："tcp"/"udp"
// };
// 主要函数：
// c
// // 主机名解析
// struct hostent *gethostbyname(const char *name);
// struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type);
// // 服务名解析
// struct servent *getservbyname(const char *name, const char *proto);
// struct servent *getservbyport(int port, const char *proto);
#include<sys/socket.h>
#include<unistd.h>

#define MAXMSG 512

int main()
{
    int sockfd;
    int n;
    char revcbuff[MAXMSG],sndbuff[MAXMSG];
    struct sockaddr_in servaddr;
    //
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){printf("socket create failed.\n");return -1;}
    //
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //
    while(fgets(sndbuff,MAXMSG,stdin)!=NULL)
    {
        if(sendto(sockfd,sndbuff,sizeof(sndbuff),0,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0){printf("(client)sending error.\n");return -1;}
    }
    if (( n =recvfrom(sockfd,revcbuff,MAXMSG,0,NULL,NULL))<0)
    {
        printf("(client)receiving error.\n");return -1;
    }
    close(sockfd);
    return 0;
}