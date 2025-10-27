#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void analyze_status(int status, const char *process_name) {
    printf("\n=== 分析 %s 的状态 ===\n", process_name);
    
    if (WIFEXITED(status)) {
        printf("✓ 正常退出\n");
        printf("  退出状态码: %d\n", WEXITSTATUS(status));
    }
    
    if (WIFSIGNALED(status)) {
        printf("✗ 被信号终止\n");
        int sig = WTERMSIG(status);
        printf("  信号编号: %d (%s)\n", sig, strsignal(sig));
        
        // 检查是否产生核心转储
        #ifdef WCOREDUMP
        if (WCOREDUMP(status)) {
            printf("  生成了核心转储文件\n");
        }
        #endif
    }
    
    if (WIFSTOPPED(status)) {
        printf("⏸️ 被信号停止\n");
        printf("  停止信号: %d (%s)\n", WSTOPSIG(status), 
               strsignal(WSTOPSIG(status)));
    }
    
    if (WIFCONTINUED(status)) {
        printf("▶️ 从停止状态继续执行\n");
    }
    
    printf("原始状态值: 0x%08x\n", status);
}

int main() {
    printf("测试不同的进程退出情况:\n");
    
    // 测试1：正常退出
    pid_t pid1 = fork();
    if (pid1 == 0) {
        exit(123);  // 正常退出，状态码123
    } else {
        int status;
        wait(&status);
        analyze_status(status, "正常退出进程");
    }
    
    // 测试2：被信号终止
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // 制造段错误
        int *p = NULL;
        *p = 42;
    } else {
        int status;
        wait(&status);
        analyze_status(status, "段错误进程");
    }
    
    // 测试3：使用 waitpid 和 WNOHANG
    pid_t pid3 = fork();
    if (pid3 == 0) {
        sleep(3);  // 子进程睡眠3秒
        exit(0);
    } else {
        printf("\n测试非阻塞等待:\n");
        int status;
        pid_t result;
        
        while ((result = waitpid(pid3, &status, WNOHANG)) == 0) {
            printf("子进程还在运行，父进程可以做其他工作...\n");
            sleep(1);
        }
        
        if (result == pid3) {
            analyze_status(status, "非阻塞等待的进程");
        }
    }
    
    return 0;
}
