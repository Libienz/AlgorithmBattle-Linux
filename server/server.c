#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>


#define PORTNUM 9000
#define BUFSIZE 1024
#define MAX_CLNT 2 // 최대 동시 접속 가능 수
#define SERV_IP "192.168.219.107"
void *handle_clnt(void *arg); 
void myfileprint(char* path);
void msgsend(int ns, char* buf);



/********* Critical Section***************/

int clnt_cnt = 0; //연결된 클라이언트 수
int clnt_socks[MAX_CLNT]; // 클라이언트 배열
pthread_mutex_t mutx; //mutex 선언

/********* Critical Section***************/
int main() {
    char buf[BUFSIZE];
    struct sockaddr_in sin, cli;
    pthread_t t_id;
    int sd, ns, clientlen = sizeof(cli);
    pid_t pid;

     
    //Socket 생성
    //AF_INET: IPv4 인터넷 프로토콜 사용
    //SOCK_STREAM: TCP 사용
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("socket");
	exit(1);
    }

    //서버 IP 주소 지정과 포트 번호 설정
    //서버로 돌릴 컴퓨터 지정하고 IP 수정해야 함
    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM); //HBO -> NBO
    sin.sin_addr.s_addr = inet_addr(SERV_IP); //str IP addr -> bin

    //bind: 소켓 파일 기술자를 지정된 IPaddr/Port와 결합
    if (bind(sd, (struct sockaddr *) &sin, sizeof(sin))) {
	perror("bind");
	exit(1);
    }
    //listen: 5로 지정했다고 5명만 할 수 있는 것은 아님
    //큐의 크기일 뿐 운영체제의 여유가 되면 알아서 accept
    if (listen(sd, 5)) {
	perror("listen");
	exit(1);
    }
	
    while (clnt_cnt < 2) { //두명 까지만 받을 거임 
    
	if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
	    perror("accept");
	    exit(1);
	}
	
	pthread_mutex_lock(&mutx); //mutex lock
	clnt_socks[clnt_cnt++] = ns;
	pthread_mutex_unlock(&mutx); //mutex unlock
	pthread_create(&t_id, NULL, handle_clnt,(void *)&ns);
	//pthread_detach(t_id);
    }
    while(1);

    
    //ready 들어올 때 까지 wait 
	//ready 눌렀는데 상대방이 ready가 되지 않았다면 상대방의 ready 기다리는 중 이런식으로 출력
    //둘다 ready 되었다면 game_start라는 제어로 넘어감 
    //서버는 while로 signal 받을 때 까지 대기중 
    //누군가 judge를 이용해서 정답을 맞추었다면 깨어남 
    //게임 결과 및 게임 종료한다고 출력


    
    close(ns);
    close(sd);

}
void *handle_clnt(void *arg){

    char buf[BUFSIZE];
    int ns = *((int *)arg);
    msgsend(ns, "서버와 연결되었습니다");
    sleep(1);
    msgsend(ns, "상대방의 입장을 기다리는 중입니다...");
    sleep(1);
    while (clnt_cnt < 2); //busy wating
    msgsend(ns,"상대방이 입장했습니다");

    
}
    
    //if room filled start game 
    
    //generate rand num and send prb

    

    //if create then create
void msgsend(int ns, char* buf) {

    if (send(ns, buf, strlen(buf) + 1, 0) == -1) {
	perror("send");
	exit(1);
    }
}



void myfileprint(char* path) {
    
    FILE* fp;
    char buf[BUFSIZE];
    int n;

    fp = fopen(path, "r");

    while(fgets(buf, BUFSIZE,fp) != NULL) {
	printf("%s",buf);
    }
    printf("\n");

    fclose(fp);
}
