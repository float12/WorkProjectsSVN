//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		存储模块的接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "gd25q64c.h"
#include "SaveMem.h"
#include "M24512.h"

BOOL api_WriteFLashMem( DWORD Addr, WORD Length, BYTE * pBuf )
{
    WORD DataLen;
    DWORD TempAddr;
    BYTE EraseNum,i;
    BYTE FlashBuf[SECTOR_SIZE];

    TempAddr = Addr%SECTOR_SIZE;
    EraseNum = (TempAddr+Length)/SECTOR_SIZE;
    if((TempAddr+Length)%SECTOR_SIZE != 0)
    {
        EraseNum += 1;
    }

    if(EraseNum>2)
    {
        return FALSE;
    }

    DataLen = 0;
    for(i=0; i<EraseNum; i++)
    {
        TempAddr = ((Addr/SECTOR_SIZE)+i)*SECTOR_SIZE;
        ReadExtFlash(CS_SPI_FLASH, TempAddr, SECTOR_SIZE, FlashBuf);

        if( i == 0 )
        {
            DataLen = (Addr%SECTOR_SIZE)+ Length;
            DataLen = ((DataLen>SECTOR_SIZE) ? (SECTOR_SIZE-(Addr%SECTOR_SIZE)) : Length);
            memcpy(FlashBuf+Addr%SECTOR_SIZE, pBuf, DataLen);
        }
        else
        {
            DataLen = Length - DataLen - (SECTOR_SIZE*(i-1));
            DataLen = ((DataLen>SECTOR_SIZE) ? SECTOR_SIZE : DataLen);
            memcpy(FlashBuf, pBuf, DataLen);
        }

        WriteExtFlash(CS_SPI_FLASH, TempAddr, SECTOR_SIZE, FlashBuf);
    }

    return TRUE;
}

BOOL api_VWriteSafeMem( DWORD Addr, WORD Length, BYTE * pBuf )
{
	lib_CheckSafeMemVerify( pBuf, Length, REPAIR_CRC );

	WriteDoubleEEPRom(Addr,Length,pBuf);
	return TRUE;
}

BOOL api_VReadSafeMem(DWORD Addr, WORD Length, BYTE *pBuf)
{
	if(Addr < UPFILE_DATA_ADDR)
	{
		if(ReadEEPRom1(Addr,Length,pBuf)==TRUE)
		{
			if (lib_CheckSafeMemVerify(pBuf, Length, UN_REPAIR_CRC) == TRUE)
			{
				return TRUE;
			}
			else
			{
				if(ReadEEPRom2(Addr,Length,pBuf)==TRUE)
				{
					if (lib_CheckSafeMemVerify(pBuf, Length, UN_REPAIR_CRC) == TRUE)
					{
						return TRUE;
					}
				}
			}
		}
		else
		{
			if(ReadEEPRom2(Addr,Length,pBuf)==TRUE)
			{
				if (lib_CheckSafeMemVerify(pBuf, Length, UN_REPAIR_CRC) == TRUE)
				{
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

BOOL api_WriteContinueMem(DWORD Addr, WORD Length, BYTE * pBuf)
{
    return WriteDoubleEEPRom(Addr,Length,pBuf);
}

BOOL api_ReadContinueMem(DWORD Addr, WORD Length, BYTE * pBuf)
{
	if(Addr < UPFILE_DATA_ADDR) //由于连续空间也是写备份，那么不能超过特殊地址,所以第一片和第二片都不能超过这个地址
	{
		if(ReadEEPRom1(Addr,Length,pBuf)==FALSE)
		{
        	if(ReadEEPRom2(Addr, Length, pBuf)==FALSE)
			{
            	return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL api_WriteMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return WriteExtFlash(CS_SPI_FLASH, Addr, Length, pBuf);
}

BOOL api_ReadMemRecordData(DWORD Addr, WORD Length, BYTE * pBuf)
{
	return ReadExtFlash(CS_SPI_FLASH, Addr, Length, pBuf);
}

void api_ClrContinueEEPRom(DWORD Addr, WORD Length)
{
	BYTE Array[8192];//申请不定长数据

	if( Length > 8192 )
	{
		return;
	}
	memset( Array, 0, Length );
	api_WriteContinueMem( Addr, Length, Array );
}
//-----------------------------------------------
//函数功能: 清安全空间,要求长度必须是结构的长度，包括校验在内，
//			而且安全空间的第二份是取反存储,校验固定认为是CRC32，该函数会自动添加校验
//
//参数: 
//			Addr[in]				起始地址
//			Length[in]				数据长度，整个结构的长度，包括校验
//                    
//返回值:  	TRUE:正确写入了两份数据	FALSE:有一份或两份数据没有正确写入
//
//备注:   
//-----------------------------------------------
WORD api_ClrSafeMem( WORD Addr, WORD Length )
{
	WORD Result;
	BYTE Buf[2000];//申请不定长数据
	
	if( Length > 2000 )
	{
		return FALSE;
	}

	// 申请缓冲
	memset( Buf, 0x00, Length );
	Result = api_VWriteSafeMem( Addr, Length, Buf );
	
	return Result;
}
//-----------------------------------------------
//函数功能: 读所有存储芯片指定地址数据，为厂内功能单独写的函数
//
//参数: 
//			No[in]			指定读哪一片EEPROM
//							0x01：第一片EEPROM
//							0x02：第二片EEPROM
//							0x11：第一片Flash
//							0x21: 第一片铁电
//							0x31: 读连续空间
//			Addr[in]		要读的起始地址（芯片内偏移地址）
//			Length[in]		要读数据的长度
//			pBuf[out]		要读出数据的缓冲
//                    
//返回值:  	读出数据的长度，如果为0，表示失败
//
//备注:   
//-----------------------------------------------
WORD api_ReadSaveMem( BYTE No, DWORD Addr, WORD Length, BYTE *pBuf )
{
	switch( No )
	{
		case 0x01:
            ReadEEPRom1(Addr,Length,pBuf);
			break;
		case 0x02:
            ReadEEPRom2(Addr, Length, pBuf);
			break;
		#if( (THIRD_MEM_CHIP==CHIP_GD25Q64C) || (THIRD_MEM_CHIP==CHIP_GD25Q32C) || (THIRD_MEM_CHIP==CHIP_GD25Q16C) )
		case 0x11:
			ReadExtFlash( CS_SPI_FLASH, Addr, Length, pBuf );
			break;
		#endif
		case 0x21://不支持铁电
			return 0;
		case 0x31:
			// api_ReadFromContinueEEPRom( Addr, Length, pBuf );
            return 0;
		default:
			return 0;
	}
	
	return Length;
}


//--------------------------------------------------
//功能描述:  存储License
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_SaveLicense(BYTE * pBuf )
{
	WORD Length = sizeof(TLicenseSafeRom);
	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(LicenseSafeRom.License[0]), Length, pBuf);
}

//--------------------------------------------------
//功能描述:  读取License
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE  api_ReadLicense(BYTE * pBuf )
{
	WORD Length = 132;

	api_VReadSafeMem(GET_SAFE_SPACE_ADDR(LicenseSafeRom.License[0]), Length, pBuf);
	return Length;
}

BOOL WriteExtFlash2( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf )
{
	BOOL bResult;
	WORD DataLen;
    BYTE Buffer[SECTOR_SIZE];
    
	while(Len)
	{
		CLEAR_WATCH_DOG;
                
		bResult = ReadExtFlash(No, (Addr/SECTOR_SIZE)*SECTOR_SIZE, SECTOR_SIZE, Buffer);
		if(bResult == FALSE)
			return FALSE;
		
                
                
		DataLen = (SECTOR_SIZE - (Addr % SECTOR_SIZE));
		
		if(Len <= DataLen)
		{
			
			memcpy(&Buffer[(Addr % SECTOR_SIZE)], pBuf, Len);
			
			Len = 0;
			
			bResult = WriteExtFlash(CS_SPI_FLASH, (Addr/SECTOR_SIZE)*SECTOR_SIZE, SECTOR_SIZE, Buffer);
			if(bResult == FALSE)
				return FALSE;
		}
		else
		{
			
			memcpy(&Buffer[(Addr % SECTOR_SIZE)], pBuf, DataLen);
			
			Len -= DataLen;
			pBuf += DataLen;
			
			bResult = WriteExtFlash(CS_SPI_FLASH, (Addr/SECTOR_SIZE)*SECTOR_SIZE, SECTOR_SIZE, Buffer);
			if(bResult == FALSE)
				return FALSE;

			Addr += DataLen;
			
		}
		
	}

	return TRUE;
}

