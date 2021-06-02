/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) Beken Corp 2011-2020
 *
 ****************************************************************************************
 */
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "mem_pub.h"
#include "uart_pub.h"
#include "app.h"
#include "ate_app.h"
#include "k_api.h"

void print_exception_addr(unsigned int pc, unsigned int lr, unsigned int sp)
{
    cpu_intrpt_save();
    os_printf("pc is %x, lr is %x, sp is %x\n", pc, lr, sp);
    while (1);
}

beken_mutex_t stdio_tx_mutex;

extern void hw_start_hal(void);
extern void fclk_init(void);
extern void test_case_task_start(void);


static int test_cnt;

void task_test3(void *arg)
{
	beken_semaphore_t sem;

	rtos_init_semaphore(&sem, 0);
    while (1) {
        os_printf("test_cnt is %d\r\n", test_cnt++);
		rtos_get_semaphore(&sem, 1000);
    }
}

void entry_main(void)
{
    sys_start();
}

void soc_driver_init(void)
{
	#if ATE_APP_FUN
    ate_app_init();
	#endif

    driver_init();
}

void soc_system_init(void)
{
    func_init_basic();
	
	func_init_extended();

    fclk_init();

    hal_init();

#ifndef AOS_NO_WIFI
    app_start();

    hw_start_hal();
#endif
}

// eof

