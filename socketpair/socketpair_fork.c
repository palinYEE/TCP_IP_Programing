#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>

char data1[] = "From Parent";
char data2[] = "From Child";

int main(){
    int pid;
    int sd[2], result;
    char buf[BUFSIZ];

    result = socketpair(PF_LOCAL, SOCK_STREAM, 0, sd);

    pid = fork();
    /* 부모 클래스 */
    if(pid > 0){
        close(sd[1]);
        read(sd[0], buf, BUFSIZ);
        printf("[parent %d]: %s\n", getpid(), buf);

        write(sd[0], data1, sizeof(data1));
        close(sd[0]);
    }
    else if(pid == 0){
        close(sd[0]);

        write(sd[1], data2, sizeof(data2));

        read(sd[1], buf, BUFSIZ);

        printf("[Child: %d]: %s\n", getpid(), buf);
        close(sd[1]);
    }
    else{
        perror("fork()");
   }
    return 0;
}