# 寄存器
15
- rax rcx rdx rbx
- rsp rbp
- rsi rdi
- r8 - r14

width: 64bit (int64_t)

rsp: stack pointer

# 条件码
- zf, sf, of
- width: 1bit


# 指令

## movq
- 子指令
  - irmovq
  - rrmovq
  - mrmovq
  - rmmovq

*i立即数 r寄存器 m内存*

- 寻址方式
  - 简单的基址和偏移量

- misc
  - 不允许m2m， i2m

## 整数操作指令 OPq
- 子指令
  - addq
  - subq
  - andq
  - xorq
- 对象
  - 仅仅寄存器数据
- misc
  - 设置三个条件码，zf零，sf符号，of溢出

## 跳转指令

- 子指令
  - jmp
  - jle
  - jl
  - je
  - jne
  - jge
  - jg
- 绝对地址寻址


## 条件传送指令

- 子指令
  - cmovl
  - cmovle
  - cmove
  - cmovne
  - cmovg
  - cmovge


## 调用

- call返回地址入栈，跳到目的地址
- ret返回

## 栈

- pushq
- popq

## misc

- halt停止处理器执行，并将状态码设置为hlt

![instructions encpding](instructions.png)

![reg encode](regencode.png)

![state code](statcode.png)

