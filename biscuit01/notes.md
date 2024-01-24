https://github.com/mit-pdos/biscuit

```
$ sudo apt update
$ sudo apt install qemu-system python2
$ docker run -it --rm -v .:/project golang:1.10.1
$ cd /project/src && ./all.bash  # this is inside the container, can quit after the go binary is built (i.e. ignore testing)
$ exit
$ cd biscuit
$ GOPATH=$(pwd) GOROOT=$(pwd/..) make
$ make qemu CPUS=1
```

Notes:
- Might need to edit qemu options from 8G memory to 1G (search for "-m 8G" in biscuit/GNUmakefile)
- See https://github.com/mit-pdos/biscuit/issues/13#issuecomment-1726960802 to fix the boot error
- To exit qemu, ctrl+a then x
- The exact commit is https://github.com/mit-pdos/biscuit/commit/70d8496d1d31ec332f09200184229b743d433d45

Also related:
- https://github.com/SanseroGames/LetsGo-OS
