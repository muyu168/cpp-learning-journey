#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void){
	printf("this pro get system data\n");
	if(execl("/usr/bin/date","date",NULL) == -1){
		printf("execl failed \n");
		perror("why");
	}
	printf("after execl\n");
	return 0;
}
