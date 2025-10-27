#include<unistd.h>
#include<stdio.h>

int main(){
	pid_t pid;
	pid_t pid2;
	pid = getpid();
	printf("before fork pid : %d \n",pid);
	fork();
	pid2 = getpid();
	printf("after fork pid : %d\n",pid2);
	if(pid == pid2){
		printf("this is father fork \n");
		wait(1);
	}else{
		printf("this is child fork \n");
	}
	return 0;
	
}
