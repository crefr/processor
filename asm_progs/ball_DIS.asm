IN
POP rbx 
PUSH 96 
POP rcx 
PUSH rcx 
PUSH 36 
POP rdx 
POP rax 
PUSH rax 
PUSH rax 
POP rcx 
PUSH rdx 
PUSH 1 
SUB
POP rdx 
PUSH rdx 
PUSH 0 
JB 83
PUSH rcx 
PUSH 1 
SUB
POP rcx 
PUSH rcx 
PUSH 0 
JB 81
PUSH rdx 
PUSH rdx 
MUL
PUSH rcx 
PUSH rcx 
MUL
ADD
PUSH rbx 
PUSH rbx 
MUL
JBE 73
CALL 87
PUSH 1 
POP [rax ]
JMP 34
CALL 87
PUSH 0 
POP [rax ]
JMP 34
JMP 13
DRAW
DUMP 0 
HLT
POP rax 
PUSH rax 
PUSH rax 
PUSH rdx 
MUL
PUSH rcx 
ADD
POP rax 
RET
