#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
void *fun(void *arg){
	printf("fun tid :%lu\n",pthread_self());
	return NULL;
}

void *fun1(void *arg){
	int var = (int)(long)arg;
	printf("thread2 var = %d\n",var);
	return NULL;
}
int main(){
	int ret = -1;
	pthread_t tid;
	/*创建一个线程*/
	ret = pthread_create(&tid,NULL,fun,NULL);
	if(ret != 0){
		printf("pthread creat failed\n");
		return 1;
	}
	/*创建一个线程*/
	ret = pthread_create(&tid,NULL,fun1,(void*)0x3);
	if(ret != 0){
		printf("pthread creat failed\n");
		return 1;
	}
	printf("main thread.... tid: %lu",pthread_self());
	getchar();
	return 0;
}
