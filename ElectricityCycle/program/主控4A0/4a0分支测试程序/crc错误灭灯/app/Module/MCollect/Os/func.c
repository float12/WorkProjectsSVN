
#include "__define.h"
#include <string.h>

static BOOL IsBigEndian(void)
{
INT a=1 ; 

    if(((char*)&a)[sizeof(INT)-1] == 1)
        return TRUE ;
    else
        return FALSE ;
}

static WORD SwapWord(WORD iw)
{
WORD w;
BYTE *sp=(BYTE *)&iw;
BYTE *dp=(BYTE *)&w;

	dp[0]=sp[1];
	dp[1]=sp[0];
	return w;
}

DWORD SwapDWord(DWORD idw)
{
DWORD dw;
BYTE *sp=(BYTE *)&idw;
BYTE *dp=(BYTE *)&dw;

	dp[0]=sp[3];
	dp[1]=sp[2];
	dp[2]=sp[1];
	dp[3]=sp[0];
	return dw;
}



WORD MW(BYTE Hi,BYTE Lo)
{
	return (WORD)((Hi<<8)+Lo);
}

DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL)
{
	return (DWORD)((HH<<24) | (HL<<16) | (LH<<8) | LL);
}

DWORD MDW2(WORD H,WORD L)
{
	return MDW( HIBYTE(H), LOBYTE(H), HIBYTE(L), LOBYTE(L) );
}

DWORD SearchHead1(HPARA hBuf, DWORD Len,DWORD Offset1,BYTE Key1)
{
BYTE *Buf=(BYTE *)hBuf;
DWORD i;

	for(i=0;i<(DWORD)(Len-Offset1);i++)
	{
		if(Buf[Offset1+i] == Key1)
			return i;
	}
	return i;
}

DWORD SearchHead2(HPARA hBuf, DWORD Len,DWORD Offset1,BYTE Key1,DWORD Offset2,BYTE Key2)
{
BYTE *Buf = (BYTE *)hBuf;
DWORD i;
DWORD MLen;
	
	if(Offset1<Offset2)
		MLen=Offset2;
	else
		MLen=Offset1;
	for(i=0;i<(DWORD)(Len-MLen);i++)
	{
		if(Buf[Offset1+i]==Key1&&Buf[Offset2+i]==Key2)
			return i;
	}
	return i;
}

//*******************************************************************************************//
//  �������ƣ�BCDTOBIN                                  
//  �������ܣ���һ���ֽڵ�BCD��ת��ΪBIN��              
//  ���������BYTE byIn,�����һ���ֽ�            
//  ����ֵ��  BYTE byOut,�����һ���ֽ�                
//*******************************************************************************************//
BYTE   Bcd2Bin(BYTE byIn)
{//OK
BYTE byOut;

	byOut=((byIn>>4)&0xf)*10+(byIn&0xf);
	return byOut;
}

//*******************************************************************************************//
//  �������ƣ�BCDTOBIN                                
//  �������ܣ���һ���ֵ�BCD��ת��ΪBIN��                 
//  ���������WORD wIn,�����һ����                      
//  ����ֵ��  WORD wOut,�����һ����                       
//*******************************************************************************************//
WORD WBcd2Bin(WORD wIn)
{//OK
BYTE byHigh,byLow;
WORD wOut;

	byHigh=HIBYTE(wIn);
	byLow=LOBYTE(wIn);
	wOut=((byHigh>>4)&0xf)*1000+(byHigh&0xf)*100+((byLow>>4)&0xf)*10+(byLow&0xf);
	return wOut;
}

DWORD DWBcd2Bin(DWORD dwIn)
{//NO TEST
BYTE HH,HL,LH,LL;
DWORD dwOut;
	HH=HHBYTE(dwIn);
	HL=HLBYTE(dwIn);
	LH=LHBYTE(dwIn);
	LL=LLBYTE(dwIn);
//12345678
	dwOut=
		 ((HH>>4)&0x0F)*10000000 +
		 (HH&0x0F)*1000000 +
		 ((HL>>4)&0x0F)*100000 +
		 (HL&0x0F)*10000 +
		 ((LH>>4)&0x0F)*1000 +
		 (LH&0x0F)*100 +
		 ((LL>>4)&0x0F)*10 +
		 (LL&0x0F);
			
	return dwOut;
}

//Binת��ΪBcd
BYTE   Bin2Bcd(BYTE byIn)
{
BYTE byOut;
BYTE d1,d2;

	d1=byIn/10;
	d2=byIn%10;
	byOut=d1;
	byOut<<=4;
	byOut+=d2;
	return byOut;
}

WORD  WBin2Bcd(WORD wIn)
{
WORD wOut;
WORD d[4];
int i;
	for(i=3;i>=0;i--)
	{
		d[i]=wIn%10;
		wIn/=10;
	}
	wOut=0;
	for(i=0;i<4;i++)
	{
		wOut<<=4;
		wOut+=d[i];
	}
	return wOut;
}

DWORD DWBin2Bcd(DWORD dwIn)
{
DWORD dwOut;
DWORD d[8];
int i;
	for(i=7;i>=0;i--)
	{
		d[i]=dwIn%10;
		dwIn/=10;
	}
	dwOut=0;
	for(i=0;i<8;i++)
	{
		dwOut<<=4;
		dwOut+=d[i];
	}
	return dwOut;
}

//BCD��ʮ������
BYTE Bcd2Dec(BYTE byBcd)
{
	if((byBcd>='0')&&(byBcd<='9'))
	{
		return byBcd-'0';
	}	
	if((byBcd>='A')&&(byBcd<='F'))
	{
		return byBcd-'A'+10;
	}
	if((byBcd>='a')&&(byBcd<='f'))
	{
		return byBcd-'a'+10;
	}
	return 0;
}

//ʮ��������BCD
BYTE Dec2Bcd(BYTE byDec)
{
	if(byDec<=9)
	{
		return byDec+'0';
	}	
	if((byDec>=0xa)&&(byDec<=0x0f))
	{
		return byDec+'A'-10;
	}
	return '0';
}

INT ASCII2INT(char *p,INT MaxLen)
{
//MaxLen:��󳤶�
INT iRc=0;

    if(MaxLen==0)
		MaxLen=100;
	while(*p && (MaxLen--))
	{
		if ((*p<'0')||(*p>'9'))
		{//������ֹͣ
			return iRc;
		}
		iRc	*= 10;
		iRc	+= *p-'0';
		p++;
	}
	return iRc;
}

INT ASCII2INTEX(char *p,INT MaxLen)
{
//MaxLen:��󳤶�
INT iRc=0;

    if(MaxLen==0)
		MaxLen=100;
	while(*p && (MaxLen--))
	{
		if ((*p>='0')&&(*p<='9'))
		{//������ֹͣ
			iRc	*= 10;
			iRc	+= *p-'0';
		}		
		p++;
	}
	return iRc;
}

BYTE AscII2Hex(BYTE byData)
{
	if(byData <= 0x39)
		return (byData - '0');
	if(byData >= 0x41 && byData<=0x46)
		return (byData - 'A' + 0x0A);
	if(byData >= 0x61 && byData<=0x66)
		return (byData - 'a' + 0x0A);
	
	return 0;
}

DWORD Asc2Hex(BYTE * pbyBuf,BYTE byLen)		
{
	BYTE byDate[5]={0,0,0,0,0};
	DWORD temp;
	BYTE i;
	
	for(i=0;i<byLen;i++)
	{
		byDate[5-byLen+i] = AscII2Hex(pbyBuf[i]);
	}
	temp=byDate[0]*10000+byDate[1]*1000+byDate[2]*100+byDate[3]*10+byDate[4];
    return temp;
}

DWORD DWCheckSum(HPARA pBuf,DWORD dwLen)
{
BYTE *p=(BYTE *)pBuf;
DWORD dwSum=0;	
DWORD i;

	for(i=0;i<dwLen;i++)
	{
		dwSum += p[i];
	}
	return dwSum;
}

BYTE CheckSum(HPARA pBuf,DWORD dwLen)
{
	return (BYTE)DWCheckSum(pBuf,dwLen);
}

WORD CRC16_1021(BYTE *pBuf,DWORD count)
{
DWORD dwI,dwJ;
WORD wCRC=0x6203,wVal;

	for(dwI=0;dwI<count;dwI++)
	{
		wVal	= (WORD)(pBuf[dwI]<<8);
		for(dwJ=0;dwJ<8;dwJ++)
		{
			if((short)(wVal ^ wCRC) < 0)
				wCRC	= (wCRC<<1)^0x1021;
			else
				wCRC	<<= 1;
			wVal<<= 1;
		}
	}
	return wCRC;
}

void SetDot  (BYTE *pBuf,  DWORD DotNo,DWORD dwMax)
{//��pBuf��DotNo�����ñ�ǵ�
DWORD ByteNo; //DotNo�������ֽں�
DWORD BitNo;  //DotNo���ֽ���������λ��
	
	if(NULL == pBuf)
		return;
	if(DotNo>= dwMax )//Խ��
	   	return ;
    ByteNo = DotNo>>3;
	BitNo  = DotNo&0x7;
	pBuf[ByteNo] |= (BYTE)(1<<BitNo);
}

void ClearDot(BYTE *pBuf,DWORD DotNo,DWORD dwMax)
{//��pBuf��DotNo�������ǵ�
DWORD ByteNo; //DotNo�������ֽں�
DWORD BitNo;  //DotNo���ֽ���������λ��
 	
	if(NULL == pBuf)
		return;

	if(DotNo>= dwMax )//Խ��
      	return ;
	
    ByteNo = DotNo>>3;
	BitNo  = DotNo&0x7;
	pBuf[ByteNo] &= ~(1<<BitNo);
}

BYTE GetDot  (BYTE *pBuf, DWORD DotNo,DWORD dwMax)
{//��ȡpBuf��DotNo����ǵ�
DWORD ByteNo; //DotNo�������ֽں�
DWORD BitNo;  //DotNo���ֽ���������λ��

	if(NULL == pBuf)
		return 0;
	if(DotNo>= dwMax )//Խ��
      	return 0;

    ByteNo = DotNo>>3;
	BitNo  = DotNo&7;

	return pBuf[ByteNo] & (1<<BitNo);
}

void InvertBuf(WORD wBufLen, BYTE *pBuf)
{//�������������ݷ�����
WORD i,L;
BYTE b;

	L = wBufLen / 2;
	for(i=0; i<L; i++)
	{
		b = pBuf[i];
		pBuf[i] = pBuf[wBufLen-i-1];
		pBuf[wBufLen-i-1] = b;
	}
}

///////////////////////////////////////////////////////////////
//	�� �� �� : IsBCD
//	�������� : �Ƿ��Ǻϸ��BCD��ʽ
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2016��5��11��
//	�� �� ֵ : BOOL
//	����˵�� : DWORD dwData
///////////////////////////////////////////////////////////////
BOOL IsBCD(DWORD dwData)
{
BYTE byI;
DWORD dwTmp=dwData;

	for(byI=0;byI<8;byI++)
	{
		if((dwTmp & 0x0f) > 9)
			return FALSE;
		dwTmp  >>= 4;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
//  ��������: BCC
//  ��������: �󻺳�����ָ�����ֽ��������ֵ
//  �������: �������Ŀ�ʼ��ַ������ĳ���
//  ����ֵ��  ���ֵ
//-----------------------------------------------------------------------------
BYTE BCC(BYTE *pBuf,DWORD dwLen)
{
	BYTE x;
	DWORD i;
	
	x = pBuf[0];
	for(i=1;i<dwLen;i++)
	{
		x ^= pBuf[i];
	}
	return x;
}