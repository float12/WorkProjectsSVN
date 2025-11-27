//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		公共函数接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define CRC32_INIT  0x55555555
#define CRC32_XOROT 0xffffffff
#if 0
static BYTE strtoday[100]=
{
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,
	0x03,0x01,0x0A,0x04,0x01,0x01,0x01,0x01,0x01,0x01,
	0x0C,0x0B,0x09,0x02,0x01,0x01,0x01,0x01,0x01,0x08,
	0x01,0x06,0x01,0x07,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};
#endif

void GetAppDateTimeString(BYTE* Info)
{
	TRealTimer tm;
#if 0	
	tm.wYear=atoi(&__DATE__[7]);
	tm.Mon=strtoday[__DATE__[0]+__DATE__[1]-__DATE__[2]];
	tm.Day=atoi(&__DATE__[4]);
	tm.Hour=atoi(&__TIME__[0]);
	tm.Min=atoi(&__TIME__[3]);
	tm.Sec=atoi(&__TIME__[6]);
#else
	tm.wYear=2023;
	tm.Mon=12;
	tm.Day=10;
#endif
	
	sprintf((char *)Info,"%02d%02d%02d", tm.wYear%100, tm.Mon, tm.Day);


	//Info[0] = tm.wYear%100;
	//Info[1] = tm.Mon;
	//Info[2] = tm.Day;
	//Info[3] = tm.Hour;
	//Info[4] = tm.Min;
	//Info[5] = tm.Sec;

}
void GetAppDateTime(TRealTimer* time)
{
#if 0	
	time->wYear=atoi(&__DATE__[7]);
	time->Mon=strtoday[__DATE__[0]+__DATE__[1]-__DATE__[2]];
	time->Day=atoi(&__DATE__[4]);
	time->Hour=atoi(&__TIME__[0]);
	time->Min=atoi(&__TIME__[3]);
	time->Sec=atoi(&__TIME__[6]);
#else
	time->wYear=2021;
	time->Mon=6;
	time->Day=30;
	time->Hour=1;
	time->Min=0;
	time->Sec=0;
#endif
	
	
	//Info[0] = tm.wYear%100;
	//Info[1] = tm.Mon;
	//Info[2] = tm.Day;
	//Info[3] = tm.Hour;
	//Info[4] = tm.Min;
	//Info[5] = tm.Sec;
}
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static const DWORD CRC32_Table[256] =
{
   0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
   0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
   0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
   0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
   0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
   0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
   0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
   0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
   0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
   0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
   0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
   0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
   0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
   0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
   0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
   0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
   0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
   0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
   0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
   0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
   0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
   0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
   0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
   0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
   0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
   0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
   0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
   0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
   0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
   0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
   0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
   0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
   0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
   0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
   0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
   0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
   0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
   0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
   0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
   0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
   0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
   0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
   0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
   0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
   0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
   0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
   0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
   0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
   0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
   0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
   0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
   0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
   0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
   0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
   0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
   0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
   0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
   0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
   0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
   0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
   0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
   0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
   0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
   0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

//FCS lookup table as calculated by the table generator.
static const WORD fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 检查一个字节的数据是否为BCD码
//
//参数:
//			In[in]					要判断的数据
//
//返回值:  	TRUE:数据符合BCD码		FALSE:数据不是BCD码
//
//备注:
//-----------------------------------------------
BOOL lib_CheckBCD(BYTE In)
{
	if( (In&0x0f) > 0x09 )
	{
		return FALSE;
	}

	if( (In>>4) > 0x09 )
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 一个字节的BCD码数据转换为HEX码数据
//
//参数: 
//			In[in]					要转换的数据
//                    
//返回值:  	转换后的HEX码数据
//
//备注:   
//-----------------------------------------------
BYTE lib_BBCDToBin(BYTE In)
{
	BYTE   Hex;
	
	if( lib_CheckBCD( In ) == FALSE )
	{
		//ASSERT( FALSE, 0 );
	}
    
    Hex = In & 0xF0;
    Hex >>= 1;
    Hex += (Hex>>2);
    Hex += In & 0x0F;
    return Hex;
}


//-----------------------------------------------
//函数功能: 多个字节的BCD码数据转换为HEX码数据
//
//参数: 
//          WORD Num                数据个数
//
//			pIn[in]					要转换的数据
//         
//          BYTE* pOut              输出的数据
//
//返回值:  	转换后的HEX码数据
//
//备注:   
//-----------------------------------------------
void lib_MultipleBBCDToBin( WORD Num, BYTE* pIn, BYTE* pOut )
{
    WORD i;

    for( i=0; i<Num; i++ )
    {
        pOut[i] = lib_BBCDToBin( pIn[i] );
    }
}

//-----------------------------------------------
//函数功能: 一个字节的HEX码数据转换为BCD码数据
//
//参数:
//			In[in]					要转换的数据
//
//返回值:  	转换后的BCD码数据
//
//备注:
//-----------------------------------------------
BYTE lib_BBinToBCD(BYTE In)
{

	In %= 100;
	return (In / 10) * 0x10 + (In%10);
}
//-----------------------------------------------
//函数功能: 多个字节的BCD码数据转换为HEX码数据
//
//参数: 
//          WORD Num                数据个数
//
//			pIn[in]					要转换的数据
//         
//          BYTE* pOut              输出的数据
//
//返回值:  	转换后的HEX码数据
//
//备注:   
//-----------------------------------------------
void lib_MultipleBBinToBCD( WORD Num, BYTE* pIn, BYTE* pOut )
{
    WORD i;

    for( i=0; i<Num; i++ )
    {
        pOut[i] = lib_BBinToBCD( pIn[i] );
    }
}


//-----------------------------------------------
//函数功能: 一个WORD类型的BCD码数据转换为HEX码数据
//
//参数: 
//			In[in]					要转换的数据
//                    
//返回值:  	转换后的HEX码数据
//
//备注:   
//-----------------------------------------------
WORD lib_WBCDToBin(WORD In)
{
	//这个地方不再判断是否为BCD码，BBCDToBina这个函数判了
	return lib_BBCDToBin( (In>>8)&0xff ) * 100 + lib_BBCDToBin(In&0xff);
}


//-----------------------------------------------
//函数功能: 一个WORD类型的HEX码数据转换为BCD码数据
//
//参数: 
//			In[in]					要转换的数据
//                    
//返回值:  	转换后的BCD码数据
//
//备注:   
//-----------------------------------------------
WORD lib_WBinToBCD(WORD In)
{
	In %= 10000;
	//这个地方不再判断是否超BCD码表达范围，BBinToBCDa这个函数判了
	return lib_BBinToBCD( (In/100) ) * 0x100 + lib_BBinToBCD(In%100);
}

//-----------------------------------------------
//函数功能: 一个DWORD类型的HEX码数据转换为BCD码数据
//
//参数: 
//			In[in]					要转换的数据
//                    
//返回值:  	转换后的BCD码数据
//
//备注:   
//-----------------------------------------------
DWORD lib_DWBinToBCD(DWORD In)
{
	In %= 100000000;
	//这个地方不再判断是否超BCD码表达范围，BBinToBCDa这个函数判了
	return ((DWORD)lib_WBinToBCD( (In/10000) )) * 0x10000 + (DWORD)lib_WBinToBCD(In%10000);
}

//-----------------------------------------------
//函数功能: 计算10的n次幂的值
//
//参数: 
//			PowNum[in]			幂
//                    
//返回值:  	计算后的值
//
//备注:   没有判断范围，最大一个DWORD
//-----------------------------------------------
DWORD lib_MyPow10( BYTE PowNum )
{
	BYTE i;
	DWORD dw;
	
	dw = 1;
	for(i=0; i<PowNum; i++)
	{
		dw *= 10;
	}
	
	return dw;
}

//-----------------------------------------------
//函数功能: 计算单字节累加和，超出一个字节的部分舍弃
//
//参数: 
//	ptr[in]:	输入缓冲
//
//	Length[in]:	缓冲长度
//                    
//返回值:	单字节累加和
//
//备注: 
//-----------------------------------------------
BYTE lib_CheckSum(BYTE * ptr, WORD Length)
{
	WORD i;
	BYTE Sum = 0;

	for(i=0; i<Length; i++)
	{
		Sum += *ptr;
		ptr ++;
	}

	return Sum;
}

//-----------------------------------------------
//函数功能: 计算四字节累加和，超出四个字节的部分舍弃
//
//参数: 
//	ptr[in]:	输入缓冲
//
//	Length[in]:	缓冲长度
//                    
//返回值:	四字节累加和
//
//备注: 
//-----------------------------------------------
DWORD lib_DWORDCheckSum(BYTE * ptr, WORD Length)
{
	WORD i;
	DWORD Sum = 0;

	for(i=0; i<Length; i++)
	{
		Sum += *ptr;
		ptr ++;
	}

	return Sum;
}

//-----------------------------------------------
//函数功能: 计算CRC32校验
//
//参数: 
//	pBuf[in]:	输入缓冲
//
//	wSize[in]:	缓冲长度，不包括校验
//                    
//返回值:	CRC32校验
//
//备注: 
//-----------------------------------------------
DWORD lib_CheckCRC32( BYTE *pBuf, WORD wSize)
{
	DWORD dwValue;

	/* init the start value */
	dwValue = CRC32_INIT;

	/* calculate CRC32 */
	while (wSize --)
	{
		dwValue = CRC32_Table[(dwValue ^ *pBuf++) & 0xFF] ^ (dwValue >> 8);
	}
	/* XOR the output value */
	return dwValue ^ CRC32_XOROT;
}

//-----------------------------------------------
//函数功能: 先检查缓冲中校验正确性，如果不正确，将计算的校验填充到校验区，校验固定认为是CRC32
//
//参数: 
//			p[in]				要计算校验的数据指针
//			Size[in]			缓冲存储数据大小，包括校验
//			Fix[in]				是否修复缓冲内的校验 0：不修复 1：修复
//                    
//返回值:  	TRUE:缓冲内的校验正确	FALSE:缓冲内的校验不正确
//
//备注:   
//-----------------------------------------------
BOOL lib_CheckSafeMemVerify(BYTE * p, WORD Size, WORD Fix)
{
	DWORD	CRC1,CRC2;

	memcpy( (BYTE *)&CRC1, (p+Size-sizeof(DWORD)), sizeof(DWORD) );

	CRC2 = lib_CheckCRC32( p, (Size - sizeof(DWORD)) );

	if( CRC1 != CRC2 )
	{
		if( Fix == REPAIR_CRC )
		{
			// 填充正确校验
			memcpy( (p+Size-sizeof(DWORD)), (BYTE *)&CRC2, sizeof(DWORD) );	
		}

		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//函数功能: 数组成员翻转函数
//
//参数:		Buf[in]	输入数据
//			Len[in]	数据长度
//返回值: 	无
//		   
//备注:   	
//-----------------------------------------------
void lib_InverseData( BYTE *Buf, WORD Len )
{
	WORD i;
	BYTE Temp08;
	
	for( i=0; i<Len/2; i++ )
	{
		Temp08 = Buf[i];
		Buf[i] = Buf[Len-1-i];
		Buf[Len-1-i] = Temp08;//交换数据
	}
}


//--------------------------------------------------
//功能描述:  把BufB的数据倒序给BufA
//         
//参数:      BufA[out] 		输出数据的缓冲
//         
//           BufB[in]		输入数据的缓冲
//         
//           Len[in]		数据长度
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len )
{
    BYTE n;
    BYTE Buf[255];
    
    memcpy( Buf, BufB, Len );
    
    for( n=0; n<Len; n++ )
    {
        BufA[n] = Buf[Len - 1 - n];
    }
}

//--------------------------------------------------
//功能描述:  把len个BufB的数据按位倒序给BufA  bit15~bit0  -> bit0~bit15
//         
//参数:      BufA[out] 		输出数据的缓冲  可以输出缓冲指针一样
//         
//           BufB[in]		输入数据的缓冲
//         
//           Len[in]		数据长度
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void lib_ExchangeBit(BYTE *BufA, BYTE *BufB, BYTE Len )
{
	BYTE i;
	for( i = 0; i < Len; i++ )
	{
		BufA[i] = BufB[i];
		BufA[i] = (((BufA[i] >> 1) & 0x55) | ((BufA[i] & 0x55) << 1));		//交换每两位
		BufA[i] = (((BufA[i] >> 2) & 0x33) | ((BufA[i] & 0x33) << 2));  	//交换每四位的前两位和后两位
		BufA[i] = (( BufA[i] >> 4 ) | (BufA[i] << 4));				  		//交换前四位和后四位
	}

	lib_InverseData( BufA, Len );//交换数据
}
//--------------------------------------------------
//功能描述:  判断字节是否为bcd码
//         
//参数:      BYTE Value[in]
//         
//返回值:    BYTE   TRUE:BCD FALSE:非BCD  
//         
//备注内容:  无
//--------------------------------------------------
BYTE lib_IsBCD( BYTE Value )
{
	if( ((Value&0xF0)<=0x90) && ((Value&0x0F)<=0x09) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  判断字节串是否为bcd码
//         
//参数:      BYTE *Buf[in]
//         
//           BYTE Len[in]
//         
//返回值:    BYTE   TRUE:BCD FALSE:非BCD  
//         
//备注内容:  无
//--------------------------------------------------
BYTE lib_IsMultiBcd( BYTE *Buf, BYTE Len )
{
	BYTE i;
	
	for( i=0; i<Len; i++ )
	{
		if( lib_IsBCD(Buf[i]) != TRUE )		return FALSE;
	}
	
	return TRUE;
}
//--------------------------------------------------
//功能描述:  对指定字节数的两个数据进行异或
//         
//参数:      BYTE *BufA[in]  输入数据( 最后作为输出数据进行输出)
//         
//           BYTE *BufB[in]  输入数据
//         
//           BYTE Num[in]	   字节个数
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void lib_XorData( BYTE *BufA, BYTE *BufB, WORD Num )
{
	WORD i;

	for( i=0; i<Num; i++ )
	{
		BufA[i] ^= BufB[i];
	}
}

//--------------------------------------------------
//函数功能: 判断指针p里面的数据是否都是AssignNum指定的数据
//
//参数:    P[in]       数据的缓冲
//			AssignNum[in] 要判断的指定数据
//         Length[in]  数据的长度
//
//返回值:TRUE、FALSE
//
//备注:
//--------------------------------------------------
BOOL lib_IsAllAssignNum(BYTE *p, BYTE AssignNum, WORD Length)
{
	WORD i;
	for( i = 0; i < Length; i++ )
	{
		if( p[i] != AssignNum )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BYTE AllocBuf[8192];
void *api_AllocBuf(WORD *BufSize)
{
	return AllocBuf;
}

void api_FreeBuf(void *pBuf)
{
	return;
}

//-----------------------------------------------
//函数功能: fcs校验函数
//
//参数:		BYTE *cp  
//			WORD len   
//返回值:	WORD
//		   
//备注:
//-----------------------------------------------
 WORD fcs16(BYTE *cp, WORD len)
{
	WORD cpLen;
	WORD fcs;
	
	fcs = PPPINITFCS16;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	fcs ^= 0xffff; // complement  //此为FCS值
	return fcs;
}

 //-----------------------------------------------
 //函数功能: fcs校验函数
 //
 //参数:		BYTE *cp  
 //			WORD len   
 //返回值:	WORD
 //		   
 //备注:
 //-----------------------------------------------
 WORD fcs16_Ex(WORD wInitCrc, BYTE *cp, WORD len)
 {
	 WORD cpLen;
	 WORD fcs;
	 
	 fcs = wInitCrc;
	 cpLen = len;
	 while (cpLen--)
	 {
		 fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	 }
	 
	 //fcs ^= 0xffff; // complement  //此为FCS值
	 return fcs;
 }

 float GetStringToFloat(char* szText, int nLen)
 {
		int i;
	  char szTemp[30];
	 
		memset(szTemp,0,30);
	 if(nLen > 30)
		 return 0;
	 
	  for(i=0; i<nLen; i++)
	 {
			if(szText[i] >= 0x30 && szText[i] <= 0x39 || szText[i] == 0x2E)
			{
					szTemp[i] = szText[i];
			}
	 }
	 return atof(szTemp);
 }