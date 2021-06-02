#include "error.h"
#include "include.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include <rtdef.h>
#include "include.h"
#include <stdio.h>
#include <string.h>
#include "typedef.h"
#include "arm_arch.h"
#include "qspi_pub.h"
#include "BkDriverQspi.h"
#include "test_config.h"
#include "drv_wdt.h"


//#define QSPI_TEST
#ifdef  QSPI_TEST

#define QSPI_TEST_LENGTH			( 0x4 * 64 )
#define QSPI_TEST_ADDR				(0x100000*8)

static UINT8 DataOffset;

static void qspi_psram_dcache_test(int argc,char *argv[])
{
	UINT32 i,ret;
	UINT32 SetLineMode;
	qspi_dcache_drv_desc qspi_cfg;
		
	UINT32*   p_WRData1;
	UINT32*   p_WRData2;
	UINT32*   p_WRData3;
	UINT32*   p_WRData4;
	UINT32*   p_WRData5;
	
	UINT32*   p_RDData1;
	UINT32*   p_RDData2;
	UINT32*   p_RDData3;
	UINT32*   p_RDData4;
	UINT32*   p_RDData5;

	p_WRData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData1[0]));
	if(p_WRData1 == RT_NULL)
	{
		rt_kprintf("p_WRData1 malloc failed\r\n");
	}
	
	p_WRData2 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData2[0]));
	if(p_WRData2 == RT_NULL)
	{
		rt_kprintf("p_WRData2 malloc failed\r\n");
	}
	
	p_WRData3 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData3[0]));
	if(p_WRData3 == RT_NULL)
	{
		rt_kprintf("p_WRData3 malloc failed\r\n");
	}
	
	p_WRData4 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData4[0]));
	if(p_WRData4 == RT_NULL)
	{
		rt_kprintf("p_WRData4 malloc failed\r\n");
	}

	p_WRData5 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData5[0]));
	if(p_WRData5 == RT_NULL)
	{
		rt_kprintf("p_WRData5 malloc failed\r\n");
	}

	p_RDData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData1[0]));
	if(p_RDData1 == RT_NULL)
	{
		rt_kprintf("p_RDData1 malloc failed\r\n");
	}
	
	p_RDData2 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData2[0]));
	if(p_RDData2 == RT_NULL)
	{
		rt_kprintf("p_RDData2 malloc failed\r\n");
	}
	
	p_RDData3 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData3[0]));
	if(p_RDData3 == RT_NULL)
	{
		rt_kprintf("p_RDData3 malloc failed\r\n");
	}

	p_RDData4 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData4[0]));
	if(p_RDData4 == RT_NULL)
	{
		rt_kprintf("p_RDData4 malloc failed\r\n");
	}

	p_RDData5 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData5[0]));
	if(p_RDData5 == RT_NULL)
	{
		rt_kprintf("p_RDData5 malloc failed\r\n");
	}
	

	for(i=0; i<QSPI_TEST_LENGTH; i++)
	{
		p_WRData1[i]= ((i+1)<<24) | ((i+1)<<16)	| ((i+1)<<8) | ((i+1)<<0) |0x70707070;
		p_WRData2[i]= ((i+1)<<24) | ((i+1)<<16)	| ((i+1)<<8) | ((i+1)<<0) |0x80808080;	
		p_WRData3[i]= ((i+1)<<24) | ((i+1)<<16)	| ((i+1)<<8) | ((i+1)<<0) |0x90909090;
		p_WRData4[i]= ((i+1)<<24) | ((i+1)<<16)	| ((i+1)<<8) | ((i+1)<<0) |0xe0e0e0e0;
		p_WRData5[i]= ((i+1)<<24) | ((i+1)<<16)	| ((i+1)<<8) | ((i+1)<<0) |0xf0f0f0f0;		
	}
	
	if(argc == 2)
	{			
		rt_kprintf("[qspi_test]:test_qspi_dcache_write_read_data\r\n");
		
		SetLineMode = atoi(argv[1]);
		

		rt_kprintf("SetLineMode = 0x%x\r\n", SetLineMode);

        if(SetLineMode == 0)
        {
            bk_qspi_psram_quad_mode_switch(0);//set psram line mode as 1 line
        }
        else if(SetLineMode == 3)
        {
            bk_qspi_psram_quad_mode_switch(1);//set psram line mode as 4 line
        }
        else
        {
            rt_kprintf("SetLineMode err! LineMode should be number 0 or 3\r\n");
        }

		qspi_cfg.mode = 0x00 | SetLineMode;		// 0: 1 line mode    3: 4 line mode
		qspi_cfg.clk_set = 0x10;
		qspi_cfg.wr_command = SetLineMode ? 0x38 : 0x02;		//write
		qspi_cfg.rd_command = SetLineMode ? 0xEB : 0x03;		//read
		qspi_cfg.wr_dummy_size = 0;
		qspi_cfg.rd_dummy_size = SetLineMode ? 0x06 : 0x00;

		bk_qspi_dcache_initialize(&qspi_cfg);
		bk_qspi_start();

		bk_qspi_dcache_write_data(0x00000, p_WRData1, QSPI_TEST_LENGTH);
		bk_qspi_dcache_write_data(0x04000, p_WRData2, QSPI_TEST_LENGTH);
		bk_qspi_dcache_write_data(0x08000, p_WRData3, QSPI_TEST_LENGTH);
		bk_qspi_dcache_write_data(0x0C000, p_WRData4, QSPI_TEST_LENGTH);
		bk_qspi_dcache_write_data(0x10000, p_WRData5, QSPI_TEST_LENGTH);

		rt_thread_delay(rt_tick_from_millisecond(100));	

		bk_qspi_dcache_read_data(0x00000, p_RDData1, QSPI_TEST_LENGTH);
		bk_qspi_dcache_read_data(0x04000, p_RDData2, QSPI_TEST_LENGTH);
		bk_qspi_dcache_read_data(0x08000, p_RDData3, QSPI_TEST_LENGTH);
		bk_qspi_dcache_read_data(0x0C000, p_RDData4, QSPI_TEST_LENGTH);
		bk_qspi_dcache_read_data(0x10000, p_RDData5, QSPI_TEST_LENGTH);
		
		if(memcmp(p_WRData1, p_RDData1, QSPI_TEST_LENGTH*4) == 0)
		{
			rt_kprintf("[qspi_test]:qspi read data 1 pass \r\n ");
		}
		else
		{
			rt_kprintf("[qspi_test]:qspi read data 1 error !!! \r\n ");
			
			for (i=0; i<QSPI_TEST_LENGTH; i++)
			{
				rt_kprintf("p_WRData[%d]=0x%lx, p_RDData[%d]=0x%lx\r\n", i, *(p_WRData1 + i), i, *(p_RDData1 + i));
			}
		}
		
		if(memcmp(p_WRData2, p_RDData2, QSPI_TEST_LENGTH*4) == 0)
		{
			rt_kprintf("[qspi_test]:qspi read data 2 pass \r\n ");
		}
		else
		{
			rt_kprintf("[qspi_test]:qspi read data 2 error !!! \r\n ");
			
			for (i=0; i<QSPI_TEST_LENGTH; i++)
			{
				rt_kprintf("p_WRData[%d]=0x%lx, p_RDData[%d]=0x%lx\r\n", i, *(p_WRData2 + i), i, *(p_RDData2 + i));
			}
		}
		
		if(memcmp(p_WRData3, p_RDData3, QSPI_TEST_LENGTH*4) == 0)
		{
			rt_kprintf("[qspi_test]:qspi read data 3 pass \r\n ");
		}
		else
		{
			rt_kprintf("[qspi_test]:qspi read data 3 error !!! \r\n ");			
			for (i=0; i<QSPI_TEST_LENGTH; i++)
			{
				rt_kprintf("p_WRData[%d]=0x%lx, p_RDData[%d]=0x%lx\r\n", i, *(p_WRData3 + i), i, *(p_RDData3 + i));
			}
		}
		if(memcmp(p_WRData4, p_RDData4, QSPI_TEST_LENGTH*4) == 0)
		{
			rt_kprintf("[qspi_test]:qspi read data 4 pass \r\n ");

		}
		else
		{
			rt_kprintf("[qspi_test]:qspi read data 4 error !!! \r\n ");
			
			for (i=0; i<QSPI_TEST_LENGTH; i++)
			{
				rt_kprintf("p_WRData[%d]=0x%lx, p_RDData[%d]=0x%lx\r\n", i, *(p_WRData4 + i), i, *(p_RDData4 + i));
			}
		}

		if(memcmp(p_WRData5, p_RDData5, QSPI_TEST_LENGTH*4) == 0)
		{
			rt_kprintf("[qspi_test]:qspi read data 5 pass \r\n ");
		}
		else
		{
			rt_kprintf("[qspi_test]:qspi read data 5 error !!! \r\n ");
			
			for (i=0; i<QSPI_TEST_LENGTH; i++)
			{
				rt_kprintf("p_WRData[%d]=0x%lx, p_RDData[%d]=0x%lx\r\n", i, *(p_WRData5 + i), i, *(p_RDData5 + i));
			}
		}

		if(p_WRData1 != RT_NULL)
		{
			rt_free(p_WRData1);
			p_WRData1= RT_NULL;
		}
		if(p_WRData2 != RT_NULL)
		{
			rt_free(p_WRData2);
			p_WRData2= RT_NULL;
		}		
		if(p_WRData3 != RT_NULL)
		{
			rt_free(p_WRData3);
			p_WRData3= RT_NULL;
		}
		if(p_WRData4 != RT_NULL)
		{
			rt_free(p_WRData4);
			p_WRData4= RT_NULL;
		}
		if(p_WRData5 != RT_NULL)
		{
			rt_free(p_WRData5);
			p_WRData5= RT_NULL;
		}

		if(p_RDData1 != RT_NULL)
		{
			rt_free(p_RDData1);
			p_RDData1= RT_NULL;
		}
		if(p_RDData2 != RT_NULL)
		{
			rt_free(p_RDData2);
			p_RDData2= RT_NULL;
		}
		if(p_RDData3 != RT_NULL)
		{
			rt_free(p_RDData3);
			p_RDData3= RT_NULL;
		}
		if(p_RDData4 != RT_NULL)
		{
			rt_free(p_RDData4);
			p_RDData4= RT_NULL;
		}
		if(p_RDData5 != RT_NULL)
		{
			rt_free(p_RDData5);
			p_RDData5= RT_NULL;
		}	
	}
	else
	{
		rt_kprintf("[qspi_test]:argc error!!! \r\n");
	}
}

static void qspi_psram_switch_line_mode(int argc,char *argv[])
{
	uint8 exit;
	if(argc == 2)
	{
		exit = atoi(argv[1]);	
		bk_qspi_psram_quad_mode_switch(exit);
	}
	else
	{
		rt_kprintf("param error\r\n");
	}
	
}

static void qspi_psram_line_mode_test(int argc,char *argv[])
{
	UINT32 i,ret;
	UINT32 SetLineMode;
	qspi_dcache_drv_desc qspi_cfg;
	UINT32 addr = 0;
		
	UINT32*   p_WRData1;
	UINT32*   p_RDData1;

	p_WRData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData1[0]));
	if(p_WRData1 == RT_NULL)
	{
		rt_kprintf("p_WRData1 malloc failed\r\n");
	}
	
	p_RDData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData1[0]));
	if(p_RDData1 == RT_NULL)
	{
		rt_kprintf("p_RDData1 malloc failed\r\n");
	}
	
	for(i=0; i<QSPI_TEST_LENGTH; i++)
	{
		p_WRData1[i]= ((i+1)<<24) | ((i+1)<<16) | ((i+1)<<8) | ((i+1)<<0) |0x70707070;
	}
	
	if(argc == 2)
	{			
		rt_kprintf("[qspi_test]:test_qspi_dcache_write_read_data\r\n");
		
		SetLineMode = atoi(argv[1]);

		if(SetLineMode == 0)						//1 Line mode 
		{
			rt_kprintf("1 line mode: %x\r\n",SetLineMode);
			bk_qspi_psram_quad_mode_switch(0);	
		}
		else if (SetLineMode == 3)
		{
			rt_kprintf("4 line mode: %x\r\n",SetLineMode);
			bk_qspi_psram_quad_mode_switch(1);			//4 Line mode 
		}	
		
		qspi_cfg.mode = 0x00 | SetLineMode; 	// 0: 1 line mode	 3: 4 line mode
		qspi_cfg.clk_set = 0x10;
		qspi_cfg.wr_command = SetLineMode ? 0x38 : 0x02;		//write
		qspi_cfg.rd_command = SetLineMode ? 0xEB : 0x03;		//read
		qspi_cfg.wr_dummy_size = 0;
		qspi_cfg.rd_dummy_size = SetLineMode ? 0x06 : 0x00;

		bk_qspi_dcache_initialize(&qspi_cfg);
		bk_qspi_start();
		
		rt_kprintf("write start time:%d\r\n",rt_tick_get());
		
		while(addr < QSPI_TEST_ADDR)
		{
			bk_qspi_dcache_write_data(addr, p_WRData1, QSPI_TEST_LENGTH);
			addr += 1024;
		}

		//rt_thread_delay(rt_tick_from_millisecond(100)); 
		rt_kprintf("write stop time:%d\r\n",rt_tick_get());

		addr = 0;
		
		rt_kprintf("read start time:%d\r\n",rt_tick_get());
		
    	while(addr < QSPI_TEST_ADDR)
	    {
	        bk_qspi_dcache_read_data(addr, p_RDData1, QSPI_TEST_LENGTH);
				    
	        if (memcmp(p_WRData1, p_RDData1, QSPI_TEST_LENGTH*4) == 0)
	        {
	            //rt_kprintf("----- psram_dcache_write_read_data pass addr:0x%x -----\r\n", addr);
	        }
	        else
	        {
	            rt_kprintf("\r\nmemcmp failed:\r\n");
	            for (i=0; i<QSPI_TEST_LENGTH; i++)
	            {
	                if (p_WRData1[i] != p_RDData1[i])
	                {
	                    rt_kprintf("p_WRData1[%d]=0x%lx, p_RDData1[%d]=0x%lx\r\n", i, p_WRData1[i], i, p_RDData1[i]);
	                }
	            }
	            rt_kprintf("----- psram_dcache_write_read_data error:addr:0x%x-----\r\n", addr);
	        }
	        
	        addr += 1024;    
	    }
		
		rt_kprintf("read stop time:%d\r\n",rt_tick_get());

		if(p_WRData1 != RT_NULL)
		{
			rt_free(p_WRData1);
			p_WRData1= RT_NULL;
		}

		if(p_RDData1 != RT_NULL)
		{
			rt_free(p_RDData1);
			p_RDData1= RT_NULL;
		}
	}
	else
	{
		rt_kprintf("[qspi_test]:argc error!!! \r\n");
	}
}

static void qspi_psram_read_id_test(int argc,char *argv[])
{	
	UINT8 psram_id;
	
	psram_id = bk_qspi_psram_read_id();

	rt_kprintf("psram_id = 0x%x\r\n",psram_id);

}
static void qspi_psram_while_test(int argc,char *argv[])
{
	UINT32 i,ret;
	UINT32 SetLineMode;
	qspi_dcache_drv_desc qspi_cfg;
	UINT32 addr = 0;
	UINT32 count,err_count =0;
	UINT32 err_flag = 1;
		
	UINT32*   p_WRData1;
	UINT32*   p_RDData1;

	p_WRData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_WRData1[0]));
	if(p_WRData1 == RT_NULL)
	{
		rt_kprintf("p_WRData1 malloc failed\r\n");
	}
	
	p_RDData1 = rt_malloc(QSPI_TEST_LENGTH * sizeof(p_RDData1[0]));
	if(p_RDData1 == RT_NULL)
	{
		rt_kprintf("p_RDData1 malloc failed\r\n");
	}
	
	for(i=0; i<QSPI_TEST_LENGTH; i++)
	{
		p_WRData1[i]= ((i+1)<<24) | ((i+1)<<16) | ((i+1)<<8) | ((i+1)<<0) |0x70707070;
	}
	
	//if(argc == 2)
	{			
		rt_kprintf("[qspi_test]:test_qspi_dcache_write_read_data\r\n");
		
		SetLineMode = 3;
	
		rt_kprintf("line mode: %x\r\n",SetLineMode);
		bk_qspi_psram_quad_mode_switch(1);	

		qspi_cfg.mode = 0x00 | SetLineMode; 	// 0: 1 line mode	 3: 4 line mode
		qspi_cfg.clk_set = 0x10;
		qspi_cfg.wr_command = SetLineMode ? 0x38 : 0x02;		//write
		qspi_cfg.rd_command = SetLineMode ? 0xEB : 0x03;		//read
		qspi_cfg.wr_dummy_size = 0;
		qspi_cfg.rd_dummy_size = SetLineMode ? 0x06 : 0x00;

		bk_qspi_dcache_initialize(&qspi_cfg);
		bk_qspi_start();
			
		while(1)	
		{	
			addr = 0;
			err_flag = 0;
			rt_kprintf("write start time:%d\r\n",rt_tick_get());
			
			while(addr < QSPI_TEST_ADDR)
			{
				bk_qspi_dcache_write_data(addr, p_WRData1, QSPI_TEST_LENGTH);
				addr += 1024;
			}
	
			//rt_thread_delay(rt_tick_from_millisecond(100)); 
			rt_kprintf("write stop time:%d\r\n",rt_tick_get());
	
			addr = 0;
			
			rt_kprintf("read start time:%d\r\n",rt_tick_get());
			
			while(addr < QSPI_TEST_ADDR)
			{
				bk_qspi_dcache_read_data(addr, p_RDData1, QSPI_TEST_LENGTH);
						
				if (memcmp(p_WRData1, p_RDData1, QSPI_TEST_LENGTH*4) == 0)
				{
					//rt_kprintf("----- psram_dcache_write_read_data pass addr:0x%x -----\r\n", addr);
				}
				else
				{
					rt_kprintf("\r\nmemcmp failed:\r\n");
					for (i=0; i<QSPI_TEST_LENGTH; i++)
					{
						if (p_WRData1[i] != p_RDData1[i])
						{
							rt_kprintf("p_WRData1[%d]=0x%lx, p_RDData1[%d]=0x%lx\r\n", i, p_WRData1[i], i, p_RDData1[i]);
						}
					}
					rt_kprintf("----- psram_dcache_write_read_data error:addr:0x%x-----\r\n", addr);
					err_flag = 1;
				}
				
				addr += 1024;			
			}

			if(err_flag ==1)
			{
				err_count ++;

			}

			count ++;
			rt_kprintf("read stop time:%d, rw test count:%d, test_fail:%d\r\n",rt_tick_get(), count, err_count);
		}

		
		if(p_WRData1 != RT_NULL)
		{
			rt_free(p_WRData1);
			p_WRData1= RT_NULL;
		}

		if(p_RDData1 != RT_NULL)
		{
			rt_free(p_RDData1);
			p_RDData1= RT_NULL;
		}
	}
//		else
//		{
//			rt_kprintf("[qspi_test]:argc error!!! \r\n");
//		}
}



#define TEST_PIN_NUM 20
void mem_bus_test(uint32_t address, uint32_t size, int test_mode,int rw_way)
{   
    uint32_t i;
    //rt_pin_mode(TEST_PIN_NUM, PIN_MODE_OUTPUT);
    //rt_pin_write(TEST_PIN_NUM, PIN_LOW);

    if (test_mode >= 0 && test_mode <= 2) {
        switch (test_mode) {
            case 0: {
                rt_kprintf("\n8bit test start memtest,address: 0x%08X size: 0x%08X\r\n", address, size);
                uint8_t *p_uint8_t = 0;
                if(rw_way == 0 || rw_way == 1) {
                    p_uint8_t = (uint8_t *)address;
                    for (i = 0; i < size / sizeof(uint8_t); i++) {
                        *p_uint8_t++ = (uint8_t)i;
                        if(i == (8*1024 -1)) {
                            //rt_pin_write(TEST_PIN_NUM, PIN_HIGH);
                            rt_kprintf("test..... \n");
                        }
                    }
                }
                if(rw_way == 0 || rw_way == 2) {
                    p_uint8_t = (uint8_t *)address;
                    for (i = 0; i < size / sizeof(uint8_t); i++) {
                        if (*p_uint8_t != (uint8_t)i) {
                            rt_kprintf("8bit test fail @ 0x%08X,rd:%x,wd:%x  system halt!!!!!\r\n", (uint32_t)p_uint8_t, (uint8_t)*p_uint8_t, (uint8_t)i);
                            //while (1)
                            //    ;
                            //continue;
                        }
                        p_uint8_t++;
                    }
                }
                rt_kprintf("8bit test pass!!\r\n");
            } break;

            case 1:
                /**< 16bit test */
                rt_kprintf("\n16bit test start memtest,address: 0x%08X size: 0x%08X\r\n", address, size);
                {
                    uint16_t *p_uint16_t = 0;
                    if(rw_way == 0 || rw_way == 1) {
                        p_uint16_t =  (uint16_t *)address;
                        for (i = 0; i < size / sizeof(uint16_t); i++) {
                            *p_uint16_t++ = (uint16_t)i;
                        }
                    }

                    if(rw_way == 0 || rw_way == 2) {
                        p_uint16_t = (uint16_t *)address;
                        for (i = 0; i < size / sizeof(uint16_t); i++) {
                            if (*p_uint16_t != (uint16_t)i) {
                                rt_kprintf("16bit test fail @ 0x%08X,rd:%x,wd:%x system halt!!!!!\r\n", (uint32_t)p_uint16_t, (uint16_t)*p_uint16_t, (uint16_t)i);
                                //while (1)
                                //    ;
                                //continue;
                            }
                            p_uint16_t++;
                        }
                    }
                    rt_kprintf("16bit test pass!!\r\n");
                }
                break;

            case 2:
                /**< 32bit test */
                rt_kprintf("\n32bit test start memtest,address: 0x%08X size: 0x%08X\r\n", address, size);
                {
                    uint32_t *p_uint32_t = 0;
                    if(rw_way == 0 || rw_way == 1) {
                        p_uint32_t = (uint32_t *)address;
                        for (i = 0; i < size / sizeof(uint32_t); i++) {
                            *p_uint32_t++ = (uint32_t)i;
                        }
                    }

                    if(rw_way == 0 || rw_way == 2) {
                        p_uint32_t = (uint32_t *)address;
                        for (i = 0; i < size / sizeof(uint32_t); i++) {
                            if (*p_uint32_t != (uint32_t)i) {
                                rt_kprintf("32bit test fail @ 0x%08X,rd:%x,wd:%x  system halt!!!!!\r\n", (uint32_t)p_uint32_t, (uint32_t)*p_uint32_t, (uint32_t)i);
                                //while (1)
                                //    ;
                                //continue;
                            }
                            p_uint32_t++;
                        }
                    }
                    rt_kprintf("32bit test pass!!\r\n");
                }
                break;

            default:
                break;
        }
    }
}

void test_bus_mem(int argc, char **argv)
{
    psram_init(0,2);//NOTE:should set according to the actual situation! here default set as 1 line;3.3v
    rt_hw_wdg_stop();

    if (argc == 4) {
        mem_bus_test(0x03000000, atoi(argv[3]) * 1024, atoi(argv[1]), atoi(argv[2]));
    } else {
        rt_kprintf("ex:test_bus_mem x1 x2 x3\r\n");
        rt_kprintf(" x1 is rw size (0:8 bit 1:16 bit 2:32bit) x2 is rw_way (0:rw 1:write only 2:read only ); x3 is checksize\r\n");
    }

    rt_hw_wdg_start(0,NULL);
}
MSH_CMD_EXPORT(test_bus_mem, test_bus_mem);


FINSH_FUNCTION_EXPORT_ALIAS(qspi_psram_dcache_test, __cmd_qspi_test, test qspi_psram_dcache mode);
MSH_CMD_EXPORT(qspi_psram_switch_line_mode, qspi_psram_switch_line_mode);
MSH_CMD_EXPORT(qspi_psram_line_mode_test, qspi_psram_line_mode_test);
MSH_CMD_EXPORT(qspi_psram_read_id_test, qspi_psram_read_id_test);
MSH_CMD_EXPORT(qspi_psram_while_test, qspi_psram_while_test);

#endif
