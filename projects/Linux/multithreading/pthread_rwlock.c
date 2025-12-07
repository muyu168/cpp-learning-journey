#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

int num = 0;
pthread_rwlock_t rwlock;

void *fun_read(void* arg){
	int index = (int)(long)arg;
	while(1){
		pthread_rwlock_rdlock(&rwlock);
		printf("thread%d ,read num : %d\n",index,num);
		pthread_rwlock_unlock(&rwlock);
		sleep(random()%3 + 1);
		if(num > 10){
			pthread_exit(NULL);
		}
	}
	return NULL;
}

void *fun_write(void* arg){
	int index = (int)(long)arg;
	while(1){
		pthread_rwlock_wrlock(&rwlock);
		num++;
		printf("thread%d , write num : %d\n",index,num);
		pthread_rwlock_unlock(&rwlock);
		sleep(random()%3 + 1);
		if(num > 10){
			pthread_exit(NULL);
		}
	}

	return NULL;
}

int main(){
	int ret = -1;
	pthread_t tid[8];
	srandom(getpid());
	ret = pthread_rwlock_init(&rwlock,NULL);
	if(ret != 0){
		printf("pthread_rwlock_init failed\n");
		return 1;
	}
	for(int i = 0; i < 8 ; i++){
		if(i < 5){
			pthread_create(&tid[i],NULL,fun_read,(void*)(long)i);
		}else{
			pthread_create(&tid[i],NULL,fun_write,(void*)(long)i);
		}
	}

	for(int i = 0 ; i < 8 ; i++){
		pthread_join(tid[i],NULL);
	}
	pthread_rwlock_destroy(&rwlock);
	return 0;
}
