#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    char buf[1024];
    char ea[MAXARG][16];  // exec args
    char* eap[MAXARG];  // exec args pointer
    int eac = 0;  // exec agrs count
    int pid;
    memset(ea, 0, sizeof(ea));
    memset(eap, 0, sizeof(eap));
    for (int i = 0; i < MAXARG - 1; ++i) {
        eap[i] = ea[i];
    }

    if (argc < 2) {
        fprintf(2, "Usage: xargs command\n");
        exit(1);
    }

    // 将argv复制到ea中（除了第一个xargs）
    eac = argc - 1;
    for (int i = 1; i < argc; ++i) {
        strcpy(ea[i-1], argv[i]);
    }

    // 从标准输入读取xargs之前命令的执行结果
    int n = read(0, buf, sizeof(buf));
    // xargs之前命令执行无结果或无意义
    if (n <= 0) {
        exec(ea[0], eap);  // 第二个参数必须是char*数组
        exit(0);
    }

    // 处理标准输入的结果字符串
    for (int i = 0; i < n; ++i) {
        if (buf[i] != ' ' && (i == 0 || buf[i-1] == ' ' || buf[i-1] == '\n')) {
            eac++; // 新的参数 
        }

        if (buf[i] == ' ') continue; // 空格
        if (buf[i] == '\n') {  // 换行符
            if ((pid = fork()) == 0) {
                exec(ea[0], eap);  // 第二个参数必须是char*数组
                exit(0);
            } else {
                wait(0);
            }

            eac = argc - 1;  // 重置eac
            // 重置本次循环迭代中标准输入的参数
            for (int j = argc - 1; j < MAXARG; ++j) {
                memset(ea[j], 0, sizeof(ea[j]));
            }
        } else {
            // 当前参数复制字符
            int eaidx = eac - 1;  // ea index
            int ealen = strlen(ea[eaidx]);  // ea length
            ea[eaidx][ealen] = buf[i];
        }
    }
    exit(0);
}

