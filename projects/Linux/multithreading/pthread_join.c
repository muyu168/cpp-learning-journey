#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

void *fun(void* arg){
	for(int i = 0; i < 5 ;i++){
		printf("fun pthread working %d\n",i);
		sleep(1);
	}
	return (void*)0x3;
}
int main(){
	int ret = -1;
	pthread_t tid = -1;
	void *retp = NULL;
	/*创建线程*/
	ret = pthread_create(&tid,NULL,fun,NULL);
	if(ret != 0){
		printf("pthread_create failed\n");
		return 1;
	}
	printf("main thread running...\n");
	/*等待线程*/
	ret = pthread_join(tid,&retp);
	if(ret != 0){
		printf("pthread_join failed\n");
		return 1;
	}
	printf("retp : %p\n",retp);
	printf("main thread exit...\n");
	return 0;
}
