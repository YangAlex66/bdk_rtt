#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include <rthw.h>
#include <string.h>
#include <time.h>
#include <drv_iic.h>
#include "typedef.h"
#include "i2s_pub.h"
#include "test_config.h"

#ifdef I2S_RTT_TEST

#define I2S_TEST_BUFFER_COUNT    1
#define I2S_TEST_BUFFER_LENGTH   256

static rt_sem_t g_i2s_sync_sem;
/************************I2S sample**********************/
rt_err_t i2s_test_rtt_tx_done(rt_device_t dev, void *buffer)
{
    rt_kprintf("%s 0x%x\r\n", __FUNCTION__, buffer);
    if (RT_NULL != g_i2s_sync_sem)
    {
        rt_sem_release(g_i2s_sync_sem);
    }
}

int i2s_test_loopback(int sample_rate, int bits_length)
{
    const char *i2s_bus_device_name = I2S_DEV_NAME;
    rt_device_t i2s_device = RT_NULL;

    rt_uint32_t *buffers[2];
    rt_err_t result;
    rt_uint32_t buffer_index;
    rt_uint8_t index;
    rt_uint8_t master = 0;

    memset((void *)buffers, 0, sizeof(buffers));

    for (index = 0; index < 2; index++)
    {
        buffers[index] = (rt_uint32_t *)malloc(I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
        if (RT_NULL == buffers[index])
        {
            rt_kprintf("%s buffers[%d]=NULL\r\n", __FUNCTION__, index);
            goto exit;
        }
        if (index == 0)
        {
            for (buffer_index = 0; buffer_index < I2S_TEST_BUFFER_LENGTH; buffer_index++)
            {
                buffers[index][buffer_index] = 0x80004000 | (index<<24) | (buffer_index<<16) | (index<<8) | (buffer_index<<0);
            }
        }
        else
        {
            memset((void *)buffers[index], 0, I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
        }
    }

    i2s_device = rt_device_find(i2s_bus_device_name);
    if (i2s_device == RT_NULL)
    {
        rt_kprintf("i2s bus device %s not found!\r\n", i2s_bus_device_name);
        goto exit;
    }
    else
    {
        rt_kprintf("find i2s success\r\n");
    }

    if(rt_device_init(i2s_device) != RT_EOK)
    {
        rt_kprintf(" --i2s device init failed---\r\n ");
        return 0;
    }

    result = rt_device_open(i2s_device, RT_DEVICE_FLAG_RDWR);
    if (RT_EOK != result)
    {
        rt_kprintf("open %s failed(%d)!\r\n", i2s_bus_device_name, result);
        goto exit;
    }

    /* NOTICE: should change sample_rate or bits_length before DMA enable */
    rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_SAMPLE_RATE_SET, (void *)&sample_rate);
    rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_BIT_LENGTH_SET, (void *)&bits_length);
    rt_uint32_t value = 1;
    rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_MASTER_ENABLE, (void *)&value);
    rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_RX_ENABLE, (void *)&value);
    rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_TX_ENABLE, (void *)&value);

    rt_device_set_tx_complete(i2s_device, i2s_test_rtt_tx_done);
    g_i2s_sync_sem = rt_sem_create("g_i2s_sync_sem", 1, RT_IPC_FLAG_FIFO);
    if (RT_NULL == g_i2s_sync_sem)
    {
        rt_kprintf("rt_sem_create(g_i2s_sync_sem) failed!\r\n");
        goto exit;
    }
    
    /* WRITE */
    {
        index = 0;
        rt_sem_take(g_i2s_sync_sem, RT_WAITING_FOREVER);
        rt_device_write(i2s_device, 0, buffers[index], I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
    }

    /* READ */
    {
        int offset = 0;
        index = 1;
        while (offset == 0)
        {
            result = rt_device_read(i2s_device, 0, buffers[index] + offset, (I2S_TEST_BUFFER_LENGTH - offset) * sizeof(buffers[index][0]));
            result = result / sizeof(buffers[index][0]);
            for (offset == 0; offset < result; offset++)
            {
                if (buffers[index][offset])
                {
                    /* skip empty data */
                    bk_printf("%d vs %d\n", offset, result);
                    memmove(buffers[index], buffers[index] + offset, (result - offset) * sizeof(buffers[index][0]));
                    break;
                }
            }
            if (offset < result)
            {
                offset = result - offset;
                break;
            }
            else
            {
                offset = 0;
                rt_thread_delay(rt_tick_from_millisecond(50));
            }
        }

        while (offset < I2S_TEST_BUFFER_LENGTH)
        {
            result = rt_device_read(i2s_device, 0, buffers[index] + offset, (I2S_TEST_BUFFER_LENGTH - offset) * sizeof(buffers[index][0]));
            offset += result / sizeof(buffers[index][0]);
            rt_thread_delay(rt_tick_from_millisecond(1));
        }

        for (buffer_index = 0; buffer_index < I2S_TEST_BUFFER_LENGTH; buffer_index++)
        {
            //rt_kprintf("buffers[%d][%d] 0x%x vs 0x%x\r\n", index, buffer_index, buffers[index][buffer_index], (0x80004000 | (index<<24) | (buffer_index<<16) | (buffer_index<<8) | (buffer_index<<0)));
            rt_kprintf("buffers[%d][%d] 0x%x vs 0x%x\r\n", index, buffer_index, buffers[index][buffer_index], buffers[0][buffer_index]);
        }
    }
    
    rt_kprintf("\r\n ---write test over---\r\n");

exit:
    for (index = 0; index < 2; index++)
    {
        if (RT_NULL != buffers[index])
        {
            free(buffers[index]);
            buffers[index] = RT_NULL;
        }
    }
    
    if (RT_NULL != i2s_device)
    {
        rt_device_set_tx_complete(i2s_device, RT_NULL);
        rt_device_close(i2s_device);
        i2s_device = RT_NULL;
    }

    if (RT_NULL != g_i2s_sync_sem)
    {
        rt_sem_delete(g_i2s_sync_sem);
        g_i2s_sync_sem = RT_NULL;
    }

    return 0;
}


static int i2s_test_rtt(int argc, char *argv[])
{
    const char *i2s_bus_device_name = I2S_DEV_NAME;
    rt_device_t i2s_device = RT_NULL;

    rt_uint32_t *buffers[I2S_TEST_BUFFER_COUNT];
    rt_err_t result;
    rt_uint32_t buffer_index;
    rt_uint8_t index;
    rt_uint8_t master = 0;


    if (argc < 2)
    {
        rt_kprintf("usage: %s both|master|slave\r\n", argv[0]);
        return 0;
    }

    if ('b' == argv[1][0])
    {
        int sample_rate = 44100;
        int bits_length = 32;

        if (argc > 3)
        {
            sample_rate = atoi(argv[2]);
            bits_length = atoi(argv[3]);
        }
        else if (argc > 2)
        {
            sample_rate = atoi(argv[2]);
        }
        rt_kprintf("%s both mode, will recieve data,sample_rate=%d,bits_length=%d\r\n", __FUNCTION__, sample_rate, bits_length);
        return i2s_test_loopback(sample_rate, bits_length);
    }
    else if ('m' == argv[1][0])
    {
        master = 1;
        rt_kprintf("%s master mode, will recieve data\r\n", __FUNCTION__);
    }
    else
    {
        rt_kprintf("%s slave mode, will send data\r\n", __FUNCTION__);
    }

    memset((void *)buffers, 0, sizeof(buffers));

    for (index = 0; index < I2S_TEST_BUFFER_COUNT; index++)
    {
        buffers[index] = (rt_uint32_t *)malloc(I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
        if (RT_NULL == buffers[index])
        {
            rt_kprintf("%s buffers[%d]=NULL\r\n", __FUNCTION__, index);
            goto exit;
        }
        if (master)
        {
            for (buffer_index = 0; buffer_index < I2S_TEST_BUFFER_LENGTH; buffer_index++)
            {
                buffers[index][buffer_index] = 0x80004000 | (index<<24) | (buffer_index<<16) | (index<<8) | (buffer_index<<0);
            }
        }
        else
        {
            memset((void *)buffers[index], 0, I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
        }
    }

    i2s_device = rt_device_find(i2s_bus_device_name);
    if (i2s_device == RT_NULL)
    {
        rt_kprintf("i2s bus device %s not found!\r\n", i2s_bus_device_name);
        goto exit;
    }
    else
    {
        rt_kprintf("find i2s success\r\n");
    }

    if(rt_device_init(i2s_device) != RT_EOK)
    {
        rt_kprintf(" --i2s device init failed---\r\n ");
        return 0;
    }
    
    if (master)
    {
        result = rt_device_open(i2s_device, RT_DEVICE_FLAG_WRONLY);
    }
    else
    {
        result = rt_device_open(i2s_device, RT_DEVICE_FLAG_RDONLY);
    }
    if (RT_EOK != result)
    {
        rt_kprintf("open %s failed(%d)!\r\n", i2s_bus_device_name, result);
        goto exit;
    }

    if (master)
    {
        rt_uint32_t enable = 1;
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_MASTER_ENABLE, (void *)&enable);
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_TX_ENABLE, (void *)&enable);
        rt_device_set_tx_complete(i2s_device, i2s_test_rtt_tx_done);
        g_i2s_sync_sem = rt_sem_create("g_i2s_sync_sem", 1, RT_IPC_FLAG_FIFO);
        if (RT_NULL == g_i2s_sync_sem)
        {
            rt_kprintf("rt_sem_create(g_i2s_sync_sem) failed!\r\n");
            goto exit;
        }
        
        for (index = 0; index < I2S_TEST_BUFFER_COUNT; index++)
        {
            rt_sem_take(g_i2s_sync_sem, RT_WAITING_FOREVER);
            rt_device_write(i2s_device, 0, buffers[index], I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
            if ('k' == argv[1][1])
            {
                int count = 100;
                for (; count > 0; count--)
                {
                    rt_sem_take(g_i2s_sync_sem, RT_WAITING_FOREVER);
                    rt_device_write(i2s_device, 0, buffers[index], I2S_TEST_BUFFER_LENGTH * sizeof(buffers[index][0]));
                    rt_thread_delay(rt_tick_from_millisecond(50));
                }
            }
        }
        
        enable = 0;
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_TX_ENABLE, (void *)&enable);
        rt_kprintf("\r\n ---write test over---\r\n");
    }
    else
    {
        rt_uint32_t enable = 0;
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_MASTER_ENABLE, (void *)&enable);

        enable = 1;
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_RX_ENABLE, (void *)&enable);
        rt_kprintf("keep rx dma enable!\r\n");
        for (index = 0; index < I2S_TEST_BUFFER_COUNT; index++)
        {
            int offset = 0;
            while (offset < I2S_TEST_BUFFER_LENGTH)
            {
                result = rt_device_read(i2s_device, 0, buffers[index] + offset, (I2S_TEST_BUFFER_LENGTH - offset) * sizeof(buffers[index][0]));
                offset += result / sizeof(buffers[index][0]);
                rt_thread_delay(rt_tick_from_millisecond(50));
            }
        }
        for (index = 0; index < I2S_TEST_BUFFER_COUNT; index++)
        {
            for (buffer_index = 0; buffer_index < I2S_TEST_BUFFER_LENGTH; buffer_index++)
            {
#if 0
                if (buffers[index][buffer_index] != ((index<<24) | (buffer_index<<16) | (buffer_index<<8) | (buffer_index<<0)))
                {
                    rt_kprintf("buffers[%d][%d] %d != %d\r\n", index, buffer_index, buffers[index][buffer_index], ((index<<24) | (buffer_index<<16) | (buffer_index<<8) | (buffer_index<<0)));
                    break;
                }
#else
                rt_kprintf("buffers[%d][%d] 0x%x vs 0x%x\r\n", index, buffer_index, buffers[index][buffer_index], (0x80004000 | (index<<24) | (buffer_index<<16) | (buffer_index<<8) | (buffer_index<<0)));
#endif
            }
        }
        
        enable = 0;
        rt_device_control(i2s_device, RT_DEVICE_CTRL_I2S_DMA_RX_ENABLE, (void *)&enable);
        rt_kprintf("\r\n ---read test over---\r\n");
    }

exit:
    for (index = 0; index < I2S_TEST_BUFFER_COUNT; index++)
    {
        if (RT_NULL != buffers[index])
        {
            free(buffers[index]);
            buffers[index] = RT_NULL;
        }
    }
    
    if (RT_NULL != i2s_device)
    {
        rt_device_set_tx_complete(i2s_device, RT_NULL);
        rt_device_close(i2s_device);
        i2s_device = RT_NULL;
    }

    if (RT_NULL != g_i2s_sync_sem)
    {
        rt_sem_delete(g_i2s_sync_sem);
        g_i2s_sync_sem = RT_NULL;
    }

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>

MSH_CMD_EXPORT(i2s_test_rtt, i2s_test_rtt);
#endif

#endif
