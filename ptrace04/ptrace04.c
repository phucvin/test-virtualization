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

#define DEBUG(...) \
    printf("\t\t\t[ptrace04] " __VA_ARGS__); \
    printf("\n"); \

#define DISABLED_DEBUG(...) \

int main(int argc, char **argv) {
    if (argc <= 1) FATAL("Too few arugments");

    int memfd = memfd_create("memfd01", 0);
    if (memfd == -1) PFATAL("Failed memfd_create");
    // So mmap won't fail because accessing beyond the end of the file
    if (ftruncate(memfd, 0xffffffff) == -1) PFATAL("Failed ftruncate");
    DEBUG("Created memfd: %d", memfd);

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
    int mmapped_size = 0;
    for (;;) {
        /* Enter next system call */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");

        /* Gather system call arguments */
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1) PFATAL("Failed PTRACE_GETREGS");

        /* Special handling per system call (entrance) */
        // DEBUG("Got syscall number=%llu", regs.orig_rax);
        switch (regs.orig_rax) {
            case SYS_exit:
            case SYS_exit_group: {
                char* memch = mmap(NULL, 0xffffffff, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0);
                // Try both, since tracee02 has 2 mmap, the later one is the one seen in tracee02.c
                DEBUG("mmap(memfd)+0 at the end: %s", memch + 0);
                DEBUG("mmap(memfd)+8192 at the end: %s", memch + 8192);
                exit(regs.rdi);
                break;
            }

            case SYS_mmap: {
                void* addr = (void*)regs.rdi;
                long len = regs.rsi;
                long prot = regs.rdx;
                long flags = regs.r10;
                long fd = regs.r8;
                long fd_offset = regs.r9;
                DISABLED_DEBUG("Getting syscall mmap, addr=%p, len=%ld, "
                      "prot=%ld, flags=%ld, fd=%ld, fd_offset=%ld",
                      addr, len, prot, flags, fd, fd_offset);
                if (addr == NULL &&
                    prot == 3 /* PROT_READ | PROT_WRITE */ &&
                    flags == 34 /* MAP_PRIVATE | MAP_ANONYMOUS */) {
                    addr = NULL;  // 0x11000000 + mmapped_size
                    flags = MAP_SHARED;
                    regs.r10 = flags;
                    fd = memfd;
                    regs.r8 = fd;
                    fd_offset = mmapped_size;
                    regs.r9 = fd_offset;
                    mmapped_size += len;
                    DEBUG("Fixing mmap(NULL, %ld, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)"
                          "\n\t\t\t\tto mmap(%p, <same>, <same>, MAP_SHARED, `shared memfd`, %ld)",
                          len, addr, fd_offset);
                    if (ptrace(PTRACE_SETREGS, pid, 0, &regs) == -1) PFATAL("Failed PTRACE_SETREGS");
                }
                break;
            }

            case SYS_write: {
                long fd = regs.rdi;
                void* buf = (void*)regs.rsi;
                long len = regs.rdx;
                DEBUG("Getting write(%ld, %p, %ld)", fd, buf, len);
                DEBUG("Accessing %p: %s", buf, buf);
                break;
            }
        }

        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");

        /* Special handling per system call (exit) */
        switch (regs.orig_rax) {
            case SYS_mmap: {
                void* addr = (void*)regs.rdi;
                long len = regs.rsi;
                long prot = regs.rdx;
                long flags = regs.r10;
                long fd = regs.r8;
                long fd_offset = regs.r9;
                DISABLED_DEBUG("Actually sent syscall mmap, addr=%p, len=%ld, "
                      "prot=%ld, flags=%ld, fd=%ld, fd_offset=%ld",
                      addr, len, prot, flags, fd, fd_offset);
                break;
            }
        }
    }
}