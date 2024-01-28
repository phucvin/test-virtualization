```
$ nasm -felf64 hello64.asm && ld -o hello64 hello64.o && ./hello64

$ sudo apt-get install gcc-multilib  # Needs to install 32-bit libs on a 64-bit OS
$ gcc -m32 -o hello32 hello32.s && ./hello32

$ strace ./hello32
execve("./hello32", ["./hello32"], 0x7ffc86939f20 /* 100 vars */) = 0
strace: [ Process PID=42759 runs in 32 bit mode. ]
[... a bunch of syscalls omitted since running a 32-bit progam on a 64-bit OS]
write(1, "Hello 32 World\n", 15Hello 32 World
)        = 15
exit(0)                                 = ?

$ strace ./hello64
execve("./hello64", ["./hello64"], 0x7fff82471350 /* 100 vars */) = 0
write(1, "Hello 64 World\n", 15Hello 64 World
)        = 15
exit(0)                                 = ?
```

TODO:
- Write a simple ptrace tool to pause process when it tries to exit, so we can view /proc/pid/* (e.g. maps, mem) of short-live processes.
- Multi-thread multi-process ptrace

References:
- Playing with ptrace: https://www.linuxjournal.com/article/6100 & https://www.linuxjournal.com/article/6210
- https://www.tutorialspoint.com/assembly_programming/assembly_system_calls.htm
- https://filippo.io/linux-syscall-table/
- https://man7.org/linux/man-pages/man2/syscall.2.html
- https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl
- https://cs.lmu.edu/~ray/notes/nasmtutorial/
- https://stackoverflow.com/questions/3305350/how-do-32-bit-applications-make-system-calls-on-64-bit-linux
- https://man7.org/linux/man-pages/man5/proc.5.html
- https://blog.cloudflare.com/diving-into-proc-pid-mem
- https://www.baeldung.com/linux/proc-id-maps