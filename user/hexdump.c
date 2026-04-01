#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

static void
print_hex_byte(uchar b)
{
  char *hex = "0123456789ABCDEF";
  printf("%c%c", hex[(b >> 4) & 0xF], hex[b & 0xF]);
}

int
main(int argc, char *argv[])
{
  int fd, n, i, r;
  uchar buf[256];

  if(argc != 3){
    fprintf(2, "fail: hexdump nust have argc=3\n");
    exit(1);
  }

  n = atoi(argv[1]);
  if(n < 0 || n > sizeof(buf)){
    fprintf(2, "fail: bad n\n");
    exit(1);
  }

  fd = open(argv[2], O_RDONLY);
  if(fd < 0){
    fprintf(2, "fail: open error\n");
    exit(1);
  }

  r = read(fd, buf, n);
  if(r < 0){
    fprintf(2, "fail: read error\n");
    close(fd);
    exit(1);
  }

  for (i = 0; i < r; ++i) {
    if(i) printf(" ");
    print_hex_byte(buf[i]);
  }
  printf("\n");

  close(fd);
  exit(0);
}