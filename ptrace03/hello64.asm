global _start

section .text
_start:
    ; write message
    mov rax, 1  ; system call for write
    mov rdi, 1  ; file handle 1 is stdout
    mov rsi, message  ; address of string to output
    mov rdx, 15  ; number of bytes
    syscall  ; call kernel

    ; exit
    mov rax, 60  ; system call for exit
    mov rdi, 0  ; exit code 0
    syscall  ; call kernel

section .data
message:
    db "Hello 64 World", 10 ; note the newline (10) at the end
