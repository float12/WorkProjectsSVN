///////////////////////////////////////////////////////////////
//	文 件 名 :sm_task.c
//	文件功能 :
//	作    者 : 智能电表组
//	创建时间 : 2021年6月25日
//	项目名称 ：QunlityVS
//	操作系统 : 
//	备    注 :
//	历史记录： : 
///////////////////////////////////////////////////////////////

/*********** Include files ******************/
// 1、电能表扩展模组状态字       
// 2、事件记录对应的超限值        
// 3、永久记录模组清零事件的发生时刻   xx
// 4 、软加密  （用户提供库验证一下）                       
// 5、 软件升级   （用表流程）           
// 6、 分钟冻结 
#include "AllHead.h"

#include <string.h>

#if(!WIN32)
#include <stdint.h>
#include <stdlib.h>
#endif

#include "sm_task.h"
#include "sm_api.h"


TSm4SavePara Sm4SavePara;

BOOL bIsSecurityMode=FALSE;

extern BOOL api_GetSoftFlagFromFlash(BYTE *Buffer);
extern BOOL api_WriteSoftFlagToFlash(BYTE *Buffer);

uint8_t devid[ID_LEN] = 
	{0x73,0x01,0x00,0x00,0x00,0x00,0x00,0x01};//默认私钥ID

uint8_t devid_bak[ID_LEN] = 
	{0x73,0x01,0x00,0x00,0x00,0x00,0x00,0x01};
//默认私钥IK
uint8_t IKsets[96] =    {
        0x2C, 0x27, 0xFA, 0xFC, 0x77, 0x40, 0x8D, 0x1A, 0x97, 0x52, 0xCC, 0xB9, 0x88, 0x87, 0xDD, 0xB1,
        0x90, 0x50, 0xF6, 0x10, 0x86, 0x4D, 0x06, 0x35, 0xD7, 0x9F, 0xF2, 0xBE, 0x99, 0xCA, 0xBC, 0x8C,
        0xCE, 0x85, 0x73, 0x87, 0x0F, 0xEF, 0xAE, 0xBD, 0xBA, 0x1C, 0xA7, 0x2F, 0x4F, 0x04, 0x45, 0xD7,
        0x8B, 0x28, 0x8B, 0x24, 0x1F, 0x13, 0xAD, 0xCF, 0x72, 0x55, 0xFC, 0x0B, 0x8E, 0x74, 0xBB, 0x84,
        0xE9, 0xCB, 0x21, 0xE2, 0x0D, 0x88, 0xC2, 0x97, 0x54, 0x12, 0xDF, 0x3A, 0xBA, 0x46, 0x58, 0xB2,
        0xED, 0x4E, 0xEB, 0x9B, 0x5B, 0x0A, 0xF4, 0x2C, 0xA9, 0xDF, 0x5C, 0xAC, 0x5E, 0x77, 0x3C, 0x57
    };

uint8_t IKsets_bak[96] =    {
        0x2C, 0x27, 0xFA, 0xFC, 0x77, 0x40, 0x8D, 0x1A, 0x97, 0x52, 0xCC, 0xB9, 0x88, 0x87, 0xDD, 0xB1,
        0x90, 0x50, 0xF6, 0x10, 0x86, 0x4D, 0x06, 0x35, 0xD7, 0x9F, 0xF2, 0xBE, 0x99, 0xCA, 0xBC, 0x8C,
        0xCE, 0x85, 0x73, 0x87, 0x0F, 0xEF, 0xAE, 0xBD, 0xBA, 0x1C, 0xA7, 0x2F, 0x4F, 0x04, 0x45, 0xD7,
        0x8B, 0x28, 0x8B, 0x24, 0x1F, 0x13, 0xAD, 0xCF, 0x72, 0x55, 0xFC, 0x0B, 0x8E, 0x74, 0xBB, 0x84,
        0xE9, 0xCB, 0x21, 0xE2, 0x0D, 0x88, 0xC2, 0x97, 0x54, 0x12, 0xDF, 0x3A, 0xBA, 0x46, 0x58, 0xB2,
        0xED, 0x4E, 0xEB, 0x9B, 0x5B, 0x0A, 0xF4, 0x2C, 0xA9, 0xDF, 0x5C, 0xAC, 0x5E, 0x77, 0x3C, 0x57
    };


//更新密钥的K
uint8_t dataSIK[74] = {
		0x84,0xD4,0x00,0x00,0x00,0x44,
		0x19,0x97,0xF2,0xF2,0xDD,0xC5,0x00,0x46,0x93,0x8C,0x4C,0x23,0x55,0x58,0x4C,0x7A,
		0xD1,0xBB,0x86,0x85,0x29,0x9A,0x9C,0x1A,0x7E,0x0C,0xB7,0x83,0x5D,0x05,0x49,0xC3,
		0x9C,0x17,0x6D,0xEC,0xFE,0x04,0xD6,0x40,0xEB,0x59,0xE6,0xAA,0xF0,0x11,0x0C,0xB8,
		0x3C,0xFA,0x14,0xA3,0x97,0xB3,0x96,0x65,0x8F,0x93,0x92,0x7B,0x67,0xC7,0x63,0xBF,
		0x0F,0xC2,0xAC,0xAF
};


BOOL ReadSm4Para(int nType)
{
	if(nType == 0)
	{
		if(ReadEEPRom2(SM4_SIK_DATA_ADDR, sizeof(TSm4SavePara), (uint8_t*)&Sm4SavePara) == TRUE)
		{
			if(Sm4SavePara.InitFlag != 0xAA1256BB)
			{
				Sm4SavePara.InitFlag = 0xAA1256BB;
				// api_FactoryInitEvent();
				// api_FactoryInitFreeze();
				// FactoryInitMeterClearPara();
			}

			if(Sm4SavePara.dwIKMagic == SM4_IK_MAGIC && Sm4SavePara.dwIDMagic == SM4_ID_MAGIC)
			{
				if( (fcs16((BYTE*)&Sm4SavePara.IKBuf[0],sizeof(Sm4SavePara.IKBuf)) == Sm4SavePara.wIKCrc)
					//&& (fcs16((BYTE*)&Sm4SavePara.SIKBuf[0],sizeof(Sm4SavePara.SIKBuf)) == Sm4SavePara.wSIKCrc) 
					&& (fcs16((BYTE*)&Sm4SavePara.DevID[0],sizeof(Sm4SavePara.DevID)) == Sm4SavePara.wIDCrc))
					memcpy(devid, (BYTE*)&Sm4SavePara.DevID[0], 8 );
				if(fcs16((BYTE*)&Sm4SavePara.SoftFlag[0],sizeof(Sm4SavePara.SoftFlag)) != Sm4SavePara.wSoftCrc)
				{
					api_WriteSoftFlagToFlash(Sm4SavePara.SoftFlag);
					Sm4SavePara.wSoftCrc = fcs16((BYTE*)&Sm4SavePara.SoftFlag[0],sizeof(Sm4SavePara.SoftFlag));
					WriteEEPRom2(SM4_SIK_DATA_ADDR, sizeof(TSm4SavePara),(uint8_t*)&Sm4SavePara);
				}
				api_GetSoftFlagFromFlash(Sm4SavePara.SoftFlag);	
				memcpy(devid, (BYTE*)&Sm4SavePara.DevID[0], 8 );
				return TRUE;
			}
		}
	}

	Sm4SavePara.dwIDMagic = SM4_ID_MAGIC;
	Sm4SavePara.dwIKMagic = SM4_IK_MAGIC;
	Sm4SavePara.dwSIKMagic = SM4_SIK_MAGIC;
	
	memcpy(&Sm4SavePara.DevID[0],devid_bak,sizeof(devid));
	
	memcpy(&Sm4SavePara.IKBuf[0],IKsets_bak,sizeof(Sm4SavePara.IKBuf));
	memcpy(&Sm4SavePara.SIKBuf[0],dataSIK,sizeof(Sm4SavePara.SIKBuf));
	
	Sm4SavePara.wIDCrc = fcs16((BYTE*)&Sm4SavePara.DevID[0],sizeof(Sm4SavePara.DevID));
	
	Sm4SavePara.wIKCrc = fcs16((BYTE*)&Sm4SavePara.IKBuf[0],sizeof(Sm4SavePara.IKBuf));
	Sm4SavePara.wSIKCrc = fcs16((BYTE*)&Sm4SavePara.SIKBuf[0],sizeof(Sm4SavePara.SIKBuf));

	api_WriteSoftFlagToFlash(Sm4SavePara.SoftFlag);
	Sm4SavePara.wSoftCrc = fcs16((BYTE*)&Sm4SavePara.SoftFlag[0],sizeof(Sm4SavePara.SoftFlag));
	
	memcpy(devid, (BYTE*)&Sm4SavePara.DevID[0], 8 );
	api_GetSoftFlagFromFlash(Sm4SavePara.SoftFlag);
	
	return WriteEEPRom2(SM4_SIK_DATA_ADDR, sizeof(TSm4SavePara),(uint8_t*)&Sm4SavePara);

}

BOOL WriteSm4Para(BYTE byType, BYTE* pBuf)
{
	//ReadSm4Para();
	
	Sm4SavePara.dwIKMagic = SM4_IK_MAGIC;
	Sm4SavePara.dwSIKMagic = SM4_SIK_MAGIC;

	Sm4SavePara.dwIDMagic = SM4_ID_MAGIC;

	switch(byType)
	{
	case 0:
			memcpy(&Sm4SavePara.IKBuf[0],pBuf,sizeof(Sm4SavePara.IKBuf));
			Sm4SavePara.wIKCrc = fcs16((BYTE*)&Sm4SavePara.IKBuf[0],sizeof(Sm4SavePara.IKBuf));
			break;
	case 1:
			memcpy(&Sm4SavePara.SIKBuf[0],pBuf,sizeof(Sm4SavePara.SIKBuf));
			Sm4SavePara.wSIKCrc = fcs16((BYTE*)&Sm4SavePara.SIKBuf[0],sizeof(Sm4SavePara.SIKBuf));
			break;
	case 3:
		memcpy(&Sm4SavePara.DevID[0],pBuf,sizeof(Sm4SavePara.DevID));
		Sm4SavePara.wIDCrc = fcs16((BYTE*)&Sm4SavePara.DevID[0],sizeof(Sm4SavePara.DevID));
		break;
	case 4:
		memcpy(&Sm4SavePara.SoftFlag[0],pBuf,sizeof(Sm4SavePara.SoftFlag));
		Sm4SavePara.wSoftCrc = fcs16((BYTE*)&Sm4SavePara.SoftFlag[0],sizeof(Sm4SavePara.SoftFlag));
		break;
	default:
		return FALSE;
	}
	
	return WriteEEPRom2(SM4_SIK_DATA_ADDR, sizeof(TSm4SavePara),(uint8_t*)&Sm4SavePara);
	
}

boolean_t sm4_SetIKey(uint8_t* inBuf)
{
    int ret=FALSE;
#if(USE_SAFETY_MODE)	
    memcpy(Sm4SavePara.IKBuf, inBuf, 96);
	
    WriteSm4Para(0, inBuf);
        
   ret = SM4SetIK(Sm4SavePara.IKBuf);
#endif			
	return ret;
}

boolean_t sm4_SetID(uint8_t* inID)
{
		int ret=FALSE;
#if(USE_SAFETY_MODE)	
		memcpy(Sm4SavePara.DevID, inID, 8);
		
		WriteSm4Para(3, inID);

	  ret = SetID(Sm4SavePara.DevID,8);
#endif		
	
		return ret;
}

boolean_t sm4_WriteSoftFlag(uint8_t* SoftBuffer)
{
		int ret=0;
	
		memcpy(Sm4SavePara.SoftFlag, SoftBuffer, sizeof(Sm4SavePara.SoftFlag));
		
		ret = WriteSm4Para(4, SoftBuffer);
		
	   api_GetSoftFlagFromFlash(Sm4SavePara.SoftFlag);
	
		return ret;
}


uint8_t sm4_GetStatus(void)
{
#if(USE_SAFETY_MODE)
    uint8_t IdTemp[8];
	//  uint8_t IdCmpare[8]={0xFF};

	//  memset(IdCmpare,0xFF,8);
	// memcpy(IdTemp, Sm4SavePara.DevID, 8);

	getStatus(IdTemp, 8);

	if(memcmp(IdTemp, Sm4SavePara.DevID,8))
	{
		sm4_InitKey();
	}
	return getStatus(IdTemp, 8);
#elif
	return 0;
#endif

}

boolean_t Sm4_Reset(void)
{
  #if(USE_SAFETY_MODE)
        ClearSm4Lib();
   return statReset();
#elif        
   return FALSE;
#endif

}

///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitKey
//	函数功能 : 根据厂家提交的设备 ID 清单，生成“ID-预置密钥”数据清单，并下发给厂家
//	处理过程 : 
//             A SetStorageSector 将存储区的起始地址和空间大小告知算法库。
//			   B SetID 将 ID 数据，写入指定扇区。
//			   C SM4SetIK 将 ID 数据对应的预置密钥数据，写入指定扇区。
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : void
///////////////////////////////////////////////////////////////
int sm4_InitKey(void)
{
	int ret;

	bIsSecurityMode = FALSE;
#if(USE_SAFETY_MODE)	
	ret = InitSm4Lib();
	if(ret==FALSE) 
		return 0;

        
	ret = SetID(Sm4SavePara.DevID, ID_LEN);		// 设备 ID 写入成功 
	if(ret==FALSE) 
		return 0;

	ret = SM4SetIK(Sm4SavePara.IKBuf);			// 预置密钥安装成功 

#endif
	return ret;

}

// 初始化组网
// 认证阶段
///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_Authent1
//	函数功能 : 下发 16 字节密文数据，执行初始认证
//	处理过程 : 输入电表下发的密文数据，若认证通过，则返回 32 字节密文数据。
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 :  uint8_t* inBuf16,     // 假设 inBuf16 为对方发送数据  16字节
//				  uint8_t* outBuf32   // outBuf32 若认证通过，则返回 32 字节密文数据
///////////////////////////////////////////////////////////////
boolean_t sm4_InitNet_Authent1( uint8_t* inBuf16,  uint8_t* outBuf32)
{
	uint8_t ret;

	if(inBuf16 == NULL)
		return FALSE;

	if(outBuf32 == NULL)
		return FALSE;
#if(USE_SAFETY_MODE)
	ivecReset(1);
	
	ret = SM4UpdateCheck(inBuf16, outBuf32); 
	if(ret == TRUE) 
	{
		return TRUE;	// 认证第一步通过. 需要返回的数据已写入 'buf' 
	}
#endif
	return FALSE;

}

BOOL sm4_UnVoidAuthent(void)
{
    #if(USE_SAFETY_MODE)
    ivecReset(1);
    #endif
    return FALSE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_Authent2
//	函数功能 : 输入 16 字节密文数据，若返回成功，则初始认证成功；否则，初始认证失败。
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBuf16   16 字节密文数据
///////////////////////////////////////////////////////////////
boolean_t sm4_InitNet_Authent2(uint8_t* inBuf16)
{
	uint8_t ret;

	if(inBuf16 == NULL)
		return FALSE;
#if(USE_SAFETY_MODE)	
	ret = SM4UpdateGen();	 // 生成认证数据/协商密钥完毕. 
	if(ret == FALSE) 
		return FALSE;

	ret =SM4UpdateFinish(inBuf16);
	if(ret == TRUE) 
	{
		bIsSecurityMode = TRUE;
		return TRUE;		 // 双向认证成功. 密钥协商完成. 
	}
#endif
	bIsSecurityMode = FALSE;
	return FALSE;
}

// 初始化组网
// 密钥下发阶段
///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_Keydistribut
//	函数功能 : 初始认证成功后，将可以下发用于更新的认证密钥数据
//	处理过程 : 传入电表下发的密文数据，以安装更新认证密钥
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBufSik,
//				 uint8_t nLen == 74
// 例如：uint8_t dataSIK[74] = {
//		0x84,0xD4,0x00,0x00,0x00,0x44,
//		0x19,0x97,0xF2,0xF2,0xDD,0xC5,0x00,0x46,0x93,0x8C,0x4C,0x23,0x55,0x58,0x4C,0x7A,
//		0xD1,0xBB,0x86,0x85,0x29,0x9A,0x9C,0x1A,0x7E,0x0C,0xB7,0x83,0x5D,0x05,0x49,0xC3,
//		0x9C,0x17,0x6D,0xEC,0xFE,0x04,0xD6,0x40,0xEB,0x59,0xE6,0xAA,0xF0,0x11,0x0C,0xB8,
//		0x3C,0xFA,0x14,0xA3,0x97,0xB3,0x96,0x65,0x8F,0x93,0x92,0x7B,0x67,0xC7,0x63,0xBF,
//		0x0F,0xC2,0xAC,0xAF
//	};
///////////////////////////////////////////////////////////////
BOOL bIinitIK=FALSE;
BYTE	globalSIKBuf[74]={0};

boolean_t sm4_InitNet_Keydistribut(uint8_t* inBufSik, uint8_t nLen)
{
//	uint8_t ret;

	if(inBufSik == NULL)
		return FALSE;

	bIinitIK = TRUE;
	
	memcpy(globalSIKBuf,inBufSik,sizeof(Sm4SavePara.SIKBuf));
		
	return TRUE;
/*	ret = SM4InitSIK(inBufSik);
	if(ret == TRUE) 
	{
		memcpy(Sm4SavePara.SIKBuf,inBufSik,sizeof(Sm4SavePara.SIKBuf));
		WriteSm4Para(1, Sm4SavePara.SIKBuf);
		return TRUE;// 认证密钥更新成功 
	}

	return FALSE;
	*/
}

extern BOOL bIsStatReset;
void Freshsm4IK_Task(void)
{
	uint8_t ret;
#if(USE_SAFETY_MODE)
	if(bIinitIK == TRUE)
	{
			ret = SM4InitSIK(globalSIKBuf);
			if(ret == TRUE) 
			{
					memcpy(Sm4SavePara.SIKBuf,globalSIKBuf,sizeof(Sm4SavePara.SIKBuf));
					WriteSm4Para(1, globalSIKBuf);
			}
			
			bIinitIK = FALSE;
	}
    if(bIsStatReset == TRUE)
    {
        statReset();
        bIsStatReset = FALSE;
        sm4_InitKey();
    }
#endif
	
}
///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_KeyAgreement1
//	函数功能 : 下发 16 字节密文数据，执行初始认证
//	处理过程 : 输入电表下发的密文数据，若认证通过，则返回 32 字节密文数据。
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 :  uint8_t* inBuf16,     // 假设 inBuf16 为对方发送数据  16字节
//				  uint8_t* outBuf32   // outBuf32 若认证通过，则返回 32 字节密文数据
///////////////////////////////////////////////////////////////
/*boolean_t sm4_InitNet_KeyAgreement1( uint8_t* inBuf16,  uint8_t* outBuf32)
{
	uint8_t ret;
	
	if(inBuf16 == NULL)
		return FALSE;
	
	if(outBuf32 == NULL)
		return FALSE;
	
	ret = SM4UpdateCheck(inBuf16, outBuf32); 
	if(ret == TRUE) 
	{
		return TRUE;	// 认证第一步通过. 需要返回的数据已写入 'buf' 
	}
	
	return FALSE;
	
}


///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_KeyAgreement2
//	函数功能 : 输入 16 字节密文数据，若返回成功，则初始认证成功；否则，初始认证失败。
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBuf16   16 字节密文数据
///////////////////////////////////////////////////////////////
boolean_t sm4_InitNet_KeyAgreement2(uint8_t* inBuf16)
{
	uint8_t ret;
	
	ret = SM4UpdateGen();	 // 生成认证数据/协商密钥完毕. 
	if(ret == FALSE) 
		return FALSE;
	
	ret =SM4UpdateFinish(inBuf16);
	if(ret == TRUE) 
	{
		return TRUE;		 // 双向认证成功. 密钥协商完成. 
	}
	
	return FALSE;
}
*/


///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_Decrypt
//	函数功能 : //解密下发报文
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBuf,
//				 uint32_t nInLen,
//				 uint8_t* outBuf,
//				 uint32_t nOutLen
///////////////////////////////////////////////////////////////
BYTE OutBuffer[2048]={0};
boolean_t sm4_Decrypt(uint8_t auth, uint8_t *inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen) 
{
	uint8_t ret;
	
	
	*nOutLen = 2048;
#if(USE_SAFETY_MODE)
	//ret = SM4Decrypt(auth, Buffer3, nInLen, Buffer, &nLen);//nOutLen
	
	ret = SM4Decrypt(auth, inBuf, nInLen, OutBuffer, nOutLen);//nOutLen
	if(ret == TRUE) 
	{
		memcpy(outBuf,OutBuffer,*nOutLen);
		return TRUE; // 解密成功. // 解密结果数据已写入 'outBuf'.	// 数据长度已返回至 buflen 
	}
#endif
	return FALSE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_Encrypt
//	函数功能 : //加密上传报文
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBuf,
//				 uint32_t nInLen,
//				 uint8_t* outBuf,
//				 uint32_t nOutLen
///////////////////////////////////////////////////////////////
//BYTE Buffer3[2048];
boolean_t sm4_Encrypt(uint8_t auth, uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen) 
{
	uint8_t ret;

	uint32_t nLen=2048;
	
	//*nOutLen = 5120;
#if(USE_SAFETY_MODE)	
	ret = SM4Encrypt(auth, inBuf, nInLen, OutBuffer, &nLen);
	if(ret == TRUE) 
	{
		*nOutLen = nLen;
		memcpy(outBuf,OutBuffer,*nOutLen);
		return TRUE; // 加密成功. // 加密结果数据已写入 'outBuf'.	// 数据长度已返回至 buflen 
	}
#endif
	return FALSE;

}

uint8_t sm4_Mac(uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen) 
{
	uint8_t ret;
#if(USE_SAFETY_MODE)
	ret = SM4Mac( inBuf, nInLen, outBuf, nOutLen);
	if(ret == TRUE) 
	{
		return TRUE; // 加密成功. // 加密结果数据已写入 'outBuf'.	// 数据长度已返回至 buflen 
	}
#endif
	return FALSE;
}
uint8_t sm4_MacVerify(uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t nOutLen) 
{
	uint8_t ret;
#if(USE_SAFETY_MODE)
	ret = SM4MacVerify( inBuf, nInLen, outBuf, nOutLen);
	if(ret == TRUE) 
	{
		return TRUE; // 加密成功. // 加密结果数据已写入 'outBuf'.	// 数据长度已返回至 buflen 
	}
#endif
	return FALSE;	
}

int smTask_main() 
{
//	BYTE byData;

	ReadSm4Para(0);

	InitSm4Lib();

//	if(sm4_GetStatus())
//		byData = 1;
//	else
//		byData = 0;

#if(USE_SAFETY_MODE)
	version();
#endif	
	return 0;

}

/********** Local type definitions ('typedef') ********************************/


//void Flash_SectorErase(uint32_t addr) {
//    uint32_t bgn = addr / 512 * 512;
//    memset(flash_sim+bgn, 0, 512);
//}


/********** Function implementation - global ('extern') and local ('static') **/

