#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* does the same as the shell command: ls /usr/bin | grep x | wc -l */
static void close_all(int a[2], int b[2])
{
    close(a[0]); close(a[1]);
    close(b[0]); close(b[1]);
}

int main(void)
{
    int p1[2], p2[2];                   /* ls -> grep, grep -> wc */
    if (pipe(p1) < 0 || pipe(p2) < 0) { perror("pipe"); return 1; }

    if (fork() == 0) {                  /* ls: 1 -> p1[1] */
        dup2(p1[1], 1);
        close_all(p1, p2);
        execlp("ls", "ls", "/usr/bin", (char *)NULL);
        perror("exec ls");
        exit(127);
    }
    if (fork() == 0) {                  /* grep: 0 <- p1[0], 1 -> p2[1] */
        dup2(p1[0], 0);
        dup2(p2[1], 1);
        close_all(p1, p2);
        execlp("grep", "grep", "x", (char *)NULL);
        perror("exec grep");
        exit(127);
    }
    if (fork() == 0) {                  /* wc: 0 <- p2[0] */
        dup2(p2[0], 0);
        close_all(p1, p2);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("exec wc");
        exit(127);
    }
    close_all(p1, p2);                  /* the parent uses no end */
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
