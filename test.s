    .pos 0
    irmovq stack, %rsp
    # irmovq stack, %rbp
    call main
    halt

    .align 8
array:
    .quad 0xaaaa
    .quad 0xbbbb0000
    .quad 0xcccc00000000
    .quad 0xdddd000000000000


main:
    # push %rbp
    # rrmovq %rsp, %rbp

    irmovq array, %rdi
    mrmovq (%rdi), %r8
    pushq %r8
    mrmovq 8(%rdi), %r8
    pushq %r8

    call sum

    popq %r8
    popq %r8




    # pop %rbp
    ret

sum:
    # push %rbp
    # rrmovq %rsp, %rbp

    xorq %rax, %rax
    mrmovq 8(%rsp), %r8
    addq %r8, %rax
    mrmovq 16(%rsp), %r8
    addq %r8, %rax

    ret




    .pos 0x200
stack: