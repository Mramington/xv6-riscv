#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "usage: logctl [off|all|syscall|intr|proc|exec] [ticks]\n");
    exit(1);
  }

  int mask = 0;
  int duration = 0;

  if (strcmp(argv[1], "off") == 0) {
    mask = 0;
  } else if (strcmp(argv[1], "all") == 0) {
    mask = LOG_ALL;
  } else if (strcmp(argv[1], "syscall") == 0) {
    mask = LOG_SYSCALL;
  } else if (strcmp(argv[1], "intr") == 0) {
    mask = LOG_INTR;
  } else if (strcmp(argv[1], "proc") == 0) {
    mask = LOG_PROC;
  } else if (strcmp(argv[1], "exec") == 0) {
    mask = LOG_EXEC;
  } else {
    fprintf(2, "logctl: unknown class: %s\n", argv[1]);
    exit(1);
  }

  if (argc >= 3)
    duration = atoi(argv[2]);

  if (logctl(mask, duration) < 0) {
    fprintf(2, "logctl: syscall failed\n");
    exit(1);
  }

  exit(0);
}