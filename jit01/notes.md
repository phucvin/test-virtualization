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
