;/*
; * void WFI(void);
; */
    .globl WFI
    .type WFI, %function
WFI:
    MOV R0, #0
    MCR p15, 0, R0, c7, c0, 4
    BX LR

    .globl SET_VECTOR
    .type SET_VECTOR, %function
SET_VECTOR:
    MOV R0, #0x00010000
    MCR p15, 0, R0, c9, c0, 0
    MRC p15, 0, R1, c1, c0, 0
    ORR R1, #(0x1<<13)
    MCR p15, 0, R1, c1, c0, 0
    BX LR

;/*
; * void cp15_enable_alignfault(void)
; */
    .globl cp15_enable_alignfault
    .type cp15_enable_alignfault, %function
cp15_enable_alignfault:
    MRC p15, 0, R0, c1, c0, 0
    ORR R0, R0, #0x02
    MCR p15, 0, R0, c1, c0, 0
    BX LR

;/*
; * void cp15_disable_alignfault(void)
; */
    .globl cp15_disable_alignfault
    .type cp15_disable_alignfault, %function
cp15_disable_alignfault:
    MRC p15, 0, R0, c1, c0, 0
    BIC R0, R0, #0x02
    MCR p15, 0, R0, c1, c0, 0
    BX LR

