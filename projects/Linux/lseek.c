#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
	char *buf = "我听到了";
	int fd = open("../../demo.md",O_RDWR);
	printf("open file success \n");
	lseek(fd,18,SEEK_SET);//将光标移动到头后，相对头偏移18个字节（因为在write.c中曾插入过18个字节的“我有话对你说”这句话）

	int n_write = write(fd,buf,strlen(buf));
	printf("write %d byte to file \n",n_write);
	lseek(fd,-n_write,SEEK_CUR);// off_t offset为负数时向文件开头偏移，也就是向前偏移
	char *readbuf = (char*)malloc(strlen(buf)+ 1);
	int n_read = read(fd,readbuf,n_write);
	printf("read %d byte,context:%s\n",n_read,readbuf);
	close(fd);
	return 0;
}
