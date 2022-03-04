/*
 * author: YYJ
 * date: 2022.03.05
 * discription: one to one chatting program server
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<time.h>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>

#define SERVER_PORT 9000
#define ID_LEN 8
#define CHATTING_ROOM_NUM 100

/* 디버그 레벨 */
#define DEBUG_LEVEL 2

/* 상태값 전역변수 */
#define SUCCESS 1
#define FAIL -1

/* DB SETTING */
#define DB_NAME "yj_chatting_DB.txt"


typedef struct ID
{
    char id_value;  /* 고유 ID 값 */
    struct ID *next;      /* 링크드 리스트 다음 구조체 포인터 */
    struct ID *before;    /* 링크드 리스트 이전 구조체 포인터 */
} _ID;

_ID Chatting_room[CHATTING_ROOM_NUM];

/* 고유 ID 값을 만드는 함수 */
int createID(char *buf, int num)
{
    int rndVal = 0;
    int i=0;
    srand((unsigned)time(NULL));
    for(i=0; i<num; i++)
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
    if(i < num)
    {
        return FAIL;
    }
#if DEBUG_LEVEL >= 2
    printf("[SERVER] - [createID] : randomID - %s\n", buf);
#endif
    return SUCCESS;
}

/* ID 값 저장 함수 */
void storeID(_ID *id, char *buf, int count)
{
    memcpy(&(id->id_value), buf, sizeof(buf)); /* 링크드 리스트로 관리 */

    /* 파일로 관리 */
    /*
        파일로 관리할 경우 다음과 같은 형식으로 저장한다. 
        <chatting number> <ID> <pid>
    */
    FILE *fp;
    int i;
    fp = fopen( DB_NAME, "a+" );
    if( fp == NULL )
    {
        printf("[SERVER] : file fopen() error\n");
    }
    else
    {   
        fprintf(fp, "%d %s %d\n", count, buf, getpid());
    }
    fclose(fp);
    clearIDfile()
}

/* ID 값 삭제 함수 */
void clearID(_ID *id[CHATTING_ROOM_NUM])
{
    FILE *fp;
    int i;

    fp = fopen(DB_NAME, "w+");
    if( fp == NULL)
    {
        printf("[SERVER] : file fopen() error\n");
    }
    else{
        system("rm yj_chatting_DB.txt");
    }

    for(i=0; i<CHATTING_ROOM_NUM; i++)
    {
        id[i]->id_value = 0;
        id[i]->next = NULL;
        id[i]->before = NULL;
    }
    
}

/* 디버깅 프린트 함수 */
void debug_print(int status, char *msg_fail, char *msg_success)
{
#if DEBUG_LEVEL >= 2
        if(status == FAIL)
        {
            printf("[SERVER] - [FAIL] %s\n", msg_fail);
        }
        else if(status == SUCCESS)
        {
            printf("[SERVER] - [SUCCESS] %s\n", msg_success);
        }
        else{
            printf("[SERVER] - status value error...\n");
        }      
#endif
}

int main()
{
    int c_socket, s_socket; /* socket descryption value */
    struct sockaddr_in s_addr, c_addr;
    int len;
    int n;
    int chatting_room_count = 0;

    int status = 0; /* status value (success or fail) */

    char randomID[ID_LEN]; /* random ID */

    pid_t pid;

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

    /* 메인 로직 */
    while(1)
    {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *)&c_addr, (socklen_t *)&len);

        if((pid = fork()) < 0)
        {
            printf("[SERVER] - [ERROR] : echo server cannot fork()\n");
            return FAIL;
        }
        else if(pid > 0)
        {
            status = createID(randomID, ID_LEN);
            debug_print(status, "fail create random ID", "success create random ID");
            if( status == SUCCESS)
            {
                write(c_socket, randomID, strlen(randomID));
                debug_print(status, "fail send random ID", "success send random ID");
                chatting_room_count+= 1;
                storeID(Chatting_room, randomID, chatting_room_count);
            }
            close(c_socket);
        }
        else if(pid == 0)
        {
            /* child process routine add. */
        }
    }
}
