#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char **argv){
	int fdSrc;
	char *readbuf = NULL;
	if(argc != 4){
		printf("parmes error");
		exit(1);
	}
	fdSrc = open(argv[1],O_RDWR);
	int size = lseek(fdSrc,0,SEEK_END);
	lseek(fdSrc,0,SEEK_SET);
	readbuf = (char*)malloc(sizeof(char)*size + 1);
	int n_read = read(fdSrc,readbuf,size);
	readbuf[n_read] = '\0';
	char *p = strstr(readbuf,argv[2]);
	if(p == NULL){
		printf("not found\n");
		exit(-1);
	}
	p = p + strlen(argv[2]);//指针偏移
	*p = argv[3][0];
	lseek(fdSrc,0,SEEK_SET);
	int n_write = write(fdSrc,readbuf,n_read);
	close(fdSrc);
	return 0;
}
