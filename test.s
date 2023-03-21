irmovq      0xf, 0, 1
irmovq      0xf, 1, 101
irmovq      0xf, 2, 0
irmovq      0xf, 3, 1
irmovq      0xf, 4, 101
irmovq      0xf, 9, 0
loop:
rrmovq      4, 1
add         0, 2
add         3, 0
sub         0, 1
jl          loop
rmmovq      2, 9, 88
