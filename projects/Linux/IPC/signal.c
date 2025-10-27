#include<signal.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

//	typedef void (*sighander_t)(int);
//	sighandler_t signal(int signum, sighandler_t handler);

void handler(int signum){
	printf("get signum = %d \n",signum);
	switch(signum){
		case 2:
			printf("SIGINT\n");
			break;
		case 9:
			printf("SIGKILL\n");
			break;
		case 10:
			printf("SIGUSR1\n");
			break;
	}
}

int main(){
	signal(SIGINT,handler);
	signal(SIGKILL,handler);
	signal(SIGUSR1,handler);
	int pid = getpid();
	printf("pid = %d\n",pid);
	while(1);
	return 0;
}
