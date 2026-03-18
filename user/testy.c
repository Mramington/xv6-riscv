#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

void test_count_only(void)
{
  int n = ps_listinfo(0, 0);
  printf("count-only-test returned: %d\n", n);
}

void test_small_buffer(void)
{
  struct procinfo one[1];
  int ret = ps_listinfo(one, 1);
  printf("small-buffer-test returned: %d (expected > 1 or < 0)\n", ret);
}

void test_bad_address(void)
{
  int ret = ps_listinfo((struct procinfo *)1, 4);
  printf("bad-address-test returned: %d (expected negative)\n", ret);
}

void test_ok(void)
{
  int lim = 16;
  struct procinfo *buf;
  int ret;

  while(1) {
    buf = malloc(sizeof(struct procinfo) * lim);
    if(!buf){
      printf("ok-test: malloc failed\n");
      return;
    }

    ret = ps_listinfo(buf, lim);
    if(ret < 0){
      printf("ok-test: ps_listinfo failed: %d\n", ret);
      free(buf);
      return;
    }

    if(ret <= lim)
      break;

    free(buf);
    lim = ret + 1;
  }

  printf("ok-test: got %d processes\n", ret);
  for(int i = 0; i < ret; ++i) {
    printf("pid=%d ppid=%d state=%d name=%s\n",
           buf[i].pid, buf[i].ppid, buf[i].state, buf[i].name);
  }

  free(buf);
}

int main(void)
{
  printf("== tests ==\n");
  test_count_only();
  test_small_buffer();
  test_bad_address();
  test_ok();
  exit(0);
}