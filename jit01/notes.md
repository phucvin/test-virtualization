Execute machine/binary code:
```
$ gcc -o execbin01 execbin01.c && ./execbin01 <<< "101110001001101000000010000000000000000011000011"
function returned 666
```
Try modify the 1s and 0s to see different results or errors (e.g. segfault).

```
$ gcc -o execbin02 execbin02.c && ./execbin02
```

```
$ gcc -o execbin03 execbin03.c && ./execbin03
$ strace ./execbin03
```

```
$ gcc -fno-builtin -static -nostdlib -m32  -fomit-frame-pointer nostd01.c -o nostd01 && ./nostd01
```

Test static linking:
```
$ file ./nostd01 && ldd ./nostd01
./nostd01: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), statically linked, BuildID[sha1]=2047fa6c46988f4983e1a1dc3d12f2c50340526d, not stripped
        not a dynamic executable

$ rm ./execbin02 && gcc -no-pie -static -o execbin02 execbin02.c && file ./execbin02 && ldd ./execbin02
./execbin02: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=65c53abe6fea2d896f9f39a83c03b01f5c97a792, for GNU/Linux 3.2.0, not stripped
        not a dynamic executable

$ rm ./execbin02 && gcc -static -o execbin02 execbin02.c && file ./execbin02 && ldd ./execbin02
./execbin02: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, BuildID[sha1]=65c53abe6fea2d896f9f39a83c03b01f5c97a792, for GNU/Linux 3.2.0, not stripped
        not a dynamic executable

$ rm ./execbin02 && gcc -o execbin02 execbin02.c && file ./execbin02 && ldd ./execbin02
./execbin02: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=261b13087a4e81da0e961bb08b3b69dc42468244, for GNU/Linux 3.2.0, not stripped
        linux-vdso.so.1 (0x00007ffdaa77c000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f774d346000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f774d549000)
```

TODO:
- Generate a ELF file (similar to a simple compiler)
- Simple WASM JIT

References:
- https://stackoverflow.com/questions/5267988/how-to-write-and-execute-pure-machine-code-manually-without-containers-like-exe
- https://eli.thegreenplace.net/2013/11/05/how-to-jit-an-introduction
- https://github.com/StanfordPL/x64asm/blob/develop/examples/hello.cc
- https://github.com/wingo/wasm-jit (maybe not really relevant here)
- https://github.com/vshymanskyy/wasm2native
- https://kripken.github.io/blog/wasm/2020/07/27/wasmboxc.html
- https://github.com/jserv/amacc
- https://github.com/Valentin271/tiny-elf & https://github.com/vishen/go-x64-executable
- https://crates.io/crates/faerie
- https://eli.thegreenplace.net/2013/10/17/getting-started-with-libjit-part-1
- https://www.gnu.org/software/libjit/
