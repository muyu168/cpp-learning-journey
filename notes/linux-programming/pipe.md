1. 无名管道
1.1 特点
它是半双工的（即数据只能在一个方向上流动），具有固定的读端和写端。
它只能用于父子进程之间的通信。
管道是创建在内存中，进程结束空间释放，管道不复存在。对于它的读写可以使用普通的read、write等函数
1.2 pipe函数
所包含库
SYNOPSIS
       #include <unistd.h>
函数原型
       int pipe(int pipefd[2]);
1.3 返回值
返回值：成功返回0，失败返回-1

当一个管道建立时，它会创建两个文件描述符：fd[0]为读而打开，fd[1]为写而打开。
1.4管道特性
管道的特性:管道里没有数据会阻塞
2. 命名管道（有名管道）
2.1 特点
命名管道可以在无关的进程之间交换数据，与无名管道不同。
命名管道有路径名与之相关联，它以一种特殊设备文件形式存在于文件系统中。
2.2 mkfifo函数
所包含库
#include<sys/types.h>
#include<sys/stat.h>
函数原型
	int mkfifo(onst char *pathname, mode_t mode);
参数说明
	其中mode参数与open函数中的mode相同。一旦创建了一个FIFO，就可以用一般的文件I/O函数来操作。

当open 一个FIFO时，是否设置非阻塞标志（O_NONBLOCK）的区别：

若没有指定O_NONBLOCK（默认），只读open要阻塞到某个其他进程为写而打开FIFO。类似的，只写open要阻塞到某个其他进程为读而打开它。（一般选择默认）
若是指定O_NONBLOCK，则只读open立即返回。而只写open将出错返回-1，如果没有进程已经为读而打开该FIFO，其errno为ENXIO。
FIFO的通信方式类似于在进程中使用文件来传输数据，只不过FIFO类型文件同时具有管道的特性。在数据读出时，FIFO管道中同时清楚数据，并且”先进先出“。
3. 消息队列
消息队列，是信息的链接表，存放在内核中。一个消息队列由一个标识符（即队列 ID）来标识。
3.1 特点
消息队列是面向记录的，其中的消息具有特地那个的格式以及特定的优先级。

消息队列独立于发送与接收进程。进程终止时，消息队列及内容并不会删除。

消息队列可以实现信息的随机查询，消息不一定要以先进先出的次序读取，也可以按消息的类型读取。 
3.2 函数
所包含库
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
函数原型
//创建或打开消息队列：成功返回队列 ID ，失败返回 -1    
int msgget(key_t key, int msgflg);
//添加消息：成功返回 0 ，失败返回 -1 
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
//读取信息：成功返回消息数据的长度，失败返回 -1
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,int msgflg);
//控制消息队列:成功返回 0 ，失败返回 -1 
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
参数说明
key值很关键，相当于一个索引。进程通过这个键值，在Linux内核中找到相应的队列（确保了队列是同一个）

在以下两种情况中，msgget 将创建一个新的消息队列：

如果没有与键值相对应的消息队列，并且flag中包含了 IPC_CREAT 标志位。
key 参数为 IPC_PRIVATE。
函数msgrcv在读取消息队列时，type参数有下面几种情况。

type == 0 ，返回队列中的第一个消息；
type > 0 ，返回队列中消息类型为 type 的第一个消息；
type < 0 ，返回队列中消息类型值小于或等于 type 绝对值的消息，如果有多个，则取类型值最小的消息。


