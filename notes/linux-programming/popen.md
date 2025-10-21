1. 包含的库
SYNOPSIS
       #include <stdio.h>
2. 函数原型
       FILE *popen(const char *command, const char *type);
 
       int pclose(FILE *stream);
3. 函数说明
	popen()函数通过创建一个管道，调用fork()产生一个子进程，执行一个shell以运行命令来开启一个进程。这个管道必须由pclose()函数关闭，而不是fclose()函数。pclose()函数关闭标准I/O流，等待命令执行结束，然后返回shell的终止状态。如果shell不能被执行，则pclose()返回的终止状态与shell已执行exit一样

4. 参数说明
	type：参数只能是读或者写中的一种，得到的返回值（标准I/O流）也具有和type相应的只读或只写类型。如果type是"r"则文件指针连接到command的标准输出；如果type是"w"则文件指针连接到command的标准输入。

	command：参数是一个指向以NULL结束的shell命令字符串的指针。这行命令将被传到bin/sh并使用-c标志，shell将执行这个命令。
	popen()：的返回值是个标准I/O流，必须由pclose来终止。前面提到这个流是单向的（只能用于读或写）。向这个流写内容相当于写入该命令的标准输入，命令的标准输出和调用popen()的进程相同；与之相反的，从流中读数据相当于读取命令的标准输出，命令的标准输入和调用popen()的进程相同。
5. 返回值
	如果调用fork()或pipe()失败，或者不能分配内存将返回NULL，否则返回标准I/O流。popen()没有为内存分配失败设置errno值。如果调用fork()或pipe()时出现错误，errno被设为相应的错误类型。如果type参数不合法，errno将返回EINVAL。
6. 对比system函数
相比只运行指令的system()，popen函数还可以获取运行的结果
