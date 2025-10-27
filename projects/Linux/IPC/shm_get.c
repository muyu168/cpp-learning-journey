#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

//int shmget(key_t key, size_t size, int shmflg);
//void *shmat(int shmid, const void *shmaddr);

int main(){
	int shmid;
	char *shmaddr;
	key_t key = ftok(".",1);
	shmid = shmget(key,1024*4,0);
	if(shmid == -1){
		printf("creat shm fail\n");
		exit(-1);
	}
	shmaddr = shmat(shmid,0,0);
	printf("get shm_write message is : %s\n",shmaddr);
	shmdt(shmaddr);
	shmctl(shmid,IPC_RMID,0);
	printf("quit\n");
	return 0;
}
