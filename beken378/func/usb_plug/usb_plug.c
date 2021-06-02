#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"

#include "include.h"
#include "mem_pub.h"
#include "uart_pub.h"
#include "str_pub.h"
#include "sys_ctrl_pub.h"
#include "gpio_pub.h"
#include "usb_pub.h"
#include "net_param_pub.h"
#include "fake_clock_pub.h"
#include "saradc_pub.h"
#include "saradc_intf.h"

#if (CFG_SOC_NAME == SOC_BK7221U)
#if CFG_USE_USB_CHARGE
#define CHARGE_INTERNAL_HW_MODE                             0
#define CHARGE_INTERNAL_SW_MODE                             1
#define CHARGE_EXTERNAL_HW_MODE                             2
#define CHARGE_EXTERNAL_SW_MODE                             3
#define CFG_CHARGE_MODE             CHARGE_EXTERNAL_SW_MODE

#define CFG_FULL_ACTION_STOP            0

#define ADC_CHANNEL0_VBAT	            0
#define VBAT_FULL_ADC_THRESHOLD         4180
#define VBAT_SAFETY_ADC_THRESHOLD       4220
#define VBAT_TRICKLE_ADC_THRESHOLD      3000
#define VBAT_RESTART_ADC_THRESHOLD      4130

#if (CFG_CHARGE_MODE == CHARGE_INTERNAL_HW_MODE)
#define USB_CHARGE_CB()  usb_charge_internal_hw_check()
#elif (CFG_CHARGE_MODE == CHARGE_INTERNAL_SW_MODE)
#define USB_CHARGE_CB()  usb_charge_internal_sw_check()
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_HW_MODE)
#define USB_CHARGE_CB()  usb_charge_external_hw_check()
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_SW_MODE)
#define USB_CHARGE_CB()  usb_charge_external_sw_check()
#endif

#define CHARGE_CAL_EFUSE            0
#define CHARGE_CAL_ITEM             1
#define CHARGE_CAL_POS        CHARGE_CAL_ITEM

static UINT32 charge_elect = 530;
static UINT32 charge_func_init = 0;
static UINT32 last_second, keep_count = 0;
static CHARGE_OPER_ST charge_cfg;
static signed long  CodeOffset = 0, dvbatref = 0;
static int last_vol;

int wifi_get_charge_cal_from_efuse(UINT8 *cal)
{
    EFUSE_OPER_ST efuse;
    int i = 0, ret;

    if(!cal)
        return 0;

    for(i = 0; i < EFUSE_CHARGE_CAL_LEN; i++)
    {
        efuse.addr = EFUSE_CHARGE_CAL_ADDR + i;
        efuse.data = 0;

        ret = sddev_control(SCTRL_DEV_NAME, CMD_EFUSE_READ_BYTE, &efuse);
        if(ret == 0)
        {
            cal[i] = efuse.data;
        }
        else
        {
            cal[i] = 0;
            os_printf("efuse get cal -1\r\n");
            return 0;
        }
    }

    os_printf("efuse get cal:%02x:%02x:%02x:%02x\r\n",
              cal[0], cal[1], cal[2], cal[3]);

    if((cal[0] == 0) && (cal[1] == 0) && (cal[2] == 0) && (cal[3] == 0))
    {
        os_printf("efuse cal all zero, see as error\r\n");
        return 0;
    }

    return 1;
}

void usb_charge_stop()
{
    UINT32 charge_type = 1;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_CHARGE_STOP, &charge_type);
}

void usb_charge_start(CHARGE_STEP step, UINT32 charge_elect)
{
    CHARGE_TYPE type;
    UINT8 cali_result[4];

#if (CFG_CHARGE_MODE == CHARGE_INTERNAL_HW_MODE)
    type = INTERNAL_HW_MODE;
#elif (CFG_CHARGE_MODE == CHARGE_INTERNAL_SW_MODE)
    type = INTERNAL_SW_MODE;
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_HW_MODE)
    type = EXTERNAL_HW_MODE;
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_SW_MODE)
    type = EXTERNAL_SW_MODE;
#endif

    charge_cfg.step = step;
    if(step == STEP_STOP)
    {
        usb_charge_stop();
        return;
    }
    else if(step == STEP_START)
    {
        usb_charge_set_charge_default_reg();
    }

#if (CHARGE_CAL_POS == CHARGE_CAL_EFUSE)
    if(!wifi_get_charge_cal_from_efuse((UINT8 *)cali_result))
#elif (CHARGE_CAL_POS == CHARGE_CAL_ITEM)
    if(!get_info_item(CHARGE_CONFIG_ITEM, (UINT8 *)cali_result, NULL, NULL))
#endif
    {
        os_printf("load charge param from %d error.\r\n", CHARGE_CAL_POS);
    }
    else
    {
        os_printf("start: %x %x %x %x\r\n", cali_result[0], cali_result[1], cali_result[2], cali_result[3]);
    }
    charge_cfg.cal[0] = cali_result[0];
    charge_cfg.cal[1] = 0x15;           //not do cal,default 0x15
    charge_cfg.cal[2] = cali_result[1];

    charge_cfg.type = type;
#if (CFG_CHARGE_MODE == CHARGE_EXTERNAL_SW_MODE || CFG_CHARGE_MODE == CHARGE_INTERNAL_SW_MODE )
    charge_cfg.elect = charge_elect;
#endif
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_CHARGE_START, (void *)&charge_cfg);
}

static int vbat_voltage_get(void)
{
    int i;
    unsigned short *usData;
    unsigned long   ul_valueDout = 0;
    float ul_valueIn = 0;
    saradc_desc_t *p_ADC_drv_desc = NULL;
    DD_HANDLE saradc_handle;
    UINT32 status;

    usData = (UINT16 *)os_malloc(53 * sizeof(UINT16));
    if (usData == NULL)
    {
        os_printf("malloc usData failed!\r\n");
        return 0;
    }

    saradc_get_adcdata(ADC_CHANNEL0_VBAT, ADC_CONFIG_MODE_CONTINUE, 0x10, 0x20, 53, usData);

    for (i = 34; i < 50; i++)
    {
        ul_valueDout += usData[i];
    }
    ul_valueDout = ul_valueDout >> 4;

    /*Dout=Din-Codeoffset;Vin=Dout*4200/dvbatref*/
    ul_valueDout = (unsigned long)(ul_valueDout - CodeOffset);
    ul_valueIn = ul_valueDout * 4200 / dvbatref;

    os_printf("vbat %d\r\n", (int)ul_valueIn);
    os_free(usData);
    return ul_valueIn;
}


#if (CFG_CHARGE_MODE == CHARGE_INTERNAL_HW_MODE)
void usb_charge_internal_hw_check(void)
{
    int vol;
    CHARGE_STEP step;

    vol = vbat_voltage_get();
    step = charge_cfg.step;
    switch (step)
    {
    case STEP_START:
        if(charger_is_full())
        {
            os_printf("OK,charger_full\r\n");
#if CFG_FULL_ACTION_STOP
            step = STEP_STOP;
#else
            return;
#endif
        }
        break;

    case STEP_STOP:
        if (vol < VBAT_RESTART_ADC_THRESHOLD)
        {
            if(keep_count++ < 3)
            {
            }
            else
            {
                step = STEP_START;
            }
        }
        else
        {
            keep_count = 0;
        }
        break;
    default:
        break;
    }

    if(step != charge_cfg.step)
    {
        os_printf("cb:%d  \r\n", step);
        usb_charge_start(step, 0);
    }
    os_printf("cb1:%d \r\n", step);
}
#elif (CFG_CHARGE_MODE == CHARGE_INTERNAL_SW_MODE)
void usb_charge_internal_sw_check(void)
{
    int vol, elect = 0;
    CHARGE_STEP step;

    vol = vbat_voltage_get();
    step = charge_cfg.step;
    elect = charge_cfg.elect;
    switch (step)
    {
    case STEP_START:
        if(vol < VBAT_TRICKLE_ADC_THRESHOLD)
        {
            step = STEP_TRICKLE;
        }
        else if (vol >= VBAT_TRICKLE_ADC_THRESHOLD)
        {
            step = STEP_INTER_CC;
            elect = 250;
        }
        break;
    case STEP_TRICKLE:
        if (vol >= VBAT_TRICKLE_ADC_THRESHOLD)
        {
            if(keep_count++ < 5)
            {
            }
            else
            {
                step = STEP_INTER_CC;
                elect = 250;
            }
        }
        else
        {
            keep_count = 0;
        }
        break;
    case STEP_INTER_CC:
        if (vol >= VBAT_FULL_ADC_THRESHOLD)
        {
            if(vol < VBAT_SAFETY_ADC_THRESHOLD)
            {
                if(keep_count++ < 20)
                {
                }
                else
                {
                    elect -= 10;
                }
            }
            else
            {
                elect -= 50;
            }
        }
        else
        {
            keep_count = 0;
        }

        if(elect < 50)
        {
            step = STEP_INTER_CV;
        }
        break;
    case STEP_INTER_CV:
        if (vol >= VBAT_FULL_ADC_THRESHOLD)
        {
            if(keep_count++ < 300)
            {
            }
            else
            {
                os_printf("OK,charger_full\r\n");
#if CFG_FULL_ACTION_STOP
                step = STEP_STOP;
#else
                return;
#endif
            }
        }
        else
        {
            keep_count = 0;
        }

        if (vol < 4160)
        {
            step = STEP_INTER_CC;
            elect = 250;
            keep_count = 0;
        }

        break;

    case STEP_STOP:
        if (vol < VBAT_RESTART_ADC_THRESHOLD)
        {
            if(keep_count++ < 3)
            {
            }
            else
            {
                step = STEP_START;
            }
        }
        else
        {
            keep_count = 0;
        }

        break;
    default:
        break;
    }

    if(step != charge_cfg.step || elect != charge_cfg.elect)
    {
        keep_count = 0;
        os_printf("cb:%d %d \r\n", step, elect);
        usb_charge_start(step, elect);
    }
    os_printf("cb2:%d %d keep_count:%d\r\n", step, elect, keep_count);

}
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_HW_MODE)
void usb_charge_external_hw_check(void)
{
    int vol;
    CHARGE_STEP step;

    vol = vbat_voltage_get();

    step = charge_cfg.step;
    switch (step)
    {
    case STEP_START:
        if (vol >= VBAT_FULL_ADC_THRESHOLD)
        {
            if(keep_count++ < 30 && vol < VBAT_SAFETY_ADC_THRESHOLD)
            {
            }
            else
            {
                step = STEP_INTER_CV;
            }
        }
        else
        {
            keep_count = 0;
        }

        break;

    case STEP_INTER_CV:
        if(charger_is_full())
        {
            os_printf("OK,charger_full\r\n");
#if CFG_FULL_ACTION_STOP
            step = STEP_STOP;
#else
            return;
#endif
        }

        if (vol < 4160)
        {
            step = STEP_START;
            keep_count = 0;
        }

        break;

    case STEP_STOP:
        if (vol < VBAT_RESTART_ADC_THRESHOLD)
        {
            if(keep_count++ < 3)
            {
            }
            else
            {
                step = STEP_START;
            }
        }
        else
        {
            keep_count = 0;
        }
        break;
    default:
        break;
    }

    if(step != charge_cfg.step)
    {
        keep_count = 0;
        os_printf("cb:%d \r\n", step);
        usb_charge_start(step, 0);
    }
    os_printf("cb3:%d keep_count:%d\r\n", step, keep_count);

}
#elif (CFG_CHARGE_MODE == CHARGE_EXTERNAL_SW_MODE)
void usb_charge_external_sw_check(void)
{
    int vol, elect = 0;
    CHARGE_STEP step;

    vol = vbat_voltage_get();

    step = charge_cfg.step;
    elect = charge_cfg.elect;
    switch (step)
    {
    case STEP_START:
        if(vol < VBAT_TRICKLE_ADC_THRESHOLD)
        {
            step = STEP_TRICKLE;
        }
        else if (vol >= VBAT_FULL_ADC_THRESHOLD)
        {
            step = STEP_INTER_CC;
            elect = 250;
        }
        else
        {
            step = STEP_EXTER_CC;
            elect = 750;
        }
        break;

    case STEP_TRICKLE:
        if (vol >= VBAT_TRICKLE_ADC_THRESHOLD)
        {
            if(keep_count++ < 5)
            {
            }
            else
            {
                step = STEP_EXTER_CC;
                elect = 750;
            }
        }
        else
        {
            keep_count = 0;
        }
        break;

    case STEP_EXTER_CC:
        if (vol >= 4250)
        {
            if(vol < 4290)
            {
                if(keep_count++ < 10)
                {
                }
                else
                {
                    elect -= 20;
                }
            }
            else
            {
                elect -= 100;
            }
        }
        else
        {
            keep_count = 0;
        }

        if(elect < 450)
        {
            step = STEP_INTER_CC;
            elect = 250;
        }
        break;

    case STEP_INTER_CC:
        if (vol >= 4200)
        {
            if(vol < 4240)
            {
                if(keep_count++ < 20)
                {
                }
                else
                {
                    elect -= 10;
                }
            }
            else
            {
                elect -= 50;
            }
        }
        else
        {
            keep_count = 0;
        }

        if(elect < 50)
        {
            step = STEP_INTER_CV;
        }
        break;

    case STEP_INTER_CV:
        if (vol >= VBAT_FULL_ADC_THRESHOLD)
        {
            if(keep_count++ < 300)
            {
            }
            else
            {
                os_printf("OK,charger_full\r\n");
#if CFG_FULL_ACTION_STOP
                step = STEP_STOP;
#else
                return;
#endif
            }
        }
        else
        {
            keep_count = 0;
        }

        if (vol < VBAT_FULL_ADC_THRESHOLD)
        {
            step = STEP_INTER_CC;
            elect = 250;
            keep_count = 0;
        }

        break;

    case STEP_STOP:
        if (vol < VBAT_RESTART_ADC_THRESHOLD)
        {
            if(keep_count++ < 3)
            {
            }
            else
            {
                step = STEP_START;
            }
        }
        else
        {
            keep_count = 0;
        }
        break;

    default:
        break;

    }

    if(step != charge_cfg.step || elect != charge_cfg.elect)
    {
        keep_count = 0;
        os_printf("cb:%d %d \r\n", step, elect);
        usb_charge_start(step, elect);
    }
    os_printf("cb4:%d %d keep_count:%d\r\n", step, elect, keep_count);
}
#endif

void usb_charge_check_cb(void)
{
    UINT32 tmp;

    if(charge_func_init && usb_power_is_pluged())
    {
        tmp = fclk_get_second();
        if(tmp > last_second)
        {
            USB_CHARGE_CB();
            last_second = tmp;
        }
    }

}

int usb_charge_set_cal_default_reg(void)
{
    unsigned long reg;
    //cal def reg
    reg = CHARGE_ANALOG_CTRL3_CAL_DEFAULT_VALUE;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG3, &reg);
    reg = CHARGE_ANALOG_CTRL4_CAL_DEFAULT_VALUE;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG4, &reg);
}

int usb_charge_set_charge_default_reg(void)
{
    unsigned long reg;
    //charge def reg
    reg = CHARGE_ANALOG_CTRL3_CHARGE_DEFAULT_VALUE;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG3, &reg);
    reg = CHARGE_ANALOG_CTRL4_CHARGE_DEFAULT_VALUE;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG4, &reg);
}

int vbat_adc_cal_step1(void)
{
    // sar ADC cal 1,add usb 5v
    unsigned short usData[53];
    unsigned long  ul_valuex = 0, ul_valuey = 0, dvbatref = 0, reg, i;

    usb_charge_set_cal_default_reg();

    // 1
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG2, &reg);
    reg &= ~(1 << 22);
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG2, &reg);

    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG7, &reg);
    reg &= ~(1 << 30);
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG7, &reg);

    // 2
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG7, &reg);
    reg &= ~(1 << 20);
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG7, &reg);

    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG7, &reg);
    reg |= (1 << 21);
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG7, &reg);

    delay(1);

    saradc_get_adcdata(ADC_CHANNEL0_VBAT, ADC_CONFIG_MODE_CONTINUE, 0x10, 0x20, 53, usData);

    for (i = 34; i < 50; i++)
    {
        ul_valuex += usData[i];
    }
    ul_valuex = ul_valuex >> 4;
    CodeOffset = ul_valuex - 2048; //-4

    os_printf("CodeOffset: %x\r\n", CodeOffset);

    usb_charge_set_charge_default_reg();

    return CodeOffset;
}

int vbat_adc_cal_step2(void)
{
    // sar ADC cal 2,add usb 5v,add vbat 4.2V
    int i;
    unsigned short usData[53];
    unsigned long  ul_valuey = 0, reg;


    // 3
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG7, &reg);
    reg &= ~(1 << 21);
    reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG7, &reg);

    saradc_get_adcdata(ADC_CHANNEL0_VBAT, ADC_CONFIG_MODE_CONTINUE, 0x10, 0x20, 53, usData);
    for (i = 34; i < 50; i++)
    {
        ul_valuey += usData[i];
    }
    ul_valuey = ul_valuey >> 4;
    dvbatref = (unsigned long)(ul_valuey - CodeOffset);//0xE1F

    os_printf("dvbatref: %x\r\n", dvbatref);
    return dvbatref;
}

void usb_charger_calibration(void)
{
    CHARGE_OPER_ST chrg;
    UINT32 reg, i, ret;
    UINT8 cali_result[4];

    usb_charge_set_cal_default_reg();

    vbat_adc_cal_step2();

    chrg.step = 1;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_CHARGE_CAL, &chrg);
    chrg.step = 3;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_CHARGE_CAL, &chrg);
    chrg.step = 4;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_CHARGE_CAL, &chrg);

    cali_result[0] = chrg.cal[0];
    cali_result[1] = chrg.cal[2];
    cali_result[2] = (CodeOffset - 0xFFFFFF80) & 0xFF;
    cali_result[3] = (dvbatref - 0xDA0) & 0xFF;
    os_printf("save charge cal %x %x %x %x \r\n", cali_result[0], cali_result[1], cali_result[2], cali_result[3]);

    save_info_item(CHARGE_CONFIG_ITEM, cali_result, NULL, NULL);

    usb_charge_set_charge_default_reg();
}
#endif
void usb_plug_func_handler(void *usr_data, UINT32 event)
{
    switch(event)
    {
    case USB_PLUG_IN_EVENT:
#if CFG_USE_USB_CHARGE
        if(charge_func_init && usb_power_is_pluged())
        {
            usb_charge_start(STEP_START, 0);
        }
#endif
        break;

    case USB_PLUG_OUT_EVENT:
#if CFG_USE_USB_CHARGE
        if(charge_func_init)
        {
            usb_charge_stop();
        }
#endif
        break;

    case USB_PLUG_NO_EVENT:

        break;

    default:
        break;
    }

}


void usb_plug_func_open(void)
{
    DD_HANDLE usb_plug_hdl;
    UINT32 status;
    USB_PLUG_INOUT_ST user_plug;
#if CFG_USE_USB_CHARGE
    UINT8 cali_result[4] = {0};
#endif

    user_plug.handler = usb_plug_func_handler;
    user_plug.usr_data = 0;

    usb_plug_hdl = ddev_open(USB_PLUG_DEV_NAME, &status, (UINT32)&user_plug);
    if(DD_HANDLE_UNVALID == usb_plug_hdl)
    {
        return;
    }

#if CFG_USE_USB_CHARGE
    charge_func_init = 1;

#if (CHARGE_CAL_POS == CHARGE_CAL_EFUSE)
    if(!wifi_get_charge_cal_from_efuse((UINT8 *)cali_result))
#elif (CHARGE_CAL_POS == CHARGE_CAL_ITEM)
    if(!get_info_item(CHARGE_CONFIG_ITEM, (UINT8 *)cali_result, NULL, NULL))
#endif
    {
        os_printf("load charge param from %d error.\r\n", CHARGE_CAL_POS);
    }
    else
    {
        os_printf("load charge cal value %x %x %x %x\r\n", cali_result[0], cali_result[1], cali_result[2], cali_result[3]);
    }

    CodeOffset = (signed long)(cali_result[2]) + 0xFFFFFF80;
    dvbatref = (signed long)(cali_result[3]) + 0xDA0;
    os_printf("adc_cal value CodeOffset:%x dvbatref:%x\r\n", CodeOffset, dvbatref);

#endif
}


#endif

void rt_user_idle_hook(void)
{
#if CFG_USE_USB_CHARGE
#if (CFG_SOC_NAME == SOC_BK7221U)
    usb_charge_check_cb();
#endif
#endif
}


