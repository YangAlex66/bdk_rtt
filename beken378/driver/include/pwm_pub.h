#ifndef _PWM_PUB_H_
#define _PWM_PUB_H_

#define PWM_FAILURE                (1)
#define PWM_SUCCESS                (0)

#define PWM_DEV_NAME                "pwm"

#define PWM_CMD_MAGIC              (0xe230000)
enum
{
    CMD_PWM_UNIT_ENABLE = PWM_CMD_MAGIC + 1,
    CMD_PWM_UNIT_DISABLE,
    CMD_PWM_IR_ENABLE,
    CMD_PWM_IR_DISABLE,
    CMD_PWM_IR_CLEAR,
    CMD_PWM_INIT_PARAM,
    CMD_PWM_SET_DUTY_CYCLE,
    CMD_PWM_SET_END_VALUE,
    CMD_PWM_CAP_GET,
    CMD_PWM_DEINIT_PARAM,
    CMD_PWM_GROUP_ENABLE,
    CMD_PWM_GROUP_DISABLE,
    CMD_PWM_UPDATE_PARAM,
    CMD_PWM_UPDATE_PARAM_ENABLE,
    CMD_PWM_INIT_LEVL_SET_LOW,
    CMD_PWM_INIT_LEVL_SET_HIGH
};

enum
{
    PWM0     = 0,
    PWM1,
    PWM2,
    PWM3,
    PWM4,
    PWM5,
    PWM_COUNT
};

typedef void (*PFUNC)(UINT8);


#define PWM_ENABLE           (0x01)
#define PWM_DISABLE          (0x00)

#define PWM_INT_EN               (0x01)
#define PWM_INT_DIS              (0x00)

#if (CFG_SOC_NAME == SOC_BK7231N)
#define PWM_IDLE_MODE               (0x00)
#define PWM_PWM_MODE                (0x01)
#define PWM_TIMER_MODE              (0x02)
#define PWM_CPUNTER_MODE            (0x03)
#define PWM_CAP_POS_MODE            (0x04)
#define PWM_CAP_NEG_MODE            (0x05)
#define PWM_CAP_EDGE_MODE           (0x06)
#else
#define PWM_PWM_MODE                (0x00)
#define PWM_TIMER_MODE              (0x01)
#define PWM_CAP_POS_MODE            (0x02)
#define PWM_CAP_NEG_MODE            (0x03)
#endif

#define PWM_CLK_32K                 (0x00)
#define PWM_CLK_26M                 (0x01)
#define PWM_CLK_DCO					(0x02)

typedef struct
{
    UINT8 channel;

    union
    {
        UINT8 val;
        struct
        {
#if (CFG_SOC_NAME == SOC_BK7231N)
            /* cfg--PWM config
               * bit[2:0]: PWM mode selection
               *           000:IDLE
               *           001: PWM mode
               *           010: TIMER
               *           100: Capture (pos -> pos)
               *           101: Capture (neg -> neg)
               *           110: Capture (edge -> edge)
               * bit[3]:   PWM enable
               *           0: PWM disable
               *           1: PWM enable
               * bit[4]:   PWM interrupt enable
               *           0: PWM interrupt disable
               *           1: PWM interrupt enable
               * bit[6:5]: PWM clock select
               *           00: PWM clock LPO
               *           01: PWM clock 26MHz
               *           10/11: PWM clock DCO
               */
            UINT8 mode: 3;
            UINT8 en: 1;
            UINT8 int_en: 1;
            UINT8 clk: 2;
            UINT8 rsv: 1;
#else
           /* cfg--PWM config
              * bit[0]:   PWM enable
              *          0:  PWM disable
              *          1:  PWM enable
              * bit[1]:   PWM interrupt enable
              *          0:  PWM interrupt disable
              *          1:  PWM interrupt enable
              * bit[3:2]: PWM mode selection
              *          00: PWM mode
              *          01: TIMER
              *          10: Capture Posedge
              *          11: Capture Negedge
              * bit[5:4]: PWM clock select
              *          00: PWM clock 32KHz
              *          01: PWM clock 26MHz
              *          10/11: PWM clock DPLL
              * bit[7:6]: reserved
              */
            UINT8 en: 1;
            UINT8 int_en: 1;
            UINT8 mode: 2;
            UINT8 clk: 2;
            UINT8 rsv: 2;
#endif
        } bits;
    } cfg;

#if (CFG_SOC_NAME == SOC_BK7231)
    UINT16 end_value;
    UINT16 duty_cycle;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    UINT32 end_value;
    UINT32 duty_cycle1;
    UINT32 duty_cycle2;
    UINT32 duty_cycle3;
#else
	UINT32 end_value;
	UINT32 duty_cycle;
#endif
    PFUNC p_Int_Handler;
} pwm_param_t;

typedef struct
{
    UINT32 ucChannel;
    UINT32 value;
} pwm_capture_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void pwm_init(void);
extern void pwm_exit(void);
extern void pwm_isr(void);

#endif //_PWM_PUB_H_
