#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 257


int judge(char* execfn, char* model_input, char* model_output) {

    
    //양방향 파이프 사용
    int fd1[2], fd2[2];
    pid_t pid;
    char buf[BUFSIZE];
    int len, status;

    if (pipe(fd1) == -1) {
	perror("pipe");
	exit(1);
    }

    if (pipe(fd2) == -1) {
	perror("pipe");
	exit(1);
    }

    switch (pid= fork()) {
	case -1 :
	    perror("fork");
	    exit(1);
	    break;
	case 0:

	    close(fd1[1]);
	    close(fd2[0]);

	    //자식 프로세스는 표준 입력을 fd[0]이 가리키는 파이프에서 읽는다
	    if(fd1[0] != 0) {
		dup2(fd1[0], 0);
		close(fd1[0]);
	    }
	
	    //자식 프로세스는 자신의 표준 출력을 fd[2]가 가리키는 파이프로 보낸다
	    if(fd2[1] != 1) {
		dup2(fd2[1], 1);
		close(fd2[1]);
	    }

	    //유저 제출물 실행
	    char *argv[1];
	    argv[0] = NULL;
	    if (execv(execfn,argv) == -1) {
		perror("execlp");
		exit(1);
	    }

	    exit(1);
	    break;
	default:

	    sleep(2); //sync...

	    close(fd1[0]);
	    close(fd2[1]);

	    model_input = strcat(model_input, "\n");
	    char* model_out;
	    model_output = strcat(model_output, "\0");
	    strcpy(model_output, model_out);
	    printf("model_out: %s\n",model_out);
	    write(fd1[1], model_input,(int) strlen(model_input)+1);

	    waitpid(pid, &status, 0);
	    len = read(fd2[0],buf,256);
	    buf[len] = '\0';
	    printf("len: %d\n", len);
	    printf("model_out: %s\n",model_out);

	    
	    if(strcmp(buf, model_out) != 0) {
		printf("buf: %s", buf);
		printf("mod_out: %s\n", model_out);
		return -1;
	    }
	    else {
		return 1;
	    }
    }
}

//argv[1]: execfn
//argv[2]: solfn
int main(int argc, char* argv[]) {

    FILE* fp;
    char buf[BUFSIZE];
    int c;
    char* model_input;
    char* model_output;

    if ((fp = fopen(argv[2],"r")) == NULL) {
	perror("fopen");
	exit(1);
    }

    //우선 한줄만 .. 근데 이거 여러줄로도 할 수 있음
    while (fgets(buf,BUFSIZE,fp)) {
	model_input = strtok(buf,":");
	model_output = strtok(NULL,":");
	//model out 잘 설정된 것 확인 
	if(judge(argv[1],model_input, model_output) == -1) { 
	    printf("오답\n");
	    return -1;
	}
    }

    printf("정답\n");
    return 0;

}
