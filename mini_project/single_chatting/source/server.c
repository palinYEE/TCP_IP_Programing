/*
 * author: YYJ
 * date: 2022.02.27
 * discription: one to one chatting program server
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<time.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>

#define SERVER_PORT 9000
#define ID_LEN 8
#define CHATTING_ROOM_NUM 100

typedef struct ID
{
    char id_value;  /* 고유 ID 값 */
    struct ID *next;      /* 링크드 리스트 다음 구조체 포인터 */
    struct ID *before;    /* 링크드 리스트 이전 구조체 포인터 */
} _ID;

_ID Chatting_room[CHATTING_ROOM_NUM];

/* 고유 ID 값을 만드는 함수 */
void createID(char *buf, int num)
{
    int rndVal = 0;
    srand((unsigned)time(NULL));
    for(int i=0; i<num; i++)
    {   
        rndVal = (int)(rand()%26); 
        if(rndVal < 10) 
        {
            sprintf(buf + i, "%d", rndVal);
        }else if(rndVal > 35) 
        { 
            sprintf(buf + i, "%c", rndVal + 61);
        }else { 
            sprintf(buf + i, "%c", rndVal + 55); 
        } 
    }
}

void storeID(_ID *id, char *buf)
{
    memcpy(&(id->id_value), buf, sizeof(buf));
}

int main()
{
    int c_socket, s_socket; /* socket descryption value */
    struct sockaddr_in s_addr, c_addr;
    int len;
    int n;
    int chatting_room_count = 0;

    char randomID[ID_LEN]; /* random ID */

    s_socket = socket(PF_INET, SOCK_STREAM, 0); /* create socket */
    memset(&s_addr, 0, sizeof(s_addr));
    /* setting structure sockaddr_in */
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* all ip allow */
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
        c_socket = accept(s_socket, (struct sockaddr *)&c_addr, (socklen_t *)&len);
        createID(randomID, ID_LEN);
        write(c_socket, randomID, strlen(randomID));
        storeID(Chatting_room, randomID);
        chatting_room_count+= 1;
        close(c_socket);
    }
}
