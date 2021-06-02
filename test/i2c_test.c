#include "include.h"
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <string.h>
#include "icu_pub.h"
#include "i2c_pub.h"
#include "drv_model_pub.h"
#include "target_util_pub.h"
#include "test_config.h"

#ifdef I2C_TEST

#define I2C_EEPROM_DEBUG
#ifdef I2C_EEPROM_DEBUG
#define I2C_EEPROM_PRT      os_printf
#define I2C_EEPROM_WARN     warning_prf
#define I2C_EEPROM_FATAL    fatal_prf
#else
#define I2C_EEPROM_PRT      null_prf
#define I2C_EEPROM_WARN     null_prf
#define I2C_EEPROM_FATAL    os_printf
#endif

#define   I2C1          	0
#define   I2C2          	1
#define   I2C_DEV_ID     	I2C1 
#define   I2C_SALVE_ID 		0x21

static DD_HANDLE i2c_hdl;

static void i2c_device_init()
{
	unsigned int oflag,status;
	oflag = 0;
    #if I2C_DEV_ID
    i2c_hdl = ddev_open(I2C2_DEV_NAME, &status, oflag);
    #else
    i2c_hdl = ddev_open(I2C1_DEV_NAME, &status, oflag);
    #endif
}


static void i2c_device_deinit()
{
	ddev_close(i2c_hdl);
}


/*************************************************************
 * I2C1_write_eeprom
 * Description: I2C1 write FT24C02 eeprom
 * Parameters:  op_addr: operate address
 *              pData: data point
 *              len: data len
 * return:      unsigned long
 * error:       none
 */

static unsigned long I2C_write_eeprom(unsigned char op_addr, unsigned char *pData, unsigned char len)
{
    unsigned char i;
    unsigned int status;
    I2C_OP_ST i2c_op;

    I2C_EEPROM_PRT("----- I2C1_write_eeprom start -----\r\n");

    i2c_op.op_addr  = op_addr;
    i2c_op.salve_id = I2C_SALVE_ID;

    do
    {
        status = ddev_write(i2c_hdl, pData, len, (unsigned long)&i2c_op);
    } while (status != 0);

    I2C_EEPROM_PRT("----- I2C1_write_eeprom over  -----\r\n");
    return 0;
}


/*************************************************************
 * I2C1_read_eeprom
 * Description: I2C1 read FT24C02 eeprom
 * Parameters:  op_addr: operate address
 *              pData: data point
 *              len: data len
 * return:      unsigned long
 * error:       none
 */
static unsigned long I2C_read_eeprom(unsigned char op_addr, unsigned char *pData, unsigned char len)
{
    unsigned char i;
    DD_HANDLE i2c_hdl;
    unsigned int status;
    I2C_OP_ST i2c_op;

    I2C_EEPROM_PRT("----- I2C1_read_eeprom start -----\r\n");

    i2c_op.op_addr  = op_addr;
    i2c_op.salve_id = I2C_SALVE_ID;
    do
    {
        status = ddev_read(i2c_hdl, pData, len, (unsigned long)&i2c_op);
    } while (status != 0);

    for (i=0; i<8; i++)
    {
        I2C_EEPROM_PRT("pData[%d] = 0x%x\r\n", i, pData[i]);
    }

    I2C_EEPROM_PRT("----- I2C1_read_eeprom over  -----\r\n");
    return status;
}

/*************************************************************
 * I2C1_test_eeprom
 * Description: I2C1 test FT24C02 eeprom
 * Parameters:  none
 * return:      unsigned long
 * error:       none
 */
static unsigned long i2c_test_eeprom(void)
{
    int i, j;
    DD_HANDLE i2c_hdl;
    unsigned char pReadData[8];
    unsigned char pWriteData[8];
    
    i2c_device_init();
    I2C_EEPROM_PRT("----- I2C1_test_eeprom start -----\r\n");

    for (j=0; j<100; j++)
    {
        delay_ms(100);

        for (i=0; i<8; i++)
        {
            pWriteData[i] = (i << 2) + 0x01 + j;
        }
        I2C_write_eeprom(0x00+j*8, pWriteData, 8);

        delay_ms(100);

        memset(pReadData, 0, 8);
        I2C_read_eeprom(0x00+j*8, pReadData, 8);

        if (memcmp(pReadData, pWriteData, 8) == 0)
        {
            os_printf("I2C_test_eeprom: memcmp %d ok!\r\n", j);
        }
        else
        {
            I2C_EEPROM_FATAL("I2C_test_eeprom: memcmp %d error!\r\n", j);
            for (i=0; i<8; i++)
            {
                I2C_EEPROM_FATAL("pReadData[%d]=0x%x, pWriteData[%d]=0x%x\r\n",
                                i, pReadData[i], i, pWriteData[i]);
            }
        }
    }

    I2C_EEPROM_PRT("----- i2c_test_eeprom over  -----\r\n");
    i2c_device_deinit();
    return 0;
}

MSH_CMD_EXPORT(i2c_test_eeprom,i2c_test_eeprom);

#endif

