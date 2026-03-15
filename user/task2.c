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
        fprintf(2, "failed pipe\n");
        return 1;
    }

    int pid = fork();
    
    if (pid < 0) {
        fprintf(2, "fork exception\n");
        return 1;
    } else if (pid == 0) {
        if (close(pipefd[1])) {
            fprintf(2, "child: failed pipefd[1]-close\n");
            return 1;
        }

        if (close(0)) {
            fprintf(2, "child: failed stdin-close\n");
            return 1;
        }

        if (dup(pipefd[0])) {
            fprintf(2, "child: failed dup\n");
            return 1;
        }

        if (close(pipefd[0])) {
            fprintf(2, "child: failed pipefd[0]-close\n");
            return 1;
        }

        char *exec_argv[] = {"/wc", 0};
        if (exec("/wc", exec_argv)) {
            fprintf(2, "child: failed exec\n");
            return 1;
        }
    } else {
        int arglen, n;
        int total_written, written;
        for (int i = 1; i < argc; ++i) {
            total_written = 0;
            arglen = strlen(argv[i]);
            while (total_written < arglen) {
                written = write(pipefd[1], argv[i] + total_written, arglen - total_written);
                if (written == -1) {
                    fprintf(2, "parent: failed write\n");
                    return 1;        
                } else if (written == 0) {
                    break;
                }
                total_written += written;
            }

            arglen = 1;
            total_written = 0;
            while (total_written < arglen) {
                written = write(pipefd[1], "\n", arglen);
                if (written == -1) {
                    fprintf(2, "parent: failed write\n");
                    return 1;        
                } else if (written == 0) {
                    break;
                }
                total_written += written;
            }
        }

        if (close(pipefd[0])) {
            fprintf(2, "parent: failed pipefd[0]-close\n");
            return 1;
        }

        if (close(pipefd[1])) {
            fprintf(2, "parent: failed pipefd[1]-close\n");
            return 1;
        }

        int ret_code;
        if (wait(&ret_code) < 0) {
            fprintf(2, "parent: failed wait\n");
            return 1;
        }
    }
    return 0;
}