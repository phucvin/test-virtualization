#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void* alloc_executable_memory(size_t size) {
  void* ptr = mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  return ptr;
}

void emit_code_into_memory_with_params(unsigned char* m, long p_start, long p_len) {
  printf("p_start=%ld, p_len=%ld\n", p_start, p_len);
  // The gist is calling sys_write to stdout with p_start
  // mov rax, 1; mov rdi, 1 // rax=1->sys_write rdi=1->stdout
  unsigned char code1[] = { 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00 };
  unsigned char code2[] = { 0x48, 0xBE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  unsigned char code3[] = { 0x48, 0xC7, 0xC6, 0xFF, 0x00, 0x00, 0x00 };
  // syscall; ret
  unsigned char code4[] = { 0x0F, 0x05, 0xC3 };
  memcpy(m, code1, sizeof(code1));
  memcpy(m+sizeof(code1), code2, sizeof(code2));
  memcpy(m+sizeof(code1)+sizeof(code2), code3, sizeof(code3));
  memcpy(m+sizeof(code1)+sizeof(code2)+sizeof(code3), code4, sizeof(code4));
}

typedef long (*JittedFunc)(long);

int main() {
  void* m = alloc_executable_memory(/*size=*/1024);
  char p[] = "hello world";
  emit_code_into_memory_with_params(m, (long) p, sizeof(p));

  JittedFunc func = m;
  int result = func(23);
  printf("result = %s\n", strerror(-result));
  return 0;
}