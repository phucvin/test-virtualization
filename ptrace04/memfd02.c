#define _GNU_SOURCE
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main()
{
    int fd = memfd_create("memfd01", 0);
    if (fd == -1) {
        perror("memfd_create");
    }
    // So mmap won't fail because accessing beyond the end of the file
    ftruncate(fd, 0xffffffff);

    pid_t pid = fork();
    long offset = 0xaaffa000;
    switch (pid) {
    case -1: {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    case 0: {  // Child
        for (int i = 1; i < 4; ++i) {
            void* mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
            if (mem == MAP_FAILED) {
                perror("mmap");
                exit(EXIT_FAILURE);
            }
            printf("writing something to mem (addr=%p)\n", mem);
            char text[] = "hello memfd and mmap 0\0";
            text[sizeof(text)-3] = '0' + i;
            memcpy(mem, text, sizeof(text));
        }

        printf("child exiting\n");
        exit(EXIT_SUCCESS);
    }

    default: {  // Parent
        int child_status;
        wait(&child_status);
        printf("waited for child (pid=%d) to exit, exited=%d status=%d\n",
               pid, WIFEXITED(child_status), WEXITSTATUS(child_status));
        if (!WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
            printf("child didn't exit successfully, parent exiting\n");
            exit(EXIT_SUCCESS);
        }

        char buf[32];
        buf[sizeof(buf)-1] = '\0';
        lseek(fd, offset, 0);
        read(fd, buf, sizeof(buf)-1);
        printf("read %ld bytes from memfd (offset=%lx): %s\n",
               sizeof(buf), offset, buf);

        printf("parent exiting\n");
        exit(EXIT_SUCCESS);
    }
    }
}