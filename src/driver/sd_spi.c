/*
 *  @file  : sd_spi.c
 *	
 *  @brief	
 *
 *  @author: franc
 *  @date  : 23 ott 2021
 */
#include "driver\sd_spi.h"
#include "driver\spi.h"

#include "diskio.h"
#include "ff.h"

#include "delay.h"

/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

static BYTE _sd_card_type;	/* Card type flags */

static BYTE sd_spi_send_cmd(BYTE cmd, DWORD arg);
static void sd_spi_deselect(void);
static ErrorStatus sd_spi_select(void);

static bool sd_spi_wait_ready(UINT delay);
static bool sd_spi_detect_card(void);

void sd_spi_init(void)
{
	SPI_Config();

	delay_ms(10);
}

/**
 *
 * @param pdrv
 * @return
 */
DSTATUS disk_initialize (BYTE pdrv)
{
	BYTE n;
	BYTE cmd;
	BYTE ty;
	BYTE ocr[4];
	DSTATUS result = 0;
	_sd_card_type = 0;

	if (!sd_spi_detect_card())
	{
		return STA_NODISK;
	}

	for (n = 0; n < 10; n++)
	{
		spi_single_dummy_write();
	}
	ty = 0;

	if (sd_spi_send_cmd(CMD0, 0) == 1u)	/* Put the card SPI/Idle state */
	{
		delay_ms(1000);	/* wait 1 sec */
		if (sd_spi_send_cmd(CMD8, 0x1AA) == 1)	/* SDv2? */
		{

		}
	}

	return result;
}

/**
 *
 * @param pdrv
 * @return
 */
DSTATUS disk_status (BYTE pdrv)
{
	DSTATUS result = 0;

	return result;
}

/**
 *
 * @param pdrv
 * @param buff
 * @param sector
 * @param count
 * @return
 */
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	DSTATUS result = 0;

	return result;
}

/**
 *
 * @param pdrv
 * @param buff
 * @param sector
 * @param count
 * @return
 */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	DSTATUS result = 0;

	return result;
}

/**
 *
 * @param pdrv
 * @param cmd
 * @param buff
 * @return
 */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	DSTATUS result = 0;

	return result;
}

/**
 *
 * @param cmd
 * @param arg
 * @return
 */
static BYTE sd_spi_send_cmd(BYTE cmd, DWORD arg)
{
	BYTE n;
	BYTE res;

	/* Send a CMD55 prior to ACMD<n> */
	if (cmd & 0x80)
	{
		cmd &= 0x7F;
		res = sd_spi_send_cmd(CMD55, 0);
		if (res > 1)
			return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12)
	{
		CS_HIGH;
		sd_spi_deselect();
		if (sd_spi_select() == ERROR)
		{
			return 0xFF;
		}
	}

	/* Send command packet */
	spi_exchange(0x40 | cmd);				/* Start + command index */
	spi_exchange((BYTE)(arg >> 24));		/* Argument[31..24] */
	spi_exchange((BYTE)(arg >> 16));		/* Argument[23..16] */
	spi_exchange((BYTE)(arg >> 8));			/* Argument[15..8] */
	spi_exchange((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */

	if (cmd == CMD0)
	{
		n = 0x95;			/* Valid CRC for CMD0(0) */
	}

	if (cmd == CMD8)
	{
		n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	}

	spi_exchange(n);

	/* Receive command resp */
	if (cmd == CMD12)
	{
		spi_exchange(0xFF);	/* Discard following one byte when CMD12 */
	}

	n = 10;								/* Wait for response (10 bytes max) */

	do
	{
		res = spi_exchange(0xFF);
	} while ((res & 0x80) && --n);

	return res;							/* Return received response */
}

/**
 *
 */
static void sd_spi_deselect(void)
{
	CS_HIGH;
	SPI_DUMMY_WRITE;
}

/**
 *
 * @return
 */
static ErrorStatus sd_spi_select(void)
{
	CS_LOW;
	SPI_DUMMY_WRITE;

	if (sd_spi_wait_ready(500u))
	{
		return SUCCESS;
	}

	sd_spi_deselect();
	return ERROR;
}

/**
 *
 * @param delay
 * @return
 */
static bool sd_spi_wait_ready(UINT delay)
{
	BYTE rx_byte;

	delay_load_ms(delay);

	do
	{
		rx_byte = spi_exchange(0xFF);
	}while((rx_byte != 0xFF)
			&& (!delay_has_expired()));

	return (rx_byte == 0xFF);
}

/**
 *
 * @return
 */
bool sd_spi_detect_card(void)
{
	/*
	 * not implemented !
	 */
	return true;
}
