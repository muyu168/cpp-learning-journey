基础知识
1.包含的头文件
#include<unistd.h>
2.函数原型
ssize_t read(int fd, void *buf, size_t count);
3.函数参数说明
int fd ：文件描述符
const void *buf ：一个无类型的指针buf，是一个缓冲区
size_t count：你要读取文件的大小
【整个函数的意思是：从 fd 指向的文件里面的数据，读取多少个字节数据到缓存区 buf 里面去。】
