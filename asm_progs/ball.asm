in
pop     rbx     ;radius

push    96      ;screen width
pop     rcx
push    rcx     ;width - on the bottom of the stack!
push    36      ;screen heigth
pop     rdx

dump    1

FOR_HEIGHT:
    pop     rax
    push    rax
    push    rax
    pop     rcx

    dump    2

    push    rdx
    push    1
    sub
    pop     rdx

    dump    3

    push    rdx
    push    0
    jb      FOR_HEIGHT_END:

    FOR_WIDTH:
        push    rcx
        push    1
        sub
        pop     rcx



        push    rcx
        push    0
        jb      FOR_WIDTH_END:

        push    rdx
        push    rdx
        mul

        push    rcx
        push    rcx
        mul

        add

        push    rbx
        push    rbx
        mul



        jbe     INSIDE:
            call CALC_ADDR:             ;outside
            dump    39
            push    1
            pop     [rax]
            jmp     END_OF_IF_ELSE:
        INSIDE:
            call CALC_ADDR:
            push    0                   ;inside
            pop     [rax]
        END_OF_IF_ELSE:
        jmp     FOR_WIDTH:

    FOR_WIDTH_END:

    jmp     FOR_HEIGHT:
FOR_HEIGHT_END:
draw

hlt

CALC_ADDR:
    dump    52

    pop     rax
    push    rax
    push    rax     ;now there are 2 widths on the bottom

    push    rdx
    mul

    push    rcx
    add

    pop     rax

ret
