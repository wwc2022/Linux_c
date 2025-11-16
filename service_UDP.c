#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>

#define MAXMSG 1024

int main()
{
    int sockfd,size,nbytes;
    struct sockaddr_in addr;
    char message[MAXMSG];
    //
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){printf("sockfd create failed!\n");return -1;}
    //
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6666);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //
    if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0){printf("bind failed!\n");return -1;}
    //
    while(1)
    {
        size = sizeof(addr);
        nbytes = recvfrom(sockfd,message,MAXMSG,0,(struct sockaddr*)&addr,&size);
        if(nbytes<0){printf("recvfrom(server)failed.\n");return -1;}
        printf("service got message: %s \n",message);
        nbytes = sendto(sockfd,message,nbytes,0,(struct sockaddr*)&addr,size);
        if(nbytes<0){printf("sendto(server) failed.\n");return -1;}
    }
}