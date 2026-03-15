#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

int main(int argc, char**argv) {
    int pipefd[2];
    
    if (pipe(pipefd)) {
        printf("failed pipe\n");
        return 1;
    }

    int pid = fork();
    
    if (pid < 0) {
        printf("fork exception\n");
        return 1;
    } else if (pid == 0) {
        if (close(pipefd[1])) {
            printf("child: failed pipefd[1]-close\n");
            return 1;
        }

        if (close(0)) {
            printf("child: failed stdin-close\n");
            return 1;
        }

        if (dup(pipefd[0])) {
            printf("child: failed dup\n");
            return 1;
        }

        if (close(pipefd[0])) {
            printf("child: failed pipefd[0]-close\n");
            return 1;
        }

        char *exec_argv[] = {"/wc", 0};
        if (exec("/wc", exec_argv)) {
            printf("child: failed exec\n");
            return 1;
        }
    } else {
        int arglen, n;
        for (int i = 1; i < argc; ++i) {
            arglen = strlen(argv[i]);
            if ((n = write(pipefd[1], argv[i], arglen)) < 0) {
                printf("parent: failed write\n");
                return 1;
            } else if (n < arglen) {
                printf("parent: write output less than arglen\n");
                return 1;
            }

            if ((n = write(pipefd[1], "\n", 1)) < 1) {
                printf("parent: failed write\n");
                return 1;
            } else if (n < 1) {
                printf("parent: write output less than 1\n");
                return 1;
            }
        }

        if (close(pipefd[0])) {
            printf("parent: failed pipefd[0]-close\n");
            return 1;
        }

        if (close(pipefd[1])) {
            printf("parent: failed pipefd[1]-close\n");
            return 1;
        }

        int ret_code;
        if (wait(&ret_code) < 0) {
            printf("parent: failed wait\n");
            return 1;
        }
    }
    return 0;
}