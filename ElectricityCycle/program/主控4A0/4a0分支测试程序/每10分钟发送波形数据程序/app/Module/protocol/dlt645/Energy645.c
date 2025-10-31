//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2017.03.29
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt645_2007.h"
#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// ������Ϣ��EEPROM�еĵ�ַ��Χ

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum{
	eCOMMON_ENERGY=0,	//��ͨ����(�����������)
	eHIGH_ENERGY,  		//�߾��ȵ���
	eACCU_ENERGY,		//�ۼƵ���
	eMETER_MONEY		//�����
}eEnergyClass;


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
	const eEnergyClass     	EnergyClass;	//�������
}TEnergy645Obj_t;


//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TEnergy645Obj_t Energy645Obj[] =
{
//	  ID3ID2	ID1���ֵ				ID0���ֵ				��������
	{ 0x0000, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12������й��ܼ����ʵ���
	{ 0x0001, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�������й��ܼ����ʵ���
	{ 0x0002, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�η����й��ܼ����ʵ���
	{ 0x000B, 0x00, 			0x01, 				eACCU_ENERGY  	},//��ǰ��1������������й����ۼ��õ���
	{ 0x000C, 0x00, 			0x01, 				eACCU_ENERGY  	},//��ǰ��1�¶�����й����ۼ��õ���
	{ 0x00FE, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  }, //����ʱ��
#if( SEL_SEPARATE_ENERGY == YES )//�Ƿ�֧�ַ������	
	#if( SEL_RACTIVE_ENERGY == YES )//�Ƿ�֧���޹�����	
	{ 0x0017, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A������޹�1����
	{ 0x0018, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A������޹�2����
	{ 0x0019, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A���һ�����޹�����
	{ 0x001A, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A��ڶ������޹�����
	{ 0x001B, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A����������޹�����
	{ 0x001C, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A����������޹�����
	{ 0x002B, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B������޹�1����
	{ 0x002C, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B������޹�2����
	{ 0x002D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B���һ�����޹�����
	{ 0x002E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B��ڶ������޹�����
	{ 0x002F, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B����������޹�����
	{ 0x0030, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B����������޹�����
	{ 0x003F, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C������޹�1����
	{ 0x0040, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C������޹�2����
	{ 0x0041, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C���һ�����޹�����
	{ 0x0042, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C��ڶ������޹�����
	{ 0x0043, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C����������޹�����
	{ 0x0044, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C����������޹�����
	#endif
	#if( SEL_APPARENT_ENERGY == YES )//�Ƿ�֧�����ڵ���
	{ 0x001D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A���������ڵ���
	{ 0x001E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A�෴�����ڵ���
	{ 0x0031, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B���������ڵ���
	{ 0x0032, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B�෴�����ڵ���
	{ 0x0045, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C���������ڵ���
	{ 0x0046, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C�෴�����ڵ���
	#endif
	{ 0x0015, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A�������й�����
	{ 0x0016, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��A�෴���й�����
	{ 0x0029, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B�������й�����
	{ 0x002A, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��B�෴���й�����
	{ 0x003D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C�������й�����
	{ 0x003E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//��ǰ����12��C�෴���й�����
#endif

#if( SEL_RACTIVE_ENERGY == YES )//�Ƿ�֧���޹�����	
	{ 0x0003, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12������޹�1�ܼ����ʵ���
	{ 0x0004, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12������޹�2���ܼ����ʵ���
	{ 0x0005, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�ε�һ�����޹��ܼ����ʵ���
	{ 0x0006, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�εڶ������޹��ܼ����ʵ���
	{ 0x0007, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�ε��������޹��ܼ����ʵ���
	{ 0x0008, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�ε��������޹��ܼ����ʵ���
#endif

#if( SEL_APPARENT_ENERGY == YES )//�Ƿ�֧�����ڵ���
	{ 0x0009, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�����������ܼ����ʵ���
	{ 0x000A, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//��ǰ����12�η��������ܼ����ʵ���
#endif

#if( MAX_PHASE != 1 )	//���಻֧�ָ߾��ȵ���
	{ 0x0060, MAX_RATIO, 		0x00, 				eHIGH_ENERGY  	},//��ǰ�����й��ܼ����ʾ�ȷ����
	{ 0x0061, MAX_RATIO, 		0x00, 				eHIGH_ENERGY  	},//��ǰ�����й��ܼ����ʾ�ȷ����
#endif

#if(PREPAY_MODE == PREPAY_LOCAL)//���ر���Ҫ֧��ʣ�����͸֧���
	{ 0x0090, 0x02,				0x01,				eMETER_MONEY	},//��ǰ�����й��ܼ����ʾ�ȷ����
		
#endif

};								

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//--------------------------------------------------
//��������:  ���ݱ����ҵ������ݱ�ʶ         
//��     ��:WORD ID[in]	���ݱ�ʶ         
//����   ֵ  :�ɹ�������������ʧ�ܣ�����0x80             
//��ע����:  BYTE
//--------------------------------------------------
BYTE Search645EnergyID( BYTE *DI  )
{
	BYTE i,Num;//����������ܳ���255
	WORD DataId;
	DataId = 0;

	DataId = (DI[3]<<8) + DI[2];
	Num = (sizeof(Energy645Obj)/sizeof(Energy645Obj[0]));
	if( Num >= 0x80 )		//������ѭ��
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( DataId == Energy645Obj[i].ID32 )
		{
			if( (DI[1]<=Energy645Obj[i].MaxDI1Value) || ((DI[1] == 0xff)&&(Energy645Obj[i].MaxDI1Value!=0)) )//ID1����Ҫ��
			{
				if( (DI[0] <= Energy645Obj[i].MaxDI0Value) || ((DI[0] == 0xff)&&(Energy645Obj[i].MaxDI0Value!=0)) )//ID0����Ҫ��
				{
					return i;
				}
			}
		}
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
//��ע����:  
//--------------------------------------------------
static WORD Get645CommonEnergy( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	BYTE i,Num[2],Ratio[2],Lenth,MinusFlag;
	WORD EnergyType,Result,LenthRec;
	TFourByteUnion DataId,OutData;
	
	Lenth = 0;
	DataId.l = 0;
	LenthRec = 0;
	Result = TRUE;
	MinusFlag = 0;
	
	memcpy( DataId.b, DI, 4 ); 
	
	if( DI[2] <= 0x0A )//��ǰ����
	{
		EnergyType = (0*0x1000)+(DI[2]-0x00);
	}
	else if( DI[2] <= 0x1E )//A�����@@hy������ж���������׼ȷ���ὫDI[2]==0x0B��ʼ�ķ�A��������ݱ�ʶ����������ִ��ʱ�������ϲ㺯��GetRequest645Energy�е�Search645EnergyID( DI )����ɸѡ
	{
		EnergyType = (1*0x1000)+(DI[2]-0x14);
	}
	else if( DI[2] <= 0x32 )//B�����@@ͬ��
	{
		EnergyType = (2*0x1000)+(DI[2]-0x28);
	}
	else if( DI[2] <= 0x46 )//C�����@@ͬ��
	{
		EnergyType = (3*0x1000)+(DI[2]-0x3C);
	}
	else if( (DI[2] == 0x60) || (DI[2] == 0x61) )
	{
		EnergyType = (0*0x1000)+(DI[2]-0x5F);
	}
	else if( DI[2] == 0xfe )//������ʱ��
	{
		
	}
	else
	{
		return DLT645_ERR_ID_02;//�����ݲ�֧�֣�����������
	}
	Num[1] = ((DI[1] == 0xff) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio[1] = ((DI[1] == 0xff) ? 0 : DI[1]);
	Num[1] = (Num[1]+Ratio[1]);

	Num[0] = ((DI[0] == 0xff) ? (Energy645Obj[Sch].MaxDI0Value) : 1);
	Ratio[0] = ((DI[0] == 0xff) ? 1 : DI[0]);
	Num[0] = (Num[0]+Ratio[0]);
		
	if( DI[1]>(FPara1->TimeZoneSegPara.Ratio) && (DI[1] != 0xff) )//Ԫ���������ܴ��ڷ�����
	{
		return DLT645_ERR_ID_02;
	}
	if( DI[0]>(Energy645Obj[Sch].MaxDI0Value ) && (DI[0] != 0xff) )//Ԫ���������ܴ��ڷ�����
	{
		return DLT645_ERR_ID_02;
	}
	Lenth =4;
	
	if( Energy645Obj[Sch].EnergyClass == eHIGH_ENERGY )
	{
		EnergyType = ( EnergyType | 0x8000 );
		Lenth = 5;	//698�߾��ȵ��ܲ���8�ֽ�,4λС��;645����5�ֽ�,4λС��
	}

	if( DI[2] == 0xfe ) //1-12�½����ս���ʱ��
	{
		if( Ratio[0] == 0x00 )
		{
			return DLT645_ERR_ID_02;
		}
		else
		{
			for(; Ratio[0] < Num[0]; Ratio[0]++ )
			{
				DataId.b[0] = Ratio[0];
				Result = api_ReadClosingRecord645( DataId.b, OutBuf );
				if( (Result == 0) || (Result >= 0x8000) )
				{
					return DLT645_ERR_ID_02;
				}
				
				OutBuf += 4; 
				LenthRec += 4; //YYMMDDHH ����4���ֽ�
			}
		}
	}
	else //�������յ���
	{
		if( (DI[0] == 0x00) || (DI[0] == 0xFF) ) //����ǰ����
		{
			for( ; Ratio[1]<Num[1]; Ratio[1]++ )
			{
				Result = api_GetCurrEnergyData(EnergyType, DATA_BCD, Ratio[1], Dot, OutBuf);
				if(Result == FALSE)
				{
					return DLT645_ERR_ID_02;
				}
	
				if( (Lenth != 4) && (Lenth != 5) )
				{
					return DLT645_ERR_ID_02;
				}
				
				OutBuf += Lenth;
				LenthRec += Lenth;
			}
		}
		if( DI[0] != 0x00 ) //�������յ��� ���ᴦ��ʱ�Ѿ�ת��ΪBCD���˴�����ת��
		{
			for( ; Ratio[0]<Num[0]; Ratio[0]++ )
			{
				DataId.b[0] = Ratio[0];
	
				Result = api_ReadClosingRecord645( DataId.b, OutBuf );
	
				if((Result == 0) || (Result >= 0x8000))
				{
					return DLT645_ERR_ID_02;
				}
				
				if( DataId.b[1] == 0xff )
				{
					Lenth = 4*( FPara1->TimeZoneSegPara.Ratio + 1 );
				}
				
				//ֻ��Dot������2 ����Ҫ���⴦��
				if( Dot < 2 )
				{
					for( i = 0; i < Lenth / 4; i++ )
					{
						memcpy( OutData.b, OutBuf + 4 * i, 4 );
						
						if( OutData.b[3]&0x80 )
						{
							MinusFlag = 1;
							OutData.b[3] &= 0x7F;
						}
						//BCDһ����ռ4��bitλ
						OutData.d >>= 4 * (2 - Dot);
						
						if( (MinusFlag == 1)&&(OutData.d != 0) )
						{
							OutData.b[3] |= 0x80;
						}
						
						memcpy( OutBuf + 4 * i, OutData.b, 4 );
					}
				}
				OutBuf += Lenth;
				LenthRec += Lenth;	
			}
		}
	}
	return LenthRec;
}
//--------------------------------------------------
//��������:  ��ȡ��������,645���       
//��     ��:    
//			Dot[in]		   С��λ��       
//          *DI[in]		   ���ݱ�ʶ                       
//          OutBufLen[in]  ��ȡ�ĳ���       
//          OutBuf[out]	   ��������     
//          Sch[in]	       ���ݱ�ʶ����       
//����ֵ     :static  Lenth:���ȣ�����0x8000��Ϊ����������     
//��ע����:  
//--------------------------------------------------
static WORD Get645EnergyData( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	WORD Lenth;
	BOOL Result;

	#if( PREPAY_MODE == PREPAY_LOCAL )
	TFourByteUnion TmpMoney;
	#endif

	Lenth = 0;
	Result = TRUE;
	
	if( Energy645Obj[Sch].EnergyClass == eACCU_ENERGY) 
	{	//�ۼƵ���
		if( DI[2] == 0x0B )//��ǰ�������ڼ���1������������й����ۼ��õ���
		{
			Result = api_GetAccuPeriEnergy( DI[0], DATA_BCD, Dot, OutBuf );
			Lenth = 4;
		}
		else if( DI[2] == 0x0C )//��ǰ�¶ȼ���1�¶�����й����ۼ��õ���
		{
			Result = api_GetAccuEnergyData( DI[0], DATA_BCD, Dot, OutBuf );
			Lenth = 4;
		}
		else if( DI[2] == 0x0D )//��ǰ����6��������ռ�¼
		{
			if( (DI[1] == 0)&&(DI[0] <= 7) )
			{
				return DLT645_ERR_ID_02;//��ʱ��֧��
			}
			else
			{
				return DLT645_ERR_ID_02;
			}
		}
		else if( ( DI[2] == 0x60 )||( DI[2] == 0x61 ) )
		{
			Lenth = DLT645_ERR_ID_02;
		}
		
		//�������ݳ��ȴ�������й����õ���4�ֽڣ�������¼4�ֽڽ���ʱ��+4�ֽڵ���
		if( Result == FALSE )
		{
			return DLT645_ERR_ID_02;
		}
		
	}
	else if( Energy645Obj[Sch].EnergyClass == eMETER_MONEY )
	{	
		#if(PREPAY_MODE == PREPAY_LOCAL)
		if((DI[2]==0x90)&&(DI[1]==0x02)&&((DI[0]==0x00)||(DI[0]==0x01)))//DI3 �϶�����00��ֻ��Ҫ�ж�DI2~DI0����
		{
			Lenth = api_ReadPrePayPara(eRemainMoney, (BYTE *)TmpMoney.b);
			if( Lenth == 0 )//���ݳ���Ϊ0��˵������ʧ��.��ȷ���Ӧ��Ϊ4
			{	
				return DLT645_ERR_ID_02;
			}
			
			if(DI[0]==0x00)//ʣ����
			{
				if(TmpMoney.l>=0)
				{
					TmpMoney.d =lib_DWBinToBCD(TmpMoney.d);
					memcpy(OutBuf,TmpMoney.b,Lenth);
				}
				else
				{
					memset(OutBuf,0x00,Lenth);
				}
			}
			else//͸֧���
			{
				if(TmpMoney.l>=0)
				{
					memset(OutBuf,0x00,Lenth);
				}
				else
				{
					TmpMoney.l *=(-1);
					TmpMoney.d =lib_DWBinToBCD(TmpMoney.d);
					memcpy(OutBuf,TmpMoney.b,Lenth);
				}
			}	
		}
		else
		{	
			return DLT645_ERR_ID_02;
		}
		#else
		return DLT645_ERR_ID_02;
		#endif
	}
	else
	{	
        //��ͨ����(�������)���߾��ȵ���
		Lenth = Get645CommonEnergy( Dot, DI, OutBuf, Sch );				
	}
	return Lenth;
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
// 			���� - ���ݳ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequest645Energy( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Sch;
	WORD Result,Lenth;
	BYTE EnergyOutBuf[100];	//���ֲ���������������

	Result = DLT645_ERR_ID_02;	
	memset( EnergyOutBuf, 0x00, 100 );

	
	//��Ѱ���ݱ�ʶ�Ƿ�֧�� ��֧�ַ���
	if( DI[2]==0xff )	//id2����Ϊ0xff
	{
		return DLT645_ERR_ID_02;
	}
	else if( (DI[0]==0xff) && (DI[1]==0xff) )	//id0��id1����ͬʱΪ0xff
	{
		return DLT645_ERR_ID_02;
	}
	
	Sch = Search645EnergyID( DI );
	if( Sch == 0x80 )
	{
		Result = DLT645_ERR_ID_02;
		return Result;
	}
	
	Lenth = Get645EnergyData( Dot, DI, EnergyOutBuf, Sch );

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
			memcpy(OutBuf, EnergyOutBuf,Lenth);
		}
	}
	return Result;
}

#endif//#if( SEL_DLT645_2007 == YES )


