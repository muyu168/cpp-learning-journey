#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/msg.h>
#include<string.h>

//消息队列结构
struct msg_form{
	long mtype;
	char mtext;
};

//联合体，用于semctl初始化
union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//初始化信号量
int init_sem(int sem_id, int value){
	union semun tmp;
	tmp.val = value;
	if(semctl(sem_id,0,SETVAL,tmp) == -1){
		perror("Init Semaphore Error");
		return -1;
	}
	return 0;
}

//p操作
int sem_p(int sem_id){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sbuf,1) == -1){
		perror("P operation Error");
		return -1;
	}
	return 0;
}

//v操作

int sem_v(int sem_id){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sbuf,1) == -1){
		perror("V operation Error");
		return -1;
	}
	return 0;
}

//删除信号量

int del_sem(int sem_id){
	union semun tmp;
	if(semctl(sem_id,0,IPC_RMID,tmp) == -1){
		perror("Delete Semaphore Error");
		return -1;
	}
	return 0;
}

//创建信号量/

int creat_sem(key_t key){
	int sem_id = semget(key,1,IPC_CREAT|0666);
	if(sem_id == -1){
		perror("semget error");
		return -1;
	}
	/*if(init_sem(sem_id,1) == -1){
		perror("Init semapore Error");
		return -1;
	}*/
	return sem_id;
}

int main(){
	key_t key;
	int semid,shmid,msqid;
	char *shm;
	struct shmid_ds buf1;/*用于删除共享内存*/
	struct msqid_ds buf2;/*用于删除消息队列*/
	struct msg_form msg;/*消息队列用来通知对方更新了共享内存*/

	//获取key值
	if((key = ftok(".",1)) < 0 ){
		perror("why");
		exit(1);
	}
	// 创建共享内存
	if((shmid = shmget(key,1024,IPC_CREAT|0666)) == -1){
		perror("why");
		exit(1);
	}
	//连接共享内存
	shm = (char*)shmat(shmid,0,0);
	if((int)shm == -1){
		perror("why");
		exit(1);
	}
	//创建消息队列
	if((msqid = msgget(key,IPC_CREAT|0777)) == -1){
		perror("why");
		exit(1);
	}
	//创建信号量
	semid = creat_sem(key);
	//读数据
	printf("this is server\n");
	while(1){
		msgrcv(msqid,&msg,1,888,0);
		if(msg.mtext == 'q') break;//退出循环
		if(msg.mtext == 'r'){
			sem_p(semid);
			printf("Received : %s\n",shm);
			sem_v(semid);
		}
	}
	shmdt(shm);
	shmctl(shmid,IPC_RMID,&buf1);
	msgctl(msqid,IPC_RMID,&buf2);
	del_sem(semid);
	return 0;
}
