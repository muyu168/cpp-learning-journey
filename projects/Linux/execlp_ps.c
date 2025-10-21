#include<stdio.h>
#include<unistd.h>

int main(){
	printf("before execl\n");

	if(execlp("ps","ps",NULL,NULL) == -1){
		printf("execl failed\n");
		perror("why");
	}
	printf("after execl\n");
	return 0;
}
