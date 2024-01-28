#define _GNU_SOURCE
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main()
{
    int fd = memfd_create("memfd01", 0);
    if (fd == -1) {
        perror("memfd_create");
    }

    pid_t pid = fork();
    long offset = 0xffffaaff;
    switch (pid) {
    case -1: {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    case 0: {  // Child
        lseek(fd, offset, 0);
        printf("writing something to memfd\n");
        write(fd, "hello memfd\0", 12);

        printf("child exiting\n");
        exit(EXIT_SUCCESS);
    }

    default: {  // Parent
        printf("wating for child (pid=%d) to exit\n", pid);
        wait(NULL);

        char buf[32];
        buf[sizeof(buf)-1] = '\0';
        lseek(fd, offset, 0);
        read(fd, buf, sizeof(buf)-1);
        printf("read from memfd: %s\n", buf);

        printf("parent exiting\n");
        exit(EXIT_SUCCESS);
    }
    }
}