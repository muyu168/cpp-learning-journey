#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

void *fun(void *arg){
	printf("fun tid :%lu\n",pthread_self());
	for(int i = 0; i < 5 ;i++){
		printf("fun thread working %d\n", i);
		usleep(1000000);
	}
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
	/*设置线程分离*/
	ret = pthread_detach(tid);
	if(ret != 0){
		printf("pthread_detach failed\n");
		return 1;
	}
	/*测试分离状态是否可以被jion*/
	ret = pthread_join(tid,NULL);
	if(ret != 0){
		printf("pthread_jion failed\n");
	}else{
		printf("pthread_jion success\n");
	}
	printf("main thread.... tid: %lu\n",pthread_self());
	printf("press any key to exit\n");
	getchar();
	return 0;
}
