system()函数
1. 所包含库
SYNOPSIS
#include <stdlib.h>
2. 函数原型
int system(const char*command);
3. 函数返回值
成功，则返回进程的状态值；

当sh不能执行时，返回127；

失败返回-1；
4. system（）函数源码
 
int system(const char * cmdstring)
{
    pid_t pid;
    int status;
    if(cmdstring == NULL)
    {
        return (1); //如果cmdstring为空，返回非零值，一般为1
    }
    if((pid = fork())<0)
    {
        status = -1; //fork失败，返回-1
    }
    else if(pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127); /* exec执行失败返回127，注意exec只在失败时才返回现在的进程，成功的话现在的
        进程就不存在啦8*/
    }
    else //父进程
    {
        while(waitpid(pid, &status, 0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1; //如果waitpid被信号中断，则返回-1
                break;
            }
        }
    }
    return status; //如果waitpid成功，则返回子进程的返回状态
}
5.函数作用
作用
执行 shell 命令：在 C 程序中直接调用和执行任意的 shell 命令。
6.system与exec的对比
|特性		|system()		|exec() 系列	|
|使用难度	|简单			|复杂		|
|功能		|执行 shell 命令	|执行可执行程序	|
|进程控制	|阻塞等待命令完成	|替换当前进程	|
|shell 功能	|支持管道、重定向等	|不支持		|
返回值	命令的退出状态	成功不返回，失败返回-1

system( ) 函数调用完之后，代码还会往下走。 而exec族函数则不会往下走。
