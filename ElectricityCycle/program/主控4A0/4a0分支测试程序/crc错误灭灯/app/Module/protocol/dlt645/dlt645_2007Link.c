/*****************************************************************************/
/*�ļ�����dlt645_2007.c
 *Copyright (c) 2008 ��̨������˼�ٵ������޹�˾������Ʒ������
 *All rights reserved
 *
 *�����ˣ�κ����
 *��  �ڣ�2008-9-21
 *�޸��ˣ�κ����
 *��  �ڣ�2009-3-26
 *��  ����DTSD/DSSD178ȫ����ʽ�๦�ܵ��ܱ�ͨѶ����DL/T645-2007��Լ����
 *
 *��  ����
 *˵  ����ReadDlt645_2007Data()���� �ɹ�LCD���¼������������ߡ�������ã�ֻҪ�����ǹ�Լ��ʽ�����Ե��ô˺���
 *		  ���������ڲ����� BYTE *ProBuf����ProBuf�ǹ�Լ����TSerial�е�ProBuf��
 *		  ���������ڲ����� BYTE *pBuf���� pBufȡ���ڵ���ʱ���������
 */
/*****************************************************************************/

#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define	DLT645_DATA33H				(0x33)
#define	DLT645_ADDR_LEN				(6)
#define	DLT645_07_DATAID_LEN		(4)
#define	DLT645_07_PASSWORD_LEN		(3)	//4��Ϊ3--hy2017.11.14
#define	DLT645_07_OPERATORCODE_LEN	(4)	//3��Ϊ4--hy2017.11.14
#define DLT645_ADDR_STARTONFRAME	(1)
#define DLT645_CONTROL_STARTONFRAME	(8)
#define DLT645_LEN_STARTONFRAME		(9)
#define DLT645_DATA_STARTONFRAME	(10)


#define	DLT645_97_DATAID_LEN		(2)

//��ʼ�� 645_2007 һЩ�������ϵ�ʹӵ͹����ϵ�ʱ��ִ��
void InitDlt645_2007(void)
{
	//Identitytimer = 0;	
}

//--------------------------------------------------
//��������: ���645��Լ����
//         
//����:      BYTE *bBuf[in] ���뻺��bBuf[0]Ϊ��ʼ�ֽ�68
//         
//����ֵ:    static
//         
//��ע����:  ֻ�ж���07��Լ����9
//--------------------------------------------------
static eTYPE_PROTOCOL GetDlt645ProtocalType( eSERIAL_TYPE PortType )
{
	TFourByteUnion DataID;
	eTYPE_PROTOCOL eResult;

	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_01 )//97��Լֻ֧�ֶ�����
	{
		eResult = ePROTOCOL_DLT645_97;
	}
	else
	{
		memcpy( DataID.b, gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME, DLT645_07_DATAID_LEN );//��ȡ���ݱ�ʶ

		if( DataID.d == DLT645_FACTORY_ID )
		{
			eResult = ePROTOCOL_DLT645_FACTORY;
		}
		else
		{
			eResult = ePROTOCOL_DLT645_07;
		}
	}
	return eResult;
}

//--------------------------------------------------
//��������:  �Ա��Ľ��м�0x33����
//         
//����:      BYTE *pBuf[in]:���뻺��pBuf[0]Ϊ68H
//         
//����ֵ:    void
//         
//��ע����:  ��
//--------------------------------------------------
void DataSubtract33H(BYTE *pBuf)
{
	WORD i;
	for(i=DLT645_DATA_STARTONFRAME;i<(DLT645_DATA_STARTONFRAME+pBuf[DLT645_LEN_STARTONFRAME]);i++)
	{
		pBuf[i]=(pBuf[i]-DLT645_DATA33H);
	}
}

//--------------------------------------------------
//��������:  �Ա��Ľ��м�0x33����
//         
//����:      BYTE *pBuf[in]:���뻺��pBuf[0]Ϊ68H
//         
//����ֵ:   void 
//         
//��ע����:  ��
//--------------------------------------------------
void DataPlus33H( BYTE *pBuf )
{
	WORD i;
	for(i=DLT645_DATA_STARTONFRAME;i<(DLT645_DATA_STARTONFRAME+pBuf[DLT645_LEN_STARTONFRAME]);i++)
	{
		pBuf[i]=(pBuf[i]+DLT645_DATA33H);
	}
}

//--------------------------------------------------
//��������:  �ж�ͨ�ŵ�ַ�Ƿ�Ϊ��ͬ����������
//         
//����:      BYTE bySpecialData[in]
//         
//           BYTE *pBuf[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
WORD JudgeMeterNoIfEqually(BYTE bySpecialData,BYTE *pBuf)
{
	BYTE i;

	for(i=0; i<6; i++)
	{
		if(pBuf[i] != bySpecialData )
		{
			return FALSE;
		}
	}

	return TRUE;
}
//--------------------------------------------------
//��������:  �ж��Ƿ���ͨ���ַ
//         
//����:      BYTE *pBuf[in]
//         
//����ֵ:    
//         
//��ע����:  ������Ϊ�жϵ�ַ�Ƿ�һ��������
//--------------------------------------------------
WORD JudgeMeterNoAccessAddress(BYTE *pBuf)
{
	BYTE *pMeterNo;

	BYTE i;

	pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);

	for(i=0; i<6; i++)
	{
		if( pMeterNo[5-i] != 0xaa )//�������ֽڱ��ַ����AA
		{
			if( pBuf[i] == 0xaa )//��ͨ��ʱ����ֽ���AA
			{
				return TRUE;
			}
		}

	}

	return FALSE;

}

//--------------------------------------------------
//��������:  ���645��ַ����
//         
//����:      BYTE *bBuf[in] ���뻺��
//         
//����ֵ:    TYPE_ADDR_TYPE
//         
//��ע����:  ��
//--------------------------------------------------
static eTYPE_ADDR_TYPE GetDlt645AddrType(BYTE *bBuf )
{
	eTYPE_ADDR_TYPE eResult;

	if( JudgeMeterNoIfEqually(0x88, bBuf) == TRUE )//�ж��Ƿ�Ϊȫ8��ַ
	{
		return eTYPE_ADDR_TYPE_88;
	}
	else if( JudgeMeterNoIfEqually(0x99, bBuf) == TRUE )//�ж��Ƿ�Ϊȫ9��ַ
	{
		return eTYPE_ADDR_TYPE_99;
	}
	else if( JudgeMeterNoIfEqually(0xAA, bBuf) == TRUE )//�ж��Ƿ�ΪȫAA��ַ
	{
		return eTYPE_ADDR_TYPE_ALLAA;
	}
	else if( JudgeMeterNoAccessAddress( bBuf ) == TRUE )//�ж��Ƿ�Ϊͨ���ַ
	{
		return eTYPE_ADDR_TYPE_AA;
	}
	else
	{
		return eTYPE_ADDR_TYPE_XX;//��ͨ��ַ
	}
}


//--------------------------------------------------
//��������:  ��645�����ݽ������ṹ����
//         
//����:      BYTE *bBuf[in]  ���뻺��bBuf[0]Ϊ��ʼ�ֽ�68
//         
//           TYPE_PROTOCOL eProtocalType[in]  ��Լ����
//         
//           BYTE *Len[in]  ���ݱ�ʶ����
//         
//����ֵ:    DWORD ���ݱ�ʶ
//         
//��ע����:  ��
//--------------------------------------------------
static void DealDlt645DataToStruct( eSERIAL_TYPE PortType, BYTE *pBuf, eTYPE_PROTOCOL eProtocalType )
{
	
	DWORD dwResult;

	if( (eProtocalType == ePROTOCOL_DLT645_07) || (eProtocalType == ePROTOCOL_DLT645_FACTORY) )//��07�������������н���
	{
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_11://������
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//��ȡ���ݱ�ʶ
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//�������ַ
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN);//����������
				break;
			case eCONTROL_RECE_12://������֡
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//��ȡ���ݱ�ʶ
				gPr645[PortType].bSEQ = pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//֡���
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1];//�������ַ
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-1);//����������		
				break;	
			case eCONTROL_RECE_14://д����
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//��ȡ���ݱ�ʶ 
				gPr645[PortType].bLevel = pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//����ȼ�
				memcpy( gPr645[PortType].bPassword,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1], DLT645_07_PASSWORD_LEN );//����
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1+DLT645_07_PASSWORD_LEN], DLT645_07_OPERATORCODE_LEN);//�����ߴ���
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1+DLT645_07_PASSWORD_LEN+DLT645_07_OPERATORCODE_LEN];//�������ַ
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-1-DLT645_07_PASSWORD_LEN-DLT645_07_OPERATORCODE_LEN);//����������
				break;	

			case eCONTROL_RECE_03://Ԥ��������
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//��ȡ���ݱ�ʶ 
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN], DLT645_07_OPERATORCODE_LEN);//�����ߴ���
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+DLT645_07_OPERATORCODE_LEN];//�������ַ
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-DLT645_07_OPERATORCODE_LEN);//����������
			break;	

			case eCONTROL_RECE_1C://����բ����
			case eCONTROL_RECE_19://��������
			case eCONTROL_RECE_1A://�������
			case eCONTROL_RECE_1B://�¼�����
				gPr645[PortType].bLevel = pBuf[DLT645_DATA_STARTONFRAME];//����ȼ�
				memcpy( gPr645[PortType].bPassword,&pBuf[DLT645_DATA_STARTONFRAME+1], DLT645_07_PASSWORD_LEN );//����
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+1+DLT645_07_PASSWORD_LEN], DLT645_07_OPERATORCODE_LEN);//�����ߴ���
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+1+DLT645_07_PASSWORD_LEN+DLT645_07_OPERATORCODE_LEN];//�������ַ
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-1-DLT645_07_PASSWORD_LEN-DLT645_07_OPERATORCODE_LEN);//����������
			break;

			default:
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME];//�������ַ
				gPr645[PortType].wDataLen = pBuf[DLT645_LEN_STARTONFRAME];//����������
			break;
		}

		gPr645[PortType].bIDLen = 4;//���ݱ�ʶ������Э����� ��Э�������� ����Ƿ����
	}
	else//97��Լ
	{
		memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_97_DATAID_LEN );//��ȡ���ݱ�ʶ
		gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_97_DATAID_LEN];//�������ַ
		gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_97_DATAID_LEN);//����������

		gPr645[PortType].bIDLen = 2;//���ݱ�ʶ������Э����� ��Э�������� ����Ƿ����
	}
	
}

//--------------------------------------------------
//��������:  645���Ľ�������
//         
//����:      TYPE_PORT PortType[in]:Э������
//         
//           WORD Len[in]:���ݳ���
//         
//           BYTE *pBuf[in]:����buf
//         
//����ֵ:    WORD
//         
//��ע����:  ��
//--------------------------------------------------
static WORD GetAndCheck645Pare( eSERIAL_TYPE PortType, BYTE *pBuf)
{
	if( PortType >= MAX_COM_CHANNEL_NUM )
	{
		return 0xffff;
	}
	
	gPr645[PortType].pMessageAddr = pBuf;
	
	gPr645[PortType].eAddrType= GetDlt645AddrType( pBuf+DLT645_ADDR_STARTONFRAME );
	
	gPr645[PortType].wSerialDataLen=pBuf[DLT645_07_PASSWORD_LEN];//�������ݳ��� ������serail�ṹ��ָ�����ȡ�����������ݳ���!!!!!
	
	gPr645[PortType].byReceiveControl=pBuf[DLT645_CONTROL_STARTONFRAME];//���ձ��Ŀ�����
	
	gPr645[PortType].eProtocolType = GetDlt645ProtocalType( PortType );
	
	DealDlt645DataToStruct( PortType, pBuf, gPr645[PortType].eProtocolType );//�������ݵ��ṹ����

	if( gPr645[PortType].byReceiveControl != eCONTROL_RECE_12 )//�����벻�Ƕ�����֡������������֡
	{
		memset( &DLT645APPFollow[PortType].FollowFlag, 0x00, sizeof(DLT645APPFollow[0]) );
	}

	return TRUE;
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
    
	p->byLastRXDCon = p->ProBuf[8];//�����ϴ�ͨ�ŵ��Ӧ������룬�����������֡�ж���

	TDataLength = p->ProBuf[9];
	
	//fe fe fe fe 68 A0...A5 68 81 L D0..DL CS 16,��D0...DL�⣬���๲16�ֽ�
	if( TDataLength > (MAX_PRO_BUF-16) )//�������
	{//�˾�Ӧ�ɷ��ѱ���ڴ����ݳ���
		p->BroadCastFlag = 0xff;
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

	//ִ����ʱ
	p->ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;


	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIEnableSend(SerialMap[i].SCI_Ph_Num);
			break;
		}
	}
}



//---------------------------------------------------------------
//��������: �ж��·������Ƿ�㲥��ַ�����б�ĵط����ô˺���������ȡ��
//
//����: 	pBuf[in] - ����ͨ�ŵ�ַ
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
WORD JudgeBroadcastMeterNo(BYTE *pBuf)
{
	//�жϽ��ձ����е�ͨ�ŵ�ַ�Ƿ�ȫ���Ǹ���������,����99��88
	return JudgeRecMeterNoOnlyOneData(0x99,pBuf);
}

//Ҫ��pBuf[0--5]����Ϊ���ձ����еı��,���ŶԷ���TRUE,���򷵻�FALSE
WORD JudgeRecMeterNo_645(BYTE *pBuf)
{
	//#if( SEL_DLT645_2007 != YES )
	BYTE k;
	//#endif

	BYTE *pMeterNo;

	BYTE i,j;


	//�ж��·������Ƿ�㲥��ַ
	//j = JudgeBroadcastMeterNo(pBuf);

	//if(j!=TRUE)
	{
		//�жϽ��ձ����е�ͨ�ŵ�ַ�Ƿ�ȫ���Ǹ���������,����99��88
		j = JudgeRecMeterNoOnlyOneData(0x88,pBuf);
	}

	if(j!=TRUE)
	{
		j=TRUE;
		pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);//ͨѶ��ַBYTE CommAddr[6]��BCD [0]-������ֽ�
		
		k = 5;//(METER_ADDR_BYTE_NUM-1);//SEL_DLT645_2007 �� k:�ݱ����һ��AA��λ��
		//������б��ַ�жϣ��ӵ��ֽڿ�ʼ�ж�
		for(i=0; i<6; i++)
		{
			if( pBuf[i] == pMeterNo[5-i] )
			{
				if( pMeterNo[5-i] != 0xaa )//�������ֽڱ��ַ����AA
				{
					//���AA����ֲ���AA�����ݣ�645Ҫ��ʣ���λ��AAH������̩��Ϊ�м�͵�λҲ������AA
					if( i > k )
					{
						j = FALSE;
						break;
					}
				}
			}
			else if( pBuf[i] == 0xaa )
			{
				k = i;//k:�ݱ����һ��AA��λ��
			}
			else
			{
				j = FALSE;
				break;
			}
		}
	}
	
	return j;
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
WORD DoReceProc_Dlt645( BYTE ReceByte, TSerial * p )
{
	WORD i, j;
	WORD Result;
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
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL];
				}
				//ӳ��ṹ
				//pDlt645Head = (TDlt645Head *)&TmpArray;
                j = p->ProStepDlt645;

				//�����ڶ�ͬ����
				if( Dlt645Head.Sync2 != 0x68 )//���û�е�2��֡��ʼ��68����ǰ���յ��ĵ�һ68���ԣ���������������һ��68
				{
					for( i=1; i<p->ProStepDlt645; i++ )
					{
						if( p->ProBuf[(p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL] == 0x68 )
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL;
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
/*				
				//if( p == &Serial[eIR] )//�����ǰͨ��Ϊ����ͨ��
				//{
				//    api_SetLcdCommunicationFlag( eIR );//��ͨѶ��־
				//}
				//�ж��·������Ƿ�㲥��ַ
				if( JudgeBroadcastMeterNo(Dlt645Head.ID) == FALSE )
				{	
					if( JudgeRecMeterNo_645(Dlt645Head.ID) == FALSE )//����ж�����68���ͨ�ŵ�ַ���ԣ����2��68��ʼ���¼����ձ���
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
				//���յ���������ʱ��dlt645 Weisheng Modbusͳһ��DisableRece()�и�λ���ڼ���
				//��λ���ڼ���
				//p->WatchSciTimer = WATCH_SCI_TIMER;
				
				//ȡ�����ֽ�
				if( Dlt645Head.Control >= 0x80 )//����жϿ����벻�ԣ����2��68��ʼ���¼����ձ���
				{
					p->BeginPosDlt645 = p->BeginPosDlt645+7;
					p->ProStepDlt645 -= 7;
					return FALSE;
				}
 */             
 				#if (SEL_SEARCH_METER == YES )
                if ( (p == &Serial[SEARCH_METER_CH])
                  &&( SKMeter.byAutoCheck != 0 ) 
                  &&( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
                   ) // �ѱ�ͨ�����ж�
                {
                    
                }
                else 
                #endif
                if (JudgeBroadcastMeterNo(Dlt645Head.ID) || JudgeRecMeterNo_645(Dlt645Head.ID))
				{
					if( Dlt645Head.Control >= 0x80 )//����жϿ����벻�ԣ����2��68��ʼ���¼����ձ���
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
                else
				{
					// ������ַ
					if (IsInSysMeters(Dlt645Head.ID))
					{
						
						if (Dlt645Head.Control >= 80) //��վӦ��֡���������մ���
						{
							// if (p == &Serial[eCR]) //�ز��ڵĽ��շ���1376.2��Լ�д���
							// {
							// 	p->BeginPosDlt645 = p->BeginPosDlt645+7;
							// 	p->ProStepDlt645 -= 7;
							// 	return FALSE;
							// }
						}
						else //��վ����֡����Ϊ��͸���а�����645֡���������մ���
						{
							if( (p->ProStepDlt698_45 >= 40) || (p->ProStep3762 >= 30) )//698����645ʱ��645���Ĳ�ֱ�ӽ��д�����698�д���							
							{
								p->BeginPosDlt645 = p->BeginPosDlt645+7;
								p->ProStepDlt645 -= 7;
								return FALSE;
							}
						}
					}
					else if(MessageTransParaRam.AllowUnknownAddrTrans == TRUE)
					{
					
					}
					else
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
                
				//ȡ���� 645�������������ݳ���
				p->Dlt645DataLen = Dlt645Head.Length;

				//�жϳ���
				//68 A0...A5 68 81 L D0..DL CS 16,��D0...DL�⣬���๲16�ֽ�
				if( p->Dlt645DataLen >= ( MAX_PRO_BUF - 12 ) )
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
				if( ReceByte != CalRXD_CheckSum(PRO_NO_SPECIAL, p) )
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

				//p->RXWPoint = MAX_PRO_BUF_REAL-1;
				//if( ReceByte != 0x16 )//���16H���ԣ���֡���ķ�����
				//{
				//	p->ProStepDlt645 = 0;//InitPoint(p);
				//	return FALSE;
				//}

				//��ֹ����
				api_DisableRece( p );

				//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
				DoReceMoveData(PRO_NO_SPECIAL, p);
				
				if( Pre_Dlt645(p) )
				{
					api_EnableRece( p );
				}
				else
                {
					ProMessage_Dlt645( p );
				}
				TX_LED_FLASH;
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



//////////////////////////////////////////////////
//--------------------------------------------------
//��������:  ����645ͨ�ŵ�ַ
//         
//����:      TYPE_PORT PortType[in]:��������
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void FillDlt645FrameAddr( eSERIAL_TYPE PortType )
{
	BYTE i;
	
	for(i=0; i<6; i++)
	{
		//ȷ�������Ƿ���ȷ
		gPr645[PortType].pMessageAddr[DLT645_ADDR_STARTONFRAME+i] =FPara1->MeterSnPara.CommAddr[5-i];
	}
}

//--------------------------------------------------
//��������:  ����645������
//         
//����:      TYPE_PORT PortType[in]:��������
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void FillDlt645FrameSendControl( eSERIAL_TYPE PortType, WORD wResult )
{
	if( wResult < 0x8000 )//����Ӧ��
	{
		if( DLT645APPFollow[PortType].FollowFlag != eNO_FOLLOW )//�ж��Ƿ��к���֡
		{
			gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0xb0);
		}
		else
		{
			gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0x80);
		}		
	}
	else//����Ӧ��
	{
		gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0xC0);
	}
}

//--------------------------------------------------
//��������:  ����645����֡����
//         
//����:      
//			 TYPE_PORT PortType[in]:��������
//			 Lenth - ��Լ���ݳ���
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
const BYTE ReportDI[4] = { 0x04, 0x00, 0x15, 0x01 }; //�����ϱ����ݱ�ʶ
static void FillDlt645FollowData( eSERIAL_TYPE PortType, WORD Lenth )
{
	WORD SendLen;//��֡�������ݳ���
	WORD wSentLen;//�ѷ��ͳ���
	
	if( (DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW) 
	  ||(DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW_END))//����Ǹ��ɼ�¼����֡
	{			
		gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = Lenth+DLT645_07_DATAID_LEN+1;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ��֡���
		memcpy( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
		gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + Lenth] = gPr645[PortType].bSEQ;
		
		if(DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW_END)
		{
			DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
		}
		DLT645APPFollow[PortType].bLastNum++;
	}
	else if( DLT645APPFollow[PortType].FollowFlag == eNORMAL_FOLLOW )//��������֡
	{
		DLT645APPFollow[PortType].bLastNum ++;
		
		//ÿ֡���ݹ̶� DLT645_FOLLOW_LEN ���ֽ� ����֡����һ�ֽ�֡���(��һ֡�������֡) 
		//����: �ѷ����ֽ��� = DLT645_FOLLOW_LEN(��һ֡���ݣ���֡���) + (DLT645_FOLLOW_LEN-1)*����֡���� 
		wSentLen = DLT645_FOLLOW_LEN + ( (gPr645[PortType].bSEQ - 1) * (DLT645_FOLLOW_LEN - 1) );
		
		if( DLT645Sendbuf[PortType].DataLen > wSentLen )
		{
			SendLen = DLT645Sendbuf[PortType].DataLen - wSentLen; //��������֡�����ܳ���ʹ�û����еĳ���
		}
		else
		{
			SendLen = 0;
		}
		
		if( SendLen >= DLT645_FOLLOW_LEN )//ʣ�����ݳ���200���ֽ�
		{
			gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = (DLT645_FOLLOW_LEN-1)+DLT645_07_DATAID_LEN+1;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ+1���Լ�֡���
			memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf+wSentLen, (DLT645_FOLLOW_LEN-1) );
			gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + DLT645_FOLLOW_LEN-1] = gPr645[PortType].bSEQ;
		}
		else
		{
			gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = SendLen+DLT645_07_DATAID_LEN+1;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ+1���Լ�֡���
			memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf+wSentLen, SendLen );
			gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + SendLen] = gPr645[PortType].bSEQ;
			
			DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
			DLT645APPFollow[PortType].bLastNum = 0;
		}
	}
	else if( DLT645APPFollow[PortType].FollowFlag == eREPORT_FOLLOW ) //�����ϱ�����֡����
	{
		SendLen = api_ReadReportStatusByte( PortType,eREAD_STATUS_PROC, DLT645Sendbuf[PortType].DataBuf ); //!!!!!!!�����ϱ���δ֧��
		
		gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = SendLen+DLT645_07_DATAID_LEN+1;
		memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, SendLen );
		gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + SendLen] = gPr645[PortType].bSEQ; //֡��Ź̶�Ϊ0x01
		memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME, ReportDI, 4 );
		lib_InverseData( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME, 4 );
		
		//���ڴ˴���������ϱ���־ ������һ������ ����������ζ�ȡ�Ƿ��Ƕ�ȡ�����ϱ�״̬��
		DLT645APPFollow[PortType].bLastNum = 0;
	}
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
static void FillDlt645FrameData( eSERIAL_TYPE PortType, WORD wResult )
{
	WORD Lenth;
	
	if( wResult < 0x8000 )//����Ӧ��
	{
		Lenth = wResult;
		
		if( gPr645[PortType].byReceiveControl != eCONTROL_RECE_12 )
		{
			DLT645Sendbuf[PortType].DataLen = Lenth;
			gPr645[PortType].wDataLen = Lenth;
		}
		
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_01://������(07��Լ) ��ʱδ��

				break;
			
			case eCONTROL_RECE_11://������
				if( DLT645APPFollow[PortType].FollowFlag != eLOAD_FOLLOW )
				{
					if( gPr645[PortType].wDataLen <= DLT645_FOLLOW_LEN )
					{
						gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ
						memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
					}
					else //��Ҫ��֡����
					{
						gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = DLT645_FOLLOW_LEN+DLT645_07_DATAID_LEN;//���ݳ��ȶ�4���ֽڵ�OAD
						memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, DLT645_FOLLOW_LEN );
						
						DLT645APPFollow[PortType].FollowFlag = eNORMAL_FOLLOW;
						DLT645APPFollow[PortType].bLastNum = 0;
					}
				}
				else//���ɼ�¼��Ҫ�������
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ
					memcpy( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
				
					DLT645APPFollow[PortType].bLastNum = 0;
				}
				//�����ϱ�����֡
				if( DLT645APPFollow[PortType].FollowFlag == eNO_FOLLOW )//!!!!!!�����ϱ������ϱ���δ����
				{
					//��������ǳ��������ϱ� ���ú���֡��־
					if(  gPr645[PortType].dwDataID.d != 0x04001501 )
					{
						if( api_GetReportReqFlag( PortType ) == TRUE )
						{
							//DLT645APPFollow[PortType].FollowFlag = eREPORT_FOLLOW;
						}
					}
				}
				break;
			case eCONTROL_RECE_12://������֡
				FillDlt645FollowData( PortType, Lenth );
				
				//�����ϱ�����֡ ����Ƕ�ȡ�����ϱ� ��Ӧ�к���֡��־
				if( DLT645APPFollow[PortType].FollowFlag == eREPORT_FOLLOW )
				{
					//����Ѿ��������ϱ�����֡ �˴ζ�ȡ��Ҫ����״̬�� ���Ҳ��ظ�����֡��־
					DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
				}
				else if( DLT645APPFollow[PortType].FollowFlag == eNO_FOLLOW )
				{
					//���û���ϱ���־ ˵�������Ѿ���ȡ���� ��ʱ����Ƿ��������ϱ� ����� ��Ҫֻ����֡��־
					if( api_GetReportReqFlag( PortType ) == TRUE )
					{
						//DLT645APPFollow[PortType].FollowFlag = eREPORT_FOLLOW;
					}
				}
				break;

			case eCONTROL_RECE_03://Ԥ��������
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//���ݳ��ȶ�4���ֽڵ����ݱ�ʶ		
				memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
				break;
			
			case eCONTROL_RECE_14://д����
			case eCONTROL_RECE_16://��������
			case eCONTROL_RECE_19://��������
			case eCONTROL_RECE_1A://�������
			case eCONTROL_RECE_1B://�¼�����
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 0;//���ݳ���Ϊ��
				break;

			case eCONTROL_RECE_1C://����բ����
				if( gPr645[PortType].bPassword[2] != 0x01 )
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 0;//���ݳ���Ϊ��
				}
				else
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2;//���ݳ���2
					memcpy( (BYTE*)&gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], DLT645Sendbuf[PortType].DataBuf, 2 );
				}
				break;
				
			default:
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen;	
				memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME, DLT645Sendbuf[PortType].DataBuf, Lenth );
				break;
		}

	}
	else//�쳣Ӧ��
	{
		wResult = wResult&0x7FFF;
		
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_01://������(07��Լ) ��ʱδ��
				break;

			case eCONTROL_RECE_03://Ԥ��������
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2; //���ݳ���2
				memcpy( (BYTE*)&gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], (BYTE*)&wResult, 2 );
				break;
			
			case eCONTROL_RECE_1C://����բ����
				if( gPr645[PortType].bPassword[2] != 0x01 )
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 1;//���ݳ���1
					gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME] = (BYTE)wResult;	
				}
				else
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2;//���ݳ���2
					memcpy( &gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], (BYTE*)&wResult, 2 );
				}
				break;
			
			default:
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 1;//���ݳ���1
				gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME] = (BYTE)wResult;
				break;
		}
	}
}

//--------------------------------------------------
//��������:   ����645֡ͷ֡βУ���
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void FillDlt645FrameHeadAndTail(eSERIAL_TYPE PortType )
{
	BYTE i;
	WORD wLen;
	
	wLen = gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] + DLT645_ADDR_LEN + 4; //4������68 һ�������� һ������
	
	gPr645[PortType].pMessageAddr[0] = 0x68;
	gPr645[PortType].pMessageAddr[DLT645_ADDR_STARTONFRAME+DLT645_ADDR_LEN] = 0x68;
	gPr645[PortType].pMessageAddr[wLen] = lib_CheckSum( gPr645[PortType].pMessageAddr, wLen );
	gPr645[PortType].pMessageAddr[wLen+1] = 0x16;
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
void Proc645LinkDataRequest( eSERIAL_TYPE PortType, BYTE *pBuf )
{
    api_SetLcdCommunicationFlag( PortType );//��ͨѶ��־

	DataSubtract33H( pBuf );//�����ݽ��м�0x33����
	
	GetAndCheck645Pare(PortType,pBuf);//���ݽ������ṹ��
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
void Proc645LinkDataResponse( eSERIAL_TYPE PortType, WORD wResult)
{
	if( wResult == DLT645_NO_RESPONSE )//����Ӧ��
	{
		api_InitSci( PortType );
		return;
	}
	
	FillDlt645FrameAddr( PortType );
	
	FillDlt645FrameData(PortType,wResult);
	
	//����������������ݺ� ��Ϊ��Ҫ�ж��Ƿ��к���֡
	FillDlt645FrameSendControl( PortType, wResult );
	
	DataPlus33H(gPr645[PortType].pMessageAddr);
	
	FillDlt645FrameHeadAndTail(PortType);
	
	SendDlt645( &Serial[PortType] );
}

#endif//#if( SEL_DLT645_2007 == YES )

