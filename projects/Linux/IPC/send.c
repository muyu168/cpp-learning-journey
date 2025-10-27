#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

struct msgbuf{
	long mtype;
	char mtext[128];
};

int main(){
	struct msgbuf sendBuf = {888,"this is msgged form que"};
	struct msgbuf readBuf;
	key_t key;
	key = ftok(".",1);
	printf("key = %d\n",key);
	int msgId = msgget(key,IPC_CREAT|0777);
	if(msgId == -1){
		perror("why");
	}
	while(1){
		msgsnd(msgId,&sendBuf,strlen(sendBuf.mtext),0);
		sleep(1);
		printf("write success\n");
		msgrcv(msgId,&readBuf,sizeof(readBuf.mtext),988,0);
		sleep(1);
		printf("read form que: %s\n",readBuf.mtext);
	}
	msgctl(msgId,IPC_RMID,NULL);
	return 0;
}
