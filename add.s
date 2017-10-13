COPY r0 0x1
COPY r1 0x0
LOAD r2 [n]
COPY r3 0x1

loop:
TEST r1 r2
JUMP EQ [end]

ADD r0 r0
ADD r1 r3
JUMP [loop]

end:
HALT

n: 0x2
