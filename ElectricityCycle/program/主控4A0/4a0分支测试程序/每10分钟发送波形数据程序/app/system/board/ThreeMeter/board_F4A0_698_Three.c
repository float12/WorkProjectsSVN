//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2016.8.30
//����		֧��698.45��Լ�ĵ����ӡ�ư������ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "F4A0_LowPower_Three.h"
#include "board_F4A0_698_Three.H"

#if (BOARD_TYPE == BOARD_HC_SEPARATETYPE)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define GPIO_DEFAULT 0xFF
#define TX_WAVE_LED_TOGGLE_CNT 25//25*20=500ms
//��ƽ�ߵ�
#define HIGH 1
#define LOW 0

#define POWER_CHECK_LOOP_COUNT 10	// 10�δ�ѭ����һ��
#define POWER_CHECK_CONTINUE_COUNT 5 // ����������
//����ʹ��       ��Ҫ
//#define USART_CR1_TCIE 0x00000040
//#define USART_CR1_TXEIE 0x00000080

#define SYSTEM_POWER_VOL(vol) ((float)vol * 3.3 / 4096)

#if (WAVE_RECORD_PHASE_NUM == 1)
#define SYSTEM_VOL_MIN_UP 1.5	//��Դ�ж�����
#define SYSTEM_VOL_MIN_DOWN 1.42 //��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)

#elif (WAVE_RECORD_PHASE_NUM == 3)
#define SYSTEM_VOL_MIN_UP 1.85   //��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)
#define SYSTEM_VOL_MIN_DOWN 1.77 //��Դ�ж�����(����12V��Դ��ѹ:12V*10/(51+10)=1.967V)
#endif

#define CAN_ERR_TIME_X10MS 5

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


typedef enum
{
	TRESET = 0U,
	TSET = !TRESET
};
typedef enum
{
	// timer0_IrqNo = Int000_IRQn,
	// timerA1_IrqNo = Int001_IRQn,
	// timerA2_IrqNo = Int002_IRQn,
	Uart1Rcv_IrqNo = INT003_IRQn,
	Uart1Snd_IrqNo = INT004_IRQn,
	Uart1Snd_Cmplt_IrqNo = INT005_IRQn,
	Uart1_Irq_Err_No = INT006_IRQn,

	Uart4Rcv_IrqNo = INT007_IRQn,
	Uart4Snd_IrqNo = INT008_IRQn,
	Uart4_Irq_Err_No = INT009_IRQn,
	Uart4Snd_Cmplt_IrqNo = INT010_IRQn,
	Uart3Rcv_IrqNo = INT011_IRQn,
	Uart3Snd_IrqNo = INT012_IRQn,
	Uart3_Irq_Err_No = INT013_IRQn,
	Uart3Snd_Cmplt_IrqNo = INT014_IRQn,
	//��·����оƬ+12·���
	HCS1_IrqNo = INT015_IRQn,
	HCS2_IrqNo = INT016_IRQn,
	HCS3_IrqNo = INT017_IRQn,
	HCS4_IrqNo = INT018_IRQn,
	Edge1_Test_IrqNo = INT019_IRQn,
	Edge2_Test_IrqNo = INT020_IRQn,
	Edge3_Test_IrqNo = INT021_IRQn,
	Edge4_Test_IrqNo = INT022_IRQn,
	Edge5_Test_IrqNo = INT023_IRQn,
	Edge6_Test_IrqNo = INT024_IRQn,
	Edge7_Test_IrqNo = INT025_IRQn,
	Edge8_Test_IrqNo = INT026_IRQn,
	Edge9_Test_IrqNo = INT027_IRQn,
	Edge10_Test_IrqNo = INT028_IRQn,
	Edge11_Test_IrqNo = INT029_IRQn,
	Edge12_Test_IrqNo = INT030_IRQn,//�ⲿ�ж�ֻ��ѡ��0-37��128-143

	Uart1Dma_TC_IrqNo = INT040_IRQn,

} IRQNUM;

typedef struct
{
	MCU_IO IO;
	uint16_t DIR;
} SYS_IO_Config;

typedef struct
{
	SYS_IO_Config IOConfig[2];
	CM_USART_TypeDef *USART_CH;

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
void UART5_IRQ_Service(void);
void UART6_IRQ_Service(void);
void UART7_IRQ_Service(void);

void UART1_IRQ_Rec_Service(void);
void UART2_IRQ_Rec_Service(void);
void UART3_IRQ_Rec_Service(void);
void UART4_IRQ_Rec_Service(void);
void UART5_IRQ_Rec_Service(void);
void UART6_IRQ_Rec_Service(void);
void UART7_IRQ_Rec_Service(void);

void UART1_IRQ_Tx_Service(void);
void UART2_IRQ_Tx_Service(void);
void UART3_IRQ_Tx_Service(void);
void UART4_IRQ_Tx_Service(void);
void UART5_IRQ_Tx_Service(void);
void UART6_IRQ_Tx_Service(void);
void UART7_IRQ_Tx_Service(void);

void UART1_IRQ_TXCMPLT_Service(void);
void UART2_IRQ_TXCMPLT_Service(void);
void UART3_IRQ_TXCMPLT_Service(void);
void UART4_IRQ_TXCMPLT_Service(void);
void UART5_IRQ_TXCMPLT_Service(void);
void UART6_IRQ_TXCMPLT_Service(void);
void UART7_IRQ_TXCMPLT_Service(void);

void ClearSPIError(CM_SPI_TypeDef *SPIx);

void UART_IRQ_Err_Service(void);
static void TopoSpiCsIrqInitConfig(void);
static void CAN_RxIrqCallBack(void);
static void HSPISample1Callback(void);
static void HSPISample2Callback(void);
static void HSPISample3Callback(void);
static void HSPISample4Callback(void);
static void Loop1ChargeCallback(void);
static void Loop2ChargeCallback(void);
static void Loop3ChargeCallback(void);
static void Loop4ChargeCallback(void);
static void Loop5ChargeCallback(void);
static void Loop6ChargeCallback(void);
static void Loop7ChargeCallback(void);
static void Loop8ChargeCallback(void);
static void Loop9ChargeCallback(void);
static void Loop10ChargeCallback(void);
static void Loop11ChargeCallback(void);
static void Loop12ChargeCallback(void);

static void TopoSignalSendHandle(void);
static void InitTimerA3_PWM1(void);

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
BYTE IsExitLowPower;	//ģ���ڱ���
DWORD LowPowerSecCount; //�͹����ۼ�����
//volatile WORD MsTickCounter; //ms������
volatile DWORD gsysTick = 0;
//�綯���г�
TExtIntCof ExtIntCof[16] =
{		// ǰ�ĸ��Ǽ���оƬ�����ݵ�cs
		[0] = {EXTINT_CH12, INT_SRC_PORT_EIRQ12, HCS1_IrqNo, &HSPISample1Callback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_07},//0111
		[1] = {EXTINT_CH15, INT_SRC_PORT_EIRQ15, HCS2_IrqNo, &HSPISample2Callback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_07},
		[2] = {EXTINT_CH00, INT_SRC_PORT_EIRQ0, HCS3_IrqNo, &HSPISample3Callback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_07},
		[3] = {EXTINT_CH11, INT_SRC_PORT_EIRQ11, HCS4_IrqNo, &HSPISample4Callback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_07},
		[4] = {EXTINT_CH01, INT_SRC_PORT_EIRQ1, Edge1_Test_IrqNo, &Loop1ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},//����ռ�������ж�
		[5] = {EXTINT_CH02, INT_SRC_PORT_EIRQ2, Edge2_Test_IrqNo, &Loop2ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[6] = {EXTINT_CH03, INT_SRC_PORT_EIRQ3, Edge3_Test_IrqNo, &Loop3ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[7] = {EXTINT_CH04, INT_SRC_PORT_EIRQ4, Edge4_Test_IrqNo, &Loop4ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[8] = {EXTINT_CH05, INT_SRC_PORT_EIRQ5, Edge5_Test_IrqNo, &Loop5ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[9] = {EXTINT_CH13, INT_SRC_PORT_EIRQ13, Edge6_Test_IrqNo, &Loop6ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[10] = {EXTINT_CH14, INT_SRC_PORT_EIRQ14, Edge7_Test_IrqNo, &Loop7ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[11] = {EXTINT_CH10, INT_SRC_PORT_EIRQ10, Edge8_Test_IrqNo, &Loop8ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[12] = {EXTINT_CH08, INT_SRC_PORT_EIRQ8, Edge9_Test_IrqNo, &Loop9ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[13] = {EXTINT_CH09, INT_SRC_PORT_EIRQ9, Edge10_Test_IrqNo, &Loop10ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[14] = {EXTINT_CH07, INT_SRC_PORT_EIRQ7, Edge11_Test_IrqNo, &Loop11ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
		[15] = {EXTINT_CH06, INT_SRC_PORT_EIRQ6, Edge12_Test_IrqNo, &Loop12ChargeCallback,EXTINT_TRIG_RISING,DDL_IRQ_PRIO_00},
};
// ����dma1��7

//dmaͨ������ hsdc1��spi2����dma1�� ͨ��0��ͨ��1��hsdc2��spi3���� 2��3�� dma2�� hsdc3��spi4����0��1��hsdc4��spi6���� 2��3 ��
TWaveDataConf WaveDataConf[4] = {
	[0] = {
		.ExtIntCofIndex = 0,
		.DmaConf = {
			.SPIInstance = CM_SPI2,
			.DmaInstance = CM_DMA1,
			.DmaTxChannel = DMA_CH0,
			.DmaTxTrigCh = AOS_DMA1_0,
			.DMA_Tx_FLAG = DMA_FLAG_TC_CH0,
			.DmaRxChannel = DMA_CH1,
			.DmaRxTrigCh = AOS_DMA1_1,
			.DMA_Rx_FLAG = DMA_FLAG_TC_CH1,
			.TxTrigSrc = EVT_SRC_SPI2_SPTI,
			.RxTrigSrc = EVT_SRC_SPI2_SPRI,
			.RecBuf = WaveDataDeal[0].RxBuf,
			.BufSize = sizeof(WaveDataDeal[0].RxBuf),
		},
		.SPIComponent = eCOMPONENT_SPI_HSDC1,
	},
	[1] = {
		.ExtIntCofIndex = 1,
		.DmaConf = {
			.SPIInstance = CM_SPI3,
			.DmaInstance = CM_DMA1,
			.DmaTxChannel = DMA_CH2,
			.DmaTxTrigCh = AOS_DMA1_2,
			.DMA_Tx_FLAG = DMA_FLAG_TC_CH2,
			.DmaRxChannel = DMA_CH3,
			.DmaRxTrigCh = AOS_DMA1_3,
			.DMA_Rx_FLAG = DMA_FLAG_TC_CH3,
			.TxTrigSrc = EVT_SRC_SPI3_SPTI,
			.RxTrigSrc = EVT_SRC_SPI3_SPRI,
			.RecBuf = WaveDataDeal[1].RxBuf,
			.BufSize = sizeof(WaveDataDeal[1].RxBuf),
		},
		.SPIComponent = eCOMPONENT_SPI_HSDC2,
	},
	[2] = {
		.ExtIntCofIndex = 2,
		.DmaConf = {
			.SPIInstance = CM_SPI4,
			.DmaInstance = CM_DMA2,
			.DmaTxChannel = DMA_CH0,
			.DmaTxTrigCh = AOS_DMA2_0,
			.DMA_Tx_FLAG = DMA_FLAG_TC_CH0,
			.DmaRxChannel = DMA_CH1,
			.DmaRxTrigCh = AOS_DMA2_1,
			.DMA_Rx_FLAG = DMA_FLAG_TC_CH1,
			.TxTrigSrc = EVT_SRC_SPI4_SPTI,
			.RxTrigSrc = EVT_SRC_SPI4_SPRI,
			.RecBuf = WaveDataDeal[2].RxBuf,
			.BufSize = sizeof(WaveDataDeal[2].RxBuf),
		},
		.SPIComponent = eCOMPONENT_SPI_HSDC3,
	},
	[3] = {
		.ExtIntCofIndex = 3,
		.DmaConf = {
			.SPIInstance = CM_SPI6,
			.DmaInstance = CM_DMA2,
			.DmaTxChannel = DMA_CH2,
			.DmaTxTrigCh = AOS_DMA2_2,
			.DMA_Tx_FLAG = DMA_FLAG_TC_CH2,
			.DmaRxChannel = DMA_CH3,
			.DmaRxTrigCh = AOS_DMA2_3,
			.DMA_Rx_FLAG = DMA_FLAG_TC_CH3,
			.TxTrigSrc = EVT_SRC_SPI6_SPTI,
			.RxTrigSrc = EVT_SRC_SPI6_SPRI,
			.RecBuf = WaveDataDeal[3].RxBuf,
			.BufSize = sizeof(WaveDataDeal[3].RxBuf),
		},
		.SPIComponent = eCOMPONENT_SPI_HSDC4,
	},
};

// ����ӳ��ͼ���������Ӳ�һ����
const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM] = {
	// ��һ·RS-485
	eRS485_I,
	SCI_MCU_USART4_NUM, // MCU ���ں�
	&InitPhSci,			// ���ڳ�ʼ��
	&SciRcvEnable,		// ��������
	&SciRcvDisable,		// ��ֹ����
	&SciSendEnable,		// ��������
	&SciBeginSend,		// ���ͽ�ֹ

	// �ز���RF
	eCR,
	SCI_MCU_USART3_NUM, // MCU ���ں�
	&InitPhSci,			// ���ڳ�ʼ��
	&SciRcvEnable,		// ��������
	&SciRcvDisable,		// ��ֹ����
	&SciSendEnable,		// ��������
	&SciBeginSend,		// ���ͽ�ֹ

	//��4g��������,��Ӧԭ��ͼrx1 tx1  52��53����
	e4gWaveData,
	SCI_MCU_USART1_NUM, // MCU ���ں�
	&InitPhSci,			// ���ڳ�ʼ��
	&SciRcvEnable,		// ��������
	&SciRcvDisable,		// ��ֹ����
	&SciSendEnable,		// ��������
	&SciBeginSend,		// ���ͽ�ֹ

	//	 ����
	//  eIR,
	//  SCI_MCU_USART1_NUM, // MCU ���ں�
	//  &InitPhSci,			// ���ڳ�ʼ��
	//  &SciRcvEnable,		// ��������
	//  &SciRcvDisable,		// ��ֹ����
	//  &SciSendEnable,		// ��������
	//  &SciBeginSend,		// ���ͽ�ֹ

	//CAN����
	//  eCAN,
	//  0xAA, // MCU ���ں�
	//  &CanInitConfig,
	//  &CanRcvEnable,
	//  &CanRcvDisable,
	//  &CanSendEnable,
	//  &CanBeginSend,

	// eEXP_PRO,
	// 0xFF,					// MCU ���ں�
	// &SimulateInitSci,		// ���ڳ�ʼ��
	// &SimulateSciRcvEnable,  // ��������
	// &SimulateSciRcvDisable, // ��ֹ����
	// &SimulateSciSendEnable, // ��������
	// &SimulateSciBeginSend,  // ���ͽ�ֹ
};
// ���±�SCI_MCU_USARTX_NUMȡ
SYS_UART_Config Uart_Config[] =
	{
		//485-1  SCI_MCU_USART4_NUM
		{
			{{
				 {PortD, Pin05, Func33_Usart4_Rx}, {PIN_DIR_OUT}, //��
			 },
			 {
				 {PortD, Pin06, Func32_Usart4_Tx}, {PIN_DIR_OUT}, //��
			 }},
			//�������к�
			{CM_USART4},
			//�жϱ��                            //�ص�����                    //�ж�Դ
			{(IRQn_Type)Uart4Rcv_IrqNo},
			{&UART4_IRQ_Rec_Service},
			{INT_SRC_USART4_RI},
			//���ڽ����жϱ��                     //���ڻص�����                //�����ж�Դ
			{(IRQn_Type)Uart4_Irq_Err_No},
			{&UART_IRQ_Err_Service},
			{INT_SRC_USART4_EI},
			//���ڷ����жϱ��                     //���ڻص�����                //�����ж�Դ
			{(IRQn_Type)Uart4Snd_IrqNo},
			{&UART4_IRQ_Tx_Service},
			{INT_SRC_USART4_TI},
			//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
			{(IRQn_Type)Uart4Snd_Cmplt_IrqNo},
			{&UART4_IRQ_TXCMPLT_Service},
			{INT_SRC_USART4_TCI},
		},

		//�ز�ͨ�� SCI_MCU_USART3_NUM
		{
			{{
				 {PortC, Pin00, Func32_Usart3_Tx}, {PIN_DIR_OUT}, //��
			 },
			 {
				 {PortC, Pin02, Func33_Usart3_Rx }, {PIN_DIR_IN}, //��
			 }},
			//���к�
			{CM_USART3},
			//���ڽ����жϱ��                   //���ڻص�����               /�����ж�Դ
			{(IRQn_Type)Uart3Rcv_IrqNo},
			{&UART3_IRQ_Rec_Service},
			{INT_SRC_USART3_RI},
			//���ڽ����жϱ��                     //���ڻص�����              //�����ж�Դ
			{(IRQn_Type)Uart3_Irq_Err_No},
			{&UART_IRQ_Err_Service},
			{INT_SRC_USART3_EI},
			//���ڷ����жϱ��                     //���ڻص�����               //�����ж�Դ
			{(IRQn_Type)Uart3Snd_IrqNo},
			{&UART3_IRQ_Tx_Service},
			{INT_SRC_USART3_TI},
			//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
			{(IRQn_Type)Uart3Snd_Cmplt_IrqNo},
			{&UART3_IRQ_TXCMPLT_Service},
			{INT_SRC_USART3_TCI},
		},

		//��4g�������� SCI_MCU_USART1_NUM
		{
			{{
				 {PortB, Pin13, Func32_Usart1_Tx}, {PIN_DIR_OUT}, //��
			 },
			 {
				 {PortB, Pin14, Func33_Usart1_Rx }, {PIN_DIR_OUT}, //��
			 }},
			{CM_USART1},
			//�����жϱ��                        //���ڻص�����            //�����ж�Դ
			{(IRQn_Type)Uart1Rcv_IrqNo},
			{&UART1_IRQ_Rec_Service},
			{INT_SRC_USART1_RI},
			//���ڽ����жϱ��                     //���ڻص�����                //�����ж�Դ
			{(IRQn_Type)Uart1_Irq_Err_No},
			{&UART_IRQ_Err_Service},
			{INT_SRC_USART1_EI},
			//���ڷ����жϱ��                     //���ڻص�����               //�����ж�Դ
			{(IRQn_Type)Uart1Snd_IrqNo},
			{&UART1_IRQ_Tx_Service},
			{INT_SRC_USART1_TI},
			//���ڷ�������жϱ��                  //���ڷ�����ɻص�����       //�����ж�Դ
			{(IRQn_Type)Uart1Snd_Cmplt_IrqNo},
			{&UART1_IRQ_TXCMPLT_Service},
			{INT_SRC_USART1_TCI},
		},
};
//------------------------------------------------------------------------------------------------------------------------------------------------------
//																		�ϵ���������
//------------------------------------------------------------------------------------------------------------------------------------------------------

// ֻ������Ҫ���õ�IO���������õ��������
const port_init_t TGPIO_Config[] =
	{
		//PORT			PIN    		FUNC      		PinState      Dir      		OutputType    	Drv    		Latch    	PullUp   Invert   ExtInt     InputType			Attr
		
		{.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	sample
		{.enPort = PortD, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS1		sample
		{.enPort = PortD, Pin02, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS2		sample
		{.enPort = PortD, Pin03, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS3		sample
		{.enPort = PortD, Pin04, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS4		sample
		{.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	sample
		{.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.PB10		SPI_MISO	sample
		//hsdc1
		{.enPort = PortC, Pin04, Func43_Spi2_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI1
		{.enPort = PortC, Pin05, Func44_Spi2_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI1
		{.enPort = PortB, Pin12, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI1
		//hsdc2
		{.enPort = PortB, Pin04, Func46_Spi3_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI2
		{.enPort = PortB, Pin05, Func47_Spi3_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI2
		{.enPort = PortB, Pin15, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI2��test��
		//hsdc3
		{.enPort = PortC, Pin06, Func43_Spi4_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI3
		{.enPort = PortC, Pin07, Func44_Spi4_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI3
		{.enPort = PortD, Pin00, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI3
		//hsdc4
		{.enPort = PortA, Pin08, Func46_Spi6_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	HSPI4
		{.enPort = PortA, Pin09, Func47_Spi6_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	HSPI4
		{.enPort = PortA, Pin11, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 45.		SPI_CS		HSPI4
		
		{.enPort = PortE, Pin02, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		ESAM CS		
		{.enPort = PortC, Pin10, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		FLASH CS		

		//12���ⲿ����
		{.enPort = PortA, Pin01, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin02, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin03, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin04, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin05, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortE, Pin13, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortE, Pin14, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortB, Pin10, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortD, Pin08, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortD, Pin09, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin07, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		{.enPort = PortA, Pin06, Func0_Gpio, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, PIN_EXTINT_ON, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 45.		TEST ����
		//��Դ����
		{.enPort = PortE, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample1��Դ����
		{.enPort = PortB, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample2��Դ����
		{.enPort = PortB, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample3��Դ����
		{.enPort = PortB, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample4��Դ����
		{.enPort = PortB, Pin03, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		ESAM��Դ����
		{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		FLASH��Դ����


		//4G
		{.enPort = PortE, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		4G��Դ����

		{.enPort = PortB, Pin13, Func32_Usart1_Tx , HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		��4g����������
		{.enPort = PortB, Pin14, Func33_Usart1_Rx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3

		{.enPort = PortC, Pin02, Func33_Usart3_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		4g������
		{.enPort = PortC, Pin00, Func32_Usart3_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		
		//485
		{.enPort = PortD, Pin05, Func33_Usart4_Rx, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 1. PE2 		485
		{.enPort = PortD, Pin06, Func32_Usart4_Tx, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 2. PE3
		{.enPort = PortD, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},//3. T/R-1 RS485�������
		// 8025+EEPROM
		{.enPort = PortD, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		8025SCL   ��©���  ����̬
		{.enPort = PortD, Pin14, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		8025SDA   ��©���  ����̬

		{.enPort = PortE, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 79.PC11		EEPROM SCL   ��©���  ����̬
		{.enPort = PortE, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_NMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 80.PC12		EEPROM SDA   ��©���  ����̬
		
		{.enPort = PortE, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 97.PE0  		E2-WC1	��1ƬEEPROMд����	����Ч
		{.enPort = PortE, Pin10, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},  // 98.PE1  		E2-WC2 	��2ƬEEPROMд����	����Ч
		
		{.enPort = PortD, Pin11, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 97.PE0  		���Ͳ���������˸��
		{.enPort = PortA, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},  // 98.PE1  		������˸��
		
		//��ʱ������
		// {.enPort = PortC, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_MID_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},  // 98.PE1  		E2-WC2 	��2ƬEEPROMд����	����Ч
	};

//SPI��λ�ܽ�����
const port_init_t TSPISetOI_Config[eCOMPONENT_TOTAL_NUM][4] =
	{
		// eCOMPONENT_SPI_sample
		{
			{.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.PB10		SPI_MISO	sample
			{.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	sample
			{.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	sample
			{.enPort = PortD, Pin01, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS		sample
		},
		// eCOMPONENT_SPI_HSDC1
		{
			{.enPort = PortC, Pin05, Func44_Spi2_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI1
			{.enPort = PortC, Pin04, Func43_Spi2_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI1
			{.enPort = PortB, Pin12, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS		HSPI1
		},
		// eCOMPONENT_SPI_HSDC2
		{
			{.enPort = PortB, Pin05, Func47_Spi3_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI2
			{.enPort = PortB, Pin04, Func46_Spi3_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI2
			{.enPort = PortB, Pin15, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS		HSPI2
		},
		// eCOMPONENT_SPI_HSDC3
		{
			{.enPort = PortC, Pin07, Func41_Spi4_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI3
			{.enPort = PortC, Pin06, Func40_Spi4_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI3
			{.enPort = PortD, Pin00, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS		HSPI3
		},
		// eCOMPONENT_SPI_HSDC4
		{
			{.enPort = PortA, Pin09, Func48_Spi6_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI4
			{.enPort = PortA, Pin08, Func46_Spi6_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI4
			{.enPort = PortA, Pin11, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.PE14		SPI_CS		HSPI4
		},
		// esam
		{
			{.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.		SPI_MISO	
			{.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	
			{.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	
			{.enPort = PortE, Pin02, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.		SPI_CS		
		},
		// flash
		{
			{.enPort = PortB, Pin02, Func42_Spi1_Miso, HIGH, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL}, // 47.		SPI_MISO	
			{.enPort = PortB, Pin01, Func41_Spi1_Mosi, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.		SPI_MOSI	
			{.enPort = PortB, Pin00, Func40_Spi1_Sck, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.		SPI_SCLK	
			{.enPort = PortC, Pin10, Func0_Gpio, HIGH, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL},		 // 45.		SPI_CS		
		},

};
//SPI��λ�ܽ����� �ӵ�Դ���ƣ������ϵ��ʼ����Դ���Ʋ���
//��Դ��ʼ��Ϊ��ͨIO�ڣ�ֻ�ڸ�λ��ʼ����������λ���ظ�
const port_init_t TSPIResetOI_Config[eCOMPONENT_TOTAL_NUM][5] =
	{
		// eCOMPONENT_SPI_sample
		{
			{.enPort = PortB, Pin02, Func0_Gpio, LOW, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 47.PB10		SPI_MISO	sample
			{.enPort = PortB, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	sample
			{.enPort = PortB, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	sample
			{.enPort = PortD, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		sample
			{.enPort = PortE, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		sample��Դ����
		},
		// HSPI1
		{
			{.enPort = PortC, Pin05, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI2
			{.enPort = PortC, Pin04, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI2
			{.enPort = PortB, Pin12, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI2
		},
		// HSPI2
		{
			{.enPort = PortB, Pin05, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI2
			{.enPort = PortB, Pin04, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI2
			{.enPort = PortB, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI2
		},
		// HSPI3
		{
			{.enPort = PortC, Pin07, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI2
			{.enPort = PortC, Pin06, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI2
			{.enPort = PortD, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI2
		},
		// HSPI4
		{
			{.enPort = PortA, Pin09, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	HSPI2
			{.enPort = PortA, Pin08, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	HSPI2
			{.enPort = PortA, Pin11, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		HSPI2
		},
		// esam
		{
			{.enPort = PortB, Pin02, Func0_Gpio, LOW, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 47.PB10		SPI_MISO	
			{.enPort = PortB, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	
			{.enPort = PortB, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	
			{.enPort = PortE, Pin02, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		
			{.enPort = PortB, Pin03, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		��Դ����
		},
		// flash
		{
			{.enPort = PortB, Pin02, Func0_Gpio, LOW, PIN_DIR_IN, PIN_OUT_TYPE_NMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_ON, DISABLE, DISABLE, PIN_IN_TYPE_CMOS, PIN_ATTR_DIGITAL},  // 47.PB10		SPI_MISO	
			{.enPort = PortB, Pin01, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 46.PE15		SPI_MOSI	
			{.enPort = PortB, Pin00, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 44.PE13		SPI_SCLK	
			{.enPort = PortC, Pin10, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 45.PE14		SPI_CS		
			{.enPort = PortA, Pin15, Func0_Gpio, LOW, PIN_DIR_OUT, PIN_OUT_TYPE_CMOS, PIN_HIGH_DRV, DISABLE, PIN_PU_OFF, DISABLE, DISABLE, PIN_IN_TYPE_SMT, PIN_ATTR_DIGITAL}, // 43.PE12		��Դ����
		},

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
static const WORD TAB_Baud[] = {0x8F5B, 0x47AC, 0x23D6, 0x11EB, 0x08F5, 0x05F9, 0x047A, 0x023C, 0x011E, 0x00BE, 0x005F};

static volatile WORD SysTickCounter;
WORD HSDCTimer = 0;
static WORD Pulse_Timer = 0; //�й������ȼ�ʱ��
BYTE CommLed_Time = 0;		 //ͨ�ŵ���ʱ��
BYTE CanErrTime;
//-----------------------------------------------
//				��������
//-----------------------------------------------
extern void IrPWMEnable(BOOL enNewSta);
//api_GetSAGStatus

//uartͨ��dma���Ͳ���������غ���
//-----------------------------------------------
//��������:��������DMA��������
//
//����: Buf ������    len ����
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void UartSendDmaConfig(BYTE *Buf, WORD len)
{
	DMA_ChCmd(UART_DMA_UNIT, UART_DMA_CH, DISABLE);
	DMA_SetSrcAddr(UART_DMA_UNIT, UART_DMA_CH, (uint32_t)(&Buf[0]));
	DMA_SetTransCount(UART_DMA_UNIT, UART_DMA_CH, len);
	/* Enable DMA channel */
	DMA_ChCmd(UART_DMA_UNIT, UART_DMA_CH, ENABLE);
}

//-----------------------------------------------
//��������:uartͨ��dma���Ͳ���������ɻص�����
//
//����: Buf ������    len ����
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
static void DmaTcIrqCallback(void)
{
	static DWORD lastNo = 0;
	while (RESET == USART_GetStatus(WAVE_DATA_UART_CH, USART_FLAG_TX_CPLT))
	{
	} //���dma���������һ�ֽ�����
	USART_FuncCmd(WAVE_DATA_UART_CH, USART_TX, DISABLE);
	// DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, TrnCpltIrq);
	DMA_ClearTransCompleteStatus(UART_DMA_UNIT, TX_DMA_TC_FLAG);
	if((SendFrameCnt != 1) && (SendFrameCnt != 0))
	{
		if(lastNo != (SendFrameCnt - 1))
		{
			SendDataLostFlag = 1;
		}
	}
	lastNo = SendFrameCnt;
	// DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, BlkTrnCpltIrq);
}

//-----------------------------------------------
//��������: ���� dma��������жϳ�ʼ��
//
//����:
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void DmaIrqInit(void)
{
	stc_irq_signin_config_t stcIrqSignConfig;

	stcIrqSignConfig.enIntSrc = UART_DMA_TC_INT_SRC;
	stcIrqSignConfig.enIRQn = (IRQn_Type)UART_DMA_TC_INT_IRQn;
	stcIrqSignConfig.pfnCallback = &DmaTcIrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSignConfig);
	NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
	NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_00);
	NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
	AOS_SetTriggerEventSrc(TX_DMA_TRIG_SEL, UART_DMA_TRG_SEL);
	DMA_TransCompleteIntCmd(UART_DMA_UNIT, TX_DMA_TC_INT, ENABLE);
}
//-----------------------------------------------
//��������:���Ͳ������ݴ���dma��ʼ��
//
//����: 
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void UartSendDmaInit(void)
{
	stc_dma_init_t stcDmaInit;
	USART_FuncCmd(WAVE_DATA_UART_CH, USART_TX, DISABLE); 
	TX_DMA_FCG_ENABLE;
	/* Enable peripheral clock */
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);
	/* Initialize DMA. */
	MEM_ZERO_STRUCT(stcDmaInit);
	(void)DMA_StructInit(&stcDmaInit);
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;
	stcDmaInit.u32BlockSize = 1UL;
	// stcDmaInit.u32TransCount = ARRAY_SZ(m_au8RxBuf);
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
	stcDmaInit.u32DestAddr = (uint32_t)(&WAVE_DATA_UART_CH->TDR);
//	 stcDmaInit.u32SrcAddr = (uint32_t)0x20007310;
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
	DMA_Init(UART_DMA_UNIT, UART_DMA_CH, &stcDmaInit);
	AOS_SetTriggerEventSrc(TX_DMA_TRIG_SEL, UART_DMA_TRG_SEL);
	DmaIrqInit();
	/* Enable DMA. */
	DMA_Cmd(UART_DMA_UNIT, ENABLE);
}
//-----------------------------------------------
//��������: ���� dma���Ͳ�������
//
//����:		Buf ����; len ����
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void UartSendWaveDataByDma(BYTE *Buf, WORD len)
{
	static WORD count = 0;
	
	count++;
	if( count >= TX_WAVE_LED_TOGGLE_CNT )
	{
		count = 0;
		TX_WAVE_LED_TOGGLE;
	}
	UartSendDmaConfig(Buf, len);
	USART_FuncCmd(WAVE_DATA_UART_CH, USART_TX, ENABLE); // ����ʹ��
}

//-----------------------------------------------
//��������: ����������DMA��ʼ��
//
//����:
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void TopWaveSpiDmaConfig(TWaveDataConf *pWaveDataConf)
{
	stc_dma_init_t stcDmaInit;
	(void)DMA_StructInit(&stcDmaInit);

	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1 | FCG0_PERIPH_AOS, ENABLE);
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2 | FCG0_PERIPH_AOS, ENABLE);

	/* Configure TX DMA */
	stcDmaInit.u32BlockSize = 1UL;
	stcDmaInit.u32TransCount = pWaveDataConf->DmaConf.BufSize;
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
	stcDmaInit.u32SrcAddr = (uint32_t)(pWaveDataConf->DmaConf.RecBuf);
	stcDmaInit.u32DestAddr = (uint32_t)(&(pWaveDataConf->DmaConf.SPIInstance)->DR);
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;

	if (LL_OK != DMA_Init(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaTxChannel, &stcDmaInit))
	{
		return;
	}
	AOS_SetTriggerEventSrc(pWaveDataConf->DmaConf.DmaTxTrigCh, pWaveDataConf->DmaConf.TxTrigSrc);

	/* Configure RX DMA */
	stcDmaInit.u32BlockSize = 1UL;
	stcDmaInit.u32TransCount = pWaveDataConf->DmaConf.BufSize;
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
	stcDmaInit.u32SrcAddr = (uint32_t)(&(pWaveDataConf->DmaConf.SPIInstance)->DR);
	stcDmaInit.u32DestAddr = (uint32_t)(pWaveDataConf->DmaConf.RecBuf);
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;

	if (LL_OK != DMA_Init(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaRxChannel, &stcDmaInit))
	{
		return;
	}
	AOS_SetTriggerEventSrc(pWaveDataConf->DmaConf.DmaRxTrigCh, pWaveDataConf->DmaConf.RxTrigSrc);

	/* ʹ��DMA��ͨ�� */
	DMA_Cmd(pWaveDataConf->DmaConf.DmaInstance, ENABLE);
}

//-----------------------------------------------
//��������: ����������DMAʹ��
//
//����:
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void TopWaveSpiDmaEnableInit(TWaveDataConf *pWaveDataConf)
{
	if (pWaveDataConf == NULL)
	{
		return;
	}
	DMA_ChCmd(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaTxChannel, DISABLE);
	DMA_ChCmd(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaRxChannel, DISABLE);

	(pWaveDataConf->DmaConf.SPIInstance)->SR = 0;
	SPI_Cmd(pWaveDataConf->DmaConf.SPIInstance, DISABLE);

	DMA_ClearTransCompleteStatus(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DMA_Rx_FLAG);
	DMA_ClearTransCompleteStatus(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DMA_Tx_FLAG);

	DMA_SetDestAddr(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaRxChannel, (uint32_t)(pWaveDataConf->DmaConf.RecBuf));
	DMA_SetTransCount(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaRxChannel, pWaveDataConf->DmaConf.BufSize);

	DMA_SetSrcAddr(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaTxChannel, (uint32_t)(pWaveDataConf->DmaConf.RecBuf));
	DMA_SetTransCount(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaTxChannel, pWaveDataConf->DmaConf.BufSize);

	/* ENABLE DMA channel */
	DMA_ChCmd(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaTxChannel, ENABLE);
	DMA_ChCmd(pWaveDataConf->DmaConf.DmaInstance, pWaveDataConf->DmaConf.DmaRxChannel, ENABLE);

	(pWaveDataConf->DmaConf.SPIInstance)->SR = 0;
	SPI_Cmd(pWaveDataConf->DmaConf.SPIInstance, ENABLE);
}

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
BOOL api_CheckSysVol(eDETECT_POWER_MODE Type)
{
	static BYTE bLoopCount = 0;
	WORD wSystemVol = 0;
	BYTE i = 0;

	api_SetSysStatus(eSYS_STATUS_POWER_ON); //Ŀǰû��ad���
	return TRUE;							//Ŀǰû��ad���
	// // �����м�⣬POWER_CHECK_LOOP_COUNT�ν�1��
	// if (Type == eOnRunDetectPowerMode)
	// {
	// 	bLoopCount++;
	// 	if ((bLoopCount % POWER_CHECK_LOOP_COUNT) == 0)
	// 	{
	// 		bLoopCount = 0;
	// 	}
	// 	else
	// 	{
	// 		return TRUE;
	// 	}
	// }

	// if ((Type == eOnInitDetectPowerMode) || (Type == eSleepDetectPowerMode))
	// {
	// 	for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
	// 	{
	// 		wSystemVol = api_GetADData(eOther_AD_PWR);

	// 		if (SYSTEM_POWER_VOL(wSystemVol) < SYSTEM_VOL_MIN_UP) // ϵͳ��Դ�ж��Ƿ��ϵ�
	// 		{
	// 			break;
	// 		}
	// 	}

	// 	// �ϵ��⣬POWER_CHECK_CONTINUE_COUNT�ζ��е����Ϊ�е�
	// 	if (i >= POWER_CHECK_CONTINUE_COUNT)
	// 	{
	// 		api_SetSysStatus(eSYS_STATUS_POWER_ON);
	// 		return TRUE;
	// 	}
	// 	else
	// 	{
	// 		api_ClrSysStatus(eSYS_STATUS_POWER_ON);
	// 		return FALSE;
	// 	}
	// }
	// else if ((Type == eOnRunDetectPowerMode) || (Type == eOnRunSpeedDetectPowerMode))
	// {
	// 	for (i = 0; i < POWER_CHECK_CONTINUE_COUNT; i++)
	// 	{
	// 		wSystemVol = api_GetADData(eOther_AD_PWR);

	// 		if (SYSTEM_POWER_VOL(wSystemVol) > SYSTEM_VOL_MIN_DOWN) // ϵͳ��Դ�ж��Ƿ��ϵ�
	// 		{
	// 			break;
	// 		}
	// 	}

	// 	if (i >= POWER_CHECK_CONTINUE_COUNT) //����
	// 	{
	// 		api_ClrSysStatus(eSYS_STATUS_POWER_ON);
	// 		return FALSE;
	// 	}
	// 	else
	// 	{
	// 		api_SetSysStatus(eSYS_STATUS_POWER_ON);
	// 		return TRUE;
	// 	}
	// }
	// return TRUE;
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

	if (Type == ePowerOnMode)
	{
		//  GPIO_SetDebugPort(GPIO_PIN_TDO, DISABLE);
		//  GPIO_SetDebugPort(GPIO_PIN_TRST, DISABLE);
		GPIO_SetDebugPort(GPIO_PIN_DEBUG_JTAG, DISABLE);

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
		for (i = 0; i < (sizeof(TGPIO_Config) / sizeof(port_init_t)); i++)
		{
			if (TGPIO_Config[i].FuncReg != GPIO_DEFAULT)
			{
				GPIO_Init(TGPIO_Config[i].enPort, TGPIO_Config[i].u16Pin, &TGPIO_Config[i].GpioReg);
				if (TGPIO_Config[i].GpioReg.u16PinDir == PIN_DIR_OUT) //�������״̬
				{
					if (TGPIO_Config[i].GpioReg.u16PinState == HIGH)
					{
						GPIO_SetPins(TGPIO_Config[i].enPort, TGPIO_Config[i].u16Pin);
					}
					else if (TGPIO_Config[i].GpioReg.u16PinState == LOW)
					{
						GPIO_ResetPins(TGPIO_Config[i].enPort, TGPIO_Config[i].u16Pin);
					}
					else
					{
					}
				}
				if (TGPIO_Config[i].FuncReg != Func0_Gpio)
				{
					GPIO_SetFunc(TGPIO_Config[i].enPort, TGPIO_Config[i].u16Pin, (en_port_func_t)TGPIO_Config[i].FuncReg);
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
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2); //����Ϊ2λ��ռ���ȼ���2λ��Ӧ���ȼ�
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
short GetADValue(BYTE Mode, BYTE Type)
{
	WORD wTemp;
	float fTemp;

	if (Type != SYS_CLOCK_VBAT_AD)
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
	if (abs(BatteryPara.ClockBatteryOffset) < (BYTE)(ClockBatteryStandardVoltage * 15 / 100)) //У׼ƫ�ã��������ֵ0.5V ����5%�ĵ�����2������������10%������15%���п��� 364*0.12= 43
	{
		wTemp += BatteryPara.ClockBatteryOffset;
	}
	if (wTemp > 368)
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
void PowerCtrl(BYTE Type)
{
	if (Type == 0)
	{
		//�򿪵�Դ
		//POWER_EEPROM_OPEN;
		POWER_BLUETOOTH_OPEN;
		POWER_FLASH_OPEN;
		if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE)
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
void ESAMSPIPowerDown(ePOWER_MODE PowerMode)
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
BOOL ResetSPIDevice(eCOMPONENT_TYPE Type, BYTE Time)
{
	BYTE i;

	if (Type >= eCOMPONENT_TOTAL_NUM)
	{
		return FALSE;
	}
    LL_PERIPH_WE(LL_PERIPH_GPIO);
	for (i = 0; i < (sizeof(TSPIResetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if ((TSPIResetOI_Config[Type][i].FuncReg != GPIO_DEFAULT) && (TSPIResetOI_Config[Type][i].u16Pin != 0))//pinΪ0 ��ʾ�����û�г�ʼ����Ӧֵ
		{
			GPIO_SetFunc(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin, (en_port_func_t)TSPIResetOI_Config[Type][i].FuncReg);
			GPIO_Init(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin, &TSPIResetOI_Config[Type][i].GpioReg);
			if (TSPIResetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //�������״̬
			{
				if (TSPIResetOI_Config[Type][i].GpioReg.u16PinState == HIGH)
				{
					GPIO_SetPins(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin);
				}
				else if (TSPIResetOI_Config[Type][i].GpioReg.u16PinState == LOW)
				{
					GPIO_ResetPins(TSPIResetOI_Config[Type][i].enPort, TSPIResetOI_Config[Type][i].u16Pin);
				}
				else
				{
				}
			}
		}
	}

	if (Type == eCOMPONENT_SPI_FLASH) // pe14 �ر�flash CS
	{
		// FLASH_CS_DISABLE;
		POWER_FLASH_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_FLASH_OPEN;
		api_Delayms(Time);
		// FLASH_CS_ENABLE;
	}
	else if (Type == eCOMPONENT_SPI_HSDC1) // �رռ���оƬ CS,ͨ���������ݵ�spi����оƬ
	{

		// SAMPLE_CS_DISABLE;
		POWER_SAMPLE1_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE1_OPEN;
		api_Delayms(Time);
		// SAMPLE_CS_ENABLE;
	}
	else if (Type == eCOMPONENT_SPI_HSDC2) // �رռ���оƬ CS
	{

		// SAMPLE_CS_DISABLE;
		POWER_SAMPLE2_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE2_OPEN;
		api_Delayms(Time);
		// SAMPLE_CS_ENABLE;
	}
	else if (Type == eCOMPONENT_SPI_HSDC3) // �رռ���оƬ CS
	{

		// SAMPLE_CS_DISABLE;
		POWER_SAMPLE3_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE3_OPEN;
		api_Delayms(Time);
		// SAMPLE_CS_ENABLE;
	}
	else if (Type == eCOMPONENT_SPI_HSDC4) // �رռ���оƬ CS
	{
		// SAMPLE_CS_DISABLE;
		POWER_SAMPLE4_CLOSE; // ���´򿪵�Դ
		api_Delayms(Time);
		POWER_SAMPLE4_OPEN;
		api_Delayms(Time);
		// SAMPLE_CS_ENABLE;
	}
	for (i = 0; i < (sizeof(TSPISetOI_Config[Type]) / sizeof(port_init_t)); i++)
	{
		if ((TSPISetOI_Config[Type][i].FuncReg != GPIO_DEFAULT) && (TSPISetOI_Config[Type][i].u16Pin != 0))
		{
			GPIO_SetFunc(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin, (en_port_func_t)TSPISetOI_Config[Type][i].FuncReg);
			GPIO_Init(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin, &TSPISetOI_Config[Type][i].GpioReg);
			if (TSPISetOI_Config[Type][i].GpioReg.u16PinDir == PIN_DIR_OUT) //�������״̬
			{
				if (TSPISetOI_Config[Type][i].GpioReg.u16PinState == HIGH)
				{
					GPIO_SetPins(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin);
				}
				else if (TSPISetOI_Config[Type][i].GpioReg.u16PinState == LOW)
				{
					GPIO_ResetPins(TSPISetOI_Config[Type][i].enPort, TSPISetOI_Config[Type][i].u16Pin);
				}
				else
				{
				}
			}
		}
	}

	if (Type == eCOMPONENT_SPI_SAMPLE)
	{
		api_InitSPI(eCOMPONENT_SPI_SAMPLE, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_HSDC1)
	{
		api_InitSPI(eCOMPONENT_SPI_HSDC1, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_HSDC2)
	{
		api_InitSPI(eCOMPONENT_SPI_HSDC2, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_HSDC3)
	{
		api_InitSPI(eCOMPONENT_SPI_HSDC3, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_HSDC4)
	{
		api_InitSPI(eCOMPONENT_SPI_HSDC4, eSPI_MODE_3);
	}
	else if (Type == eCOMPONENT_SPI_FLASH)
	{
		api_InitSPI(eCOMPONENT_SPI_FLASH, eSPI_MODE_3);
	}
	//	else
	//	{
	//		return FALSE;
	//	}

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
void InitBoard(void)
{
	BYTE i;
	WORD PowerUpStatus = 0;

	//	initTestPin(); //��ʱ������

	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnInitEnterDownMode);
	}

	CLEAR_WATCH_DOG;

	//�˿ڳ�ʼ��
	// ResetSPIDevice(eCOMPONENT_SPI_ESAM, 100);	// ��esam���и�λ
	ResetSPIDevice(eCOMPONENT_SPI_SAMPLE, 200); //�Լ���оƬ���и�λ
	ResetSPIDevice( eCOMPONENT_SPI_FLASH, 5 ); //��Flash���и�λ

	InitPort(ePowerOnMode); //������ʼ��

#if (SEL_TOPOLOGY == YES)
	for (i = 0; i < SAMPLE_CHIP_NUM; i++)
	{
		api_TopWaveSpiInit(i); // ����΢�ظ������ȳ�ʼ��Ƭѡ�ٳ�ʼ��SPI��DMA��Ƭѡ��InitPort�г�ʼ�������Է���InitPort�±�
	}
#endif

	PowerCtrl(0);

	// ESAMSPIPowerDown(ePowerOnMode); // �ر�ESAM��Դ��CS��SPI�ܽ�����Ϊ��ͨ

	// ADC_Config();

	InitINT(); //�رղ��õ��ж�

	//��ʼ����ʱ��
	//InitTimer();

	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnInitEnterDownMode);
	}

	//����ģ���ϵ��ʼ����ע�ⲻҪ����˳��
	api_PowerUpFlag();
	//api_PowerUpRtc�ж�RTC�ĸ�λ���� ��λ�������������RTC��TBS��ؼĴ��� �����Ҫ���µ���һ��ADC����
	api_PowerUpRtc();
	//ADC_Config(); //!!!!�ر�ע�� �˴�ADC_Config����ɾ - yxk

	for (i = 0; i < 16; i++) 
	{
		PowerUpStatus |= RMU_GetStatus(RMU_FLAG_PWR_ON << i) << i; 
	}
	api_WriteFreeEvent(EVENT_SYS_START, PowerUpStatus);
	RMU_ClearStatus(); //�����־

	api_PowerUpPara();
	api_PowerUpSave();
	api_PowerUpWaveTxRx();
		
	// api_PowerUpLcd(ePowerOnMode);

	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnInitEnterDownMode);
	}

	CLEAR_WATCH_DOG;
	for(i = 0;i < SAMPLE_CHIP_NUM;i++)
	{
		api_PowerUpSample(i);
	}

	api_PowerUpProtocol();
	// api_PowerUpCanBus();
	//ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnInitEnterDownMode);
	}

	// api_PowerUpPrePay();
	// PowerUpRelay();

	// api_PowerUpEnergy();
	// api_PowerUpFreeze();

	//���ϵ粹�������е��磬�������͹���
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnInitEnterDownMode);
	}
	// api_ReflashFreeze_Ram();
#if (SEL_SEARCH_METER == YES)
//	api_PowerUpSchClctMeter();
#endif

#if (SEL_TOPOLOGY == YES)
// TopoPowerUp();
#endif

	// api_PowerUpEvent();
	for(i = 0;i < SAMPLE_CHIP_NUM;i++)
	{
		api_PowerUpSysWatch(i); // ��ˢ�������ϱ�ģʽ�ֺ���е�ص�ѹ�ȵ�����ж�-��event�ϵ紦������˳���ϵ-���ĺ�
	}
#if (MAX_PHASE == 3)
	// api_PowerUpDemand();
#endif

	// api_PowerUpVersion();

	// ϵͳ��ѹ�Լ죬ȷ��ϵͳ��ѹΪȷ��״̬
	if (api_CheckSysVol(eOnInitDetectPowerMode) == FALSE)
	{
		api_EnterLowPower(eFromOnRunEnterDownMode); //�ڴ�֮ǰ�ϵ��ѳ�ʼ����� �˴���⵽�͹��İ��������紦�� ���ڽ��������絫��Ϊ���쳣��λ������
	}

	// api_MidLayerInit();

#if (SEL_EVENT_DI_CHANGE == YES)
	// api_PowerUpDI();
#endif

	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)//ֻʹ��ǰ�������ڣ��޸ĺ궨��MAX_COM_CHANNEL_NUMҪ�ĵ�̫��
	{
		SerialMap[i].SCIInit(SerialMap[i].SCI_Ph_Num);
	}

	//��ʼ��_noinit_��־
	api_InitNoInitFlag();
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
	if (Do == FALSE)
	{
		switch (No)
		{
		case CS_SPI_ESAM:
			ESAM_CS_DISABLE;
			break;
		case CS_SPI_FLASH: //����SPI,ֻ��һ��CS��Ч
			FLASH_CS_DISABLE;
			break;
		case CS_SPI_SAMPLE1:
			SAMPLE1_CS_DISABLE;
			break;
		case CS_SPI_SAMPLE2:
			SAMPLE2_CS_DISABLE;
			break;
		case CS_SPI_SAMPLE3:
			SAMPLE3_CS_DISABLE;
			break;
		case CS_SPI_SAMPLE4:
			SAMPLE4_CS_DISABLE;
			break;
		default:
			break;
		}
		return;
	}

	switch (No)
	{
	case CS_SPI_ESAM:
		ESAM_CS_ENABLE;
		break;
	case CS_SPI_FLASH:
		FLASH_CS_ENABLE;
		break;
	case CS_SPI_SAMPLE1:
		SAMPLE1_CS_ENABLE;
		break;
	case CS_SPI_SAMPLE2:
		SAMPLE2_CS_ENABLE;
		break;
	case CS_SPI_SAMPLE3:
		SAMPLE3_CS_ENABLE;
		break;
	case CS_SPI_SAMPLE4:
		SAMPLE4_CS_ENABLE;
		break;
	default:
		break;
	}
	api_Delay10us(1);

	return;
}

void UART_IRQ_Err_Service(void)
{
	// UART1
	if (SET == USART_GetStatus(CM_USART1, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART1, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART1, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART1, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART1, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART1, USART_FLAG_OVERRUN);
	}

	// UART2
	if (SET == USART_GetStatus(CM_USART2, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART2, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART2, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART2, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART2, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART2, USART_FLAG_OVERRUN);
	}

	// UART3
	if (SET == USART_GetStatus(CM_USART3, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART3, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART3, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART3, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART3, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART3, USART_FLAG_OVERRUN);
	}

	// UART4
	if (SET == USART_GetStatus(CM_USART4, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART4, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART4, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART4, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART4, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART4, USART_FLAG_OVERRUN);
	}

	// UART5
	if (SET == USART_GetStatus(CM_USART5, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART5, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART5, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART5, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART5, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART5, USART_FLAG_OVERRUN);
	}

	// UART6
	if (SET == USART_GetStatus(CM_USART6, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART6, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART6, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART6, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART6, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART6, USART_FLAG_OVERRUN);
	}

	// UART7
	if (SET == USART_GetStatus(CM_USART7, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART7, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART7, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART7, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART7, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART7, USART_FLAG_OVERRUN);
	}

	// UART8
	if (SET == USART_GetStatus(CM_USART8, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART8, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART8, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART8, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART8, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART8, USART_FLAG_OVERRUN);
	}

	// UART9
	if (SET == USART_GetStatus(CM_USART9, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART9, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART9, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART9, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART9, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART9, USART_FLAG_OVERRUN);
	}

	// UART10
	if (SET == USART_GetStatus(CM_USART10, USART_FLAG_FRAME_ERR))
	{
		USART_ClearStatus(CM_USART10, USART_FLAG_FRAME_ERR);
	}

	if (SET == USART_GetStatus(CM_USART10, USART_FLAG_PARITY_ERR))
	{
		USART_ClearStatus(CM_USART10, USART_FLAG_PARITY_ERR);
	}

	if (SET == USART_GetStatus(CM_USART10, USART_FLAG_OVERRUN))
	{
		USART_ClearStatus(CM_USART10, USART_FLAG_OVERRUN);
	}
} // Rx
void UART1_IRQ_Rec_Service(void)
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
void UART5_IRQ_Rec_Service(void)
{
	UART5_IRQ_Service();
}
void UART6_IRQ_Rec_Service(void)
{
	UART6_IRQ_Service();
}
void UART7_IRQ_Rec_Service(void)
{
	UART7_IRQ_Service();
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
void UART5_IRQ_Tx_Service(void)
{
	UART5_IRQ_Service();
}
void UART6_IRQ_Tx_Service(void)
{
	UART6_IRQ_Service();
}
void UART7_IRQ_Tx_Service(void)
{
	UART7_IRQ_Service();
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
void UART5_IRQ_TXCMPLT_Service(void)
{
	UART5_IRQ_Service();
}
void UART6_IRQ_TXCMPLT_Service(void)
{
	UART6_IRQ_Service();
}
void UART7_IRQ_TXCMPLT_Service(void)
{
	UART7_IRQ_Service();
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
	if (i >= MAX_COM_CHANNEL_NUM)
	{
		ASSERT(0, 0);
		return;
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
	if (USART_GetStatus(CM_USART1, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART1);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART1_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART1, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART1->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART1_NUM, CM_USART1);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART1, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART1->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART1, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART1, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

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
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART2, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART2);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART2_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART2, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART2->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART2_NUM, CM_USART2);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART2, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART2->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART2, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART2, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

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
	if (USART_GetStatus(CM_USART3, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART3);
		USARTx_Rcv_IRQHandler(SCI_MCU_USART3_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART3, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART3->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART3_NUM, CM_USART3);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART3, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART3->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART3, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART3, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

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
	if (USART_GetStatus(CM_USART4, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART4);
		USARTx_Rcv_IRQHandler(SCI_MCU_USART4_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART4, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART4->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART4_NUM, CM_USART4);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART4, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART4->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART4, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART4, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART4_NUM);
		}
	}
}

void UART5_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART5, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART5);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART5_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART5, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART5->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART5_NUM, CM_USART5);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART5, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART5->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART5, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART5, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART5_NUM);
		}
	}
}

void UART6_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART6, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART6);
		USARTx_Rcv_IRQHandler(SCI_MCU_USART6_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART6, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART6->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART6_NUM, CM_USART6);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART6, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART6->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART6, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART6, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART6_NUM);
		}
	}
}

void UART7_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART7, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART7);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART7_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART7, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART7->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART7_NUM, CM_USART7);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART7, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART7->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART7, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART7, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART7_NUM);
		}
	}
}

void UART8_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART8, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART8);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART8_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART8, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART8->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART8_NUM, CM_USART8);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART8, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART8->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART8, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART8, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART8_NUM);
		}
	}
}

void UART9_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART9, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART9);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART9_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART9, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART9->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART9_NUM, CM_USART9);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART9, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART9->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART9, USART_TX, DISABLE);		  //��ֹ����
			USART_FuncCmd(CM_USART9, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART9_NUM);
		}
	}
}

void UART10_IRQ_Service(void)
{
	uint8_t ch, j;

	// ������
	if (USART_GetStatus(CM_USART10, USART_FLAG_RX_FULL) != RESET)
	{
		ch = USART_ReadData(CM_USART10);

		USARTx_Rcv_IRQHandler(SCI_MCU_USART10_NUM, ch);
	}
	// ���ͼĴ�����
	if (USART_GetStatus(CM_USART10, USART_FLAG_TX_EMPTY) != RESET)
	{
		if ((CM_USART10->CR1 & USART_CR1_TXEIE) != TRESET)
		{
			USARTx_Send_IRQHandler(SCI_MCU_USART10_NUM, CM_USART10);
		}
	}
	// �������
	if (USART_GetStatus(CM_USART10, USART_FLAG_TX_CPLT) != RESET)
	{
		if (((CM_USART10->CR1 & USART_CR1_TCIE) != TRESET))
		{
			// �ر��ж�
			USART_FuncCmd(CM_USART10, USART_TX, DISABLE);		   //��ֹ����
			USART_FuncCmd(CM_USART10, USART_INT_TX_CPLT, DISABLE); //��ֹ��������ж�

			//����в����ʸĶ� �������ݸ��²����� ����ÿ���������� ������485_1����485_2�����
			for (j = 0; j < MAX_COM_CHANNEL_NUM; j++)
			{
				DealSciFlag(j);
			}
			SendToRec(SCI_MCU_USART10_NUM);
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
	uint32_t u32Baud, u32Fcg1Periph;
	BYTE BpsBak, IntStatus, Bps, i;
	stc_irq_signin_config_t stcIrqRegiCfg;

	stc_usart_uart_init_t stcInitCfg = {
		USART_CLK_SRC_INTERNCLK,
		USART_CLK_DIV4,
		USART_CK_OUTPUT_DISABLE,
		9600UL,
		USART_DATA_WIDTH_8BIT,
		USART_STOPBIT_1BIT,
		USART_PARITY_EVEN,
		USART_OVER_SAMPLE_8BIT,
		USART_FIRST_BIT_LSB,
		USART_START_BIT_FALLING,
		USART_HW_FLOWCTRL_NONE,
	};

	if (SciPhNum == SCI_MCU_USART1_NUM)
	{
		u32Fcg1Periph = FCG3_PERIPH_USART1;
	}
	else if (SciPhNum == SCI_MCU_USART3_NUM)
	{
		u32Fcg1Periph = FCG3_PERIPH_USART3;
		//PORT_DebugPortSetting(GPIO_PIN_TRST, DISABLE);// PB4 RX
		//PORT_DebugPortSetting(GPIO_PIN_TDO, DISABLE);// PB3 �ز���λ
	}
	else if (SciPhNum == SCI_MCU_USART4_NUM)
	{
		u32Fcg1Periph = FCG3_PERIPH_USART4;
	}
	else if (SciPhNum == SCI_MCU_USART6_NUM)
	{
		u32Fcg1Periph = FCG3_PERIPH_USART6;
	}
	else if (SciPhNum == SCI_MCU_USART7_NUM)
	{
		u32Fcg1Periph = FCG3_PERIPH_USART7;
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
	if (i >= (sizeof(Uart_Config) / sizeof(SYS_UART_Config)))
	{
		ASSERT(0, 0);
		return FALSE;
	}

	//115200�����ʹر��˲�����
	BpsBak = api_GetBaudRate(0, SciPhNum);
	Bps = (BpsBak & 0x0f);
	if ((Bps > 9) || (Bps < 2))
	{
		//		USART_StopModeNoiseFilterCmd(Uart_Config[i].USART_CH, DISABLE);
	}
	else
	{
//		USART_StopModeNoiseFilterCmd(Uart_Config[i].USART_CH, ENABLE);
	}

	/* ENABLE peripheral clock */
	FCG_Fcg3PeriphClockCmd(u32Fcg1Periph, ENABLE);
	LL_PERIPH_WE(LL_PERIPH_GPIO);
	/* Initialize USART IO */
	GPIO_SetFunc(Uart_Config[i].IOConfig[0].IO.Port, Uart_Config[i].IOConfig[0].IO.Pin, Uart_Config[i].IOConfig[0].IO.IoFunc_x);
	GPIO_SetFunc(Uart_Config[i].IOConfig[1].IO.Port, Uart_Config[i].IOConfig[1].IO.Pin, Uart_Config[i].IOConfig[1].IO.IoFunc_x);

	/* Set USART RX IRQ */
	stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Rcv_IRQn;
	stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartRcvIrqCallback;
	stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Rcv_NUM;
	INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIO_00);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	/* Set USART RX error IRQ */
	stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Err_IRQn;
	stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartErrIrqCallback;
	stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Err_NUM;
	INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIO_00);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	/* Set USART TX IRQ */
	stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_IRQn;
	stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndIrqCallback;
	stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_NUM;
	INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIO_00);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	/* Set USART TX complete IRQ */
	stcIrqRegiCfg.enIRQn = Uart_Config[i].USART_Snd_Cmplt_IRQn;
	stcIrqRegiCfg.pfnCallback = Uart_Config[i].UsartSndCmpltIrqCallback;
	stcIrqRegiCfg.enIntSrc = (en_int_src_t)Uart_Config[i].USART_Snd_Cmplt_NUM;
	INTC_IrqSignIn(&stcIrqRegiCfg);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIO_00);
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
	// BpsBak = api_GetBaudRate(0, SciPhNum);
	IntStatus = api_splx(0);

	// ֹͣλ
	Bps = ((BpsBak >> 4) & 0x01);

	switch (Bps)
	{
	case 0x00: //1λֹͣλ
		stcInitCfg.u32StopBit = USART_STOPBIT_1BIT;
		break;
	case 0x01: //2λֹͣλ
		stcInitCfg.u32StopBit = USART_STOPBIT_2BIT;
		break;
	default: //Ĭ��1λֹͣλ
		stcInitCfg.u32StopBit = USART_STOPBIT_1BIT;
		break;
	}

	//У��
	Bps = ((BpsBak >> 5) & 0x03);
	switch (Bps)
	{
	case 0x00: //��У��
		stcInitCfg.u32Parity = USART_PARITY_NONE;
		break;
	case 0x01: //����У�飬��У��
		stcInitCfg.u32Parity = USART_PARITY_ODD;
		break;
	case 0x02: //����У�飬żУ��
		stcInitCfg.u32Parity = USART_PARITY_EVEN;
		break;
	default: //Ĭ������У�飬żУ��
		stcInitCfg.u32Parity = USART_PARITY_EVEN;
		break;
	}

	// // �̻��뱾��ͨ�ŵ�UART�Լ������� һλֹͣλ��żУ��
	// if(SciPhNum == SCI_MCU_USART3_NUM)
	// {
	// 	stcInitCfg.enStopBit = UsartOneStopBit;
	// 	stcInitCfg.enParity = UsartParityEven;
	// }
	Bps = BpsBak & 0x0f;

	if (Bps <= 6) //������С�ڵ���9600ʱ 64��Ƶ
	{
		stcInitCfg.u32ClockDiv = USART_CLK_DIV64;
	}
	USART_UART_Init(Uart_Config[i].USART_CH, &stcInitCfg, NULL);

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

	stcInitCfg.u32Baudrate = u32Baud;
	USART_SetBaudrate(Uart_Config[i].USART_CH, u32Baud, NULL);
	/*ENABLE RX && RX interupt && timeout interrupt function*/
	USART_FuncCmd(Uart_Config[i].USART_CH, USART_RX, ENABLE);
	USART_FuncCmd(Uart_Config[i].USART_CH, USART_INT_RX, ENABLE);

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
		// api_Delay100us(1); // ��ʱ100us		����ת���� ��ʱ������ ���һ�Ӱ�����ͨ��
		USART_FuncCmd(CM_USART1, USART_RX, ENABLE);
		USART_FuncCmd(CM_USART1, USART_INT_RX, ENABLE);
		break;
	case SCI_MCU_USART3_NUM:
		ENABLE_HARD_SCI_3_RECE;
		// api_Delay100us(1); // ��ʱ100us
		USART_FuncCmd(CM_USART3, USART_RX, ENABLE);
		USART_FuncCmd(CM_USART3, USART_INT_RX, ENABLE);
		break;
	case SCI_MCU_USART4_NUM:
		ENABLE_HARD_SCI_4_RECE;
		// api_Delay100us(1);// ��ʱ100us
		USART_FuncCmd(CM_USART4, USART_RX, ENABLE);
		USART_FuncCmd(CM_USART4, USART_INT_RX, ENABLE);
		break;
	case SCI_MCU_USART6_NUM:
		ENABLE_HARD_SCI_6_RECE;
		// api_Delay100us(1);// ��ʱ100us
		USART_FuncCmd(CM_USART6, USART_RX, ENABLE);
		USART_FuncCmd(CM_USART6, USART_INT_RX, ENABLE);
		break;
	case SCI_MCU_USART7_NUM:
		ENABLE_HARD_SCI_7_RECE;
		// api_Delay100us(1);// ��ʱ100us
		USART_FuncCmd(CM_USART7, USART_RX, ENABLE);
		USART_FuncCmd(CM_USART7, USART_INT_RX, ENABLE);
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
	case SCI_MCU_USART1_NUM:
		USART_FuncCmd(CM_USART1, USART_RX, DISABLE);
		USART_FuncCmd(CM_USART1, USART_INT_RX, DISABLE);
		break;
	case SCI_MCU_USART3_NUM:
		USART_FuncCmd(CM_USART3, USART_RX, DISABLE);
		USART_FuncCmd(CM_USART3, USART_INT_RX, DISABLE);
		break;
	case SCI_MCU_USART4_NUM:
		USART_FuncCmd(CM_USART4, USART_RX, DISABLE);
		USART_FuncCmd(CM_USART4, USART_INT_RX, DISABLE);
		break;
	case SCI_MCU_USART6_NUM:
		USART_FuncCmd(CM_USART6, USART_RX, DISABLE);
		USART_FuncCmd(CM_USART6, USART_INT_RX, DISABLE);
		break;
	case SCI_MCU_USART7_NUM:
		USART_FuncCmd(CM_USART7, USART_RX, DISABLE);
		USART_FuncCmd(CM_USART7, USART_INT_RX, DISABLE);
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
	case SCI_MCU_USART6_NUM:
		ENABLE_HARD_SCI_6_SEND;
		api_Delay100us(1); // ��ʱ100us
		break;
	case SCI_MCU_USART7_NUM:
		ENABLE_HARD_SCI_7_SEND;
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
			if (Serial[i].TXPoint == 0)
			{
				switch (SciPhNum)
				{
				case SCI_MCU_USART1_NUM:
					USART_FuncCmd(CM_USART1, USART_TX, ENABLE);						   //����ʹ��
					USART_WriteData(CM_USART1, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(CM_USART1, USART_INT_TX_EMPTY, ENABLE);			   //�����ж�ʹ��
					NVIC_EnableIRQ((IRQn_Type)Uart1Snd_IrqNo);						   //���ж�

					break;
				case SCI_MCU_USART3_NUM:
					USART_FuncCmd(CM_USART3, USART_TX, ENABLE);						   //����ʹ��
					USART_WriteData(CM_USART3, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(CM_USART3, USART_INT_TX_EMPTY, ENABLE);			   //�����ж�ʹ��
					NVIC_EnableIRQ((IRQn_Type)Uart3Snd_IrqNo);						   //���ж�

					break;
				case SCI_MCU_USART4_NUM:
					USART_FuncCmd(CM_USART4, USART_TX, ENABLE);						   //����ʹ��
					USART_WriteData(CM_USART4, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
					USART_FuncCmd(CM_USART4, USART_INT_TX_EMPTY, ENABLE);			   //�����ж�ʹ��
					NVIC_EnableIRQ((IRQn_Type)Uart4Snd_IrqNo);						   //���ж�
					break;
//				case SCI_MCU_USART6_NUM:
//					USART_FuncCmd(CM_USART6, USART_TX, ENABLE);						   //����ʹ��
//					USART_WriteData(CM_USART6, Serial[i].ProBuf[Serial[i].TXPoint++]); //д������
//					USART_FuncCmd(CM_USART6, USART_INT_TX_EMPTY, ENABLE);			   //�����ж�ʹ��
//					NVIC_EnableIRQ((IRQn_Type)Uart6Snd_IrqNo);						   //���ж�
//
//					break;
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
	// BYTE i,IntStatus,BpsBak,Bps;
	// stc_can_init_config_t stcCanInitCfg;
	// stc_irq_regi_conf_t     stcIrqRegiConf;
	// stc_can_filter_t *astcFilters=NULL;
	// // stc_can_filter_t astcFilters[2] = \
    // // {
	// //     {0x00000000ul, 0x1FFFFFFFul, CanFilterSel1, CanAllFrames},
	// //     {0x00000123ul, 0x1FFFFFFFul, CanFilterSel2, CanAllFrames}
	// // };

	// for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	// {
	// 	if(SciPhNum == SerialMap[i].SCI_Ph_Num)
	// 	{
	// 		break;
	// 	}
	// }
	// //�Ϸ����ж�
	// if( i != eCAN)
	// {
	// 	ASSERT(0,0);
	// 	return FALSE;
	// }

	// IntStatus = api_splx(0);

	// stcIrqRegiConf.enIRQn = (IRQn_Type)CAN_Rx_IrqNo;
	// stcIrqRegiConf.enIntSrc = INT_CAN_INT;
	// stcIrqRegiConf.pfnCallback = &CAN_RxIrqCallBack;
	// enIrqRegistration(&stcIrqRegiConf);
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
	// NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
	// NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
	// //<< Enable can peripheral clock and buffer(ram)
	// PWC_RamOpMdConfig(HighSpeedMd);
	// PWC_RamPwrdownCmd(PWC_RAMPWRDOWN_CAN, Enable);
	// PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_CAN, Enable);

	// //�򿪽����жϣ�������ʱʹ���������ͣ����ڿ��Ը�Ϊ�жϷ���
	// CAN_IrqCmd(CanRxIrqEn, Enable);
	// CAN_IrqCmd(CanTxPrimaryIrqEn, Enable);

	// //<< CAN GPIO config
	// //PORT_SetFunc(PortE, Pin15, Func_Can1_Rx, Disable);
	// //PORT_SetFunc(PortB, Pin10, Func_Can1_Tx, Disable);

	// BpsBak = api_GetBaudRate(0, SciPhNum);
	// Bps = BpsBak;
	// MEM_ZERO_STRUCT(stcCanInitCfg);
	// //<< Can bit time config

	// switch (Bps)
	// {
	// case 0x0D:
	// 	stcCanInitCfg.stcCanBt.PRESC = 100u-1u;	// 0.1   200��Ƶ    10k
	// 	break;
	// case 0x0E:
	// 	stcCanInitCfg.stcCanBt.PRESC = 40u-1u;	// 0.04  80��Ƶ		25k
	// 	break;
	// case 0x0F:
	// 	stcCanInitCfg.stcCanBt.PRESC = 20u-1u;	// 0.02	 40��Ƶ		50k
	// 	break;
	// case 0x10:
	// 	stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16��Ƶ	125K
	// 	break;
	// default:
	// 	stcCanInitCfg.stcCanBt.PRESC = 8u-1u;	// 0.008  16��Ƶ	Ĭ��125K
	// 	break;
	// }

	// stcCanInitCfg.stcCanBt.SEG_1 = 9u-2u;
	// stcCanInitCfg.stcCanBt.SEG_2 = 3u-1u;
	// stcCanInitCfg.stcCanBt.SJW   = 3u-1u;

	// stcCanInitCfg.stcWarningLimit.CanErrorWarningLimitVal = 10u;
	// stcCanInitCfg.stcWarningLimit.CanWarningLimitVal = 16u-1u;

	// stcCanInitCfg.enCanRxBufAll  = CanRxNormal;
	// stcCanInitCfg.enCanRxBufMode = CanRxBufNotStored;
	// stcCanInitCfg.enCanSAck = CanSelfAckDisable ;	//�ػ�ģʽ���Զ��ظ�ACK CanSelfAckEnable
	// stcCanInitCfg.enCanSTBMode   = CanSTBFifoMode;

	// stcCanInitCfg.pstcFilter     = astcFilters;
	// stcCanInitCfg.u8FilterCount  = 0;

	// CAN_Init(&stcCanInitCfg);

	// api_InitSubProtocol(i);
	// //���ڼ��Ӷ�ʱ��
	// Serial[i].WatchSciTimer = WATCH_SCI_TIMER;

	// api_splx(IntStatus);

	// //�ڲ��ػ�ģʽ�����ڲ���
	// //CAN_ModeConfig(CanInternalLoopBackMode,Enable);
	// //CAN_ModeConfig(CanExternalLoopBackMode, Enable);
	// ChangeCanMod();

	// return TRUE;
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
//	Serial[eCAN].WatchSciTimer = WATCH_SCI_TIMER;
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
	//	BYTE i;
	//	static WORD Oldpoint;
	//	stc_can_txframe_t stcTxFrame;
	//
	//	if(Serial[eCAN].TXPoint != 0)
	//	{
	//		if (Serial[eCAN].TXPoint!=Oldpoint)
	//		{
	//			Oldpoint=Serial[eCAN].TXPoint;
	//			CanErrTime=0;
	//		}
	//		else
	//		{
	//			if (CanErrTime>CAN_ERR_TIME_X10MS)//����500ms
	//			{
	//				Serial[eCAN].TXPoint=0;
	//				Serial[eCAN].SendLength=0;
	//			}
	//
	//		}
	//
	//		return FALSE;
	//	}
	//	Oldpoint=0;
	//
	//	MEM_ZERO_STRUCT(stcTxFrame);
	//	//<<Can Tx
	//	api_SetCanID(2,1);
	//	stcTxFrame.StdID = api_GetCanID();	//���ȼ���ַ
	//	stcTxFrame.Control_f.DLC = 8;
	//	stcTxFrame.Control_f.IDE = 1;
	//	for(i = 0; i < 8; i++)
	//	{
	//		stcTxFrame.Data[i] = Serial[eCAN].ProBuf[i];
	//	}
	//	// ���ܽ���CAN�ж��޸���SendLength
	//	if(Serial[eCAN].SendLength == 0)
	//	{
	//		return FALSE;
	//	}
	//
	//	CAN_SetFrame(&stcTxFrame);
	//	CAN_TransmitCmd(CanPTBTxCmd);
	//
	//	Serial[eCAN].TXPoint = 8;
	//
	//	return TRUE;
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
static void ExtIntConfig(TExtIntCof *pExtIntCof)
{
	stc_extint_init_t stcExtIntInit;
	stc_irq_signin_config_t stcIrqSignConfig;

	/* ExtInt config */
	(void)EXTINT_StructInit(&stcExtIntInit);
	stcExtIntInit.u32Filter = EXTINT_FILTER_ON;
	stcExtIntInit.u32FilterClock = EXTINT_FCLK_DIV64;
	stcExtIntInit.u32Edge = pExtIntCof->edge;
	(void)EXTINT_Init(pExtIntCof->channel, &stcExtIntInit);
    EXTINT_ClearExtIntStatus(pExtIntCof->channel);//ǰ�ĸ��Ǽ���оƬ�����ݵ�cs


	/* IRQ sign-in */
	stcIrqSignConfig.enIntSrc = (en_int_src_t)pExtIntCof->IntSrc;
	stcIrqSignConfig.enIRQn = (IRQn_Type)pExtIntCof->IntIRQn;
	stcIrqSignConfig.pfnCallback = pExtIntCof->pfnCallback;
	(void)INTC_IrqSignIn(&stcIrqSignConfig);

	/* NVIC config */
	NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
	NVIC_SetPriority(stcIrqSignConfig.enIRQn, pExtIntCof->Priority);
	NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}
#if (SEL_TOPOLOGY == YES)
//-----------------------------------------------
//��������:HDSC��SPI��ʼ��
//
//����:SampleChipNo ��0��ʼ
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void api_TopWaveSpiInit(BYTE SampleChipNo)
{
	ExtIntConfig(&ExtIntCof[SampleChipNo]);
	api_InitSPI((eCOMPONENT_TYPE)WaveDataConf[SampleChipNo].SPIComponent, eSPI_MODE_1);
	TopWaveSpiDmaConfig(&WaveDataConf[SampleChipNo]);
	TopWaveSpiDmaEnableInit(&WaveDataConf[SampleChipNo]);
	WaveDataDeal[SampleChipNo].HSDCTimer = 0;
}
//-----------------------------------------------
//��������://�ϵ����ʼ��
//
//����:
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void api_ChargeDetectInit(void)
{
	BYTE i = 0;

	for(i = 4;i <= 15;i++)
	{
		ExtIntConfig(&ExtIntCof[i]);
	}
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
void api_DisTopoIqr(void)
{
	//	stc_exint_config_t stc;
	//	stc_irq_regi_conf_t stcIrqRegiConf;
	//
	//	MEM_ZERO_STRUCT(stc);
	//	MEM_ZERO_STRUCT(stcIrqRegiConf);
	//	stc.enExitCh = ExtiCh05;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�
	//	stc.enExtiLvl = ExIntRisingEdge;
	//	stc.enFilterEn = Disable;
	//	stc.enFltClk = Pclk3Div8;
	//
	//	EXINT_Init(&stc);
	//
	//	stcIrqRegiConf.enIRQn = (IRQn_Type)TopoSpiCs_IrqNo;
	//
	//	/* Select External Int Ch.8 */
	//    stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ5;//����IO����Ҫ�����ж���ţ������жϺ���Ҫ�鿴�����ֲ���2.2���Ź��ܱ�
	//
	//    /* Callback function */
	//    stcIrqRegiConf.pfnCallback = &HSPISample2Callback;
	//
	//    /* Registration IRQ */
	//    enIrqRegistration(&stcIrqRegiConf);
	//
	//    /* Clear pending */
	//    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
	//
	//    /* Set priority */
	//    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
	//
	//    /* Enable NVIC */
	//    NVIC_DisableIRQ(stcIrqRegiConf.enIRQn);
}

#endif //#if( SEL_TOPOLOGY == YES )
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
BYTE api_MultiFunPortSet(BYTE Type)
{
	//û��5v�������κβ�����ֱ���˳�
	if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return FALSE;
	}

	if (Type == eCLOCK_PULS_OUTPUT)
	{
		api_InitOutClockPuls(0); //��ʱ��оƬ�������
		SWITCH_TO_CLOCK_PULS_OUTPUT;
	}
	else if ((Type == eDEMAND_PERIOD_OUTPUT) || (Type == eSEGMENT_SWITCH_OUTPUT))
	{
		api_InitOutClockPuls(55); //�ر�ʱ��оƬ�������
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
void api_MultiFunPortCtrl(BYTE Type)
{
	//û��5v�������κβ�����ֱ���˳�
	if (api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE)
	{
		return;
	}

	if (GlobalVariable.g_byMultiFunPortType != Type)
	{
		return;
	}

	if ((Type == eDEMAND_PERIOD_OUTPUT) || (Type == eSEGMENT_SWITCH_OUTPUT))
	{
		api_InitOutClockPuls(55);											//�ر�ʱ��оƬ�������
		GlobalVariable.g_byMultiFunPortCounter = (80 / SYS_TICK_PERIOD_MS); //80ms ��systick��ʱ���
		MULTI_FUN_PORT_OUT_L;
	}
	else if (Type == eCLOCK_PULS_OUTPUT)
	{
		//�л�Ϊ��ģʽʱ���Զ������û�е��õĵط�
		api_InitOutClockPuls(0); //��ʱ��оƬ�������
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
DWORD GetTicks(void)
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
	SysTickCounter++;
	CanErrTime++;
	if (SysTickCounter >= 10000)
	{
		SysTickCounter = 0;
	}

	api_SetAllTaskFlag(eFLAG_10_MS);

	if ((SysTickCounter % 50) == 0)
	{
		api_SetAllTaskFlag(eFLAG_500_MS);
	}
//������
#if (PREPAY_MODE == PREPAY_LOCAL)
	api_BeepReadCard();
#endif
	//ͨѶ��ʱ��
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (Serial[i].ReceToSendDelay > 0) //10ms����һ��
		{
			Serial[i].ReceToSendDelay--;
		}
		if (Serial[i].RxOverCount > 1) //10ms����һ�� ע�⣬�����Ǵ���1
		{
			Serial[i].RxOverCount--;
		}
	}
	//�����ϱ���ʱ����
	ReportTimeMinus();

	if (GlobalVariable.g_byMultiFunPortCounter != 0)
	{
		GlobalVariable.g_byMultiFunPortCounter--;
		if (GlobalVariable.g_byMultiFunPortCounter == 0)
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
void RTCIRQ_Service(void)
{
}
void SysTick_Handler(void)
{
	BYTE i;
	static BYTE FlashCnt = 0; //��˸led������жϼ�����
#if (SEL_TOPOLOGY == YES)
	static WORD SysTickCounter1 = 0; //����������Ϣ����ʹ��
#endif

#if (SEL_DEBUG_VERSION == YES)
//TickDelayTest();
#endif

	gsysTick += 10;
	SysTickCounter++;
	if (SysTickCounter >= 10000)
	{
		SysTickCounter = 0;
	}

#if (SEL_TOPOLOGY == YES)
	if (bySamplePowerOnTimer <= HSDC_OPEN_TIME)
	{
		for ( i = 0; i < SAMPLE_CHIP_NUM; i++)
		{
			WaveDataDeal[i].HSDCTimer++;
		}
	}
#endif

	api_SetAllTaskFlag(eFLAG_10_MS);

	if ((SysTickCounter % 50) == 0)
	{
		api_SetAllTaskFlag(eFLAG_500_MS);
	}

	//ͨѶ��ʱ��
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (Serial[i].ReceToSendDelay > 0) //10ms����һ��
		{
			Serial[i].ReceToSendDelay--;
		}
		if (Serial[i].RxOverCount > 1) //10ms����һ�� ע�⣬�����Ǵ���1
		{
			Serial[i].RxOverCount--;
		}
		if (Serial[i].SendToSendDelay > 0)
		{
			Serial[i].SendToSendDelay--;
		}
	}

	if (GlobalVariable.g_byMultiFunPortCounter != 0)
	{
		GlobalVariable.g_byMultiFunPortCounter--;
		if (GlobalVariable.g_byMultiFunPortCounter == 0)
		{
			MULTI_FUN_PORT_OUT_H;
		}
	}
	//�����ϱ���ʱ����
	ReportTimeMinus();

#if (SEL_EVENT_DI_CHANGE == YES)
	api_DISample();
#endif

#if (SEL_TOPOLOGY == YES)
	if (api_GetSysStatus(eSYS_STATUS_TOPOING) == TRUE)
	{
		SysTickCounter1++;
		if (SysTickCounter1 >= 60)
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
	//	BYTE i,bLen,bLen1,j;
	//	WORD wPoint;
	//
	//	stc_can_rxframe_t stcRxFrame;
	//	stc_can_txframe_t stcTxFrame;
	//
	//    if(true == CAN_IrqFlgGet(CanRxIrqFlg))
	//	{
	//		//CAN_IrqCmd(CanRxIrqEn, Disable);
	//		CAN_Receive(&stcRxFrame);
	//		// if( CanBusPara.bRxNum != 0 )
	//		// {
	//		// 	for ( i = 0; i < CanBusPara.bRxNum; i++)
	//		// 	{
	//		// 		if (CanBusPara.bRxNode[i]==(stcRxFrame.StdID&0x000000ff))//���˼���ID
	//		// 		{
	//					if (api_ChargingCan(stcRxFrame)==FALSE)//���ǳ��׮����
	//					{
	//						if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
	//						{
	//							//��ȡ��Ч����
	//							memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
	//							Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
	//						}
	//						else
	//						{
	//							bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
	//							memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
	//							bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
	//							memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
	//							Serial[eCAN].RXWPoint = bLen1;
	//						}
	//						api_SetCanID(1,(BYTE)stcRxFrame.ExtID);
	//					}
	//		// 			break;
	//		// 		}
	//		// 	}
	//		// }
	//		// else //δ����CAN����ID����ʱ������ͨ�ڴ���
	//		// {
	//		// 	if((Serial[eCAN].RXWPoint + stcRxFrame.Cst.Control_f.DLC) < MAX_PRO_BUF_REAL)
	//		// 	{
	//		// 		//��ȡ��Ч����
	//		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],stcRxFrame.Cst.Control_f.DLC);
	//		// 		Serial[eCAN].RXWPoint += stcRxFrame.Cst.Control_f.DLC;
	//		// 	}
	//		// 	else
	//		// 	{
	//		// 		bLen = MAX_PRO_BUF_REAL - Serial[eCAN].RXWPoint;
	//		// 		memcpy(&Serial[eCAN].ProBuf[Serial[eCAN].RXWPoint],&stcRxFrame.Data[0],bLen);
	//		// 		bLen1 = stcRxFrame.Cst.Control_f.DLC - bLen;
	//		// 		memcpy(&Serial[eCAN].ProBuf[0],&stcRxFrame.Data[bLen],bLen1);
	//		// 		Serial[eCAN].RXWPoint = bLen1;
	//		// 	}
	//		// }
	//
	//		CAN_IrqFlgClr(CanRxIrqFlg);
	//	}
	//	if(true == CAN_IrqFlgGet(CanTxPrimaryIrqFlg))
	//	{
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
	//		else if((Serial[eCAN].TXPoint + 8) <= Serial[eCAN].SendLength)
	//		{
	//			bLen = 8;
	//		}
	//		else
	//		{
	//			api_SetCanID(2,0);
	//			bLen = Serial[eCAN].SendLength - Serial[eCAN].TXPoint;
	//		}
	//
	//		MEM_ZERO_STRUCT(stcTxFrame);
	//		//<<Can Tx
	//		stcTxFrame.StdID = api_GetCanID();	//���ȼ���ַ
	//		stcTxFrame.Control_f.DLC = 8;	//�ϸ���8���ֽڻظ�
	//		stcTxFrame.Control_f.IDE = 1;
	//		for(i = 0; i < bLen; i++)
	//		{
	//			stcTxFrame.Data[i] = Serial[eCAN].ProBuf[Serial[eCAN].TXPoint+i];
	//		}
	//
	//		Serial[eCAN].TXPoint += i;
	//
	//		if( (i < 8) && (i == bLen) )
	//		{
	//			for(i = bLen; i < 8; i++)
	//			{
	//				stcTxFrame.Data[i] = 0x00;
	//			}
	//		}
	//
	//		CAN_SetFrame(&stcTxFrame);
	//		CAN_TransmitCmd(CanPTBTxCmd);
	//
	//		CanErrTime=0;
	//
	//		CAN_IrqFlgClr(CanTxPrimaryIrqFlg);
	//
	//	}
	//    if(true == CAN_IrqFlgGet(CanRxOverIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanRxOverIrqFlg);
	//    }
	//    if(true == CAN_IrqFlgGet(CanRxBufFullIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanRxBufFullIrqFlg);
	//    }
	//    if(true == CAN_IrqFlgGet(CanRxBufAlmostFullIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanRxBufAlmostFullIrqFlg);
	//    }
	//    if(true == CAN_IrqFlgGet(CanTxSecondaryIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanTxSecondaryIrqFlg);
	//    }
	//    if(true == CAN_IrqFlgGet(CanErrorIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanErrorIrqFlg);
	//    }
	//    if(true == CAN_IrqFlgGet(CanAbortIrqFlg))
	//    {
	//        CAN_IrqFlgClr(CanAbortIrqFlg);
	//    }
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
static void InitTimerA3_PWM1(void)
{
	//	stc_timera_base_init_t timera_base;
	//	stc_timera_compare_init_t timera_compare;
	//
	//	PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3,Enable);
	//
	//	MEM_ZERO_STRUCT(timera_base);
	//
	//	TIMERA_DeInit(M4_TMRA3);//��ʼ����3��TIMERA��Ԫ
	//
	//	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1 ʱ��Ƶ��12M  18M  12M
	//	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//��ݲ���ʽ
	//	timera_base.enCntDir = TimeraCountDirUp;				//���ϼ���
	//	timera_base.enSyncStartupEn = Disable;					//����ģ��1ͬ��
	//
	//	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	//	timera_base.u16PeriodVal = 316;							//�ӽ�38kHz
	//	#elif(SPD_HDSC_MCU == SPD_72000K)
	//	timera_base.u16PeriodVal = 474;							//�ӽ�38kHz
	//	#endif
	//	TIMERA_BaseInit(M4_TMRA3,&timera_base);
	//	MEM_ZERO_STRUCT(timera_compare);
	//
	//	#if((SPD_HDSC_MCU == SPD_48000K) || (SPD_HDSC_MCU == SPD_96000K))
	//	timera_compare.u16CompareVal = 79;						//25%ռ�ձ�
	//	#elif(SPD_HDSC_MCU == SPD_72000K)
	//	timera_compare.u16CompareVal = 118;						//�ӽ�25%ռ�ձ�
	//	#endif
	//	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//��ʼ����ʱΪ�ߵ�ƽ
	//	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//��������ʱΪ�ߵ�ƽ
	//	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//�Ƚ�ֵƥ��ʱ�л�Ϊ��ƽת��
	//	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//��������ֵʱ�л�Ϊ�ߵ�ƽ
	//	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//��һ���ڿ�ʼʱ�л���Ч
	//	TIMERA_CompareInit(M4_TMRA3,TimeraCh1,&timera_compare);
	//
	//	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Disable);	//�ر�PWM���ܣ���Ҫʱֱ�ӿ���
	//
	//	TIMERA_Cmd(M4_TMRA3,Enable);
}

//����PWMʹ��
void IrPWMEnable(BOOL enNewSta)
{
	//	if(enNewSta == TRUE)
	//	{
	//		TIMERA_Cmd(M4_TMRA3,Enable);
	//	}
	//	else
	//	{
	//		TIMERA_Cmd(M4_TMRA3,Disable);
	//	}
	//
	//	TIMERA_CompareCmd(M4_TMRA3,TimeraCh1,Enable);		//��PWM�����ȷ����ʱ���ر�ʱ����ߵ�ƽ
}
#if (SEL_TOPOLOGY == YES)

// ͨ�� callback,LoopNum ��1��ʼ
static void LoopChargeCallback(BYTE LoopNum)
{
    StartChargeFlag[LoopNum - 1] = 1;
	NVIC_DisableIRQ((IRQn_Type)(Edge1_Test_IrqNo + LoopNum - 1));
    IsChargingFlag |= 1 << (LoopNum - 1);
	EXTINT_ClearExtIntStatus(ExtIntCof[3+LoopNum].channel);//ǰ�ĸ��Ǽ���оƬ�����ݵ�cs
}
//�����ص�
static void Loop1ChargeCallback(void)
{
	LoopChargeCallback(1);
}
static void Loop2ChargeCallback(void)
{
	LoopChargeCallback(2);
}

static void Loop3ChargeCallback(void)
{
	LoopChargeCallback(3);
}

static void Loop4ChargeCallback(void)
{
	LoopChargeCallback(4);
}

static void Loop5ChargeCallback(void)
{
	LoopChargeCallback(5);
}

static void Loop6ChargeCallback(void)
{
	LoopChargeCallback(6);
}

static void Loop7ChargeCallback(void)
{
	LoopChargeCallback(7);
}

static void Loop8ChargeCallback(void)
{
	LoopChargeCallback(8);
}

static void Loop9ChargeCallback(void)
{
	LoopChargeCallback(9);
}

static void Loop10ChargeCallback(void)
{
	LoopChargeCallback(10);
}

static void Loop11ChargeCallback(void)
{
	LoopChargeCallback(11);
}

static void Loop12ChargeCallback(void)
{
	LoopChargeCallback(12);
}


//-----------------------------------------------
//��������: �������ݽ��մ�����
//
//����:
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
static void HSPIDataDeal(TWaveDataDeal *pWaveDataDeal)
{
	api_TopWaveHsdcBufTransfer(pWaveDataDeal);
	memset(&pWaveDataDeal->RxBuf[0], 0, sizeof(pWaveDataDeal->RxBuf));
	TopWaveSpiDmaEnableInit(&WaveDataConf[pWaveDataDeal->SampleChipNo]);
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
static void HSPISample1Callback(void)
{
	BYTE ret = HSDC_SPI_CS1_STATUS;
	if (ret)
	{
		IsRecWaveData |= 0x01;
		HSPIDataDeal(&WaveDataDeal[0]);
	}
	else
	{
		// printf(" err");
	}
}

//-----------------------------------------------
// ��������: ���spi����
// ����:		��
//
// ����ֵ: 	��
//
// ��ע:
//-----------------------------------------------
void ClearSPIError(CM_SPI_TypeDef *SPIx)
{
	SPI_ClearStatus(SPIx, SPI_FLAG_CLR_ALL);
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
static void HSPISample2Callback(void)
{
	BYTE ret = HSDC_SPI_CS2_STATUS;

	if (ret)
	{
		IsRecWaveData |= 0x02;
		HSPIDataDeal(&WaveDataDeal[1]);
	}
	else
	{
		// printf(" err");
	}
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
static void HSPISample3Callback(void)
{
	BYTE ret = HSDC_SPI_CS3_STATUS;

	if (ret)
	{
		IsRecWaveData |= 0x04;
		HSPIDataDeal(&WaveDataDeal[2]);
	}
	else
	{
		// printf(" err");
	}
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
static void HSPISample4Callback(void)
{
	BYTE ret = HSDC_SPI_CS4_STATUS;

	if (ret)
	{
		IsRecWaveData |= 0x08;
		HSPIDataDeal(&WaveDataDeal[3]);
	}
	else
	{
		// printf(" err");
	}
}
//����PWMʹ��
void TopoPWMEnable(BOOL enNewSta)
{
	//	if(enNewSta == TRUE)
	//	{
	//		TP_PWMEN_ON;
	//		TIMERA_Cmd(M4_TMRA2,Enable);
	//	}
	//	else
	//	{
	//		TP_PWMEN_OFF;
	//		TIMERA_Cmd(M4_TMRA2,Disable);
	//	}
	//
	//	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,(en_functional_state_t)Enable);	//��PWM�����ȷ����ʱ���ر�ʱ����ߵ�ƽ
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
	static BYTE AlreadySendBitLen = 0, SignatureCodeLen = 0; //�����Ѵ�������(bit)����,��ȡ���ݳ���
	static BYTE SignatureCode[16] = {0};					 //������д���ݻ���
	BYTE ByteIndex, BitIndex;

	if (AlreadySendBitLen == 0)
	{
		SignatureCodeLen = ModulatSignalInfoRAM.SignatureCodeLen;

		if (SignatureCodeLen > sizeof(SignatureCode))
		{
			SignatureCodeLen = sizeof(SignatureCode);
		}

		memcpy(SignatureCode, ModulatSignalInfoRAM.SignatureCode, SignatureCodeLen);
	}

	if (((AlreadySendBitLen / 8) >= SignatureCodeLen) || (SignatureCodeLen != 2) //�����볤�ȹ̶�2�ֽ�
	)
	{
		//�������
		TopoPWMEnable(FALSE);
		api_ClrSysStatus(eSYS_STATUS_TOPOING);
		AlreadySendBitLen = 0;
		SignatureCodeLen = 0;
		memset(SignatureCode, 0x00, sizeof(SignatureCode));
		return;
	}

	ByteIndex = SignatureCodeLen - 1 - AlreadySendBitLen / 8; //�Ӹ��ֽڿ�ʼ��
	BitIndex = 7 - (AlreadySendBitLen % 8);					  //�Ӹ�λ��ʼ��

	if (SignatureCode[ByteIndex] & (1 << BitIndex))
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
	//	stc_timera_base_init_t timera_base;
	//	stc_timera_compare_init_t timera_compare;
	//
	//    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA2,Enable);
	//
	//	MEM_ZERO_STRUCT(timera_base);
	//
	//	TIMERA_DeInit(M4_TMRA2);//��ʼ���ڶ���TIMERA��Ԫ
	//
	//	timera_base.enClkDiv = TimeraPclkDiv8;					//PCLK1/8 ʱ��Ƶ��12M
	//	timera_base.enCntMode = TimeraCountModeSawtoothWave; 	//��ݲ���ʽ
	//	timera_base.enCntDir = TimeraCountDirUp;				//���ϼ���
	//	timera_base.enSyncStartupEn = Disable;				//����ģ��1ͬ��
	//	timera_base.u16PeriodVal = 12 * wPeriod;			//0.0012 * (12*1000*1000) = 14400
	//	TIMERA_BaseInit(M4_TMRA2,&timera_base);
	//
	//	MEM_ZERO_STRUCT(timera_compare);
	//	timera_compare.u16CompareVal = 12 * wCompare;		//2/3�ĸߵ�ƽ(���߼�)
	//	timera_compare.enStartCountOutput = TimeraCountStartOutputHigh;	//��ʼ����ʱΪ�ߵ�ƽ
	//	timera_compare.enStopCountOutput = TimeraCountStopOutputHigh;	//��������ʱΪ�ߵ�ƽ
	//	timera_compare.enCompareMatchOutput = TimeraCompareMatchOutputReverse;	//�Ƚ�ֵƥ��ʱ�л�Ϊ��ƽת��
	//	timera_compare.enPeriodMatchOutput = TimeraPeriodMatchOutputHigh;		//��������ֵʱ�л�Ϊ�ߵ�ƽ
	//	timera_compare.enSpecifyOutput = TimeraSpecifyOutputInvalid;			//��һ���ڿ�ʼʱ�л���Ч
	//	TIMERA_CompareInit(M4_TMRA2,TimeraCh4,&timera_compare);
	//
	//	TIMERA_Cmd(M4_TMRA2,Disable);
	//	TIMERA_CompareCmd(M4_TMRA2,TimeraCh4,Enable);	//�ر�PWM���ܣ���Ҫʱֱ�ӿ���
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
//-----------------------------------------------
//��������: ʹ�ܳ�����ж�
//
//����:		loop  ��· ��0��ʼ
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_EnableChargeInt(BYTE loop)
{
	NVIC_ClearPendingIRQ((IRQn_Type)(Edge1_Test_IrqNo + loop));
	NVIC_EnableIRQ((IRQn_Type)(Edge1_Test_IrqNo + loop));
}

#endif // #if( (BOARD_TYPE == BOARD_HC_SEPARATETYPE) )
