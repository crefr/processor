push    5
pop     rax

push    rax
push    5

JE      TEST:
push    52
out


hlt



TEST:
push    1
out
hlt
