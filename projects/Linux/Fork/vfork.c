#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
	pid_t pid;
	int cnt = 0;
	pid = vfork();
	if(pid > 0){
		while(1){
			printf("this is father fork, pid : %d \n",getpid());
			printf("cnt = %d \n",cnt);
			sleep(1);
			if(cnt < 6){
				cnt++;
			}else{
				break;
			}
		}
	}
	else if(pid == 0){
		while(1){
			printf("this is child fork, pid : %d \n",getpid());
			printf("cnt = %d \n",cnt);
			cnt++;
			if(cnt > 3){
				exit(0);
			}
			sleep(1);
		}
	}
	return 0;
}
