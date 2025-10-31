//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
//������	��Ȫ
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define MAX_BLE_PACKET_SIZE 			244
#define FLOW_REFRESH_FRE				(20*60)		//����ˢ��Ƶ�� 60�����һ��
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
// File_txt File;
char PrintLogSign = 0;
//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
BYTE FactoryRecvByteTimeoutFlag = 0;
T_DLT645    gPr645;   
tTranData FactoryTranData;
BYTE IsBLETranData = 0; //1��ʾ�յ�����͸������
TSerial ParaSerial;
BYTE DataBuf[UART_BUFF_MAX_LEN];
const char filepara_name[64] = {"/para.txt"};
const ServerParaRam_t ServerparaConst[2] = {
	[0] = {								//tcp
		.ServerPort = 7214,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0 
	},
	[1] = {								//mqtt
		.ServerPort = 6211,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0 
	}
};
int UartWh = 0;
BYTE WhFlag = 0;	//����bit0 ����bit1
int BleLogSwitch = 0;//������ӡlog����
static nwy_time_t poweronTimer;
static float flow_per_day = 0;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
BOOL  api_DeleteSysParaFile( void );
void  api_PowerOnCreatParaTable( void );
extern int nwy_data_get_flowcalc_info(nwy_data_flowcalc_info_t *flowcalc_info);
//-----------------------------------------------
//				��������
//-----------------------------------------------
//��ӡlog
static nwy_osiMutex_t *fact_mutex = NULL;
void nwy_ext_factory(char *fmt, ...)
{
	static char echo_str[NWY_EXT_SIO_RX_MAX];
	va_list a;
	int i, size;
	if(NULL == fact_mutex)
		fact_mutex = nwy_create_mutex();
	if(NULL == fact_mutex)
		return;
	nwy_lock_mutex(fact_mutex, 0);
	va_start(a, fmt);
	vsnprintf(echo_str, NWY_EXT_SIO_RX_MAX, fmt, a);
	va_end(a);
	size = strlen((char *)echo_str);
	i = 0;
	while(1)
	{
		int tx_size;
		tx_size = nwy_usb_serial_send((char *)echo_str + i, size - i);
		if(tx_size <= 0)
			break;
		i += tx_size;
		if((i < size))
			nwy_sleep(10);
		else
			break;
	}
	nwy_unlock_mutex(fact_mutex);
	
}

void nwy_ext_sio_data_cb(const char *data, uint32 length)
{
    WORD wLen,wLen1;
	wLen = ParaSerial.RXWPoint + length;
	if (length >= UART_BUFF_MAX_LEN)
	{
		length = UART_BUFF_MAX_LEN;
	}	
	if (wLen < UART_BUFF_MAX_LEN)
	{
		memcpy((char*)&ParaSerial.ProBuf[ParaSerial.RXWPoint],data,length);
		ParaSerial.RXWPoint += length;
	}
	else 
	{
		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
		memcpy((char*)&ParaSerial.ProBuf[ParaSerial.RXWPoint],data,wLen1);
		memcpy((char*)&ParaSerial.ProBuf[0],data+wLen1,length-wLen1);
		ParaSerial.RXWPoint = length-wLen1;
	}
	return;
}
void api_GetSoftBuildTime(TRealTimer *pT)
{
	int tmp[5]={1};
	BYTE strTemp[4] = {0}, i = 0;
	BYTE strMonth[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov","Dec"};
	
	sscanf(__DATE__, "%s %2d %4d", strTemp, &tmp[0], &tmp[1]);
    sscanf(__TIME__, "%2d:%2d:%2d", &tmp[2],&tmp[3],&tmp[4]);
    for (i = 0; i < 12; i++)
    {
        if (strTemp[0] == strMonth[i][0] && strTemp[1] == strMonth[i][1] && strTemp[2] == strMonth[i][2])
        {
            pT->Mon = i + 1;
            break;
        }
    }
	if(i >= 12)
	{
		pT->Mon = 1;
	}
	pT->Day = (BYTE)tmp[0];
	pT->wYear= (WORD)tmp[1];
	pT->Hour= (BYTE)tmp[2];
	pT->Min	= (BYTE)tmp[3];
	pT->Sec	= (BYTE)tmp[4];
} 
void factory_Recv_timer_cb(void *type)
{
	nwy_ext_echo("\r\n Ble_Recv_time out ");
	FactoryRecvByteTimeoutFlag = 1;
}
//---------------------------------------------------------------
//��������: ����645����
//
//����: 	p[in]-������
//                   
//����ֵ:   ��
//
//��ע:   
//---------------------------------------------------------------
void SendDlt645(TSerial * p)
{
	WORD TDataLength;
	BYTE i;
    
	TDataLength = p->ProBuf[9];
	
	//fe fe fe fe 68 A0...A5 68 81 L D0..DL CS 16,��D0...DL�⣬���๲16�ֽ�
	if( TDataLength > (UART_BUFF_MAX_LEN-16) )//�������
	{//�˾�Ӧ�ɷ��ѱ���ڴ����ݳ���
		return;
	}
	
	i=(11+TDataLength);
	for(;;)
	{
		p->ProBuf[i+4]=p->ProBuf[i];
		if(i==0)
		{
			break;
		}
		i--;
	}

	p->ProBuf[0]=0x0fe;
	p->ProBuf[1]=0x0fe;
	p->ProBuf[2]=0x0fe;
	p->ProBuf[3]=0x0fe;

	p->SendLength = (TDataLength + 16);//����FE FE FE FE ���16

	#if( UART_WH== YES)
	if (WhFlag&BIT1)
	{
		nwy_uart_send_data(UartWh,(BYTE*) &p->ProBuf[0],p->SendLength);
		WhFlag &= ~(BIT1);
		nwy_ext_echo("uart  send");
	}
	#endif
	#if( BLE_WH== YES)
	if(WhFlag&(BIT0))
	{
		nwy_ble_send_data(p->SendLength, (char*) &p->ProBuf[0]);
		WhFlag &= ~(BIT0);
		nwy_ext_echo("ble   send");
	}
	#endif
	#if (TCP_COMM_MODULE == YES)
	if (WhFlag & BIT2)
	{
		WhFlag &= ~(BIT2);

		memcpy(FactoryTranData.buf,(char *)&p->ProBuf[0],p->SendLength);
		FactoryTranData.len = p->SendLength;
		if (nwy_put_msg_que(ModuleDataToTcpMsgQue, &FactoryTranData, 0xffffffff))
		{
			nwy_ext_echo("\r\n module to tcp success");
		}
	}
	#endif
	nwy_usb_serial_send((char*) &p->ProBuf[0],p->SendLength);
}
//--------------------------------------------------
//��������:  ����645���ݳ���
//         
//����:      TYPE_PORT PortType[in]:��������
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void FillDlt645FrameData( WORD wResult )
{
    WORD Lenth;
	
    if( wResult < 0x8000 )//����Ӧ��
    {
        Lenth = wResult;
        gPr645.wDataLen = Lenth;
        switch( gPr645.byReceiveControl )
        {
            case 0x11:
                if( gPr645.wDataLen <= 196 )
                {
                	gPr645.pMessageAddr[9] = gPr645.wDataLen+4;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ
                	memcpy( gPr645.pMessageAddr+10+4, DataBuf, Lenth );
                }
                else //��Ҫ��֡����
                {

                }
                break;
            case 0x14:
                gPr645.pMessageAddr[9] = 0;//���ݳ���Ϊ��
                break;
        }
    }
    else//�쳣Ӧ��
	{
	}
}
//--------------------------------------------------
//��������:  ����698Э��͸������
//
//����:     p  
//
//����ֵ:    ��
//
//��ע����:  ��
//--------------------------------------------------
void Deal_BLETran698(TSerial *p)
{
	nwy_ext_echo("\r\n 698 whole received");

	//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ������÷��� 698.45Э�鴦���� ��
	api_DoReceMoveData(p,PROTOCOL_698);

	nwy_ext_echo("\r\n698 FrameLength:%d", p->ProStepDlt698_45);


	memcpy(FactoryTranData.buf, &(p->ProBuf[0]), p->ProStepDlt698_45);
	FactoryTranData.len = p->ProStepDlt698_45;
	FactoryTranData.TranType = eTRAN_BLETOOTH;

	if (nwy_put_msg_que(TranDataToUartMessageQueue, &FactoryTranData, 0xffffffff) == TRUE)
	{
		nwy_ext_echo("\r\nput blueTran data to uart queue success");
	}
	else
	{
		nwy_ext_echo("\r\nput blueTran data to uart queue fail");
	}
}
//--------------------------------------------------
//��������:  ��Ӧ���Ĵ���
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
void Proc645LinkDataResponse( WORD wResult)
{
    BYTE i;
	BYTE *pBuf;
    WORD wLen;
    pBuf = gPr645.pMessageAddr;

    for(i=0; i<6; i++)
    {
    	//ȷ�������Ƿ���ȷ
    	gPr645.pMessageAddr[1+i] = 0x88;
    }
	
	FillDlt645FrameData(wResult);
	
	//����������������ݺ� ��Ϊ��Ҫ�ж��Ƿ��к���֡
    if (wResult<0x8000)
    {
        gPr645.pMessageAddr[8] = (gPr645.byReceiveControl | 0x80);
    }
    else
    {
        gPr645.pMessageAddr[8] = (gPr645.byReceiveControl | 0xC0);
    }
	
	for(i=10;i<(10+pBuf[9]);i++)
    {
    	pBuf[i]=(pBuf[i]+0x33);
    }
    wLen = gPr645.pMessageAddr[9] + 6 + 4; //4������68 һ�������� һ������
    gPr645.pMessageAddr[0] = 0x68;
    gPr645.pMessageAddr[1+6] = 0x68;
    gPr645.pMessageAddr[wLen] = lib_CheckSum( gPr645.pMessageAddr, wLen );
    gPr645.pMessageAddr[wLen+1] = 0x16;
    
	SendDlt645( &ParaSerial );
}
//-----------------------------------------------
//��������: ��չ��Լ������
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						
//����ֵ:	���ض�ȡ���ݳ���
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��:����ʱ��04dfdfdb������չ4���ֽڣ�����ʱ����������չ8���ֽڣ���չ�ֽ����¼����չ��ʶ����
//	��1,2�ֽ�Ϊ������չ���ݱ�ʶ���ֱ��ӦDI0��DI1.
//	��3,4�ֽڹ̶�Ϊ0xdf��0xdb��
//	��5,6,7,8�ֽ�Ϊ��ǰʱ����00��1��1��0ʱ0�ֵ���Է�������hex�룬����ʱ���ں󣬵���ǰ��������ʱ�޴����ݣ���
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf)
{
    BYTE *p;
    WORD wReturnLen;
	char BuilTime[20] = {0};
	TRealTimer Time = {0};
	char ServerConnectFlag = 0;
	char *mac = NULL;
	char  buf[12] = {0};
	char  buf1[15] = {0};
	char buf2[100] = {0};
	BYTE i;
	BYTE csq;
	int addr;
	WORD tw,wLen;
	tw = pBuf[0];
	wLen = 0;
	if ((pBuf[2] != 0xdf) || (pBuf[3] != 0xdb))
	{
		IsBLETranData = 1;
		return 0;
	}
	if ((pBuf[1] != 0xFF) && (pBuf[1] != 0xF0) &&(pBuf[1] != 0xFD))	//����͸��ʱ FD ���ֻ����� ģ��������쳣�¼�
	{
		IsBLETranData = 1;
		return 0;
	}
	
    p = pBuf + 2;
    wReturnLen = 0;

	switch (pBuf[1])
	{
	case 0xFD://�쳣�¼���¼			
		if(tw == 0)
		{
			return 0;
		}
		
		for(i=1;i<=6;i++)
		{
			wLen = api_ReadSysUNMsg((tw-1)*6+i,p);
			p += wLen;
			wReturnLen += wLen;
		}
		
		wReturnLen += 2;
		break;
	case 0xff:
		if (pBuf[0] == 0x00)
		{
			api_GetSoftBuildTime(&Time);
			sprintf((char *)BuilTime, "%04d%02d%02d%02d%02d", Time.wYear, Time.Mon, Time.Day, Time.Hour, Time.Min);
			lib_ExchangeData(p, (BYTE *)&BuilTime[0], sizeof(BuilTime));
			wReturnLen = sizeof(BuilTime);
		}
		else if (pBuf[0] == 0x01)
		{
			lib_ExchangeData(p, imei.nImei, sizeof(imei.nImei));
			wReturnLen = sizeof(imei.nImei);
		}
		else if ( pBuf[0] == 0x02 )//����״̬
		{
			for (i = CycleReadMeterNum; i < (CycleReadMeterNum + POWERON_READRPARA_NUM); i++) //���ϵ�Ҫ������bit
			{
				if ((PowerOnReadMeterFlag & (1 << i)) != (1 << i))
				{
					p[0] = 0;
					return (wReturnLen += 3);
				}
			}
			p[0] = 0x0F;// ����qdlt
			wReturnLen = 1;
		}
		else if (pBuf[0] == 0x03)//����վ״̬
		{
			if (api_GetMqttConStatus(0) == TRUE)//ά����mqtt
			{
				ServerConnectFlag |= (BIT0);
			}
			else
			{
				ServerConnectFlag &= ~(BIT0);
			}
			if (api_GetTcpConStatus(1))
			{
				ServerConnectFlag |= (BIT1);
			}
			else
			{
				ServerConnectFlag &= ~(BIT1);
			}
			p[0] = ServerConnectFlag;
			wReturnLen = 1;
		}
		else if (pBuf[0] == 0x04)//��ǰ�豸������mac��ַ
		{
			mac = nwy_ble_get_mac_addr();

			lib_ExchangeData((BYTE *)&buf[0], (BYTE *)mac, 6);
			sprintf((char *)&buf1[0], "%02x%02x%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			nwy_ext_echo("(%s) \r\n", buf1);
			for (i = 0; i < 12; i++)
			{
				if ((buf1[i] > 96) && (buf1[i] < 123))
				{
					buf1[i] = (buf1[i] - 32);
				}
			}
			memcpy(p, (BYTE *)&buf1[0], 12);
			wReturnLen = 12;
		}
		#if (GPRS_POSITION == YES)
		else if (pBuf[0] == 0x06) //������λ��
		{
			if (api_CheckorSetPosition(0) == TRUE)
			{
				p[0] = 1;
			}
			else
			{
				p[0] = 0;
			}
			wReturnLen = 1;
		}
		#endif
		else if (pBuf[0] == 0x07) //���ײ� �汾
		{
			nwy_dm_get_inner_version(buf2, 100);
			nwy_ext_echo("\r\nble is %s", buf2);
			lib_ExchangeData(p, (BYTE *)&buf2[0], 100);
			wReturnLen = 100;
		}
		else if (pBuf[0] == 0x08) //��ȡ�ź�ǿ�� cqs
		{
			nwy_nw_get_signal_csq(&csq); // ����Ƶ��
			p[0] = csq;
			wReturnLen = 1;
		}
		else if (pBuf[0] == 0x09) //��ȡ��������״̬
		{
			p[0] = nwy_ext_check_data_connect();
			wReturnLen = 1;
		}
		//0A-0F��Ӧ����¼����װ
		else if (pBuf[0] == 0x10) //��ȡ����ʹ��  MB/��
		{
			memcpy(p,(BYTE*)&flow_per_day,sizeof(flow_per_day));
			wReturnLen = sizeof(flow_per_day);
		}
		if (wReturnLen)
		{
			wReturnLen += 2;
		}
		break;
	case 0xF0:
		if (pBuf[0] == 0x01)//�˿ں�
		{
			memcpy(p, (BYTE *)&Serverpara[0].ServerPort, sizeof(Serverpara[0].ServerPort));
			wReturnLen = 4;
		}
		else if (pBuf[0] == 0x02) //IP
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[0].ServerIP[0], sizeof(Serverpara[0].ServerIP));
			wReturnLen = sizeof(Serverpara[0].ServerIP) + 2;
		}
		else if (pBuf[0] == 0x03) //�˺�
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[0].ServerUserName[0], sizeof(Serverpara[0].ServerUserName));
			wReturnLen = sizeof(Serverpara[0].ServerUserName) + 2;
		}
		else if (pBuf[0] == 0x04) //����
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[0].ServerUserPwd[0], sizeof(Serverpara[0].ServerUserPwd));
			wReturnLen = sizeof(Serverpara[0].ServerUserPwd) + 2;
		}
		else if (pBuf[0] == 0x05) //�����ϱ�����
		{
			addr = GET_SAFE_SPACE_ADDR(ReportPara.cycleDataTopic);
			if (api_OperateFileSystem(READ, addr, (BYTE *)&ReportPara.cycleDataTopic[0], sizeof(ReportPara.cycleDataTopic)) == TRUE)
			{
				lib_ExchangeData(p, (BYTE *)&ReportPara.cycleDataTopic[0], sizeof(ReportPara.cycleDataTopic));
				wReturnLen = sizeof(ReportPara.cycleDataTopic) + 2;
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x07) //�����ϱ�Ƶ��
		{
			addr = GET_SAFE_SPACE_ADDR(ReportPara.reportfre);
			if (api_OperateFileSystem(READ, addr, p, sizeof(ReportPara.reportfre)) == TRUE)
			{
				wReturnLen = sizeof(ReportPara.reportfre) + 2;
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x0C) //mqtt client id
		{
			addr = GET_SAFE_SPACE_ADDR(ReportPara.mqttClientld);
			if (api_OperateFileSystem(READ, addr, (BYTE *)&ReportPara.mqttClientld[0], sizeof(ReportPara.mqttClientld)) == TRUE)
			{
				lib_ExchangeData(p, (BYTE *)&ReportPara.mqttClientld[0], sizeof(ReportPara.mqttClientld));
				wReturnLen = sizeof(ReportPara.mqttClientld) + 2;
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x0D) //�豸��ID �������Ļ�·id
		{
			addr = GET_SAFE_SPACE_ADDR(ReportPara.device[0][0]);
			if (api_OperateFileSystem(READ, addr, (BYTE *)&ReportPara.device[0][0], sizeof(ReportPara.device)) == TRUE)
			{
				memcpy(p, (BYTE *)&ReportPara.device[0][0], 50);
				wReturnLen = sizeof(ReportPara.device) + 2;
				// lib_ExchangeData(p,(BYTE*)&ReportPara.mqttClientld[0],sizeof(ReportPara.mqttClientld));
				// wReturnLen = sizeof(ReportPara.mqttClientld) + 2;
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x11)//�˿ں�
		{
			memcpy(p, (BYTE *)&Serverpara[1].ServerPort, sizeof(Serverpara[1].ServerPort));
			wReturnLen = 4;
		}
		else if (pBuf[0] == 0x12) //IP
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[1].ServerIP[0], sizeof(Serverpara[1].ServerIP));
			wReturnLen = sizeof(Serverpara[1].ServerIP) + 2;
		}
		else if (pBuf[0] == 0x13) //�˺�
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[1].ServerUserName[0], sizeof(Serverpara[1].ServerUserName));
			wReturnLen = sizeof(Serverpara[1].ServerUserName) + 2;
		}
		else if (pBuf[0] == 0x14) //����
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[1].ServerUserPwd[0], sizeof(Serverpara[1].ServerUserPwd));
			wReturnLen = sizeof(Serverpara[1].ServerUserPwd) + 2;
		}
		break;
	default:
		break;
	}
	return wReturnLen;
}
//-----------------------------------------------
//��������: ��չ��Լд����
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--��Է�����
//����ֵ:	����ֵbit15Ϊ1Ϊ��������Ϊ��ȷ
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf)
{
	BYTE bTemp;
	WORD wReturnLen;
	int addr;
	wReturnLen = 0;
	#if(CYCLE_METER_READING == PROTOCOL_645)
	BYTE i,j;
	#endif
	
	if( (pBuf[2]!=0xdf) || (pBuf[3]!=0xdb) )
	{
		IsBLETranData = 1;
		return 0;
	}
	if ((pBuf[1]!=0xFF) && (pBuf[1]!=0xF0) )
	{
		IsBLETranData = 1;
		return 0;
	}
	switch (pBuf[1])
	{
		case 0xFF:
			if (pBuf[0] == 0xAA)
			{
				if (pBuf[8] == 1)
				{
					PrintLogSign = 0;
				}
				else if (pBuf[8] == 0)
				{
					PrintLogSign = 0xAA;
				}
			}
			else if (pBuf[0] == 0x05)//�ر����� 
			{
				if (pBuf[8] == 1)
				{
					nwy_ble_disable();
				}
			}
			else if (pBuf[0] == 0x0A)//��ӡ������ӡ���
			{
				if (pBuf[8] == 1)
				{
					BleLogSwitch = 0xAA;
				}	
				else if (pBuf[8] == 0)
				{
					BleLogSwitch = 0;
				}
				else
				{
					return 0;
				}
			}
			#if(CYCLE_METER_READING == PROTOCOL_645)
			if (pBuf[0] == 0xFF)
			{
				for (i = 0; i < bUsedChannelNum; i++)
				{
					for (j = 0; j < CycleReadMeterNum; j++)
					{
					qwReadMeterFlag[i] |= (1 << j);
					}
				}
				wReturnLen = 1;
			}
			#endif	
			
			wReturnLen = 1;
			break;
		case 0xF0:
			if (pBuf[0] == 0x01)//�˿ں�			//tcp ����
			{		
				memcpy((BYTE*)&Serverpara[0].ServerPort,&pBuf[8],sizeof(Serverpara[0].ServerPort));
			}
			else if (pBuf[0] == 0x02)//IP
			{
				lib_ExchangeData((BYTE*)&Serverpara[0].ServerIP[0],&pBuf[8],sizeof(Serverpara[0].ServerIP));
			}
			else if(pBuf[0] == 0x03)//�û���
			{
				lib_ExchangeData((BYTE*)&Serverpara[0].ServerUserName[0],&pBuf[8],sizeof(Serverpara[0].ServerUserName));
			}
			else if(pBuf[0] == 0x04)//����
			{
				lib_ExchangeData((BYTE*)&Serverpara[0].ServerUserPwd[0],&pBuf[8],sizeof(Serverpara[0].ServerUserPwd));
			}
			else if(pBuf[0] == 0x05)//�����ϱ�����
			{
				lib_ExchangeData((BYTE*)&ReportPara.cycleDataTopic,&pBuf[8],sizeof(ReportPara.cycleDataTopic));
				addr = GET_SAFE_SPACE_ADDR(ReportPara.cycleDataTopic);
				if(api_OperateFileSystem(WRITE,addr,(BYTE*)&ReportPara.cycleDataTopic[0],sizeof(ReportPara.cycleDataTopic)) == TRUE)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (pBuf[0] == 0x07)//�����ϱ�Ƶ��
			{		
				addr = GET_SAFE_SPACE_ADDR(ReportPara.reportfre);
				if((pBuf[8]>0)&&(pBuf[8]<=60))
				{
					if(api_OperateFileSystem(WRITE,addr,&pBuf[8],sizeof(ReportPara.reportfre)) == TRUE)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else if(pBuf[0] == 0x0C)//�����Դmqtt_client_id
			{
				lib_ExchangeData((BYTE*)&ReportPara.mqttClientld,&pBuf[8],sizeof(ReportPara.mqttClientld));
				addr = GET_SAFE_SPACE_ADDR(ReportPara.mqttClientld[0]);
				if(api_OperateFileSystem(WRITE,addr,(BYTE*)&ReportPara.mqttClientld[0],sizeof(ReportPara.mqttClientld)) == TRUE)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (pBuf[0] == 0x0D)//�����Դ �ϱ�ID
			{
				for (BYTE i = 0; i < 5; i++)
				{
					memcpy((BYTE*)&ReportPara.device[i][0],(BYTE*)&pBuf[8+10*i],10);
				}
				addr = GET_SAFE_SPACE_ADDR(ReportPara.device[0][0]);
				if(api_OperateFileSystem(WRITE,addr,(BYTE*)&ReportPara.device[0][0],sizeof(ReportPara.device)) == TRUE)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (pBuf[0] == 0x11)//�˿ں�			//tcp ����
			{		
				memcpy((BYTE*)&Serverpara[1].ServerPort,&pBuf[8],sizeof(Serverpara[1].ServerPort));
			}
			else if (pBuf[0] == 0x12)//IP
			{
				lib_ExchangeData((BYTE*)&Serverpara[1].ServerIP[0],&pBuf[8],sizeof(Serverpara[1].ServerIP));
			}
			else if(pBuf[0] == 0x13)//�û���
			{
				lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserName[0],&pBuf[8],sizeof(Serverpara[1].ServerUserName));
			}
			else if(pBuf[0] == 0x14)//����
			{
				lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserPwd[0],&pBuf[8],sizeof(Serverpara[0].ServerUserPwd));
			}
			else if (pBuf[0] == 0xFF)// ��ʼ��ģ�����
			{
				#if(ONLINE_PARA_SECLET == FROM_MODULE)
				if(api_DeleteSysParaFile())
				{
					api_PowerOnCreatParaTable();
					return 1;
				}
				else
				{
					return 0;
				}
				#endif
			}
			else if (pBuf[0] == 0xAA)//��λ����ȡ�� ����д�ļ�����
			{
				if (pBuf[8] == 1)
				{
					nwy_power_off(2);
				}
			}
			if ( ((pBuf[0]>=0x01)&&(pBuf[0]<=0x04))
			||((pBuf[0]>=0x11)&&(pBuf[0]<=0x14)) )
			{
				bTemp = (pBuf[0]>>4);
				Serverpara[bTemp].wCrc = CalCRC16((BYTE *)&Serverpara[bTemp].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[bTemp].wCrc)));
				addr = GET_SAFE_SPACE_ADDR(Serverpara[bTemp].ServerPort);
				if(api_OperateFileSystem(WRITE,addr,(BYTE*)&Serverpara[bTemp].ServerPort,sizeof(ServerParaRam_t)) == TRUE)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			
			wReturnLen = 1;
			break;
		case 0xF1:
			break;
		default:
			break;
	}
	return wReturnLen;
}
//--------------------------------------------------
//��������:  645Ӧ�ò㺯��
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - ���ݳ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD DLT645APPPro(void)
{
	WORD wResult = 0; 
	
    switch( gPr645.byReceiveControl )
	{
		case 0x11:			
			wResult = ReadFactoryExtPro( gPr645.pMessageAddr + 14 );
			if( wResult != 0 )
			{
				memcpy( DataBuf, gPr645.pMessageAddr + 14, wResult );
			}
			else
			{
				wResult = 0x8001;
			}
			break;
		case 0x14:
			wResult = WriteFactoryExtPro( gPr645.pMessageAddr + 22 );
			if( wResult == 0 )
			{
				wResult = 0x8001;
			}
			else if( wResult > 0x8000 )
			{
				return wResult;//�õ�ʧ��ԭ��
			}
			else
			{
				DataBuf[0] = 0x00;
				wResult = 1;
			}
			break;
		default:
			break;
	}
	return wResult;		
}

//--------------------------------------------------
//��������:  ��Ӧ���Ĵ���
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
void Proc645LinkDataRequest( BYTE *pBuf )
{
    WORD i;

	for(i=10;i<(10+pBuf[9]);i++)
	{
		pBuf[i]=(pBuf[i]-0x33);
	}
	
    gPr645.pMessageAddr = pBuf;
	
	gPr645.wSerialDataLen=pBuf[3];//�������ݳ��� ������serail�ṹ��ָ�����ȡ�����������ݳ���!!!!!
	
	gPr645.byReceiveControl=pBuf[8];//���ձ��Ŀ�����
	
	// gPr645.eProtocolType = ePROTOCOL_DLT645_FACTORY;
	
    switch( gPr645.byReceiveControl )
	{
		case 0x11://������
			// memcpy( gPr645.dwDataID.b, &pBuf[10], 4 );//��ȡ���ݱ�ʶ
			gPr645.pDataAddr = &pBuf[14];//�������ַ
			gPr645.wDataLen=(pBuf[9]-4);//����������
			break;
		case 0x14://д����
			// memcpy( gPr645.dwDataID.b, &pBuf[10], 4 );//��ȡ���ݱ�ʶ 
			gPr645.bLevel = pBuf[14];//����ȼ�
			gPr645.pDataAddr = &pBuf[22];//�������ַ
			gPr645.wDataLen=(pBuf[9]-4-1-3-4);//����������
			break;	
        default:
            break;
    }
}
//--------------------------------------------------
//��������:  645��Լ����������
//         
//����:      TYPE_PORT PortType[in]
//         
//           BYTE *pBuf[in]
//         
//����ֵ:    WORD
//         
//��ע����:  ��
//--------------------------------------------------
void ProMessage_Dlt645( TSerial *p )
{
	WORD wResult = 0;
	WORD i = 0;

	memset(&FactoryTranData.buf, 0, sizeof(FactoryTranData.buf));
    Proc645LinkDataRequest(ParaSerial.ProBuf);    //��·�����ݴ���
    wResult = DLT645APPPro();                      //Ӧ�ò����ݽ���
	if (IsBLETranData == 0)
	{	
    	Proc645LinkDataResponse( wResult );          //��·��������֡
	}
	else //���ݱ�ʶ��ģ�鲻ƥ�䣬͸������
	{
		IsBLETranData = 0;
		nwy_ext_echo("\r\nFrameLength %d", p->ProStepDlt645);

		for (i = 10; i < (10 + gPr645.pMessageAddr[9]); i++)
		{
			gPr645.pMessageAddr[i] = (gPr645.pMessageAddr[i] + 0x33);
		}

		// nwy_ext_echo("\r\n ble tran data:");

		memcpy(FactoryTranData.buf, gPr645.pMessageAddr, p->ProStepDlt645);
		FactoryTranData.len = p->ProStepDlt645;
		FactoryTranData.TranType = eTRAN_BLETOOTH;

		if (nwy_put_msg_que(TranDataToUartMessageQueue, &FactoryTranData, 0xffffffff) == TRUE)
		{
			nwy_ext_echo("\r\nput blueTran data to uart queue success");
		}
		else
		{
			nwy_ext_echo("\r\nput blueTran data to uart queue fail");
		}
		// for (i = 0; i < FrameLength; i++)
		// {
		// 		nwy_ext_echo(" %02x ", gPr645.pMessageAddr[i]);
		// }
	}
}
//---------------------------------------------------------------
//��������: 645��ʽ�ж�
//
//����: 	portType[in] - ͨѶ�˿�
//                   
//����ֵ:  FALSE: ����û������
//		  TRUE : ������������InitPoint()
//
//��ע:   
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_WH( BYTE ReceByte, TSerial * p )
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

				//ȡ�����ֽ�
				if( Dlt645Head.Control >= 0x80 )//����жϿ����벻�ԣ����2��68��ʼ���¼����ձ���
				{
					p->BeginPosDlt645 = p->BeginPosDlt645+7;
					p->ProStepDlt645 -= 7;
					return FALSE;
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

			//����У��
			//p->DltCheckSum += ReceByte;

			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		//��������
		else if( p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2 ) )
		{
			//��������
			//p->ProBuf[ p->ProStepDlt645 ] = ReceByte;

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
				//У��ͨ�������Դ�������

				//��ֹ����
				//api_DisableRece( p );
				//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ���� 
				api_DoReceMoveData(p,PROTOCOL_645);

 				ProMessage_Dlt645( p );

				//�������
				InitPoint(p);

				return TRUE;
			}
			else
			{
				//����У��
			//	p->DltCheckSum += ReceByte;
			}

			//������ն�ʱ
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		else
		{
			p->ProStepDlt645 = 0;//InitPoint(p);
		}
	}
	return FALSE;
}
//---------------------------------------------------------------
//��������: ͸��698��ʽ�ж�
//
//����: 	ReceByte�������ֽ� 	
//
//����ֵ:  FALSE: ����û������
//		  TRUE : ������������InitPoint()
//
//��ע:
//---------------------------------------------------------------
WORD DoReceProc_Dlt698_WH(BYTE ReceByte, TSerial *p)            //698ά�����жϿ���ɾ��������Deal_BLETran698
{
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
				// DoSearch_68_DLT698(p);

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
				if (p->Addr_Len != 6) // ����ַ��6�ֽ�BCD����12λBCD��
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}
			}
		}
		else if (p->ProStepDlt698_45 == (8 + p->Addr_Len)) // ��ַ�����ˣ�HCS�����ˣ��ж�HCS�Ƿ���ȷ
		{
			if (p->Addr_Len > 6)
			{
				DoSearch_68_DLT698(p);

				return FALSE;
			}
			// //һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
			if (JudgeDlt698_45_HCS_FCS(0, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			// for (BYTE i = 0; i < p->Addr_Len; i++)���жϵ�ַ
			// {
			// 	if (reverAddr[i] != p->ProBuf[p->BeginPosDlt698_45 + 5 + i])
			// 	{
			// 		// DoSearch_68_DLT698(p);
			// 		nwy_ext_echo("\r\n addr is error \r\n");
			// 		return FALSE;
			// 	}
			// }
			// nwy_ext_echo("\r\n addr is correct \r\n");
			// //�ж�ͨ�ŵ�ַ�Ƿ���ȷ
			// if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			// {

			// }
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
				Deal_BLETran698(p);
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
//--------------------------------------------------
//��������:  ά���ڹ�Լ���մ���
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void api_ReceData_WHTask( void )
{
	BYTE Result = FALSE;
	WORD ReceNum = 0;

	ReceNum = ParaSerial.RXWPoint;
	while (ReceNum != ParaSerial.RXRPoint)
	{
		nwy_start_timer(FactoryRecv_ByteTimeout_timer, 500);
		Result = DoReceProc_Dlt645_WH( ParaSerial.ProBuf[ParaSerial.RXRPoint], &ParaSerial );
		if (Result == FALSE)
		{
			Result = DoReceProc_Dlt698_WH(ParaSerial.ProBuf[ParaSerial.RXRPoint], &ParaSerial);
		}
		if( Result == FALSE )
		{
			ParaSerial.RXRPoint++;
			if( ParaSerial.RXRPoint >= UART_BUFF_MAX_LEN )
			{
				ParaSerial.RXRPoint = 0;
			}
		}
		else//���ܵ���������
		{
			nwy_stop_timer(FactoryRecv_ByteTimeout_timer);
			break;
		}
	}
	if(FactoryRecvByteTimeoutFlag == 1)
	{
		FactoryRecvByteTimeoutFlag = 0;
		InitPoint(&ParaSerial);
	}
}
#if(ONLINE_PARA_SECLET == FROM_MODULE)
//--------------------------------------------------
//��������:  ɾ��ģ��para.txt ���� ��Ϊģ�������ʼ��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_DeleteSysParaFile( void )
{
	if (nwy_sdk_fexist(filepara_name) == TRUE)
	{
		nwy_ext_echo("\r\n exit fle");
		if(nwy_sdk_file_unlink(filepara_name) == NWY_SUCESS)
		{
			nwy_ext_echo("\r\n dele secc");
			return TRUE;
		}
		else
		{
			nwy_ext_echo("\r\ndelte fail is%d",nwy_sdk_file_unlink(filepara_name));
			return FALSE;
		}
	}		
	else
	{
		return FALSE;
	}
}
#endif
//--------------------------------------------------
//��������:  �����ļ�ϵͳ
//         
//����:      Operation 1д���� 0 ������
//           addr Ҫ�������ļ���ַ
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_OperateFileSystem( BYTE Operation,int addr,BYTE*Buf,WORD bLen)//�˺�������û����
{
	int fd;

	fd = nwy_sdk_fopen(filepara_name, NWY_RDWR);
	if (fd >= 0)
	{
		if(nwy_sdk_fseek(fd,addr,NWY_SEEK_SET) == addr)
		{
			if (Operation == WRITE)//д����
			{
				if(nwy_sdk_fwrite(fd,Buf,bLen) == bLen )
				{
					nwy_sdk_fclose(fd);
					//nwy_ext_echo("\r\n write file success!!!");
					return TRUE;
				}
				else
				{
					//nwy_ext_echo("\r\n write file fail!!!");
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
			else
			{
				if (nwy_sdk_fread(fd,Buf,bLen) == bLen)
				{
					nwy_sdk_fclose(fd);
					//nwy_ext_echo("\r\n read file success!!!");
					return TRUE;
				}
				else
				{
					//nwy_ext_echo("\r\n read file fail!!!");
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
		}
		else
		{
			//nwy_ext_echo("\r\n fseek file fail!!!");
			nwy_sdk_fclose(fd);
			return FALSE;
		}
	}
	else
	{
		//nwy_ext_echo("\r\n open/creat file fail!!!");
		return FALSE;
	}
}
#if(ONLINE_PARA_SECLET == FROM_MODULE)
//--------------------------------------------------
//��������:  �ϵ紴�����ʱ��para�ļ�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_PowerOnCreatParaTable( void )
{
    int fd,addr;

	fd = nwy_sdk_fopen(filepara_name, NWY_RDWR);
	if(fd < 0)
	{
		fd = nwy_sdk_fopen(filepara_name, NWY_CREAT | NWY_RDWR);
    	if (fd >= 0)
    	{
			api_OperateFileSystem(WRITE,0,(BYTE*)&ServerparaConst[0].ServerPort,(sizeof(TSafeMem))); 
    	}
	}
	else
	{
		api_OperateFileSystem(READ,0,(BYTE*)&Serverpara[0].ServerPort,(sizeof(ServerParaRam_t)*2)); 
		addr = GET_SAFE_SPACE_ADDR(ReportPara.reportfre);
		api_OperateFileSystem(READ,addr,(BYTE*)&ReportPara.reportfre,(sizeof(ReportPara_txt)));
		IntervalTime = ReportPara.reportfre;
		nwy_sdk_fclose(fd);
	}
}
#endif
//--------------------------------------------------
//��������: 
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_ReadSystemFiletoRam( eREAD_METER_BIT eBit )
{
	int fd;
	ServerParaRam_t	TempServerpara[2];
	WORD crc16_1 = 0;
	WORD crc16_2 = 0;

	memset((BYTE*)&TempServerpara[0].ServerPort,0,sizeof(ServerParaRam_t)*2);
	fd = nwy_sdk_fopen(filepara_name, NWY_RDONLY );
	if (fd >=0)
	{
		if(nwy_sdk_fread(fd,(char*)&TempServerpara[0].ServerPort,2*sizeof(ServerParaRam_t)) ==(2*sizeof(ServerParaRam_t)))
		{
			crc16_1 = CalCRC16((BYTE *)&TempServerpara[0].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[0].wCrc)));
			crc16_2 = CalCRC16((BYTE *)&TempServerpara[1].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[1].wCrc)));
			nwy_ext_echo("\r\n2 calculated serverpara[0].wCrc is %04x", crc16_1);
			nwy_ext_echo("\r\n2 calculated Serverpara[1].wCrc is %04x", crc16_2);

			if (crc16_1 == (TempServerpara[0].wCrc))
			{
				if (eBit == eBIT_TCPNET)
				{
					memcpy((BYTE*)&Serverpara[0].ServerPort,(BYTE*)&TempServerpara[0].ServerPort,(sizeof(TempServerpara[0].ServerPort)+sizeof(TempServerpara[0].ServerIP)));
					nwy_ext_factory("\r\n%d",TempServerpara[0].ServerPort);
					nwy_ext_factory("\r\n%s",TempServerpara[0].ServerIP);
				}
				else if (eBit == eBIT_TCPUSER)
				{
					memcpy((BYTE*)&Serverpara[0].ServerUserName[0],(BYTE*)&TempServerpara[0].ServerUserName[0],(sizeof(TempServerpara[0].ServerUserName)+sizeof(TempServerpara[0].ServerUserPwd)));
					nwy_ext_factory("\r\n%s",TempServerpara[0].ServerUserName);
					nwy_ext_factory("\r\n%s",TempServerpara[0].ServerUserPwd);
				}	
			}
			else
			{
				if (eBit == eBIT_TCPNET)
				{
					memcpy((BYTE *)&Serverpara[0].ServerPort, (BYTE *)&ServerparaConst[0].ServerPort, (sizeof(TempServerpara[0].ServerPort) + sizeof(TempServerpara[0].ServerIP)));
				}
				else if (eBit == eBIT_TCPUSER)
				{
					memcpy((BYTE *)&Serverpara[0].ServerUserName[0], (BYTE *)&ServerparaConst[0].ServerUserName[0], (sizeof(TempServerpara[0].ServerUserName) + sizeof(TempServerpara[0].ServerUserPwd)));
				}

			}
			if (crc16_2 == (TempServerpara[1].wCrc))
			{
				if (eBit == eBIT_MQTTNET)
				{
					memcpy((BYTE*)&Serverpara[1].ServerPort,(BYTE*)&TempServerpara[1].ServerPort,(sizeof(TempServerpara[1].ServerPort)+sizeof(TempServerpara[1].ServerIP)));
					nwy_ext_factory("\r\n%d",TempServerpara[1].ServerPort);
					nwy_ext_factory("\r\n%s",TempServerpara[1].ServerIP);
				}
				else if (eBit == eBIT_MQTTUSER)
				{			
					memcpy((BYTE*)&Serverpara[1].ServerUserName[0],(BYTE*)&TempServerpara[1].ServerUserName[0],(sizeof(TempServerpara[1].ServerUserName)+sizeof(TempServerpara[1].ServerUserPwd)));
					nwy_ext_factory("\r\n%s",TempServerpara[1].ServerUserName);
					nwy_ext_factory("\r\n%s",TempServerpara[1].ServerUserPwd);
				}
			}	
			else
			{
				if (eBit == eBIT_MQTTNET)
				{
					memcpy((BYTE *)&Serverpara[1].ServerPort, (BYTE *)&ServerparaConst[1].ServerPort, (sizeof(TempServerpara[1].ServerPort) + sizeof(TempServerpara[1].ServerIP)));
				}
				else if (eBit == eBIT_MQTTUSER)
				{
					memcpy((BYTE *)&Serverpara[1].ServerUserName[0], (BYTE *)&ServerparaConst[1].ServerUserName[0], (sizeof(TempServerpara[1].ServerUserName) + sizeof(TempServerpara[1].ServerUserPwd)));
				}
			}
		}
		else//��ȡʧ�� ��û���ļ� ҲҪ��const
		{
			if (eBit == eBIT_TCPNET)
			{
				memcpy((BYTE *)&Serverpara[0].ServerPort, (BYTE *)&ServerparaConst[0].ServerPort, (sizeof(TempServerpara[0].ServerPort) + sizeof(TempServerpara[0].ServerIP)));
			}
			else if (eBit == eBIT_TCPUSER)
			{
				memcpy((BYTE *)&Serverpara[0].ServerUserName[0], (BYTE *)&ServerparaConst[0].ServerUserName[0], (sizeof(TempServerpara[0].ServerUserName) + sizeof(TempServerpara[0].ServerUserPwd)));
			}
			else if (eBit == eBIT_MQTTNET)
			{
				memcpy((BYTE *)&Serverpara[1].ServerPort, (BYTE *)&ServerparaConst[1].ServerPort, (sizeof(TempServerpara[1].ServerPort) + sizeof(TempServerpara[1].ServerIP)));
			}
			else if (eBit == eBIT_MQTTUSER)
			{
				memcpy((BYTE *)&Serverpara[1].ServerUserName[0], (BYTE *)&ServerparaConst[1].ServerUserName[0], (sizeof(TempServerpara[1].ServerUserName) + sizeof(TempServerpara[1].ServerUserPwd)));
			}
		}
		nwy_sdk_fclose(fd);
	}
}

#if( BLE_WH==  YES)
void nwy_ble_conn_status_func()
{
	int conn_status = 0;

	conn_status = nwy_ble_get_conn_status();
	if(conn_status != 0)
	{
		nwy_ext_echo("\r\n Ble connect,status:%d", conn_status);
	}
	else
	{
		nwy_ext_echo("\r\n Ble disconnect,status:%d", conn_status);
	}
	
	return;
}
 void nwy_ble_recv_data_func ()
{
	DWORD length = 0;
	BYTE *precv = NULL;
	WORD wLen = 0, wLen1 = 0;
	// WORD i = 0;
	nwy_osiEvent_t event;

	memset(&event, 0, sizeof(event));

	length = (DWORD)nwy_ble_receive_data(0);
	precv = (BYTE*)nwy_ble_receive_data(1);
	if ((NULL != precv) && (0 != length))
	{
		nwy_ext_echo("\r\n nwy_bel_receive_data:%d\r\n", length);
		// for (i = 0; i < length; i++)
		// {
		// 	nwy_ext_echo(" 0x%02x ", precv[i]);
		// }
	}
	else
	{
		nwy_ext_echo("\r\nnwy_ble receive data is null.");
	}
	wLen = ParaSerial.RXWPoint + length;
	if (length >= UART_BUFF_MAX_LEN)
	{
		length = UART_BUFF_MAX_LEN;
	}
	if (wLen < UART_BUFF_MAX_LEN)
	{
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], precv, length);
		ParaSerial.RXWPoint += length;
	}
	else
	{
		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], precv, wLen1);
		memcpy((char *)&ParaSerial.ProBuf[0], precv + wLen1, length - wLen1);
		ParaSerial.RXWPoint = length - wLen1;
	}

	nwy_ble_receive_data(2);
	WhFlag |= BIT0;
	return;

}

//--------------------------------------------------
//��������:  ��ͨ������ �û����ò�����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  BuleToothWH( void )
{
	char dev_name[30] = "wsd";
	char *ble_name = NULL;
	char bleflag = 0;
	char bFailcount = 0;

	strcat(dev_name,(char*)&LtuAddr[5]);
	nwy_ble_set_device_name(dev_name);
	nwy_ext_echo("nwy_ble_enable(%d) \r\n", nwy_ble_enable());
	nwy_sleep(3000);
	// nwy_ext_echo("nwy_ble_set_adv(%d) \r\n", nwy_ble_set_adv(1));
	while (bleflag == 0)
	{
		bleflag = nwy_ble_set_adv(1);
		if (bleflag == 1)
		{
			nwy_ext_echo("BLE broadcast \r\n");
			break;
		}
		else 
		{
			bFailcount++;
			nwy_sleep(200);
		}
		if (bFailcount > 10)
		{
			break;
		}
	}
	ble_name = nwy_ble_get_device_name();
	nwy_ext_echo("\n new_set_dev_name:(%s) \r\n", ble_name);
	nwy_ble_register_callback(nwy_ble_recv_data_func);
	nwy_ble_conn_status_cb(nwy_ble_conn_status_func);
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
BYTE SendBleDataInPiece(char *data, WORD total_len)
{
	WORD BytesSent = 0; // �ѷ��͵��ֽ���
	WORD ret = 0;		 // �洢ÿ�η��͵ķ���ֵ
	WORD ChunkSize = 0;

	while (BytesSent < total_len)
	{
		// ����ʣ�������Ƿ񳬹� MAX_BLE_PACKET_SIZE
		if ((total_len - BytesSent) > MAX_BLE_PACKET_SIZE)
		{
			ChunkSize = MAX_BLE_PACKET_SIZE;
		}
		else
		{
			ChunkSize = (total_len - BytesSent);
		}
		// ���͵�ǰ������
		ret = nwy_ble_send_data(ChunkSize, data + BytesSent);
		nwy_sleep(100);
		if (ret != 1)
		{
			nwy_ext_echo(" \r\nsending ble data Error, ret = %d", ret);
			return FALSE;
		}
		// �����ѷ��͵��ֽ���
		BytesSent += ChunkSize;
	}
	return TRUE; // ȫ�����ͳɹ�
}
#endif

#if(UART_WH == YES)
static void nwy_uart_wh_handle(const char *str, uint32_t length)
{
	WORD wLen,wLen1;

	wLen = ParaSerial.RXWPoint + length;
	if (length >= UART_BUFF_MAX_LEN)
	{
		length = UART_BUFF_MAX_LEN;
	}	
	if (wLen < UART_BUFF_MAX_LEN)
	{
		memcpy((char*)&ParaSerial.ProBuf[ParaSerial.RXWPoint],str,length);
		ParaSerial.RXWPoint += length;
	}
	else
	{
		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
		memcpy((char*)&ParaSerial.ProBuf[ParaSerial.RXWPoint],str,wLen1);
		memcpy((char*)&ParaSerial.ProBuf[0],str+wLen1,length-wLen1);
		ParaSerial.RXWPoint = length-wLen1;
	}
	WhFlag |= (BIT1);
}

void  InitUartWH( void )
{
	UartWh = nwy_uart_init(NWY_NAME_UART2, 1);   // ��ʼ��
	nwy_uart_set_baud(UartWh, 9600);   // ����Ĭ�ϲ�����
	nwy_uart_set_para(UartWh, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
	nwy_uart_reg_recv_cb(UartWh, nwy_uart_wh_handle);
}
#endif
//--------------------------------------------------
//��������:  ��������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void CalculateFlow(void)
{
	static WORD i = 0;
	i++;

	QWORD total_bytes = 0;
	nwy_data_flowcalc_info_t flowcalc_info = {0};
	nwy_time_t tTimer;
	// ����ʱ���(��)
	int total_seconds = 0;
	float flow_per_second = 0;
	BYTE timezone = 8;

	if (i >= FLOW_REFRESH_FRE)
	{
		i = 0;
		nwy_data_get_flowcalc_info(&flowcalc_info);
		total_bytes = flowcalc_info.tx_bytes + flowcalc_info.rx_bytes;
		nwy_get_time(&tTimer, (char*)&timezone);
		// ת��Ϊ������
		total_seconds = (tTimer.hour - poweronTimer.hour) * 3600 +
						(tTimer.min - poweronTimer.min) * 60 +
						(tTimer.sec - poweronTimer.sec);
		if (total_seconds > 0)
		{
			// ����ÿ������(�ֽ�/��)
			flow_per_second = (float)total_bytes / total_seconds;
			// ����ÿ������(MB/��)
			flow_per_day = flow_per_second * 86400 / (1024 * 1024);
			nwy_ext_echo("\r\ntotal_bytes: %llu bytes", total_bytes);
			nwy_ext_echo("\r\nflow per second: %.2f bytes/s", flow_per_second);
			nwy_ext_echo("\r\nflow per one days: %.2f MB/day", flow_per_day);
		}
	}
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
void  Factory_Task( void *param )
{
	#if( BLE_WH== YES)
	WORD i = 0;
	BYTE ret = 0;
	#endif
	nwy_osiEvent_t event;
	InitPoint(&ParaSerial);
	memset((char*)&ParaSerial.ProBuf[0],0,UART_BUFF_MAX_LEN);
	nwy_usb_serial_reg_recv_cb((nwy_sio_recv_cb_t)nwy_ext_sio_data_cb);
	
	#if(ONLINE_PARA_SECLET == FROM_MODULE)
	api_PowerOnCreatParaTable();
	#endif
	nwy_sleep(10000);
	#if(UART_WH == YES)
	InitUartWH();
	#endif
	#if (BLE_WH== YES)
	BuleToothWH();
	#endif

	while (1)
	{
		api_ReceData_WHTask();
		#if( BLE_WH== YES)
		if (nwy_get_msg_que(TranDataToFactoryMessageQueue, &FactoryTranData, 0xffffffff) == TRUE)
		{
			nwy_ext_echo("\r\nble send data leng :%d", FactoryTranData.len);
			ret = SendBleDataInPiece((char *)FactoryTranData.buf, FactoryTranData.len);
			nwy_ext_echo("\r\nret %d ", ret);
			nwy_ext_echo("\r\nuart to factory data: ");
			for (i = 0; i < FactoryTranData.len; i++)
			{
				nwy_ext_echo(" %02X ", FactoryTranData.buf[i]);
			}
		}
		else
		{
		}
		#endif
		#if (TCP_COMM_MODULE == YES)
		if (nwy_get_msg_que(TcpDataToModuleMsgQue, &FactoryTranData, 0xffffffff))
		{
			memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], FactoryTranData.buf, FactoryTranData.len);
			ParaSerial.RXWPoint += FactoryTranData.len;
			WhFlag |= BIT2;
		}
		#endif
		CalculateFlow();
		nwy_wait_thead_event(factorypara_thread, &event, 50);//����ɾ�� �ȴ��¼�
	}
}
