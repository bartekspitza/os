.global _start
_start:
    # A dummy user program to be loaded as binary data in the
    li a7, 2          # SYS_read
    li a0, 0x1000     # buffer
    li a1, 64         # buf len
    ecall             # syscall

1:
    j 1b
