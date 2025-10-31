//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		֧��698.45��Լ�ĵ����ӡ�ư������ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "F460_LowPower_Three.h"

#if( BOARD_TYPE == BOARD_HC_SEPARATETYPE  )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define GPIO_DEFAULT				0xFF

//��ƽ�ߵ�
#define HIGH						1
#define LOW							0

#define POWER_CHECK_LOOP_COUNT		10			// 10�δ�ѭ����һ��
#define POWER_CHECK_CONTINUE_COUNT	5			// ����������
//����ʹ��       ��Ҫ
#define USART_CR1_TCIE             0x00000040 
#define USART_CR1_TXEIE            0x00000080

#define SYSTEM_POWER_VOL(vol)	((float)vol * 3.3 / 4096)

#if (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
#define SYSTEM_VOL_MIN_UP				1.5		//��Դ�ж�����
#define SYSTEM_VOL_MIN_DOWN				1.42		//��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)

#elif (HARDWARE_TYPE == THREE_PHASE_BOARD)
#define SYSTEM_VOL_MIN_UP				1.85		//��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)
#define SYSTEM_VOL_MIN_DOWN				1.77		//��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)
#endif

#define CAN_ERR_TIME_X10MS			5

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
enum 
{
    TRESET = 0U, 
    TSET = !TRESET
};
typedef enum
{
	TimeA5_IrqNo = Int002_IRQn,
	Uart1Rcv_IrqNo = Int003_IRQn,
	Uart1Snd_IrqNo = Int004_IRQn,
	Uart1Snd_Cmplt_IrqNo = Int005_IRQn,
	Uart1_Irq_Err_No = Int006_IRQn,
	Uart2Rcv_IrqNo = Int007_IRQn,
	Uart2Snd_IrqNo = Int008_IRQn,
	Uart2_Irq_Err_No = Int009_IRQn,
	Uart2Snd_Cmplt_IrqNo = Int010_IRQn,
	Uart3Rcv_IrqNo = Int011_IRQn,
	Uart3Snd_IrqNo = Int012_IRQn,
	Uart3_Irq_Err_No = Int013_IRQn,
	Uart3Snd_Cmplt_IrqNo = Int014_IRQn,
	CAN_Rx_IrqNo = Int015_IRQn,
	TopoSpiCs_IrqNo = Int016_IRQn,
	TopoSignalSend_IrqNo = Int017_IRQn,
	UART2_DMA2_IrqNo = Int018_IRQn,
	UART2_DMA2_Err_IrqNo = Int019_IRQn,
} IRQNUM;

typedef struct
{
	MCU_IO IO;
	en_pin_mode_t MODE;
} SYS_IO_Config;

typedef struct
{
	SYS_IO_Config IOConfig[2];
	M4_USART_TypeDef *USART_CH;

	IRQn_Type USART_Rcv_IRQn;
	func_ptr_t UsartRcvIrqCallback;
	uint16_t USART_Rcv_NUM;

	IRQn_Type USART_Err_IRQn;
	func_ptr_t UsartErrIrqCallback;
	uint16_t USART_Err_NUM;

	IRQn_Type USART_Snd_IRQn;
	func_ptr_t UsartSndIrqCallback;
	uint16_t USART_Snd_NUM;

	IRQn_Type USART_Snd_Cmplt_IRQn;
	func_ptr_t UsartSndCmpltIrqCallback;
	uint16_t USART_Snd_Cmplt_NUM;
} SYS_UART_Config;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum);
static BOOL SciRcvEnable(BYTE SciPhNum);
static BOOL SciRcvDisable(BYTE SciPhNum);
static BOOL SciSendEnable(BYTE SciPhNum);
static BOOL SciBeginSend(BYTE SciPhNum);
static BOOL CanInitConfig(BYTE SciPhNum);
static BOOL CanRcvEnable(BYTE SciPhNum);
static BOOL CanRcvDisable(BYTE SciPhNum);
static BOOL CanSendEnable(BYTE SciPhNum);
static BOOL CanBeginSend(BYTE SciPhNum);
static BYTE SimulateInitSci(BYTE SciPhNum);
static BYTE SimulateSciRcvEnable(BYTE SciPhNum);
static BYTE SimulateSciRcvDisable(BYTE SciPhNum);
static BYTE SimulateSciSendEnable(BYTE SciPhNum);
static BYTE SimulateSciBeginSend(BYTE SciPhNum);
void UART1_IRQ_Service(void);
void UART2_IRQ_Service(void);
void UART3_IRQ_Service(void);
void UART4_IRQ_Service(void);
void UART1_IRQ_Rec_Service(void);
void UART2_IRQ_Rec_Service(void);
void UART3_IRQ_Rec_Service(void);
void UART4_IRQ_Rec_Service(void);
void UART1_IRQ_Tx_Service(void);
void UART2_IRQ_Tx_Service(void);
void UART3_IRQ_Tx_Service(void);
void UART4_IRQ_Tx_Service(void);
void UART1_IRQ_TXCMPLT_Service(void);
void UART2_IRQ_TXCMPLT_Service(void);
void UART3_IRQ_TXCMPLT_Service(void);
void UART4_IRQ_TXCMPLT_Service(void);
void UART_IRQ_Err_Service( void);
static void TopoSpiCsIrqInitConfig(void);
static void CAN_RxIrqCallBack(void);
static void TopExtInt05_Callback(void);
static void TopoSignalSendHandle(void);
static void InitTimerA3_PWM1( void );

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE IsExitLowPower;//ģ���ڱ���
DWORD LowPowerSecCount;//�͹����ۼ�����
//volatile WORD MsTickCounter; //ms������
volatile DWORD gsysTick = 0; 
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

	// �ز���RF
	eCR,
	SCI_MCU_USART2_NUM,		// MCU ���ں�
	&InitPhSci,				// ���ڳ�ʼ��
	&SciRcvEnable, 			// ��������
	&SciRcvDisable,			// ��ֹ����
	&SciSendEnable,			// ��������
	&SciBeginSend,			// ���ͽ�ֹ
	
	// // ����
	// eBlueTooth,
	// SCI_MCU_USART1_NUM,		// MCU ���ں�
	// &InitPhSci,				// ���ڳ�ʼ��
	// &SciRcvEnable,			// ��������
	// &SciRcvDisable,			// ��ֹ����
	// &SciSendEnable,			// ��������
	// &SciBeginSend,			// ���ͽ�ֹ
	
	// ����
	// eIR,
	// SCI_MCU_USART1_NUM,		// MCU ���ں�
	// &InitPhSci,				// ���ڳ�ʼ��
	// &SciRcvEnable,			// ��������
	// &SciRcvDisable,			// ��ֹ����
	// &SciSendEnable,			// ��������
	// &SciBeginSend,			// ���ͽ�ֹ


	//CAN����
	// eCAN,
	// 0xAA,                   // MCU ���ں�
	// &CanInitConfig,
	// &CanRcvEnable,
	// &CanRcvDisable,
	// &CanSendEnable,
	// &CanBeginSend,

	// eEXP_PRO,
	// 0xFF, // MCU ���ں�
	// &SimulateInitSci,       // ���ڳ�ʼ��
	// &SimulateSciRcvEnable, 	// ��������
	// &SimulateSciRcvDisable,	// ��ֹ����
	// &SimulateSciSendEnable,	// ��������
	// &SimulateSciBeginSend,  // ���ͽ�ֹ
};
// ���±�SCI_MCU_USARTX_NUMȡ
SYS_UART_Config Uart_Config[] =
{
	//485-1  SCI_MCU_USART3_NUM
	{
		{	
			{	
				{PortE, Pin14, Func_Usart3_Rx}, {Pin_Mode_Out}, //��
			},
			{
				{PortE, Pin12, Func_Usart3_Tx}, {Pin_Mode_Out}, //��
			}
		},
		//�������к�
		{M4_USART3},
		//�жϱ��                            //�ص�����                    //�ж�Դ
		{(IRQn_Type)Uart3Rcv_IrqNo},		{&UART3_IRQ_Rec_Service},		{INT_USART3_RI},
		//���ڽ����жϱ��                     //���ڻص�����                //�����ж�Դ
		{(IRQn_Type)Uart3_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART3_EI},
		//���ڷ����жϱ��                     //���ڻص�����                //�����ж�Դ
		{(IRQn_Type)Uart3Snd_IrqNo},		{&UART3_IRQ_Tx_Service},		{INT_USART3_TI},
		//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
		{(IRQn_Type)Uart3Snd_Cmplt_IrqNo},	{&UART3_IRQ_TXCMPLT_Service},	{INT_USART3_TCI},
	},

	//ģ��ͨ�� SCI_MCU_USART2_NUM
	{
		{
			{
				{PortA, Pin15, Func_Usart2_Rx}, {Pin_Mode_Out}, //��
			},
			{
				{PortC, Pin10, Func_Usart2_Tx}, {Pin_Mode_Out}, //��
			}
		},
		//���к�
		{M4_USART2},
		//���ڽ����жϱ��                   //���ڻص�����               /�����ж�Դ
		{(IRQn_Type)Uart2Rcv_IrqNo},		{&UART2_IRQ_Rec_Service},		{INT_USART2_RI},
		//���ڽ����жϱ��                     //���ڻص�����              //�����ж�Դ
		{(IRQn_Type)Uart2_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART2_EI},
		//���ڷ����жϱ��                     //���ڻص�����               //�����ж�Դ
		{(IRQn_Type)Uart2Snd_IrqNo},		{&UART2_IRQ_Tx_Service},		{INT_USART2_TI},
		//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
		{(IRQn_Type)Uart2Snd_Cmplt_IrqNo},	{&UART2_IRQ_TXCMPLT_Service},	{INT_USART2_TCI},
	},

	//����  SCI_MCU_USART1_NUM
	{
		{
			{
				{PortA, Pin00, Func_Usart1_Rx}, {Pin_Mode_Out}, //��
			},
			{
				{PortA, Pin01, Func_Usart1_Tx}, {Pin_Mode_Out}, //��
			}
		},
		{M4_USART1},
		//�����жϱ��                        //���ڻص�����            //�����ж�Դ
		{(IRQn_Type)Uart1Rcv_IrqNo},		{&UART1_IRQ_Rec_Service},		{INT_USART1_RI},
		//���ڽ����жϱ��                     //���ڻص�����                //�����ж�Դ
		{(IRQn_Type)Uart1_Irq_Err_No},		{&UART_IRQ_Err_Service},		{INT_USART1_EI},
		//���ڷ����жϱ��                     //���ڻص�����               //�����ж�Դ
		{(IRQn_Type)Uart1Snd_IrqNo},		{&UART1_IRQ_Tx_Service},		{INT_USART1_TI},
		//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
		{(IRQn_Type)Uart1Snd_Cmplt_IrqNo},	{&UART1_IRQ_TXCMPLT_Service},	{INT_USART1_TCI},
	},
};

//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�ϵ���������
//------------------------------------------------------------------------------------------------------------------------------------------------------

// ֻ������Ҫ���õ�IO���������õ��������
const port_init_t TGPIO_Config[] =
	{
		// PORT		      PIN    FUNC      Mode           LTE      INTE    INVE      PUU     DRV         NOD             BFE      Level
		{.enPort = PortE, Pin02, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 1. PE02		¼����ʼֹͣ��ⰴť������Ϊ�͵�ƽ��ʼת������
		{.enPort = PortC, Pin00, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 9. PC00		��Դ���//��װ��������Ŵ�pa2��Ϊpc0
		{.enPort = PortB, Pin01, Func_Gpio, Pin_Mode_Ana, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 10.PB01		ʱ�ӵ�ص�ѹ���
		{.enPort = PortA, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 11.PA04		�๦�����
		{.enPort = PortB, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 12.PB06		CV8306
		{.enPort = PortB, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 13.PB07		CVFLASH
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 14.PB08		SPI_CS		��8306ͨ��  ��
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},			  // 15.PH02		SPI_CS		��FLASHͨ��  ��
		{.enPort = PortB, Pin09, Func_Spi4_Miso, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 16.PB09		SPI_MISO	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin00, Func_Spi4_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 17.PE00		SPI_MOSI	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin01, Func_Spi4_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},  // 18.PE01		SPI_SCLK	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin12, Func_Usart3_Tx, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT}, // 20.PE12		USART_3_TX 485
		{.enPort = PortE, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},				  // 21.PE13		485�������
		{.enPort = PortE, Pin14, Func_Usart3_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 22.PE14		USART_3_RX 485
		#if(HARDWARE_TYPE ==THREE_PHASE_BOARD)	
		{.enPort = PortB, Pin14, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 25.PB14		SDA   ��©���  ����̬(�����EE)
		{.enPort = PortD, Pin10, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 26.PD10		SCL   ��©���  ����̬(�����EE)
		#elif(HARDWARE_TYPE == SINGLE_PHASE_BOARD)
		{.enPort = PortD, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},			// 25.PD03		SDA   ��©��� ����̬(�����EE)
		{.enPort = PortD, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},			// 26.PD02		SCL   ��©��� ����̬(�����EE)
		#endif
		{.enPort = PortD, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 27.PD08		E2-WC2   ������� eppд����������Ч
		{.enPort = PortD, Pin09, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 28.PD09		E2-WC1   ������� eppд����������Ч
		{.enPort = PortB, Pin12, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 29.PB12		SDA   ��©���  ����̬(�����RTC)
		{.enPort = PortB, Pin13, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 30.PB13		SCL   ��©���  ����̬(�����RTC)
		{.enPort = PortA, Pin15, Func_Usart2_Rx, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 31.PA15		USART_2_RX ͨѶģ��
		{.enPort = PortC, Pin10, Func_Usart2_Tx, Pin_Mode_Out, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 32.PC10		USART_2_TX ͨѶģ��
		{.enPort = PortC, Pin11, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 33.PC11		MDM_SET ��©��� ��ʼΪ��
		{.enPort = PortC, Pin12, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Enable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 34.PC12		MDM_STA ��������
		{.enPort = PortD, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, LOW},				  // 35.PD00		MDM_EVE ��©��� ��ʼΪ��
		{.enPort = PortD, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, HIGH},				  // 36.PD01		MDM_RST ��©��� ��ʼΪ��
		{.enPort = PortB, Pin04, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 40.PB04		ͨ��ָʾ��  ������� ����Ч
		{.enPort = PortD, Pin07, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 41.PD07		TF��ת��ָʾ��  ������� ����Ч
		{.enPort = PortC, Pin05, Func_Gpio, Pin_Mode_In, Disable, Enable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		  // 42.PC05		SPI_CS	8306�����460	�ֲ��е�HSDC
		{.enPort = PortC, Pin04, Func_Spi1_Mosi, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},	  // 43.PC04		SPI_MOSI  8306�����460
		{.enPort = PortA, Pin07, Func_Spi1_Sck, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	  // 44.PA07		SPI_SCLK  8306�����460
		{.enPort = PortB, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},				  // 45.PB03		���еƣ������л�һ��
};

//SPI��λ�ܽ�����
const port_init_t TSPISetOI_Config[eCOMPONENT_TOTAL_NUM-1][4] =
{
	// eCOMPONENT_SPI_SAMPLE
	{
		{.enPort = PortB, Pin09, Func_Spi1_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 13.PB09 		SPI_MISO	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin00, Func_Spi1_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 14.PE00 		SPI_MOSI	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin01, Func_Spi1_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 15.PE01		SPI_SCLK	��8306,FLASHͨ�� ��
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 12.PB08 		SPI_CS		��8306ͨ��  ��
	},
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin09, Func_Spi1_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 13.PB09 		SPI_MISO	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin00, Func_Spi1_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 14.PE00 		SPI_MOSI	��8306,FLASHͨ�� ��
		{.enPort = PortE, Pin01, Func_Spi1_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 15.PE01		SPI_SCLK 	��8306,FLASHͨ�� ��
		{.enPort = PortH, Pin02, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 12.PH02 		SPI_CS		��FLASHͨ��  ��
	},
	//eCOMPONENT_SPI_ESAM
	// {
	// 	{.enPort = PortE, Pin04, Func_Spi3_Miso, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Od, Disable, GPIO_DEFAULT},		// 3. PE04 		SPI_MISO  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin03, Func_Spi3_Mosi, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 2. PE03 		SPI_MOSI  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin05, Func_Spi3_Sck, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, GPIO_DEFAULT},	// 4. PE05 		SPI_SCLK  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, HIGH},				// 5. PE06 		SPI_CS 	  ��ESAMͨ�� ��
	// },
};
//SPI��λ�ܽ�����
const port_init_t TSPIResetOI_Config[eCOMPONENT_TOTAL_NUM-1][4] =
{
	// eCOMPONENT_SPI_SAMPLE
	{
		{.enPort = PortB, Pin09, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 13.PB09 		SPI_MISO  ��8306ͨ�� ��
		{.enPort = PortE, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 14.PE00 		SPI_MOSI  ��8306ͨ�� ��
		{.enPort = PortE, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 15.PE01		SPI_SCLK  ��8306ͨ�� ��
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 12.PB08 		SPI_CS  ��8306ͨ��  ��
	},
	//eCOMPONENT_SPI_FLASH
	{
		{.enPort = PortB, Pin09, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 13.PB09 		SPI_MISO  ��8306ͨ�� ��
		{.enPort = PortE, Pin00, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 14.PE00 		SPI_MOSI  ��8306ͨ�� ��
		{.enPort = PortE, Pin01, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 15.PE01		SPI_SCLK  ��8306ͨ�� ��
		{.enPort = PortB, Pin08, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 12.PB08 		SPI_CS  ��FLASHͨ��  ��
	},
	//eCOMPONENT_SPI_ESAM
	// {
	// 	{.enPort = PortE, Pin03, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_M, Pin_OType_Cmos, Disable, LOW},		// 2. PE03 		SPI_MOSI  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin04, Func_Gpio, Pin_Mode_In, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 3. PE04 		SPI_MISO  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin05, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 4. PE05 		SPI_SCLK  ��ESAMͨ�� ��
	// 	{.enPort = PortE, Pin06, Func_Gpio, Pin_Mode_Out, Disable, Disable, Disable, Disable, Pin_Drv_H, Pin_OType_Cmos, Disable, LOW},		// 5. PE06 		SPI_CS 	  ��ESAMͨ�� ��
	// },
};
//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�͹�����������
//------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------------------------------------
//																	�͹��Ļ�����������
//------------------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//���ڲ��������ñ�        �ֱ��Ӧ300   /600   /1200  /2400  /4800 /7200  /9600  /19200 /38100 /57600 /115200
static const WORD TAB_Baud[]={0x8F5B,0x47AC,0x23D6,0x11EB,0x08F5,0x05F9,0x047A,0x023C,0x011E,0x00BE,0x005F};

static volatile WORD SysTickCounter;
WORD HSDCTimer = 0;
static WORD Pulse_Timer = 0;//�й������ȼ�ʱ��
BYTE CommLed_Time = 0; //ͨ�ŵ���ʱ��
BYTE CanErrTime;
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern void IrPWMEnable(BOOL enNewSta);
//api_GetSAGStatus

//-----------------------------------------------
//��������: ϵͳ��Դ���
//
//����: 	
//          	Type[in]:	
//				eOnInitDetectPowerMode		�ϵ�ϵͳ��Դ���
//				eOnRunDetectPowerMode		���������ڼ��ϵͳ��Դ���
//				eWakeupDetectPowerMode		�͹��Ļ����ڼ�ϵͳ��Դ���
//				eSleepDetectPowerMode		�͹��������ڼ�ϵͳ��Դ���
//				eOnRunSpeedDetectPowerMode	�������п���ϵͳ��Դ���

//����ֵ: 	TRUE:�е�   FALSE:û�е�
//
//��ע:   �ϵ�vcc�ͱȽ������У�����ֻ��LVDIN0�Ƚ�������Ϊvcc���г�������
//-----------------------------------------------
BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
{
	static BYTE bLoopCount = 0;
	WORD wSystemVol = 0;
	BYTE bResult = 0, i = 0;

	// �����м�⣬POWER_CHECK_LOOP_COUNT�ν�1��
	if (Type == eOnRunDetectPowerMode)
	{
		bLoopCount++;
		if ((bLoopCount % POWER_CHECK_LOOP_COUNT) == 0)
		{
			bLoopCount = 0;
		}
		else
		{
			return TRUE;
		}
	}

	if( ( Type == eOnInitDetectPowerMode ) || (Type == eSleepDetectPowerMode) )
	{
		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
		{
			wSystemVol = api_GetADData(eOther_AD_PWR);

			if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN_UP) // ϵͳ��Դ�ж��Ƿ��ϵ�
			{
				break;
			}
		}

		// �ϵ��⣬POWER_CHECK_CONTINUE_COUNT�ζ��е����Ϊ�е�
		if (i >= POWER_CHECK_CONTINUE_COUNT)
		{
			api_SetSysStatus(eSYS_STATUS_POWER_ON);
			return TRUE;
		}
		else
		{
			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
			return FALSE;
		}
	}
	else if((Type == eOnRunDetectPowerMode) || (Type == eOnRunSpeedDetectPowerMode) )
	{
		for(i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
		{
			wSystemVol = api_GetADData(eOther_AD_PWR);

			if (SYSTEM_POWER_VOL(wSystemVol) > SYSTEM_VOL_MIN_DOWN) // ϵͳ��Դ�ж��Ƿ��ϵ�
			{
				break;
			}
			
		}
		
		if( i >= POWER_CHECK_CONTINUE_COUNT )//����
		{
			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
			return FALSE;
		}
		else
		{
			api_SetSysStatus(eSYS_STATUS_POWER_ON);
			return TRUE;
		}

	}
	return TRUE;
}
////-----------------------------------------------
////��������: ϵͳ��Դ���
////
////����: 	
////          	Type[in]:	
////				eOnInitDetectPowerMode		�ϵ�ϵͳ��Դ���
////				eOnRunDetectPowerMode		���������ڼ��ϵͳ��Դ���
////				eWakeupDetectPowerMode		�͹��Ļ����ڼ�ϵͳ��Դ���
////				eSleepDetectPowerMode		�͹��������ڼ�ϵͳ��Դ���
////				eOnRunSpeedDetectPowerMode	�������п���ϵͳ��Դ���
//
////����ֵ: 	TRUE:�е�   FALSE:û�е�
////
////��ע:   �ϵ�vcc�ͱȽ������У�����ֻ��LVDIN0�Ƚ�������Ϊvcc���г�������
////-----------------------------------------------
//BOOL api_CheckSysVol( eDETECT_POWER_MODE Type )
//{
//	static BYTE bLoopCount = 0;
//	WORD wSystemVol = 0;
//	BYTE bResult = 0, i = 0;
//
//	// �����м�⣬POWER_CHECK_LOOP_COUNT�ν�1��
//	if (Type == eOnRunDetectPowerMode)
//	{
//		bLoopCount++;
//		if ((bLoopCount % POWER_CHECK_LOOP_COUNT) == 0)
//		{
//			bLoopCount = 0;
//		}
//		else
//		{
//			return TRUE;
//		}
//	}
//
//	if( Type == eOnInitDetectPowerMode )
//	{
//		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//		{
//			wSystemVol = api_GetADData(eOther_AD_PWR);
//
//			if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN) // ϵͳ��Դ�ж��Ƿ��ϵ�
//			{
//				break;
//			}
//		}
//
//		// �ϵ��⣬POWER_CHECK_CONTINUE_COUNT�ζ��е����Ϊ�е�
//		if (i >= POWER_CHECK_CONTINUE_COUNT)
//		{
//			api_SetSysStatus(eSYS_STATUS_POWER_ON);
//			return TRUE;
//		}
//		else
//		{
//			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//			return FALSE;
//		}
//	}
//	else if((Type == eOnRunDetectPowerMode) || (Type == eOnRunSpeedDetectPowerMode) )
//	{
//		if( api_GetSAGStatus() == TRUE )//��ֹ��ѭ������ �е�ֱ���˳�
//		{
//			return TRUE;
//		}
//		else
//		{	
//			for(i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//			{
//				if( Type == eOnRunDetectPowerMode )
//				{
//					api_Delayms( 1 );
//				}
//				else//���ټ��
//				{
//					api_Delay100us( 2 );
//				}
//				
//				if( api_GetSAGStatus() == TRUE )
//				{
//					break;
//				}
//			}
//			
//			if( i >= POWER_CHECK_CONTINUE_COUNT )//����
//			{
//				api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//				return FALSE;
//			}
//			else
//			{
//				api_SetSysStatus(eSYS_STATUS_POWER_ON);
//				return TRUE;
//			}
//		}
//
//	}
//	else if(Type == eSleepDetectPowerMode)	//SAG��12Vͬʱ��Ч
//	{
//		for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
//		{
//			wSystemVol = api_GetADData(eOther_AD_PWR);
//
//			if( (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN) 
//			|| ( api_GetSAGStatus() != TRUE ) )// ϵͳ��Դ�ж��Ƿ��ϵ�
//			{
//				break;
//			}
//		}
//
//		// �ϵ��⣬POWER_CHECK_CONTINUE_COUNT�ζ��е����Ϊ�е�
//		if (i >= POWER_CHECK_CONTINUE_COUNT)
//		{
//			api_SetSysStatus(eSYS_STATUS_POWER_ON);
//			return TRUE;
//		}
//		else
//		{
//			api_ClrSysStatus(eSYS_STATUS_POWER_ON);
//			return FALSE;
//		}		
//	}
//	return TRUE;
//}

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
	BYTE i;
	
	if(Type == ePowerOnMode)
	{
		PORT_DebugPortSetting(TDO_SWO, Disable);
		PORT_DebugPortSetting(TRST, Disable);
		PORT_DebugPortSetting(TDI, Disable);

		#if(SPD_HDSC_MCU == SPD_96000K)
		PORT_PCCRRDWTSetting(WaitCycle2);	//	84M~128M�ȴ�2�����ڶ�ȡʱ��
		#endif
//		#if( SEL_DEBUG_VERSION == YES )
//		DWORD pTGPIO_Config;
//		scanf("%x",&pTGPIO_Config);
//		if(memcmp((const void *)pTGPIO_Config, TGPIO_Config, sizeof(TGPIO_Config)) != 0)   // BOOT���ò�һ��
//		{
//			ASSERT(FALSE, 1);
//		}
//		else
//		{
//			/* code */
//		}
//		#endif
		for(i = 0; i < (sizeof(TGPIO_Config) / sizeof(port_init_t)); i++)
		{
			if(TGPIO_Config[i].FuncReg != GPIO_DEFAULT)
			{
				PORT_Init(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,&TGPIO_Config[i].GpioReg);
				if(TGPIO_Config[i].GpioReg.enPinMode == Pin_Mode_Out)	//�������״̬
				{
					if(TGPIO_Config[i].bLevel == HIGH)
					{
						PORT_SetPortData(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else if(TGPIO_Config[i].bLevel == LOW)
					{
						PORT_ResetPortData(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin);
					}
					else
					{
						
					}
				}	
				if(TGPIO_Config[i].FuncReg != Func_Gpio)
				{
					PORT_SetFunc(TGPIO_Config[i].enPort,TGPIO_Config[i].u16Pin,(en_port_func_t)TGPIO_Config[i].FuncReg,Disable);
				}
			}
		}
	}
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
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);	//����Ϊ2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

//-----------------------------------------------
//��������: 1ms��ʱ�ж�����
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
//static void InitTimer(void)
//{
//	stc_timera_base_init_t stcTimeraInit;
//    stc_irq_regi_conf_t stcIrqRegiConf;
//
//    MEM_ZERO_STRUCT(stcTimeraInit);
//    MEM_ZERO_STRUCT(stcIrqRegiConf);
//
//    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA5, Enable);
//
//    stcTimeraInit.enClkDiv = TimeraPclkDiv8;		//PCLK1ʱ��Ϊ12M
//    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
//    stcTimeraInit.enCntDir = TimeraCountDirUp;
//    stcTimeraInit.enSyncStartupEn = Disable;
//    
//	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
//	stcTimeraInit.u16PeriodVal = 0x05dcu;        			//1.5M  1ms
//	#elif(SPD_HDSC_MCU == SPD_72000K)
//	stcTimeraInit.u16PeriodVal = 0x08cau;        			//2.25M 1ms
//	#endif
//    TIMERA_BaseInit(M4_TMRA5, &stcTimeraInit);
//    TIMERA_IrqCmd(M4_TMRA5, TimeraIrqOverflow, Enable);
//
//    /* Configure interrupt of timera unit 1 */
//    stcIrqRegiConf.enIntSrc = INT_TMRA5_OVF;
//    stcIrqRegiConf.enIRQn = (IRQn_Type)TimeA5_IrqNo;
//    stcIrqRegiConf.pfnCallback = &TimerA5_Handler;
//    enIrqRegistration(&stcIrqRegiConf);
//    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
//
//    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
//
//    TIMERA_Cmd(M4_TMRA5, Enable);
//}

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
	WORD wTemp;
	float fTemp;

	if( Type != SYS_CLOCK_VBAT_AD )
	{
		return 0;
	}
	wTemp = api_GetADData(eOther_AD_BAT);
	fTemp = SYSTEM_POWER_VOL(wTemp);
	wTemp = (WORD)(fTemp * 200); //תΪʵ�ʵ�ѹ��2λС��
	if (wTemp < 243)
	{
		return (short)wTemp;
	}
    if(abs(BatteryPara.ClockBatteryOffset) < (BYTE)(ClockBatteryStandardVoltage*15/100) )//У׼ƫ�ã��������ֵ0.5V ����5%�ĵ�����2������������10%������15%���п��� 364*0.12= 43
	{
		wTemp += BatteryPara.ClockBatteryOffset;
	}
	if( wTemp > 368 )
	{
		wTemp = 368;
	}
	return (short)wTemp;
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
		//POWER_EEPROM_OPEN;
		POWER_BLUETOOTH_OPEN;
		POWER_FLASH_OPEN;
		if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == TRUE )
		{
			POWER_SAMPLE_OPEN;
			POWER_ESAM_OPEN;
		}
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
	BYTE i;
	
	if( Type >= eCOMPONENT_TOTAL_NUM )
	{
		return FALSE;
	}

	for(i = 0; i < (sizeof(TSPIResetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if(TSPIResetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{
			PORT_SetFunc(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin,(en_port_func_t)TSPIResetOI_Config[Type][i].FuncReg,Disable);
			PORT_Init(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin,&TSPIResetOI_Config[Type][i].GpioReg);
			if(TSPIResetOI_Config[Type][i].GpioReg.enPinMode == Pin_Mode_Out)	//�������״̬
			{
				if(TSPIResetOI_Config[Type][i].bLevel == HIGH)
				{
					PORT_SetPortData(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin);
				}
				else if(TSPIResetOI_Config[Type][i].bLevel == LOW)
				{
					PORT_ResetPortData(TSPIResetOI_Config[Type][i].enPort,TSPIResetOI_Config[Type][i].u16Pin);
				}
				else
				{
					
				}
			}	
		
		}
	}
	
//	if( Type == eCOMPONENT_SPI_ESAM ) //  �ر�ESAM CS
//	{
//		//ESAM_CS_DISABLE;
//		POWER_ESAM_CLOSE;//���´򿪵�Դ
//		#if(SEL_ESAM_TYPE != 0)
//		api_Delayms(20);
//		POWER_ESAM_OPEN;
//		api_Delayms(Time);
//		//ESAM_CS_ENABLE;
//		#endif
//	}
	if( Type == eCOMPONENT_SPI_FLASH ) // �ر�flash CS
	{
		//FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		//FLASH_CS_ENABLE;
	}
	else if( Type == eCOMPONENT_SPI_SAMPLE )// �رռ���оƬ CS
	{

		//SAMPLE_CS_DISABLE;
		POWER_SAMPLE_CLOSE;//���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE_OPEN;
		api_Delayms(Time);
		//SAMPLE_CS_ENABLE;
	}
	
	for(i = 0; i < (sizeof(TSPISetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if(TSPISetOI_Config[Type][i].FuncReg != GPIO_DEFAULT)
		{

			PORT_SetFunc(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin,(en_port_func_t)TSPISetOI_Config[Type][i].FuncReg,Disable);
			PORT_Init(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin,&TSPISetOI_Config[Type][i].GpioReg);
			if(TSPISetOI_Config[Type][i].GpioReg.enPinMode == Pin_Mode_Out)	//�������״̬
			{
				if(TSPISetOI_Config[Type][i].bLevel == HIGH)
				{
					PORT_SetPortData(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin);
				}
				else if(TSPISetOI_Config[Type][i].bLevel == LOW)
				{
					PORT_ResetPortData(TSPISetOI_Config[Type][i].enPort,TSPISetOI_Config[Type][i].u16Pin);
				}
				else
				{
					
				}
			}	

		}
	}	
	
	if (Type == eCOMPONENT_SPI_FLASH)
	{
		api_InitSPI(eCOMPONENT_SPI_FLASH, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_SAMPLE)
	{
		api_InitSPI(eCOMPONENT_SPI_SAMPLE, eSPI_MODE_1);
	}
//	else if (Type == eCOMPONENT_SPI_ESAM)
//	{
//		#if(SEL_ESAM_TYPE != 0)
//		api_InitSPI(eCOMPONENT_SPI_ESAM, eSPI_MODE_3);
//		#endif
//	}
	else
	{
		return FALSE;
	}

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

	// initTestPin();//��ʱ������

	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
    
    CLEAR_WATCH_DOG;
	
	
	//�˿ڳ�ʼ��
	// ResetSPIDevice(eCOMPONENT_SPI_ESAM, 100);	// ��esam���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_SAMPLE, 200 ); //�Լ���оƬ���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //��Flash���и�λ
    
    InitPort( ePowerOnMode );//������ʼ��

	api_TopWaveSpiInit();//����΢�ظ������ȳ�ʼ��Ƭѡ�ٳ�ʼ��SPI��DMA��Ƭѡ��InitPort�г�ʼ�������Է���InitPort�±�

	
	PowerCtrl( 0 );
	
	// ESAMSPIPowerDown(ePowerOnMode); // �ر�ESAM��Դ��CS��SPI�ܽ�����Ϊ��ͨ
	
	// ADC_Config();
	
	InitINT() ;//�رղ��õ��ж�

	//��ʼ����ʱ��
	//InitTimer();
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	//����ģ���ϵ��ʼ����ע�ⲻҪ����˳��
	api_PowerUpFlag();
	//api_PowerUpRtc�ж�RTC�ĸ�λ���� ��λ�������������RTC��TBS��ؼĴ��� �����Ҫ���µ���һ��ADC����
	api_PowerUpRtc();
	//ADC_Config(); //!!!!�ر�ע�� �˴�ADC_Config����ɾ - yxk
	api_WriteFreeEvent(EVENT_SYS_START, M4_SYSREG->RMU_RSTF0);
	if ((M4_SYSREG->RMU_RSTF0_f.WDRF==1)||(M4_SYSREG->RMU_RSTF0_f.SWDRF==1))
	{
		api_WriteSysUNMsg( SYSERR_WDTRST_ERR );
	}
	
	M4_SYSREG->RMU_RSTF0_f.CLRF=1;//�����־
	
	api_PowerUpPara();
	api_PowerUpSave();
	WaveRecordInit();
	// api_PowerUpLcd(ePowerOnMode);
	
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	CLEAR_WATCH_DOG;
	
	api_PowerUpSample();
	
	api_PowerUpProtocol();
	// api_PowerUpCanBus();
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	
	// api_PowerUpPrePay();
	// PowerUpRelay();
	
	// api_PowerUpEnergy();
	// api_PowerUpFreeze();
	
	//���ϵ粹�������е��磬�������͹���
	if( api_CheckSysVol( eOnInitDetectPowerMode ) == FALSE )
	{
		api_EnterLowPower( eFromOnInitEnterDownMode );
	}
	// api_ReflashFreeze_Ram();
#if (SEL_SEARCH_METER == YES)
//	api_PowerUpSchClctMeter();
#endif

    #if (SEL_TOPOLOGY == YES )
    TopoPowerUp();
    #endif

	// api_PowerUpEvent();
	api_PowerUpSysWatch(); // ��ˢ�������ϱ�ģʽ�ֺ���е�ص�ѹ�ȵ�����ж�-��event�ϵ紦������˳���ϵ-���ĺ�
#if (MAX_PHASE == 3)
	// api_PowerUpDemand();
#endif

	// api_PowerUpVersion();

	// ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower( eFromOnRunEnterDownMode );//�ڴ�֮ǰ�ϵ��ѳ�ʼ����� �˴���⵽�͹��İ��������紦�� ���ڽ��������絫��Ϊ���쳣��λ������
	}

	// api_MidLayerInit();

#if (SEL_EVENT_DI_CHANGE == YES)
	// api_PowerUpDI();
#endif

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}

	//��ʼ��_noinit_��־
	api_InitNoInitFlag();
	
	RESET_PLC_MODUAL;
    api_Delayms(20);
	ENABLE_PLC_PROGRAMM;
	
}

void WatchDog(void)
{
	CLEAR_WATCH_DOG;
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
	if( Do == FALSE )
	{
		switch( No )
		{
			case CS_SPI_ESAM:
				ESAM_CS_DISABLE;
				break;
			case CS_SPI_FLASH:	//����SPI,ֻ��һ��CS��Ч
				FLASH_CS_DISABLE;
				break;
			case CS_SPI_SAMPLE:
				SAMPLE_CS_DISABLE;
				break;
			// case CS_SPI_CPU:
			// 	SAMPLE_HCS_DISABLE;
			// 	break;
			default:
				break;
		}
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
		// case CS_SPI_CPU:
		// 	SAMPLE_HCS_ENABLE;
		// 	break;
		default:
			break;
	}
	api_Delay10us(1);
	
	return;
}

void UART_IRQ_Err_Service( void)
{
	// UART1
	if (Set == USART_GetStatus(M4_USART1, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART1, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART1, UsartParityErr))
	{
		USART_ClearStatus(M4_USART1, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART1, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART1, UsartOverrunErr);
	}

	// UART2
	if (Set == USART_GetStatus(M4_USART2, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART2, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART2, UsartParityErr))
	{
		USART_ClearStatus(M4_USART2, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART2, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART2, UsartOverrunErr);
	}

	// UART3
	if (Set == USART_GetStatus(M4_USART3, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART3, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART3, UsartParityErr))
	{
		USART_ClearStatus(M4_USART3, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART3, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART3, UsartOverrunErr);
	}

	// UART4
	if (Set == USART_GetStatus(M4_USART4, UsartFrameErr))
	{
		USART_ClearStatus(M4_USART4, UsartFrameErr);
	}

	if (Set == USART_GetStatus(M4_USART4, UsartParityErr))
	{
		USART_ClearStatus(M4_USART4, UsartParityErr);
	}

	if (Set == USART_GetStatus(M4_USART4, UsartOverrunErr))
	{
		USART_ClearStatus(M4_USART4, UsartOverrunErr);
	}
}
// Rx
void UART1_IRQ_Rec_Service( void )
{
	UART1_IRQ_Service();
}
void UART2_IRQ_Rec_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_Rec_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_Rec_Service(void)
{
	UART4_IRQ_Service();
}
// Tx
void UART1_IRQ_Tx_Service(void)
{
	UART1_IRQ_Service();
}
void UART2_IRQ_Tx_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_Tx_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_Tx_Service(void)
{
	UART4_IRQ_Service();
}

// Tx Complete
void UART1_IRQ_TXCMPLT_Service(void)
{
	UART1_IRQ_Service();
}
void UART2_IRQ_TXCMPLT_Service(void)
{
	UART2_IRQ_Service();
}
void UART3_IRQ_TXCMPLT_Service(void)
{
	UART3_IRQ_Service();
}
void UART4_IRQ_TXCMPLT_Service(void)
{
	UART4_IRQ_Service();
}
//-----------------------------------------------
//��������: ����ת���պ���
//
//����:		SCI_Ph_Num:�����ں�
//
//����ֵ: 	��
//		   
//��ע: 
//-----------------------------------------------
static void SendToRec(BYTE SCI_Ph_Num)
{
	BYTE i;
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SCI_Ph_Num == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//�Ϸ����ж�
	if( i >= MAX_COM_CHANNEL_NUM )
	{
		ASSERT(0,0);
		return ;
	}
	//��ת����״̬
	SerialMap[i].SCIEnableRcv(SCI_Ph_Num);
	api_InitSciStatus(i);
}
//--------------------------------------------------
//��������:  ������485�жϷ������
//
//����:      ��
//
//����ֵ:    ��
//
//��ע:      485  1ͨ��
//--------------------------------------------------
void UART1_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(M4_USART1, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART1);

//		ble_uart_rx_call_back(ch);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART1_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(M4_USART1, UsartTxEmpty) != Reset)
	{
		if ((M4_USART1->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART1_NUM, M4_USART1);
		}
	}
	// �������
	if (USART_GetStatus(M4_USART1, UsartTxComplete) != Reset)
	{
		if( ((M4_USART1->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(M4_USART1, UsartTx, Disable);			//��ֹ����
			USART_FuncCmd(M4_USART1, UsartTxCmpltInt, Disable); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART1_NUM);
		}
	}
}

void UART2_IRQ_Service(void)
{
	uint8_t ch,j;

	// ������
	if( USART_GetStatus(M4_USART2,UsartRxNoEmpty)!= Reset)
	{
		ch = USART_RecData(M4_USART2);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART2_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(M4_USART2, UsartTxEmpty) != Reset)
	{
		if ((M4_USART2->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART2_NUM, M4_USART2);
		}
	}
	// �������
	if (USART_GetStatus(M4_USART2, UsartTxComplete) != Reset)
	{
		if (((M4_USART2->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(M4_USART2, UsartTx, Disable);		 //��ֹ����
			USART_FuncCmd(M4_USART2, UsartTxCmpltInt, Disable); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART2_NUM);
		}
	}
}

void UART3_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(M4_USART3, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART3);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART3_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(M4_USART3, UsartTxEmpty) != Reset)
	{
		if ((M4_USART3->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART3_NUM, M4_USART3);
		}
	}

	// �������
	if (USART_GetStatus(M4_USART3, UsartTxComplete) != Reset)
	{
		if (((M4_USART3->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(M4_USART3, UsartTx, Disable);			//��ֹ����
			USART_FuncCmd(M4_USART3, UsartTxCmpltInt, Disable); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART3_NUM);
		}
	}
}

void UART4_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(M4_USART4, UsartRxNoEmpty) != Reset)
	{
		ch = USART_RecData(M4_USART4);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART4_NUM, ch);
	}

	// ���ͼĴ�����
	if (USART_GetStatus(M4_USART4, UsartTxEmpty) != Reset)
	{
		if ((M4_USART4->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART4_NUM, M4_USART4);
		}
	}

	// �������
	if (USART_GetStatus(M4_USART4, UsartTxComplete) != Reset)
	{
		if (((M4_USART4->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(M4_USART4, UsartTx, Disable);			//��ֹ����
			USART_FuncCmd(M4_USART4, UsartTxCmpltInt, Disable); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART4_NUM);
		}
	}
}
//-----------------------------------------------
//��������: ���ڳ�ʼ������
//
//����:		SciPhNum[in]		��Ҫ��ʼ���Ĵ���������
//
//����ֵ: 	TRUE����ȷ��ʼ��   FALSE:û����ȷ��ʼ��
//		   
//��ע:   �������ȼ���������ռ���ȼ�����ֹ�����ж����У��޷����д���ͨ��
//-----------------------------------------------
static BOOL InitPhSci(BYTE SciPhNum)
{
	uint32_t u32Baud, u32Fcg1Periph,u32EncodePriority;
	BYTE BpsBak, IntStatus, Bps,i;
	en_result_t enRet = Ok;
	stc_irq_regi_conf_t stcIrqRegiCfg;

	stc_usart_uart_init_t stcInitCfg = {
		UsartIntClkCkNoOutput,
		UsartClkDiv_4, //PCLK1
		UsartDataBits8,
		UsartDataLsbFirst,
		UsartOneStopBit, // ���ݲ�����������
		UsartParityEven, // ���ݲ�����������
		UsartSampleBit8,
		UsartStartBitFallEdge,
		UsartRtsEnable,
	};
	
	if( SciPhNum == SCI_MCU_USART1_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART1;
	}
	else if (SciPhNum == SCI_MCU_USART2_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART2;
	}
	else if (SciPhNum == SCI_MCU_USART3_NUM)
	{
		u32Fcg1Periph = PWC_FCG1_PERIPH_USART3;
		//PORT_DebugPortSetting(TRST, Disable);// PB4 RX
		//PORT_DebugPortSetting(TDO_SWO, Disable);// PB3 �ز���λ
	}
	else
	{
		return FALSE;
	}
	


	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//�Ϸ����ж�
	if( i >= (sizeof(Uart_Config)/sizeof(SYS_UART_Config)) )
	{
		ASSERT(0,0);
		return FALSE;
	}

	if(SerialMap[i].SerialType == eIR)//����
	{
		InitTimerA3_PWM1();
	}

	/* Enable peripheral clock */
	PWC_Fcg1PeriphClockCmd(u32Fcg1Periph, Enable);

	 /* Initialize USART IO */
	 PORT_SetFunc(Uart_Config[i].IOConfig[0].IO.Port, Uart_Config[i].IOConfig[0].IO.Pin, Uart_Config[i].IOConfig[0].IO.IoFunc_x, Disable);
	 PORT_SetFunc(Uart_Config[i].IOConfig[1].IO.Port, Uart_Config[i].IOConfig[1].IO.Pin, Uart_Config[i].IOConfig[1].IO.IoFunc_x, Disable);

	 /* Set USART RX IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Rcv_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartRcvIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Rcv_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART RX error IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Err_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartErrIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Err_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART TX IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	 /* Set USART TX complete IRQ */
	 stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_Cmplt_IRQn;
	 stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndCmpltIrqCallback;
	 stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_Cmplt_NUM;
	 enIrqRegistration(&stcIrqRegiCfg);
	 u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);
	 NVIC_SetPriority(stcIrqRegiCfg.enIRQn, u32EncodePriority);
	 NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	 NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
	 /* Initialize UART */
	 /* Set baudrate */
	 //		000 -- 300
	 //		001 -- 600
	 //		010 -- 1200
	 //		011 -- 2400
	 //		100 -- 4800
	 //		101 -- 7200
	 //		110 -- 9600
	 //		111 -- 19200
	 BpsBak = api_GetBaudRate(0, SciPhNum);
	 IntStatus = api_splx(0);

	// ֹͣλ
	Bps = ((BpsBak>>4) & 0x01);

	switch (Bps)
	{
	case 0x00: //1λֹͣλ
		stcInitCfg.enStopBit = UsartOneStopBit;
		break;
	case 0x01: //2λֹͣλ
		stcInitCfg.enStopBit = UsartTwoStopBit;
		break;
	default: //Ĭ��1λֹͣλ
		stcInitCfg.enStopBit = UsartOneStopBit;
		break;
	}

	//У��
	Bps = ((BpsBak >> 5)&0x03);
	switch (Bps)
	{
	case 0x00: //��У��
		stcInitCfg.enParity = UsartParityNone;
		break;
	case 0x01: //����У�飬��У��
		stcInitCfg.enParity = UsartParityOdd;
		break;
	case 0x02: //����У�飬żУ��
		stcInitCfg.enParity = UsartParityEven;
		break;
	default: //Ĭ������У�飬żУ��
		stcInitCfg.enParity = UsartParityEven;
		break;
	}

	// // �̻��뱾��ͨ�ŵ�UART�Լ������� һλֹͣλ��żУ��
	// if (SciPhNum == SCI_MCU_USART3_NUM)
	// {
	// 	stcInitCfg.enStopBit = UsartOneStopBit;
	// 	stcInitCfg.enParity = UsartParityEven;
	// }
	Bps = BpsBak & 0x0f;

	if( Bps <= 6 )  //������С�ڵ���9600ʱ 64��Ƶ
	{
		stcInitCfg.enClkDiv = UsartClkDiv_64;
	}
	enRet = USART_UART_Init(Uart_Config[i].USART_CH, &stcInitCfg);
	
	switch (Bps)
	{
		//case 0: //300BPS
		//	u32Baud = 300;
		//	break;
		//case 1: //600BPS
		//	u32Baud = 600;
		//	break;
		case 2: //1200bps
			u32Baud = 1200;
			break;
		case 3: //2400BPS
			u32Baud = 2400;
			break;
		case 4: //4800BPS
			u32Baud = 4800;
			break;
		case 5: //7200BPS
			u32Baud = 7200;
			break;
		case 6: //9600
			u32Baud = 9600;
			break;
		case 7: //19200
			u32Baud = 19200;
			break;
		case 8: //38400
			u32Baud = 38400;
			break;
		case 9: //57600
			u32Baud = 57600;
			break;
		case 10: //115200
			u32Baud = 115200;
			break;
		case 11: // 230400
			u32Baud = 230400;
			break;
		case 12: // 460800
			u32Baud = 460800;
			break;
		case 13: // 921600
			u32Baud = 921600;
			break;
		default: //Ĭ��115200BPS
			u32Baud = 115200;
			break;
	}
	
	enRet = USART_SetBaudrate(Uart_Config[i].USART_CH, u32Baud);
	/*Enable RX && RX interupt && timeout interrupt function*/
	USART_FuncCmd(Uart_Config[i].USART_CH, UsartRx, Enable);
	USART_FuncCmd(Uart_Config[i].USART_CH, UsartRxInt, Enable);

	SciRcvEnable(SciPhNum);

	api_InitSubProtocol(i);
	//���ڼ��Ӷ�ʱ��
	Serial[i].WatchSciTimer = WATCH_SCI_TIMER;

	api_splx(IntStatus);

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
	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:
		ENABLE_HARD_SCI_1_RECE;
		POWER_HONGWAI_TXD_CLOSE;
		///api_Delay100us(1); // ��ʱ100us
		USART_FuncCmd(M4_USART1, UsartRx, Enable);
		USART_FuncCmd(M4_USART1, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART2_NUM:
		ENABLE_HARD_SCI_2_RECE;
		//api_Delay100us(1); // ��ʱ100us
		USART_FuncCmd(M4_USART2, UsartRx, Enable);
		USART_FuncCmd(M4_USART2, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART3_NUM:
		ENABLE_HARD_SCI_3_RECE;
		//api_Delay100us(1); // ��ʱ100us
		USART_FuncCmd(M4_USART3, UsartRx, Enable);
		USART_FuncCmd(M4_USART3, UsartRxInt, Enable);
		break;
	case SCI_MCU_USART4_NUM:
		ENABLE_HARD_SCI_4_RECE;
		//api_Delay100us(1);// ��ʱ100us
		USART_FuncCmd(M4_USART4, UsartRx, Enable);
		USART_FuncCmd(M4_USART4, UsartRxInt, Enable);
		break;
        
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
	BYTE i;

	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:	//�������շ��ͻ��治һ�£�����ȫ˫��
		USART_FuncCmd(M4_USART1, UsartRx, Disable);
		USART_FuncCmd(M4_USART1, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART2_NUM:
		USART_FuncCmd(M4_USART2, UsartRx, Disable);
		USART_FuncCmd(M4_USART2, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART3_NUM:
		USART_FuncCmd(M4_USART3, UsartRx, Disable);
		USART_FuncCmd(M4_USART3, UsartRxInt, Disable);
		break;
	case SCI_MCU_USART4_NUM:
		USART_FuncCmd(M4_USART4, UsartRx, Disable);
		USART_FuncCmd(M4_USART4, UsartRxInt, Disable);
		break;
	default:
		break;
	}

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
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
	switch (SciPhNum)
	{
	case SCI_MCU_USART1_NUM:
		ENABLE_HARD_SCI_1_SEND;
		POWER_HONGWAI_TXD_OPEN;
		api_Delay100us(1); // ��ʱ100us
		break;
	case SCI_MCU_USART2_NUM:
		ENABLE_HARD_SCI_2_SEND;
		api_Delay100us(1); // ��ʱ100us
		break;
	case SCI_MCU_USART3_NUM:
		ENABLE_HARD_SCI_3_SEND;
		api_Delay100us(1); // ��ʱ100us
		break;
	case SCI_MCU_USART4_NUM:
		ENABLE_HARD_SCI_4_SEND;
		api_Delay100us(1); // ��ʱ100us
		break;
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

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			if(Serial[i].SendLength == 0)
			{
				return FALSE;
			}

			if (Serial[i].TXPoint == 0)
			{
				switch (SciPhNum)
				{
				// case 0:
				// 	HT_UART0->UARTCON |= 0x0001;							//����ʹ��
				// 	HT_UART0->SBUF = Serial[i].ProBuf[Serial[i].TXPoint++]; // д������
				// 	HT_UART0->UARTCON |= 0x0004;							//�����ж�ʹ��
				// 	NVIC_EnableIRQ(UART0_IRQn);								//���ж�
				// 	break;
				case SCI_MCU_USART1_NUM:
					USART_FuncCmd(M4_USART1, UsartTx, Enable);							//����ʹ��
					USART_SendData(M4_USART1, Serial[i].ProBuf[Serial[i].TXPoint++]);	//д������
					USART_FuncCmd(M4_USART1, UsartTxEmptyInt, Enable);					//�����ж�ʹ��
					NVIC_EnableIRQ(Int004_IRQn);										//���ж�
					break;
				case SCI_MCU_USART2_NUM:
					USART_FuncCmd(M4_USART2, UsartTx, Enable);						  //����ʹ��
					USART_SendData(M4_USART2, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(M4_USART2, UsartTxEmptyInt, Enable);				  //�����ж�ʹ��
					NVIC_EnableIRQ(Int008_IRQn);									  //���ж�
					break;
				case SCI_MCU_USART3_NUM:
					USART_FuncCmd(M4_USART3, UsartTx, Enable);						  //����ʹ��
					USART_SendData(M4_USART3, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(M4_USART3, UsartTxEmptyInt, Enable);				  //�����ж�ʹ��
					NVIC_EnableIRQ(Int012_IRQn);									  //���ж�
					break;
				case SCI_MCU_USART4_NUM:
					USART_FuncCmd(M4_USART4, UsartTx, Enable);						  //����ʹ��
					USART_SendData(M4_USART4, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(M4_USART4, UsartTxEmptyInt, Enable);				  //�����ж�ʹ��
					NVIC_EnableIRQ(Int016_IRQn);									  //���ж�
					break;
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
//��������:CAN���߳�ʼ��
//
//����: 
//                    
//����ֵ: 
//
//��ע: 
//-----------------------------------------------
static BOOL CanInitConfig(BYTE SciPhNum)
{
	BYTE i,IntStatus,BpsBak,Bps;
    stc_can_init_config_t stcCanInitCfg;
    stc_irq_regi_conf_t     stcIrqRegiConf;
	stc_can_filter_t *astcFilters=NULL;
    // stc_can_filter_t astcFilters[2] = \
    // {
    //     {0x00000000ul, 0x1FFFFFFFul, CanFilterSel1, CanAllFrames},
    //     {0x00000123ul, 0x1FFFFFFFul, CanFilterSel2, CanAllFrames}
    // };

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if(SciPhNum == SerialMap[i].SCI_Ph_Num)
		{
			break;
		}
	}
	//�Ϸ����ж�
	// if( i != eCAN)
	// {
	// 	ASSERT(0,0);
	// 	return FALSE;
	// }
	
	IntStatus = api_splx(0);
	
    stcIrqRegiConf.enIRQn = (IRQn_Type)CAN_Rx_IrqNo;
    stcIrqRegiConf.enIntSrc = INT_CAN_INT;
    stcIrqRegiConf.pfnCallback = &CAN_RxIrqCallBack;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    //<< Enable can peripheral clock and buffer(ram)
    PWC_RamOpMdConfig(HighSpeedMd);
    PWC_RamPwrdownCmd(PWC_RAMPWRDOWN_CAN, Enable);
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_CAN, Enable);

    //�򿪽����жϣ�������ʱʹ���������ͣ����ڿ��Ը�Ϊ�жϷ���
    CAN_IrqCmd(CanRxIrqEn, Enable);
    CAN_IrqCmd(CanTxPrimaryIrqEn, Enable);
    
    //<< CAN GPIO config
    //PORT_SetFunc(PortE, Pin15, Func_Can1_Rx, Disable);
    //PORT_SetFunc(PortB, Pin10, Func_Can1_Tx, Disable);

    BpsBak = api_GetBaudRate(0, SciPhNum);
   	Bps = BpsBak;
    MEM_ZERO_STRUCT(stcCanInitCfg);
    //<< Can bit time config

	switch (Bps)
	{
	case 0x0D:
		stcCanInitCfg.stcCanBt.PRESC = 100u-1u;	// 0.1   200��Ƶ    10k
		break;
	case 0x0E:
		stcCanInitCfg.stcCanBt.PRESC = 40u-1u;	// 0.04  80��Ƶ		25k
		break;
	case 0x0F:
		stcCanInitCfg.stcCanBt.PRESC = 20u-1u;	// 0.02	 40��Ƶ		50k
		break;
	case 0x10:
		stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16��Ƶ	125K
		break;
	default:
		stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16��Ƶ	Ĭ��125K 
		break;
	}

    stcCanInitCfg.stcCanBt.SEG_1 = 9u-2u;
    stcCanInitCfg.stcCanBt.SEG_2 = 3u-1u;
    stcCanInitCfg.stcCanBt.SJW   = 3u-1u;

    stcCanInitCfg.stcWarningLimit.CanErrorWarningLimitVal = 10u;
    stcCanInitCfg.stcWarningLimit.CanWarningLimitVal = 16u-1u;

    stcCanInitCfg.enCanRxBufAll  = CanRxNormal;
    stcCanInitCfg.enCanRxBufMode = CanRxBufNotStored;
    stcCanInitCfg.enCanSAck = CanSelfAckDisable ;	//�ػ�ģʽ���Զ��ظ�ACK CanSelfAckEnable 
    stcCanInitCfg.enCanSTBMode   = CanSTBFifoMode;

    stcCanInitCfg.pstcFilter     = astcFilters;
    stcCanInitCfg.u8FilterCount  = 0;

    CAN_Init(&stcCanInitCfg);

   	api_InitSubProtocol(i);
	//���ڼ��Ӷ�ʱ��
	Serial[i].WatchSciTimer = WATCH_SCI_TIMER;
	
	api_splx(IntStatus);
    
	//�ڲ��ػ�ģʽ�����ڲ���
    //CAN_ModeConfig(CanInternalLoopBackMode,Enable);
    //CAN_ModeConfig(CanExternalLoopBackMode, Enable);
	// ChangeCanMod();
	
    return TRUE;
}

//-----------------------------------------------
//��������: ʹ��CAN���պ���
//
//����:		SciPhNum[in]		
//
//����ֵ: 	TRUE����ȷʹ��   FALSE:û����ȷʹ��
//		   
//��ע:   
//-----------------------------------------------
static BOOL CanRcvEnable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanRxIrqEn, Enable);
	return TRUE;
}
//-----------------------------------------------
//��������: ��ֹCAN���պ���
//
//����:		SciPhNum[in]		
//
//����ֵ: 	TRUE����ȷ��ֹ   FALSE:û����ȷ��ֹ
//		   
//��ע:   
//-----------------------------------------------
static BOOL CanRcvDisable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanRxIrqEn, Disable);
	// Serial[eCAN].WatchSciTimer = WATCH_SCI_TIMER;
	return TRUE;
}

//-----------------------------------------------
//��������: ��ֹCAN���պ���
//
//����:		SciPhNum[in]		
//
//����ֵ: 	TRUE����ȷ��ֹ   FALSE:û����ȷ��ֹ
//		   
//��ע:   
//-----------------------------------------------
static BOOL CanSendEnable(BYTE SciPhNum)
{
	//CAN_IrqCmd(CanTxPrimaryIrqEn, Enable);
	return TRUE;
}
//-----------------------------------------------
//��������: Can��ʼ���ͺ���
//
//����:		SciPhNum[in]		
//
//����ֵ: 	TRUE����ȷ��ʼ����   FALSE:û����ȷ��ʼ����
//		   
//��ע:   CAN����ʹ���жϷ���
//-----------------------------------------------
static BOOL CanBeginSend(BYTE SciPhNum)
{
	BYTE i;
	static WORD Oldpoint;
	stc_can_txframe_t stcTxFrame;
	
	// if(Serial[eCAN].TXPoint != 0)
	// {
	// 	if (Serial[eCAN].TXPoint!=Oldpoint)
	// 	{
	// 		Oldpoint=Serial[eCAN].TXPoint;
	// 		CanErrTime=0;
	// 	}
	// 	else
	// 	{
	// 		if (CanErrTime>CAN_ERR_TIME_X10MS)//����500ms
	// 		{
	// 			Serial[eCAN].TXPoint=0;
	// 			Serial[eCAN].SendLength=0;
	// 		}
			
	// 	}
		
	// 	return FALSE;
	// }
	Oldpoint=0;
	
	MEM_ZERO_STRUCT(stcTxFrame);
	//<<Can Tx
	api_SetCanID(2,1);
	stcTxFrame.StdID = api_GetCanID();	//���ȼ���ַ
	stcTxFrame.Control_f.DLC = 8;
	stcTxFrame.Control_f.IDE = 1;
	// for(i = 0; i < 8; i++)
	// {
	// 	stcTxFrame.Data[i] = Serial[eCAN].ProBuf[i];	
	// }
	// ���ܽ���CAN�ж��޸���SendLength
	// if(Serial[eCAN].SendLength == 0)
	// {
	// 	return FALSE;
	// }
	
	CAN_SetFrame(&stcTxFrame);
	CAN_TransmitCmd(CanPTBTxCmd);

	// Serial[eCAN].TXPoint = 8;
        
	return TRUE;
	
}

//-----------------------------------------------
//��������:HDSC��SPI��ʼ��
//
//����: 
//                    
//����ֵ: 
//
//��ע: 
//-----------------------------------------------
void api_TopWaveSpiInit(void)
{
	TopoSpiCsIrqInitConfig();
	api_InitSPI(eCOMPONENT_SPI_HSDC, eSPI_MODE_1);
	TopWaveSpiDmaConfig();
	TopWaveSpiDmaEnableInit();
	HSDCTimer = 0;
}

//--------------------------------------------------
//��������:  �ر�����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_DisTopoIqr( void )
{
	stc_exint_config_t stc;
	stc_irq_regi_conf_t stcIrqRegiConf;

	MEM_ZERO_STRUCT(stc);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	stc.enExitCh = ExtiCh05;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�
	stc.enExtiLvl = ExIntRisingEdge;
	stc.enFilterEn = Disable;
	stc.enFltClk = Pclk3Div8;
	
	EXINT_Init(&stc);

	stcIrqRegiConf.enIRQn = (IRQn_Type)TopoSpiCs_IrqNo;
	
	/* Select External Int Ch.8 */
    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ5;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�

    /* Callback function */
    stcIrqRegiConf.pfnCallback = &TopExtInt05_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_DisableIRQ(stcIrqRegiConf.enIRQn);		
}
//-----------------------------------------------
//��������:�������ݴ���SPI-CSƬѡ�жϳ�ʼ��
//
//����: 
//                    
//����ֵ: 
//
//��ע: 
//-----------------------------------------------
static void TopoSpiCsIrqInitConfig(void)
{
	stc_exint_config_t stc;
	stc_irq_regi_conf_t stcIrqRegiConf;

	MEM_ZERO_STRUCT(stc);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	
	stc.enExitCh = ExtiCh05;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�
	stc.enExtiLvl = ExIntRisingEdge;
	stc.enFilterEn = Enable;
	stc.enFltClk = Pclk3Div8;
	
	EXINT_Init(&stc);
	
	stcIrqRegiConf.enIRQn = (IRQn_Type)TopoSpiCs_IrqNo;
	
	/* Select External Int Ch.8 */
    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ5;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�

    /* Callback function */
    stcIrqRegiConf.pfnCallback = &TopExtInt05_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);		
}

//-----------------------------------------------
//��������:��ʼ��ģ�⴮��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateInitSci(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//��������:����ʹ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateSciRcvEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//��������:����ʧ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateSciRcvDisable(BYTE SciPhNum)
{
    
    return TRUE;
}
//-----------------------------------------------
//��������:����ʹ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateSciSendEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//��������:����SCI��һ���ֽ�
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateSciBeginSend(BYTE SciPhNum)
{    
    return TRUE;
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
	    api_InitOutClockPuls(0);	//��ʱ��оƬ�������
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
	else if( ( Type == eDEMAND_PERIOD_OUTPUT ) || ( Type == eSEGMENT_SWITCH_OUTPUT ) )
	{
	    api_InitOutClockPuls(55);   //�ر�ʱ��оƬ�������
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
//��������: �๦�ܶ��ӿ��ƺ���
//
//����:		Mode[in]        0x00:���Ʋ���        0X55:���ò���
//          Type[in]		�๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
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
	
	if( (Type == eDEMAND_PERIOD_OUTPUT)||(Type == eSEGMENT_SWITCH_OUTPUT) )
	{
		api_InitOutClockPuls( 55 );    //�ر�ʱ��оƬ�������
		GlobalVariable.g_byMultiFunPortCounter = (80 / SYS_TICK_PERIOD_MS); //80ms ��systick��ʱ���
		MULTI_FUN_PORT_OUT_L;
	}
	else if( Type == eCLOCK_PULS_OUTPUT )
	{
		//�л�Ϊ��ģʽʱ���Զ������û�е��õĵط�
		api_InitOutClockPuls( 0 );    //��ʱ��оƬ�������
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
}
//-----------------------------------------------
//��������: ��ȡ����ʱ��
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
DWORD  GetTicks(void)
{
	return gsysTick;
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
    CanErrTime++;
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
	// ReportTimeMinus();
    
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

}
void SysTick_Handler(void)
{
    BYTE i;
	static BYTE FlashCnt = 0;  //��˸led������жϼ�����
	#if( SEL_TOPOLOGY == YES )
	static WORD SysTickCounter1 = 0;//����������Ϣ����ʹ��
	#endif
    
	#if( SEL_DEBUG_VERSION == YES )
	//TickDelayTest();
	#endif
	//�������ļ�����
    if(CreateNewFileFlag == 1)
	{
		FlashCnt++;
		//ÿ30ms��˸һ��
		if((FlashCnt % 3) == 0)
		{
			TF_TOGGLE_LED;
		}
		if (FlashCnt == 30)
		{
			CreateNewFileFlag = 0;
			FlashCnt = 0;
		}
	}

	gsysTick += 10;
	SysTickCounter ++;
    if( SysTickCounter >= 10000 )
    {
    	SysTickCounter = 0;
    }

	// ����û�б仯���۲���ʱ
	if (TFKeySmoothTimer != 0)
	{
		TFKeySmoothTimer--;
	}

	if(bySamplePowerOnTimer <= HSDC_OPEN_TIME)
	{
		HSDCTimer ++;
	}
	
    api_SetAllTaskFlag( eFLAG_10_MS );
    
    if( (SysTickCounter%50) == 0 )
    {
    	api_SetAllTaskFlag( eFLAG_500_MS );
    }
    
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
		if(Serial[i].SendToSendDelay > 0)
		{
			Serial[i].SendToSendDelay --;
		}
    }
    
    if( GlobalVariable.g_byMultiFunPortCounter != 0 )
    {
    	GlobalVariable.g_byMultiFunPortCounter--;
    	if( GlobalVariable.g_byMultiFunPortCounter == 0 )
    	{
    		MULTI_FUN_PORT_OUT_H;
    	}
    }
	//�����ϱ���ʱ����
	// ReportTimeMinus();
	
	#if(SEL_EVENT_DI_CHANGE == YES)
	api_DISample();
	#endif

	#if( SEL_TOPOLOGY == YES )
	if(api_GetSysStatus(eSYS_STATUS_TOPOING) == TRUE)
	{
		SysTickCounter1++;
		if(SysTickCounter1 >= 60)
		{
			SysTickCounter1 = 0;
			TopoSignalSendHandle();
		}
	}
	#endif
}
//-----------------------------------------------
//��������: TimerA5�жϷ������1ms�ж�һ��
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   ִ��10.8us
//-----------------------------------------------
//static void TimerA5_Handler(void)
//{
//	static BYTE bSpiCsCount=0;
//	TRealTimer TmpRealTimer;
//	
//	MsTickCounter ++;
//
//	if(MsTickCounter % 2)
//	{
//		MsTickCounter =0;
//	}
//
//    TIMERA_ClearFlag(M4_TMRA5, TimeraFlagOverflow);
//}
//-----------------------------------------------
//��������: CAN�����жϺ���
//
//����: 	
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void CAN_RxIrqCallBack(void)
{
	BYTE i,bLen,bLen1,j;
	WORD wPoint;

	stc_can_rxframe_t stcRxFrame;
	stc_can_txframe_t stcTxFrame;	
	
    if(true == CAN_IrqFlgGet(CanRxIrqFlg))
	{
		//CAN_IrqCmd(CanRxIrqEn, Disable);
		CAN_Receive(&stcRxFrame); 
		// if( CanBusPara.bRxNum != 0 )
		// {
		// 	for ( i = 0; i < CanBusPara.bRxNum; i++)
		// 	{
		// 		if (CanBusPara.bRxNode[i]==(stcRxFrame.StdID&0x000000ff))//���˼���ID
		// 		{
					if (api_ChargingCan(stcRxFrame)==FALSE)//���ǳ��׮����
					{
						// if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
						// {
						// 	//��ȡ��Ч����
						// 	memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
						// 	Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
						// }
						// else
						{
							// bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
							// memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
							// bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
							// memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
							// Serial[eCAN].RXWPoint = bLen1;	
						}
						api_SetCanID(1,(BYTE)stcRxFrame.ExtID);
					}
		// 			break;
		// 		}
		// 	}
		// }
		// else //δ����CAN����ID����ʱ������ͨ�ڴ���
		// {
		// 	if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
		// 	{
		// 		//��ȡ��Ч����
		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
		// 		Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
		// 	}
		// 	else
		// 	{
		// 		bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
		// 		bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
		// 		memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
		// 		Serial[eCAN].RXWPoint = bLen1;	
		// 	}
		// }
		
		CAN_IrqFlgClr(CanRxIrqFlg);
	}
	if(true == CAN_IrqFlgGet(CanTxPrimaryIrqFlg))
	{
//		if(Serial[eCAN].TXPoint >= Serial[eCAN].SendLength)
//		{
//			//���ݷ������
//			//Serial[eRS485_II].SendToSendDelay = MAX_TX_COMMIDLETIME;
//			CAN_IrqFlgClr(CanTxPrimaryIrqFlg);
//			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
//			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
//			{
//				DealSciFlag(j);
//			}
//			SendToRec(0xAA);
//			return; 
//		}
		// else if((Serial[eCAN].TXPoint + 8) <= Serial[eCAN].SendLength)
		// {
		// 	bLen = 8;
		// }
//		else
//		{
//			api_SetCanID(2,0);
//			// bLen = Serial[eCAN].SendLength - Serial[eCAN].TXPoint;
//		}

		MEM_ZERO_STRUCT(stcTxFrame);
		//<<Can Tx
		stcTxFrame.StdID = api_GetCanID();	//���ȼ���ַ
		stcTxFrame.Control_f.DLC = 8;	//�ϸ���8���ֽڻظ�
		stcTxFrame.Control_f.IDE = 1;
		// for(i = 0; i < bLen; i++)
		// {
		// 	stcTxFrame.Data[i] = Serial[eCAN].ProBuf[Serial[eCAN].TXPoint+i];	
		// }
                
		// Serial[eCAN].TXPoint += i;
                
		if( (i < 8) && (i == bLen) )
		{
			for(i = bLen; i < 8; i++)
			{
				stcTxFrame.Data[i] = 0x00;	
			}			
		}
		
		CAN_SetFrame(&stcTxFrame);
		CAN_TransmitCmd(CanPTBTxCmd);

		CanErrTime=0;

		CAN_IrqFlgClr(CanTxPrimaryIrqFlg);

	}
    if(true == CAN_IrqFlgGet(CanRxOverIrqFlg))
    {
        CAN_IrqFlgClr(CanRxOverIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanRxBufFullIrqFlg))
    {
        CAN_IrqFlgClr(CanRxBufFullIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanRxBufAlmostFullIrqFlg))
    {
        CAN_IrqFlgClr(CanRxBufAlmostFullIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanTxSecondaryIrqFlg))
    {
        CAN_IrqFlgClr(CanTxSecondaryIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanErrorIrqFlg))
    {
        CAN_IrqFlgClr(CanErrorIrqFlg);
    }
    if(true == CAN_IrqFlgGet(CanAbortIrqFlg))
    {
        CAN_IrqFlgClr(CanAbortIrqFlg);
    }
}


//-----------------------------------------------
//��������:��ʼ��PWM���������38k)��ʵ��Ϊ37.807K��
//
//����: 
//                    
//����ֵ:  ��
//
//��ע: ���ں������,ռ�ձ�25%��Ӳ����Ʒ��࣬���ⷢ���ʵ�����Ϊ76.9%ռ�ձȣ�
//-----------------------------------------------
static void InitTimerA3_PWM1( void )
{
	stc_timera_base_init_t timera_base;
	stc_timera_compare_init_t timera_compare;

	PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3,Enable);

	MEM_ZERO_STRUCT(timera_base);
	
	TIMERA_DeInit(M4_TMRA3);//��ʼ����3��TIMERA��Ԫ

	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1 ʱ��Ƶ��12M  18M  12M
	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//��ݲ���ʽ
	timera_base.enCntDir = TimeraCountDirUp;				//���ϼ���
	timera_base.enSyncStartupEn = Disable;					//����ģ��1ͬ��
	
	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	timera_base.u16PeriodVal = 316;							//�ӽ�38kHz
	#elif(SPD_HDSC_MCU == SPD_72000K)
	timera_base.u16PeriodVal = 474;							//�ӽ�38kHz
	#endif
	TIMERA_BaseInit(M4_TMRA3,&timera_base);
	MEM_ZERO_STRUCT(timera_compare);
	
	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	timera_compare.u16CompareVal = 79;						//25%ռ�ձ�
	#elif(SPD_HDSC_MCU == SPD_72000K)
	timera_compare.u16CompareVal = 118;						//�ӽ�25%ռ�ձ�
	#endif
	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//��ʼ����ʱΪ�ߵ�ƽ
	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//��������ʱΪ�ߵ�ƽ
	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//�Ƚ�ֵƥ��ʱ�л�Ϊ��ƽת��
	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//��������ֵʱ�л�Ϊ�ߵ�ƽ
	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//��һ���ڿ�ʼʱ�л���Ч
	TIMERA_CompareInit(M4_TMRA3,TimeraCh1,&timera_compare);

	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Disable);	//�ر�PWM���ܣ���Ҫʱֱ�ӿ���

	TIMERA_Cmd(M4_TMRA3,Enable);
}

//����PWMʹ��
void IrPWMEnable(BOOL enNewSta)
{
	if(enNewSta == TRUE)
	{
		TIMERA_Cmd(M4_TMRA3,Enable);
	}
	else
	{
		TIMERA_Cmd(M4_TMRA3,Disable);
	}

	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Enable);		//��PWM�����ȷ����ʱ���ر�ʱ����ߵ�ƽ
}

//-----------------------------------------------
//��������: �������ݽ���SPI_CS�жϺ���
//
//����: 	
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void TopExtInt05_Callback(void)
{
	if( HSDC_SPI_CS_STATUS )
	{
		api_ParseHsdcWaveBuf();
		memset(&sTopoWave.RxBuf[0], 0, sizeof(sTopoWave.RxBuf));
		TopWaveSpiDmaEnableInit();
	}
}
#if( SEL_TOPOLOGY == YES )
//����PWMʹ��
void TopoPWMEnable(BOOL enNewSta)
{
	if(enNewSta == TRUE)
	{
		TP_PWMEN_ON;
		TIMERA_Cmd(M4_TMRA2,Enable);
	}
	else
	{
		TP_PWMEN_OFF;
		TIMERA_Cmd(M4_TMRA2,Disable);
	}
	
	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,(en_functional_state_t)Enable);	//��PWM�����ȷ����ʱ���ر�ʱ����ߵ�ƽ
}

//-----------------------------------------------
//��������: ���˷��ʹ���
//
//����: 	
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void TopoSignalSendHandle(void)
{
	static BYTE AlreadySendBitLen = 0 ,SignatureCodeLen = 0;//�����Ѵ�������(bit)����,��ȡ���ݳ���
	static BYTE SignatureCode[16] = {0}; //������д���ݻ���
	BYTE ByteIndex, BitIndex;


	if(AlreadySendBitLen == 0)
	{
		SignatureCodeLen = ModulatSignalInfoRAM.SignatureCodeLen;

		if(SignatureCodeLen > sizeof(SignatureCode))
		{
			SignatureCodeLen = sizeof(SignatureCode);
		}

		memcpy(SignatureCode,ModulatSignalInfoRAM.SignatureCode,SignatureCodeLen);
	}

	if( ((AlreadySendBitLen / 8) >= SignatureCodeLen)
	 || (SignatureCodeLen != 2)			//�����볤�ȹ̶�2�ֽ�
	)
	{
		//�������
		TopoPWMEnable(FALSE);
		api_ClrSysStatus(eSYS_STATUS_TOPOING);
		AlreadySendBitLen = 0;
		SignatureCodeLen = 0;
		memset(SignatureCode,0x00,sizeof(SignatureCode));
		return;
	}

	ByteIndex = SignatureCodeLen - 1 - AlreadySendBitLen / 8;	//�Ӹ��ֽڿ�ʼ��
	BitIndex  = 7 - (AlreadySendBitLen % 8);				//�Ӹ�λ��ʼ��

	if(SignatureCode[ByteIndex] & (1 << BitIndex))
	{
		TopoPWMEnable(TRUE);
	}
	else
	{
		TopoPWMEnable(FALSE);
	}
	
	AlreadySendBitLen++;
}


//-----------------------------------------------
//��������: ��ʼ�����˷���PWM��ؼĴ���
//
//����: 	
//
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void InitTimerA2_PWM4(WORD wPeriod, WORD wCompare)
{
	stc_timera_base_init_t timera_base;
	stc_timera_compare_init_t timera_compare;

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA2,Enable);

	MEM_ZERO_STRUCT(timera_base);
	
	TIMERA_DeInit(M4_TMRA2);//��ʼ���ڶ���TIMERA��Ԫ

	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1/8 ʱ��Ƶ��12M
	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//��ݲ���ʽ
	timera_base.enCntDir = TimeraCountDirUp;				//���ϼ���
	timera_base.enSyncStartupEn = Disable;				//����ģ��1ͬ��
	timera_base.u16PeriodVal = 12 * wPeriod;			//0.0012 * (12*1000*1000) = 14400	
	TIMERA_BaseInit(M4_TMRA2,&timera_base);

	MEM_ZERO_STRUCT(timera_compare);
	timera_compare.u16CompareVal = 12 * wCompare;		//2/3�ĸߵ�ƽ(���߼�)
	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//��ʼ����ʱΪ�ߵ�ƽ
	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//��������ʱΪ�ߵ�ƽ
	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//�Ƚ�ֵƥ��ʱ�л�Ϊ��ƽת��
	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//��������ֵʱ�л�Ϊ�ߵ�ƽ
	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//��һ���ڿ�ʼʱ�л���Ч
	TIMERA_CompareInit(M4_TMRA2,TimeraCh4,&timera_compare);

	TIMERA_Cmd(M4_TMRA2,Disable);
	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,Enable);	//�ر�PWM���ܣ���Ҫʱֱ�ӿ���
}
#endif
//-----------------------------------------------
// ��������:     ��ѯ�˿ڶ�Ӧ����
//
// ����:       Type[in] ������ eSERIAL_TYPE
//
//
// ����ֵ:   ����õĴ��ں�
//
// ��ע:   ����< MAX_COM_CHANNEL_NUM ��ȷ ������MAX_COM_CHANNEL_NUM����
//-----------------------------------------------
BYTE api_GetSerialIndex(BYTE Type)
{
	BYTE i;

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++) // ����жϴ��ں�
	{
		if (Type == SerialMap[i].SerialType)
		{
			break;
		}
	}
	if (i == MAX_COM_CHANNEL_NUM) // δ�ҵ��ô��� ����
	{
		ASSERT(0, 0);
	}

	return i;
}

#endif//#if( (BOARD_TYPE == BOARD_HC_SEPARATETYPE) )

