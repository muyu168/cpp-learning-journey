#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(){
	int fd;
	char *buf = "我有话对你说";

	fd = open("../../demo.md",O_RDWR);
	if(fd == -1){
		printf("open file fail \n");
		fd = open("../../demo.md",O_RDWR|O_CREAT,0600);//如果demo.md不存在，就创建它
		if(fd > 0){
			printf("creat demo success \n");
		}
	}
	printf("open demo success \n");
	write(fd,buf,strlen(buf));
	close(fd);
	return 0;
}
