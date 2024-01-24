.text  # Section declaration
.global main
main:
loop:
    # Write our string to stdout
    movl $4, %eax  # System call number (4 = sys_write)
    movl $1, %ebx  # 1st param of sys_write: fd, (0 = stdin, 1 = stdout, 2 = stderr)
    lea msg, %ecx  # 2nd param of sys_write: addr, here we load the effective address (lea) of msg
    movl $len, %edx  # 3rd param of sys_write: len
    int $0x80  # Call kernel
    # jmp loop

    # Exit
    movl $60, %eax  # System call number (1 = sys_exit)
    movl $0, %ebx  # 1st param of sys_exit: error_code
    int $0x80  # Call kernel

.data
msg:
    .ascii "Hello 32 World\n"  # Our string
    len = .-msg  # Length of the string
