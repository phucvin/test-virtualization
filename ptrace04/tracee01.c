#include <stdio.h>
#include <stdlib.h>

int main() {
    int size = 32;
    char* buf = malloc(size);
    buf[size-1] = '\0';
    for (int i = 0; i < size-1; ++i) {
        buf[i] = 'a' + (i % 26);
    }
    printf("%s\n", buf);
    return 0;
}