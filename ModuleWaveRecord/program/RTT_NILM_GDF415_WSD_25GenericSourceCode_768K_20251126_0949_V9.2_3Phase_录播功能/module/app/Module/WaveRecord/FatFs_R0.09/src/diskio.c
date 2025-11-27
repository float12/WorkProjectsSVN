/****************************************Copyright (c)****************************************************
**
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               diskio.c
** Descriptions:            The FATFS Diskio
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-4
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"
#include "sys.h"
#include "waveRecord.h"
#include "dbase.h"
#include "dsp_time.h"
#include "sdcard.h"
#include "__def.h"
#include "SDIO_MSD_Driver.h"
// #include "rtc.h"

/* Private variables ---------------------------------------------------------*/


DSTATUS disk_initialize(
	BYTE drv /* Physical drive nmuber (0..) */
)
{
	int Status;

	switch (drv)
	{
	case 0:
		Status = MSD_Init();
		if (Status == 0)
			return 0;
		else
			return STA_NOINIT;

	case 1:
		return STA_NOINIT;

	case 2:
		return STA_NOINIT;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status(
	BYTE drv /* Physical drive nmuber (0..) */
)
{
	switch (drv)
	{
	case 0:

		/* translate the reslut code here	*/

		return 0;

	case 1:

		/* translate the reslut code here	*/

		return 0;

	case 2:

		/* translate the reslut code here	*/

		return 0;

	default:

		break;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read(
	BYTE drv,	  /* Physical drive nmuber (0..) */
	BYTE *buff,	  /* Data buffer to store read data */
	DWORD sector, /* Sector address (LBA) */
	BYTE count	  /* Number of sectors to read (1..255) */
)
{

	sd_error_enum Status;
	if (!count)
	{
		return RES_PARERR; /* count不能等于0，否则返回参数错误 */
	}

	switch (drv)
	{

	case 0:
		Status = sd_multiblocks_read((uint32_t *)buff,sector,512,count);
		if (Status == SD_OK)
		{
			return RES_OK;
		}
		else
		{
			return RES_ERROR;
		}

	case 1:
		break;

	case 2:
		break;

	default:
		break;
	}

	return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write(
	BYTE drv,		  /* Physical drive nmuber (0..) */
	const BYTE *buff, /* Data to be written */
	DWORD sector,	  /* Sector address (LBA) */
	BYTE count		  /* Number of sectors to write (1..255) */
)
{
	sd_error_enum  Status;
	if (!count)
	{
		return RES_PARERR; /* count不能等于0，否则返回参数错误 */
	}

	switch (drv)
	{
	case 0:
		Status = sd_multiblocks_write((uint32_t *)buff,sector,512,count);
		if (Status == SD_OK)
		{
			return RES_OK;
		}
		else
		{
			return RES_ERROR;
		}
	case 2:
		break;
	default:
		break;
	}
	return RES_ERROR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl(
	BYTE drv,  /* Physical drive nmuber (0..) */
	BYTE ctrl, /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{
	if (drv)
	{
		return RES_PARERR; /* 仅支持单磁盘操作，否则返回参数错误 */
	}

	switch (ctrl)
	{
	case GET_SECTOR_SIZE:
		return RES_OK;

	case CTRL_SYNC:

		return RES_OK;

	case GET_SECTOR_COUNT:
		// *(DWORD *)buff =stcSdhandle.stcSdCardInfo. / CardInfo.BlockSize
        *(DWORD *)buff = sd_card_capacity_get() * 1024/ 512;
		return RES_OK;

	case GET_BLOCK_SIZE:
		*(WORD *)buff = 512;
		return RES_OK;

	case CTRL_POWER:
		break;

	case CTRL_LOCK:
		break;

	case CTRL_EJECT:
		break;

	/* MMC/SDC command */
	case MMC_GET_TYPE:
		break;

	case MMC_GET_CSD:
		break;

	case MMC_GET_CID:
		break;

	case MMC_GET_OCR:
		break;

	case MMC_GET_SDSTAT:
		break;
	}
	return RES_PARERR;
}

/* 得到文件Calendar格式的建立日期,是DWORD get_fattime (void) 逆变换 */
/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
DWORD get_fattime(void) // 该函数是王伟从efsl中移植的2012.2.15--------------------------
{
#ifdef DATE_TIME_SUPPORT

	DWORD dateTime;
	// BYTE date_time[6] = {0};

	// api_GetRtcDateTime(DATETIME_YYMMDDhhmmss, date_time);
	dateTime = RealTimer.wYear - 1980;	// 计算年差
	dateTime = dateTime << 25;		// 移位到高25位
	dateTime |= (RealTimer.Mon << 21);	// 将月份移位并合并
	dateTime |= (RealTimer.Day << 16);	// 将日期移位并合并
	dateTime |= (RealTimer.Hour << 11); // 将小时移位并合并
	dateTime |= (RealTimer.Min << 5);	// 将分钟移位并合并
	dateTime |= (RealTimer.Sec / 2);

	return dateTime;

#else
	return 0;
#endif
}

/*********************************************************************************************************
	  END FILE
*********************************************************************************************************/
