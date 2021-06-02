#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <stdio.h>
#include <string.h>

#include "sys_config.h"
#include "test_config.h"
#include "mem_pub.h"

#ifdef VIDEO_BUF_TEST
#include "video_transfer.h"
void vbuf(int argc, char** argv)
{
    if(strcmp(argv[1], "open") == 0)
    {
        video_buffer_open();
    }
    else if(strcmp(argv[1], "read") == 0)
    {
        uint8_t *mybuf, i;
        uint32_t my_len;
        
        my_len = atoi(argv[2]);
        mybuf = os_malloc(my_len);

        if(mybuf == NULL)
        {
            rt_kprintf("vbuf test no buff\r\n");
            return;
        }

        my_len = video_buffer_read_frame(mybuf, my_len);
        rt_kprintf("frame_len: %d\r\n", my_len);

        if(0)
        {
            for(int i=0; i<my_len; i++)
            {
                rt_kprintf("%02x,", mybuf[i]);
                if((i+1)%32 == 0)
                    rt_kprintf("\r\n");
            }
        }

        os_free(mybuf);
    }
    else if(strcmp(argv[1], "close") == 0)
    {
        video_buffer_close();
    }
    else if(strcmp(argv[1], "setp") == 0)
    {
        uint32_t ppi, pfs;
        
        ppi = atoi(argv[2]);
        pfs = atoi(argv[3]);
        video_transfer_set_video_param(ppi, pfs);
    }
    else
    {
        rt_kprintf("vbuf open/read len/close/setp ppi pfs\r\n");
    }    
}
MSH_CMD_EXPORT(vbuf, vbuf);

#include "common.h"
extern void camera_intf_sccb_write(UINT8 addr, UINT8 data);
extern void camera_intf_sccb_read(UINT8 addr, UINT8 *data);
void sccb_read(int argc, char** argv)
{
    u8 addr;
    u8 data = 0;

    hexstr2bin(argv[1], &addr, 1);
    
    camera_intf_sccb_read(addr, &data);

    rt_kprintf("r:0x%02x: 0x%02x\r\n", addr, data);
}
MSH_CMD_EXPORT(sccb_read, sccb_read);

void sccb_write(int argc, char** argv)
{
    u8 addr;
    u8 data = 0;

    hexstr2bin(argv[1], &addr, 1);
    hexstr2bin(argv[2], &data, 1);

    rt_kprintf("w:0x%02x: 0x%02x\r\n", addr, data);
    
    camera_intf_sccb_write(addr, data);
}
MSH_CMD_EXPORT(sccb_write, sccb_write);

#endif // CONFIG_VIDEO_BUF_TEST

