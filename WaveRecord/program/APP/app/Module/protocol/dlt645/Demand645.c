//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	�����
//��������	2017.09.22
//����		645Э���ȡ����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt645_2007.h"
#if( SEL_DLT645_2007 == YES )
#if( MAX_PHASE == 3 )//ֻ�������Ŵ���������
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// ������Ϣ��EEPROM�еĵ�ַ��Χ

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������

//-----------------------------------------------
 
//-----------------------------------------------
//				���ļ�ʹ�õı���������
typedef struct
{
	const WORD				ID32;			//645���ݱ�ʶ ID2,ID3
	const BYTE				MaxDI1Value;	//ID1���ݱ�ʶ�����ֵ
	const BYTE      		MaxDI0Value;	//ID0���ݱ�ʶ�����ֵ
}TDemand645Obj_t;


//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TDemand645Obj_t TDemand645Obj[] =
{
//	  ID3ID2	ID1���ֵ				ID0���ֵ				��������	
	{ 0x0101, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�������й����������������ʱ��
	{ 0x0102, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�η����й����������������ʱ��

//��֧�ַ�������
//#if( SEL_REACTIVE_DEMAND == YES )//�Ƿ�֧�ַ�������	
//	#if( SEL_REACTIVE_DEMAND == YES )//�Ƿ�֧���޹�����	
//	{ 0x0117, 0x00, 			DLT_CLOSEING_NO 	},//��ǰ����12��A������޹�1����
//	{ 0x0118, 0x00, 			DLT_CLOSEING_NO 	},//��ǰ����12��A������޹�2����
//	{ 0x0119, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A���һ�����޹�����
//	{ 0x011A, 0x00, 			DLT_CLOSEING_NO 	},//��ǰ����12��A��ڶ������޹�����
//	{ 0x011B, 0x00, 			DLT_CLOSEING_NO		},//��ǰ����12��A����������޹�����
//	{ 0x011C, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A����������޹�����
//	{ 0x012B, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B������޹�1����
//	{ 0x012C, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B������޹�2����
//	{ 0x012D, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B���һ�����޹�����
//	{ 0x012E, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B��ڶ������޹�����
//	{ 0x012F, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B����������޹�����
//	{ 0x0130, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B����������޹�����
//	{ 0x013F, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C������޹�1����
//	{ 0x0140, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C������޹�2����
//	{ 0x0141, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C���һ�����޹�����
//	{ 0x0142, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C��ڶ������޹�����
//	{ 0x0143, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C����������޹�����
//	{ 0x0144, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C����������޹�����
//	#endif
//	#if( SEL_APPARENT_DEMAND == YES )//�Ƿ�֧����������
//	{ 0x011D, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A��������������
//	{ 0x011E, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A�෴����������
//	{ 0x0131, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B��������������
//	{ 0x0132, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B�෴����������
//	{ 0x0145, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C��������������
//	{ 0x0146, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C�෴����������
//	#endif
//	{ 0x0115, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A�������й�����
//	{ 0x0116, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��A�෴���й�����
//	{ 0x0129, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B�������й�����
//	{ 0x012A, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��B�෴���й�����
//	{ 0x013D, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C�������й�����
//	{ 0x013E, 0x00, 			DLT_CLOSEING_NO  	},//��ǰ����12��C�෴���й�����
//#endif

#if( SEL_REACTIVE_DEMAND == YES )//�Ƿ�֧���޹�����	
	{ 0x0103, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12������޹�1���������������ʱ��
	{ 0x0104, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12������޹�2���������������ʱ��
	{ 0x0105, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�ε�һ�����޹����������������ʱ��
	{ 0x0106, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�εڶ������޹����������������ʱ��
	{ 0x0107, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�ε��������޹����������������ʱ��
	{ 0x0108, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�ε��������޹����������������ʱ��
#endif

#if( SEL_APPARENT_DEMAND == YES )//�Ƿ�֧����������
	{ 0x0109, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�������������������������ʱ��
	{ 0x010A, MAX_RATIO, 	DLT_CLOSEING_NO 	},//��ǰ����12�η����������������������ʱ��
#endif
};								

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//--------------------------------------------------
//��������:  ���ݱ������������ݱ�ʶ         
//��     ��:WORD ID[in]	���ݱ�ʶ         
//����   ֵ  :�ɹ�������������ʧ�ܣ�����0x80             
//��ע����:  BYTE
//--------------------------------------------------
BYTE Search645DemandID( BYTE *DI  )
{
	BYTE i,Num;//����������ܳ���255
	WORD DataId;
	DataId = 0;

	DataId = (DI[3]<<8) + DI[2];
	Num = (sizeof(TDemand645Obj)/sizeof(TDemand645Obj[0]));
	if( Num >= 0x80 )		//������ѭ��
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( DataId == TDemand645Obj[i].ID32 )
		{
			if( (DI[1]<=TDemand645Obj[i].MaxDI1Value) || ((DI[1] == 0xff)&&(TDemand645Obj[i].MaxDI1Value!=0)) )//ID1����Ҫ��
			{
				if( (DI[0] <= TDemand645Obj[i].MaxDI0Value) || ((DI[0] == 0xff)&&(TDemand645Obj[i].MaxDI0Value!=0)) )//ID0����Ҫ��
				{
					return i;
				}
			}
		}
	}

	if( i == (sizeof(TDemand645Obj)/sizeof(TDemand645Obj[0])) )		//δ�ҵ�
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//��������:  ��ȡ��ͨ��������,645���       
//��     ��:BYTE Dot[in]		С��λ��       
//         BYTE *DI[in]		���ݱ�ʶ                       
//         WORD OutBuf[in]  ��������        
//         BYTE Sch[in]	    ���ݱ�ʶ����       
//����ֵ     :static ��ȡ�����ݳ��� 
//����ֵ     :static  Lenth:���ȣ�����0x8000��Ϊ���������� 
//��ע����:  
//--------------------------------------------------
static WORD Get645CommonDemand( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	BYTE Num[2],Ratio[2],Lenth;
	WORD EnergyType,Result,LenthRec;
	TFourByteUnion DataId,DemandData;
	
	Lenth = 0;
	DataId.l = 0;
	DemandData.l = 0;
	LenthRec = 0;
	Result = TRUE;
	
	memcpy( DataId.b, DI, 4 ); 
	
	if( DI[2] <= 0x0A )//��ǰ����
	{
		EnergyType = (0*0x1000)+(DI[2]-0x00);
	}
	else if( DI[2] <= 0x1E )//A������
	{
		EnergyType = (1*0x1000)+(DI[2]-0x14);
	}
	else if( DI[2] <= 0x32 )//B������
	{
		EnergyType = (2*0x1000)+(DI[2]-0x28);
	}
	else if( DI[2] <= 0x46 )//C������
	{
		EnergyType = (3*0x1000)+(DI[2]-0x3C);
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	Num[1] = ((DI[1] == 0xff) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio[1] = ((DI[1] == 0xff) ? 0 : DI[1]);
	Num[1] = (Num[1]+Ratio[1]);

	Num[0] = ((DI[0] == 0xff) ? (TDemand645Obj[Sch].MaxDI0Value) : 1);
	Ratio[0] = ((DI[0] == 0xff) ? 1 : DI[0]);		
	Num[0] = (Num[0]+Ratio[0]);
		
	if( DI[1]>(FPara1->TimeZoneSegPara.Ratio) && (DI[1] != 0xff) )//Ԫ���������ܴ��ڷ�����
	{
		return DLT645_ERR_ID_02;
	}
	if( DI[0]>(TDemand645Obj[Sch].MaxDI0Value ) && (DI[0] != 0xff) )//Ԫ���������ܴ��ڷ�����
	{
		return DLT645_ERR_DATA_01;
	}
	Lenth = 8; //�������������ʱ��


	if( (DI[0]==0x00) || (DI[0]==0xFF) )//����ǰ����
	{
		for( ; Ratio[1]<Num[1]; Ratio[1]++ )
		{
			Result = api_GetDemand(EnergyType, DATA_HEXCOMP, Ratio[1], Dot, OutBuf);
			if(Result == FALSE)
			{
				return DLT645_ERR_ID_02;
			}

			//ת���������XX.XXXX
			memcpy( DemandData.b, OutBuf,4 );

			ProBinToBCD645( eCOMB_MODE, 3, DemandData.b);	////���ݷ��ش��� ҲҪ���ش������� ���ܷ�������������


			memcpy( OutBuf, DemandData.b, 3 );				
	
			DemandData.l = 0;
			//ת������ʱ�䣬YYMMDDhhmm
			memcpy( DemandData.b, OutBuf+4, 2 );
			Result = ProBinToBCD( eCOMB_MODE, 2, DemandData.b );
			if(Result == 0)
			{
				return DLT645_ERR_ID_02;
			}
			else
			{
				memcpy( OutBuf+3, DemandData.b, 1 );
			}

			DemandData.l = 0;
			memcpy( DemandData.b, OutBuf+6, 4 );
			Result = ProBinToBCD( eBYTE_MODE, 4, DemandData.b );
			if(Result == 0)
			{
				return DLT645_ERR_ID_02;
			}
			else
			{
				memcpy( OutBuf+4, DemandData.b, 4 );
			}
			lib_InverseData( OutBuf+3, 5 );
			OutBuf += Lenth;
			LenthRec += Lenth;
		}
	}
	// if( DI[0] != 0x00 ) //������������
	// {
	// 	for( ; Ratio[0]<Num[0]; Ratio[0]++ )
	// 	{
	// 		DataId.b[0] = Ratio[0];
	// 		Result = api_ReadClosingRecord645(DI, OutBuf);
	// 		if((Result == 0) || (Result >= 0x8000))
	// 		{
	// 			return DLT645_ERR_ID_02;
	// 		}
	// 		else
	// 		{
	// 			Lenth = Result;
	// 		}

	// 		if( DataId.b[1] == 0xff )
	// 		{
	// 			Lenth = 8*(FPara1->TimeZoneSegPara.Ratio + 1 );
	// 		}
	// 		OutBuf += Lenth;
	// 		LenthRec += Lenth;
			
	// 	}
	// }
	return LenthRec;
}

//--------------------------------------------------
//��������:  ��ȡ����       
//��     ��:
//			Dot[in]		   С��λ��       
//          *DI[in]		   ���ݱ�ʶ                       
//          OutBufLen[in]  ��ȡ�ĳ���       
//          OutBuf[out]	   ��������       
//����    ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���    
//��ע����:  ��
//--------------------------------------------------
WORD GetRequest645Demand( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Sch;
	WORD Result,Lenth;
	BYTE EnergyOutBuf[200];	//���ֲ���������������

	Result = DLT645_OK_00;	
	memset( EnergyOutBuf, 0x00, 200 );

	
	//��Ѱ���ݱ�ʶ�Ƿ�֧�� ��֧�ַ���
	if( DI[2]==0xff )	//id2����Ϊ0xff
	{
		return DLT645_ERR_ID_02;
	}
	else if( (DI[0]==0xff) && (DI[1]==0xff) )	//id0��id1����ͬʱΪ0xff
	{
		return DLT645_ERR_ID_02;
	}
	
	Sch = Search645DemandID( DI );
	if( Sch == 0x80 )
	{
		Result = DLT645_ERR_ID_02;
		return Result;
	}
	
	
	Lenth = Get645CommonDemand( Dot, DI, EnergyOutBuf, Sch ); 

	//���ݷ��س��ȣ�ȷ��Ӧ�÷��ص�״̬�ֽ�
	if( Lenth > 0x8000 )
	{
		Result = Lenth;		
	}
	else
	{
		if( Lenth > OutBufLen )
		{
			Result = DLT645_ERR_ID_02;
		}
		else
		{
			Result = Lenth;
			memcpy(OutBuf, EnergyOutBuf, Lenth);
		}
	}
	return Result;
}

#endif//#if( MAX_PHASE == 3 )

#endif//#if ( SEL_DLT645_2007 == YES)


