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
#include "waveRecord.h"
#include "sd_card.h"
#include "__def.h"
#include "RTC_API.h"
#include "SDIO_MSD_Driver.h"
// #include "rtc.h"

/* Private variables ---------------------------------------------------------*/

extern void ToggleTransferLED(void);
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

	int Status;
	if (!count)
	{
		return RES_PARERR; /* count���ܵ���0�����򷵻ز������� */
	}

	switch (drv)
	{

	case 0:
		Status = SDCARD_ReadBlocks(&stcSdhandle, sector, count, (uint8_t *)buff, 20000u);
		if (Status == Ok)
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
	int  Status;
	if (!count)
	{
		return RES_PARERR; /* count���ܵ���0�����򷵻ز������� */
	}

	switch (drv)
	{
	case 0:
		Status = SDCARD_WriteBlocks(&stcSdhandle, sector, count, (uint8_t *)buff, 2000u);
		if (Status == Ok)
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
		return RES_PARERR; /* ��֧�ֵ����̲��������򷵻ز������� */
	}

	switch (ctrl)
	{
	case GET_SECTOR_SIZE:
		return RES_OK;

	case CTRL_SYNC:

		return RES_OK;

	case GET_SECTOR_COUNT:
		// *(DWORD *)buff =stcSdhandle.stcSdCardInfo. / CardInfo.BlockSize
        *(DWORD *)buff = stcSdhandle.stcSdCardInfo.u32LogBlockNbr;
		return RES_OK;

	case GET_BLOCK_SIZE:
		*(WORD *)buff = stcSdhandle.stcSdCardInfo.u32BlockSize;
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

/* �õ��ļ�Calendar��ʽ�Ľ�������,��DWORD get_fattime (void) ��任 */
/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
DWORD get_fattime(void) // �ú�������ΰ��efsl����ֲ��2012.2.15--------------------------
{
#ifdef DATE_TIME_SUPPORT

	DWORD dateTime;
	// BYTE date_time[6] = {0};

	// api_GetRtcDateTime(DATETIME_YYMMDDhhmmss, date_time);
	dateTime = RealTimer.wYear - 1980;	// �������
	dateTime = dateTime << 25;		// ��λ����25λ
	dateTime |= (RealTimer.Mon << 21);	// ���·���λ���ϲ�
	dateTime |= (RealTimer.Day << 16);	// ��������λ���ϲ�
	dateTime |= (RealTimer.Hour << 11); // ��Сʱ��λ���ϲ�
	dateTime |= (RealTimer.Min << 5);	// ��������λ���ϲ�
	dateTime |= (RealTimer.Sec / 2);

	return dateTime;

#else
	return 0;
#endif
}

/*********************************************************************************************************
	  END FILE
*********************************************************************************************************/
