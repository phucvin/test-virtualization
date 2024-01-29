// From: https://stackoverflow.com/questions/52238441/c-how-to-change-my-own-program-in-my-program-in-runtime
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

void * alignptr(void * ptr, uintptr_t alignment) {
    return (void *)((uintptr_t)ptr & ~(alignment - 1));
}

// pattern is a 0-terminated string
char* find(char *string, unsigned int stringLen, char *pattern) {
    unsigned int iString = 0;
    unsigned int iPattern;
    for (unsigned int iString = 0; iString < stringLen; ++iString) {
        for (iPattern = 0;
            pattern[iPattern] != 0
            && string[iString+iPattern] == pattern[iPattern];
            ++iPattern);
        if (pattern[iPattern] == 0) { return string+iString; }
    }
    return NULL;
}

int main() {
    void *p = alignptr(main, 4096);
    int result = mprotect(p, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (result == -1) {
        printf("Error: %s\n", strerror(errno));
    }

    // Correct a part of THIS program directly in RAM
    char programSubcode[12] = {'H','e','l','l','o',
                                ' ','W','o','r','l','t',0};
    char *programCode = (char *)main;
    char *helloWorlt = find(programCode, 4096, programSubcode);
    if (helloWorlt != NULL) {
        printf("Found bug, fixing...\n");
        helloWorlt[10] = 't';
    }   
    printf("Hello Worlt\n");
    return 0;
}