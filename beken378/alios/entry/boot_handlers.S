/**
 ****************************************************************************************
 *
 * @file boot_handlers.s
 *
 * @brief ARM Exception Vector handler functions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */	
	.globl entry_main
	.globl do_irq
	.globl do_fiq
	.globl do_swi
	.globl boot_reset
	.globl boot_swi
	.globl boot_undefined
	.globl boot_pabort
	.globl boot_dabort
	.globl boot_reserved
	.globl irq_handler
	.globl fiq_handler
	.globl vPortStartFirstTask
	.globl fiq_pre_proc
	.globl fiq_end_proc
	.globl print_exception_addr
	.globl do_undefined
	.globl do_pabort
	.globl do_dabort
	.globl do_reserved

    #include "sys_config.h"
/* ========================================================================
 *                                Macros
 * ======================================================================== */
#define _FIQ_STACK_SIZE_ 	              0x7F0
#define _IRQ_STACK_SIZE_ 	              0xFF0
#define _SVC_STACK_SIZE_ 	              0x3F0
#define _SYS_STACK_SIZE_ 	              0x3F0
#define _UND_STACK_SIZE_ 	              0x010
#define _ABT_STACK_SIZE_ 	              0x010

#define BOOT_MODE_MASK     	              0x1F
#define BOOT_MODE_USR       	          0x10
#define BOOT_MODE_FIQ       	          0x11
#define BOOT_MODE_IRQ      	              0x12
#define BOOT_MODE_SVC    	              0x13
#define BOOT_MODE_ABT     	              0x17
#define BOOT_MODE_UND    	              0x1B
#define BOOT_MODE_SYS     	              0x1F
#define BOOT_FIQ_IRQ_MASK 	              0xC0
#define BOOT_IRQ_MASK       	          0x80

#define BOOT_COLOR_UNUSED  	              0xAAAAAAAA      //Pattern to fill UNUSED stack
#define BOOT_COLOR_SVC     	              0xBBBBBBBB      //Pattern to fill SVC stack
#define BOOT_COLOR_IRQ     	              0xCCCCCCCC      //Pattern to fill IRQ stack
#define BOOT_COLOR_FIQ     	              0xDDDDDDDD      //Pattern to fill FIQ stack
#define BOOT_COLOR_SYS     	              0xEEEEEEEE      //Pattern to fill SYS stack

/* ========================================================================
 Context save and restore macro definitions
 * ======================================================================== */
	
/* ========================================================================*/


/* ========================================================================
/**
 * Macro for switching ARM mode
 */
	.macro	BOOT_CHANGE_MODE, mode, mode_mask
	MRS   R0, CPSR
	BIC   R0, R0, #\mode_mask
	ORR   R0, R0, #\mode
	MSR   CPSR_c, R0
	.endm

/* ========================================================================
/**
 * Macro for setting the stack
 */
	.macro  BOOT_SET_STACK, stackStart, stackLen, color
	LDR   R0, \stackStart
	LDR   R1, \stackLen

	ADD   R1, R1, R0
	MOV   SP, R1        //Set stack pointer

	LDR   R2, =\color

3:
	CMP   R0, R1        //End of stack?
	STRLT R2, [r0]      //Colorize stack word
	ADDLT R0, R0, #4
	BLT   3b           //branch to previous local label
	.endm

	.section .data
	.align 3
	.global und_stack_start
und_stack_start:
	.space _UND_STACK_SIZE_
 
	.align 3
	.global abt_stack_start
abt_stack_start:
	.space _ABT_STACK_SIZE_

	.align 3
	.global fiq_stack_start
fiq_stack_start:
	.space _FIQ_STACK_SIZE_

	.align 3
	.global irq_stack_start
irq_stack_start:
	.space _IRQ_STACK_SIZE_
 
	.align 3
	.global sys_stack_start
sys_stack_start:
	.space _SYS_STACK_SIZE_

	.align 3
	.global svc_stack_start
svc_stack_start:
	.space _SVC_STACK_SIZE_


/* ========================================================================
 *                                Functions
 * ========================================================================

/* ========================================================================
 * Function to handle reset vector
 */
	.globl	boot_reset
	.section ".boot", "ax"

boot_reset:
    //Disable IRQ and FIQ before starting anything
    MRS   R0, CPSR
    ORR   R0, R0, #0xC0
    MSR   CPSR_c, R0

    //Setup all stacks //Note: Abt and Usr mode are not used
	BOOT_CHANGE_MODE BOOT_MODE_SYS BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_SYS boot_stack_len_SYS BOOT_COLOR_SYS

	BOOT_CHANGE_MODE BOOT_MODE_ABT BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_SVC boot_stack_len_SVC BOOT_COLOR_SVC
#if (CFG_SOC_NAME == SOC_BK7231N)
	B AFTER_FLAG
	.org  0xc0 //0x100 - sizeof(section.vector) = 0x100 - 0x40
	.word 0x32374B42
	.word 0x00003133

AFTER_FLAG:
#endif

	BOOT_CHANGE_MODE BOOT_MODE_UND BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_SVC boot_stack_len_SVC BOOT_COLOR_SVC

	BOOT_CHANGE_MODE BOOT_MODE_IRQ BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_IRQ boot_stack_len_IRQ BOOT_COLOR_IRQ

	BOOT_CHANGE_MODE BOOT_MODE_FIQ BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_FIQ boot_stack_len_FIQ BOOT_COLOR_FIQ

	//Clear FIQ banked registers while in FIQ mode
	MOV     R8,  #0
	MOV     R9,  #0
	MOV     R10, #0
	MOV     R11, #0
	MOV     R12, #0

	BOOT_CHANGE_MODE BOOT_MODE_SVC BOOT_MODE_MASK
	BOOT_SET_STACK   boot_stack_base_SVC boot_stack_len_SVC BOOT_COLOR_SVC

    //Stay in Supervisor Mode
    //copy data from binary to ram
    BL _sysboot_copy_data_to_ram
#if (CFG_SOC_NAME == SOC_BK7231N)
    BL _sysboot_copy_code_to_itcm
#endif
    
    ///*Init the BSS section*/
    BL _sysboot_zi_init
#if (CFG_SOC_NAME == SOC_BK7231N)
    BL _sysboot_tcmbss_init
#endif
    
    //==================
    //Clear Registers
    MOV R0, #0
    MOV R1, #0
    MOV R2, #0
    MOV R3, #0
    MOV R4, #0
    MOV R5, #0
    MOV R6, #0
    MOV R7, #0
    MOV R8, #0
    MOV R9, #0
    MOV R10, #0
    MOV R11, #0
    MOV R12, #0

    B entry_main

/* ========================================================================
 *								  Globals
 * ======================================================================== */
boot_stack_base_UNUSED:
	.word und_stack_start

boot_stack_len_UNUSED:
	.word _UND_STACK_SIZE_

boot_stack_base_IRQ:
	.word irq_stack_start

boot_stack_len_IRQ:
	.word _IRQ_STACK_SIZE_

boot_stack_base_SVC:
	.word svc_stack_start

boot_stack_len_SVC:
	 .word _SVC_STACK_SIZE_

boot_stack_base_FIQ:
	 .word fiq_stack_start

boot_stack_len_FIQ:
	.word _FIQ_STACK_SIZE_

boot_stack_base_SYS:
	 .word sys_stack_start

boot_stack_len_SYS:
	 .word _SYS_STACK_SIZE_  

/*FUNCTION:     _sysboot_copy_data_to_ram*/
/*DESCRIPTION:  copy main stack code from FLASH/ROM to SRAM*/
_sysboot_copy_data_to_ram:
    LDR     R0, =_data_flash_begin
    LDR     R1, =_data_ram_begin
    LDR     R2, =_data_ram_end
	
4: CMP R1, R2
    LDRLO   R4, [R0], #4
    STRLO   R4, [R1], #4
    BLO     4b
    BX LR
        
/*FUNCTION:     _sysboot_zi_init*/
/*DESCRIPTION:  Initialise Zero-Init Data Segment*/
_sysboot_zi_init:
    LDR     R0, =_bss_start
    LDR     R1, =_bss_end
    
    MOV R3, R1
    MOV R4, R0
    MOV R2, #0
5: CMP R4, R3
    STRLO R2, [R4], #4
    BLO 5b
    BX LR

#if (CFG_SOC_NAME == SOC_BK7231N)
/*FUNCTION:     _sysboot_copy_code_to_itcm*/
/*DESCRIPTION:  copy itcm code from FLASH/ROM to SRAM*/
_sysboot_copy_code_to_itcm:
    LDR     R0, =_itcmcode_flash_begin
    LDR     R1, =_itcmcode_ram_begin
    LDR     R2, =_itcmcode_ram_end

6: CMP R1, R2
    LDRLO   R4, [R0], #4
    STRLO   R4, [R1], #4
    BLO     6b
    BX LR

/*FUNCTION:     _sysboot_sdbss_init*/
/*DESCRIPTION:  Initialise Zero-Init Data Segment of TCM */
_sysboot_tcmbss_init:
    LDR     R0, =_tcmbss_start
    LDR     R1, =_tcmbss_end

    MOV R3, R1
    MOV R4, R0
    MOV R2, #0
8: CMP R4, R3
    STRLO R2, [R4], #4
    BLO 8b
    BX LR
#endif

		.align	5 
do_undefined:
    LDMFD   SP!, {R0-R1}^
    MOV R0, LR
    MSR     CPSR_c, #0xd3
    MOV R1, LR
    MOV R2, SP
    B print_exception_addr

		.align	5 
boot_swi:
	MOV R0, LR
    MSR     CPSR_c, #0xd3
    MOV R1, LR
    MOV R2, SP
    B print_exception_addr

		.align	5 
do_pabort:
    LDMFD   SP!, {R0-R1}^
    MOV R0, LR
    MSR     CPSR_c, #0xd3
    MOV R1, LR
    MOV R2, SP
    B print_exception_addr

		.align	5 
do_dabort:
    LDMFD   SP!, {R0-R1}^
    MOV R0, LR
    MSR     CPSR_c, #0xd3
    MOV R1, LR
    MOV R2, SP
    B print_exception_addr

		.align	5 
do_reserved:
    LDMFD   SP!, {R0-R1}^
    B boot_reserved

		.align	5 
boot_undefined:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x40000c
	LDR    r0, [R1]
	BX     r0
    
		.align	5 
boot_pabort:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x400010
	LDR    r0, [R1]
	BX     r0

		.align	5 
boot_dabort:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x400014
	LDR    r0, [R1]
	BX     r0

		.align	5 
boot_reserved:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x400018
	LDR    r0, [R1]
	BX     r0

		.align	5
irq_handler:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x400000
	LDR    r0, [R1]
	BX     r0


		.align	5
fiq_handler:
	STMFD  sp!,{r0-r1}
	LDR    R1, =0x400004
	LDR    r0, [R1]
	BX     r0

do_swi:
	B do_swi

do_irq:
    LDMFD   SP!, {R0-R1}^
    STMFD   SP!, {R1-R3}           @We will use R1-R3 as temporary registers

    MOV     R1, SP
    ADD     SP, SP, #12            @Adjust IRQ stack pointer
    SUB     R2, LR, #4             @Adjust PC for return address to task

    MRS     R3, SPSR               @Copy SPSR (Task CPSR)

    MSR     CPSR_c, #0xd3          @Change to SVC mode with interrupt disabled

    /* SAVE TASK'S CONTEXT ONTO OLD TASK'S STACK */
    STMFD   SP!, {R2}              @Push task''s PC      
    STMFD   SP!, {R4-R12, LR}       
    LDMFD   R1!, {R4-R6}           @Load Task''s R1-R3 from IRQ stack
    STMFD   SP!, {R4-R6}           @Push Task''s R1-R3 to SVC stack
    STMFD   SP!, {R0}              @Push Task''s R0 to SVC stack

    STMFD   SP!, {R3}              @Push task''s CPSR

    LDR     R4,=g_active_task            
    LDR     R5,[R4]
    STR     SP,[R5] 

    BL      krhino_intrpt_enter  

    MSR     CPSR_c,#0xd2           @Change to IRQ mode to use IRQ stack to handle interrupt with interrupt disbaled  
 
    BL      intc_irq
    MSR     CPSR_c,#0xd3           @Change to SVC mode with interrupt disabled   
    BL      krhino_intrpt_exit
    LDMFD   SP!,{R4}               @POP the task''s CPSR             
    MSR     SPSR_cxsf,R4
    LDMFD   SP!,{R0-R12,LR,PC}^    @POP new Task''s context


do_fiq:
    LDMFD   SP!, {R0-R1}^
    MRS     R8, spsr               @R8 R9 is unique to fiq
    MRS     R9, spsr               @save SPSR_fiq
    AND     R8, R8, #0x1f
    CMP     R8, #0x12
    BNE     88f
    STMFD   SP!, {R0-R12, LR}

    BL      fiq_pre_proc
    BL      intc_fiq
    BL      fiq_end_proc

    LDMFD   SP!,{R0-R12,LR}

    MSR      SPSR_cxsf,R9         @restore SPSR_fiq
    SUBS PC, LR,#4                @restore CPSR

    88:
    STMFD   SP!, {R1-R3}           @We will use R1-R3 as temporary registers

    MOV     R1, SP
    ADD     SP, SP, #12            @Adjust IRQ stack pointer
    SUB     R2, LR, #4             @Adjust PC for return address to task

    MRS     R3, SPSR               @Copy SPSR (Task CPSR)

    MSR     CPSR_c, #0xd3          @Change to SVC mode with interrupt disabled

    /* SAVE TASK'S CONTEXT ONTO OLD TASK'S STACK */
    STMFD   SP!, {R2}              @Push task''s PC      
    STMFD   SP!, {R4-R12, LR}       
    LDMFD   R1!, {R4-R6}           @Load Task''s R1-R3 from IRQ stack
    STMFD   SP!, {R4-R6}           @Push Task''s R1-R3 to SVC stack
    STMFD   SP!, {R0}              @Push Task''s R0 to SVC stack

    STMFD   SP!, {R3}              @Push task''s CPSR

    LDR     R4,=g_active_task            
    LDR     R5,[R4]
    STR     SP,[R5] 

    BL      krhino_intrpt_enter  

    MSR     CPSR_c,#0xd1           @Change to FIRQ mode to use IRQ stack to handle interrupt with interrupt disbaled
 
    BL      intc_fiq
    MSR     CPSR_c,#0xd3           @Change to SVC mode with interrupt disabled   
    BL      krhino_intrpt_exit
    LDMFD   SP!,{R4}               @POP the task''s CPSR             
    MSR     SPSR_cxsf,R4
    LDMFD   SP!,{R0-R12,LR,PC}^    @POP new Task''s context

    .code 32
    .global WFI
    .type WFI,%function
WFI:
	MOV R0, #0
	MCR p15, 0, R0, c7, c0, 4
	BX LR

	

/*EOF*/

