//      步骤	         当前状态	
// 1. socket()	    创建了"裸"套接字	
// 2. 配置servaddr	设置了地址信息	
// 3. bind()	    绑定IP和端口	
// 4. listen()	    开始监听连接	
// 5. accept()	    接受客户端连接	
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>

#define LISTENQ 5
#define MAXLINE 512 
 
 int main()
{
    int listenfd,connfd;
    socklen_t len;
    struct sockaddr_in servaddr,cliaddr;
    // 结体成员详解
    // 成员	                类型	      说明	         示例
    // sin_family	    sa_family_t	     地址族     	AF_INET
    // sin_port	         in_port_t	   16位端口号	  htons(8080)
    // sin_addr	      struct in_addr  IPv4地址结构体	见下文
    // sin_zero[8]	       char	        填充字节	    全0构
    // struct sockaddr_in (16字节)
    // ┌─────────────┬─────────────┬─────────────┬────────────────┐
    // │ sin_family  │   sin_port  │  sin_addr   │   sin_zero[8]  │
    // │   (2字节)   │   (2字节)   │  (4字节)    │    (8字节)     │
    // └─────────────┴─────────────┴─────────────┴────────────────┘
    char buff[MAXLINE];
    time_t ticks;

    listenfd =socket(AF_INET,SOCK_STREAM,0);//1、创建套接字
    //参数1：domain	地址族 - 指定通信域
    // AF_INET = IPv4网络协议
    // AF_INET6 = IPv6网络协议
    // AF_UNIX = 本地进程间通信
    //
    // 参数2：type	套接字类型
    // SOCK_STREAM = 面向连接的TCP套接字
    // SOCK_DGRAM = 无连接的UDP套接字
    // SOCK_RAW = 原始套接字
    //
    // 参数3：protocol	具体协议
    // 0 = 自动选择默认协议
    // IPPROTO_TCP = TCP协议
    // IPPROTO_UDP = UDP协议
    //
    // 常见组合	(AF_INET, SOCK_STREAM, 0) = TCP/IPv4套接字
    //          (AF_INET, SOCK_DGRAM, 0) = UDP/IPv4套接字
    // 错误代码	EACCES = 权限不足
    //         EAFNOSUPPORT = 不支持的地址族
    //         EMFILE = 进程文件描述符用完
    //         ENFILE = 系统文件描述符用完
    //
    // 参数组合	                      实际效果	            使用场景
    // AF_INET, SOCK_STREAM, 0	创建TCP/IPv4套接字	    网页服务器、文件传输
    // AF_INET, SOCK_DGRAM, 0	创建UDP/IPv4套接字	    视频流、DNS查询
    // AF_INET6, SOCK_STREAM, 0	创建TCP/IPv6套接字	    新一代网络应用
    // AF_UNIX, SOCK_STREAM, 0	创建本地进程通信套接字	 同一机器进程间通信
    if(listenfd<0){perror("socket created failed/\n");return -1;}

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family =AF_INET; //使用 IPv4 地址族
    servaddr.sin_port = htons(6666);//服务端口号 6666，htons() 转换字节序   ---htons()	主机字节序 → 网络字节序转换
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//绑定到所有网络接口   ---长整型主机字节序 → 网络字节序
    // 结构体初始化步骤
    //     步骤	                        代码	                                  说明
    // 1. 清空结构体	memset(&servaddr, 0, sizeof(servaddr));	               避免垃圾值
    // 2. 设置地址族	servaddr.sin_family = AF_INET;	                       必须设置
    // 3. 设置端口	    servaddr.sin_port = htons(6666);	                       网络字节序
    // 4. 设置IP地址	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	       网络字节序
    // 5. 填充字节	    自动补0	保持结构体大小
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){perror("bind failed.\n");return -1;}//绑定套接字到地址和端口

    printf("listening .....\n");
    listen(listenfd,LISTENQ);//监听   ---将套接字设置为被动监听模式，准备接受连接
    // 参数详解
    // 参数	     类型	      说明
    // sockfd	 int	    已绑定的套接字描述符
    // backlog	 int	    连接队列的最大长度

    while (1)
    {
        len = sizeof(cliaddr);
        connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);//接受
        printf("connect from %s,port %d \n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
        // inet_ntoa(cliaddr.sin_addr)	IP地址转换	网络字节序→点分十进制字符串
        // ntohs(cliaddr.sin_port)	端口转换	网络字节序→主机字节序
        ticks = time(NULL);
        sprintf(buff," % .24s \r \n",ctime(&ticks));
        write(connfd,buff,strlen(buff));   
        // 代码	                    功能	        说明
        // ticks = time(NULL)	获取当前时间	返回从1970.1.1开始的秒数
        // ctime(&ticks)	    时间格式转换	秒数→可读字符串
        // sprintf(buff, ...)	格式化字符串	将时间写入缓冲区
        close(connfd);
    }

}

// 服务器端流程
// socket() → bind() → listen() → accept() → read()/write() → close()

// 服务器程序
//     ↓
// time() → 获取当前时间
//     ↓
// ctime() → 转换为字符串
//     ↓
// sprintf() → 格式化到buff
//     ↓
// write(connfd) → 通过网络发送
//     ↓
// 网络传输
//     ↓
// 客户端read(sockfd) → 接收数据
//     ↓
// fputs(stdout) → 显示在屏幕