in
pop         rbx

push        0
pop         rax

START:

push        rax
push        rax
mul
out

push        rax 1
pop         rax

push        rax
push        rbx

jbe         START:

hlt
