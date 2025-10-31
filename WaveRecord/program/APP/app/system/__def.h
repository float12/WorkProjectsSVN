#ifndef __DEFINE_INCLUDE
#define __DEFINE_INCLUDE

///////////////////////////////////////////////////
//�������Ͷ���
#define BYTE	unsigned char
#define WORD    unsigned short
#define SWORD   short
#define DWORD	unsigned long
#define SDWORD	long
#define BOOL	unsigned char
#define	QWORD   unsigned long long
#define SQWORD	long long

// λ����      
#define BIT0            0x0001
#define BIT1            0x0002
#define BIT2            0x0004
#define BIT3            0x0008
#define BIT4            0x0010
#define BIT5            0x0020
#define BIT6            0x0040
#define BIT7            0x0080
#define BIT8            0x0100
#define BIT9            0x0200
#define BIT10           0x0400
#define BIT11           0x0800
#define BIT12           0x1000
#define BIT13           0x2000
#define BIT14           0x4000
#define BIT15           0x8000


////////////////////////////////////
#define	ILLEGAL_VALUE_8F		0xffffffff

//�߼�����
#define NO					0
#define YES					1

#define FALSE				0
#define TRUE				1

#define DATA_BCD			0
#define DATA_HEX			1		//16����ԭ��
#define DATA_HEXCOMP		2		//16���Ʋ��� 

#define READ				0
#define WRITE				1
#define CLRDT				2

#define UN_REPAIR_CRC		0		//���޸�CRC
#define REPAIR_CRC			1		//�޸�CRC

//////////////////////////////////////////////////
//������������
#define	PHASE_ALL	0x0000			//��
#define	PHASE_A		0x1000			//A��
#define	PHASE_B		0x2000			//B��
#define	PHASE_C		0x3000			//C��
#define	PHASE_N		0x4000			//����
#define	PHASE_N2	0x5000			//����
#define	PHASE_RA	PHASE_A			//����A��
#define	PHASE_RB	PHASE_B			//����B��
#define	PHASE_RC	PHASE_C			//����C��


#define	BASE		0x0100			//����
#define	HARMONIC	0x0200			//г��
#define	INTERHAR	0x0300			//��г��
#define THD			0x0400			//��г��������
#define GROUP		0x0500			//г��Ⱥ����
#define INTERGROUP	0x0600			//��г��Ⱥ����


#define	REMOTE_U		0x0000		//��ѹ
#define	REMOTE_I		0x0001		//����	
#define	REMOTE_PHASEU	0x0002		//��ѹ���
#define	REMOTE_PHASE	0x0003		//��ѹ�������
#define	REMOTE_P		0x0004		//�й�����
#define	REMOTE_Q		0x0005		//�޹�����
#define	REMOTE_S		0x0006		//���ڹ���
#define	REMOTE_P_AVE	0x0007		//һ����ƽ���й�����
#define	REMOTE_Q_AVE	0x0008		//һ����ƽ���޹�����
#define	REMOTE_S_AVE	0x0009		//һ����ƽ�����ڹ���
#define	REMOTE_COS		0x000A		//��������
#define	REMOTE_HZ		0x000F		//����Ƶ��
#define	REMOTE_U_AVE	0x0011		//һ����ƽ����ѹ
#define	REMOTE_I_AVE	0x0012		//һ����ƽ������	
#define	REMOTE_P_P		0x0013		//�����й����ʣ�г����
#define	REMOTE_N_P		0x0014		//�����й����ʣ�г����

//�������Ͷ���
#define	COMB_ACTIVE	0		//����й�
#define	P_ACTIVE	1		//�����й�
#define	N_ACTIVE	2		//�����й�
#define	P_RACTIVE	3		//�����޹�
#define	N_RACTIVE	4		//�����޹�
#define	RACTIVE1	5		//1�����޹�
#define	RACTIVE2	6		//2�����޹�
#define	RACTIVE3	7		//3�����޹�
#define	RACTIVE4	8		//4�����޹�
#define APPARENT_P	9		// ��������
#define APPARENT_N	10		// ��������

//////////////////////////////////////////////////
//����ֵ����
#define NULL	0

///////////////////////////////////////
//�����Ͷ��� ��Ҫ����255
//ֱͨ��
#define METER_ZT		1
//�������߱�
#define METER_3P3W		2
//�������߱�
#define METER_3P4W		3

#define METER_57V		1	//57V��ѹ
#define METER_100V		2	//100V��ѹ
#define METER_220V		3	//220v��ѹ
#define METER_380V		4	//380v��ѹ


//////////////////
//�������Ͷ��壬��Ҫ����255
//0.3(1.2)A��
#define CURR_1A			1
//1.5(6)A��
#define CURR_6A			2
//5(60)A�� ����10(60)
#define CURR_60A		3
//10(100)A��
#define CURR_100A		4
//100(500)
#define CURR_500A		5
//5(20)A��
#define CURR_20A		6
//////////////////
//ͨѶ����
//������
#define	COMM_NEAR		0
//Զ����
#define COMM_FAR		1


#define CARD			0
#define ESAM			1

//////////////////////////////////////////////////
// IIC���Ͷ���
#define IIC_SIM			0
#define IIC_HARD		1

//////////////////////////////////////////////////
//��Լ����
//��֧�������Լ
#define PRO_NO_SPECIAL		0
//֧��Mod-bus��Լ
#define PRO_MODBUS			4
//֧��DL/T698.45�������Э��
#define PRO_DLT698_45		5
//////////////////////////////////////////////////
//�����û�����
//��ͨ�û�
#define SPECIAL_GUO_WANG                 0xff
#define SPECIAL_NAN_WANG                 0xfe
//�Ĵ�ʡ����Ҫ��       
#define SPECIAL_GW_SI_CHUAN             62

//==============================================================================
//�������й�оƬ�Ķ���                            
#define CS_SPI_RESET_161                1
#define CS_SPI_256401                   10
#define CS_SPI_256402                   11
#define CS_SPI_FLASH                    12//ָFlashоƬ
#define CS_SPI_SAMPLE               	23
#define CS_SPI_CPU						24//�������ݽ���
#define CS_SPI_ESAM               		30
#define CS_SPI_256401_CONFIG_ADDR       40
#define CS_SPI_256402_CONFIG_ADDR       41

//==============================================================================
//���ɿ������Ͷ���
//��ѡ�񸺺ɿ���
#define SEL_LOAD_CONTROL_NO			0xff
//���ɿ��Ʋ���������ֵ������ʽ
#define SEL_LOAD_CONTROL_DEMAND		1
//���ɿ��Ʋ��õ�����ֵ������ʽ
#define SEL_LOAD_CONTROL_CUR		2


//////////////////////////////////////////////////
//�ɼ�оƬ����
#define CHIP_HT7017			6
#define CHIP_HT7038			7
#define CHIP_HT7026			8
#define CHIP_HT7627			9
#define CHIP_RN8302B		10

//�洢оƬ����
#define CHIP_24LC256		(8)
#define CHIP_24LC512		(9)
#define CHIP_GD25Q64C		(10)
#define CHIP_GD25Q32C		(11)
#define CHIP_GD25Q16C		(12)
#define CHIP_GD25Q256C		(13)
#define CHIP_NO				(6)


//��ͬ��Һ������
#define LCD_GW3PHASE_09		21		//09��8�¹��������Һ��
#define LCD_HT_SINGLE_METER 23
#define LCD_NO				25

//Һ������оƬ��ַ�߿��
#define LCD_CHIP_1623		7
#define LCD_CHIP_1622		6
#define LCD_CHIP_BU97950	5
#define LCD_CHIP_BU9792                 4
#define LCD_CHIP_PCF8576                3
#define LCD_CHIP_BU9794                 2
#define LCD_CHIP_BU9799                 1
#define LCD_CHIP_NO                     0

//ʱ��оƬ����
#define SOC_CLOCK						0//SOC����ʱ��
#define OUT_CLOCK						1//�ⲿ����ʱ��
                
//ʱ��оƬ���Ͷ���              
#define RTC_CHIP_8025T                  4
#define RTC_HT_INCLOCK                  1
#define RTC_VG_INCLOCK                  2

//Ԥ����ģʽ
#define	PREPAY_NO				0xff	//û��Ԥ���ѹ���
#define	PREPAY_LOCAL			0x01	//���Ԥ���ѣ����أ�
#define	PREPAY_REMOTE			0x02	//��վԤ����(Զ��)

#define	PREPAY_GUOWANG_698	 	0x01	//����698��Լ
#define	PREPAY_GUOWANG_13		0x02	//����13�淶
#define	PREPAY_NANWANG_15		0x03	//����15�淶
#define MULT_NANWANG_11			0x04	//����11�淶�๦��

#define ENCRYPT_ESAM			0x01	//Ӳ����
#define ENCRYPT_SOFTWARE		0x02	//�����

#define TYPE_SPI				0x01	//SPIͨ�ŷ�ʽ
#define TYPE_7816				0x02	//7816ͨ�ŷ�ʽ

#define CARD_NO					0x01	//�޿�
#define CARD_CONTACT			0x02	//�Ӵ�ʽ��
#define CARD_RADIO				0x03	//��Ƶ��



#define RELAY_NO	 			0x01	//�޼̵���
#define RELAY_INSIDE			0x02	//���ü̵���
#define RELAY_OUTSIDE			0x03	//���ü̵���

#define PPRECISION_A	 		0x01	//A����-2��
#define PPRECISION_B			0x02	//B����-1��
#define PPRECISION_C			0x03	//C����-0.5s
#define PPRECISION_D			0x04	//D����-0.2S

#define OSCILATOR_JG			0x01	//��������
#define OSCILATOR_XTC			0x02	//�����Ǿ���
#define OSCILATOR_XTC_3215		0x03	//�����Ǿ���_3215(С��װ����)

//���Ͷ��壨5λ����������1��ͷ��������3��ͷ��
#define BOARD_HT_SINGLE_78201602		11602//20�淶 ����SPI 9600������
#define BOARD_HT_SINGLE_78202201		12201//20�淶 ����SPI 115200������ ̼Ĥ����
#define BOARD_HT_SINGLE_78202202		12202//20�淶 ����SPI 115200������ �߷���

#define BOARD_HT_THREE_0134566          34566//20�淶�����+��������Ϸ�ʽ+6AС��������Ϊ5ŷ
#define BOARD_HT_THREE_0131699			31699//20�淶�����+��������·�ʽ+6AС��������Ϊ5ŷ
#define BOARD_HT_THREE_0130347			30347//20�淶�����+��������·�ʽ+С��������ӱ�+֧��115200������+��E2
#define BOARD_HC_SEPARATETYPE			36000//����ʽ�����

#define FLASH_PROTECT_OPEN			0X01	//����FLASH������
#define FLASH_PROTECT_COLSE			0X02	//�ر�FLASH������

//CPU����
#define CPU_HT6015              5
#define CPU_HT6025              6
#define CPU_ST091               7
#define CPU_VG8530              8
#define CPU_HC32F460			9

//�������Ͷ���
#define SERIAL_HARD_SCI				0
#define SERIAL_SIM_SCI				1

//���⻽������
#define IR_WAKEUP_NO_FUNC			0//�޺��⻽�ѹ���
#define IR_WAKEUP_PHOTO_SWITCH		1//�����ܻ���
#define IR_WAKEUP_UART_COMM			2//uart���ջ���

// ADCͨ������
#define SYS_POWER_AD		    		0//ϵͳ��ѹ
#define	SYS_CLOCK_VBAT_AD		    	1//ʱ�ӵ�ص�ѹ
#define	SYS_READ_VBAT_AD	    		2//�����ĵ��
#define	SYS_TEMPERATURE_AD		    	3//�¶�AD����

typedef void (*Fun)( BYTE Step );
#define 	OP_FUNCTION(Step)		((Fun)FunctionConst)(Step)

typedef enum
{
	ePowerOnMode = 0,	//�ϵ�ģʽ
	ePowerDownMode,		//����ģʽ
	ePowerWakeUpMode,	//�͹��Ļ���ģʽ
}ePOWER_MODE;

typedef enum
{
	ePHASE_ALL = 0,
	ePHASE_A,
	ePHASE_B,
	ePHASE_C,
}ePHASE_TYPE;

//SPI����������
typedef enum
{
	eCOMPONENT_SPI_SAMPLE = 0,	//����оƬ
	eCOMPONENT_SPI_FLASH,		//����FLASH
//	eCOMPONENT_SPI_LCD,			//����Һ��
	// eCOMPONENT_SPI_ESAM,		//SPI�ӿ�ESAM
	// eCOMPONENT_SPI_CPU,			//˫оͨ��SPI
	eCOMPONENT_SPI_HSDC,		//����оƬHSDC�ӿ�
	eCOMPONENT_TOTAL_NUM,
}eCOMPONENT_TYPE;


//SPIģʽ 0 1 2 3
typedef enum
{
	eSPI_MODE_0 = 0,      
	eSPI_MODE_1 = 1, 
	eSPI_MODE_2 = 2,
	eSPI_MODE_3 = 3, 
}eSPI_MODE;

typedef enum
{
	//��4��˳���ܶ�
	eRS485_I = 0,	//485
	eCR,			//�ز�
	// eBlueTooth,		//����
	eIR,			//����
	eCAN,
//	eRS485_II,		//�ڶ�·485
	eEXP_PRO,
}eSERIAL_TYPE;

#endif//��Ӧ�ļ���ǰ���#ifndef __DEFINE_INCLUDE

