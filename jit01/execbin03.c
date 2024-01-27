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

void emit_code_into_memory_with_params(unsigned char* m, long p_start, int p_len) {
  // The gist is calling sys_write to stdout with p_start and p_len
  // printf("p_start=%ld, p_len=%d\n", p_start, p_len);

  // mov rax, 1; mov rdi, 1 // rax=1->sys_write rdi=1->stdout
  unsigned char code1[] = { 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00 };
  /*
  int* p_code1 = (int*)&code1[3];
  printf("code1=%d\n", *p_code1);
  p_code1 = (int*)&code1[10];
  printf("code1=%d\n", *p_code1);
  */

  // movabs rsi, p_start
  unsigned char code2[] = { 0x48, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  for (int i = 0; i < 8; ++i) {
    code2[2+i] = p_start >> (i * 8);
  }
  /*
  long* p_code2 = (long*)&code2[2];
  printf("code2=%ld\n", *p_code2);
  for (int i = 0; i < sizeof(code2); ++i) {
    printf("0x%X, ", code2[i]);
  }
  printf("\n");
  */

  // mov rdx, p_len
  unsigned char code3[] = { 0x48, 0xC7, 0xC2, 0x00, 0x00, 0x00, 0x00 };
  for (int i = 0; i < 4; ++i) {
    code3[3+i] = p_len >> (i * 8);
  }
  /*
  int* p_code3 = (int*)&code3[3];
  printf("code3=%d\n", *p_code3);
  */

  // syscall; mov rax, 0; ret
  unsigned char code4[] = { 0x0F, 0x05, 0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC3  };

  memcpy(m, code1, sizeof(code1));
  memcpy(m+sizeof(code1), code2, sizeof(code2));
  memcpy(m+sizeof(code1)+sizeof(code2), code3, sizeof(code3));
  memcpy(m+sizeof(code1)+sizeof(code2)+sizeof(code3), code4, sizeof(code4));
}

typedef int (*JittedFunc)();

int main() {
  void* m = alloc_executable_memory(/*size=*/1024);
  char p[] = "hello world\n";
  emit_code_into_memory_with_params(m, (long) p, sizeof(p));
  /*
  printf("machine (x86_64) code:\n");
  for (int i = 0; i < 40; ++i) {
    printf("%x ", ((unsigned char*)m)[i]);
  }
  printf("\n");
  */

  JittedFunc func = m;
  int result = func();
  printf("result = %s\n", strerror(-result));
  return 0;
}