//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������
//��������
//����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_dlt645.h"

#if (READMETER_PROTOCOL == PROTOCOL_645)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

#define WARNING_BIT	0x7ff
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
#pragma pack(1)
typedef struct _position
{
	BYTE location; //��λ
	WORD degree;   //��
	float min;	 //��
} position;
#pragma pack()

#pragma pack(1)
typedef struct _TOad40040200
{
	position longitude; //����
	position latitude;  //γ��
	DWORD high;			//�߶� cm
} TOad40040200;
#pragma pack()

typedef struct TUserIdTable_t
{
	BYTE bTcpConnectId;		//tcp ����id
	BYTE bMeterParaId;		//������� id
}TUserIdTable;
BYTE bUsedChannelNum = 1; //��ʹ��ͨ������ �ϵ���̽��
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
char bAddr[6] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};	 //���ͨ�ŵ�ַ 
char reverAddr[6]; //��ַ���𷽱㷢��
TSerial Serial;
ServerParaRam_t Serverpara[MAX_SPECIAL_USER_NUM] = {0};
char MeterVersion[35] = {0};
char LtuAddr[30] = {0};
DWORD PowerOnReadMeterFlag = 0; //�ϵ糭��ɹ���bitλ Ŀǰ�߸�
BYTE bHisReadMeter[MAX_SAMPLE_CHIP] = {0}; //�ϴγ���bitλ
QWORD qwReadMeterFlag[MAX_SAMPLE_CHIP] = {0};
BYTE bReadMeterRetry = 0;
TUserIdTable EpTcpUserTable[5] = {0};//Ŀǰ ���5�� 
BYTE g_EpTcpUserNum = MAX_SPECIAL_USER_NUM;// ����Э���û�����վ ����
BYTE g_EpTcpUserChannel = 0;//����Э���û�����վͨ��bit
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
TOtherPara OtherPara;
static BYTE Loop = 0;

#if (EVENT_REPORT == YES)
static BYTE bHisDIStatus;		 //��ǰң��״̬
static WORD bHisWarning = 0;
static WORD bWarningNumber = 0;
// static BYTE EventPowerOn = 0xAA; //��ֹ���ϵ���ϱ�
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//--------------------------------------------------
//��������:  ����վ��ѯ��� ���Ҷ�Ӧ��ϵ
//         
//����:      eMETER  �������л���id
//			eMODULE ����ģ������id
//         
//����ֵ:    
//         
//��ע:  	����ʱ ����bIDǰ��1  ����ʱ ����returnֵ ��1
//--------------------------------------------------
BYTE  api_GetTcpChannelTable( MeterorModu Para,BYTE bID )
{
	BYTE  i;

	if (Para == eMETER)
	{
		for (i = 0; i < 5; i++)
		{
			if (bID == EpTcpUserTable[i].bTcpConnectId)
			{
				// nwy_ext_echo("\r\n %d", EpTcpUserTable[i].bTcpConnectId);
				// nwy_ext_echo("\r\n %d", EpTcpUserTable[i].bMeterParaId);
				return (EpTcpUserTable[i].bMeterParaId);
			}
		}
		return 0;
	}
	else
	{
		for (i = 0; i < 5; i++)
		{
			if (bID == EpTcpUserTable[i].bMeterParaId)
			{
				// nwy_ext_echo("\r\n %d", EpTcpUserTable[i].bTcpConnectId);
				// nwy_ext_echo("\r\n %d", EpTcpUserTable[i].bMeterParaId);
				return (EpTcpUserTable[i].bTcpConnectId);
			}
		}
		return 0;
	}
}
//-----------------------------------------------
//��������:	PT CT����
//
//����:	bType[in]: ��ȡ����
//
//����ֵ:	����
//
//��ע:
//-----------------------------------------------
double api_GetPtandCT(BYTE bType)
{
	double dbData = 1;
	#if (PT_CT == YES)
	switch (bType)
	{
	case eType_PT:
		if (OtherPara.bPTSecond == 0)
		{
			dbData = 1;
		}
		else
		{

			dbData = (double)OtherPara.dwPTPrim / OtherPara.bPTSecond; //PT��ȣ��Ƿ��ӳ��Է�ĸ���ֵ
		}
		break;
	case eType_CT:
		if (OtherPara.bCTSecond == 0)
		{
			dbData = 1;
		}
		else
		{
			dbData = (double)OtherPara.dwCTPrim / OtherPara.bCTSecond; //CT��ȣ��Ƿ��ӳ��Է�ĸ���ֵ
		}
		break;
	case eType_PT_CT:
		if ((OtherPara.bCTSecond == 0) || (OtherPara.bPTSecond == 0))
		{
			dbData = 1;
		}
		else
		{
			dbData = (double)OtherPara.dwCTPrim * OtherPara.dwPTPrim / OtherPara.bCTSecond / OtherPara.bPTSecond;
		}
		break;
	default:
		break;
	}
	#endif
	return dbData;
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
	if ((In & 0x0f) > 0x09)
	{
		return FALSE;
	}

	if ((In >> 4) > 0x09)
	{
		return FALSE;
	}

	return TRUE;
}
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
	BYTE Hex;

	if (lib_CheckBCD(In) == FALSE)
	{
	}

	Hex = In & 0xF0;
	Hex >>= 1;
	Hex += (Hex >> 2);
	Hex += In & 0x0F;
	return Hex;
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
WORD MW(BYTE Hi, BYTE Lo)
{
	return (WORD)((Hi << 8) + Lo);
}
DWORD MDW(BYTE HH, BYTE HL, BYTE LH, BYTE LL)
{
	return (DWORD)((HH << 24) | (HL << 16) | (LH << 8) | LL);
}
static void nwy_hex_to_string(int len, char *data, char *buf)
{
	int i = 0;
	unsigned char highByte;
	unsigned char lowByte;

	for (i = 0; i < len; i++)
	{
		highByte = data[i] >> 4;
		lowByte = data[i] & 0x0f;

		highByte += 0x30;

		if (highByte > 0x39)
			buf[i * 2] = highByte + 0x07;
		else
			buf[i * 2] = highByte;

		lowByte += 0x30;
		if (lowByte > 0x39)
			buf[i * 2 + 1] = lowByte + 0x07;
		else
			buf[i * 2 + 1] = lowByte;
	}
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
//��ע����:  ��
//--------------------------------------------------
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len)
{
	BYTE n;
	BYTE Buf[255];

	memcpy(Buf, BufB, Len);

	for (n = 0; n < Len; n++)
	{
		BufA[n] = Buf[Len - 1 - n];
	}
}
//--------------------------------------------------
//��������:  16���� ת 2�����ַ���
//         
//����:      
//        buf[out]
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  HextoB( WORD wData,char* outbuf) 
{
	for (BYTE i = 0; i < 16; i++)
	{
		if (wData&(1<<i))
		{
			outbuf[15-i] = '1';
		}
		else
		{
			outbuf[15-i] = '0';
		}
	}
	outbuf[16] = '\0';
}
//-----------------------------------------------
//��������: ������ڲ�����ȡʱ��
//
//����: 
//			Type[in]				��Ҫ��ȡ��ʱ���ʽ
//									bit15: 0������������ʱ���� ����buf[0]������С��ģʽ   1�����ʱ���������� ����С��ģʽ
//									bit14: 0: Hex��  	1:BCD��
//									bit13-bit8:����
//									bit7:���Ƿ��2000  1����2000
//									bit6-bit0:����������ʱ����  1:��ʾ��Ҫ��λ��Ӧ������
//			pBuf[out]				��Ŷ���������
//                    
//����ֵ:  	���ض�ȡ�����ݳ���
//��ע:     ʱ�䶼��Hex�룬�궼��word����
//#define DATETIME_SECOND_FIRST			BIT15//����buf[0]
//#define DATETIME_BCD					BIT14//����������bcd
//#define DATETIME_20YY					(BIT7+BIT6)
//#define DATETIME_YY					BIT6
//#define DATETIME_MM					BIT5
//#define DATETIME_DD					BIT4
//#define DATETIME_WW					BIT3
//#define DATETIME_hh					BIT2
//#define DATETIME_mm					BIT1
//#define DATETIME_ss					BIT0
//
//#define DATETIME_YYMMDD				(BIT6+BIT5+BIT4)
//#define DATETIME_20YYMMDD				(BIT7+BIT6+BIT5+BIT4)
//#define DATETIME_YYMMDDWW				(BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_20YYMMDDWW			(BIT7+BIT6+BIT5+BIT4+BIT3)
//#define DATETIME_hhmmss				(BIT2+BIT1+BIT0)
//
//#define DATETIME_MMDDhhmm				(BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_YYMMDDhhmm			(BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_20YYMMDDhhmm			(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1)
//#define DATETIME_MMDDhhmmss			(BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_YYMMDDhhmmss			(BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//#define DATETIME_20YYMMDDhhmmss		(BIT7+BIT6+BIT5+BIT4+BIT2+BIT1+BIT0)
//-----------------------------------------------
WORD api_GetRtcDateTime( WORD Type, BYTE *pBuf )
{
	BYTE i,Length;
	WORD wTmp;
	TRealTimer tTime = {0};
	
	Length = 0;
	get_N176_time(&tTime);
	for( i=0; i<7; i++)
	{
		if( Type&(0x01<<i) )
		{
			switch( i )
			{
				case 0:
					pBuf[Length++] = tTime.Sec;
					break;
				case 1:
					pBuf[Length++] = tTime.Min;
					break;
				case 2:
					pBuf[Length++] = tTime.Hour;
					break;
				case 3:
					pBuf[Length++] = ((api_CalcInTimeRelativeSec( &tTime )/60/1440)+6)%7;
					break;
				case 4:
					pBuf[Length++] = tTime.Day;
					break;
				case 5:
					pBuf[Length++] = tTime.Mon;
					break;
				case 6:
					pBuf[Length++] = tTime.wYear%100;
					break;
			}
		}
	}
	
	if( Type&0x4000 )//Ҫ��BCD���ʽ
	{
		for(i=0; i<Length; i++)
		{
			pBuf[i] = lib_BBinToBCD( pBuf[i] );
		}
	}
	
	if( (Type & 0xc0) == 0xc0 )//�������Ƿ��2000���������Ҫ�������Ҳѡ��
	{
		if( (Type&0x4000) == 0x0000 )//Hex��
		{
			Length -= 1;//���ⳤ�Ȳ���Ϊ0��
			wTmp = tTime.wYear;
			memcpy( &pBuf[Length], &wTmp, sizeof(WORD) );
			Length += 2;
		}
		else//Ҫ��BCD���ʽ WORD����ͬ�ⰴС��ģʽ����
		{
			pBuf[Length] = 0x20;
			Length ++;
		}
	}
	
	if( (Type&0x8000) == 0x0000 )//���Ҫ�� ����������ʱ���� ����buf[0] ˳��
	{
		lib_InverseData( pBuf, Length );
		if( (Type&0xc0) == 0xc0 )//�������Ƿ��2000���������Ҫ�������Ҳѡ��
		{
			lib_InverseData( pBuf, 2 );
		}
	}
	return Length;
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
BYTE lib_CheckSum(BYTE *ptr, WORD Length)
{
	WORD i;
	BYTE Sum = 0;

	for (i = 0; i < Length; i++)
	{
		Sum += *ptr;
		ptr++;
	}

	return Sum;
}
//--------------------------------------------------
//��������:  ���̵���״̬
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void CheckRelay(void)
{
	BYTE bLen,Buf[60],i;
	BYTE bBuf[] = {0x0E,0x12,0x12,0x37,0x59,0x44,0x12,0x0E};
	bLen = 12;
	// memset(Buf,0,sizeof(Buf));
	//��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );
	Buf[4] = 0x68;
	memcpy(&Buf[5],reverAddr,6);

	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x08;//�̶�����16���ֽ�
	memcpy((BYTE*)&Buf[bLen],(BYTE*)&bBuf[0],sizeof(bBuf));
	bLen = bLen + sizeof(bBuf);
	// for (i = 14; i <(14+16); i++)
	// {
	// 	Buf[i] += 0x33;
	// }
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;

	nwy_ext_echo("\r\n");
	for (i = 0; i < bLen; i++)
    {
        nwy_ext_echo("%02X ",Buf[i]);
    }
	nwy_uart_send_data(UART_HD,Buf,bLen);
}
//--------------------------------------------------
//��������:  ���Ʒѿص������բ ��բ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  ControlRelay(void)
{
	BYTE bLen,Buf[60],i;
	BYTE bBuf[] = {0x02,0,0,0,0,0,0,0};
	BYTE bTime[6] = {0};
	bLen = 12;
	nwy_ext_echo("\r\n ControlRelay cmd is %02X", RelayControlInfo.RelayCmd);
	memset(Buf,0,sizeof(Buf));
	//��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );

	Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);

	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x1C;
	Buf[bLen++] = 0x10;//�̶�����16���ֽ�
	memcpy((BYTE*)&Buf[bLen],(BYTE*)&bBuf[0],sizeof(bBuf));
	bLen = bLen + sizeof(bBuf);
	Buf[bLen++] = RelayControlInfo.RelayCmd -0x33;
	Buf[bLen++] = 0;
	//ssmmhhDDMMYY
	api_GetRtcDateTime(DATETIME_YYMMDDhhmmss|DATETIME_BCD,bTime);
	bTime[0] = 0x30 + bTime[0];
	// INSERT_YOUR_CODE
	// nwy_ext_echo("\r\nbTime: ");
	// for(i = 0; i < sizeof(bTime); i++) {
	// 	nwy_ext_echo("%02x ", bTime[i]);
	// }
	nwy_ext_echo("\r\n");
	lib_ExchangeData(&Buf[bLen],bTime,sizeof(bTime));
	for (i = 14; i <(14+16); i++)
	{
		Buf[i] += 0x33;
	}
	bLen += sizeof(bTime);
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;
	RelayControlInfo.RelayCmd = 0;
	nwy_ext_echo("\r\ncontrol relay");
	// for (i = 0; i < bLen; i++)
    // {
    //     nwy_ext_echo("%02X ",Buf[i]);
    // }
	nwy_uart_send_data(UART_HD,Buf,bLen);
}  //���ƻ�����բ��բ api
//--------------------------------------------------
//��������:   645�����  ���û���ʱ��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  OpenSystemProgram(void)
{
	BYTE Buf[] =  {0x68,0x88,0x88,0x88,0x88,0x88,0x88,0x68,0x14,0x15,0x0E,0x12,0x12,0x37,0x35,0x33,0x33,0x33,0x37,0x36,0x35,0x34,0x34,0x35,0x12,0x0E,0xBB,0xBB,0xBB,0xBB,0x34,0xDF,0x16};
	nwy_uart_send_data(UART_HD,Buf,sizeof(Buf));
}
//--------------------------------------------------
//��������:  ̽������ַ
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Adrr_Txd(void)
{
	BYTE reqAddr[] = {0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x13, 0x00, 0xDF, 0x16};
	nwy_uart_send_data(UART_HD, reqAddr, sizeof(reqAddr));
}
//--------------------------------------------------
//��������:  645 ��չ��Լ��ȡ �汾
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_Read_Version(void) //����OK
{
	BYTE Buf[] = {0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x68,
				  0x11, 0x08, 0x0e, 0x12, 0x12, 0x37, 0x0e, 0x3b, 0x12, 0x0e, 0xb7, 0x16};

	nwy_uart_send_data(UART_HD, Buf, sizeof(Buf));
}
//--------------------------------------------------
//��������:  645 ͨ�ù�Լ��ȡ 0x11
//
//����:      dwID ���ݱ�ʶ
//         	bAddress �����ַ
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_Read(DWORD dwID) //����OK
{
	BYTE bLen, Buf[20];

	bLen = 12;
	memset(Buf, 0, sizeof(Buf));
	//��ǰ��֡
	memset((BYTE *)&Buf[0], 0xFE, 4);

	Buf[4] = 0x68;
	memset(&Buf[5], 0x88, 6);
	// lib_ExchangeData((BYTE*)&Buf[5],(BYTE*)&bAddr[0],6);
	Buf[5 + 6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;
	Buf[bLen++] = (BYTE)(LLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(LHBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID) + 0x33);
	Buf[bLen] = lib_CheckSum(&Buf[4], bLen - 4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD, Buf, bLen);
}
//--------------------------------------------------
//��������:  645 ͨ�ù�Լ��ȡ 0x11	�Ļ�·ʹ��
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_ReadMultiLoop(DWORD dwID, BYTE *bAddress)
{
	BYTE bLen, Buf[20];

	bLen = 12;
	memset(Buf, 0, sizeof(Buf));
	//��ǰ��֡
	memset((BYTE *)&Buf[0], 0xFE, 4);

	Buf[4] = 0x68;
	// memset(&Buf[5],0x88,6);
	lib_ExchangeData((BYTE *)&Buf[5], bAddress, 6);
	Buf[5 + 6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;
	Buf[bLen++] = (BYTE)(LLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(LHBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID) + 0x33);
	Buf[bLen] = lib_CheckSum(&Buf[4], bLen - 4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD, Buf, bLen);
	// nwy_ext_echo("\r\n");
	// for (size_t i = 0; i < bLen; i++)
	// {
	// 	nwy_ext_echo("%02X ",Buf[i]);
	// }
	
}
#if (CYCLE_METER_READING == YES)
//--------------------------------------------------
//��������:  645 ��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Dlt645_Tx_ContinueRead( BYTE bStep, BYTE *bBuf)
{
	if (MeterReadOI[bStep].IsExtendOI == 1)
	{
		CommWithMeter_DBDF(0x11, MeterReadOI[bStep].Slen * MeterReadOI[bStep].Num, (BYTE *)&MeterReadOI[bStep].OI, NULL);
	}
	else
	{
		Dlt645_Tx_ReadMultiLoop((DWORD)MeterReadOI[bStep].OI,bBuf);
	}
}
#endif
//--------------------------------------------------
//��������:  645 ͨ�ù�Լ���� 0x14
//
//����:      dwDI ����
//           bLen
//           pBuf ������������
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_Write(DWORD dwID, BYTE bDataLen, BYTE *pBuf) //ȫ8 ��ַ������   дbuf����ʱ��֪�� Ҫ��Ҫת��ʽ
{
	BYTE i, bLen, Buf[128];

	bLen = 12;
	memset(Buf, 0, sizeof(Buf));
	//��ǰ��֡
	memset((BYTE *)&Buf[0], 0xFE, 4);
	Buf[4] = 0x68;
	memset(&Buf[5], 0x88, 6);
	Buf[5 + 6] = 0x68;
	Buf[bLen++] = 0x14;
	Buf[bLen++] = 12 + bDataLen;

	Buf[bLen++] = (BYTE)(LLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(LHBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID) + 0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID) + 0x33);
	//���� �����ߴ���
	Buf[bLen++] = 0x35;
	memset((BYTE *)&Buf[bLen], 0x33, 7);
	bLen = bLen + 7;
	//����
	memcpy((BYTE *)&Buf[bLen], pBuf, bDataLen);
	for (i = 0; i < bDataLen; i++)
	{
		Buf[bLen] = Buf[bLen] + 0x33;
		bLen++;
	}

	Buf[bLen] = lib_CheckSum(&Buf[4], bLen - 4);
	bLen++;
	Buf[bLen++] = 0x16;
	// for (i = 0; i < bLen; i++)
	// {
	//     nwy_ext_echo("%c",Buf[i]);
	// }
	nwy_uart_send_data(UART_HD, Buf, bLen);
}
//--------------------------------------------------
//��������: ��չ��ԼDBDF ��LTU����һ��ͨ��
//
//����:		Type[int],  �����룬
//
//          ID[in],	���ݱ�ʶ
//
//			bDataLen[in],���ò����ĳ���
//
//          pBuf[in],Ҫ���õ�����
//����ֵ: 	TRUE/FALSE
//
//��ע:     0x11,0x14
//-----------------------------------------------
BOOL CommWithMeter_DBDF(BYTE Type, BYTE bDataLen, BYTE *ID, BYTE *pBuf) //��ȡ������OK
{
	BYTE i;
	WORD wLen = 12;
	BYTE Buf[128];
	//��ͬ��֡
	memset((BYTE *)&Buf[0], 0x00, sizeof(Buf));

	//��ǰ��֡
	memset((BYTE *)&Buf[0], 0xFE, 4);

	Buf[4] = 0x68;
	memset(&Buf[5], 0x88, 6);
	Buf[5 + 6] = 0x68;
	Buf[wLen++] = Type;

	if (Type == 0x11) //��չ��Լ����
	{
		Buf[wLen++] = 8;
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = ID[0] + 0x33;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12; //df
		Buf[wLen++] = 0x0E; //db
	}
	else if (Type == 0x14) //��չ��Լд
	{
		Buf[wLen++] = bDataLen + 20; //���ȿ�����
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = 0x35;
		memset((BYTE *)&Buf[wLen], 0x33, 7);
		Buf[wLen + 7] = ID[0] + 0x33;
		wLen = wLen + 8;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12; //DF
		Buf[wLen++] = 0x0E; //DB
		memset((BYTE *)&Buf[wLen], 0xBB, 4);
		wLen = wLen + 4;

		for (i = 0; i < bDataLen; i++)
		{
			Buf[wLen++] = pBuf[i] + 0x33;
		}
	}
	Buf[wLen] = lib_CheckSum(&Buf[4], wLen - 4);
	wLen++;
	Buf[wLen++] = 0x16;
	nwy_uart_send_data(UART_HD, Buf, wLen);
	// nwy_ext_echo("\r\n send dbdf data ");
	// for (size_t i = 0; i < wLen; i++)
	// {
	// 	nwy_ext_echo("%02X ",Buf[i]);
	// }
	// nwy_ext_echo("\r\n");
	return TRUE;
}
#if (GPRS_POSITION == YES)
//--------------------------------------------------
//��������:  ��չ��Լ�������õ���λ��
//
//����:      bMode  1���õ���λ�ã�0 ������λ��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL api_CheckorSetPosition(BYTE bMode)
{
	int result = 0;
	char message[2048] = {0};
	DWORD dwJWD[9] = {0};
	TOad40040200 TLocation;

	memset((BYTE *)&TLocation, 0, sizeof(TOad40040200));
	// ��ȡ��γ��λ��
	result = nwy_loc_get_nmea_data(message);
	if (result)
	{
		nwy_ext_echo("\r\n get nmea data success");
	}
	else
	{
		nwy_ext_echo("\r\n get nmea data fail");
	}
	get_gps_position_info(message, dwJWD);
	TLocation.latitude.location = (BYTE)dwJWD[3];
	TLocation.latitude.degree = dwJWD[2];
	TLocation.latitude.min = (dwJWD[1] * 100000 + dwJWD[0]) / pow(10, 5);
	TLocation.longitude.location = dwJWD[7];
	TLocation.longitude.degree = dwJWD[6];
	TLocation.longitude.min = (dwJWD[5] * 100000 + dwJWD[4]) / pow(10, 5);
	TLocation.high = dwJWD[8] * 100;
	nwy_ext_echo("\r\nTLocation.longitude.location is %d", TLocation.longitude.location);
	nwy_ext_echo("\r\nTLocation.longitude.degree is %d", TLocation.longitude.degree);
	nwy_ext_echo("\r\nTLocation.longitude.min is %f", TLocation.longitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.location is %d", TLocation.latitude.location);
	nwy_ext_echo("\r\nTLocation.latitude.degree is %d", TLocation.latitude.degree);
	nwy_ext_echo("\r\nTLocation.latitude.min is %f", TLocation.latitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.high is %d", TLocation.high);
	if ((TLocation.longitude.degree == 0) && (TLocation.latitude.degree == 0))
	{
		nwy_ext_echo("\r\nget location failed");
		if (bMode != 0)
		{
			nwy_gpio_set_value(RX_LIGHT, 1);
		}
		//���ص���
		return FALSE;
	}
	else
	{
		if (bMode == 1)
		{
			Dlt645_Tx_Write(0x0400041F, sizeof(TLocation), (BYTE *)&TLocation);
		}
		return TRUE;
	}
}
#endif


//--------------------------------------------------
//��������:  ���ݽ��ճɹ������Ӧbitλ
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void ReceiveClearBitFlag(void)
{
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	// nwy_ext_echo("\r\ncurrent loop is %d ", Loop);
	qwReadMeterFlag[Loop] &= ~(1ull << bHisReadMeter[Loop]);
	PowerOnReadMeterFlag |= (1ull << bHisReadMeter[Loop]);
	bHisReadMeter[Loop] = 0xFF;
	bReadMeterRetry = 0;
	nwy_stop_timer(uart_timer);
	nwy_gpio_set_value(LOCAL_LIGHT, 1);
}
//--------------------------------------------------
//��������:  ��ӡ�̵����仯ԭ��
//
//����:      pReason[in] �̵����仯ԭ��
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void PrintRelayChangeReason(TRelayChangeReason *pReason)
{
	nwy_ext_echo("\r\n relay  reason is %08X", pReason->Reason);
	nwy_ext_echo(" Time: %02X %02X %02X %02X %02X %02X",
	pReason->Time[0], pReason->Time[1], pReason->Time[2],
	pReason->Time[3], pReason->Time[4], pReason->Time[5]);
}
//--------------------------------------------------
// ��������:  ������������
//
// ����:      pBuf[in] ����
//         	Len[in] ���ݳ���
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void AnalyseReadMeterData(DWORD dwID, BYTE *pBuf, BYTE Len)
{
	BYTE j = 0,k = 0;
	BYTE* buf = NULL;
	// nwy_ext_echo("\r\n AnalyseReadMeterData dwID is %08x", dwID);
#if (CYCLE_METER_READING == YES)
	WORD i;
	BYTE bOffset;
	tMeterRead tMeterReadOI;

	for (i = 0; i < CycleReadMeterNum; i++)
	{
		if (dwID == MeterReadOI[i].OI)
		{
			memcpy((BYTE *)&tMeterReadOI, (BYTE *)&MeterReadOI[i], sizeof(tMeterRead));
			bOffset = Loop * (COLLECT_DOUBLE_DATA_ITEM_NUM);
			if (tMeterReadOI.IsDoubleData == 1)
			{
				DataAnalyseandSave(pBuf, tMeterReadOI.Buf + bOffset, Len, tMeterReadOI);
			}
			else
			{
				// nwy_ext_echo("\r\n pBuf data is");
				// for(int j = 0; j < Len; j++)
				// {
				// 	nwy_ext_echo("%02X ", pBuf[j]);
				// }
				// nwy_ext_echo("\r\n");
				memcpy((BYTE *)MeterReadOI[i].Buf, pBuf, MeterReadOI[i].Slen * MeterReadOI[i].Num);
			}
			// ִ�е�����Ϊһ֡�������
			if (i == (CycleReadMeterNum - 1)) // ����������
			{
				//�̵����仯�ֽ�˳�����
				for(j = 0; j < METER_PHASE_NUM; j++)
				{
					for(k = 0; k < 2; k++)
					{
						buf = (BYTE *)&RelayChangeReason[j][k].Time;
						lib_ExchangeData(buf, buf, sizeof(RelayChangeReason[j][k].Time));
						// PrintRelayChangeReason(&RelayChangeReason[j][k]);
					}
				}
				lib_ExchangeData((BYTE *)&gRelayStatus[0], (BYTE *)&gRelayStatus[0], sizeof(gRelayStatus));
				// nwy_ext_echo("\r\n Relay status is %02X %02X %02X ", gRelayStatus[0],gRelayStatus[1],gRelayStatus[2]);
				
				if (Loop == (bUsedChannelNum - 1))
				{
					SampleDatatoReport(&tTimer);
				}
			}
			break;
		}
	}
#endif
}
//--------------------------------------------------
//��������:  645_07Ӧ�ò㺯��
//
//����:      dwID[in] Ҫ���������ݱ�ʶ
//         		pBuf[in] ����
//				len[in] ���ݳ���
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL DealDLT645_2007(DWORD dwID, BYTE *pBuf, BYTE Len) //ֱ�ӿ������ֽ����� ���赹��
{
	DWORD dwIP;
	TRealTimer tTime = {0};
	BYTE j;
	BYTE Sum = 0;
	BYTE bCorrect = 0;
	BYTE bReceiveBit = 0;
	#if (EVENT_REPORT == YES)//�澯 ������������н��գ���׼��ʶ��645��չ��ʶ��
	WORD wTemp;
	char buf[17] = {0};
	QWORD qwMs;
	Eventmessage EventData;
	#endif

	if ((dwID == TCP_REQ_NET) || (dwID == MQTT_REQ_NET) || (dwID == TCP_REQ_USER_PD) || (dwID == MQTT_REQ_USER_PD))
	{
		for (j = 0; j < (Len - 1); j++)
		{
			Sum += pBuf[j];
			if ((pBuf[j] != 0) && (pBuf[j] != 0xff)) //ֻҪ�в���0��ff��ֵ����Ϊ�ǺϷ�ֵ
			{
				bCorrect = 1;
			}
		}
		if ((bCorrect != 1) || (Sum != pBuf[Len - 1]))
		{
			nwy_ext_echo("\r\n DealDLT645_2007 bCorrect != 1 || Sum != pBuf[Len - 1]");
			return FALSE;
		}
	}
	if (dwID == DAY_TIME)
	{
		//��Ӧ���ݱ�ʶ���պ���
		tTime.Sec = lib_BBCDToBin(pBuf[0]);
		tTime.Min = lib_BBCDToBin(pBuf[1]);
		tTime.Hour = lib_BBCDToBin(pBuf[2]);
		tTime.Timezone = 8;
		tTime.Day = lib_BBCDToBin(pBuf[4]);
		tTime.Mon = lib_BBCDToBin(pBuf[5]);
		tTime.wYear = 2000 + lib_BBCDToBin(pBuf[6]);
		if (api_CheckClock(&tTime) == TRUE)
		{
			set_N176_time(&tTime);
			nwy_ext_echo("\r\n enter set time ");
			bReceiveBit = 1;
			api_WriteSysUNMsg(SYSUN_POWER_ON);
		}
	}
	else if (dwID == TCP_REQ_NET)
	{
		dwIP = MDW(pBuf[3], pBuf[2], pBuf[1], pBuf[0]);
		sprintf(Serverpara[0].ServerIP, "%d.%d.%d.%d", (int)LLBYTE(dwIP), (int)LHBYTE(dwIP), (int)HLBYTE(dwIP), (int)HHBYTE(dwIP));
		Serverpara[0].ServerPort = MW(pBuf[5], pBuf[4]);
		nwy_ext_echo("\r\ntcp ip%s", Serverpara[0].ServerIP);
		nwy_ext_echo("\r\ntcp port %d", Serverpara[0].ServerPort);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_NET)
	{
		dwIP = MDW(pBuf[3], pBuf[2], pBuf[1], pBuf[0]);
		sprintf(Serverpara[1].ServerIP, "%d.%d.%d.%d", (int)LLBYTE(dwIP), (int)LHBYTE(dwIP), (int)HLBYTE(dwIP), (int)HHBYTE(dwIP));
		Serverpara[1].ServerPort = MW(pBuf[5], pBuf[4]);
		nwy_ext_echo("\r\n mqtt ip%s", Serverpara[1].ServerIP);
		nwy_ext_echo("\r\n mqtt port%d", Serverpara[1].ServerPort);
		bReceiveBit = 1;
	}
	else if (dwID == TCP_REQ_USER_PD)
	{
		memcpy((BYTE *)&Serverpara[0].ServerUserName[0], pBuf, 32);
		memcpy((BYTE *)&Serverpara[0].ServerUserPwd[0], pBuf + 32, 32);
		nwy_ext_echo("\r\nTCP USER %s", Serverpara[0].ServerUserName);
		nwy_ext_echo("\r\nTCP PWD %s", Serverpara[0].ServerUserPwd);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_USER_PD)
	{
		lib_ExchangeData((BYTE *)&Serverpara[1].ServerUserName[0], pBuf, 32);
		lib_ExchangeData((BYTE *)&Serverpara[1].ServerUserPwd[0], pBuf + 32, 32);
		nwy_ext_echo("\r\n MQTT USER %s", Serverpara[1].ServerUserName);
		nwy_ext_echo("\r\n MQTT PWD %s", Serverpara[1].ServerUserPwd);
		bReceiveBit = 1;
	}
	else if (dwID == USED_CHANNEL_NUM)
	{
		if ((pBuf[0] > 4) || (pBuf[0] < 1))
		{
			bReceiveBit = 0;
		}
		else
		{
			bUsedChannelNum = pBuf[0];
			nwy_ext_echo("\r\n bUsedChannelNum is %d",bUsedChannelNum);
			bReceiveBit = 1;
		}
	}
	#if (EVENT_REPORT == YES)//�澯 ������������н��գ���׼��ʶ��645��չ��ʶ��
	#if (CYCLE_METER_READING == YES)
	else if (dwID == WARNING_REPORT)
	{
		bReceiveBit = 1;
		memcpy((BYTE*)&wTemp,&pBuf[0],2);
		if (wTemp != bHisWarning)
		{
			get_N176_time(&tTime);
			qwMs = getmktimems(&tTime);
			EventData.TimeMs = qwMs;
			EventData.Number = bWarningNumber;
			HextoB(wTemp,buf);
			nwy_ext_echo("\r\n waring is %s",buf);
			memcpy(&EventData.Warning[0],&buf[0],17);
			nwy_ext_echo("\r\n waring1 is %s",EventData.Warning);
			{	
				#if (EVENT_REPORT == YES)
				if (nwy_get_queue_spaceevent_cnt(EventReportMessageQueue) != 0)//״̬�澯
				{
					nwy_put_msg_que(EventReportMessageQueue, &EventData, 0xffffffff);

					for (i = 0; i < bUsedChannelNum; i++)
					{
						for (j = 0; j < CycleReadMeterNum; j++)
						{
							qwReadMeterFlag[i] |= (1ull << j);
						}
					}
					get_N176_time(&tTimer);
				}
				#endif
			}
		}
		bHisWarning = wTemp;
	}
	#endif
	#endif//#if (EVENT_REPORT == YES)
	else
	{
		AnalyseReadMeterData(dwID, pBuf, Len);
		bReceiveBit = 1;
	}
	if (bReceiveBit == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//��������:  ��չ��ԼӦ�ò㺯��
//
//����:      pBuf[in] ��չ��Լ���ݱ�ʶ DBDFXXXX
//
//			bDataLen[in] ���������ݳ���
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL DealDLT645_Factory(BYTE *pBuf, BYTE bDataLen)
{
	BYTE bReceiveBit = 0,t_EpTcpUserNum = 0,j = 0;
	DWORD temp;
	BYTE dwID[4] = {0x00, 0x00, 0xDF, 0xDB};

	memcpy((BYTE *)&dwID[0], (BYTE *)&pBuf[0], 2);
	switch (pBuf[1])
	{
		#if (EVENT_REPORT == YES)//�澯 ������������н��գ���׼��ʶ��645��չ��ʶ)

		#endif
		case 0x11: //���
			if (pBuf[0] == 0x26)//�̵���״̬
			{
				if(IsCycleReadRelayFlag == 0)
				{
					nwy_put_msg_que(RelayStatusMessageQueue, &pBuf[2], 0xffffffff);
					api_SetSysStatus(eSYS_STASUS_UART_AVAILABLE);
				}
				else
				{
					AnalyseReadMeterData(*(DWORD*)dwID, pBuf + 2, bDataLen);
				}
				bReceiveBit = 1;
			}
			#if (PT_CT == YES)
			if (pBuf[0] == 0x18)
			{
				memcpy((BYTE *)&OtherPara.dwCTPrim, (BYTE *)&pBuf[2], 4);
				nwy_ext_echo("CT1\r\n %d", OtherPara.dwCTPrim);
				bReceiveBit = 1;
			}
			else if (pBuf[0] == 0x19)
			{
				OtherPara.bCTSecond = pBuf[2];
				nwy_ext_echo("CT2\r\n %d", OtherPara.bCTSecond);
				bReceiveBit = 1;
			}
			else if (pBuf[0] == 0x1A)
			{
				memcpy((BYTE *)&OtherPara.dwPTPrim, (BYTE *)&pBuf[2], 4);
				nwy_ext_echo("PT1\r\n %d", OtherPara.dwPTPrim);
				bReceiveBit = 1;
			}
			else if (pBuf[0] == 0x1B)
			{
				OtherPara.bPTSecond = pBuf[2];
				nwy_ext_echo("PT2\r\n %d", OtherPara.bPTSecond);
				bReceiveBit = 1;
			}
			#endif
			break;
		case 0x08: //�汾
			if (pBuf[0] == 0xDB)
			{
				sprintf(MeterVersion, "version:%02X%02X,verification:%02X%02X",
						pBuf[11], pBuf[10], pBuf[16], pBuf[17]);
				nwy_ext_echo("\r\nMeterVersion %s", MeterVersion);
			bReceiveBit = 1;
		}
		break;
	case 0xFF://212��ػ������
		if (pBuf[0] == 0xFE) //5��ͨ���� ip/�˿�
		{
			for (BYTE i = 0; i < 5; i++)
			{
				if (pBuf[2+37*i+36] == 1)
				{
					lib_ExchangeData((BYTE*)&Serverpara[j].ServerIP[0],&pBuf[2+i*37],32);//Ԥ�� û�� \0������
					memcpy((BYTE*)&temp,&pBuf[2+37*i+32],4);
					Serverpara[j].ServerPort = lib_DWBCDToBin(temp);
					nwy_ext_echo("\r\nServerIP [%s][%d]", Serverpara[j].ServerIP,j);
					nwy_ext_echo("\r\nServerPort [%d][%d]", Serverpara[j].ServerPort,j);
					t_EpTcpUserNum++;
					//��Ӧ����Ӧbitλ ���ɶ�Ӧͨ���� bit��
					// g_EpTcpUserChannel |=(1<<i);
					// EpTcpUserTable[j++] = i;
					EpTcpUserTable[j].bTcpConnectId = j;
					EpTcpUserTable[j++].bMeterParaId  = i;
				}
			}
			g_EpTcpUserNum = t_EpTcpUserNum;
			bReceiveBit = 1;
			}
			break;
		default:
			break;
	}
	if (bReceiveBit == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//��������:  �Ա��Ľ��м�0x33����
//
//����:      BYTE *pBuf[in]:���뻺��pBuf[0]Ϊ68H
//
//����ֵ:    ���͵����ݱ�ʶ
//
//��ע����:  ��
//--------------------------------------------------
DWORD DataSubtract33H(BYTE *pBuf)
{
	WORD i;
	DWORD dwID;

	for (i = 1; i <= pBuf[0]; i++)
	{
		pBuf[i] = (pBuf[i] - 0x33);
	}
	memcpy((BYTE *)&dwID, (BYTE *)&pBuf[1], 4);

	return dwID;
}
//--------------------------------------------------
//��������:  4Gģ�� �Խ��ܵı��Ľ���(07��Լ)
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
BOOL RxUartMessage_Dlt645(TSerial *p)
{
	DWORD dwDataType;
	T645FmRx *pRxFm;
	BYTE *pBuf, bLen, i;

	pRxFm = (T645FmRx *)&p->ProBuf[0];
	if (pRxFm->byCtrl == 0x91)
	{
		//�жϵڼ���·����
		Loop = ((pRxFm->MAddr[0]) - bAddr[5]);
		if (Loop >= MAX_SAMPLE_CHIP)
		{
			Loop = 0; //��ֹ̽���ĩλ��ַ 9��10���� �������Ļ�·�ĵ�ַ
		}
		pBuf = (BYTE *)&pRxFm->byDataLen;
		dwDataType = DataSubtract33H(pBuf); //�����ݽ��м�0x33����

		if (dwDataType == DLT645_FACTORY_ID)
		{
			bLen = pRxFm->byDataLen - 6;
			return (DealDLT645_Factory(&pBuf[5], bLen));
		}
		else
		{
			bLen = pRxFm->byDataLen - 4;
			return (DealDLT645_2007(dwDataType, &pBuf[5], bLen));
		}
	}
	else if (pRxFm->byCtrl == 0x94)
	{
		return TRUE;
	}
	else if (pRxFm->byCtrl == 0x93)
	{
		pBuf = (BYTE *)&pRxFm->byDataLen;
		bLen = pRxFm->byDataLen;
		for (i = 1; i <= pBuf[0]; i++)
		{
			pBuf[i] = (pBuf[i] - 0x33);
		}
		lib_ExchangeData((BYTE *)&bAddr[0], (BYTE *)&pBuf[1], bLen);
		memcpy((BYTE *)&reverAddr[0], (BYTE *)&pBuf[1], bLen);
		nwy_hex_to_string(6, bAddr, LtuAddr);
		nwy_ext_echo("\r\n LtuAddr:%s", LtuAddr);
		return TRUE;
	}
	else if ((pRxFm->byCtrl == 0x9C) || (pRxFm->byCtrl == 0xDC))//����բ����
	{
		if(IsCycleReadRelayFlag == 0)
		{
			api_SetSysStatus(eSYS_STASUS_UART_AVAILABLE);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//��������:  645��Լ����������
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void DealUartMessage_Dlt645(TSerial *p)
{
	if (RxUartMessage_Dlt645(p))
	{
		ReceiveClearBitFlag();
	}
}
//---------------------------------------------------------------
//��������: uartͨ��645��ʽ�ж�
//
//����: 	portType[in] - ͨѶ�˿�
//
//����ֵ:  FALSE: ����û������
//		  TRUE : ������������InitPoint()
//
//��ע:
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_UART(BYTE ReceByte, TSerial *p)
{
	WORD i, j;
	BYTE *pBuf;
	TDlt645Head Dlt645Head;

	//����ͬ��ͷ
	if (p->ProStepDlt645 == 0)
	{
		if (ReceByte == 0x68)
		{
			//д������
			//p->ProBuf[p->ProStepDlt645] = ReceByte;
			//Dlt645��Լ������Serial[].ProBuf�еĿ�ʼλ��
			p->BeginPosDlt645 = p->RXRPoint;

			//����ָ��
			p->ProStepDlt645++;

			//��ʼ���ն�ʱ
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
	}
	else if (p->ProBuf[p->BeginPosDlt645] == 0x68)
	{
		//���չ�Լͷ�������֡����� 68 1 2 3 4 5 6 68 01 02
		if (p->ProStepDlt645 < sizeof(TDlt645Head))
		{
			//д������
			//p->ProBuf[p->ProStepDlt645] = ReceByte;

			//����ָ��
			p->ProStepDlt645++;

			//�Ƿ���Դ�������
			if (p->ProStepDlt645 == sizeof(TDlt645Head))
			{
				//�ѹ�Լ�����Ƶ���ʱ������ ѭ�������ô�
				//Num���ƶ������ֽ�
				//BeginPos:��ʲô��ַ��ʼ��
				pBuf = (BYTE *)&Dlt645Head;
				for (i = 0; i < sizeof(Dlt645Head); i++)
				{
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN];
				}
				//ӳ��ṹ
				//pDlt645Head = (TDlt645Head *)&TmpArray;
				j = p->ProStepDlt645;

				//�����ڶ�ͬ����
				if (Dlt645Head.Sync2 != 0x68) //���û�е�2��֡��ʼ��68����ǰ���յ��ĵ�һ68���ԣ���������������һ��68
				{
					for (i = 1; i < p->ProStepDlt645; i++)
					{
						if (p->ProBuf[(p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN] == 0x68)
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN;
							p->ProStepDlt645 -= i;
							break;
						}
						//�ĺ�68ǰ���������ݻ�68����ͨ���ϣ������ͨ�Ų��ϣ�û�����֣�����̫���ӣ�68 fe fe fe fe fe fe 68 88 88 88 88 88 88 68 11 04 33 33 34 33 e2 16
					}
					if (i >= j) //j �� p->ProStepDlt645 �ı��ݣ������յ������������꣬�����ң�ֱ��ProStepDlt645��0����
					{
						p->ProStepDlt645 = 0; //InitPoint;
						return FALSE;
					}
					//p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
				else
				{
				}
				//ȡ���� 645�������������ݳ���
				p->Dlt645DataLen = Dlt645Head.Length;

				//�жϳ���
				//68 A0...A5 68 81 L D0..DL CS 16,��D0...DL�⣬���๲16�ֽ�
				if (p->Dlt645DataLen >= (UART_BUFF_MAX_LEN - 12))
				{
					p->ProStepDlt645 = 0; //InitPoint(p);
					return FALSE;
				}
			}
		}
		//��������
		else if (p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2))
		{
			//����ָ��
			p->ProStepDlt645++;

			//����У���ֽ�
			if (p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 1))
			{
				//�ѹ�Լ�����Ƶ���ʱ������ ѭ�������ô�
				//Num���ƶ������ֽ�
				//BeginPos:��ʲô��ַ��ʼ��
				if (ReceByte != api_CalRXD_CheckSum(0, p))
				//if( ReceByte != ( lib_CheckSum(p->ProBuf+p->BeginPosDlt645,(10+p->Dlt645DataLen))&0xff) )
				{
					//У�鲻��ȷ
					p->ProStepDlt645 = 0; //InitPoint(p);
					return FALSE;
				}
			}

			//�յ�0x16 �Ž��г�����
			else if (p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 2))
			{
				//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
				api_DoReceMoveData(p, PROTOCOL_645);

				if (Pre_Dlt645(p))
				{
				}
				else
				{
					DealUartMessage_Dlt645(p);
				}
				//�������
				InitPoint(p);

				return TRUE;
			}
			else
			{
			}
		}
		else
		{
			p->ProStepDlt645 = 0; //InitPoint(p);
		}
	}
	return FALSE;
}
#endif //#if( READMETER_PROTOCOL ==  PROTOCOL_645)