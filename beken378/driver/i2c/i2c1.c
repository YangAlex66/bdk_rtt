#include "include.h"
#include "arm_arch.h"

#include "i2c1.h"
#include "i2c_pub.h"

#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

typedef struct i2c1_msg {
	UINT8 TxMode;		//0: Read;  1: Write
	UINT16 RegAddr;
    UINT16 RemainNum;
	UINT8 *pData;
	UINT8 SalveID;
	UINT8 AddrFlag;
	UINT8 TransDone;
	UINT8 ErrorNO;
	UINT8 AddrWidth;
} I2C1_MSG_ST, *I2C1_MSG_PTR;

static DD_OPERATIONS i2c1_op =
{
    i2c1_open,
    i2c1_close,
    i2c1_read,
    i2c1_write,
    i2c1_ctrl
};

volatile I2C1_MSG_ST gi2c1;

static void i2c1_set_ensmb(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_ENSMB;
    else
        reg_val &= ~I2C1_ENSMB;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_sta(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_STA;
    else
        reg_val &= ~I2C1_STA;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_stop(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_STO;
    else
        reg_val &= ~I2C1_STO;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_ack_tx(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_ACK_TX;
    else
        reg_val &= ~I2C1_ACK_TX;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_smbus_tx_mode(UINT32 enable)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= I2C1_TX_MODE;
    else
        reg_val &= ~I2C1_TX_MODE;
    REG_WRITE(reg_addr, reg_val);
}

static void i2c1_set_freq_div(UINT32 div)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val = (reg_val & ~(I2C1_FREQ_DIV_MASK << I2C1_FREQ_DIV_POSI))
        | ((div & I2C1_FREQ_DIV_MASK) << I2C1_FREQ_DIV_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static UINT32 i2c1_get_smbus_interrupt(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_SI)? 1: 0;
}

static void i2c1_clear_smbus_interrupt(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val |= I2C1_SI;

    REG_WRITE(reg_addr, reg_val);
}

static UINT32 i2c1_get_ack_rx(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_ACK_RX)? 1: 0;
}

static UINT32 i2c1_get_ack_req(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_ACK_REQ)? 1: 0;
}

static UINT32 i2c1_get_smbus_busy(void)
{
    UINT32 reg_addr = REG_I2C1_CONFIG;
    UINT32 reg_val = REG_READ(reg_addr);

    return (reg_val & I2C1_BUSY)? 1: 0;
}

////////////////////////////////////////////////////////////////////////////////
static void i2c1_gpio_config(void)
{
    UINT32 param;

    param = GFUNC_MODE_I2C1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

static void i2c1_power_up(void)
{
    UINT32 param;
    param = PWD_I2C1_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
}

static void i2c1_power_down(void)
{
    UINT32 param;
    param = PWD_I2C1_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
}

static void i2c1_enable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C1_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void i2c1_disable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_I2C1_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

static void i2c1_isr(void)
{
    unsigned long   i2c1_config;
    unsigned long   work_mode, ack_rx, sta, si;

    i2c1_config = REG_READ(REG_I2C1_CONFIG);
    si = i2c1_config & I2C1_SI;

    I2C1_PRT("i2c1_isr\r\n");
    I2C1_PRT("i2c1_isr: i2c1_config=0x%x\r\n", i2c1_config);

    if (!si)     // not SMBUS/I2C Interrupt
    {
        I2C1_EPRT("i2c1_isr not SI! i2c1_config = 0x%lx\r\n", i2c1_config);
        return;
    }

    ack_rx = i2c1_config & I2C1_ACK_RX;
    sta = i2c1_config & I2C1_STA;
    work_mode = (gi2c1.TxMode & 0x01) ? 0x00 : 0x01;

    i2c1_config &= (~I2C1_STA);
    i2c1_config &= (~I2C1_STO);
    I2C1_PRT("1: gi2c1.AddrFlag=0x%x\r\n", gi2c1.AddrFlag);
    switch (work_mode)
    {
        case 0x00:      // master write
        {
            if (!ack_rx)  // nack
            {
                i2c1_config |= I2C1_STO;        // send stop
                gi2c1.TransDone = 1;
                gi2c1.ErrorNO = 1;
                break;
            }
            I2C1_PRT("2: gi2c1.AddrFlag=0x%x\r\n", gi2c1.AddrFlag);

            if (gi2c1.AddrFlag & 0x10)      // all address bytes has been tx, now tx data
            {
                if (gi2c1.RemainNum == 0)   // all data bytes has been tx, now send stop
                {
                    i2c1_config |= I2C1_STO;    // send stop
                    gi2c1.TransDone = 1;
                }
                else
                {
                    REG_WRITE(REG_I2C1_DAT, *gi2c1.pData);
                    gi2c1.pData ++;
                    gi2c1.RemainNum --;
                }
                break;
            }

            if ((gi2c1.AddrFlag & 0x08) == 0)
            {
                if(gi2c1.AddrWidth == ADDR_WIDTH_8)
                {
                    REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr&0xFF));
                    gi2c1.AddrFlag |= 0x13;
                }
                else if(gi2c1.AddrWidth == ADDR_WIDTH_16)
                {
                    REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr>>8));
                    gi2c1.AddrFlag |= 0x08;
                }
            }
            else
            {
                REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr&0xFF));
                gi2c1.AddrFlag |= 0x10;
            }
        }
        break;

        case 0x01:      // master read
        {
            if (((gi2c1.AddrFlag & 0x10) == 0) && (ack_rx == 0))      // NACK
            {
                i2c1_config = i2c1_config | I2C1_STO;
                gi2c1.TransDone = 1;
                gi2c1.ErrorNO = 1;
                break;
            }

            if (sta && !ack_rx)         // when tx address, we need ACK
            {
                i2c1_config = i2c1_config | I2C1_STO;
                gi2c1.TransDone = 1;
                break;
            }

            if (gi2c1.AddrFlag & 0x10)  // all address has been tx, now rx data
            {
                i2c1_config &= (~I2C1_TX_MODE);

                if (sta)
                {
                    i2c1_config = i2c1_config | I2C1_ACK_TX;        // send ACK
                    break;
                }

                *gi2c1.pData = REG_READ(REG_I2C1_DAT);
                gi2c1.pData ++;
                gi2c1.RemainNum --;

                if (gi2c1.RemainNum == 0)
                {
                    i2c1_config = (i2c1_config & (~I2C1_ACK_TX)) | I2C1_STO;     // send NACK and STOP
                    gi2c1.TransDone = 1;
                }
                else
                {
                    i2c1_config = i2c1_config | I2C1_ACK_TX;    // send ACK
                }
                break;
            }

            if ((gi2c1.AddrFlag & 0x08) == 0)       // inner address need to be tx
            {
				if(gi2c1.AddrWidth == ADDR_WIDTH_8)
				{
					REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr&0xFF));
					gi2c1.AddrFlag = (gi2c1.AddrFlag & (~0x0B)) | 0x0A;
				}
				else if(gi2c1.AddrWidth == ADDR_WIDTH_16)
				{
	                REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr>>8));
	                gi2c1.AddrFlag = (gi2c1.AddrFlag & (~0x0B)) | 0x08;
				}
            }
			else if ((gi2c1.AddrFlag & 0x02) == 0)
			{
                REG_WRITE(REG_I2C1_DAT, (gi2c1.RegAddr&0xFF));
                gi2c1.AddrFlag = (gi2c1.AddrFlag & (~0x0B)) | 0x0A;
            }
            else                            // inner address has been tx
            {
                i2c1_config |= I2C1_STA;
                REG_WRITE(REG_I2C1_DAT, (gi2c1.SalveID<<1) | 0x01);
                gi2c1.AddrFlag |= 0x13;
            }
        }
        break;

        default:        // by gwf
            break;
    }

    REG_WRITE(REG_I2C1_CONFIG, i2c1_config & (~I2C1_SI));
}

static void i2c1_software_init(void)
{
    ddev_register_dev(I2C1_DEV_NAME, &i2c1_op);
}

static void i2c1_hardware_init(void)
{
    /* register interrupt */
    intc_service_register(IRQ_I2C1, PRI_IRQ_I2C1, i2c1_isr);

    /* clear all setting */
    REG_WRITE(REG_I2C1_CONFIG, 0);
}

void i2c1_init(void)
{
    os_memset((unsigned char *)&gi2c1, 0, sizeof(I2C1_MSG_ST));
    i2c1_software_init();
    i2c1_hardware_init();
}

void i2c1_exit(void)
{
    REG_WRITE(REG_I2C1_CONFIG, 0);

    ddev_unregister_dev(I2C1_DEV_NAME);
}

static UINT32 i2c1_open(UINT32 op_flag)
{
   // UINT32 reg;
    os_printf("i2c1_open\r\n");
    if(op_flag) {
        i2c1_set_freq_div(op_flag);
    } else {
        i2c1_set_freq_div(I2C_CLK_DIVID(I2C_DEFAULT_BAUD));  // 400KHZ
    }

    i2c1_enable_interrupt();
    i2c1_power_up();
    i2c1_gpio_config();

    //i2c1_set_ensmb(1);

    return I2C1_SUCCESS;
}

static UINT32 i2c1_close(void)
{
    os_printf("i2c1_close\r\n");
    i2c1_set_ensmb(0);
    i2c1_disable_interrupt();
    i2c1_power_down();

    return I2C1_SUCCESS;
}

static UINT32 i2c1_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    I2C1_PRT("i2c1_read\r\n");

    I2C1_PRT("i2c1_read: i2c_op->salve_id = 0x%x, i2c_op->op_addr = 0x%x\r\n",
              i2c_op->salve_id, i2c_op->op_addr);

    I2C1_PRT("i2c1_read: count = %d\r\n", count);

    I2C1_PRT("i2c1_read: gi2c1.TransDone = %d\r\n", gi2c1.TransDone);

    if (gi2c1.TransDone != 0)
    {
        return 0;
    }

    GLOBAL_INT_DISABLE();
    // write cycle, write the subaddr to device
    gi2c1.TxMode = 0;
    gi2c1.RegAddr = i2c_op->op_addr;
    gi2c1.RemainNum = count;
    gi2c1.pData = (UINT8 *)user_buf;
    gi2c1.SalveID = i2c_op->salve_id;
    gi2c1.AddrFlag = 0;
    gi2c1.TransDone = 0;
    gi2c1.ErrorNO = 0;
	gi2c1.AddrWidth = i2c_op->addr_width;

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_TX_MODE | I2C1_ENSMB;// Set TXMODE | ENSMB
    REG_WRITE(REG_I2C1_CONFIG, reg);

    reg = ((i2c_op->salve_id & 0x7f) << 1) + 0;  // SET LSB 0
    REG_WRITE(REG_I2C1_DAT, reg);

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_STA;// Set STA
    REG_WRITE(REG_I2C1_CONFIG, reg);
    GLOBAL_INT_RESTORE();

    while(gi2c1.TransDone == 0);

    GLOBAL_INT_DISABLE();
    gi2c1.TransDone = 0;
    GLOBAL_INT_RESTORE();

    return gi2c1.ErrorNO;
}

static UINT32 i2c1_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 reg;
    I2C_OP_PTR i2c_op;
    GLOBAL_INT_DECLARATION();

    i2c_op = (I2C_OP_PTR)op_flag;

    I2C1_PRT("i2c1_write\r\n");

    I2C1_PRT("i2c1_write: i2c_op->salve_id = 0x%x, i2c_op->op_addr = 0x%x\r\n",
              i2c_op->salve_id, i2c_op->op_addr);

    I2C1_PRT("i2c1_write: count = %d\r\n", count);

    I2C1_PRT("i2c1_write: gi2c1.TransDone = %d\r\n", gi2c1.TransDone);

    if (gi2c1.TransDone != 0)
    {
        return 0;
    }

    GLOBAL_INT_DISABLE();
    gi2c1.TxMode = 1;
    gi2c1.RegAddr = i2c_op->op_addr;
    gi2c1.RemainNum = count;
    gi2c1.pData = (UINT8 *)user_buf;
    gi2c1.SalveID = i2c_op->salve_id;
    gi2c1.AddrFlag = 0;
    gi2c1.TransDone = 0;
    gi2c1.ErrorNO = 0;
    gi2c1.AddrWidth = i2c_op->addr_width;

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_TX_MODE | I2C1_ENSMB;// Set TXMODE | ENSMB
    REG_WRITE(REG_I2C1_CONFIG, reg);

    reg = ((i2c_op->salve_id & 0x7f)<<1) + 0;  // SET LSB 0
    REG_WRITE(REG_I2C1_DAT, reg);

    reg = REG_READ(REG_I2C1_CONFIG);
    reg |= I2C1_STA;// Set STA
    REG_WRITE(REG_I2C1_CONFIG, reg);
    GLOBAL_INT_RESTORE();

    while(gi2c1.TransDone == 0);

    GLOBAL_INT_DISABLE();
    gi2c1.TransDone = 0;
	GLOBAL_INT_RESTORE();

    return gi2c1.ErrorNO;
}

static UINT32 i2c1_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = I2C1_SUCCESS;

    switch(cmd)
    {
    case I2C1_CMD_SET_ENSMB:
        i2c1_set_ensmb(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_STA:
        i2c1_set_smbus_sta(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_STOP:
        i2c1_set_smbus_stop(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_ACK_TX:
        i2c1_set_smbus_ack_tx(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_SMBUS_TX_MODE:
        i2c1_set_smbus_tx_mode(*((UINT32 *)param));
        break;
    case I2C1_CMD_SET_FREQ_DIV:
        i2c1_set_freq_div(*((UINT32 *)param));
        break;
    case I2C1_CMD_GET_SMBUS_INTERRUPT:
        ret = i2c1_get_smbus_interrupt();
        break;
    case I2C1_CMD_CLEAR_SMBUS_INTERRUPT:
        i2c1_clear_smbus_interrupt();
        break;
    case I2C1_CMD_GET_ACK_RX:
        ret = i2c1_get_ack_rx();
        break;
    case I2C1_CMD_GET_ACK_REQ:
        ret = i2c1_get_ack_req();
        break;
    case I2C1_CMD_GET_SMBUS_BUSY:
        ret = i2c1_get_smbus_busy();
        break;

    default:
        break;
    }

    return ret;
}

