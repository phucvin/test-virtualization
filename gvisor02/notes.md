Use runsc directly

https://github.com/google/gvisor/tree/master?tab=readme-ov-file#using-go-get
```
$ echo "module runsc" > go.mod
$ GO111MODULE=on go get gvisor.dev/gvisor/runsc@go
$ CGO_ENABLED=0 GO111MODULE=on go build -o runsc gvisor.dev/gvisor/runsc
```

```
$ ./runsc --rootless --network=none do dmesg
$ ./runsc --rootless --network=none do ls .
$ ./runsc --rootless --network=host do curl google.com
$ sudo ./runsc --platform=kvm do curl google.com
```

Test with userfaultfd:
- Should fail when running with gVisor since gVisor's kernel doesn't support userfaultfd yet
```
$ gcc -pthread -o userfaultfdtest01 userfaultfdtest01.c
$ ./userfaultfdtest01
++ userfaultfd failed: Operation not permitted
$ sudo ./userfaultfdtest01
-- handled page fault.
-- handled page fault.

$ ./runsc --rootless --network=none do ./userfaultfdtest01
++ userfaultfd failed: Function not implemented
$ sudo ./runsc --platform=kvm do sudo ./userfaultfdtest01
sudo: unable to resolve host codespaces-509483: Name or service not known
++ userfaultfd failed: Function not implemented
```

Notes:
- gVisor seems not working inside containers (e.g. Gitpod), only in VMs (Github Codespaces)

TODO:
- Try modify gVisor code to print debug message when got a syscall, etc.

References:
- https://github.com/google/gvisor/issues/266 (Implement userfaultfd)
- https://blog.lizzie.io/using-userfaultfd.html
