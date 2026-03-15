#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const int PGSIZE = 4096;
const int PG_COUNT = 32;

int main(int argc, char**argv) {
    int pipefd[2];
    
    if (pipe(pipefd)) {
        fprintf(stderr, "failed pipe\n");
        return 1;
    }

    int pid = fork();
    
    if (pid < 0) {
        fprintf(stderr, "fork exception\n");
        return 1;
    } else if (pid == 0) {
        if (close(pipefd[1])) {
            fprintf(stderr, "child: failed pipefd[1]-close\n");
            return 1;
        }

        char* buf = malloc(PG_COUNT * PGSIZE);
        if (!buf) {
            fprintf(stderr, "child: failed malloc]\n");
            return 1;
        }

        int n, total_written, written;
        n = read(pipefd[0], buf, PGSIZE);
        while(n != 0) {
            if (n < 0) {
                fprintf(stderr, "child: failed read\n");
                return 1;
            }

            total_written = 0;
            while (total_written < n) {
                written = write(1, buf + total_written, n - total_written);
                if (written < 0) {
                    fprintf(stderr, "child: failed write\n");
                    return 1;
                } else if (written == 0) {
                    break;
                }

                total_written += written;
            }

            n = read(pipefd[0], buf, PGSIZE);
        }
        
        free(buf);

        if (close(pipefd[0])) {
            fprintf(stderr, "child: failed pipefd[0]-close\n");
            return 1;
        }
    } else {
        int arglen;
        int written, total_written;
        for (int i = 0; i < argc; ++i) {
            total_written = 0;
            arglen = strlen(argv[i]) + 1;
            while (total_written < arglen) {
                written = write(pipefd[1], argv[i] + total_written, arglen - total_written);
                if (written == -1) {
                    fprintf(stderr, "parent: failed write\n");
                    return 1;
                } else if (written == 0) {
                    break;
                }

                total_written += written;
            }
        }

        if (close(pipefd[0])) {
            fprintf(stderr, "parent: failed pipefd[0]-close\n");
            return 1;
        }

        if (close(pipefd[1])) {
            fprintf(stderr, "parent: failed pipefd[1]-close\n");
            return 1;
        }

        int ret_code;
        if (wait(&ret_code) < 0) {
            fprintf(stderr, "parent: failed wait\n");
            return 1;
        }
    }
    return 0;
}