# irmovq 0x142857, %rcx
# irmovq 0x142857, %rdx
# irmovq 0x142857, %r8

# addq %rax, %rbx
# subq %rax, %rcx
# andq %rax, %rdx
# xorq %rax, %r8

# .pos 0
#     irmovq 0x114514, %rax
#     irmovq 0x142857, %rbx
#     subq %rax, %rbx
#     jl target
#     irmovq 0x142857, %rax
#     halt
# .pos 0x13
# target:
#     irmovq 0x114514, %rax
#     halt


# .pos 0
#     irmovq stack, %rsp
#     call main
# .align 8
# main:
#     irmovq 0x114514, %rax
#     halt


# .pos 0x100
# stack:

.pos 0
    irmovq stack, %rsp
    irmovq 0x114514, %rax
    pushq %rax
    popq %rbx
    halt
.pos 0x200
stack: