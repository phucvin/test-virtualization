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
        fprintf(stderr, "\t[ptrace04] " __VA_ARGS__); \
        fputc('\n', stderr); \
        exit(EXIT_FAILURE); \
    } while (0)

#define PFATAL(...) \
    do { \
        fprintf(stderr, "\t[ptrace04] " __VA_ARGS__); \
        fprintf(stderr, ", str(errno): %s\n", strerror(errno)); \
        exit(EXIT_FAILURE); \
    } while (0)

#define DEBUG(...) \
    printf("\t[ptrace04] " __VA_ARGS__); \
    printf("\n"); \

#define DISABLED_DEBUG(...) \

int main(int argc, char **argv) {
    if (argc <= 1) FATAL("Too few arugments");

    int pid = fork();
    if (pid == -1) PFATAL("Failed fork");
    if (pid == 0) {  /* Child */
        DEBUG("Child PID: %d", getpid());
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
        PFATAL("Failed execvp");  // Only reachable if excecvp fails
    }

    // Parent
    if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");
    if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL) == -1) PFATAL("Failed PTRACE_SETOPTIONS");
    int mmapped_size = 0;
    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");

        /* Gather system call arguments */
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1) PFATAL("Failed PTRACE_GETREGS");

        /* Special handling per system call (entrance) */
        switch (regs.orig_rax) {
            case SYS_exit:
            case SYS_exit_group: {
                DEBUG("Child exiting, paused, <ctrl+c> to exit");
                pause();
            }
        }

        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");
    }
}