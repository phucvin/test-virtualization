#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

const char text[] = "this is in the program data section\n";

int main() {
    char stack_txt[] = "this is on stack\n";
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
    write(1, text, sizeof(text));
    write(1, stack_txt, sizeof(stack_txt));
    pause();
    // [ptrace03] Getting write(1, 0x7ffea5670480, 18)
    // 7ffea5651000-7ffea5672000 stack
    return 0;
}