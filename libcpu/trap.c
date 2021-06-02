/*
 * File      : trap.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2013, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <rthw.h>
#include "sys_config.h"
#include "start_type_pub.h"
#include "arm_mcu_pub.h"

#define INT_IRQ     0x00
#define INT_FIQ     0x01

extern struct rt_thread *rt_current_thread;
#ifdef RT_USING_FINSH
extern long list_thread(void);
#endif

struct rt_hw_register
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t fp;
    rt_uint32_t ip;
    rt_uint32_t sp;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t spsr;
    rt_uint32_t cpsr;
};

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */

void rt_hw_show_register (struct rt_hw_register *regs)
{
	rt_kprintf("Current regs:\n");
	rt_kprintf("r00:0x%08x r01:0x%08x r02:0x%08x r03:0x%08x\n",
		regs->r0, regs->r1, regs->r2, regs->r3);
	rt_kprintf("r04:0x%08x r05:0x%08x r06:0x%08x r07:0x%08x\n",
		regs->r4, regs->r5, regs->r6, regs->r7);
	rt_kprintf("r08:0x%08x r09:0x%08x r10:0x%08x\n",
		regs->r8, regs->r9, regs->r10);
	rt_kprintf("fp :0x%08x ip :0x%08x\n",
		regs->fp, regs->ip);
	rt_kprintf("sp :0x%08x lr :0x%08x pc :0x%08x\n",
		regs->sp, regs->lr, regs->pc);
	rt_kprintf("SPSR:0x%08x\n", regs->spsr);
	rt_kprintf("CPSR:0x%08x\n", regs->cpsr);

	const unsigned int *reg_fiq, *reg_irq, *reg_und, *reg_abt, *reg_svc, *reg_sys;
	int i;

	reg_fiq = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_FIQ;
	reg_irq = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_IRQ;
	reg_und = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_UND;
	reg_abt = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_ABT;
	reg_svc = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_SVC;
	reg_sys = (const unsigned int *)MCU_REG_BACKUP_BOTTOM_SYS;

	rt_kprintf("\nseparate regs:\n");
	rt_kprintf(" reg      FIQ        IRQ        UND        ABT        SVC        SYS\r\n");
	rt_kprintf("------ ---------- ---------- ---------- ---------- ---------- ----------\r\n");
	rt_kprintf("%-6s 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n", "cpsr", reg_fiq[0], reg_irq[0],
		reg_und[0], reg_abt[0], reg_svc[0], reg_sys[0]);
	rt_kprintf("%-6s 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n", "spsr", reg_fiq[1], reg_irq[1],
		reg_und[1], reg_abt[1], reg_svc[1], reg_sys[1]);
	rt_kprintf("%-6s 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n", "r13-SP", reg_fiq[7], reg_irq[7],
		reg_und[7], reg_abt[7], reg_svc[7], reg_sys[7]);
	rt_kprintf("%-6s 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n", "r14-LR", reg_fiq[8], reg_irq[8],
		reg_und[8], reg_abt[8], reg_svc[8], reg_sys[8]);

	//display backup R8~R12 of FIQ
	for (i=2; i<7; i++) {
		rt_kprintf("r%-5d 0x%08x %-10s %-10s %-10s %-10s %-10s\r\n", i+6, reg_fiq[i],
			"-", "-", "-", "-", "-");
	}

	rt_kprintf("\r\n");
}

void rt_hw_print_exception_info(const char* exception_str, struct rt_hw_register *regs)
{
	rt_kprintf("%s\n", exception_str);
	rt_hw_show_register(regs);

#ifdef RT_USING_FINSH
	list_thread();
	rt_hw_stack_print_after_exception();
#endif
}

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_udef(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_UNDEFINED_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_UNDEFINED_VALUE;
#endif

    rt_hw_print_exception_info("undefined instruction", regs);
    rt_hw_cpu_shutdown();
}

/**
 * The software interrupt instruction (SWI) is used for entering
 * Supervisor mode, usually to request a particular supervisor
 * function.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_swi(struct rt_hw_register *regs)
{
    rt_hw_print_exception_info("software interrupt", regs);
    rt_hw_cpu_shutdown();
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during an instruction prefetch.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_pabt(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_PREFETCH_ABORT_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_PREFETCH_ABORT_VALUE;
#endif

    rt_hw_print_exception_info("prefetch abort", regs);
    rt_hw_cpu_shutdown();
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_dabt(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_DATA_ABORT_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_DATA_ABORT_VALUE;
#endif

    rt_hw_print_exception_info("data abort", regs);
    rt_hw_cpu_shutdown();
}

/**
 * Normally, system will never reach here
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_resv(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_UNUSED_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_UNUSED_VALUE;
#endif

    rt_hw_print_exception_info("not used", regs);
    rt_hw_cpu_shutdown();
}

extern void rt_interrupt_dispatch(void);

void rt_hw_trap_irq(void)
{   
    rt_interrupt_dispatch();
}

void rt_hw_trap_fiq(void)
{
    rt_interrupt_dispatch();
}
