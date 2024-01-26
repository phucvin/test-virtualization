Execute machine/binary code:
```
$ gcc -o execbin execbin.c && ./execbin <<< "101110001001101000000010000000000000000011000011"
function returned 666
```
Try modify the 1s and 0s to see different results or errors (e.g. segfault).


TODO:
- Simple WASM JIT

References:
- https://stackoverflow.com/questions/5267988/how-to-write-and-execute-pure-machine-code-manually-without-containers-like-exe
- https://eli.thegreenplace.net/2013/11/05/how-to-jit-an-introduction
- https://github.com/wingo/wasm-jit
