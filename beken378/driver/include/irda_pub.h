#ifndef _IRDA_PUB_H_
#define _IRDA_PUB_H_

#define IRDA_FAILURE                (1)
#define IRDA_SUCCESS                (0)

#define IRDA_DEV_NAME                "irda"

#define IRDA_CMD_MAGIC              (0xe290000)
enum
{
    IRDA_CMD_ACTIVE = IRDA_CMD_MAGIC + 1,
    IRDA_CMD_SET_POLARITY,
    IRDA_CMD_SET_CLK,
	IRDA_CMD_SET_INT_MASK,
	TRNG_CMD_GET,
};

enum
{
	IR_KEY_TYPE_SHORT = 0,
	IR_KEY_TYPE_LONG,
	IR_KEY_TYPE_HOLD,
	IR_KEY_TYPE_MAX,
};

#define USERCODE_MASK 	0xffff

#define KEY_CODE_MASK	0xff0000
#define KEY_CODE_SHIFT  16

#define KEY_CODE_INVERS_MASK  0xff000000
#define KEY_CODE_INVERS_SHIFT 24

#define KEY_SHORT_CNT         3
#define KEY_LONG_CNT          8
#define KEY_HOLD_CNT          11


#define GENERATE_KEY(type,value) (((type) << 24) | (value))
#define GET_KEY_TYPE(msg) (((msg) >> 24) & 0xff)
#define GET_KEY_VALUE(msg) ((msg) & 0xff)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void irda_init(void);
void irda_exit(void);
void irda_isr(void);
void Irda_init_app(void);
void set_irda_usrcode(UINT16 ir_usercode);
long IR_get_key(void *buffer, unsigned long  size, INT32 timeout);
#endif //_IRDA_PUB_H_

