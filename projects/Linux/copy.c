#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char **argv){
	int fdSrc;
	int fdDes;
	char *readbuf;
	if(argc != 3){
		printf("param error \n");
		exit(-1);
	}
	fdSrc = open(argv[1],O_RDWR);
	int size = lseek(fdSrc,0,SEEK_END);
	lseek(fdSrc,0,SEEK_SET);
	readbuf = (char*)malloc(sizeof(char)*size + 1);
	read(fdSrc,readbuf,size);
	readbuf[size] = '\0';
	fdDes = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0600);
	char *writebuf = readbuf;
	write(fdDes,writebuf,size);
	close(fdSrc);
	close(fdDes);
	free(readbuf);
	return 0;
}
