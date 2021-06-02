
#include "error.h"
#include "include.h"

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include <rtdef.h>
#define SARADC_VBAT_DETECT
#ifdef SARADC_VBAT_DETECT
#include "saradc_intf.h"
#include "sys_ctrl_pub.h"

#define ADC_CHANNEL0_VBAT	0
#define ADC_CHANNEL1_P4 	1
#define ADC_CHANNEL2_P5 	2
#define ADC_CHANNEL3_P23 	3
#define ADC_CHANNEL4_P2		4
#define ADC_CHANNEL5_P3 	5
#define ADC_CHANNEL6_P12 	6
#define ADC_CHANNEL7_P13 	7

#define VBAT_COUNT_NUMBER                   (500)
typedef struct _vbat_detect_ {
int tmp_vol;
int mean_vol;
int detect_cnt;
} VBAT_DETECT_ST;
static ADC_OBJ vbat;
static VBAT_DETECT_ST vbat_det_st;
extern int Step_Flag;
extern int	Adctest_Flag ;
//channe vbat

static void vbat_adc_detect_callback(int new_mv, void *user_data)
{
    VBAT_DETECT_ST *vbat = (VBAT_DETECT_ST *)user_data;
	static int cnt = 0;

	if(vbat_det_st.detect_cnt++ <= VBAT_COUNT_NUMBER)
	{
		return ;
	}
	new_mv = new_mv << 1;//for vbat channel,need to *2

	vbat_det_st.tmp_vol += new_mv;
	
	if(cnt ++ >=100)
	{
		vbat->mean_vol = vbat_det_st.tmp_vol/cnt;
		cnt = 0;
		vbat_det_st.detect_cnt = 0;
		rt_kprintf("---vbat voltage:%d---\r\n",vbat->mean_vol);
		vbat_det_st.tmp_vol = 0;
	}
	
	
}

static void vbat_detect_config(void)
{
    vbat_det_st.tmp_vol = 0;
    vbat_det_st.mean_vol = 0;
	vbat_det_st.detect_cnt = 0;

    adc_obj_init(&vbat, vbat_adc_detect_callback,ADC_CHANNEL0_VBAT, &vbat_det_st);
    adc_obj_start(&vbat);
}

int vbat_detect_test(void)
{
	Step_Flag = 1;
	Adctest_Flag  = 1;
	saradc_work_create();
	vbat_detect_config();
	return 0;
}
//INIT_APP_EXPORT(vbat_detect_test);
MSH_CMD_EXPORT(vbat_detect_test,vbat_detect_test);
#endif