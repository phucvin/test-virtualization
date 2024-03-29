// From: https://eli.thegreenplace.net/2018/launching-linux-threads-and-processes-with-clone/
// We have to define the _GNU_SOURCE to get access to clone(2) and the CLONE_*
// flags from sched.h
//
// Eli Bendersky [http://eli.thegreenplace.net]
// This code is in the public domain.
#define _GNU_SOURCE
#include <sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int child_func(void* arg) {
  char* buf = (char*)arg;
  printf("Child sees buf = \"%s\"\n", buf);
  printf("Writing something else to buf\n");
  strcpy(buf, "hello from CHILD");
  return 0;
}

int main(int argc, char** argv) {
  // Allocate stack for child task.
  const int STACK_SIZE = 65536;
  char* stack = malloc(STACK_SIZE);
  if (!stack) {
    perror("malloc");
    exit(1);
  }

  // When called with the command-line argument "vm", set the CLONE_VM flag on.
  unsigned long flags = 0;
  if (argc > 1 && !strcmp(argv[1], "vm")) {
    flags |= CLONE_VM;
  }

  char buf[100];
  strcpy(buf, "hello from parent");
  if (clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, buf) == -1) {
    perror("clone");
    exit(1);
  }

  int status;
  if (wait(&status) == -1) {
    perror("wait");
    exit(1);
  }

  printf("Child exited with status %d\n", status);
  printf("Checking buf again, buf = \"%s\"\n", buf);
  if (strcmp(buf, "hello from parent") == 0) {
    printf("The data in buf doesn't change, try running again with `vm` argument\n");
  }
  return 0;
}
