//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.5
//����		�û���ȡ���İ汾��Ϣ
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//�±�[0--MAX_VERSION-1]����Ϊ ��������1 ��������2
//��������3  FLASHУ��  ����汾��
//��������  ��������   ��������
WORD Version[MAX_VERSION];


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
#if( MAX_PHASE == 3 )
typedef struct tConstAdder_t
{
	DWORD OAD;
	DWORD Adder;
} tConstAdder;

tConstAdder ConstAdder[] =    //��������ַ����58������
{
	{ 0x40070201, (DWORD)&LCDParaDef.PowerOnDispTimer },               //�ϵ�ȫ��ʱ��
	{ 0x40070202, (DWORD)&LCDParaDef.BackLightPressKeyTimer },         //����ʱ�������ʱ��
	{ 0x40070203, (DWORD)&LCDParaDef.BackLightViewTimer },             //��ʾ�鿴�������ʱ��
	{ 0x40070204, (DWORD)&LCDParaDef.LcdSwitchTime },                  //�޵簴����Ļפ�����ʱ��
	{ 0x40070205, (DWORD)&LCDParaDef.EnergyFloat },                    //����С����
	{ 0x40070206, (DWORD)&LCDParaDef.DemandFloat },                    //���ʣ�������С����
	{ 0xF3010300, (DWORD)&LCDParaDef.KeyDisplayTime },                 //�����л���ѭ����ʾ��ʱ��
	{ 0xF3000300, (DWORD)&LCDParaDef.LoopDisplayTime },                //ѭ����ʾʱÿ������ʾʱ��
	{ 0x41120200, (DWORD)&EnereyDemandModeConst.byActiveCalMode },     //�й���Ϸ�ʽ������
	{ 0x41130200, (DWORD)&EnereyDemandModeConst.PReactiveMode },       //�޹����1ģʽ������
	{ 0x41140200, (DWORD)&EnereyDemandModeConst.NReactiveMode },       //�޹����2ģʽ������
	{ 0x41000200, (DWORD)&EnereyDemandModeConst.DemandPeriod },        //��������
	{ 0x41010200, (DWORD)&EnereyDemandModeConst.DemandSlip },          //��������ʱ��
	{ 0x41160800, (DWORD)&MonBillParaConst },                          //������
	{ 0x80010200, (DWORD)&RelayKeepPowerFlag },                        //�����־
	{ 0x41070200, (DWORD)&PPrecisionConst },                           //�й����ȵȼ�
	{ 0x410B0200, (DWORD)&MeterModelConst },                           //����ͺ�
	{ 0x43000202, (DWORD)&SoftWareVersionConst },                      //��������汾��
	{ 0x43000203, (DWORD)&SoftWareDateConst },                         //��������汾����
	{ 0x43000204, (DWORD)&HardWareVersionConst },                      //����Ӳ���汾��
	{ 0x43000205, (DWORD)&HardWareDateConst },                         //����Ӳ���汾����
	{ 0x43000201, (DWORD)&FactoryCodeConst },                          //���ұ��
	{ 0x41110200, (DWORD)&SoftRecordConst },                           //���������
	{ 0xF3000400, (DWORD)&LCDParaDef.DispItemNum[0] },                    //ѭ������
	{ 0xF3000200, (DWORD)&LoopItemDef },                               //ѭ����ĿĬ������
	{ 0xF3010400, (DWORD)&LCDParaDef.DispItemNum[1]},              //��������
	{ 0xF3010200, (DWORD)&KeyItemDef },                                //������ĿĬ������
	{ 0x40080200, (DWORD)&SwitchTimeParaConst.dwSwitchTime[0] },       //����ʱ�����л�ʱ��
	{ 0x40140200, (DWORD)&TimeAreaTableConst1 },                       //��ǰ��ʱ����
	{ 0x40150200, (DWORD)&TimeAreaTableConst2 },                       //������ʱ����
	{ 0x400C0201, (DWORD)&TimeZoneSegParaConst.TimeZoneNum },          //��ʱ����
	{ 0x400C0202, (DWORD)&TimeZoneSegParaConst.TimeSegTableNum },      //��ʱ�α���
	{ 0x400C0203, (DWORD)&TimeZoneSegParaConst.TimeSegNum },           //��ʱ����
	{ 0x400C0204, (DWORD)&TimeZoneSegParaConst.Ratio },                //������
	{ 0x40160300, (DWORD)&TimeSegTableFlag },                          //ʱ�α�����ѡ��
	{ 0x40090200, (DWORD)&SwitchTimeParaConst.dwSwitchTime[1] },       //������ʱ���л�ʱ��
	{ 0x40160201, (DWORD)&TimeSegTableConst1 },                        //ʱ�α�1
	{ 0x40160202, (DWORD)&TimeSegTableConst2 },                        //ʱ�α�2
	{ 0x40160203, (DWORD)&TimeSegTableConst3 },                        //ʱ�α�3
	{ 0x40160204, (DWORD)&TimeSegTableConst4 },                        //ʱ�α�4
	{ 0x50000300, (DWORD)&InstantFreeze },                             //˲ʱ��������������Ա�
	{ 0x50020300, (DWORD)&MinFreeze },                                 //���Ӷ�������������Ա�
	{ 0x50030300, (DWORD)&HourFreeze },                                //Сʱ��������������Ա�
	{ 0x50040300, (DWORD)&DayFreeze },                                 //�ն�������������Ա�
	{ 0x50050300, (DWORD)&ClosingFreeze },                             //�����ն�������������Ա�
	{ 0x50060300, (DWORD)&MonFreeze },                                 //�¶�������������Ա�
	{ 0x50080300, (DWORD)&TZChgFreeze },                               //ʱ�����л���������������Ա�
	{ 0x50090300, (DWORD)&DTTChgFreeze },                              //ʱ�α��л���������������Ա�
	{ 0x43000700, (DWORD)&MeterReportFlagConst },                      //�����豸������7��������ϱ�
	{ 0x43000800, (DWORD)&MeterActiveReportFlagConst },                //�����豸������8���������ϱ�
	{ 0x20150500, (DWORD)&MeterReportStatusFlagConst },                //Ĭ�ϸ����ϱ�״̬���ϱ���ʽ
	{ 0x43000A00, (DWORD)&FollowReportChannelConst },                  //�����ϱ�ͨ��
	{ 0x33200400, (DWORD)&ReportActiveMethodConst },                   //Ĭ���¼��ϱ���ʽΪ�����ϱ����б�
	{ 0x20150400, (DWORD)&FollowReportModeConst },                     //�����ϱ�ģʽ��
	{ 0x33200300, (DWORD)&ReportModeConst },                           //���ϱ��¼����ϱ�ģʽ
	{ 0xF2010201, (DWORD)&CommParaConst.I485 },                        //��һ·485������
	{ 0xF2090201, (DWORD)&CommParaConst.ComModule },                   //ģ�鲨����
	{ 0xF2010202, (DWORD)&CommParaConst.II485 }                        //�ڶ�·485������
};
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: �汾����ģ���ϵ��ʼ������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע: ֻ�Ǹ���ʾ�ã��Ƿ���ҪŲ����ʾ������ߴ�����Ϣ����һ�£�!!!!!!  
//-----------------------------------------------
void api_PowerUpVersion( void )
{
	//��ʼ���汾��
	//�����ͼ�����

	if( MeterTypesConst == METER_3P4W )
	{
		if( MeterVoltageConst == METER_220V )
		{
			Version[0] = 0xC220;
		}
		else
		{
			Version[0] = 0xC057;
		}
	}
    else if( MeterTypesConst == METER_3P3W )
    {
		if( MeterVoltageConst == METER_380V )
		{
			Version[0] = 0xC380;
		}
		else
		{
			Version[0] = 0xC100;
		}
    }
	else//METER_ZT
	{
		Version[0] = 0xC080;
	}

	//����
	Version[1] = MeterCurrentTypesConst;//��������
	//
	Version[2] = (WORD)RelayTypeConst;
	
	Version[3] = api_CheckCpuFlashSum(0xff);

	//����汾
	Version[4] = (WORD)SoftVersionConst;

	Version[5] = api_CheckCpuFlashSum(0);//������У����

	Version[6] = 0xDF00; //VERSION7; ��λ����DF����λ��ʾ���ڱ�����ַ1����һ���ֽڡ�
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(FactoryRcvArea[4]), 1, (BYTE *)&Version[6]);

	Version[7] = (wStandardVoltageConst/10);//(WORD)VERSION8;

	Version[8] = (MeterCurrentTypesConst<<8)|(RelayTypeConst);//VERSION9;

	Version[9] = 0;//VERSION10 Bit1:ѡ�������ռ�,��λ��ϢҪ����ά�����Ҫ��;

	Version[10] = 0;//VERSION11;

	Version[11] = 0;//(WORD)VERSION12;// ��ʾ�ⱨ��ԭ��  ;

	Version[12] = 0;//VERSION13;// ��ʾ�ⱨ��ԭ��  ;

	Version[13] = 0;//VERSION14;

	//�����������ڱ�ʶģ����
	Version[14] = 0;

	Version[15] = 0;
}


//-----------------------------------------------
//��������: ��ȡ��Ӧ���Ͱ汾
//
//����: 	Type[in]	��Ӧ����
//                    
//����ֵ:  	�汾��Ϣ
//
//��ע: 
//-----------------------------------------------
WORD api_GetMeterVersion(BYTE Type)
{
	if( Type >= MAX_VERSION )
	{
		return 0;
	}
	
	return Version[Type];
}



//-----------------------------------------------
//��������: �����汾
//
//����:		Buf[out] �������
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
WORD api_ReadMeterVersion(BYTE *Buf)
{
	WORD i;
	WORD tw;
	
	i = 0;
	//��ʼ��
	Buf[i++] = 0xaa;
	//����˵������ Type+Len�ṹ��Type+LenΪ1�ֽڣ�Typeռ��5λ��Lenռ��3λ
	//00001 001    �����ͣ�1�ֽڣ�
	Buf[i++] = 0x09;
	Buf[i++] = MeterTypesConst;
	//00010 001    ��ѹ��1�ֽڣ�
	Buf[i++] = 0x11;

	Buf[i++] = wStandardVoltageConst/10;

	//00011 001    ������1�ֽڣ�
	Buf[i++] = 0x19;
	Buf[i++] = MeterCurrentTypesConst;

	//00101 010    �汾��2�ֽڣ�
	Buf[i++] = 0x2A;
	Buf[i++] = (BYTE)SoftVersionConst;
	Buf[i++] = (WORD)SoftVersionConst>>8;
	//00110 010    �û���2�ֽڣ�
	Buf[i++] = 0x32;
	Buf[i++] = (BYTE)SpecialClientsConst;
	Buf[i++] = (WORD)SpecialClientsConst>>8;
	//00111 010    ȫ��FLASHЧ���루2�ֽڣ�
	Buf[i++] = 0x3a;
	tw = api_CheckCpuFlashSum(0xff);
	lib_ExchangeData(Buf+i,(BYTE*)&tw,2);
	i += 2;
	//01000 010    ��ţ�2�ֽڣ�
	Buf[i++] = 0x42;	
	tw = SelThreeBoard;
	memcpy(Buf+i,(BYTE*)&tw,2);
	i += 2;
	//01001 001    ����CPU��1�ֽڣ�
	Buf[i++] = 0x49;	
	Buf[i++] = CPU_TYPE;
	//01010 001    ����оƬ��1�ֽڣ�
	Buf[i++] = 0x51;	
	Buf[i++] = SAMPLE_CHIP;
	//01011 100    �洢оƬ��4�ֽڣ�
	Buf[i++] = 0x5c;	
	Buf[i++] = MASTER_MEMORY_CHIP;
	Buf[i++] = SLAVE_MEM_CHIP;
	Buf[i++] = THIRD_MEM_CHIP;
	Buf[i++] = CHIP_NO;
	//01110 001    Һ��������1�ֽڣ�
	Buf[i++] = 0x71;	
	Buf[i++] = LCD_DRIVE_CHIP;
	//01111 001    ESAM��1�ֽڣ�
	Buf[i++] = 0x79;	
	Buf[i++] = PREPAY_MODE;
	//10000 001    cpu����1�ֽڣ�
	Buf[i++] = 0x81;	
	Buf[i++] = CARD_COM_TYPE;
	//10001 001    �̵�����1�ֽڣ�
	Buf[i++] = 0x89;
	Buf[i++] = RelayTypeConst;
	
	//10010 100    ������ܣ�4�ֽڣ�
	Buf[i++] = 0x94;	
	memset(Buf+i,0x00,4);
	if( SEL_DEBUG_VERSION == YES )
	{
		Buf[i] |= 0x01;
	}
	if( SEL_DLT645_2007 == YES )
	{
		Buf[i] |= 0x02;
	}
	if( SEL_DLT698_2016_FUNC == YES )
	{
		Buf[i] |= 0x04;
	}
	//if( SelCorrectVrefgain == YES ) ��ɾ�������ֶ���������
	//{
	//	Buf[i] |= 0x10;
	//}
	if( SelDivAdjust == YES )
	{
		Buf[i] |= 0x20;
	}
	if( SelMChipVrefgain == YES )
	{
		Buf[i] |= 0x40;
	}
	
	if(SelZeroCurrentConst == YES )
	{
		Buf[i+1] |= 0x01;
	}
	if( SelSecPowerConst == YES )
	{
		Buf[i+1] |= 0x02;
	}
/*	if( SelSwitchPowerSupplyConst == YES )
	{
		Buf[i+1] |= 0x04;
	}*/	
	#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )
	Buf[i+1] |= 0x08;
	#endif
	
	#if( SEL_24LC256_COMMUNICATION_MOD == YES )
	Buf[i+1] |= 0x10;
	#endif
	
	i = i+4;
	
	//10011 010    ����FLASHЧ���루2�ֽڣ�
	Buf[i++] = 0x9a;
	tw = api_CheckCpuFlashSum(0x00);
	lib_ExchangeData(Buf+i,(BYTE*)&tw,2);
	i += 2;
	
	//������
	Buf[i++] = 0xaa;
	
	return i;
}
//-----------------------------------------------
//��������:	��ȡ������������Ӧ�ĵ�ַ
//
//����:	pBuf[in/out]
//
//����ֵ:	
//
//��ע:	������������
//-----------------------------------------------
#if( MAX_PHASE == 3 )
WORD api_GetDefaultConstParaAdder(WORD Num, BYTE* pBuf )
{
	WORD i,Offset;
	DWORD td; 

	Offset = 0;
	
	for(i = Num; i < (Num+20); i++)
	{
		if( i > (sizeof(ConstAdder) / sizeof(ConstAdder[0]) - 1) )
		{
			td = 0xA5A5A5A5;
			lib_ExchangeData(pBuf + Offset, (BYTE *)&td, 4);
			Offset += 4;
			td = 0x5A5A5A5A;
			lib_ExchangeData(pBuf + Offset, (BYTE *)&td, 4);
			Offset += 4;
			break;
		}

		td = (DWORD)ConstAdder[i].OAD;
		lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
		Offset += 4;

		td = (DWORD)ConstAdder[i].Adder;
		lib_ExchangeData( pBuf + Offset, (BYTE *)&td, 4 );
		Offset += 4;
	}

	return Offset;
}
#endif


