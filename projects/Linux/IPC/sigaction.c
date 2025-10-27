#include<signal.h>
#include<stdio.h>
#include<unistd.h>
//	int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
// (*sa_sigaction)(int ,siginfo_t *, void *);


void handler(int signum, siginfo_t *info, void *context){//参数1：信号值，参数2：内容，参数3：是否为NULL（决定有没有内容）
	printf("get signum %d\n",signum);
	if(context != NULL){
		//非空代表有内容
		printf("get data = %d\n",info->si_int);
		printf("get data = %d\n",info->si_value.sival_int);
		printf("get data = %d\n",info->si_pid);
	}
}

int main(){
	struct sigaction act;
	act.sa_sigaction = handler;
        act.sa_flags = SA_SIGINFO;

	printf("%d\n",getpid());
	sigaction(SIGUSR1,&act,NULL);
	while(1);
	return 0;
}
