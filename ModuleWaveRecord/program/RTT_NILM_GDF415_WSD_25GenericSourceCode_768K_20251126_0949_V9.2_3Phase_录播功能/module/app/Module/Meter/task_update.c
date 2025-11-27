/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjs
 *日  期：   2021年5月
 *修改人：
 *日  期：
 *描  述：
 *
 *版  本：
 *说  明:    升级程序
 *
 */
/*****************************************************************************/
#include "AllHead.h"
#include "task_update.h"
#include "M24512.h"
extern BYTE eraseflashflag[2];//擦除flash的标志，分别是sid和sid_mac
BOOL bIsVipUpdateFlag = FALSE;

void api_ResetCpu(void)
{
#if(!WIN32)
    __set_FAULTMASK(1);

    NVIC_SystemReset();

#endif
}
/**
 * @brief 设置缓冲区长度
 * 
 * @param ID    缓冲区ID
 * @param Len   缓冲区长度
 */
void api_SetCodeBufferLen(BYTE ID, DWORD Len)
{
    TUpdatePara tempUpdatePara;

    if(ID >= 2)
    {
        return;
    }

    if(Len > CODEBUFFER_SIZE)
    {
        return;
    }

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    tempUpdatePara.CodeBufferLen[ID] = Len;

    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);
}

/**
 * @brief 获取缓冲区长度
 * 
 * @param ID        缓冲区ID
 * @return DWORD    缓冲区长度
 */
DWORD api_GetCodeBufferLen(BYTE ID)
{
    TUpdatePara tempUpdatePara;
    DWORD Len;

    if(ID >= 2)
    {
        return 0;
    }

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    Len = tempUpdatePara.CodeBufferLen[ID];

    if(Len > CODEBUFFER_SIZE)
    {
        Len = 0;
    }

    return Len;
}

/**
 * @brief 写入缓冲区校验和
 * 
 * @param ID    缓冲区ID
 * @param Sum   校验和
 */
void api_SetCodeBufferSum(BYTE ID, WORD Sum)
{
    TUpdatePara tempUpdatePara;

    if(ID >= 2)
    {
        return;
    }

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    tempUpdatePara.CodeBufferSum[ID] = Sum;

    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);
}

/**
 * @brief 获取缓冲区校验和
 * 
 * @param ID        缓冲区ID
 * @return WORD     校验和
 */
WORD api_GetCodeBufferSum(BYTE ID)
{
    TUpdatePara tempUpdatePara;

    if(ID >= 2)
    {
        return 0;
    }

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    return tempUpdatePara.CodeBufferSum[ID];
}

/**
 * @brief 设置升级标志
 * 
 * @param state TRUE:置位升级标志 FALSE:清除升级标志
 */
void api_SetUpdateFlag(BOOL state)
{
    TUpdatePara tempUpdatePara;

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    if(state)
    {
        tempUpdatePara.UpdateFlag = UPDATEFLAG_CONST;
    }
    else
    {
        tempUpdatePara.UpdateFlag = 0x00000000;
    }

    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);
}

/**
 * @brief 获取升级标志
 * 
 * @return BOOL TRUE:需要升级 FALSE:不需要升级
 */
BOOL api_GetUpdateFlag(void)
{
    TUpdatePara tempUpdatePara;

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    if(tempUpdatePara.UpdateFlag == UPDATEFLAG_CONST)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief 设置缓冲区标志
 * 
 * @param type  0-正式缓冲区 1-临时缓冲区
 * @param ID    0-使用第1缓冲区 1-使用第2缓冲区
 */
void api_SetCodeBufferFlag(BYTE type, BYTE ID)
{
    TUpdatePara tempUpdatePara;

    if(type >= 2)
    {
        return;
    }

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    if(ID == 0)
    {
        tempUpdatePara.CodeBufferFlag[type] = CODEBUFFERFLAG1_CONST;
    }
    else if(ID == 1)
    {
        tempUpdatePara.CodeBufferFlag[type] = CODEBUFFERFLAG2_CONST;
    }
    else
    {
        return;
    }

    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);
}

/**
 * @brief 获取缓冲区标志
 * 
 * @param type  0-正式缓冲区 1-临时缓冲区
 * @return BYTE 0-正在使用第1缓冲区 1-正在使用第2缓冲区
 */
BYTE api_GetCodeBufferFlag(BYTE type)
{
    TUpdatePara tempUpdatePara;

    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    if(tempUpdatePara.CodeBufferFlag[type] == CODEBUFFERFLAG1_CONST)
    {
        return 0;
    }
    else if(tempUpdatePara.CodeBufferFlag[type] == CODEBUFFERFLAG2_CONST)
    {
        return 1;
    }

    return 0xff;
}

BOOL bIsBoot=FALSE;
BYTE* pBootBuf;

BYTE api_GetCodeNextBufferFlag(BYTE type)
{
    TUpdatePara tempUpdatePara;

    if(bIsVipUpdateFlag == TRUE)
		return 2;
	
    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

    if (tempUpdatePara.CodeBufferFlag[type] == CODEBUFFERFLAG1_CONST)
    {
        return 1;
    }
    else if (tempUpdatePara.CodeBufferFlag[type] == CODEBUFFERFLAG2_CONST)
    {
        return 0;
    }

    return 0;//0xff;
}

/**
 * @brief 写数据到FLASH程序缓冲
 * 
 * @param ID        缓冲区ID
 * @param Offset    地址偏移
 * @param Len       数据长度
 * @param pBuf      数据指针
 * @return BOOL     TRUE:写成功 FALSE:写失败
 */
BOOL api_WriteDataToCodeBuffer(BYTE ID, DWORD Offset, WORD Len, BYTE *pBuf)
{
    DWORD Addr;

    if(Len == 0)
    {
        return FALSE;
    }

    if((Offset + Len) > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

    if(ID == 0)
    {
        Addr = FLASH_CODEBUFFER1_START_ADDR;
    }
    else if(ID == 1)
    {
        Addr = FLASH_CODEBUFFER2_START_ADDR;
    }
    else if(ID == 2)
    {
       Addr = FLASH_CODEBUFFER3_START_ADDR;
    }
    else
    {
        return FALSE;
    }

	#if(USER_TEST_MODE)
	{
		if(eraseflashflag[0] == 0x0D)//sid = 0x0D
		{
			// 地址取整
			TrueAddr = ((Addr+Offset) / SECTOR_SIZE) * SECTOR_SIZE;
			if( EraseExtFlashSector( CS_SPI_FLASH, TrueAddr ) != TRUE )//擦除失败由重试机制进行重试
			{
				return FALSE;//返回失败
			}
			eraseflashflag[0] = 0;
		}
		else if(eraseflashflag[1] == 0xDA)//sid_mac = 0xDA
		{
			// 地址取整
			TrueAddr = ((Addr+Offset) / SECTOR_SIZE) * SECTOR_SIZE;
			if( EraseExtFlashSector( CS_SPI_FLASH, TrueAddr ) != TRUE )//擦除失败由重试机制进行重试
			{
				return FALSE;//返回失败
			}
			eraseflashflag[1] = 0;
		}
	}
    #endif

    return api_WriteMemRecordData(Addr + Offset, Len, pBuf);

//WriteDataToCode(Offset, Len, pBuf);
}

BOOL api_ReadDataFromCodeBuffer(BYTE ID, DWORD Offset, WORD Len, BYTE *pBuf)
{
    DWORD Addr;

    if (Len == 0)
    {
        return FALSE;
    }

    if ((Offset + Len) > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

    if (ID == 0)
    {
        Addr = FLASH_CODEBUFFER1_START_ADDR;
    }
    else if (ID == 1)
    {
        Addr = FLASH_CODEBUFFER2_START_ADDR;
    }
	else if(ID == 2)
    {
       Addr = FLASH_CODEBUFFER3_START_ADDR;
    }
    else
    {
        return FALSE;
    }

    return api_ReadMemRecordData(Addr + Offset, Len, pBuf);

    //return ReadDataToCode(Offset, Len, pBuf);
}

/**
 * @brief 计算FLASH中程序缓冲区累加和
 * 
 * @param ID        缓冲区ID
 * @return DWORD    累加和
 */
WORD api_CalcCodeBufferSum(BYTE ID)
{
    DWORD i, j, buflen, rlen, Addr;
    WORD Sum;
    WORD tempbuffer[2048];

    buflen = api_GetCodeBufferLen(ID);

    if(buflen > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

    if(ID == 0)
    {
        Addr = FLASH_CODEBUFFER1_START_ADDR;
    }
    else if(ID == 1)
    {
        Addr = FLASH_CODEBUFFER2_START_ADDR;
    }
    else
    {
        return FALSE;
    }

    Sum = 0;
    for(i=0;i<buflen;i+=rlen)
    {
        CLEAR_WATCH_DOG;

        rlen = buflen - i;
        if(rlen > sizeof(tempbuffer))
        {
            rlen = sizeof(tempbuffer);
        }

        if(api_ReadMemRecordData(Addr + i, rlen, (BYTE *)tempbuffer) == TRUE)
        {
            for(j=0;j<rlen/2;j++)
            {
                Sum += tempbuffer[j];
            }
        }
        else
        {
            return 0;
        }
    }

    return Sum;
}

/**
 * @brief 判断FLASH中程序校验码是否正确
 * 
 * @param ID        缓冲区ID，0-第1缓冲区，1-第2缓冲区
 * @return BOOL     TRUE:校验正确 FALSE:校验失败
 */
BOOL api_CheckCodeBufferSum(BYTE ID)
{
    if(ID >= 2)
    {
        return FALSE;
    }

    if(api_CalcCodeBufferSum(ID) == api_GetCodeBufferSum(ID))
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief 检查RAM中程序校验码是否正确
 * 
 * @param ID    缓冲区ID
 * @return BOOL TRUE:校验正确 FALSE:校验失败
 */
BOOL api_CheckRAMCodeSum(BYTE ID)
{
	WORD Sum;
    DWORD StartAddr, EndAddr;
    WORD *pData;

    Sum = 0;
    StartAddr = RAMCODE_START_ADDR;
    EndAddr = StartAddr + api_GetCodeBufferLen(ID);
    if(EndAddr > RAMCODE_END_ADDR)
    {
        return FALSE;
    }

    for(pData=(WORD *)StartAddr;pData<(WORD *)EndAddr;pData++)
    {
        Sum += *pData;
    }
    
    if(Sum == api_GetCodeBufferSum(ID))
    {
        return TRUE;
    }

    return FALSE;

}

/**
 * @brief 复制程序缓冲区数据到RAM
 * 
 * @param ID    缓冲区ID
 * @return BOOL TRUE:复制正确 FALSE:复制失败
 */
BOOL api_CopyCodeBufferToRAM(BYTE ID)
{
    DWORD i, buflen, rlen, FlashAddr, RamAddr;
    BYTE tempbuffer[4096];
	
	buflen = api_GetCodeBufferLen(ID);

    if(buflen > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

    if(ID == 0)
    {
        FlashAddr = FLASH_CODEBUFFER1_START_ADDR;
    }
    else if(ID == 1)
    {
        FlashAddr = FLASH_CODEBUFFER2_START_ADDR;
    }
    else
    {
        return FALSE;
    }
    RamAddr = RAMCODE_START_ADDR;

    for(i=0;i<buflen;i+=rlen)
    {
        rlen = buflen - i;
        if(rlen > sizeof(tempbuffer))
        {
            rlen = sizeof(tempbuffer);
        }

        if(api_ReadMemRecordData(FlashAddr + i, rlen, tempbuffer) == TRUE)
        {
            memcpy((void *)(RamAddr + i), tempbuffer, rlen);
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

#define USE_UPFILE_MEMSAVE		1


#define UPFILE_MAGIC			0x55665A6B

extern TFileInfo tFileInfo;
BOOL ReadUpFileStatus(TFileInfo* tFile)
{
#if (USE_UPFILE_MEMSAVE)
WORD wResult;
	
	wResult = ReadEEPRom2(UPFILE_DATA_ADDR, sizeof(TFileInfo), (BYTE*)tFile);

	if(wResult == TRUE)
	{
		if(tFile->dwMagic == UPFILE_MAGIC)
		{
			if( (fcs16((BYTE*)&tFile->dwMagic,sizeof(TFileInfo)-2) == tFile->wCrc) )
				return TRUE;
		}
				
	}
#endif
	return FALSE;

}

BOOL WriteUpFileStatus(void)
{
#if (USE_UPFILE_MEMSAVE)
	
	tFileInfo.dwMagic = UPFILE_MAGIC;

	tFileInfo.wCrc = fcs16((BYTE*)&tFileInfo.dwMagic,sizeof(TFileInfo)-2);
	
	return WriteEEPRom2(UPFILE_DATA_ADDR, sizeof(TFileInfo),(BYTE*)&tFileInfo);
#else
	return FALSE;
#endif
}

void InitUpFileStatus(void)
{
	TFileInfo upfile;

#if (USE_UPFILE_MEMSAVE)
	if(ReadUpFileStatus(&upfile))
	{
		if(upfile.dwUpFlag == 0xAA)//UPFILE_MAGIC
		{
			api_WriteMeterTime(&upfile.ResetTime);
			memcpy(&tFileInfo, &upfile, sizeof(TFileInfo) );
			
			api_UpdateAppEventEnd();
			api_SetUpdateFlag(FALSE);

			tFileInfo.dwUpFlag = 0;
			WriteUpFileStatus();
		}
	}
#endif

}
	

BOOL WriteUpFileReset(void)
{

#if (USE_UPFILE_MEMSAVE)
	TRealTimer time;
	DWORD dwTimeSec;
			
	tFileInfo.dwMagic = UPFILE_MAGIC;

	get_sys_time(&time);

	dwTimeSec = api_CalcInTimeRelativeSec(&time);
	dwTimeSec += 10;//升级前后差10s
			
	api_SecToAbsTime(dwTimeSec, &time);

	memcpy(&tFileInfo.ResetTime,&time,sizeof(TRealTimer));
	tFileInfo.FileStatus = 6;
	tFileInfo.dwUpFlag = 0xAA;//UPFILE_MAGIC;

	tFileInfo.wCrc = fcs16((BYTE*)&tFileInfo.dwMagic,sizeof(TFileInfo)-2);
	
	return WriteEEPRom2(UPFILE_DATA_ADDR, sizeof(TFileInfo),(BYTE*)&tFileInfo);
#else
	return FALSE;
#endif
}

BOOL CompareUpFile(TFileInfo* tFile)
{
	TFileInfo upfile;

#if (USE_UPFILE_MEMSAVE)

	if(ReadUpFileStatus(&upfile))
	{
		if(memcmp(&upfile.sourseFile, &tFile->sourseFile, 185) )
			return FALSE;

		memcpy(&tFileInfo, &upfile, sizeof(TFileInfo) );
		return TRUE;
	}
#endif

	return FALSE;
}

void writeBoot(void)
{
//	int i;
/*	
	FLASH_MassErase(FLASH_VOLTAGE_RANGE_1,FLASH_BANK_1);
	
	  HAL_FLASH_Unlock();
		for(i=0;i<50*1024; i+=4)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, 0x8000000, (DWORD)pBootBuf[i]);
		}
		
    HAL_FLASH_Lock();
	*/
}






//////////////////////////////////
// 标识头		个数	第一个序号	偏移量		长度	第二个序号	偏移量	长度	... 总计长度50	
// A1 B2 C3 D4   05		01			0x80100		256		02
BOOL IsVipUserUpdate(BYTE* pBuf, WORD wLen)
{
	if(pBuf == NULL)
		return FALSE;

	if(wLen < 50)
		return FALSE;

	if((pBuf[0] == 0xA1) && (pBuf[1] == 0xB2) && (pBuf[2] == 0xC3) && (pBuf[3] == 0xD4))
	{
		if(pBuf[4] > 0)
			return TRUE;
	}

	return FALSE;

}




BOOL api_CopyFlash0ToFlash1(BYTE From, BYTE To)
{
    DWORD i, buflen, rlen, FlashAddr, AppAddr;
    BYTE tempbuffer[4096];
    TUpdatePara tempUpdatePara;


    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

/*	if(tempUpdatePara.CodeBufferLen[To] != 0 && tempUpdatePara.CodeBufferLen[To] != 0xFFFFFFFF
		&& tempUpdatePara.CodeBufferSum[To] != 0 && tempUpdatePara.CodeBufferSum[To] != 0xFFFFFFFF)
		return FALSE;
*/

    buflen = api_GetCodeBufferLen(From);

    if(buflen > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

	if(From == 0)
	{
		AppAddr = FLASH_CODEBUFFER1_START_ADDR;
	}
	else if(From == 1)
	{
		AppAddr = FLASH_CODEBUFFER2_START_ADDR;
	}
        else
        {
          return FALSE;
        }
	if(To == 0)
	{
		FlashAddr = FLASH_CODEBUFFER1_START_ADDR;
	}
	else if(To == 1)
	{
		FlashAddr = FLASH_CODEBUFFER2_START_ADDR;
	}
        else
          return FALSE;
    
    for(i=0;i<buflen;i+=rlen)
    {
		CLEAR_WATCH_DOG;
			
        rlen = buflen - i;
        if(rlen > sizeof(tempbuffer))
        {
            rlen = sizeof(tempbuffer);
        }

        if(api_ReadMemRecordData((AppAddr + i), rlen, tempbuffer))
        {
			api_WriteMemRecordData(FlashAddr + i, rlen, tempbuffer);
           // memcpy((void *)(AppAddr + i), tempbuffer, rlen);
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



#define DEBUG_UP_FILE  0

#define SECTOR_LEN 4096

#define HEAD_LEN	9 //4头 2版本 2校验 1个数

BYTE BufferIn[SECTOR_LEN], BufferCode[SECTOR_LEN];

BOOL WriteVipUpdateData(BYTE* pBuf, WORD wLen)
{
	int i, nLen,buflen;
	int nNum;
//	int nDataType;
	// BYTE byNo;
	DWORD dwAddr,dwOffset, dwLength, dwAddrTmp, dwDataLen;
	BYTE Buffer[SECTOR_LEN], BufferCode[SECTOR_LEN];
	BYTE BufferFlag[256];
//	TUpdatePara tempUpdatePara;
	WORD wData;
	BYTE byHeadLen;
	BYTE* pbyData;
	BYTE byCodeFlagR, byCodeFlagW;
	BOOL bResult;
        
	if(bIsVipUpdateFlag == 0)
		return FALSE;

	bIsVipUpdateFlag = 0;

        api_ReadDataFromCodeBuffer(2, 0, (HEAD_LEN+10*9), &BufferFlag[0]);

	wData = api_CheckCpuFlashSum(0x01);

	if((BufferFlag[0] != 0xA1) || (BufferFlag[1] != 0xB2) && (BufferFlag[2] != 0xC3) || (BufferFlag[3] != 0xD4))
	{
		return FALSE;
	}
	if(MW(BufferFlag[6],BufferFlag[7]) != wData)
	{
		return FALSE;
	}

	byCodeFlagR = api_GetCodeNextBufferFlag(0);
	byCodeFlagW = (byCodeFlagR==1)?0:1;
        
        api_CopyFlash0ToFlash1(byCodeFlagW, byCodeFlagR);
                
	nNum = BufferFlag[HEAD_LEN-1];

	nLen = HEAD_LEN;
	dwOffset = 0;
	byHeadLen = (HEAD_LEN+10*9);
	for(i=0; i<nNum; i++)
	{
		// byNo = BufferFlag[nLen++];
		nLen++;
		dwAddr = MDW(BufferFlag[nLen], BufferFlag[nLen+1], BufferFlag[nLen+2], BufferFlag[nLen+3]);
		nLen += 4;

		dwLength = MDW(BufferFlag[nLen], BufferFlag[nLen+1], BufferFlag[nLen+2], BufferFlag[nLen+3]);
		nLen += 4;

		if(dwLength > 256*1024)
			return FALSE;

		while(dwLength)
		{
                        CLEAR_WATCH_DOG;
			if(dwLength > SECTOR_LEN)
			{
				dwDataLen = (SECTOR_LEN - (dwAddr%SECTOR_LEN));
				dwLength -= dwDataLen;
			}
			else
			{
				dwDataLen = dwLength;
				dwLength = 0;
			}

			//读出指定区域的代码写到代码区
		
			api_ReadDataFromCodeBuffer(2, dwOffset+byHeadLen, dwDataLen, &BufferIn[0]);
		
			dwOffset += dwDataLen;

			dwAddrTmp = (dwAddr / SECTOR_LEN);
						
        
			if( api_ReadDataFromCodeBuffer(byCodeFlagR, dwAddrTmp*SECTOR_LEN, SECTOR_LEN, BufferCode) )
			{
				memcpy(&BufferCode[dwAddr % SECTOR_LEN], &BufferIn[0], dwDataLen);
				bResult = api_WriteDataToCodeBuffer(byCodeFlagR, dwAddrTmp*SECTOR_LEN, SECTOR_LEN, BufferCode);
                                if(bResult == FALSE)
                                {
                                   //     bResult = WriteExtFlash2(0, dwAddrTmp*SECTOR_LEN, SECTOR_LEN,BufferCode );
                                }
			}
		
			dwAddr += dwDataLen;

		}
		
	}
    api_ReadDataFromCodeBuffer(2, dwOffset+byHeadLen-40, 40, &Buffer[0]);

	nLen = 0;


	pbyData = ((BYTE *)Buffer);
    
    while(nLen < SECTOR_LEN )
	{
		if((pbyData[0] == 0x68) && (pbyData[1] == 0x55) && (pbyData[2] == 0xAA) && (pbyData[3] == 0x68))
		{
			buflen = (DWORD)((pbyData[11]<<24)+(pbyData[10]<<16)+(pbyData[9]<<8)+(pbyData[8]));
			break;
		}
		nLen++;
		pbyData++;
	}

	tFileInfo.lFileSize = buflen+40;

	return TRUE;
	
}

