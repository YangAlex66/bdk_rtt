#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <string.h>
#include "comm_uart.h"


typedef struct 
{
    rt_device_t device;
    rt_sem_t rx_sem;
} comm_uart_st;
static comm_uart_st g_comm_uart;

static int comm_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    if(!dev)
    {
        return -1;
    }

    
    return rt_sem_release(g_comm_uart.rx_sem);
}

uint8_t comm_uart_getchar(void)
{
    uint8_t ch;
    

    while (rt_device_read(g_comm_uart.device, (-1), &ch, 1) != 1) 
    {
        rt_sem_take(g_comm_uart.rx_sem, RT_WAITING_FOREVER);
    }

    return ch;
}

int comm_uart_putchar(const rt_uint8_t ch)
{
    return rt_device_write(g_comm_uart.device, 0, &ch, 1);
}

int comm_uart_sendstr(const char *str, size_t len) 
{
    return rt_device_write(g_comm_uart.device, 0, str, len);
}

int comm_uart_init(void)
{
    rt_device_t dev = RT_NULL;
    comm_uart_st *pcomm_uart = NULL;

    
    pcomm_uart = &g_comm_uart;
    pcomm_uart->rx_sem = rt_sem_create("com_rxsem", 0, RT_IPC_FLAG_FIFO);
    dev = rt_device_find(RT_CONSOLE_DEVICE_NAME);
    if (dev == RT_NULL) 
    {
        return -1;
    }

    int ret = rt_device_open(dev,RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (ret == RT_EOK) 
    {
        pcomm_uart->device = dev;
        rt_device_set_rx_indicate(dev, comm_uart_rx_ind);
    }
    else
    {
        comm_uart_sendstr("rt_device_open fail!\n", strlen("rt_device_open fail!\n"));
        return -1;
    }

    return 0;
}

int comm_uart_deinit(void)
{
    if(g_comm_uart.rx_sem)
    {
        rt_sem_delete(g_comm_uart.rx_sem);
        g_comm_uart.rx_sem = NULL;
    }
    
    if(g_comm_uart.device)
    {
        rt_device_set_rx_indicate(g_comm_uart.device, NULL);
        
        rt_device_close(g_comm_uart.device);
        g_comm_uart.device = NULL;
    }

    return 0;
}
