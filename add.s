LOAD r0 0x1
LOAD r1 [n]
LOAD r2 0x10

loop:
TEST r1 r2
JUMP EQ [end]
ADD r1 r0
JUMP [loop]

end:
HALT

n: 0x2
