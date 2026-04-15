#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int gvar = 123;
char garr[64];

static void
show_flags(char *name, void *ptr, int len)
{
  int a = pgcheck(ptr, len, PTE_A);
  int d = pgcheck(ptr, len, PTE_D);
  printf("%s: addr=%p len=%d A=%d D=%d\n", name, ptr, len, a, d);
}

int
main(void)
{
  int svar = 10;
  char sarr[128];
  int *heap1;
  char *heaparr;
  int npages = 3;
  int heapsz = npages * 4096;

  memset(sarr, 0, sizeof(sarr));
  memset(garr, 0, sizeof(garr));

  printf("=== start ===\n");
  pgtprint();

  show_flags("gvar", &gvar, sizeof(gvar));
  show_flags("svar", &svar, sizeof(svar));
  show_flags("garr[10]", &garr[10], 1);
  show_flags("sarr[20]", &sarr[20], 1);

  printf("\n=== alloc heap ===\n");
  heap1 = (int*)sbrk(sizeof(int));
  if((uint64)heap1 == (uint64)-1){
    printf("sbrk heap1 failed\n");
    exit(1);
  }

  heaparr = sbrk(heapsz);
  if((uint64)heaparr == (uint64)-1){
    printf("sbrk heaparr failed\n");
    exit(1);
  }

  pgtprint();

  show_flags("heap1", heap1, sizeof(int));
  show_flags("heaparr[0]", &heaparr[0], 1);
  show_flags("heaparr[4096]", &heaparr[4096], 1);
  show_flags("heaparr[heapsz - 1]", &heaparr[heapsz - 1], 1);

  printf("\n=== clear A, D ===\n");
  if(pgclear(&gvar, sizeof(gvar), PTE_A | PTE_D) < 0) printf("pgclear gvar failed\n");
  if(pgclear(&svar, sizeof(svar), PTE_A | PTE_D) < 0) printf("pgclear svar failed\n");
  if(pgclear(&garr[10], 1, PTE_A | PTE_D) < 0) printf("pgclear garr failed\n");
  if(pgclear(&sarr[20], 1, PTE_A | PTE_D) < 0) printf("pgclear sarr failed\n");
  if(pgclear(heap1, sizeof(int), PTE_A | PTE_D) < 0) printf("pgclear heap1 failed\n");
  if(pgclear(heaparr, heapsz, PTE_A | PTE_D) < 0) printf("pgclear heaparr failed\n");

  pgtprint();

  show_flags("gvar", &gvar, sizeof(gvar));
  show_flags("svar", &svar, sizeof(svar));
  show_flags("heaparr", heaparr, heapsz);

  printf("\n=== read data ===\n");
  volatile int x = 0;
  x += gvar;
  x += svar;
  x += garr[10];
  x += sarr[20];
  x += *heap1;
  x += heaparr[0];
  x += heaparr[4096];
  x += heaparr[heapsz - 1];
  printf("x=%d\n", x);

  pgtprint();

  show_flags("gvar", &gvar, sizeof(gvar));
  show_flags("svar", &svar, sizeof(svar));
  show_flags("heaparr", heaparr, heapsz);

  printf("\n=== write data ===\n");
  gvar = 1;
  svar = 2;
  garr[10] = 3;
  sarr[20] = 4;
  *heap1 = 5;
  heaparr[0] = 6;
  heaparr[4096] = 7;
  heaparr[heapsz - 1] = 8;

  pgtprint();

  show_flags("gvar", &gvar, sizeof(gvar));
  show_flags("svar", &svar, sizeof(svar));
  show_flags("heaparr", heaparr, heapsz);

  printf("\n=== free heap ===\n");
  if(sbrk(-(sizeof(int) + heapsz)) == (char*)-1){
    printf("sbrk free failed\n");
    exit(1);
  }

  pgtprint();

  exit(0);
}