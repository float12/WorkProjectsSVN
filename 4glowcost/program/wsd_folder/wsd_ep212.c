//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_212)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define EP_DATA_LEN_OFFSET	2		//���ݳ���ƫ��
#define EP_CHECK_CRC_OFFSET 6		//����У�����ݶ�ƫ��
#define EP_HEAD_TAIL_LEN	(10+2)	//212Э�� ͷβ�ܳ���   ������ĩλ���� \r \n ������ ������ +2
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TEpReserve_T
{
	BYTE Channel;
	BYTE Reserve;
	WORD wCrc;
}TEpReserve;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  212Э�� ���� ����У��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
unsigned int CRC16_Checkout(BYTE *puchMsg,unsigned int usDataLen)
{
	unsigned int i,j,crc_reg,check;
	crc_reg = 0xFFFF;

	for( i=0; i < usDataLen; i++)
	{
		crc_reg=(crc_reg>>8)^puchMsg[i];
		for(j=0;j<8;j++)
		{
			check=crc_reg&0x0001;
			crc_reg>>=1;
			if(check==0x0001)
			{
				crc_reg^=0xA001;
			}
		}	
	}								
	return crc_reg;	
}
//--------------------------------------------------
//��������:  ����212���� ��������Ӧtcp ������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  DealUartMessage_Ep212( TSerial *p )
{
	BYTE bChannel,bReserve,bFlowFrame;
	tTranData tempTrandata; //��ȷ����ջ�ռ��Ƿ񹻣�����
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};

	memset((BYTE*)&tempTrandata,0,sizeof(tempTrandata));
	nwy_ext_echo("\r\n recv success 212"); 
	//ȡ��ͨ��ID
	bReserve = p->ProBuf[p->BeginPos212 + p->Hb212DataLen + EP_HEAD_TAIL_LEN];
	bChannel = api_GetTcpChannelTable(eMODULE, bReserve&0x0f);
	bFlowFrame = bReserve&0xF0;
	memcpy(tempTrandata.buf,&p->ProBuf[p->BeginPos212],p->Hb212DataLen+EP_HEAD_TAIL_LEN);
	tempTrandata.len = p->Hb212DataLen + EP_HEAD_TAIL_LEN;
	tempTrandata.threadid = bChannel + 1;//��������Ҫ�û�������ID
	//���� ��������ͨ�� ���·���ͨ���ƽ�һ�£�
	if (QueueWrite(&MessageQueueInfoArr[eQUEUE_UART_TO_EP212], &tempTrandata, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
	{
		nwy_ext_echo("\r\n nwy_put_msg_que UartToEp212MsgQue success ");
	}
	//�ж��Ƿ��к���֡
	if (bFlowFrame)
	{
		api_SetSysStatus(eSYS_STASUS_START_COLLECT);
		nwy_sdk_timer_stop(uart_timer);
		OneTimeTImerPara.expired_time = 5000;
		nwy_sdk_timer_start(uart_timer, &OneTimeTImerPara); //������ϱ��ٶ� ���е���
	}
	else
	{
		api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
		nwy_sdk_timer_stop(uart_timer);
	}
}
//--------------------------------------------------
//��������:  uart ͨ��212��ʽ�ж�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  DoReceProc_EP212_UART(BYTE ReceByte, TSerial *p)
{
	WORD wCrc,wCrc1;
	char buf[10] = {0};

	if (p->ProStep212 == 0)
	{
		if (ReceByte == '#')
		{
			p->BeginPos212 = p->RXRPoint;
			p->ProStep212++;
		}
		else
		{
			return FALSE;
		}
	}
	else if (p->ProStep212 == 1)//���� ��Լ ���ж�crc
	{
		if ((p->ProBuf[p->BeginPos212] == '#')&&(ReceByte == '#'))
		{
			p->ProStep212++;
			memcpy(buf,&p->ProBuf[p->BeginPos212+EP_DATA_LEN_OFFSET],4);
			p->Hb212DataLen = atol(buf);
			nwy_ext_echo("\r\ndata len is %d\n",p->Hb212DataLen );
		}
		else
		{
			return FALSE;
		}
	}
	else if (p->ProBuf[p->BeginPos212+EP_CHECK_CRC_OFFSET] == 'Q')
	{
		if (p->ProStep212 < p->Hb212DataLen)
		{
			p->ProStep212++;
			// nwy_ext_echo("\r\np->ProStep212 is [%d] \n",p->ProStep212);
			if (p->ProStep212 == (p->Hb212DataLen))
			{
				wCrc = CRC16_Checkout((BYTE*)&p->ProBuf[p->BeginPos212 + p->Hb212DataLen + EP_HEAD_TAIL_LEN],2);
				sprintf(buf,"%04x",wCrc);
				nwy_ext_echo("crc is %s\n",buf);
	
				lib_ExchangeData((BYTE*)&wCrc1,(BYTE*)&p->ProBuf[p->BeginPos212 + p->Hb212DataLen + EP_HEAD_TAIL_LEN + 2],2);
				sprintf(buf,"%04x",wCrc1);
				nwy_ext_echo("crc 1is %s\n",buf);

				if (memcmp((BYTE*)&wCrc,(BYTE*)&wCrc1,2) != 0)
				{
					return FALSE;
				}
				DealUartMessage_Ep212( p );
				//�������
				InitPoint(p);
				return TRUE;
			}
			
		}		
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}
//--------------------------------------------------
//��������:  uart ͨ��212��ʽ�ж�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  DoReceProc_EP212_UART_test(BYTE ReceByte, TSerial *p)
{
	WORD wCrc;
	char buf[10] = {0};
	BYTE tempcrc[5] = {0};

	if (p->ProStep212 == 0)
	{
		if (ReceByte == '#')
		{
			p->BeginPos212 = p->RXRPoint;
			p->ProStep212++;
		}
		else
		{
			return FALSE;
		}
	}
	else if (p->ProStep212 == 1)//���� ��Լ ���ж�crc
	{
		if ((p->ProBuf[p->BeginPos212] == '#')&&(ReceByte == '#'))
		{
			p->ProStep212++;
			memcpy(buf,&p->ProBuf[p->BeginPos212+EP_DATA_LEN_OFFSET],4);
			p->Hb212DataLen = atol(buf);
			nwy_ext_echo("data len is %d\n",p->Hb212DataLen );
		}
		else
		{
			return FALSE;
		}
	}
	else if (p->ProBuf[p->BeginPos212+EP_CHECK_CRC_OFFSET] == 'Q')
	{
		wCrc = CRC16_Checkout((BYTE*)&p->ProBuf[p->BeginPos212+EP_CHECK_CRC_OFFSET],p->Hb212DataLen);
		sprintf(buf,"%04x",wCrc);
		nwy_ext_echo("crc is %s\n",buf);

		memcpy(tempcrc,&p->ProBuf[p->BeginPos212 + p->Hb212DataLen + EP_CHECK_CRC_OFFSET],4);
		tempcrc[4] = '\0';

		nwy_ext_echo("last1 crc is %s\n",buf);
		nwy_ext_echo("last2 crc is %s\n",tempcrc);

		for (BYTE i = 0; i < 4; i++)
		{
			if (buf[i] != tempcrc[i])
			{
				p->ProStep212 = 0;
				return FALSE;
			}	
		}

		// DealUartMessage_Ep212( p );

		//�������
		InitPoint(p);
		return TRUE;

	}
	else
	{
		return FALSE;
	}
	return FALSE;
}
//--------------------------------------------------
//��������:  ������վ�·�212����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Deal_UserServer_Ep212( tTranData *tempTranData ,BYTE bChannel)
{
	TEpReserve temp = {0};

	temp.Channel = bChannel;
	temp.wCrc = CRC16_Checkout((BYTE*)&temp.Channel,2);
	lib_InverseData((BYTE*)&temp.wCrc,2);
	memcpy((BYTE*)&tempTranData->buf[tempTranData->len],(BYTE*)&temp,4);
	tempTranData->len += 4;
	
	nwy_ext_echo("\r\n ep212 recv \r\n");
	for (WORD i = 0; i < tempTranData->len; i++)
	{
		nwy_ext_echo("%02X ",tempTranData->buf[i]);
	}
	nwy_ext_echo("\r\n ep212 recv \r\n");
	if (QueueWrite(&MessageQueueInfoArr[eQUEUE_EP212_TO_UART], tempTranData, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
	{
		nwy_ext_echo("\r\nnwy_put_msg_que  Ep212ToUartMsgQue success");
	}
}
#endif//#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_212)