#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int
is_leap(int year)
{
  if(year % 400 == 0)
    return 1;
  if(year % 100 == 0)
    return 0;
  return (year % 4 == 0);
}

static int
days_in_year(int year)
{
  return is_leap(year) ? 366 : 365;
}

static int
days_in_month(int year, int month)
{
  static int mdays[12] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
  };

  if(month == 2 && is_leap(year))
    return 29;
  return mdays[month - 1];
}

int
main(int argc, char *argv[])
{
  uint64 ns = rtc_time();
  uint64 sec = ns / 1000000000ULL;
  uint64 nsec = ns % 1000000000ULL;

  uint64 days = sec / 86400ULL;
  uint64 rem = sec % 86400ULL;

  int hour = rem / 3600;
  rem %= 3600;
  int min = rem / 60;
  int s = rem % 60;

  int year = 1970;
  while(days >= (uint64)days_in_year(year)){
    days -= days_in_year(year);
    year++;
  }

  int month = 1;
  while(days >= (uint64)days_in_month(year, month)){
    days -= days_in_month(year, month);
    month++;
  }

  int day = days + 1;

//   printf("%04d-%02d-%02d %02d:%02d:%02d.%09d\n",
//          year, month, day, hour, min, s, (int)nsec);
  printf("%d-%d-%d %d:%d:%d.%d\n",
    year, month, day, hour, min, s, (int)nsec);
  exit(0);
}