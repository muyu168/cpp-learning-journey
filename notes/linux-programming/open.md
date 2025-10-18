另一个没用的东西（忘记O_TRUNC会删除文件内容QAQ）
1.open函数头文件
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
2.open函数原型
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
int creat(const char *pathname, mode_t mode);
3.函数参数说明
pathname：要打开的文件名（含路径）

flags ： 

 O_RDONLY 只读打开，O_WRONLY 只写打开，O_RDWR 可读可写打开

当我们附带了权限后,打开的文件就只能按照这种权限来操作。

以上这三个参数中应当只指定一个。下列参数是可选择的:

1.O-CREAT    ：若文件不存在则创建它。使用此选项时,需要同时说明第三个参数mode.用其说明该新文件的存取许可权限。
2.O_EXCL      ：以这种属性去打开文件时，如果同时指定了O_CREAT，而文件已经存在，则打开文件失败（也可以说返回值是-1）。
3.O_APPEND ：以这种属性去打开文件时，每次写时都加到文件的尾端。(不想写入时文件被覆盖，用这个flag，正常写入没有加其他条件的话，原来文件中的数据内容会被覆盖，注意是覆盖，覆盖本身的字节长度，没有覆盖的会保留，不是删除)
4.O_TRUNC   ：以这种属性去打开文件时,如果这个文件中本来是有内容的,而且为只读或只写成功打开,则将其长度截短为0，通俗点的意思就是把原来的文件中的内容干掉，写入你自己要的数据内容
Mode：一定是在flags中使用了 O-CREAT 标志,mode 记录待创建的文件的访问权限，（比如：给的是0600时，则文件所有者给的权限是 可读可写
4.open函数概述
DESCRIPTION
Given  a  pathname  for  a  file,  open() returns a file descriptor, a small, nonnegative integer for use in subsequent system calls(read(2), write(2), lseek(2), fcntl(2), etc.).  The file descriptor returned by a successful call will be the  lowest-numbered  file descriptor not currently open for the process.

给定一个文件的路径名，open()返回一个文件描述符，一个小的非负整数，用于后续的系统调用(read(2)， write(2)， Iseek(2)， fcntl(2)，等等)。成功调用返回的文件描述符将是进程当前未打开的编号最低的文件描述符。失败：返回 -1
没用的东西
没用的东西