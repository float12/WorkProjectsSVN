//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_dlt698.h"
#if( READMETER_PROTOCOL ==  PROTOCOL_645)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define UNSIGNED			0x00								//�޷���
#define SIGNED				0x80								//�з���
#define PT_MULTI			0x01								//�ϴ����ݳ�PT
#define CT_MULTI			0x02						
#define PT_CT_MULTI			0x03						


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
#pragma pack(1)
typedef struct _position{
	BYTE location;				//��λ
	WORD degree;				//��  
	float min;					//�� 
}position;
#pragma pack()

#pragma pack(1)
typedef struct _TOad40040200{
	position longitude;				//����  
	position latitude;				//γ��   
	DWORD high;					//�߶� cm
}TOad40040200;
#pragma pack()

typedef struct tMeterRead_t
{
	DWORD OI;				//645���ݱ�ʶ
	BYTE Num;				//һ�����ݿ��м���������
	BYTE Slen;				//�����������
	BYTE Dot;				//��������С��λ
	BYTE Symbol;			//�Ƿ���Ҫ�������λ bit7 �����ж� bit1 bit0 ���ֱ��
	double* Buf;			//���ݴ�ŵ�ַ
}tMeterRead;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
char bAddr[6];				//���ͨ�ŵ�ַ
char reverAddr[6];			//��ַ���𷽱㷢��
TSerial Serial;	
ServerParaRam_t	Serverpara[2];
char MeterVersion[35] = {0};
char LtuAddr[30] = {0};
DWORD PowerOnReadMeterFlag = 0;//�ϵ糭��ɹ���bitλ Ŀǰ�߸�
BYTE PublishInstantData[MAX_PHASE_NUM*INSTANT_DATA_LEN_PER_PHASE + 4];//4�ֽ����

BYTE bHisReadMeter[MAX_SAMPLE_CHIP] = {0}; // �ϴγ���bitλ
DWORD dwReadMeterFlag[MAX_SAMPLE_CHIP] = {0};
BYTE bReadMeterRetry = 0;
//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
TOtherPara OtherPara;
static BYTE Loop = 0;
//��չ��Լ���ݱ�ʶ
const WORD  EventOI[]  = { 0x1700 };
const WORD  PtandCt[]  = { 0x1118,0x1119,0x111A,0x111B};//CT1,CT2,pt1,pt2
const WORD  Position[] = { 0x1300};
BYTE SampleStep = 1;		//˲ʱ�������ܳ���Step��������
BYTE ParaPowerOnStep = 0; 	//�ϵ糭����Step

#if(CYCLE_REPORT == YES)
double MeterData[MAX_SAMPLE_CHIP][SINGLE_LOOP_ITEM+MAX_RATIO*4];//���ɼ������ݴ�Ŵ�
tMeterRead tMeterReadOIRam;
const tMeterRead  MeterReadOI[] = 
{     
	{0x0201FF00,3,2,1,PT_MULTI,&MeterData[0][0]  },											//��ѹ���ݿ�	
	{0x0211FF00,3,2,1,PT_MULTI,&MeterData[0][3]  },											//�ߵ�ѹ���ݿ�
	{0x02800001,1,3,3,SIGNED|CT_MULTI,&MeterData[0][6]  },									//�������
	{0x0202FF00,3,3,3,SIGNED|CT_MULTI,&MeterData[0][7]  },									//�������ݿ�
	{0x020CFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][10] },								//�й��������ݿ� //��׼645��ȡ3���ֽ�,��չ645����4���ֽ�
	{0x020DFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][14] },								//�޹��������ݿ�
	{0x020EFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][18] },								//���ڹ������ݿ�
	{0x0206FF00,4,2,3,SIGNED,&MeterData[0][22] },											//�����������ݿ�
	{0x02800002,1,2,2,UNSIGNED,&MeterData[0][26] },											//Ƶ��
	{0x0001FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM] },				//�����й��ܵ������ݿ�
	{0x0002FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO] },		//�����й��ܵ���
	{0x0003FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO*2] },	//�޹�1�ܵ���
	{0x0004FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO*3] },	//�޹�2�ܵ���
};
#endif //if(CYCLE_REPORT == YES)
#if(CYCLE_REPORT == YES)
const BYTE CycleReadMeterNum =  sizeof(MeterReadOI)/sizeof(tMeterRead);
#else
const BYTE CycleReadMeterNum =  0;
#endif
#if(EVENT_REPORT == YES)
static BYTE bHisDIStatus;							//��ǰң��״̬
static BYTE EventPowerOn = 0xAA;					//��ֹ���ϵ���ϱ�
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
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
	#if(PT_CT == YES)
	switch(bType)
	{
		case eType_PT:
			if( OtherPara.bPTSecond == 0)
			{
				dbData = 1;
			}
			else
			{
	
				dbData = (double)OtherPara.dwPTPrim / OtherPara.bPTSecond;//PT��ȣ��Ƿ��ӳ��Է�ĸ���ֵ
			}
			break;
		case eType_CT:
			if( OtherPara.bCTSecond == 0 )
			{
				dbData = 1;
			}
			else
			{
				dbData = (double)OtherPara.dwCTPrim / OtherPara.bCTSecond;//CT��ȣ��Ƿ��ӳ��Է�ĸ���ֵ
			}	
			break;
		case eType_PT_CT:
			if( ( OtherPara.bCTSecond == 0 ) || ( OtherPara.bPTSecond == 0) )
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
		
	}
    
    Hex = In & 0xF0;
    Hex >>= 1;
    Hex += (Hex>>2);
    Hex += In & 0x0F;
    return Hex;
}

WORD MW(BYTE Hi,BYTE Lo)
{
	return (WORD)((Hi<<8)+Lo);
}
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL)
{
	return (DWORD)((HH<<24) | (HL<<16) | (LH<<8) | LL);
}
static void nwy_hex_to_string(int len, char *data ,char *buf)
{
	int i = 0;
	unsigned char highByte;
	unsigned char lowByte;
	
	for(i = 0; i < len; i++)
	{
		highByte = data[i] >> 4;
		lowByte = data[i] & 0x0f;

		highByte += 0x30;

		if(highByte > 0x39)
			buf[i * 2] = highByte + 0x07;
		else
			buf[i * 2] = highByte;

		lowByte += 0x30;
		if(lowByte > 0x39)
			buf[i * 2 + 1] = lowByte + 0x07;
		else
			buf[i * 2 + 1] = lowByte;
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
//--------------------------------------------------
//��������:  ̽������ַ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Adrr_Txd(void)
{
	BYTE reqAddr[]={0xFE,0xFE,0xFE,0xFE,0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x13,0x00,0xDF,0x16};
	nwy_uart_send_data(UART_HD,reqAddr,sizeof(reqAddr));
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
void  Dlt645_Tx_Read_Version(void) //����OK
{
	BYTE Buf[] = {0xfe,0xfe,0xfe,0xfe,0x68,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x68,
				  0x11,0x08,0x0e,0x12,0x12,0x37,0x0e,0x3b,0x12,0x0e,0xb7,0x16};

	nwy_uart_send_data(UART_HD,Buf,sizeof(Buf));
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
void  Dlt645_Tx_Read( DWORD dwID ) //����OK
{
	BYTE bLen,Buf[20];

	bLen = 12;
	memset(Buf,0,sizeof(Buf));
	//��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );

	Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	// lib_ExchangeData((BYTE*)&Buf[5],(BYTE*)&bAddr[0],6);
	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;	
	Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);	
	Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);	
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,bLen);
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
void  Dlt645_Tx_ReadMultiLoop( DWORD dwID, BYTE *bAddress )
{
	BYTE bLen,Buf[20];

	bLen = 12;
	memset(Buf,0,sizeof(Buf));
	//��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );

	Buf[4] = 0x68;
	// memset(&Buf[5],0x88,6);
	lib_ExchangeData((BYTE*)&Buf[5],bAddress,6);
	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;	
	Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);	
	Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);	
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,bLen);
}
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
void  Dlt645_Tx_Write( DWORD dwID,BYTE bDataLen,BYTE *pBuf )//ȫ8 ��ַ������   дbuf����ʱ��֪�� Ҫ��Ҫת��ʽ
{
    BYTE i,bLen,Buf[128];

    bLen = 12;
    memset(Buf,0,sizeof(Buf));
    //��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );
    Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	Buf[5+6] = 0x68;
    Buf[bLen++] = 0x14;
    Buf[bLen++] = 12+bDataLen;
    

    Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);
    //���� �����ߴ���
    Buf[bLen++] = 0x35;
    memset((BYTE *)&Buf[bLen],0x33,7);
    bLen = bLen + 7;
    //����
    memcpy((BYTE*)&Buf[bLen],pBuf,bDataLen);
    for (i = 0; i < bDataLen; i++)
    {
       Buf[bLen] =Buf[bLen]  + 0x33;
       bLen++;
    }
    
    Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
    bLen++;
    Buf[bLen++] = 0x16;
    // for (i = 0; i < bLen; i++)
    // {
    //     nwy_ext_echo("%c",Buf[i]);
    // }
    nwy_uart_send_data(UART_HD,Buf,bLen);
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
BOOL CommWithMeter_DBDF( BYTE Type, BYTE bDataLen, BYTE *ID ,BYTE*pBuf)//��ȡ������OK
{
	BYTE i;
	WORD wLen = 12;
	BYTE Buf[128];
	//��ͬ��֡ 
	memset( (BYTE *)&Buf[0], 0x00, sizeof(Buf));
    
    //��ǰ��֡
	memset( (BYTE *)&Buf[0], 0xFE, 4 );
	
	Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	Buf[5+6] = 0x68;
	Buf[wLen++] = Type;
	
	if(Type == 0x11)//��չ��Լ����
	{
		Buf[wLen++] = 8;
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;	
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = ID[0] + 0x33;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12;//df
		Buf[wLen++] = 0x0E;//db
	}
	else if(Type == 0x14) //��չ��Լд 
	{
		Buf[wLen++] = bDataLen+20; //���ȿ�����
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;	
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = 0x35;
		memset((BYTE *)&Buf[wLen],0x33,7);
		Buf[wLen+7] = ID[0] + 0x33; 
	    wLen = wLen+8;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12;//DF
		Buf[wLen++] = 0x0E;//DB
		memset((BYTE *)&Buf[wLen],0xBB,4);
		wLen = wLen+4;

		for ( i = 0; i <bDataLen; i++)
		{
			Buf[wLen++] = pBuf[i] + 0x33;
		}
	}
	Buf[wLen] = lib_CheckSum(&Buf[4],wLen-4);
    wLen++;
	Buf[wLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,wLen);
    return TRUE;
}
#if(CYCLE_REPORT == YES	)
//--------------------------------------------------
//��������:  �¼��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void api_EventDetect( void )
{
	CommWithMeter_DBDF(0x11,0,(BYTE*)&EventOI[0],0);
}
#endif
#if(GPRS_POSITION == YES)
//--------------------------------------------------
//��������:  ��չ��Լ�������õ���λ��
//         
//����:      bMode  1���õ���λ�ã�0 ������λ��
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_CheckorSetPosition( BYTE bMode )
{
	int result = 0;
	char message[2048] = {0};
	DWORD dwJWD[9] = {0};
	TOad40040200 TLocation;
	
	memset((BYTE*)&TLocation,0,sizeof(TOad40040200));
	// ��ȡ��γ��λ��
	result = nwy_loc_get_nmea_data(message);
	if (result) {
	    nwy_ext_echo("\r\n get nmea data success");
	} else {
	    nwy_ext_echo("\r\n get nmea data fail");
	}
	get_gps_position_info(message,dwJWD);
	TLocation.latitude.location = (BYTE)dwJWD[3];
	TLocation.latitude.degree = dwJWD[2];
	TLocation.latitude.min = (dwJWD[1]*100000 + dwJWD[0])/pow(10,5);
	TLocation.longitude.location = dwJWD[7];
	TLocation.longitude.degree = dwJWD[6];
	TLocation.longitude.min = (dwJWD[5]*100000 + dwJWD[4])/pow(10,5);
	TLocation.high = dwJWD[8]*100;
	nwy_ext_echo("\r\nTLocation.longitude.location is %d",TLocation.longitude.location);
	nwy_ext_echo("\r\nTLocation.longitude.degree is %d",TLocation.longitude.degree);
	nwy_ext_echo("\r\nTLocation.longitude.min is %f",TLocation.longitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.location is %d",TLocation.latitude.location);
	nwy_ext_echo("\r\nTLocation.latitude.degree is %d",TLocation.latitude.degree);
	nwy_ext_echo("\r\nTLocation.latitude.min is %f",TLocation.latitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.high is %d",TLocation.high);
	if ((TLocation.longitude.degree == 0)&&(TLocation.latitude.degree == 0))
	{
		nwy_ext_echo("\r\nget location failed");
		if (bMode != 0)
		{
			nwy_gpio_set_value(LOCAL_LIGHT,1);
		}
		//���ص���
		return FALSE;
	}
	else
	{
		if (bMode == 1)
		{
			Dlt645_Tx_Write(0x0400041F,sizeof(TLocation),(BYTE*)&TLocation);
		}
		return TRUE;
	}
}
#endif
//--------------------------------------------------
//��������:  ��������
//         
//����:      bBtep �����־bitλ
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_ReadMeterTask645( BYTE bStep,BYTE* bAddr )
{
	BYTE bTempStep;

	bTempStep = (bStep-CycleReadMeterNum);
	if (bTempStep & 0x80)
	{
		bTempStep = 0xff;
	}
	nwy_ext_echo("\r\nreadMeter step is%d",bStep);
	// nwy_ext_echo("\r\nreadMeter bTempstep is%d",bTempStep);
	nwy_gpio_set_value(LOCAL_LIGHT,0);
	switch (bTempStep)
	{
		case eBIT_ADDR:
			Adrr_Txd();
			break;
		case eBIT_TIME:
			Dlt645_Tx_Read(DAY_TIME);
			break;
		case eBIT_TCPNET:
			Dlt645_Tx_Read(TCP_REQ_NET);
			break;
		case eBIT_MQTTNET:
			Dlt645_Tx_Read(MQTT_REQ_NET);
			break;
		case eBIT_TCPUSER:
			Dlt645_Tx_Read(TCP_REQ_USER_PD);
			break;
		case eBIT_MQTTUSER:
			Dlt645_Tx_Read(MQTT_REQ_USER_PD);
			break;
		case eBIT_VERSION:
			Dlt645_Tx_Read_Version();		
			break;
		#if(PT_CT == YES)
		case eBIT__CT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[1],0);
			break;
		case eBIT__CT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[2],0);
			break;
		case eBIT_PT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[3],0);
			break;
		case eBIT_PT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[4],0);
			break;
		#endif
		case eBIT_POSITION:
#if (GPRS_POSITION == YES)
			api_CheckorSetPosition(1);
#endif
			break;
#if(CYCLE_REPORT == YES)
		case 0xFF:
			Dlt645_Tx_ReadMultiLoop((DWORD)MeterReadOI[bStep].OI,bAddr);
			break;
#endif
		default:
			break;
	}

	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer,1000);//1s��ʱ
	api_SetSysStatus(eSYS_STASUS_START_COLLECT);
	
}
#if(CYCLE_REPORT == YES)
//-----------------------------------------------
//��������:  �������ݽ���������
//         
//����:      pBuf[in]  dataBuf[in] ���ݱ���λ��
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  DataAnalyseandSave( BYTE* pBuf ,double* dataBuf ,BYTE Len)
{
	BYTE ID[4];
	BYTE i,j,bStart,bEnd,bOffset,bSymbol,bDot;
	int symbol[4] = {1,1,1,1};		
	SDWORD temp[MAX_RATIO] = {0};//�����С������������
	char Buf[MAX_RATIO][10] = {0};
	int value = 0;
	double dwData = 0;
	j = 0;

	memcpy((BYTE*)&ID[0],(BYTE*)&tMeterReadOIRam.OI,sizeof(DWORD));
	bStart = tMeterReadOIRam.Slen -1;
	bOffset = tMeterReadOIRam.Slen;
	
	if (Len < (tMeterReadOIRam.Slen*tMeterReadOIRam.Num))	//�������ֵ�����
	{
		bEnd = Len;
	}
	else
	{
		bEnd = tMeterReadOIRam.Slen*tMeterReadOIRam.Num;
	}
	bSymbol = tMeterReadOIRam.Symbol;
	bDot = tMeterReadOIRam.Dot;
	
	for (i = bStart; i < bEnd;)
	{
		if ((bSymbol & 0x80)==0x80)
		{	
			if ((pBuf[i] & 0x80)==0x80)//���λ�ķ��Ŵ���  
			{
				pBuf[i] &= 0x7F;//���Ŵ���		
				symbol[j] = -1;
			}	
		}
		memcpy((BYTE*)&temp[j],(BYTE*)&pBuf[i-bStart],bOffset);
		sprintf((char*)Buf[j],"%lx",temp[j]);
		if ((bSymbol & 0x80)==0x80)
		{
			value =  atol(Buf[j])*symbol[j];
		}
		else
		{
			value =  atol(Buf[j]);
		}
		dwData = value / (1.0 * pow(10, bDot));
		//����֮����ܻ���ϱ�� ���double����// �Ļ�·����չ��Լ����ȣ�����·��
		if ((bSymbol & 0x0F) == PT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_PT);
		}
		else if ((bSymbol & 0x0F) == CT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_CT);
		}
		else if ((bSymbol & 0x0F) == PT_CT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_PT_CT);
		}

		//sprintf();���ϱ�Ⱥ� ĩλ���޾�С�� ͳһ���̶�С��
		// nwy_ext_echo("\r\n %f",dwData);
		memcpy(dataBuf,&dwData,sizeof(double));
		dataBuf ++;
		i = i+bOffset;
		j++;
	}	
}
//--------------------------------------------------
//��������: �������ݴ��͸�MQTT����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void SampleDatatoMqtt( TRealTimer *pTime )
{
	BYTE i;    
	QWORD qwMs;
	CollectionDatas CollectValue;
	char heapinfo[100]={0};
	int iTmp;
	for (i = 0; i < MAX_SAMPLE_CHIP; i++)
    {
        memcpy((double*)&CollectValue.CollectMeter[i][0],(double*)&MeterData[i][0],(SINGLE_LOOP_ITEM+MAX_RATIO*4)*sizeof(double)); 
		for ( BYTE j = 0; j < (SINGLE_LOOP_ITEM+MAX_RATIO*4); j++)
		{
			nwy_ext_echo("\r\n %f",CollectValue.CollectMeter[i][j]);
		}
    }
    memcpy(CollectValue.Addr,LtuAddr,sizeof(LtuAddr));
    //ʱ���
    pTime->Sec = 0;
    qwMs = getmktimems(pTime);
    CollectValue.TimeMs = qwMs;
	#if( EVENT_REPORT ==  YES)
	CollectValue.bDI = bHisDIStatus;
	#endif
    nwy_ext_echo("\r\n hour is %d",pTime->Hour);
    nwy_ext_echo("\r\n min is %d",pTime->Min);
	nwy_dm_get_device_heapinfo(heapinfo);
	iTmp = nwy_sdk_vfs_ls();
	nwy_ext_echo("RAM[%s]ROM[%d]",heapinfo,iTmp);
    if(nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
    {
        nwy_put_msg_que(CollectMessageQueue,&CollectValue,0xffffffff);
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
void  ReceiveClearBitFlag(void)
{
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	// nwy_ext_echo("\r\ncurrent loop is %d ", Loop);
	dwReadMeterFlag[Loop] &= ~(1<<bHisReadMeter[Loop]);
	PowerOnReadMeterFlag |=(1<<bHisReadMeter[Loop]);
	bHisReadMeter[Loop] = 0xFF;
	bReadMeterRetry = 0;
	nwy_stop_timer(uart_timer);
	nwy_gpio_set_value(LOCAL_LIGHT,1);
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
BOOL  DealDLT645_2007( DWORD dwID,BYTE* pBuf,BYTE Len )//ֱ�ӿ������ֽ����� ���赹�� 
{
	DWORD dwIP;
	TRealTimer tTime = {0};
	BYTE j;
	BYTE Sum = 0;
	BYTE bCorrect = 0;
	BYTE bReceiveBit = 0;
	#if(CYCLE_REPORT == YES)
	BYTE bOffset;
	WORD i;
	#endif

	if ((dwID == TCP_REQ_NET)||(dwID == MQTT_REQ_NET)||(dwID == TCP_REQ_USER_PD)||(dwID == MQTT_REQ_USER_PD))
	{
		for (j = 0; j < (Len-1); j++)
		{
			Sum += pBuf[j];
			if ((pBuf[j] != 0)&&(pBuf[j] != 0xff))//ֻҪ�в���0��ff��ֵ����Ϊ�ǺϷ�ֵ
			{
				bCorrect = 1;
			}
		}	
		if ((bCorrect !=1)||(Sum != pBuf[Len-1]))
		{
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
		tTime.wYear = 2000+lib_BBCDToBin(pBuf[6]);
		if(api_CheckClock(&tTime) == TRUE)
		{
			set_N176_time(&tTime);
			nwy_ext_echo("\r\n enter set time ");
			bReceiveBit = 1;
		}
	}
	else if (dwID == TCP_REQ_NET)
	{		
		dwIP  = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		sprintf(Serverpara[0].ServerIP,"%d.%d.%d.%d",(int)LLBYTE(dwIP),(int)LHBYTE(dwIP),(int)HLBYTE(dwIP),(int)HHBYTE(dwIP));
		Serverpara[0].ServerPort = MW(pBuf[5],pBuf[4]);
		nwy_ext_echo("\r\ntcp ip%s",Serverpara[0].ServerIP);
		nwy_ext_echo("\r\ntcp port %d",Serverpara[0].ServerPort);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_NET)
	{	
		dwIP  = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		sprintf(Serverpara[1].ServerIP,"%d.%d.%d.%d",(int)LLBYTE(dwIP),(int)LHBYTE(dwIP),(int)HLBYTE(dwIP),(int)HHBYTE(dwIP));
		Serverpara[1].ServerPort = MW(pBuf[5],pBuf[4]);
		nwy_ext_echo("\r\n mqtt ip%s",Serverpara[1].ServerIP);
		nwy_ext_echo("\r\n mqtt port%d",Serverpara[1].ServerPort);
		bReceiveBit = 1;	
	}
	else if (dwID == TCP_REQ_USER_PD)
	{
		memcpy((BYTE*)&Serverpara[0].ServerUserName[0],pBuf,32);
		memcpy((BYTE*)&Serverpara[0].ServerUserPwd[0],pBuf+32,32);
		nwy_ext_echo("\r\nTCP USER %s",Serverpara[0].ServerUserName);
		nwy_ext_echo("\r\nTCP PWD %s",Serverpara[0].ServerUserPwd);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_USER_PD)
	{	
		lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserName[0],pBuf,32);
		lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserPwd[0],pBuf+32,32);
		nwy_ext_echo("\r\n MQTT USER %s",Serverpara[1].ServerUserName);
		nwy_ext_echo("\r\n MQTT PWD %s",Serverpara[1].ServerUserPwd);
		bReceiveBit = 1;
	}
	else
	{
		#if(CYCLE_REPORT == YES)
		for (i = 0; i < CycleReadMeterNum; i++)
		{
			if (dwID == MeterReadOI[i].OI)
			{
				memcpy((BYTE*)&tMeterReadOIRam,(BYTE*)&MeterReadOI[i],sizeof(tMeterRead));
				bOffset = Loop*(SINGLE_LOOP_ITEM+MAX_RATIO*4); 
				DataAnalyseandSave(pBuf,tMeterReadOIRam.Buf + bOffset,Len);
				//ִ�е�����Ϊһ֡�������
				bReceiveBit = 1;
				if (i == (CycleReadMeterNum-1))//����������
				{
					nwy_ext_echo("\r\n %d loop is success",Loop);
					if (Loop == (MAX_SAMPLE_CHIP - 1))
					{	
						SampleDatatoMqtt(&tTimer);
					}
				}
				break;
			}
		}
		#endif
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
BOOL  DealDLT645_Factory( BYTE* pBuf,BYTE bDataLen)
{
	#if(EVENT_REPORT == YES)
	Eventmessage EventData;
	TRealTimer tTime = {0};
	QWORD qwMs;
	#endif
	BYTE bReceiveBit = 0;

	switch (pBuf[1])
	{
		case 0x13://ÿ���ȡ˲ʱ��
		if (pBuf[0] == 0x07)
		{
			if (Serial.ReadFollowFlag == 1) // ��֡����
			{
				memcpy((BYTE *)&PublishInstantData[0], (BYTE *)&pBuf[2], MAX_DATA_LEN_PER_FRAME); // 194�ֽ�����
			}
			else
			{
			}
			nwy_stop_timer(uart_timer);
		}
		break;
		case 0x17:
			#if(EVENT_REPORT == YES)
			if(EventPowerOn == 0)
			{
				if (pBuf[2] != bHisDIStatus)
				{
					nwy_ext_echo("report DI\r\n %d",pBuf[2]);
					get_N176_time(&tTime);
					qwMs = getmktimems(&tTime);
					EventData.TimeMs = qwMs;
					EventData.DIStatus = pBuf[2];
					memcpy((BYTE*)&EventData.Addr,(BYTE*)&LtuAddr,sizeof(LtuAddr));
					bReceiveBit = 1;
					if(nwy_get_queue_spaceevent_cnt(EventReportMessageQueue) != 0)
					{
					    nwy_put_msg_que(EventReportMessageQueue,&EventData,0xffffffff);
					}
				}
			}
			EventPowerOn = 0;
			bHisDIStatus = pBuf[2];
			#endif
			break;
		case 0x11://���
			#if(PT_CT == YES)
			if (pBuf[0] == 0x18)
			{
				memcpy((BYTE*)&OtherPara.dwCTPrim,(BYTE*)&pBuf[2], 4);
				nwy_ext_echo("CT1\r\n %d",OtherPara.dwCTPrim);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x19)
			{
				OtherPara.bCTSecond = pBuf[2];
				nwy_ext_echo("CT2\r\n %d",OtherPara.bCTSecond);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x1A)
			{
				memcpy((BYTE*)&OtherPara.dwPTPrim,(BYTE*)&pBuf[2], 4);
				nwy_ext_echo("PT1\r\n %d",OtherPara.dwPTPrim);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x1B)
			{
				OtherPara.bPTSecond = pBuf[2];
				nwy_ext_echo("PT2\r\n %d",OtherPara.bPTSecond);
				bReceiveBit = 1;
			}
			#endif
			break;
		case 0x08://�汾
			if (pBuf[0] == 0xDB)
			{	
				sprintf(MeterVersion, "version:%02X%02X,verification:%02X%02X", 
						pBuf[11], pBuf[10], pBuf[16], pBuf[17]);
				nwy_ext_echo("\r\nMeterVersion %s",MeterVersion);
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

	for(i=1;i<=pBuf[0];i++)
	{
		pBuf[i]=(pBuf[i]-0x33);
	}
	memcpy((BYTE*)&dwID,(BYTE*)&pBuf[1],4);

	return dwID;
	
}
void ReverseData(BYTE* buf,BYTE len)
{
	BYTE temp = 0;
	if (buf == NULL || len <= 1)
	{
		return; // ��ָ��򳤶Ȳ����Է�ת
	}

	BYTE start = 0;		// ��ʼ����
	BYTE end = len - 1; // ��������
	while (start < end)
	{
		// ���� buf[start] �� buf[end]
		temp = buf[start];
		buf[start] = buf[end];
		buf[end] = temp;

		start++;
		end--;
	}
}
//--------------------------------------------------
//��������: intתfloat
//         
//����:      ��ʼ�ֽڵ�ַ
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
float BytesToFloat(BYTE *bytes)
{
	DWORD intValue = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
	return (float)intValue;
}
//--------------------------------------------------
//��������: �����ϴ����ݸ�ʽ  
//         
//����:     
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void processInstantData(void)
{
	BYTE i = 0, j = 0;
 	BYTE *buf;
    float Float;
	//�����������ֽ�����
    for ( i = 0; i < sizeof(PublishInstantData) / 4; i++)
    {
        ReverseData(&PublishInstantData[4*i], 4);
    }
	//uipqС���㴦��
    for (i = 0; i < MAX_PHASE_NUM; i++)
    {	
		for (j = 0; j < 4; j++)
		{
			buf = &PublishInstantData[i *4*DATA_ITEM_NUM_PER_PHASE + j * 4];
			Float = BytesToFloat(buf);	
			if(j == 1)
			{
				Float /=10000;	
			}
			else
			{
				Float /=10;	
			}
			memcpy(buf, &Float, sizeof(float));
			ReverseData(buf, 4);
		}
    }

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
BOOL  RxUartMessage_Dlt645( TSerial *p )
{
	DWORD dwDataType;
	T645FmRx *pRxFm;
	BYTE *pBuf,bLen,i=0;
	BYTE *pTmp = &PublishInstantData[0];
	BYTE res = 0;
	
	pRxFm = (T645FmRx *)& p->ProBuf[0];
	
	if ((pRxFm->byCtrl == 0xB1) || (pRxFm->byCtrl == 0x92)) // ��ȡ˲ʱ��
	{
		pBuf = (BYTE *)&pRxFm->byDataLen;
		dwDataType = DataSubtract33H(pBuf); // �����ݽ��м�0x33����
		if (dwDataType == DLT645_FACTORY_ID)
		{	
			bLen = pRxFm->byDataLen - 4;
			return (DealDLT645_Factory(&pBuf[5], bLen));
		}
		else if (dwDataType == DLT645_FACTORY_FOLLOW_ID) // ����֡
		{
			bLen = pRxFm->byDataLen - 5;
			nwy_stop_timer(uart_timer);
			api_SetSysStatus(eSYS_STASUS_UART_IDLE);
			memcpy(&pTmp[MAX_DATA_LEN_PER_FRAME], &(p->ProBuf[14]), bLen);
			if (nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
			{	
				// nwy_ext_echo("publishData:\r\n len:%d\r\n",sizeof(PublishInstantData));
				// for(WORD j = 0; j < sizeof(PublishInstantData); j++)
				// {
				// 	nwy_ext_echo("%02x ",pTmp[j]);
				// }
				processInstantData();
				Compose_InstantData_698();
				// nwy_ext_echo("\r\nfinish one 698");
				res = nwy_put_msg_que(CollectMessageQueue, &InstantData698Frame[0], 10);
				memset(PublishInstantData, 0, sizeof(PublishInstantData));
				if (res == TRUE)
				{
					// nwy_ext_echo("\r\nInstant Data put to collect queue suc");
				}
				else
				{
					nwy_ext_echo("\r\nInstant Data put to collect queue fail");
				}
			}
			else
			{
				nwy_ext_echo("\r\n collect queue full");
			}
			return FALSE;		
		}
		else
		{
			bLen = pRxFm->byDataLen - 4;
			return (DealDLT645_2007(dwDataType, &pBuf[5], bLen));
		}
	}
	else if (pRxFm->byCtrl == 0x91)
	{
		//�жϵڼ���·����	
		Loop = ((pRxFm->MAddr[0])-bAddr[5]);
		if (Loop>=MAX_SAMPLE_CHIP)
		{
			Loop = 0;//��ֹ̽���ĩλ��ַ 9��10���� �������Ļ�·�ĵ�ַ
		}
		pBuf = (BYTE*)&pRxFm->byDataLen;
		dwDataType = DataSubtract33H( pBuf );//�����ݽ��м�0x33����

		if (dwDataType == DLT645_FACTORY_ID)
		{
			bLen = pRxFm->byDataLen - 6;
			return	(DealDLT645_Factory(&pBuf[5],bLen));
		}
		else
		{
			bLen = pRxFm->byDataLen - 4;
			return	(DealDLT645_2007(dwDataType,&pBuf[5],bLen));
		}
	}
	else if (pRxFm->byCtrl == 0x94)
	{
		return TRUE;
	}
	else if (pRxFm->byCtrl == 0x93)
	{
		pBuf = (BYTE*)&pRxFm->byDataLen;
		bLen = pRxFm->byDataLen;
		for(i=1;i<=pBuf[0];i++)
		{
			pBuf[i]=(pBuf[i]-0x33);
		}
		lib_ExchangeData((BYTE*)&bAddr[0],(BYTE*)&pBuf[1],bLen);
		memcpy((BYTE*)&reverAddr[0],(BYTE*)&pBuf[1],bLen);
		nwy_hex_to_string(6,bAddr,LtuAddr);
		nwy_ext_echo("\r\n meter addr:%s", LtuAddr);
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
void  DealUartMessage_Dlt645( TSerial *p  )
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
WORD DoReceProc_Dlt645_UART( BYTE ReceByte, TSerial * p )
{
	WORD i, j;
	BYTE *pBuf;
	TDlt645Head Dlt645Head;

	
	//����ͬ��ͷ
	if( p->ProStepDlt645 == 0 )
	{
		if(ReceByte == 0x68)
		{
			//д������
			//p->ProBuf[p->ProStepDlt645] = ReceByte;
			//Dlt645��Լ������Serial[].ProBuf�еĿ�ʼλ��
			p->BeginPosDlt645 = p->RXRPoint;

			//����ָ��
			p->ProStepDlt645 ++;

			//��ʼ���ն�ʱ
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
	}
	else if( p->ProBuf[p->BeginPosDlt645] == 0x68 )
	{
		//���չ�Լͷ�������֡����� 68 1 2 3 4 5 6 68 01 02
		if( p->ProStepDlt645 < sizeof(TDlt645Head) )
		{
			//д������
			//p->ProBuf[p->ProStepDlt645] = ReceByte;

			//����ָ��
			p->ProStepDlt645 ++;
			
			//�Ƿ���Դ�������
			if( p->ProStepDlt645 == sizeof(TDlt645Head) )
			{
				//�ѹ�Լ�����Ƶ���ʱ������ ѭ�������ô�
				//Num���ƶ������ֽ�
				//BeginPos:��ʲô��ַ��ʼ��
				pBuf = (BYTE *)&Dlt645Head;
				for( i=0; i<sizeof(Dlt645Head); i++ )
				{
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN];
				}
				//ӳ��ṹ
				//pDlt645Head = (TDlt645Head *)&TmpArray;
                j = p->ProStepDlt645;
				if (Dlt645Head.Control == 0xB1) // ��֡
				{
					p->ReadFollowFlag = 1;
					// nwy_ext_echo("\r\nReadFollowFlag set to 1");
				}
				else
				{
					// nwy_ext_echo("\r\nReadFollowFlag set to 0,control:%02X", Dlt645Head.Control);
					p->ReadFollowFlag = 0;
				}
				//�����ڶ�ͬ����
				if( Dlt645Head.Sync2 != 0x68 )//���û�е�2��֡��ʼ��68����ǰ���յ��ĵ�һ68���ԣ���������������һ��68
				{
					for( i=1; i<p->ProStepDlt645; i++ )
					{
						if( p->ProBuf[(p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN] == 0x68 )
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN;
							p->ProStepDlt645 -= i;
							break;
						}
						//�ĺ�68ǰ���������ݻ�68����ͨ���ϣ������ͨ�Ų��ϣ�û�����֣�����̫���ӣ�68 fe fe fe fe fe fe 68 88 88 88 88 88 88 68 11 04 33 33 34 33 e2 16
					}
					if( i >= j )//j �� p->ProStepDlt645 �ı��ݣ������յ������������꣬�����ң�ֱ��ProStepDlt645��0����
					{
						p->ProStepDlt645 = 0;//InitPoint;
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
				if( p->Dlt645DataLen >= ( UART_BUFF_MAX_LEN - 12 ) )
				{
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}
		}
		//��������
		else if( p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2 ) )
		{
			//����ָ��
			p->ProStepDlt645 ++;

			//����У���ֽ�
			if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 1) )
			{
				//�ѹ�Լ�����Ƶ���ʱ������ ѭ�������ô�
				//Num���ƶ������ֽ�
				//BeginPos:��ʲô��ַ��ʼ��
				if( ReceByte != api_CalRXD_CheckSum(0, p) )
				//if( ReceByte != ( lib_CheckSum(p->ProBuf+p->BeginPosDlt645,(10+p->Dlt645DataLen))&0xff) )
				{
					//У�鲻��ȷ
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}

			//�յ�0x16 �Ž��г�����
			else if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 2) )
			{
				//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
				api_DoReceMoveData(p,PROTOCOL_645);

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
			p->ProStepDlt645 = 0;//InitPoint(p);
		}
	}
	return FALSE;
}
#endif//#if( READMETER_PROTOCOL ==  PROTOCOL_645)