#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096

/*
   judgeproc
   명령행으로 받은 프로그램이 특정 알고리즘 문제에 대해 
   정답인지 확인하는 채점 프로그램
   입출력 모범사례가 5가지가 들어있는 답지를 참고하며
   유저 프로그램에게 입력을 넣어 모범출력을 내는지 확인하는 방식으로 채점한다.
   유저 프로그램은 후에 fork를 이용해 실행되는데 
   양방향 파이프를 설정하여 유저 프로그램으로 입력을 넘기고 출력을 체크한다.
   argv[1]: execfn (실행파일명)
   argv[2]: solfn  (답지)
   사용하기 위한 명령은 다음과 같다
   ./judgeproc [progname] [sol_file_name]
*/

int main(int argc, char* argv[]) {

    FILE* fp;
    pid_t pid;
    int status, len;
    int size;
    char buf[BUFSIZE];
    char* sol;
    char model_input[BUFSIZE];
    char model_output[BUFSIZE];
    int fd1[2], fd2[2];

    //답지 파일 오픈 
    //답지는 5가지 입력 케이스와 각 입력에 대한 모범출력 답안을 가지고 있다.
    if ((fp = fopen(argv[2],"r")) == NULL) {
	perror("fopen");
	exit(1);
    }

    

    //답지 읽어오기
    fread(buf, sizeof(char)*2, BUFSIZE, fp);
    strtok(buf,":");
    printf("\n============제출물 도착 채점 시작==================\n");
    //5번 동안 반복 채점 사례가 5가지 이기 때문
    for (int i = 0; i < 5; i++) {
	
	//유저 프로그램이 실행되었을 때 
	//입력을 넘겨주고 출력을 받아올 파이프
	//양방향으로 설정
	if (pipe(fd1) == -1) {
	    perror("pipe");
	    exit(1);
	}

	if (pipe(fd2) == -1) {
	    perror("pipe");
	    exit(1);
	}

	//답지는 입력과 입력에 대한 정답을 ':'으로 구분 strtok을 이용해서 
	//한가지 사례 입력과 입력에 대한 모범답안을 받는다.
	strcpy(model_input, strtok(NULL,":"));
	strcpy(model_output, strtok(NULL,":"));	

	//파이프에 입력을 미리 넣어 놓는다.
	//후에 자식 프로세스가 입출력 redirection 설정을 완료하고
        //채점할 프로그램을 실행하면 자연스럽게 입력으로 넘어갈 것 
	write(fd1[1], model_input,(int) strlen(model_input)+1);

	switch (pid= fork()) {
	    //fork error
	    case -1 :
		perror("fork");
		exit(1);
		break;
	    //child proc
	    case 0:
		//파이프 방향 설정
		close(fd1[1]);
		close(fd2[0]);

		//자식 프로세스는 표준 입력을 fd1[0]이 가리키는 파이프에서 읽는다
		if(fd1[0] != 0) {
		    dup2(fd1[0], 0);
		    close(fd1[0]);
		}
		//자식 프로세스는 자신의 표준 출력을 fd2[1]가 가리키는 파이프로 보낸다
		if(fd2[1] != 1) {
		    dup2(fd2[1], 1);
		    close(fd2[1]);
		}

		//유저 제출물 실행
		//자식 프로세스가 무언가 출력했다면 해당 출력은 
		//부모 프로세스로 향하는 파이프에 있을 것임
		char *args[1];
		args[0] = NULL;
		if (execv(argv[1], args) == -1) {
		    perror("execv");
		    exit(1);
		}
 
		exit(1);

		//각각의 입출력 사례에 대해 파이프를 매번 새로 연결해야 함으로 (새로 실행한 프로세스에게 연결해야 하기 때문) 파이프 닫기
		close(fd1[0]);
		close(fd2[1]);
		break;

	    //parent proc
	    default:
		//자식이 종료할 때 까지 wait
		waitpid(pid, &status, 0);
		close(fd1[0]);
		close(fd2[1]);
		
		//입력에 대해서 프로그램이 어떤 출력을 내었는지 파이프에서 꺼내본다.
		len = read(fd2[0],buf,256);
		if (len == -1) {
		    perror("read");
		    exit(1);
		}

		len = strlen(model_output);
		buf[len-1]= '\0';
		model_output[len-1] = '\0';
		printf("case %d\n", i+1);
		printf("예시 입력\n");
		printf("%s\n",model_input);
		printf("예시 입력에 대한 모범 출력:  %s\n",model_output);
		printf("유저 프로세스의 출력값: %s\n\n",buf);

		//특정 입력을 주었을 때 자식의 답이 모범출력과 다르다면 오답
		if (strcmp(buf,model_output) != 0) {
		    printf("user process 오답\n");
		    printf("\n============채점완료==================\n");
		    return -1;
		}
		//파이프 닫아주기
		close(fd1[1]);
		close(fd2[0]);
		break;
	}
    }

    //모든 case에 대해 모범출력을 보였다면 정답
    printf("user process 정답\n");
    printf("\n============채점완료==================\n");
    return 1;

}
