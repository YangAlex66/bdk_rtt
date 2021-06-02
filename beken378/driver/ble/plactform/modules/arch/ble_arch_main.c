/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ******** ********************************************************************************
 */

 
/*
 * INCLUDES
 ****************************************************************************************
 */
#include "rwip_config.h" // RW SW configuration

#include "architect.h"      // architectural platform definitions
#include <stdlib.h>    // standard lib functions
#include <stddef.h>    // standard definitions
#include <stdint.h>    // standard integer definition
#include <stdbool.h>   // boolean definition
#include "rwip.h"      // RW SW initialization
#if (BLE_APP_PRESENT)
#include "application.h"       // application functions
#endif // BLE_APP_PRESENT
#include "app_task.h"
#include "ble_uart.h"
#include "app_sdp.h"
#include "RomCallFlash.h"
#include "error.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"
#include "ps_debug_pub.h"
#include "intc_pub.h"
#include "rwapp_config.h"
#include "sys_ctrl_pub.h"
#include "arch.h"
#include "drv_model_pub.h"
#include "drv_model.h"
#include "param_config.h"
#include "sys_ctrl.h"

/**
 ****************************************************************************************
 * @addtogroup DRIVERS
 * @{
 *
 *
 * ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
// NVDS location in FLASH : 0x00041000 (260KB )
#define NVDS_FLASH_ADDRESS          (0x00041000) 

#define NVDS_FLASH_SIZE             (0x00001000) // NVDS SIZE at flash Use 0x1000

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

struct rwip_eif_api uart_api;

extern void ble_uart_read(uint8_t *bufptr, uint32_t size, uint8_t (*callback) (void*, uint8_t), void* dummy);
extern void ble_uart_write(uint8_t *bufptr, uint32_t size, uint8_t (*callback) (void*, uint8_t), void* dummy);
extern void ble_uart_flow_on(void);
extern bool ble_uart_flow_off(void);

beken_semaphore_t ble_interrupt_sema;
 uint8_t ble_deep_sleep = 0;
static uint8_t ble_first_sleep = 1;
 uint8_t ble_init_over = 0;
static uint32_t ble_sleep_enable = 0;
extern uint8_t system_mode;
extern uint8_t system_sleep_flag;


void ble_ps_enable_set(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
	system_sleep_flag = 1;
    ble_sleep_enable = 1;
    GLOBAL_INT_RESTORE();
}

void ble_ps_enable_clear(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ble_sleep_enable = 0;
    GLOBAL_INT_RESTORE();
}

UINT32 ble_ps_enabled(void )
{
    uint32_t value = 0;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    value =  ble_sleep_enable;
    GLOBAL_INT_RESTORE();
    return value;
}
int ble_sem_create(void)
{
    return rtos_init_semaphore(&ble_interrupt_sema, 1);
}

int ble_sem_set(void)
{
    return rtos_set_semaphore(&ble_interrupt_sema);
}

int ble_sem_wait(uint32_t ms)
{
	return rtos_get_semaphore(&ble_interrupt_sema, ms);
}


void rwip_eif_api_init(void)
{
	uart_api.read = &ble_uart_read;
	uart_api.write = &ble_uart_write;
	uart_api.flow_on = &ble_uart_flow_on;
	uart_api.flow_off = &ble_uart_flow_off;
}


#if !(BLE_EMB_PRESENT) && !(BT_EMB_PRESENT)
// Creation of uart second external interface api

#endif // !BLE_EMB_PRESENT && !(BT_EMB_PRESENT)

#if (PLF_DEBUG)
/// Variable to enable infinite loop on assert
volatile int dbg_assert_block = 1;
#endif //PLF_DEBUG


/// Variable storing the reason of platform reset
uint32_t error = RESET_NO_ERROR;

uint32_t critical_sec_cnt = 0;

/*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */

static void Stack_Integrity_Check(void);


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint16_t get_stack_usage(void)
{
#if 0
    uint8_t *ptr = (uint8_t*)(STACK_BASE_SVC);

    while(*ptr++ == BOOT_PATTERN_SVC);

    return (uint16_t)((uint32_t)STACK_BASE_SVC + (uint32_t)STACK_LEN_SVC - (uint32_t)ptr);
#endif
    return 0;
}

void platform_reset(uint32_t error)
{
    void (*pReset)(void);

    // Disable interrupts
    GLOBAL_INT_STOP();
    bk_printf("-----platform_reset----%x\r\n", error);

    if(error == RESET_AND_LOAD_FW || error == RESET_TO_ROM)
    {
        // Not yet supported
    }
    else
    {
        // Restart FW
        pReset = (void * )(0x0);
        pReset();
    }
}

#if CFG_USE_BLE_PS
void bk_ble_sleep_check(void)
{
#if SYSTEM_SLEEP
    if((0 == if_ble_sleep()))
    {
    	uint8_t sleep_type = 0;

    	GLOBAL_INT_DIS();
		// Check if the processor clock can be gated
    	sleep_type = rwip_sleep();

		if(sleep_type &  RW_MCU_DEEP_SLEEP)
    	{	
            ble_deep_sleep = 1;
            PS_DEBUG_RX_TRIGER;
                
            UINT32 reg = RF_HOLD_BY_BLE_BIT;
            sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
            //ble_switch_rf_to_wifi();

            if(ble_first_sleep)
                ble_first_sleep = 0;
		}
        GLOBAL_INT_RES();
      }
#endif
		Stack_Integrity_Check();
}
#endif

/**
 *******************************************************************************
 * @brief RW main function.
 *
 * This function is called right after the booting process has completed.
 *
 * @return status   exit status
 *******************************************************************************
 */

extern struct rom_env_tag rom_env;

void rw_main(void)
{
    /*
     ***************************************************************************
     * Platform initialization
     ***************************************************************************
     */

	em_map_check();

	//*((volatile unsigned long *)(0x0802800 + 0x19 * 4)) = 0x40;
	//*((volatile unsigned long *)(0x0802800 + 0x1C * 4)) = 0x40;
#if CFG_USE_BLE_PS
    sctrl_ble_ps_init();
    ble_ps_enable_set();
#endif
    // Initialize random process
    srand(1);

    rwip_eif_api_init();

#if  (NVDS_SUPPORT)
    // Initialize NVDS module
    struct nvds_env_tag env;
    env.flash_read = &ble_flash_read;
    env.flash_write = &ble_flash_write;
    env.flash_erase = &ble_flash_erase;
	nvds_init(env);
#endif 
    rom_env_init(&rom_env);

    if(!ble_dut_flag)
    {
	    bk_printf("-----rw_main task init----\r\n");
	}
    //krhino_task_sleep(5000);
		
    /*
      ***************************************************************************
      * RW SW stack initialization
      ***************************************************************************
      */
    // Initialize RW SW stack
	rwip_reg_init();
    rwip_init(0);

#if (BLE_APP_SDP)
	appm_set_max_scan_nums(MAX_SCAN_NUM);
#endif
	
    // finally start interrupt handling
  ///  GLOBAL_INT_START();

    if(!ble_dut_flag)
    {
        bk_printf("-----rw_main  start----\r\n");
    }
    /*
     ***************************************************************************
     * Main loop
     ***************************************************************************
     */

    ble_active = 1;
    
    while(1)
    {	
        OSStatus err;
        BLE_MSG_T msg;
        
        err = rtos_pop_from_queue(&ble_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
        	switch(msg.data) 
            {
                case BLE_MSG_POLL:
                {
                  	//schedule all pending events      
                 	rwip_schedule();	  
                }
                break;
                case BLE_MSG_EXIT: 
                {
                    extern uint32_t  ble_dut_flag;
                    if(ble_dut_flag == 1)
                    {
                        ble_dut_flag = 0;
                        extern void uart2_isr(void);
                        extern void uart1_isr(void);
                        extern void intc_service_change_handler(UINT8 int_num, FUNCPTR isr);

                        #if (BLE_DUT_UART_PORT == PORT_UART2)
                        intc_service_change_handler(IRQ_UART2, uart2_isr);
                        #else
                        intc_service_change_handler(IRQ_UART1, uart1_isr);
                        #endif // (BLE_DUT_UART_PORT == PORT_UART2)

                        bk_printf("exit ble dut\r\n");
                    }
                    goto ble_main_exit;
                }
                break;

                default:
                break;
        	}
        }

#if CFG_USE_BLE_PS
        if(ble_init_over
        && ble_ps_enabled()
        && blemsg_is_empty())
        {
            bk_ble_sleep_check();
        }
#endif
    }

ble_main_exit:
    bk_printf("ble main exit\r\n");
    if(ble_ps_enabled())
    {
        GLOBAL_INT_DIS();
        #if CFG_USE_BLE_PS
        UINT32 reg = RF_HOLD_BY_BLE_BIT;
        sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_SET, &reg);
        ble_switch_rf_to_wifi();
        #endif
        ble_deep_sleep = 0;
        ble_first_sleep = 1;
        ble_sleep_enable = 0;
        ble_init_over = 0;
        GLOBAL_INT_RES();
    }
    
    ble_active = 0;
    
	rwip_reg_deinit();
}	



uint8_t if_ble_sleep(void)
{
    if (ble_is_start() == 0) 
    {
        return 1;
    }
    else
    {
#if SYSTEM_SLEEP	
        uint8_t value = 0;
    	if(ble_ps_enabled())
    	{
            GLOBAL_INT_DIS();
            if(!ble_first_sleep)
            {
            value =  ble_deep_sleep;
            }
            GLOBAL_INT_RES();
            return value;
    	}
    	else
    	{
    	    return 0;
    	}
#else
        return 0;
#endif
    }
}

const struct rwip_eif_api* rwip_eif_get(uint8_t type)
{
    const struct rwip_eif_api* ret = NULL;
    switch(type)
    {
        case RWIP_EIF_AHI:
        {
            ret = &uart_api;
        }
        break;
        #if (BLE_EMB_PRESENT) || (BT_EMB_PRESENT)
        case RWIP_EIF_HCIC:
        {
            ret = &uart_api;
        }
        break;
        #elif !(BLE_EMB_PRESENT) || !(BT_EMB_PRESENT)
        case RWIP_EIF_HCIH:
        {
            ret = &uart_api;
        }
        break;
        #endif 
        default:
        {
            BLE_ASSERT_INFO(0, type, 0);
        }
        break;
    }
    
    return ret;
}

static void Stack_Integrity_Check(void)
{

	
	
}

#if SYSTEM_SLEEP	
void ble_globel_prevent_sleep_set(void)
{
    rwip_prevent_sleep_set(RW_GLOBEL_EVENT_ONGOING);
}

void ble_globel_prevent_sleep_clr(void)
{
    rwip_prevent_sleep_clear(RW_GLOBEL_EVENT_ONGOING);
}

static BLE_PS_FORBID_STATUS ble_ps_forbid_code = 0;
static UINT16 ble_ps_forbid_count = 0;
UINT16 ble_ps_forbid_trace(BLE_PS_FORBID_STATUS forbid)
{
    ble_ps_forbid_count ++;

    if(ble_ps_forbid_code != forbid || (ble_ps_forbid_count % 100 == 0))
    {
        //BLE_PS_PRT("ble_forbitd count:%d\r\n\r\n", ble_ps_forbid_count);
        //BLE_PS_PRT("ble_cd:%d %d\r\n", ble_ps_forbid_code,forbid);
        ble_ps_forbid_count = 0;
    }

    ble_ps_forbid_code = forbid;
    return ble_ps_forbid_count;
}

void ble_ps_dump(void)
{
    ble_rwip_ps_dump();
    bk_printf("rf_wifi_used:%x %x %x %x %x %x\r\n",if_rf_wifi_used(),ble_ps_enabled(),
        ble_init_over,if_ble_sleep(),ble_is_start(),ble_deep_sleep);
}
#endif
/// @} DRIVERS
