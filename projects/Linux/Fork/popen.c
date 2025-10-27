#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

//函数原型FILE *popen(const char *command, const char *type);
//	size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

int main(){
	FILE *fp;
	char ret[1024] = {0};
	fp = popen("ps","r");
	int nread = fread(ret,1,1024,fp);
	printf("read ret %d byte,ret = \n %s",nread,ret);
	return 0;
}
