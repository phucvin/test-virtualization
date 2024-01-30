Minimal manual steps:
```
$ sudo apt update && sudo apt install build-essential flex bison xz-utils wget ca-certificates bc linux-headers-generic gcc-multilib
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.7.2.tar.gz
$ tar linux-6.7.2.tar.gz
$ cd tar linux-6.7.2
$ make ARCH=um allnoconfig KCONFIG_ALLCONFIG=../mini.config
$ make ARCH=um
$ ./linux root=/dev/root rootfstype=hostfs rw init=/bin/bash
#$ cd ..
#$ wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-minirootfs-3.19.1-x86_64.tar.gz
#$ mkdir alpine-3.19.1 && tar xf alpine-minirootfs-3.19.1-x86_64.tar.gz -C alpine-3.19.1
#$ cd ..
#$ export TMPDIR=/tmp
#$ ./linux-6.7.2/linux root=/dev/root rootfstype=hostfs rootflags=/workspaces/test-virtualization/uml02/alpine-3.19.1 rw init=/bin/sh
```

Using a simple Dockerfile:
```
```

References:
- https://www.landley.net/code/UML.html
- https://github.com/Xe/furry-happiness