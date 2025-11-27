/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjs
 *日  期：   2wNo021年5月
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

#define FLASH_BOOT		0x0				//留用大小31K
#define FLASH_FLAG		0x08010000			//留用大小1K
#define FLASH_APP1		0x08010000			//留用大小112K
#define FLASH_APP2		0x38000  //0x24000         //留用大小112K

//#define CODEBUFFER_SIZE         (400*1024)

BYTE UpdateBuffer[2][UPDATEBUFFERLEN];
BYTE UpdateBuf_No = 0;
DWORD UpdateBuf_pos = 0;

DWORD UpdateCodeBufferID;
DWORD UpdateDataLen = 0;



void api_ResetCpu(void)
{
#if(!WIN32)
	__set_FAULTMASK(1);

	NVIC_SystemReset();

#endif
}

//-----------------------------------------------
//功能描述：获取地址所在的扇区
//
//参    数：addr[in],		地址
//
//返 回 值：扇区号
//
//备注内容：无
//-----------------------------------------------
uint32_t fmc_sector_get(uint32_t address)
{
    uint32_t sector = 0;
	
    if((address < ADDR_FMC_SECTOR_1) && (address >= ADDR_FMC_SECTOR_0)){
        sector = CTL_SECTOR_NUMBER_0;
    }else if((address < ADDR_FMC_SECTOR_2) && (address >= ADDR_FMC_SECTOR_1)){
        sector = CTL_SECTOR_NUMBER_1;
    }else if((address < ADDR_FMC_SECTOR_3) && (address >= ADDR_FMC_SECTOR_2)){
        sector = CTL_SECTOR_NUMBER_2;
    }else if((address < ADDR_FMC_SECTOR_4) && (address >= ADDR_FMC_SECTOR_3)){
        sector = CTL_SECTOR_NUMBER_3;  
    }else if((address < ADDR_FMC_SECTOR_5) && (address >= ADDR_FMC_SECTOR_4)){
        sector = CTL_SECTOR_NUMBER_4;
    }else if((address < ADDR_FMC_SECTOR_6) && (address >= ADDR_FMC_SECTOR_5)){
        sector = CTL_SECTOR_NUMBER_5;
    }else if((address < ADDR_FMC_SECTOR_7) && (address >= ADDR_FMC_SECTOR_6)){
        sector = CTL_SECTOR_NUMBER_6;
    }else if((address < ADDR_FMC_SECTOR_8) && (address >= ADDR_FMC_SECTOR_7)){
        sector = CTL_SECTOR_NUMBER_7;  
    }else if((address < ADDR_FMC_SECTOR_9) && (address >= ADDR_FMC_SECTOR_8)){
        sector = CTL_SECTOR_NUMBER_8;
    }else if((address < ADDR_FMC_SECTOR_10) && (address >= ADDR_FMC_SECTOR_9)){
        sector = CTL_SECTOR_NUMBER_9;
    }else if((address < ADDR_FMC_SECTOR_11) && (address >= ADDR_FMC_SECTOR_10)){
        sector = CTL_SECTOR_NUMBER_10;
    }else if((address < ADDR_FMC_SECTOR_12) && (address >= ADDR_FMC_SECTOR_11)){
        sector = CTL_SECTOR_NUMBER_11;
    }else if((address < ADDR_FMC_SECTOR_13) && (address >= ADDR_FMC_SECTOR_12)){
        sector = CTL_SECTOR_NUMBER_12;
    }else if((address < ADDR_FMC_SECTOR_14) && (address >= ADDR_FMC_SECTOR_13)){
        sector = CTL_SECTOR_NUMBER_13;
    }else if((address < ADDR_FMC_SECTOR_15) && (address >= ADDR_FMC_SECTOR_14)){
        sector = CTL_SECTOR_NUMBER_14;
    }else if((address < ADDR_FMC_SECTOR_16) && (address >= ADDR_FMC_SECTOR_15)){
        sector = CTL_SECTOR_NUMBER_15;
    }else if((address < ADDR_FMC_SECTOR_17) && (address >= ADDR_FMC_SECTOR_16)){
        sector = CTL_SECTOR_NUMBER_16;
    }else if((address < ADDR_FMC_SECTOR_18) && (address >= ADDR_FMC_SECTOR_17)){
        sector = CTL_SECTOR_NUMBER_17;
    }else if((address < ADDR_FMC_SECTOR_19) && (address >= ADDR_FMC_SECTOR_18)){
        sector = CTL_SECTOR_NUMBER_18;
    }else if((address < ADDR_FMC_SECTOR_20) && (address >= ADDR_FMC_SECTOR_19)){
        sector = CTL_SECTOR_NUMBER_19;
    }else if((address < ADDR_FMC_SECTOR_21) && (address >= ADDR_FMC_SECTOR_20)){
        sector = CTL_SECTOR_NUMBER_20;
    }else if((address < ADDR_FMC_SECTOR_22) && (address >= ADDR_FMC_SECTOR_21)){
        sector = CTL_SECTOR_NUMBER_21;
    }else if((address < ADDR_FMC_SECTOR_23) && (address >= ADDR_FMC_SECTOR_22)){
        sector = CTL_SECTOR_NUMBER_22;
    }else if((address < ADDR_FMC_SECTOR_24) && (address >= ADDR_FMC_SECTOR_23)){
        sector = CTL_SECTOR_NUMBER_23;
    }else if((address < ADDR_FMC_SECTOR_25) && (address >= ADDR_FMC_SECTOR_24)){
        sector = CTL_SECTOR_NUMBER_24;
    }else if((address < ADDR_FMC_SECTOR_26) && (address >= ADDR_FMC_SECTOR_25)){
        sector = CTL_SECTOR_NUMBER_25;
    }else if((address < ADDR_FMC_SECTOR_27) && (address >= ADDR_FMC_SECTOR_26)){
        sector = CTL_SECTOR_NUMBER_26;
    }else {
        sector = CTL_SECTOR_NUMBER_27;
    }	
		
    return sector;
}

//-----------------------------------------------
//功能描述：falsh读
//
//参    数：addr[in],		要读取的flash起始地址
//          *buf[out],		数据缓冲
//          size[in],       读取长度
//
//返 回 值：返回已读取的长度
//
//备注内容：无
//-----------------------------------------------
int scm_flash_read( uint32_t addr, uint8_t *buf, size_t size )
{
    size_t DataSize, RemainSize;

    if ((addr + size) > FLASH_ADDR_END)
    {
        return -1;
    }
	
    DataSize = size >> 2;
    RemainSize = size & 0x03;

    while (DataSize--)
    {
        *(uint32_t *)buf = *(uint32_t *)addr;
        buf += 4;
        addr += 4;
    }

    while (RemainSize--)
    {
        *buf = *(uint8_t *)addr;
        buf++;
        addr++;
    }

    return size;
}

//-----------------------------------------------
//功能描述：falsh写
//
//参    数：addr[in],		要写入的flash起始地址
//          *buf[out],		数据缓冲
//          size[in],       写入长度
//
//返 回 值：返回已写入的长度
//
//备注内容：无
//-----------------------------------------------
int scm_flash_write( uint32_t addr, const uint8_t *buf, size_t size )
{
	size_t cnt;

	if ((addr+size) > FLASH_ADDR_END)
    {
        return -1;
    }
	
	if (size == 0)
    {
        return size;
    }
    


    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    
	for (cnt=0; cnt<size; cnt++)
	{
        if (FMC_READY == fmc_byte_program(addr, buf[cnt]))
		{
            addr++;
        }
		else
		{ 
			return -1;
        }
    }
	
    fmc_lock();
	
	return cnt;
}

//-----------------------------------------------
//功能描述：falsh擦除
//
//参    数：addr[in],		要擦除的flash起始地址
//          size[in],       擦除长度
//
//返 回 值：执行结果
//
//备注内容：会擦除传入范围所在的扇区，不一定是传入的范围！
//-----------------------------------------------
int scm_flash_erase( uint32_t addr, size_t size )
{
	uint16_t StartSector, EndSector;
	uint16_t sector_idx;
	
	if ((addr+size) > FLASH_ADDR_END)
    {
        return -1;
    }
	
    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    
	StartSector = fmc_sector_get(addr);
    EndSector = fmc_sector_get(addr + size - 1);
    for (sector_idx=StartSector; sector_idx<=EndSector; sector_idx+=8) //+8 mean <<3
	{
		if(FMC_READY != fmc_sector_erase(sector_idx))
		{
			return -1;
		}
    }
	
	fmc_lock();
	
	return 1;
}


//api_VReadSafeMem， api_VWriteSafeMem
BOOL api_Flash_read_flag(DWORD Addr, WORD Length, BYTE *pBuf)
{
WORD wNo;

      return scm_flash_read(Addr, pBuf, Length);

       //HT_Flash_ByteRead(pBuf, (FLASH_FLAG + Addr), Length);
	return TRUE;
}

BOOL api_Flash_write_flag( DWORD Addr, WORD Length, BYTE * pBuf )
{
unsigned char Buffer[1024];

	WORD wNo;
        
       scm_flash_write(Addr,pBuf,Length);
       
        
        return TRUE;
}

BOOL api_Flash_read(DWORD Addr, WORD Length, BYTE *pBuf)
{
  WORD wNo;
	
     scm_flash_read(Addr, pBuf, Length);
	
	return TRUE;
}

BOOL api_Flash_write( DWORD Addr, WORD Length, BYTE * pBuf )
{
	WORD wNo;
        
        scm_flash_write(Addr,pBuf,Length);
	return TRUE;
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
 * @brief 初始化缓冲区
 * 
 */
void api_InitCodeBudder(void)
{
    UpdateBuf_No = 0;
    UpdateBuf_pos = 0;
    UpdateDataLen = 0;
    UpdateCodeBufferID = api_GetCodeBufferFlag(0);
    if(UpdateCodeBufferID == 0xff)
    {
        UpdateCodeBufferID = 0;
    }
    else
    {
        UpdateCodeBufferID = UpdateCodeBufferID ? 0 : 1;
    }
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
    // printf("ID%d,tempUpdatePara.CodeBufferSum[ID]%2X\n",ID,tempUpdatePara.CodeBufferSum[ID]);
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
    else
    {
        return FALSE;
    }

    return api_WriteMemRecordData(Addr + Offset, Len, pBuf);

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
    // printf("buflen%d\n",buflen);
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
            // printf("i:%d,j:%d,Sum:%2X\n",i,j,Sum);
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
        return TRUE;//如果jlink仿真下载的程序，可以把这个屏蔽，就可以不从缓冲区恢复。
    }

    return FALSE;
}

WORD api_CalcCodeAppSum(BYTE ID)
{
    DWORD i, j, buflen, rlen, Addr;
    WORD Sum;
    WORD  tempbuffer[2048];

    buflen = api_GetCodeBufferLen(ID);

    if(buflen > CODEBUFFER_SIZE)
    {
        return FALSE;
    }

     Addr = FLASH_APP1;
   

    Sum = 0;
    for(i=0;i<buflen;i+=rlen)
    {
        CLEAR_WATCH_DOG;

        rlen = buflen - i;
        if(rlen > sizeof(tempbuffer))
        {
            rlen = sizeof(tempbuffer);
        }

        if(api_Flash_read(Addr + i, rlen, (BYTE *)tempbuffer) == TRUE)
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

BOOL api_CheckAppCodeSum(BYTE ID)
{
 //  ID = 0;
    if(api_CalcCodeAppSum(ID) == api_GetCodeBufferSum(ID))
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

/**
 * @brief 程序跳转到RAM运行
 * 
 */

/**
 * @brief 复制程序缓冲区数据到RAM
 * 
 * @param ID    缓冲区ID
 * @return BOOL TRUE:复制正确 FALSE:复制失败
 */
BOOL api_CopyCodeBufferToApp(BYTE ID)
{
    DWORD i, buflen, rlen, FlashAddr, AppAddr;
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


    AppAddr = FLASH_APP1;

        scm_flash_erase(AppAddr,buflen );
        
        
    for(i=0;i<buflen;i+=rlen)
    {
			CLEAR_WATCH_DOG;
			
        rlen = buflen - i;
        if(rlen > sizeof(tempbuffer))
        {
            rlen = sizeof(tempbuffer);
        }

        if(api_ReadMemRecordData(FlashAddr + i, rlen, tempbuffer) == TRUE)
        {
			 api_Flash_write((AppAddr + i), rlen, tempbuffer);
           // memcpy((void *)(AppAddr + i), tempbuffer, rlen);
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL api_CopyAppToCodeBuffer(BYTE ID)
{
	DWORD i, buflen, rlen, FlashAddr, AppAddr;
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

	AppAddr = FLASH_APP1;
		
	for(i=0;i<buflen;i+=rlen)
	{
		CLEAR_WATCH_DOG;
			
		rlen = buflen - i;
		if(rlen > sizeof(tempbuffer))
		{
			rlen = sizeof(tempbuffer);
		}

		if(api_Flash_read((AppAddr + i), rlen, tempbuffer))
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

BOOL WriteAppToFlashBuffer(BYTE ID)
{
	DWORD i, buflen, nlen, FlashAddr, AppAddr;
  	BYTE tempbuffer[4096];
	BYTE* pbyData;
	BYTE byData[12];
	
	AppAddr = FLASH_APP1;

	pbyData = ((BYTE *)AppAddr);

	nlen = 0;
	buflen = 0;
	while((DWORD)pbyData < (FLASH_APP1+CODEBUFFER_SIZE) )
	{
		if((pbyData[0] == 0x68) && (pbyData[1] == 0x55) && (pbyData[2] == 0xAA) && (pbyData[3] == 0x68))
		{
				buflen = (DWORD)((pbyData[11]<<24)+(pbyData[10]<<16)+(pbyData[9]<<8)+(pbyData[8]));
				break;
		}
		nlen++;
		pbyData++;
	}
	if((buflen != nlen) )
		return FALSE;
	
	if(nlen == 0)
		return FALSE;
	
	api_SetCodeBufferLen(ID, nlen+40);
	
	api_CopyAppToCodeBuffer(ID);//app代码拷贝到外部flash

	api_SetCodeBufferSum(ID, api_CalcCodeAppSum(ID));//更新sum到eep
	
	return TRUE;
		
}