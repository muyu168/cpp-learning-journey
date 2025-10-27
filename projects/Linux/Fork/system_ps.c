#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(){
	printf("this pro get system date:\n");
	if(system("ps -l") == -1){
		printf("system failed!\n");
		perror("why");
	}
	printf("after system\n");
	return 0;
}
