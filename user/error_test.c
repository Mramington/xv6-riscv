#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int mfd = mutex();
  char buf[4];
  int pid;

  if(mfd < 0){
    printf("mutex create failed\n");
    exit(1);
  }

  if(read(mfd, buf, sizeof(buf)) >= 0)
    printf("FAIL: read on mutex should fail\n");
  else
    printf("OK: read on mutex failed\n");

  if(write(mfd, "x", 1) >= 0)
    printf("FAIL: write on mutex should fail\n");
  else
    printf("OK: write on mutex failed\n");

  if(mutex_lock(mfd) < 0){
    printf("FAIL: lock failed\n");
    close(mfd);
    exit(1);
  }
  if(close(mfd) < 0)
    printf("FAIL: close locked mutex by owner failed\n");
  else
    printf("OK: close locked mutex by owner\n");

  mfd = mutex();
  if(mfd < 0){
    printf("mutex recreate failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(mfd);
    exit(1);
  }

  if(pid == 0){
    pause(10);
    if(mutex_unlock(mfd) >= 0)
      printf("FAIL: child unlocked another mutex\n");
    else
      printf("OK: child cannot unlock another mutex\n");

    if(close(mfd) < 0)
      printf("FAIL: child close failed\n");
    else
      printf("OK: child closed fd to foreign-locked mutex\n");

    exit(0);
  } else {
    if(mutex_lock(mfd) < 0){
      printf("FAIL: parent lock failed\n");
      close(mfd);
      wait(0);
      exit(1);
    }

    wait(0);

    if(mutex_unlock(mfd) < 0)
      printf("FAIL: parent unlock failed\n");
    else
      printf("OK: parent unlock after child actions\n");

    close(mfd);
  }

  mfd = mutex();
  if(mfd < 0){
    printf("mutex recreate failed 2\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    close(mfd);
    exit(1);
  }

  if(pid == 0){
    if(mutex_lock(mfd) < 0){
      printf("FAIL: child lock failed\n");
      exit(1);
    }
    printf("child exits with locked mutex and open fd\n");
    exit(0);
  } else {
    wait(0);
    if(mutex_lock(mfd) < 0)
      printf("FAIL: parent could not lock after child exit\n");
    else{
      printf("OK: mutex usable after child exit\n");
      mutex_unlock(mfd);
    }
    close(mfd);
  }

  exit(0);
}