#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
print_args_unsync(int argc, char **argv)
{
  for(int i = 1; i < argc; i++){
    char *s = argv[i];
    for(int j = 0; s[j]; j++){
      printf("%d: arg %d, char '%c'\n", getpid(), i, s[j]);
      pause(10);
    }
  }
}

static void
print_args_sync(int mfd, int argc, char **argv)
{
  for(int i = 1; i < argc; i++){
    char *s = argv[i];
    for(int j = 0; s[j]; j++){
      if(mutex_lock(mfd) < 0){
        printf("lock error\n");
        exit(1);
      }
      printf("%d: arg %d, char '%c'\n", getpid(), i, s[j]);
      if(mutex_unlock(mfd) < 0){
        printf("unlock error\n");
        exit(1);
      }
      pause(10);
    }
  }
}

int
main(int argc, char **argv)
{
  int pid, mfd;

  printf("=== unsynchronized ===\n");
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }
  if(pid == 0){
    print_args_unsync(argc, argv);
    exit(0);
  } else {
    print_args_unsync(argc, argv);
    wait(0);
  }

  printf("=== synchronized ===\n");
  mfd = mutex();
  if(mfd < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(mfd);
    exit(1);
  }

  if(pid == 0){
    print_args_sync(mfd, argc, argv);
    close(mfd);
    exit(0);
  } else {
    print_args_sync(mfd, argc, argv);
    wait(0);
    close(mfd);
  }

  exit(0);
}