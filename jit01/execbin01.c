// From: https://stackoverflow.com/a/58489219
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

/* Allocate size bytes of executable memory. */
unsigned char *alloc_exec_mem(size_t size)
{
    void *ptr;

    ptr = mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
               MAP_PRIVATE | MAP_ANON, -1, 0);

    if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(1);
    }

    return ptr;
}

/* Read up to buffer_size bytes, encoded as 1's and 0's, into buffer. */
void read_ones_and_zeros(unsigned char *buffer, size_t buffer_size)
{
    unsigned char byte = 0;
    int bit_index = 0;
    int c;

    while ((c = getchar()) != EOF) {
            if (isspace(c)) {
                    continue;
            } else if (c != '0' && c != '1') {
                    fprintf(stderr, "error: expected 1 or 0!\n");
                    exit(1);
            }

            byte = (byte << 1) | (c == '1');
            bit_index++;

            if (bit_index == 8) {
                    if (buffer_size == 0) {
                            fprintf(stderr, "error: buffer full!\n");
                            exit(1);
                    }
                    *buffer++ = byte;
                    --buffer_size;
                    byte = 0;
                    bit_index = 0;
            }
    }

    if (bit_index != 0) {
            fprintf(stderr, "error: left-over bits!\n");
            exit(1);
    }
}

int main()
{
    typedef int (*func_ptr_t)(void);

    func_ptr_t func;
    unsigned char *mem;
    int x;

    mem = alloc_exec_mem(1024);
    func = (func_ptr_t) mem;

    read_ones_and_zeros(mem, 1024);

    x = (*func)();

    printf("function returned %d\n", x);

    return 0;
}