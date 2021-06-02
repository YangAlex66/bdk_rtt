#ifndef _SPI_H_
#define _SPI_H_

#include "sys_config.h"
#include "general_dma_pub.h"

#if(CFG_SOC_NAME == SOC_BK7271)
#define SPI_DEBUG
#ifdef SPI_DEBUG
#define SPI_PRT      os_printf
#define SPI_WARN     warning_prf
#define SPI_FATAL    fatal_prf
#else
#define SPI_PRT      null_prf
#define SPI_WARN     null_prf
#define SPI_FATAL    null_prf
#endif

#define SPI_BASE(x)                         (0x00802500+(x * 0x40))

#define SPI_CTRL(x)							(SPI_BASE(x)+ 0 * 4)
#define TXINT_MODE_POSI						0
#define TXINT_MODE_MASK						(0x03UL << 0)
#define TXINT_MODE_1						(0x00UL << 0)
#define TXINT_MODE_16						(0x01UL << 0)
#define TXINT_MODE_32						(0x02UL << 0)
#define TXINT_MODE_48						(0x03UL << 0)

#define RXINT_MODE_POSI						2
#define RXINT_MODE_MASK						(0x03UL << 2)
#define RXINT_MODE_1						(0x00UL << 2)
#define RXINT_MODE_16						(0x01UL << 2)
#define RXINT_MODE_32						(0x02UL << 2)
#define RXINT_MODE_48						(0x03UL << 2)
#define TXOVR_EN							(0x01UL << 4)
#define RXOVR_EN							(0x01UL << 5)
#define TXINT_EN							(0x01UL << 6)
#define RXINT_EN							(0x01UL << 7)
#define SPI_CKR_POSI						(8)
#define SPI_CKR_MASK						(0xFF)
#define SPI_S_CS_UP_INT_EN					(0x01UL << 16)
#define CTRL_NSSMD_4						(0x00UL << 17)
#define CTRL_NSSMD_3						(0x01UL << 17)
#define BIT_WDTH							(0x01UL << 18)
#define LSB_FIRST							(0x01UL << 19)
#define MSB_FIRST							(0x00UL << 19)
#define CKPOL								(0x01UL << 20)
#define CKPHA								(0x01UL << 21)
#define MSTEN								(0x01UL << 22)
#define SPIEN								(0x01UL << 23)

#define SPI_CONFIG(x)						(SPI_BASE(x) + 0x1 * 4)
#define SPI_TX_EN							(0x01UL << 0)
#define SPI_RX_EN							(0x01UL << 1)
#define SPI_TX_FINISH_EN					(0x01UL << 2)
#define SPI_RX_FINISH_EN					(0x01UL << 3)
#define SPI_TX_TRAHS_LEN_POSI				8
#define SPI_RX_TRAHS_LEN_POSI				20

#define SPI_STAT(x)							(SPI_BASE(x) + 0x2 * 4)
#define SPI_STAT_RXFIFO_RD_POSI				1
#define SPI_STAT_TXFIFO_WR_POSI				2
#define TXFIFO_WR_READ						(0x01UL << 1)
#define RXFIFO_RD_READ						(0x01UL << 2)
#define TXINT								(0x01UL << 8)
#define RXINT								(0x01UL << 9)
#define SPI_S_CS_UP_INT_STATUS				(0x01UL << 10)
#define TXOVR								(0x01UL << 11)
#define RXOVR								(0x01UL << 12)
#define TX_FINISH_INT						(0x01UL << 13)
#define RX_FINISH_INT						(0x01UL << 14)
#define TXFIFO_CLR_EN						(0x01UL << 16)
#define RXFIFO_CLR_EN						(0x01UL << 17)

#define SPI_DAT(x)							(SPI_BASE(x) + 3 * 4)
#define SPI_DAT_POSI						(0)
#define SPI_DAT_MASK						(0xFFFF)

//dma channel Overwirte same definition in general_dma_pub.h
#ifdef GDMA_X_DST_GSPI_TX_REQ
#undef GDMA_X_DST_GSPI_TX_REQ
#endif
#define GDMA_X_DST_GSPI_TX_REQ				(0x9)

#define GDMA_X_DST_GSPI1_TX_REQ				(0x9)
#define GDMA_X_DST_GSPI2_TX_REQ				(0xa)
#define GDMA_X_DST_GSPI3_TX_REQ				(0xb)

#define GDMA_X_SRC_GSPI1_RX_REQ				(0x9)
#define GDMA_X_SRC_GSPI2_RX_REQ				(0xa)
#define GDMA_X_SRC_GSPI3_RX_REQ				(0xb)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 spi_ctrl(UINT32 cmd, void *param);
#endif
#endif //_SPI_H_

