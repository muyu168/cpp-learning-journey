#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<wait.h>
int main(int argc,char **argv){
	if(argc != 3){
		printf("parames error\n");
		exit(-1);
	}
	pid_t pid;
	int data = -1;
	printf("please input your data:\n");
	scanf("%d",&data);
	if(data > -1){
		pid = fork();
		if(pid == 0){
			char data_str[20];
            		snprintf(data_str, sizeof(data_str), "%d", data);
			execl("./modify_data","modify_data",argv[1],argv[2],data_str,NULL);
			perror("execl失败");
            		exit(1);
		}else{
			int status;
           		wait(&status);

            		if(WIFEXITED(status) && WEXITSTATUS(status) == 0){
                		printf("修改成功！\n");
           		} else {
                		printf("修改失败\n");
            		}	
			wait(NULL);
		}
	}else{
		printf("modify data failed\n");
	}
	return 0;
}
