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

        char* buf = malloc(PG_COUNT * PGSIZE);
        if (!buf) {
            printf("child: failed malloc]\n");
            return 1;
        }

        int n, pos = 0;
        int flag = 0;
        while(buf[pos] != '\n') {
            pos = 0;
            while ((n = read(pipefd[0], buf + pos, 1))) {
                if (n < 0) {
                    printf("child: failed read\n");
                    return 1;
                }

                if (buf[pos] == '\0') {
                    break;
                } else if (buf[pos] == '\n') {
                    flag = 1;
                    break;
                }

                ++pos;
            }

            if (!flag && buf[pos] != '\0') {
                printf("child: failed read - EOF before null\n");
                return 1;
            }

            if ((n = write(1, buf, pos + 1)) < 0) {
                printf("child: failed write\n");
                return 1;
            } else if (n < pos + 1) {
                printf("child: write output less than pos + 1\n");
                return 1;
            }
        }
        
        free(buf);

        if (close(pipefd[0])) {
            printf("child: failed pipefd[0]-close\n");
            return 1;
        }
    } else {
        int arglen;
        int n;
        for (int i = 0; i < argc; ++i) {
            arglen = strlen(argv[i]);
            if ((n = write(pipefd[1], argv[i], arglen + 1)) < 0) {
                printf("parent: failed write\n");
                return 1;
            } else if (n < arglen + 1) {
                printf("parent: write output less than arglen + 1\n");
                return 1;
            }
        }
        
        if (write(pipefd[1], "\n", 1) < 0) {
            printf("parent: failed write");
            return 1;
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