```
$ nasm -felf64 hello64.asm && ld -o hello64 hello64.o && ./hello64

$ gcc -m32 -o hello32 hello32.s && ./hello32
```

References:
- Playing with ptrace: https://www.linuxjournal.com/article/6100 & https://www.linuxjournal.com/article/6210
- https://www.tutorialspoint.com/assembly_programming/assembly_system_calls.htm
- https://filippo.io/linux-syscall-table/