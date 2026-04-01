#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

static int
hexval(char c)
{
  if(c >= '0' && c <= '9') return c - '0';
  if(c >= 'a' && c <= 'f') return c - 'a' + 10;
  if(c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

int
main(int argc, char *argv[])
{
  int fd, len, i, n, r;
  char *s;
  uchar buf[256];
  int hi, lo;

  if(argc != 3){
    fprintf(2, "fail: hexwrite must have argc=3\n");
    exit(1);
  }

  s = argv[1];
  len = strlen(s);
  if(len % 2 != 0 || len / 2 > sizeof(buf)){
    fprintf(2, "fail: bad hex\n");
    exit(1);
  }

  n = len / 2;
  for(i = 0; i < n; i++){
    hi = hexval(s[2*i]);
    lo = hexval(s[2*i + 1]);
    if(hi < 0 || lo < 0){
      fprintf(2, "fail: bad hex\n");
      exit(1);
    }
    buf[i] = (hi << 4) | lo;
  }

  fd = open(argv[2], O_WRONLY);
  if(fd < 0){
    fprintf(2, "fail: open error\n");
    exit(1);
  }

  r = write(fd, buf, n);
  if(r != n){
    fprintf(2, "Write error\n");
    close(fd);
    exit(1);
  }

  close(fd);
  exit(0);
}