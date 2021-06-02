#include "test_config.h"
#include <string.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <finsh.h>
#include "BkDriverFlash.h"

#ifdef RBL_HEADER_COPY_TEST

static int flash_protect(int argc, char **argv)
{
    PROTECT_TYPE protect_val = FLASH_PROTECT_NONE;

    if(argc == 2)
    {
        protect_val = atoi(argv[1]);
        
        if((uint32_t)protect_val > FLASH_UNPROTECT_LAST_BLOCK)
        {
            protect_val = 0;
        }
    }

    rt_kprintf("flash_protect  ==> %d\n", protect_val);

    bk_flash_enable_security(protect_val);
}
MSH_CMD_EXPORT(flash_protect, flash_protect);

static int flash_erase(int argc, char **argv)
{
    if(argc == 2)
    {
        uint32_t address = atoi(argv[1]);

        rt_kprintf("flash_erase %d 0x%08X\n", address, address);

        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &address);
    }
    else
    {
        rt_kprintf("ex: flash_erase 2093056\n");
    }

    return 0;
}
MSH_CMD_EXPORT(flash_erase, flash_erase 2093056);

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void dump_hex(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;

    for (i=0; i<buflen; i+=16)
    {
        rt_kprintf("%08X: ", i);

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%02X ", buf[i+j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%c", __is_print(buf[i+j]) ? buf[i+j] : '.');
        rt_kprintf("\n");
    }
}

static int flash_dump(int argc, char **argv)
{
    uint32_t address, len;
    void *data;

    if(argc != 3)
    {
        rt_kprintf("ex: flash_dump 2093056\n");
        return -1;
    }

    address = atoi(argv[1]);
    len = atoi(argv[2]);

    rt_kprintf("flash_dump 0x%08X %d\n", address, len);

    if( (address > (1024*1024*4)) || (len > 256) )
    {
        rt_kprintf("out of range\n");
        return -1;
    }

    data = rt_malloc(len);
    if(!data)
    {
        rt_kprintf("no memory for data\n");
        return -1;
    }

    flash_read(data, len, address);

    dump_hex(data, len);

    rt_free(data);

    return 0;
}
MSH_CMD_EXPORT(flash_dump, flash_dump 2093056 32);

static int flash_copy(int argc, char **argv)
{
    uint32_t address_src, address_dest, len;
    void *data;

    if(argc != 4)
    {
        rt_kprintf("ex: flash_copy 0 4096 256\n");
        return -1;
    }

    address_src = atoi(argv[1]);
    address_dest = atoi(argv[2]);
    len = atoi(argv[3]);
    rt_kprintf("address src 0x%08X ==> dest 0x%08X len:%d\n", address_src, address_dest, len);

    data = rt_malloc(len);
    if(!data)
    {
        rt_kprintf("no memory for data\n");
        return -1;
    }

    flash_read(data, len, address_src);
    flash_write(data, len, address_dest);

    rt_free(data);

    return 0;
}
MSH_CMD_EXPORT(flash_copy, flash_dump 2093056 32);
#define PRINT_RBL_INFO
#include <dfs_posix.h>
/**
 *指令格式 rbl_header_copy 2084864 /sd/rtthread.rbl
 *2084864为内部flash倒数第三个分区的起始地址,如对应2M bytes为0x1fd000<-->2084864
*/
static int rbl_header_copy(int argc, char **argv)
{
    uint32_t address = atoi(argv[1]);
    const char *fn = argv[2];
    int fd = open(fn, O_RDONLY, 0);

    if(fd >= 0)
    {
        uint8_t data[96];
        int len = 96;
        
        read(fd, data, 96);
#if defined (PRINT_RBL_INFO)
		int i;
		for(i = 0;i<96;i++)
		{
			rt_kprintf("%02x ",data[i]);
			if((i+1)%16 == 0)
				rt_kprintf("\r\n");
		}
#endif
        
        bk_flash_enable_security(/*FLASH_PROTECT_HALF*/FLASH_PROTECT_NONE); // half or custom
        
        rt_kprintf("flash_erase %d 0x%08X\n", address, address);
        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &address);
        flash_write(data, len, address);
		
#if defined (PRINT_RBL_INFO)		
		memset(data,0,len);
		flash_read(data, len, address);
		for(i = 0;i<96;i++)
		{
			rt_kprintf("%02x ",data[i]);
			if((i+1)%16 == 0)
				rt_kprintf("\r\n");
		}
#endif

        close(fd);
    }
	else
	{
		 rt_kprintf("[%s] fd:%d\n",__func__,fd);
	}

    return 0;
}
MSH_CMD_EXPORT(rbl_header_copy, copy rbl.header to flash);
#endif
