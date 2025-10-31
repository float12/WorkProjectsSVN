//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2018.08.13
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
typedef struct
{
	const WORD				ID21_32;		//645���ݱ�ʶ ID2,ID3
	const TFourByteUnion	OAD;	        //698���ݱ�ʶ
	const BYTE				TableNum;	    //Table����
	const DWORD*      		pRelationTable;	//��Ӧ�Ĺ����������Ա�
}TEvent645Obj_t;


//-----------------------------------------------
//				ȫ��ʹ�õı���������

//-----------------------------------------------
 
//-----------------------------------------------
//				���ļ�ʹ�õı���������
static const DWORD Start_EndTime_Table[] =
{
    //2,          //����
    0x00021e20, //�¼�����ʱ��
    0x00022020,  //�¼�����ʱ��
};


static const DWORD Adjust_Time_Table[] =
{
	//3,          //����
	0x55555555,  //�����ߴ���
	//0x00220040, //Уʱǰʱ��
	0x00021E20,//���¼�����ǰʱ��������Уʱǰʱ��
	//0x00820040,//Уʱ��ʱ��
	0x00022020,//���¼�����ʱ��������Уʱ��ʱ��
};
static const DWORD Relay_Table[] =
{
    //8,            //����
    0x00021E20,   //�¼�����ʱ��
    0x55555555,   //�����ߴ���
    0x01221000,   //�����й��ܵ���    
    0x01222000,   //�����й��ܵ���    
    0x01225000,   //��һ�����޹��ܵ���
    0x01226000,   //�ڶ������޹��ܵ���
    0x01227000,   //���������޹��ܵ���
    0x01228000,   //���������޹��ܵ���
};

static const DWORD Prog_Table[] =
{
    0x00021E20,   //�¼�����ʱ��
    0x55555555,   //�����ߴ���
    0x66666666,   //��̼�¼����
};
static const DWORD Clear_Event_Table[] =
{
    0x00021E20,   //�¼�����ʱ��
    0x55555555,   //�����ߴ���
    0x55555555,   //�¼��������ݱ�ʶ��
};
static const DWORD Cover_Table[] =
{
    //14,            //����
    0x00021E20,   //�¼�����ʱ��
    0x00022020,   //�¼�����ʱ��
    0x01221000,   //�¼�����ʱ�������й��ܵ���    
    0x01222000,   //�¼�����ʱ�̷����й��ܵ���    
    0x01225000,   //�¼�����ʱ�̵�һ�����޹��ܵ���
    0x01226000,   //�¼�����ʱ�̵ڶ������޹��ܵ���
    0x01227000,   //�¼�����ʱ�̵��������޹��ܵ���
    0x01228000,   //�¼�����ʱ�̵��������޹��ܵ���
    0x01821000,   //�¼�����ʱ�������й��ܵ���    
    0x01822000,   //�¼�����ʱ�̷����й��ܵ���    
    0x01825000,   //�¼�����ʱ�̵�һ�����޹��ܵ���
    0x01826000,   //�¼�����ʱ�̵ڶ������޹��ܵ���
    0x01827000,   //�¼�����ʱ�̵��������޹��ܵ���
    0x01828000,   //�¼�����ʱ�̵��������޹��ܵ���
};

static const DWORD Magnetic_PowerErr_Table[] =
{
    //6,            //����
    0x00021E20,   //�¼�����ʱ��
    0x00022020,   //�¼�����ʱ��
    0x01221000,   //�����й��ܵ���    
    0x01222000,   //�����й��ܵ���    
    0x01821000,   //�����й��ܵ���    
    0x01822000,   //�����й��ܵ���    
};
static const DWORD Realy_Err_Table[] =
{
    //7,            //����
    0x00021E20,   //�¼�����ʱ��
    0x00022020,   //�¼�����ʱ��
    0x00020F40,	  //�̵�����״̬-������Կ����-ռλ��-1�ֽ�
    0x01221000,   //�����й��ܵ���    
    0x01222000,   //�����й��ܵ���    
    0x01821000,   //�����й��ܵ���    
    0x01822000,   //�����й��ܵ���    
};

static const DWORD ClosingDay_Prog_Table[] =
{
	//5,            //����
	0x00021E20,   //�¼�����ʱ��
	0x55555555,   //�����ߴ���
	0x01221641,   //�����ձ��ǰÿ�µ�1����������
	0x02221641,   //�����ձ��ǰÿ�µ�2����������
	0x03221641,   //�����ձ��ǰÿ�µ�3����������
};
static const DWORD Over_I_Table[] = 
{
    //49,              //����
    0x00021E20,      //�¼�����ʱ��
    0x01221000,      //�¼�����ʱ�������й��ܵ���    
    0x01222000,      //�¼�����ʱ�̷����й��ܵ���    
    0x01223000,      //�¼�����ʱ������޹�1�ܵ���   
    0x01224000,      //�¼�����ʱ������޹�2�ܵ���   
    0x01221100,      //�¼�����ʱ��A�������й�����   
    0x01222100,      //�¼�����ʱ��A�෴���й�����   
    0x01223100,      //�¼�����ʱ��A������޹�1����  
    0x01224100,      //�¼�����ʱ��A������޹�2����  
    0x01220020,      //�¼�����ʱ��A��ѹ              
    0x01220120,      //�¼�����ʱ��A����              
    0x02220420,      //�¼�����ʱ��A�й�����          
    0x02220520,      //�¼�����ʱ��A�޹�����          
    0x02220A20,      //�¼�����ʱ��A��������          
    0x01221200,      //�¼�����ʱ��B�������й�����   
    0x01222200,      //�¼�����ʱ��B�෴���й�����   
    0x01223200,      //�¼�����ʱ��B������޹�1����  
    0x01224200,      //�¼�����ʱ��B������޹�2����  
    0x02220020,      //�¼�����ʱ��B��ѹ              
    0x02220120,      //�¼�����ʱ��B����              
    0x03220420,      //�¼�����ʱ��B�й�����          
    0x03220520,      //�¼�����ʱ��B�޹�����          
    0x03220A20,      //�¼�����ʱ��B��������          
    0x01221300,      //�¼�����ʱ��C�������й�����   
    0x01222300,      //�¼�����ʱ��C�෴���й�����   
    0x01223300,      //�¼�����ʱ��C������޹�1����  
    0x01224300,      //�¼�����ʱ��C������޹�2����  
    0x03220020,      //�¼�����ʱ��C��ѹ              
    0x03220120,      //�¼�����ʱ��C����              
    0x04220420,      //�¼�����ʱ��C�й�����          
    0x04220520,      //�¼�����ʱ��C�޹�����          
    0x04220A20,      //�¼�����ʱ��C��������          
    0x00022020,      //�¼�����ʱ��
    0x01821000,      //�¼�����ʱ�������й��ܵ���    
    0x01822000,      //�¼�����ʱ�̷����й��ܵ���    
    0x01823000,      //�¼�����ʱ������޹�1�ܵ���   
    0x01824000,      //�¼�����ʱ������޹�2�ܵ���   
    0x01821100,      //�¼�����ʱ��A�������й�����   
    0x01822100,      //�¼�����ʱ��A�෴���й�����   
    0x01823100,      //�¼�����ʱ��A������޹�1����  
    0x01824100,      //�¼�����ʱ��A������޹�2����         
    0x01821200,      //�¼�����ʱ��B�������й�����   
    0x01822200,      //�¼�����ʱ��B�෴���й�����   
    0x01823200,      //�¼�����ʱ��B������޹�1����  
    0x01824200,      //�¼�����ʱ��B������޹�2����          
    0x01821300,      //�¼�����ʱ��C�������й�����   
    0x01822300,      //�¼�����ʱ��C�෴���й�����   
    0x01823300,      //�¼�����ʱ��C������޹�1����  
    0x01824300,      //�¼�����ʱ��C������޹�2����        
};

static const DWORD Lost_V_Table[] = 
{
    //53,              //����
    0x00021E20,      //�¼�����ʱ��
    0x01221000,      //�¼�����ʱ�������й��ܵ���    
    0x01222000,      //�¼�����ʱ�̷����й��ܵ���    
    0x01223000,      //�¼�����ʱ������޹�1�ܵ���   
    0x01224000,      //�¼�����ʱ������޹�2�ܵ���   
    0x01221100,      //�¼�����ʱ��A�������й�����   
    0x01222100,      //�¼�����ʱ��A�෴���й�����   
    0x01223100,      //�¼�����ʱ��A������޹�1����  
    0x01224100,      //�¼�����ʱ��A������޹�2����  
    0x01220020,      //�¼�����ʱ��A��ѹ              
    0x01220120,      //�¼�����ʱ��A����              
    0x02220420,      //�¼�����ʱ��A�й�����          
    0x02220520,      //�¼�����ʱ��A�޹�����          
    0x02220A20,      //�¼�����ʱ��A��������          
    0x01221200,      //�¼�����ʱ��B�������й�����   
    0x01222200,      //�¼�����ʱ��B�෴���й�����   
    0x01223200,      //�¼�����ʱ��B������޹�1����  
    0x01224200,      //�¼�����ʱ��B������޹�2����  
    0x02220020,      //�¼�����ʱ��B��ѹ              
    0x02220120,      //�¼�����ʱ��B����              
    0x03220420,      //�¼�����ʱ��B�й�����          
    0x03220520,      //�¼�����ʱ��B�޹�����          
    0x03220A20,      //�¼�����ʱ��B��������          
    0x01221300,      //�¼�����ʱ��C�������й�����   
    0x01222300,      //�¼�����ʱ��C�෴���й�����   
    0x01223300,      //�¼�����ʱ��C������޹�1����  
    0x01224300,      //�¼�����ʱ��C������޹�2����  
    0x03220020,      //�¼�����ʱ��C��ѹ              
    0x03220120,      //�¼�����ʱ��C����              
    0x04220420,      //�¼�����ʱ��C�й�����          
    0x04220520,      //�¼�����ʱ��C�޹�����          
    0x04220A20,      //�¼�����ʱ��C��������
    0x01622920,      //�¼������ڼ��ܰ�ʱֵ
    0x02622920,      //�¼������ڼ�A��ʱֵ
    0x03622920,      //�¼������ڼ�B��ʱֵ
    0x04622920,      //�¼������ڼ�C��ʱֵ
    0x00022020,      //�¼�����ʱ��
    0x01821000,      //�¼�����ʱ�������й��ܵ���    
    0x01822000,      //�¼�����ʱ�̷����й��ܵ���    
    0x01823000,      //�¼�����ʱ������޹�1�ܵ���   
    0x01824000,      //�¼�����ʱ������޹�2�ܵ���   
    0x01821100,      //�¼�����ʱ��A�������й�����   
    0x01822100,      //�¼�����ʱ��A�෴���й�����   
    0x01823100,      //�¼�����ʱ��A������޹�1����  
    0x01824100,      //�¼�����ʱ��A������޹�2����         
    0x01821200,      //�¼�����ʱ��B�������й�����   
    0x01822200,      //�¼�����ʱ��B�෴���й�����   
    0x01823200,      //�¼�����ʱ��B������޹�1����  
    0x01824200,      //�¼�����ʱ��B������޹�2����          
    0x01821300,      //�¼�����ʱ��C�������й�����   
    0x01822300,      //�¼�����ʱ��C�෴���й�����   
    0x01823300,      //�¼�����ʱ��C������޹�1����  
    0x01824300,      //�¼�����ʱ��C������޹�2����        
};

static const DWORD Clear_Meter_Table[] = 
{
    //26,           //����
    0x00021E20,     //�¼�����ʱ��
    0x55555555,		//�����ߴ���
    0x01221000,     //�¼�����ʱ�������й��ܵ���    
    0x01222000,     //�¼�����ʱ�̷����й��ܵ���        
    0x01225000,   	//�¼�����ʱ�̵�һ�����޹��ܵ���
    0x01226000,  	//�¼�����ʱ�̵ڶ������޹��ܵ���
    0x01227000,   	//�¼�����ʱ�̵��������޹��ܵ���
    0x01228000,  	//�¼�����ʱ�̵��������޹��ܵ���
    0x01221100,     //�¼�����ʱ��A�������й�����   
    0x01222100,     //�¼�����ʱ��A�෴���й����� 
    0x01225100,   	//�¼�����ʱ��A���һ�����޹��ܵ���
    0x01226100,  	//�¼�����ʱ��A��ڶ������޹��ܵ���
    0x01227100,   	//�¼�����ʱ��A����������޹��ܵ���
    0x01228100,  	//�¼�����ʱ��A����������޹��ܵ���
    0x01221200,     //�¼�����ʱ��B�������й�����   
    0x01222200,     //�¼�����ʱ��B�෴���й����� 
    0x01225200,   	//�¼�����ʱ��B���һ�����޹��ܵ���
    0x01226200,  	//�¼�����ʱ��B��ڶ������޹��ܵ���
    0x01227200,   	//�¼�����ʱ��B����������޹��ܵ���
    0x01228200,  	//�¼�����ʱ��B����������޹��ܵ���
    0x01221300,     //�¼�����ʱ��C�������й�����   
    0x01222300,     //�¼�����ʱ��C�෴���й����� 
    0x01225300,   	//�¼�����ʱ��C���һ�����޹��ܵ���
    0x01226300,  	//�¼�����ʱ��C��ڶ������޹��ܵ���
    0x01227300,   	//�¼�����ʱ��C����������޹��ܵ���
    0x01228300,  	//�¼�����ʱ��C����������޹��ܵ���
};

//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
#define EVENT645_03NUM          SearchEvent645DI3Is03Num()

static const TEvent645Obj_t Event645Obj[] =
{
    //ID2ID1        698���ݱ�ʶ	      Table����   ��Ӧ�Ĺ����������Ա�
    #if( SEL_EVENT_LOST_POWER == YES )//�����¼�
	{ 0x1100,       0x30110200,		sizeof(Start_EndTime_Table)/sizeof(DWORD),		Start_EndTime_Table		},
	#endif
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//�������
	{ 0x3001,		0x30130200,		sizeof(Clear_Meter_Table)/sizeof(DWORD), 		Clear_Meter_Table		},
	#endif
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//Уʱ�¼�
	{ 0x3004,		0x30160200,		sizeof(Adjust_Time_Table)/sizeof(DWORD), 		Adjust_Time_Table		},
	#endif
	#if( SEL_EVENT_METERCOVER == YES )//������¼�
	{ 0x300D,       0x301B0200,     sizeof(Cover_Table)/sizeof(DWORD),    			Cover_Table             },
	#endif
    #if( SEL_EVENT_BUTTONCOVER == YES )//����Ŧ���¼� ����
    { 0x300E,       0x301C0200,     sizeof(Cover_Table)/sizeof(DWORD),    			Cover_Table             },
	#endif
    #if( SEL_EVENT_MAGNETIC_INT == YES )//�㶨�ų����� ����
    { 0x3500,       0x302A0200,     sizeof(Magnetic_PowerErr_Table)/sizeof(DWORD),  Magnetic_PowerErr_Table	},
	#endif
    #if( SEL_EVENT_POWER_ERR == YES )//��Դ�쳣 ����
    { 0x3700,       0x302C0200,     4,												Magnetic_PowerErr_Table	},
	#endif
	#if( SEL_EVENT_RELAY_ERR == YES )//���ɿ�������
	{0x3600,		0x302B0200,		sizeof(Realy_Err_Table)/sizeof(DWORD),			Realy_Err_Table			},
	#endif
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//�¼������¼�
	{0x3003,		0x30150200,		sizeof(Clear_Event_Table)/sizeof(DWORD),		Clear_Event_Table		},
	#endif
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//�����ձ���¼�
	{0x300C,		0x301A0200,		sizeof(ClosingDay_Prog_Table)/sizeof(DWORD),	ClosingDay_Prog_Table	},
	#endif

    {0xFFFF,		0xFFFFFFFF,		sizeof(Start_EndTime_Table)/sizeof(DWORD),	    Start_EndTime_Table	    },
	//�����¼���DI3Ϊ03�ġ���֧�ַ�������¼��������� EVENT645_03NUM �ϸ��Ӧ ����������Ҫ�� EVENT645_03NUM ͬ��
    
    //ID3ID2        698���ݱ�ʶ	       Table����   ��Ӧ�Ĺ����������Ա�
    #if( SEL_EVENT_LOST_V == YES )//ʧѹ�¼� ����
    { 0x1000,       0x30000700,     sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //A��ʧѹ�¼�	
    { 0x1001,       0x30000700,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //B��ʧѹ�¼�
    { 0x1002,       0x30000800,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //C��ʧѹ�¼�
    { 0x1003,       0x30000900,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
	#endif
	
    #if( SEL_EVENT_LOST_I == YES ) //A��ʧ���¼� ����
    { 0x1801,       0x30040700,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //B��ʧ���¼�
    { 0x1802,       0x30040800,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //C��ʧ���¼�
    { 0x1803,       0x30040900,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
    //A������¼�
    { 0x1901,       0x30050700,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //B������¼�
    { 0x1902,       0x30050800,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //C������¼�
    { 0x1903,       0x30050900,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
	#endif

    //��բ�¼�
    { 0x1D00,       0x301F0200,		sizeof(Relay_Table)/sizeof(DWORD),				Relay_Table             },
    //��բ�¼�
    { 0x1E00,       0x30200200,		sizeof(Relay_Table)/sizeof(DWORD),				Relay_Table              },

};								

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
extern BOOL RecordJudgePhase( WORD OI, BYTE ClassAttribute, TDLT698RecordPara *DLT698RecordPara);


//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������: ���Event645Obj �У� DI3 = 03��ͷ���¼�����
//
//����: ��
//
//����ֵ: ���Event645Obj �У� DI3 = 03��ͷ���¼����� 
//
//��ע: DI3 = 03��ͷ���¼���DI2DI1���Event645Obj�е�ID21_32
//      ������DI3DI2��䣬���ID21_32���ܻ����ظ�����Ҫ������������
//--------------------------------------------------
static BYTE SearchEvent645DI3Is03Num(void)
{
    BYTE i, Ret = 0;

    for (i = 0; i < (sizeof(Event645Obj) / sizeof(Event645Obj[0])); i++)
    {
        if(Event645Obj[i].OAD.d == 0xFFFFFFFF)
        {
            break;
        }
        Ret++;
    }

    return Ret;
}

//--------------------------------------------------
//��������:  ���ݱ������¼����ݱ�ʶ         
//��     ��:	BYTE Mode[in] 0x00:0x03���ݱ�ʶ���¼�       0x55:0x10��ͷ�����ݱ�ʶ�¼�
//			WORD ID[in]	���ݱ�ʶ         
//����   ֵ  :�ɹ�������������ʧ�ܣ�����0x80             
//��ע����:  BYTE
//--------------------------------------------------
BYTE Search645EventID( BYTE Mode, WORD DataId )
{
	BYTE i,Num,Index;//����������ܳ���255

	if( Mode == 0x00 )
	{
		Num = EVENT645_03NUM;
		if( Num >= 0x80 )		//������ѭ��
		{
			return 0x80;
		}

		for(i=0; i<Num; i++)
		{
			if( DataId == Event645Obj[i].ID21_32 )
			{
		        return i;
			}
		}
	}
	else
	{
		Num = (sizeof(Event645Obj)/sizeof(Event645Obj[0]));
		if( (Num >= 0x80) ||(Num < EVENT645_03NUM) )//������ѭ��
		{
			return 0x80;
		}
		
		for (i=EVENT645_03NUM; i<Num; i++)
		{
			if( DataId == Event645Obj[i].ID21_32 )
			{
		        return i;
			}
		}
	}

	return 0x80;
}

//--------------------------------------------------
//��������:     ��ȡ�����������Ա�     
//��     ��:
//          Sch[in]	    ���ƫ��   
//          Index[in]   ��ϵ��������ƫ��
//          pBuf[out]   ��������
//����ֵ    
//��ע����:     WORD  ���ݳ��� 0x8000�������ݳ���
//--------------------------------------------------
WORD GetRelationTable( BYTE Sch, BYTE Index, BYTE* pBuf )
{
    BYTE Num;
    WORD Len;
    
    Num = (sizeof(Event645Obj)/sizeof(Event645Obj[0]));
    if( Sch > Num )
    {
        return 0x8000;
    }

    if( Index == 0xff )
    {
        Len = sizeof(DWORD)*Event645Obj[Sch].TableNum;
        if( Len > ((MAX_EVENT_OAD_NUM+40)*4+50))//���շ���
        {
			return 0x8000;
        }
        
        memcpy( pBuf, (BYTE*)Event645Obj[Sch].pRelationTable, Len );//��俪ʼ�����������Ա�
    }
    else
    {
        Len = 4;
        
        //Index�ж� 
        if( (Index > Event645Obj[Sch].TableNum) ||( Index == 0) )
        {
            return 0x8000;
        }
        
        memcpy( pBuf, (BYTE*)&Event645Obj[Sch].pRelationTable[Index-1], sizeof(DWORD) );

    }
   
    return (Len/4);
}

//--------------------------------------------------
//��������:     ��ȡ645�¼��ӿں���     
//��     ��:
//          pDI[in]	    ���ݱ�ʶ   
//          OutBuf[in]  ����Buf
//����ֵ    
//��ע����:     WORD  ���ݳ��� 0x8000�������ݳ���
//--------------------------------------------------
WORD api_ReadEventRecord645( BYTE* pDI , BYTE* OutBuf )
{
    TTwoByteUnion DI21_32,OI;
    BYTE Sch,ClassAttribute,Type;
    TDLT698RecordPara DLT698RecordPara;
	BYTE Buf[(MAX_EVENT_OAD_NUM+40)*4+50];
	WORD Result,Len;
	
    Len = 0;
    
    if( pDI[3] == 0X03 )
    {
    	Type = 0x00;
        DI21_32.w = ((WORD)(pDI[2]<<8)+pDI[1]);
        #if(SEL_PRG_RECORD645 == YES)
        //645��̼�¼û�в���698������Դ�������ڴ˵�������
        if( (pDI[2] == 0x30)&&(pDI[1] == 0x00) )
        {
            Len = api_ReadProgramRecord645( pDI, OutBuf );
            return Len; 
        }
        #endif
    }
    else
    {
    	Type = 0x55;
        DI21_32.w = ((WORD)(pDI[3]<<8)+pDI[2]);
    }

    Sch = Search645EventID( Type, DI21_32.w );//���б�����
    if( Sch == 0x80 )//δ�ҵ�����
    {
        return 0x8000;
    }
    
	OI.w = Event645Obj[Sch].OAD.w[1];
	ClassAttribute = Event645Obj[Sch].OAD.b[1];     //��ȡ��������
    Result = RecordJudgePhase( OI.w, ClassAttribute, &DLT698RecordPara);
    if( Result == FALSE )
    {
        return 0x8000;
    }

    if( pDI[3] == 0x03 )
    {
        if( pDI[0] == 0x00 )//�¼��ܴ���
        {
           Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 1, OutBuf );
        }
        else//��ȡ��¼
        {
            if( pDI[0] == 0xff )
            {
                return 0x8000;
            }
            
            Result = GetRelationTable( Sch, 0xff, Buf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }
            
            DLT698RecordPara.OI = OI.w;                       //OI����
            DLT698RecordPara.Ch = 0;                        //�����ϱ� �̶�ͨ������
            DLT698RecordPara.pOAD = Buf;                    //pOADָ��
            DLT698RecordPara.OADNum = Result;               //OAD����
            DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
            DLT698RecordPara.TimeOrLast = pDI[0];           //����ʱ����
            
            Len = api_ReadEvent645RecordByNo((TDLT698RecordPara *)&DLT698RecordPara, MAX_APDU_SIZE, OutBuf );

        }
    }
    else//0x1x��ͷ���¼�
    {
        if( DI21_32.w == 0x1000 )//ʧѹ�¼�
        {
            DLT698RecordPara.Phase = ePHASE_ALL;
            
            if( pDI[1] == 0x00 )//ʧѹ�ܴ�����ʧѹ�ۼ�ʱ��
            {
                if( (pDI[0] == 0x01) ||(pDI[0] == 0x02))
                {
                    Len = api_ReadCurrentRecordTable645(OI.w, DLT698RecordPara.Phase, pDI[0], OutBuf );
                }
                else
                {
                    return 0x8000;
                }
            }
            else if( (pDI[1] == 0x01) || (pDI[1] == 0x02) )//���1��ʧѹ����ʱ�̡�����ʱ�� 
            {
                if( pDI[0] == 0x01 )
                {
                    Len = api_ReadCurrentRecordTable645(OI.w, DLT698RecordPara.Phase, pDI[1]+2, OutBuf );
                }
            }
            else
            {
                return 0x8000;
            }
        }
        else if( pDI[1] == 0x00 )//�¼��ܴ������ۼ�ʱ��
        {
            if( pDI[0] == 1 )//��ȡ�¼��ܴ���
            {
                Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 1, OutBuf );
            }
            else if( pDI[0] == 2 )//��ȡ�ۼ�ʱ��
            {
                Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 2, OutBuf );
            }
            else
            {
                Len = 0x8000;
            } 
        }
        else//��ȡ��¼
        {
            if( pDI[0] == 0xff )
            {
                return 0x8000;
            }
            
            Result = GetRelationTable( Sch, pDI[1], Buf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }
            
            DLT698RecordPara.OI = OI.w;                       //OI����
            DLT698RecordPara.Ch = 0;                        //�����ϱ� �̶�ͨ������
            DLT698RecordPara.pOAD = Buf;                    //pOADָ��
            DLT698RecordPara.OADNum = Result;               //OAD����
            DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //ѡ�����
            DLT698RecordPara.TimeOrLast = pDI[0];           //����ʱ����

            Len = api_ReadEvent645RecordByNo((TDLT698RecordPara *)&DLT698RecordPara, MAX_APDU_SIZE, OutBuf );
            
        }
    }

    return Len;
} 

#endif//#if( SEL_DLT645_2007 == YES )


