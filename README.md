# TCP_IP_Programing

TCP/IP 소켓 프로그래밍 공부 레파지토리입니다. 

# 목차

1. basic: TCP/IP 소켓 프로그래밍의 기초 틀을 구현한 코드입니다. 
2. hello_world: 서버가 클라이언트에게 "hello world" 문자열을 전달하여 이를 출력하는 코드입니다. 
3. ls: 클라이언트가 서버에게 "ls" 문자열을 전달하면, 서버는 클라이언트에게 서버 기준 현재 경로의 파일 리스트를 전달하는 코드입니다. 
4. socketpair: `socketpair()` 함수의 기초 사용법을 구현한 코드입니다. 
5. getservent: `getservent()` 함수의 기초 사용법을 구현한 코드입니다. 
6. network_ordering: 네트워크 바이트 순서와 빅엔디안, 리틀 엔디안을 이해할 수 있도록 구현한 코드입니다. 
7. transport_address: 주소 형식 변환을 제공하는 함수를 사용하는 코드입니다. 
8. echo_service_tcp: 소켓 프로그래밍을 사용하여 echo system을 개발한 코드입니다. (TCP)
9. echo_service_udp: UDP 를 이용한 소켓 프로그래밍을 사용하여 echo system을 개발한 코드입니다. 

# 소켓 프로그래밍 기본 API 실행 흐름 

![flow](https://t1.daumcdn.net/cfile/tistory/995C23465C7DD7E30B)

# 함수 또는 타입 정리.

## 타입 또는 구조체 

* 주소 체계 종류: AF_INET, AF_INET6, AF_UNIX, AF_LOCAL, AF_LINK(Low level socket 인터페이스를 이용), AF_PACKET(IPX 노벨 프로토콜을 사용한다.)
* sockaddr: 소켓의 주소를 담는 기본 구조체
```c
struct sockaddr {
    u_short sa_family;      /* 주소체계를 구분하기 위한 변수, 2 bytes, u_short는 unsigned short를 의미 */
    char sa_data[14];       /* 실제 주소를 저장하기 위한 변수. 14 bytes*/
}
```
* sockaddr_in: sockaddr 구조체에서 sa_family가 AF_INET인 경우 (즉. 주소 체계를 IPv4를 사용하는 경우)
```c
struct sockaddr_in{
    short sin_family;           /* 주소 체계: AF_INET */
    u_short sin_port;           /* 포트 번호를 의미, 범위는 0 ~ 65535, 2 bytes */
    struct in_addr sin_addr;    /* 호스트 IP 주소 */
    char sin_zero[8];           /* 8 bytes dummy data. 반드시 모두 0으로 채워져 있어야 한다. 이유는 sockaddr 구조체와 크기를 일치시키기 위함이다. */
}

struct in_addr{
    u_long s_addr;              /* 32비트 IP 주소를 저장할 변수 */
}
```
* sockaddr_in6: AF_INET6인 경우 (IPv6 주소체계를 사용하는 경우)
```c
struct sockaddr_in6{
    sa_family_t     sin6_family;        /* AF_INET6 */
    in_port_t       sin6_port;          /* IPv6 포트를 저장, ntohs() 또는 htons()로 조작하는 것이 좋음 */
    uint32_t        sin6_flowinfo;      /* IPv6 헤더와 연관된 트래픽 클래스와 플로루 레이블을 포함 */
    struct          in6_addr sin6_addr; /* 16 bytes IPv6 주소를 저장하는 변수 */
    uint32_t        sin6_scope_id;      /* sin6_addr의 주소 범위에 따라 달라지는 식별자를 포함할 수 있다. */
}

struct in6_addr{
    unsigned char s6_addr[16];          /* IPv6 address store */
}
```
* sockaddr_un: 하나의 시스템에서 서로 다른 프로세스 사이의 통신에 사용되는 소켓의 주소 지정하는데 사용되는 구조체 (AF_UNIX, AF_LOCAL 인 경우)
```c
struct sockaddr_un{
    sa_family_t     sun_family;             /* AF_UNIX */
    char            sun_path[UNIX_PATH_MAX] /* 파일 시스템 경로 지정. NULL로 끝나는 문자열이여야 한다. 경로의 최대 길이는 NULL terminator를 포함해서 108 bytes 이다. */
}
```
* servent: `/etc/services`의 정보를 읽어 사용되는 구조체로, 현재 열려있는 포트 및 서비스 명을 나타낸다. 
```c
struct servent{
    char     *s_name;     /* 서비스 이름, 포트명 */
    char     **s_aliases; /* 서비스의 별명 리스트 */
    int      s_port;      /* 서비스가 이용하는 포트 */
    char     *s_proto;    /* 서비스가 사용하는 프로토콜 */
}
```
* hostent: 도메인 정보를 나타내는 구조체 이다. 
```c
struct hostent{
    char    *h_name;        /* 공식 도메인 이름 */
    char    **h_aliases;    /* 별명들 */
    int     h_addrtype;     /* 주소 유형 */
    int     h_length;       /* 주소 길이 */
    char    **h_addr_list;  /* 네트워크 바이트 순서의 이진값의 IP 주소 */
}
```

## 함수 

### socket

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int socket(int domain, int type, int protocol);`
* 설명: 소켓을 생성하여 반환한다. 
* 입력 변수:
  * `int domain`:  프로토콜 family를 지정해주는 변수
    * `AF_UNIX`(프로토콜 내부), `AF_INET`(IPv4), `AF_INET6`(IPv6)
  * `int type`: 어떤 타입의 프로토콜을 사용할 것인지 설정
    * `SOCK_STREAM`(TCP), `SOCK_DGRAM`(UDP), `SOCK_RAW`(사용자 정의)
  * `int protocal`: 어떤 프로토콜의 값을 결정하는 것
    * `0`, `IPPROTO_TCP`(TCP), `IPPROTO_UDP`(UDP)
* 반환값:
  * 에러: `-1`
  * 정상: 소켓에 대한 파일디스크립터
    * 파일디스크립터 참고 자료: https://twofootdog.tistory.com/51

### bind

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);`
* 설명: 소켓을 바인딩 하는 함수 (쉽게 말하자면 소켓과 해당 프로세스를 묶음으로서 해당 프로세스가 소켓을 통해 다른 컴퓨터로부터 연결을 받아들일 수 있게 한다.)
* 입력 변수:
  * `int sockfd`: 소켓 식별자 또는 소켓 디스크립터
  * `struct sockaddr *myaddr`: `sockaddr` 포인터
  * `socklen_t addrlen`: `myaddr` 구조체 크기
* 반환값:
  * 에러: `-1`
  * 성공: `0`

### listen
 * 헤더 파일: `<sys/socket.h>`
 * 함수 원형: `int listen(int sock, int backlog);`
 * 설명: 클라이언트가 연결요청 할 수 있는 상태로 만들어주는 함수.
 * 입력 변수:
   * `int sock`: 연결요청 대기상태에 두고자 하는 소켓의 파일 디스크립터 전달. 이 함수의 인자로 전달된 디스크립터의 소켓이 서버 소켓이 된다.
   * `int backlog`: 연결요청 대시 큐(queue)의 크기정보 전달. 
     * Ex. 5가 전달되면 큐의 크기가 5가 되어서 클라이언트의 연결 요청을 5개까지 대기시킬 수 있다. 
 * 반환값
   * 에러: `-1`
   * 성공: `0`

### accept
* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int accept(int sockfd, struct sockaddr* addr, socklent_t *addrlen);`
* 설명: 해당 소켓에 연결 요청이 왔을 때 연결을 받아들이는 함수이다. 
* 입력 변수:
  * `int sockfd`: 연결을 기다리는 소켓 디스크립터.
  * `struct sockaddr* addr`: 받아들인 클라이언트 주소 및 포트 정보가 저장될 구조체 주소값.
  * `socklen_t *addrlen`: `sockaddr` 구조체의 길이가 저장된 변수의 주소값
* 반환값
  * 에러: `-1`
  * 성공: 새로운 소켓 디스크립터

### connect

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int connect(int socket, const struct sockaddr *address, socklen_t address_len);`
* 설명: 클라이언트 소켓을 생성하고, 서버로 연결을 요청
* 입력 변수:
  * `int socket`: 클라이언트 소켓의 파일 디스크립터.
  * `const struct sockaddr *address`: 연결 요청을 보낼 서버의 주소 정보를 지닌 구조체 변수의 포인터.
  * `socklen_t address_len`: 포인터가 가리키는 주소 정보 구조체 변수의 크기. 
* 반환값
  * 에러: `-1`
  * 성공: `0`

### inet_addr
* 헤더 파일: `<arpa/inet.h>`
* 함수 원형: `in_addr_t inet_addr(const char *cp);`
* 설명: 인터넷 주소 cp를 32bit 바이너리 주소로 변경한 값을 리턴한다. 
* 입력 변수:
  * `const char *cp`: 인터넷 주소 char 값
* 반환값:
  * 에러: `-1`
  * 성공: `바이너리 주소 값 (이떄 리턴 값은 네트워크 바이트 오더를 따른다.)`

### htons & htonl
* 헤더 파일: `<netinet/in.h>`
* 함수 원형: 
  * `unsigned short int htons(unsigned short int hostshort);`
  * `unsigned long int htonl(unsigned long int hostshort);`
* 설명
  * `htonl()`함수는 long intger(일반적으로 4byte)데이터를 네트워크 byte order로 변경한다.
  * `htons()`함수는 short intger(일반적으로 2byte)데이터를 네트워크 byte order로 변경한다.

### write
* 헤더 파일: `<unistd.h>`
* 함수 원형: `ssize_t write(int fildes, const void *buf, size_t nbyte);`
* 설명: 파일에 데이터를 출력하는 함수.
* 입력 변수:
  * `int fileds`: 데이터 전송 영역을 나타내는 파일 디스크립터
  * `const void *buf`: 전송할 데이터를 가지고 있는 버퍼의 포인터
  * `size_t nbyte`: 전송할 데이터의 바이트 수 
* 반환값
  * 에러: `-1`
  * 성공: 전달한 바이트 수

### close
* 헤더 파일: `<unistd.h>`
* 함수 원형: `int close(int fildes);`
* 설명: 열려 있는 파일을 닫는다.
* 입력 변수:
  * `int fildes`: 파일 디스크립터
* 반환값
  * 에러: `-1`
  * 성공: `0`

### getservent
* 헤더 파일: `<netdb.h>`
* 함수 원령: `struct servent *getservent();`
* 설명: `/etc/services`에 있는 서비스 목록들을 가져와 주는 함수
* 입력 변수: 없음
* 반환값
  * 에러: `NULL`
  * 성공: `struct servent 포인터`

### sendto

### recvfrom 



# IP 주소를 표현하는 법

# 기타

* 네트워크 바이트 순서: Big Endian 
* AF_INET vs PF_INET
  * AF_INET: 주소 체계
  * PF_INET: 프로토콜 체계 
* `close()` vs `shutdown()`