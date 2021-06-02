#include "include.h"
#include <stdlib.h>

#include "stdarg.h"
#include "mem_pub.h"
#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"
#include "sys_ctrl_pub.h"
#include "wlan_cli_pub.h"
#include "arm_mcu_pub.h"
#include "BkDriverPwm.h"
#include "lwip/ping.h"
#include "ble_pub.h"
#include "sensor.h"
#include "spi_pub.h"
#include "i2c_pub.h"
#include "BkDriverTimer.h"
#include "BkDriverPwm.h"
#include "saradc_intf.h"
#include "drv_model_pub.h"

#define BK_LOGW( tag, format, ... ) os_printf(format, ##__VA_ARGS__)
#define BK_LOGI( tag, format, ... ) os_printf(format, ##__VA_ARGS__)
#define BK_LOGD( tag, format, ... ) os_printf(format, ##__VA_ARGS__)
#define BK_LOGV( tag, format, ... ) os_printf(format, ##__VA_ARGS__)

#define CFG_LOG_LEVEL			BK_LOG_DEBUG

#define PERI_LOGI				BK_LOGI
#define PERI_LOGW				BK_LOGW
#define PERI_LOGD				BK_LOGD

#if CFG_PERIPHERAL_TEST

INT32 os_strcmp(const char *s1, const char *s2);


#define I2C_TEST_LEGNTH					32
#define I2C_TEST_EEPROM_LEGNTH			8

#if CFG_USE_I2C1
static void i2c1_test_eeprom(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{

	int i;
	DD_HANDLE i2c_hdl;
	unsigned int status;
	unsigned int oflag;
	I2C_OP_ST i2c1_op;
	I2C1_MSG_ST i2c_msg_config;

	os_printf(" i2c1_test_eeprom start  \r\n");

	i2c_msg_config.pData = (UINT8 *)os_malloc(I2C_TEST_EEPROM_LEGNTH);
	if (i2c_msg_config.pData == NULL) {
		os_printf("malloc fail\r\n");
		goto exit;
	}

	oflag   = (0 & (~I2C1_MSG_WORK_MODE_MS_BIT)     // master
			   & (~I2C1_MSG_WORK_MODE_AL_BIT))    // 7bit address
			  | (I2C1_MSG_WORK_MODE_IA_BIT);     // with inner address

	i2c_hdl = ddev_open(I2C1_DEV_NAME, &status, oflag);

	if (os_strcmp(argv[1], "write_eeprom") == 0) {
		os_printf("eeprom write\r\n");

		for (i = 0; i < I2C_TEST_EEPROM_LEGNTH; i++)
			i2c_msg_config.pData[i] = (i << 2) + 0x10 ;

		i2c1_op.op_addr    = 0x08;
		i2c1_op.salve_id   = 0x50;      //send slave address
		i2c1_op.slave_addr = 0x73;      //slave: as slave address

		do {
			status = ddev_write(i2c_hdl, (char *)i2c_msg_config.pData, I2C_TEST_EEPROM_LEGNTH, (unsigned long)&i2c1_op);
		} while (status != 0);
	}
	if (os_strcmp(argv[1], "read_eeprom") == 0) {
		os_printf("eeprom read\r\n");

		i2c1_op.op_addr    = 0x08;
		i2c1_op.salve_id   = 0x50;      //send slave address
		i2c1_op.slave_addr = 0x73;      //slave: as slave address

		do {
			status = ddev_read(i2c_hdl, (char *)i2c_msg_config.pData, I2C_TEST_EEPROM_LEGNTH, (unsigned long)&i2c1_op);
		} while (status != 0);
	}

	for (i = 0; i < I2C_TEST_EEPROM_LEGNTH; i++)
		os_printf("pData[%d]=0x%x\r\n", i, i2c_msg_config.pData[i]);

	ddev_close(i2c_hdl);

	os_printf(" i2c2 test over\r\n");

exit:

	if (NULL != i2c_msg_config.pData) {
		os_free(i2c_msg_config.pData);
		i2c_msg_config.pData = NULL;
	}
}
#endif

#if CFG_USE_I2C2
static void i2c2_test_eeprom(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{

	int i;
	DD_HANDLE i2c_hdl;
	unsigned int status;
	unsigned int oflag;
	I2C_OP_ST i2c2_op;
	I2C2_MSG_ST i2c_msg_config;

	os_printf(" i2c2_test_eeprom start  \r\n");

	i2c_msg_config.pData = (UINT8 *)os_malloc(I2C_TEST_EEPROM_LEGNTH);
	if (i2c_msg_config.pData == NULL) {
		os_printf("malloc fail\r\n");
		goto exit;
	}

	oflag   = (0 & (~I2C2_MSG_WORK_MODE_MS_BIT)     // master
			   & (~I2C2_MSG_WORK_MODE_AL_BIT))    // 7bit address
			  | (I2C2_MSG_WORK_MODE_IA_BIT);     // with inner address

	i2c_hdl = ddev_open(I2C2_DEV_NAME, &status, oflag);

	if (os_strcmp(argv[1], "write_eeprom") == 0) {
		os_printf("eeprom write\r\n");

		for (i = 0; i < I2C_TEST_EEPROM_LEGNTH; i++)
			i2c_msg_config.pData[i] = (i << 2) + 0x10 ;

		i2c2_op.op_addr     = 0x08;
		i2c2_op.salve_id    = 0x50;     //send slave address
		i2c2_op.slave_addr  = 0x73;     //slave: as slave address

		do {
			status = ddev_write(i2c_hdl, (char *)i2c_msg_config.pData, I2C_TEST_EEPROM_LEGNTH, (unsigned long)&i2c2_op);
		} while (status != 0);
	}
	if (os_strcmp(argv[1], "read_eeprom") == 0) {
		os_printf("eeprom read\r\n");

		i2c2_op.op_addr    = 0x08;
		i2c2_op.salve_id   = 0x50;      //send slave address
		i2c2_op.slave_addr = 0x73;      //slave: as slave address

		do {
			status = ddev_read(i2c_hdl, (char *)i2c_msg_config.pData, I2C_TEST_EEPROM_LEGNTH, (unsigned long)&i2c2_op);
		} while (status != 0);
	}

	for (i = 0; i < I2C_TEST_EEPROM_LEGNTH; i++)
		os_printf("pData[%d]=0x%x\r\n", i, i2c_msg_config.pData[i]);
	os_free(i2c_msg_config.pData);

	ddev_close(i2c_hdl);
	os_printf(" i2c2 test over\r\n");

exit:

	if (NULL != i2c_msg_config.pData) {
		os_free(i2c_msg_config.pData);
		i2c_msg_config.pData = NULL;
	}
}
#endif

#if CFG_SUPPORT_SPI_TEST
#define SPI_BAUDRATE       (26 * 1000 * 1000)
#define SPI_BAUDRATE_5M    (5* 1000 * 1000)

#define SPI_TX_BUF_LEN     (1024)
#define SPI_RX_BUF_LEN     (1024)

#if (CFG_SOC_NAME == SOC_BK7271)
int spi_channel;
#endif

void gspi_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	struct spi_message  msg;
	UINT32 max_hz;
	UINT32 mode;

#if (CFG_SOC_NAME == SOC_BK7271)
	spi_channel = 2;
#endif

	/* SPI Interface with Clock Speeds Up to 30 MHz */
	if (argc == 5)
	{
		max_hz = SPI_BAUDRATE ;//atoi(argv[3]);
	} else
	{
		max_hz = SPI_BAUDRATE; //master/slave
	}

	if (os_strcmp(argv[1], "master") == 0)
	{
		mode = SPI_MODE_0 | SPI_MSB | SPI_MASTER;

		//bk_spi_master_init (cfg->max_hz, cfg->mode);
	} else if (os_strcmp(argv[1], "slave") == 0)
	{
		mode = SPI_MODE_0 | SPI_MSB | SPI_SLAVE;

		bk_spi_slave_init(max_hz, mode);
	}
#if CFG_USE_SPI_DMA
	else if (os_strcmp(argv[1], "slave_dma_rx") == 0)
	{
		UINT8 *buf;
		int rx_len, ret;

		if (argc < 2)
			rx_len = SPI_RX_BUF_LEN;
		else
			rx_len = atoi(argv[2]);

		bk_printf("spi dma rx: rx_len:%d\n", rx_len);

		buf = os_malloc(rx_len * sizeof(UINT8));
		if (!buf) {
			bk_printf("spi test malloc buf fail\r\n");
			return ;
		}

		os_memset(buf, 0, rx_len);

		msg.send_buf = NULL;
		msg.send_len = 0;
		msg.recv_buf = buf;
		msg.recv_len = rx_len;

		mode = SPI_MODE_0 | SPI_MSB | SPI_SLAVE;
		max_hz = atoi(argv[3]);

		bk_spi_dma_init(mode, max_hz, &msg);

		ret = bk_spi_dma_transfer(mode, &msg);
		if (ret)
			bk_printf("spi dma recv error%d\r\n", ret);
		else {
			for (int i = 0; i < rx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");
			os_free(buf);
		}
	} else if ((os_strcmp(argv[1], "slave_dma_tx") == 0))
	{
		UINT8 *buf;
		int tx_len, ret;

		if (argc < 2)
			tx_len = SPI_RX_BUF_LEN;
		else
			tx_len = atoi(argv[2]);

		bk_printf("spi dma tx: tx_len:%d,%d\n", tx_len, max_hz);

		buf = os_malloc(tx_len * sizeof(UINT8));
		if (!buf) {
			bk_printf("spi test malloc buf fail\r\n");
			return ;
		}

		os_memset(buf, 0, tx_len);

		for (int i = 0; i < tx_len; i++)
			buf[i] = i & 0xFF;

		msg.send_buf = buf;
		msg.send_len = tx_len;
		msg.recv_buf = NULL;
		msg.recv_len = 0;

		mode = SPI_MODE_0 | SPI_MSB | SPI_SLAVE;
		max_hz = atoi(argv[3]);

		bk_spi_dma_init(mode, max_hz, &msg);

		ret = bk_spi_dma_transfer(mode, &msg);
		if (ret)
			bk_printf("spi dma send error%d\r\n", ret);
		else {
			for (int i = 0; i < tx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");
			os_free(buf);
		}
	} else if ((os_strcmp(argv[1], "master_dma_tx") == 0))
	{
		UINT8 *buf;
		int tx_len, ret;

		if (argc < 2)
			tx_len = SPI_RX_BUF_LEN;
		else
			tx_len = atoi(argv[2]);

		max_hz = atoi(argv[3]);//SPI_BAUDRATE;

		bk_printf("spi master  dma tx: tx_len:%d max_hz:%d\r\n", tx_len, max_hz);

		buf = os_malloc(tx_len * sizeof(UINT8));
		if (!buf) {
			bk_printf("spi test malloc buf fail\r\n");
			return ;
		}

		os_memset(buf, 0, tx_len);

		for (int i = 0; i < tx_len; i++)
			buf[i] = i & 0xFF;

		msg.send_buf = buf;
		msg.send_len = tx_len;
		msg.recv_buf = NULL;
		msg.recv_len = 0;

		mode = SPI_MODE_0 | SPI_MSB | SPI_MASTER;

		bk_spi_dma_init(mode, max_hz, &msg);

		ret = bk_spi_dma_transfer(mode,&msg);
		if (ret)
			bk_printf("spi dma send error%d\r\n", ret);
		else {
			for (int i = 0; i < tx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");
			os_free(buf);
		}
	} else if ((os_strcmp(argv[1], "master_dma_rx") == 0))
	{
		UINT8 *buf;
		int rx_len, ret;

		if (argc < 2)
			rx_len = SPI_RX_BUF_LEN;
		else
			rx_len = atoi(argv[2]) + 1;	//slave tx first send 0x72 so must send one more

		max_hz = atoi(argv[3]);//SPI_BAUDRATE;

		bk_printf("spi master  dma rx: rx_len:%d max_hz:%d\r\n\n", rx_len, max_hz);

		buf = os_malloc(rx_len * sizeof(UINT8));
		if (!buf) {
			bk_printf("spi test malloc buf fail\r\n");
			return ;
		}

		os_memset(buf, 0, rx_len);

		msg.send_buf = NULL;
		msg.send_len = 0;
		msg.recv_buf = buf;
		msg.recv_len = rx_len;

		mode = SPI_MODE_0 | SPI_MSB | SPI_MASTER;

		bk_spi_dma_init(mode, max_hz, &msg);

		ret = bk_spi_dma_transfer(mode,&msg);
		if (ret)
			bk_printf("spi dma recv error%d\r\n", ret);
		else {
			for (int i = 1; i < rx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");
			os_free(buf);
		}

	} else if ((os_strcmp(argv[1], "master_tx_loop") == 0))
	{
		UINT8 *buf;
		int tx_len, ret;
		UINT32 cnt = 0;

		if (argc < 2)
			tx_len = SPI_RX_BUF_LEN;
		else
			tx_len = atoi(argv[2]);

		max_hz = atoi(argv[3]); //SPI_BAUDRATE;

		bk_printf("spi master  dma tx: tx_len:%d max_hz:%d\r\n", tx_len, max_hz);

		buf = os_malloc(tx_len * sizeof(UINT8));
		if (!buf) {
			bk_printf("buf malloc fail\r\n");
			return;
		}

		os_memset(buf, 0, tx_len);

		for (int i = 0; i < tx_len; i++)
			buf[i] = i + 0x60;

		msg.send_buf = buf;
		msg.send_len = tx_len;
		msg.recv_buf = NULL;
		msg.recv_len = 0;

		mode = SPI_MODE_0 | SPI_MSB | SPI_MASTER;

		bk_spi_dma_init(mode, max_hz, &msg);

		while (1) {
			if (cnt >= 0x1000)
				break;

			ret = bk_spi_dma_transfer(mode,&msg);
			if (ret)
				bk_printf("spi dma send error%d\r\n", ret);

			else
				bk_printf("%d\r\n", cnt++);
			rtos_delay_milliseconds(80);
		}
	}


#endif

	else
		bk_printf("gspi_test master/slave	 tx/rx	rate  len\r\n");

	//CLI_LOGI("cfg:%d, 0x%02x, %d\r\n", cfg->data_width, cfg->mode, cfg->max_hz);

	if (os_strcmp(argv[2], "tx") == 0)
	{
		UINT8 *buf;
		int tx_len;

		if (argc < 4)
			tx_len = SPI_TX_BUF_LEN;
		else
			tx_len = atoi(argv[4]);

		bk_printf("spi init tx_len:%d\n", tx_len);

		buf = os_malloc(tx_len * sizeof(UINT8));

		if (buf) {
			os_memset(buf, 0, tx_len);
			for (int i = 0; i < tx_len; i++)
				buf[i] = i & 0xff;
			msg.send_buf = buf;
			msg.send_len = tx_len;
			msg.recv_buf = NULL;
			msg.recv_len = 0;

			bk_spi_slave_xfer(&msg);

			for (int i = 0; i < tx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");

			os_free(buf);
		}
	} else if (os_strcmp(argv[2], "rx") == 0)
	{
		UINT8 *buf;
		int rx_len;

		if (argc < 4)
			rx_len = SPI_RX_BUF_LEN;
		else
			rx_len = atoi(argv[4]);

		bk_printf("SPI_RX: rx_len:%d\n", rx_len);

		buf = os_malloc(rx_len * sizeof(UINT8));

		if (buf) {
			os_memset(buf, 0, rx_len);

			msg.send_buf = NULL;
			msg.send_len = 0;
			msg.recv_buf = buf;
			msg.recv_len = rx_len;

			//CLI_LOGI("buf:%d\r\n", buf);
			rx_len = bk_spi_slave_xfer(&msg);
			bk_printf("rx_len:%d\r\n", rx_len);

			for (int i = 0; i < rx_len; i++) {
				bk_printf("%02x,", buf[i]);
				if ((i + 1) % 32 == 0)
					bk_printf("\r\n");
			}
			bk_printf("\r\n");

			os_free(buf);
		}
	} else
	{
		//CLI_LOGI("gspi_test master/slave tx/rx rate len\r\n");
	}
}





uint32 spi_dma_slave_rx_thread_main(void);

void spi_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{

#if (CFG_SOC_NAME == SOC_BK7271)
	spi_channel = 2;
#endif

	if (os_strcmp(argv[1], "slave_rx_loop") == 0)
	{
		bk_printf("spi dma rx loop test\r\n");
		spi_dma_slave_rx_thread_main();
	}
}


#endif

static void pwm_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	UINT8 channel1;
	UINT32 duty_cycle1, cycle, cap_value;
#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236)
	UINT8 channel2;
	UINT32 duty_cycle2;
	UINT32 dead_band;
#endif

	/*get the parameters from command line*/
	channel1	= atoi(argv[2]);
	duty_cycle1	= atoi(argv[3]);
	cycle		= atoi(argv[4]);
#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236)
	channel2	= atoi(argv[5]);
	duty_cycle2	= atoi(argv[6]);
	dead_band	= atoi(argv[7]);
#endif

	if (cycle < duty_cycle1)
	{
		PERI_LOGW(TAG, "pwm param error: end < duty\r\n");
		return;
	}

	if (os_strcmp(argv[1], "single") == 0)
	{
		if (5 != argc) {
			PERI_LOGW(TAG, "pwm single test usage: pwm [single][channel][duty_cycle][freq]\r\n");
			return;
		}
		PERI_LOGI(TAG, "pwm channel %d: duty_cycle: %d  freq:%d \r\n", channel1, duty_cycle1, cycle);

		bk_pwm_initialize(channel1, cycle, duty_cycle1,0,0);
		bk_pwm_start(channel1);				/*start single pwm channel once */
	} else if (os_strcmp(argv[1], "stop") == 0)
		bk_pwm_stop(channel1);
#if ((CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236) ||(CFG_SOC_NAME == SOC_BK7271))
	else if (os_strcmp(argv[1], "update") == 0)
	{
		if (5 != argc) {
			PERI_LOGW(TAG, "pwm update usage: pwm [update][channel1][duty_cycle][freq]\r\n");
			return;
		}

		PERI_LOGI(TAG, "pwm %d update: %d\r\n", duty_cycle1);
		bk_pwm_update_param(channel1, cycle, duty_cycle1);		/*updata pwm freq and duty_cycle */
	} else if (os_strcmp(argv[1], "cap") == 0)
	{
		uint8_t cap_mode = duty_cycle1;

		if (5 != argc) {
			PERI_LOGW(TAG, "pwm cap usage: pwm [cap][channel1][mode][freq]\r\n");
			return;
		}

		bk_pwm_capture_initialize(channel1, cap_mode);			/*capture pwm value */
		bk_pwm_start(channel1);
	} else if (os_strcmp(argv[1], "capvalue") == 0)
	{
		cap_value = bk_pwm_get_capvalue(channel1);
		PERI_LOGI(TAG, "pwm : %d cap_value=%x \r\n", channel1, cap_value);
	}
	#if ((CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236))
	else if (os_strcmp(argv[1], "cw") == 0)
	{

		if (8 != argc) {
			PERI_LOGW(TAG, "pwm cw test usage: pwm [cw][channel1][duty_cycle1][freq][channel2][duty_cycle2][dead_band]\r\n");
			return;
		}

		PERI_LOGI(TAG, "pwm : %d / %d cw pwm test \r\n", channel1, channel2);

		bk_pwm_cw_initialize(channel1, channel2, cycle, duty_cycle1, duty_cycle2, dead_band);
		bk_pwm_cw_start(channel1, channel2);
	} else if (os_strcmp(argv[1], "updatecw") == 0)
	{
		if (8 != argc) {
			PERI_LOGW(TAG, "pwm cw test usage: pwm [cw][channel1][duty_cycle1][freq][channel2][duty_cycle2][dead_band]\r\n");
			return;
		}

		PERI_LOGI(TAG, "pwm : %d / %d cw updatw pwm test \r\n", channel1, channel2);

		bk_pwm_cw_update_param(channel1, channel2, cycle, duty_cycle1, duty_cycle2, dead_band);
	} else if (os_strcmp(argv[1],  "loop") == 0)
	{
		uint16_t cnt = 1000;

		PERI_LOGI(TAG, "pwm : %d / %d pwm update loop test \r\n", channel1, channel2);

		while (cnt--) {
			duty_cycle1 = duty_cycle1 - 100;

			bk_pwm_cw_update_param(channel1, channel2, cycle, duty_cycle1, duty_cycle2, dead_band);
			rtos_delay_milliseconds(10);

			if (duty_cycle1 == 0)
				duty_cycle1 = cycle;
		}
	}
	#endif
	#endif
}



const struct cli_command peripheral_clis[] = {

#if CFG_SUPPORT_SPI_TEST
	{"gspi_test", "general spi", gspi_test},

	{"spi_test", "spi dma rx loop", spi_Command},
#endif

#if CFG_USE_I2C1
	{"i2c1_test", "i2c1_test write/read_eeprom", i2c1_test_eeprom},
#endif

#if CFG_USE_I2C2
	{"i2c2_test", "i2c2_test write/read_eeprom", i2c2_test_eeprom},
#endif

	{"pwm_test", "pwm single/update/cw ", pwm_Command},

};


void bk_peripheral_cli_init(void)
{
	int ret;

	os_printf("peripheral cli int \r\n");
	ret = cli_register_commands(peripheral_clis, sizeof(peripheral_clis) / sizeof(struct cli_command));
	if (ret)
		os_printf("ret: %d peripheral commands fail.\r\n",ret);
}

#endif
