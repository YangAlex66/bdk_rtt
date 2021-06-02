/**
 ****************************************************************************************
 *
 * @file lowlevel.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef LOWLEVEL_H_
#define LOWLEVEL_H_



#include <stdint.h>


#include "ble_compiler.h"

/* * @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */
#if (!defined(CFG_SUPPORT_RTT))
#define GLOBAL_INT_DIS(); 		\
do { 								\
    uint32_t  fiq_tmp; 				\
	  uint32_t  irq_tmp; 			\
		fiq_tmp = portDISABLE_FIQ();\
		irq_tmp = portDISABLE_IRQ();

#define GLOBAL_INT_RES();		\
			if(!fiq_tmp)			\
			{                     	\
				portENABLE_FIQ(); 	\
			}                     	\
			if(!irq_tmp)           	\
			{                      	\
				portENABLE_IRQ();	\
			}                     	\
} while(0) ;

#else
extern long rt_hw_interrupt_disable(void);
extern void rt_hw_interrupt_enable(long level);

#define GLOBAL_INT_DIS();		\
			do {								\
				uint32_t  fiq_irq_tmp = rt_hw_interrupt_disable();

#define GLOBAL_INT_RES();		\
				rt_hw_interrupt_enable(fiq_irq_tmp);\
			} while(0);

#endif


/** @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *
 */


#endif // LOWLEVEL_H_


