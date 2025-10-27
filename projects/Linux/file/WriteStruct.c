#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

struct Test {
	int a;
	char b;
};

int main(int argc,char **argv){
	int fd;
	struct Test data[2] = {{100,'a'},{101,'b'}};
	struct Test data2[2];
	fd = open("../../demo.md",O_RDWR);
	if(fd == -1){
		printf("open fail \n");
		exit(-1);
	}
	lseek(fd,0,SEEK_END);
	int n_write = write(fd,&data,sizeof(struct Test)*2);
	if(n_write == -1){
		printf("write fail \n");
		exit(-1);
	}
	lseek(fd,-n_write,SEEK_END);
	int n_read = read(fd,&data2,sizeof(struct Test)*2);
	printf("read %d,%c \n",data2[0].a,data2[0].b);
	printf("read %d,%c \n",data2[1].a,data2[1].b);
	close(fd);
	return 0;
}
