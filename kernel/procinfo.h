#ifndef _PROCINFO_H_
#define _PROCINFO_H_

#define NAME_LEN 16

struct procinfo {
  int pid;
  int ppid;
  int state;
  char name[NAME_LEN];
};

#endif