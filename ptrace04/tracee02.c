#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

const char text[] = "this is in the program data section\n";

int main() {
    int size = 32;
    void* mem = mmap(NULL, 4096,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     0, 0);
    if (mem == MAP_FAILED) {
        printf("Failed to mmap: %d\n", errno);
        return 1;
    }
    char* buf = (char*)mem;
    buf[size-1] = '\n';
    for (int i = 0; i < size-1; ++i) {
        buf[i] = 'A' + (i % 26);
    }
    printf("this is %s", buf);
    write(1, buf, size);
    write(1, text, sizeof(text));
    pause();
    // [ptrace03] Getting write(1, 0x559250c6[2020], 37)
    // 559250c6[0000]-559250c61000 r--p
    // ...
    // 559250c64000-559250c65000 rw-p
    // --
    // [ptrace03] Getting write(1, 0x55dad73e[a020], 37)
    // 55dad73e[8000]-55dad73e9000 r--p
    // 55dad73ec000-55dad73ed000 rw-p
    return 0;
}