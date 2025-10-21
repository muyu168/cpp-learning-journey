#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

//int mkfifo(const char *pathname,mode_t mode);
//int open(const char *pathname, int flags);


int main(){
	int fd;
	char buf[128] = {0};
	if(mkfifo("mkfifo",0600) == -1 && errno != EEXIST){
		printf("mkfifo fail\n");
		perror("why");
	}
	fd = open("mkfifo",O_RDONLY);
	if(fd != -1){
		printf("read open success\n");
	}
	while(1){
		int n_read = read(fd,buf,128);
		printf("read %d byte,context = %s\n",n_read,buf);
	}
	close(fd);
	return 0;
}
