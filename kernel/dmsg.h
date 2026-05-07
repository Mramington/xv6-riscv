#ifndef DMSG_H
#define DMSG_H

#include "types.h"

void dmsginit(void);
void dmsg_putc(char c);
void pr_msg(const char *fmt, ...);
int dmsg_read(uint64 user_dst, int max);
int log_enabled(int cls);
void log_set(int mask, int duration);

#endif
