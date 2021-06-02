#include "include.h"
#include "arm_arch.h"
#include "drv_model_pub.h"
#include "intc_pub.h"

#if CFG_USE_QSPI
#include "qspi.h"
#include "qspi_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "gpio.h"
#include "uart_pub.h"
#include "mem_pub.h"

static volatile QSPI_GE0_DRV_DESC   *sg_p_qspi_ge0_drv_desc ;
__maybe_unused static UINT8  qspi_ge0_busy(void);
void qspi_printf(void);

#define QSPI_VID_DEP                    0x80
#define QSPI_AUD_DEP                    0x10
#define QSPI_GE0_DEP                    0x10
#define QSPI_GE1_DEP                    0x10
#define QSPI_FLS_DEP                    0x40

static const SDD_OPERATIONS qspi_op = 
{
	qspi_ctrl
};

void qspi_init(void)
{
	os_printf("QSPi_init\r\n");
	intc_service_register(FIQ_PSRAM, PRI_IRQ_QSPI, qspi_isr); 
	sddev_register_dev(QSPI_DEV_NAME, (SDD_OPERATIONS *)&qspi_op);	
	os_printf("QSPi_init1\r\n");
}

void qspi_exit(void)
{
    sddev_unregister_dev(QSPI_DEV_NAME);
}

static void qspi_psram_set_voltage(UINT32 mode)
{
    UINT32 param;

    param = mode;
    sddev_control(SCTRL_DEV_NAME, CMD_QSPI_IO_VOLTAGE, &param);
    sddev_control(SCTRL_DEV_NAME, CMD_QSPI_VDDRAM_VOLTAGE, &param);
}

static void qspi_gpio_configuration(UINT8 LineMode)
{
    uint32_t val;

	val = GFUNC_MODE_QSPI_CLK;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);
	
	val = GFUNC_MODE_QSPI_CSN;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);

	if(LineMode == 1)
	{
		val = GFUNC_MODE_QSPI_1LINE;
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);	
		gpio_config(18, GMODE_INPUT_PULLDOWN);
		gpio_config(19, GMODE_INPUT_PULLDOWN);
	}
	else if(LineMode == 4)
	{
		//set io0/io1
		val = GFUNC_MODE_QSPI_1LINE;
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);	

		//set io2/io3
		val = GFUNC_MODE_QSPI_4LINE;
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);			
	}
}

static void qspi_icu_configuration(UINT32 enable)
{
    UINT32 param;

    if(enable) 
    {
        param = PWD_QSPI_CLK_BIT;
	    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

        param = (FIQ_PSRAM_BIT);
        sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

		param = BLK_BIT_MIC_QSPI_RAM_OR_FLASH;
    	sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);
    }
    else
    {
        param = (FIQ_PSRAM_BIT);
        sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
        
        param = PWD_QSPI_CLK_BIT;
	    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);

		param = (~BLK_BIT_MIC_QSPI_RAM_OR_FLASH);
    	sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);
    }
}

static void qspi_clk_set_26M(void)
{		
	UINT32 param;
	param = PCLK_POSI_QSPI_26M;
	sddev_control(ICU_DEV_NAME, CMD_QSPI_CLK_SEL, &param);
}

static void qspi_clk_set_dco(void)
{		
	UINT32 param;
	param = PCLK_POSI_QSPI_DCO;
	sddev_control(ICU_DEV_NAME, CMD_QSPI_CLK_SEL, &param);
}

static void qspi_clk_set_120M(void)
{		
	UINT32 param;
	param = PCLK_POSI_QSPI_120M;
	sddev_control(ICU_DEV_NAME, CMD_QSPI_CLK_SEL, &param);
}

static void qspi_div_clk_set(UINT32 clk)
{
	UINT32 val;

	val = REG_READ(QSPI_CTRL);
	val = ((val & (~(0x07 << 8))) | ((clk & 0x07) << 8));
	REG_WRITE(QSPI_CTRL, val);
}

static void qspi_dcache_wr_cmd(UINT8 command ,UINT8 linemode)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_WR_CMD);

	if(linemode == 0)
	{
		val = 0x2009;	
	}
	else if(linemode == 3)
	{
		val = 0x8e3;	
	}
	
	val = val | (command << 2) | (1 << 0); // enable	

	REG_WRITE(QSPI_DCACHE_WR_CMD, val);	
}

static void qspi_dcache_wr_addr(UINT8 linemode)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_WR_ADDR);

	if(linemode == 0)
	{
		val = 0x60000001;	
	}
	else if(linemode == 3)
	{
		val = 0x18000003;	
	}
	
	val = val | (1 << 0); // enable	

	REG_WRITE(QSPI_DCACHE_WR_ADDR, val);	
}

static void qspi_dcache_wr_dum(UINT8 linemode, UINT8 dummy_size)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_WR_DUM);

	if(linemode == 0)
	{
		val = 0x0;	
	}
	else if(linemode == 3)
	{
		val = 0x2;
	}
	
	val = val | (dummy_size << 2) | ((dummy_size? 1 : 0 )<< 0 );

	REG_WRITE(QSPI_DCACHE_WR_DUM, val);	
}

static void qspi_dcache_wr_dat(UINT8 linemode)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_WR_DAT);

	if(linemode == 0)
	{
		val = 0x4201;	
	}
	else if(linemode == 3)
	{
		val = 0x4083;	
	}
	
	val = val | (1 << 0) | (1 << 14); // enable	

	REG_WRITE(QSPI_DCACHE_WR_DAT, val);	
}

static void qspi_dcache_rd_cmd(UINT8 command ,UINT8 linemode)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_RD_CMD);

	if(linemode == 0)
	{
		val = 0x200d;	
	}
	else if(linemode == 3)
	{
		val = 0xbaf;	
	}
	
	val = val | (command << 2) | (1 << 0); // enable	

	REG_WRITE(QSPI_DCACHE_RD_CMD, val);	
}

static void qspi_dcache_rd_addr(UINT8 linemode)
{
	UINT32 val = 0;

	if(linemode == 0)
	{
		val = 0x60000001;	
	}
	else if(linemode == 3)
	{
		val = 0x18000003;	
	}
	
	val = val | (1 << 0); // enable	

	REG_WRITE(QSPI_DCACHE_RD_ADDR, val);	
}

static void qspi_dcache_rd_dum(UINT8 linemode, UINT8 dummy_size)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_RD_DUM);

	if(linemode == 0)
	{
		val = 0x0;	
	}
	else if(linemode == 3)
	{
		val = 0x1b;	
	}
	
	val = val | (dummy_size << 2) | ((dummy_size? 1 : 0 ) << 0 );

	REG_WRITE(QSPI_DCACHE_RD_DUM, val);	
}

static void qspi_dcache_rd_dat(UINT8 linemode)
{
	UINT32 val;

	val = REG_READ(QSPI_DCACHE_RD_DAT);

	if(linemode == 0)
	{
		val = 0x201;	
	}
	else if(linemode == 3)
	{
		val = 0x83;	
	}

	val = val & (~(1 << 14));
	val = val | (1 << 0) ; // enable	

	REG_WRITE(QSPI_DCACHE_RD_DAT, val);	
}

static void qspi_dcache_request(void)
{
	REG_WRITE(QSPI_DCACHE_REQUEST, 1);	
}

static UINT32 qspi_open(UINT32 op_flag)
{
	UINT32 value;
	
	value = op_flag;
	qspi_icu_configuration(value);
	
	return QSPI_SUCCESS;
}

static UINT32 qspi_close(void)
{
	qspi_icu_configuration(0);
	return QSPI_SUCCESS;
}

static UINT32 qspi_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = QSPI_SUCCESS;
	qspi_dcache_drv_desc * p_param;
	
    peri_busy_count_add();

	switch(cmd){
	case QSPI_CMD_SET_VOLTAGE:
		qspi_psram_set_voltage(*(UINT8 *)param);
		break;
	case QSPI_CMD_DCACHE_CONFIG:
		p_param = (qspi_dcache_drv_desc *)param;
		ret = bk_qspi_dcache_configure(p_param);
		break;
	case QSPI_CMD_GPIO_CONFIG:
		qspi_gpio_configuration(*(UINT8 *)param);
		break;		
	case QSPI_CMD_DIV_CLK_SET:
		qspi_div_clk_set(*(UINT8 *)param);
		break;
	case QSPI_CMD_CLK_SET_26M:
		qspi_clk_set_26M();
		break;	
	case QSPI_CMD_CLK_SET_DCO:
		qspi_clk_set_dco();
		break;	
	case QSPI_CMD_CLK_SET_120M:
		qspi_clk_set_120M();
		break;
	case QSPI_DCACHE_CMD_OPEN:	
		qspi_open(1);
		break;	
	case QSPI_DCACHE_CMD_CLOSE:
		qspi_close();
		break;
	
	default:
		ret = QSPI_FAILURE;
		break;
	}

    peri_busy_count_dec();

	return ret;
}

int bk_qspi_dcache_configure(qspi_dcache_drv_desc *qspi_cfg)
{
	UINT32 line_mode, param;
	UINT32 wr_cmd, rd_cmd, wr_dummy_size, rd_dummy_size;

	ASSERT(qspi_cfg != NULL);
		
	wr_cmd = qspi_cfg->wr_command;
	rd_cmd = qspi_cfg->rd_command;
	wr_dummy_size = qspi_cfg->wr_dummy_size;
	rd_dummy_size = qspi_cfg->rd_dummy_size;	

	if ((qspi_cfg->mode & 0x03) == 0)		// 1 Linemode
	{
		line_mode = 0;
	}
	else if ((qspi_cfg->mode & 0x03) == 3)	//3 linemode
	{
		line_mode = 3;
	}
	else
	{
		os_printf("[QSPI]:line mode error\r\n");
		return QSPI_FAILURE;
	}
	
	param = qspi_cfg->clk_set;
	qspi_div_clk_set(param);

	qspi_dcache_wr_cmd(wr_cmd, line_mode);
	
	qspi_dcache_wr_addr(line_mode);
	qspi_dcache_wr_dum(line_mode, wr_dummy_size);
	qspi_dcache_wr_dat(line_mode);
	
	qspi_dcache_rd_cmd(rd_cmd, line_mode);
	qspi_dcache_rd_addr(line_mode);
	qspi_dcache_rd_dum(line_mode, rd_dummy_size);
	qspi_dcache_rd_dat(line_mode);

	qspi_dcache_request();	

	return QSPI_SUCCESS;
}

static void cpu_delay( volatile unsigned int times)
{
    while(times--)
	{
		for(uint32_t i = 0;i < 1000;i++)
			;
	}				
}

int qspi_ge0_init(QSPI_GE0_DRV_DESC *p_QSPI_ge0_drv_desc)
{
    int i, reg, param;
    unsigned char ucBW;
    unsigned char ucCNT;
    unsigned char ucDir;        // 0: write to psram    1: read from psram

    if (p_QSPI_ge0_drv_desc == NULL)
    {
        QSPI_DEBUG_PRINTF("p_QSPI_ge0_drv_desc == NULL\r\n");
        return QSPI_FAILURE;
    }
	
	QSPI_DEBUG_PRINTF("\r\nqspi_set_line_config:p_QSPI_ge0_drv_desc->mode=%x \r\n",p_QSPI_ge0_drv_desc->mode);

    if ((p_QSPI_ge0_drv_desc->mode & 0x03) == 0)        // 1 line mode
    {
    	QSPI_DEBUG_PRINTF("\r\n 1 line mode send cmd\r\n");
        ucBW  = 0;
        ucCNT = 8;
    }
    else if ((p_QSPI_ge0_drv_desc->mode & 0x03) == 3)   // 4 line mode
    {
		QSPI_DEBUG_PRINTF("\r\n 4 line mode send cmd\r\n");
        ucBW  = 1;
        ucCNT = 2;
    }
    else
    {
        QSPI_DEBUG_PRINTF("p_QSPI_drv_desc->mode = %d\r\n", p_QSPI_ge0_drv_desc->mode);
        return QSPI_FAILURE;
    }

	reg = REG_READ(REG_QSPI_ENABLE);
    reg &= (~((1<<2) | (1<<3) | (1<<4) | (1<<5)));
	REG_WRITE(REG_QSPI_ENABLE, reg);	

    param = (FIQ_PSRAM_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    ucDir = (p_QSPI_ge0_drv_desc->mode & 0x04) ? 1 : 0;
    sg_p_qspi_ge0_drv_desc = p_QSPI_ge0_drv_desc;
	
	qspi_gpio_configuration((p_QSPI_ge0_drv_desc->mode & 0x03) + 1);

	reg = REG_READ(QSPI_CTRL);
	reg = (reg & (~(0x7<<8))) | ((p_QSPI_ge0_drv_desc->clk_set & 0x07) << 8);
	REG_WRITE(QSPI_CTRL, reg);
	
	reg = REG_READ(REG_QSPI_SW_CMD);
    reg  = (p_QSPI_ge0_drv_desc->command << 2)
                     | (ucCNT << 10)
                     | (ucBW  << 1)
                     | (1     << 0);
	REG_WRITE(REG_QSPI_SW_CMD, reg);	

	reg = REG_READ(REG_QSPI_SW_ADDR);
    reg = ((p_QSPI_ge0_drv_desc->data_buff_psram_addr & 0x00FFFFFF) << 2)
                     | ((ucCNT*(24/8)) << 26)
                     | (ucBW << 1)
                     | ((((p_QSPI_ge0_drv_desc->data_buff_psram_addr>>31) & 1) ? 1 : 0) << 0);
	REG_WRITE(REG_QSPI_SW_ADDR, reg);

	reg = REG_READ(REG_QSPI_SW_DUM);
    reg  = (p_QSPI_ge0_drv_desc->dummy_size << 2)
                     | (ucBW << 1)
                     | ((p_QSPI_ge0_drv_desc->dummy_size ? 1 : 0) << 0);
	REG_WRITE(REG_QSPI_SW_DUM, reg);
	
	reg = REG_READ(REG_QSPI_SW_DAT);
    reg  = ((ucDir ? 0 : 1) << 14)
                     | ((MIN(p_QSPI_ge0_drv_desc->data_buff_size, QSPI_GE0_DEP)*ucCNT*4) << 2)
                     | (ucBW << 1)
                     | ((p_QSPI_ge0_drv_desc->data_buff_size ? 1 : 0) << 0);
	REG_WRITE(REG_QSPI_SW_DAT, reg);
	
    reg = REG_READ(REG_QSPI_SW_OP);
    reg   = 5 + ucDir;
	REG_WRITE(REG_QSPI_SW_OP, reg);

	reg = REG_READ(REG_QSPI_GE0_DEP);
    reg = (QSPI_GE0_DEP << 0)
        | (QSPI_GE0_DEP << 11);
	REG_WRITE(REG_QSPI_GE0_DEP, reg);

	reg = REG_READ(REG_QSPI_GE0_DEP);
	REG_WRITE(REG_QSPI_GE1_DEP, reg);

	reg = REG_READ(REG_QSPI_GE0_TH);
    reg  = (MIN(p_QSPI_ge0_drv_desc->data_buff_size/2, QSPI_GE0_DEP) << 0)
         | (MIN(p_QSPI_ge0_drv_desc->data_buff_size/2, QSPI_GE0_DEP) << 11);
	REG_WRITE(REG_QSPI_GE0_TH, reg);

	reg =  REG_READ(REG_QSPI_GE0_TH);	 
	REG_WRITE(REG_QSPI_GE1_TH, reg);
	
    if (ucDir == 0)     // write to psram
    {
    	reg = REG_READ(REG_QSPI_ENABLE);
        reg |= (1<<5) | (1<<9);
		REG_WRITE(REG_QSPI_ENABLE, reg);

		
        for (i=0; i<MIN(p_QSPI_ge0_drv_desc->data_buff_size, QSPI_GE0_DEP); i++)
        {

			reg = REG_READ(REG_QSPI_GE0_DATA);
            reg = *(unsigned long *)(p_QSPI_ge0_drv_desc->pdata_buff_ram_addr + p_QSPI_ge0_drv_desc->data_buff_offset);
			REG_WRITE(REG_QSPI_GE0_DATA, reg);
			
            p_QSPI_ge0_drv_desc->data_buff_offset ++;
        }
    }
    else                // read from psram
    {
    	param = (FIQ_PSRAM_BIT);
        sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
		
    	QSPI_WPRT("++++read op++++\r\n");
		
    	reg = REG_READ(REG_QSPI_ENABLE);
        reg  |= (1<<4) | (1<<9);
		REG_WRITE(REG_QSPI_ENABLE, reg);

		reg =REG_READ(REG_QSPI_SW_OP);
        reg |= (1<<23) | (1<<4);
		REG_WRITE(REG_QSPI_SW_OP, reg);

		QSPI_WPRT("++++qqq\r\n");
        for (i=0; i<100; i++)      
        {
        	reg = REG_READ(REG_QSPI_INT_STATUS);
            if (reg & (1<<16))
            {
                break;
            }
            cpu_delay(100);
        }
        if (i == 100)
        {
            os_printf("ge0 read timeout\r\n");
            return QSPI_FAILURE;
        }

        QSPI_WPRT("start ge1 rx\r\n");

		reg = REG_READ(REG_QSPI_SW_OP);
        reg = (1<<6) | 8;     // 7 + ucDir;
		REG_WRITE(REG_QSPI_SW_OP, reg);
        
        p_QSPI_ge0_drv_desc->mode |= 0x10;

		reg = REG_READ(REG_QSPI_ENABLE);
        reg = (reg & (~(1<<4))) | (1<<2) | (1<<9);
		REG_WRITE(REG_QSPI_ENABLE, reg);
    }
	
#ifdef QSPI_DEBUG
	qspi_printf();
#endif

	reg = REG_READ(REG_QSPI_SW_OP);
    reg |= (1<<23) | (1<<4);
	REG_WRITE(REG_QSPI_SW_OP, reg);

    param = (FIQ_PSRAM_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

	qspi_icu_configuration(1);

    return QSPI_SUCCESS;
}

// ucEnterOrExit:  0: exit  1:enter
void bk_qspi_psram_quad_mode_switch(unsigned char is_single_mode)
{
	QSPI_WPRT("psram:switch line mode \r\n");
    QSPI_GE0_DRV_DESC *p_QSPI_GE0_drv_desc ;

    is_single_mode &= 0x01;
    p_QSPI_GE0_drv_desc = (QSPI_GE0_DRV_DESC *)os_malloc(sizeof(QSPI_GE0_DRV_DESC));
    if (p_QSPI_GE0_drv_desc == NULL)
    {
        printf("malloc1 failed!\r\n");
        return;
    }
    os_memset(p_QSPI_GE0_drv_desc, 0, sizeof(QSPI_GE0_DRV_DESC));

    p_QSPI_GE0_drv_desc->mode                 = (is_single_mode ? 0x00 : 0x03);
    p_QSPI_GE0_drv_desc->clk_set              = 0x10;
    p_QSPI_GE0_drv_desc->command              = (is_single_mode ? 0x35 : 0xF5);
    p_QSPI_GE0_drv_desc->dummy_size           = 0x00;
    p_QSPI_GE0_drv_desc->data_buff_psram_addr = 0x00;
    p_QSPI_GE0_drv_desc->pdata_buff_ram_addr  = NULL;
    p_QSPI_GE0_drv_desc->data_buff_size       = 0x00;
    p_QSPI_GE0_drv_desc->data_buff_offset     = 0x00;
    p_QSPI_GE0_drv_desc->p_Int_Handler        = NULL;

    if (qspi_ge0_init(p_QSPI_GE0_drv_desc) == QSPI_FAILURE)
    {
        os_printf("qspi_ge0_init ERROR!\r\n");
        return;
    }

	os_free(p_QSPI_GE0_drv_desc);
	p_QSPI_GE0_drv_desc = NULL;
}

void bk_qspi_psram_reset_enable(void)
{
	QSPI_WPRT("psram: reset enable\r\n");

	QSPI_GE0_DRV_DESC *p_QSPI_GE0_drv_desc ;
	p_QSPI_GE0_drv_desc = (QSPI_GE0_DRV_DESC *)os_malloc(sizeof(QSPI_GE0_DRV_DESC));
	if (p_QSPI_GE0_drv_desc == NULL)
	{
		printf("malloc1 failed!\r\n");
		return;
	}
	os_memset(p_QSPI_GE0_drv_desc, 0, sizeof(QSPI_GE0_DRV_DESC));

	REG_WRITE(QSPI_CTRL, 0xa0);

	p_QSPI_GE0_drv_desc->mode				  = 0x00;
	p_QSPI_GE0_drv_desc->clk_set			  = 0x10;
	p_QSPI_GE0_drv_desc->command			  = 0x66;
	p_QSPI_GE0_drv_desc->dummy_size 		  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_psram_addr = 0x00;
	p_QSPI_GE0_drv_desc->pdata_buff_ram_addr  = NULL;
	p_QSPI_GE0_drv_desc->data_buff_size 	  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_offset	  = 0x00;
	p_QSPI_GE0_drv_desc->p_Int_Handler		  = NULL;
	
	if (qspi_ge0_init(p_QSPI_GE0_drv_desc) == QSPI_FAILURE)
	{
		os_printf("qspi_ge0_init ERROR!\r\n");
		return;
	}

	os_free(p_QSPI_GE0_drv_desc);
}

void bk_qspi_psram_reset(void)
{
	int ucEnterOrExit = 0;
	int param;
    QSPI_GE0_DRV_DESC *p_QSPI_GE0_drv_desc ;
	
	QSPI_WPRT("psram: reset \r\n");

	ucEnterOrExit &= 0x01;
	p_QSPI_GE0_drv_desc = (QSPI_GE0_DRV_DESC *)os_malloc(sizeof(QSPI_GE0_DRV_DESC));
	if (p_QSPI_GE0_drv_desc == NULL)
	{
		printf("malloc1 failed!\r\n");
		return;
	}
	os_memset(p_QSPI_GE0_drv_desc, 0, sizeof(QSPI_GE0_DRV_DESC));


	p_QSPI_GE0_drv_desc->mode				  = 0x00;//(ucEnterOrExit ? 0x00 : 0x03);
	p_QSPI_GE0_drv_desc->clk_set			  = 0x10;
	p_QSPI_GE0_drv_desc->command			  = 0x99;
	p_QSPI_GE0_drv_desc->dummy_size 		  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_psram_addr = 0x00;
	p_QSPI_GE0_drv_desc->pdata_buff_ram_addr  = NULL;
	p_QSPI_GE0_drv_desc->data_buff_size 	  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_offset	  = 0x00;
	p_QSPI_GE0_drv_desc->p_Int_Handler		  = NULL;

	if (qspi_ge0_init(p_QSPI_GE0_drv_desc) == QSPI_FAILURE)
	{
		os_printf("qspi_ge0_init ERROR!\r\n");
		return;
	}

	os_free(p_QSPI_GE0_drv_desc);
	QSPI_WPRT("psram: reset ok\r\n");
	
	param = (FIQ_PSRAM_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

void bk_qspi_psram_set_length(void)
{
	int ucEnterOrExit = 0;
    QSPI_GE0_DRV_DESC *p_QSPI_GE0_drv_desc ;
	
	QSPI_WPRT("qspi set length\r\n");
	
	ucEnterOrExit &= 0x01;
	p_QSPI_GE0_drv_desc = (QSPI_GE0_DRV_DESC *)os_malloc(sizeof(QSPI_GE0_DRV_DESC));
	if (p_QSPI_GE0_drv_desc == NULL)
	{
		printf("malloc1 failed!\r\n");
		return;
	}
	os_memset(p_QSPI_GE0_drv_desc, 0, sizeof(QSPI_GE0_DRV_DESC));


	p_QSPI_GE0_drv_desc->mode				  = 0x00;//(ucEnterOrExit ? 0x00 : 0x03);
	p_QSPI_GE0_drv_desc->clk_set			  = 0x10;
	p_QSPI_GE0_drv_desc->command			  = 0xC0;
	p_QSPI_GE0_drv_desc->dummy_size 		  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_psram_addr = 0x00;
	p_QSPI_GE0_drv_desc->pdata_buff_ram_addr  = NULL;
	p_QSPI_GE0_drv_desc->data_buff_size 	  = 0x00;
	p_QSPI_GE0_drv_desc->data_buff_offset	  = 0x00;
	p_QSPI_GE0_drv_desc->p_Int_Handler		  = NULL;

	if (qspi_ge0_init(p_QSPI_GE0_drv_desc) == QSPI_FAILURE)
	{
		os_printf("qspi_ge0_init ERROR!\r\n");
		return;
	}

	os_free(p_QSPI_GE0_drv_desc);
}

static UINT8  qspi_ge0_busy(void)
{
	int reg;
	reg = REG_READ(REG_QSPI_ENABLE);
	if (reg & ((1<<((sg_p_qspi_ge0_drv_desc->mode & 0x04) ? 2 : 5))))
	{
		os_printf("busy\r\n");

		return 1;
	}

	return 0;
}

#define GPIO_PSRSM_CE 		REG_GPIO_26_CONFIG
#define GPIO_PSRSM_CLK 		REG_GPIO_24_CONFIG
#define GPIO_PSRSM_SI 		REG_GPIO_16_CONFIG
#define GPIO_PSRSM_SO 		REG_GPIO_17_CONFIG

UINT16 bk_qspi_psram_read_id(void)
{
    unsigned char i;
    unsigned int kgd;
	UINT16 reg;

	kgd = 0;
	REG_WRITE(GPIO_PSRSM_CE, 0x02);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	REG_WRITE(GPIO_PSRSM_SI, 0x00);
	REG_WRITE(GPIO_PSRSM_SO, 0x0C);

	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CE, 0x00);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);
	

	REG_WRITE(GPIO_PSRSM_SI, 0x00);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);

	REG_WRITE(GPIO_PSRSM_SI, 0x00);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);

	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);	

	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);	

	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);	
	
	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);	

	REG_WRITE(GPIO_PSRSM_SI, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x02);
	cpu_delay(100);
	REG_WRITE(GPIO_PSRSM_CLK, 0x00);
	cpu_delay(100);	

    for(i=0;i<24;i=i+1)
    {
		REG_WRITE(GPIO_PSRSM_SI, 0x00);
		cpu_delay(100);
		
		REG_WRITE(GPIO_PSRSM_CLK, 0x02);
		cpu_delay(100);
		
		REG_WRITE(GPIO_PSRSM_CLK, 0x00);
		cpu_delay(100); 
    }

    for(i=0;i<16;i=i+1)
    {
		REG_WRITE(GPIO_PSRSM_SI, 0x00);
		cpu_delay(100);
		REG_WRITE(GPIO_PSRSM_CLK, 0x02);
		cpu_delay(100);

		reg = REG_READ(GPIO_PSRSM_SO);
		reg &=  0x01;
		kgd |=(reg << (15-i)) & 0xFFFF; 		
 		REG_WRITE(GPIO_PSRSM_CLK, 0x00);
		cpu_delay(100);
    }

	REG_WRITE(GPIO_PSRSM_CE, 0x02);
	cpu_delay(100);
	
	return kgd;
}

void bk_qspi_mode_start(UINT32 mode, UINT32 div)
{
	UINT32 param;
	qspi_open(1);

	param = 2;
	qspi_ctrl(QSPI_CMD_SET_VOLTAGE, (void *)&param);
	
	qspi_ctrl(QSPI_CMD_CLK_SET_26M, NULL);
	
	param = div;
	qspi_ctrl(QSPI_CMD_DIV_CLK_SET,(void *)&param);

	param = mode;
	qspi_ctrl(QSPI_CMD_GPIO_CONFIG, (void *)&param);	
}

int bk_qspi_dcache_write_data(UINT32 set_addr, UINT32 *wr_data, UINT32 data_length)
{
	UINT32 i ;
	UINT32 value;

	if(wr_data == NULL)
	{
		return QSPI_FAILURE;
	}
	
	for(i = 0; i < data_length; i++)
	{
		value = *wr_data ;
		
		REG_WRITE(QSPI_DCACHE_BASE + set_addr + i*4 , value);
		wr_data	 ++;
	}
	
	return QSPI_SUCCESS;	
}

int bk_qspi_dcache_read_data(UINT32 set_addr, UINT32 *rd_data, UINT32 data_length)

{
	UINT32 i ;
	UINT32 value;

	if(rd_data == NULL)
	{
		return QSPI_FAILURE;
	}
	
	for(i = 0; i < data_length; i++)
	{		
		value = REG_READ((QSPI_DCACHE_BASE + set_addr + i*4) );
		*rd_data = value;
		rd_data	 ++;
	}
	
	return QSPI_SUCCESS;	
}

void qspi_printf(void)
{
    os_printf("REG_QSPI_SW_CMD = 0x%lx\r\n", REG_READ(REG_QSPI_SW_CMD));
    os_printf("REG_QSPI_SW_ADDR = 0x%lx\r\n", REG_READ(REG_QSPI_SW_ADDR));
    os_printf("REG_QSPI_SW_DUM = 0x%lx\r\n", REG_READ(REG_QSPI_SW_DUM));
    os_printf("REG_QSPI_SW_DAT = 0x%lx\r\n", REG_READ(REG_QSPI_SW_DAT));
    os_printf("REG_QSPI_SW_OP = 0x%lx\r\n", REG_READ(REG_QSPI_SW_OP));
    os_printf("REG_QSPI_GE0_TH = 0x%lx\r\n", REG_READ(REG_QSPI_GE0_TH));
    os_printf("REG_QSPI_GE0_DEP = 0x%lx\r\n", REG_READ(REG_QSPI_GE0_DEP));
    os_printf("REG_QSPI_GE1_TH = 0x%lx\r\n", REG_READ(REG_QSPI_GE1_TH));
    os_printf("REG_QSPI_GE1_DEP = 0x%lx\r\n", REG_READ(REG_QSPI_GE1_DEP));
    os_printf("REG_QSPI_ENABLE = 0x%lx\r\n", REG_READ(REG_QSPI_ENABLE));
    os_printf("REG_QSPI_CONFIG = 0x%lx\r\n", REG_READ(QSPI_CTRL));
    os_printf("REG_QSPI_FIFO_STATUS = 0x%lx\r\n", REG_READ(REG_QSPI_FIFO_STATUS));
    os_printf("REG_QSPI_INT_STATUS = 0x%lx\r\n", REG_READ(REG_QSPI_INT_STATUS));
}

void qspi_isr(void)
{   
	unsigned long ulStatus;
	int reg;

    QSPI_DEBUG_PRINTF("In QSPI_InterruptHandler\r\n");

    ulStatus = REG_READ(REG_QSPI_INT_STATUS);
    QSPI_DEBUG_PRINTF("REG_QSPI_INT_STATUS before mask = 0x%lx\r\n", ulStatus);
	
	reg =  REG_READ(REG_QSPI_SW_OP);
    ulStatus = ulStatus & (reg >> 7) & 0x0001FFFF;
    QSPI_DEBUG_PRINTF("REG_QSPI_INT_STATUS after mask  = 0x%lx\r\n", ulStatus);
   
    if (ulStatus & (1<<16))      // sw op int
    {
        QSPI_DEBUG_PRINTF("sw op int\r\n");
        if (sg_p_qspi_ge0_drv_desc->mode & 0x04)        // read from psram
        {
            int i = 0;
            QSPI_DEBUG_PRINTF("a\r\n");
			reg = REG_READ(REG_QSPI_FIFO_STATUS);
			#if 1
            while (reg & (1<<4))
            {
                os_printf("b\r\n");
                if ((sg_p_qspi_ge0_drv_desc->data_buff_offset >= sg_p_qspi_ge0_drv_desc->data_buff_size)
                 || (i >= QSPI_GE0_DEP))
                {
                    os_printf("c\r\n");
                    break;
                }
                else
                {
                    os_printf("d\r\n");
                    i ++;

                   reg =  *(sg_p_qspi_ge0_drv_desc->pdata_buff_ram_addr + sg_p_qspi_ge0_drv_desc->data_buff_offset);
                   REG_WRITE(REG_QSPI_GE1_DATA, reg) ;
                    sg_p_qspi_ge0_drv_desc->data_buff_offset ++;
                }
                QSPI_DEBUG_PRINTF("e\r\n");
            }
            QSPI_DEBUG_PRINTF("f\r\n");

            {
                QSPI_DEBUG_PRINTF("g\r\n");
				reg = REG_READ(REG_QSPI_SW_OP);
                reg = (REG_QSPI_SW_OP & (~(1<<4))) | (1<<6);
				REG_WRITE(REG_QSPI_SW_OP, reg);
				
				reg = REG_READ(REG_QSPI_ENABLE);
                reg &= (~((1<<2)));
				REG_WRITE(REG_QSPI_ENABLE, reg);
				
                if (sg_p_qspi_ge0_drv_desc->p_Int_Handler != NULL)
                {
                    QSPI_DEBUG_PRINTF("h\r\n");
                    (void)sg_p_qspi_ge0_drv_desc->p_Int_Handler();
                }
                QSPI_DEBUG_PRINTF("i\r\n");
                sg_p_qspi_ge0_drv_desc = NULL;

				//debug:close int
				reg = (FIQ_PSRAM_BIT);
        		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &reg);
            }

            QSPI_DEBUG_PRINTF("j\r\n");
			#endif
        }
        else                                            // write to psram
        {
            QSPI_DEBUG_PRINTF("k\r\n");
            if (sg_p_qspi_ge0_drv_desc->data_buff_offset >= sg_p_qspi_ge0_drv_desc->data_buff_size)
            {
                QSPI_DEBUG_PRINTF("l\r\n");
				reg = REG_READ(REG_QSPI_SW_OP);
                reg = (reg & (~(1<<4))) | (1<<6);
				REG_WRITE(REG_QSPI_SW_OP, reg);

				reg = REG_READ(REG_QSPI_ENABLE);
                reg &= (~((1<<5)));
				REG_WRITE(REG_QSPI_ENABLE, reg);
				
                if (sg_p_qspi_ge0_drv_desc->p_Int_Handler != NULL)
                {
                    QSPI_DEBUG_PRINTF("m\r\n");
                    (void)sg_p_qspi_ge0_drv_desc->p_Int_Handler();
                    sg_p_qspi_ge0_drv_desc = NULL;
                }
                sg_p_qspi_ge0_drv_desc = NULL;
            }
            else
            {
            }
            QSPI_DEBUG_PRINTF("t\r\n");
        }
        QSPI_DEBUG_PRINTF("u\r\n");
		REG_WRITE(REG_QSPI_INT_STATUS,ulStatus);
    }
	 QSPI_DEBUG_PRINTF("Out QSPI_InterruptHandler\r\n");
}
#endif
