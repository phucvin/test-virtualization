TODO:
- Try PTRACE_SYSEMU, PTRACE_O_TRACECLONE
- Clone to keep the same address space, and load binary/ELF to executed in the cloned/tracee process so the tracer can directly access memory of the tracee as well. This would be an example of manage memory somewhat similar to gVisor.
- Write a simple ptrace tool to pause process when it tries to exit, so we can view /proc/pid/* (e.g. maps, mem) of short-live processes.
- Multi-thread multi-process ptrace

References:
- https://man7.org/linux/man-pages/man2/ptrace.2.html
- https://groups.google.com/g/gvisor-users/c/9KOz4qBKl18?pli=1 (Sentry memory management)
- https://github.com/google/gvisor/blob/master/pkg/sentry/mm/README.md
- https://man7.org/linux/man-pages/man5/proc.5.html
- https://blog.cloudflare.com/diving-into-proc-pid-mem
- https://www.baeldung.com/linux/proc-id-maps
- https://stackoverflow.com/questions/5395769/any-good-guides-on-using-ptrace-sysemu
- https://github.com/astroza/vkern
- https://github.com/TUD-OS/libelkvm
- https://github.com/aleden/ptrace-multi-threaded-demo
