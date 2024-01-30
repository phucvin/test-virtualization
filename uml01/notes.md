User Mode Linux (UML) 01

Manual steps:
- Follow https://gist.github.com/hughpearse/98c1ae466e9462b44e305649fbfd5db2
- The `.config` file is copied here
Current issues:
- Cannot connect to internet (e.g. `ping 8.8.8.8` is not working)
- Copied a binary (e.g. `gvisor02/userfaultfdtest01`) to rootfs but keep getting `not found` when `exec` it

TODO:
- Create a Dockerfile that build and boot UML so it can be easily reproduced

References:
- https://www.kernel.org/doc/html/v5.9/virt/uml/user_mode_linux.html
- https://xeiaso.net/blog/howto-usermode-linux-2019-07-07/
- https://gist.github.com/hughpearse/98c1ae466e9462b44e305649fbfd5db2 (Building and running User Mode Linux (UML))
- https://github.com/weber-software/diuid (Docker in User Mode Linux)