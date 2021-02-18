#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    uint ticks;

    if (argc < 2) {
        fprintf(2, "Usage: sleep ticks...\n");
        exit(1);
    }

    ticks = atoi(argv[1]);
    sleep(ticks); 

    exit(0);
}

