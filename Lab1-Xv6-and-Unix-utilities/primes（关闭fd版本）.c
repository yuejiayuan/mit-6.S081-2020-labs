#include "kernel/types.h"
#include "user/user.h"

static const int START_NUM = 2;
static const int END_NUM = 100;

void primes(int*);

int main(int argc, char* argv[]) {
    int fd[2];  // pipe fds
    int pid;

    // 主进程创建管道
    if (pipe(fd) < 0) {
        printf("error: main pipe() failed\n");
        exit(1);
    }

    if ((pid = fork()) == 0) { // 子进程
        primes(fd);
    } else {                   // 主进程
        close(fd[0]); // 关闭管道的读fd（主进程不需要用）

        // 向管道依次写入2~35
        for (int i = START_NUM; i <= END_NUM; ++i) {
            write(fd[1], (void*)&i, sizeof(i));
        }
        close(fd[1]);
        wait(0);
    }

    exit(0);
}

//
// 参数 
//   lpfd: left neighbor pipe fd
//
void primes(int* lpfd) {
    int p, n; // p: base number, n: numbers to be sieved
    int rpfd[2]; // right neighbor pipe fds
    int pid;
    int rdrv; // read return value

    close(lpfd[1]); // 关闭父进程中管道的写fd（当前进程不需要用）

    rdrv = read(lpfd[0], (void*)&p, sizeof(p));
    if (rdrv <= 0) {
        exit(0);
    }
    printf("prime %d\n", p);

    if (pipe(rpfd) < 0) {
        printf("error: sub pipe() failed\n");
        exit(1);
    }

    if ((pid = fork()) == 0) { // 子进程
        primes(rpfd);
    } else {                   // 当前进程
        close(rpfd[0]); // 关闭当前进程创建管道的读fd（当前进程不需要用）
        while ((rdrv = read(lpfd[0], (void*)&n, sizeof(n))) > 0) {
            if (n % p != 0) {
                write(rpfd[1], (void*)&n, sizeof(n));
            }
        }
        close(lpfd[0]);
        close(rpfd[1]);
        wait(0);
    }

    exit(0);
}

