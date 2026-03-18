#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

static char *
state2str(int st)
{
  switch(st){
  case 0: return "UNUSED";
  case 1: return "USED";
  case 2: return "SLEEP";
  case 3: return "RUNNABLE";
  case 4: return "RUNNING";
  case 5: return "ZOMBIE";
  default: return "?";
  }
}

static char * get_pname(struct procinfo *list, int n, int ppid)
{
  int i;
  if(ppid < 0)
    return "-";

  for(i = 0; i < n; ++i) {
    if(list[i].pid == ppid)
      return list[i].name;
  }
  return "?";
}

int main(void)
{
  int n, lim;
  struct procinfo *list;

  n = ps_listinfo(0, 0);
  if(n < 0) {
    fprintf(2, "ps: ps_listinfo(NULL) failed\n");
    exit(1);
  }

  lim = n + 1;
  while(1) {
    list = malloc(sizeof(struct procinfo) * lim);
    if(!list){
      fprintf(2, "ps: malloc failed\n");
      exit(1);
    }

    n = ps_listinfo(list, lim);
    if(n < 0){
      fprintf(2, "ps: ps_listinfo failed\n");
      free(list);
      exit(1);
    }

    if(n <= lim)
      break;

    free(list);
    lim = n + 1;
  }

  printf("PID\tNAME\t\tSTATE\t\tPPID\tPNAME\n");
  for(int i = 0; i < n; ++i) {
    printf("%d\t%s\t\t%s\t\t%d\t%s\n",
           list[i].pid,
           list[i].name,
           state2str(list[i].state),
           list[i].ppid,
           get_pname(list, n, list[i].ppid));
  }

  free(list);
  exit(0);
}