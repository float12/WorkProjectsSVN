

#include "AllHead.h"

#include <string.h>

#if(!WIN32)
#include <stdint.h>
#include <stdlib.h>
#endif

#include "light_sm.h"

//typedef boolean_t	(*flash_set_zeros)(uint32_t addr, uint32_t len);
//typedef boolean_t	(*flash_set_words)(uint32_t addr, uint32_t len, uint32_t *arr);
//typedef void		(*flash_get_words)(uint32_t addr, uint32_t len, uint32_t *arr);
//typedef boolean_t	(*flash_set_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);
//typedef void		(*flash_get_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);
//typedef boolean_t	(*flash_clean_t)(uint32_t addr, uint32_t len);
//typedef boolean_t	(*random_gen_t)(uint32_t *ranL, uint32_t len);


flash_set_zeros		callback_flash_setWords_zero;
flash_set_words		callback_flash_setWords;
flash_get_words		callback_flash_getWords;
flash_clean_t		callback_flash_cleanWords;
flash_set_bytes		callback_flash_setBytes;
flash_get_bytes		callback_flash_getBytes;
flash_clean_t		callback_flash_cleanBytes;
random_gen_t		callback_getRandomWords;


#define USE_RAM_DEBUG			0
#define USR_EX_FALSH			0

#define MAX_SM4_BUFFER		1024//1536		//不小于 512 字节

#if(USR_EX_FALSH)
#define SM4_FLASH_ADDR		FLASH_SM4_BASE  //	FLASH_SM4_BASE	//0x00
#elif(USE_RAM_DEBUG)
#define SM4_FLASH_ADDR		0
#else
#define SM4_FLASH_ADDR		(0xC800 - HALFRMN_MEM_SPACE)
#endif
	
uint8_t flash_sim[MAX_SM4_BUFFER];

extern BOOL WriteExtFlash( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );
extern BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);
extern WORD ReadDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf1, BYTE * Buf2 );
extern WORD WriteDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf );

extern BOOL api_VWriteSafeMem(DWORD Addr, WORD Length, BYTE* pBuf);
extern BOOL api_VReadSafeMem(DWORD Addr, WORD Length, BYTE* pBuf);

extern BOOL WriteEEPRom2(DWORD Addr, WORD Length, BYTE* pBuf);
extern BOOL ReadEEPRom2(DWORD Addr, WORD Length, BYTE* pBuf);


BOOL WriteExtFlashPage(BYTE No, DWORD Addr, WORD Len, BYTE *pBuf)
{
	if(ReadExtFlash(No, SM4_FLASH_ADDR, MAX_SM4_BUFFER, flash_sim) == TRUE)
	{
			Addr = Addr - SM4_FLASH_ADDR;
			if((Addr + Len) > MAX_SM4_BUFFER)
					return FALSE;
			
			memcpy(&flash_sim[Addr], pBuf, Len);
			return WriteExtFlash(No, SM4_FLASH_ADDR, MAX_SM4_BUFFER, flash_sim);
	}
	return FALSE;
	
}

boolean_t Flash_clean_long(uint32_t addr, uint32_t len)
{
#if(USE_RAM_DEBUG)
    memset(flash_sim+addr, 0, 4*len);
	return TRUE;
#else
	uint8_t buf[MAX_SM4_BUFFER] = {0};
	memset(buf, 0, 4*len);

#if (USR_EX_FALSH)	
	WriteExtFlashPage(CS_SPI_FLASH, addr, 4*len, buf);
#else
	WriteEEPRom2(addr, 4*len, buf);
#endif
	
#endif
    return TRUE;
}

boolean_t Flash_clean_bytes(uint32_t addr, uint32_t len) 
{
#if(USE_RAM_DEBUG)
    memset(flash_sim+addr, 0, len);
	return TRUE;
#else
	uint8_t buf[MAX_SM4_BUFFER] = {0};
	
	memset(buf, 0, len);
	
	#if (USR_EX_FALSH)
	WriteExtFlashPage(CS_SPI_FLASH, addr, len, buf);
	#else
	WriteEEPRom2(addr, len, buf);
	#endif
	
#endif
    return TRUE;
};
//////////////////////////////////////////////////////////////////
#if(USE_RAM_DEBUG)
static uint32_t Flash_getWord(uint32_t addr, uint32_t offset) 
{
#if(USE_RAM_DEBUG)
   return *((uint32_t *)(flash_sim + addr) + offset);
#endif
	return 0;
}
static boolean_t Flash_setWord(uint32_t addr, uint32_t src) 
{
#if(USE_RAM_DEBUG)
   memcpy(flash_sim+addr, &src, 4); 
	return TRUE;
#endif
  return TRUE;
}
#endif

#if(USE_RAM_DEBUG)
static uint32_t Flash_getData_byte(uint32_t addr, uint32_t offset) 
{
#if(USE_RAM_DEBUG)
   return flash_sim[addr + offset];
#endif
	return 0;
}
static boolean_t Flash_WriteByte(uint32_t addr, uint8_t src) 
{
#if(USE_RAM_DEBUG)
   memcpy(flash_sim+addr, &src, 1);
	return TRUE;
#endif
   return TRUE;
}
#endif
///////////////////////////////////////////////////////////////////////

boolean_t Flash_set_zero_long(uint32_t addr, uint32_t len) 
{
#if(USE_RAM_DEBUG)
    uint32_t i;
    for(i = 0; i < len; i++) 
	{
        if(TRUE == Flash_setWord(addr + 4 * i, 0x00)) 
		{
            if(Flash_getWord(addr, i) != 0x00)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
#else
	uint8_t buf[MAX_SM4_BUFFER] = {0};
	
	if(len*4 > MAX_SM4_BUFFER)
		len = MAX_SM4_BUFFER / 4;

	memset(buf, 0, len*4);
	
	#if (USR_EX_FALSH)
	if(WriteExtFlashPage(CS_SPI_FLASH, addr, len*4, buf) == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	#else
	return WriteEEPRom2(addr, len*4, buf);
	#endif
#endif//#if(USE_RAM_DEBUG)

}

boolean_t Flash_set_data_long(uint32_t addr, uint32_t len, uint32_t *arr) 
{
#if(USE_RAM_DEBUG)
	uint32_t i;
	for(i = 0; i < len; i++) 
	{ 
		if(TRUE == Flash_setWord(addr + 4 * i, arr[i])) 
		{ 
			if(Flash_getWord(addr, i) != arr[i]) 
			{ 
				return FALSE; 
			} 
		} 
	}
	return TRUE;

#else
//	uint8_t buf[MAX_SM4_BUFFER] = {0};
	
	if(len*4 > MAX_SM4_BUFFER)
		len = MAX_SM4_BUFFER / 4;

	#if (USR_EX_FALSH)
	if(WriteExtFlashPage(CS_SPI_FLASH, addr, len*4, (uint8_t *)arr) == TRUE)
	{
		if(ReadExtFlash(CS_SPI_FLASH, addr, len*4, buf) == TRUE)
		{
			if(!memcmp(arr, buf,len*4 ))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	#else
	return WriteEEPRom2(addr, len*4,(uint8_t *) arr);
	#endif
#endif

}

void Flash_get_data_long(uint32_t addr, uint32_t len, uint32_t *arr) 
{
#if(USE_RAM_DEBUG)
    uint32_t i;
    for(i = 0; i < len; i++) 
	{
        arr[i] = Flash_getWord(addr, i);
    }
#else

	if(len*4 > MAX_SM4_BUFFER)
		len = MAX_SM4_BUFFER / 4;

	#if (USR_EX_FALSH)
	ReadExtFlash(CS_SPI_FLASH, addr, len*4, (uint8_t *)arr) ;
	#else
	ReadEEPRom2(addr, len*4, (uint8_t *)arr);
	#endif
#endif
}


boolean_t Flash_set_data_byte(uint32_t addr, uint32_t len, uint8_t *arr) 
{
#if(USE_RAM_DEBUG)
	uint32_t i;
	for(i = 0; i < len; i++) 
	{
		if(TRUE == Flash_WriteByte(addr + i, arr[i]))
		{
			if(Flash_getData_byte(addr, i) != arr[i])
			{ 
				return FALSE; 
			} 
		} 
	}
	return TRUE;

#else
//	uint8_t buf[MAX_SM4_BUFFER] = {0};
	
	if(len > MAX_SM4_BUFFER)
		len = MAX_SM4_BUFFER;
	
	#if (USR_EX_FALSH)
	if(WriteExtFlashPage(CS_SPI_FLASH, addr, len, arr) == TRUE)
	{
		if(ReadExtFlash(CS_SPI_FLASH, addr, len, buf) == TRUE)
		{
			if(!memcmp(arr, buf,len ))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	#else
	return WriteEEPRom2(addr, len, arr);
	#endif
#endif//#if(USE_RAM_DEBUG)
}

void Flash_get_data_byte(uint32_t addr, uint32_t len, uint8_t *arr) 
{
#if(USE_RAM_DEBUG)

    uint32_t i;
    for(i = 0; i < len; i++) 
	{
        arr[i] = Flash_getData_byte(addr, i);
    }
#else
	if(len > MAX_SM4_BUFFER)
		len = MAX_SM4_BUFFER;
	
	#if (USR_EX_FALSH)
	ReadExtFlash(CS_SPI_FLASH, addr, len, arr) ;
	#else
	ReadEEPRom2(addr, len, arr);
	#endif
#endif
}


//static uint32_t g_Counter = 0;

boolean_t my_getRand(uint32_t *ranL, uint32_t len) 
{
    uint32_t i;
#if 1
    uint32_t test[4]={0x98765432,0x98765432,0x01234567,0x89abcdef};
    for(i = 0; i < len; i++) 
	{
        ranL[i] = test[i%4];
    }
#else
    srand(g_Counter++);
    for(i = 0; i < len; i++) 
	{
        ranL[i] = (((uint8_t)rand()) << 24) | (((uint8_t)rand()) << 16) | (((uint8_t)rand()) << 8) | ((uint8_t)rand());
    }
#endif
    return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : InitSm4Lib
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : void
///////////////////////////////////////////////////////////////
boolean_t InitSm4Lib(void)
{
    callback_flash_setWords_zero = Flash_set_zero_long;
    callback_flash_setWords = Flash_set_data_long;
    callback_flash_getWords = Flash_get_data_long;
    callback_flash_cleanWords = Flash_clean_long;
	
    callback_flash_setBytes = Flash_set_data_byte;
    callback_flash_getBytes = Flash_get_data_byte;
    callback_flash_cleanBytes = Flash_clean_bytes;
	
    callback_getRandomWords = my_getRand;
	
		return SetStorageSector(SM4_FLASH_ADDR, MAX_SM4_BUFFER); // 所提供空间大小合法 
		
}
boolean_t ClearSm4Lib(void)
{
    uint8_t  arr[MAX_SM4_BUFFER] = {0xFF};
    WriteEEPRom2(SM4_FLASH_ADDR, MAX_SM4_BUFFER,(uint8_t *) arr);
	return TRUE;
}
#if(WIN32)
///////////////////////////////////////////////////////////////
//	函 数 名 : SetStorageSector
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint32_t bgn,
//				 uint32_t bytelen
///////////////////////////////////////////////////////////////
boolean_t SetStorageSector(uint32_t bgn, uint32_t bytelen)
{
	//SetStorageSector(MY_ADDR,  176);

	bgn = bgn;

	bytelen = bytelen;


	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SetID
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t *devid,
//				 uint32_t idlen
///////////////////////////////////////////////////////////////
boolean_t SetID(uint8_t *devid, uint32_t idlen)//ok
{
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : getStatus
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t* devid,
//				 uint32_t idlen
///////////////////////////////////////////////////////////////
uint8_t getStatus(uint8_t* devid, uint32_t idlen)
{
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4SetIK
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t* keySets
///////////////////////////////////////////////////////////////
uint8_t SM4SetIK(uint8_t* keySets)//ok
{
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4UpdateCheck
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t* cRanA,
//				 uint8_t* cRanCat
///////////////////////////////////////////////////////////////
uint8_t SM4UpdateCheck(uint8_t* cRanA, uint8_t* cRanCat)//ok
{
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4UpdateGen
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : void
///////////////////////////////////////////////////////////////
boolean_t SM4UpdateGen(void)//ok
{
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4UpdateFinish
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t *ciphB
///////////////////////////////////////////////////////////////
uint8_t SM4UpdateFinish(uint8_t *ciphB)//ok
{
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4InitSIK
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t* keyData
///////////////////////////////////////////////////////////////
uint8_t SM4InitSIK(uint8_t* keyData)//ok
{
	return 0;
}

//auth:  false: enc/dec   true: enc+mac / dec+verifymac
///////////////////////////////////////////////////////////////
//	函 数 名 : SM4Encrypt
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t auth,
//				 uint8_t *inData,
//				 uint32_t inLen,
//				 uint8_t *outData,
//				 uint32_t *outLen
///////////////////////////////////////////////////////////////
uint8_t SM4Encrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen)//ok // EK1 enc & SIK auth
{
	*outLen = inLen;
	return 1;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4Decrypt
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t auth,
//				 uint8_t *inData,
//				 uint32_t inLen,
//				 uint8_t *outData,
//				 uint32_t *outLen
///////////////////////////////////////////////////////////////
uint8_t SM4Decrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen)//ok // EK2 dec & SIK auth
{
	*outLen = inLen;
	return 1;
}

//plaintext + mac
///////////////////////////////////////////////////////////////
//	函 数 名 : SM4Mac
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t *inData,
//				 uint32_t inLen,
//				 uint8_t *macData,
//				 uint32_t *outLen
///////////////////////////////////////////////////////////////
uint8_t SM4Mac(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t *outLen) // EK1 plaintext + mac
{
	*outLen = inLen;
	return 1;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SM4MacVerify
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t *inData,
//				 uint32_t inLen,
//				 uint8_t *macData,
//				 uint32_t outLen
///////////////////////////////////////////////////////////////
uint8_t SM4MacVerify(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t* outLen) // EK2 plaintext + mac verify
{
	*outLen = inLen;
	return 1;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : statReset
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : void
///////////////////////////////////////////////////////////////
boolean_t statReset(void)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : ivecReset
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : boolean_t rmEK
///////////////////////////////////////////////////////////////
boolean_t ivecReset(boolean_t rmEK)
{
	return TRUE;
}

// New interface:
///////////////////////////////////////////////////////////////
//	函 数 名 : initConnection
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : uint8_t *info,
//				 uint32_t infolen,
//				 uint8_t *outData,
//				 uint32_t *outLen
///////////////////////////////////////////////////////////////
uint8_t initConnection(uint8_t *info, uint32_t infolen, uint8_t *outData, uint32_t *outLen)
{
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : version
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : uint8_t
//	参数说明 : 
///////////////////////////////////////////////////////////////
DllAPI uint8_t version()
{
	return 0;
}

#endif
