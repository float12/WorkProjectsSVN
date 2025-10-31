//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.11.15
//����		

//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"
#include "Dlt645_2007.h"

#if( PREPAY_MODE != PREPAY_NO )
#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#if( SEL_DLT645_2007 == YES )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//SERR�����֣����λ8������󣬹�Լ������û�д�λ;
#define		SERR_NO				0x0000	//û�д���;
#define		SERR_OTHER			0x8001	//��������;
#define		SERR_RECHARGE		0x8002	//�ظ���ֵ;
#define		SERR_ESAM_VERIFY	0x8004	//ESAM��֤����;
#define		SERR_AUTH			0x8008	//�����֤����;
#define		SERR_USER_ID		0x8010	//�ͻ���Ų�ƥ��;
#define		SERR_BUY_TIMES		0x8020	//��ֵ��������;
#define		SERR_OVER_REGRATE	0x8040	//���糬�ڻ�;
#define		SERR_ADDR_ABR		0x8080	//��ַ�쳣;
#define		SERR_HUNGUP			0x8100	//������;
#define  	SERR_CLOSE_RELAY 	0x8200  //��բ����ʧ��;
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						



//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------




//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: Զ�������֤
//
//����:		ProBuf[in/out]	645����
//				����		18~25		in
//				�����1		26~33		in
//				��ɢ����	34~41		in
//				�����2		14~17		out
//				esam���к�	18~25		out		
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լ�����֤����  ��֤ʱЧ����־û����!!!!!!
//-----------------------------------------------
static WORD RemoteAuthorization(BYTE *ProBuf,BYTE *OutBuf)
{
// 	BOOL Result;
// 	BYTE Buf[24];
// 	BYTE SecretData1[10];
// 	WORD ReturnLen,EsamLen;

// 	lib_InverseData(ProBuf+18,8);//����1
// 	lib_InverseData(ProBuf+26,8);//�����1
// 	lib_InverseData(ProBuf+34,8);//��ɢ����

// 	if( ProBuf[34] || ProBuf[35] )//��ɢ���Ӹ�λ����Ϊ��
// 	{
// 		return SERR_OTHER;
// 	}
	
// 	if( memcmp(ProBuf+36,FPara1->MeterSnPara.MeterNo,6) != 0 )//��ű���һ��
// 	{
// 		return SERR_OTHER;
// 	}
// 	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x03, 0x00, 0x10);
// 	memcpy( Buf+6, ProBuf+34, 8 );//��ӷ�ɢ����
// 	memcpy( Buf+14, ProBuf+26, 8 );//��������1
	
// 	Result = Link_ESAM_TxRx(6,Buf,16,Buf+6,SecretData1);
// 	if(Result == FALSE)
// 	{
// 		return SERR_AUTH;
// 	}
		
// 	ReturnLen = 0;
	
// 	if( memcmp(ProBuf+18,SecretData1+2,8) == 0 )
// 	{
// 		//�������
// 		api_ReadEsamRandom(4,Buf);
// 		lib_ExchangeData(OutBuf,Buf+2,4);
// 		ReturnLen += 4;
// 		//ESAM���к�
// 		EsamLen = api_GetEsamInfo( 0x02, Buf );
// 		if( EsamLen > (sizeof(Buf)-2) )
// 		{
// 			return SERR_OTHER;
// 		}
// 		memcpy( SafeAuPara.EsamRst8B, Buf, 8 );
// 		lib_ExchangeData(OutBuf+4,SafeAuPara.EsamRst8B,8);
		
// 	    api_SetSysStatus(eSYS_STATUS_ID_645AUTH);//��λ��������־
// 	    api_Release698_Connect( );
// //		api_ReadPrePayPara( eRemoteTime, (BYTE *)&APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime );//645Ĭ������վ����Ӧ������
//         APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime = (APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime*60);
//         APPConnect.ConnectInfo[eConnectMaster].ConstConnectValidTime = APPConnect.ConnectInfo[eConnectMaster].ConnectValidTime;

// 		ReturnLen += 8;
		
// //		api_SaveProgramRecord645(EVENT_END, (BYTE *)NULL, (BYTE *)NULL );
// 		return ReturnLen;
// 	}
	
	// return SERR_AUTH;
}

//-----------------------------------------------
//��������: д������ESAM �еĲ������˺���û�����ã�CmdBufǰ��2�ֽڴ�����
//
//����:		Type[in]	0x04--������Ϣ�ļ�����Ĳ�������
//						0x85--��ǰ�׵�۸���
//						0x86--�����׵�۸���
//			P2[in]		ƫ�Ƶ�ַ
//			LC[in]		����InData����
//			InData[in]	Data+Mac����Ϊ����
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//
//��ע:698�汾ESAM ��645��Լд����(��������ǰ�ס�������)����
//-----------------------------------------------
static WORD WriteEsamData( BYTE Type, WORD P2, WORD LC, BYTE *InData )
{
	BOOL Result;
	BYTE CmdBuf[7];
	
	if( (Type!=0x04) && (Type!=0x85) && (Type!=0x86) )
	{
		return 0x8000;
	}
	
	//MAC����
	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return 0x8000;
	}
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = Type;
	CmdBuf[3] = P2>>8;
	CmdBuf[4] = P2;
	CmdBuf[5] = LC>>8;
	CmdBuf[6] = LC;
	
	Result = Link_ESAM_TxRx(7,CmdBuf,LC,InData,CmdBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	return 0;
}

//-----------------------------------------------
//��������: ������������
//
//����:		Type[in]	0x82--Զ�̿���
//						0x83--�����������
//						0x84--��������
//						0x85--�¼�/��������
//			LC[in]		����InData����
//			InData[in]	����+Mac����Ϊ����
//			OutBuf[out]	���ܺ������				
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��ԼԶ�̿��ơ�д����������������㡢�¼�/�����������
//-----------------------------------------------
WORD api_DecipherSecretData(BYTE Type, WORD LC, BYTE *InData, BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[6];
	WORD ReturnLen;

	if( (Type<0x82) || (Type>0x85) )
	{
		return 0x8000;
	}
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x12;
	CmdBuf[2] = 0xc0;
	CmdBuf[3] = Type;
	CmdBuf[4] = LC>>8;
	CmdBuf[5] = LC;
	
	ReturnLen = 0;

	Result = Link_ESAM_TxRx(6,CmdBuf,LC,InData,OutBuf);
	if(Result == FALSE)
	{
		return 0x8000;
	}
	
	if(Type == 0x83)
	{
		//��ȷ���ܽ��Ϊ00 LEN (LEN-1) DI DI DI DI DATA DATA ...
		lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
		ReturnLen -= 1;
		memmove( OutBuf, OutBuf+3, ReturnLen );
		lib_InverseData(OutBuf,ReturnLen);
	}
	else
	{
        lib_ExchangeData( (BYTE*)&ReturnLen, OutBuf, 2 );
        memmove( OutBuf, OutBuf+2, ReturnLen );
	}
	
	return ReturnLen;
}

//-----------------------------------------------
//��������: ���������֤ʱЧ
//
//����:		ProBuf[in/out]	645����
//				
//				
//				
//				
//						
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:
//-----------------------------------------------
static WORD RemoteSetAuthTime(BYTE *ProBuf,BYTE *OutBuf)
{
	TFourByteUnion Temp32;
	BYTE Buf[12],RecBuf[10];//6+2+4

	Temp32.d = 0;
	
    if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}
    
    lib_InverseData(ProBuf+18,2);//����
	//lib_InverseData(ProBuf+20,4);//MAC����ESAM�д��

	if( (ProBuf[18]==0x00) && (ProBuf[19]== 0x00) )	//��ȫ��֤ʱЧ����д��0����д��0�򷵻�ʧ�ܡ�
	{
		return SERR_OTHER;
	}

	if( lib_IsMultiBcd( ProBuf+18, 2 ) == FALSE )
	{
		return SERR_OTHER;
	}

	//дESAM--����ESAM�д��
	//api_ProcPrePayCommhead( Buf, 0x83, 0x2A, 0x84, 0x2B, 0x00, 0x06);
	//memcpy( Buf+6, ProBuf+18, 2 );//����
	//memcpy( Buf+8, ProBuf+20, 4 );//MAC
	//if(Link_ESAM_TxRx(6,Buf,6,Buf+6,RecBuf) == FALSE)
	{
	//	return SERR_ESAM_VERIFY;
	}

	//дEEprom
	lib_InverseData(ProBuf+18,2);
	memcpy(Temp32.b,ProBuf+18,2);
	Temp32.d = lib_DWBCDToBin(Temp32.d);
	api_WritePrePayPara(eRemoteTime,(BYTE *)&Temp32);
	//��̼�¼!!!!!

	return 2;
}

//-----------------------------------------------
//��������: ״̬��ѯ
//
//����:		ProBuf[out]	���645��ʽ״̬��ѯ �Ѿ������
//									
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լ״̬��ѯ����
//-----------------------------------------------
static WORD ReadEsamStatus(BYTE *ProBuf,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE DataBuf[20];
	WORD ReturnLen;

	if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}

	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return SERR_HUNGUP;
	}
	
	CmdBuf[0] = 0x80;
	CmdBuf[1] = 0x48;
	CmdBuf[2] = 0x00;
	CmdBuf[3] = 0x00;
	CmdBuf[4] = 0x00;
	CmdBuf[5] = 0x0d;
	CmdBuf[6] = 0x48;
	//���
	DataBuf[0] = 0x00;
	DataBuf[1] = 0x00;	
	memcpy(DataBuf+2,FPara1->MeterSnPara.MeterNo,6);
	//�����
	memcpy(DataBuf+8,SafeAuPara.EsamRand,4);
	
	Result = Link_ESAM_TxRx(7,CmdBuf,12,DataBuf,DataBuf);//
	if(Result == FALSE)
	{
		return SERR_OTHER;
	}
	
	ReturnLen = 0;
	
	//ʣ����
	lib_ExchangeData(OutBuf,DataBuf+2,4);
	ReturnLen += 4;
	//���MAC
	memset(OutBuf+4,0x00,4);
	ReturnLen += 4;
	//�������
	lib_ExchangeData(OutBuf+8,DataBuf+2+4,4);
	ReturnLen += 4;
	//����MAC
	lib_ExchangeData(OutBuf+12,DataBuf+2+4+4+6,4);
	ReturnLen += 4;
	//�ͻ����
	lib_ExchangeData(OutBuf+16,DataBuf+2+4+4,6);
	ReturnLen += 6;
	//��Կ��Ϣ��645��ʽΪ4�ֽڣ���Ϊ698�޴����ݣ�ֱ��ǿ����Ϊȫ0
	//api_GetEsamInfo(0x04,DataBuf);
	memset(OutBuf+22,0x00,4);
	ReturnLen += 4;
	
	return ReturnLen;
}


//-----------------------------------------------
//��������: ���ݻس�
//
//����:	ProBuf[in/out]	645����
//				�ļ�Ŀ¼	24~25		in
//				�ļ���		22~23		in
//				��ʼ��ַ	20~21		in
//				����		18~19		in
//				��������	14~nn		out						
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լ���ݻس����� �˺���ָ����������!!!!!!
//-----------------------------------------------
static WORD ReadEsamDataWithMac(BYTE *ProBuf,BYTE *OutBuf)
{
	BOOL Result;
	BYTE CmdBuf[7];
	BYTE DataBuf[20];
	WORD ReturnLen;
	//�˺�������������--181220-songchen
	if(api_GetSysStatus(eSYS_STATUS_ID_645AUTH) == FALSE )
	{
		return SERR_AUTH;
	}

	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return SERR_HUNGUP;
	}

	if(ProBuf[22] != 1)
	{
		CmdBuf[0] = 0x83;
		CmdBuf[1] = 0x2c;
		CmdBuf[2] = ProBuf[23];
		CmdBuf[3] = ProBuf[22];
		CmdBuf[4] = ProBuf[21];
		CmdBuf[5] = ProBuf[20];
		CmdBuf[6] = 0x48;
		//���
		DataBuf[0] = 0x00;
		DataBuf[1] = 0x00;	
		memcpy(DataBuf+2,FPara1->MeterSnPara.MeterNo,6);
		memcpy(DataBuf+8,SafeAuPara.EsamRand,4);
		DataBuf[12] = ProBuf[19];
		DataBuf[13] = ProBuf[18];
		DataBuf[14] = 0x04;
		DataBuf[15] = 0xd6;
		DataBuf[16] = 0x85;
		DataBuf[17] = 0x00;
		memcpy( (BYTE*)&ReturnLen, ProBuf+18, 2 );
		DataBuf[18] = (ReturnLen+4)>>8;
		DataBuf[19] = (ReturnLen+4);
		
		Result = Link_ESAM_TxRx(7,CmdBuf,20,DataBuf,ProBuf+14);
		if(Result == FALSE)
		{
			return SERR_OTHER;
		}
		
		lib_ExchangeData( (BYTE*)&ReturnLen, ProBuf+14, 2 );
		memmove(ProBuf+14,ProBuf+14+2,ReturnLen);
		lib_ExchangeData(OutBuf,ProBuf+14,ReturnLen);
	}
	else
	{
		ReturnLen = ReadEsamStatus(ProBuf,OutBuf);
	}
	
	return ReturnLen;

}

//-----------------------------------------------
//��������: �����ѯ��������
//
//����:		
//		ProBuf[in/out]		in--���뻺��645�ĵ�һ���ֽ�68��ͷ
//							out--������ݣ��Ѿ�����					
//����ֵ:	���ݳ���
//		   
//��ע:698�汾ESAM ��645��Լ03�����ֵ���
//-----------------------------------------------
static WORD Ir_Request( BYTE *UartBuf, BYTE *OutBuf )
{
	BYTE result;
	BYTE Buf[30];
	BYTE K1[10];
	WORD EsamLen;
								
	//������Ϣ: ���(6)+ESAM���к�(8)+�����1����(8)+�����2(8)
	api_ResetEsamSpi();			//Esam Reset
	EsamLen = api_GetEsamInfo( 0x02, Buf ); //��ȡESAM���к�
	if( EsamLen > (sizeof(Buf)-2) )
	{
		return SERR_OTHER;
	}
	memcpy( SafeAuPara.EsamRst8B, Buf, 8 );
	
	//��Ų�0000
	Buf[6] = 0x00;
	Buf[7] = 0x00;
	memcpy( Buf+8, FPara1->MeterSnPara.MeterNo, 6 );
	lib_ExchangeData( Buf+14, UartBuf+18, 8 );	//�����1
	api_ProcPrePayCommhead( Buf, 0x80, 0x08, 0x08, 0x03, 0x00, 0x10);
	
	result = Link_ESAM_TxRx(6,Buf,16,Buf+6,K1);
	if(result != TRUE)
	{
		return SERR_OTHER;
	}

	//�������2
	result = api_ReadEsamRandom(8,Buf);
	if(result != TRUE)
	{
		return SERR_OTHER;
	}
	
	lib_ExchangeData( OutBuf, FPara1->MeterSnPara.MeterNo, 6 );//���, 6bytes
	lib_ExchangeData( OutBuf + 6, SafeAuPara.EsamRst8B, 8 ); //ESAM���к�, 8bytes
	lib_ExchangeData( OutBuf+14, K1+2, 8 );	//�����1����, 8bytes
	lib_ExchangeData( OutBuf + 22, SafeAuPara.EsamRand, 8 );   //�����2, 8bytes

	return 30;
}
//-----------------------------------------------
//��������:������֤
//
//����:		
//		ProBuf[in/out]		in--���뻺��645�ĵ�һ���ֽ�68��ͷ
//							out--������ݣ��Ѿ�����					
//����ֵ:	���ݳ���
//		   
//��ע:698�汾ESAM ��645��Լ03�����ֵ���
//-----------------------------------------------
static WORD Ir_Authorazation( BYTE *ProBuf, BYTE *OutBuf )
{
// 	BYTE result;
// 	BYTE Buf[20];	
									
// 	lib_InverseData( ProBuf+18, 8 );	//�����2����
// 	api_ProcPrePayCommhead( Buf, 0x80, 0x06, 0x00, 0x01, 0x00, 0x08);
// 	memcpy( Buf+6, ProBuf+18, 8 );//��ӷ�ɢ����
		
// 	result = Link_ESAM_TxRx(6,Buf,8,Buf+6,Buf);
// 	if( result != TRUE )
// 	{
// //		APPConnect.IRAuthTime = 0;
// 		api_ClrSysStatus(eSYS_IR_ALLOW_PRG);
// 		return SERR_ESAM_VERIFY;
// 	}				
// //	api_ReadPrePayPara( eIRTime, (BYTE *)&APPConnect.IRAuthTime );
// 	APPConnect.IRAuthTime*=60;
	
// 	if( APPConnect.IRAuthTime == 0 )//���������֤ʱЧ����Ϊ0����ȡ��������Ĭ�Ϻ�����֤Ĭ��30min;
// 	{
// 		APPConnect.IRAuthTime = 1800;
// 	}
// 	api_SetSysStatus(eSYS_IR_ALLOW_PRG);
	
// 	return 0;
}

//-----------------------------------------------
//��������: ��ȫ������
//
//����:		
//		ProBuf[in/out]		in--���뻺��645�ĵ�һ���ֽ�68��ͷ
//							out--������ݣ��Ѿ�����?
//		PortType			ͨ��		
//����ֵ:	���ݳ���
//		   
//��ע:698�汾ESAM ��645��Լ03�����ֵ���
//-----------------------------------------------
WORD api_ProcProPrePay(eSERIAL_TYPE PortType, BYTE *ProBuf ,BYTE *OutBuf )
{
	WORD wSerr;
	DWORD DI645;
	
	wSerr = SERR_OTHER;
	
	memcpy((BYTE*)&DI645,ProBuf+10,4);
	
	//68H A0 �� A5 68H 03H L DIO �� DI3 C0 �� C3 N1 �� Nm CS 16H
	switch( DI645 )
	{
		// case 0x070000ff://�����ָ֤��
		// 	//if((PortType == eIR)&&(api_GetSysStatus(eSYS_IR_ALLOW_PRG)==FALSE)&&(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY )==FALSE ))
		// 	//{
		// 	//	break;
		// 	//}
		// 	wSerr = RemoteAuthorization(ProBuf,OutBuf);	
		// 	break;
		case 0x070001ff://�����֤ʱЧ����
			wSerr = RemoteSetAuthTime(ProBuf,OutBuf);
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	//���طѿر�
		case 0x070101ff://����
		case 0x070102ff://��ֵ
		case 0x070103ff://Ǯ����ʼ��
			//wSerr = Remote_Proc_Account(ProBuf);
			break;
		#endif
		case 0x078001ff://���ݻس�
			wSerr = ReadEsamDataWithMac(ProBuf,OutBuf);
			break;
        //case 0x078003ff://��������
        	//if((PortType == eIR)||(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ))
        	//{
        	//	wSerr = Ir_Request(ProBuf,OutBuf);
        	//}
            //break;
        //case 0x070003ff://������֤
        //	if((PortType == eIR)||(api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ))
        //	{
        //		wSerr = Ir_Authorazation(ProBuf,OutBuf);
        //	}	
        //    break;	
		case 0x078102ff://״̬��ѯ
			wSerr = ReadEsamStatus(ProBuf,OutBuf);
			break;
		
		default:
			break;
	}
	
	return wSerr;
}


//-----------------------------------------------
//��������: �̵��������������
//
//����:	ProBuf[in/out]	645����		
//		
//����ֵ:	ִ��״̬�������ֽ�
//		   
//��ע:698�汾ESAM ��645��Լ1c�����ֵ���
//-----------------------------------------------
//0��բ 1�����բ 2ֱ�Ӻ�բ 3Ԥ��բ1 4Ԥ��բ2	5���� 6������ 7ռλ 8ռλ 9ռλ 10�ϵ� 11���� 12�������
#if(PREPAY_MODE == PREPAY_LOCAL)
static const BYTE CtrlCmd645[] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x3a,0x3b,0xA5,0xA5,0xA5,0xff,0x2a,0x2b};
#else
static const BYTE CtrlCmd645[] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x3a,0x3b,0xff,0x2a,0x2b};
#endif
WORD api_ProcRemoteRelayCommand645(BYTE *ProBuf)
{
	BYTE CtrlCmd,i;
	BYTE TimeBuf[6];
	WORD wRelayOffWarnTime;
	WORD ReturnStatus;
	WORD wRelayWaitOffTime;
	
	//����Ϊ98��
	if(ProBuf[10] == CLASS_2_PASSWORD_LEVEL)
	{
		//MAC����
		if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
		{
			return R_ERR_MAC;
		}
		
		//����
		lib_InverseData(ProBuf+22,16);
		//MAC
		lib_ExchangeData( ProBuf+22+16, ProBuf+18,  4 );
		//����
		ReturnStatus = api_DecipherSecretData(0x82, 0x14, ProBuf+22, ProBuf+22);
		if(ReturnStatus & 0x8000)
		{
			return R_ERR_MAC;
		}
		
	}
	else if( (ProBuf[10]==MINGWEN_H_PASSWORD_LEVEL) || (ProBuf[10]==MINGWEN_L_PASSWORD_LEVEL) )
	{	
	}
	else
	{
		return R_ERR_PASSWORD;
	}
	
	CtrlCmd = ProBuf[22];
	if( api_JudgeClock645( ProBuf+24 ) == FALSE )
	{
		return R_ERR_TIME;
	}
	
	//��բ�Իָ�ʱ�䣬��λ����
	if( (CtrlCmd==0x1d) || (CtrlCmd==0x1e) )
	{
		wRelayOffWarnTime = (DWORD)ProBuf[23]*5;
	}
	else
	{
		wRelayOffWarnTime = 0;
	}
	
	//��բ��ʱʱ��
	if(CtrlCmd == 0x1a)
	{
		ReadRelayPara(2,(BYTE*)&wRelayWaitOffTime);
	}
	else
	{
		wRelayWaitOffTime = 0;
	}
	
	for(i=0;i<sizeof(CtrlCmd645);i++)
	{
		if(CtrlCmd == CtrlCmd645[i])
		{
			break;
		}
	}
	
	ReturnStatus = R_ERR_PASSWORD;

	#if(PREPAY_MODE == PREPAY_LOCAL)
	if((i==7)||(i==8)||(i==9))//CtrlCmd645��������ռλ����0xa5������·������������������򷵻ش���
	{
		return ReturnStatus;
	}
	#endif
	
	if( i < sizeof(CtrlCmd645) )
	{
		ReturnStatus = api_Set_RemoteRelayCmd(i, wRelayWaitOffTime, wRelayOffWarnTime);
	}
	
	return ReturnStatus;
}


//-----------------------------------------------
//��������: ��������������ݣ�û��ʹ�ô˺�����
//
//����:	ProBuf[in/out]	645����	�����ܺ�����ݷ��ڴ�ProBuf+22��ʼ������
//		
//����ֵ:	TRUE/FALSE
//		   
//��ע:698�汾ESAM ��645��Լ19/1A/1B�����ֵ���
//-----------------------------------------------
BOOL api_ProcClrCmd645(BYTE *ProBuf)
{
	BYTE Type;
	WORD ReturnStatus;
	
	if(ProBuf[10] != CLASS_2_PASSWORD_LEVEL)
	{
		memmove( (BYTE*)&ProBuf[22], (BYTE*)&ProBuf[18], ProBuf[9]-8);
		lib_InverseData( (BYTE*)&ProBuf[24], 6 );//��ʱ����е���
		return TRUE;
	}
	
	//MAC����
	if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
	{
		return FALSE;
	}
	
	if(ProBuf[8] == 0x1a)
	{
		Type = 0x84;
	}
	else if(ProBuf[8] == 0x1b)
	{
		Type = 0x85;
	}
	#if( MAX_PHASE == 3 )
	else if(ProBuf[8] == 0x19)
	{
		Type = 0x85;
	}
	#endif
	else
	{
		return FALSE;
	}
	lib_InverseData( ProBuf+22, 16 );
	lib_ExchangeData(ProBuf+22+16,ProBuf+18, 4);
	//����
	ReturnStatus = api_DecipherSecretData(Type, 0x14, ProBuf+22, ProBuf+22);
	if(ReturnStatus == 0x8000)
	{
		return FALSE;
	}
	
	return TRUE;
}
#endif//#if( SEL_DLT645_2007 == YES )

#if(SEL_DLT698_2016_FUNC == YES )
#if( PREPAY_MODE == PREPAY_LOCAL)

//-----------------------------------------------
//��������: 645����һ�����
//
//����:		Type[in]	0x84--������Ϣ�ļ�
//						0x85--��ǰ�׵���ļ����������ʺͽ���
//						0x86--�����׵���ļ����������ʺͽ���
//			P1[in]		��ʼ��ַ
//			LC[in]		����InData����
//			InData[in]	����+Mac����Ϊ����			
//����ֵ:	�ɹ�/ʧ��
//		   
//��ע:698�汾ESAM ��645��Լ����esam��һ�����
//-----------------------------------------------
BYTE api_UpdateFirstData645(BYTE Type, BYTE P1,WORD LC, BYTE *InData)
{
	BOOL Result;
	BYTE CmdBuf[6];
	BYTE OutBuf[20];
	
	if( (Type<0x84) || (Type>0x86) )
	{
		return FALSE;
	}
	
	CmdBuf[0] = 0x83;
	CmdBuf[1] = 0x2a;
	CmdBuf[2] = Type;
	CmdBuf[3] = P1;
	CmdBuf[4] = LC>>8;
	CmdBuf[5] = LC;
	

	Result = Link_ESAM_TxRx(6,CmdBuf,LC,InData,OutBuf);

	return Result;
}

//-----------------------------------------------
//��������: 645д���ʵ�ۡ����ݵ�ۡ������л�ʱ�䡢�����л�ʱ��
//
//����:		Type[in]	0x82--Զ�̿���
//						0x83--�����������
//						0x84--��������
//						0x85--�¼�/��������
//			LC[in]		����InData����
//			InData[in]	����+Mac����Ϊ����
//			OutBuf[out]	���ܺ������				
//����ֵ:	���ݳ��ȣ�bit15Ϊ1�������
//		   
//��ע:698�汾ESAM ��645��Լдһ�����
//-----------------------------------------------
BYTE api_Proc_uart_Write_FirstData_645( BYTE *pBuf )
{
	BYTE k,i,j,m,n, result,ESAMFile,ESAMOffset;
	TFourByteUnion DI07;
	TFourByteUnion Tmpbuf;
	WORD ParaType,ESAMLenth;
	TRealTimer TmpRealTimer;
	TRatePrice TmpRatePrice;
	TLadderPrice TmpLadderPrice;
	
	memcpy(DI07.b,pBuf+10,4);
	k = 0;
	
	switch( DI07.d )
	{
		#if( PREPAY_MODE == PREPAY_LOCAL )				//���طѿر�
		case 0x04000108://���׷��ʵ���л�ʱ��			YYMMDDhhmm	5	������ʱ��
		case 0x04000109://���׽��ݵ���л�ʱ��			YYMMDDhhmm	5	������ʱ��	
			//���ݷ�������ֽڣ���������ֽڡ����д��ESAM,������Ҫת��
			//�ж��л�ʱ���Ƿ�Ϸ�
			if( CheckSwitchTime( pBuf+22 ) == 0x01 )
            {
                break;
            }
				
			if( lib_IsMultiBcd( pBuf+22, 5 ) == FALSE )	
			{	
				break;
			}
			
			//��������--����󣬷�������ֽڣ���������ֽ�
			lib_InverseData( pBuf+22,pBuf[9]-16 );
			//MAC
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMLenth = 5+4;			//5λ����+4λMAC
			if( DI07.d == 0x04000108 )	//���׷��ʵ���л�ʱ��
            {               
				ESAMOffset = 10;
				ParaType = 2;
				ESAMFile = 0x84;
            }
            else						//���׽��ݵ���л�ʱ��
            {  
				ESAMOffset = 196;
				ParaType = 3;
				ESAMFile = 0x86;
            }
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);//д��ESAM
			if( result != TRUE )	
			{	
				break;
			}
			//����--����󣬻ָ���������ֽڣ���������ֽ�
			lib_InverseData( pBuf+22,pBuf[9]-16 );
			
			api_ConvertYYMMDDhhmm_TRealTimer( &TmpRealTimer, pBuf+22 );
            
			if( api_ReadAndWriteSwitchTime( WRITE, ParaType, (BYTE *)&TmpRealTimer ) == FALSE )
			{
				break;
			}	
			
			k = 1;
			break;
		case 0x040502ff:	//���ñ����׷��ʵ��			
			//���鱨�ļ�������Ч��
			m = (pBuf[9]-16);
			n = m/4;
			if( (m%4) || (n==0) || (n>MAX_RATIO) ) 
			{
				break; //���ݳ��ȴ���, �˳� 
			}

			if( lib_IsMultiBcd( pBuf+22, m ) == FALSE )	
			{	
				break;
			}
			
            //�������ݵ���--Ϊ��ӦESAM����Ҫ��
            for(i=0;i<n;i++)
			{
				lib_InverseData( pBuf+22+i*4,4 );
            }	
			//MAC����
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMOffset = 4;
			ESAMFile = 0x86;
			ESAMLenth = m+4;
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);
			if( result != TRUE )
			{
				break;
			}

			for( i=0; i<n; i++ ) 
        	{
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[4*i+22], 4 );//��ȡ���
      		 	TmpRatePrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCDת��ΪHex
        	}
			
			for( i=n; i<MAX_RATIO; i++ )//�Ѳ���5���ʵģ������һ����������
			{
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*(n-1)], 4 );//��ȡ���
   				TmpRatePrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCDת��ΪHex
			}
			
			if( api_WritePrePayPara( eBackupRateTable, (BYTE*)&TmpRatePrice.Price ) == FALSE )
		    {
		        break;
		    }
			
			k = 1;
			break;
		case 0x040605FF:	//���ñ����׽���ֵ+���ݵ��										
			if(pBuf[9] != (4*MAX_LADDER+4*(MAX_LADDER+1)+MAX_YEAR_BILL*3+16) )//���ݳ�����ȷ�Լ��
			{
				break;
			}
			if( lib_IsMultiBcd( pBuf+22, (4*MAX_LADDER+4*(MAX_LADDER+1)+MAX_YEAR_BILL*3) ) == FALSE )	//�Ƿ�bcd���
			{	
				break;
			}

			for( i=0; i<MAX_YEAR_BILL; i++ )//645�Ƿ�ʱ��ֻ��99�������������ԣ��Ϸ�ʱ�����ڲ��ܳ���28��
			{
				if( api_CheckClockBCD( 2, 3+0x80, pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1) ) == FALSE )
				{
					for( j=0; j<3; j++ )
					{
						if( pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+j] != 0x99 )
						{
							break;
						}
					}
				}
				else //��������첻�ܳ���28
				{
					if( pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+1] > 0x28 )
					{
						break;
					}
				}
			}
			if( i<MAX_YEAR_BILL )
			{
				break;
			}

		   if((lib_IsAllAssignNum( pBuf+22, 0x00,24 )==FALSE) 
             && (api_JudgeYearClock( pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1) ) == FALSE)) 
            {	
                break; //����ֵ��ȫ0����4�����ݽ�����ȫ����Ч����������
            }
			 
            //�������ݷ����
            for(i=0;i<13;i++)//����ֵ�ͽ��ݵ��
			{
				lib_InverseData( pBuf+22+i*4,4 );
            }
			for(i=0;i<MAX_YEAR_BILL;i++)//�������
			{
				lib_InverseData( pBuf+22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i,3 );
            }
			//MAC����
			lib_InverseData( pBuf+22+pBuf[9]-16, 4 );

			ESAMOffset = 132;
			ESAMFile = 0x86;
			ESAMLenth = 64+4;
			result = api_UpdateFirstData645( ESAMFile, ESAMOffset,ESAMLenth, pBuf+22);
			if( result != TRUE )
			{
				break;
			}

			for( i=0; i<MAX_LADDER; i++ )//���ݵ���
		    {  
		  		lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*i], 4 );//��ȡ���ݵ���
		  		TmpLadderPrice.Ladder_Dl[i] = lib_DWBCDToBin(Tmpbuf.d);//BCDת��ΪHex     
		    }

		    for( i=0; i<MAX_LADDER+1; i++ )//���ݵ��
		    {
				lib_ExchangeData( Tmpbuf.b, (BYTE*)&pBuf[22+4*MAX_LADDER+4*i], 4 );//��ȡ���ݵ��
		  		TmpLadderPrice.Price[i] = lib_DWBCDToBin(Tmpbuf.d);//BCDת��ΪHex 
		    }

  			//memset( (BYTE*)TmpLadderPrice.YearBill, 99, sizeof(TmpLadderPrice.YearBill) );//���ý�����Ϊ��Ч--����ûʲô�ô�--songchen
			for( i=0; i<MAX_YEAR_BILL; i++ )//�������
		    {    
	            for( j=0; j<3;j++ )
	            {
	                TmpLadderPrice.YearBill[i][j] = pBuf[22+4*MAX_LADDER+4*(MAX_LADDER+1)+3*i+j];
	                //if( lib_IsBCD(TmpLadderPrice.YearBill[i][j]) == TRUE )--����Ҫ��飬ǰ���Ѿ�������Ƿ�bcd�ͺϷ���--songchen
	                {
	                    TmpLadderPrice.YearBill[i][j] = lib_BBCDToBin( TmpLadderPrice.YearBill[i][j] );
	                }
	            }    
			}
			
			if( api_WritePrePayPara( eBackupLadderTable, (BYTE*)&TmpLadderPrice.Ladder_Dl ) == FALSE )
		    {
		        break;
		    }
			
			k = 1;
			break;	
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )	//���طѿر�								

		default:
			break;
	}
	
	if( k == 0 )
	{
		return FALSE;
	}	
	return TRUE;	
}

BYTE api_RemoteActionMoneybag( BYTE Type, DWORD Money, DWORD BuyTimes, BYTE *pCustomCode, BYTE *pSIDMAC, BYTE *pMeterNo )
{
    WORD Result;
	TFourByteUnion TmpMoneyInMeter;
	DWORD TmpBuyTimesInMeter;
    WORD TmpNewBuyTimes;
    BYTE OpenFlag;

    if((memcmp( FPara1->MeterSnPara.MeterNo, pMeterNo, 6)) != 0 )//��Ų���ȷ���ʧ��
    {
        return DAR_OtherErr;
    }

	//��ʣ����͹������
	api_ReadPrePayPara( eRemainMoney, (BYTE*)TmpMoneyInMeter.b );
	api_ReadPrePayPara( eBuyTimes, (BYTE*)&TmpBuyTimesInMeter );
      
    if( (Type == 0) || (Type == 1) )//���� ��ֵ
    {
        if( Type == 0 )//���� �жϹ������
        {
            if( BuyTimes > 1 )
            {
                return DAR_ChargeTimes;
            }
        }

        if( (CardInfo.LocalFlag == eOpenAccount) || (CardInfo.RemoteFlag == eOpenAccount))//Զ�̻򱾵��ѿ���
        {
            if((memcmp( FPara1->MeterSnPara.CustomCode, pCustomCode, 6)) != 0 )//�жϿͻ���ű������
            {
                return DAR_CustomNo;
            }
        }
        else
        {
            if( Type == 1 )//��ֵ
            {
                return DAR_ChargeTimes;
            }
        }
        
        if( TmpBuyTimesInMeter > BuyTimes )//�ѹ���������ڹ������ ����ʧ��
        {
            return DAR_ChargeTimes;
        }
        else
        {
            TmpNewBuyTimes = (BuyTimes - TmpBuyTimesInMeter);
        }

        if( TmpNewBuyTimes > 1 )//�����������1����ʧ��
        {
            return DAR_ChargeTimes;
        }

        if( TmpNewBuyTimes == 1 )
        {
			// ʣ��+��ֵ
			if( (long)(TmpMoneyInMeter.l + Money) > 99999999L ) //��Ӻ������(����999999.99Ԫ)
			{
				return DAR_OverHold;
			}
			//�ж϶ڻ����
			if( LimitMoney.RegrateLimit.d )
			{
				if( (long)(TmpMoneyInMeter.l + Money) > LimitMoney.RegrateLimit.l ) //��ֵǰ��֤�Ƿ񳬶ڻ� //��ʽת�����з���ת��Ϊ�޷������� - 2018.12.28 - lzn
				{
					return DAR_OverHold;
				}
			}

             //��֤ESAM
             Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
             if( Result == FALSE )
             {
                return DAR_EsamFial;
             }

             api_UpdataRemainMoney( eChargeMoneyMode, Money , BuyTimes);
			 api_RemoteChargeCloseRelay(TmpMoneyInMeter.l);
        }
        else
        {
			if( Type == 0 )
			{
				//��֤ESAM ���¿ͻ����
				if( CardInfo.RemoteFlag != eOpenAccount )//Զ�̿�������֤esam
				{
					Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
					if( Result == FALSE )
					{
					   return DAR_EsamFial;
					}
				}
			}
			else if( Type == 1 ) //Զ�̳�ֵ������������ʾ�ظ���ֵ
			{
				return DAR_Recharge;
			}
		}
        

        //���±���״̬

        
        if( Type == 0 )//����ǿ��� ����ͻ����
        {
            //����ͻ����
            Result = api_ProcMeterTypePara( WRITE, eMeterCustomCode, pCustomCode );
            if( Result == FALSE )//дeppromʧ�� ��ֵ�ɹ�����������
            {
               return DAR_OtherErr;
            }

            //��λԶ�̿���״̬
            OpenFlag = eOpenAccount;
			api_WritePrePayPara( eRemoteAccountFlag, (BYTE *)&OpenFlag );
        }

    }
    else if( Type == 2 )//�˷�
    {
        if( TmpBuyTimesInMeter != BuyTimes )
        {
            return DAR_ChargeTimes;
        }
		//�˷�ǰ��ͬ��ESAM�ͱ���ʣ����
		MeteringUpdateMoneybag();
        //��֤ESAM
        Result = api_DealMoneyDataToESAM( Money, BuyTimes, pCustomCode, pSIDMAC );
        if( Result == FALSE )
        {
            return DAR_EsamFial;
        }

        api_UpdataRemainMoney( eReturnMoneyMode, Money , BuyTimes);
    }
    else
    {
        return DAR_EsamFial;
    }

    return DAR_Success;
}
#endif
#endif//#if(SEL_DLT698_2016_FUNC == YES )

#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#endif//#if( PREPAY_MODE != PREPAY_NO )

