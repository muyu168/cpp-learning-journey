1. 线程的基本概念
什么是线程？

// 进程 vs 线程的直观理解
进程 = 工厂厂房 + 原材料 + 工人(线程) + 生产设备
线程 = 工厂中的工人（共享厂房和资源，各自独立工作）
线程定义
线程：程序执行流的最小单元，是进程中的一个实体

轻量级进程：比进程更小的独立运行单位

共享环境：同一进程内的线程共享大部分资源

2. 线程的主要特点
	类Unio系统中，早期是没有“线程”概念的，80年代才引入，借助进程机制实现了线程的概念。
因此在这类系统中，进程和线程关系密切：
1）线程是轻量级进程(light-weight-
		process),也有PCB，创建线程使用的底层函数和进程一样，都是clone
2）从内核里看进程和线程是一样的，都有各自不同的PCB
3）进程可以蜕变为线程
4）Linux下，线程是最小的执行单位；进程是最小的分配资源单位
共享性

// 线程共享进程的：
// - 代码段
// - 数据段（全局变量、静态变量）
// - 堆空间
// - 打开的文件描述符
// - 信号处理程序

//线程非共享资源
1）线程Id
2）处理器现场和栈指针（内核栈）
3）独立的栈空间（用户空间栈）
4）errno变量
5）信号屏蔽字
6）调度优先级
独立性

// 每个线程独有：
// - 线程ID
// - 寄存器集合（包括程序计数器PC）
// - 栈空间（局部变量）
// - 错误码errno
// - 信号掩码
// - 优先级
并发性

// 多线程可以并发执行
// 单核CPU：时间片轮转，看起来并发
// 多核CPU：真正并行执行
3. 线程的优缺点
优点：
提高程序并发性，开销小，数据通信、共享数据方便
缺点：
库函数，不稳定，调试、编写困难、GDB不支持，信号支持不好
4. 函数原型
1. 线程管理函数
1.1 线程创建 - pthread_create()
c
#include <pthread.h>

int pthread_create(pthread_t *thread, 
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *), 
                   void *arg);
参数说明：

thread：指向线程标识符的指针（输出参数）

attr：线程属性，NULL表示默认属性

start_routine：线程函数的指针

arg：传递给线程函数的参数
1.2 线程退出 - pthread_exit()
c
#include <pthread.h>

void pthread_exit(void *retval);
参数说明：

retval：线程的返回值，可以被其他线程通过pthread_join获取
1.3 线程等待 - pthread_join()
c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
参数说明：

thread：要等待的线程ID

retval：指向存储线程返回值的指针（输出参数）
1.4 线程分离 - pthread_detach()
c
#include <pthread.h>

int pthread_detach(pthread_t thread);
1.5 线程取消 - pthread_cancel()
c
#include <pthread.h>

int pthread_cancel(pthread_t thread);
2. 线程属性函数
2.1 属性初始化/销毁
c
#include <pthread.h>

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
2.2 设置线程属性
c
// 设置分离状态
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

// 设置栈大小
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

// 设置调度策略
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);

// 设置调度参数
int pthread_attr_setschedparam(pthread_attr_t *attr,
                              const struct sched_param *param);
3. 互斥锁函数
3.1 互斥锁初始化/销毁
c
#include <pthread.h>

// 静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 动态初始化
int pthread_mutex_init(pthread_mutex_t *mutex,
                      const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
3.2 互斥锁操作
c
int pthread_mutex_lock(pthread_mutex_t *mutex);    // 阻塞加锁
int pthread_mutex_trylock(pthread_mutex_t *mutex); // 非阻塞加锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);  // 解锁
4. 条件变量函数
4.1 条件变量初始化/销毁
c
#include <pthread.h>

// 静态初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// 动态初始化
int pthread_cond_init(pthread_cond_t *cond,
                     const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
4.2 条件变量操作
c
int pthread_cond_wait(pthread_cond_t *cond,
                     pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond,
                          pthread_mutex_t *mutex,
                          const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);     // 唤醒一个线程
int pthread_cond_broadcast(pthread_cond_t *cond);  // 唤醒所有线程
5. 读写锁函数
5.1 读写锁初始化/销毁
c
#include <pthread.h>

int pthread_rwlock_init(pthread_rwlock_t *rwlock,
                       const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
5.2 读写锁操作
c
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);    // 读锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);    // 写锁
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);    // 解锁
6. 线程局部存储
6.1 TLS 键管理
c
#include <pthread.h>

int pthread_key_create(pthread_key_t *key,
                      void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
6.2 TLS 数据访问
c
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
7. 返回值检查宏
实用错误检查宏：

c
#define CHECK_PTHREAD(call) \
    do { \
        int result = (call); \
        if (result != 0) { \
            fprintf(stderr, "Pthread error at %s:%d - %s\n", \
                   __FILE__, __LINE__, strerror(result)); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// 使用示例
CHECK_PTHREAD(pthread_create(&thread, NULL, worker, NULL));
CHECK_PTHREAD(pthread_join(thread, NULL));
8. 编译说明
编译时需要链接 pthread 库：

bash
gcc -o program program.c -lpthread
# 或者
gcc -pthread -o program program.c
