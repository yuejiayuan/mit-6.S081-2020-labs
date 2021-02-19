#include "kernel/types.h"
#include "user/user.h"

static const int START_NUM = 2;
static const int END_NUM = 100;
// 将0作为各进程的终止标志（0既不是质数也不是合数）
static const int TERMINATE_FLAG = 0; 

void primes();

int main(int argc, char* argv[]) {
    int fd[2];  // pipe fds
    int pid;

    // 主进程创建管道
    if (pipe(fd) < 0) {
        printf("error: main pipe() failed\n");
        exit(1);
    }

    // 将标准输出重定向到fd 2（2原本是标准错误输出）
    close(2);
    dup(1); 
    // 将pipe管道的读写分别重定向到fd 1和2
    close(0);
    close(1);
    dup(fd[0]);
    dup(fd[1]);
    close(fd[0]);
    close(fd[1]);

    if ((pid = fork()) == 0) { // 子进程
        primes();
    } else {                   // 主进程
        // 向管道依次写入数字
        for (int i = START_NUM; i <= END_NUM; ++i) {
            write(1, (void*)&i, sizeof(i));
        }

        write(1, (void*)&TERMINATE_FLAG, sizeof(TERMINATE_FLAG));
        wait(0);
    }

    exit(0);
}

void primes() { 
    int p, n; // p: base number, n: numbers to be sieved
    int fd[2]; // pipe fds
    int pid;
    int rdrv; // read return value

    rdrv = read(0, (void*)&p, sizeof(p));
    if (rdrv <= 0) {
        exit(0);
    }
    if (p == TERMINATE_FLAG) exit(0);
    fprintf(2, "prime %d\n", p);

    if (pipe(fd) < 0) {
        fprintf(2, "error: sub pipe() failed\n");
        exit(1);
    }
    // 将新管道的写重定向到当前进程的fd 1
    close(1);
    dup(fd[1]);
    close(fd[1]);

    if ((pid = fork()) == 0) { // 当前进程的子进程
        // 将新管道的读重定向到子进程的fd 0
        close(0);
        dup(fd[0]);
        close(fd[0]);
        primes();
    } else {                   // 当前进程
        while ((rdrv = read(0, (void*)&n, sizeof(n))) > 0) {
            if (n % p != 0 || n == TERMINATE_FLAG) {
                write(1, (void*)&n, sizeof(n));
                if (n == TERMINATE_FLAG) break;
            }
        }
        wait(0);
    }

    exit(0);
}

