Minimal manual steps:
```
$ sudo apt update && sudo apt install build-essential flex bison xz-utils wget ca-certificates bc linux-headers-generic gcc-multilib
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.7.2.tar.gz
$ tar linux-6.7.2.tar.gz
$ cd tar linux-6.7.2
$ make ARCH=um allnoconfig KCONFIG_ALLCONFIG=../mini.config
$ make ARCH=um
```

Using a simple Dockerfile:
```
```

References:
- https://www.landley.net/code/UML.html
- https://github.com/Xe/furry-happiness