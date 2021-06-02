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
#include "app.h"
#include "ate_app.h"
#include "start_type_pub.h"

beken_semaphore_t extended_app_sema = NULL;
uint32_t  extended_app_stack_size = 2048;

extern void user_main_entry(void);

void extended_app_launch_over(void)
{  
	OSStatus ret;
	ret = rtos_set_semaphore(&extended_app_sema);
	
	(void)ret;
}
    
void extended_app_waiting_for_launch(void)
{
	OSStatus ret;

	ret = rtos_get_semaphore(&extended_app_sema, BEKEN_WAIT_FOREVER);
	ASSERT(kNoErr == ret);

	(void)ret;
}

static void extended_app_task_handler(void *arg)
{
    /* step 0: function layer initialization*/
    func_init_extended();  

    /* step 1: startup application layer*/
    if(get_ate_mode_state())
    {
	    ate_start();
    }
    else
    {
	    app_start();
    }
         
	extended_app_launch_over();
	
    rtos_delete_thread( NULL );
}

void extended_app_init(void)
{
	OSStatus ret;
	
    ret = rtos_init_semaphore(&extended_app_sema, 1);	
	ASSERT(kNoErr == ret);
}

void extended_app_uninit(void)
{
	OSStatus ret;
	
    ret = rtos_deinit_semaphore(&extended_app_sema);	
	ASSERT(kNoErr == ret);
}

void extended_app_launch(void)
{
	OSStatus ret;
	
	ret = rtos_create_thread(NULL,
					   THD_EXTENDED_APP_PRIORITY,
					   "extended_app",
					   (beken_thread_function_t)extended_app_task_handler,
					   extended_app_stack_size,
					   (beken_thread_arg_t)0);
	ASSERT(kNoErr == ret);
}

void entry_main(void)
{  
    ate_app_init();
	
    bk_misc_init_start_type();
    /* step 1: driver layer initialization*/
    driver_init();
	bk_misc_check_start_type();
	func_init_basic();

	extended_app_init();
    /* step 2: user entry
       attention: if the user wants to increase boot time and light up the board, you may invoke the
       			  routine:user_main_entry before invoking extended_app_launch; and then user MUST wait 
       			  for the init of most other devices, such as:wlan and so on;
       			  
       			  if you do not care the boot time, the function: extended_app_launch shall be invoked 
       			  BEFORE user_main_entry;*/
	user_main_entry();	
	
    /* step 3: init of the most of devices*/
	extended_app_launch();
	
#if (CFG_OS_FREERTOS)
    vTaskStartScheduler();
#endif
}
// eof

