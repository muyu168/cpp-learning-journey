#include<stdio.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	/*创建套接字*/
	int lfd = socket(AF_INET,SOCK_STREAM,0);
	/*绑定*/
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	//addr.sin_addr.s_addr = INADDR_ANY;绑定的是通配地址
	inet_pton(AF_INET,"192.168.0.194",&addr.sin_addr.s_addr);
	bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
	/*监听*/
	listen(lfd,128);
	/*提取*/
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);
	char ip[16] = "";
	printf("new client ip = %s port = %d\n",inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ip,16),ntohs(cliaddr.sin_port));
	/*读写*/
	char hallo[] = "hello this is server\n";
	write(cfd,hallo,sizeof(hallo));
	
	while(1){
		char buf[1024] = "";
		int n = read(cfd,buf,sizeof(buf));
		write(STDOUT_FILENO,buf,n);
	}
	/*关闭*/
	close(lfd);
	close(cfd);
	return 0;
}

