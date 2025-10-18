#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
//打开一个文件（文件存在时）
void open1(){
	int fd;
	fd = open("../../notes/linux-programming/open.md",O_RDWR);
	printf("fd = %d \n",fd);
	close(fd);
};
//打开一个文件，如果需要打开的文件不存在，那么创建该文件。 
void open2(){
	int fd;
	fd = open("../../notes/linux-programming/open.md",O_RDWR);
	if(fd == -1){
		printf("open file fail \n");
		fd = open("../../notes/linux-programming/open.md",O_RDWR|O_CREAT,0600);
		if(fd > 0){
			printf("creat file success \n");
		}
	}
	printf("open file success:fd = %d \n",fd);
	close(fd);
}
//O_EXCL的使用
void excl(){
        int fd;
        fd = open("../../notes/linux-programming/open.md",O_RDWR|O_CREAT|O_EXCL,0600);
        if(fd == -1){
                printf("file exist \n");
        }
        close(fd);
}
//O_APPEND的使用
void append(){
	char *buf = "没用的东西";
	int fd = open("../../notes/linux-programming/open.md",O_RDWR|O_APPEND);
	//ssize_t write(int fd, const void *buf, size_t count);
	int n_write = write(fd,buf,strlen(buf));
	printf("write %d byte to file \n",n_write);
	close(fd);	
}
//O_TRUNC的使用
void o_trunc(){
	char *buf ="另一个没用的东西";
	int fd = open("../../notes/linux-programming/open.md",O_RDWR|O_TRUNC);
	//ssize_t write(int fd, const void *buf, size_t count);
	int n_write = write(fd,buf,strlen(buf));
	printf("write %d byte to file \n",n_write);
	close(fd);
}
int main(){
	open1();
	open2();
	excl();
	append();
	//o_trunc();
	int fd = creat("./write.c",S_IRWXU);
	return 0;
}
