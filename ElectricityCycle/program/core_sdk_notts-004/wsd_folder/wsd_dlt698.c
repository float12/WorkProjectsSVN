//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������
//��������
//����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_dlt698.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define LOCKSET_PARA_MAX_LEN			128
#define OBJ_START_FLAG					0x68 			//��ʼ�ַ�
#define PPPINITFCS16 					0xffff 			/* Initial FCS value */
#define PPPGOODFCS16 					0xf0b8 			/* Good final FCS value */
//֧��DL/T698.45�������Э��
#define PRO_DLT698_45 5
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef union TTwoByteUnion_t 
{
	// �ַ�ʽ
	WORD w;
	// �ֽڷ�ʽ
	BYTE b[2];
	// �з�����
	SWORD sw;

} TTwoByteUnion;

typedef enum
{
	eLockset_GetReqNormal = 0,
	eLockset_GetReqRecord,
	eLockset_SetReqNormal,
	eLockset_ActionReqNormal,
	eLockset_ActionRespNormal,
	eLockset_MessageTypeMax
} eReqMessageType_t;

typedef struct
{
	BYTE Para[LOCKSET_PARA_MAX_LEN];
	WORD Len;
} ParaInfo_t;

typedef enum
{
	eNO_SECURITY = 0, //���ǰ�ȫ����
	eSECURITY_FAIL,   //��ȫ����ʧ��
	eRNMAC,			  //�����+MAC
	eSECURITY_RN,
	eEXPRESS_MAC, //����+mac
	eSECRET_TEXT, //����
	eSECRET_MAC   //����+mac
} eSecurityMode;  //��ȫ��������

typedef struct
{
	eReqMessageType_t Type; // Apdu����
	DWORD Oad;				// OAD��OMD
	TSA Sa;					// ��������ַ
	BYTE ClientAddr;		// �ͻ�����ַ
	BYTE Ctrl;				// ������
	ParaInfo_t ParaInfo;	// ����
	eSecurityMode SafeMode; // ����ʹ�õİ�ȫģʽ
	BYTE *pOutBuf;
	WORD *pOutBufLen;
} GenerateReqMessage_t;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE InstantData698Frame[INSTANT_DATA_698_FRAME_LEN];		// 698֡����
BYTE ReadModuleVersion = 0;									//���Ͷ��汾�������Ҫ��1���ظ�����698������645�����ⱻ645������
BYTE ModuleAppVersion[2];									//�汾
BYTE ModuleAppVerif[2];										//У��
char ModuleVersion[100] ={0};
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
char bLtoAddr[6];				//����������ͨ�ŵ�ַ
//һ������ݸ�ʽ
eDataType698 InstantDataFormatTable[DATA_ITEM_NUM_PER_PHASE] =
{
	efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32,
	efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32,
	efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32, efloat32
};
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

// FCS lookup table as calculated by the table generator.
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
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: fcsУ�麯��
//
//����:		BYTE *cp
//			WORD len
//����ֵ:	WORD
//
//��ע:
//-----------------------------------------------
static WORD fcs16(BYTE *cp, WORD len)
{
	WORD cpLen;
	WORD fcs;

	fcs = PPPINITFCS16;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ((fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	fcs ^= 0xffff; // complement  //��ΪFCSֵ
	return fcs;
}
//-----------------------------------------------
// ��������: ����FCSУ��ֵ
//
// ����:		BYTE *cp
//			WORD len
// ����ֵ:	WORD
// ��ע:
//-----------------------------------------------
WORD api_fcs16(BYTE *cp, WORD len)
{
	return fcs16(cp, len);
}
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  SetLtoTime( void )
{
	BYTE i = 0;
	BYTE buf[60];
	TTwoByteUnion Hrc, Fcs;
	TRealTimer time = {0};

	buf[i++] = 0x68;
	buf[i++] = 0x1f;
	buf[i++] = 0x00;
	buf[i++] = 0x43;
	buf[i++] = 0x15;
	lib_ExchangeData((BYTE *)&buf[i], (BYTE *)&bLtoAddr[0], 6);
	i = i+6;
	buf[i++] = 0xa0;
	Hrc.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Hrc.b[0];
	buf[i++] = Hrc.b[1];

	buf[i++] = 0x06;
	buf[i++] = 0x01;
	buf[i++] = 0x00;
	buf[i++] = 0x40;
	buf[i++] = 00;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x1c;//����
	get_N176_time(&time);
	lib_InverseData((BYTE*)&time.wYear,2);
	memcpy((char *)&buf[i], (char *)&time, sizeof(time)-1);
	i = i+sizeof(time)-1;
	buf[i++] = 0;
	Fcs.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Fcs.b[0];
	buf[i++] = Fcs.b[1];
	buf[i++] = 0x16;
	nwy_ext_echo("\r\nSetLtoTime");
	for (BYTE j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ",buf[j]);
	}	
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO,buf,i);
}
//--------------------------------------------------
//��������:  ����������ģʽ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  open_ltofactory( void )
{
	BYTE i = 0;
	TTwoByteUnion Hrc, Fcs;
	char apdu[] = {0x07,0x01,0,0x20,0xeb,0x03,0x00,0x09,0x21,0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x68,0x14,0x15,0x0E,0x12,0x12,0x37,0x35,
	0x33,0x33,0x33,0x37,0x36,0x35,0x34,0x34,0x35,0x12,0x0E,0xBB,0xBB,0xBB,0xBB,0x34,0xAF,0x16};
	BYTE buf[100];

	buf[i++] = 0x68;
	buf[i++] = 0x3a;
	buf[i++] = 0x00;
	buf[i++] = 0x43;
	buf[i++] = 0x15;
	lib_ExchangeData((BYTE *)&buf[i], (BYTE *)&bLtoAddr[0], 6);
	i = i+6;
	buf[i++] = 0xa0;
	Hrc.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Hrc.b[0];
	buf[i++] = Hrc.b[1];

	memcpy(&buf[i],apdu,sizeof(apdu));
	i = i+ sizeof(apdu);
	buf[i++] = 0;
	Fcs.w = fcs16((BYTE *)&buf[1], i-1);
	buf[i++] = Fcs.b[0];
	buf[i++] = Fcs.b[1];
	buf[i++] = 0x16;

	nwy_ext_echo("\r\nopen lto factory");
	for (BYTE j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ",buf[j]);
	}	
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO,buf,i);
}
//--------------------------------------------------
//��������:  �׿�698
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BYTE  Add698Shell( BYTE *buf,WORD wLen,BYTE *Outbuf )	
{
	BYTE w698Len,i;
	TTwoByteUnion Hrc, Fcs;
	TRealTimer time = {0};

	i= 3;
	memset(&Hrc, 0, sizeof(Hrc));
	Outbuf[0] = 0x68;
	Outbuf[i++] = 0x43;
	Outbuf[i++] = 0x26;
	Outbuf[i++] = 0x03;
	lib_ExchangeData((BYTE *)&Outbuf[i], (BYTE *)&MeterWaveAddr[0], 6);
	i = i+6;
	Outbuf[i++] = 0;//�ͻ�����ַ

	Outbuf[i++] = Hrc.b[0];
	Outbuf[i++] = Hrc.b[1];
	

	Outbuf[i++] = 0x85;
	Outbuf[i++] = 0x01;
	Outbuf[i++] = 0;
	Outbuf[i++] = 0x20;
	Outbuf[i++] = 0xeb;
	Outbuf[i++] = 0x00;
	Outbuf[i++] = 0x03;

	Outbuf[i++] = 0x01;//������� ����
	Outbuf[i++] = 0x09;
	Outbuf[i++] = 0x81;
	Outbuf[i++] = (BYTE)wLen;
	memcpy(&Outbuf[i],buf,wLen);
	i = i+wLen;
	Outbuf[i++] = 0;
	Outbuf[i++] = 0x01;
	get_N176_time(&time);
	lib_InverseData((BYTE*)&time.wYear,2);
	// i = i+2;
	memcpy((char *)&Outbuf[i], (char *)&time, sizeof(time)-1);
	i = i+sizeof(time)-1;
	Outbuf[i++] = 0;
	Outbuf[i++] = 0;
	Outbuf[i++] = 0x05;

	w698Len = i+3-2;
	Outbuf[1] = (BYTE)w698Len; //����
	Outbuf[2] = (BYTE)(w698Len>>8);
	
	
	Hrc.w = fcs16((BYTE *)&Outbuf[1], 12);
	Outbuf[13] = Hrc.b[0];
	Outbuf[14] = Hrc.b[1];
	Fcs.w = fcs16((BYTE *)&Outbuf[1], i-1);
	Outbuf[i++] = Fcs.b[0];
	Outbuf[i++] = Fcs.b[1];
	Outbuf[i++] = 0x16;
	
	// for ( i = 0; i < w698Len+2; i++)
	// {
		// nwy_ext_echo("%02x ",Outbuf[i]);
	// }
	return (w698Len+2);
}
//--------------------------------------------------
// ��������:  ˲ʱ���ϴ�698��֡
//
// ����:       buf ���ݻ���
//         	  len ���ݳ���
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void Compose_InstantData_698(void)
{
	TTwoByteUnion Hcs, Fcs;
	WORD i = 0, j = 0, k = 0;
	BYTE *pTmp = &PublishInstantData[0];

	memset(&Hcs, 0, sizeof(Hcs));
	memset(&Fcs, 0, sizeof(Fcs));
	memset(InstantData698Frame, 0, sizeof(InstantData698Frame));

	InstantData698Frame[i++] = 0x68;
	//������
	InstantData698Frame[i++] = LLBYTE(INSTANT_DATA_698_FRAME_LEN - 2);
	InstantData698Frame[i++] = LHBYTE(INSTANT_DATA_698_FRAME_LEN - 2);
	//������
	InstantData698Frame[i++] = 0xc3;
	//SA��־
	InstantData698Frame[i++] = 0x05;
	//���ַ
	memcpy(&InstantData698Frame[i], &MeterReverAddr[0], 6);
	i += 6;
	//�ͻ�����ַ
	InstantData698Frame[i++] = 0x00;
	//У��
	Hcs.w = fcs16((BYTE *)&InstantData698Frame[1], 11);
	InstantData698Frame[i++] = Hcs.b[0];
	InstantData698Frame[i++] = Hcs.b[1];

	InstantData698Frame[i++] = 0x85;
	InstantData698Frame[i++] = 0x01;
	InstantData698Frame[i++] = 0x00;
	memset(&InstantData698Frame[i], 0xee, 4); //oad
	i += 4;

	InstantData698Frame[i++] = 0x01;
	//�ṹ�� ������+��ţ�
	InstantData698Frame[i++] = 0x02;
	InstantData698Frame[i++] = 0x02;
	//��������
	InstantData698Frame[i++] = 01;
	InstantData698Frame[i++] = 03;
	//����ṹ����������
	for (j = 0; j < MAX_PHASE_NUM; j++)
	{
		InstantData698Frame[i++] = 02;
		InstantData698Frame[i++] = DATA_ITEM_NUM_PER_PHASE;
		for (k = 0; k < DATA_ITEM_NUM_PER_PHASE; k++)
		{
			if (InstantDataFormatTable[k] == edouble_long)
			{
				InstantData698Frame[i++] = 5;
			}
			else if (InstantDataFormatTable[k] == efloat32)
			{
				InstantData698Frame[i++] = 23;
			}
			memcpy(&InstantData698Frame[i], pTmp, 4);
			i += 4;
			pTmp += 4;
		}
	}
	//���
	InstantData698Frame[i++] = 5;
	memcpy(&InstantData698Frame[i], pTmp, 4);
	i += 4;
	//�����ϱ���ʱ���ǩ
	InstantData698Frame[i++] = 0x00;
	InstantData698Frame[i++] = 0x00;
	//У��
	Fcs.w = fcs16((BYTE *)&InstantData698Frame[1], i - 1);
	InstantData698Frame[i++] = Fcs.b[0];
	InstantData698Frame[i++] = Fcs.b[1];
	InstantData698Frame[i++] = 0x16;
	nwy_ext_echo("\r\n698len:%d\r\n", i);
	for (WORD j = 0; j < i; j++)
	{
		nwy_ext_echo("%02x ", InstantData698Frame[j]);
	}
}
//--------------------------------------------------
//��������:  ����վ���� ��ѹ����ϵ�� ��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Compose_SendRequestfactor_698( float uk,float ik,BYTE *buf,BYTE Loop)
{
	BYTE i = 17;
	BYTE MessageBuf[54] = {0};
	BYTE start[] = {0x68, 0x33, 0x00, 0xc3, 0X05};
	QWORD	qwSec;
	TTwoByteUnion Hrc, Fcs;
	TRealTimer RealTimeTmp= {.Mon=1,.Day=1,.Hour=0,.Min=0,.Sec=0};
	TRealTimer RealTimelocal = {0};
	BYTE bRequset[] = {0x85,0x01,0x00};
	get_N176_time(&RealTimelocal);

	RealTimeTmp.wYear = RealTimelocal.wYear;
	qwSec = api_CalcInTimeRelativeSec(&RealTimelocal) - api_CalcInTimeRelativeSec(&RealTimeTmp);
	memcpy((char *)&MessageBuf, (char *)&start, sizeof(start));
	lib_ExchangeData((BYTE *)&MessageBuf[5], (BYTE *)&MeterWaveAddr[0], 6);
	MessageBuf[11] = 0xA0; //�ͻ�����ַ
	Hrc.w = fcs16((BYTE *)&MessageBuf[1], 11);
	MessageBuf[12] = Hrc.b[0];
	MessageBuf[13] = Hrc.b[1];
	memcpy((char *)&MessageBuf[14], (char *)&bRequset, sizeof(bRequset));
	// oi
	MessageBuf[i++] = 0xee;
	MessageBuf[i++] = 0xee;
	MessageBuf[i++] = 0x44;
	MessageBuf[i++] = 0x44;

	MessageBuf[i++] = 0x01;
	MessageBuf[i++] = 0x02;
	MessageBuf[i++] = 0x06;
	MessageBuf[i++] = 0x0f;

	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x0f;
	MessageBuf[i++] = Loop;//  ��ǰ��·��

	MessageBuf[i++] = 0x06;
	lib_ExchangeData(&MessageBuf[i],(BYTE*)&qwSec,4);
	i = i+4;
	MessageBuf[i++] = 0x06;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x00;

	MessageBuf[i++] = 0x17;
	lib_ExchangeData(&MessageBuf[i],(BYTE*)&uk,sizeof(uk));
	i = i+4;
	MessageBuf[i++] = 0x17;
	lib_ExchangeData(&MessageBuf[i],(BYTE*)&ik,sizeof(uk));
	i = i+4;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x00;

	Fcs.w = fcs16((BYTE *)&MessageBuf[1], 50 - 1);
	MessageBuf[50] = Fcs.b[0];
	MessageBuf[51] = Fcs.b[1];
	MessageBuf[52] = 0x16;

	nwy_ext_echo("\r\n request data to server\r\n");
	for (BYTE i = 0; i < 54; i++)
	{
		nwy_ext_echo("%02x ", MessageBuf[i]);
	}
	memcpy(buf,MessageBuf,sizeof(MessageBuf));
}
//--------------------------------------------------
//��������:  ����������֡
//
//����:       bLink 0 ��¼ 1����
//         	  transdata �����ַ
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Compose_Heartbeatframe_698(BYTE *buf, BYTE bLink)
{
	char MessageBuf[50];
	BYTE start[5] = {0x68, 0x1e, 0x00, 0x81, 0x05};
	BYTE datamid[] = {0x01, 0x01}; // Linkrequest
	WORD heartbeat = 60;
	TRealTimerDate time = {0};
	TTwoByteUnion Hrc, Fcs;
	memset(MessageBuf, 0, sizeof(MessageBuf));
	memcpy((char *)&MessageBuf, (char *)&start, sizeof(start));
	// memcpy((char*)&MessageBuf[5],(char*)&bLtoAddr,sizeof(bLtoAddr));
	lib_ExchangeData((BYTE *)&MessageBuf[5], (BYTE *)&MeterWaveAddr[0], 6);
	// MessageBuf[5] = 5; //��ַ��ʱ�Ĳ���
	MessageBuf[11] = 0; //�ͻ�����ַ
	Hrc.w = fcs16((BYTE *)&MessageBuf[1], 11);
	MessageBuf[12] = Hrc.b[0];
	MessageBuf[13] = Hrc.b[1];
	memcpy((char *)&MessageBuf[14], (char *)&datamid, sizeof(datamid));
	MessageBuf[16] = bLink;
	lib_ExchangeData((BYTE *)&MessageBuf[17], (BYTE *)&heartbeat, sizeof(heartbeat));
	GetTimeto698(&time);
	memcpy((char *)&MessageBuf[19], (char *)&time, sizeof(time));
	Fcs.w = fcs16((BYTE *)&MessageBuf[1], 29 - 1);
	MessageBuf[29] = Fcs.b[0];
	MessageBuf[30] = Fcs.b[1];
	MessageBuf[31] = 0x16;
	// nwy_ext_echo("\r\n 698 heart/login\r\n");
	// for (BYTE i = 0; i < 40; i++)
	// {
	// 	nwy_ext_echo("%02x ", MessageBuf[i]);
	// }

	memcpy(buf,MessageBuf,32);
	// transdata->len = 32;
	// nwy_put_msg_que(TranDataToTcpUserServerMsgQue, transdata, 0xffffffff);
}
//--------------------------------------------------
//��������:  �ر���������ȫģʽ 
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Close_SafeMode( void )
{
	BYTE i = 18;
	BYTE MessageBuf[54] = {0};
	BYTE start[6] = {0x68, 0x24, 0x00, 0x43, 0x26,0X03};
	TTwoByteUnion Hrc, Fcs;
	TRealTimerDate time = {0};
	BYTE bRequset[] = {0x06,0x01,0x00};
	
	memcpy((char *)&MessageBuf, (char *)&start, sizeof(start));
	lib_ExchangeData((BYTE *)&MessageBuf[6], (BYTE *)&bLtoAddr[0], 6);
	MessageBuf[12] = 0xA0; //�ͻ�����ַ
	Hrc.w = fcs16((BYTE *)&MessageBuf[1], 12);
	MessageBuf[13] = Hrc.b[0];
	MessageBuf[14] = Hrc.b[1];
	memcpy((char *)&MessageBuf[15], (char *)&bRequset, sizeof(bRequset));
		// oi
	MessageBuf[i++] = 0xF1;
	MessageBuf[i++] = 0x01;
	MessageBuf[i++] = 0x02;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x16;
	MessageBuf[i++] = 0x00;

	MessageBuf[i++] = 0x01;

	GetTimeto698(&time);
	memcpy((char *)&MessageBuf[i], (char *)&time, sizeof(time));
	i = i+sizeof(time)-3;
	MessageBuf[i++] = 0x05;
	MessageBuf[i++] = 0x00;
	MessageBuf[i++] = 0x01;
	Fcs.w = fcs16((BYTE *)&MessageBuf[1], 34);
	MessageBuf[35] = Fcs.b[0];
	MessageBuf[36] = Fcs.b[1];
	MessageBuf[37] = 0x16;

	// nwy_ext_echo("\r\n CLOSE MODE  is\r\n");
	// for (BYTE i = 0; i < 54; i++)
	// {
		// nwy_ext_echo("%02x ", MessageBuf[i]);
	// }
	nwy_uart_send_data(UART_HD_LTO,MessageBuf,sizeof(MessageBuf));
}
//--------------------------------------------------
//��������:   ��ȫ������֡  ��֡������������Ӷ��� ���
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Compose_SecurityRequsetRecordFreezeMin_698( void )
{
	BYTE MessageBuf[75] = {0};
	BYTE start[6] = {0x68, 0x49, 0x00, 0x43, 0x26,0X03};
	TTwoByteUnion Hrc, Fcs;
	BYTE bRequset[] = {0x10,0x00,0x24};
	BYTE bData[] ={0x05,0x03,0x00,0x50,0x02,0x02,0x00,0x09,0x01,0x03,0x00,0x20,0x23,0x02,0x00,0x00,0x20,0x21,0x02,0x00,0x00,0x48,0x00,0x04,0x00,0x01,0x07,0xe9,0x02,0x0f,0x0e,0x0e,0x2b,0x05,0x00,0x01};
	BYTE Rn[] ={0x01,0x10,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
	
	memcpy((char *)&MessageBuf, (char *)&start, sizeof(start));
	lib_ExchangeData((BYTE *)&MessageBuf[6], (BYTE *)&bLtoAddr[0], 6);
	MessageBuf[12] = 0xA0; //�ͻ�����ַ
	Hrc.w = fcs16((BYTE *)&MessageBuf[1], 12);
	MessageBuf[13] = Hrc.b[0];
	MessageBuf[14] = Hrc.b[1];
	memcpy((char *)&MessageBuf[15], (char *)&bRequset, sizeof(bRequset));
	memcpy((char *)&MessageBuf[15+sizeof(bRequset)], (char *)&bData, sizeof(bData));
	memcpy((char *)&MessageBuf[15+sizeof(bRequset)+sizeof(bData)], (char *)&Rn, sizeof(Rn));
	Fcs.w = fcs16((BYTE *)&MessageBuf[1], 72 - 1);
	MessageBuf[72] = Fcs.b[0];
	MessageBuf[73] = Fcs.b[1];
	MessageBuf[74] = 0x16;

	nwy_ext_echo("\r\n freeze SAFE is\r\n");
	for (BYTE i = 0; i < 75; i++)
	{
		nwy_ext_echo("%02x ", MessageBuf[i]);
	}
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO,MessageBuf,sizeof(MessageBuf));
}
//--------------------------------------------------
//��������:  ̽������ַ698
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Adrr_Txd_698(void)
{
	BYTE reqAddr[]={0x68,0x17,0x00,0x43,0x45,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xA0,0x51,0xEA,0x05,0x01,0x00,0x40,0x01,0x02,0x00,0x00,0xED,0x03,0x16};
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO,reqAddr,sizeof(reqAddr));
}

//--------------------------------------------------
// ��������:  ��ģ��汾
//
// ����:
//
//
//
// ����ֵ:
//
// ��ע����:  �ظ�����������645���ģ�����Ҫͨ��ReadModuleVersion��־����645��ʽ�ж�
//--------------------------------------------------
void ComposeReadModuleVersion(void)
{
	ReadModuleVersion = 1;
	BYTE Buf[100] ={0};
	BYTE head[] = {0x68, 0x2e,0x00, 0x43, 0x26, 0x03};
	BYTE Apdu[] = {0x07, 0x01, 0x00, 0x20, 0xEB, 0x03, 0x00, 0x09,
				   0x14, 0x68, 0x05, 0x00, 0x08, 0x03, 0x24, 0x20,
				   0x68, 0x11, 0x08, 0x0E, 0x12, 0x12, 0x37, 0x0E,
				   0x3B, 0x12, 0x0E, 0x0F, 0x16, 0x00};
	WORD Len = 0;
	WORD Hcs = 0, Fcs = 0;

	memcpy(Buf, head, sizeof(head));
	Len += sizeof(head);
	memcpy(&Buf[Len], ModuleReverAddr, sizeof(ModuleReverAddr));
	Len += sizeof(ModuleReverAddr);
	Buf[Len++] = 0;
	Hcs = fcs16(&Buf[1], 12);
	Buf[Len++] = Hcs;
	Buf[Len++] = Hcs >> 8;
	memcpy(&Buf[Len], Apdu, sizeof(Apdu));
	Len += sizeof(Apdu);
	Fcs = fcs16(&Buf[1], 44);
	Buf[Len++] = Fcs;
	Buf[Len++] = Fcs >> 8;
	Buf[Len++] = 0x16;

	nwy_ext_echo("\r\nread module version");
	for (int i = 0; i < Len; i++)
	{
		nwy_ext_echo("%02x ", Buf[i]);
	}
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO, Buf, Len);
}
//--------------------------------------------------
//��������:  ��ȡ��ʶģ��İ汾 ��ȫ���䣿
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  ComposeReadModuleVersion_security( void )
{
	BYTE MessageBuf[57] = {0};
	BYTE start[6] = {0x68, 0x37, 0x00, 0x43, 0x26,0X03};
	TTwoByteUnion Hrc, Fcs;
	BYTE bRequset[] = {0x10,0x00,0x12};
	BYTE bData[] ={0x05,0x01,0x00,0x40,0x31,0x03,0x00,0x01,0x07,0xe9,0x02,0x0f,0x0e,0x0e,0x2b,0x05,0x00,0x01};
	BYTE Rn[] ={0x01,0x10,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
	
	memcpy((char *)&MessageBuf, (char *)&start, sizeof(start));
	lib_ExchangeData((BYTE *)&MessageBuf[6], (BYTE *)&bLtoAddr[0], 6);
	MessageBuf[12] = 0xA0; //�ͻ�����ַ
	Hrc.w = fcs16((BYTE *)&MessageBuf[1], 12);
	MessageBuf[13] = Hrc.b[0];
	MessageBuf[14] = Hrc.b[1];
	memcpy((char *)&MessageBuf[15], (char *)&bRequset, sizeof(bRequset));
	memcpy((char *)&MessageBuf[15+sizeof(bRequset)], (char *)&bData, sizeof(bData));
	memcpy((char *)&MessageBuf[15+sizeof(bRequset)+sizeof(bData)], (char *)&Rn, sizeof(Rn));
	Fcs.w = fcs16((BYTE *)&MessageBuf[1], 53);
	MessageBuf[54] = Fcs.b[0];
	MessageBuf[55] = Fcs.b[1];
	MessageBuf[56] = 0x16;

	nwy_ext_echo("\r\n safe read module versionis\r\n");
	for (BYTE i = 0; i < 57; i++)
	{
		nwy_ext_echo("%02x ", MessageBuf[i]);
	}
	ENABLE_HARD_SCI_SEND;
	nwy_uart_send_data(UART_HD_LTO,MessageBuf,sizeof(MessageBuf));
}
//--------------------------------------------------
//��������:  ����698����֡��ʼ��68����
//
//����:
//           TSerial * p[in]
//
//����ֵ:
//
//��ע����:  ��
//--------------------------------------------------
void DoSearch_68_DLT698(TSerial *p)
{
	WORD i, wNum;
	wNum = p->ProStepDlt698_45; //3
	for (i = 1; i < wNum; i++)  //i  1 2
	{
		// nwy_ext_echo("begin pos %d\r\n", p->BeginPosDlt698_45);
		if (p->ProBuf[(p->BeginPosDlt698_45 + i) % UART_BUFF_MAX_LEN] == 0x68)
		{
			if (p->ProStepDlt698_45 == 0)
			{
				p->ProStepDlt698_45 = UART_BUFF_MAX_LEN - 1;
			}
			else
			{
				p->ProStepDlt698_45--;
			}
			p->BeginPosDlt698_45 += i;
			if (p->BeginPosDlt698_45 >= UART_BUFF_MAX_LEN)
			{
				p->BeginPosDlt698_45 = 0;
			}
			break;
		}
	}
	if (i >= wNum)
	{
		p->ProStepDlt698_45 = 0;
		// nwy_ext_echo("\r\n2 pro setp is set to 0");
	}
}
//-----------------------------------------------
// ��������: �ж�У���Ƿ���ȷ
//
// ����:		Type 0--HCS 1--FCS
//
// ����ֵ:
//
// ��ע:һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
//-----------------------------------------------
WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p)
{
	WORD i, wLen, wData;
	BYTE Buf[MAX_PRO_BUF + 30];

	if (Type == 0)
	{
		wLen = (4 + p->Addr_Len + 1);
	}
	else if (Type == 1)
	{
		wLen = p->wLen - 2;
	}
	else
	{
		return FALSE;
	}
	if (wLen >= (MAX_PRO_BUF - 1))
	{
		return FALSE;
	}

	for (i = 0; i < wLen; i++)
	{
		Buf[i] = p->ProBuf[(p->BeginPosDlt698_45 + 1 + i) % MAX_PRO_BUF];
	}

	// 68 L L 43 05 01 00 29 01 16 20 0A HCS_L HCS_H 10 00 08 05 01 01 40 01 02 00 00 00 85 01 02 03 06 12 34 56 78 90 12 04 12 34 56 78 FCS_L FCS_H 16
	wData = p->ProBuf[(p->BeginPosDlt698_45 + p->ProStepDlt698_45 - 1) % MAX_PRO_BUF] * 0x100 + p->ProBuf[(p->BeginPosDlt698_45 + p->ProStepDlt698_45 - 2) % MAX_PRO_BUF];
	if (wData != fcs16(Buf, wLen)) // �Ա��Ľ���У��
	{
		return FALSE;
	}

	return TRUE;
}
//--------------------------------------------------
//��������:  4Gģ�� �Խ��ܵı��Ľ���(698��Լ)
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  RxUartMessage_Dlt698( TSerial *p )
{
	BYTE *buf = NULL;
	BYTE  bResponse;
	WORD  wOI,i;
	tTranData tempTrandata;
	TTwoByteUnion Hrc, Fcs;

	memset((BYTE*)&tempTrandata,0,sizeof(tempTrandata));
	bResponse = p->ProBuf[p->Addr_Len+8];
	buf = &(p->ProBuf[p->Addr_Len+11]);
	lib_ExchangeData((BYTE*)&wOI,buf,2);

	switch (bResponse)
	{
	case 0x85:
		if (wOI == 0x4001)
		{
			buf = &(p->ProBuf[24]);
			memcpy(bLtoAddr,buf,6);
			lib_ExchangeData((BYTE*)&ModuleReverAddr,buf,6);
			nwy_ext_echo("\r\n addr success");
		}
		break;
	case 0x86:
		nwy_ext_echo("\r\n close success");
		if (wOI == 0x4000)
		{
			if (buf[4] == 0)
			{
				nwy_ext_echo("\r\n Set time success");
			}
		}
		break;
	case 0x90://��ȫ��Ӧ
		nwy_ext_echo("\r\n safe response");
		lib_ExchangeData(&p->ProBuf[p->Addr_Len-1],(BYTE*)&MeterWaveAddr,sizeof(MeterWaveAddr));
		Hrc.w = fcs16((BYTE *)&p->ProBuf[1], p->Addr_Len+5);
		p->ProBuf[p->Addr_Len+6] = Hrc.b[0];
		p->ProBuf[p->Addr_Len+7] = Hrc.b[1];

		Fcs.w = fcs16((BYTE *)&p->ProBuf[1], p->wLen-2);
		p->ProBuf[p->wLen-1]  = Fcs.b[0];
		p->ProBuf[p->wLen]  = Fcs.b[1];

		for (i = 0; i < p->wLen+2; i++)
		{
			nwy_ext_echo("%02x",p->ProBuf[i]);
		}
		memcpy((BYTE*)&tempTrandata,p->ProBuf,p->wLen+2);
		tempTrandata.len = p->wLen+2;
		if (nwy_put_msg_que(TranDataToTcpUserServerMsgQue, &tempTrandata, 0xffffffff))
		{
			nwy_ext_echo("\r\nuart to mqtt/tcp queue ok,length:%d", tempTrandata.len);
		}
		else
		{
			nwy_ext_echo("\r\nto mqtt queue err");
		}
		
		break;
	default:
		break;
	}
}
void Proc698(TSerial *p)
{
	BYTE *buf = p->ProBuf;
	WORD offset = 18;

	if ((*(DWORD *)(buf + offset)) == 0x0003eb20) //�򿪳��ڻظ����Ͷ��汾һ����ʶ,apdu��645����
	{
		if (ReadModuleVersion == 1)//���汾
		{
			ReadModuleVersion = 0;
			ModuleAppVersion[0] = buf[50] - 0x33;
			ModuleAppVersion[1] = buf[51] - 0x33;
			ModuleAppVerif[0]  = buf[56] - 0x33;
			ModuleAppVerif[1]  = buf[57] - 0x33;
			nwy_ext_echo("\r\nModuleAppVersion: %02X%02X", ModuleAppVersion[0], ModuleAppVersion[1]);
			nwy_ext_echo("\r\nModuleAppVerif: %02X%02X", ModuleAppVerif[0], ModuleAppVerif[1]);
			sprintf(ModuleVersion, "Moduleversion:%02X%02X_%02X%02X", 
						ModuleAppVersion[0], ModuleAppVersion[1], ModuleAppVerif[0], ModuleAppVerif[1]);
			ReceiveClearBitFlag();
		}
		else//�򿪳��ڻظ�
		{
			if (buf[offset + 4] == 0x00)
			{
				// MoniIapNo = 1;
				OpenFactoryStep = eSuc;
				nwy_ext_echo("\r\n open factory suc ");
			}
			else //dar����
			{
				ExitUpgrade698(eUpgradeDARErr);
				nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
			}
			nwy_stop_timer(uart_timer);
		}
	}
	else
	{
		RxUartMessage_Dlt698(p);
		ReceiveClearBitFlag();
		nwy_ext_echo("\r\n other omd ");
	}
	
}
//--------------------------------------------------
// ��������:  ����698��ʽ�ж�
//
// ����:      BYTE ReceByte[in]
//
//           TSerial * p[in]
//
// ����ֵ:
//
// ��ע����:  ��
//--------------------------------------------------
WORD DoReceProc_Dlt698_UART(BYTE ReceByte, TSerial *p)
{
	// static BYTE AddrOffset = 0;
	// ����ͬ��ͷ

	if (p->ProStepDlt698_45 == 0)
	{
		if (ReceByte == 0x68)
		{
			// Dlt698.45��Լ������Serial[].ProBuf�еĿ�ʼλ��
			p->BeginPosDlt698_45 = p->RXRPoint;

			// ����ָ��
			p->ProStepDlt698_45++;
		}
	}
	else if (p->ProBuf[p->BeginPosDlt698_45] == 0x68)
	{
		// ����ָ��
		p->ProStepDlt698_45++;
		if (p->ProStepDlt698_45 < 3) // ���û�յ�3�ֽڣ���������û��ȫ���򲻴���
		{
		}
		else if (p->ProStepDlt698_45 == 3) // �Ѿ��յ���3�����ˣ�������L
		{
			p->wLen = (ReceByte * 0x100 + p->ProBuf[p->BeginPosDlt698_45 + 1]);
			if (p->wLen > (UART_BUFF_MAX_LEN - 2)) //(UART_BUFF_MAX_LEN-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			if (p->wLen < (10 + 6)) // ��������СΪ���٣�ͨ�ŵ�ַ��6�ֽ�
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if (p->ProStepDlt698_45 == 4) // �Ѿ��յ���4�����ˣ���������C
		{
			if (!(ReceByte & 0x80)) // ������Ƿ������ظ�����������ʼ��
			{
				DoSearch_68_DLT698(p);

				return FALSE;
			}
			else if (ReceByte == 0x01)
			{
				// ֻ�ж��Ƿ� ����·���� ���������false
			}
			else
			{
				return FALSE;
			}
			// //�������жϣ���û��
			// //1	��·����	��·���ӹ�����¼���������˳���¼��
			// //3	�û�����	Ӧ�����ӹ������ݽ�������
			// if( (ReceByte & 0x07) != 0x03 )//���Ӧ�� ERROR-Response ������Ȫ���˶���Ӧ���Ų���������� 2017-7-29 wlk
			// {
			// 	DoSearch_68_DLT698(p);
			// 	return FALSE;
			// }
		}
		else if (p->ProStepDlt698_45 == 5) // �Ѿ��յ���5�����ˣ�����������ַSA�ĵ�1�ֽ�
		{
			// a)	bit0��bit3��Ϊ��ַ���ֽ�����ȡֵ��Χ��0��15����Ӧ��ʾ1��16���ֽڳ��ȣ�
			// b)	bit4��bit5���߼���ַ��
			// c)	bit6��bit7��Ϊ��������ַ�ĵ�ַ���ͣ�0��ʾ����ַ��1��ʾͨ���ַ��2��ʾ���ַ��3��ʾ�㲥��ַ��
			p->Addr_Len = (ReceByte & 0xf) + 1;
			if ((ReceByte / 0x40) <= 1) // ����ǵ���ַ��ͨ���ַ
			{
				if (ReceByte & 0x20) //���߼���ַ	??�ϲ�������
				{
					p->HasLogicAddr = 1;
					// AddrOffset = 6;
					// HCSOffset  = 9;
					if (p->Addr_Len != 7) // ����ַ��6�ֽ�BCD����12λBCD��
					{
						DoSearch_68_DLT698(p);
						return FALSE;
					}
				}
				else
				{
					p->HasLogicAddr = 0;
					// AddrOffset = 5;
					// HCSOffset = 8;
					if (p->Addr_Len != 6) // ����ַ��6�ֽ�BCD����12λBCD��
					{
						DoSearch_68_DLT698(p);
						return FALSE;
					}
				}
			}
		}
		else if (p->ProStepDlt698_45 == (8 + p->Addr_Len)) // ��ַ�����ˣ�HCS�����ˣ��ж�HCS�Ƿ���ȷ
		{
			// if (p->Addr_Len > 6)
			// {
				// DoSearch_68_DLT698(p);

				// return FALSE;
			// }
			// //һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
			if (JudgeDlt698_45_HCS_FCS(0, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}

		}
		else if (p->ProStepDlt698_45 == (1 + p->wLen)) // �յ�֡������ 16H�ˣ��ж�FCS�Ƿ���ȷ
		{
			// һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
			if (JudgeDlt698_45_HCS_FCS(1, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if (p->ProStepDlt698_45 == (2 + p->wLen)) // �յ�֡������ 16H��,��û�ж�16H�Ƿ������ȷ
		{
			if (ReceByte == 0x16)
			{
				// 	//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
				api_DoReceMoveData(p, PROTOCOL_698); // ��÷��� 698.45Э�鴦���� ��

				// //	�˴�����698.45Э�鴦����//ProcDlt698App();//ProMessage( p );
				// 	ProcMessageDlt698(p);
				if (ProcUpgradeResponse698(p)) // �������һ֡
				{
				}
				else //��������698����
				{
					Proc698(p);
				}
				// �������
				InitPoint(p);
				// nwy_ext_echo("\r\n  698 RECEIVE process over \r\n");

				return TRUE;
			}
			else
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else
		{
			if (p->ProStepDlt698_45 >= UART_BUFF_MAX_LEN) //(UART_BUFF_MAX_LEN-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			{
				nwy_ext_echo("\r\n3 pro setp is set to 0 ");
				p->ProStepDlt698_45 = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}