#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

/*函数原型 pthread_t pthread_self(void);*/
/*指定连接线程库 gcc -o pthread_self pthread_self.c -pthread*/

int main(){
	pthread_t tid = 0;
	/*获取当前线程的线程号*/
	tid = pthread_self();
	printf("tid : %lu\n",tid);
	/*比较两个线程ID是否相同*/
	if(pthread_equal(tid,pthread_self())){
		printf("thread id is same\n");
	}else{
		printf("thread id is not the same\n");
	}
	return 0;
}
