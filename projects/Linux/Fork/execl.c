#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void){
	printf("before execl\n");
	//函数原型：int execl(const char *path, const char *arg, ...);
	if(execl("./echoarg", "echoarg", "abc" ,NULL) == -1){
		printf("execl failed! \n");
		perror("why");//如果execl返回出错，返回了一个error，可以被这个perror（）解析出来。
	}
	printf("after execl\n");
	return 0;
	//实验说明：
	//我们先用gcc编译echoarg.c，生成可执行文件echoarg并放在当前路径目录下。文件echoarg的作用是打印命令行参数。然后再编译execl.c并执行execl可执行文件。用execl 找到并执行echoarg，将当前进程main替换掉，所以”after execl” 没有在终端被打印出来。
	//也就是说exec函数会将原本进程代码中调用exec函数后的代码替换为新打开程序的程序内容，所以只有在调用失败之后才能输出after execl，成功则没有输出，因为该printf被覆盖掉了。	
}
