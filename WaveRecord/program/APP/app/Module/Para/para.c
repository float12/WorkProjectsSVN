//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.2
//����		��������Դ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define MAX_PARA_LEN (sizeof(FPara1Ram) > sizeof(FPara2Ram) ? sizeof(FPara1Ram): sizeof(FPara2Ram))


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
TFPara1 FPara1Ram;
TFPara2 FPara2Ram;
//�������ݿ�
TFPara1* const FPara1 = (TFPara1 *)&FPara1Ram;
TFPara2* const FPara2 = (TFPara2 *)&FPara2Ram;
BYTE CurrArea, CurrSegment, CurrSegTable,CurrRatioBak;
BYTE CurrRatio;

//��������ȫ�ֱ���
TGlobalVariable GlobalVariable;

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------


//0x5008--ʱ�����л�����
//0x5009--��ʱ�α��л�����
//0x500a--���ʵ���л�����
//0x500b--�����л�����
#if( PREPAY_MODE == PREPAY_LOCAL )
const eFreezeType SwitchFreezeIndex[] =
{ 	
	eFREEZE_TIME_ZONE_CHG,
	eFREEZE_DAY_TIMETABLE_CHG, 
	eFREEZE_TARIFF_RATE_CHG, 
	eFREEZE_LADDER_CHG 
};
#else
const eFreezeType SwitchFreezeIndex[] =
{ 
	eFREEZE_TIME_ZONE_CHG, 
	eFREEZE_DAY_TIMETABLE_CHG 
};
#endif

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ˢ�²�����RAM�еĲ���
//
//����: 
//			No[in]					0--FPara1  
//									1--FPara2 
//                    
//����ֵ:  	TRUE: ������ȷ 	FALSE����������
//��ע:  ����eeprom�е����ݸ��µ�ǰram�еĲ��� 
//-----------------------------------------------
BOOL api_FreshParaRamFromEeprom( WORD No )
{
    WORD	MyAddr, MyLength;
    BOOL	Result;

	if( No == 0 )
	{
        MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1);
		MyLength = sizeof(TFPara1);
		Result = api_VReadSafeMem( MyAddr, MyLength, (BYTE *)FPara1 );
	}
	else if( No == 1 )
	{
        MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
		MyLength = sizeof(TFPara2);
		Result = api_VReadSafeMem( MyAddr, MyLength, (BYTE *)FPara2 );
	}
	else
	{
		Result = FALSE;
	}
	
	return Result;
}

//-----------------------------------------------
//��������: �Ѳ���д��FPara1��FPara2��������ǰram�����Ҵ洢����Ӧ��eeprom��
//
//����: 
//			No[in]					0��FPara1  1��FPara2
//			Addr[in]				�ṹ�ڵ�ƫ�Ƶ�ַ
//			Length[in]				Ҫд������ݳ���
//			Ptr[in]					���Ҫд�������
//                    
//����ֵ:  	TRUE: ������ȷ 	FALSE����������
//��ע:   
//-----------------------------------------------
BOOL api_WritePara(WORD No, WORD Addr, WORD Length, BYTE * Ptr)
{
	WORD	MyAddr, MyLength;
	WORD	Result;
	BYTE	*p,Buf[MAX_PARA_LEN+30];

	if( No == 0 )
	{
		MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1);
		MyLength = sizeof(TFPara1);
		p = (BYTE *)FPara1;
	}
	else if( No == 1 )
	{
		MyAddr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
		MyLength = sizeof(TFPara2);
		p = (BYTE *)FPara2;
	}
	else
	{
		return FALSE;
	}
	
	if( (Addr+Length) > MyLength )
	{
		ASSERT( FALSE, 1 );
		return FALSE;
	}
	
	api_VReadSafeMem( MyAddr, MyLength, (BYTE *)Buf );
	
	memcpy( Buf+Addr, (void*)(Ptr), Length);
	
	memcpy(p,Buf, MyLength);
	
	
	Result = api_VWriteSafeMem( MyAddr, MyLength, p );
	
	if( Result != TRUE )
	{
		return FALSE;
	}
	// if (No == 1)
	// {
	// 	memcpy(tfpara2bak,FPara2, sizeof(TFPara2));
	// }
	return TRUE;
}
//-----------------------------------------------
//��������: ��д4Gģ����ز���
//
//����: 
//			Operation[in]			������ʽ ֻ����WRITE��READ
//			Type[in]				��Ҫ�����������ͣ�Type���͵���Ŷ��岻Ҫ�ģ��ǰ�645-2007��˳�����
//			pBuf[in]				�������ݵĻ���
//                    
//����ֵ:  	���ز������ݵĳ��ȣ����дʧ�ܣ�����0
//
//��ע:  	
//-----------------------------------------------
BYTE api_ProcGprsTypePara( BYTE Operation, TGprsTypeEnum Type, BYTE *pBuf)
{
	BYTE ParaLength;
	WORD Result;
	BOOL (*pFunc)(DWORD Addr, WORD Length, BYTE *Buf);
	if( Operation == WRITE )
	{
		pFunc = api_WriteToContinueEEPRom;
	}
	else
	{
		pFunc = api_ReadFromContinueEEPRom;
	}

	Result = TRUE;
	switch ( Type )
	{
		case eTCPIP_IP_Port_APN:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.IP_Port_APN );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.IP_Port_APN), ParaLength, pBuf);
			break;
		case eTCPIP_AreaCode_Ter:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.AreaCode_Ter );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.AreaCode_Ter), ParaLength, pBuf);
			break;
		case eTCPIP_APN_UseName:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.APN_UseName );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.APN_UseName), ParaLength, pBuf);
			break;
		case eTCPIP_APN_PassWord:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.TcpipPara.APN_PassWord );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.TcpipPara.APN_PassWord), ParaLength, pBuf);
			break;

		case eMQTT_IP_Port_APN:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.IP_Port_APN );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.IP_Port_APN), ParaLength, pBuf);
			break;
		case eMQTT_AreaCode_Ter:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.AreaCode_Ter );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.AreaCode_Ter), ParaLength, pBuf);
			break;
		case eMQTT_APN_UseName:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.APN_UseName );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.APN_UseName), ParaLength, pBuf);
			break;
		case eMQTT_APN_PassWord:
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, GprsConRom.MqttPara.APN_PassWord );
			Result = pFunc( GET_CONTINUE_ADDR(GprsConRom.MqttPara.APN_PassWord), ParaLength, pBuf);
			break;
		default:
			ParaLength = 0;
			break;
	}

	if( Operation == WRITE )
	{
		if( Result == FALSE )
		{
			ParaLength = 0;
		}
	}

	return ParaLength;
}

//-----------------------------------------------
//��������: ������һЩ��ز���
//
//����: 
//			Operation[in]			������ʽ ֻ����WRITE��READ
//			Type[in]				��Ҫ�����������ͣ�Type���͵���Ŷ��岻Ҫ�ģ��ǰ�645-2007��˳�����
//			pBuf[in]				�������ݵĻ���
//                    
//����ֵ:  	���ز������ݵĳ��ȣ����дʧ�ܣ�����0
//
//��ע:  	
//-----------------------------------------------
BYTE api_ProcMeterTypePara( BYTE Operation, BYTE Type, BYTE *pBuf )
{
	BYTE ParaLength;
	WORD Result;
	DWORD Data;
	BOOL (*pFunc)(DWORD Addr, WORD Length, BYTE * Buf);
	
	if( Operation == WRITE )
	{
		pFunc = api_WriteToContinueEEPRom;
	}
	else
	{
		pFunc = api_ReadFromContinueEEPRom;
	}
	
	Result = TRUE;
	switch( Type )
	{
		case eMeterCommAddr://ͨѶ��ַ
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CommAddr );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CommAddr ), ParaLength, pBuf );
				// api_delayinit_ble();
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.CommAddr, ParaLength );
			}
			break;
		case eMeterMeterNo://���
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.MeterNo );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.MeterNo ), ParaLength, pBuf );
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.MeterNo, ParaLength );
			}
			break;
		case eMeterPropertyCode://�ʲ�������� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.PropertyCode );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.PropertyCode), ParaLength, pBuf );
			break;
		case eMeterRateVoltage://���ѹ ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.RateVoltage );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.RateVoltage), ParaLength, pBuf );
			break;
		case eMeterRateCurrent://����� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.RateCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.RateCurrent), ParaLength, pBuf );
			break;
		case eMeterMaxCurrent://������ ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MaxCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MaxCurrent), ParaLength, pBuf );
			break;
		case eMeterMinCurrent://��С���� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MinCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MinCurrent), ParaLength, pBuf );
			break;
		case eMeterTurningCurrent://ת�۵��� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.TurningCurrent );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.TurningCurrent), ParaLength, pBuf );
			break;
		case eMeterPPrecision://�й����ȵȼ� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.PPrecision );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.PPrecision), ParaLength, pBuf );
			break;
		#if(SEL_RACTIVE_ENERGY == YES)
		case eMeterQPrecision://�޹����ȵȼ� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.QPrecision );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.QPrecision), ParaLength, pBuf );
			break;
		#endif
		case eMeterActiveConstant://�й����峣��
		#if(SEL_RACTIVE_ENERGY == YES)
		case eMeterReactiveConstant://�޹����峣��
		#endif
			ParaLength = GET_STRUCT_MEM_LEN( TFPara2, ActiveConstant );
			if( Operation == WRITE )
			{
				if( Type == eMeterActiveConstant )
				{
					Result = api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, ActiveConstant ), ParaLength, pBuf );
				}
				else
				{
					Result = api_WritePara( 1, GET_STRUCT_ADDR( TFPara2, ReactiveConstant ), ParaLength, pBuf );
				}
			}
			else
			{
				if( Type == eMeterActiveConstant )
				{
					Data = FPara2->ActiveConstant;
				}
				else
				{
					Data = 	FPara2->ReactiveConstant;
				}
				memcpy( (void*)pBuf, (void*)&Data, ParaLength );
			}
			break;
		case eMeterMeterModel://����ͺ� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MeterModel );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MeterModel), ParaLength, pBuf );
			break;
		// case eMeterProduceDate://�������� ascii��
		// 	ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.ProduceDate );
		// 	Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.ProduceDate), ParaLength, pBuf );
		// 	break;
		case eMeterProtocolVersion://698.45Э��汾��(��������:WORD)
			//Э��汾�ŷ�constֵ ��ֹEE������·�����ֵ
			pBuf[0] = (BYTE)(ProtocolVersionConst&0x00FF);
			pBuf[1] = (BYTE)((ProtocolVersionConst >> 8)&0x00FF);
			ParaLength = 2;
			break;
		case eMeterCustomCode://�ͻ���� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TFPara1, MeterSnPara.CustomCode );
			if( Operation == WRITE )
			{
				Result = api_WritePara( 0, GET_STRUCT_ADDR( TFPara1, MeterSnPara.CustomCode ), ParaLength, pBuf );
			}
			else
			{
				memcpy( pBuf, FPara1->MeterSnPara.CustomCode, ParaLength );
			}
			break;
		case eMeterMeterPosition://���ܱ�λ����Ϣ
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.MeterPosition );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.MeterPosition), ParaLength, pBuf );
			break;
		case eMeterSoftWareVersion://��������汾�� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftWareVersion );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftWareVersion), ParaLength, pBuf );
			break;
		case eMeterSoftWareDate://��������汾����
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftWareDate );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftWareDate), ParaLength, pBuf );
			break;
		case eMeterHardWareVersion://����Ӳ���汾�� ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.HardWareVersion );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.HardWareVersion), ParaLength, pBuf );
			break;
		case eMeterHardWareDate://����Ӳ���汾����
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.HardWareDate );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.HardWareDate), ParaLength, pBuf );
			break;
		case eMeterFactoryCode://���ұ��  ascii��
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.FactoryCode );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.FactoryCode), ParaLength, pBuf );
			break;
		case eMeterSoftRecord://���������
			ParaLength = GET_STRUCT_MEM_LEN( TConMem, ParaConRom.MeterTypePara.SoftRecord );
			Result = pFunc( GET_CONTINUE_ADDR(ParaConRom.MeterTypePara.SoftRecord), ParaLength, pBuf );
			break;
		default:
			ParaLength = 0;
			break;
	}
	
	if( Operation == WRITE )
	{
		if( Result == FALSE )
		{
			ParaLength = 0;
		}
	}
	
	return ParaLength;
}


//-----------------------------------------------
//��������: ��дʱ��ʱ�νڼ��ձ�
//
//����: 
//			Operation[in]	������ʽ ֻ����WRITE��READ
//			Type[in]				
//							D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
//							D6--D0:
//								1 -- TTimeAreaTable
//								2 -- TTimeHolidayTable
//								3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable			
//			pBuf[in]		��������д�����ݵĻ���
//                    
//����ֵ:  	TRUE/FALSE
//
//��ע: ʱ����ʱ�α���Ҫ����48�ֽ�
//		�ڼ�����Ҫ����124�ֽ�
//-----------------------------------------------
static BOOL ReadAndWriteTimeTable(BYTE Operation, WORD Type, BYTE *pTmpArray)
{
	WORD MyAddr, MyLength;

	//Type D7:0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
	if( Type & 0x80 )
	{
		Type &= 0x7f;
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.SecondTimeTable );
	}
	else
	{
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable );
	}

	if( Type > (MAX_TIME_SEG_TABLE+2) )
	{
		return FALSE;
	}

	if( Type == 0 )
	{
		return FALSE;
	}

	if( Type == 1 )//ʱ��
	{
		MyAddr += GET_STRUCT_ADDR( TTimeTable, TimeAreaTable );
		MyLength = sizeof(TTimeAreaTable);
	}
	else if( Type == 2 )//��������
	{
		MyAddr = GET_SAFE_SPACE_ADDR( ParaSafeRom.TimeHolidayTable );
		MyLength = sizeof(TTimeHolidayTable);
	}
	else//ʱ�α�
	{
		MyAddr += GET_STRUCT_ADDR( TTimeTable, TimeSegTable[Type-3] );
		MyLength = sizeof(TTimeSegTable);
	}

	if( Operation == READ )
	{
		return api_VReadSafeMem( MyAddr, MyLength, pTmpArray );
	}
	else
	{
		return api_VWriteSafeMem( MyAddr, MyLength, pTmpArray );
	}
}
//---------------------------------------------------------------
//��������: �õ��л�ʱ�����Է��������������Ƿ�Խ��
//
//����: 	
//			TmpRealTimer[in] --�л�ʱ�䣬��ʽYYYYMMDDHHmm��HEX				
//			pdwTime[out]--�л�ʱ�䣬�����2000��1��1��0ʱ0�ֵ���Է�����
//                    
//����ֵ:  	TRUE--���ݺϸ�FALSE--����Խ��
//
//��ע:   
//---------------------------------------------------------------
BOOL api_GetSwitchTimeRelativeMin( DWORD *pdwTime, TRealTimer TmpRealTimer )
{
	if( TmpRealTimer.wYear == 0 )
	{
		*pdwTime = 0;
	}
	else if( TmpRealTimer.wYear == 9999 )
	{
		*pdwTime = 99999999;
	}
	else if( TmpRealTimer.wYear == 0x9999 )
	{
		*pdwTime = 0x99999999;
	}
	else if( TmpRealTimer.wYear == 0xffff )
	{
		*pdwTime = 0xffffffff;
	}
	else if(( TmpRealTimer.wYear == 2000 ) && ( TmpRealTimer.Mon == 1 ) && ( TmpRealTimer.Day == 1 )
		&& ( TmpRealTimer.Hour == 0 ) && ( TmpRealTimer.Min == 0 ))
	{
		//��Է�������Ϊ1�����0�в������л�
		*pdwTime = 1;
	}
	else
	{
		if( api_CheckMonDay( TmpRealTimer.wYear, TmpRealTimer.Mon, TmpRealTimer.Day ) == FALSE )
		{
			return FALSE;
		}
		if( (TmpRealTimer.Hour > 23) && (TmpRealTimer.Hour != 0xff) )
		{
			return FALSE;
		}
		if( (TmpRealTimer.Min > 59) && (TmpRealTimer.Min != 0xff) )	
		{
			return FALSE;
		}
			
		TmpRealTimer.Sec = 0;//ǿ������Ϊ��
			
		*pdwTime = api_CalcInTimeRelativeMin( &TmpRealTimer );//�������ʱ��
		
		if( *pdwTime == ILLEGAL_VALUE_8F )
		{
			return FALSE;
		}		
	}
	
	return TRUE;
}
//---------------------------------------------------------------
//��������: ��д�л�ʱ��
//
//����: 	
//			Operation[in]	������ʽ ֻ����WRITE��READ
//			Type[in]				
//							0--����ʱ�����л�ʱ��
// 							1--������ʱ���л�ʱ��
// 							2--���׷�ʱ�����л�ʱ��
// 							3--���׽����л�ʱ�� 
// 							4--����
//										
//			pBuf[in]		��������д�����ݵĻ���HEX(YYMMDDhhmm) YYΪ�����ֽ� ��20 ����2017
//                    
//����ֵ:  	TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BOOL api_ReadAndWriteSwitchTime( BYTE Operation, WORD Type, BYTE *pTmpArray )
{
	BYTE i;
	BYTE Buf[sizeof(TSwitchTimePara)];
	TRealTimer TmpRealTimer;
	BYTE TimeBuf[sizeof(TRealTimer)];
	TFourByteUnion dwTime;
	WORD Result;

	if( Type > 4 )
	{
		return FALSE;
	}
	
	memset(Buf,0,sizeof(Buf));
	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.SwitchTimePara ), sizeof(TSwitchTimePara), Buf );
	if( Operation == READ )
	{
		if( Result == FALSE )
		{
			return FALSE;
		}

		memcpy( (void *)&dwTime.b, Buf + Type * sizeof(DWORD), sizeof(DWORD) );
		if(dwTime.d == 0)
		{
			memset(pTmpArray,0x00,sizeof(TRealTimer));
		}
		else if( dwTime.d == 99999999 )//�ֱ�����������ʱ�����л�ʱ������9999/99/99 99:99:99�������api_ToAbsTime ����ת�����ʱ��Ϊ99999999����Ϊ����100��(2018-07-01Ϊ9728640)��ת����Ϊ2100-13-182 10:39:00
		{
			memset(pTmpArray,99,sizeof(TRealTimer));
			pTmpArray[0] = 0x0f; //9999 = 0x270F
			pTmpArray[1] = 0x27;
		}	
		else if( dwTime.d == 0x99999999 )
		{
			memset(pTmpArray,99,sizeof(TRealTimer));
			pTmpArray[0] = 0x0f; //9999 = 0x270F
			pTmpArray[1] = 0x27;
		}
		else if( dwTime.d == 0xffffffff )
		{
			memset(pTmpArray,0xff,sizeof(TRealTimer));
		}		
		else
		{
			Result = api_ToAbsTime( dwTime.d, &TmpRealTimer );
			if(Result == FALSE)
			{
				return FALSE;
			}
			memcpy( pTmpArray, (void*)&TmpRealTimer, sizeof(TRealTimer) );	//YYMMDDhhmm û����
		}
		
		return TRUE;
	}
	else
	{
		//���ݴ�����¼һ���쳣�¼� ���ǻ��������� ��ֹ���ݴ������Ҳ����������
		if( Result == FALSE )
		{
			api_WriteSysUNMsg(SWITCH_TIME_CRC_ERR);
		}
		
		memcpy( (void *)&TmpRealTimer, pTmpArray, 6 );
		if( api_GetSwitchTimeRelativeMin( (DWORD *)&(dwTime.d), TmpRealTimer) == FALSE )
		{
			return FALSE;
		}
		
		if( (dwTime.d == 0)
			||(dwTime.d == 99999999L)
			||(dwTime.d == 0x99999999)
			||(dwTime.d == 0xffffffff) )
		{
			api_ClrRunHardFlag( eRUN_HARD_SWITCH_FLAG1 + Type );
		}

		memcpy( Buf + Type * sizeof(DWORD), dwTime.b, sizeof(DWORD) );
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.SwitchTimePara ), sizeof(TSwitchTimePara), Buf );
		if( Result != TRUE )
		{
			api_WriteSysUNMsg(SWITCH_TIME_CRC_ERR);
			
			return FALSE;
		}
		
		api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
		return Result;
	}


}


//-----------------------------------------------
//��������: ����ǰ�������׵�ʱ����ʱ�α��������ձ�
//
//����: 	Type[in]				
//						D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	����Ӧ�ṹ�е�ƫ��
//			Length[in]	Ҫ�������ݵĳ���
//			pBuf[out]	�������ݵĻ���
//                    
//����ֵ:  	TRUE:��ȷ	FALSE:����
//
//��ע:  	
//-----------------------------------------------
BOOL api_ReadTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE * Buf)
{
	WORD Result;
	BYTE Array[128];

	Result = ReadAndWriteTimeTable(READ, Type, Array);

	if( Result == FALSE )
	{
		return FALSE;
	}

	memcpy((void*)Buf, (void *)&Array[Addr], Length);

	return TRUE;
}


//-----------------------------------------------
//��������: д�뵱ǰ�������׵�ʱ����ʱ�α��������ձ�
//
//����: 	Type[in]				
//						D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�
//						D6--D0:
//							1 -- TTimeAreaTable
//							2 -- TTimeHolidayTable
//							3~MAX_TIME_SEG_TABLE+2 -- TTimeSegTable
//			Addr[in]	����Ӧ�ṹ�е�ƫ��
//			Length[in]	Ҫд�����ݵĳ���
//			pBuf[in]	д�����ݵĻ���
//                    
//����ֵ:  	TRUE:��ȷ	FALSE:����
//
//��ע:  	
//-----------------------------------------------
BOOL api_WriteTimeTable(BYTE Type, WORD Addr, WORD Length, BYTE *Buf)
{
	BYTE Array[128];

	ReadAndWriteTimeTable(READ, Type, Array);

	//�߽��ж�
	if( Length > sizeof(Array) )
	{
		return FALSE;
	}

	//���ڻ����е����ݵ���ȷ��
	//��ʼˢ��
	memcpy((void *)&Array[Addr], (void*)Buf, Length);

	ReadAndWriteTimeTable(WRITE, Type, Array);

	return TRUE;
}

//-----------------------------------------------
//��������: �ж��л�ʱ��
//
//����: 
//			byType[in]				
//				0--����ʱ�����л����� 
//				1--������ʱ�α��л����� 
//				2--���׷�ʱ�����л����� 
//				3--�����ݶ��л����� 
//				4--����
//         
//����ֵ:  	TRUE/FALSE
//��ע: 
//-----------------------------------------------
static BOOL JudgeTableSwitch(BYTE byType)
{
	WORD Result;
	TSwitchTimePara TmpSwitchTimePara;

	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );
	if( Result == FALSE )
	{
		return FALSE;
	}

	//����л�ʱ��Ϊ0��Ӧ�����л�����Ҳ���ܳ�ʼ��Ϊ0��û�л��������л�
	if( (TmpSwitchTimePara.dwSwitchTime[byType] == 0) || ((TmpSwitchTimePara.dwSwitchTime[byType] > 0x03228D1F)) )//0x03228D1F==2099.12.31.23:59:00��Ϊ�˱�֤�л�����Ϊ9999999999ʱҲ��������
	{        
		return FALSE;
	}

	//����л�ʱ��û�������л�
	if( g_RelativeMin < TmpSwitchTimePara.dwSwitchTime[byType] )
	{
		//�л�ʱ����Ч��Һ����ʾ��
		api_SetRunHardFlag(eRUN_HARD_SWITCH_FLAG1+byType);
		return FALSE;
	}

	api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG1+byType);
	TmpSwitchTimePara.dwSwitchTime[byType] = 0;
	
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );

	return TRUE;
}

//-----------------------------------------------
//��������: ������������ֵ�л�����ǰ��
//
//����: 	Type[in] 0--ʱ����  1--ʱ�α�
//                    
//����ֵ:  	TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
static BOOL CopyBackupToCurrent( WORD Type )
{
	BYTE i,Length,SuitNum;
	BYTE Array[76];//�����������ݱ� 

	if( Type == 0 )
	{
		Type = 1;
		Length = sizeof(TTimeAreaTable);
		SuitNum = 1;
	}
	else if( Type == 1 )
	{
		Type = 3;
		Length = sizeof(TTimeSegTable);
		SuitNum = MAX_TIME_SEG_TABLE;
	}
	#if( PREPAY_MODE == PREPAY_LOCAL )
	else if( Type == 2 )//�����л�
	{
		SuitNum = 0;
		Length = sizeof(TRatePrice);
		
		api_ReadPrePayPara( eBackupRateTable, Array );
        api_WritePrePayPara( eCurRateTable, Array );//�����и��ķ��ʵ�ۻ��Զ������жϵ��
	}
	else if( Type == 3 ) //�����л�
	{
		SuitNum = 0;
		Length = sizeof(TLadderPrice);
		
		api_ReadPrePayPara( eBackupLadderTable, Array );
        api_WritePrePayPara( eCurLadderTable, Array );//�����и��ķ��ʵ�ۻ��Զ������жϵ��
	}
	#endif
	else
	{
		return FALSE;
	}
	
	for(i=0; i<SuitNum; i++)
	{
		api_ReadTimeTable( 0x80+Type+i, 0, Length, Array );
	
		api_WriteTimeTable( 0x00+Type+i, 0, Length, Array );
	}
	
	return TRUE;
}


//-----------------------------------------------
//��������: ÿ�����ж��л�ʱ��
//
//����: 	��
//			
//����ֵ:  	��
//��ע: ÿ���ӵ���һ�Σ�ֻ�ö����־�������������ݣ��������ݺ�������������ʱ�ٰѱ�־���
//
//-----------------------------------------------
static void ProcTableSwitchTask(void)
{
	BYTE i;

//	0--����ʱ�����л����� 
//	1--������ʱ�α��л����� 
//	2--���׷�ʱ�����л����� 
//	3--�����ݶ��л����� 
	#if( PREPAY_MODE == PREPAY_LOCAL )
	for( i=0; i<4; i++ )
	#else
	for( i=0; i<2; i++ )
	#endif
	{
		if( JudgeTableSwitch(i) == TRUE )
		{
			//������Ӧ���л�����
			CopyBackupToCurrent( i ); //!!!!! �˴�ֻ�ܽ���ʱ���� ��ʱ�α��л������ܽ��з��ʺ��ݶȵ��л� �˴�ע��!!!!!! --jwh
			
			// api_SetFreezeFlag( SwitchFreezeIndex[i], 0 );
		}
	}
}


//-----------------------------------------------
//��������: Ч�鵱ǰRAM���� FPara1/FPara2/g_WarnDispPara
//
//����: 	
//                    
//����ֵ:  	TRUE/FALSE
//
//��ע: �ϵ��ÿ����Ч��RAM����  
//-----------------------------------------------
void WatchPara(void)
{
	//���ӵ�ǰ����
	//������1У�����
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{
		//��EEProm�ָ�����
		if( api_FreshParaRamFromEeprom(0) != TRUE )
		{
			api_SetError(ERR_FPARA1);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA1);
	}
	
	if( FPara1->TimeZoneSegPara.Ratio > MAX_RATIO )
	{
		FPara1->TimeZoneSegPara.Ratio = 4;
	}
	
	//������2У�����
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		//��EEProm�ָ�����
		if( api_FreshParaRamFromEeprom(1) != TRUE )
		{
			memcpy( &FPara2->CommPara, &CommParaConst, sizeof(TCommPara) );	//CRC���� FPara�е�ͨ�Ų����ظ�
			api_SetError(ERR_FPARA2);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA2);
	}

	//���ӵ�ǰ����
	if( CurrRatio != CurrRatioBak )
	{
		//���¼������2
		api_CheckTimeTable(TRUE);
		// api_SetError(ERR_RATIO_NOW);
	}
}
//-----------------------------------------------
//��������: �õ���ǰ����
//
//����: 	��
//                   
//����ֵ:   BYTE:���ص�ǰ����
//
//��ע:   
//-----------------------------------------------
BYTE api_GetCurRatio( void )
{
	return CurrRatio;
}
//-----------------------------------------------
//��������: ������ѭ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_ParaTask( void )
{
	BYTE tSec;
	
	//�Ƿ�1��
	if( api_GetTaskFlag(eTASK_PARA_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_PARA_ID,eFLAG_SECOND);
		
		FunctionConst(PARA_TASK1);
		//����Ϊ1����ִ��һ��
		api_GetRtcDateTime(DATETIME_ss,&tSec);
		if( tSec == (eTASK_PARA_ID*3+3) )
		{
			//����Flash�еĲ���
			WatchPara();
		}
	}
	
	
	if( (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_MINUTE ) == TRUE) || 
	   (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_SWITCH_JUDGE ) == TRUE)|| 
	   (api_GetTaskFlag( eTASK_PARA_ID, eFLAG_TIME_CHANGE ) == TRUE))
	{
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_MINUTE );
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_SWITCH_JUDGE );
		api_ClrTaskFlag( eTASK_PARA_ID, eFLAG_TIME_CHANGE );
		
		FunctionConst(PARA_TASK2);
		// ProcTableSwitchTask();
		
		//����Ƿ���ʱ�α����Ҫ��
		//�������DealMinSetDlt645_2007FreezeStatus֮������ʱ�α��л���ͻ������л����ʣ���ǰ����ʱ1�������л�����
		// api_CheckTimeTable(TRUE);
	}
}

//-----------------------------------------------
//��������: ���������ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_PowerUpPara( void )
{
    BYTE result = 0;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	//��E2��ȡ��ǰ���ݱ�����ǰ���ݱ�,�����Y�ã��ڳ�ʼ������ǰ��ȡ()
	//��ֹ��������ݽ��㽫E2�е�ǰ���ݱ��ǣ�����Ų��λ�ã�����--��ا��
	api_ReadPowerLadderTable();
	#endif
	//ϵͳ�Լ�
	//RAM�еĲ�������FPara1��FPara2��CurrRatio��
	WatchPara();
	//��UIk����
	result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKSafeRom.Uk[0]), sizeof(TSampleUIK), (BYTE *)&UIFactor);

	api_CheckTimeTable(FALSE);
	
	ProcTableSwitchTask();
	
	//����Ƿ���ʱ�α����Ҫ��
	//�������DealMinSetDlt645_2007FreezeStatus֮������ʱ�α��л���ͻ������л����ʣ���ǰ����ʱ1�������л�����
	api_CheckTimeTable(TRUE);
	//�๦�ܶ��ӳ�ʼ��
	api_MultiFunPortSet( eCLOCK_PULS_OUTPUT );
}

//-----------------------------------------------
//��������: �ж�ʱ������ʱ���Ƿ�Ϸ�
//
//����: 
//			Type[in]				0��ʱ����  1��ʱ�α�
//          pAreaAndSegment[in]		3�ֽڣ���ǰʱ����ǰʱ��
//         
//����ֵ:  	TRUE/FALSE
//��ע: 
//-----------------------------------------------
static BOOL CheckAreaAndSegment( BYTE Type, BYTE *pAreaAndSegment )
{
	BYTE i;

//	for(i=0; i<3; i++)
//	{
//		if( lib_CheckBCD( pAreaAndSegment[i] ) == FALSE )
//		{
//			return FALSE;
//		}
//	}

	if( Type == 0 )//ʱ��
	{
		for(i=0; i<3; i++)
		{
			if( pAreaAndSegment[i] == 0x00 )
			{
				return FALSE;
			}
		}

		if( (pAreaAndSegment[2] > MAX_TIME_SEG_TABLE) 
			|| (pAreaAndSegment[1] > MAX_VALID_DAY) 
			|| (pAreaAndSegment[0] > MAX_VALID_MONTH) )
		{
			return FALSE;
		}
	}
	else//ʱ��
	{
		if( (pAreaAndSegment[2] > MAX_RATIO) 
			|| (pAreaAndSegment[2] == 0x00) 
			|| (pAreaAndSegment[1] > MAX_VALID_MINUTE) 
			|| (pAreaAndSegment[0] > MAX_VALID_HOUR) )
		{
			return FALSE;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ��ʱ�����ʱ�α��������
//
//����: 
//			AreaAndSegmentNum[in]	ʱ������ʱ������ �� ʱ�α���ʱ�θ���
//          pAreaAndSegment[in]		ʱ�����ʱ�α�
//         
//����ֵ:  	��
//��ע: 
//-----------------------------------------------
static void SortAreaAndSegment( BYTE AreaAndSegmentNum, BYTE *pAreaAndSegment )
{
	BYTE i,j,i0,i1,i2,j0,j1,j2;
	BYTE Temp;

	for(i=0; i<AreaAndSegmentNum; i++)
	{
		for(j=i; j<AreaAndSegmentNum; j++)
		{
			i0 = i*3+0;
			j0 = j*3+0;
			i1 = i*3+1;
			j1 = j*3+1;
			i2 = i*3+2;
			j2 = j*3+2;

			//����մ��ڣ����յ��ڵ�ʱ����
			if( (pAreaAndSegment[i0] > pAreaAndSegment[j0])||
				( (pAreaAndSegment[i0] == pAreaAndSegment[j0])&&(pAreaAndSegment[i1] > pAreaAndSegment[j1]) ) )
			{
				Temp = pAreaAndSegment[i0];
				pAreaAndSegment[i0] = pAreaAndSegment[j0];
				pAreaAndSegment[j0] = Temp;

				Temp = pAreaAndSegment[i1];
				pAreaAndSegment[i1] = pAreaAndSegment[j1];
				pAreaAndSegment[j1] = Temp;

				Temp = pAreaAndSegment[i2];
				pAreaAndSegment[i2] = pAreaAndSegment[j2];
				pAreaAndSegment[j2] = Temp;
			}
		}
	}
}

//-----------------------------------------------
//��������: ����ʱ�����ʱ�α�Ĺ�������
//
//����: 
//			Type[in]				0��ʱ����  1��ʱ�α�
//          Num[in]					ʱ������ʱ�α���
//			pBuf[in]				ʱ�������ʱ�α�          
//����ֵ:  	������������ʱ�α�Ż������
//��ע: 
//-----------------------------------------------
static BYTE SearchTabel( BYTE Type, BYTE Num, BYTE *pBuf )
{
	BYTE CurrValue,j,k;
	WORD i,l,wDateTime;
	BYTE DateBuf[4];
	BYTE *p;

	p = (BYTE *)pBuf;
	l = FALSE;
	j = Num;

	//--------�ѷǷ���ʱ�λ�ʱ��ȥ��
	k = 0;
	for(i=0; i<j; i++)
	{
		if( CheckAreaAndSegment( Type, &p[i*3] ) == FALSE )
		{
			continue;
		}
		else
		{
			memcpy( &p[k*3], &p[i*3], 3 );

			k++;
		}
	}

	//--------����
	j = k;
	SortAreaAndSegment( j, p );

	//--------��ȡ��ǰ����ʱ��
	api_GetRtcDateTime(DATETIME_MMDDhhmm,DateBuf);	
	if( Type == 0 )//ʱ����--����
	{
		wDateTime = DateBuf[0];
		wDateTime <<= 8;
		wDateTime += DateBuf[1];
	}
	else//ʱ�α�--ʱ��
	{
		wDateTime = DateBuf[2];
		wDateTime <<= 8;
		wDateTime += DateBuf[3];
	}

	//-------�ѵ�ǰʱ�α�Ż��߷��ʺ� CurrValue
	i = p[0];
	i <<= 8;
	i += p[1];

	//��ǰ����/ʱ�仹û����һʱ��/ʱ��
	if( wDateTime < i )//��������һ��ʱ��
	{
		l = TRUE;
	}
	//��ǰ����/ʱ���Ѿ�����һʱ��/ʱ��
	else
	{
		i = p[3*j-3];
		i <<= 8;
		i += p[3*j-2];
		//��ǰ����/ʱ���Ѿ������һʱ��/ʱ��
		if( wDateTime >= i )
		{
			l = TRUE;
		}
	}

	//�����ǰ����/ʱ��û����һʱ��/ʱ�Σ��Ѿ������һʱ��/ʱ��
	if( l == TRUE )
	{
		//kȡ���һʱ��/ʱ�ε�ʱ�α�Ż������
		k = p[3*j-1];
		//CurrValue ȡ ʱ����/ʱ�α���
		CurrValue = j;
	}
	else
	{
		//����������
		p += 3;//��ΪС�ڵ�һ��ʱ�������ǰ���Ѿ��ж���

		for(i=1; i<j; i++)//��Ϊǰ���Ѿ�������һ��ʱ������������Ӧ����һ��ʱ��
		{
			l = p[0];
			l <<= 8;
			l += p[1];
			if( wDateTime < l )
			{
				//��ǰ����/ʱ��û�����ʱ��/ʱ��
				//kȡ��һ��ʱ��/ʱ�ε�ʱ�α�Ż������
				p -= 3;
				k = p[2];
				l = TRUE;

				break;
			}

			p += 3;
		}

		//û�ҵ�ʱ����ȡ��һ��ʱ����ʱ�α��
		if( l == FALSE )
		{
			k = p[2];
			CurrValue = 1;
		}
		else
		{
			CurrValue = i;
		}
	}

	//�жϵ�ǰʱ�α���뱸��ʱ�α���Ƿ�һ��
	//�жϵ�ǰ���ʺ��뱸�ݷ��ʺ��Ƿ�һ��
	if( Type == 0 )//ʱ���� 
	{
		if( CurrValue != CurrArea )
		{
			CurrArea = CurrValue;
		}
	}
	else//ʱ�α� 
	{
		if( CurrSegment != CurrValue )
		{
			api_MultiFunPortCtrl( eSEGMENT_SWITCH_OUTPUT );

			CurrSegment = CurrValue;
		}
	}

	return k;
}


//-----------------------------------------------
//��������: ���ʱ�α�ÿ���ӵ���һ��
//
//����: 
//			Type[in]				TRUE ת��  FALSE ��ת��
//                    
//����ֵ:  	��
//��ע: ������ǰ�ڼ��ձ��������֣�ʱ����ʱ�α��ҵ���ǰʱ��  
//-----------------------------------------------
void api_CheckTimeTable(WORD Type)
{
	WORD Addr;
	WORD i, j, k;
	BYTE DateBuf[4];
	BYTE * p;
    BYTE Array[128];

	//ʱ�α��
	k = 0xff00;

	//�������մ���
	{
		j = FPara1->TimeZoneSegPara.HolidayNum;		//����������
	
		if( (j==0xff) || (j==0xffff) )				//����Ҫ�󣺹�������������ΪFF����ʾ������
		{
			j = 0;
		}
	
		if( j != 0 )
		{		
			if( j > MAX_HOLIDAY )//��������������������Ĵ���
			{
				j = MAX_HOLIDAY;
				//��������Ϣ
			}
			
			//���������ձ�
			//ÿ���ʽΪ-----��(2�ֽ�)���£��գ����� ʱ�α�ţ�3��2��1��0��
			ASSERT( sizeof(TTimeHolidayTable) < sizeof(Array), 1 );
			if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.TimeHolidayTable), sizeof(TTimeHolidayTable), Array) == TRUE )
			{
				p = Array;
		
				//�ж��Ƿ�Ϊ��������
				api_GetRtcDateTime(DATETIME_20YYMMDD,DateBuf);
				for(i=0; i<j; i++)
				{
					//�Ƚ�������
					if( memcmp( DateBuf, p, 4 ) == 0 )
					{
						k = p[sizeof(TTimeHoliday)-1];
		
						if( k == 0 )
						{
							k = 0xff00;
							continue;
						}
						else
						{
							break;
						}
					}
					
					p += sizeof(TTimeHoliday);
				}
			}
			else//��ȡʧ���޼��ձ�
			{
				k = 0xff00;
			}

		}
	}

	//û�ҵ��ڼ���
	//�����մ���
	if( k == 0xff00 )
	{
		//�����մ��� ������Ϊ0
		//�ж��Ƿ�Ϊ������
		api_GetRtcDateTime( DATETIME_WW, (BYTE*)&j );
		if( ((FPara1->TimeZoneSegPara.WeekStatus>>j) & 0x01) == 0 )
		{
			k = FPara1->TimeZoneSegPara.WeekSeg;

			if( k == 0 )
			{
				k = 0xff00;
			}
		}
	}

	if( k != 0xff00 )//�����ա��ڼ�������ҵ�ʱ�α���ʱ�α�Ϸ��Խ����жϣ�������Ϸ�����ʱ�����������
	{
		if( (k==0) || (k>FPara1->TimeZoneSegPara.TimeSegTableNum) )
		{
			k = 0xff00;
		}
	}
	
	//û�ҵ�������
	//ʱ������
	if( k == 0xff00)
	{
		//����ʱ����
		j = FPara1->TimeZoneSegPara.TimeZoneNum;

		//ʱ�������ж��Ƿ�ʱ��
		Addr = GET_STRUCT_ADDR(TTimeTable,TimeAreaTable.TimeArea[0][0]);//��ʼ��ַ
		//��Ϊ�˵�ǰ�׺ͱ����׵ķ�ʽ�����ÿ��ֻҪ����ǰ�׼���
		if( api_VReadSafeMem((GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable )+Addr), sizeof(TTimeAreaTable), Array) == FALSE )
		{
			k = 0;//Ĭ��ʹ�õ�һ��
			CurrArea = 1;

		}
		else
		{
			if( (j == 0)||(j>MAX_TIME_AREA) )//Ϊ���������ʱ������ȡ��һʱ��
			{
				k = Array[0];
				CurrArea = 1;
			}
			else
			{
				k = SearchTabel( 0, j, Array );
			}
		}
		

	}

	//ʱ�δ���
	j = FPara1->TimeZoneSegPara.TimeSegNum;
	if( (j==0)||(j>MAX_TIME_SEG) )//���ʱ����Ϊ0��������ʱ�θ���������Ϊ���ʱ�θ���
	{
		j = MAX_TIME_SEG;
	}

	//��ʱ�α�
	if( (k==0)||(k>FPara1->TimeZoneSegPara.TimeSegTableNum) )//MAX_TIME_SEG_TABLE) )//ʱ�α��Ϊ0��������ʱ�α���ʱȡ��һʱ�α�
	{
		k = 0;
	}
	else
	{
		k -= 1;
	}
	//��ǰʱ�α��
	CurrSegTable = k + 1;

	Addr = GET_STRUCT_ADDR(TTimeTable,TimeSegTable[k].TimeSeg[0][0]);
	if(api_VReadSafeMem((GET_SAFE_SPACE_ADDR( ParaSafeRom.FirstTimeTable )+Addr), sizeof(TTimeSegTable), Array) == FALSE)
	{
		//���ʴ���
		if( (CurrRatio==0) || (CurrRatio>MAX_RATIO) || (CurrRatio>FPara1->TimeZoneSegPara.Ratio) )//������ʺ�Ϊ0��������������򵱷���1����
		{
			CurrRatio = DefCurrRatioConst;//ȱʡ��Ϊ��ƽʱ��
			#if( PREPAY_MODE == PREPAY_LOCAL )
			//���ʵ������ˢ��
			api_SetUpdatePriceFlag( eChangeCurRatePrice );
			#endif
		}
		//���ָ����ݣ�����������뱸�ݲ�ͬ�ᵼ��1�����ж�2��
		return ;
	}

	k = SearchTabel( 1, j, Array );

	//���ʴ���
	j = FPara1->TimeZoneSegPara.Ratio;
	if( (k==0) || (k>MAX_RATIO) || (k>j) )//������ʺ�Ϊ0��������������򵱷���1����
	{
		k = DefCurrRatioConst;//ȱʡ��Ϊ��ƽʱ��
	}

	if( CurrRatio != k )
	{
		if( (CurrRatio==0) || (CurrRatio>MAX_RATIO) )//������ʺ�Ϊ0��������������򵱷���1����
		{
			CurrRatio = k;
		}
		if( Type == TRUE )
		{
			api_SwapEnergy();
		}
		
		#if( PREPAY_MODE == PREPAY_LOCAL )
		//���ʵ������ˢ��
		api_SetUpdatePriceFlag( eChangeCurRatePrice );
		#endif
		
		//�л�����
		CurrRatio = k;
		CurrRatioBak = k;
	}
}

//-----------------------------------------------
//��������: ��ʼ��4Gģ�����
//
//����: 	��
//			
//����ֵ:  	��
//��ע: 	ֻ�г���ģʽ����������ֻ������
//-----------------------------------------------
static void FactoryInitGprsPara( void )
{
	BYTE Buf[33];
	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
	{
		return;
	}
	memset( Buf, 0, sizeof(Buf) );
	//IP��ַ �˿ں� APN
	memcpy(Buf, TCPIP_IP_Port_APNConst,sizeof(TCPIP_IP_Port_APNConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_IP_Port_APN, Buf);
	//�������� �ն˵�ַ
	memcpy(Buf, TCPIP_AreaCode_TerConst, sizeof(TCPIP_AreaCode_TerConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_AreaCode_Ter, Buf);
	//APN�û���
	memcpy(Buf, TCPIP_APN_UseNameConst, sizeof(TCPIP_APN_UseNameConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_APN_UseName, Buf);
	//APN����
	memcpy(Buf, TCPIP_APN_PassWordConst, sizeof(TCPIP_APN_PassWordConst));
	api_ProcGprsTypePara(WRITE, eTCPIP_APN_PassWord, Buf);

	//IP��ַ �˿ں� APN
	memcpy(Buf, MQTT_IP_Port_APNConst,sizeof(MQTT_IP_Port_APNConst));
	api_ProcGprsTypePara(WRITE, eMQTT_IP_Port_APN, Buf);
	//�������� �ն˵�ַ
	memcpy(Buf, MQTT_AreaCode_TerConst, sizeof(MQTT_AreaCode_TerConst));
	api_ProcGprsTypePara(WRITE, eMQTT_AreaCode_Ter, Buf);
	//APN�û���
	memcpy(Buf, MQTT_APN_UseNameConst, sizeof(MQTT_APN_UseNameConst));
	api_ProcGprsTypePara(WRITE, eMQTT_APN_UseName, Buf);
	//APN����
	memcpy(Buf, MQTT_APN_PassWordConst, sizeof(MQTT_APN_PassWordConst));
	api_ProcGprsTypePara(WRITE, eMQTT_APN_PassWord, Buf);	
}
//-----------------------------------------------
//��������: ��ʼ��������йصĲ���
//
//����: 	��
//			
//����ֵ:  	��
//��ע: 	ֻ�г���ģʽ����������ֻ������
//-----------------------------------------------
static void FactoryInitMeterPara( void )
{
	BYTE Buf[32];//����С�ڵ���λ��12�ֽ�wlk
	
	//ֻ�г���ģʽ����������ֻ������
	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
	{
		return;
	}
	memset( Buf, 0, sizeof(Buf) );
	
	//���ѹ ascii��     
	memcpy(Buf,RateVoltageConst,sizeof(RateVoltageConst));
	api_ProcMeterTypePara(WRITE, eMeterRateVoltage, Buf );
	//����� ascii��     
	memcpy(Buf,RateCurrentConst,sizeof(RateCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterRateCurrent, Buf );
	//������ ascii��     
	memcpy(Buf,MaxCurrentConst,sizeof(MaxCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterMaxCurrent, Buf );
	//��С���� ascii�� 
	memcpy(Buf,MinCurrentConst,sizeof(MinCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterMinCurrent, Buf );
	//ת�۵��� ascii��     
	memcpy(Buf,TurnCurrentConst,sizeof(TurnCurrentConst));
	api_ProcMeterTypePara(WRITE, eMeterTurningCurrent, Buf );
	//�й����ȵȼ� ascii��
	memcpy(Buf,PPrecisionConst,sizeof(PPrecisionConst));
	api_ProcMeterTypePara(WRITE, eMeterPPrecision, Buf );
	//�޹����ȵȼ� ascii�� 
	#if(SEL_RACTIVE_ENERGY == YES)
	memcpy(Buf,QPrecisionConst,sizeof(QPrecisionConst));
	api_ProcMeterTypePara(WRITE, eMeterQPrecision, Buf );
	#endif
	//�й����峣��         
	memcpy(Buf,(BYTE*)&ActiveConstantConst,sizeof(ActiveConstantConst));
	api_ProcMeterTypePara(WRITE, eMeterActiveConstant, Buf );
	#if(SEL_RACTIVE_ENERGY == YES)
	//�޹����峣��          �޹����ú��й�һ��������
	memcpy(Buf,(BYTE*)&ActiveConstantConst,sizeof(ActiveConstantConst));
	api_ProcMeterTypePara(WRITE, eMeterReactiveConstant, Buf );
	#endif
	//����ͺ� ascii��     	
	memcpy(Buf,MeterModelConst,sizeof(MeterModelConst));
	api_ProcMeterTypePara(WRITE, eMeterMeterModel, Buf );	
	//���ܱ�λ����Ϣ 	
	memcpy(Buf,MeterPositionConst,sizeof(MeterPositionConst));
	api_ProcMeterTypePara(WRITE, eMeterMeterPosition, Buf );
	//698.45Э��汾��(��������:WORD)   
	memcpy(Buf,(BYTE*)&ProtocolVersionConst,sizeof(ProtocolVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterProtocolVersion, Buf );
	//��������汾�� ascii�� 
	memcpy(Buf,SoftWareVersionConst,sizeof(SoftWareVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftWareVersion, Buf );
	//��������汾����
	memcpy(Buf,SoftWareDateConst,sizeof(SoftWareDateConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftWareDate, Buf );
	//����Ӳ���汾�� ascii�� 
	memcpy(Buf,HardWareVersionConst,sizeof(HardWareVersionConst));
	api_ProcMeterTypePara(WRITE, eMeterHardWareVersion, Buf );
	//����Ӳ���汾����
	memcpy(Buf,HardWareDateConst,sizeof(HardWareDateConst));
	api_ProcMeterTypePara(WRITE, eMeterHardWareDate, Buf );

	//���ұ��  ascii��       
	memcpy(Buf,FactoryCodeConst,sizeof(FactoryCodeConst));
	api_ProcMeterTypePara(WRITE, eMeterFactoryCode, Buf );
	//���������  ASCII��   
	memcpy(Buf,SoftRecordConst,sizeof(SoftRecordConst));
	api_ProcMeterTypePara(WRITE, eMeterSoftRecord, Buf );
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//�ͻ����
	memset(Buf,0x00,sizeof(FPara1->MeterSnPara.CustomCode));
	api_ProcMeterTypePara(WRITE, eMeterCustomCode, Buf );
	#endif
}


//-----------------------------------------------
//��������: ��ʼ����Լ����
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע: 	
//-----------------------------------------------
static void FactoryInitDlt645Password(void)
{
	TMuchPassword TmpMuchPassword;

	//�����ʼ��
	memcpy( &TmpMuchPassword, (void*)&MuchPasswordConst, sizeof(TMuchPassword) );

	//д�����룬�Լ�����У��
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
}


//-----------------------------------------------
//��������: �л�ʱ���ʼ��Ϊ0
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע: 	
//-----------------------------------------------
static void FactoryInitTableSwitchTime(void)
{
	// TSwitchTimePara TmpSwitchTimePara;

	// //��ʼ���л�ʱ�䶼Ϊ0����ֹ��ʼ��ʱ�����л�
	// memcpy( (BYTE *)&TmpSwitchTimePara, (BYTE*)&SwitchTimeParaConst, sizeof(TSwitchTimePara) );
	// //д���л�ʱ�䣬�Լ�����У��
	// api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara), sizeof(TSwitchTimePara), (BYTE *)&TmpSwitchTimePara );

}

//-----------------------------------------------
//��������: ��ʼ����������
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע: 	
//-----------------------------------------------
static void FactoryInitHoliday( void )
{
    BYTE i;
	TTimeHolidayTable TmpTimeHolidayTable;
	//��ʼ���������ձ�ΪFFFFFFFFFFFFFF01
	memset( (BYTE *)&TmpTimeHolidayTable, 0xff, sizeof(TTimeHolidayTable) );
    for( i = 0; i < MAX_HOLIDAY; i++ )
    {
        TmpTimeHolidayTable.TimeHoliday[i].HolidaySegNum = 0x01;
    }
    
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.TimeHolidayTable), sizeof(TTimeHolidayTable), (BYTE *)&TmpTimeHolidayTable );
}


//-----------------------------------------------
//��������: ���ع���ʱ��
//
//����: 	��
//			
//����ֵ:  	��
//
//��ע: 	
//-----------------------------------------------
static void FactoryInitBatteryTime(void)
{
	DWORD BattTime = 0;
	
	//д���ع���ʱ��
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(BatteryTime), sizeof(DWORD), (BYTE *)&BattTime);
}

//-----------------------------------------------
//��������: ������ʱ���ʼ��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	  ��ʼ������
//-----------------------------------------------
void FactoryInitBilling(void)
{
	TBillingPara TmpBillingPara;
	
	memcpy( &TmpBillingPara.MonSavePara, &MonBillParaConst, sizeof(TMonSavePara)*MAX_MON_CLOSING_NUM );
	
	//д���л�ʱ�䣬�Լ�����У��
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.BillingPara), sizeof(TBillingPara), (BYTE*)&TmpBillingPara );
}


//-----------------------------------------------
//��������: ��ʼ��������Ĭ��ֵ
//
//����: 	��
//                    
//����ֵ:  	��
//��ע:   
//-----------------------------------------------
void FactoryInitPara(void)
{
	BYTE i;
	WORD Addr;

	api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2), sizeof(TFPara2), (BYTE*)FPara2 );

	memcpy((BYTE*)&FPara2->CommPara.I485,(BYTE*)&CommParaConst,sizeof(TCommPara));
	// memcpy((BYTE*)&tfpara2bak->CommPara.I485,(BYTE*)&CommParaConst,sizeof(TCommPara));//���¸ñ���
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}
	//�й���Ϸ�ʽ������ ����й�
	memcpy((BYTE*)&FPara2->EnereyDemandMode.byActiveCalMode, &EnereyDemandModeConst, sizeof(TEnereyDemandMode));
	
	Addr = GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara2);
	api_VWriteSafeMem( Addr, sizeof(TFPara2), (BYTE*)FPara2 );
	CLEAR_WATCH_DOG;

    api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.FPara1), sizeof(TFPara1), (BYTE*)FPara1 );
	
	memcpy((BYTE*)&FPara1->TimeZoneSegPara,(BYTE*)&TimeZoneSegParaConst,sizeof(TTimeZoneSegPara));
	Addr = GET_SAFE_SPACE_ADDR( ParaSafeRom.FPara1 );
	api_VWriteSafeMem( Addr, sizeof(TFPara1), (BYTE*)FPara1 );
	CLEAR_WATCH_DOG;
	
	//д��ǰ��ʱ����
	// api_WriteTimeTable(0x01, GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]), sizeof(TTimeAreaTable), (BYTE*)&TimeAreaTableConst1);
	//д������ʱ����
	// api_WriteTimeTable(0x81, GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]), sizeof(TTimeAreaTable), (BYTE*)&TimeAreaTableConst2);
	
	for(i=0;i<(MAX_TIME_SEG_TABLE*2);i++)
	{
		//д��ǰ��8����ʱ�α�
		if( i < MAX_TIME_SEG_TABLE )
		{
			// if( TimeSegTableFlag[i] == 1 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
			// else if( TimeSegTableFlag[i] == 2 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst2);
			// }
			// else if( TimeSegTableFlag[i] == 3 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst3);
			// }
			// else if( TimeSegTableFlag[i] == 4 )
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst4);
			// }
			// else
			// {
			// 	api_WriteTimeTable(0x03+i, GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
		    

		}
		else//д������8����ʱ�α�
		{

			// if( TimeSegTableFlag[i] == 1 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
			// else if( TimeSegTableFlag[i] == 2 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst2);
			// }
			// else if( TimeSegTableFlag[i] == 3 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst3);
			// }
			// else if( TimeSegTableFlag[i] == 4 )
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst4);
			// }
			// else
			// {
			// 	api_WriteTimeTable((0x83+i-MAX_TIME_SEG_TABLE), GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]), sizeof(TTimeSegTable), (BYTE*)&TimeSegTableConst1);
			// }
		}
	}

	api_FactoryInitRunHardFlag();
	api_FactoryInitProHardFlag();

	//�����ʼ��,��ʼ������
	FactoryInitDlt645Password();
	
	// FactoryInitTableSwitchTime();

	FactoryInitGprsPara();

	FactoryInitMeterPara();
	
	#if( MAX_PHASE == 3 )
	api_InitSamplePara();
	#endif

	#if(SEL_EVENT_DI_CHANGE == YES)
	api_FactoryInitDIPara();
	#endif
		
	FactoryInitHoliday();
	
	// api_ClrProSafeModePara(3);//������ʽ��ȫģʽ����

	api_InitMessageTransPara();
	
	FactoryInitBatteryTime();
	
	FactoryInitBilling();
}

//-----------------------------------------------
//��������: ����ʼ��
//
//����:
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_FactoryInitTask( void )
{	
	if(SelEESoftAddrConst == YES)
	{
		api_FactoryInitEEPRomAddr();
	}
	//���Ҫ����LCD���ڳ�ʼ��ǰ�棬������LCD��ʼ���õĲ�����д
	FactoryInitPara();
	
	api_FactoryInitSyswatch();

	// api_FactoryInitFreeze();

	// api_FactoryInitEvent();
	// api_FacinitBT();
    
	#if (SEL_SEARCH_METER == YES )
	api_InitSchClctMeter();
	#endif
    #if (SEL_TOPOLOGY == YES )	 
    api_TopoParaInit();
    #endif
	
	// api_FactoryInitLcd();
	
//	ClearReport( eFACTORY_INIT );//�����������ϱ�
	
	// FactoryInitRelay();
	
	api_FactoryInitRtc();
	
	// api_FactoryInitPrePay();

	FactoryInitProtocol( );
	
	// api_ModparaInit();

	api_FactoryInitSample();
	//��ʼ������ʱ������¼��ʼ�������¼���������������ʼ��ʱ��������
	if(api_CheckSysVol(eOnRunSpeedDetectPowerMode) == TRUE)
	{
		api_WriteFreeEvent(EVENT_SYS_INIT_BEGIN, 0);
	}
}


//-----------------------------------------------
//��������: �����峣�����кϷ����ж�
//
//����:		��
//						
//����ֵ:	�й����峣��
//		   
//��ע:
//-----------------------------------------------
DWORD api_GetActiveConstant( void )
{
	if( (FPara2->ActiveConstant<10) 
		|| (FPara2->ActiveConstant>200000) 
		|| ((FPara2->ActiveConstant%10)!=0) )
	{
		ASSERT( 0, 0 );
		return ActiveConstantConst;
	}
	
	return FPara2->ActiveConstant;
}

//-----------------------------------------------
//��������: ��Para1������E2�ָ�
//
//����:		��
//						
//����ֵ:	
//		   
//��ע:�͹����ϵ��дʱ��ǰ����
//-----------------------------------------------
void api_ReadPara1FromEE(void)
{
	//���ӵ�ǰ����
	//������1У�����
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara1), sizeof(TFPara1), UN_REPAIR_CRC ) == FALSE )
	{
		//��EEProm�ָ�����
		if( api_FreshParaRamFromEeprom(0) != TRUE )
		{
			api_SetError(ERR_FPARA1);
		}
	}
	else
	{
		api_ClrError(ERR_FPARA1);
	}
	
	if( FPara1->TimeZoneSegPara.Ratio > MAX_RATIO )
	{
		FPara1->TimeZoneSegPara.Ratio = 4;
	}
}



