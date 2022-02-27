/*
 * author: YYJ
 * date: 2022.02.27
 * discription: one to one chatting program server
 */
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>

#define SERVER_PORT 9000

int main()
{
    int c_socket, s_socket; /* socket descryption value */
    struct sockaddr_in s_addr, c_addr;
    int len;
    int n;

    s_socket = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
    memset(&s_addr, 0, sizeof(s_addr));
    /* setting structure sockaddr_in */
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr = htonl(INADDR_ANY); /* all ip allow */
    s_addr.sin_port = htons(SERVER_PORT); 

    /* 소켓을 포트에 연결 */
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
    {
        printf("[SERVER] - [ERROR] : Fail bind\n");
        return -1;
    }

    /* 커널에 개발 요청 */
    if(listen(s_socket, 5) == -1)
    {
        printf("[SERVER] - [ERROR] : Fail listen\n");
    }

    while(1)
    {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *)&c_addr, (socklen_t)&len);
        /* 서비스가 들어갈 자리.
        write(c_socket, ....);
        close(c_socket);
        /*
    }
}