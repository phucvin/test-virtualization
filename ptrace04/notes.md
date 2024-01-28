Test clone(2):
```
$ gcc -o clone01 clone01.c && ./clone01
Child sees buf = "hello from parent"
Writing something else to buf
Child exited with status 0
Checking buf again, buf = "hello from parent"
The data in buf doesn't change, try running again with `vm` argument

$ ./clone01 vm
Child sees buf = "hello from parent"
Writing something else to buf
Child exited with status 0
Checking buf again, buf = "hello from CHILD"
```

Test clone and trace (WIP):
```
$ gcc -o clonentrace01 clonentrace01.c && ./clonentrace01
```
Notes & TODO:
- Clone to keep the same address space, and load binary/ELF to executed in the cloned/tracee process so the tracer can directly access memory of the tracee as well.
- The above would be a foundation for managing memory somewhat similar to gVisor. Next is to handle guest mmap and mmap it to a memfd file + protecting the host process's memory from guest binary.
- Doesn't work yet, probably because cloned process is not ptraceable trivially.

TODO:
- Manage all mmap memory of tracee by intercepting mmap syscall and redirect it to a shared memfd/file.
- Write a simple ptrace tool to pause process when it tries to exit, so we can view /proc/pid/* (e.g. maps, mem) of short-live processes.
- Multi-thread multi-process ptrace

Test memfd, mmap and fork:
```
$ gcc -o memfd01 memfd01.c && ./memfd01
wating for child (pid=11686) to exit
writing something to memfd (offset=ffffaaff)
child exiting
read 32 bytes from memfd (offset=ffffaaff): hello memfd
parent exiting

$ gcc -o memfd02 memfd02.c && ./memfd01
writing something to mem (addr=0x7f46ddd81000)
child exiting
waited for child (pid=25915) to exit, exited=1 status=0
read 32 bytes from memfd (offset=aaffa000): hello memfd and mmap
parent exiting
```

References:
- https://man7.org/linux/man-pages/man2/ptrace.2.html
- https://man7.org/linux/man-pages/man2/clone.2.html
- https://groups.google.com/g/gvisor-users/c/9KOz4qBKl18?pli=1 (Sentry memory management)
- https://github.com/google/gvisor/blob/master/pkg/sentry/mm/README.md
- https://man7.org/linux/man-pages/man5/proc.5.html
- https://blog.cloudflare.com/diving-into-proc-pid-mem
- https://www.baeldung.com/linux/proc-id-maps
- https://stackoverflow.com/questions/5395769/any-good-guides-on-using-ptrace-sysemu
- https://github.com/astroza/vkern (uses PTRACE_SYSEMU)
- https://github.com/TUD-OS/libelkvm
- https://github.com/aleden/ptrace-multi-threaded-demo
- https://eli.thegreenplace.net/2018/launching-linux-threads-and-processes-with-clone/
