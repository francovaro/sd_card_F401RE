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

/**
 *
 * @param pdrv
 * @return
 */
DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS result = 0;

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
