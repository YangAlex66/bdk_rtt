#include "include.h"
#include "arm_arch.h"

#if(CFG_SOC_NAME == SOC_BK7271)
#include "spi_bk7271.h"
#include "spi_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

#define SPI_PERI_CLK_26M		(26 * 1000 * 1000)
#define SPI_PERI_CLK_DCO		(180 * 1000 * 1000)

UINT8 spi_channel = 0;

static SDD_OPERATIONS spi_op = {
	spi_ctrl
};

void spi_channel_set(UINT8 channel )
{
	spi_channel = channel;
}

static void spi_active(BOOLEAN val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~SPIEN;
	else if (val == 1)
		value |= SPIEN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_set_msten(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~MSTEN;
	else if (val == 1)
		value |= MSTEN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_set_ckpha(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~CKPHA;
	else if (val == 1)
		value |= CKPHA;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_set_skpol(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~CKPOL;
	else if (val == 1)
		value |= CKPOL;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_set_bit_wdth(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~BIT_WDTH;
	else if (val == 1)
		value |= BIT_WDTH;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_set_nssmd(UINT8 val)
{
	UINT32 value;
	value = REG_READ(SPI_CTRL(spi_channel));
	value &= ~CTRL_NSSMD_3;
	value |= (val << 17);
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

/*
    spi_clk : 90M/3=30M    - DC0180 - DIV2
    spi_clk : 90M/4=22.5M  - DC0180 - DIV3
    spi_clk : 90M/5=18M    - DC0180 - DIV4
    spi_clk : 90M/6=15M    - DC0180 - DIV5
    spi_clk : 90M/7=12.85M - DC0180 - DIV6
    spi_clk : 90M/8=11.25M - DC0180 - DIV7
    spi_clk : 90M/9=10M    - DC0180 - DIV8
    spi_clk : 90M/10=9M    - DC0180 - DIV9
    spi_clk : 90M/11=8.18M - DC0180 - DIV10
*/
static void spi_set_clock(UINT32 max_hz)
{
	int source_clk = 0;
	int spi_clk = 0;
	int div = 0;
	UINT32 param;
	os_printf("max_hz :%d\r\n", max_hz);

	if (spi_channel == 0)
		param = PCLK_POSI_SPI1;
	else if (spi_channel == 1)
		param = PCLK_POSI_SPI2;
	else if (spi_channel == 2)
		param = PCLK_POSI_SPI3;

	if ((max_hz == 26000000) || (max_hz == 13000000) || (max_hz == 6500000)) {
		BK_SPI_PRT("config spi clk source 26MHz\n");

		spi_clk = max_hz;
#if CFG_XTAL_FREQUENCE
		source_clk = CFG_XTAL_FREQUENCE;
#else
		source_clk = SPI_PERI_CLK_26M;
#endif

		sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
	} else if (max_hz > 4333000) {
		BK_SPI_PRT("config spi clk source DCO\n");

		if (max_hz > 30000000) { // 180M/2 / (2 + 1) = 30M
			spi_clk = 30000000;
			BK_SPI_PRT("input clk > 30MHz, set input clk = 30MHz\n");
		} else
			spi_clk = max_hz;

		source_clk = SPI_PERI_CLK_DCO;

		sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, &param);
	} else {
		BK_SPI_PRT("config spi clk source 26MHz\n");

		spi_clk = max_hz;
#if CFG_XTAL_FREQUENCE
		source_clk = CFG_XTAL_FREQUENCE;
#else
		source_clk = SPI_PERI_CLK_26M;
#endif

		sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
	}
	if ((max_hz == 26000000) || (max_hz == 13000000) || (max_hz == 6500000))
		div = source_clk / spi_clk - 1;
	else {
		// spi_clk = in_clk / (2 * (div + 1))
		div = ((source_clk >> 1) / spi_clk) - 1;

		if (div < 2)
			div = 2;
		else if (div >= 255)
			div = 255;
	}

	param = REG_READ(SPI_CTRL(spi_channel));
	param &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
	param |= (div << SPI_CKR_POSI);
	REG_WRITE(SPI_CTRL(spi_channel), param);

	BK_SPI_PRT("div = %d \n", div);
	BK_SPI_PRT("spi_clk = %d \n", spi_clk);
	BK_SPI_PRT("source_clk = %d \n", source_clk);
	BK_SPI_PRT("target frequency = %d, actual frequency = %d \n", max_hz, source_clk / 2 / (div + 1));
}



static void spi_rxint_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~RXINT_EN;
	else if (val == 1)
		value |= RXINT_EN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_txint_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~TXINT_EN;
	else if (val == 1)
		value |= TXINT_EN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_rxovr_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~RXOVR_EN;
	else if (val == 1)
		value |= RXOVR_EN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_txovr_enable(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~TXOVR_EN;
	else if (val == 1)
		value |= TXOVR_EN;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_rxint_mode(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));

	value &= ~(RXINT_MODE_MASK << RXINT_MODE_POSI);
	value |= ((val & RXINT_MODE_MASK) << RXINT_MODE_POSI);

	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_txint_mode(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));

	value &= ~(TXINT_MODE_MASK << TXINT_MODE_POSI);
	value |= ((val & TXINT_MODE_MASK) << TXINT_MODE_POSI);

	REG_WRITE(SPI_CTRL(spi_channel), value);
}

static void spi_slave_set_cs_finish_interrupt(UINT32 enable)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (enable)
		value |= SPI_S_CS_UP_INT_EN;
	else
		value &= ~(SPI_S_CS_UP_INT_EN);
	REG_WRITE(SPI_CTRL(spi_channel), value);

	// don't clean cs finish status
	value =  REG_READ(SPI_STAT(spi_channel));
	value &= ~(SPI_S_CS_UP_INT_STATUS);
	REG_WRITE(SPI_STAT(spi_channel), value);
}

static void spi_gpio_configuration(void)
{
	UINT32 val;
	if (spi_channel == 0)
		val = GFUNC_MODE_SPI1;
	else if (spi_channel == 1)
		val = GFUNC_MODE_SPI2;
	else if (spi_channel == 2)
		val = GFUNC_MODE_SPI3_1;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);
}

static void spi_icu_configuration(UINT32 enable)
{
	UINT32 param, val;

	if (spi_channel == 0) {
		param = PWD_SPI1_CLK_BIT;
		val = IRQ_SPI1_BIT;
	} else if (spi_channel == 1) {
		param = PWD_SPI2_CLK_BIT;
		val = IRQ_SPI2_BIT;
	} else if (spi_channel == 2) {
		param = PWD_SPI3_CLK_BIT;
		val = IRQ_SPI3_BIT;
	}

	if (enable) {
		sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &val);
	} else {
		sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &val);
	}
}


static void spi_lsb_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CTRL(spi_channel));
	if (val == 0)
		value &= ~LSB_FIRST;
	else if (val == 1)
		value |= LSB_FIRST;
	REG_WRITE(SPI_CTRL(spi_channel), value);
}


static void spi_tx_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));
	if (val == 0)
		value &= ~SPI_TX_EN;
	else if (val == 1)
		value |= SPI_TX_EN;
	REG_WRITE(SPI_CONFIG(spi_channel), value);
}

static void spi_rx_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));
	if (val == 0)
		value &= ~SPI_RX_EN;
	else if (val == 1)
		value |= SPI_RX_EN;
	REG_WRITE(SPI_CONFIG(spi_channel), value);
}

static void spi_txfinish_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));
	if (val == 0)
		value &= ~SPI_TX_FINISH_EN;
	else if (val == 1)
		value |= SPI_TX_FINISH_EN;
	REG_WRITE(SPI_CONFIG(spi_channel), value);

}

static void spi_rxfinish_enbale(UINT8 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));
	if (val == 0)
		value &= ~SPI_RX_FINISH_EN;
	else if (val == 1)
		value |= SPI_RX_FINISH_EN;
	REG_WRITE(SPI_CONFIG(spi_channel), value);

}

static void set_txtrans_len(UINT32 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));

	value &= ~(0xFFF << SPI_TX_TRAHS_LEN_POSI);
	value |= ((val & 0xFFF) << SPI_TX_TRAHS_LEN_POSI);

	REG_WRITE(SPI_CONFIG(spi_channel), value);
}

__maybe_unused static void set_rxtrans_len(UINT32 val);
static void set_rxtrans_len(UINT32 val)
{
	UINT32 value;

	value = REG_READ(SPI_CONFIG(spi_channel));

	value &= ~(0xFFF << SPI_RX_TRAHS_LEN_POSI);
	value |= ((val & 0xFFF) << SPI_RX_TRAHS_LEN_POSI);

	REG_WRITE(SPI_CONFIG(spi_channel), value);
}

static void spi_init_msten(UINT8 param)
{
	UINT32 value = 0;
	UINT8 msten = (param & 0x0F);

	value = REG_READ(SPI_CTRL(spi_channel));
	value &= ~((TXINT_MODE_MASK << TXINT_MODE_POSI) | (RXINT_MODE_MASK << RXINT_MODE_POSI));

	value |= RXOVR_EN
			 | TXOVR_EN;
	//| (0x3UL << RXINT_MODE_POSI)   // fifo_level :32
	//| (0x3UL << TXINT_MODE_POSI);	//  fifo_level :32

	REG_WRITE(SPI_CTRL(spi_channel), value);
	if (msten == 0)
		spi_slave_set_cs_finish_interrupt(1);
	else
		spi_slave_set_cs_finish_interrupt(0);

	spi_icu_configuration(1);
	spi_gpio_configuration();
}

static void spi_deinit_msten(void)
{
	UINT32 status;

	spi_icu_configuration(0);

	REG_WRITE(SPI_CTRL(spi_channel), 0);

	status = REG_READ(SPI_STAT(spi_channel));
	REG_WRITE(SPI_STAT(spi_channel), status);
}

static void spi_rxfifo_clr(void)
{
	UINT32 value;

	value = REG_READ(SPI_STAT(spi_channel));

	while (value & RXFIFO_RD_READ) {
		REG_READ(SPI_DAT(spi_channel));
		value = REG_READ(SPI_STAT(spi_channel));
	}
}

UINT32 spi_read_rxfifo(UINT8 *data)
{
	UINT32 value;

	value = REG_READ(SPI_STAT(spi_channel));

	if (value & RXFIFO_RD_READ)
	{
		//REG_WRITE((0x00802800 + (0x1c * 4)), 0x02);
		//REG_WRITE((0x00802800 + (0x1c * 4)), 0x00);

		value = REG_READ(SPI_DAT(spi_channel));
		if (data)
			*data = value;
		return 1;
	}

	return 0;
}


UINT32 spi_write_txfifo(UINT8 data)
{
	UINT32 value;

	value = REG_READ(SPI_STAT(spi_channel));

	if (value & TXFIFO_WR_READ) {
		REG_WRITE(SPI_DAT(spi_channel), data);
		return 1;
	}

	return 0;
}

static struct spi_callback_des spi_receive_callback = {NULL, NULL};
static struct spi_callback_des spi_txfifo_needwr_callback = {NULL, NULL};
static struct spi_callback_des spi_tx_end_callback = {NULL, NULL};

static void spi_rx_callback_set(spi_callback callback, void *param)
{
	spi_receive_callback.callback = callback;
	spi_receive_callback.param = param;
}

static void spi_tx_fifo_needwr_callback_set(spi_callback callback, void *param)
{
	spi_txfifo_needwr_callback.callback = callback;
	spi_txfifo_needwr_callback.param = param;
}

static void spi_tx_end_callback_set(spi_callback callback, void *param)
{
	spi_tx_end_callback.callback = callback;
	spi_tx_end_callback.param = param;
}

void spi_init(void)
{
	intc_service_register(IRQ_SPI1, PRI_IRQ_SPI, spi_isr);
	sddev_register_dev(SPI_DEV_NAME, &spi_op);
}

void spi2_init(void)
{
	intc_service_register(IRQ_SPI2, PRI_IRQ_SPI, spi_isr);
	sddev_register_dev(SPI2_DEV_NAME, &spi_op);
}

void spi3_init(void)
{
	intc_service_register(IRQ_SPI3, PRI_IRQ_SPI, spi_isr);
	sddev_register_dev(SPI3_DEV_NAME, &spi_op);
}

void spi_exit(void)
{
	sddev_unregister_dev(SPI_DEV_NAME);
}

void spi2_exit(void)
{
	sddev_unregister_dev(SPI2_DEV_NAME);
}

void spi3_exit(void)
{
	sddev_unregister_dev(SPI3_DEV_NAME);
}

UINT32 spi_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = SPI_SUCCESS;

	peri_busy_count_add();

	switch (cmd) {
	case CMD_SPI_UNIT_ENABLE:
		spi_active(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_MSTEN:
		spi_set_msten(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_CKPHA:
		spi_set_ckpha(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_CKPOL:
		spi_set_skpol(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_BITWIDTH:
		spi_set_bit_wdth(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_NSSMD:
		spi_set_nssmd(*(UINT8 *)param);
		break;
	case CMD_SPI_SET_CKR:
		spi_set_clock(*(UINT32 *)param);
		break;
	case CMD_SPI_RXINT_EN:
		spi_rxint_enable(*(UINT8 *)param);
		break;
	case CMD_SPI_TXINT_EN:
		spi_txint_enable(*(UINT8 *)param);
		break;
	case CMD_SPI_RXOVR_EN:
		spi_rxovr_enable(*(UINT8 *)param);
		break;
	case CMD_SPI_TXOVR_EN:
		spi_txovr_enable(*(UINT8 *)param);
		break;
	case CMD_SPI_RXFIFO_CLR:
		spi_rxfifo_clr();
		break;
	case CMD_SPI_RXINT_MODE:
		spi_rxint_mode(*(UINT8 *)param);
		break;
	case CMD_SPI_TXINT_MODE:
		spi_txint_mode(*(UINT8 *)param);
		break;
	case CMD_SPI_INIT_MSTEN:
		spi_init_msten(*(UINT8 *)param);
		break;
	case CMD_SPI_GET_BUSY:
		break;
	case CMD_SPI_SET_RX_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi_rx_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI_SET_TX_NEED_WRITE_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi_tx_fifo_needwr_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI_SET_TX_FINISH_CALLBACK: {
		struct spi_callback_des *callback = (struct spi_callback_des *)param;
		spi_tx_end_callback_set(callback->callback, callback->param);
	}
	break;
	case CMD_SPI_DEINIT_MSTEN:
		spi_deinit_msten();
		break;
	case CMD_SPI_LSB_EN:
		spi_lsb_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI_TX_EN:
		spi_tx_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI_RX_EN:
		spi_rx_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI_TXFINISH_EN:
		spi_txfinish_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI_RXFINISH_EN:
		spi_rxfinish_enbale(*(UINT8 *)param);
		break;
	case CMD_SPI_TXTRANS_EN:
		set_txtrans_len(*(UINT32 *)param);
		break;
	case CMD_SPI_RXTRANS_EN:
		set_txtrans_len(*(UINT32 *)param);
		break;
	case CMD_SPI_CS_EN:
		spi_slave_set_cs_finish_interrupt(*(UINT32 *)param);
		break;
	default:
		ret = SPI_FAILURE;
		break;
	}

	peri_busy_count_dec();

	return ret;
}

void spi_isr(void)
{
	UINT32 status;

	status = REG_READ(SPI_STAT(spi_channel));
	REG_WRITE(SPI_STAT(spi_channel), status);

	os_printf("0x%08x, %x\r\n", status, spi_channel);

	if ((status & RXINT) || (status & SPI_S_CS_UP_INT_STATUS)) {
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x02);

		if (spi_receive_callback.callback != 0) {
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x02);
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x00);

			void *param = spi_receive_callback.param;
			int is_rx_end = (status & SPI_S_CS_UP_INT_STATUS) ? 1 : 0;
			spi_receive_callback.callback(is_rx_end, param);
		} else {
			/*drop data*/
			spi_rxfifo_clr();
		}
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x00);
	}

	if (status & TXINT) {
		//REG_WRITE((0x00802800+(0x1c*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1c*4)), 0x00);
		os_printf("spi txint\r\n");

		if (spi_txfifo_needwr_callback.callback != 0) {
			void *param = spi_txfifo_needwr_callback.param;

			spi_txfifo_needwr_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}

	if (status & TXOVR)
		os_printf("txovr\r\n");

	if (status & RXOVR)
		os_printf("rxovr\r\n");

	if (status & RX_FINISH_INT)
		os_printf("rx finish int \r\n");

	if (status & TXFIFO_WR_READ)

	{
		if (spi_tx_end_callback.callback != 0) {
			void *param = spi_tx_end_callback.param;

			spi_tx_end_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}
}
void spi2_isr(void)
{
	UINT32 status;

	status = REG_READ(SPI_STAT(spi_channel));
	REG_WRITE(SPI_STAT(spi_channel), status);

	os_printf("0x%08x, %x\r\n", status, spi_channel);

	if ((status & RXINT) || (status & SPI_S_CS_UP_INT_STATUS)) {
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x02);

		if (spi_receive_callback.callback != 0) {
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x02);
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x00);

			void *param = spi_receive_callback.param;
			int is_rx_end = (status & SPI_S_CS_UP_INT_STATUS) ? 1 : 0;
			spi_receive_callback.callback(is_rx_end, param);
		} else {
			/*drop data*/
			spi_rxfifo_clr();
		}
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x00);
	}

	if (status & TXINT) {
		//REG_WRITE((0x00802800+(0x1c*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1c*4)), 0x00);
		os_printf("spi txint\r\n");

		if (spi_txfifo_needwr_callback.callback != 0) {
			void *param = spi_txfifo_needwr_callback.param;

			spi_txfifo_needwr_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}

	if (status & TXOVR)
		os_printf("txovr\r\n");

	if (status & RXOVR)
		os_printf("rxovr\r\n");

	if (status & RX_FINISH_INT)
		os_printf("rx finish int \r\n");

	if (status & TXFIFO_WR_READ)

	{
		if (spi_tx_end_callback.callback != 0) {
			void *param = spi_tx_end_callback.param;

			spi_tx_end_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}
}

void spi3_isr(void)
{
	UINT32 status;

	status = REG_READ(SPI_STAT(spi_channel));
	REG_WRITE(SPI_STAT(spi_channel), status);

	os_printf("0x%08x, %x\r\n", status, spi_channel);

	if ((status & RXINT) || (status & SPI_S_CS_UP_INT_STATUS)) {
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x02);

		if (spi_receive_callback.callback != 0) {
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x02);
			REG_WRITE((0x00802800 + (0x1a * 4)), 0x00);

			void *param = spi_receive_callback.param;
			int is_rx_end = (status & SPI_S_CS_UP_INT_STATUS) ? 1 : 0;
			spi_receive_callback.callback(is_rx_end, param);
		} else {
			/*drop data*/
			spi_rxfifo_clr();
		}
		REG_WRITE((0x00802800 + (0x18 * 4)), 0x00);
	}

	if (status & TXINT) {
		//REG_WRITE((0x00802800+(0x1c*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1c*4)), 0x00);
		os_printf("spi txint\r\n");

		if (spi_txfifo_needwr_callback.callback != 0) {
			void *param = spi_txfifo_needwr_callback.param;

			spi_txfifo_needwr_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}

	if (status & TXOVR)
		os_printf("txovr\r\n");

	if (status & RXOVR)
		os_printf("rxovr\r\n");

	if (status & RX_FINISH_INT)
		os_printf("rx finish int \r\n");

	if (status & TXFIFO_WR_READ)

	{
		if (spi_tx_end_callback.callback != 0) {
			void *param = spi_tx_end_callback.param;

			spi_tx_end_callback.callback(0, param);
		} else {
			/*fill txfifo with 0xff*/
			//spi_txfifo_fill();
		}
	}
}
#endif
// eof
