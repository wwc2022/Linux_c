#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTENQ 5  // 监听队列的最大长度

// 全局变量声明
int listenfd, connfd;  // 监听套接字和连接套接字

// 信号处理函数声明
void sig_urg(int signo);

int main()
{
    int n;                    // 读取的字节数
    char buff[100];           // 数据缓冲区
    socklen_t len;            // 地址结构体长度
    struct sockaddr_in servaddr, cliaddr;  // 服务器和客户端地址结构体
    
    // === 1. 配置服务器地址 ===
    servaddr.sin_family = AF_INET;           // IPv4地址族
    servaddr.sin_port = htons(6666);         // 端口6666，转换为网络字节序
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 绑定到所有网络接口
    
    // === 2. 配置信号处理 ===
    struct sigaction action;                  // 信号动作结构体
    action.sa_handler = sig_urg;             // 设置信号处理函数
    sigemptyset(&action.sa_mask);            // 清空信号掩码（不阻塞其他信号）
    action.sa_flags = 0;                     // 无特殊标志
    
    // === 3. 创建监听套接字 ===
    listenfd = socket(AF_INET, SOCK_STREAM, 0);  // 创建TCP套接字
    if(listenfd < 0){
        printf("socket created failed.\n");
        return -1;
    }
    
    // === 4. 绑定地址到套接字 ===
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("bind failed!\n");
        close(listenfd);
        return -1;
    }
    
    // === 5. 开始监听 ===
    printf("listening...\n");
    listen(listenfd, LISTENQ);  // 设置套接字为监听状态，队列长度为5
    
    // === 6. 接受客户端连接 ===
    len = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
    // 程序会在这里阻塞，直到有客户端连接
    
    // === 7. 注册信号处理并设置套接字所有者 ===
    if(sigaction(SIGURG, &action, NULL) == -1){
        printf("couldn't register signal handler.\n");
        close(connfd);
        close(listenfd);
        return -2;
    }
    
    // 设置套接字所有者，这样当有带外数据时，当前进程会收到SIGURG信号
    fcntl(connfd, F_SETOWN, getpid());
    
    // === 8. 主循环：处理普通数据 ===
    while (1)
    {
        // 读取普通数据（阻塞调用）
        n = read(connfd, buff, sizeof(buff));
        
        if(n == 0){  // 读到0表示客户端关闭连接（EOF）
            printf("received EOF\n");
            break;
        } else if(n < 0){  // 读取错误
            perror("read failed");
            break;
        }
        
        buff[n] = 0;  // 添加字符串终止符
        printf("read %d byte: %s \n", n, buff);
    }
    
    // === 9. 清理资源 ===
    close(connfd);
    close(listenfd);
    return 0;
}

// === 10. 带外数据信号处理函数 ===
void sig_urg(int signo)
{
    int n;
    char buff[100];
    
    printf("SIGURG received - 检测到带外数据!\n");
    
    // 使用MSG_OOB标志读取带外数据（紧急数据）
    n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
    
    if(n > 0) {
        buff[n] = 0;  // 添加字符串终止符
        printf("read %d OOB byte: %s\n", n, buff);
    } else if(n == 0) {
        printf("连接已关闭\n");
    } else {
        perror("recv MSG_OOB failed");
    }
}