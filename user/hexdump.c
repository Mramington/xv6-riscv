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
  int fd, n, i, r, got;
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

  got = 0;
  while(got < n){
    r = read(fd, buf + got, n - got);
    if(r < 0){
      fprintf(2, "fail: read error\n");
      close(fd);
      exit(1);
    }
    if(r == 0)
      break;
    got += r;
  }

  for(i = 0; i < got; i++){
    if(i) printf(" ");
    print_hex_byte(buf[i]);
  }
  printf("\n");

  close(fd);
  exit(0);
}