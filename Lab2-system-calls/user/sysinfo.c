#include "kernel/types.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int main(int argc, char* argv[]) 
{
  struct sysinfo info;
  
  if (sysinfo(&info) < 0) {
    printf("FAIL: sysinfo failed\n");
    exit(1);
  }

  printf("freemem: %d, nporc: %d\n"
      , info.freemem, info.nproc);
  exit(0);
}
