#define _GNU_SOURCE
#include <sched.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int tracee_main(void* arg) {
  ptrace(PTRACE_TRACEME, 0, 0, 0);
  printf("tracee_main\n");
  return 0;
}

int tracer_main(int tracee_pid) {
  printf("tracer_main\n");

  while (1) {
    /* Enter next system call */
    if (ptrace(PTRACE_SYSCALL, tracee_pid, 0, 0) == -1) {
      perror("PTRACE_SYSCALL failed");
      return 1;
    }
    if (waitpid(tracee_pid, 0, 0) == -1) {
      perror("waitpid failed");
      return 1;
    }
  }

  return 0;
}

int main() {
  // Allocate stack for tracee (i.e. child) task.
  const int STACK_SIZE = 1024 * 1;
  void* tracee_stack = malloc(STACK_SIZE);
  if (!tracee_stack) {
    perror("tracee_stack malloc failed");
    return 1;
  }

  unsigned long flags = SIGCHLD | CLONE_VM;
  int tracee_pid = clone(tracee_main, tracee_stack + STACK_SIZE, flags, NULL);
  if (tracee_pid == -1) {
    perror("clone failed");
    return 1;
  }

  waitpid(tracee_pid, 0, 0);
  ptrace(PTRACE_SETOPTIONS, tracee_pid, 0, PTRACE_O_EXITKILL);

  printf("tracee_pid=%d\n", tracee_pid);
  return tracer_main(tracee_pid);
}