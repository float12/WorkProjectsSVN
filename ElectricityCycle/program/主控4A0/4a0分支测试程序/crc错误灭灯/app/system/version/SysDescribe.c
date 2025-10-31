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


