//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		���������ӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define CRC32_INIT  0x55555555
#define CRC32_XOROT 0xffffffff

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
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

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: һ���ֽڵ�BCD������ת��ΪHEX������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����HEX������
//
//��ע:   
//-----------------------------------------------
BYTE lib_BBCDToBin(BYTE In)
{
	BYTE   Hex;
	
	if( lib_CheckBCD( In ) == FALSE )
	{
		ASSERT( FALSE, 0 );
	}
    
    Hex = In & 0xF0;
    Hex >>= 1;
    Hex += (Hex>>2);
    Hex += In & 0x0F;
    return Hex;
}


//-----------------------------------------------
//��������: ����ֽڵ�BCD������ת��ΪHEX������
//
//����: 
//          WORD Num                ���ݸ���
//
//			pIn[in]					Ҫת��������
//         
//          BYTE* pOut              ���������
//
//����ֵ:  	ת�����HEX������
//
//��ע:   
//-----------------------------------------------
void lib_MultipleBBCDToBin( BYTE *pIn, BYTE *pOut, WORD Num )
{
    WORD i;

    for( i=0; i<Num; i++ )
    {
        pOut[i] = lib_BBCDToBin( pIn[i] );
    }
}

//-----------------------------------------------
//��������: ����ֽڵ�HEX������ת��ΪBCD������
//
//����: 
//          WORD Num                ���ݸ���
//
//			pIn[in]					Ҫת��������
//         
//          BYTE* pOut              ���������
//
//����ֵ:  	ת�����BCD������
//
//��ע:   
//-----------------------------------------------
void lib_MultipleHEXToBBCD( BYTE *pIn, BYTE *pOut, WORD Num )
{
    WORD i;

    for( i=0; i<Num; i++ )
    {
        pOut[i] = lib_BBinToBCD( pIn[i] );
    }
}


//-----------------------------------------------
//��������: һ���ֽڵ�HEX������ת��ΪBCD������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����BCD������
//
//��ע:   
//-----------------------------------------------
BYTE lib_BBinToBCD(BYTE In)
{
	ASSERT( In<=99, 0 );
	
	In %= 100;
	return (In / 10) * 0x10 + (In%10);
}


//-----------------------------------------------
//��������: һ��WORD���͵�BCD������ת��ΪHEX������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����HEX������
//
//��ע:   
//-----------------------------------------------
WORD lib_WBCDToBin(WORD In)
{
	//����ط������ж��Ƿ�ΪBCD�룬BBCDToBina�����������
	return lib_BBCDToBin( (In>>8)&0xff ) * 100 + lib_BBCDToBin(In&0xff);
}


//-----------------------------------------------
//��������: һ��WORD���͵�HEX������ת��ΪBCD������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����BCD������
//
//��ע:   
//-----------------------------------------------
WORD lib_WBinToBCD(WORD In)
{
	In %= 10000;
	//����ط������ж��Ƿ�BCD���ﷶΧ��BBinToBCDa�����������
	return lib_BBinToBCD( (In/100) ) * 0x100 + lib_BBinToBCD(In%100);
}


//-----------------------------------------------
//��������: һ��DWORD���͵�BCD������ת��ΪHEX������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����HEX������
//
//��ע:   
//-----------------------------------------------
DWORD lib_DWBCDToBin(DWORD In)
{
	//����ط������ж��Ƿ�ΪBCD�룬BBCDToBina�����������
	return ((DWORD)lib_WBCDToBin( (In>>16)&0xffff )) * 10000 + (DWORD)lib_WBCDToBin(In&0xffff);
}


//-----------------------------------------------
//��������: һ��DWORD���͵�HEX������ת��ΪBCD������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����BCD������
//
//��ע:   
//-----------------------------------------------
DWORD lib_DWBinToBCD(DWORD In)
{
	In %= 100000000;
	//����ط������ж��Ƿ�BCD���ﷶΧ��BBinToBCDa�����������
	return ((DWORD)lib_WBinToBCD( (In/10000) )) * 0x10000 + (DWORD)lib_WBinToBCD(In%10000);
}

//-----------------------------------------------
//��������: һ��SQWORD���͵�BCD������ת��ΪHEX������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����HEX������
//
//��ע:   
//-----------------------------------------------
SQWORD lib_QWBCDToBin(SQWORD In)
{
	//����ط������ж��Ƿ�ΪBCD�룬BBCDToBina�����������
	return ((SQWORD)lib_DWBCDToBin( (In>>32)&0xffffffff )) * 100000000 + (SQWORD)lib_DWBCDToBin(In&0xffffffff);
}

//-----------------------------------------------
//��������: һ��SQWORD���͵�HEX������ת��ΪBCD������
//
//����: 
//			In[in]					Ҫת��������
//                    
//����ֵ:  	ת�����BCD������
//
//��ע:   
//-----------------------------------------------
SQWORD lib_QWBinToBCD(SQWORD In)
{
	In %= (SQWORD)10000000000000000;
	//����ط������ж��Ƿ�BCD���ﷶΧ��BBinToBCDa�����������
	return ((SQWORD)lib_DWBinToBCD( (In/100000000) )) * 0x100000000 + (SQWORD)lib_DWBinToBCD(In%100000000);
}


//-----------------------------------------------
//��������: ����10��n���ݵ�ֵ
//
//����: 
//			PowNum[in]			��
//                    
//����ֵ:  	������ֵ
//
//��ע:   û���жϷ�Χ�����һ��DWORD
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
//��������: ���һ���ֽڵ������Ƿ�ΪBCD��
//
//����: 
//			In[in]					Ҫ�жϵ�����
//                    
//����ֵ:  	TRUE:���ݷ���BCD��		FALSE:���ݲ���BCD��
//
//��ע:   
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
//��������: ���㵥�ֽ��ۼӺͣ�����һ���ֽڵĲ�������
//
//����: 
//	ptr[in]:	���뻺��
//
//	Length[in]:	���峤��
//                    
//����ֵ:	���ֽ��ۼӺ�
//
//��ע: 
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
//��������: ����CRC32У��
//
//����: 
//	pBuf[in]:	���뻺��
//
//	wSize[in]:	���峤�ȣ�������У��
//                    
//����ֵ:	CRC32У��
//
//��ע: 
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
//��������: �ȼ�黺����У����ȷ�ԣ��������ȷ���������У����䵽У������У��̶���Ϊ��CRC32
//
//����: 
//			p[in]				Ҫ����У�������ָ��
//			Size[in]			����洢���ݴ�С������У��
//			Fix[in]				�Ƿ��޸������ڵ�У�� 0�����޸� 1���޸�
//                    
//����ֵ:  	TRUE:�����ڵ�У����ȷ	FALSE:�����ڵ�У�鲻��ȷ
//
//��ע:   
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
			// �����ȷУ��
			memcpy( (p+Size-sizeof(DWORD)), (BYTE *)&CRC2, sizeof(DWORD) );	
		}

		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------
//��������: �����Ա��ת����
//
//����:		Buf[in]	��������
//			Len[in]	���ݳ���
//����ֵ: 	��
//		   
//��ע:   	
//-----------------------------------------------
void lib_InverseData( BYTE *Buf, WORD Len )
{
	WORD i;
	BYTE Temp08;
	
	for( i=0; i<Len/2; i++ )
	{
		Temp08 = Buf[i];
		Buf[i] = Buf[Len-1-i];
		Buf[Len-1-i] = Temp08;//��������
	}
}


//--------------------------------------------------
//��������:  ��BufB�����ݵ����BufA
//
//����:      BufA[out] 		������ݵĻ���
//
//           BufB[in]		�������ݵĻ���
//
//           Len[in]		���ݳ���
//
//����ֵ:
//
//��ע����:  
//--------------------------------------------------
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len )
{
	BYTE n;
	BYTE Buf[255];
	
	memcpy( Buf, BufB, Len );
	
	for( n=0; n<Len; n++ )
	{
		BufA[n] = Buf[Len - 1 - n]; //��������
	}
}

//--------------------------------------------------
//��������:  ��len��BufB�����ݰ�λ�����BufA  bit15~bit0  -> bit0~bit15
//         
//����:      BufA[out] 		������ݵĻ���  �����������ָ��һ��
//         
//           BufB[in]		�������ݵĻ���
//         
//           Len[in]		���ݳ���
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void lib_ExchangeBit(BYTE *BufA, BYTE *BufB, BYTE Len )
{
	BYTE i;
	for( i = 0; i < Len; i++ )
	{
		BufA[i] = BufB[i];
		BufA[i] = (((BufA[i] >> 1) & 0x55) | ((BufA[i] & 0x55) << 1));		//����ÿ��λ
		BufA[i] = (((BufA[i] >> 2) & 0x33) | ((BufA[i] & 0x33) << 2));  	//����ÿ��λ��ǰ��λ�ͺ���λ
		BufA[i] = (( BufA[i] >> 4 ) | (BufA[i] << 4));				  		//����ǰ��λ�ͺ���λ
	}

	lib_InverseData( BufA, Len );//��������
}
//--------------------------------------------------
//��������:  �ж��ֽ��Ƿ�Ϊbcd��
//         
//����:      BYTE Value[in]
//         
//����ֵ:    BYTE   TRUE:BCD FALSE:��BCD  
//         
//��ע����:  ��
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
//��������:  �ж��ֽڴ��Ƿ�Ϊbcd��
//         
//����:      BYTE *Buf[in]
//         
//           BYTE Len[in]
//         
//����ֵ:    BYTE   TRUE:BCD FALSE:��BCD  
//         
//��ע����:  ��
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
//��������:  ��ָ���ֽ������������ݽ������
//         
//����:      BYTE *BufA[in]  ��������( �����Ϊ������ݽ������)
//         
//           BYTE *BufB[in]  ��������
//         
//           BYTE Num[in]	   �ֽڸ���
//         
//����ֵ:    
//         
//��ע����:  ��
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
//��������: �ж�ָ��p����������Ƿ���AssignNumָ��������
//
//����:    P[in]       ���ݵĻ���
//			AssignNum[in] Ҫ�жϵ�ָ������
//         Length[in]  ���ݵĳ���
//
//����ֵ:TRUE��FALSE
//
//��ע:
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


