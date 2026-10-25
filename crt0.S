    .section .text.start
    .globl _start
_start:
    la   sp, _stack_top
    call main
    li   a7, 93          # exit(a0), Linux syscall number
    ecall