//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2018.8.8
//����		DLT645-2007��������ģ��
//�޸ļ�¼
//----------------------------------------------------------------------

#include "AllHead.h"
#include "Freeze.h"

#if(SEL_DLT645_2007 == YES)

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define	FREEZE_RECORD_TIME_OAD				0x00022120			// ����ʱ��OAD ����

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct TFreeze645Map_t
{
	BYTE	DIUnit;         //645��Ӧ�����ݱ�ʶ
	BYTE	FollowFlag;     //645���ݱ�ʶ�Ƿ�һ��698OAD�ܱ�ʾ�������Ƿ���Ҫ����OADһ���ʾ��0--����û�У�1--��������
	BYTE	DataLen;        //645��Ӧ���ݳ��ȣ��������ݣ������������ݿ鳤��
	DWORD	OAD;         	//645��Ӧ��698OAD
	BYTE	( *Freeze645TransformFunc )( BYTE *Input,BYTE *Output );   //698����Դת��Ϊ645��ʽ����						  
}TFreeze645Map;

typedef struct TLpfClosing645Map_t
{
	WORD	wDIUnit;         //645��Ӧ�����ݱ�ʶ
	BYTE	FollowFlag;     //645���ݱ�ʶ�Ƿ�һ��698OAD�ܱ�ʾ�������Ƿ���Ҫ����OADһ���ʾ��0--����û�У�1--��������
	BYTE	DataLen;		//645��Ӧ���ݳ��ȣ��������ݣ������������ݿ鳤��
	DWORD	OAD;            //645��Ӧ��698OAD
	BYTE	( *Freeze645TransformFunc)( BYTE *Input, BYTE *Output );   //698����Դת��Ϊ645��ʽ����
}TLpfClosing645Map;
 
typedef struct TFreezeType645Map_t
{
	BYTE	DIUnit;         //645��Ӧ�����ݱ�ʶ
	BYTE	FreezeIndex;    //�����Ӧ��ö�ٺ�
}TFreezeType645Map;

typedef struct TLpfRemainFrame_t
{
	BYTE	RemainDotNum;		//�˺���֡����Ҫ���ĵ���
	DWORD	StartTime;    		//�˺���֡Ҫ������ʼʱ��
	DWORD	EndTime;          	//�˺���֡Ҫ���Ľ�ֹʱ��
}TLpfRemainFrame;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//�������ߺ���֡��Ϣ ��ͨ��
static TLpfRemainFrame	LpfRemainFrame[MAX_COM_CHANNEL_NUM];

BYTE FreezeTime645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeEnergy645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeU645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeI645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezePower645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeCos645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeUBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeIBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezePowerBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeCosBlock645Transform( BYTE *InBuf, BYTE *OutBuf );
BYTE FreezeDemandBlock645Transform( BYTE *InBuf, BYTE *OutBuf ); 
BYTE FreezeEnergyBlock645Transform( BYTE *InBuf, BYTE *OutBuf ); 
BYTE FreezeCombEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf);
BYTE FreezeCombEnergy645Transform( BYTE *InBuf, BYTE *OutBuf );

//645 DI1�붳�����Ͷ�Ӧ���
static const TFreezeType645Map	FreezeType645MapTab[] =
{
	{ 0x01,		eFREEZE_INSTANT,			},	//˲ʱ����
	{ 0x02,		eFREEZE_TIME_ZONE_CHG,		},	//ʱ�����л�����
	{ 0x03,		eFREEZE_DAY_TIMETABLE_CHG,	},	//ʱ�α��л�����
	{ 0x04,		eFREEZE_HOUR,				},  //���㶳��
	#if(PREPAY_MODE == PREPAY_LOCAL)	
	{ 0x05,		eFREEZE_TARIFF_RATE_CHG,	},	//���ʱ��л�����
	#endif
	{ 0x06,		eFREEZE_DAY,				},	//�ն���	
	#if(PREPAY_MODE == PREPAY_LOCAL)	
	{ 0x07,		eFREEZE_LADDER_CHG,			},	//���ݱ��л�����
	#endif
};

//645 DI2��������Ϣ��񣨰���FreezeType645MapTab�г����㶳����Ķ������ͣ�
static const TFreeze645Map	Freeze645MapTab[] =
{
	{ 0x00,		0,		5,	0x20210200,		 FreezeTime645Transform				},	//����ʱ��
	// { 0x01,		0,		4,	0x00100200, 	 FreezeEnergyBlock645Transform 		},  //�����й��������ݿ�
	// { 0x02,		0,		4,	0x00200200, 	 FreezeEnergyBlock645Transform 		},  //�����й��������ݿ�
	// { 0x03,		0,		4,	0x00300200, 	 FreezeCombEnergyBlock645Transform 	},  //����޹�1�������ݿ�
	// { 0x04,		0,		4,	0x00400200, 	 FreezeCombEnergyBlock645Transform 	},  //����޹�2�������ݿ�
	// { 0x05,		0,		4,	0x00500200, 	 FreezeEnergyBlock645Transform 		},  //��һ�����޹��������ݿ�
	// { 0x06,		0,		4,	0x00600200, 	 FreezeEnergyBlock645Transform 		},  //�ڶ������޹��������ݿ�
	// { 0x07,		0,		4,	0x00700200, 	 FreezeEnergyBlock645Transform 		},  //���������޹��������ݿ�
	// { 0x08,		0,		4,	0x00800200, 	 FreezeEnergyBlock645Transform 		},  //���������޹��������ݿ�
	// { 0x09,		0,		8,	0x10100200, 	 FreezeDemandBlock645Transform 		},  //�����й��������������ʱ������
	// { 0x0A,		0,		8,	0x10200200, 	 FreezeDemandBlock645Transform 		},  //�����й��������������ʱ������
	// { 0x10,		1,		12,	0x20040200, 	 FreezePowerBlock645Transform		},  //�������� �й�����
	// { 0x10,		0,		12,	0x20050200, 	 FreezePowerBlock645Transform		},  //�������� �޹�����
}; 

//645 DI2��������Ϣ������㶳�ᣩ
static const TFreeze645Map	FreezeHour645MapTab[] =
{
	{ 0x00,		0,		5,	0x20210200,		FreezeTime645Transform			},  //����ʱ��
	{ 0x01,		0,		4,	0x00100201, 	 FreezeEnergy645Transform 		},  //�����й��ܵ�������
	{ 0x02,		0,		4,	0x00200201, 	 FreezeEnergy645Transform 		},  //�����й��ܵ�������
}; 

//645�н��㶳��DI0 DI1��������Ϣ��񣬰��������������ݡ���֧����N�����ݿ飬��֧�ֵ�ǰ����12��ĳ���������ݿ�
//DataLen Ϊ���ݿ���ÿ�����ݵĳ���,����ʱ��ʵ��Ϊ4���˴�����Ϊ5����ҪΪ��ͨ���������ֳ����ܺ�������ʱ������ʸ����޹أ�
static const TLpfClosing645Map	Closing645MapTab[] =
{
	{ 0x00FE,		0,	5,	0x20210200,		FreezeTime645Transform		 		},  //����ʱ��
	{ 0x0000,		0,	4,	0x00000200,		FreezeCombEnergyBlock645Transform	},  //����й��������ݿ�
	{ 0x0001,		0,	4,	0x00100200,		FreezeEnergyBlock645Transform 		},  //�����й��������ݿ�
	{ 0x0002,		0,	4,	0x00200200,		FreezeEnergyBlock645Transform 		},  //�����й��������ݿ�
#if(MAX_PHASE == 3)
	{ 0x0003,		0,	4,	0x00300200,		FreezeCombEnergyBlock645Transform 	},  //����޹�1�������ݿ�
	{ 0x0004,		0,	4,	0x00400200,		FreezeCombEnergyBlock645Transform 	},  //����޹�2�������ݿ�
	{ 0x0005,		0,	4,	0x00500200,		FreezeEnergyBlock645Transform 		},  //��һ�����޹��������ݿ�
	{ 0x0006,		0,	4,	0x00600200,		FreezeEnergyBlock645Transform 		},  //�ڶ������޹��������ݿ�
	{ 0x0007,		0,	4,	0x00700200,		FreezeEnergyBlock645Transform 		},  //���������޹��������ݿ�
	{ 0x0008,		0,	4,	0x00800200,		FreezeEnergyBlock645Transform 		},  //���������޹��������ݿ�
	{ 0x0009,		0,	4,	0x00900200,		FreezeEnergyBlock645Transform 		},  //�������ڵ������ݿ�
	{ 0x000A,		0,	4,	0x00a00200,		FreezeEnergyBlock645Transform 		},  //�������ڵ������ݿ�
	{ 0x0101,		0,	8,	0x10100200, 	FreezeDemandBlock645Transform 		},  //�����й��������������ʱ������
	{ 0x0102,		0,	8,	0x10200200, 	FreezeDemandBlock645Transform 		},  //�����й��������������ʱ������
#endif
};

//��������DI2 DI3��������Ϣ���ֻ֧�ֲ���4Ҫ��
static const TLpfClosing645Map	Lpf645MapTab[] =
{
	{ 0x0101,	0,	2,	0x20000201,	FreezeU645Transform 			},  //A���ѹ
	// { 0x0201,	0,	3,	0x20010201,	FreezeI645Transform 			},  //A�����
	// { 0x0300,	0,	3,	0x20040201,	FreezePower645Transform 		},  //���й�����
	// { 0x0500,	0,	2,	0x200A0201,	FreezeCos645Transform 			},  //�ܹ�������
	// { 0x0601,	0,	4,	0x00100201,	FreezeEnergy645Transform 		},  //�����й�����
	// { 0x0602,	0,	4,	0x00200201,	FreezeEnergy645Transform 		},  //�����й�����
#if(MAX_PHASE == 3)
	// { 0x0102,	0,	2,	0x20000202,	FreezeU645Transform 			},  //B���ѹ
	// { 0x0103,	0,	2,	0x20000203,	FreezeU645Transform 			},  //C���ѹ
	// { 0x01ff,	0,	6,	0x20000200,	FreezeUBlock645Transform 		},  //��ѹ���ݿ�	
	// { 0x0202,	0,	3,	0x20010202,	FreezeI645Transform 			},  //B�����
	// { 0x0203,	0,	3,	0x20010203,	FreezeI645Transform 			},  //C�����
	// { 0x02ff,	0,	9,	0x20010200,	FreezeIBlock645Transform 		},  //�������ݿ�	
	// { 0x0301,	0,	3,	0x20040202,	FreezePower645Transform 		},  //A���й�����
	// { 0x0302,	0,	3,	0x20040203,	FreezePower645Transform 		},  //B���й�����
	// { 0x0303,	0,	3,	0x20040204,	FreezePower645Transform 		},  //C���й�����
	// { 0x03ff,	0,	12,	0x20040200,	FreezePowerBlock645Transform 	},  //�й��������ݿ�
	// { 0x0400,	0,	3,	0x20050201,	FreezePower645Transform 		},  //���޹�����
	// { 0x0401,	0,	3,	0x20050202,	FreezePower645Transform 		},  //A���޹�����
	// { 0x0402,	0,	3,	0x20050203,	FreezePower645Transform 		},  //B���޹�����
	// { 0x0403,	0,	3,	0x20050204,	FreezePower645Transform 		},  //C���޹�����
	// { 0x04ff,	0,	12,	0x20050200,	FreezePowerBlock645Transform 	},  //�޹��������ݿ�	
	// { 0x0501,	0,	2,	0x200A0202,	FreezeCos645Transform 			},  //A�๦������
	// { 0x0502,	0,	2,	0x200A0203,	FreezeCos645Transform 			},  //B�๦������
	// { 0x0503,	0,	2,	0x200A0204,	FreezeCos645Transform 			},  //C�๦������
	// { 0x05ff,	0,	8,	0x200A0200,	FreezeCosBlock645Transform 		},  //�����������ݿ�	
	// { 0x0603,	0,	4,	0x00300201,	FreezeCombEnergy645Transform 	},  //����޹�����1
	// { 0x0604,	0,	4,	0x00400201,	FreezeCombEnergy645Transform 	},  //����޹�����2
	// { 0x06ff,	1,	4,	0x00100201,	FreezeEnergy645Transform 	 	},  //�����й�����
	// { 0x06ff,	1,	4,	0x00200201,	FreezeEnergy645Transform 	 	},  //�����й�����
	// { 0x06ff,	1,	4,	0x00300201,	FreezeCombEnergy645Transform 	},  //����޹�����1
	// { 0x06ff,	0,	4,	0x00400201,	FreezeCombEnergy645Transform 	},  //����޹�����2
	// { 0x0701,	0,	4,	0x00500201,	FreezeEnergy645Transform 		},  //��1�����޹��ܵ���
	// { 0x0702,	0,	4,	0x00600201,	FreezeEnergy645Transform 	 	},  //��2�����޹��ܵ���
	// { 0x0703,	0,	4,	0x00700201,	FreezeEnergy645Transform 	 	},  //��3�����޹��ܵ���
	// { 0x0704,	0,	4,	0x00800201,	FreezeEnergy645Transform 	 	},  //��4�����޹��ܵ���
	// { 0x07ff,	1,	4,	0x00500201,	FreezeEnergy645Transform 	 	},  //��1�����޹��ܵ���
	// { 0x07ff,	1,	4,	0x00600201,	FreezeEnergy645Transform 	 	},  //��2�����޹��ܵ���
	// { 0x07ff,	1,	4,	0x00700201,	FreezeEnergy645Transform 	 	},  //��3�����޹��ܵ���
	// { 0x07ff,	0,	4,	0x00800201,	FreezeEnergy645Transform 	 	},  //��4�����޹��ܵ���
	// { 0x0801,	0,	3,	0x20170200,	FreezePower645Transform 		},  //��ǰ�й�����
	// { 0x0802,	0,	3,	0x20180200,	FreezePower645Transform 		},  //��ǰ�޹�����
	// { 0x08ff,	1,	3,	0x20170200,	FreezePower645Transform 		},  //��ǰ�й�����
	// { 0x08ff,	0,	3,	0x20180200,	FreezePower645Transform 		},  //��ǰ�޹�����
#endif
};

//�������߼��ʱ���Ӧ�������Զ�����еļ��ʱ�䣨645����ÿ���һ�����ݶ�Ӧ��OAD��
static DWORD	Lpf645IntervalTime[] = 
{ 
	0x20000200, //��1�ฺ�ɼ�¼���ʱ�� ��ѹ���ݿ�
	0x20040200, //��2�ฺ�ɼ�¼���ʱ�� �й��������ݿ�
	0x200A0200, //��3�ฺ�ɼ�¼���ʱ�� �����������ݿ�
	0x00100201, //��4�ฺ�ɼ�¼���ʱ�� �����й�����
	0x00500201, //��5�ฺ�ɼ�¼���ʱ�� ��1�����޹��ܵ���
	0x20170200, //��6�ฺ�ɼ�¼���ʱ�� ��ǰ�й�����
};
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
extern const TFreezeInfoTab	FreezeInfoTab[];
extern TFreezeDataInfo  MinInfo[MAX_FREEZE_PLANNUM];

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
extern WORD ReadFreezeByLastNum( TFreezeData	*pData );
extern WORD ReadFreezeRecord645Sub( eFreezeType inFreezeIndex, TFreezeData *pData );
extern BOOL SearchFreezeRecordNoByTime( TFreezeData	*pData );
extern WORD ReadFreezeByTime( TFreezeData	*pData );

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��698��ʽʱ��ת��Ϊ645��ʽ����ʱ������ BCD�룩
//
//����: 	InBuf[in]:698��ʽ��ʱ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��698��ʽʱ��ת��Ϊ645��ʽ����ʱ������ BCD�룩
//-----------------------------------------------
BYTE FreezeTime645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Buf[6];

	api_TimeFormat698To645( (TRealTimer*)InBuf, Buf );
	lib_ExchangeData( OutBuf, Buf, 5 );
	
	return 5;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ�ĵ�������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:������������
//-----------------------------------------------
BYTE FreezeEnergy645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );

	if( tdw.d == 0xFFFFFFFF ) //�������ΪȫFF ˵���������ʷ������û�д������� ��������Ϊ0 -1�������������
	{
		tdw.d = 0;
	}
	else
	{
		tdw.d = lib_DWBinToBCD( tdw.d );
	}
	memcpy( OutBuf, tdw.b, 4 );

	return 4;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ�ĵ�������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:�����������ݣ���ϵ��ܣ�������
//-----------------------------------------------
BYTE FreezeCombEnergy645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );
	
	if( tdw.d == 0xFFFFFFFF ) //�������ΪȫFF ˵���������ʷ������û�д������� ��������Ϊ0 -1�������������
	{
		tdw.d = 0;
	}
	
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}

	tdw.d %= (DWORD)(80000000);  //����ϵ��ܽ�������

	//��Ϊǰ��if( tdw.l < 0 )tdw.l *= -1;ִ�е��˲����ж�tdw.d != 0xffffffff
	tdw.d = lib_DWBinToBCD( tdw.d );

	memcpy( OutBuf, tdw.b, 4 );

	if( Flag == 1 )
	{
		OutBuf[3] |= 0x80;
	}

	return 4;
}

//-----------------------------------------------
//��������: ������������й����ܣ�698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ�ĵ�������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:�������ݿ飨���ݷ��ʸ�����
//-----------------------------------------------
BYTE FreezeCombEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf)
{
  BYTE i, tCurrRatio;
  
  tCurrRatio = FPara1->TimeZoneSegPara.Ratio;
  
  if( tCurrRatio > MAX_RATIO )
  {
    tCurrRatio = 4;
  }
  for( i = 0; i < (tCurrRatio+1); i++ )
  {
    FreezeCombEnergy645Transform( InBuf + 4 * i, OutBuf + 4 * i );
  }
  
  return (tCurrRatio+1) * 4;
}


//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ�ĵ�������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:�������ݿ飨���ݷ��ʸ�����
//-----------------------------------------------
BYTE FreezeEnergyBlock645Transform(BYTE *InBuf, BYTE *OutBuf)
{
	BYTE i, tCurrRatio;
	TFourByteUnion tdw;
	
	tCurrRatio = FPara1->TimeZoneSegPara.Ratio;
	
	if( tCurrRatio > MAX_RATIO )
	{
		tCurrRatio = 4;
	}

	for( i = 0; i < (tCurrRatio+1); i++ )
	{
   		FreezeEnergy645Transform( InBuf + 4 * i, OutBuf + 4 * i ); 
    }
	
	return (tCurrRatio+1) * 4;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��2�ֽڵ�ѹת��Ϊ645��ʽ2�ֽڵ�ѹ
//-----------------------------------------------
BYTE FreezeU645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TTwoByteUnion tw;

	memcpy(tw.b, InBuf, 2 );
	tw.w = lib_WBinToBCD( tw.w );
	memcpy( OutBuf, tw.b, 2 );

	return 2;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��2�ֽڵ�ѹת��Ϊ645��ʽ2�ֽڵ�ѹ��ABC���ࣩ��ֻ���������ô˺���
//-----------------------------------------------
BYTE FreezeUBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	TTwoByteUnion tw;

	for( i = 0; i < 3; i++ )
	{
		memcpy( tw.b, InBuf + 2 * i, 2 );
		tw.w = lib_WBinToBCD( tw.w );
		memcpy( OutBuf + 2 * i, tw.b, 2 );
	}

	return 3 * 2;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��4�ֽڹ���ת��Ϊ645��ʽ3�ֽڹ���
//-----------------------------------------------
BYTE FreezePower645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TFourByteUnion tdw;

	memcpy( tdw.b, InBuf, 4 );
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}
	
	tdw.d = lib_DWBinToBCD( tdw.d );
		
	memcpy( OutBuf, tdw.b, 3 );

	if( Flag == 1 )
	{
		OutBuf[2] |= 0x80;
	}
	
	return 3;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��4�ֽڹ���ת��Ϊ645��ʽ3�ֽڹ��ʣ���ABC�ࣩ
//-----------------------------------------------
BYTE FreezePowerBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TFourByteUnion tdw;

	for( i = 0; i < 4; i++ )
	{
		memcpy( tdw.b, InBuf + 4 * i, 4 );
				
		if( tdw.l < 0 )
		{
			tdw.l *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		tdw.d = lib_DWBinToBCD( tdw.d );
		
		memcpy( OutBuf + 3 * i, tdw.b, 3 );
		
		if( Flag == 1 )
		{
			OutBuf[3*i+2] |= 0x80;
		}
		
		#if(MAX_PHASE == 1)
		if( i >= 2 )
		{
			//�����Բ�֧�ֵ������ff
			memset( OutBuf + 3 * i, 0xff, 3 ); 
		}
		#endif
	}

	return 12;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��4�ֽڵ���ת��Ϊ645��ʽ3�ֽڵ���
//-----------------------------------------------
BYTE FreezeI645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	TFourByteUnion tdw;
	BYTE	Flag;

	Flag = 0;

	memcpy( tdw.b, InBuf, 4 );
	if( tdw.l < 0 )
	{
		tdw.l *= -1;
		Flag = 1;
	}
	tdw.d = lib_DWBinToBCD( tdw.d );
	memcpy( OutBuf, tdw.b, 3 );
	
	if( Flag == 1 )
	{
		OutBuf[2] |= 0x80;
	}
	
	return 3;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��4�ֽڵ���ת��Ϊ645��ʽ3�ֽڵ�����ABC�ࣩ��ֻ��������ô˺���
//-----------------------------------------------
BYTE FreezeIBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TFourByteUnion tdw;

	for( i = 0; i < 3; i++ )
	{
		memcpy( tdw.b, InBuf + 4 * i, 4 );
		if( tdw.l < 0 )
		{
			tdw.l *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		tdw.d = lib_DWBinToBCD( tdw.d );
		memcpy( OutBuf + 3 * i, tdw.b, 3 );
		if( Flag == 1 )
		{
			OutBuf[3 * i + 2] |= 0x80;
		}
	}

	return 3 * 3;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��11�ֽ�������ʱ��ת��Ϊ645��ʽ8�ֽ�������ʱ�䣨���ݷ�������
//-----------------------------------------------
BYTE FreezeDemandBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i, tCurrRatio;
	TFourByteUnion tdw;
	BYTE Buf[6]; 

	tCurrRatio = FPara1->TimeZoneSegPara.Ratio;

	if( tCurrRatio > MAX_RATIO )
	{
		tCurrRatio = 4;
	}

	for( i = 0; i < (tCurrRatio + 1); i++ )
	{
		//����
		memcpy( tdw.b, InBuf + 11 * i, 4 );
        if( tdw.d == 0xffffffff )//645����2��3��������������ʱҪ�õ���
        {
        	memset( OutBuf + 8 * i, 0xff, 8 );
        }
        else
        {	
            tdw.d = lib_DWBinToBCD( tdw.d );
            memcpy( OutBuf + 8 * i, tdw.b, 3 );
            //��������ʱ��		
            api_TimeFormat698To645( (TRealTimer *)(InBuf+11*i+4), Buf );
            lib_ExchangeData( OutBuf+8*i+3, Buf, 5 );
        }
	}

	return(tCurrRatio + 1) * 8;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��2�ֽڹ�������ת��Ϊ645��ʽ2�ֽڹ�������
//-----------------------------------------------
BYTE FreezeCos645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE Flag;
	TTwoByteUnion tw;
	
	memcpy( tw.b, InBuf, 2 );
	
	if( tw.sw < 0 )
	{
		tw.sw *= -1;
		Flag = 1;
	}
	else
	{
		Flag = 0;
	}
	
	tw.w = lib_WBinToBCD( tw.w );
	memcpy( OutBuf, tw.b, 2 );
	
	if( Flag == 1 )
	{
		OutBuf[1] |= 0x80;
	}	

	return 2;
}

//-----------------------------------------------
//��������: ������������Դ��698��ʽ��HEX��ת��Ϊ645��ʽ��BCD�룩
//
//����: 	InBuf[in]:698��ʽ������Դ
//			OutBuf[out]��645��ʽ������
//����ֵ:	645��ʽ���ݳ���
//
//��ע:��2�ֽڹ�������ת��Ϊ645��ʽ2�ֽڹ�����������ABC�ࣩ��ֻ��������ô˺���
//-----------------------------------------------
BYTE FreezeCosBlock645Transform( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i,Flag;
	TTwoByteUnion tw;
	for( i = 0; i < 4; i++ )
	{
		memcpy( tw.b, InBuf + 2 * i, 2 );
		if( tw.sw < 0 )
		{
			tw.sw *= -1;
			Flag = 1;
		}
		else
		{
			Flag = 0;
		}
		
		tw.w = lib_WBinToBCD( tw.w );
		memcpy( OutBuf + 2 * i, tw.b, 2 );
		if( Flag == 1 )
		{
			OutBuf[2*i+1] |= 0x80;
		}
	}

	return 4 * 2;
}


//-----------------------------------------------
//��������: �������¼645��˲ʱ��Լ�������㡢�ն��ᣩ
//
//����:
//				DI[in]:	���ݱ�ʶ
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit0~bit14 �����صĶ������ݳ���
//
//��ע:֧�����ݿ飬��֧�ֶ�ʱ����
//-----------------------------------------------
WORD api_ReadFreezeRecord645( BYTE *DI,BYTE *pOutBuf )
{
	BYTE i, j,tFreezeIndex;
	TFourByteUnion tdw,tOAD;
	BYTE Buf[260];
	WORD Len,wReturnLen;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
    TFreezeData Data;
	
	memcpy( tdw.b, DI, 4 );
	wReturnLen = 0;
	Len = 0;
	
	if( tdw.b[3] != 0x05 )
	{
		return 0x8000;
	}
	
	//���Ҷ�������
	for( i = 0; i < sizeof(FreezeType645MapTab) / sizeof(TFreezeType645Map); i++ )
	{
		if( FreezeType645MapTab[i].DIUnit == tdw.b[2] )
		{
			break;
		}
	}
	
	if( i == sizeof(FreezeType645MapTab) / sizeof(TFreezeType645Map) )
	{
		return 0x8000;
	}
	
	tFreezeIndex = FreezeType645MapTab[i].FreezeIndex;
	if( tFreezeIndex >= eFREEZE_MAX_NUM )
	{
		return 0x8000;
	}
	
	// ��ø��ֵ�ַ
	if( GetFreezeAddrInfo( tFreezeIndex, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[tFreezeIndex].MaxAttNum )
	{
		return 0x8000;
	}

	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[tFreezeIndex].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );
	//�������еĹ������Զ��� �������
	api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	//����������RecordNo
	if( api_VReadSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo ) != TRUE )
	{
		return 0x8000;
	}
	
	//�ֽⰴ��698�ķ���9��������
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG; 
	DLT698RecordPara.OADNum = 1;
	DLT698RecordPara.TimeOrLast = tdw.b[0];
	Data.pDLT698RecordPara = &DLT698RecordPara; 
	Data.Tag = eData;
	Data.FreezeIndex = tFreezeIndex;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	
	if( tFreezeIndex == eFREEZE_HOUR )//Сʱ����
	{
		//���Ҵ��ֶ����ڵ�������
		if( tdw.b[1] == 0xff )
		{
			for( j = 0; j < sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map); j++ )
			{
				//Ҫ���ĵ��Ѿ�����
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = FreezeHour645MapTab[j].OAD;
					lib_InverseData( tOAD.b, 4 );
	
					Data.pDLT698RecordPara->pOAD = tOAD.b;
	
					//RSD ����9�����մ�������
					Len = ReadFreezeByLastNum( &Data );
					if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
						return 0x8000;
					}
					
					Len = FreezeHour645MapTab[j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen ); 
				}
				
				//Ҫ���ĵ㻹û�в���
				if( tdw.b[0] > DataInfo.SaveDot )
				{
					Len = FreezeHour645MapTab[j].DataLen;
					memset( pOutBuf + wReturnLen, 0x00, Len );
				}
				
				wReturnLen += Len;
				pOutBuf[wReturnLen] = 0xaa;
				wReturnLen++;
			}
		}
		else
		{
			for( i = 0; i < sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map); i++ )
			{
				if( FreezeHour645MapTab[i].DIUnit == tdw.b[1] )
				{
					break;
				}
			}
			if( i == sizeof(FreezeHour645MapTab) / sizeof(TFreeze645Map) )
			{
				return 0x8000;
			}

			//Ҫ���ĵ��Ѿ�����
			if( tdw.b[0] <= DataInfo.SaveDot )
			{
				tOAD.d = FreezeHour645MapTab[i].OAD;
				lib_InverseData( tOAD.b, 4 );
	
				Data.pDLT698RecordPara->pOAD = tOAD.b;
	
				//RSD ����9�����մ�������
				Len = ReadFreezeByLastNum( &Data );
				if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
				{
					return 0x8000;
				}
				
				wReturnLen = FreezeHour645MapTab[i].Freeze645TransformFunc( Data.pBuf, pOutBuf ); 
			}
			
			//Ҫ���ĵ㻹û�в���
			if( tdw.b[0] > DataInfo.SaveDot )
			{
				wReturnLen = FreezeHour645MapTab[i].DataLen;
				memset( pOutBuf, 0x00, wReturnLen );
			}
		}
	}
	else//�ն���
	{
		//���Ҵ��ֶ����ڵ�������
		if( tdw.b[1] == 0xff )
		{
			for( j = 0; j < sizeof(Freeze645MapTab) / sizeof(TFreeze645Map); j++ )
			{
				//Ҫ���ĵ��Ѿ�����
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = Freeze645MapTab[j].OAD;
					lib_InverseData( tOAD.b, 4 );
		
					Data.pDLT698RecordPara->pOAD = tOAD.b;
		
					//RSD ����9�����մ�������
					Len = ReadFreezeByLastNum( &Data );
					if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
						//�������쳣ʱ��������ֱ����AA�Թ�
						if( (Freeze645MapTab[j].OAD == 0x20050200)||(Freeze645MapTab[j].OAD == 0x20040200) )
						{
							memset( pOutBuf + wReturnLen, 0xFF, 12 );
							wReturnLen += 12;

							if( Freeze645MapTab[j].FollowFlag == 0 )
							{					
								pOutBuf[wReturnLen] = 0xaa;
								wReturnLen++;
							}

							continue;
						}
						else
						{
							pOutBuf[wReturnLen] = 0xaa;
							wReturnLen++;
							continue;
						}
					}					
					
				}
				
				Len = Freeze645MapTab[j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
				
				if( tdw.b[0] > DataInfo.SaveDot ) 
				{
					//Ҫ���ĵ㻹û�в���					
					#if(MAX_PHASE == 1)
					//�����֧�ֵ�������aa�չ����޹���������
					if( (j >= 0x03)&&(j <= 0x0a) )
					{
						pOutBuf[wReturnLen] = 0xaa;
						wReturnLen++;
						continue;
					}
					if( j == 0x0c )//�޹��������ff
					{
						memset( pOutBuf + wReturnLen, 0xff, Len );
					}
					else if( j == 0x0b ) //�й�����
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );//�� A
						memset( pOutBuf + wReturnLen + 6, 0xff, Len );//B C
					}
					else
					#endif
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );
					}
				}
				
				wReturnLen += Len;
	
				if( Freeze645MapTab[j].FollowFlag == 0 )
				{					
					pOutBuf[wReturnLen] = 0xaa;
					wReturnLen++;
				}
			}
		}
		else
		{
			for( i = 0; i < sizeof(Freeze645MapTab) / sizeof(TFreeze645Map); i++ )
			{
				if( Freeze645MapTab[i].DIUnit == tdw.b[1] )
				{
					break;
				}
			}
			if( i == sizeof(Freeze645MapTab) / sizeof(TFreeze645Map) )
			{
				return 0x8000;
			}
			
			for( j = 0; j < 10; j++ ) //������10�Σ��ڱ����û������10�ε�����������FollowFlag����������
			{
				if ((i + j) > (sizeof(Freeze645MapTab) / sizeof(TFreeze645Map) - 1))
				{
					break;
				}
				//Ҫ���ĵ��Ѿ�����
				if( tdw.b[0] <= DataInfo.SaveDot )
				{
					tOAD.d = Freeze645MapTab[i + j].OAD;
					lib_InverseData( tOAD.b, 4 );
			
					Data.pDLT698RecordPara->pOAD = tOAD.b;
					
					//RSD ����9�����մ�������
					Len = ReadFreezeByLastNum( &Data );
										
                    if( (Len == 0)||(Len == 1)||(Len == 0x8000) )
					{
					    if( (Freeze645MapTab[i + j].OAD == 0x20050200)||(Freeze645MapTab[i + j].OAD == 0x20040200) )
    					{
							memset( pOutBuf + wReturnLen, 0xFF, 12 );
							wReturnLen += 12;
							continue;
    					}
    					else
    					{
						    return 0x8000;
    					}
					}					
					
				}
		
				Len = Freeze645MapTab[i+j].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );				

				//Ҫ���ĵ㻹û�в���
				if( tdw.b[0] > DataInfo.SaveDot )
				{					
					//Ҫ���ĵ㻹û�в���
					#if(MAX_PHASE == 1)
					//�����֧�ֵ�������ش����޹���������
					if( ((i + j) >= 0x03)&&((i + j) <= 0x0a) )
					{
						return 0x8000;
					}
					else if( (i + j) == 0x0c )//�޹����ʷ���ff
					{
						memset( pOutBuf + wReturnLen, 0xff, Len ); 
					}
					else if( (i + j) == 0x0b ) //�й�����
					{
						memset( pOutBuf + wReturnLen, 0x00, Len ); //�� A
						memset( pOutBuf + wReturnLen + 6, 0xff, Len ); //B C
					}
					else
					#endif
					{
						memset( pOutBuf + wReturnLen, 0x00, Len );
					}
				}
			
				wReturnLen += Len; 
				
				if( Freeze645MapTab[i + j].FollowFlag == 0 )
				{
					break;
				}
			}
		}
	}
	
	return wReturnLen;	
}

//-----------------------------------------------
//��������: �������¼645�����ܽ��㡢�������㣩
//
//����:
//				DI[in]:	���ݱ�ʶ
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit0~bit14 �����صĶ������ݳ���
//
//��ע:��֧�����ݿ�
//-----------------------------------------------
WORD api_ReadClosingRecord645( BYTE *DI, BYTE *pOutBuf )
{
	BYTE i, j, tFreezeIndex;
	TFourByteUnion tdw, tOAD;
	BYTE Buf[200];
	WORD wReturnLen,Len;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
	TFreezeData Data; 

	memcpy(tdw.b,DI,4);
	wReturnLen = 0;

	if( (tdw.b[3] != 0x00)&&(tdw.b[3] != 0x01) )
	{
		return 0x8000;
	}
	
	if( (tdw.b[1] != 0xff)&&(tdw.b[1] > FPara1->TimeZoneSegPara.Ratio) )
	{
		return 0x8000;
	}
	
	// ��ø��ֵ�ַ
	if( GetFreezeAddrInfo( eFREEZE_CLOSING, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_CLOSING].MaxAttNum )
	{
		return 0x8000;
	}

	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_CLOSING].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );
	//�������еĹ������Զ��� �������
	api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr, sizeof(TFreezeAttCycleDepth), (BYTE *)&AttCycleDepth );
	//����������RecordNo
	if( api_VReadSafeMem( FreezeAddrLen.wDataInfoEeAddr, sizeof(TFreezeDataInfo), (BYTE *)&DataInfo ) != TRUE )
	{
		return 0x8000;
	}
	
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
	DLT698RecordPara.OADNum = 1;
	DLT698RecordPara.TimeOrLast = tdw.b[0];
	Data.pDLT698RecordPara = &DLT698RecordPara;
	Data.Tag = eData;
	Data.FreezeIndex = eFREEZE_CLOSING;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	
	for( i = 0; i < sizeof(Closing645MapTab) / sizeof(TLpfClosing645Map); i++ )
	{
		if( Closing645MapTab[i].wDIUnit == tdw.w[1] )
		{
			break;
		}
	}
	if( i == sizeof(Closing645MapTab) / sizeof(TLpfClosing645Map) )
	{
		return 0x8000;
	}
		
	if( tdw.b[0] <= DataInfo.SaveDot )
	{	
		tOAD.d = Closing645MapTab[i].OAD;
		lib_InverseData( tOAD.b, 4 );
	
		Data.pDLT698RecordPara->pOAD = tOAD.b;
	
		//RSD ����9�����մ�������
		Len = ReadFreezeByLastNum( &Data );
		if( (Len == 0)||(Len == 0x8000) )
		{
			return 0x8000;
		}
		else if(Len == 1)
		{
		  	if((tOAD.d == 0x00021010)||(tOAD.d == 0x00022010))//645���ڶ�������������������0XFF
			{
				memset((BYTE *)Buf,0xFF,Data.Len);
			}
			else
			{
		  		return 0x8000;
			}
		}
		
		wReturnLen = Closing645MapTab[i].Freeze645TransformFunc( Data.pBuf, Buf );
		
		//���ַ��ʵ��ܣ�DataLenΪ5�������ʱ�䣩
		if( (Closing645MapTab[i].DataLen != 5)&&(tdw.b[1] != 0xff) )
		{
			memcpy( pOutBuf, Buf + tdw.b[1] * Closing645MapTab[i].DataLen, Closing645MapTab[i].DataLen );
			wReturnLen = Closing645MapTab[i].DataLen;
		}
		else
		{
			if( wReturnLen < 160 )
			{
				if( wReturnLen == 5 )
				{
					//����ʱ��Ϊ4���ֽڣ�ʱ�����꣬��Ҫ����һ�¡������Ϊ��ͨ���������ֳ����ܣ���˽�ʱ�䶨Ϊ5���ֽ�
					memcpy( pOutBuf, Buf+1, 4 ); 
				}
				else
				{
					memcpy( pOutBuf, Buf, wReturnLen );
				}
			}
			else
			{
				return 0x8000;
			}
		}
	}
	else if( tdw.b[0] == 0xff )
	{
		return 0x8000;
	}
	else
	{
		//��û�в�������,��������0
		if( Closing645MapTab[i].DataLen == 5 )
		{
			//����ʱ��Ϊ4���ֽڣ�ʱ�����꣬��Ҫ����һ�¡������Ϊ��ͨ���������ֳ����ܣ���˽�ʱ�䶨Ϊ5���ֽ�
			wReturnLen = 4;
		}
		else	
		{
			if( tdw.b[1] != 0xff )
			{
				wReturnLen = Closing645MapTab[i].DataLen;
			}
			else
			{
				wReturnLen = Closing645MapTab[i].DataLen * (FPara1->TimeZoneSegPara.Ratio + 1);
				if( wReturnLen > ((MAX_RATIO + 1) * 4) )
				{
					wReturnLen = 20;
				}
			}
		}
		memset( pOutBuf, 0x00, wReturnLen );
	}
	
	return wReturnLen;
}

//-----------------------------------------------
//��������: �����ɼ�¼645������4��
//
//����:
//				Ch[in]:	��Լͨ��
// 				ReadCmd[in]:�������� 0x11--������	0x12--����֡��ʽ��
//				DI[in]:	���ݱ�ʶ
// 				ReadTime[in]:��Լ�·���ʱ�� ��ʱ������
// 				DotNum[in]:��Լ�·�Ҫ���ĵ���
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit14��1  �����к���֡��0Ϊû�к���֡
//				bit0~bit13 �����صĶ������ݳ���
// 				����0�����Լֻ�������ݱ�ʶ
//
//��ע:ֻ֧�ֲ���4Ҫ�� pOutBuf�������Ϊ��200-4=196���������˳�����Ҫ��֡
//-----------------------------------------------
WORD api_ReadLpfRecord645( eSERIAL_TYPE Ch, BYTE ReadCmd, BYTE *DI, BYTE *ReadTime, BYTE DotNum, BYTE *pOutBuf )
{
	BYTE i, j,tFreezeIndex, tReadDotNum, tStartOadPosi, tOadNum, SingleDataLen;
	TFourByteUnion tdw, tOAD;
	TFourByteUnion tEnergyBak[4];//�������ã��ڱ�����������ĸ���������OAD
	BYTE Buf[100],ClassAttribute;
	WORD wReturnLen,Len,wOffset,OI,tEmptyDotNum;
	DWORD dwReadStartTime,dwReadEndTime,SearchTime,dwFirstAddr,dwFirstTime,dwTempOad;
	DWORD dwOad[10];//645���ɼ�¼Ҫ��ȡ��OAD��ʵ�����4��
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth AttCycleDepth;
	TFreezeAttCycleDepth MinAttCycleDepth[MAX_FREEZE_PLANNUM]; 
	TFreezeDataInfo	DataInfo;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	TDLT698RecordPara DLT698RecordPara;
	TFreezeData Data;
	TRealTimer t;
	TFreezeMinInfo FreezeMinInfo;

	memcpy( tdw.b, DI, 4 );
	
	if( tdw.w[1] != 0x0610 )
	{
		return 0x8000;
	}
	
	SingleDataLen = 0;
	wReturnLen = 0; 
	Len = 0;
	memset( tEnergyBak[0].b, 0xff, sizeof(tEnergyBak) );
	memset( (BYTE*)&Data, 0x00, sizeof(TFreezeData) );

	//�������ݱ�ʶ���Ҷ�Ӧ��OAD
	for( i = 0; i < sizeof(Lpf645MapTab) / sizeof(TLpfClosing645Map); i++ )
	{
		if( Lpf645MapTab[i].wDIUnit == tdw.w[0] )
		{
			break;
		}
	}
	if( i == sizeof(Lpf645MapTab) / sizeof(TLpfClosing645Map) )
	{
		return 0x8000;
	}

	for( j = 0; j < 10; j++ ) //������10�Σ��ڱ����û������10�ε������ʵ�����4��
	{
		//����һ�����ݵĳ��� ��Ҫ���ĸ��޹����ܣ��򳤶�Ϊ16
		SingleDataLen += Lpf645MapTab[i + j].DataLen;
		
		if( Lpf645MapTab[i + j].FollowFlag == 0 )
		{
			break;
		}
	}
	
	tStartOadPosi = i;//Ҫ����OAD�ڱ���е���ʼλ��  ��Ϊ��һ����һ��OAD
	tOadNum = j+1;		//�����ݱ�ʶ��ӦOAD�ĸ���

	// ��ø��ֵ�ַ
	if( GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen ) == FALSE )
	{
		return 0;
	}

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != TRUE )
	{
		return 0;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_MIN].MaxAttNum )
	{
		return 0;
	}

	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_MIN].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );

	//��ȡ���Ӷ����ַ��Ϣ
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( FreezeSafeRom.FreezeMinInfo ), sizeof(FreezeMinInfo), (BYTE *)&FreezeMinInfo ); 
	if( FreezeMinInfo.DataAddr[0] != FreezeAddrLen.dwDataAddr )//����0��ַ �����ַһ��
	{
		FreezeMinInfo.DataAddr[0] = FreezeAddrLen.dwDataAddr;
	}

	//�������еĹ������Զ��� �������
	for( i = 0; i < MAX_FREEZE_PLANNUM; i++ )
	{
		api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth[i] );
	}
	
	Buf[0] = 0xff;//�����ڱ����ȡ�ĵ�1��OAD��698�����������Ա��е�λ��
	for( i=0; i<tOadNum; i++ )//�ҵ�645ӳ�����OAD��Ӧ��698�����������Ա���OAD���õ�698Ҫ��ȡ�Ĺ����������Ա���OAD
	{
		if( i >= 4 )
		{
			break;
		}
		
		//����645��ʶ��Ӧ�ĵ�һ��oadѰ�ҹ������Զ�����ж�Ӧ�Ķ���
		tOAD.d = Lpf645MapTab[tStartOadPosi+i].OAD;//����������й��ܵ��� Lpf645MapTab[tStartOadPosi].OAD:0x00100201
		lib_InverseData( tOAD.b, 4 );
		tdw.d = tOAD.d&(~0xFF000000);//����698��������������õ�ѹ���ݿ飬645ֻ��A���ѹ
		dwOad[i] = tOAD.d;//Ҫ��ȡ��OAD�б��ȸ�645ӳ����е�,����for ѭ���ټ�698���᷽����
	
		lib_ExchangeData((BYTE *)&OI,(BYTE *)&dwOad[i],2);
		ClassAttribute =(BYTE)((dwOad[i]&0x001f0000)>>16);
		
		for( j = 0; j < FreezeAllInfo.NumofOad; j++ )
		{
			//����������й��ܵ��� AttOad[j].d:0x01021000 tOAD.d��������lib_InverseDataҲ��Ϊ:0x01021000,tdw.d:0x00021000
			if( (tOAD.d == (AttOad[j].Oad&(~0x00E00000)) )
				||( tdw.d == (AttOad[j].Oad&(~0x00E00000)) ) )				
			{
				if( i == 0 )//Ĭ��645Ҫ���ĵ�1��OAD��698�����������Ա��д��ڣ�����645��ȡ��֧�ֵ�1��OAD����698�����������Ա��е��������������ڷ�������������
				{
					Buf[0] = j;//�����ڱ����ȡ�ĵ�1��OAD��698�����������Ա��е�λ��
				}
				dwOad[i] = (tOAD.d | (AttOad[j].Oad&0x00E00000) );//��ȡ��OAD��645��ӳ����ȡ������698���Ա��еķ�����
				break;
			}
		}
		if( j >= FreezeAllInfo.NumofOad )
		{
			if( (OI < 0x1000) && (ClassAttribute == 2))//����OI������2δ������
			{
				dwTempOad = dwOad[i]+0x20000;//��λԪ������Ϊ04��������
				for( j = 0; j < FreezeAllInfo.NumofOad; j++ )
				{
					if( dwTempOad == (AttOad[j].Oad&(~0x00E00000)) )				
					{
						if( i == 0 )//Ĭ��645Ҫ���ĵ�1��OAD��698�����������Ա��д��ڣ�����645��ȡ��֧�ֵ�1��OAD����698�����������Ա��е��������������ڷ�������������
						{
							Buf[0] = j;//�����ڱ����ȡ�ĵ�1��OAD��698�����������Ա��е�λ��
						}
						dwOad[i] = (tOAD.d | (AttOad[j].Oad&0x00E00000) );//��ȡ��OAD��645��ӳ����ȡ������698���Ա��еķ�����
						break;
					}
				}
			}
		}
	}
	j = Buf[0];//645��ȡ�ĵ�1��OAD��698�����������Ա��е�λ��
	if( j >= FreezeAllInfo.NumofOad )
	{
		return 0; 		
	}
	
	j = CURR_PLAN(AttOad[j].Oad);//��ȡ������
	
	AttCycleDepth.Cycle = MinAttCycleDepth[j].Cycle;
	AttCycleDepth.Depth = MinAttCycleDepth[j].Depth;

	//���ҷ��Ӷ����·��Ķ����Ӧ��RecordNo
	memcpy( (BYTE *)&DataInfo, (BYTE *)&MinInfo[j], sizeof(TFreezeDataInfo) );

	//��û�в�������
	if( DataInfo.RecordNo == 0 )
	{
		return 0;
	}
	
	DLT698RecordPara.OADNum = 1; //������ʱÿ�ζ��ǰ���һ��OAD������������Ϊ����Դ��645Ҫ������ݳ��Ȳ�һ��һ�������Ե���OAD����
	Data.pDLT698RecordPara = &DLT698RecordPara;
	Data.Tag = eData;
	Data.FreezeIndex = eFREEZE_MIN;
	Data.pBuf = Buf;
	Data.Len = sizeof(Buf);
	Data.pTime = NULL;
	Data.MinPlanIndex = j;
	Data.pAttOad = &AttOad[0];
	Data.pAttCycleDepth = &AttCycleDepth;
	Data.pDataInfo = &DataInfo;
	Data.pFreezeAddrLen = &FreezeAddrLen;
	Data.pAllInfo = &FreezeAllInfo;
	Data.pMinInfo = &FreezeMinInfo;
	//�Ǻ���֡��ȡʱ���ȸ��ݹ�Լ�·�����¼Ҫ���ĵ�һ����RT1�����һ�����Ӧ��ʱ��RT2
	//�ٶ�ȡRT1��ʼ����¼�ĵ�һ����T1,����¼�����һ����T2���������ĸ�ʱ����бȽϷ�������
	//����֡ʱ������11���������¼����ʼ����ֹʱ���¼��ȡ
	if( ReadCmd  == 0x11 )
	{
		memset( (BYTE *)&LpfRemainFrame[Ch], 0X00, sizeof(TLpfRemainFrame) ); 

		//����Ҫ������ʼ���ʱ��
		lib_ExchangeData( Buf, ReadTime, 5 );
		Buf[5] = 0x00; //��
		api_TimeFormat645To698( Buf, &t );
		dwReadStartTime = api_CalcInTimeRelativeMin( &t );
		if( AttCycleDepth.Cycle == 0 )
		{
			return 0x8000;
		}		
		if( dwReadStartTime % AttCycleDepth.Cycle )
		{
			//�����ܱ�ʱ����������������һ���ܱ������ĵ�
			dwReadStartTime = (dwReadStartTime / AttCycleDepth.Cycle)*AttCycleDepth.Cycle + AttCycleDepth.Cycle;
		}
		
		DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
		DLT698RecordPara.TimeOrLast = Data.pDataInfo->SaveDot;
		DLT698RecordPara.OADNum = 2; 
		tEnergyBak[0].d = FREEZE_RECORD_TIME_OAD;
		tEnergyBak[1].d = dwOad[0];
		DLT698RecordPara.pOAD = tEnergyBak[0].b;
		ReadFreezeByLastNum( &Data );
		wOffset = api_GetProOADLen( eGetNormalData, eData, tEnergyBak[0].b, 0 );
		if( wOffset == 0x8000 )
		{
		 	return 0x8000;
		}
		memcpy((BYTE*)&t,Data.pBuf,wOffset);
		dwFirstTime=api_CalcInTimeRelativeMin( &t );
		//����Ҫ���Ľ�ֹ���ʱ��
		tReadDotNum = lib_BBCDToBin( DotNum );
		if( dwReadStartTime < dwFirstTime )
		{
			tEmptyDotNum = (dwFirstTime - dwReadStartTime) / Data.pAttCycleDepth->Cycle;
			if( tReadDotNum <= tEmptyDotNum )
			{
				return 0;
			}
			tReadDotNum -= tEmptyDotNum;
			dwReadStartTime = dwFirstTime;
		}
		if( tReadDotNum == 0x00 )
		{
		    return 0x8000;
		}
		
		dwReadEndTime = dwReadStartTime + AttCycleDepth.Cycle * (tReadDotNum-1);
		if( dwReadEndTime > g_RelativeMin )
		{
			dwReadEndTime = (g_RelativeMin / AttCycleDepth.Cycle)*AttCycleDepth.Cycle;
		}
		
		//Ѱ��Ҫ���Ĳ������Ѿ���¼�����ĵ�һ����
//        DLT698RecordPara.eTimeOrLastFlag = eFREEZE_TIME_FLAG; 
//		DLT698RecordPara.TimeOrLast = dwReadStartTime;
//		DLT698RecordPara.pOAD = (BYTE *)&dwOad[0];
//		Len = SearchFreezeRecordNoByTime( &Data );
//		if( Len == FALSE )
//		{
//			return 0;
//		}
//		dwReadStartTime = Data.SearchTime;
//
		//Ѱ�������¼��һ���㣬����ֱ��ʹ��Data.SearchTimeֱ���ѣ��˺���û���õ�Data.SearchTime
		DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
		DLT698RecordPara.TimeOrLast = 1;
		DLT698RecordPara.OADNum = 2; 
		tEnergyBak[0].d = FREEZE_RECORD_TIME_OAD;
		tEnergyBak[1].d = dwOad[0];
		DLT698RecordPara.pOAD = tEnergyBak[0].b;
		Len = ReadFreezeByLastNum( &Data );
		if( Len == FALSE )
		{
			return 0;
		}
		
		//�ݴ��ֹ���ʱ�䣬�������еĺ���֡ʹ��
		LpfRemainFrame[Ch].EndTime = dwReadEndTime;
		LpfRemainFrame[Ch].RemainDotNum = tReadDotNum;
		
		//�÷���ʱ��
		api_ToAbsTime( dwReadStartTime, &t );
		api_TimeFormat698To645( &t, pOutBuf + wReturnLen );
		lib_InverseData( pOutBuf + wReturnLen, 5 );
		wReturnLen += 5;
	}
	else
	{
		//����Ҫ������ʼ���ʱ��
		dwReadStartTime = LpfRemainFrame[Ch].StartTime;
		//����Ҫ���Ľ�ֹ���ʱ��
		dwReadEndTime = LpfRemainFrame[Ch].EndTime;
	}
	
	if( dwReadStartTime > dwReadEndTime )
	{
		return 0;
	}
	
	
	DLT698RecordPara.eTimeOrLastFlag = eFREEZE_TIME_FLAG; 
	DLT698RecordPara.OADNum = 1; 
	DLT698RecordPara.pOAD =  (BYTE *)&dwOad[0];
	
	for( SearchTime = dwReadStartTime; SearchTime <= dwReadEndTime;  )
	{		
		CLEAR_WATCH_DOG; 

		//һ�����ĵ���󳤶ȣ�������Ҫ��֡
		if( (wReturnLen + SingleDataLen) > 195 )
		{
			//�ú���֡��־
			wReturnLen |= 0x4000;
			//����˴�Ҫ������ʱ�䣬���´κ���֡����ʼʱ��ʹ��
			LpfRemainFrame[Ch].StartTime = SearchTime;
			LpfRemainFrame[Ch].EndTime = dwReadEndTime;
			break;
		}

		if( LpfRemainFrame[Ch].RemainDotNum == 0 )
		{
			return 0;
		}
		
		//���ص����ݼ�
		LpfRemainFrame[Ch].RemainDotNum--;
		
		for( i = 0; i < tOadNum; i++ )
		{
			if( i >= 4 )
			{
				break;
			}
			j = CURR_PLAN(dwOad[i]);//��ȡ������
			if(j != Data.MinPlanIndex ) //�������һ�����ܲ���ͬһ�����еĵ���ֱ�����ȫFF
			{
				//�������ݿ��в�֧�ֵ����������ff
				memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi + i].DataLen ); 
				wReturnLen += Lpf645MapTab[tStartOadPosi+i].DataLen;
				continue;
			}
			
			DLT698RecordPara.TimeOrLast = SearchTime;
			Data.SearchTime = 0x5555AAAA;
			DLT698RecordPara.pOAD = (BYTE *)&dwOad[i];
			Len = ReadFreezeByTime( &Data );
			
			if( (Data.SearchTime == SearchTime)&&(Len != 1) )
			{
				Lpf645MapTab[tStartOadPosi+i].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
				if( Lpf645MapTab[tStartOadPosi + i].DataLen == 4 )
				{
					memcpy( tEnergyBak[i].b, pOutBuf + wReturnLen, 4 );
				}
			}
			else		
			{
				if( SearchTime == dwReadStartTime )
				{
					tdw.d = Data.RecordNo;
					//�����Ǳ���ĵ�һ���㣬�������һ��RecordNo��Ӧ�㣬����ֱ�Ӹ�ֵ�ϴζ�ȡ������
					if( Data.SearchTime != 0x5555AAAA )
					{
						if( Data.RecordNo == 0 )
						{
							return 0;
						}
						
						if( Data.RecordNo )
						{
							Data.RecordNo--;
						}
					}
					else
					{
						Data.RecordNo = MinInfo[j].RecordNo;
							
						if( Data.RecordNo )
						{
							Data.RecordNo--;
						}
					}
					
					Len = ReadFreezeByRecordNo( &Data );
					if( (Len == 0x8000)||(Len == 0) )
					{
						return 0;
					}
					Data.RecordNo = tdw.d; 
				}
				
				if( (Len == 1)&&(tOadNum > 1) )
				{
					//�������ݿ��в�֧�ֵ����������ff
					memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi + i].DataLen ); 
				}
				else
				{
					//������
					if( Lpf645MapTab[tStartOadPosi+i].DataLen != 4 )
					{
						memset( pOutBuf + wReturnLen, 0xff, Lpf645MapTab[tStartOadPosi+i].DataLen );
					}
					else
					{
						//����һ������,���Ƕ��ĵ�һ��������Ҫ�Ǻ���֡��������
						//�������һ��RecordNo��Ӧ�㣬����ֱ�Ӹ�ֵ�ϴζ�ȡ������
						if( SearchTime == dwReadStartTime )
						{
							//�����Ǳ���ĵ�һ���㣬�������һ��RecordNo��Ӧ�㣬����ֱ�Ӹ�ֵ�ϴζ�ȡ������
							Lpf645MapTab[tStartOadPosi + i].Freeze645TransformFunc( Data.pBuf, pOutBuf + wReturnLen );
							if( Lpf645MapTab[tStartOadPosi + i].DataLen == 4 )
							{
								memcpy( tEnergyBak[i].b, pOutBuf + wReturnLen, 4 );
							}
						}
						else
						{
							memcpy( pOutBuf + wReturnLen, tEnergyBak[i].b, 4 );
						}
					}
				}
			}
			wReturnLen += Lpf645MapTab[tStartOadPosi+i].DataLen;			
		}
		
		SearchTime += AttCycleDepth.Cycle;
		
	}

	return wReturnLen;
}


//-----------------------------------------------
//��������: �����ɼ�¼���ʱ��
//
//����:
//				DI[in]:	���ݱ�ʶ
//  			pOutBuf[out]: ��������
//
//����ֵ:		bit15λ��1 �������޴����� ��0������������ȷ���أ�
//				bit0~bit14 �����صļ��ʱ�䳤��
//
//��ע:����645 ���ɼ�¼���ʱ���698������еļ��ʱ�������һ����Ӧ���˴�ֻ��Ϊ�˽��DB1�������������ȶ����TIME����ʵ���Բ�֧��
//-----------------------------------------------
WORD api_ReadFreezeLpf645IntervalTime( BYTE *DI, BYTE *pOutBuf )
{
	BYTE i;
	TFourByteUnion tdw;
	TFreezeAttOad	AttOad[MAX_FREEZE_ATTRIBUTE + 1];
	TFreezeAttCycleDepth MinAttCycleDepth;
	TFreezeAddrLen FreezeAddrLen;
	TFreezeAllInfoRom FreezeAllInfo;
	
	memcpy( tdw.b, DI, 4 );
	
	if( (tdw.d < 0x04000a02)||(tdw.d > 0x04000a07) )
	{
		return 0x8000;
	}
		
	tdw.d = Lpf645IntervalTime[DI[0] - 2];
	lib_InverseData( tdw.b, 4 );
	
	// ��ø��ֵ�ַ
	if( GetFreezeAddrInfo( eFREEZE_MIN, &FreezeAddrLen ) == FALSE )
	{
		return 0x8000;
	}

	//������������ĳ��ȼ��������Եĸ���
	if( api_VReadSafeMem( FreezeAddrLen.wAllInfoAddr, sizeof(TFreezeAllInfoRom), (BYTE *)&FreezeAllInfo ) != 	TRUE )
	{
		return 0x8000;
	}

	if( FreezeAllInfo.NumofOad > FreezeInfoTab[eFREEZE_MIN].MaxAttNum )
	{
		return 0x8000;
	}

	//�������еĹ������Զ���
	api_VReadSafeMem( FreezeAddrLen.dwAttOadEeAddr, sizeof(TFreezeAttOad) * FreezeInfoTab[eFREEZE_MIN].MaxAttNum + sizeof(DWORD), (BYTE *)&AttOad[0] );


	//�ȽϹ������Զ��� ��ȡ����
	for( i = 0; i < FreezeAllInfo.NumofOad; i++ )
	{
		if( (tdw.d & 0x0000ffff) == (AttOad[i].Oad & 0x0000ffff) )
		{
			//������ȡ��������
			i = CURR_PLAN(AttOad[i].Oad);
			api_VReadSafeMem( FreezeAddrLen.dwAttCycleDepthEeAddr + sizeof(TFreezeAttCycleDepth) * i, sizeof(TFreezeAttCycleDepth), (BYTE *)&MinAttCycleDepth ); 
			tdw.w[0] = lib_WBinToBCD( MinAttCycleDepth.Cycle );
			memcpy( pOutBuf, tdw.b, 2 );
			return 2;
		}
	}
	
	return 0x8000;	
}


#endif//#if(SEL_DLT645_2007 == YES)


