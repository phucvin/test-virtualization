#define _GNU_SOURCE
#include <ctype.h>
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
        fprintf(stderr, "\t[ptrace03] " __VA_ARGS__); \
        fputc('\n', stderr); \
        exit(EXIT_FAILURE); \
    } while (0)

#define PFATAL(...) \
    do { \
        fprintf(stderr, "\t[ptrace03] " __VA_ARGS__); \
        fprintf(stderr, ", str(errno): %s\n", strerror(errno)); \
        exit(EXIT_FAILURE); \
    } while (0)

#define DEBUG(...) \
    printf("\t[ptrace03] " __VA_ARGS__); \
    printf("\n"); \

#define DISABLED_DEBUG(...) \

// From: https://github.com/ouadev/proc_maps_parser/blob/master/pmparser.c
void _pmparser_split_line(
		char*buf,char*addr1,char*addr2,
		char*perm,char* offset,char* device,char*inode,
		char* pathname){
	//
	int orig=0;
	int i=0;
	//addr1
	while(buf[i]!='-'){
		addr1[i-orig]=buf[i];
		i++;
	}
	addr1[i]='\0';
	i++;
	//addr2
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		addr2[i-orig]=buf[i];
		i++;
	}
	addr2[i-orig]='\0';

	//perm
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		perm[i-orig]=buf[i];
		i++;
	}
	perm[i-orig]='\0';
	//offset
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		offset[i-orig]=buf[i];
		i++;
	}
	offset[i-orig]='\0';
	//dev
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		device[i-orig]=buf[i];
		i++;
	}
	device[i-orig]='\0';
	//inode
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		inode[i-orig]=buf[i];
		i++;
	}
	inode[i-orig]='\0';
	//pathname
	pathname[0]='\0';
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' ' && buf[i]!='\n'){
		pathname[i-orig]=buf[i];
		i++;
	}
	pathname[i-orig]='\0';

}

int main(int argc, char **argv) {
    if (argc <= 1) FATAL("Too few arugments");

    // Memory for the program's mmaps
    int memfd = memfd_create("memfd01", 0);
    if (memfd == -1) PFATAL("Failed memfd_create");
    // So mmap won't fail because accessing beyond the end of the file
    if (ftruncate(memfd, 0xffffffff) == -1) PFATAL("Failed ftruncate");
    void* mem_start = (void*)0x11000000;
    void* mem_end = (void*)0x11ffffffff;
    void* mem = mmap(mem_start, mem_end-mem_start,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_FIXED,
                     memfd, 0);
    if (mem == MAP_FAILED) PFATAL("Failed mmap");
    DEBUG("Created memfd, fd=%d, start=%p, end=%p", memfd, mem_start, mem_end);

    // Memory of the program binary itself
    int pfd = memfd_create("memfd02", 0);
    FILE *pf = fopen(argv[1], "rb");
    if (pf == NULL) PFATAL("Failed fopen");
    fseek(pf, 0L, SEEK_END);
    int psize = ftell(pf);
    fseek(pf, 0L, SEEK_SET);
    if (ftruncate(pfd, psize) == -1) PFATAL("Failed ftruncate");
    void* pmem = mmap(NULL, psize,
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_SHARED, pfd, 0);
    if (pmem == MAP_FAILED) PFATAL("Failed mmap");
    if (fread(pmem, psize, 1, pf) != 1) PFATAL("Failed fread");
    DEBUG("Loaded tracee ELF to another memfd");

    int pid = fork();
    if (pid == -1) PFATAL("Failed fork");
    if (pid == 0) {  /* Child */
        char* envp[] = {NULL};
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        fexecve(pfd, argv + 1, envp);
        PFATAL("Failed fexecve");  // Only reachable if fexecve fails
    }

    // Parent
    // Sync with child and start ptracing
    if (waitpid(pid, 0, 0) == -1) PFATAL("Failed waitpid");
    if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL) == -1)
        PFATAL("Failed PTRACE_SETOPTIONS");

    // Parse /proc/pid/maps to get the base address of the loaded ELF
    void* pmem_start = NULL;
    void* pmem_end = NULL;
    char tmp[256];
    sprintf(tmp, "/proc/%d/maps", pid);
    FILE* mapsf = fopen(tmp, "r");
    DEBUG("Parsing child's /proc/%d/maps", pid);
    while (!feof(mapsf)) {
        if (fgets(tmp, sizeof(tmp), mapsf) == NULL && errno != 0)
            PFATAL("Failed fgets");
        char addr1[20], addr2[20], perm[8];
        char offset[20], dev[10], inode[30], pathname[128];
        _pmparser_split_line(tmp, addr1, addr2, perm,
                             offset, dev, inode, pathname);
		DEBUG("  %s-%s %s %s %s %s %s",
              addr1, addr2, perm, offset, dev, inode, pathname);
        if (strcmp(pathname, "/memfd:memfd02") == 0) {
            if (pmem_start == NULL) sscanf(addr1, "%p", &pmem_start);
            sscanf(addr2, "%p", &pmem_end);
            DEBUG("  Updated pmem_start=%p, pmem_end=%p", pmem_start, pmem_end);
        }
    }
    fclose(mapsf);

    // Handle syscalls
    int mmapped_size = 0;
    for (;;) {
        /* Enter next system call */
        DEBUG("PTRACE_SYSCALL");
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            PFATAL("Failed PTRACE_SYSCALL");
        DEBUG("waitpid");
        if (waitpid(pid, 0, 0) == -1)
            PFATAL("Failed waitpid");

        /* Gather system call arguments */
        struct user_regs_struct regs;
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
            PFATAL("Failed PTRACE_GETREGS");

        /* Special handling per system call (entrance) */
        DEBUG("Got syscall number=%llu", regs.orig_rax);
        int skipped_syscall = -1;
        switch (regs.orig_rax) {
            case -1:
            case SYS_exit:
            case SYS_exit_group: {
                exit(regs.rdi);
                break;
            }

            case SYS_brk: {
                void* addr = (void*)regs.rdi;
                DEBUG("Getting syscall brk, addr=%p", addr);
                skipped_syscall = SYS_brk;
                // Set to an harmless syscall (35 - nanosleep)
                regs.orig_rax = 35;
                regs.rdi = 0;
                regs.rsi = 0;
                if (ptrace(PTRACE_SETREGS, pid, 0, &regs) == -1)
                    PFATAL("Failed PTRACE_SETREGS");
                break;
            }

            case SYS_mmap: {
                void* addr = (void*)regs.rdi;
                long len = regs.rsi;
                long prot = regs.rdx;
                long flags = regs.r10;
                long fd = regs.r8;
                long fd_offset = regs.r9;
                DEBUG("Getting syscall mmap, addr=%p, len=%ld, "
                      "prot=%ld, flags=%ld, fd=%ld, fd_offset=%ld",
                      addr, len, prot, flags, fd, fd_offset);
                if (addr == NULL &&
                    prot == 3 /* PROT_READ | PROT_WRITE */ &&
                    flags == 34 /* MAP_PRIVATE | MAP_ANONYMOUS */) {
                    addr = mem + mmapped_size;
                    regs.rdi = (long)addr;
                    flags = MAP_SHARED | MAP_FIXED;
                    regs.r10 = flags;
                    fd = memfd;
                    regs.r8 = fd;
                    fd_offset = mmapped_size;
                    regs.r9 = fd_offset;
                    mmapped_size += len;
                    DEBUG("Fixing mmap(NULL, %ld, "
                          "PROT_READ | PROT_WRITE, "
                          "MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)"
                          "\n\t\tto mmap(%p, <same>, <same>, "
                          "MAP_SHARED, `shared memfd`, %ld)",
                          len, addr, fd_offset);
                    if (ptrace(PTRACE_SETREGS, pid, 0, &regs) == -1)
                        PFATAL("Failed PTRACE_SETREGS");
                }
                break;
            }

            case SYS_write: {
                long fd = regs.rdi;
                void* buf = (void*)regs.rsi;
                long len = regs.rdx;
                DEBUG("Getting write(%ld, %p, %ld)", fd, buf, len);
                if (buf >= mem_start && buf < mem_end) {
                    DEBUG("The address is inside memfd, "
                          "will modifying (case reversing) it");
                    // Tracee's address can be dereference without translation
                    // since it's mmaped at the same address at tracer
                    char* bufc = (char*)buf;
                    printf("\t\t%p before: %s", bufc, bufc);
                    for (int i = 0; i < len; ++i) {
                        char c = bufc[i];
                        if (isalpha(c) && islower(c)) c = toupper(c);
                        else if (isalpha(c) && isupper(c)) c = tolower(c);
                        bufc[i] = c;
                    }
                    printf("\t\t%p after: %s", bufc, bufc);
                } else if (buf >= pmem_start && buf < pmem_end) {
                    DEBUG("The address is inside program/ELF memfd, "
                          "will modifying (ROT13, uppercase) it");
                    char* bufc = (char*)((unsigned long long)pmem +
                                         buf - pmem_start);
                    printf("\t\t%p before: %s", bufc, bufc);
                    for (int i = 0; i < len; ++i) {
                        char c = bufc[i];
                        if (isalpha(c)) c = 'A' + ((toupper(c)-'A'+13)%26);
                        bufc[i] = c;
                    }
                    printf("\t\t%p after: %s", bufc, bufc);
                } else {
                    DEBUG("The address is outside memfd, won't modify it");
                }
                break;
            }
        }

        /* Run system call and stop on exit */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)
            PFATAL("Failed PTRACE_SYSCALL");
        if (waitpid(pid, 0, 0) == -1)
            PFATAL("Failed waitpid");

        /* Gather register values after syscall exit */
        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
            PFATAL("Failed PTRACE_GETREGS");

        /* Special handling per system call (exit) */
        DEBUG("Kernel returned %llu for syscall %llu", regs.rax, regs.orig_rax);
        if (skipped_syscall != -1) {
            switch (skipped_syscall) {
                case SYS_brk: {
                    DEBUG("Returning failed syscall brk");
                    regs.orig_rax = SYS_brk;
                    regs.rax = -ENOMEM;
                    if (ptrace(PTRACE_SETREGS, pid, 0, &regs) == -1)
                        PFATAL("Failed PTRACE_SETREGS");
                    break;
                }
                default: {
                    DEBUG("Returning not implemented for "
                          "unknown skipped syscall");
                    regs.rax = -ENOSYS;
                    if (ptrace(PTRACE_SETREGS, pid, 0, &regs) == -1)
                        PFATAL("Failed PTRACE_SETREGS");
                    break;
                }
            }
        } else {
            switch (regs.orig_rax) {
                case SYS_mmap: {
                    void* addr = (void*)regs.rdi;
                    long len = regs.rsi;
                    long prot = regs.rdx;
                    long flags = regs.r10;
                    long fd = regs.r8;
                    long fd_offset = regs.r9;
                    DEBUG("Sent syscall mmap, addr=%p, len=%ld, "
                        "prot=%ld, flags=%ld, fd=%ld, fd_offset=%ld. "
                        "Got result=%p",
                        addr, len, prot, flags, fd, fd_offset, (void*)regs.rax);
                    break;
                }
            }
        }
    }
}