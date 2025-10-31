#ifndef __USRFUN_H__
#define __USRFUN_H__
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

//������ת��ΪС��ģʽ
WORD WX86(WORD wVal);
DWORD SwapDWord(DWORD idw);
DWORD DWX86(DWORD dwVal);


WORD MW(BYTE Hi,BYTE Lo);
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL);
DWORD MDW2(WORD H,WORD L);

///////////////////////////////////////////////////////////////
//	�� �� �� : SearchHead1
//	�������� : ���ҷ��������ı���ͷ����1
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��7��22��
//	�� �� ֵ : DWORD Ӧ�ӵ����ֽڸ���
//	����˵�� : HPARA hBuf,���Ļ�����ָ��
//				 DWORD dwLen,��ǰ���ĳ���
//				DWORD dwOffset1,ƫ����1
//				BYTE Key1 �ؼ��ֽ�1
///////////////////////////////////////////////////////////////
DWORD SearchHead1(HPARA hBuf, DWORD dwLen,DWORD dwOffset1,BYTE Key1);

///////////////////////////////////////////////////////////////
//	�� �� �� : SearchHead2
//	�������� : ���ҷ��������ı���ͷ����2
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2015��7��22��
//	�� �� ֵ : DWORD Ӧ�ӵ����ֽڸ���
//	����˵�� : HPARA hBuf,���Ļ�����ָ��
//				 DWORD dwLen,��ǰ���ĳ���
//				DWORD dwOffset1,ƫ����1
//				BYTE Key1,�ؼ��ֽ�1
//				DWORD dwOffset2,ƫ����2
//				BYTE Key2 �ؼ��ֽ�2
///////////////////////////////////////////////////////////////
DWORD SearchHead2(HPARA hBuf, DWORD dwLen,DWORD dwOffset1,BYTE Key1,DWORD dwOffset2,BYTE Key2);

//Bcdת��ΪBin
BYTE  Bcd2Bin(BYTE byIn);
WORD  WBcd2Bin(WORD wIn);
DWORD DWBcd2Bin(DWORD dwIn);

//Binת��ΪBcd
BYTE  Bin2Bcd(BYTE byIn);
WORD  WBin2Bcd(WORD wIn);
DWORD DWBin2Bcd(DWORD dwIn);

//���ø�ʽת��
BYTE Bcd2Dec(BYTE byBcd); //BCD��ת10������
BYTE Dec2Bcd(BYTE byDec);//10������תBCD��
INT ASCII2INT(char *p,INT MaxLen);//ASC�ַ���ת��������
INT ASCII2INTEX(char *p,INT MaxLen);//�����������ַ�����
//����У�����㷨
DWORD DWCheckSum(HPARA pvBuf,DWORD dwLen);
BYTE  CheckSum(HPARA pvBuf,DWORD dwLen); //�ۼӺͼ���
WORD CRC16_1021(BYTE *pBuf,DWORD count);//CRC16 1021У�����㷨
//���á�������ʶ
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
