#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>

/*创建守护进程*/
int main(){
	int ret;
	/*1.创建子进程 父进程退出	*/
	pid_t pid = -1;
	pid = fork();
	if(pid == -1){
		perror("fork");
		return 1;
	}
	if(pid > 0){
		/*父进程退出*/
		exit(0);
	}
	/*2.创建新的会话*/
	if((pid = setsid()) == -1){
		perror("setsid");
		exit(1);
	}

	/*3.改变当前工作目录*/

	if((ret = chdir("./")) == -1){
		perror("chdir");
		return 1;
	}
	/*4.设计权限掩码*/
	umask(0);
	/*5.关闭文件描述符*/
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	/*6.执行核心的任务*/
	while(1){
		system("date >> time.log");
		sleep(1);
	}
	return 0;
}
