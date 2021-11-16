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

static volatile DSTATUS _stat = STA_NOINIT;		/* Physical drive status */
static BYTE _media_type;						/* Card type flags */

static BYTE sd_spi_send_cmd(BYTE cmd, DWORD arg);
static void sd_spi_deselect(void);
static ErrorStatus sd_spi_select(void);

static bool sd_spi_wait_ready(UINT delay);
static bool sd_spi_detect_card(void);
static bool sd_spi_receive_datablock(BYTE *buff, UINT btr);
#if FF_FS_READONLY == 0
static bool sd_spi_sends_datablock(const BYTE *buff, BYTE token);
#endif

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


	_media_type = 0;

	if (!sd_spi_detect_card())	/* check if card is present */
	{
		_stat = STA_NODISK;
	}
	else
	{
		for (n = 0; n < 10; n++)
		{
			SPI_DUMMY_WRITE;	/* dummy cycle */
		}
		ty = 0;

		if (sd_spi_send_cmd(CMD0, 0) == 1u)	/* Put the card SPI/Idle state */
		{
			delay_load_ms(1000);	/* load 1 sec */
			if (sd_spi_send_cmd(CMD8, 0x1AA) == 1)	/* SDv2? */
			{
				for (n = 0; n < 4; n++)
				{
					ocr[n] = spi_exchange(0xFF);	/* Get 32 bit return value of R7 resp */
				}

				if ((ocr[2] == 0x01) && (ocr[3] == 0xAA))	/* Is the card supports vcc of 2.7-3.6V? */
				{
					while ((!delay_has_expired())
							&& sd_spi_send_cmd(ACMD41, 1UL << 30)) ;	/* Wait for end of initialization with ACMD41(HCS) */

					if ( (!delay_has_expired())
							&& sd_spi_send_cmd(CMD58, 0) == 0)
					{		/* Check CCS bit in the OCR */
						for (n = 0; n < 4; n++)
						{
							ocr[n] = spi_exchange(0xFF);
						}
						ty = (ocr[0] & 0x40) ? CT_SDC2 | CT_BLOCK : CT_SDC2;	/* Card id SDv2 */
					}
				}
			}
			else
			{	/* Not SDv2 card */
				if (sd_spi_send_cmd(ACMD41, 0) <= 1)
				{	/* SDv1 or MMC? */
					ty = CT_SDC1;
					cmd = ACMD41;	/* SDv1 (ACMD41(0)) */
				}
				else
				{
					ty = CT_MMC3;
					cmd = CMD1;			/* MMCv3 (CMD1(0)) */
				}
				while ((!delay_has_expired())
						&& sd_spi_send_cmd(cmd, 0)) ;		/* Wait for end of initialization */

				if (delay_has_expired()
						|| sd_spi_send_cmd(CMD16, 0x200) != 0)	/* Set block length: 512 */
				{
					ty = 0;
				}
			}

		}

		_media_type = ty;	/* Card type */
		sd_spi_deselect();

		if (ty)
		{			/* OK */
			_stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
		}
		else
		{			/* Failed */
			_stat = STA_NOINIT;
		}
	}



	return _stat;
}

/**
 *
 * @param pdrv
 * @return
 */
DSTATUS disk_status (BYTE pdrv)
{
	if (pdrv > 0)
	{
		return STA_NOINIT;
	}

	return _stat;
}

/**
 *
 * @param pdrv		Physical drive number (0)
 * @param buff		Pointer to the data buffer to store read data
 * @param sector	Start sector number (LBA)
 * @param count		Number of sectors to read (1..128)
 * @return
 */
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	DSTATUS result = 0;

	DWORD sect = (DWORD)sector;

	if (pdrv || !count)		/* Check parameter */
	{
		return RES_PARERR;
	}

	if (_stat & STA_NOINIT)	/* Check if drive is ready */
	{
		return RES_NOTRDY;
	}

	if (!(_media_type & CT_BLOCK))
	{
		sect *= 512;	/* LBA ot BA conversion (byte addressing cards) */
	}

	if (count == 1)
	{	/* Single sector read */
		if ((sd_spi_send_cmd(CMD17, sect) == 0)	/* READ_SINGLE_BLOCK */
			&& sd_spi_receive_datablock(buff, 512))
		{
			count = 0;
		}
	}
	else
	{				/* Multiple sector read */
		if (sd_spi_send_cmd(CMD18, sect) == 0)
		{	/* READ_MULTIPLE_BLOCK */
			do
			{
				if (!sd_spi_receive_datablock(buff, 512))
				{
					break;
				}

				buff += 512;
			} while (--count);

			sd_spi_send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}

	sd_spi_deselect();

	return result;
}

#if FF_FS_READONLY == 0
/**
 *
 * @param pdrv		Physical drive number (0)
 * @param buff		Pointer to the data to write
 * @param sector	Start sector number (LBA)
 * @param count
 * @return
 */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	DWORD sect = (DWORD)sector;

	if (pdrv || !count)
	{
		return RES_PARERR;		/* Check parameter */
	}

	if (_stat)
	{
		return RES_NOTRDY;	/* Check drive status */
	}

	if (_stat & STA_PROTECT)
	{
		return RES_WRPRT;	/* Check write protect */
	}

	if (!(_media_type & CT_BLOCK))
	{
		sect *= 512;	/* LBA ==> BA conversion (byte addressing cards) */
	}

	if (count == 1)
	{	/* Single sector write */
		if ((sd_spi_send_cmd(CMD24, sect) == 0)	/* WRITE_BLOCK */
			&& sd_spi_sends_datablock((BYTE*)buff, 0xFE))
		{
			count = 0;
		}
	}
	else
	{				/* Multiple sector write */
		if (_media_type & CT_SDC)
		{
			sd_spi_send_cmd(ACMD23, count);	/* Predefine number of sectors */
		}

		if (sd_spi_send_cmd(CMD25, sect) == 0)
		{	/* WRITE_MULTIPLE_BLOCK */
			do
			{
				if (!sd_spi_sends_datablock(buff, 0xFC))
					break;
				buff += 512;
			} while (--count);

			if (!sd_spi_sends_datablock(0, 0xFD))
			{
				count = 1;	/* STOP_TRAN token */
			}
		}
	}

	sd_spi_deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}
#endif

/**
 *
 * @param pdrv
 * @param cmd
 * @param buff
 * @return
 */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	DSTATUS result = RES_ERROR;	/* */

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
 * @return 1 Ready, 0 timeout
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

/**
 *
 * @param buff	Data buffer
 * @param btr	Data block length (byte)
 * @return		1:OK, 0:Error
 */
static bool sd_spi_receive_datablock(BYTE *buff, UINT btr)
{
	bool received = false;

	BYTE token;

	delay_load_ms(200);				/* Wait for DataStart token in timeout of 200ms */
	do
	{
		token = spi_exchange(0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF)
			&& (!delay_has_expired()));

	if(token == 0xFE)
	{
		spi_multiple_write(buff, btr);		/* Store trailing data to the buffer */
		spi_exchange(0xFF);
		spi_exchange(0xFF);					/* Discard CRC */

		received = true;
	}

	return received;							/* Function succeeded */
}

#if FF_FS_READONLY == 0
/**
 *
 * @param buff		Ponter to 512 byte data to be sent
 * @param token		Token
 * @return			1:OK, 0:Failed
 */
static bool sd_spi_sends_datablock(const BYTE *buff, BYTE token)
{
	bool sent = false;
	BYTE resp;

	if (sd_spi_wait_ready(500))
	{
		spi_exchange(token);					/* Send token */

		if (token != 0xFD)
		{				/* Send data if token is other than StopTran */
			spi_multiple_write(buff, 512u);		/* Data */
			spi_exchange(0xFF);
			spi_exchange(0xFF);	/* Dummy CRC */

			resp = spi_exchange(0xFF);				/* Receive data resp */
			if ((resp & 0x1F) == 0x05)
			{
				sent = true;
			}
		}
	}

	return sent;
}
#endif
