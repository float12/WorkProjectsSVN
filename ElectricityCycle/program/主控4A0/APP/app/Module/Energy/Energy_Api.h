//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.8.5
//����		����ģ��Apiͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __ENERGY_API_H
#define __ENERGY_API_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

typedef enum
{
	SwitchNormalEnergy = 0,	//������ת��Ϊ��ͨ����
	SwitchHighEnergy,	 	//������ת��Ϊ�߾��ȵ���
}eSwitchEnergyType;


//���ܽṹ
typedef struct TEnergyDBase_t
{
	//�����й�
	DWORD PActive;

	//�����й�
	DWORD NActive;
	
	#if(SEL_RACTIVE_ENERGY == YES)
	//�������޹�
	DWORD RActive[4];
	#endif

	#if( SEL_APPARENT_ENERGY == YES )
	// ���ڣ����򡢷���
	DWORD Apparent[2];
	#endif

	//У��
	DWORD CRC32;
}TEnergyDBase;

typedef struct TEnergyRam_t
{
	TEnergyDBase Energy[1+NUM_OF_SEPARATE_ENERGY];							// ��,A,B,C
}TEnergyRam;

// EEPROM�е���
// ��ǰ����ڰ�ȫ�ռ�
// ��1~12����������ռ�
typedef struct TEnergyRom_t
{
	TEnergyDBase Energy[1+NUM_OF_SEPARATE_ENERGY];							// ��,A,B,C

	TEnergyDBase RatioEnergy[MAX_RATIO][1+NUM_OF_SEPARATE_ENERGY_RATIO];	// ��,A,B,C�ķ��ʵ���  MAX_RATIOΪ5
}TEnergyRom;

// ��������
typedef struct TEnergyRomRemain_t
{
	TEnergyDBase RemainEnergy[1+NUM_OF_SEPARATE_ENERGY];						// ��,A,B,C
}TEnergyRomRemain;

//�ۼƵ��ܴ洢�ṹ
typedef struct TDEnergyAccuBase_t
{
	DWORD PActive;	//�����й�

	DWORD NActive;	//�����й�
	
	DWORD CRC32;	//У��
}TDEnergyAccuBase;

// ��ȫ�ռ���ܽṹ����
typedef struct TEnergySafe_t
{
	TEnergyRom 			Energy;	// ���ܣ������������

	TEnergyRomRemain	Remain;	// β���������������

	TDEnergyAccuBase	EnergyAccMon[2];	// �£�����0.01�ȵ���
}TEnergySafeRom;

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
// 				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �����
//
//����: 
//	��
//                    
//����ֵ:
//  ��
//
//��ע:   
//-----------------------------------------------
void api_ClrEnergy( void );

//-----------------------------------------------
//��������: д�����
//
//����:
//	Type[in]:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	Value[in]:	�������
//
//����ֵ:	��
//
//��ע:   ��������
//-----------------------------------------------
void api_WritePulseEnergy( WORD Type, BYTE Value );

//-----------------------------------------------
//��������: ��ȡ��ǰ��������
//
//����:
//	Type[in]:
//			D15--	�߾��ȵ��� 				8000
//			D14-D12��PHASE_ALL -- �ܵ��� 	0000
//					PHASE_A -- A�����  	1000
//					PHASE_B -- B�����  	2000
//					PHASE_C -- C�����  	3000
// 					PHASE_N -- N�ߵ���		4000
//			D11-D8����ʱ�ò���
//					�������	--		0000
//					��������	--		0100
//					г������	--		0200
//			D7-D0��	COMB_ACTIVE		0000
//					P_ACTIVE		0001
//					N_ACTIVE		0002
//					P_RACTIVE		0003
//					N_RACTIVE		0004
//					RACTIVE1		0005
//					RACTIVE2		0006
//					RACTIVE3		0007
//					RACTIVE4		0008
//					APPARENT_P		0009
//					APPARENT_N		000a
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//	Ratio[in]:
//			����
//			1~MAX_RATIO		 ������
//			0				 �������ܼ�
//
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С����
//
//	Energy[out]:ָ����ܵ�ָ��
//			Hex�����ʾ�����λΪ����λ
//			�߾��ȵ��ܷ��� 8�ֽڣ��Ǹ߾��ȵ��ܷ���4�ֽ�
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_GetCurrEnergyData(WORD Type, BYTE DataType, WORD Ratio, BYTE Dot, BYTE * Energy);

//-----------------------------------------------
//��������: ��ȡ�ۼ��õ������½��㣩
//
//����:  
//	No[in]:
//			0 ��ǰ��
//			1 ����
//
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//
//	Dot[in]: С����λ��
//			0: --	��С����
//			1~N: --	1~N��С����
//
//	pEnergy[in]:
//			ָ����ܵ�ָ��
//			Hex�����ʾ�����λΪ����λ %0x80000000
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	
//-----------------------------------------------
BOOL api_GetAccuEnergyData( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy );
#if(PREPAY_MODE == PREPAY_LOCAL)
//-----------------------------------------------
//��������: ���ݽ�����ܣ����ڱ�����ݽ���ʱ���ܵ��룬���ڼ�������õ���
//
//����:  	��
//	
//����ֵ:	��
//
//��ע:�����������	
//-----------------------------------------------
void api_LadderClosingEnergy( void );
//-----------------------------------------------
//��������: ��ȡ�������õ��������ݿ۷��ã����ݽ���ʱ���㣩
//
//����:  
//	No[in]:
//			0 ��ǰ��
//			1 ����
//
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
// 
//
//	pEnergy[OUT]:
//			ָ����ܵ�ָ��
// 			Hex�����ʾ�����λΪ����λ %0x80000000
//
//����ֵ:	TRUE   ��ȷ
//			FALSE  ����
//
//��ע:	
//-----------------------------------------------
BOOL api_GetLadderAccuEnergyData(  BYTE DataType, BYTE *pEnergy );
#endif
//-----------------------------------------------
//��������: ���ۼ��õ���BCD-645��Լ���������ڣ�
//
//����:
//	No[in]:
//			0 ��ǰ����
//			1 ��һ����
//
//	DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
//
//	Dot[in]: С����λ�� ���֧��2λС��
//			0: --	��С����
//			1~N: --	1~N��С����
//
//	pEnergy[in]:
//			ָ����ܵ�ָ��
// 			����hex
//
//����ֵ:	TRUE   ��ȷ
//		  FALSE  ����
//
//��ע:
//-----------------------------------------------
BOOL api_GetAccuPeriEnergy( BYTE No, BYTE DataType, BYTE Dot, BYTE *pEnergy );

//-----------------------------------------------
//��������: ������ܴ��� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_PowerDownEnergy( void );

//-----------------------------------------------
//��������: �ϵ���ܴ��� 
//
//����:  
//			��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_PowerUpEnergy( void );

//-----------------------------------------------
//��������: ���ܴ������� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:	��ѭ������
//-----------------------------------------------
void api_EnergyTask(void);

//-----------------------------------------------
//��������: ����ת�� 
//
//����:  ��
//
//����ֵ:	��
//
//��ע:	
//-----------------------------------------------
void api_SwapEnergy(void);

//-----------------------------------------------
//��������: �½�����ܣ��������ۼƵ��ܼ���
//
//����:  	��
//	
//����ֵ:	��
//
//��ע:	
//-----------------------------------------------
void api_ClosingMonEnergy( void );

DWORD api_GetEnergyConst( void );
//-----------------------------------------------
//��������: ����ת��Ϊ������
//
//����:
//
//		Num[in]: ���ʸ���+1
//
//		BufLen[in]: ����buf����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
WORD api_EnergyToEnergyDBase( BYTE Num, BYTE BufLen, BYTE* Buf );

//-----------------------------------------------
//��������: ����β��ת��Ϊ����
//
//����:
//		SwitchEnergyType[in]: ת���߾��ȵ���   	ת����ͨ����
//
//		Num[in]: ���ʸ���+1
//
//		BufLen[in]: ����bufg����
//
//		Buf[in]:ָ����ܵ�ָ�� ����hex
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
WORD api_EnergyDBaseToEnergy( eSwitchEnergyType SwitchEnergyType, BYTE Num, BYTE BufLen, BYTE* Buf );

//-----------------------------------------------
//��������:	��ȡ���ܵ�ַ
//
//����:	pBuf[in/out]
//
//����ֵ:	4�ֽڵ���������ַ+4�ֽڵ��ܱ��ݵ�ַ+4�ֽ�EE�洢��ַ
//
//��ע:	������������
//-----------------------------------------------
BYTE api_GetFactoryEnergyPara(BYTE *pBuf);

#endif//__ENERGY_API_H
