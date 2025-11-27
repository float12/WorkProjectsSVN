//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	低压台区软件-zhaohang
//创建日期	20230214
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "lib/gcm1.h"
#include "Dlt698_45.h"
#include "rtthread.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
BYTE g_byWSDParaFlag = 0;
BYTE g_byWSDParaBuff[1024];
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
TParaInfoSafeRom TParaInfo;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
WORD CheckParaRes = 0x0000;
const DWORD LegalOITableDefNS[] = {0x0003EB20,0x00090043,0x00020250};//厂内模式(参数设置)，初始化,分钟冻结


//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

extern void mbedtls_gcm_init_my( mbedtls_gcm_context *ctx );
extern int mbedtls_gcm_setkey( mbedtls_gcm_context *ctx, mbedtls_cipher_id_t cipher,
                        const unsigned char *key, unsigned int keybits );
extern int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx, int mode,
                       size_t length, const unsigned char *iv, size_t iv_len,
                       const unsigned char *add, size_t add_len, const unsigned char *input,
                       unsigned char *output, size_t tag_len, unsigned char *tag );
extern void mbedtls_gcm_free( mbedtls_gcm_context *ctx );

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//功能描述:  
//         
//参数:		*InputMessage, 		//要加密的信息
//			InputMessageLength, //要加密的信息字节数
//			*KValue, 			//密钥 16个字节
//			KeyLength, 			//密钥字节数 16
//			*IV, 				//随机数 4个字节
//			IVSize, 			//随机数字节数 4
//			*Header, 			//填null,附加信息ADD：用于生成Mac
//			HeaderSize, 		//填0
//			*Tag, 				//输出密文的哈希，mac：消息认证码
//			TagSize, 			//输出密文的哈希字节数
//			*OutputMessage); 	//输出的密文
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
uint8_t ReadParaInfo(uint8_t* InputMessage,
    uint32_t			InputMessageLength,
    uint8_t* 			KValue,
    uint32_t			KeyLength,
    uint8_t* 			IV,
    uint32_t			IVSize,
    uint8_t* 			Header,
    uint32_t			HeaderSize,
    uint8_t* 			Tag,
    uint32_t			TagSize,
    uint8_t* 			OutputMessage)
{
    mbedtls_gcm_context ctx;
    int ret = 0;
	g_byWSDParaFlag = 0xAC;
    mbedtls_gcm_init_my(&ctx);
    ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, KValue, KeyLength * 8);

    ret = mbedtls_gcm_crypt_and_tag(&ctx,
        MBEDTLS_GCM_ENCRYPT,
        InputMessageLength,
        IV,
        IVSize,
        Header,
        HeaderSize,
        InputMessage,
        OutputMessage,
        TagSize,
        Tag);

    mbedtls_gcm_free(&ctx);
	g_byWSDParaFlag = 0;
    if(ret)
        return(FALSE);
    else
        return(TRUE);
}

//--------------------------------------------------
//功能描述:  获取参数信息
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetParaInfo( BYTE *Ptr )
{
	INT PARA_ID = 0x1FFF7A10;
	BYTE i;

	for(i = 0; i < 12; i++)
	{
		Ptr[i] = *(BYTE*)(PARA_ID + i);
	}
}

//--------------------------------------------------
//功能描述:  检查参数信息
//         
//参数:      
//         
//返回值:    
//         
//备注:   license结果			参数设置开关当前值						最终结果(运行通过的OI)
//			FALSE					0x0000					除了厂内模式(相关参数设置)，初始化,分钟冻结之外的所有报文均可通过
//			TRUE					------					任何报文均可通过
//			FALSE					0x8A8A					只允许参数设置开关，厂内模式，初始化,校时，运行状态字通过
//			TRUE					------					任何报文均可通过
//--------------------------------------------------
BYTE  api_CheckParaInfo(BYTE *CurOI)
{
	BYTE Flag = 0, Parameter[12], Tag[16], OutputMessage[16], ParaSafe[16];
	BYTE KValue[16] = {5, 1, 2, 6, 4, 3, 6, 7, 8, 6, 2, 4, 5, 2, 9, 0};
	BYTE IV[4] = {9, 8, 3, 3};
    BYTE j;

	for(j = 0; j < sizeof(LegalOITableDefNS)/sizeof(DWORD); j++)
	{
		if(memcmp(CurOI,(BYTE*)&LegalOITableDefNS[j],sizeof(DWORD)) == 0)//判断是否为非法OI
		{
			memset(Tag, 0xFF,sizeof(Tag));
			memset(OutputMessage, 0xFF,sizeof(OutputMessage));
			memset(ParaSafe, 0xFF,sizeof(ParaSafe));
			
			GetParaInfo(Parameter);
			if(ReadParaInfo(Parameter, 12, KValue, 16, IV, 4, NULL, 0, Tag, sizeof(Tag), OutputMessage))
			{
				if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaInfoSafeRom.Para),sizeof(TParaInfoSafeRom),ParaSafe))
				{
					if((memcmp(OutputMessage, ParaSafe, 12)) == 0)
					{
						Flag = TRUE;//license正确
					}
					else
					{
						Flag = FALSE;//license错误
					}
				}
				else
				{
					Flag = FALSE;//eep错误
				}
			}
			else
			{
				Flag = FALSE;//加密错误
			}
			break; //如果进来判断，无论什么结果，都break，只进入一次
		}
		else
		{
			Flag = TRUE;//参数设置开关也会返回ture
		}
	}

	if(Flag == FALSE)//非法OI导致的错误
	{
		if(CheckParaRes == 0x8A8A)//出厂参数设置开关（打开）
		{
			for(j = 0; j < (sizeof(LegalOITableDefNS)/sizeof(DWORD))-1; j++)//开关打开的情况下，运行通过的OI，和LegalOITableDefNS有交集
			{
				if(memcmp(CurOI,(BYTE*)&LegalOITableDefNS[j],sizeof(DWORD)) == 0)//判断是否为合法OI
				{
					Flag = TRUE;
					break;
				}
				else
				{
					Flag = FALSE;
				}
			}
		}
		else
		{
			Flag = FALSE;
		}
	}

	return Flag;
}
