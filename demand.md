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

# 其余要求

1. 能够输出寄存器，储存器的值
   
   - 类似gdb调试的窗口，同时增加list指令

2. 正确性检验
   - 增加输出指定信息的函数
   - 脚本检验


# todo list
1. 重构执行指令函数
   - 所有类型的指令函数参数统一,均为所需参数的指针(同时都返回void)
   - 将上述函数的指针存到一个数组里
   - exec-single-instruction中通过调用函数将所有参数进行解码,传参,不存在的参数就给null