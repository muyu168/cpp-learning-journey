#include<signal.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>

//	int kill(pid_t pid, int sig);

int main(int argc, char **argv){
	int signum = atoi(argv[1]);
	int pid = atoi(argv[2]);
	char cmd = {0};

	printf("signum = %d , pid = %d",signum,pid);
	kill(pid,signum);
	sprintf(cmd,"kill %d %d",signum,pid);
	printf("send signal ok\n");
	return 0;

}
