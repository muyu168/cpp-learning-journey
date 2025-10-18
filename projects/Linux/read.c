#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

int main(){
	int fd;
	char *buf = (char*)malloc(19);
	fd = open("../../demo.md",O_RDWR);
	//read函数原型: ssize_t read(int fd, void *buf, size_t count);
	int n_read = read(fd,buf,18);
	printf("read %d byte,context:%s\n",n_read,buf);	
	close(fd);
	return 0;
}
