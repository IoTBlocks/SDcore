/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/*
 * File	: diskio.c
 * This file has been modified as part of the IoTBLocks project
 * Find out more:
 * http://www.pratikpanda.com/iotblocks/SDcore
 */

#include "diskio.h"		/* FatFs lower layer API */
#include "sdcard.h"		/* Header file of existing MMC/SDC contorl module */

// NOTE: pdrv parameter is ignored in all functions.
// It is not required for interfacing to a single
// storage device using FatFs.

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (BYTE pdrv)
{
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                     */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (BYTE pdrv)
{
	int result;
// initialize_sd() returns 0 for success
// -1 for time-out during initialization
// and returns 2 when card is not responding as expected
result = initialize_sd();

if (result==0)
	return 0;
else if (result == -1)
	return STA_NODISK;
else
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	int result;

	result = read_blocks_sd(buff, sector, count);

	if (result == 0)
		return RES_OK;
	else if (result == 1)
		return RES_ERROR;
	else	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
	int result;

	result = write_blocks_sd(buff, sector, count);

	if (result == 0)
		return RES_OK;
	else if (result == 1)
		return RES_ERROR;
	else	return RES_PARERR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
// NOTE: The following has NOT been implemented for SDcore
// DO NOT use any function that calls the following code

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	//case ATA :

		// Process of the command for the ATA drive

	//	return res;

	default:

		// Process of the command for the MMC/SD card

		return res;

	//case USB :

		// Process of the command the USB drive

//		return res;
	}

	return RES_PARERR;
}
#endif
