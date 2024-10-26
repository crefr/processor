call SCAN_COEFS:
call SOLVE_QUADRATIC:

hlt


SCAN_COEFS:
    in                      ;scanning a coef
    pop     [0]

    in                      ;scanning b coef
    pop     [1]

    in                      ;scanning c coef
    pop     [2]
ret

SOLVE_QUADRATIC:
    push 0
    push [0]
    JE          LIN:

    call CALC_DISCR:
    push        rax
    pop         rbx

    push        rbx
    push        0
    JB          ERR:

    call CALC_MIN_ROOT:
    push        rax
    pop         [3]

    call CALC_MAX_ROOT:
    push        rax
    pop         [4]

    call PRINT_TWO_ROOTS:
ret
  LIN:
    call SOLVE_LINEAR:
    push        rax
    pop         [3]
    call PRINT_ONE_ROOT:
ret
  ERR:
    call PRINT_ERROR:
ret

SOLVE_LINEAR:
    push        0
    push        [2]
    sub

    push        [1]

    div
    pop         rax
ret

CALC_DISCR:
    push        [1]        ;b
    push        [1]        ;b
    mul

    push        4
    push        [0]        ;a
    push        [2]        ;c
    mul
    mul

    sub

    pop         rax
ret

CALC_MIN_ROOT:
    push    0
    push    [1]
    sub                     ;-b

    push    rbx             ;discriminant
    sqrt

    sub                     ;-b - sqrt(D)

    push    2
    push    [0]
    mul                     ;2a

    div
    pop     rax
ret

CALC_MAX_ROOT:
    push    0
    push    [1]
    sub                     ;-b

    push    rbx             ;discriminant
    sqrt

    add                     ;-b + sqrt(D)

    push    2
    push    [0]
    mul                     ;2a

    div
    pop     rax
ret

PRINT_TWO_ROOTS:
    push        [3]
    out

    push        [4]
    out
ret

PRINT_ONE_ROOT:
    push        [3]
    out
ret

PRINT_ERROR:
    push        5252
    out
ret
