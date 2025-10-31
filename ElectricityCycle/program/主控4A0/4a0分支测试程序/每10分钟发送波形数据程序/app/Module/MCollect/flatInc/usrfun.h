#ifndef __USRFUN_H__
#define __USRFUN_H__
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

//将数据转换为小端模式
WORD WX86(WORD wVal);
DWORD SwapDWord(DWORD idw);
DWORD DWX86(DWORD dwVal);


WORD MW(BYTE Hi,BYTE Lo);
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL);
DWORD MDW2(WORD H,WORD L);

///////////////////////////////////////////////////////////////
//	函 数 名 : SearchHead1
//	函数功能 : 查找符合条件的报文头函数1
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月22日
//	返 回 值 : DWORD 应扔掉的字节个数
//	参数说明 : HPARA hBuf,报文缓冲区指针
//				 DWORD dwLen,当前报文长度
//				DWORD dwOffset1,偏移量1
//				BYTE Key1 关键字节1
///////////////////////////////////////////////////////////////
DWORD SearchHead1(HPARA hBuf, DWORD dwLen,DWORD dwOffset1,BYTE Key1);

///////////////////////////////////////////////////////////////
//	函 数 名 : SearchHead2
//	函数功能 : 查找符合条件的报文头函数2
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2015年7月22日
//	返 回 值 : DWORD 应扔掉的字节个数
//	参数说明 : HPARA hBuf,报文缓冲区指针
//				 DWORD dwLen,当前报文长度
//				DWORD dwOffset1,偏移量1
//				BYTE Key1,关键字节1
//				DWORD dwOffset2,偏移量2
//				BYTE Key2 关键字节2
///////////////////////////////////////////////////////////////
DWORD SearchHead2(HPARA hBuf, DWORD dwLen,DWORD dwOffset1,BYTE Key1,DWORD dwOffset2,BYTE Key2);

//Bcd转换为Bin
BYTE  Bcd2Bin(BYTE byIn);
WORD  WBcd2Bin(WORD wIn);
DWORD DWBcd2Bin(DWORD dwIn);

//Bin转换为Bcd
BYTE  Bin2Bcd(BYTE byIn);
WORD  WBin2Bcd(WORD wIn);
DWORD DWBin2Bcd(DWORD dwIn);

//常用格式转换
BYTE Bcd2Dec(BYTE byBcd); //BCD码转10进制码
BYTE Dec2Bcd(BYTE byDec);//10进制码转BCD码
INT ASCII2INT(char *p,INT MaxLen);//ASC字符串转换成整数
INT ASCII2INTEX(char *p,INT MaxLen);//遇到非数字字符跳过
//常用校验码算法
DWORD DWCheckSum(HPARA pvBuf,DWORD dwLen);
BYTE  CheckSum(HPARA pvBuf,DWORD dwLen); //累加和计算
WORD CRC16_1021(BYTE *pBuf,DWORD count);//CRC16 1021校验码算法
//设置、清除点标识
void SetDot(BYTE *pBuf,  DWORD DotNo,DWORD dwMax);
void ClearDot(BYTE *pBuf,DWORD DotNo,DWORD dwMax);
BYTE GetDot(BYTE *pBuf, DWORD DotNo,DWORD dwMax);

void InvertBuf(WORD wBufLen, BYTE *pBuf);
BOOL IsBCD(DWORD dwData);

BYTE BCC(BYTE *pBuf,DWORD dwLen);


#ifndef _max
 #define _max(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
 #define min(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#define CHECKPTR( ptr, Ret )	\
	if( ptr == NULL )			\
	{							\
		return Ret;				\
	}
#define CHECKPTR0( ptr )	\
	if( ptr == NULL )		\
	{						\
		return;				\
	}

 #ifdef __cplusplus
  #if __cplusplus
	}
  #endif
 #endif
#endif
