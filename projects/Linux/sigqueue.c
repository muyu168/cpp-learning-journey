#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

//int sogqueque(pid_t pid, int sig, const union sigval value);

int main(int argc, char **argv){
	int signum;
	int pid;
	printf("%d\n",getpid());
	signum = atoi(argv[1]);
	pid = atoi(argv[2]);
	union sigval value;
	value.sival_int = 100;
	sigqueue(pid,signum,value);
	printf("done\n");
	return 0;
}
