#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

int flag = 1;
//互斥量
pthread_mutex_t mutex;

//信号量

pthread_cond_t cond;

//改变条件的线程
void *fun1(void	*arg){
		while(1){
			/*加锁*/
			pthread_mutex_lock(&mutex);
			/*改变*/
			flag = 1;
			/*解锁*/
			pthread_mutex_unlock(&mutex);
			/*唤醒*/
			pthread_cond_signal(&cond);
			sleep(2);
		}
	return NULL;
}

void *fun2(void	*arg){
	while(1){
		/*加锁*/
		pthread_mutex_lock(&mutex);
		if(flag == 0){
			/*阻塞等待（每次阻塞会自动释放一次所占用的资源）*/
			pthread_cond_wait(&cond,&mutex);
		}
		printf("thread2 running\n");
		/*改变变量*/
		flag = 0;
		/*解锁*/
		pthread_mutex_unlock(&mutex);
		sleep(2);

	}	
	return NULL;
}

//条件变量的应用
int main(){
	int ret = -1;
	pthread_t tid1,tid2;
	//初始化条件变量
	ret = pthread_cond_init(&cond,NULL);
	if(ret != 0){
		printf("pthread_cond_init failed\n");
		return 1;
	}

	//初始化互斥量
	ret = pthread_mutex_init(&mutex,NULL);
	if(ret != 0){
		printf("pthread_mutex_init failed\n");
		return 1;
	}
	

	//创建两个线程
	pthread_create(&tid1,NULL,fun1,NULL);
	pthread_create(&tid2,NULL,fun2,NULL);
	
	//销毁互斥量
	pthread_mutex_destroy(&mutex);	
	//销毁条件变量
	pthread_cond_destroy(&cond);	
	//回收线程资源
	ret = pthread_join(tid1,NULL);
	if(ret != 0){
		printf("thread1 join failed\n");
	}
	ret = pthread_join(tid2,NULL);
	if(ret != 0){
		printf("thread2 join failed\n");
	}
	return 0;
}
