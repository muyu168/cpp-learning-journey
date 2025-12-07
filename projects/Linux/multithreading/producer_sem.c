#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

sem_t sem_producer;
sem_t sem_customer;

void *producer(void *arg){
	int val;
	while(1){
		sem_wait(&sem_producer);
		printf("producer running\n");
		sem_post(&sem_customer);
		sem_getvalue(&sem_producer,&val);
		printf("producer ok,shekves num: %d\n",val);
		sleep(random()%3 + 1);
	}
	return NULL;
}

void *customer(void *arg){
	int val;
	while(1){
		sem_wait(&sem_customer);
		printf("customer running\n");
		sem_post(&sem_producer);
		sem_getvalue(&sem_customer,&val);
		printf("customer ok,goods num: %d\n",val);
		sleep(random()%3 + 1);
	}
	return NULL;
}

int main(){

	int ret = -1;
	pthread_t tid1,tid2;
	
	srandom(getpid());
	
	ret = sem_init(&sem_producer,0,4);
	if(ret != 0){
		printf("sem_init producer failed\n");
		return 1;
	}
	ret = sem_init(&sem_customer,0,0);
	if(ret != 0){
		printf("sem_init customer failed\n");
		return 1;
	}

	pthread_create(&tid1,NULL,producer,NULL);
	pthread_create(&tid2,NULL,customer,NULL);

	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	sem_destroy(&sem_producer);
	sem_destroy(&sem_customer);

	return 0;
}
