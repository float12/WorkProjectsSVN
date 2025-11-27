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

BOOL api_WriteSafeMem(DWORD Addr, WORD Length, BYTE * pBuf)
{
    WriteDoubleEEPRom(Addr,Length,pBuf);
    return TRUE;
}

BOOL api_VWriteSafeMem( DWORD Addr, WORD Length, BYTE * pBuf )
{
    lib_CheckSafeMemVerify( pBuf, Length, REPAIR_CRC );

    return api_WriteSafeMem( Addr, Length, pBuf );
}

BOOL api_VReadSafeMem(DWORD Addr, WORD Length, BYTE *pBuf)
{
    if(ReadEEPRom1(Addr,Length,pBuf)==TRUE)
    {
        if (lib_CheckSafeMemVerify(pBuf, Length, UN_REPAIR_CRC) == TRUE)
        {
            return TRUE;
        }
        else
        {
            ReadEEPRom2(Addr, Length, pBuf);
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
	if(ReadEEPRom1(Addr,Length,pBuf)==FALSE)
    {
        if(ReadEEPRom2(Addr, Length, pBuf)==FALSE)
        {
            return FALSE;
        }    
    }
    return TRUE;
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
	BYTE* Array;

	Array = api_AllocBuf(&Length);
	memset( Array, 0, Length );
	api_WriteContinueMem( Addr, Length, Array );
	api_FreeBuf(Array);
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
