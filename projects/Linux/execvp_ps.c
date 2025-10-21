#include<stdio.h>
#include<unistd.h>

int main(){
	printf("before execl\n");
	char *argv[] = {"ps",NULL,NULL};
	if(execvp("ps",argv) == -1){
		printf("execl failed\n");
		perror("why");
	}
	printf("after execl\n");
	return 0;
}
