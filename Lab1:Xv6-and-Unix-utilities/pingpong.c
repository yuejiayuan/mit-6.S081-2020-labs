#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int fd[2];
    char read_buf[4];
    int n;

    if (pipe(fd) < 0) {
        printf("pipe() failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid == 0) {
        n = read(fd[0], read_buf, 4);
        if (n > 0) {
            printf("%d: received %s\n", getpid(), read_buf);
        }

        n = write(fd[1], "pong", 4);
    } else {
        n = write(fd[1], "ping", 4);
        wait(0);

        n = read(fd[0], read_buf, 4);
        if (n > 0) {
            printf("%d: received %s\n", getpid(), read_buf);
        }
    }
    exit(0);
}

