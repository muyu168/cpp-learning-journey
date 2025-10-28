#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

void *fun(void *arg){
	for(int i = 0; i < 5 ;i++){
		printf("fun thread working %d\n",i);
		sleep(1);		
	}
	pthread_exit(NULL);
}
int main(){
	int ret = -1;
	pthread_t tid;
	ret = pthread_create(&tid,NULL,fun,NULL);
	if(ret != 0){\
		printf("pthread_create failed\n");
		return 1;
	}
	ret = pthread_detach(tid);
	if(ret != 0){
		printf("pthread_detach failed\n");
		return 1;
	}
	sleep(3);
	printf("kill thread\n");
	pthread_cancel(tid);
	return 0;
}
