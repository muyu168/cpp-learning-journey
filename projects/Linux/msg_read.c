#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<unistd.h>

struct msgbuf{
	long mtype;
	char mtext[128];
};

int main(){
	struct msgbuf readBuf;
	struct msgbuf sendBuf = {988,"this is msgged father"};
	key_t key;
	key = ftok(".",1);
	printf("key = %d\n",key);
	int msgId = msgget(key,IPC_CREAT|0777);
	if(msgId == -1){
		perror("why");
	}
	while(1){
		msgrcv(msgId,&readBuf,sizeof(readBuf.mtext),888,0);
		sleep(1);
		printf("read form que: %s\n",readBuf.mtext);
		msgsnd(msgId,&sendBuf,strlen(sendBuf.mtext),0);
		printf("write success\n");
	}
	msgctl(msgId,IPC_RMID,NULL);
	return 0;
}
