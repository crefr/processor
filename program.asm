in
pop         rax
call        PRINT:

hlt

PRINT:
    push        rax
    out
    call        PRINT:
ret
