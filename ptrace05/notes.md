TODO:
- Limit CPU time or wall clock time by having the tracer sending signal to the tracee
- Use ptrace with seccomp
- Not really related to ptrace, but use KVM to sandbox a program/elf to have even better isolation

References:
- https://tbrindus.ca/on-online-judging-part-1/ & https://tbrindus.ca/on-online-judging-part-5/
- https://lwn.net/Articles/656307/
- https://stackoverflow.com/questions/1935537/how-to-intercept-linux-signals-in-c
- https://stackoverflow.com/questions/7696925/how-to-send-a-signal-to-a-process-in-c
- https://man7.org/linux/man-pages/man2/kill.2.html
- https://lwn.net/Articles/902585/ Direct host system calls from KVM
- https://cs.brown.edu/media/filer_public/b9/5e/b95e3e74-3d84-4434-b9cc-900b98e89cbf/ayer.pdf KVMSandbox: Application-Level Sandboxing with x86 Hardware
Virtualization and KVM
