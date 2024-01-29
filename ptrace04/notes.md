Main test in this folder: use ptrace, fork, memfd, mmap to allow tracer to read & write tracee memory faster than PTRACE_(PEEKDATA/POKEDATA)
```
$ gcc -o ptrace04 ptrace04.c && gcc -o tracee02 tracee02.c && ./ptrace04 ./tracee02
        [ptrace04] Created memfd, fd=3, start=0x11000000, end=0x11ffffffff
        [ptrace04] Getting syscall brk, addr=(nil)
        [ptrace04] Returning failed syscall brk
        [ptrace04] Getting syscall mmap, addr=(nil), len=38391, prot=1, flags=2, fd=4, fd_offset=0
        [ptrace04] Sent syscall mmap, addr=(nil), len=38391, prot=1, flags=2, fd=4, fd_offset=0. Got result=0x7f2d7852e000
        [ptrace04] Getting syscall mmap, addr=(nil), len=8192, prot=3, flags=34, fd=4294967295, fd_offset=0
        [ptrace04] Fixing mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)
                to mmap(0x11000000, <same>, <same>, MAP_SHARED, `shared memfd`, 0)
        [ptrace04] Sent syscall mmap, addr=0x11000000, len=8192, prot=3, flags=17, fd=3, fd_offset=0. Got result=0x11000000
        [ptrace04] Getting syscall mmap, addr=(nil), len=2037344, prot=1, flags=2050, fd=4, fd_offset=0
        [ptrace04] Sent syscall mmap, addr=(nil), len=2037344, prot=1, flags=2050, fd=4, fd_offset=0. Got result=0x7f2d7833c000
        [ptrace04] Getting syscall mmap, addr=0x7f2d7835e000, len=1540096, prot=5, flags=2066, fd=4, fd_offset=139264
        [ptrace04] Sent syscall mmap, addr=0x7f2d7835e000, len=1540096, prot=5, flags=2066, fd=4, fd_offset=139264. Got result=0x7f2d7835e000
        [ptrace04] Getting syscall mmap, addr=0x7f2d784d6000, len=319488, prot=1, flags=2066, fd=4, fd_offset=1679360
        [ptrace04] Sent syscall mmap, addr=0x7f2d784d6000, len=319488, prot=1, flags=2066, fd=4, fd_offset=1679360. Got result=0x7f2d784d6000
        [ptrace04] Getting syscall mmap, addr=0x7f2d78524000, len=24576, prot=3, flags=2066, fd=4, fd_offset=1994752
        [ptrace04] Sent syscall mmap, addr=0x7f2d78524000, len=24576, prot=3, flags=2066, fd=4, fd_offset=1994752. Got result=0x7f2d78524000
        [ptrace04] Getting syscall mmap, addr=0x7f2d7852a000, len=13920, prot=3, flags=50, fd=4294967295, fd_offset=0
        [ptrace04] Sent syscall mmap, addr=0x7f2d7852a000, len=13920, prot=3, flags=50, fd=4294967295, fd_offset=0. Got result=0x7f2d7852a000
        [ptrace04] Getting syscall mmap, addr=(nil), len=4096, prot=3, flags=34, fd=0, fd_offset=0
        [ptrace04] Fixing mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)
                to mmap(0x11002000, <same>, <same>, MAP_SHARED, `shared memfd`, 8192)
        [ptrace04] Sent syscall mmap, addr=0x11002000, len=4096, prot=3, flags=17, fd=3, fd_offset=8192. Got result=0x11002000
        [ptrace04] Getting syscall brk, addr=(nil)
        [ptrace04] Returning failed syscall brk
        [ptrace04] Getting syscall mmap, addr=(nil), len=1048576, prot=3, flags=34, fd=4294967295, fd_offset=0
        [ptrace04] Fixing mmap(NULL, 1048576, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)
                to mmap(0x11003000, <same>, <same>, MAP_SHARED, `shared memfd`, 12288)
        [ptrace04] Sent syscall mmap, addr=0x11003000, len=1048576, prot=3, flags=17, fd=3, fd_offset=12288. Got result=0x11003000
        [ptrace04] Getting write(1, 0x110032a0, 40)
        [ptrace04] The address is inside memfd, will modifying (case reversing) it
                0x110032a0 before: this is ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
                0x110032a0 after: THIS IS abcdefghijklmnopqrstuvwxyzabcde
THIS IS abcdefghijklmnopqrstuvwxyzabcde
        [ptrace04] Getting write(1, 0x11002000, 32)
        [ptrace04] The address is inside memfd, will modifying (case reversing) it
                0x11002000 before: ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
                0x11002000 after: abcdefghijklmnopqrstuvwxyzabcde
abcdefghijklmnopqrstuvwxyzabcde
        [ptrace04] Getting write(1, 0x55ac6427d020, 37)
        [ptrace04] The address is outside memfd, won't modify it
this is in the program data section

$ gcc -o ptrace04 ptrace04.c && gcc -o tracee02 tracee02.c && ./ptrace04 ls ../
...
        [ptrace04] Getting write(1, 0x11004500, 93)
        [ptrace04] The address is inside memfd, will modifying (case reversing) it
                0x11004500 before: README.md  biscuit01  gvisor01       gvisor02  jit01  kvm01  ptrace01  ptrace02  ptrace03  ptrace04
59-2            0x11004500 after: readme.MD  BISCUIT01  GVISOR01        GVISOR02  JIT01  KVM01  PTRACE01  PTRACE02  PTRACE03  PTRACE04
readme.MD  BISCUIT01  GVISOR01  GVISOR02  JIT01  KVM01  PTRACE01  PTRACE02  PTRACE03  PTRACE04
```
Notes:
- This is to improve https://github.com/phucvin/ptrace-examples (which uses PTRACE_(PEEKDATA/POKEDATA))
- The main challenge is to handle tracee's mmap syscall and redirect it to a shared memfd
- This is probably better than process_vm_readv/process_vm_writev since it doesn't require copying memory between processes.
- But it might not provide more isolation since the linux process's memory is already isolated good enough. Also, not all memory are accessible in the tracer, e.g. non-mmap memory like sbrk (update: can work-aroud) or program's text.

- Using `execve` shouldn't close the memfd in the tracee, since file descriptors are kept open across `exec` unless specified close on exec.
TODO:
- Allow tracer to access tracee .data sections directly by using `execveat` and memfd contains the tracee binary. Another option is to load ELF file and run it directly, but it seems more complicated.


The things below here are smaller related tests.


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
- https://stackoverflow.com/questions/5395769/any-good-guides-on-using-ptrace-sysemu
- https://github.com/astroza/vkern (uses PTRACE_SYSEMU)
- https://github.com/TUD-OS/libelkvm
- https://github.com/aleden/ptrace-multi-threaded-demo
- https://eli.thegreenplace.net/2018/launching-linux-threads-and-processes-with-clone/
- https://man7.org/linux/man-pages/man2/process_vm_readv.2.html
- https://blog.cloudflare.com/how-to-execute-an-object-file-part-1
- https://github.com/topics/elf-loader
- https://github.com/malisal/loaders/blob/master/elf/main.c
- https://github.com/EntySec/libpawn/blob/main/src/elf_pawn.c
- https://stackoverflow.com/questions/63790377/how-memfd-create-and-fexecve-works-together
