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
//p操作
int sem_p(int semid){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = SEM_UNDO;
	if(semop(semid,&sbuf,1) == -1){
		perror("P operation Error");
		return -1;
	}
	return 0;
}
//v操作
int sem_v(int semid){
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = 1;
	sbuf.sem_flg = SEM_UNDO;
	if((semop(semid,&sbuf,1)) == -1){
		perror("V operation Error");
		return -1;
	}
	return 0;
}
//创建信号量
int creat_sem(key_t key){
	int sem_id;
	if((sem_id = semget(key,1,IPC_CREAT|0666)) == -1){
		perror("semget error");
		return -1;
	}
	return 0;
}
//初始化信号量
int init_sem(int semid,int value){
	union semun tmp;
	tmp.val = value;
	if(semctl(semid,0,SETVAL,tmp) == -1){
		perror("Init semaphare Error");
		return -1;
	}
	return 0;
}
//删除信号量
int del_sem(semid){
	union semun tmp;
	if(semctl(semid,0,IPC_RMID,tmp) == -1){
		perror("Delete Semaphore Error");
		return -1;
	}
	return 0;
}

int main(){
	key_t key;
	int shmid,semid,msqid;
	char *shm;
	struct msg_form msg;
	int flag = 1;

	if((key = ftok(".",1)) < 0){
		perror("ftok error");
		exit(1);
	}
	if((shmid = shmget(key,0,0)) == -1){
		perror("shmget error");
		exit(1);
	}
	shm = (char*)shmat(shmid,0,0);
	if((int)shm == -1){
		perror("Attach Share Memory Error");
		exit(1);
	}
	if((msqid = msgget(key,0)) == -1){
		perror("msgget error");
		exit(1);
	}
	if((semid = semget(key,0,0)) == -1){
		perror("semget error");
		exit(1);
	}
	//写数据
	printf("***************************************\n");
    	printf("*                 IPC                 *\n");
    	printf("*    Input r to send data to server.  *\n");
    	printf("*    Input q to quit.                 *\n");
    	printf("***************************************\n");
	
	while(flag){
		char c;
		printf("Please input command: ");
		scanf(" %c",&c);
		switch(c){
			case 'r':
				printf("Data to send : ");
				sem_p(semid);
				scanf("%s",shm);
				sem_v(semid);
				//清空标准输入输出缓冲区
				while((c=getchar()) != '\n' && c!=EOF);
				msg.mtype = 888;
				msg.mtext = 'r';
				msgsnd(msqid,&msg,sizeof(msg.mtext),0);
				break;
			case 'q':
				msg.mtype = 888;
				msg.mtext = 'q';
				msgsnd(msqid,&msg,sizeof(msg.mtext),0);
				flag = 0;
				break;
			default:
				printf("Wrong input!\n");
				//清空标准输入缓冲区
				while((c=getchar()) != '\n' && c !=EOF);

		}
	}
	shmdt(shm);
	return 0;

}
