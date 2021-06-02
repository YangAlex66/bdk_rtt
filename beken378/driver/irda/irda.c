#include "include.h"
#include "arm_arch.h"

#include "irda.h"
#include "irda_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"
#include "bk_timer_pub.h"
#include "rtos_pub.h"

struct IR_KEY_ST
{
	beken_queue_t IRkey_mq; 
	
	UINT16 IR_UserCode;
	UINT8 valid_flag;
	UINT8 IR_key_code;
	UINT8 repeat_flag;
	UINT8 repeat_cnt;
	UINT8 timer_cnt;
};

static struct IR_KEY_ST IR_key = {0};


static UINT32 irda_ctrl(UINT32 cmd, void *param);
static SDD_OPERATIONS irda_op = {
            irda_ctrl
};


static int IR_send_key(UINT32 msg)
{
	if(NULL != IR_key.IRkey_mq)
		return rtos_push_to_queue(&IR_key.IRkey_mq, (void *)&msg, sizeof(msg));
	else
		return -1;
}

static void irda_key_timer_hdl(UINT8 param)
{
#define SND_KEY (1 << 0)
#define STOP_TIMER (1 << 1)

    UINT32 	channel = IRDA_KEY_HTIMER_CHNAL;
	static 	UINT8 hold_flag = 0;
	UINT8  	key_type = 0;
	UINT8	op_flag = 0;
	
	IR_key.timer_cnt ++;
	if(IR_key.timer_cnt > IR_key.repeat_cnt)
	{
		if(IR_key.repeat_cnt < KEY_SHORT_CNT)
		{
			key_type = IR_KEY_TYPE_SHORT;
			op_flag = SND_KEY|STOP_TIMER;
		}
		else if(IR_key.repeat_cnt < KEY_LONG_CNT)
		{
			key_type = IR_KEY_TYPE_LONG;
			op_flag = SND_KEY|STOP_TIMER;
		}
		else
		{
			if(hold_flag == 0)
			{
				key_type = IR_KEY_TYPE_HOLD;
				op_flag = SND_KEY|STOP_TIMER;
			}
			else
			{
				op_flag = STOP_TIMER;
			}
		}
		hold_flag = 0;
	}
	else
	{
		if(IR_key.repeat_cnt >= KEY_HOLD_CNT)
		{
			hold_flag = 1;
			op_flag = SND_KEY;
			
			key_type = IR_KEY_TYPE_HOLD;
			IR_key.repeat_cnt = KEY_LONG_CNT;
			IR_key.timer_cnt = KEY_LONG_CNT;
		}
	}

	if(op_flag & SND_KEY)
		IR_send_key(GENERATE_KEY(key_type, IR_key.IR_key_code));
	if(op_flag & STOP_TIMER)
		sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_DISABLE, &channel);
}

static void start_irda_handle_timer(UINT32 period)
{
	timer_param_t param;
    
	param.channel = IRDA_KEY_HTIMER_CHNAL;
	param.div = 1;
	param.period = period;
	param.t_Int_Handler= irda_key_timer_hdl;
        
	sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);    
}

static void irda_active(UINT8 enable)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	if(enable)
	{
		value |= IRDA_NEC_EN;
	}
	else
	{
		value &= ~IRDA_NEC_EN;
	}
	REG_WRITE(IRDA_CTRL, value);
}

static void irda_set_polarity(UINT8 polarity)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	if(polarity){
		value |= IRDA_POLARITY;
	}else{
		value &= ~IRDA_POLARITY;
	}
	REG_WRITE(IRDA_CTRL, value);
}

static void irda_set_clk(UINT16 clk)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	value &= ~(CLK_DIVID_MASK << CLK_DIVID_POSI);
	value |= (clk << CLK_DIVID_POSI);
	REG_WRITE(IRDA_CTRL, value);
}
static void irda_set_int_mask(UINT8 int_en_bits)
{
	UINT32 value;
	value = REG_READ(IRDA_INT_MASK);
	value &= ~INT_MASK_EN;
	value |= int_en_bits;
	REG_WRITE(IRDA_INT_MASK, value);
}

static void trng_active(UINT8 enable)
{
}

static UINT32 trng_get_random(void)
{
	UINT32 reg, value;

	reg = REG_READ(TRNG_CTRL);
	reg |= TRNG_EN;
	REG_WRITE(TRNG_CTRL, reg);

	value = REG_READ(TRNG_DATA);

	reg = REG_READ(TRNG_CTRL);
	reg &= ~TRNG_EN;
	REG_WRITE(TRNG_CTRL, reg);

	return value;
}

static UINT32 irda_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = IRDA_SUCCESS;

	switch(cmd)
	{
		case IRDA_CMD_ACTIVE:
			irda_active(*(UINT8 *)param);
			break;
		case IRDA_CMD_SET_POLARITY:
			irda_set_polarity(*(UINT8 *)param);
			break;
		case IRDA_CMD_SET_CLK:
			irda_set_clk(*(UINT16 *)param);
			break;
		case IRDA_CMD_SET_INT_MASK:
			irda_set_int_mask(*(UINT8*)param);
			break;
		case TRNG_CMD_GET:
			*(UINT32 *)param = trng_get_random();
			break;
		default:
			ret = IRDA_FAILURE;
			break;
	}

	return ret;
}

long IR_get_key(void *buffer, unsigned long  size, INT32 timeout)
{
	OSStatus ret;

	ret = rtos_pop_from_queue(&IR_key.IRkey_mq, buffer, timeout);

	//TODO double check whether need to to return value convert!!!
	return ret;
}

void irda_init(void)
{
	intc_service_register(IRQ_IRDA, PRI_IRQ_IRDA, irda_isr); 
	sddev_register_dev(IRDA_DEV_NAME, &irda_op);
	
	trng_active(1);
}

void irda_exit(void)
{
	sddev_unregister_dev(IRDA_DEV_NAME);
}

void set_irda_usrcode(UINT16 ir_usercode)
{
	IR_key.IR_UserCode = ir_usercode;
}

void Irda_init_app(void)
{
	OSStatus ret;
	UINT32 param;

	IR_key.valid_flag = 0;
	ret = rtos_init_queue(&IR_key.IRkey_mq, "ir_mq", 4, 3);
	if(kNoErr != ret) {
		os_printf("create ir mq error!!\r\n");
		return;
	}

	param = PCLK_POSI_IRDA;
	sddev_control(ICU_DEV_NAME,CMD_CONF_PCLK_26M,&param);//irda clk:26M
	
	param = PWD_IRDA_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);//clk power up

	param = GFUNC_MODE_IRDA;
	sddev_control(GPIO_DEV_NAME,CMD_GPIO_ENABLE_SECOND,&param);//gpio config

	irda_set_polarity(0);//polarity set: low effective
	irda_active(1);//NEC IRDA enable
	irda_set_clk(0x3921);//irda_clk*562.5 (26*562.5 = 0x3921)
	irda_set_int_mask(IRDA_RIGHT_INT_MASK|IRDA_REPEAT_INT_MASK|IRDA_END_INT_MASK);
	
    /*interrupt setting about IRDA*/
    intc_enable(IRQ_IRDA);
    param = GINTR_IRQ_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_ENABLE, &param);
}


void irda_isr(void)
{
#define REPEAT_KEY_TIME_INTERVAL 112 /*bigger than 108 ms*/

	UINT32 irda_int;
	UINT32 end_int, right_int, repeat_int;
	UINT32 tmp;

	irda_int = REG_READ(IRDA_INT);
	end_int = irda_int&IRDA_END_INT;
	right_int = irda_int&IRDA_RIGHT_INT;
	repeat_int = irda_int&IRDA_REPEAT_INT;
	REG_WRITE(IRDA_INT, irda_int);
	if(right_int)
	{ 
		//leader code is received
  	    IR_key.valid_flag = 1;
		IR_key.repeat_flag = 0;
		IR_key.repeat_cnt = 0;
    }
	
    if(end_int)
	{
	//custom code and data code are received (32bits)
    	if(IR_key.valid_flag)
		{
		 	tmp = REG_READ(RX_FIFO_DOUT);
			
			bk_printf("ir value:%x\r\n",tmp);
			
			if( ((tmp & USERCODE_MASK) != IR_key.IR_UserCode) ||
		  	((((tmp&KEY_CODE_INVERS_MASK)>>KEY_CODE_INVERS_SHIFT) ^ ((tmp & KEY_CODE_MASK) >> KEY_CODE_SHIFT)) != 0xff) )
			{
				bk_printf("invalid ir value\r\n");
				IR_key.valid_flag = 0;
				return;
			}
		
			IR_key.IR_key_code = (tmp & KEY_CODE_MASK) >> KEY_CODE_SHIFT;
			IR_key.timer_cnt = 0;
			start_irda_handle_timer(REPEAT_KEY_TIME_INTERVAL);
    	}
    }
    if(repeat_int)
	{ 
	//repeat code is received
		IR_key.repeat_flag = 1;
		IR_key.repeat_cnt++;
    }
}

