#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdlib.h>
#include<unistd.h>

//联合体，用于semctl初始化
union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//初始化信号量
int init_sem(int sem_id,int value){
	union semun tmp;
	tmp.val = value;
	if(semctl(sem_id,0,SETVAL,tmp) == -1){
		perror("why");
		return -1;;
	}
	return 0;
}

//P操作
//信号量为1，获取资源并将信号量-1
//若信号量为0，进程挂起等待
int sem_p(int sem_id){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sbuf,1) == -1){
		perror("why");
		return -1;
	}
	return 0;
}
//v操作
//释放资源并将信号值+1
//如果有进程正在挂起等待，则唤醒它们
int sem_v(int sem_id){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sbuf,1) == -1){
		perror("why");
		return -1;
	}
	return 0;
}
//删除信号量集
int del_sem(int sem_id){
	union semun tmp;
	if(semctl(sem_id,0,IPC_RMID,tmp) == -1){
		perror("delete semaphore error\n");
		return -1;
	}
	return -1;
}
int main(){
	int sem_id;
	key_t key;
	pid_t pid;
	//获取key值
	if(key = ftok(".",'z') < 0){
		perror("ftok error\n");
		exit(1);
	}
	//创建信号量集，其中只有一个信号量
	if((sem_id = semget(key,1,IPC_CREAT|0666)) == -1){
		perror("semget error\n");
		exit(1);
	}
	//初始化：初值设置为0
	init_sem(sem_id,0);
	if((pid = fork()) == -1){
		perror("Fork error");
	}else if(pid == 0){
		sleep(2);
		printf("process child : pid = %d\n",getpid());
		sem_v(sem_id);
	}else{
		sem_p(sem_id);
		printf("process father : pid = %d\n",getpid());
		sem_v(sem_id);
		del_sem(sem_id);
	}
	return 0;
	
}

