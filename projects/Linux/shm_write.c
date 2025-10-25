#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
// int shmget(key_t key,size_t size, int shmflg);
// void *shmat(int shmid, const void *shmaddr, int shmflg);
//

int main(){
	int shmid;
	char *shmaddr;
	key_t key = ftok(".",1);
	shmid = shmget(key,1024*4,IPC_CREAT|0666);
	if(shmid == -1){
		printf("creat shm fail\n");
		exit(-1);
	}
	shmaddr = shmat(shmid,0,0);
	printf("shmat OK\n");
	strcpy(shmaddr,"hello world\n");
	sleep(5);
	shmdt(shmaddr);
	shmctl(shmid,IPC_RMID,0);
	printf("quit\n");
	return 0;
}
