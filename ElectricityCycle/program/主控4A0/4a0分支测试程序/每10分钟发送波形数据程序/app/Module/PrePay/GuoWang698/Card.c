//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.17
//����		

//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"


#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------



//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						

//������Ϣ�ļ�����
typedef enum
{
	eSetRateSwitchTime = 0, //���׷�ʱ�����л�ʱ��
	eSetLadderSwitchTime,   //���׽����л�ʱ��
	eSetAlarmLimit1,        //�������1
	eSetAlarmLimit2,        //�������2
	eSetCTCoe,              //�������
	eSetPTCoe,              //��ѹ���
	eSetMeterNo,            //���
	eSetCustomCode,         //�ͻ����
}eSetInforParaType; 

//��Ҫ���������ʹ��
typedef struct TFourByteMoneyBag_t
{
	TFourByteUnion		Remain;        //ʣ����
	DWORD				BuyTimes;      //�������
}TFourByteMoneyBag;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
static BYTE 	CardCheck;                      //�����
static BYTE		CardType;						//������
static BYTE		Ic_Err_Reg;                     //IC�����������ʶ
static BYTE		UpdateFlag;                     //���±�־
static BYTE 	InsertCardSta;                  //�忨״̬��
BYTE			CommandHead[7];					//�ݴ�����ͷ,��һ���ֽڴ���ESAM������6���ֽ�����ͷ����CARD������5���ֽ�����ͷ����

//������Ϣ�ļ�����������Ӧapi_WritePrePayPara������ö�ٺ�
const ePrePayParaType ParaFileEnum[4] = { eAlarm_Limit1, eAlarm_Limit2, eCTCoe, ePTCoe };

static TFourByteMoneyBag 	FourByteNewBuyMoneyBag; 
static TFourByteMoneyBag 	FourByteOldBuyMoneyBag; //Ϊ��¼�����¼׼���Ľṹ��

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ѡ���ļ�
//
//����:
//			F1[in]	�ļ����
//			F2[in]	�ļ����
//����ֵ:	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
static BYTE SelectFile( BYTE F1, BYTE F2 )
{
	BYTE Result;

	BYTE Buf[10];
	Buf[0] = F1;  //DATA0
	Buf[1] = F2;  //DATA1
	Result = Link_Card_TxRx( 0x00, 0xa4, 0x00, 0x00, 0x02, WRITE, Buf );

	return Result;
}

//������ESAM�ж������ļ�
static BYTE ReadBinFile( BYTE P1, BYTE P2, BYTE P3, BYTE *peeerom )
{
	BYTE Result;

	Result = Link_Card_TxRx( 0x00, 0xb0, P1, P2, P3, READ, peeerom );

	return Result;
}

//-----------------------------------------------
//��������: ��MAC��ȡ��Ƭ�ж���������
//
//����:
//			CH[in]
// 				01:��ȡ�û�����Ԥ�ÿ��г���Ǯ��֮����ļ�
// 				21:��ȡԤ�ÿ���Ǯ���ļ�
// 				31:��ȡ�û�����Ǯ���ļ�
//			P1[in],P2[in],M1[in],M2[in]
// 				ָ�����ĸ�ʽ���������ֲ�˵��
//			Len[in]
// 				�ӿ��ж������ݵĳ���+4�ֽ�mac�ĳ���(Lc+4)
//����ֵ:	�ɹ�/ʧ��
//
//��ע:
//
//-----------------------------------------------
static BYTE ReadBinWithMacCard( BYTE CH, BYTE P1, BYTE P2, BYTE M1, BYTE M2, BYTE Len, BYTE *peerom )
{
	BYTE Result;
	BYTE Buf[10];

	Result = FALSE;

	memcpy( Buf, SafeAuPara.EsamRand, 4 );
	Buf[4] = 0x83;
	Buf[5] = 0x2A;
	Buf[6] = M1;
	Buf[7] = M2;
	Buf[8] = Len;

	if( CH == 0x21 )
	{
		Buf[5] = 0x3E;
	}
	else if( CH == 0x31 )
	{
		Buf[4] = 0x80;
		Buf[5] = 0x42;
	}
	Result = Link_Card_TxRx( 0x04, 0xb0, P1, P2, 0x09, WRITE, Buf );
	if( Result != TRUE )
	{
		return FALSE;
	}
	CLEAR_WATCH_DOG;
	Result = Link_Card_TxRx( 0x00, 0xC0, 0x00, 0x00, Len, READ, peerom ); //����data+MAC
	if( Result != TRUE )
	{
		return FALSE;
	}

	return Result;
}

//-----------------------------------------------
//��������: ȡCARD�����
//
//����:		Len[in]		��������� 4/8/10
//			peeerom[out]	�������+���ݣ��������ֽڣ�����ǰ�����ں�
//����ֵ:	TRUE/FALSE
//
//��ע:
//-----------------------------------------------
BOOL ReadRandomCARD( BYTE Len, BYTE *peeerom )
{
	BYTE Result;

	Result = Link_Card_TxRx( 0x00, 0x84, 0x00, 0x00, Len, READ, peeerom );
	if( Result != TRUE )
	{
		return Result;
	}
	return Result;
}

//-----------------------------------------------
//��������: ���Ƿ��忨����
//
//����:  		Buf[out] ����Ƿ��忨���� 
//
//����ֵ:		��
//		   
//��ע:			
//-----------------------------------------------
void api_ReadIllegalCardNum(BYTE *Buf)
{
	memset( (void *)Buf, 0x00, 4 );
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(PrePayConMem.dwIllegalCardNum), sizeof(DWORD), Buf);	
	//lib_InverseData(Buf, 4 );
}

//-----------------------------------------------
//��������:     �쳣�忨�ͷǷ��忨��¼��¼
//
//����:  		��
//
//����ֵ:		��
//		   
//��ע:			�쳣�忨��¼�ͷǷ��忨��¼���ڴ˼�¼���Ƿ��忨��¼ֻ�Ǵ���
//-----------------------------------------------	
static void Card_Err_Record(void)
{
	BYTE ErrRecordBuf[18];
	DWORD dwData;
	
	//if( Ic_Err_Reg == IC_IC_RST_ERR )
	//{
		//�����ѿص��ܱ�ϵ�м�����׼����(��).pdf:�������Ϊ���ܱ���ʶ��Ľ��ʣ�����Ƭ�򷴲忨��ʱ�����ܱ�������зǷ��忨���쳣�忨��¼��
		//�������ܹ淶�����ܱ��������Ľ��ʲ������������������Ϊ���쳣�忨�����Ƿ��忨��ע�����д����������㵫�������ɹ�����Ϊ�������쳣�忨�� 
		//return;
	//}
	if((!CARD_IN_SLOT)&& (Ic_Err_Reg==IC_NOIN_ERR))
	{
		Ic_Err_Reg = IC_PREIC_ERR;	//��ǰ�ο�
	}
	
	memcpy( ErrRecordBuf, SafeAuPara.CardRst8B, 8);	//�����к�
	ErrRecordBuf[8]= lib_BBCDToBin(Ic_Err_Reg);					//������Ϣ��
	
	if( (Ic_Err_Reg==IC_ESAM_RDWR_ERR)
	 || (Ic_Err_Reg == IC_NOFX_ERR)
	 || (Ic_Err_Reg == IC_MAC_ERR) 
	 || (Ic_Err_Reg == IC_NOIN_ERR)
	 || (Ic_Err_Reg == IC_EXAUTH_ERR)
	 || (Ic_Err_Reg == IC_TYPE_ERR) )
	{
		//����ͷ����ʱ���� CH,Cla,Ins,P1,P2,P31,P32,����CH:0x01--ESAM 0x00--CARD,esam����ͷΪCH���6�ֽڣ���ΪCH���5�ֽ�
		lib_ExchangeData( ErrRecordBuf+9, CommandHead, 7 );		//��������ͷ
		ErrRecordBuf[16] = Err_Sw12[1];						//������Ӧ״̬SW2
		ErrRecordBuf[17] = Err_Sw12[0];						//������Ӧ״̬SW1
	}
	else
	{
		memset(ErrRecordBuf+9, 0x00, 9);
	}
	
	//api_SavePrgOperationRecord �ж�����¼���ݴ� PreProgramData.Data[4] ��ʼ���������д� PreProgramData.Data[4] ����
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(ErrRecordBuf), ErrRecordBuf );
	api_SavePrgOperationRecord( ePRG_ABR_CARD_NO ); ////���ܱ��쳣�忨�¼�

	if( (Ic_Err_Reg == IC_AUTH_ERR) || (Ic_Err_Reg == IC_EXAUTH_ERR) || (Ic_Err_Reg == IC_MAC_ERR)|| (Ic_Err_Reg == IC_KEY_ERR) || (Ic_Err_Reg == IC_IC_RST_ERR) )
    {
		api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD), (BYTE *)&dwData );
		dwData++;
		api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD), (BYTE *)&dwData );
    }

}
//-----------------------------------------------
//��������: �����û������кźͿ�����־
//
//����:  		Type[in] �û������ͣ�1 ������ 3 ���� 0 Զ�̿��� 
//
//����ֵ:		��
//		   
//��ע:
//-----------------------------------------------								
static void UpdateUsercardSerial( BYTE Type )
{
	BYTE Flag;

	if( Type == 1)					//������
	{
		Flag = eOpenAccount;                //���ؿ�����־
		api_WritePrePayPara( eLocalAccountFlag, (BYTE *)&Flag );
		api_WritePrePayPara(eSerialNo, SafeAuPara.CardRst8B);
	}
	else if( Type == 3)				//����
	{
		Flag = eOpenAccount;
		api_WritePrePayPara( eLocalAccountFlag, (BYTE *)&Flag );
		api_WritePrePayPara(eSerialNo, SafeAuPara.CardRst8B);
	}
	else if( Type == 0)				//Զ�̿���	
	{
		Flag = eOpenAccount;                //Զ�̿�����־
		api_WritePrePayPara( eRemoteAccountFlag, (BYTE *)&Flag );
	}

}

//---------------------------------------------------------------
//��������: �ж��л�ʱ��Ϸ��� - ��ʱ�ŵ�����
//
//����: 	Buf[in]  ������ʱ��
//                   
//����ֵ:   
// 		   0x00 - ȫ0
// 		   0x01 - ʱ�����
//		   0x02 - ʱ����ȷ
// 		   0x03 - ȫ9
//
//��ע:   
//---------------------------------------------------------------
BYTE CheckSwitchTime( BYTE *Buf )
{
	BYTE i, result;

	if( lib_IsAllAssignNum( Buf, 0x00, 5 ) == TRUE )
	{
		return 0x00;//�Ϸ�����ʱ��(ȫ0x00)
	}
	
	for( i = 0; i < 5; i++ )
	{
		if( Buf[i] != 0x99 )
		{
			break;
		}
	}
	
	if( i < 5 )
	{
		if( api_CheckClockBCD( 1, 5+0x80, Buf ) == TRUE )
		{
			return 0x02;
		}
		else
		{
			return 0x01;		   
		}
	}
	else
	{
		return 0x03;		//�Ϸ�����ʱ��(ȫ0x99)
	}
}


//-----------------------------------------------
//��������: ��MACд������ESAM �еĲ���
//
//����:  		Ch[in]		00:�忨д�����ļ�������ļ�
//						01: �û�������ESAMǮ���ļ�
//						02:Ԥ�ÿ�����ESAMǮ���ļ�
//						03:����������Ϣ�ļ�
//			Type[in]	0x04--������Ϣ�ļ�����Ĳ�������
//						0x85--��ǰ�׵�۸���
//						0x86--�����׵�۸���
//						0x07--������Ϣ�ļ�
//			P2[in]		ƫ�Ƶ�ַ
//			LC[in]		����InData����
//			InData[in]	Data+Mac����Ϊ����
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		 
//��ע:698�汾ESAM ��645��Լд����(��������ǰ�ס�������)����,��������������ESAMǮ���ļ�
//-----------------------------------------------
static WORD WriteEsamWithMac(BYTE Ch, BYTE Type, BYTE P2, WORD LC, BYTE *InData)
{
	BOOL Result;
	BYTE CmdBuf[9];
	BYTE CmdLen;
	BYTE OutBuf[20];
	
	if( (Ch == 0x00 )&&( (Type!=0x84) && (Type!=0x85) && (Type!=0x86) &&(Type!=0x8F)) )
	{
		return 0x8000;
	}
	
	//MAC����--�����ж�MAC����
	
	if(Ch == 0x01)//�û�������ESAMǮ���ļ�
	{	
		CmdBuf[0] = 0x80;
		CmdBuf[1] = 0x42;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if(Ch == 0x02)//Ԥ�ÿ�����ESAMǮ���ļ�
	{	
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x3E;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if( Ch ==0x00 )//�忨д�����ļ�������ļ�
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC;
		CmdLen=6;
	}
	else if( Ch == 0x03 )//����������Ϣ�ļ�
	{
		CmdBuf[0] = 0x80;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = 0x00;
		CmdBuf[3] = Type;
		CmdBuf[4] = LC>>8;
		CmdBuf[5] = LC+3;
		CmdBuf[6] = 0x00;
		CmdBuf[7] = P2>>8;
		CmdBuf[8] = P2;
		CmdLen=9;	
	}
	else//Ŀǰ���ţ�����Զ�����õõ�������ò�������ɾ�������
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2a;
		CmdBuf[2] = Type;
		CmdBuf[3] = P2>>8;
		CmdBuf[4] = P2;
		CmdBuf[5] = LC>>8;
		CmdBuf[6] = LC;
		CmdLen=7;
	}	
	
	Result = Link_ESAM_TxRx(CmdLen,CmdBuf,LC,InData,OutBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	return 1;
}

//-----------------------------------------------
//��������: ϵͳ�ڲ���֤
//
//����:  		��
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD InterAuth( void )
{
	WORD Result;
	BYTE Buf[32];
	BYTE K1[10];
	BYTE K2[8];
 
	Result = api_ReadEsamRandom(0x08,Buf);//��ESAMȡ8�ֽ������
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR  ;	//����esam����err31
	}

	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x01, 0x00, 0x10);//ȡesam���ܽ��K1
	memcpy( Buf+6, SafeAuPara.CardRst8B, 8 );//��ӷ�ɢ����
	memcpy( Buf+14, SafeAuPara.EsamRand, 8 );//��������1	
	Result = Link_ESAM_TxRx(6,Buf,16,Buf+6,K1);
	if( Result != TRUE )
	{
		return IC_AUTH_ERR;  //�����֤����(ͨѶ�ɹ������Ĳ�ƥ��)err32
	}
	 	
	Result = Link_Card_TxRx(0x00, 0x88, 0x00, 0x01, 0x08, WRITE,SafeAuPara.EsamRand );//ȡ��Ƭ���ܽ��K2
	if( Result != TRUE )
	{
		return IC_NOIN_ERR; //������ƬͨѶ����(��д��Ƭʧ��)��err35
	}
	Result = Link_Card_TxRx(0x00, 0xC0, 0x00, 0x00, 0x08, READ,K2 );
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}
	
	if( memcmp( K1+2, K2, 8) != 0 ) 
	{
		return IC_AUTH_ERR;//�����֤����(ͨ�ųɹ������Ĳ�ƥ��);err32
	}
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��Ƭ�ⲿ��֤
//
//����:  		��
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD CardExtAuth( void )
{
	WORD Result;
	BYTE CmdBuf[26];
	BYTE K3[10];

	Result = ReadRandomCARD( 8, CmdBuf+14 );			//�ӿ�Ƭȡ8�ֽ������
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}

	api_ProcPrePayCommhead( CmdBuf, 0x80, 0x08, 0x08, 0x02, 0x00, 0x10);
	memcpy( CmdBuf+6, SafeAuPara.CardRst8B, 8 );					//��ӷ�ɢ����
	 	
	Result = Link_ESAM_TxRx(6,CmdBuf,16,CmdBuf+6,K3);
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR;
	}
	
	Result = Link_Card_TxRx(0x00, 0x82, 0x00, 0x02, 0x08, WRITE,K3+2 );//��Ƭ���ⲿ��֤
	if( Result != TRUE )
	{
		return IC_EXAUTH_ERR;	
	}

	return TRUE;
}
//-----------------------------------------------
//��������: ���ݽ����պϷ����ж�:������Ҫ�ж�����,����ֵ��ȫ0����4�����ݽ�����ȫ����Ч����������
//
//����:  		type  �û�����Ƭ����  / FF:Ԥ�ÿ�
//
//����ֵ:		TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
static BYTE Judge_Ladder_Data(BYTE Type)
{
	BYTE Result;
	BYTE i;
	BYTE CurLadderBuf[LADDER_BUF_LENGTH];
	BYTE BakLadderBuf[LADDER_BUF_LENGTH];
	BYTE TmpFlag;

	if((Type == BUY_CARD)||(Type == LOST_CARD))
	{
		TmpFlag = (UpdateFlag&0x7A);
	}
	else
	{
		TmpFlag = UpdateFlag;
	}
	
	//���ݲ������±�־λbit2����Ƭ�����ļ�1�����±��ڵ�ǰ�׽��ݵ���+���ݵ�۲���
	if( TmpFlag & UPDATE_CUR_LADDER )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 132, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, CurLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
		if( lib_IsMultiBcd( CurLadderBuf, 52 ) == FALSE) 
		{	
			return FALSE; //���ݵ����ͽ��ݵ�۷Ƿ�
		}	
		for( i=0; i<4; i++ ) 
		{	
			lib_InverseData(CurLadderBuf+52+3*i,3);//��������ֽڽ���˳��
		}
		if( (lib_IsAllAssignNum( CurLadderBuf, 0x00,24 )==FALSE) 
			&& (api_JudgeYearClock( CurLadderBuf+52 ) == FALSE) ) 
		{	
			return FALSE; //����ֵ��ȫ0����4�����ݽ�����ȫ����Ч����������
		}
	}
	
	//���ݲ������±�־λbit3����Ƭ�����ļ�2�����±��ڱ����׽��ݵ���+���ݵ�۲���
	if( TmpFlag & UPDATE_BAK_LADDER )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 132, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, BakLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
		if( lib_IsMultiBcd( BakLadderBuf, 52 ) == FALSE ) 
		{	
			return FALSE; //���ݵ����ͽ��ݵ�۷Ƿ�
		}	
		for( i=0; i<4; i++ ) 
		{	
			lib_InverseData(BakLadderBuf+52+3*i,3);//��������ֽڽ���˳��
		}
		if( (lib_IsAllAssignNum( BakLadderBuf, 0x00,24 )==FALSE) 
			&& (api_JudgeYearClock( BakLadderBuf+52 ) == FALSE) ) 
		{	
			return FALSE; //����ֵ��ȫ0����4�����ݽ�����ȫ����Ч����������
		}
	}

	return TRUE;
}

//-----------------------------------------------
//��������: ���ݲ������±�־λ�ӿ���һ�ζ�ȡ�������������ʵ�ۡ����ݵ��
//
//����:  		��
//
//����ֵ:		TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
static WORD ReadDataFromCardWithMac( TCardParaUpdate *pCardPara )
{
	WORD Result;
	
	//��MAC�����������±�־λ
	Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 5, ESAM_PARA_FILE, 5, UPDATE_FLAG_LENGTH, pCardPara->FlagBuf );
	if( Result != TRUE ) 
	{
		return FALSE;
	}
	
	//���ݲ������±�־λbit7��д�������1��2+��ѹ���������������+MAC
	if( UpdateFlag & UPDATE_PARA_FILE )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 16, ESAM_PARA_FILE, 16, PARA_BUF_LENGTH, pCardPara->ParaBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	
	//���ݲ������±�־λbit0����Ƭ�����ļ�1�����±��ڵ�ǰ�׷��ʵ�۲���
	if( UpdateFlag & UPDATE_CUR_RATE )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 0, ESAM_CUR_RATE_FILE, 0, RATE_BUF_LENGTH, pCardPara->CurRateBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}	
	}
	
	//���ݲ������±�־λbit1����Ƭ�����ļ�2�����±��ڱ����׷��ʵ�۲���
	if( UpdateFlag & UPDATE_BAK_RATE )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 0, ESAM_BAK_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->BakRateBuf );
		if( Result != TRUE )  
		{
			return FALSE;
		}
		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 10,  ESAM_PARA_FILE, 10, TIME_BUF_LENGTH, pCardPara->BakRateTimeBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	
	//���ݲ������±�־λbit2����Ƭ�����ļ�1�����±��ڵ�ǰ�׽��ݵ���+���ݵ�۲���
	if( UpdateFlag & UPDATE_CUR_LADDER )
	{		
		Result = ReadBinWithMacCard( 0x01, USER_CARD_CUR_RATE_FILE, 132, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->CurLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}		
	}
	
	//���ݲ������±�־λbit3����Ƭ�����ļ�2�����±��ڱ����׽��ݵ���+���ݵ�۲���
	if( UpdateFlag & UPDATE_BAK_LADDER )
	{
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 132, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->BakLadderBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}		
		
		//����������ʱ��
		Result = ReadBinWithMacCard( 0x01, USER_CARD_BAK_RATE_FILE, 196, ESAM_BAK_RATE_FILE, 196, TIME_BUF_LENGTH, pCardPara->BakLadderTimeBuf );
		if( Result != TRUE ) 
		{
			return FALSE;
		}
	}
	return TRUE;
}

//---------------------------------------------------------------
//��������: ��MAC���ò�����Ϣ��д��ESAM��EEPROM��
//
//����:
//		MacMode[in] - �Ƿ��MAC
// 			NO_MAC   -- ����MAC ֱ��дEEPROM
// 			WITH_MAC -- ��MAC ��дESAM ��дEEPROM
//      Type[in] - ������������ ���eSetInforParaType
//      pBuf[in] - ��������
//      Len - ���ݳ��ȣ�����MAC��
//
//����ֵ:
//
//��ע:  �������ݺ�ESAM��ʽ��Ӧ ���ģʽ.
//		�� MacMode == WITH_MAC���򽫲���д��ESAM��ͬʱд��EEPROM
//		�� MacMode == NO_MAC����ֻд��EEPROM
//---------------------------------------------------------------
static BOOL SetSingleParaWithMac( BYTE MacMode, eSetInforParaType Type, BYTE *pBuf, BYTE Len )
{
	WORD 			Result;
	BYTE            File, Offset, ParaType,Buf[sizeof(TSwitchTimePara)];
    TFourByteUnion  TmpData;
    TRealTimer      TmpRealTimer;
	//������Ϣ�ļ�������������ESAM�ļ��е���ʼλ��
	BYTE ParaFileOffset[8] =
	{
		10,     //eSetRateSwitchTime 	���׷�ʱ�����л�ʱ��
		196,    //eSetLadderSwitchTime	���׽����л�ʱ��
		16,     //eSetAlarmLimit1		�������1
		20,     //eSetAlarmLimit2		�������2
		24,     //eSetCTCoe				�������
		27,     //eSetPTCoe				��ѹ���
		30,     //eSetMeterNo			���
		36      //eSetCustomCode		�ͻ����
	}; 
    
    TmpData.d = 0;
    
	//�ж���������Ϸ���
	if( (Type > eSetCustomCode) || (Len > 6 ) )
	{
		return FALSE;
	}

	//������д��ESAM��
	if( MacMode == WITH_MAC )
	{
		if( Type == eSetLadderSwitchTime )
		{
			File = ESAM_BAK_RATE_FILE;
		}
		else
		{
			File =  ESAM_PARA_FILE;
		}
		Offset = ParaFileOffset[Type];
		
		//������д��ESAM��
		Result = WriteEsamWithMac( 0x00, File, Offset, Len + 4, pBuf );
		if( Result == 0x8000 )
		{
			return FALSE;
		}
	}
	
	//���ݸ�ʽ��ESAM��Ӧ ESAM�Ǵ��ģʽ ������Ҫ��������
	lib_InverseData( pBuf, Len );

    switch( Type )
    {
        case eSetRateSwitchTime:
        case eSetLadderSwitchTime:
            //�ж��л�ʱ���Ƿ�Ϸ�
            if( CheckSwitchTime( pBuf ) == 0x01 )
            {
                return FALSE;
            }

            //����˳��������ʱ��
			api_ConvertYYMMDDhhmm_TRealTimer( &TmpRealTimer, pBuf );
            if( Type == eSetRateSwitchTime )
            {
                ParaType = 2;
            }
            else
            {
                ParaType = 3;
            }
			
			if( api_ReadAndWriteSwitchTime( WRITE, ParaType, (BYTE *)&TmpRealTimer ) == FALSE )
			{
				return FALSE;
			}
            break;
        case eSetAlarmLimit1:
        case eSetAlarmLimit2:
        case eSetCTCoe:
        case eSetPTCoe:
            if( lib_IsMultiBcd( pBuf, Len ) == FALSE )
            {
                return FALSE;
            }
            if( Len > sizeof(DWORD) )
			{
				return FALSE;
			}
            memcpy( TmpData.b, pBuf, Len );
            TmpData.d = lib_DWBCDToBin( TmpData.d );
            
            ParaType = ParaFileEnum[Type - eSetAlarmLimit1];
			if( api_WritePrePayPara( (ePrePayParaType)ParaType, TmpData.b ) == FALSE )
			{
				return FALSE;
			}
            break;
        case eSetMeterNo:
        case eSetCustomCode: 
            if( lib_IsMultiBcd( pBuf, Len ) == FALSE )
            {
                return FALSE;
            }
            lib_InverseData( pBuf, 6 );
            if( Type == eSetMeterNo )
            {
                ParaType = eMeterMeterNo;
            }
            else
            {
                ParaType = eMeterCustomCode;
            }
			if( api_ProcMeterTypePara( WRITE, ParaType, pBuf ) == FALSE )
			{
				return FALSE;
			} 
            break;
        default:
            return FALSE;
    }
    
    return TRUE;
}

//---------------------------------------------------------------
//��������: ���²�����Ϣ�ļ�����
//
//����:
//		InBuf[in] -- ��������
//
//����ֵ:
//
//��ע: ���ڿ��ͽ����ն˸��Ĳ�����Ϣ�ļ����������1���������2��PT��CT��
//---------------------------------------------------------------
static BOOL SetAllParaWithMac( BYTE *InBuf )
{
    BYTE i, Offset;
    BYTE ParaLen[4] = { 4, 4, 3, 3, };
    eSetInforParaType ParaAddr[4] = { eSetAlarmLimit1, eSetAlarmLimit2, eSetCTCoe, eSetPTCoe };
    TFourByteUnion ParaData;
	WORD Result;
    
    Offset = 0;
	
	Result = WriteEsamWithMac( 0x00, ESAM_PARA_FILE, 16, PARA_BUF_LENGTH, InBuf );
	if( Result == 0x8000 )
	{
		return FALSE;
	}
	
	for( i = 0; i < 4; i++ )
    {
		ParaData.d = 0;
		memcpy( ParaData.b, InBuf + Offset, ParaLen[i] );
		
		if( SetSingleParaWithMac( NO_MAC, ParaAddr[i], ParaData.b, ParaLen[i] ) != TRUE )
		{
			return FALSE;
		}
		Offset += ParaLen[i];
    }
	
	return TRUE;
}

//---------------------------------------------------------------
//��������: ��MAC���õ�ۣ�д��ESAM��EEPROM��
//
//����:
//			MacMode[in] - �Ƿ��MAC
// 				NO_MAC   -- ����MAC ֱ��дEEPROM
// 				WITH_MAC -- ��MAC ��дESAM ��дEEPROM
//          Type[in] - ���õ������
//              eCurRate             ��ǰ�׷��ʵ��
//	            eBackRate            �����׷��ʵ��
//              eCurLadder1          ��ǰ�׵�һ�Ž��ݵ��
//	            eCurLadder2          ��ǰ�׵ڶ��Ž��ݵ��
//	            eBackLadder1         �����׵�һ�Ž��ݵ��
//	            eBackLadder2         �����׵ڶ��Ž��ݵ��
//          pBuf[in] - �������(BCD) + MAC(�Ѿ�������)
//			Len  - ���ݳ��ȣ�����MAC��
//����ֵ:  TRUE/FALSE
//
//��ע:
//---------------------------------------------------------------
static BOOL SetPriceWithMac( BYTE MacMode, ePriceType Type, BYTE *pBuf, BYTE Len )
{
    BYTE i;
    BYTE File_Name;
    BYTE File_Offset;
    ePrePayParaType SetPriceNum;
    TFourByteUnion Tmpbuf;
	
	//������Ч�Լ��
	if( Type > eBackLadder )
	{
		return FALSE;
	}

    switch( Type )
    {
        case eCurRate://��ǰ�׷��ʵ��
			if( (Len / 4) > MAX_RATE_PRICE  )
            {
                return FALSE;
            }
            File_Name = ESAM_CUR_RATE_FILE;
            File_Offset = 4;
            SetPriceNum = eCurRateTable; //���ڵ���EEPROM����
            break;
        case eBackRate://�����׷��ʵ��
			if( (Len / 4) > MAX_RATE_PRICE  )
            {
                return FALSE;
            }
            File_Name = ESAM_BAK_RATE_FILE;
            File_Offset = 4;
            SetPriceNum = eBackupRateTable; //���ڵ���EEPROM����
            break;
        case eCurLadder://��ǰ�׽��ݵ��
            File_Name = ESAM_CUR_RATE_FILE;
            File_Offset = 52;
            SetPriceNum = eCurLadderTable;
            break;
        case eBackLadder://�����׽��ݵ��
            File_Name = ESAM_BAK_RATE_FILE;
            File_Offset = 52;
            SetPriceNum = eBackupLadderTable;
            break;
        default:
            return FALSE;
            break;
    }
    
    //��������õ�ESAM��----Զ������һ�����ʹ�ã�698������ʱ�ò������
	//if( MacMode == WITH_MAC )
	{
		//memcpy( Ic_Comm_RxBuf, pBuf, Len + 4 );
		//if( WriteBinWithMac( ESAM, File_Name, File_Offset, Len + 4 ) == FALSE )
		{
		//	return FALSE;
		}
	}

    //���ʵ��ת��ΪHEX
    if( (Type == eCurRate)||(Type == eBackRate) )
    {
        for( i=0; i<(Len/4); i++ ) 
        {
            lib_InverseData( &pBuf[4 * i], 4 );
            memcpy( Tmpbuf.b, &pBuf[4 * i], 4 );
            Tmpbuf.d = lib_DWBCDToBin( Tmpbuf.d );
            memcpy( &pBuf[4 * i], Tmpbuf.b, 4 );
        }
    }
    else//���ݵ��ת��ΪHEX
    {
        for( i=0; i<13; i++ ) 
		{
            lib_InverseData( &pBuf[4 * i], 4 );
            memcpy( Tmpbuf.b, &pBuf[4 * i], 4 );
            Tmpbuf.d = lib_DWBCDToBin( Tmpbuf.d );
            memcpy( &pBuf[4 * i], Tmpbuf.b, 4 );
		}
		for( i=0; i<MAX_YEAR_BILL; i++ ) 
		{
            memcpy( Tmpbuf.b, &pBuf[52 + 3 * i], 3 );
			//�����ý��ݽ�������Ҫ�ж��Ƿ���BCD���ܽ���ת�� ����0xFF��ת������ ���ڸ�@@@@��������
			lib_MultipleBBCDToBin(Tmpbuf.b, &pBuf[52 + 3 * i], 0x03 );
		}
	}
    
    //��������õ�EEPROM��
    if( api_WritePrePayPara( SetPriceNum, pBuf ) == FALSE )
    {
        return FALSE;
    }
    
    return TRUE;
}


//-----------------------------------------------
//��������: (��\�����ն�)���ʱ����ݱ��̼�¼
//
//����: 
//		TYPE  -- �������±�־λ
// 			UPDATE_CUR_RATE    -- ��ǰ�׷���   
//          UPDATE_BAK_RATE    -- �����׷���   
//          UPDATE_CUR_LADDER  -- ��ǰ�׽���   
//          UPDATE_BAK_LADDER  -- �����׽���
//
//����ֵ:		��
//		   
//��ע:			
//-----------------------------------------------
static void ProcRateLadderRecord( BYTE ParaUpdateFlag ) //��������û���ƣ���Ҫ�������
{	
	//ע�⣬�����12��34���ܺϲ�������ֱ�д!!!BUY BTC AND HOLD ,YOU'LL BE RICH
	if( (ParaUpdateFlag&UPDATE_CUR_RATE) != 0x00 ) //���ʱ��̼�¼--��ǰ�׷��ʵ��
	{		
		api_WritePreProgramData( 0,0x00221840 );//40182200�������ǰ��ǰ�׷��ʵ�� 
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //���ܱ���ʲ��������¼�
	}
	
	if( (ParaUpdateFlag&UPDATE_BAK_RATE) != 0x00 ) //���ʱ��̼�¼--�����׷��ʵ��
	{
		api_WritePreProgramData( 0,0x00221940 );//40192200�������ǰ�����׷��ʵ�� 
		api_SavePrgOperationRecord( ePRG_TARIFF_TABLE_NO ); //���ܱ���ʲ��������¼�
	}
	
	if( (ParaUpdateFlag&UPDATE_CUR_LADDER) != 0x00 ) //���ݱ��̼�¼--��ǰ�׽��ݵ��
	{
		api_WritePreProgramData( 0,0x00221A40 );//401A2200�������ǰ��ǰ�׽��ݵ�� 
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //���ܱ���ݱ����¼�
	}
	
	if( (ParaUpdateFlag&UPDATE_BAK_LADDER) != 0x00 ) //���ݱ��̼�¼--�����׽��ݵ��
	{
		api_WritePreProgramData( 0,0x00221B40 );//401B2200�������ǰ�����׽��ݵ�� 
		api_SavePrgOperationRecord( ePRG_LADDER_TABLE_NO ); //���ܱ���ݱ����¼�
	}
}


//---------------------------------------------------------------
//��������: ������һ�����
//
//����:
//		OpType[in] -- ��������
// 			CARD_MODE      -- ���۲忨���ò���
//          TERMINAL_MODE  -- �����ն˲忨���ò���
//		inCardType[in] -- ������
// 			PRESET_CARD_COMMAND -- Ԥ�ÿ�
// 			USER_CARD_COMMAND -- �û���
// 		UpdateFlag  -- �������±�־
// 			UPDATE_PARA_FILE   -- ������Ϣ�ļ�
//          UPDATE_CUR_RATE    -- ��ǰ�׷���
//          UPDATE_BAK_RATE    -- �����׷���
//          UPDATE_CUR_LADDER  -- ��ǰ�׽���
//          UPDATE_BAK_LADDER  -- �����׽���
// 		InBuf[in]
// 			��������
//����ֵ:
//
//��ע: �����ն������ļ���Ҫ�����ļ���MACд�룬���ڽ����ն˺�������
//---------------------------------------------------------------
static BOOL CardSetFirstData( BYTE OpType, BYTE inCardType, BYTE inUpdateFlag, TCardParaUpdate *pCardPara )
{
	BOOL Result;
	BYTE Buf[10];
	TBackupYearBill YearBillBuf;
	
	Result = TRUE;
	
	if( (inCardType != PRESET_CARD_COMMAND)&&(inCardType != USER_CARD_COMMAND) )
	{
		return FALSE;
	}
	
	//��MACд�������±�־λ
	Result = WriteEsamWithMac( 0x00, ESAM_PARA_FILE, 5, UPDATE_FLAG_LENGTH, pCardPara->FlagBuf );
	if( Result &0x8000 ) 
	{
		return FALSE;
	}
	
	//���ݲ������±�־λbit7��д����+MAC
	if( (inUpdateFlag&UPDATE_PARA_FILE) != 0 )
	{
		//�������1��2+��ѹ���������������+MAC
		if( SetAllParaWithMac( pCardPara->ParaBuf ) == FALSE )
		{
			return FALSE;
		}
	}
	
	//���ݲ������±�־λbit0�����±��ڵ�ǰ�׷��ʵ�۲���
	if( (inUpdateFlag&UPDATE_CUR_RATE) != 0 )
	{
		//�忨���÷��ʵ�� �����ͷ��ʼ��ȡ���� ���ܴӷ��ʵ�ۿ�ʼ ���򲻳ɹ� ������Ҫ����дESAM
		Result = WriteEsamWithMac( 0x00, ESAM_CUR_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->CurRateBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//�û�������ʱ��¼���ʣ����ݱ�̼�¼ ���������һ�� �����ǰ�׺ͱ�����ͬʱ���� �ȼ�¼ �����-yxk
		if( inCardType == USER_CARD_COMMAND ) //ֱ�Ӳ忨�ͽ����ն˲忨
		{
			ProcRateLadderRecord( UPDATE_CUR_RATE );
		}

		//��ͷ��ʼ��ȡ ��Ҫȥ��4���ֽڵ�ͷ�����ݲ��Ƿ��ʵ��
		memmove( pCardPara->CurRateBuf, pCardPara->CurRateBuf + 4, RATE_BUF_LENGTH - 8 );
		
		if( SetPriceWithMac( NO_MAC, eCurRate, pCardPara->CurRateBuf, RATE_BUF_LENGTH - 8 ) == FALSE )
		{
			return FALSE;
		}
	}
	
	//���ݲ������±�־λbit1�����±��ڱ����׷��ʵ�۲���
	if( (inUpdateFlag&UPDATE_BAK_RATE) != 0 )
	{
		Result = WriteEsamWithMac( 0x00, ESAM_BAK_RATE_FILE, 0,  RATE_BUF_LENGTH, pCardPara->BakRateBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//�û�������ʱ��¼���ʣ����ݱ�̼�¼ ���������һ�� �����ǰ�׺ͱ�����ͬʱ���� �ȼ�¼ �����-yxk
		if( inCardType == USER_CARD_COMMAND ) //ֱ�Ӳ忨�ͽ����ն˲忨
		{
			ProcRateLadderRecord( UPDATE_BAK_RATE );
		}
		//��ͷ��ʼ��ȡ ��Ҫȥ��4���ֽڵ�ͷ�����ݲ��Ƿ��ʵ��
		memmove( pCardPara->BakRateBuf, pCardPara->BakRateBuf + 4, RATE_BUF_LENGTH - 8 );

		Result &= SetSingleParaWithMac( WITH_MAC, eSetRateSwitchTime, pCardPara->BakRateTimeBuf, TIME_BUF_LENGTH - 4 ); //���ݳ���û��MAC
		Result &= SetPriceWithMac( NO_MAC, eBackRate, pCardPara->BakRateBuf, RATE_BUF_LENGTH - 8 );
		if( Result != TRUE )
		{
			return FALSE;
		}
	}
	
	//���ݲ������±�־λbit2����Ƭ�����ļ�1�����±��ڵ�ǰ�׽��ݵ���+���ݵ��+������ղ���
	if( (inUpdateFlag&UPDATE_CUR_LADDER) != 0 )
	{		
		Result = WriteEsamWithMac( 0x00, ESAM_CUR_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->CurLadderBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//�û�������ʱ��¼���ʣ����ݱ�̼�¼ ���������һ�� �����ǰ�׺ͱ�����ͬʱ���� �ȼ�¼ �����-yxk
		if( inCardType == USER_CARD_COMMAND ) //ֱ�Ӳ忨�ͽ����ն˲忨
		{
			ProcRateLadderRecord( UPDATE_CUR_LADDER );
		}
		
		Result &= SetPriceWithMac( NO_MAC, eCurLadder, pCardPara->CurLadderBuf, LADDER_BUF_LENGTH - 4 );

		if( Result != TRUE  )
		{
			return FALSE;
		}
		
		if( inCardType == PRESET_CARD_COMMAND )
		{
			//��Ԥ�ÿ����µ�ǰ�׽��ݣ��ȸ��ݵ�ǰ�õĽ�����ֵ�ж�һ�µ�ǰ�Ľ���ģʽ�����������������
			InitJudgeBillMode();
			
			memcpy( (void *)&YearBillBuf, pCardPara->CurLadderBuf + 52, 3 * MAX_YEAR_BILL ); //���ݿ��е�ǰ��������գ������ж������Ԥ�ÿ����浽EEPROM
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&YearBillBuf.YearBill );
		}
		else
		{
			//�û����޸ĵ�ǰ�׽�����Ҫ�жϽ���ģʽ�л�
			api_SetUpdatePriceFlag( eChangeCurLadderPrice ); 
		}
	}
	
	//���ݲ������±�־λbit3����Ƭ�����ļ�2�����±��ڱ����׽��ݵ���+���ݵ��+�������+����ʱ�����
	if( (inUpdateFlag&UPDATE_BAK_LADDER) != 0 )
	{					
		Result = WriteEsamWithMac( 0x00, ESAM_BAK_RATE_FILE, 132, LADDER_BUF_LENGTH, pCardPara->BakLadderBuf );
		if( Result &0x8000 ) 
		{
			return FALSE;
		}

		//�û�������ʱ��¼���ʣ����ݱ�̼�¼ ���������һ�� �����ǰ�׺ͱ�����ͬʱ���� �ȼ�¼ �����-yxk
		if( inCardType == USER_CARD_COMMAND ) //ֱ�Ӳ忨�ͽ����ն˲忨
		{
			ProcRateLadderRecord( UPDATE_BAK_LADDER );
		}
		
		Result &= SetPriceWithMac( NO_MAC, eBackLadder, pCardPara->BakLadderBuf, LADDER_BUF_LENGTH - 4 );
		
		//�����л�ʱ��
		if( Result == TRUE )
		{
			Result &= SetSingleParaWithMac( WITH_MAC, eSetLadderSwitchTime, pCardPara->BakLadderTimeBuf, TIME_BUF_LENGTH - 4 );
		}
		if( Result != TRUE  )
		{
			return FALSE;
		}
	}
	
	
	//��̼�¼ ֻ��¼�忨���� �����ն˴��������ط��Ѿ���¼	
	if( (inCardType == USER_CARD_COMMAND)&&(inUpdateFlag != UPDATE_NO_PARA) ) //�û���
	{		
		Buf[0] = 0x99;				//��ʶ
		Buf[1] = 0x98;				//˽Կ
		Buf[2] = inCardType;            //�翨����
		Buf[3] = inUpdateFlag;      //���±�־
		//ʹ�ÿ�Ƭ���ò���ʱ������¼���¼�е����ݱ�ʶΪ 9998������Ƭ��ʽ�У������룫���±�־λ�������ߴ���Ϊ��Ƭ���кŵĵ� 4�ֽڡ�
		lib_ExchangeData( Buf+4, SafeAuPara.CardRst8B+4, 4);	//�����ߴ��루����ź�4�ֽڣ�
        api_SaveProgramRecord( EVENT_START, 0x51, Buf );
        api_SaveProgramRecord( EVENT_END, 0x51, Buf );
	}
	
	
	return TRUE;
}

//-----------------------------------------------
//��������:��MACд�û�����д�ļ�
//
//����:		��
//
//����ֵ:	�ɹ�/ʧ��
//
//��ע:
//
//-----------------------------------------------
static BYTE WriteCardReturnDataWithMac( void )
{
	BYTE Result;
	BYTE Buf[64];
	BYTE CmdBuf[6];
	BYTE DataBuf[22];
	BYTE Buf_EsamRunFile[ESAM_RUN_FILE_LENTH+4];			//������4�ֽڣ������������ڴ�
	BYTE Buf_Backup[50];                                    //����������Ϣ�ļ�
	BYTE CmdBufEsam[9];
	BYTE CmdEsamLen;
	TFourByteUnion td;

	api_ReadEsamData( ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_Backup );

	memset( Buf_EsamRunFile, 0x00, ESAM_RUN_FILE_LENTH );                   //�Ȱ�bufȫ����

	Buf_EsamRunFile[0] = 0x68;
	Buf_EsamRunFile[1] = 0x11;
	Buf_EsamRunFile[2] = 0x00;
	Buf_EsamRunFile[3] = 0x2b;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		td.d = CurMoneyBag.Money.Remain.ll / 10000;                                               //ʣ����   23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //�������
		td.d = 0;                                                                                       //͸֧(BCD)
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	else
	{
		td.d = 0;                                                                                       //ʣ����	 23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //�������
		td.d = ((-1) * CurMoneyBag.Money.Remain.ll) / 10000;                                               //͸֧(BCD)
		td.d = lib_DWBinToBCD( td.d );
		//memcpy( Buf_EsamRunFile + 31, td.b, 4 );
		lib_ExchangeData(Buf_EsamRunFile + 31, td.b, 4);
	}
	//35-38--����Ĭ��Ϊ00
	api_ReadIllegalCardNum( td.b );                                                       //�Ƿ�������
	td.d = lib_DWBinToBCD( td.d );
	lib_InverseData( td.b, 4 );
	memcpy( Buf_EsamRunFile + 38, td.b, 4 );

	Buf_EsamRunFile[42] = (BYTE)lib_WBinToBCD( RealTimer.wYear );                       //��������Ҫ��дʱ��,Զ��ͨѶ��07����д
	Buf_EsamRunFile[43] = lib_BBinToBCD( RealTimer.Mon );                                 //������ʱ��
	Buf_EsamRunFile[44] = lib_BBinToBCD( RealTimer.Day );
	Buf_EsamRunFile[45] = lib_BBinToBCD( RealTimer.Hour );
	Buf_EsamRunFile[46] = lib_BBinToBCD( RealTimer.Min );

	td.d = PrePayPara.CTCoe;                                                                //��ѹ���  11--22��Ҫת��Ϊbcd������ȶ���Ҫ
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 5, td.b, 3 );
	td.d = PrePayPara.PTCoe;                                                                //�������
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 8, td.b, 3 );

	memcpy( Buf_EsamRunFile + 11, FPara1->MeterSnPara.MeterNo, 12 );                         //��ſͻ����

	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 2] = lib_CheckSum( Buf_EsamRunFile + 1, ESAM_RUN_FILE_LENTH - 3 ); //����CS
	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 1] = 0x16;

	if( memcmp( Buf_EsamRunFile, Buf_Backup, ESAM_RUN_FILE_LENTH ) != 0 ) //�ж�07�ļ������Ƿ����仯,����仯�˲Ÿ�д07�ļ�
	{
		if( WriteEsamWithMac( 0x03, ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_EsamRunFile ) != TRUE )
		{
			return FALSE;
		}
	}

	Result = ReadRandomCARD( 4, SafeAuPara.CardRand );  //����ȡ��Ƭ�����
	if( Result != TRUE )
	{
		return FALSE;
	}

	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2c;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x07;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x16;                       //������������ݵĳ���

	DataBuf[0] = 0x48;
	memcpy( DataBuf + 1, SafeAuPara.CardRst8B, 8 );
	memcpy( DataBuf + 9, SafeAuPara.CardRand, 4 );
	DataBuf[13] = 0x00;                     //P3
	DataBuf[14] = 0x00;                     //P3
	DataBuf[15] = 0x00;                     //LEN
	DataBuf[16] = ESAM_RUN_FILE_LENTH;      //LEN
	DataBuf[17] = 0x04;
	DataBuf[18] = 0xd6;
	DataBuf[19] = 0x85;
	DataBuf[20] = 0x00;                     //P2
	DataBuf[21] = ESAM_RUN_FILE_LENTH + 4;    //LC

	Result = Link_ESAM_TxRx( 6, CmdBuf, 22, DataBuf, Buf );
	if( Result != TRUE )
	{
		return FALSE;
	}

	memmove( Buf, Buf + 2, ESAM_RUN_FILE_LENTH + 4 );
	
	Result = Link_Card_TxRx( 0x04, 0xd6, USER_CARD_RETURN_WRITE_FILE, 0x00, ESAM_RUN_FILE_LENTH+4, WRITE, Buf );

	if( Result != TRUE )
	{
		if( (Err_Sw12[0] == 0x69)&&(Err_Sw12[1] == 0x89) )
		{
			Result = IC_MAC_ERR; //MACУ�����(6989)
		}
		else
		{
			Result = IC_NOIN_ERR; //������ƬͨѶ����(��д��Ƭʧ��);	err35
		}
	}

	return Result;

}

//-----------------------------------------------
//��������: Ԥ�ƿ�Ǯ������
//
//����:  		��
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD ProcPresetCardMoney(void)
{
	WORD Result;
	BYTE MoneyBuf[8];//MoneyBuf	4�ֽ�money+4�ֽ�MAC	
	
	//��MAC��ȡ��Ǯ���ļ������µ�ESAMǮ��--��ֵ���+MAC
	Result = ReadBinWithMacCard( 0x21, 0x82, 0x00, 0x00, 0x00, 4+4 , MoneyBuf);
	if( Result != TRUE )
	{
		return IC_NOIN_ERR;
	}
	
	//������ֻ�н���MAC�����Բ��жϴ�����--�·���ϵͳ����Ԥ�ÿ��������ֻ��Ϊ0						
	Result = WriteEsamWithMac( 0x02, 0x00, 0x00, 4+4, MoneyBuf);	
	if( Result !=1 ) 
	{
		return IC_ESAM_RDWR_ERR;		
	}
	
	//�幺���¼������͸֧���������,����ǰʣ����
	memset(FourByteNewBuyMoneyBag.Remain.b, 0x00, sizeof(FourByteNewBuyMoneyBag));	    //�幺���¼������͸֧���������,����ǰʣ����
    memset(FourByteOldBuyMoneyBag.Remain.b, 0x00, sizeof(FourByteOldBuyMoneyBag));	        //�幺���¼������͸֧���������,����ǰʣ����
	//================RAM�й���ʣ����Ԥ��ΪMoneyBuf������crcУ��								
	lib_ExchangeData(FourByteNewBuyMoneyBag.Remain.b,MoneyBuf,4);
	 
	Result = api_UpdataRemainMoney( eInitMoneyMode, FourByteNewBuyMoneyBag.Remain.d, 0 );	

	return OK_CARD_PROC;
}

//-----------------------------------------------
//��������: Ԥ�ÿ�����
//
//����:  		��
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD Proc_PresetCard(void)
{
	WORD Result;
	TCardParaUpdate CardPara;

	if( Judge_Ladder_Data( 0xff ) == FALSE )
	{
		return IC_FORMAT_ERR;
	}
	
	//Ԥ�ÿ�Ǯ������	
	Result = ProcPresetCardMoney();
	if( Result != OK_CARD_PROC ) 
	{
		return Result;
	}
	
	//��MAC��ȡ��ز�����RAM����������
	Result = ReadDataFromCardWithMac( &CardPara );
	if( Result != TRUE ) 
	{
		return IC_NOIN_ERR;
	}
	
	//��MAC������ز���
	Result = CardSetFirstData( CARD_MODE, PRESET_CARD_COMMAND, UpdateFlag, &CardPara );
	if( Result != TRUE )
	{
		return IC_ESAM_RDWR_ERR;	//����ESAM��
	}	

	return OK_CARD_PROC;
}

 //-----------------------------------------------
 //��������: �û���Ǯ������
 //  
 //����:  		UserCardMoney[in] �û���������Ϣ 
 //             InMoneyBuf[in]    ������(4)+�������(4)+MAC(4)
 //
 //����ֵ:		TRUE/��������
 //		   
 //��ע:   
 //-----------------------------------------------
static WORD Proc_UserCardMoney(TUserCardMoney	*UserCardMoney, BYTE *InMoneyBuf)
{
	TFourByteUnion Temp32;
	BYTE Buf[20],UserCardSerialBit;
	WORD Result;

	UserCardSerialBit = 0;
	if ( memcmp( SafeAuPara.CardRst8B, CardInfo.SerialNo, 8) != 0 )//�жϿ����к��Ƿ����
	{
        UserCardSerialBit = 1;
	}
	
	//================ ����ж� ===============
	if ( memcmp( FPara1->MeterSnPara.MeterNo, UserCardMoney->MeterNo, 6) != 0 ) 
	{
		return IC_ID_ERR;	//��Ų����
	}	
	
	//========== ��Ƭ��ż������ж� ===========

	if( (UserCardMoney->UserCardType!=OPEN_CARD) 
		&& (UserCardMoney->UserCardType!=BUY_CARD) 
		&& (UserCardMoney->UserCardType!=LOST_CARD) ) 
	{
		return IC_TYPE_ERR;			//�����ʹ���
	}

	if( (CardInfo.LocalFlag == eOpenAccount) || (CardInfo.RemoteFlag==eOpenAccount) )	//���ػ�Զ���ѿ���
	{
		Result = memcmp( FPara1->MeterSnPara.CustomCode, UserCardMoney->CustomCode, 6 );
		if( Result != 0 ) 			//�ѿ����ı��ͻ���Ų����
		{
			return IC_HUHAO_ERR;	//�ͻ���Ŵ�
		}
				
		if( ( UserCardMoney->UserCardType == BUY_CARD ) && ( UserCardSerialBit == 1 ) ) 
		{
			return IC_SERIAL_ERR;	//���翨����Ŵ�
		}
	}
	else							//δ����
	{
		if( UserCardMoney->UserCardType == BUY_CARD )
		{
			return IC_NOOPEN_ERR; 	//���翨����δ������
		}
		else if( UserCardMoney->UserCardType == LOST_CARD )
		{
			return IC_NOOPEN2_ERR; 	//��������δ������
		}
	}
	
	if( FourByteNewBuyMoneyBag.BuyTimes > 1 )
	{
		if( UserCardMoney->UserCardType == OPEN_CARD ) 
		{
			return IC_TIMES_ERR;	//���������(�������������ֻ����0��1)
		}
	}
	
	//======== �жϹ����������ʱ��ֵ ==========	
	Result = ReadEsamMoneyBag( Buf );                                 //��Ǯ���������
	if( Result != TRUE ) 
	{
		return IC_ESAM_RDWR_ERR;
	}
	FourByteOldBuyMoneyBag.Remain.l = (CurMoneyBag.Money.Remain.ll/10000);						//ʣ�ࡢ͸֧HEX�ŵ�������
	lib_ExchangeData( Temp32.b, Buf+4, 4 );										//Temp32�ݴ�esam������������������esamΪ׼
	
	//��Ƭ�������С�ڱ���ֵ 
	if( FourByteNewBuyMoneyBag.BuyTimes < Temp32.d )
	{	//�����ѿص��ܱ�ϵ�м�����׼����(��) ��������С�ڱ��ڹ������ ���濨���к�	
		if(( UserCardMoney->UserCardType == OPEN_CARD ) || ( UserCardMoney->UserCardType == LOST_CARD )) 
		{
			UpdateUsercardSerial( UserCardMoney->UserCardType );    //���±��ؿ�����־�������к�
		}
		UserCardMoney->BuyTimesFlag = 0xff;							//��Ƭ�������С�ڱ���ֵ 	
	}
	else
	{
		Temp32.d = FourByteNewBuyMoneyBag.BuyTimes-Temp32.d;			//��Ƭ������� - ����ֵ
		
		if( Temp32.d == 0 ) 					//����������
		{
			UserCardMoney->BuyTimesFlag = 0;	//����������
			if( Judge_Ladder_Data(UserCardMoney->UserCardType) == FALSE )
			{
				return IC_FORMAT_ERR;
			}
			
		}
		else if( Temp32.d == 1 ) 				//���������1
		{									
			UserCardMoney->BuyTimesFlag = 1;	//��������� 1
			
			//�����������������1����д��Ϣ��Ϊ��ʱ���·�д����ʾ����
			//�����ѿص��ܱ�ϵ�м�����׼����(��) ���п����������1 ��д��Ϊ�� ����д����
			if( UserCardMoney->ReturnFileStatus == 1 )
			{
				return OK_CARD_PROC;
			}
   			
			// ʣ��+��ֵ			
			if( (FourByteOldBuyMoneyBag.Remain.l+FourByteNewBuyMoneyBag.Remain.l) > 99999999L )//��Ӻ������(����999999.99Ԫ)
			{
				return IC_OVERMAX_ERR;
			}
			//�ж϶ڻ����
			if( LimitMoney.RegrateLimit.d )
			{
				if( (FourByteOldBuyMoneyBag.Remain.l+FourByteNewBuyMoneyBag.Remain.l) > (long)LimitMoney.RegrateLimit.d ) //L
				{
					return IC_OVERMAX_ERR;
				}
			}
			if( Judge_Ladder_Data(UserCardMoney->UserCardType) == FALSE )
			{
				return IC_FORMAT_ERR;
			}	
		}
		else 									//���������>=2
		{
			UserCardMoney->BuyTimesFlag = 2;	
		}
	}
	
	//----------------------------------------------------------------------------
   	if( UserCardMoney->BuyTimesFlag == 1 )
	{
		if( ((UserCardMoney->UserCardType==OPEN_CARD) && (UserCardMoney->ReturnFileStatus == 0)) 
			|| (UserCardMoney->UserCardType==BUY_CARD)
			|| (UserCardMoney->UserCardType==LOST_CARD) )
		{
			//Ǯ���ļ���ֵ
			Result = WriteEsamWithMac( 0x01, 0x00, 0x00, 8+4, InMoneyBuf);	//����Ǯ�����--��ֵ���+MAC				

			if(Result != TRUE) return IC_MAC_ERR;
			
			api_UpdataRemainMoney( eChargeMoneyMode, FourByteNewBuyMoneyBag.Remain.d, FourByteNewBuyMoneyBag.BuyTimes );
			
		}
	}   
	
	//�����������ڹ������С�ڱ��ڹ������  ǰ�洦���� ��������϶����л��ţ���˲����ٴ洢����
	//�����������ڹ�������ȱ��ڹ��������1���ҷ�д�ļ�Ϊ��
	//�����������ڹ�������ͱ��ڹ��������ȣ��ҷ�д�ļ�Ϊ��
	//���Ľ��ͣ�����δ��������Զ�̿����ĵ��ܱ��ڲ忪����ʱ�����ܱ���Ҫ�жϿ������ķ�д��Ϣ
	//�ļ��Ƿ�Ϊ�գ�ֻ��Ϊ��ʱ������������������ 
	if( UserCardMoney->UserCardType == OPEN_CARD )//������ ������ͺʹ����޹��ˣ���Ϊ����������������Ϊ0��1
	{
		if(UserCardMoney->ReturnFileStatus == 0)	//��дΪ�յĿ��������߲���,���濨��ż�������־
		{
			memcpy(Buf, UserCardMoney->CustomCode, 6+4);//����RAM����������
			Result = WriteEsamWithMac(0x00,0x8F,0x06,0x0A,Buf);//��MACд��ESAM��ز���
			if( Result&0x8000 ) 
			{
				return IC_ESAM_RDWR_ERR;	
			}
			//����ͻ���š����¿����кźͿ�����־	
			api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CustomCode ),GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CustomCode ), Buf );//����RAM����������		
			UpdateUsercardSerial( UserCardMoney->UserCardType );
		}
	}	
	
	//���������ڹ�������ͱ�����Ȼ��ߴ�1���ҷ�д�ļ�Ϊ��
	//���������޷�д�Ĳ��������濨���
	if( UserCardMoney->UserCardType == LOST_CARD )//����
	{
		if( ( !UserCardMoney->ReturnFileStatus ) && ( UserCardMoney->BuyTimesFlag<2 ) ) 
		{
			UpdateUsercardSerial( UserCardMoney->UserCardType );
		}
	}
		
	if( ( UserCardMoney->UserCardType==OPEN_CARD )||( UserCardMoney->UserCardType==LOST_CARD ) )
	{
		//������ȣ���д��Ϊ�գ�����Ų��ȣ�����.���ǵ������������ο��������ֻʶ������һ�β��Ŀ� �����ڷ�д��Ϊ�յ�ʱ���жϿ����кš�
		if( (UserCardMoney->ReturnFileStatus!=0) 
			&& (UserCardMoney->BuyTimesFlag==0) 
			&& (UserCardSerialBit==1) ) 
		{
			return IC_SERIAL_ERR;	
		}
	}
	
	return OK_CARD_PROC;
}

//-----------------------------------------------
//��������: �û�������
//  
//����:  		
//			Type[in]: 01����, 02��ֵ, 03����
//          InBuf[in]��������+�������+MAC1, �ͻ����+MAC2, ���(���ز忨)
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD Proc_UserCard( BYTE *InBuf )
{
	WORD Result;
	BYTE InMoneyBuf[12];
	BYTE Buf[8];
	TCardParaUpdate ParaBuf;
	TUserCardMoney	UserCardMoney;
	
	UserCardMoney.UserCardType = InBuf[0x2a];
	memcpy(UserCardMoney.MeterNo,InBuf+0x1e,6);
		
	//=========== CARD ����ⲿ��֤ ===========
	Result = CardExtAuth();
	if( Result != TRUE ) 
	{
		return Result;
	}	
	
	//=============== ����׼�� =============
	//============= ��д�ļ��ж� ===========
	Link_Card_TxRx( 0x00, 0xb0, USER_CARD_RETURN_WRITE_FILE, 0x00, 8 , READ, Buf );
	if( (Buf[0] == 0x68) || (Buf[5] != 0) || (Buf[6] != 0) || (Buf[7] != 0) ) //������������Ȳ�ȫΪ0
	{
		UserCardMoney.ReturnFileStatus = 1;	//��д�ļ���Ϊ��
	}
	else
	{
		UserCardMoney.ReturnFileStatus = 0;	//��д�ļ�Ϊ��
	}

	//============��MAC��ȡ����Ǯ���ļ� ===========
	ReadBinWithMacCard ( 0x31, 0x82, 0x00, 0x00, 0x00, 8+4 ,InMoneyBuf);//��ȡ������(4)+�������(4)+MAC(4)					
	lib_ExchangeData( FourByteNewBuyMoneyBag.Remain.b, InMoneyBuf, 4 );							//���ڹ�����HEX
	lib_ExchangeData( (BYTE *)&FourByteNewBuyMoneyBag.BuyTimes, InMoneyBuf+4, 4 );					//���ڹ������HEX			
	ReadBinWithMacCard( 0x01, USER_CARD_PARA_FILE, 0x24, 0x8F, 0x06, 6+4 ,(BYTE *)&UserCardMoney.CustomCode);//��ȡ����(6)+MAC(4)	

	//�û���Ǯ������
	Result = Proc_UserCardMoney(&UserCardMoney,InMoneyBuf);
	if( Result != OK_CARD_PROC ) 
	{
		return Result;
	}
		
	//����������д��Ϊ�գ����������1��ֻ��д������ 
	//�����ѿص��ܱ�ϵ�м�����׼����(��) ���п����������1 ��д��Ϊ�� ����д����
	if( (UserCardMoney.ReturnFileStatus==1) && (UserCardMoney.BuyTimesFlag==1) )
	{
	}
	else
	{	
		//���翨�Ͳ�����ֻ������±�����;
		if( UserCardMoney.UserCardType != OPEN_CARD )
		{
			UpdateFlag &= 0x7A;	
		} 	
		
		//(��д�ļ�Ϊ�գ��ҹ���������)  ���ߴ�1ʱ�ɸ��²���
		if( ((!UserCardMoney.ReturnFileStatus)&&(UserCardMoney.BuyTimesFlag==0)) || (UserCardMoney.BuyTimesFlag==1) ) 
		{		
			//��MAC��ȡ��ز���
			Result = ReadDataFromCardWithMac(&ParaBuf);
			if( Result != TRUE ) 
			{
				return IC_NOIN_ERR;
			}
	
			//����ESAM��EEPROM����
			Result = CardSetFirstData( CARD_MODE, USER_CARD_COMMAND, UpdateFlag, &ParaBuf );
			if( Result != TRUE )
			{
				return( IC_ESAM_RDWR_ERR );	//����ESAM��
			}	
		}
	}
	
	//================ ��д��Ƭ =============
	Result =WriteCardReturnDataWithMac();
	if( Result != TRUE ) 
	{
		return IC_NOIN_ERR;
	}
		
	if(UserCardMoney.BuyTimesFlag == 2) 
	{
		return IC_TIMES_ERR;			//���й�������ȱ��ڴ�2���ϣ���д��Ƭ����ʾ�����������
	}
	
	if((UserCardMoney.BuyTimesFlag == 1) && (UserCardMoney.ReturnFileStatus)) 
	{
		return IC_NONULL_ERR;			//���������1����д��Ϊ�գ����ط�д��Ϊ�մ���
	}
	
	return OK_CARD_PROC;
}


//-----------------------------------------------
//��������: ��Ƭ����
//  
//����:  		��
//
//����ֵ:		TRUE/��������
//		   
//��ע:
//-----------------------------------------------
static WORD Card_Esam_Proc( void )
{
	BYTE i, Length;
	BYTE Buf[46];
	WORD Result;
	
	CLEAR_WATCH_DOG;
	
	Ic_Err_Reg  = 0x00;
	UpdateFlag = 0x00;	
	memset( SafeAuPara.CardRst8B, 0xff, 8 );//��ѿص��ܱ����������� CPU �����кţ���¼�еĿ����к�Ӧ��ΪȫFF---�淶 7.13 c) 1)	

	//=========== ��Ƭ��λ ================
	for(i=0; i<3; i++)
	{
		//������λͬʱ��λESAM��SPI
		Result = Link_CardReset(Buf); 
		if( Result == TRUE ) 
		{
			break;
		}	
		api_Delayms(100);
		CLEAR_WATCH_DOG;
	}

	if( i == 0x03 )  
	{
		return IC_IC_RST_ERR; 	//��λCARDʧ��
	}
	
	//=========== ��λESAM ===============
	//Ϊ�˼��ٸ�λESAM�Ĵ��� ֻ�е�һ�鿨��λ�ɹ�����ܸ�λESAM ��ֹ����Ƿ���(��Ƭ��) ����ESAM��λ
	api_ResetEsamSpi(); 

	//=========== ��Ƭ��λ ================
	//��ΪCARD��Դ����ESAM��Դ ����λESAM��ͨ����ESAM����ʵ�ֵ� ���Կ�Ҳ����� ���Ը�λ��ESAM֮���ٸ�λ��һ��
	for(i=0; i<3; i++)
	{
		Result = Link_CardReset(Buf); 
		if( Result == TRUE ) 
		{
			break;
		}	
		api_Delayms(100);
		CLEAR_WATCH_DOG;
	}

	if( i == 0x03 )  
	{
		return IC_IC_RST_ERR; 	//��λCARDʧ��
	}
	memcpy( SafeAuPara.CardRst8B, Buf+5, 8);

	//�˴�Ҫ��ӽ������1�μ�¼�Ĵ���--songchen
	api_SaveProgramRecord( EVENT_END, 0x51, NULL );
	
	//=========== ѡ���ļ� ================
	Result = SelectFile( 0x3f, 0x00 );
	if(Result != TRUE)   
	{
		return IC_TYPE_ERR;
	}	
	Result = SelectFile(0xdf, 0x01 );
	if(Result != TRUE)   
	{
		return IC_TYPE_ERR;
	}			
	//=========== �жϿ�Ƭ���� ================
	Result = ReadBinFile( 0x81, 0x00, 0x04, Buf );
	if(Result != TRUE) 
	{	
		if( (Err_Sw12[0] == 0x6b)&&(Err_Sw12[1] == 00) )
		{
			return IC_NOFX_ERR;         //δ���еĿ�Ƭ
		}
		else
		{
			return IC_NOIN_ERR;         //��Ƭ��������
		}
	}	
	//��ô�ж���ʵ���ԣ����Ƿ��صĴ���ǡ�ô�0x02��ʼ������û����

	CardType = Buf[1];

	if( CardType == USER_CARD_COMMAND )
	{
		//������ô��У��쳣�忨������Ϣ�ּ�09--��Ƭδ��Ȩ����Կ�²��û�����˽Կ�²�Ԥ�ÿ����������05--�����֤ʧ��
		if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
		{
			return IC_KEY_ERR;
		}
		Length = UserCardLength;	 		//�û���
		//Ǯ���ۼ�������������Ϣ�е�ʣ����
		MeteringUpdateMoneybag();
	}
	else if( CardType == PRESET_CARD_COMMAND )
	{
		//������ô��У��쳣�忨������Ϣ�ּ�09--��Ƭδ��Ȩ����Կ�²��û�����˽Կ�²�Ԥ�ÿ����������05--�����֤ʧ��
		if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == FALSE)
		{
			return IC_KEY_ERR;
		}
		Length = PresetCardLength;			//Ԥ�ÿ�
	}
	else 
	{
		return IC_TYPE_ERR;
	}
		
	api_Delayms(5);

	//=========== ϵͳ�ڲ���֤=================
	Result = InterAuth();
	if( Result != TRUE ) //��ô�ж���ʵ���ԣ����Ƿ��صĴ���ǡ�ô�0x02��ʼ������û����
	{
		return Result;
	}
	
	Result = api_ReadEsamRandom( 4, Buf);		//��ESAMȡ4�ֽ�����������ڴ�mac��дǮ���ļ�
	if(Result != TRUE) 
	{	
		return IC_ESAM_RDWR_ERR;
	}
	//=========== �ж�ָ���ļ���ʽ=============
	Result = ReadBinFile(0x81, 0x00, Length+6, Buf );
	if( Result != TRUE )  
	{
		return IC_NOIN_ERR; 
	}

	Result = lib_CheckSum( Buf+1, Length+3);
	if( (Buf[0]!=0x68) || (Buf[Length+4]!=Result) || (Buf[Length+5]!=0x16) ) 
	{
		return IC_FORMAT_ERR;   	//��Ƭ�ļ���ʽ���Ϸ�
	}
		
	UpdateFlag = Buf[5];			//����������±�־λ				
	if( UpdateFlag == 0xff ) 
	{
		UpdateFlag = 0;				//��֤�±���ݾɿ�
	}
	//=============== ��Ƭ����=================	
	if( CardType == PRESET_CARD_COMMAND )//--------------Ԥ�ÿ�
	{
		Result = Proc_PresetCard();
		if( Result != OK_CARD_PROC ) 
		{
			return Result;
		}
	}//-----------�û���	
	else if( CardType == USER_CARD_COMMAND )
	{					
		Result = Proc_UserCard( Buf );
		if( Result != OK_CARD_PROC ) 
		{
			return Result;
		}
	}
	else 
	{
		return IC_TYPE_ERR;
	}
	
	return OK_CARD_PROC;
}

//-----------------------------------------------
//��������: �¿�Ƭ����
//  
//����:  		��
//
//����ֵ:		TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
static WORD SearchCard(void)
{
	WORD Result;
	static BYTE bDelayms = 0;
	Result = FALSE;

	if( CARD_IN_SLOT ) 
	{
		if( bDelayms < 10 )
		{
			bDelayms++;
		}
	}
	else
	{
		bDelayms = 0;
	}
	
	if( bDelayms >2 ) 
	{
		if( (CardCheck == 0) )
		{
			//���¿�Ƭ����
			CardCheck = 1;	
			Result = TRUE;		
		}
	}
	else
	{
		CardCheck = 0;	
	}
	return Result;
}
//-----------------------------------------------
//��������: ��ȡ�忨״̬��
//
//����: 	��
//                   
//����ֵ:   01 �ɹ�   02 ʧ�� 
//
//��ע:   
//-----------------------------------------------
void api_GetCardInsertStatus(BYTE *OutBuf)
{
	WORD InsertCardStaBuf;
	InsertCardStaBuf = InsertCardSta;
	InsertCardSta = 0;	//�忨״̬��Ҫ���ȡ���ϵ����δ֪ Bit1Bit0:00 δ֪��01�ɹ���10ʧ��
	lib_ExchangeData(OutBuf,(BYTE *)&InsertCardStaBuf,2);
}	
//-----------------------------------------------
//��������: �忨�������
//	
//����:		��
//
//����ֵ:		��
//		   
//��ע:
//-----------------------------------------------
void Card_Proc( void )
{
	WORD Result,twU;
	BYTE i, OptBuf[4];
    TFourByteUnion dwMoney;
	
	//���粻����,����ֱ�ӷ���
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return;							 
	}
	
	Result = SearchCard();
	if(Result != TRUE)
	{
		return;
	}
	CardType = 0xFF;
	dwMoney.l = (CurMoneyBag.Money.Remain.ll / 10000);
	
	//���쳣�忨��¼ʹ�ã������㣬�ڲ���ESAM��CARD��·�㺯���б�������ͷ�����忨ʧ�ܣ��ô˱������ݼ�¼�쳣�忨����ͷ
	memset( CommandHead, 0x00, sizeof(CommandHead) );
	
	//��ѹ�Ƿ����0.75Un	
	twU = wStandardVoltageConst / 10 * 75 / 10;//0.75Un
	for(i=0;i<MAX_PHASE;i++)
	{
		api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, 2, (BYTE*)&Result);
		if( Result > twU )
		{
			break;
		}
	}
	if( i < MAX_PHASE )
	{	
		api_ReadCardDisplay( 0x00, Ic_Err_Reg, dwMoney.l );
		Ic_Err_Reg = Card_Esam_Proc();		//��Ƭ����
		api_CardCloseRelay( dwMoney.l );			//�忨�պϼ̵���
	}	
	else
	{ 
		Ic_Err_Reg = IC_VLOW_ERR;			//��ѹ����
	}
	
	if( Ic_Err_Reg == OK_CARD_PROC )
	{
		if( CardType == PRESET_CARD_COMMAND )
		{	
			lib_ExchangeData(OptBuf,SafeAuPara.CardRst8B+4,4); //�������,���������¼,�����кź���λΪ�����ߴ���
			api_SetClearFlag( eCLEAR_MONEY, 0 ); //���������־
			api_SavePrgOperationRecord( ePRG_CLEAR_METER_NO ); // �����¼
		}
		else
		{	
		}
		
		InsertCardSta = 0x01;			//�忨�ɹ�
	}
	else
	{
		InsertCardSta = 0x02;			//�忨ʧ��
		
		Card_Err_Record();				//��Ƭ�����������Ƭ�����¼�м䲻�ܲ�����Ƭ��ESAM
		
		if(Ic_Err_Reg < 0x04) Ic_Err_Reg = 0x31;
		else if(Ic_Err_Reg < 0x11) Ic_Err_Reg = 0x32;
		else if(Ic_Err_Reg < 0x14) Ic_Err_Reg = 0x33;
		else if(Ic_Err_Reg < 0x19) Ic_Err_Reg = 0x34;
		else if(Ic_Err_Reg < 0x21) Ic_Err_Reg = 0x35;
		else Ic_Err_Reg = 0x36;
	}
	
	api_ReadCardDisplay( CardType, Ic_Err_Reg, dwMoney.l );
	api_CloseCard();//��Ƭ�����꣬�رտ�
	#if( MAX_PHASE == 1 )
	CLOSE_CARD_RESET;
	#endif//#if( MAX_PHASE == 1 )
}

//-----------------------------------------------
//��������: �忨��Ϣ
//
//����:  		��
//
//����ֵ:		��
//
//��ע:
//-----------------------------------------------
void ClrCardInfo( void )
{
	BYTE Buf[16]; //ʵ��ֻ�õ�8���ֽ�

	memset( Buf, 0, 16 );       //��ʼ�������
	api_WritePrePayPara( eLocalAccountFlag, Buf );
	api_WritePrePayPara( eSerialNo, Buf );
	api_WritePrePayPara( eRemoteAccountFlag, Buf );
	//��Ƿ��忨����
	api_ClrContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwIllegalCardNum ), sizeof(DWORD) ); 
}

//-----------------------------------------------
//��������:     ESAM����֤
//
//����:		��
//
//����ֵ:	    �ɹ�/ʧ��
//
//��ע:
//
//-----------------------------------------------
BYTE api_TestEsamCard( void )
{
	BYTE Buf[64];
	BYTE CmdBuf[6];
	BYTE DataBuf[22];
	BYTE Buf_EsamRunFile[50];
	BYTE Buf_Backup[50];                                    //����������Ϣ�ļ�
	BYTE CmdBufEsam[9];
	BYTE CmdEsamLen;
	TFourByteUnion td;

	api_ReadEsamData( ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_Backup );

	memset( Buf_EsamRunFile, 0x00, ESAM_RUN_FILE_LENTH );                   //�Ȱ�bufȫ����

	Buf_EsamRunFile[0] = 0x68;
	Buf_EsamRunFile[1] = 0x11;
	Buf_EsamRunFile[2] = 0x00;
	Buf_EsamRunFile[3] = 0x2b;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		td.d = CurMoneyBag.Money.Remain.ll / 10000;                                               //ʣ����   23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //�������
		td.d = 0;                                                                                       //͸֧(BCD)
		td.d = lib_DWBinToBCD( td.d );
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	else
	{
		td.d = 0;                                                                                       //ʣ����	 23-34
		lib_ExchangeData( Buf_EsamRunFile + 23, td.b, 4 );
		lib_ExchangeData( Buf_EsamRunFile + 27, (BYTE *)&CurMoneyBag.Money.BuyTimes, 4 );         //�������
		td.d = ((-1) * CurMoneyBag.Money.Remain.ll) / 10000;                                               //͸֧(BCD)
		td.d = lib_DWBinToBCD( td.d );
		memcpy( Buf_EsamRunFile + 31, td.b, 4 );
	}
	//35-38--����Ĭ��Ϊ00
	api_ReadIllegalCardNum( td.b );                                                       //�Ƿ�������
	td.d = lib_DWBinToBCD( td.d );
	lib_InverseData( td.b, 4 );
	memcpy( Buf_EsamRunFile + 38, td.b, 4 );

	Buf_EsamRunFile[42] = (BYTE)lib_WBinToBCD( RealTimer.wYear );                       //��������Ҫ��дʱ��,Զ��ͨѶ��07����д
	Buf_EsamRunFile[43] = lib_WBinToBCD( RealTimer.Mon );                                 //������ʱ��
	Buf_EsamRunFile[44] = lib_WBinToBCD( RealTimer.Day );
	Buf_EsamRunFile[45] = lib_WBinToBCD( RealTimer.Hour );
	Buf_EsamRunFile[46] = lib_WBinToBCD( RealTimer.Min );

	td.d = PrePayPara.CTCoe;                                                                //��ѹ���  11--22��Ҫת��Ϊbcd������ȶ���Ҫ
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 5, td.b, 3 );
	td.d = PrePayPara.PTCoe;                                                                //�������
	td.d = lib_DWBinToBCD( td.d );
	lib_ExchangeData( Buf_EsamRunFile + 8, td.b, 3 );

	memcpy( Buf_EsamRunFile + 11, FPara1->MeterSnPara.MeterNo, 12 );                         //��ſͻ����

	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 2] = lib_CheckSum( Buf_EsamRunFile + 1, ESAM_RUN_FILE_LENTH - 3 ); //����CS
	Buf_EsamRunFile[ESAM_RUN_FILE_LENTH - 1] = 0x16;

	if( WriteEsamWithMac( 0x03, ESAM_RUN_INFO_FILE, 0, ESAM_RUN_FILE_LENTH, Buf_EsamRunFile ) != TRUE )
	{
		return FALSE;
	}

	return TRUE;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )

