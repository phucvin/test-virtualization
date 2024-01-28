#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <syscall.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/mman.h>

#define FATAL(...) \
    do { \
        fprintf(stderr, "\t\t\t[ptrace04] " __VA_ARGS__); \
        fputc('\n', stderr); \
        exit(EXIT_FAILURE); \
    } while (0)

#define PFATAL(...) \
    do { \
        fprintf(stderr, "\t\t\t[ptrace04] " __VA_ARGS__); \
        fprintf(stderr, ", str(errno): %s\n", strerror(errno)); \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char **argv) {
    if (argc <= 1) FATAL("Too few arugments");

    int memfd = memfd_create("memfd01", 0);
    if (memfd == -1) PFATAL("Failed memfd_create");

    int pid = fork();
    if (pid == -1) PFATAL("Failed fork");
    if (pid == 0) {  /* Child */
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
        PFATAL("Failed execvp");  // Only reachable if excecvp fails
    }

    // Parent
    if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");
    if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL) == -1) PFATAL("Failed PTRACE_SETOPTIONS");
    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");

        /* Gather system call arguments */
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1) PFATAL("Failed PTRACE_GETREGS");

        /* Special handling per system call (entrance) */
        printf("\t\t\t[ptrace04] Got syscall number=%llu\n", regs.orig_rax);
        switch (regs.orig_rax) {
            case SYS_exit: exit(regs.rdi);
            case SYS_exit_group: exit(regs.rdi);
        }

        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");

        /* Special handling per system call (exit) */
        switch (regs.orig_rax) {
            // TODO
        }
    }
}