#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include <rthw.h>
#include <string.h>
#include <time.h>
#include <drv_iic.h>
#include "test_config.h"
#if (BEKEN_USING_IIC && RT_USING_I2C)
#define eeprom_addr   0x50;  /* 1010A2A1A0 -R/W */

#ifdef I2C_RTT_TEST
/************************I2C sample**********************/
static int i2c_test_rtt(int argc, char *argv)
{
    const char *i2c_bus_device_name = "i2c";
    struct rt_i2c_bus_device *i2c_device;
    struct rt_i2c_msg msgs[2];
    
    rt_uint8_t buffer1[2];
    rt_uint8_t buffer2[3];
    rt_size_t i, ret;
    rt_uint8_t ret1;
    
    ret1 = iic_bus_attach( );				/*gpio init and add bus*/   
    rt_kprintf("iic_bus_attach  ret:%d\n", ret1);    

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if (i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    else
    {
        rt_kprintf("find i2c success\n");
    }
	
    /**********************step 1: read out.************************/
	
    buffer1[0] = 0;
    msgs[0].addr = eeprom_addr;
    msgs[0].flags = RT_I2C_WR;		/* write to slave */
    msgs[0].buf = buffer1;			/* eeprom offset. */
    msgs[0].len = 1;
	
    msgs[1].addr = eeprom_addr;
    msgs[1].flags = RT_I2C_RD; 		/* Read from slave */
    msgs[1].buf = buffer2;
    msgs[1].len = sizeof(buffer2);
	
	if ( rt_i2c_transfer(i2c_device, msgs, 2) !=2 )  /* write or read data */
	{
		rt_kprintf("--read eeprom fail--\r\n");
	}
	else

	{
		rt_kprintf("--read eeprom sucess--\r\n");
	}
    for(i=0; i<sizeof(buffer2); i++)
    {
        rt_kprintf("%02X ", buffer2[i]);
    }
	
	rt_thread_delay(rt_tick_from_millisecond(50));	
    rt_kprintf("\r\n---read test over---\r\n");	

/**********************step 2: write back.************************/

	for(i=0; i<sizeof(buffer2); i++)
	{
		buffer2[i] = buffer2[i]+5 ;
	}
	
	msgs[0].addr = eeprom_addr;
    msgs[0].flags = RT_I2C_WR;		/* write to slave */
    msgs[0].buf = buffer1;			/* eeprom offset. */
    msgs[0].len = 1;
	
    msgs[1].addr = eeprom_addr;
    msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START; 		
    msgs[1].len = sizeof(buffer2);
		
	if ( rt_i2c_transfer(i2c_device, msgs, 2) !=2 )
	{
		rt_kprintf("---write eeprom fail---\r\n");
	}
	else

	{
		rt_kprintf("---write eeprom sucess---\r\n");
	}
	
	
	rt_thread_delay(rt_tick_from_millisecond(50));	
	
    for(i=0; i<msgs[1].len; i++)
    {
        rt_kprintf("%02X ", buffer2[i]);
    }
	
    rt_kprintf("\r\n ---write test over---\r\n");
	
/**********************step 3:re-read out.************************/

    buffer1[0] = 0;

	msgs[0].addr = eeprom_addr;
	msgs[0].flags = RT_I2C_WR;		/* write to slave */
	msgs[0].buf = buffer1;			/* eeprom offset. */
	msgs[0].len = 1;
		
	msgs[1].addr = eeprom_addr;
	msgs[1].flags = RT_I2C_RD; 		/* Read from slave */
	msgs[1].buf = buffer2;
	msgs[1].len = sizeof(buffer2);
				
	if ( rt_i2c_transfer(i2c_device, msgs, 2) !=2 )
	{
		rt_kprintf("---re-read eeprom fail---\r\n");
	}
	else

	{
		rt_kprintf("---re-read eeprom sucess---\r\n");
	}
	
   // rt_kprintf("rt_i2c_transfer :%d\r\n",ret);		
	rt_thread_delay(rt_tick_from_millisecond(50));	
		
	for(i=0; i<msgs[1].len; i++)
	{
		rt_kprintf("%02X ", buffer2[i]);
	}
		
	rt_kprintf("\r\n ---re-read test over---\r\n");


    return 0;
}


//MSH_CMD_EXPORT(i2c_test, i2c_test);

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT_ALIAS(i2c_test_rtt, __cmd_i2c_test_rtt, i2c test rtt cm);

//FINSH_FUNCTION_EXPORT_ALIAS(i2c_test_set, __cmd_i2c_test_set, i2c test set cmd);
//FINSH_FUNCTION_EXPORT_ALIAS(i2c_test_get, __cmd_i2c_test_get, i2c test get cmd);
#endif

#endif
#endif // (BEKEN_USING_IIC && RT_USING_I2C)


