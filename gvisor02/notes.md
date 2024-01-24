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

Notes:
- gVisor seems not working inside containers (e.g. Gitpod), only in VMs (Github Codespaces)