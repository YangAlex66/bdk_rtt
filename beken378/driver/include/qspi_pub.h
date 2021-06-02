#ifndef _QSPI_PUB_H_
#define _QSPI_PUB_H_

#include "uart_pub.h"

#define BK_QSPI_DEBUG                0
#if BK_QSPI_DEBUG
#define BK_QSPI_PRT               os_printf
#define BK_QSPI_WPRT              warning_prf
#define BK_QSPI_FATAL             fatal_prf
#else
#define BK_QSPI_PRT               null_prf
#define BK_QSPI_WPRT              null_prf
#define BK_QSPIFATAL	          null_prf
#endif

#define QSPI_FAILURE                (1)
#define QSPI_SUCCESS                (0)

#define QSPI_DEV_NAME                "qspi"
#define QSPI_DCACHE_BASE			(0x03000000)
#define QSPI_CMD_MAGIC              (0xa250000)
#define QSPI_DEBUG                   0

#if QSPI_DEBUG
#define QSPI_WPRT                os_printf
#define QSPI_EPRT                os_printf
#define QSPI_DEBUG_PRINTF		 os_printf
#else
#define QSPI_WPRT                os_printf
#define QSPI_EPRT                os_null_printf
#define QSPI_DEBUG_PRINTF		 os_null_printf
#endif

enum
{
    QSPI_CMD_SET_VOLTAGE = QSPI_CMD_MAGIC + 1,
    QSPI_CMD_DCACHE_CONFIG,
    QSPI_CMD_GPIO_CONFIG, 
    QSPI_CMD_DIV_CLK_SET,
    QSPI_CMD_CLK_SET_26M,
    QSPI_CMD_CLK_SET_DCO,
    QSPI_CMD_CLK_SET_120M,
    QSPI_DCACHE_CMD_OPEN,
    QSPI_DCACHE_CMD_CLOSE,
};

//----------------------------------------------
// QSPI GE0 driver description
//----------------------------------------------
typedef struct
{
/* mode:     QSPI mode
 * bit[0:1]: QSPI line mode
 *          00:  QSPI line mode 1
 *          01:  QSPI line mode 2
 *          10:  reserved
 *          11:  QSPI line mode 4
 * bit[2]:   QSPI dir
 *          0: write to psram
 *          1: read from psram
 * bit[3]:   QSPI dma enable
 *          0: dma disable
 *          1: dma enable
 * bit[4]:   QSPI ge0/ge1
 *          0: ge0 is working
 *          1: ge1 is working
 * bit[7:5]: reserved
 */
    unsigned char           mode;                       // QSPI mode

/* clk_sel:  QSPI clk set
 * bit[0:2]: QSPI clk devide
 * bit[3]:   reserved
 * bit[5:4]:   QSPI clk select
 *          00: DCO clock
 *          01: 26MHz XTAL clock
 *          10: 120MHz DPLL clock
 *          11: reserved
 * bit[7:6]: reserved
 */
    unsigned char           clk_set;                    // QSPI clk set
    unsigned char           command;
    unsigned char           dummy_size;
    unsigned long           data_buff_psram_addr;       // data buffer psram address, bit31: data_buff_psram_addr enable
    unsigned long           *pdata_buff_ram_addr;       // data buffer ram address
    unsigned short          data_buff_size;             // data buffer size, the unit are in word
    volatile unsigned short data_buff_offset;           // data buffer offset, the unit are in word
    void                    (*p_Int_Handler)(void);     // QSPI Interrupt Handler
} QSPI_GE0_DRV_DESC;

//----------------------------------------------
// QSPI DCACHE driver description
//----------------------------------------------
typedef struct
{
/* mode:     QSPI mode
 * bit[0:1]: QSPI line mode
 *          00:  QSPI line mode 1
 *          01:  QSPI line mode 2
 *          10:  reserved
 *          11:  QSPI line mode 4
 * bit[2]:   reserved
 * bit[3]:   QSPI dma enable
 *          0: dma disable
 *          1: dma enable
 * bit[4]:   reserved
 * bit[7:5]: reserved
 */
    unsigned char           mode;                       // QSPI mode

/* clk_sel:  QSPI clk set
 * bit[0:2]: QSPI clk devide
 * bit[3]:   reserved
 * bit[5:4]:   QSPI clk select
 *          00: DCO clock
 *          01: 26MHz XTAL clock
 *          10: 120MHz DPLL clock
 *          11: reserved
 * bit[7:6]: reserved
 */
    unsigned char           clk_set;                    // QSPI clk set
    unsigned char           wr_command;
    unsigned char           rd_command;
    unsigned char           wr_dummy_size;
    unsigned char           rd_dummy_size;
    unsigned char           voltage_level;
} qspi_dcache_drv_desc;

void qspi_init(void);
void qspi_exit(void);

void qspi_isr(void);
UINT32 qspi_ctrl(UINT32 cmd, void *param);

/*
 *	mode: 1: 1 linemode
 *		  2: 4 linemode
 *  div：qspi clk freq div
 *
 */
extern void bk_qspi_mode_start(UINT32 mode, UINT32 div);
extern void bk_qspi_psram_quad_mode_switch(unsigned char ucEnterOrExit);
extern void bk_qspi_psram_reset_enable(void);
extern void bk_qspi_psram_reset(void);
extern void bk_qspi_psram_set_length(void);
extern UINT16 bk_qspi_psram_read_id(void);
extern void psram_init(uint8_t line_mode,uint8_t voltage_level);
extern int bk_qspi_dcache_configure(qspi_dcache_drv_desc *qspi_cfg);
extern int bk_qspi_dcache_write_data(UINT32 set_addr, UINT32 *wr_data, UINT32 data_length);
extern int bk_qspi_dcache_read_data(UINT32 set_addr, UINT32 *rd_data, UINT32 data_length);
#endif //_QSPI_PUB_H_

