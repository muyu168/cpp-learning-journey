#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t mutex;

void *fun1(void *arg){
	pthread_mutex_lock(&mutex);
	for(int i = 'A' ; i <= 'Z' ; i++){
		putchar(i);
		fflush(stdout);
		usleep(100000);
	}
	pthread_mutex_unlock(&mutex);

	return NULL;
}

void *fun2(void *arg){
	pthread_mutex_lock(&mutex);
	for(int i = 'a' ; i <= 'z' ; i++){
		putchar(i);
		fflush(stdout);
		usleep(100000);
	}
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(){
	int ret = -1;
	pthread_t tid1,tid2;
	ret = pthread_mutex_init(&mutex,NULL);
	if(ret != 0){
		printf("muntex_init failed\n");
		return 1;
	}
	ret = pthread_create(&tid1,NULL,fun1,NULL);
	if(ret != 0){
		printf("thread1 create failed\n");
		return 1;
	}
	ret = pthread_create(&tid2,NULL,fun2,NULL);
	if(ret != 0){
		printf("thread2 create failed\n");
		return 1;
	}
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	pthread_mutex_destroy(&mutex);
	return 0;
}
