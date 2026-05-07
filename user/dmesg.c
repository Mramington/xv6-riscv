#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSZ (DMSG_NPAGE * 4096)

int
main(int argc, char *argv[])
{
  char *buf = malloc(BUFSZ + 1);

  if (buf == 0) {
    fprintf(2, "dmesg: malloc failed\n");
    exit(1);
  }

  int n = dmesg(buf, BUFSZ + 1);

  if (n < 0) {
    fprintf(2, "dmesg: syscall failed\n");
    free(buf);
    exit(1);
  }

  printf("%s", buf);

  free(buf);
  exit(0);
}