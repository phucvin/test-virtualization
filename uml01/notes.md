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
- Fix current issues, similar to the ones in manual steps above
- Share folder with host and execute host-built binary inside UML
- Add gcc to build & run program inside UML

Run binary built from host:
```
$ cd furry-happiness
$ gcc -static -no-pie -o tracee02 ../../ptrace04/tracee02.c
$ # Add `COPY tracee02 /chroot/home/tracee02` to furry-happiness/Dockerfile, after `COPY init.sh /chroot/init.sh`
$ docker build -t xena/docker-uml .
$ docker run --rm -it xena/docker-uml
# cd home
# ./tracee02
# apk add file
# file tracee02
tracee02: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=e87696a4c5d34915c8bb0ae058df2437c9f5cb59, for GNU/Linux 3.2.0, not stripped
```
Notes:
- Dynamically linked binaries might not work inside UML, if the UML rootfs is different from the host's (e.g. UML uses Alpine, host uses Ubuntu)

References:
- https://www.kernel.org/doc/html/v5.9/virt/uml/user_mode_linux.html
- https://github.com/Xe/furry-happiness
- https://xeiaso.net/blog/howto-usermode-linux-2019-07-07/
- https://gist.github.com/hughpearse/98c1ae466e9462b44e305649fbfd5db2 (Building and running User Mode Linux (UML))
- https://github.com/weber-software/diuid (Docker in User Mode Linux)
- https://www.youtube.com/watch?v=kbMhke7s0VM