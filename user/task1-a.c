#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int main(void) {
    int pid = fork();
    
    if (pid < 0) {
        printf("fork exception\n");
        return 1;
    } else if (pid == 0) {
        pause(50);
    } else {
        printf("parent pid: %d\n", getpid());
        printf("child pid: %d\n", pid);

        int ret_code;
        int finished_pid = wait(&ret_code);

        printf("finished pid: %d\n", finished_pid);
        printf("return code: %d\n", ret_code);
    }
    return 0;
}