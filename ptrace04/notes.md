Main test in this folder: use ptrace, fork, memfd, mmap to allow tracer to read & write tracee memory faster than PTRACE_(PEEKDATA/POKEDATA)
```
$ gcc -o ptrace04 ptrace04.c && gcc -o tracee02 tracee02.c && ./ptrace04 ./tracee02
...
        [ptrace03] Getting syscall brk, addr=(nil)
        [ptrace03] Returning failed syscall brk
        [ptrace03] Getting syscall mmap, addr=(nil), len=1048576, prot=3, flags=34, fd=4294967295, fd_offset=0
        [ptrace03] Fixing mmap(NULL, 1048576, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, ?, ?)
                to mmap(0x11003000, <same>, <same>, MAP_SHARED, `shared memfd`, 12288)
        [ptrace03] Sent syscall mmap, addr=0x11003000, len=1048576, prot=3, flags=17, fd=3, fd_offset=12288. Got result=0x11003000
        [ptrace03] Getting write(1, 0x110032a0, 40)
        [ptrace03] The address is inside memfd, will modifying (case reversing) it
                0x110032a0 before: this is ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
                0x110032a0 after: THIS IS abcdefghijklmnopqrstuvwxyzabcde
THIS IS abcdefghijklmnopqrstuvwxyzabcde
        [ptrace03] Getting write(1, 0x11002000, 32)
        [ptrace03] The address is inside memfd, will modifying (case reversing) it
                0x11002000 before: ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
                0x11002000 after: abcdefghijklmnopqrstuvwxyzabcde
abcdefghijklmnopqrstuvwxyzabcde
        [ptrace03] Getting write(1, 0x55c4111ce020, 37)
        [ptrace03] The address is inside program/ELF memfd, will modifying (ROT13, uppercase) it
                0x7fe9ad9a9020 before: this is in the program data section
                0x7fe9ad9a9020 after: GUVF VF VA GUR CEBTENZ QNGN FRPGVBA
GUVF VF VA GUR CEBTENZ QNGN FRPGVBA
        [ptrace03] Getting write(1, 0x55c4111ce020, 37)
        [ptrace03] The address is inside program/ELF memfd, will modifying (ROT13, uppercase) it
                0x7fe9ad9a9020 before: GUVF VF VA GUR CEBTENZ QNGN FRPGVBA
                0x7fe9ad9a9020 after: THIS IS IN THE PROGRAM DATA SECTION
THIS IS IN THE PROGRAM DATA SECTION

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
- Allow tracer to access tracee .data sections directly by using `execveat` and memfd contains the tracee binary (another option is to load ELF file and run it directly, but it seems more complicated). Then, read from `/proc/<tracee pid>/maps` to get the base address of the ELF in tracee space to calculate potential-in-binary addresses in tracer space.

TODO:
- Fix static linked binary hang when the tracer returns failed syscall brk `gcc -o ptrace04 ptrace04.c && gcc -static -no-pie -o tracee02 tracee02.c && ./ptrace04 ./tracee02`
- If can even access data on tracee's stack, that can be even better
- Handle more syscalls (e.g. execve, arch_prclt, uname, mprotect, fstat, pause, access, open/openat, close, read, write, pread/pread64, munmap, etc.)
- Handle open & mmap to files, including shared libraries (e.g. linux-vdso.so.1, libc.so.6, /lib64/ld-linux-x86-64.so.2, etc.), to allow only known/customized files.
- Block all other syscalls.

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

$ gcc -o memfd02 memfd02.c && ./memfd02
writing something to mem (addr=0x7f46ddd81000)
child exiting
waited for child (pid=25915) to exit, exited=1 status=0
read 32 bytes from memfd (offset=aaffa000): hello memfd and mmap
parent exiting
```

Test memfd, fexecve:
```
$ gcc -o execveat01 execveat01.c && gcc -o tracee02 tracee02.c && ./execveat01 ./tracee02
fsize: 871896
msize: 871896
fread: 1
mem: 0x7fb86c7d3000, errno: 0
this is ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE
this is in the program data section
waited for child (pid=39590) to exit, exited=1 status=0
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
