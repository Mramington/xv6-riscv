#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "stdarg.h"

struct dmsg_buffer {
  struct spinlock lock;
  char buf[DMSG_BUFSIZE];
  uint head;
  uint tail;
  int full;
};

struct dmsg_buffer dmsg;
struct spinlock loglock;
int log_mask;
uint log_until;

void
dmsginit(void)
{
  initlock(&dmsg.lock, "dmsg");
  initlock(&loglock, "logctl");

  acquire(&dmsg.lock);

  dmsg.head = 0;
  dmsg.tail = 0;
  dmsg.full = 0;

  dmsg.buf[0] = '\n';
  dmsg.tail = 1;

  release(&dmsg.lock);

  acquire(&loglock);
  log_mask = 0;
  log_until = 0;
  release(&loglock);
}

void
dmsg_putc_locked(char c)
{
  dmsg.buf[dmsg.tail] = c;
  dmsg.tail = (dmsg.tail + 1) % DMSG_BUFSIZE;

  if (dmsg.full) {
    dmsg.head = (dmsg.head + 1) % DMSG_BUFSIZE;
  }

  if (dmsg.tail == dmsg.head) {
    dmsg.full = 1;
  }
}

void
dmsg_putc(char c)
{
  acquire(&dmsg.lock);
  dmsg_putc_locked(c);
  release(&dmsg.lock);
}

char digits[] = "0123456789abcdef";

void
dmsg_printint(long long x, int base, int sign)
{
  char buf[32];
  int i;
  unsigned long long y;

  if (sign && x < 0) {
    y = -x;
    dmsg_putc_locked('-');
  } else y = x;

  i = 0;
  do {
    buf[i++] = digits[y % base];
    y /= base;
  } while (y != 0);

  while (--i >= 0) dmsg_putc_locked(buf[i]);
}

void
dmsg_printptr(uint64 x)
{
  dmsg_putc_locked('0');
  dmsg_putc_locked('x');

  for (int i = 0; i < 16; i++, x <<= 4) {
    dmsg_putc_locked(digits[x >> 60]);
  }
}

void
dmsg_vprintf(const char *fmt, va_list ap)
{
  char *s;
  int c;

  for (int i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      dmsg_putc_locked(c);
      continue;
    }

    c = fmt[++i] & 0xff;

    if (c == 0)
      break;

    switch (c) {
    case 'd':
      dmsg_printint(va_arg(ap, int), 10, 1);
      break;

    case 'x':
      dmsg_printint(va_arg(ap, int), 16, 0);
      break;

    case 'p':
      dmsg_printptr(va_arg(ap, uint64));
      break;

    case 's':
      s = va_arg(ap, char *);
      if (s == 0)
        s = "(null)";
      while (*s)
        dmsg_putc_locked(*s++);
      break;

    case 'c':
      dmsg_putc_locked(va_arg(ap, int));
      break;

    case '%':
      dmsg_putc_locked('%');
      break;

    default:
      dmsg_putc_locked('%');
      dmsg_putc_locked(c);
      break;
    }
  }
}

void
pr_msg(const char *fmt, ...)
{
  va_list ap;
  uint t;

  acquire(&tickslock);
  t = ticks;
  release(&tickslock);

  acquire(&dmsg.lock);

  dmsg_putc_locked('[');
  dmsg_printint(t, 10, 0);
  dmsg_putc_locked(']');
  dmsg_putc_locked(' ');

  va_start(ap, fmt);
  dmsg_vprintf(fmt, ap);
  va_end(ap);

  dmsg_putc_locked('\n');

  release(&dmsg.lock);
}

int
dmsg_read(uint64 user_dst, int max)
{
  struct proc *p = myproc();

  if (max <= 0)
    return -1;

  acquire(&dmsg.lock);

  int n;
  if (dmsg.full)
    n = DMSG_BUFSIZE;
  else if (dmsg.tail >= dmsg.head)
    n = dmsg.tail - dmsg.head;
  else
    n = DMSG_BUFSIZE - dmsg.head + dmsg.tail;

  if (n > max - 1)
    n = max - 1;

  int pos = dmsg.head;

  if (n > 0 && dmsg.buf[pos] != '\n') {
    while (n > 0 && dmsg.buf[pos] != '\n') {
      pos = (pos + 1) % DMSG_BUFSIZE;
      n--;
    }

    if (n > 0 && dmsg.buf[pos] == '\n') {
      pos = (pos + 1) % DMSG_BUFSIZE;
      n--;
    }
  } else if (n > 0 && dmsg.buf[pos] == '\n') {
    pos = (pos + 1) % DMSG_BUFSIZE;
    n--;
  }

  for (int i = 0; i < n; i++) {
    char c = dmsg.buf[pos];

    if (copyout(p->pagetable, user_dst + i, &c, 1) < 0) {
      release(&dmsg.lock);
      return -1;
    }

    pos = (pos + 1) % DMSG_BUFSIZE;
  }

  char zero = '\0';
  if (copyout(p->pagetable, user_dst + n, &zero, 1) < 0) {
    release(&dmsg.lock);
    return -1;
  }

  release(&dmsg.lock);
  return n;
}

int
log_enabled(int cls)
{
  uint t;
  int enabled;

  acquire(&tickslock);
  t = ticks;
  release(&tickslock);

  acquire(&loglock);

  if (log_until != 0 && t >= log_until) {
    log_mask = 0;
    log_until = 0;
  }

  enabled = (log_mask & cls) != 0;

  release(&loglock);

  return enabled;
}

void
log_set(int mask, int duration)
{
  uint t;

  acquire(&tickslock);
  t = ticks;
  release(&tickslock);

  acquire(&loglock);

  log_mask = mask;

  if (duration > 0)
    log_until = t + duration;
  else
    log_until = 0;

  release(&loglock);

  pr_msg("log mask set to %d, duration = %d", mask, duration);
}