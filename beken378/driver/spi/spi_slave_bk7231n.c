#include "include.h"
#include "arm_arch.h"
#include "typedef.h"
#include "arm_arch.h"
#include "icu_pub.h"
#include "sys_config.h"

#if(CFG_SOC_NAME == SOC_BK7231N)
#include "spi_bk7231n.h"
#include "spi_pub.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"
#include "error.h"
#include "rtos_pub.h"
#include "general_dma_pub.h"
#include "general_dma.h"

#if CFG_USE_SPI_SLAVE
#define TRUE		1
#define FALSE		0

#define SPI_SLAVE_RX_FIFO_LEN      (512)

struct spi_rx_fifo {
	UINT8 *buffer;

	UINT16 put_index, get_index;

	UINT32 is_full;
};

struct bk_spi_slave_dev {
	UINT32 flag;

	beken_semaphore_t tx_sem;
	UINT8 *tx_ptr;
	UINT32 tx_len;

	beken_semaphore_t rx_sem;
	struct spi_rx_fifo *rx_fifo;

	beken_mutex_t mutex;
};

static struct bk_spi_slave_dev *spi_slave_dev;

UINT32 bk_spi_slave_get_rx_fifo(void)
{
	UINT32 rx_length;
	struct spi_rx_fifo *rx_fifo = spi_slave_dev->rx_fifo;
	GLOBAL_INT_DECLARATION();

	/* get rx length */
	GLOBAL_INT_DISABLE();

	rx_length = (rx_fifo->put_index >= rx_fifo->get_index) ?
				(rx_fifo->put_index - rx_fifo->get_index) :
				(SPI_SLAVE_RX_FIFO_LEN - (rx_fifo->get_index - rx_fifo->put_index));

	GLOBAL_INT_RESTORE();

	return rx_length;
}

static void bk_spi_slave_spi_rx_callback(int is_rx_end, void *param)
{
	UINT8 ch;
	struct spi_rx_fifo *rx_fifo;
	//GLOBAL_INT_DECLARATION();

	rx_fifo = (struct spi_rx_fifo *)spi_slave_dev->rx_fifo;
	ASSERT(rx_fifo != NULL);

	os_printf("rx callback:rx_end:%d\r\n ", is_rx_end);
	//REG_WRITE((0x00802800+(18*4)), 0x02);
	while (1) {
		if (spi_read_rxfifo(&ch) == 0)
			break;
		//REG_WRITE((0x00802800+(0x1a*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1a*4)), 0x00);
		//GLOBAL_INT_DISABLE();

		rx_fifo->buffer[rx_fifo->put_index] = ch;
		rx_fifo->put_index += 1;
		if (rx_fifo->put_index >= SPI_SLAVE_RX_FIFO_LEN)
			rx_fifo->put_index = 0;

		if (rx_fifo->put_index == rx_fifo->get_index) {
			rx_fifo->get_index += 1;
			rx_fifo->is_full = TRUE;
			if (rx_fifo->get_index >= SPI_SLAVE_RX_FIFO_LEN)
				rx_fifo->get_index = 0;
		}
		//GLOBAL_INT_RESTORE();

		if (spi_slave_dev->tx_ptr == NULL)
			spi_write_txfifo(0xFF);
	}

	if (is_rx_end) {
		// only rx end happened, wake up rx_semp
		os_printf("----> rx end\r\n");
		rtos_set_semaphore(&spi_slave_dev->rx_sem);
	}
	//REG_WRITE((0x00802800+(18*4)), 0x00);
}

static int bk_spi_slave_get_rx_data(UINT8 *rx_buf, int len)
{
	struct spi_rx_fifo *rx_fifo;
	rx_fifo = (struct spi_rx_fifo *)spi_slave_dev->rx_fifo;
	int size = len;
	os_printf("len:%d\r\n", len);

	//os_printf("rx_fifo:%d\r\n", rx_fifo);

	ASSERT(rx_fifo != NULL);

	//os_printf("rx_buf:%d\r\n", rx_buf);

	if (rx_buf == NULL)
		return 0;

	//os_printf("%d %d %d\r\n", bk_spi_slave_get_rx_fifo(),
	//rx_fifo->get_index, rx_fifo->put_index);

	while (size) {
		uint8_t ch;
		GLOBAL_INT_DECLARATION();

		GLOBAL_INT_DISABLE();

		if ((rx_fifo->get_index == rx_fifo->put_index)
			&& (rx_fifo->is_full == FALSE)) {
			GLOBAL_INT_RESTORE();
			os_printf("break:get rx data \r\n");
			break;
		}

		ch = rx_fifo->buffer[rx_fifo->get_index];
		rx_fifo->get_index += 1;
		if (rx_fifo->get_index >= SPI_SLAVE_RX_FIFO_LEN)
			rx_fifo->get_index = 0;

		if (rx_fifo->is_full == TRUE)
			rx_fifo->is_full = FALSE;

		GLOBAL_INT_RESTORE();

		*rx_buf = ch & 0xff;
		rx_buf ++;
		size --;

	}

	return (len - size);
}

static void bk_spi_slave_tx_needwrite_callback(int port, void *param)
{
	UINT8 *tx_ptr = spi_slave_dev->tx_ptr;
	UINT32 tx_len = spi_slave_dev->tx_len;
	GLOBAL_INT_DECLARATION();

	if (tx_ptr && tx_len) {
		UINT8 data = *tx_ptr;

		while (spi_write_txfifo(data) == 1) {
			spi_read_rxfifo(&data);
			tx_len --;
			tx_ptr ++;
			if (tx_len == 0) {
				UINT32 enable = 0;
				sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&enable);
				break;
			}
			data = *tx_ptr;
		}
	} else {
		//rt_kprintf("nw:%p,%d\r\n", tx_ptr, tx_len);
		while (spi_write_txfifo(0xff)) {
			if (tx_len)
				tx_len--;

			if (tx_len == 0) {
				os_printf("close tx\r\n");
				UINT32 enable = 0;
				sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&enable);
				break;
			}
		}
	}

	GLOBAL_INT_DISABLE();
	spi_slave_dev->tx_ptr = tx_ptr;
	spi_slave_dev->tx_len = tx_len;
	GLOBAL_INT_RESTORE();

}

static void bk_spi_slave_tx_finish_callback(int port, void *param)
{
	if ((spi_slave_dev->tx_len == 0) && (spi_slave_dev->tx_ptr)) {
		if ((spi_slave_dev->flag & TX_FINISH_FLAG) == 0) {
			spi_slave_dev->flag |= TX_FINISH_FLAG;
			rtos_set_semaphore(&spi_slave_dev->tx_sem);
		}
	}
}

static void bk_spi_slave_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;
	struct spi_callback_des spi_dev_cb;

	/* data bit width */
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPHA, (void *)&param);

	/* slave */
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_MSTEN, (void *)&param);

	// 4line :7231N nssms is 0
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_INIT_MSTEN, (void *)&param);


	/* set call back func */
	spi_dev_cb.callback = bk_spi_slave_spi_rx_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_RX_CALLBACK, (void *)&spi_dev_cb);

	spi_dev_cb.callback = bk_spi_slave_tx_needwrite_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_NEED_WRITE_CALLBACK, (void *)&spi_dev_cb);

	spi_dev_cb.callback = bk_spi_slave_tx_finish_callback;
	spi_dev_cb.param = NULL;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_FINISH_CALLBACK, (void *)&spi_dev_cb);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_LSB_EN, (void *)&param);

	//enable rx/tx finish enable bit
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXFINISH_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXFINISH_EN, (void *)&param);

#ifdef SPI_DMA_TRANS
	//disable rx int bit
	param = 0;
#else
	//disable rx int bit
	param = 1;
#endif
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);

	/* enable spi */
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_UNIT_ENABLE, (void *)&param);

	//BK_SPI_PRT("spi_slave [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));
	//BK_SPI_PRT("spi_slave [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));


}

static void bk_spi_slave_unconfigure(void)
{
	sddev_control(SPI_DEV_NAME, CMD_SPI_DEINIT_MSTEN, NULL);
}

int bk_spi_slave_xfer(struct spi_message *msg)
{
	UINT8 *recv_ptr = NULL;
	const UINT8 *send_ptr = NULL;
	UINT32 param, send_len, recv_len;
	GLOBAL_INT_DECLARATION();

	ASSERT(spi_slave_dev != NULL);
	ASSERT(msg != NULL);

	rtos_lock_mutex(&spi_slave_dev->mutex);

	recv_ptr = msg->recv_buf;
	recv_len = msg->recv_len;
	send_ptr = msg->send_buf;
	send_len = msg->send_len;

	//new spi hardware bug
	param = send_len - 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXTRANS_EN, (void *)&param);

	param = recv_len;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXTRANS_EN, (void *)&param);

	//enbale rx/tx enable bit
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TX_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RX_EN, (void *)&param);

	BK_SPI_PRT("spi_slave [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));
	BK_SPI_PRT("spi_slave [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));


	if ((send_ptr) && send_len) {
		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = (UINT8 *)send_ptr;
		spi_slave_dev->tx_len = send_len;
		spi_slave_dev->flag &= ~(TX_FINISH_FLAG);
		GLOBAL_INT_RESTORE();

		param = 1;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		//BK_SPI_PRT("0 %p-%d\r\n", send_ptr, send_len);
		rtos_get_semaphore(&spi_slave_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

		param = 0;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = NULL;
		spi_slave_dev->tx_len = 0;
		spi_slave_dev->flag |= TX_FINISH_FLAG;
		GLOBAL_INT_RESTORE();

		//BK_SPI_PRT("1 %p-%d\r\n", send_ptr, send_len);
		param = send_len;
	} else if ((recv_ptr) && recv_len) {
		OSStatus err;
		int len;

		GLOBAL_INT_DISABLE();
		spi_slave_dev->tx_ptr = NULL;
		spi_slave_dev->tx_len = recv_len;
		GLOBAL_INT_RESTORE();

		param = 1;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		do {
			len = bk_spi_slave_get_rx_data(recv_ptr, recv_len);
			if (len == 0) {
				err = rtos_get_semaphore(&spi_slave_dev->rx_sem, BEKEN_WAIT_FOREVER);
				if (err != kNoErr)
					break;
			}
		} while (len == 0);

		param = 0;
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		// clear all rx semp for this time
		do {
			err = rtos_get_semaphore(&spi_slave_dev->rx_sem, 0);
		} while (err == kNoErr);

		param = len;
	}

	rtos_unlock_mutex(&spi_slave_dev->mutex);

	return param;
}

int bk_spi_slave_init(UINT32 rate,  UINT32 mode)
{
	OSStatus result = 0;

	if (spi_slave_dev)
		bk_spi_slave_deinit();

	spi_slave_dev = os_malloc(sizeof(struct bk_spi_slave_dev));
	if (!spi_slave_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_slave_dev, 0, sizeof(struct bk_spi_slave_dev));

	result = rtos_init_semaphore(&spi_slave_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	result = rtos_init_semaphore(&spi_slave_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi rx semp init failed\n");
		goto _exit;
	}

	result = rtos_init_mutex(&spi_slave_dev->mutex);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi mutex init failed\n");
		goto _exit;
	}

	struct spi_rx_fifo *rx_fifo;

	rx_fifo = (struct spi_rx_fifo *)os_malloc(sizeof(struct spi_rx_fifo) +
			  SPI_SLAVE_RX_FIFO_LEN);
	if (!rx_fifo) {
		BK_SPI_PRT("[spi]: spi rx fifo malloc failed\n");
		goto _exit;
	}

	rx_fifo->buffer = (uint8_t *)(rx_fifo + 1);
	os_memset(rx_fifo->buffer, 0, SPI_SLAVE_RX_FIFO_LEN);
	rx_fifo->put_index = 0;
	rx_fifo->get_index = 0;
	rx_fifo->is_full = 0;

	spi_slave_dev->rx_fifo = rx_fifo;

	spi_slave_dev->tx_ptr = NULL;
	spi_slave_dev->tx_len = 0;
	spi_slave_dev->flag |= TX_FINISH_FLAG;

	bk_spi_slave_configure(rate, mode);

	return 0;

_exit:
	if (spi_slave_dev->mutex)
		rtos_deinit_mutex(&spi_slave_dev->mutex);

	if (spi_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->tx_sem);

	if (spi_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->rx_sem);

	if (spi_slave_dev->rx_fifo)
		os_free(spi_slave_dev->rx_fifo);

	if (spi_slave_dev) {
		os_free(spi_slave_dev);
		spi_slave_dev = NULL;
	}

	return 1;
}


int bk_spi_slave_deinit(void)
{
	if (spi_slave_dev == NULL)
		return 0;

	bk_spi_slave_unconfigure();

	if (spi_slave_dev->mutex)
		rtos_lock_mutex(&spi_slave_dev->mutex);

	if (spi_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->tx_sem);

	if (spi_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->rx_sem);

	if (spi_slave_dev->rx_fifo)
		os_free(spi_slave_dev->rx_fifo);

	if (spi_slave_dev->mutex) {
		rtos_unlock_mutex(&spi_slave_dev->mutex);
		rtos_deinit_mutex(&spi_slave_dev->mutex);
	}

	os_free(spi_slave_dev);
	spi_slave_dev = NULL;

	return 0;
}

#if CFG_USE_SPI_DMA
volatile int dma_trans_flag = 0;

#define SPI_TEST_POART1		0
#define SPI_TEST_POART2		1
#define SPI_TX_BUFFER_SIZE		1024
#define SPI_RX_BUFFER_SIZE		1024*2
#define SPI_RX_DMA_CHANNEL     GDMA_CHANNEL_1
#define SPI_TX_DMA_CHANNEL     GDMA_CHANNEL_3

void spi_dma_tx_enable(UINT8 enable);

void spi_dma_tx_half_handler(UINT32 param)
{
	BK_SPI_PRT("spi_dma half handler\r\n");
}

void spi_dma_tx_finish_handler(UINT32 param)
{
	if ((spi_slave_dev->flag & TX_FINISH_FLAG) == 0)
	{
		spi_slave_dev->flag |= TX_FINISH_FLAG;
		rtos_set_semaphore(&spi_slave_dev->tx_sem);
	}

	BK_SPI_PRT("spi_dma finish handler\r\n");
}


void spi_dma_rx_half_handler(UINT32 param)
{
	BK_SPI_PRT("spi_dma rx half hander\r\n");
}

void spi_dma_rx_finish_handler(UINT32 param)
{
	rtos_set_semaphore(&spi_slave_dev->rx_sem);
	BK_SPI_PRT("spi_dma rx finish hander\r\n");
}

void spi_debug_prt(UINT32 dma_channel)
{
	int reg_addr = 0;

	// wf debug
	bk_printf("dma channel:0x%x\r\n", dma_channel);

	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x00 + 8*dma_channel) * 4));
	bk_printf("reg00:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x01 + 8*dma_channel) * 4));
	bk_printf("reg01:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x02 + 8*dma_channel) * 4));
	bk_printf("reg02:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x03 + 8*dma_channel) * 4));
	bk_printf("reg03:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x04 + 8*dma_channel) * 4));
	bk_printf("reg04:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x05 + 8*dma_channel) * 4));
	bk_printf("reg05:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x06 + 8*dma_channel) * 4));
	bk_printf("reg06:0x%lx\r\n", reg_addr);
	reg_addr = REG_READ(GENER_DMA0_REG0_CONF + ((0x07 + 8*dma_channel) * 4));
	bk_printf("reg07:0x%lx\r\n", reg_addr);
}

void spi_dma_tx_init(struct spi_message *spi_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;

	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI_DAT;

	init_cfg.channel = SPI_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi_msg->send_buf + spi_msg->send_len;

	init_cfg.half_fin_handler = spi_dma_tx_half_handler;
	init_cfg.fin_handler = spi_dma_tx_finish_handler;

	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI_TX_DMA_CHANNEL;

	en_cfg.param = spi_msg->send_len ;			// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI_TX_DMA_CHANNEL;

	en_cfg.param = 0;					// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	en_cfg.param = 0;					// src no loop:important
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_SRCADDR_LOOP, &en_cfg);

}

void spi_dma_rx_init(struct spi_message *spi_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;

	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI_DAT;
	init_cfg.dst_start_addr = spi_msg->recv_buf;

	init_cfg.channel = SPI_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi_msg->recv_buf + spi_msg->recv_len;

	init_cfg.half_fin_handler = spi_dma_rx_half_handler;
	init_cfg.fin_handler = spi_dma_rx_finish_handler;

	init_cfg.src_module = GDMA_X_SRC_GSPI_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	en_cfg.param   = spi_msg->recv_len;			// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	en_cfg.param = 1;							// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	en_cfg.param = 0;					// src no loop:important
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_DSTADDR_LOOP, &en_cfg);
}

void spi_dma_tx_enable(UINT8 enable)
{
	int param;
	GDMA_CFG_ST en_cfg;

	en_cfg.channel = SPI_TX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	//enable tx
	param = enable;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TX_EN, (void *)&param);
}




void spi_dma_rx_enable(UINT8 enable)
{
	int param ;
	GDMA_CFG_ST en_cfg;

	en_cfg.channel = SPI_RX_DMA_CHANNEL;

	if (enable)
		en_cfg.param = 1;
	else
		en_cfg.param = 0;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

	//enable rx
	param = enable;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RX_EN, (void *)&param);
}



void bk_spi_slave_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;

	bk_spi_slave_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXFINISH_EN, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXOVR_EN, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_CS_EN, (void *)&param);

	BK_SPI_PRT("spi_slave [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));
	BK_SPI_PRT("spi_slave [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));
}

int bk_spi_slave_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	OSStatus result = 0;
	//UINT32 recv_len= spi_msg->recv_len;

	if (spi_slave_dev)
		bk_spi_slave_deinit();

	spi_slave_dev = os_malloc(sizeof(struct bk_spi_slave_dev));
	if (!spi_slave_dev) {
		BK_SPI_WPRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_slave_dev, 0, sizeof(struct bk_spi_slave_dev));

	result = rtos_init_semaphore(&spi_slave_dev->rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_WPRT("[spi]: spi rx semp init failed\n");
		goto _exit;
	}

	bk_spi_slave_dma_config(mode, rate);
	spi_dma_rx_init(spi_msg);

	//sddev_control(SPI_DEV_NAME, CMD_SPI_RXTRANS_EN, (void *)&recv_len);

	return result;

_exit:

	if (spi_slave_dev->rx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->rx_sem);

	if (spi_slave_dev) {
		os_free(spi_slave_dev);
		spi_slave_dev = NULL;
	}

	return result;
}

int bk_spi_slave_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	OSStatus result = 0;
	spi_msg->send_len = spi_msg->send_len + 1;	//slave tx:first byte is 0x72, so must send +1;

	UINT32 send_len= spi_msg->send_len;

	if (spi_slave_dev)
		bk_spi_slave_deinit();

	spi_slave_dev = os_malloc(sizeof(struct bk_spi_slave_dev));
	if (!spi_slave_dev) {
		BK_SPI_WPRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_slave_dev, 0, sizeof(struct bk_spi_slave_dev));

	result = rtos_init_semaphore(&spi_slave_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_WPRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	bk_spi_slave_dma_config(mode, rate);
	spi_dma_tx_init(spi_msg);

	sddev_control(SPI_DEV_NAME, CMD_SPI_TXTRANS_EN, (void *)&send_len);

	return result;

_exit:

	if (spi_slave_dev->tx_sem)
		rtos_deinit_semaphore(&spi_slave_dev->tx_sem);

	if (spi_slave_dev) {
		os_free(spi_slave_dev);
		spi_slave_dev = NULL;
	}

	return result;
}


int bk_spi_slave_dma_send(struct spi_message *spi_msg)
{
	UINT32 ret;

	GLOBAL_INT_DECLARATION();
	ASSERT(spi_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi_slave_dev->flag &= ~(TX_FINISH_FLAG);
	GLOBAL_INT_RESTORE();

	spi_dma_tx_enable(ENABLE);

	ret = rtos_get_semaphore(&spi_slave_dev->tx_sem, BEKEN_NEVER_TIMEOUT);
	if (ret)
		BK_SPI_WPRT("spi_dma tx error:wait tx_sem\r\n");

	return ret;
}


int bk_spi_slave_dma_recv(struct spi_message *spi_msg)
{
	UINT32 ret;
	GLOBAL_INT_DECLARATION();
	ASSERT(spi_msg != NULL);

	GLOBAL_INT_DISABLE();
	spi_dma_rx_enable(ENABLE);
	GLOBAL_INT_RESTORE();

	ret = rtos_get_semaphore(&spi_slave_dev->rx_sem, BEKEN_WAIT_FOREVER);
	if (ret)
		BK_SPI_WPRT("spi_dma rx error:wait rx_sem\r\n");

	spi_dma_rx_enable(DISABLE);

	return ret;
}

int bk_spi_slave_dma_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	UINT32 ret = 0;

	if (spi_msg->send_buf)
	{
		ret = bk_spi_slave_dma_tx_init( mode, rate, spi_msg);
		if (ret)
			BK_SPI_WPRT("master:spi dma int error:%d\r\n", ret);
	}

	if (spi_msg->recv_buf)
	{
		ret = bk_spi_slave_dma_rx_init( mode, rate, spi_msg);
		if (ret)
			BK_SPI_WPRT("master:spi dma init error:%d\r\n", ret);
	}

	return ret;
}



int bk_spi_slave_dma_transfer(struct spi_message *spi_msg)
{
	UINT32 ret = 0;

	if (spi_msg->send_buf)
	{
		ret = bk_spi_slave_dma_send(spi_msg);
		if (ret)
			BK_SPI_WPRT("spi dma send error:%d\r\n", ret);
	}

	if (spi_msg->recv_buf)
	{
		ret = bk_spi_slave_dma_recv(spi_msg);
		if (ret)
			BK_SPI_WPRT("spi dma recv error:%d\r\n", ret);
	}

	return ret;
}




#define LOOP_TEST_LEN    30*24

void spi_rx_loop_test(void *arg)
{
	struct spi_message	msg;

	UINT8 *buf, *tx_buff;
	int rx_len, ret, i;
	int cnt = 0;
	int err_cnt;

	rx_len = LOOP_TEST_LEN;

	BK_SPI_PRT("spi dma rx: rx_len:%d\n", rx_len);

	tx_buff = os_malloc(rx_len * sizeof(UINT8));
	for (i = 0 ; i < 30 * 24; i++)
		tx_buff[i] = 0x60 + i ;

	buf = os_malloc(rx_len * sizeof(UINT8));

	if (!buf)
		BK_SPI_WPRT("spi dma buff malloc error\r\n");

	os_memset(buf, 0, rx_len);

	msg.send_buf = NULL;
	msg.send_len = 0;
	msg.recv_buf = buf;
	msg.recv_len = rx_len;

	UINT8 mode = SPI_MODE_0 | SPI_MSB | SPI_SLAVE;
	UINT32 max_hz = 8 * 1000 * 1000;

	bk_spi_slave_dma_rx_init(mode, max_hz, &msg);

	while (1) {
		err_cnt = 0;
		ret = bk_spi_slave_dma_recv(&msg);
		if (ret == 0) {
			for (i = 0; i < LOOP_TEST_LEN; i++) {
				if (buf[i] != tx_buff[i]) {
					if (err_cnt > 4)
						break;

					BK_SPI_PRT("tx_buff[%d]=0x%x,  buff[%d]=0x%x, \r\n", i, tx_buff[i], i, buf[i]);
					err_cnt ++;
				}
			}
		}

		cnt ++;

		BK_SPI_PRT("cnt:%d\r\n", cnt);
		os_memset(buf, 0x55, rx_len);
	}
}

#if (!CFG_SUPPORT_RTT)

beken_thread_t spi_dma_slave_rx_thread_handle = NULL;
uint32 spi_dma_slave_rx_thread_main(void)
{
	int ret;

	ret = rtos_create_thread(&spi_dma_slave_rx_thread_handle,
							 BEKEN_DEFAULT_WORKER_PRIORITY,
							 "spidma",
							 (beken_thread_function_t)spi_rx_loop_test,
							 4096,
							 (beken_thread_arg_t)0);
	if (ret != kNoErr) {
		bk_printf("Error: airkiss_start_process: %d\r\n", ret);
		goto init_err;
	}

	return kNoErr;

init_err:
	return kGeneralErr;
}

#endif
#endif	 //CFG_USE_SPI_DMA
#endif	 // CFG_USE_SPI_SLAVE
#endif   //(CFG_SOC_NAME == SOC_BK7231N)


