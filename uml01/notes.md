User Mode Linux (UML) 01

Manual steps:
- Follow https://gist.github.com/hughpearse/98c1ae466e9462b44e305649fbfd5db2
- The `.config` file is copied here
Current issues:
- Cannot ping the internet (e.g. `ping 8.8.8.8` is not working)
- Copied a binary (e.g. `gvisor02/userfaultfdtest01`) to rootfs but keep getting `not found` when `exec` it

Use docker:
```
$ git clone https://github.com/Xe/furry-happiness.git
$ cd furry-happiness
$ git checkout 7ba319db25e37279db7600bebf6651375902155a
$ docker build -t xena/docker-uml .  # This might take 10+ minutes
$ docker run --rm -it xena/docker-uml
$ apk update
$ apk add curl
$ curl google.com  # This works
$ curl https://www.google.com  # This doesn't work, probably related to HTTPS protocol
```
TODO:
- Try different options for networking (e.g. TUN/TAP, pcap, see https://www.kernel.org/doc/html/v5.9/virt/uml/user_mode_linux.html#setting-up-the-network)
- Try different rootfs (e.g. https://cdimage.ubuntu.com/ubuntu-base/releases/23.10/release/)
- Fix current issues, similar to the ones in manual steps above
- Share folder with host and execute host-built binary inside UML
- Add gcc to build & run program inside UML

Run binary built from host:
```
$ cd furry-happiness
$ gcc -static -no-pie -o tracee02_sl ../../ptrace04/tracee02.c
$ gcc -o tracee02_dl ../../ptrace04/tracee02.c
$ # Add `COPY tracee02_sl /chroot/home/tracee02_sl` to furry-happiness/Dockerfile, after `COPY init.sh /chroot/init.sh`
$ docker build -t xena/docker-uml .
$ docker run --rm -it xena/docker-uml
# cd home
# ./tracee02_sl
<OK>
# apk add file
# file tracee02_sl
tracee02_sl: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=e87696a4c5d34915c8bb0ae058df2437c9f5cb59, for GNU/Linux 3.2.0, not stripped
# ./tracee02_dl
/bin/sh: ./tracee02_dl: not found
# ldd tracee02_dl 
        /lib64/ld-linux-x86-64.so.2 (0x40000000)
        libc.so.6 => /lib64/ld-linux-x86-64.so.2 (0x40000000)
# apk add gcompat  # install ld-linux-x86-64.so.2 into /lib 
# ln -s /lib /lib64  # link /lib to /lib64 since this is the folder tracee02_dl uses
# ./tracee02_dl
<OK now>
```
Notes:
- Dynamically linked binaries might not work inside UML, if the UML rootfs is different from the host's (e.g. UML uses Alpine, host uses Ubuntu). Need `apk add gcompat` (see https://wiki.alpinelinux.org/wiki/Running_glibc_programs) and some copying/linking to satisfy the host's binary regarding libraries.

Trying some other rootfs:
- https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-minirootfs-3.19.1-x86_64.tar.gz -> OK
- https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86/alpine-minirootfs-3.19.1-x86.tar.gz -> Error (using 32-bit rootfs in a 64-bit host OS)
- https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/armv7/alpine-minirootfs-3.19.1-armv7.tar.gz -> Error (using ARM rootfs on a x86_64 host OS)
- https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/aarch64/alpine-minirootfs-3.19.1-aarch64.tar.gz -> Error (using ARM rootfs on a x86_64 host OS)
- https://cdimage.ubuntu.com/ubuntu-base/releases/23.10/release/ ubuntu-base-23.10-base-amd64.tar.gz -> init.sh error (see fix below)

Use Ubuntu rootfs:
- In furry-happiness/Dockerfile:
- Use https://cdimage.ubuntu.com/ubuntu-base/releases/23.10/release/ubuntu-base-23.10-base-amd64.tar.gz instead of `alpine-minirootfs`
- Replace `init=/init.sh` with `init=/bin/bash`
- Build and run the docker image again
- tracee02 if built GitHub Codespaces, should works inside UML now

TODO:
- Try building UML in 32-bit (x86) and running it inside a 64-bit (x86_64) host OS
- Try building UML in a different architecture (e.g. ARM, RISC-V) and running it inside a x86_64 host OS

References:
- https://www.kernel.org/doc/html/v5.9/virt/uml/user_mode_linux.html
- https://github.com/Xe/furry-happiness
- https://xeiaso.net/blog/howto-usermode-linux-2019-07-07/
- https://gist.github.com/hughpearse/98c1ae466e9462b44e305649fbfd5db2 (Building and running User Mode Linux (UML))
- https://github.com/weber-software/diuid (Docker in User Mode Linux)
- https://www.youtube.com/watch?v=kbMhke7s0VM
- https://stackoverflow.com/questions/14510619/how-to-build-uml-user-mode-linux-in-32-bit-mode-on-64-bit-host