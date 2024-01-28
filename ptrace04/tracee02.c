#include <stdio.h>
#include <sys/mman.h>

int main() {
    int size = 32;
    void* mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    char* buf = (char*)(mem + 1024);
    buf[size-1] = '\0';
    for (int i = 0; i < size-1; ++i) {
        buf[i] = 'a' + (i % 26);
    }
    printf("%s\n", buf);
    return 0;
}