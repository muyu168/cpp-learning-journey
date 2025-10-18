1. 进程的相关概念
1.1什么是程序，什么是进程
程序是静态的概念，gcc xxx.c -o pro

磁盘中生成的文件，叫做程序。

进程是程序的一次运行活动，通俗点的意思就是程序跑起来了，系统中就多了一个进程。
2. 如何查看系统中有哪些进程
2.1使用ps指令查看
实际工作中，配合grep来查找程序中是否存在某一个进程
ps -aux|grep xxx;
2.2使用top指令查看
类似windows的任务管理器
3. 什么是进程标识符
每一个进程都有一个非负整数表示的唯一ID，叫做pid，类似身份证

pid = 0 ；称为交换进程（swapper）

作用——进程的调度

pid = 1 ；init 进程

作用——系统的初始化

编程调用 getpid 函数获取滋生的进程标识符，getppid 获取父进程的进程标识符
4. 什么叫父进程，什么叫子进程
进程 A 创建了进程 B 

那么 A 叫做父进程，B 叫做子进程，父子进程是相对的概念，理解为人类中的父子关系
5. C程序存储空间分配 
高地址
┌─────────────────┐
│   栈(stack)     │ ← 向下增长
├─────────────────┤
│       ↓        │
│       ↑        │
├─────────────────┤
│    堆(heap)     │ ← 向上增长  
├─────────────────┤
│   BSS段        │
├─────────────────┤
│   数据段        │
├─────────────────┤
│   代码段        │
└─────────────────┘
低地址

【详细介绍】
5.1 代码段 (Text Segment)
内容: 程序指令（机器代码）

特性: 只读、共享、固定大小

存储: 所有可执行代码

5.2 数据段 (Data Segment)
初始化数据段
存储: 显式初始化的全局/静态变量

示例: int x = 10; static int y = 20;

BSS段 (Block Started by Symbol)
存储: 未初始化或初始化为0的全局/静态变量

特性: 程序启动时自动清零

示例: int a; static char buf[100];

5.3 堆 (Heap)
管理: 手动分配释放 (malloc/free)

特性: 动态增长、需要手动管理

生命周期: 直到调用free()

示例: int *p = malloc(100 * sizeof(int));

5.4 栈 (Stack)
存储: 局部变量、函数参数、返回地址

管理: 自动分配释放

特性: 后进先出(LIFO)、大小有限

生命周期: 函数调用期间

示例: int local_var = 5;
6. 创建进程函数fork的使用，与vfork的区别
6.1 fork函数简介
使用 fork 函数创建一个进程

pid_t  fork(void);

fork 函数调用成功，返回两次

返回值为0，代表当前进程是子进程

返回值是非负数，代表当前进程为父进程

调用失败，返回 -1
7. vfork函数
关键区别一：

vfork直接使用父进程的存储空间，不拷贝

关键区别二：

vfork保证子进程先运行，当子进程调用exit退出后，父进程才执行
8. 进程的退出
8.1 正常退出
1 Main函数调用return
2 进程调用 exit( ) ，标准C库，一般写 exit(0);
3 进程调用 _exit( ) 或者 _Exit( )，属于系统调用
进程最后一个线程返回
最后一个线程调用pthread_exit
8.2 异常退出
调用 abort
当进程收到某些信号时，如 ctrl + c
最后一个线程对取消（cancellation）请求做出响应
总之，不管进程如何终止，最后都会执行内核中的同一段代码。这段代码为相应进程关闭所有打开描述符，释放它所使用的存储器等。

对上述任意一种终止清醒，我们都希望终止进程能够通知其父进程它是如何终止的。对于三个终止函数（ exit、_exit、 _Exit ），实现这一点的方法是，将其退出状态（exit status）作为参数传送给函数。在异常终止情况下，内核（不是进程本身）产生一个指示其异常终止原因的终止状态（termination stastus）。在任意一种情况下，该终止进程的父进程都能用 wait 或 waitpid 函数取得其终止状态。

同时值得注意的是，在编程的时候，进程的退出最好是用三个终止函数（ exit、_exit、 _Exit ）其中的一个，当然建议 exit 即可。不能用 break 等其他的终止，因为会导致数据被破坏
9. 父进程等待子进程退出
9.1 僵尸进程
一般情况下，父进程需要等待子进程退出，然后回收子进程的资源，如果没有及时回收子进程的资源，就会产生僵尸进程，每个僵尸进程会占用一个进程ID（PID），Linux系统的PID数量有限（通常32768个），如果短时间内产生大量僵尸进程会导致无法创建新进程，而为了防止这种情况发生，通常使用wait函数。
9.2 wait函数
（1 wait类函数
	NAME
	wait, waitpid, waitid - wait for process to change state
	用于等待进程的更改状态
（2包含的库
SYNOPSIS
#include <sys/types.h>
#include <sys/wait.h>
（3函数原型
   pid_t wait(int *status);

   pid_t waitpid(pid_t pid, int *status, int options);

   int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
（4参数说明
pid_t pid;      // 指定要等待的子进程PID
int *status;    // 输出参数，存储退出状态（非空：子进程退出状态存放在它指向的地址中。空：不关心退出状态
int options;    // 选项标志
idtype_t idtype;    // 指定id的类型
id_t id;            // 根据idtype指定要等待的进程
siginfo_t *infop;   // 输出参数，存储详细的进程信息
（5参数说明
pid参数的取值
pid == -1    // 等待任意子进程（与wait相同）
pid > 0      // 等待指定PID的子进程
pid == 0     // 等待与调用进程同进程组的任意子进程
pid < -1     // 等待进程组ID等于|pid|的任意子进程
idtype参数的取值
P_PID       // 等待特定PID的进程
P_PGID      // 等待特定进程组的所有进程  
P_ALL       // 等待任意子进程
options参数的常用标志
WEXITED     // 等待已终止的进程
WSTOPPED    // 等待被信号停止的进程
WCONTINUED  // 等待被继续执行的进程
WNOHANG     // 非阻塞模式
WNOWAIT     // 不消费退出状态，进程保持可等待状态
（6状态检查宏函数
 所有wait函数都可以使用这些宏来检查状态,当wait函数status参数非空，并且我们要查看返回的是哪一个子进程的终止状态的状态码的时候

我们就需要下面这个表：检查wait和waitpid所返回的终止状态的宏，来解析状态码。
WIFEXITED(status)    // 正常退出返回真
WEXITSTATUS(status)  // 获取退出状态码（当WIFEXITED为真时）

WIFSIGNALED(status)  // 被信号终止返回真  
WTERMSIG(status)     // 获取导致终止的信号编号

WIFSTOPPED(status)   // 进程停止返回真
WSTOPSIG(status)     // 获取导致停止的信号编号

WIFCONTINUED(status) // 进程从停止状态继续返回真
9.3孤儿进程
父进程如果不等待子进程退出，在子进程之前就结束了自己的“生命”，此时的子进程叫做孤儿进程

Linux避免系统存在过多的孤儿进程，init进程（系统的一个初始化进程，它的pid号为1）收留孤儿进程，变成孤儿进程的父进程
