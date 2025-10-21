1. exec族函数的作用

我们用fork函数创建新进程后，经常会在新进程中调用exec函数去执行另外一个程序。当进程调用exec函数时，该进程被完全替换为新程序。因为调用exec函数并不创建新进程，所以前后进程的ID并没有改变
2. exec族函数的功能

在调用进程内部执行一个可执行文件。可执行文件既可以是二进制文件，也可以是任何Linux下可执行的脚本文件。
3. exec函数与fork函数的关系和区别

fork() - 复制进程：

创建当前进程的完整副本

子进程继承父进程的所有代码、数据和状态

父子进程并行执行相同代码

调用一次，返回两次（父进程返回子进程PID，子进程返回0）

exec() - 替换进程：

用新程序完全替换当前进程

进程PID不变，但代码、数据、堆栈全部被新程序覆盖

原进程代码不再执行，开始执行新程序的main函数

成功调用不返回，失败才返回

一句话总结
fork() 是"生孩子"（创建新进程），exec() 是"换灵魂"（替换程序内容） —— 通常先 fork() 创建子进程，再在子进程中 exec() 加载新程序，这样既能并行执行，又能运行不同程序。
4. 函数族

exec函数族分别是：execl,execlp,execle,execv,execvp,execvpe
5. 函数原型

#include <unistd.h>
extern char **environ;
 
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg,..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[],char *const envp[]);
6. 参数说明

path：可执行文件的路径名字
arg：可执行程序所带的参数，第一个参数为可执行文件名字，没有带路径且arg必须以NULL结束
file：如果参数file中包含/，则就将其视为路径名，否则就按 PATH环境变量，在它所指定的各目录中搜寻可执行文件。

exec族函数参数极难记忆和分辨，函数名中的字符会给我们一些帮助：
l : 使用参数列表
p：使用文件名，并从PATH环境进行寻找可执行文件
v：应先构造一个指向各参数的指针数组，然后将该数组的地址作为这些函数的参数。
e：多了envp[]数组，使用新的环境变量代替调用进程的环境变量

7. 返回值

*exec函数族的函数执行成功后不会返回，调用失败时，会设置errno并返回-1，然后从原程序的调用点接着往下执行。*

