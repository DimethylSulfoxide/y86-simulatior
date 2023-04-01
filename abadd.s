    .pos 0
    irmovq stack, %rsp
    call main
    halt

    .align 8

begin:
    .quad 0x1           # long begin = 1
end:
    .quad 100           # long begin = 100

main:
    irmovq begin, %rdi  # rdi = &begin
    irmovq end, %rsi    # rsi = &end
    call sum            # sum(1, 100)
    ret


sum:
    irmovq $1, %r8      # r8 = 1
    mrmovq (%rdi), r9   # r9 = begin
    mrmovq (%rsi), r10  # r10 = end
    xorq %rax, %rax     # rax = 0
    andq %r8, %r8       # set flags
    jmp test            # goto test

loop:

    addq %r10, %rax     # rax += end
    subq %r8, %r10      # end -= 1
    rrmovq %r10, %r11   # r11 = end
    subq %r9, %r11      # r11 - begin

    

test:
    jge loop            # stop when start > end
    ret                 # return


    .pos 0x200
stack: