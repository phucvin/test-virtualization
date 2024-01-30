Minimal manual steps:
```
$ sudo apt update && sudo apt install build-essential flex bison xz-utils wget ca-certificates bc linux-headers-generic gcc-multilib
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.7.2.tar.gz
$ tar linux-6.7.2.tar.gz
$ cd linux-6.7.2
$ cp ../config01.config ./.config
$ make ARCH=um
$ export TMPDIR=/tmp && ./linux root=/dev/root rootfstype=hostfs rw init=/bin/bash
$ uname -av
$ ls /  # should be similar to the host since UML is using host's root in this config
```

Optional testing commands:
```
$ cd /workspaces/test-virtualization/ptrace04
$ gcc -o tracee02 tracee02.c && ./tracee02
```

Enable userfaultfd & test:
```
$ cp ../config02.config ./.config  # Has CONFIG_USERFAULTFD=y
$ make ARCH=um
$ export TMPDIR=/tmp && ./linux root=/dev/root rootfstype=hostfs rw init=/bin/bash
$ cd /workspaces/test-virtualization/gvsior02
$ gcc -o userfaultfdtest01 userfaultfdtest01.c
$ ./userfaultfdtest01
++ unexpected UFFD ioctls: 11c.
-- handled page fault.
-- handled page fault.
```

Using a simple Dockerfile:
```
```

References:
- https://www.landley.net/code/UML.html
- https://github.com/Xe/furry-happiness
