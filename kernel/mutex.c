#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "proc.h"
#include "fs.h"
#include "defs.h"
#include "file.h"

struct file*
mutexalloc(void)
{
  struct file *f;
  struct sleeplock *m;

  f = filealloc();
  if(f == 0)
    return 0;

  m = (struct sleeplock*)kalloc();
  if(m == 0){
    fileclose(f);
    return 0;
  }

  initsleeplock(m, "mutex");

  f->type = FD_MUTEX;
  f->readable = 0;
  f->writable = 0;
  f->mutex = m;

  return f;
}

void
mutexclose(struct sleeplock *m)
{
  if(m == 0)
    panic("mutexclose null");

  if(holdingsleep(m))
    panic("mutexclose locked by caller");

  kfree((void*)m);
}