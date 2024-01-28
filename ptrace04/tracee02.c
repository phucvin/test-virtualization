#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    int size = 32;
    void* mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (mem == MAP_FAILED) {
        printf("Failed to mmap: %d\n", errno);
        return 1;
    }
    char* buf = (char*)mem;
    buf[size-1] = '\n';
    for (int i = 0; i < size-1; ++i) {
        buf[i] = 'A' + (i % 26);
    }
    // Note: printf or puts won't work with ptrace04
    // since they use sbrk internally to construct the final buffer
    write(1, buf, size);
    return 0;
}