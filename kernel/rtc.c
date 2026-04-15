#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "spinlock.h"
#include "defs.h"

#define RTCReadMinor() (*((volatile uint32*)RTC_TIME_MINOR))
#define RTCReadMajor() (*((volatile uint32*)RTC_TIME_MAJOR))

static struct spinlock rtc_lock;

void
init_rtc_lock()
{
    initlock(&rtc_lock, "rtc_lock");
}

uint64
rtc_read_time()
{
    acquire(&rtc_lock);

    uint32 minor = RTCReadMinor();
    uint32 major = RTCReadMajor();

    release(&rtc_lock);

    uint64 time = ((uint64)major << 32) | minor;

    return time;
}