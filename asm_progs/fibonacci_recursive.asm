in
pop     rbx

call    FIBBONACCI:
push    rax

out

hlt

FIBBONACCI:
    push    rbx
    push    2

    jbe     NUMBER_ONE:
    push    rbx

    push    rbx -1
    pop     rbx

    call    FIBBONACCI:
    push    rax
    pop     rcx

    pop     rbx
    push    rcx

    push    rbx -2
    pop     rbx
    call    FIBBONACCI:
    push    rax

    add
    pop     rax
ret
    NUMBER_ONE:
    push    1
    pop     rax
ret
