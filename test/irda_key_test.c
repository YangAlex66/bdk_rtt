#include <stdio.h>
#include <stdint.h>
#include <rtthread.h>

#include "include.h"
#include "arm_arch.h"
#include "irda_pub.h"
#include "saradc_intf.h"
#include "sys_ctrl_pub.h"
#include "test_config.h"

#ifdef  IRDA_KEY_TEST 

#if CFG_USE_IRDA

typedef void (*irkey_handle_func)(void) ;
#define	IR_USER_CODE 	0x7f80 /*each remote-contrl unit has its own user_code*/
#define	IR_CODE_SIZE 	0xde
enum
{
	IR_KEY_POWEROFF = 0,/*0xdd*/
	IR_KEY_POWERON, 	/*0x9c*/
	IR_KEY_VOL_PLUS,	/*0x99*/
	IR_KEY_VOL_MINUS,   /*0xc5*/
	IR_KEY_PREV,		/*0xce*/
	IR_KEY_NEXT,		/*0xd2*/
	IR_KEY_PLAY,		/*0xc1*/
	IR_KEY_MAX,
};
const static UINT8 IR_key_map[IR_CODE_SIZE] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,IR_KEY_VOL_PLUS,0xff,0xff,IR_KEY_POWERON,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,IR_KEY_PLAY,0xff,0xff,0xff,IR_KEY_VOL_MINUS,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,IR_KEY_PREV,0xff,
	0xff,0xff,IR_KEY_NEXT,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,IR_KEY_POWEROFF
};
/*IR_KEY_POWEROFF*/
static void poweroff_short_handler(void)
{
	rt_kprintf("key poweroff short handler\r\n");
}
static void poweroff_long_handler(void)
{
	rt_kprintf("key poweroff long handler\r\n");
}
static void poweroff_hold_handler(void)
{
	rt_kprintf("key poweroff hold handler\r\n");
}
/*IR_KEY_POWERON*/
static void poweron_short_handler(void)
{
	rt_kprintf("key poweron short handler\r\n");
}
static void poweron_long_handler(void)
{
	rt_kprintf("key poweron long handler\r\n");
}
static void poweron_hold_handler(void)
{
	rt_kprintf("key poweron hold handler\r\n");
}
/*IR_KEY_VOL_PLUS*/
static void volp_short_handler(void)
{
	rt_kprintf("key volp short handler\r\n");
}
static void volp_long_handler(void)
{
	rt_kprintf("key volp long handler\r\n");
}
static void volp_hold_handler(void)
{
	rt_kprintf("key volp hold handler\r\n");
}
/*IR_KEY_VOL_MINUS*/
static void volm_short_handler(void)
{
	rt_kprintf("key volm short handler\r\n");
}
static void volm_long_handler(void)
{
	rt_kprintf("key volm long handler\r\n");
}
static void volm_hold_handler(void)
{
	rt_kprintf("key volm hold handler\r\n");
}
/*IR_KEY_PREV*/
static void prev_short_handler(void)
{
	rt_kprintf("key prev short handler\r\n");
}
static void prev_long_handler(void)
{
	rt_kprintf("key prev long handler\r\n");
}
static void prev_hold_handler(void)
{
	rt_kprintf("key prev hold handler\r\n");
}
/*IR_KEY_NEXT*/
static void next_short_handler(void)
{
	rt_kprintf("key next short handler\r\n");
}
static void next_long_handler(void)
{
	rt_kprintf("key next long handler\r\n");
}
static void next_hold_handler(void)
{
	rt_kprintf("key next hold handler\r\n");
}

/*IR_KEY_PLAY*/
static void play_short_handler(void)
{
	rt_kprintf("key play short handler\r\n");
}
static void play_long_handler(void)
{
	rt_kprintf("key play long handler\r\n");
}
static void play_hold_handler(void)
{
	rt_kprintf("key play hold handler\r\n");
}


const static irkey_handle_func IRKey_handler[IR_KEY_TYPE_MAX][IR_KEY_MAX] =
{
	{
		poweroff_short_handler,
		poweron_short_handler,
		volp_short_handler,
		volm_short_handler,
		prev_short_handler,
		next_short_handler,
		play_short_handler
	},
	
	{
		poweroff_long_handler,
		poweron_long_handler,
		volp_long_handler,
		volm_long_handler,
		prev_long_handler,
		next_long_handler,
		play_long_handler
	},
	
	{
		poweroff_hold_handler,
		poweron_hold_handler,
		volp_hold_handler,
		volm_hold_handler,
		prev_hold_handler,
		next_hold_handler,
		play_hold_handler
	}
};


void ir_key_handle_thread(void *parameter)
{
	UINT32 IR_msg;
	UINT8 key_type;
	UINT8 key_value;
	UINT8 ir_code;
	while(1)
	{
		if(IR_get_key(&IR_msg, sizeof(IR_msg), RT_WAITING_FOREVER) == RT_EOK)
		{
			key_type = GET_KEY_TYPE(IR_msg);
			ir_code = GET_KEY_VALUE(IR_msg);
			if(ir_code < IR_CODE_SIZE)
			{
				key_value = IR_key_map[ir_code];
				if((key_type < IR_KEY_TYPE_MAX) && (key_value < IR_KEY_MAX))
				{
					IRKey_handler[key_type][key_value]();
				}
			}
		}
	}
}

void irda_key_start(void)
{
	rt_thread_t tid;
	
	saradc_config_vddram_voltage(PSRAM_VDD_3_3V);//use vddram for IRDA triode power supply
	set_irda_usrcode(IR_USER_CODE);
	Irda_init_app();

	tid = rt_thread_create("ir_key",ir_key_handle_thread,NULL,1024,15,10);
	if(tid != NULL)
		rt_thread_startup(tid );
}

MSH_CMD_EXPORT(irda_key_start,irda key);
#endif

#endif
