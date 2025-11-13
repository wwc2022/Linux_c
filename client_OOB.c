#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 6666
#define HOST_ADDR "127.0.0.1"

int main()
{
    int sockfd,n;
    struct  sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){printf("socket creat failed.\n");return -1;}

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("connecting........\n");
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){printf("connect failed!");return -1;}

    //
    write(sockfd,"123",3);
    printf("wrote 3 byte of normal data.\n");
    sleep(1);
    //
    send(sockfd,"a",1,MSG_OOB);
    printf("wrote 1 byte of OOB.\n");
    sleep(1);
    //
    write(sockfd,"56",2);
    printf("wrote 2 byte of normal.\n");
    sleep(1);
    //
    send(sockfd,"b",1,MSG_OOB);
    printf("wrote 1 byte of normal data.\n");
    sleep(1);    
    //
    write(sockfd,"789",3);
    printf("wrote 3 byte of OOB.\n");
    sleep(1);
    //
    return 0;
}