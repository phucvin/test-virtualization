#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <linux/fcntl.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Too few arugments\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Failed fopen, errno: %d\n", errno);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    int memfd = memfd_create("memfd01", 0);
    if (ftruncate(memfd, fsize) == -1) {
        printf("Failed ftruncate, errno: %d\n", errno);
        return 1;
    }
    char* mem = mmap(NULL, fsize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, memfd, 0);
    if (mem == MAP_FAILED) {
        printf("Failed mmap, errno: %d\n", errno);
        return 1;
    }
    if (fread(mem, fsize, 1, f) != 1) {
        printf("Failed fread, errno: %d\n", errno);
        return 1;
    }

    int pid = fork();
    if (pid == -1) {
        printf("Failed fork, errno: %d\n", errno);
        return 1;
    }
    if (pid == 0) {
        char* argv2[] = {NULL};
        char* envp[] = {NULL};
        // The first byte is already 0x7F (magic number for ELF file),
        // changing this to anything else will make fexecve fail
        mem[0] = 0x7F;
        // Try finding the constant string in tracee02 and modifing it
        const char* txt = "this is in the program data section\n";
        const char* modified_txt = "This is in the program data section\n";
        for (int i = 0; i < fsize; ++i) {
            int j;
            for (j = 0; j < sizeof(txt); ++j) {
                if (mem[i] == txt[j]) break;
            }
            if (j == sizeof(txt)) {
                printf("found tracee02 string constant, modifying it\n");
                char* p = &mem[i];
                // TODO: This gives ENOEXEC, probably have to wait after fexecve
                // memcpy(p, modified_txt, sizeof(modified_txt));
                break;
            }
        }
        fexecve(memfd, argv2, envp);
        printf("Failed fexecve, errno: %d\n", errno);
        return 1;
    } else {
        int child_status;
        wait(&child_status);
        printf("waited for child (pid=%d) to exit, exited=%d status=%d\n",
                pid, WIFEXITED(child_status), WEXITSTATUS(child_status));
        if (!WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
            printf("child didn't exit successfully, parent exiting\n");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}