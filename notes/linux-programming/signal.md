1. 什么是信号
	对于 Linux来说，实际信号是软中断，许多重要的程序都需要处理信号。信号，为 Linux 提供了一种处理异步事件的方法。比如，终端用户输入了 ctrl+c 来中断程序，会通过信号机制停止一个程序。
2. 信号概述
2.1 信号的名字和编号
	每个信号都有一个名字和编号，这些名字都以“SIG”开头，例如“SIGIO ”、“SIGCHLD”等等。
	信号定义在signal.h头文件中，信号名都定义为正整数。
	具体的信号名称可以使用kill -l来查看信号的名字以及序号，信号是从1开始编号的，不存在0号信号。kill对于信号0有特殊的应用。
2.2 信号的处理
信号的处理有三种方法，分别是：忽略、捕捉 和 默认动作

	忽略信号，大多数信号可以使用这个方式来处理，但是有两种信号不能被忽略（分别是 SIGKILL和SIGSTOP）。因为他们向内核和超级用户提供了进程终止和停止的可靠方法，如果忽略了，那么这个进程就变成了没人能管理的的进程，显然是内核设计者不希望看到的场景
	捕捉信号，需要告诉内核，用户希望如何处理某一种信号，说白了就是写一个信号处理函数，然后将这个函数告诉内核。当该信号产生时，由内核来调用用户自定义的函数，以此来实现某种信号的处理。
系统默认动作，对于每个信号来说，系统都对应由默认的处理动作，当发生了该信号，系统会自动执行。不过，对系统来说，大部分的处理方式都比较粗暴，就是直接杀死该进程。
	具体的信号默认动作可以使用man 7 signal来查看系统的具体定义。在此，我就不详细展开了，需要查看的，可以自行查看。也可以参考 《UNIX 环境高级编程（第三部）》的 P251——P256中间对于每个信号有详细的说明

2.3 信号如何使用
	其实对于常用的 kill 命令就是一个发送信号的工具，kill 9 PID来杀死进程。比如，我在后台运行了一个 top 工具，通过 ps 命令可以查看他的 PID，通过 kill 9 来发送了一个终止进程的信号来结束了 top 进程。如果查看信号编号和名称，可以发现9对应的是 9) SIGKILL，正是杀死该进程的信号。
3. 信号处理函数的注册
信号处理函数的注册不只一种方法，分为入门版和高级版

入门版：函数 signal
高级版：函数 sigaction
4. 信号注册函数（入门版）
4.1 signal函数
所包含库
#include<signal.h>
函数原型
typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
函数说明
	根据函数原型可以看出由两部分组成，一个是真实处理信号的函数，另一个是注册函数了。
	对于**sighandler_t signal(int signum, sighandler_t handler)**;函数来说，signum 显然是信号的编号，handler 是中断函数的指针。
	同样，**typedef void (sighandler_t)(int)**;中断函数的原型中，有一个参数是 int 类型，显然也是信号产生的类型，方便使用一个函数来处理多个信号。
参数说明
signum：信号编号

handler：信号处理函数指针

常用信号列表
信号名	信号值	默认行为	说明
SIGINT	2	终止		键盘中断 (Ctrl+C)
SIGQUIT	3	终止+core	键盘退出 (Ctrl+)
SIGKILL	9	终止		强制杀死 (不能被捕获)
SIGTERM	15	终止		终止信号
SIGALRM	14	终止		定时器信号
SIGCHLD	17	忽略		子进程状态改变
SIGUSR1	10	终止		用户自定义信号1
SIGUSR2	12	终止		用户自定义信号2

信号处理还有另外两种状态，分别是默认处理和忽略，这两种设置很简单，只需要将 handler 设置为 SIG_IGN（忽略信号）或 SIG_DFL（默认动作）即可。
5. kill函数
5.1 函数原型
所包含库
#include<sys/types.h>
#include<signal.h>
函数原型
int kill(pid_ pid, int sig);
6. 信号注册函数（高级版）
6.1 sigaction函数
所包含库
#include<signal.h>
函数原型
int sigaction(int signum, const struct sigaction *act,struct sigaction *oldact);
参数说明
signum：信号编号

act：新的信号处理配置

oldact：旧的信号处理配置（可为 NULL）

struct sigaction {
   void (*sa_handler)(int); 	//信号处理程序，不接受额外数据，SIG_IGN 为忽略，SIG_DFL 为默认动作
   void (*sa_sigaction)(int, siginfo_t *, void *); 	//信号处理程序，能够接受额外数据和sigqueue配合使用
   sigset_t sa_mask;	//阻塞关键字的信号集，可以再调用捕捉函数之前，把信号添加到信号阻塞字，信号捕捉函数返回之前恢复为原先的值。
   int sa_flags;	//影响信号的行为SA_SIGINFO表示能够接受数据
 };
//回调函数句柄sa_handler、sa_sigaction只能任选其一
函数说明
这个函数的原版帮助信息，可以通过**man sigaction**来查看。

sigaction 是一个系统调用，根据这个函数原型，我们不难看出，在函数原型中，第一个参数signum应该就是注册的信号的编号；第二个参数act如果不为空说明需要对该信号有新的配置；第三个参数oldact如果不为空，那么可以对之前的信号配置进行备份，以方便之后进行恢复。

在这里额外说一下struct sigaction结构体中的 sa_mask 成员，设置在其的信号集中的信号，会在捕捉函数调用前设置为阻塞，并在捕捉函数返回时恢复默认原有设置。这样的目的是，在调用信号处理函数时，就可以阻塞默写信号了。在信号处理函数被调用时，操作系统会建立新的信号阻塞字，包括正在被递送的信号。因此，可以保证在处理一个给定信号时，如果这个种信号再次发生，那么他会被阻塞到对之前一个信号的处理结束为止。

sigaction 的时效性：当对某一个信号设置了指定的动作的时候，那么，直到再次显式调用 sigaction并改变动作之前都会一直有效。

关于结构体中的 flag 属性的详细配置，在此不做详细的说明了，只说明其中一点。如果设置为 SA_SIGINFO 属性时，说明了信号处理程序带有附加信息，也就是会调用 sa_sigaction 这个函数指针所指向的信号处理函数。否则，系统会默认使用 sa_handler 所指向的信号处理函数。在此，还要特别说明一下，sa_sigaction 和 sa_handler 使用的是同一块内存空间，相当于 union，所以只能设置其中的一个，不能两个都同时设置。

关于void (*sa_sigaction)(int, siginfo_t *, void *);处理函数来说还需要有一些说明。void* 是接收到信号所携带的额外数据；而struct siginfo这个结构体主要适用于记录接收信号的一些相关信息。
6.2 信号发送函数：sigqueque
所包含库
#include<signal.h>
函数原型
int sigqueque(pid_t pid, int sig, const union sigval value);
参数说明
pid：接收信号的进程ID

sig：要发送的信号编号

value：一个联合体，用于传递附加数据

union sigval{
	int sival_int;
	void *sival_ptr;
};
函数说明
使用 sigaction 函数安装信号处理程序时，制定了 SA_SIGINFO 的标志。
sigaction 结构体中的 sa_sigaction 成员提供了信号捕捉函数。如果实现的时 sa_handler 成员，那么将无法获取额外携带的数据。
sigqueue 函数只能把信号发送给单个进程，可以使用 value 参数向信号处理程序传递整数值或者指针值。

sigqueue 函数不但可以发送额外的数据，还可以让信号进行排队（操作系统必须实现了 POSIX.1的实时扩展），对于设置了阻塞的信号，使用 sigqueue 发送多个同一信号，在解除阻塞时，接受者会接收到发送的信号队列中的信号，而不是直接收到一次。

但是，信号不能无限的排队，信号排队的最大值受到SIGQUEUE_MAX的限制，达到最大限制后，sigqueue 会失败，errno 会被设置为 EAGAIN。

