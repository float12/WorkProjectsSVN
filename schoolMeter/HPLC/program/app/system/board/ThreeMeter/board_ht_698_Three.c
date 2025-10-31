//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		֧��698.45��Լ�ĵ����ӡ�ư������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "HT_LowPower_Three.h"

#if( ( BOARD_TYPE == BOARD_HT_THREE_0134566 ) || (BOARD_TYPE == BOARD_HT_THREE_0131699) \
 || (BOARD_TYPE == BOARD_HT_THREE_0130347) || (BOARD_TYPE == BOARD_HT_THREE_0132515) )
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

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

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
DWORD LowPowerSecCount;//�͹����ۼ�����

// ����ӳ��ͼ���������Ӳ�һ����
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = {
	// ��һ·RS-485
	eRS485_I,
	SCI_MCU_USART3_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable,			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���ͽ�ֹ
	// ����
	eIR,
	SCI_MCU_USART1_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable,			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���ͽ�ֹ
	// �ز���RF
	eCR,
	SCI_MCU_USART0_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable, 			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���ͽ�ֹ
	//�ڶ�·485
	eRS485_II,
	SCI_MCU_USART2_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable, 			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���ͽ�ֹ
};


//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�ϵ���������
//------------------------------------------------------------------------------------------------------------------------------------------------------
const GPIO_InitTypeDef GPIO_PowerOn[GPIO_NUM] = 
{
  //GPIOA��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PA0	PLCEVE		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		��ͨ�˿� ��� ��© ��ʼΪ�� �͵�ƽʹ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA3	PLCSTA		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PA4	/CV7038		��ͨ�˿� ��� ���� ��ʼΪ��  ����оƬ��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA5	IR_W		��ͨ�˿� ���� ���� ���⻽�Ѽ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA6	JTAG		��ͨ�˿� ���� ���� �Ź���˵��������� �������Ϊ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		��ͨ�˿� ���� ���� ���ϸǼ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	��ͨ�˿� ���� ���� ��β�Ǽ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		��ͨ�˿� ��� ���� ��ʼΪ�� ����д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		��ͨ�˿� ���� ���� �Ϸ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	��ͨ�˿� ���� ���� �·������
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA12	VIN_LC		���ܶ˿� �ڶ����� ���� ���� ͣ�糭���ؼ��
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA13	BAT_LC		���ܶ˿� �ڶ����� ���� ���� ʱ�ӵ�ؼ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	��ͨ�˿� ��� ���� �ڶ�·485������� ��ʼΪ��  �ߵ�ƽʹ�ܽ���
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PB1	YK			��ͨ�˿� ��� ���� �����̵��� ��ʼΪ��  �ߵ�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PB2	CV3.3V		��ͨ�˿� ��� ���� ����ȵ�Դ���� ��ʼΪ��  �͵�ƽ�򿪵�Դ
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB3	/CVBL		��ͨ�˿� ��� ���� ������� ��ʼΪ��  �ߵ�ƽ��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB7	SCLK		���ܶ˿� �ڶ����� (HT6025K����) ��� ����
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB8	MISO		���ܶ˿� �ڶ����� (HT6025K����) ���� ����
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB9	MOSI		���ܶ˿� �ڶ����� (HT6025K����) ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	��ͨ�˿� ��� ���� FLASHƬѡ ��ʼΪ�� �͵�ƽѡ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PB12	/RST_IC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB13	MTMS		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB15	MTCK		���ܶ˿� ��һ���� ��� ����
  
  //GPIOC��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	IR_T		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	IR_R		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC3	PLCTX		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC4	MOSI		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC5	MISO		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC6	SPI_CLK		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		��ͨ�˿� ��� ���� �ɼ�оƬCS ��ʼΪ�� �͵�ƽѡ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC8	SEC			���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC9	RELAYOFF	��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC10	RELAYON		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC11	TX485_2		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX485_2		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PC13	LCD_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PC14	LCD_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD0	EWP2		��ͨ�˿� ��� ���� ��һƬEEPROMд���� ��ʼΪ�� �ߵ�ƽʹ��д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD1	ESDA		��ͨ�˿� ��� ��© ��ʼΪ�� EEPROM I2C
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD2	ESCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD3	EWP1		��ͨ�˿� ��� ���� �ڶ�ƬEEPROMд���� ��ʼΪ�� �ߵ�ƽʹ��д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD5	/CVEPROM	��ͨ�˿� ��� ���� ��ʼΪ�� �洢оƬ��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD6	CEKHALL		��ͨ�˿� ���� ���� ǿ�ż��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PD9	AUX/THZJC	��ͨ�˿� ���� ���� �̵������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD10	/CVIR		��ͨ�˿� ��� ���� �����Դ���� ��ʼΪ�� �͵�ƽ�򿪵�Դ
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD11	/CVESAM		��ͨ�˿� ��� ���� ��ʼΪ�� ESAM��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		��ͨ�˿� ��� ���� ��բ�� ��ʼΪ�� �͵�ƽ����
   #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
   {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD13	VOPTM		��ͨ�˿� ��� ���� ��ʼΪ�� ʱ�ӵ�ؿ��Ǽ����Ƶ�Դ
   #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	VOPTM		��ͨ�˿� ��� ���� ��ʼΪ�� ģ�鷢�͵�Դ
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD14	ESAM_CS		��ͨ�˿� ��� ���� ESAMƬѡ ��ʼΪ�� �͵�ƽѡ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		��ͨ�˿� ��� ���� ��ʼΪ��  Һ����Դ���� �ߵ�ƽ����
  
  //GPIOE��������
  #if( PREPAY_MODE == PREPAY_LOCAL )
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		����:  ��ͨ�˿� ��� ��© ��ʼΪ�� ������ PWMģʽ
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE1	IC_TXD		����:  ���ܶ˿� ��һ���� ��� ���� �����ڷ���
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE2	IC_RXD		����:  ���ܶ˿� ��һ���� ���� ���� �����ڽ���
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		����:  ��ͨ�˿� ��� ��© ��ʼΪ�� �ڵ���ʱ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX485_1		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX485_1		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	��ͨ�˿� ��� ���� ��һ·485���� ��ʼΪ�� �ߵ�ƽ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE8	IC_KEY		����:  ��ͨ�˿� ���� ����
  #else
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		Զ�̱�: ��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		Զ�̱�: ��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		Զ�̱�: ��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		Զ�̱�: ��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE4	RX485_1		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PE5	TX485_1		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	��ͨ�˿� ��� ���� ��һ·485���� ��ʼΪ�� �ߵ�ƽ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		Զ�̱�: ��ͨ�˿� ��� ��© ��ʼΪ��
  #endif
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
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PLCEVE		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCSTA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA4	/CV7038		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	IR_W		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA6	JTAG		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		��һ���� ���� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA12	VIN_LC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA13	BAT_LC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB1	YK			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB2	CV3.3V		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB3	/CVBL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB7	SCLK		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB8	MISO		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB9	MOSI		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB12	/RST_IC		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB13	MTMS		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB15	MTCK		��ͨ�˿� ��� ���� ��ʼΪ��
  
  //GPIOC��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC0	IR_T		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC1	IR_R		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC2	PLCRX		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC3	PLCTX		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC4	MOSI		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC5	MISO		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC6	SPI_CLK		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC8	SEC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC9	RELAYOFF	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC10	RELAYON		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC11	TX485_2		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC12	RX485_2		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	LCD_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	LCD_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD0	EWP2		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD1	ESDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD2	ESCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD3	EWP1		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD5	/CVEPROM	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD6	CEKHALL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD9	AUX/THZJC	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD10	/CVIR		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD11	/CVESAM		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		��ͨ�˿� ��� ��© ��ʼΪ��
  #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  #else
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT,   eGPIO_MODE_OD,	   eGPIO_OUTPUT_HIGH},	 //PD13  NC		  ��ͨ�˿� ��� ��© ��ʼΪ��
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD14	ESAM_CS		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		��ͨ�˿� ��� ��© ��ʼΪ��
  
  //GPIOE��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	RX485_1		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	TX485_1		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE7	LVDIN		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		��ͨ�˿� ��� ��© ��ʼΪ��
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
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA0	PLCEVE		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA1	PLCRST		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA2	PLCSET		��ͨ�˿� ��� ��© ��ʼΪ�� �͵�ƽʹ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA3	PLCSTA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PA4	/CV7038		��ͨ�˿� ��� ���� ��ʼΪ�� ����оƬ��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA5	IR_W		��ͨ�˿� ��� ��© ��ʼΪ�� �⻽�Ѽ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA6	JTAG		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA7	COVER		��ͨ�˿� ���� ���� ���ϸǼ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA8	/TERMINAL	��ͨ�˿� ���� ���� ��β�Ǽ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA9	EWP3		��ͨ�˿� ��� ��© ��ʼΪ�� ����д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA10	UP_KEY		��ͨ�˿� ���� ���� �Ϸ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PA11	DOWN_KEY	��ͨ�˿� ���� ���� �·������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA12	VIN_LC		��ͨ�˿� ��� ��© ��ʼΪ�� ͣ�糭���ؼ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PA13	BAT_LC		��ͨ�˿� ��� ��© ��ʼΪ�� ʱ�ӵ�ؼ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA14				�޴�����
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PA15				�޴�����
  
  //GPIOB��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB0	DIR485_2	��ͨ�˿� ��� ��© ��ʼΪ�� �ڶ�·485������� �ߵ�ƽʹ�ܽ���
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB1	YK			��ͨ�˿� ��� ��© ��ʼΪ�� �����̵��� �ߵ�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB2	CV3.3V		��ͨ�˿� ��� ��© ��ʼΪ�� ����ȵ�Դ���� �͵�ƽ�򿪵�Դ
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT, eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB3   /CVBL 	  ��ͨ�˿� ��� ���� ��ʼΪ�� ������� �ߵ�ƽ��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB4	RTC_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB5	RTC_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB6	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB7	SCLK		���ܶ˿� �ڶ����� (HT6025K����) ��� ����
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PB8	MISO		���ܶ˿� �ڶ����� (HT6025K����) ���� ����
  {eGPIO_TYPE_AF_SECOND, eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PB9	MOSI		���ܶ˿� �ڶ����� (HT6025K����) ��� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB10	FLASH_CS	��ͨ�˿� ��� ���� ��ʼΪ�� FLASHƬѡ �͵�ƽѡ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB11	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB12	/RST_IC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB13	MTMS		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PB14	NC			��ͨ�˿� ��� ��© ��ʼΪ�� ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PB15	MTCK		��ͨ�˿� ��� ���� ��ʼΪ��
  
  //GPIOC��������
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_NONE},	//PC0	IR_T		���ܶ˿� ��һ���� ��� ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC1	IR_R		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC2	PLCRX		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC3	PLCTX		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC4	MOSI		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC5	MISO		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PC6	SPI_CLK		��ͨ�˿� ��� ���� ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PC7	/CS7038		��ͨ�˿� ��� ���� ��ʼΪ�� �ɼ�оƬCS �͵�ƽѡ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC8	SEC			��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC9	RELAYOFF	��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC10	RELAYON		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC11	TX485_2		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PC12	RX485_2		��ͨ�˿� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC13	LCD_SCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PC14	LCD_SDA		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_NONE, 	 eGPIO_DIRECTION_NONE, 	eGPIO_MODE_NONE, 	 eGPIO_OUTPUT_NONE},	//PC15				�޴�����
  
  //GPIOD��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD0	EWP2		��ͨ�˿� ��� ���� ��ʼΪ�� ��һƬEEPROMд���� �ߵ�ƽʹ��д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD1	ESDA		��ͨ�˿� ��� ��© ��ʼΪ�� EEPROM I2C
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD2	ESCL		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_LOW},		//PD3	EWP1		��ͨ�˿� ��� ���� ��ʼΪ�� �ڶ�ƬEEPROMд���� �ߵ�ƽʹ��д����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD4	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD5	/CVEPROM	��ͨ�˿� ��� ���� ��ʼΪ�� �洢оƬ��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD6	CEKHALL		��ͨ�˿� ��� ��© ��ʼΪ�� ǿ�ż��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD7	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD8	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PD9	AUX/THZJC	��ͨ�˿� ��� ��© ��ʼΪ�� �̵������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD10	/CVIR		��ͨ�˿� ��� ���� ��ʼΪ�� �����Դ���� �͵�ƽ�򿪵�Դ
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD11	/CVESAM		��ͨ�˿� ��� ���� ��ʼΪ�� ESAM��Դ���� �͵�ƽ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD12	/LED_T		��ͨ�˿� ��� ���� ��ʼΪ�� ��բ�� �͵�ƽ����
   #if(BOARD_TYPE == BOARD_HT_THREE_0132515)
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD13	NC			��ͨ�˿� ��� ��© ��ʼΪ��
  #else
  {eGPIO_TYPE_COMMON,	   eGPIO_DIRECTION_OUT,   eGPIO_MODE_OD,	   eGPIO_OUTPUT_HIGH},	 //PD13  NC		  ��ͨ�˿� ��� ��© ��ʼΪ��
  #endif
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_LOW},		//PD14	ESAM_CS		��ͨ�˿� ��� ��© ��ʼΪ�� ESAMƬѡ �͵�ƽѡ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_PP, 		 eGPIO_OUTPUT_HIGH},	//PD15	CVLCD		��ͨ�˿� ��� ���� ��ʼΪ�� Һ����Դ���� �ߵ�ƽ����
  
  //GPIOE��������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE0	BEEP		��ͨ�˿� ��� ��© ��ʼΪ�� ��Դ������
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE1	IC_TXD		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE2	IC_RXD		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE3	/CVIC		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE4	RX485_1		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE5	TX485_1		��ͨ�˿� ��� ��© ��ʼΪ��
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE6	DIR485_1	��ͨ�˿� ��� ��© ��ʼΪ�� ��һ·485���� �ߵ�ƽ����
  {eGPIO_TYPE_AF_FIRST,  eGPIO_DIRECTION_IN, 	eGPIO_MODE_FLOATING, eGPIO_OUTPUT_NONE},	//PE7	LVDIN		���ܶ˿� ��һ���� ���� ����
  {eGPIO_TYPE_COMMON, 	 eGPIO_DIRECTION_OUT, 	eGPIO_MODE_OD, 		 eGPIO_OUTPUT_HIGH},	//PE8	IC_KEY		��ͨ�˿� ��� ��© ��ʼΪ��
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
static WORD Pulse_Timer = 0;//�й������ȼ�ʱ��

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ϵͳ��Դ���
//
//����: 	
//          	Type[in]:	
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
	
	//LVDIN0
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
	
	HT_PMU->VDETCFG = 0x004D;					//�ϵ�Ƚ�VCC��ѹ3.0V BOR 2.0����λ
	HT_PMU->VDETPCFG = 0x003A;					//�Ƽ����ã�Ӱ��͹����ϵ��⣬�����������
	HT_GPIOE->PTOD  |= 0x0080;					//����©
	HT_GPIOE->PTUP  |= 0x0080;					//������
	HT_GPIOE->PTDIR &=~0x0080;					//����
		
	NVIC_DisableIRQ(PMU_IRQn);					//��ֹPMU�ж�
	HT_PMU->PMUIE   = 0x0000;					//��ֹPMU�ж�
	
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
//��ע:   
//-----------------------------------------------
static void InitExtInt(void)
{

}


//-----------------------------------------------
//��������: ��CPU��AD��������
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:  
//-----------------------------------------------
static void ADC_Config( void )
{
   	//ʹ��VCC��ADC1(ʱ�ӵ��)��ADC0(ͣ�糭����)���¶Ȳ���
	if (HT_TBS->TBSCON	!= 0x655d)		{HT_TBS->TBSCON	 = 0x655d;}
	//��ֹTBS�ж�
	if (HT_TBS->TBSIE	!= 0x0000)		{HT_TBS->TBSIE	 = 0x0000;}
	//��ص�ѹ��������=1s;�¶Ȳ�������=1/2s
	if (HT_TBS->TBSPRD	!= 0x0000)		{HT_TBS->TBSPRD	 = 0x0000;}
	//pdf��û��TBSTEST����Ĵ���˵�����Ź���˵Ĭ��д0x0200
	//TBS_ADC����=1uA;ADC_ldo��ѹ=2.7V;
	if (HT_TBS->TBSTEST	!= 0x0200)		{HT_TBS->TBSTEST = 0x0200;}
}


//-----------------------------------------------
//��������: ���ڼ��ά��cpu�ļĴ���ֵ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע: û�е��ã���Ҫ����@@@@@@   
//-----------------------------------------------
void Maintain_MCU( void )
{
	//TBS ��
	ADC_Config();
}


//-----------------------------------------------
//��������: ��ȡADת���Ľ����С����λ���ǹ̶��ģ����С���㲻���ʣ���Ҫ�������Լ�ת��
//
//����:
//				BYTE Mode[in]		��׼��ѹ�Ƿ�У׼ 0x00 У׼ 0x55��У׼----HT�޴˹���
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
            //���㹫ʽ VADCIN1 = 0.0258*ADC1DAT+4.7559 ��λmV
            if( HT_TBS->ADC0DAT & 0x8000 )//����
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->ADC0DAT;
				Voltage *= ADCINcoffConst_K;
				Voltage += ADCINOffsetConst_K;
				Voltage /= 22;
				Voltage *= 222;//3/23��ѹ
				Voltage /= 1000000L;
				
				if(abs(BatteryPara.ReadBatteryOffset) < (BYTE)(ReadBatteryStandardVoltage*15/100) )//У׼ƫ�ã��������ֵ0.5V ����5%�ĵ�����2������������10%������15%���п��� 600*0.12= 72
				{
					Voltage += BatteryPara.ReadBatteryOffset;
				}
				
				if( Voltage >= 660 )
				{
					Voltage = 660;
				}
				
				if( Voltage < 0 )
				{
					Voltage = 0;
				}
            }           
            break;
        case SYS_CLOCK_VBAT_AD://ʱ�ӵ�ص�ѹ ������λС��
            //���㹫ʽ VADCIN1 = 0.0258*ADC1DAT+4.7559 ��λmV
            if( HT_TBS->ADC1DAT & 0x8000 )//����
            {
                Voltage = 0;
            }
            else
            {
                Voltage = HT_TBS->ADC1DAT;
				Voltage *= ADCINcoffConst_K;
				Voltage += ADCINOffsetConst_K;
				if(( SelThreeBoard != BOARD_HT_THREE_0130347 )&&( SelThreeBoard != BOARD_HT_THREE_0132515 ))	
				{
					Voltage /= 47;
					Voltage *= 247;//47/247��ѹ
				}
				else
				{
					Voltage /= 10;//Voltage /= 68;
					Voltage *= 57;//68/338��ѹ//Voltage *= 338;//68/338��ѹ	
				}
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
		//�򿪵�Դ
		POWER_FLASH_OPEN;
		POWER_SAMPLE_OPEN;
		POWER_HONGWAI_REC_OPEN;
		POWER_HONGWAI_TXD_OPEN;
		POWER_LCD_OPEN;
		POWER_HALL_OPEN;
		POWER_ESAM_OPEN;
	}
}

//--------------------------------------------------
//��������:  �뵥�ຯ����Ӧ��������SPI���ã��޷��ر�esam��Դ���պ���
//         
//����  : ��
//
//����ֵ:    
//         
//��ע����:  �պ��� ������
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE PowerMode )
{

	//PC4           MOSI                //��ͨ�˿� ��� ���� ��ʼΪ��
	//PC5           MISO                //��ͨ�˿� ��� ���� ��ʼΪ��
	//PC6           SPI_CLK             //��ͨ�˿� ��� ���� ��ʼΪ��
	//PD14          SPI_CS              //��ͨ�˿� ��� ���� ��ʼΪ��
	//�˿ڷ���
	HT_GPIOC->IOCFG &=  0xff8f;

	HT_GPIOC->PTDIR |=  GPIO_Pin_4|
						GPIO_Pin_5|	
	                    GPIO_Pin_6;
	//����
	HT_GPIOC->PTOD |=   GPIO_Pin_4|
						GPIO_Pin_5|
	                    GPIO_Pin_6;
	                    
	//��ʼΪ��
	HT_GPIOC->PTCLR |=  GPIO_Pin_4|
						GPIO_Pin_5|
	                    GPIO_Pin_6;

	HT_GPIOD->IOCFG  &= 0xBFFF;	//�ر�CS
	HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
	HT_GPIOD->PTOD   |=	GPIO_Pin_14;
	HT_GPIOD->PTCLR  |= GPIO_Pin_14;

	POWER_ESAM_CLOSE;//���´򿪵�Դ
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
	
	if( Type == eCOMPONENT_SPI_ESAM )
	{
		//PC4           MOSI            	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PC5           MISO            	//��ͨ�˿� ���� ��ֹ����
		//PC6           SPI_CLK         	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PC7           SPI_CS          	//��ͨ�˿� ��� ���� ��ʼΪ��
		//�˿ڷ���
		HT_GPIOC->IOCFG &= 	0xff8f;
		
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5); 
		
		HT_GPIOC->PTDIR |=	GPIO_Pin_4|
							GPIO_Pin_6;
		//����
		HT_GPIOC->PTOD |=	GPIO_Pin_4|
							GPIO_Pin_6;
							
		HT_GPIOC->PTUP |=   GPIO_Pin_5;	
		
		//��ʼΪ��
		HT_GPIOC->PTCLR |= 	GPIO_Pin_4|
							GPIO_Pin_6;
	}
	else
	{
		//PB9           MOSI            	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PB8           MISO            	//��ͨ�˿� ���� ��ֹ����
		//PB7           SPI_CLK         	//��ͨ�˿� ��� ���� ��ʼΪ��
		
		//�˿ڷ���
		HT_GPIOB->IOCFG &= 	0xfc7f;
		
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8); 
		
		HT_GPIOB->PTDIR |=	GPIO_Pin_7|
							GPIO_Pin_9;
		//����
		HT_GPIOB->PTOD |=	GPIO_Pin_7|
							GPIO_Pin_9;
							
		HT_GPIOB->PTUP |=   GPIO_Pin_8;	
		
		//��ʼΪ��
		HT_GPIOB->PTCLR |= 	GPIO_Pin_7|
							GPIO_Pin_9;
	}
	
	if( Type == eCOMPONENT_SPI_ESAM ) //pd14  �ر�ESAM CS
	{
		HT_GPIOD->IOCFG  &= 0xbfff;	//�ر�CS
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		
		POWER_ESAM_CLOSE;//���´򿪵�Դ
		api_Delayms(20);
		POWER_ESAM_OPEN;
		api_Delayms(Time);
		
		HT_GPIOD->IOCFG  &= 0xbfff; //��������CS   
		HT_GPIOD->PTDIR  |=	GPIO_Pin_14;
		HT_GPIOD->PTOD   |=	GPIO_Pin_14;
		HT_GPIOD->PTCLR  |= GPIO_Pin_14;
		//��ʼΪ��
	    HT_GPIOD->PTSET	|= GPIO_Pin_14;
	      
		HT_GPIOD->PTUP	|= 0x4000;
	}
	else if( Type == eCOMPONENT_SPI_FLASH ) //pb10 �ر�flash CS
	{
		HT_GPIOB->IOCFG &= 0xfbff;	//�ر�CS
		HT_GPIOB->PTDIR |= GPIO_Pin_10;
		HT_GPIOB->PTOD |= GPIO_Pin_10;
		HT_GPIOB->PTCLR |= GPIO_Pin_10;
		
		POWER_FLASH_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		
		HT_GPIOB->IOCFG  &= 0xfbff; //��������CS   
		HT_GPIOB->PTDIR  |=	GPIO_Pin_10;
		HT_GPIOB->PTOD   |=	GPIO_Pin_10;
		HT_GPIOB->PTCLR  |= GPIO_Pin_10;
		//��ʼΪ��
	    HT_GPIOB->PTSET	|=	GPIO_Pin_10;
	      
		HT_GPIOB->PTUP   |=0x0400;
	}
	else //pc7 �رռ���оƬ CS
	{
		HT_GPIOC->IOCFG  &= 0xff7f;	//�ر�CS
		HT_GPIOC->PTDIR |= GPIO_Pin_7;
		HT_GPIOC->PTOD |= GPIO_Pin_7;
		HT_GPIOC->PTCLR |= GPIO_Pin_7;
		
		POWER_SAMPLE_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		
		HT_GPIOC->IOCFG  &= 0xff7f; //��������CS   
		HT_GPIOC->PTDIR |= GPIO_Pin_7;
		HT_GPIOC->PTOD |= GPIO_Pin_7;
		HT_GPIOC->PTCLR |= GPIO_Pin_7;
		//��ʼΪ��
	    HT_GPIOC->PTSET	|= GPIO_Pin_7;
		HT_GPIOC->PTUP |=0x0080;
	}

	if( Type == eCOMPONENT_SPI_ESAM )
	{
		//PC4           MOSI            	//���ܶ˿�
		//PC5           MISO            	//���ܶ˿� ���� ��ֹ����
		//PC6           SPI_CLK         	//���ܶ˿�
		//PC7           SPI_CS          	//��ͨ�˿� ��� ���� ��ʼΪ��//PA11           SPI_CS          	//��ͨ�˿� ��� ���� ��ʼΪ��
		HT_GPIOC->IOCFG |= 	GPIO_Pin_4|
							GPIO_Pin_5|
							GPIO_Pin_6;   
		//�˿ڷ���
		HT_GPIOC->PTDIR &= 	(~GPIO_Pin_5);
		
		HT_GPIOC->PTDIR |= 	GPIO_Pin_4|
							GPIO_Pin_6;
		//����
		HT_GPIOC->PTOD |=	GPIO_Pin_4|
							GPIO_Pin_6;
							
		HT_GPIOC->PTUP |=   GPIO_Pin_5;
		
		api_InitSPI( eCOMPONENT_SPI_ESAM, eSPI_MODE_3 );
	}
	else
	{
		//PB9           MOSI            	//��ͨ�˿� ��� ���� ��ʼΪ��
		//PB8           MISO            	//��ͨ�˿� ���� ��ֹ����
		//PB7           SPI_CLK         	//��ͨ�˿� ��� ���� ��ʼΪ��
		
		HT_GPIOB->NEWAFEN = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		
		HT_GPIOB->IOCFG |= 	GPIO_Pin_7|
							GPIO_Pin_8|
							GPIO_Pin_9;
		
		HT_GPIOB->AFCFG=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		
		//�˿ڷ���
		HT_GPIOB->PTDIR &= 	(~GPIO_Pin_8);
		
		HT_GPIOB->PTDIR |= 	GPIO_Pin_7|
							GPIO_Pin_9;
		//����
		HT_GPIOB->PTOD |=	GPIO_Pin_7|
							GPIO_Pin_9;
							
		HT_GPIOB->PTUP |=   GPIO_Pin_8;
		
		if( Type == eCOMPONENT_SPI_SAMPLE )
		{
			api_InitSPI( eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1 );
		}
		else
		{
			api_InitSPI( eCOMPONENT_SPI_FLASH, eSPI_MODE_3 );
		}
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
	
	//�˿ڳ�ʼ��
	ResetSPIDevice( eCOMPONENT_SPI_ESAM, 100 ); //��esam���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 5 ); //�Լ���оƬ���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //��Flash���и�λ
    
    InitPort( ePowerOnMode );//������ʼ��
	
	PowerCtrl( 0 );
	
	ESAMSPIPowerDown( ePowerOnMode );//�ر�ESAM��Դ��CS��SPI�ܽ�����Ϊ��ͨ
	
	ADC_Config();
	
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
	ADC_Config(); //!!!!�ر�ע�� �˴�ADC_Config����ɾ - yxk
	
	api_WriteFreeEvent(EVENT_SYS_START, HT_PMU->RSTSTA);
	if((HT_PMU->RSTSTA&0x0005)==0x0004)//���Ź���λ��¼�쳣�¼�
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	HT_PMU->RSTSTA = 0;//�����־
	
	api_PowerUpPara();
	api_PowerUpSave();
	
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
// 			CS_SPI_SAMPLE   - ����оƬ
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
	FLASH_CS_DISABLE;
	SAMPLE_CS_DISABLE;

	if( No == CS_SPI_ESAM )
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
	BpsBak = api_GetBaudRate( 0, SciPhNum ); 

    switch(SciPhNum)
    {
        case 0:
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
        case 1:
            USARTx = HT_UART1;
			UARTx_IRQn = UART1_IRQn;
            NVIC_DisableIRQ(UART1_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART1_IRQn);           //�������״̬
            NVIC_SetPriority(UART1_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART1_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000020;               //��������ʱ��
           	#if( COMM_TYPE == COMM_NEAR )
            HT_UART1->IRCON   = 0x0000;                   //���ⲻ����
            HT_UART1->IRDUTY  = 0x0000;                   //50%ռ�ձ�
			#else
			HT_UART1->IRCON   = 0x0001;                   //�������
            HT_UART1->IRDUTY  = 0x0001;                   //25%ռ�ձ�
			#endif
            DisWr_WPREG();
            break;
		case 2:
			if( (BpsBak&0x0F) == 10 ) //115200�����ʹر��˲�����
			{
				HT_GPIOC->FILT &= (~(1 << 12));               //�ر��˲�����
			}
			else
			{
				HT_GPIOC->FILT |= (1 << 12);
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
		case 3:
			if( (BpsBak&0x0F) == 10 ) //115200�����ʹر��˲�����
			{
				HT_GPIOE->FILT &= (~(1 << 4));                //�ر��˲�����
			}
			else
			{
				HT_GPIOE->FILT |= (1 << 4);
			}
			USARTx = HT_UART3;
			UARTx_IRQn = UART3_IRQn;
            NVIC_DisableIRQ(UART3_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART3_IRQn);           //�������״̬
            NVIC_SetPriority(UART3_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART3_IRQn);                 //ʹ�ܴ����ж�
            EnWr_WPREG();
            HT_CMU->CLKCTRL1 |= 0x000080;               //��������ʱ��
            DisWr_WPREG();
            break;
		/*
		case 4:
			if( (BpsBak&0x0F) == 10 ) //115200�����ʿ����˲�����
			{
				HT_GPIOE->FILT &=(~(1<<2));					//�ر��˲�����
			}
			else
			{
				HT_GPIOE->FILT |=(1<<2);					//�ر��˲�����
			}
			USARTx = HT_UART4;
			UARTx_IRQn = UART4_IRQn;
            NVIC_DisableIRQ(UART4_IRQn);                //��ֹ�����ж�
            NVIC_ClearPendingIRQ(UART4_IRQn);           //�������״̬
            NVIC_SetPriority(UART4_IRQn, 3);            //�������ȼ�
            //NVIC_EnableIRQ(UART4_IRQn);                 //ʹ�ܴ����ж�
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
    Bps = BpsBak & 0x0F;
	if( Bps >= (sizeof(TAB_Baud) / sizeof(TAB_Baud[0])) )
	{
		Bps = 6;		//������ݴ��� Ĭ��Ϊ9600
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
	//Bps=0x40;//ǿ��żУ�飨ʵ����żУ�鶼�����ϣ�
	//Bps=0x20;//ǿ����У�飨ʵ����żУ�鶼�����ϣ�
	//Bps=0;//ǿ����У��
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
        /*
        case 4:
            ENABLE_HARD_SCI_4_RECE;
            api_Delay100us(1);// ��ʱ100us
            HT_UART4->UARTCON |= 0x000A;
            break;
        */
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
        /*
        case 4:
            HT_UART4->UARTCON &= ~0x000A;
            break;
        */
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
        /*
        case 4:
            ENABLE_HARD_SCI_4_SEND;
            api_Delay100us(1);          // ��ʱ100us
            break;
        */
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
                    case 0:
                        HT_UART0->UARTCON |= 0x0001; //����ʹ��
                        HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART0->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART0_IRQn);//���ж�
                        break;
                    case 1:
                        HT_UART1->UARTCON |= 0x0001; //����ʹ��
                        HT_UART1->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART1->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART1_IRQn);//���ж�
                        break;
                    case 2:
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
                    /*
                    case 4:
                        HT_UART4->UARTCON |= 0x0001; //����ʹ��
                        HT_UART4->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
                        HT_UART4->UARTCON |= 0x0004; //�����ж�ʹ��
                        NVIC_EnableIRQ(UART4_IRQn);//���ж�
                        break;
                    */
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
	if( Component == eCOMPONENT_SPI_ESAM )
	{
		EnWr_WPREG();
		HT_CMU->CLKCTRL0 |= 0x000004; //ʹ��SPI0ʱ��
		//0x00bX��ʵ��1.4M����
		//0x00cX��ʵ��680K����
		//0x00aX������2.8M����
		//0x009X������5.6M����
		//0x008X������11M����
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI0->SPICON = 0x00a3; //
				break;
			case eSPI_MODE_1:
				HT_SPI0->SPICON = 0x00cb; //7026��7038����ģʽ1����������ʱ���1M
				break;
			case eSPI_MODE_2:
				HT_SPI0->SPICON = 0x00a7;
				break;
			case eSPI_MODE_3:
				HT_SPI0->SPICON = 0x00af;
				break;
			default:
				HT_SPI0->SPICON = 0x00af;
				break;
		}

		HT_SPI0->SPISTA = 0x0000; //״̬����
		DisWr_WPREG();
	}
	else
	{
		EnWr_WPREG();
		HT_CMU->CLKCTRL1 |= 0x020000; //ʹ��SPI3ʱ��
		//0x00bX��ʵ��1.4M����
		//0x00cX��ʵ��680K����
		//0x00aX������2.8M����
		//0x009X������5.6M����
		//0x008X������11M����
		switch( Type )
		{
			case eSPI_MODE_0:
				HT_SPI3->SPICON = 0x00a3; //
				break;
			case eSPI_MODE_1:
				HT_SPI3->SPICON = 0x00cb; //7026��7038����ģʽ1����������ʱ���1M
				break;
			case eSPI_MODE_2:
				HT_SPI3->SPICON = 0x00a7;
				break;
			case eSPI_MODE_3:
				HT_SPI3->SPICON = 0x00af;
				break;
			default:
				HT_SPI3->SPICON = 0x00af;
				break;
		}

		HT_SPI3->SPISTA = 0x0000; //״̬����
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
	if( Component == eCOMPONENT_SPI_ESAM )
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
//����:	    Type[in]		�๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
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
//����:	    Type[in]		�๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
//          
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_MultiFunPortCtrl( BYTE Type )
{	
	//û��5v�������κβ�����ֱ���˳�
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		return;
	}

    if( GlobalVariable.g_byMultiFunPortType != Type )
	{
        return;
	}
    
	if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
    {
        GlobalVariable.g_byMultiFunPortCounter = (80/SYS_TICK_PERIOD_MS);//80ms ��systick��ʱ���
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
    api_BeepReadCard();
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


//-----------------------------------------------
//��������: �ⲿ�ж�0�������Ŀǰ��������ɼ�
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void EXTI0IRQ_Service( void )
{
	#if( SEL_CONTINUS_FRAM_WAKEUP == IR_WAKEUP_PHOTO_SWITCH )
	
	if(DeltaTestNo > (MAX_PORT_INT_NUM-1))
	{
		DeltaTestNo = 0;
	}
	WakeUpCountNew = HT_TMR4->TMRCNT;
	DeltaTest[DeltaTestNo] = WakeUpCountNew - WakeUpCountOld;
	DeltaTestNo ++;
	WakeUpCountOld = WakeUpCountNew;
	
	#endif//#if( SEL_CONTINUS_FRAM_WAKEUP == YES )
		
	HT_INT->EXTIF &= ~0x0101;									//���жϱ�־
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


//Ŀǰû����
void EXTI4IRQ_Service( void )
{
	HT_INT->EXTIF &= ~0x1010;									//���жϱ�־
}


//�͹��İ�������(�ϼ�)
void EXTI5IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x2000 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( UP_KEY_PRESSED )
			{
				api_Delay10us( 5 ); //�������� ʵ��Լ36.7ms
				if( UP_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.UpKey = 10;
					LcdKeyDetTimer.DownKey = 10;
					LcdKeyDetTimer.UpDownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x2020;									//���жϱ�־
}


//�͹��İ�������(�¼�)
void EXTI6IRQ_Service( void )
{
	if( HT_INT->EXTIF&0x4000 )
	{
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{
			if( DOWN_KEY_PRESSED )
			{
				api_Delay10us( 5 ); //�������� ʵ��Լ36.7ms
				if( DOWN_KEY_PRESSED )
				{
					IsExitLowPower = 10;
					LcdKeyDetTimer.UpKey = 10;
					LcdKeyDetTimer.DownKey = 10;
					LcdKeyDetTimer.UpDownKey = 10;
				}
			}
		}
	}
	HT_INT->EXTIF &= ~0x4040;									//���жϱ�־
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
//��ע:   
//-----------------------------------------------
void RTCIRQ_Service( void )
{
	#if( RTC_CHIP_TYPE == SOC_CLOCK )
	
	TRealTimer t;
	
	if( HT_RTC->RTCIF & 0x0001 )
	{	
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
		{

			if( LowPowerSecCount < (IR_WAKEUP_LIMIT_TIME+86400) ) //8������ 24*8*3600 ���ⳬ�޺������ۼ�
		    {
                LowPowerSecCount ++;
		    }
			
			#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )
			ProcContinueFramWakeup();

			#endif//#if( SEL_CONTINUS_FRAM_WAKEUP == YES )
			
			#if( (SEL_EVENT_LOST_ALL_V == YES) || (SEL_EVENT_POWER_ERR == YES) )

			if( g_byDownPowerTime < 100 )
			{
				g_byDownPowerTime++;
			}

			if( (g_byDownPowerTime == START_TEST_LOST_ALLV) || (g_byDownPowerTime == START_TEST_POWER_ERR) )
			{
				//�͹��Ļ����ڼ䲻�ܹر�eeprom��Դ  ����͹��Ļ��Զ��رյ�Դ��Ӧ BUG_VERSION -- 4
				if( api_GetSysStatus(eSYS_LOW_POWER_WAKEUP) == FALSE )
				{
					if( IsExitLowPower == 0 )
					{
						#if( SEL_EVENT_POWER_ERR == YES )
						if( g_byDownPowerTime == START_TEST_POWER_ERR )
						{
							IsExitLowPower = 20;
						}
						#endif
						
						#if( SEL_EVENT_LOST_ALL_V == YES )
						if( g_byDownPowerTime == START_TEST_LOST_ALLV )
						{
							IsExitLowPower = 30;
						}
						#endif
					}
				}
				else
				{
					g_byDownPowerTime -= START_TEST_POWER_ERR;
				}
			}

			#endif//#if( (SEL_EVENT_LOST_ALL_V == YES) || (SEL_EVENT_POWER_ERR == YES) )
		}
	}
	
	HT_RTC->RTCIF = 0;
	
	#endif//#if( RTC_CHIP_TYPE == SOC_CLOCK )
}


//Ŀǰû����
void TIMER4IRQ_Service( void )
{
	HT_TMR4->TMRIF = 0;
}


//Ŀǰû����
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


#endif//#if( (BOARD_TYPE == BOARD_HT_THREE_0134566) )

