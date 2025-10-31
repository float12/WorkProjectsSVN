//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		֧��698.45��Լ�ĵ����ӡ�ư������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Single.h"

#if ( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303)\
   || (BOARD_TYPE == BOARD_HT_SINGLE_78202401) || (BOARD_TYPE == BOARD_HT_SINGLE_20250819))
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//GPIO
#define GPIO_NUM				80			//GPIOA..GPIOE(16 * 5 = 80)
#define GPIOA_POSITION			0			//GPIOA���λ��
#define GPIOB_POSITION			16			//GPIOB���λ��
#define GPIOC_POSITION			32			//GPIOC���λ��
#define GPIOD_POSITION			48			//GPIOD���λ��
#define GPIOE_POSITION			64			//GPIOE���λ��



#define SWITCH_VOL_5_MIN			1.1			//3.3V��Դ (����Դ5V����ѹ1/4)
#define MAX_COUNT					150
#define CURR_7P5MV_SAMPLE			15900		//�ٷ�֮ʮ������²���ֵ(7.5mV)
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct
{
	long yn1;
	long yn2;
	long yn3;
}
 lpf_vars_t;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum);
static BOOL SciRcvEnable(BYTE SciPhNum);
static BOOL SciRcvDisable(BYTE SciPhNum);
static BOOL SciSendEnable(BYTE SciPhNum);
static BOOL SciBeginSend(BYTE SciPhNum);
static void InitExtInt(void);

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE IsExitLowPower;//ģ���ڱ���

// ����ӳ��ͼ���������Ӳ�һ����
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = 
{
	// ��һ·RS-485
	eRS485_I,
	SCI_MCU_USART2_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable,			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���Ϳ�ʼ
	// 415ͨ��
	ePT_UART1_F415,
	SCI_MCU_USART1_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable,			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,          // ���Ϳ�ʼ
	// �ز�
	eCR,
	SCI_MCU_USART0_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable, 			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,          // ���Ϳ�ʼ
	// ����
	eBlueTooth,
	SCI_MCU_USART3_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable, 			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,          // ���Ϳ�ʼ
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�ϵ���������
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerOn[GPIO_NUM] = 
{
  //GPIOA��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA0	3.3V        �ز�����/������Դ����--�͵�ƽ���ز���������Դ�����������������������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA1	SEG1		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA2	PLCEVE		��ͨ�˿� ��� ���� ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST		��ͨ�˿� ��� ���� ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA4	SEG2		��һ���� ���� ����
  {eGPIO_TYPE_COMMON,	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW },	//PA5	FLASH_PWR_EN		��ͨ�˿� ��� ���� ��(�͵�ƽ��Ч) ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	JTAG		��ͨ�˿� ���� ����  
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		��ͨ�˿� ��� ���� �� �͵�ƽ������
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	SEG3        �ڶ����� ���� ����

  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA9	PF-����оƬ�޹���������  ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	QF-����оƬ�й���������  ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DISPKEY--��ҳ����		��ͨ�˿� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	SEG15--------��һ���� ���� ���� �� 
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	SEG16--------��һ���� ���� ���� ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB0	SEG4		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB1	SEG5		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB2	SEG6		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB3	SEG7		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB4	SEG8		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB5	SEG9		��һ���� ���� ����
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB6	ESAM-CS		��ͨ�˿� ��� ��   SPI��������SPIʱ�ٴ���
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB7	ESAM-CLK	��ͨ�˿� ��� ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB8	ESAM-SO		��ͨ�˿� ��� ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB9	ESAM-SI		��ͨ�˿� ��� ��  
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP,       eGPIO_OUTPUT_LOW },	//PB10	VESAM		��ͨ�˿� ��� ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB11	SEG10  		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB12	SEG11		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB13	SEG12		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB14	SEG13		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB15	SEG14		��һ���� ���� ����
  
  //GPIOC��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	UTX1--415	��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	URX1--415	��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX0		��һ���� ���� ���� �ز�ģ�鴮������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC3	PLCTX0		��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC4	FLASH-SI	��һ���� ��� ����  
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC5	FLASH-SO	��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC6	FLASK-CLK	��һ���� ��� ����
  {eGPIO_TYPE_COMMON,  	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW },	//PC7	FLASH_CS	��һ���� ��� ����--�͵�ƽ��Ч
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC8	CV415		415��Դ����: ��ͨ�˿� ��� ���� ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC9	RELAYOFF	��ͨ�˿� ��� ���� ��--�̵�������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC10	RELAYON		��ͨ�˿� ��� ���� ��--�̵�������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC11	TX2--485	��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX2--485	��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD0	SEG17		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD1	SEG18		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD2	SEG19		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD3	SEG20		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD4	SEG21		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD5	SEG22		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	SEG23		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD7	SEG24		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD8	COM1		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	COM2		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD10	COM3		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD11	COM4		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD12	COM5		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD13	COM6		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD14	CSJL		����оƬƬѡ
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD15	RELAY_TEST	��ͨ�˿� ���� ����
  
  //GPIOE��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PE0	CVJL		����оƬ�������: ��ͨ�˿� ��� ���� ��
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PE1	SEC			������/�๦�����	�ڶ����� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE2	PULSE_LED	��ͨ�˿� ��� ���� �� �͵�ƽ�������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE3	LEDRELAY	��ͨ�˿� ��� ���� �ߣ��͵�ƽ��Ч
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX3			�������գ���һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX3			�������ͣ��ڶ����� ��� ����
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},    //PE6	ʱ�ӵ�ص�ѹ �ڶ����� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	��ͨ�˿� ��� ���� �� ����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				�޴�����
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�͹�����������
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerDown[GPIO_NUM] = 
{
	//���ھ���Ӱ��, LCD����������Ϊ���ܶ˿�(��һ����)--PA2  PB0 - PB10  PB12 - PB15  PD0 - PD13
	
  //GPIOA��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PWR_EN		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA1	SEG1		��һ���� ��� ��©
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCEVE'		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST'		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA4	SEG2		��һ���� ��� ��©
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	ACTIVEP		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	CoverKey	��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA8	SEG3		��һ���� ��� ��©
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	PULSE_LED	��ͨ�˿� ��� ��© ��
  #if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	CoverKey	��ͨ�˿� ���� ����
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA10	ICKEY		��ͨ�˿� ��� ��© ��  
  #endif
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DispKey		��ͨ�˿� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA12	SEG15		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PA13	SEG16		��һ���� ��� ��©
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB0	SEG4		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB1	SEG5		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB2	SEG6		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB3	SEG7		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB4	SEG8		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB5	SEG9		��һ���� ��� ��©
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	ESAM-CS2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	ESAM-CLK2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	ESAM-SO2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	ESAM-SI2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON,    eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	VESAM		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB11	SEG10		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB12	SEG11		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB13	SEG12		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB14	SEG13		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PB15	SEG14		��һ���� ��� ��©
  
  //GPIOC��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	TXHW		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	RXHW		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	FLASH-SI	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	FLASH-SO	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	FLASK-CLK	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	FLASH-CS	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	LEDRELAY	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD0	SEG17		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD1	SEG18		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD2	SEG19		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD3	SEG20		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD4	SEG21		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD5	SEG22		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD6	SEG23		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD7	SEG24		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD8	COM1		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD9	COM2		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD10	COM3		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD11	COM4		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD12	COM5		��һ���� ��� ��©
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_NONE},	//PD13	COM6		��һ���� ��� ��©
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	RELAY_CTRL	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	RELAY_TEST	��ͨ�˿� ��� ��© ��
  
  //GPIOE��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	SEC			��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	ICIO		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	MCUCLK		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	METERIC_RX	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	METERIC_TX	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	ICRST		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE7	LVDIN		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				�޴�����
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																	�͹��Ļ�����������
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_WakeUp[GPIO_NUM] = 
{
  //GPIOA��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PWR_EN		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA1	SEG1 		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCEVE'		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCRST'		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA4	SEG2 		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	ACTIVEP		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	CoverKey	��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA7	LCDLED		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	SEG3 		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	PULSE_LED	��ͨ�˿� ��� ��© ��
  #if(BOARD_TYPE == BOARD_HT_SINGLE_78202401)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	CoverKey	��ͨ�˿� ���� ����
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA10	ICKEY		��ͨ�˿� ��� ��© ��  
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DispKey		��ͨ�˿� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	SEG15 		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	SEG16 		��һ���� ���� ����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB0	SEG4		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB1	SEG5		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB2	SEG6		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB3	SEG7		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB4	SEG8		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB5	SEG9		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	ESAM-CS2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	ESAM-CLK2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	ESAM-SO2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	ESAM-SI2	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	VESAM		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB11	SEG10		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB12	SEG11		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB13	SEG12		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB14	SEG13		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB15	SEG14		��һ���� ���� ����
  
  //GPIOC��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	TXHW		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	RXHW		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	FLASH-SI	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	FLASH-SO	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	FLASK-CLK	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	FLASH-CS	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	LEDRELAY	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	EE_SCL		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	EE_SDA		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD0	SEG17		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD1	SEG18		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD2	SEG19		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD3	SEG20		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD4	SEG21		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD5	SEG22		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	SEG23		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD7	SEG24		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD8	COM1		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	COM2		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD10	COM3		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD11	COM4		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD12	COM5		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD13	COM6		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	RELAY_CTRL	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	RELAY_TEST	��ͨ�˿� ��� ��© ��
  
  //GPIOE��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	SEC			��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	ICIO		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	MCUCLK		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	METERIC_RX	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	METERIC_TX	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	ICRST		��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	485_CTRL	��ͨ�˿� ��� ��© ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE9				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE10				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE11				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE12				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE13				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PE15				�޴�����
};

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//���ڲ��������ñ�        �ֱ��Ӧ300   /600   /1200  /2400  /4800 /7200  /9600  /19200 /38100 /57600 /115200
static const WORD TAB_Baud[]={0x8F5B,0x47AC,0x23D6,0x11EB,0x08F5,0x05F9,0x047A,0x023C,0x011E,0x00BE,0x005F};

static volatile WORD SysTickCounter;
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ϵͳ��Դ���
//
//����: 	
//          Type[in]:	
//				eOnInitDetectPowerMode		�ϵ�ϵͳ��Դ���
//				eOnRunDetectPowerMode		���������ڼ��ϵͳ��Դ���--ֻ���ڴ�ѭ���е���,����ʱ�н���͹��Ĳ���
//				eWakeupDetectPowerMode		�͹��Ļ����ڼ�ϵͳ��Դ���
//				eSleepDetectPowerMode		�͹��������ڼ�ϵͳ��Դ���
//				eOnRunSpeedDetectUnsetFlag	�������п���ϵͳ��Դ���--���ڴ�ѭ�������,����λ�κα�־,������Դ���
//����ֵ: 	TRUE:�е�   FALSE:û�е�
//
//��ע:   �ϵ�vcc�ͱȽ������У�����ֻ��LVDIN0�Ƚ�������Ϊvcc���г�������
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	BYTE i,Result;
	
	Result = TRUE;
	
	if( (HT_GPIOE->IOCFG&0x0080) == 0 )
	{
		Result = FALSE;
	}
	
	if( (HT_GPIOE->AFCFG&0x0080) != 0 )
	{
		Result = FALSE;
	}
	
	if( HT_PMU->PMUCON != 0x0007 )
	{
		Result = FALSE;
	}
	
	if( Result == FALSE )
	{		
		HT_GPIOE->IOCFG |= 0x0080;					//����
		HT_GPIOE->AFCFG &=~0x0080;					//����1
		EnWr_WPREG();
		HT_PMU->PMUCON  = 0x0007;					//����LVD_DET,BOR_DET,BOR��λģʽ
		DisWr_WPREG();
	}
	
	
	HT_PMU->VDETCFG = 0x004D;					//�ϵ�Ƚ�VCC��ѹ3.3V BOR 2.0����λ
	HT_PMU->VDETPCFG = 0x003A;		            //�Ƽ����ã�Ӱ��͹����ϵ��⣬�����������
	HT_GPIOE->PTOD  |= 0x0080;					//����©
	HT_GPIOE->PTUP  |= 0x0080;					//������
	HT_GPIOE->PTDIR &=~0x0080;					//����
		
	// //�͹����ڼ�Ҫ��PMU�ж�
	// if( Type == eSleepDetectPowerMode )
	// {
	// 	NVIC_DisableIRQ( PMU_IRQn );                                        	//��ֹPMU�ж�
	// 	HT_PMU->PMUIF	= 0x0000;                                               //���жϱ�־
	// 	HT_PMU->PMUIE	= 0x0004;                                               //ʹ��VCC,LVD����ж�
	// 	NVIC_ClearPendingIRQ( PMU_IRQn );                                       //�������״̬
	// 	NVIC_EnableIRQ( PMU_IRQn );                                             //ʹ��PMU�ж�
	// }
	// else
	// {
	// 	//��ֹPMU�ж�
	// 	NVIC_DisableIRQ( PMU_IRQn );
	// 	HT_PMU->PMUIF   = 0x0000;
	// 	HT_PMU->PMUIE   = 0x0000; 
	// }
	//��ֹPMU�ж�
	NVIC_DisableIRQ( PMU_IRQn );
	HT_PMU->PMUIF   = 0x0000;
	HT_PMU->PMUIE   = 0x0000; 
	
//	HT_PMU->PMUSTA  = 0x0000;					//��״̬��־ �üĴ���Ϊֻ��
	
	if( Type == eOnInitDetectPowerMode ) //�ϵ�ϵͳ��Դ���
	{
		for(i=0; i<50; i++)
		{
			api_Delayms( 1 );
			if( (HT_PMU->PMUSTA&0x05) != 0x05 )
			{
				break;
			}
		}
		if( i >= 50 )//ȷʵ�ϵ���
		{
			//���ϵ��־
			api_SetSysStatus( eSYS_STATUS_POWER_ON );
			return TRUE;
		}
		else
		{
			//���ϵ��־
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
	}
	else if( Type == eOnRunDetectPowerMode ) //���������ڼ��ϵͳ��Դ���,ֻ���ڴ�ѭ���е���,����ʱ�н���͹��Ĳ���
	{
		if( (HT_PMU->PMUSTA&0x05) == 0x05 )//��ֹ��ѭ������ �е�ֱ���˳�
		{
			//���ϵ��־
			api_SetSysStatus( eSYS_STATUS_POWER_ON );
			return TRUE;
		}
		else
		{
			for(i=0; i<10; i++)
			{
				api_Delayms( 1 );
				
				if( (HT_PMU->PMUSTA&0x05) == 0x05 )
				{
					break;
				}
			}
			if( i >= 10 )//����
			{
				//���ϵ��־
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
			else
			{
				//���ϵ��־
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
		}
	}
	else if( Type == eOnRunSpeedDetectUnsetFlag ) //�������п���ϵͳ��Դ���,���ڴ�ѭ�������,����λ�κα�־,������Դ���
	{
		if( (HT_PMU->PMUSTA&0x05) == 0x05 )//�е�ֱ���˳�
		{
			return TRUE;
		}
		else
		{
			for(i=0; i<10; i++)
			{
				api_Delay100us( 2 );
				
				if( (HT_PMU->PMUSTA&0x05) == 0x05 )
				{
					break;
				}
			}
			if( i >= 10 )//����
			{
				return FALSE;
			}
			else  //�е�
			{
				return TRUE;
			}
		}
	}
	else if( Type == eWakeupDetectPowerMode ) //�͹��Ļ����ڼ�ϵͳ��Դ���
	{
		if( (HT_PMU->PMUSTA&0x05) != 0x05 )//û�ϵ�
		{
			//���ϵ��־
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
		else
		{
			for(i=0; i<3; i++)
			{
				api_Delayms( 1 );
				if( (HT_PMU->PMUSTA&0x05) != 0x05 )
				{
					break;
				}
			}
			if( i >= 3 )//ȷʵ�ϵ���
			{
				//���ϵ��־
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
			else
			{
				//���ϵ��־
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
		}
	}
	else //�͹��������ڼ�ϵͳ��Դ���
	{
		if( (HT_PMU->PMUSTA&0x05) != 0x05 )//û�ϵ�
		{
			//���ϵ��־
			api_ClrSysStatus( eSYS_STATUS_POWER_ON );
			return FALSE;
		}
		else
		{
			for(i=0; i<3; i++)
			{
				//api_Delayms( 1 ); �͹����ڼ䲻����ô��ʱ
				api_DelayNop(5);
				if( (HT_PMU->PMUSTA&0x05) != 0x05 )
				{
					break;
				}
			}
			if( i >= 3 )//ȷʵ�ϵ���
			{
				//���ϵ��־
				api_SetSysStatus( eSYS_STATUS_POWER_ON );
				return TRUE;
			}
			else
			{
				//���ϵ��־
				api_ClrSysStatus( eSYS_STATUS_POWER_ON );
				return FALSE;
			}
		}
	}
}

//---------------------------------------------------------------
//��������: GPIO���ʼ����������ʼ��ÿ��GPIO����
//
//����:		HT_GPIOx��
//					HT_GPIOA..HT_GPIOE
//
//			Type:
//					ePowerOnMode		�����ϵ���еĹܽų�ʼ��
//					ePowerDownMode		�͹��Ľ��еĹܽų�ʼ��
//					ePowerWakeUpMode	�͹��Ļ��ѽ��еĹܽų�ʼ��
//
//����ֵ:	��
//
//��ע:
//		1��оƬ����ѡ�� GPIO ����:
//			������Ĵ�������Ϊ���, ��© OD ���ܿ�����Ч, ����������Ч
//			������Ĵ�������Ϊ����, ��© OD ���ܿ�����Ч, ����������Ч
//		2��оƬ����ѡ���ù���:
//			���ù��ܵ�����������Ŷ����俪©����, ��������������Ч
//			���ù��ܵ������������ţ��� RST/TEST/JTAGWDTEN�������������ڲ��㶨��������������������, ��©������Ч
//			������Ϊģ������, ��©���ƺ��������ƶ���Ч
//		3�����ù���Ϊģ�⹦��: 
//			LVDINx�� LCD_SEG\COM�� POWIN�� ADCINx�� ADCBAT
//			����֮����������ù���Ϊ���ֹ���
//---------------------------------------------------------------
void GPIO_Init( HT_GPIO_TypeDef *HT_GPIOx, ePOWER_MODE Type )
{
	const GPIO_InitTypeDef *GPIO_InitStructure;
	BYTE i = 0;
	WORD HT_NEWAFEN = 0;
	WORD HT_IOCFG = 0, HT_AFCFG = 0;
	WORD HT_PTDIR = 0, HT_PTOD = 0, HT_PTUP = 0;
	WORD HT_PTSET = 0, HT_PTCLR = 0;
	
	if( Type > ePowerWakeUpMode )
	{
		return;
	}
	
	//�ϵ�ģʽ
	if( Type == ePowerOnMode )
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_PowerOn[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	//�͹���ģʽ
	else if( Type == ePowerDownMode )
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_PowerDown[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	//�͹��Ļ���ģʽ
	else
	{
		if( HT_GPIOx == HT_GPIOA )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOA_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOB )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOB_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOC )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOC_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOD )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOD_POSITION];
		}
		else if( HT_GPIOx == HT_GPIOE )
		{
			GPIO_InitStructure = &GPIO_WakeUp[GPIOE_POSITION];
		}
		else
		{
			return;
		}
	}
	
	for(i = 0; i < 16; i++)
	{
		//��������Ϊ���ܶ˿�
		if (((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_COMMON) &&
			((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_NONE))
		{
			//�������ù�����Ч
			HT_NEWAFEN |= (WORD)(0x01) << i;
			HT_IOCFG |= (WORD)(0x01) << i;
			//�ڶ�����
			if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_AF_SECOND)
			{
				HT_AFCFG |= (WORD)(0x01) << i;
			}
			//���ǵڶ����þ�����Ϊ��һ����
			else
			{
				HT_AFCFG &= ~((WORD)(0x01) << i);
			}
		}
		//��������ΪGPIO�������Ų�����
		else
		{
			HT_IOCFG &= ~((WORD)(0x01) << i);
			HT_AFCFG &= ~((WORD)(0x01) << i);
		}
		
		//��Ϊ���ܶ˿ڻ�GPIO
		if ((GPIO_InitStructure + i)->eGPIO_TYPE != eGPIO_TYPE_NONE)
		{
			//��������Ϊ���
			if ((GPIO_InitStructure + i)->eGPIO_DIRECTION == eGPIO_DIRECTION_OUT)
			{
				HT_PTDIR |= (WORD)(0x01) << i;
				//����
				if ((GPIO_InitStructure + i)->eGPIO_MODE == eGPIO_MODE_PP)
				{
					HT_PTOD |= (WORD)(0x01) << i;
				}
				//�������������Ϊ��©
				else
				{
					HT_PTOD &= ~((WORD)(0x01) << i);
				}
				
				//��ΪGPIOʱ������ߵ͵�ƽ
				if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_COMMON)
				{
					//����ߵ�ƽ
					if ((GPIO_InitStructure + i)->eGPIO_OUTPUT == eGPIO_OUTPUT_HIGH)
					{
						HT_PTSET |= (WORD)(0x01) << i;
						HT_PTCLR &= ~((WORD)(0x01) << i);
					}
					//����͵�ƽ
					else if ((GPIO_InitStructure + i)->eGPIO_OUTPUT == eGPIO_OUTPUT_LOW)
					{
						HT_PTSET &= ~((WORD)(0x01) << i);
						HT_PTCLR |= (WORD)(0x01) << i;
					}
					//���Ǹߵ�ƽ���ߵ͵�ƽ, Ĭ��״̬
					else
					{
						HT_PTSET &= ~((WORD)(0x01) << i);
						HT_PTCLR &= ~((WORD)(0x01) << i);
					}
				}
				//��Ϊ���ܶ˿�ʱ, Ĭ��״̬
				else
				{
					HT_PTSET &= ~((WORD)(0x01) << i);
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
			}
			//��������Ϊ����
			else if ((GPIO_InitStructure + i)->eGPIO_DIRECTION == eGPIO_DIRECTION_IN)
			{
				HT_PTDIR &= ~((WORD)(0x01) << i);
				//����
				if ((GPIO_InitStructure + i)->eGPIO_MODE == eGPIO_MODE_FLOATING)
				{
					HT_PTUP |= (WORD)(0x01) << i;
				}
				//���Ǹ��վ�����Ϊ����
				else
				{
					HT_PTUP &= ~((WORD)(0x01) << i);
				}
			}
			//�����������������, ���ܶ˿�����Ϊ��©���; GPIO����Ϊ��©���, �����
			else
			{
				HT_PTDIR |= (WORD)(0x01) << i;
				HT_PTOD &= ~((WORD)(0x01) << i);
				//��ΪGPIO, ����ߵ�ƽ
				if ((GPIO_InitStructure + i)->eGPIO_TYPE == eGPIO_TYPE_COMMON)
				{
					HT_PTSET |= (WORD)(0x01) << i;
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
				//��Ϊ���ܶ˿�, Ĭ��״̬
				else
				{
					HT_PTSET &= ~((WORD)(0x01) << i);
					HT_PTCLR &= ~((WORD)(0x01) << i);
				}
			}
		}
	}
	HT_GPIOx->NEWAFEN = HT_NEWAFEN;
	HT_GPIOx->IOCFG = HT_IOCFG;
	HT_GPIOx->AFCFG = HT_AFCFG;
	HT_GPIOx->PTDIR = HT_PTDIR;
	HT_GPIOx->PTOD = HT_PTOD;
	HT_GPIOx->PTUP = HT_PTUP;
	HT_GPIOx->PTSET = HT_PTSET;
	HT_GPIOx->PTCLR = HT_PTCLR;
}

//-----------------------------------------------
//��������: CPU�ܽų�ʼ����ͳһ����
//
//����: 	
//          Type[in]:	ePowerOnMode		�����ϵ���еĹܽų�ʼ��
//						ePowerDownMode		�͹��Ľ��еĹܽų�ʼ��
//						ePowerWakeUpMode	�͹��Ļ��ѽ��еĹܽų�ʼ��
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void InitPort(ePOWER_MODE Type)
{
	if( Type > ePowerWakeUpMode )
	{
		return;
	}
	
	GPIO_Init(HT_GPIOA, Type);		//��ʼ��GPIOA
	GPIO_Init(HT_GPIOB, Type);		//��ʼ��GPIOB
	GPIO_Init(HT_GPIOC, Type);		//��ʼ��GPIOC
	GPIO_Init(HT_GPIOD, Type);		//��ʼ��GPIOD
	GPIO_Init(HT_GPIOE, Type);		//��ʼ��GPIOE
}


#if(LCD_DRIVE_CHIP == LCD_CHIP_NO)
BYTE TimerLcdTmp;

//�͹����µ�ѹ��LCD�Աȶȣ�С�ڶ�Ӧ��ѹ���ö�Ӧ�ĶԱȶ�
const WORD LowPowerLCDVRSEL[2][7] = { {320,    336,    360,    390,    410,    430,    430   }, 
									  {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006} };

//-----------------------------------------------
//��������: �Զ�����Һ���Աȶ�
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   �˺������Բ�ȡVRTC��VCC�̽ӷ����ı�����Ч��
//-----------------------------------------------
// Ϊ�˵�һ�ε�����ѣ�Һ���Աȶȹ�������ʾ��������⣡
static void AutoAdjustContrast(void)
{
	BYTE i;
	SWORD Vcc = 0;

	Vcc = GetADValue(0x55,SYS_POWER_AD);

	if( TimerLcdTmp > 5 )
	{
		TimerLcdTmp = 0;
	}
	
	if(TimerLcdTmp > 0)
	{
		TimerLcdTmp--;
		if (Vcc > 430)
		{
			HT_LCD->LCDCON = 0x0001;//�Աȶ�=92.4%
		}
		else
		{
			HT_LCD->LCDCON = 0x0000;//�Աȶ�=100%VLCD
		}
	}
	else
	{
		for(i = 0; i < 6; i++)
		{
			if(Vcc > LowPowerLCDVRSEL[0][5])
			{
				HT_LCD->LCDCON = LowPowerLCDVRSEL[1][6];
				break;
			}
			else if(Vcc <= LowPowerLCDVRSEL[0][i] )
			{
				HT_LCD->LCDCON = LowPowerLCDVRSEL[1][i];
				break;
			}
		}

		if (i >= 6)
		{
			HT_LCD->LCDCON = LowPowerLCDVRSEL[1][0];
		}
	}

}

//-----------------------------------------------
//��������: ��ʼ��Һ������оƬ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void InitLCDDriver(void)
{
    EnWr_WPREG();
    HT_CMU->CLKCTRL0 |= 0x000002;
	//LCD��ѹ
	HT_LCD->LCDCLK = 0x008C;//1/3ƫѹ,6COM,85.3Hz
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
	{
		#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
		//�� �� �� �� �� �� Ƭ �� �� �� �� �� 5V һ 3V �� �� �� �� �� Ҫ �� �� �� ѹ �� �� �� Ӱ
		AutoAdjustContrast();//�Զ�����Һ���Աȶȡ�
		#else
		HT_LCD->LCDCON = 0x0000;//�Աȶ�=100%VLCD
		#endif
	}
	else
	{
		//lcd��ѹ�Աȶ�
		HT_LCD->LCDCON = 0x0000;//�Աȶ�=99%VLCD
	}

    DisWr_WPREG();
}
#endif//#if(LCD_DRIVE_CHIP == LCD_CHIP_NO)


//-----------------------------------------------
//��������: ��ʼ���ж�
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void InitINT(void)
{
	InitExtInt();
}


//-----------------------------------------------
//��������: �ⲿ�ж�����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   hplc-�綯��-�ж�4���޹�---�ж�5���й�
//-----------------------------------------------
static void InitExtInt(void)
{
	NVIC_DisableIRQ(EXTI4_IRQn);
	NVIC_DisableIRQ(EXTI5_IRQn);
	HT_INT->EXTIE = 0;
	HT_INT->EXTIE |= 0x3030; //ʹ���������ж�
	HT_INT->PINFLT |= 0x0030; //�����˲�
	HT_INT->EXTIF = 0;
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI5_IRQn);
}


//-----------------------------------------------
//��������: ��CPU��AD��������
//
//����: 	Type[in]:ePowerOnMode/ePowerWakeUpMode
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_AdcConfig( ePOWER_MODE Type )
{
	/*
   	//ʹ��VCC��ADC1(ʱ�ӵ��)���¶Ȳ���
	if (HT_TBS->TBSCON	!= 0x655D)		{HT_TBS->TBSCON	 = 0x655D;}			
	//��ֹTBS�ж�
	if (HT_TBS->TBSIE	!= 0x0000)		{HT_TBS->TBSIE	 = 0x0000;}			
	//��ص�ѹ��������=1s;�¶Ȳ�������=1/2s
	if (HT_TBS->TBSPRD	!= 0x0000)		{HT_TBS->TBSPRD	 = 0x0000;}			
	//pdf��û��TBSTEST����Ĵ���˵�����Ź���˵Ĭ��д0x0200
	//TBS_ADC����=1uA;ADC_ldo��ѹ=2.7V;
	if (HT_TBS->TBSTEST	!= 0x0200)		{HT_TBS->TBSTEST = 0x0200;}	*/
	
	if( HT_TBS->VBATCMP != 0x0000 )
	{
		HT_TBS->VBATCMP = 0x0000;
	}

	if( HT_TBS->ADC0CMP != 0x0000 )
	{
		HT_TBS->ADC0CMP = 0x0000;
	}

	if( HT_TBS->TRIREQ != 0x0000 )
	{
		HT_TBS->TRIREQ = 0x0000;
	}

	if( HT_TBS->TBSTESTCTRL != 0x0000 )
	{
		HT_TBS->TBSCON |= 0x800000; //�ر�д����
		HT_TBS->TBSTESTCTRL = 0x0000;
		HT_TBS->TBSCON &= (~0x800000); //��д����
	}

	if( HT_TBS->TBSINJECTCTRL != 0x0500 )
	{
		HT_TBS->TBSCON |= 0x800000; //�ر�д����
		HT_TBS->TBSINJECTCTRL = 0x0500;
		HT_TBS->TBSCON &= (~0x800000); //��д����
	}

	if( Type == ePowerOnMode )
	{
		//ʹ��VCC��ʱ�ӵ�ء��¶Ȳ���
		if( HT_TBS->TBSCON	!= 0x14553 )  //6553
		{
			HT_TBS->TBSCON	 = 0x14553;
		}
		//��ֹTBS�ж�
		if( HT_TBS->TBSIE	!= 0x0000 )
		{
			HT_TBS->TBSIE	 = 0x0000;
		}
		//��ص�ѹ��������=2s;�¶Ȳ�������=1/2s
		if( HT_TBS->TBSPRD	!= 0x0008 )
		{
			HT_TBS->TBSPRD	 = 0x0008;
		}
		//pdf��û��TBSTEST����Ĵ���˵�����Ź���˵Ĭ��д0x0200
		//TBS_ADC����=1uA;ADC_ldo��ѹ=2.7V;
		if( HT_TBS->TBSTEST != 0x0200 )
		{
			HT_TBS->TBSTEST = 0x0200;
		}
	}
	else if( Type == ePowerWakeUpMode )
	{
		//�¶Ȳ���,Vcc
		if( HT_TBS->TBSCON	!= 0x6551 )
		{
			HT_TBS->TBSCON	 = 0x6551;
		}
		//��ֹTBS�ж�
		if( HT_TBS->TBSIE	!= 0x0000 )
		{
			HT_TBS->TBSIE	 = 0x0000;
		}
		//�¶Ȳ�������=64s VCC��������1s
		if( HT_TBS->TBSPRD	!= 0x0007 )
		{
			HT_TBS->TBSPRD	 = 0x0007;
		}
		//pdf��û��TBSTEST����Ĵ���˵�����Ź���˵Ĭ��д0x0200
		//TBS_ADC����=1uA;ADC_ldo��ѹ=2.7V;
		if( HT_TBS->TBSTEST != 0x0200 )
		{
			HT_TBS->TBSTEST = 0x0200;
		}
	}
}


//-----------------------------------------------
//��������: ���ڼ��ά��cpu�ļĴ���ֵ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void Maintain_MCU( void )
{
	//TBS ��
	api_AdcConfig(ePowerOnMode);
}


//-----------------------------------------------
//��������: ��ȡADת���Ľ����С����λ���ǹ̶��ģ����С���㲻���ʣ���Ҫ�������Լ�ת��
//
//����:
//				BYTE Mode[in]		��׼��ѹ�Ƿ�У׼ 0x00 У׼ 0x55��У׼
//				Type[in]			��ȡAD���ݵ�����
//				SYS_POWER_AD		    		0//ϵͳ��ѹ ������λС�� ��λv
//				SYS_CLOCK_VBAT_AD		    	1//ʱ�ӵ�ص�ѹ ������λС�� ��λv
//				SYS_READ_VBAT_AD	    		2//�����صĵ�ѹ ������λС�� ��λv
//				SYS_TEMPERATURE_AD		    	3//�¶�AD���� ����һλС��
//                    
//����ֵ:  	ADת���Ľ����ֻ���¶�֧�ָ�ֵ��������֧�ָ�ֵ���¶ȵĸ�ֵ���ò����ʾ
//
//��ע:   
//-----------------------------------------------
short GetADValue( BYTE Mode, BYTE Type)
{
    long Voltage;
    short ReturnValue;
    
    switch( Type )
    {
        case SYS_POWER_AD://ϵͳ��ѹ ������λС�� ��λv
            //���㹫ʽ VCC = 0.1798*VCCDAT+ 0.5943 ��λmV
            if( HT_TBS->VCCDAT & 0x8000 )//����
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->VCCDAT;
				Voltage *= VcccoffConst_K;
				Voltage += VccOffsetConst_K;
				Voltage /= 1000000;//��λV������2λС��

				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_READ_VBAT_AD://ͣ�糭���ص�ѹ
        case SYS_CLOCK_VBAT_AD://ʱ�ӵ�ص�ѹ ������λС��
            //���㹫ʽ VADCIN1 = 0.0258*ADC1DAT+4.7559 ��λmV
			//���㹫ʽ VADCIN4 = 0.0258*ADC1DAT+4.7559 ��λmV
            // if( HT_TBS->VBATDAT & 0x8000 )//����
            // {
            //     Voltage = 0;				
            // }
			if( HT_TBS->ADC4DAT & 0x8000 )//����
            {
                Voltage = 0;				
            }
            else
            {
				//ϵ�����ܲ��Դ���!!!!!!
				//Voltage = HT_TBS->VBATDAT;
				Voltage = HT_TBS->ADC4DAT;
				Voltage *= VBatcoffConst_K;
				Voltage += VBatOffsetConst_K; 
				Voltage /= 1000000L; 
				
				
				if(abs(BatteryPara.ClockBatteryOffset) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//У׼ƫ�ã��������ֵ0.5V ����5%�ĵ�����2������������10%������15%���п��� 364*0.12= 43
				{
					Voltage += BatteryPara.ClockBatteryOffset;
				}
				
				if( Voltage >= 368 )
				{
					Voltage = 368;
				}

				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_TEMPERATURE_AD://�¶ȼĴ��� ����һλС��
            //���㹫ʽV = 12.9852 - 0.0028*TMPDAT
            ReturnValue = ((short)HT_TBS->TMPDAT-(short)HT_RTC->Toff);
            Voltage = ((long)ReturnValue * TPScoffConst_K);
            Voltage += TPSOffsetConst_K;
            Voltage /= 10000;
            break;
        default:
            Voltage = 0;
            break;
    }

    ReturnValue = (short)Voltage;
    
    return ReturnValue;
}



//-----------------------------------------------
//��������: ����оƬURAT��ȡ����оƬһ���ֽ�
//
//����:		�� 
//                    
//����ֵ: 
//		   ���ض�ȡ�����ݣ���ʱ����0
//��ע:   
//-----------------------------------------------
BYTE Drv_SampleReadByte(void)
{	
	WORD SampleComTmr;
	
	SampleComTmr = 0;
	
	while(SampleComTmr < 210)
	{
		if(EMU_UARTSTA & UART_UARTSTA_RXIF)	
		{
			EMU_UARTSTA &= ~UART_UARTSTA_RXIF;
	
			return EMU_UARTBUF;
		}
		
		api_Delay100us( 1 );
		SampleComTmr++;
	}
	
	return 0;
}


//-----------------------------------------------
//��������: ����оƬURATд�����оƬһ���ֽ�
//
//����:		Data[in] Ҫд������оƬ������ 
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void Drv_SampleWriteByte(BYTE Data)
{	
	WORD SampleComTmr;
	
	SampleComTmr = 0;
	
	while(SampleComTmr < 210)
	{
		if(EMU_UARTSTA & UART_UARTSTA_TXIF)
		{
			EMU_UARTSTA &= ~UART_UARTSTA_TXIF;
	
			EMU_UARTBUF = Data;
			
			return;
		}
		
		api_Delay100us( 1 );
		SampleComTmr++;
	}
}


//-----------------------------------------------
//��������: ����Χ�豸�ĵ�Դ����
//
//����:		Type[in] 0:�ϵ�ʱ���ⲿ��Դ�Ŀ���  1:����
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void PowerCtrl( BYTE Type )
{
	if( Type == 0 )
	{
		//��LCD��Դ
		POWER_LCD_OPEN;
		POWER_SAMPLE_OPEN;//һ��Ҫ�Ѳ���5v��Դ���ڲ���������Դ����ǰ��
		POWER_FLASH_OPEN;
	    POWER_HALL_OPEN;
		//�����Ƿ�ʹ��ESAM�ĵ�Դ����
	    POWER_ESAM_OPEN;

		#if (SEL_F415 == YES)
		POWER_F415_CV_OPEN;
		#else
		//��ʹ��415������ڸ���ر����ע������gpio����������
		POWER_F415_CV_CLOSE;
		#endif
	}
}

//--------------------------------------------------
//��������:  �ر�esam��Դ
//         
//����  : ePOWER_MODE  POWER_MODE ������������������
//
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE  POWER_MODE )
{
	//PB6	ESAM-CS2
	//PB7	ESAM-CLK2
	//PB8	ESAM-SO2
	//PB9	ESAM-SI2
	//�˿ڹ�������
	HT_GPIOB->IOCFG &= (~(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9));
	//�˿ڷ���
	HT_GPIOB->PTDIR |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	//����
	HT_GPIOB->PTOD |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	//��ʼΪ��
	HT_GPIOB->PTCLR = (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
	POWER_ESAM_CLOSE;//�ر�ESAM��Դ
	
	api_Delayms(5);
}

//--------------------------------------------------
//��������:  ��SPI�¹��ص��豸���и�λ
//
//����:     eCOMPONENT_TYPE  Type[in]  eCOMPONENT_SPI_SAMPLE/eCOMPONENT_SPI_FLASH/eCOMPONENT_SPI_LCD/eCOMPONENT_SPI_ESAM/eCOMPONENT_SPI_CPU
//         BYTE  Time[in]  ��ʱʱ�䣨MS��
//
//����ֵ:    BOOL  TRUE: �ɹ� FALSE: ʧ��
//
//��ע:	  ��������ĸ�λesam spi ���⸴λ�ᵼ�½���Ӧ������ʧЧ
//--------------------------------------------------
BOOL ResetSPIDevice( eCOMPONENT_TYPE Type, BYTE Time )
{
	if( Type >= eCOMPONENT_TOTAL_NUM )
	{
		return FALSE;
	}
	
	api_Delayms(5);
	
	if( (Type == eCOMPONENT_SPI_ESAM )  ) //PB6  �ر�ESAM CS
	{
		//PB6	ESAM-CS   

		//PB7	ESAM-CLK2
		//PB8	ESAM-SO2
		//PB9	ESAM-SI2
		//�˿ڹ�������
		HT_GPIOB->IOCFG &= (~(GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9));
		//�˿ڷ���
		HT_GPIOB->PTDIR |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		//����
		HT_GPIOB->PTOD |= (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		//��ʼΪ��
		HT_GPIOB->PTCLR = (GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
		
		POWER_ESAM_CLOSE;//���´򿪵�Դ
		api_Delayms(20);
		POWER_ESAM_OPEN;
		api_Delayms(Time);

		//CS����
		HT_GPIOB->PTSET |= 	GPIO_Pin_6;

		//MISO����Ϊ��ֹ����(����)
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		HT_GPIOB->PTUP |= GPIO_Pin_8; 

		//api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 ); 
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
		//SPI��������Ϊ��������
		//PB7           SCLK             	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PB8           MISO            	//��ͨ�˿� ���� ����
		//PB9           MOSI           		//��ͨ�˿� ��� ���� ��ʼΪ��
		HT_GPIOB->NEWAFEN |= GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		HT_GPIOB->AFCFG |=  GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
	}
	else if( Type == eCOMPONENT_SPI_SAMPLE )
	{
		//PC4	SAMPLE_SI
		//PC5	SAMPLE_SO
		//PC6	SAMPLE_CLK
		//PD14	SAMPLE_CS
		//�˿ڹ�������
		HT_GPIOC->IOCFG &= 	(~(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
		HT_GPIOD->IOCFG &= 	(~(GPIO_Pin_14)); 
		
		//��©
		// //Flash������ CPU������Ϊ��©
		// HT_GPIOC->PTOD &= (~(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
				
		//����
		//���������pc5��������
		HT_GPIOC->PTOD |= ((GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)); 
		HT_GPIOD->PTOD |= 	((GPIO_Pin_14)); 
	
		//��ʼΪ��
		HT_GPIOC->PTCLR |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;
		HT_GPIOD->PTCLR |= 	GPIO_Pin_14;

		
		POWER_SAMPLE_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		//CS��ʼΪ��
		HT_GPIOD->IOCFG  &= 0xbfff; //��������CS 
		// HT_GPIOD->PTSET |=  GPIO_Pin_14;		
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		HT_GPIOD->PTSET |=  GPIO_Pin_14;

		HT_GPIOC->PTDIR |=	GPIO_Pin_4|
							GPIO_Pin_6;
		
		//MISO����Ϊ����
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5);
		//��ֹ���� ��ֹCPU��Flash���ѹ
		HT_GPIOC->PTUP |= GPIO_Pin_5; 
		api_Delayms(Time);
		api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 ); 
		
		//SPI����Ϊ��������
		HT_GPIOC->IOCFG |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;
	}
	else if( Type == eCOMPONENT_SPI_FLASH )
	{
		//PB9	FLASH_SI    	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PB8	FLASH_MISO
		//PB7	FLASH_CLK		������� ��
		//PC7	FLASH_CS		
		//�˿ڹ�������
		HT_GPIOB->IOCFG &= 	(~(GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9)); 
		HT_GPIOC->IOCFG &= 	(~(GPIO_Pin_7)); 
		
		//����
		HT_GPIOB->PTOD |= (GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9); 
		HT_GPIOC->PTOD |= (GPIO_Pin_7); 
		//��ʼΪ��
		HT_GPIOB->PTCLR |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOC->PTCLR |= 	GPIO_Pin_7;
		
		POWER_FLASH_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		//CS��ʼΪ��
        HT_GPIOC->IOCFG  &= 0xff7f; //��������CS   					
		HT_GPIOC->PTDIR  |=	GPIO_Pin_7;
		HT_GPIOC->PTOD   |=	GPIO_Pin_7;
		HT_GPIOC->PTCLR  |= GPIO_Pin_7;
		HT_GPIOC->PTSET |=  GPIO_Pin_7;

		HT_GPIOB->PTDIR |=	GPIO_Pin_7|
							GPIO_Pin_9;
		//MISO����Ϊ����
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		//��ֹ���� ��ֹCPU��Flash���ѹ
		HT_GPIOB->PTUP |= GPIO_Pin_8; 
		api_Delayms(Time);
		api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 ); 
		//SPI����Ϊ��������
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOB->AFCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		HT_GPIOB->NEWAFEN |= GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	}
	
	api_Delayms(5);
	
	return TRUE;
}

//-----------------------------------------------
//��������: �����ʼ������
//
//����:		��
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void InitBoard( void )
{
	BYTE i;
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
    CLEAR_WATCH_DOG;
	
	InitPort( ePowerOnMode );//������ʼ��
	
	//�˿ڳ�ʼ��
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 100 ); //�Բ���оƬ���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 100 ); //��esam���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //��Flash���и�λ   
    
	PowerCtrl( 0 );
	
	ESAMSPIPowerDown( ePowerOnMode );//�رյ���ESAM��Դ
	
	api_AdcConfig(ePowerOnMode);
	
	InitINT() ;//�رղ��õ��ж�
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	//����ģ���ϵ��ʼ����ע�ⲻҪ����˳��
	api_PowerUpFlag();
	
	//api_PowerUpRtc�ж�RTC�ĸ�λ���� ��λ�������������RTC��TBS��ؼĴ��� �����Ҫ���µ���һ��ADC����
	api_PowerUpRtc();
	api_AdcConfig(ePowerOnMode); //!!!!�ر�ע�� �˴�Aapi_AdcConfig����ɾ - yxk
	
	api_WriteFreeEvent(EVENT_SYS_START, HT_PMU->RSTSTA);
	if((HT_PMU->RSTSTA&0x0005)==0x0004)//���Ź���λ��¼�쳣�¼�
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	HT_PMU->RSTSTA = 0;
	
	api_PowerUpPara();
	api_PowerUpSave();

	api_PowerUpVersion();
	
	api_PowerUpLcd( ePowerOnMode );
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_PowerUpSample();

	api_PowerUpProtocol();
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_PowerUpPrePay();
	PowerUpRelay();
	
	api_PowerUpEnergy();
    api_PowerUpFreeze();
	
	//���ϵ粹�������е��磬�������͹���
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	api_ReflashFreeze_Ram();
    api_PowerUpEvent();
    api_PowerUpSysWatch();//��ˢ�������ϱ�ģʽ�ֺ���е�ص�ѹ�ȵ�����ж�-��event�ϵ紦������˳���ϵ-���ĺ�
    	
	#if( MAX_PHASE == 3 )
	api_PowerUpDemand();
	#endif
	
	api_PowerUpVersion();
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnRunEnterDownMode );//�ڴ�֮ǰ�ϵ��ѳ�ʼ����� �˴���⵽�͹��İ��������紦�� ���ڽ��������絫��Ϊ���쳣��λ������
	}
	
	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}

	#if(SEL_F415 == YES)
	api_PowerUpCollect();
	#endif

	//��ʼ��_noinit_��־
	api_InitNoInitFlag();
	
	RESET_PLC_MODUAL;
    api_Delayms(20);
	ENABLE_PLC_PROGRAMM;
}


//-----------------------------------------------
//��������: SPIƬѡ��������
//
//����:		
//		No[in]		��Ҫѡ���оƬ
// 			CS_SPI_FLASH    - FLASH
// 			CS_SPI_ESAM     - ESAM
//      Do[in]
// 			TRUE��ʹ��Ƭѡ
// 			FALSE����ֹƬѡ
//
//����ֵ: ��
//		   
//��ע: 
//-----------------------------------------------
void DoSPICS(WORD No, WORD Do)
{
	if( No == CS_SPI_FLASH )
	{
		FLASH_CS_DISABLE;
	}
	else if( No == CS_SPI_SAMPLE )
	{
		SAMPLE_CS_DISABLE;
	}
	else
	{
		ESAM_CS_DISABLE; 
	}
	
	if( Do == FALSE )
	{
		return;
	}
	
	switch( No )
	{
		case CS_SPI_ESAM:
			ESAM_CS_ENABLE;
			break;
		case CS_SPI_FLASH:
			FLASH_CS_ENABLE;
			break;
		case CS_SPI_SAMPLE:
			SAMPLE_CS_ENABLE;
			break;
		default:
			break;
	}
	
	api_Delay10us(1);
	
	return;
}


//-----------------------------------------------
//��������: ���ڳ�ʼ������
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷ��ʼ��   FALSE:û����ȷ��ʼ��
//		   
//��ע:   
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum)
{
    BYTE BpsBak, IntStatus, Bps, i;
    HT_UART_TypeDef *USARTx;
	IRQn_Type UARTx_IRQn;

    if(SciPhNum == 0xff)
    {
        return TRUE;
    }

	USARTx = NULL;
	BpsBak = 0;
	BpsBak = api_GetBaudRate( 0, SciPhNum ); 

    switch(SciPhNum)
    {
        case 0:	//�ز�
            USARTx = HT_UART0;
			UARTx_IRQn = UART0_IRQn;
            NVIC_DisableIRQ(UART0_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART0_IRQn);           //�������״̬
            NVIC_SetPriority(UART0_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART0_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000010;               //��������ʱ��
            DisWr_WPREG();
            break;
        case 1:	//415
            USARTx = HT_UART1;
			UARTx_IRQn = UART1_IRQn;
            NVIC_DisableIRQ(UART1_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART1_IRQn);           //�������״̬
            NVIC_SetPriority(UART1_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART1_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000020;               //��������ʱ��
            #if( COMM_TYPE == COMM_NEAR )||(( COMM_TYPE == COMM_NOAR ))
            HT_UART1->IRCON   = 0x0000;                   //���ⲻ����
            HT_UART1->IRDUTY  = 0x0000;                   //50%ռ�ձ�
            #else
            HT_UART1->IRCON   = 0x0001;                   //�������
            HT_UART1->IRDUTY  = 0x0001;                   //25%ռ�ձ�
            #endif
            DisWr_WPREG();
            break;
		case 2:	//485
			if( (BpsBak&0x0F) == 10 ) //115200�����ʹر��˲�����
			{
				HT_GPIOC->FILT &= (~(1 << 12));               //�ر��˲�����
			}
			else
			{
				HT_GPIOC->FILT |= (1 << 12);               //�����˲�����
			}
			USARTx = HT_UART2;
			UARTx_IRQn = UART2_IRQn;
            NVIC_DisableIRQ(UART2_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART2_IRQn);           //�������״̬
            NVIC_SetPriority(UART2_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART2_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000040;               //��������ʱ��
            DisWr_WPREG();
            break;
        case 3://����
			USARTx = HT_UART3;
			UARTx_IRQn = UART3_IRQn;
            NVIC_DisableIRQ(UART3_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART3_IRQn);           //�������״̬
            NVIC_SetPriority(UART3_IRQn, 3);            //�������ȼ�
            NVIC_EnableIRQ(UART3_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x0080;                 //��������ʱ��
            DisWr_WPREG();
            break;
        /*
        case 4:
			USARTx = HT_UART4;
			UARTx_IRQn = UART4_IRQn;
            NVIC_DisableIRQ(UART4_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART4_IRQn);           //�������״̬
            NVIC_SetPriority(UART4_IRQn, 3);            //�������ȼ�
            NVIC_EnableIRQ(UART4_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x0100;                 //��������ʱ��
            DisWr_WPREG();
            break;
        */
        default:
            USARTx = NULL;
            break;
    }

    if(USARTx == NULL)
    {
        return FALSE;
    }

    USARTx->MODESEL = 0x0000;//UART���ܣ���7016���ܣ�
    USARTx->UARTSTA = 0x0000;//����жϱ�־λ
    USARTx->UARTCON = 0x0000;//����ƼĴ���
    //USARTx->IRCON   = 0x0000;//�ǵ���
    //BpsBak = api_GetBaudRate(0, SciPhNum);
    //IntStatus = api_splx(0);
	Bps = BpsBak&0x0F;
	if( Bps >= (sizeof(TAB_Baud) / sizeof(TAB_Baud[0])) )
	{
		Bps = 6;    	//������ݴ��� Ĭ��Ϊ9600
		BpsBak = 0x46;
	}
	USARTx->SREL = TAB_Baud[Bps];

    //ȡֹͣλ
    Bps = ((BpsBak>>4) & 0x01);

    switch(Bps)
    {
        case 0x00://1λֹͣλ
            USARTx->UARTCON &= ~0x0100;
            break;
        case 0x01://2λֹͣλ
            USARTx->UARTCON |= 0x0100;
            break;
        default://Ĭ��1λֹͣλ
            USARTx->UARTCON &= ~0x0100;
            break;
    }
    //ȡУ��
	Bps = ((BpsBak >> 5)&0x03);
    switch(Bps)
    {            
        case 0x00://��У��
            USARTx->UARTCON &= ~0x0070;
            break;
        case 0x01://����У�飬��У��
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0030;
            break;
        case 0x02://����У�飬żУ��
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0050;//2016.8.3ȷ��0x0050����żУ�飬��Ȫ�ľ���
            break;
        default://Ĭ������У�飬żУ��
            USARTx->UARTCON &= ~0x0070;
            USARTx->UARTCON |= 0x0050;
            break;
    }
    
    USARTx->UARTCON |= 0x0008; //�����ж�ʹ��
    USARTx->UARTCON |= 0x0002; //����ʹ��
    SciRcvEnable(SciPhNum);

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            api_InitSubProtocol(i);
            //���ڼ��Ӷ�ʱ��
            Serial[ i ].WatchSciTimer = WATCH_SCI_TIMER;
            break;
        }
    }
	NVIC_EnableIRQ(UARTx_IRQn);                 //ʹ�ܴ����ж�
    //api_splx(IntStatus);
    return TRUE;
}


//-----------------------------------------------
//��������: ʹ�ܴ��ڽ��պ���
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷʹ��   FALSE:û����ȷʹ��
//		   
//��ע:   
//-----------------------------------------------
static BOOL SciRcvEnable(BYTE SciPhNum)
{
    switch(SciPhNum)
    {
        case 0:
            ENABLE_HARD_SCI_0_RECE;  
            //api_Delay100us(1);// ��ʱ100us ����ת���� ��ʱ������ ���һ�Ӱ�����ͨ��
            HT_UART0->UARTCON |= 0x000A;
            break;
        case 1:
            ENABLE_HARD_SCI_1_RECE;
            //api_Delay100us(1);// ��ʱ100us
            HT_UART1->UARTCON |= 0x000A;
            break;
        case 2:
            ENABLE_HARD_SCI_2_RECE;
            //api_Delay100us(1);// ��ʱ100us
            HT_UART2->UARTCON |= 0x000A;
            break;
        case 3:
            ENABLE_HARD_SCI_3_RECE;
            //api_Delay100us(1);// ��ʱ100us
            HT_UART3->UARTCON |= 0x000A;
            break;		
        // case 4:
        //     ENABLE_HARD_SCI_4_RECE;
        //     //api_Delay100us(1);// ��ʱ100us
        //     HT_UART4->UARTCON |= 0x000A;
        //     break;
        default:
            break;
    }

    return TRUE;
}


//-----------------------------------------------
//��������: ��ֹ���ڽ��պ���
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷ��ֹ   FALSE:û����ȷ��ֹ
//		   
//��ע:   
//-----------------------------------------------
static BOOL SciRcvDisable(BYTE SciPhNum)
{
    BYTE i = 0;

    switch(SciPhNum)
    {
        case 0:
            HT_UART0->UARTCON &= ~0x000A;//��ֹ���գ���ֹ�����ж�
            break;
        case 1:
            HT_UART1->UARTCON &= ~0x000A;
            break;
        case 2:
            HT_UART2->UARTCON &= ~0x000A;
            break;
        case 3:
            HT_UART3->UARTCON &= ~0x000A;
            break;
        // case 4:
        //     HT_UART4->UARTCON &= ~0x000A;
        //     break;
        default:
            break;
    }

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            Serial[i].WatchSciTimer = WATCH_SCI_TIMER;
        }
    }

    return TRUE;
}


//-----------------------------------------------
//��������: ʹ�ܴ��ڷ��ͺ���
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷʹ��   FALSE:û����ȷʹ��
//		   
//��ע:   
//-----------------------------------------------
static BOOL SciSendEnable(BYTE SciPhNum)
{
    switch(SciPhNum)
    {
        case 0:
            ENABLE_HARD_SCI_0_SEND;//
            api_Delay100us(1);          // ��ʱ100us
            break;
        case 1:
            ENABLE_HARD_SCI_1_SEND;
            api_Delay100us(1);          // ��ʱ100us
            break;
        case 2:
            ENABLE_HARD_SCI_2_SEND;
            api_Delay100us(1);          // ��ʱ100us
            break;
        case 3:
            ENABLE_HARD_SCI_3_SEND;
            api_Delay100us(1);          // ��ʱ100us
            break;
        // case 4:
        //     ENABLE_HARD_SCI_4_SEND;
        //     api_Delay100us(1);          // ��ʱ100us
        //     break;
        default:
            break;
    }

    return TRUE;
}


//-----------------------------------------------
//��������: ���ڿ�ʼ���ͺ���
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷ��ʼ����   FALSE:û����ȷ��ʼ����
//		   
//��ע:   
//-----------------------------------------------
static BOOL SciBeginSend(BYTE SciPhNum)
{
    BYTE i;

    for(i = 0; i < MAX_COM_CHANNEL_NUM; i++)
    {
        if(SciPhNum == SerialMap[i].SCI_Ph_Num)
        {
            if(Serial[i].TXPoint == 0)
            {
                switch(SciPhNum)
                {
                    case 0://�d��
                        HT_UART0->UARTCON |= 0x0001; //����ʹ��
                        HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART0->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART0_IRQn);//���ж�
                        break;
                    case 1://415uart
                        HT_UART1->UARTCON |= 0x0001; //����ʹ��
                        HT_UART1->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART1->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART1_IRQn);//���ж�
                        break;
                    case 2://485
                        HT_UART2->UARTCON |= 0x0001; //����ʹ��
                        HT_UART2->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART2->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART2_IRQn);//���ж�
                        break;
                    case 3:
                        HT_UART3->UARTCON |= 0x0001; //����ʹ��
                        HT_UART3->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART3->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART3_IRQn);//���ж�
                        break;
                    // case 4:
                    //     HT_UART4->UARTCON |= 0x0001; //����ʹ��
                    //     HT_UART4->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                    //     HT_UART4->UARTCON |= 0x0004; //�����ж�ʹ��
                    //     NVIC_EnableIRQ(UART4_IRQn);//���ж�
                    //     break;
                    default:
                        break;
                }
            }

            break;
        }
    }

    return TRUE;
}


//-----------------------------------------------
//��������: ��������ģ��Ĵ��ڡ���ʼ�����ڡ��������ֽ�
//
//����:		head[in]		Ҫ���͵ĵ�һ���ֽ�
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void Open_EMUUART(BYTE Head)
{
	BYTE Result = TRUE;
	
	if( (HT_CMU->CLKCTRL1&0x000080) == 0 )
	{
		Result = FALSE;
	}
	if( (HT_GPIOE->IOCFG&0x0030) == 0 )
	{
		Result = FALSE;
	}
	if( (HT_GPIOE->AFCFG&0x0030) != 0 )
	{
		Result = FALSE;
	}
	
	if( Result == FALSE )
	{
		EnWr_WPREG();
	    HT_CMU->CLKCTRL1 |= 0x000080;                 //��������ʱ��
		DisWr_WPREG();
	    HT_GPIOE->IOCFG |= 0x0030;
	    HT_GPIOE->AFCFG &= ~0x0030;	    
	    api_Delay100us(2);
	}
		
	NVIC_DisableIRQ(UART3_IRQn);				//��ֹ�����ж�
	HT_UART3->MODESEL = 0x0000;					//UART����
	HT_UART3->UARTSTA = 0x0000;					//����жϱ�־λ
	HT_UART3->UARTCON = 0x0053;					//���߼�,8λ����,żУ��,1ֹͣλ,����,����ʹ��
	HT_UART3->IRCON   = 0x0000;					//�ǵ���
	HT_UART3->SREL    = TAB_Baud[4];			//������:4800bps
	HT_UART3->SBUF    = Head;
}


//-----------------------------------------------
//��������: �رռ���ģ��Ĵ���
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void Close_EMUUART(void)
{
	NVIC_DisableIRQ(UART3_IRQn);				//��ֹ�����ж�
	HT_UART3->UARTCON = 0x0000;					//��ֹ���ڹ���
}

//-----------------------------------------------
//��������: ��ʼ��SPI����
//
//����:
//			Component[in]	Ԫ����eCOMPONENT_TYPE
//			Type[in]		SPI��ģʽ  eSPI_MODE_0/eSPI_MODE_1/eSPI_MODE_2/eSPI_MODE_3�ֱ��ӦSPI��ģʽ0/1/2/3
//����ֵ:  	��
//
//��ע:
//�����
//	FALSH - SPI0
//  ESAM  - SPI3
//-----------------------------------------------
void api_InitSPI( eCOMPONENT_TYPE Component, eSPI_MODE Type )
{
	//0x00bX��ʵ��1.4M����
	//0x00cX��ʵ��680K����
	//0x00aX������2.8M����
	//0x009X������5.6M����
	//0x008X������11M����
	if((Component == eCOMPONENT_SPI_ESAM)||(Component == eCOMPONENT_SPI_FLASH))
	{
		//flash��essam����ʹ��SPI3
		EnWr_WPREG();
		HT_CMU->CLKCTRL1 |= 0x020000; //ʹ��SPI3ʱ��

		//�ر�SPIģ��
		HT_SPI3->SPICON &= (~BIT0);
		//0x00bX��ʵ��1.4M����
		//0x00cX��ʵ��680K����
		//0x00aX������2.8M����
		//0x009X������5.6M����
		//0x008X������11M����
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI3->SPICON = 0x00a2; //ESAM ͨѶ����Ϊ2.8M
				break;
			case eSPI_MODE_1:
				HT_SPI3->SPICON = 0x00aa;
				break;
			case eSPI_MODE_2:
				HT_SPI3->SPICON = 0x00a6;
				break;
			case eSPI_MODE_3:
				HT_SPI3->SPICON = 0x00ae;
				break;
			default:
				HT_SPI3->SPICON = 0x00ae;
				break;
		}

		//��SPI
		HT_SPI3->SPICON |= BIT0;

		HT_SPI3->SPISTA = 0x0000; //״̬����
		DisWr_WPREG();
	}
	else 
	{
		//����оƬ
		EnWr_WPREG();
		HT_CMU->CLKCTRL0 |= 0x000004; //ʹ��SPI0ʱ��

		//�ر�SPIģ��
		HT_SPI0->SPICON &= (~BIT0);
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI0->SPICON = 0x00b2; //FLASH ͨѶ����Ϊ1.4M
				break;
			case eSPI_MODE_1:
				HT_SPI0->SPICON = 0x005b; //����оƬͨ��330K����
				break;
			case eSPI_MODE_2:
				HT_SPI0->SPICON = 0x00b6;
				break;
			case eSPI_MODE_3:
				HT_SPI0->SPICON = 0x00be;
				break;
			default:
				HT_SPI0->SPICON = 0x00be;
				break;
		}

		//��SPI
		HT_SPI0->SPICON |= BIT0;

		HT_SPI0->SPISTA = 0x0000; //״̬����
		DisWr_WPREG();
	}
}


//-----------------------------------------------
//��������: ���жϷ�ʽ��дSPI�ĺ���
//
//����:
//			Component[in]	����������
//			Data[in]		д���ֵ
//
//����ֵ:  	������ֵ
//
//��ע:
//-----------------------------------------------
BYTE api_UWriteSpi( eCOMPONENT_TYPE Component, BYTE Data )
{
	if( Component == eCOMPONENT_SPI_SAMPLE )
	{
		while( HT_SPI0->SPISTA&0x0008 )
		{
			;
		}

		HT_SPI0->SPIDAT = Data;

		while( !(HT_SPI0->SPISTA&0x0008) )
		{
			;
		}

		return HT_SPI0->SPIDAT;
	}
	else
	{
		//eCOMPONENT_SPI_FLASH ��eCOMPONENT_SPI_ESAM����spi3
		while( HT_SPI3->SPISTA&0x0008 )
		{
			;
		}

		HT_SPI3->SPIDAT = Data;

		while( !(HT_SPI3->SPISTA&0x0008) )
		{
			;
		}

		return HT_SPI3->SPIDAT;
	}
}

//-----------------------------------------------
//��������:     ���ö๦�ܶ��Ӻ���
//
//����:	    Type[in] �๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
//          
//
//����ֵ: 	    ��
//		   
//��ע:   
//-----------------------------------------------
BYTE api_MultiFunPortSet( BYTE Type )
{
    //û��5v�������κβ�����ֱ���˳�
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return FALSE;
	}

	if( Type == eCLOCK_PULS_OUTPUT )
	{
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
	else if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
	{
        MULTI_FUN_PORT_OUT_H;
	}
    else
    {
        return FALSE;
    }
	
    GlobalVariable.g_byMultiFunPortType = Type;

    return TRUE;
}


//-----------------------------------------------
//��������:     �๦�ܶ��ӿ��ƺ���
//
//����:	    Type[in] �๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
//          
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_MultiFunPortCtrl( BYTE Type )
{
	//û��5v�������κβ�����ֱ���˳�
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
	{
		return;
	}

	if( GlobalVariable.g_byMultiFunPortType != Type )
	{
		return;
	}

	if( (Type == eDEMAND_PERIOD_OUTPUT)||(Type == eSEGMENT_SWITCH_OUTPUT) )
	{
		GlobalVariable.g_byMultiFunPortCounter = (80 / SYS_TICK_PERIOD_MS); //80ms ��systick��ʱ���
		MULTI_FUN_PORT_OUT_L;
	}
	else if( Type == eCLOCK_PULS_OUTPUT )
	{
		//�л�Ϊ��ģʽʱ���Զ������û�е��õĵط�
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
}


///////////////////////////////////////////////////////////////////////
//�жϷ������
////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
//��������: systick�жϷ������10ms�ж�һ��
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
//-----------------------------------------------
//��������: systick�жϷ������10ms�ж�һ��
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void SysTickIRQ_Service(void)
{
    BYTE i;
    
    SysTickCounter ++;
    if( SysTickCounter >= 10000 )
    {
    	SysTickCounter = 0;
    }
    
    api_SetAllTaskFlag( eFLAG_10_MS );
    
    if( (SysTickCounter%50) == 0 )
    {
    	api_SetAllTaskFlag( eFLAG_500_MS );
    }
    //������
    #if( PREPAY_MODE == PREPAY_LOCAL )
	//�ϵ�ʱִ�д˺���
	if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
	{
		api_BeepReadCard();
	}
    #endif 
    //ͨѶ��ʱ��
    for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
    {
        if(Serial[i].ReceToSendDelay > 0)//10ms����һ��
        {
            Serial[i].ReceToSendDelay --;
        }
        if(Serial[i].RxOverCount > 1)//10ms����һ�� ע�⣬�����Ǵ���1
        {
            Serial[i].RxOverCount --;
        }
		if (Serial[i].SendToSendDelay > 0)
		{
			Serial[i].SendToSendDelay--;
		}
    }
    
	//�����ϱ���ʱ����
	ReportTimeMinus();

    if( GlobalVariable.g_byMultiFunPortCounter != 0 )
    {
    	GlobalVariable.g_byMultiFunPortCounter--;
    	if( GlobalVariable.g_byMultiFunPortCounter == 0 )
    	{
    		MULTI_FUN_PORT_OUT_H;
    	}
    }
}


//-----------------------------------------------
//��������: �ⲿ�ж�0�������Ŀǰ��������ɼ�
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:����оƬ������   
//-----------------------------------------------
void EXTI0IRQ_Service( void )
{	
    // hplc���ô��ж�
	// //�����ؿ�������� �½��عر������ ͬʱ��ʱ������ ˫����
	// if( EMU_ACTIVE )
	// {
	// 	PULSE_LED_ON;
	// 	api_AddSamplePulse();
	// }
	// else
	// {
	// 	PULSE_LED_OFF;
	// }

	HT_INT->EXTIF &= ~0x0101;	//���жϱ�־
}


//Ŀǰû����
void EXTI1IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0202;									//���жϱ�־
}


//Ŀǰû����
void EXTI2IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0404;									//���жϱ�־
}


//Ŀǰû����
void EXTI3IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x0808;									//���жϱ�־
}

//----hplc-����ѿ����ܵ��ܱ�-�޹������ж�
void EXTI4IRQ_Service( void )
{
	//�����ؿ�������� �½��عر������ ͬʱ��ʱ������ ˫����
	if( EMU_QCTIVE )
	{
		//���Խ����޹������жϣ����Ǳ����޹�
		// PULSE_LED_ON;
		// api_AddSamplePulse();
	}
	else
	{
		// PULSE_LED_OFF;
	}
	HT_INT->EXTIF &= ~0x1010;									//���жϱ�־
}


//---hplc,�й������ж�
void EXTI5IRQ_Service( void )
{
	//�����ؿ�������� �½��عر������ ͬʱ��ʱ������ ˫����
	if( EMU_ACTIVE )
	{
		PULSE_LED_ON;
		api_AddSamplePulse();
	}
	else
	{
		PULSE_LED_OFF;
	}
	HT_INT->EXTIF &= ~0x2020;									//���жϱ�־
}


//Ŀǰû����
void EXTI6IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x0040 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( DOWN_KEY_PRESSED )
			{
				api_Delay100us( 1 ); //�������� ʵ��Լ35ms
				if( DOWN_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.DownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x4040;                                   //���жϱ�־
}


//-----------------------------------------------
//��������: ������0�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART0IRQ_Service( void )
{
	if (HT_UART0->UARTSTA & 0x0002)
	{
		HT_UART0->UARTSTA &= ~0x0002;
		if((HT_UART0->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(0, HT_UART0->SBUF);
		}
		else
		{
			HT_UART0->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART0->UARTSTA & 0x0001)
	{
		HT_UART0->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(0,HT_UART0);
	}
}


//-----------------------------------------------
//��������: ������1�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART1IRQ_Service( void )
{
	if (HT_UART1->UARTSTA & 0x0002)
	{
		HT_UART1->UARTSTA &= ~0x0002;
		if((HT_UART1->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(1, HT_UART1->SBUF);
		}
		else
		{
			HT_UART1->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART1->UARTSTA & 0x0001)
	{
		HT_UART1->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(1,HT_UART1);
	}
}


//-----------------------------------------------
//��������: ������2�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART2IRQ_Service( void )
{
	if (HT_UART2->UARTSTA & 0x0002)
	{
		HT_UART2->UARTSTA &= ~0x0002;
		if((HT_UART2->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(2, HT_UART2->SBUF);
		}
		else
		{
			HT_UART2->UARTSTA &= ~0x0004;
		}

	}
	if (HT_UART2->UARTSTA & 0x0001)
	{
		HT_UART2->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(2,HT_UART2);
	}
}


//Ŀǰû����
void UART3IRQ_Service( void )
{
	if (HT_UART3->UARTSTA & 0x0002)
	{
		HT_UART3->UARTSTA &= ~0x0002;
		if((HT_UART3->UARTSTA & 0x0004)==0)
		{
			USARTx_Rcv_IRQHandler(3, HT_UART3->SBUF);
		}
		else
		{
			HT_UART3->UARTSTA &= ~0x0004;
		}
	}
	if (HT_UART3->UARTSTA & 0x0001)
	{
		HT_UART3->UARTSTA &= ~0x0001;
        USARTx_Send_IRQHandler(3,HT_UART3);
	}
}


//Ŀǰû����
void UART4IRQ_Service( void )
{
}


//Ŀǰû����
void UART5IRQ_Service( void )
{
}


//Ŀǰû����
void TIMER0IRQ_Service( void )
{
	HT_TMR0->TMRIF = 0;
}


//Ŀǰû����
void TIMER1IRQ_Service( void )
{
	HT_TMR1->TMRIF = 0;
}


//Ŀǰû����
void TIMER2IRQ_Service( void )
{
	HT_TMR2->TMRIF = 0;
}


//Ŀǰû����
void TIMER3IRQ_Service( void )
{
	HT_TMR3->TMRIF = 0;
}


//-----------------------------------------------
//��������: ʱ���жϷ���������ڲ����롢�֡�ʱ�����־
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   ��Ҫ�����жϣ��ô�ѭ���ã��Ժ�Ҫ�ģ�����������
//-----------------------------------------------
void RTCIRQ_Service( void )
{
	#if( RTC_CHIP_TYPE == SOC_CLOCK )
	HT_RTC->RTCIF = 0;
	#endif
}


//-----------------------------------------------
//��������: PMU�жϴ�����
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void PMUIRQ_Service( void )
{
	//���������� �ϵ��ж��ڵ͹��Ĵ�ѭ������
	HT_PMU->PMUIF = 0x0000;
}

//Ŀǰû����
void TIMER4IRQ_Service( void )
{
	HT_TMR4->TMRIF = 0;
}


//-----------------------------------------------
//��������: ��ʱ��������
//
//����:		��
//
//����ֵ: 	��
//
//��ע:��Ҫ��ʱ���������ʱ��
//-----------------------------------------------
void TIMER5IRQ_Service( void )
{
	HT_TMR5->TMRIF = 0;
}


//Ŀǰû����
void UART6IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x8080;									//���жϱ�־
}


//Ŀǰû����
void EXTI7IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0101;									//���жϱ�־
}


//Ŀǰû����
void EXTI8IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0202;									//���жϱ�־
}


//Ŀǰû����
void EXTI9IRQ_Service( void )
{
	HT_INT->EXTIF2 &= ~0x0404;									//���жϱ�־
}
#endif//#if (BOARD_TYPE == BOARD_HT_SINGLE_78202201)

