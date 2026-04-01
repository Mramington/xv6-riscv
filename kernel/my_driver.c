#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"


uint64 urandom_seed;
struct spinlock urandom_lock;

uint64 nullstat_stat;
struct spinlock nullstat_lock;

static uint8
lcg_next_byte(void)
{
  acquire(&urandom_lock);
  urandom_seed = urandom_seed * 6364136223846793005ULL + 1;
  uint8 x = (urandom_seed >> 24) & 0xFF;
  release(&urandom_lock);
  return x;
}

int
urandom_write(int user_src, uint64 src, int n)
{
    if (n != sizeof(urandom_seed)) return -1;

    uint64 new_seed;
    if(either_copyin(&new_seed, user_src, src, sizeof(urandom_seed)))
        return -1;

    acquire(&urandom_lock);
    urandom_seed = new_seed;
    release(&urandom_lock);

    return n;
}

int
nullstat_write(int user_src, uint64 src, int n)
{
    acquire(&nullstat_lock);
    nullstat_stat += n;
    release(&nullstat_lock);
    return n;
}

int
my_driver_write(int user_src, uint64 src, int n, short minor)
{
    switch (minor)
    {
    case MINOR_NULL:
        return n;
    
    case MINOR_ZERO:
        return -1;
    
    case MINOR_URANDOM:
        return urandom_write(user_src, src, n);
    
    case MINOR_NULLSTAT:
        return nullstat_write(user_src, src, n);
    
    default:
        return -1;
    }
}


int
zero_read(int user_dst, uint64 dst, int n)
{
  int i;
  uchar c = 0;

  for(i = 0; i < n; i++){
    if(either_copyout(user_dst, dst + i, &c, 1) < 0)
      return -1;
  }
  return n;
}

int
urandom_read(int user_dst, uint64 dst, int n)
{
    int i;
    uchar c;

    for (i = 0; i < n; ++i) {
        c = lcg_next_byte();
        if (either_copyout(user_dst, dst + i, &c, 1) < 0)
            return -1;
    }

    return n;
}

int
nullstat_read(int user_dst, uint64 dst, int n)
{
    if (n != sizeof(nullstat_stat)) 
        return -1;
    
    acquire(&nullstat_lock);
    if (either_copyout(user_dst, dst, &nullstat_stat, n) < 0)
        return -1;
    release(&nullstat_lock);

    return n;
}

int
my_driver_read(int user_dst, uint64 dst, int n, short minor)
{

    switch (minor)
    {
    case MINOR_NULL:
        return 0;
    
    case MINOR_ZERO:
        return zero_read(user_dst, dst, n);
    
    case MINOR_URANDOM:
        return urandom_read(user_dst, dst, n);
    
    case MINOR_NULLSTAT:
        return nullstat_read(user_dst, dst, n);
    
    default:
        return -1;
    }
}

void
my_driver_init(void)
{
  devsw[MY_DRIVER].read = my_driver_read;
  devsw[MY_DRIVER].write = my_driver_write;
}
