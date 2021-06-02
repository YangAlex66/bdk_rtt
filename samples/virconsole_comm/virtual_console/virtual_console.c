#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_POSIX)
#include <dfs_poll.h>
#include <dfs_posix.h>
#include <libc.h>

static int dev_old_flag;
#endif

#define VIRTUAL_DEVICE_NAME "dummy"

static struct rt_device _dummy_console;
static struct rt_device_ops dummy_console_ops = {0};

int virtual_console_init(void) 
{
     struct rt_device *device = &_dummy_console;


     device->type = RT_Device_Class_Char;
     device->rx_indicate = RT_NULL;
     device->tx_complete = RT_NULL;
     device->user_data   = RT_NULL;
     
#ifdef RT_USING_DEVICE_OPS  
    device->ops = &dummy_console_ops;
#endif

    return rt_device_register(device, VIRTUAL_DEVICE_NAME, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);
}

void virtual_console_start(void)
{
    /* set console */
    rt_console_set_device(VIRTUAL_DEVICE_NAME);
    
    /* set finsh device */
#if defined(RT_USING_POSIX)
    /* backup flag */
    dev_old_flag = ioctl(libc_stdio_get_console(), F_GETFL, (void *)RT_NULL);
    /* add non-block flag */
    ioctl(libc_stdio_get_console(), F_SETFL, (void *)(dev_old_flag | O_NONBLOCK));
    /* set tcp shell device for console */
    libc_stdio_set_console(VIRTUAL_DEVICE_NAME, O_RDWR);
    /* resume finsh thread, make sure it will unblock from last device receive */
    rt_thread_t tid = rt_thread_find(FINSH_THREAD_NAME);
    if (tid) 
    {
        rt_thread_resume(tid);
        rt_schedule();
    }
#else
    /* set finsh device */
    finsh_set_device(VIRTUAL_DEVICE_NAME);
#endif /* RT_USING_POSIX */
}

void virtual_console_stop(void)
{
    /* set console */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    
    /* set finsh device */
#if defined(RT_USING_POSIX)
    ioctl(libc_stdio_get_console(), F_SETFL, (void *)(uintptr_t)dev_old_flag);
    libc_stdio_set_console(RT_CONSOLE_DEVICE_NAME, O_RDWR);
#else
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif /* RT_USING_POSIX */
}

int virtual_console_deinit(void) 
{
    struct rt_device *device = &_dummy_console;

    
    return rt_device_unregister(device);
}

