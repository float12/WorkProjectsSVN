#ifndef __DEFINE_INCLUDE
#define __DEFINE_INCLUDE

///////////////////////////////////////////////////
//数据类型定义
#define BYTE	unsigned char
#define WORD    unsigned short
#define SWORD   short
#define SDWORD	long
#define DWORD	unsigned long
#define BOOL	unsigned char
#define	QWORD   unsigned long long
#define SQWORD	long long

// 位定义      
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

//逻辑定义
#define NO					0
#define YES					1

#define FALSE				0
#define TRUE				1

#define DATA_BCD			0
#define DATA_HEX			1		//16进制原码
#define DATA_HEXCOMP		2		//16进制补码 

#define READ				0
#define WRITE				1
#define CLEAR				2

#define UN_REPAIR_CRC		0		//不修复CRC
#define REPAIR_CRC			1		//修复CRC

//////////////////////////////////////////////////
//电表基本量定义
#define	PHASE_ALL	0x0000			//总
#define	PHASE_A		0x1000			//A相
#define	PHASE_B		0x2000			//B相
#define	PHASE_C		0x3000			//C相
#define	PHASE_N		0x4000			//零线
#define	PHASE_N2	0x5000			//零序
#define	PHASE_RA	0x6000			//基波A相
#define	PHASE_RB	0x7000			//基波B相
#define	PHASE_RC	0x8000			//基波C相

#define	REMOTE_U		0x0000		//电压
#define	REMOTE_I		0x0001		//电流	
#define	REMOTE_PHASEU	0x0002		//电压相角
#define	REMOTE_PHASE	0x0003		//电压电流相角
#define	REMOTE_P		0x0004		//有功功率
#define	REMOTE_Q		0x0005		//无功功率
#define	REMOTE_S		0x0006		//视在功率
#define	REMOTE_P_AVE	0x0007		//一分钟平均有功功率
#define	REMOTE_Q_AVE	0x0008		//一分钟平均无功功率
#define	REMOTE_S_AVE	0x0009		//一分钟平均视在功率
#define	REMOTE_COS		0x000A		//功率因数
#define	REMOTE_HZ		0x000F		//电网频率


//数据类型定义
#define	COMB_ACTIVE	0		//组合有功
#define	P_ACTIVE	1		//正向有功
#define	N_ACTIVE	2		//反向有功
#define	P_RACTIVE	3		//正向无功
#define	N_RACTIVE	4		//反向无功
#define	RACTIVE1	5		//1象限无功
#define	RACTIVE2	6		//2象限无功
#define	RACTIVE3	7		//3象限无功
#define	RACTIVE4	8		//4象限无功
#define APPARENT_P	9		// 正向视在
#define APPARENT_N	10		// 反向视在

//////////////////////////////////////////////////
//返回值定义
#define NULL	0

///////////////////////////////////////
//表类型定义 不要大于255
//直通表
#define METER_ZT		1
//三相三线表
#define METER_3P3W		2
//三相四线表
#define METER_3P4W		3

#define METER_57V		1	//57V电压
#define METER_100V		2	//100V电压
#define METER_220V		3	//220v电压
#define METER_380V		4	//380v电压


//////////////////
//电流类型定义，不要大于255
//0.3(1.2)A表
#define CURR_1A			1
//1.5(6)A表
#define CURR_6A			2
//5(60)A表 或者10(60)
#define CURR_60A		3
//10(100)A表
#define CURR_100A		4

//////////////////
//通讯类型
//近红外
#define	COMM_NEAR		0
//远红外
#define COMM_FAR		1
//无红外
#define COMM_NOAR		2

#define CARD			0
#define ESAM			1

//////////////////////////////////////////////////
// IIC类型定义
#define IIC_SIM			0
#define IIC_HARD		1

//////////////////////////////////////////////////
//规约定义
//不支持特殊规约
#define PRO_NO_SPECIAL		0
//支持Mod-bus规约
#define PRO_MODBUS			4
//支持DL/T698.45面向对象协议
#define PRO_DLT698_45		5
//////////////////////////////////////////////////
//特殊用户定义
//普通用户
#define SPECIAL_GUO_WANG                 0xff
#define SPECIAL_NAN_WANG                 0xfe
//四川省特殊要求       
#define SPECIAL_GW_SI_CHUAN             62

//==============================================================================
//以下是有关芯片的定义                            
#define CS_SPI_RESET_161                1
#define CS_SPI_256401                   10
#define CS_SPI_256402                   11
#define CS_SPI_FLASH                    12//指Flash芯片
#define CS_SPI_SAMPLE               	23
#define CS_SPI_ESAM               		30
#define CS_SPI_256401_CONFIG_ADDR       40
#define CS_SPI_256402_CONFIG_ADDR       41

//==============================================================================
//负荷控制类型定义
//不选择负荷控制
#define SEL_LOAD_CONTROL_NO			0xff
//负荷控制采用需量超值动作方式
#define SEL_LOAD_CONTROL_DEMAND		1
//负荷控制采用电流超值动作方式
#define SEL_LOAD_CONTROL_CUR		2


//////////////////////////////////////////////////
//采集芯片定义
#define CHIP_HT7017			6
#define CHIP_HT7038			7
#define CHIP_HT7026			8
#define CHIP_HT7053D		9

//存储芯片定义
#define CHIP_24LC128		(14)
#define CHIP_24LC256		(8)
#define CHIP_24LC512		(9)
#define CHIP_GD25Q64C		(10)
#define CHIP_GD25Q32C		(11)
#define CHIP_GD25Q16C		(12)
#define CHIP_NO				(6)
//E2厂家
#define CHIP_HARD_ADDR			(0)								//E2采用软件地址
#define CHIP_SOFT_ADDR			(0x80)							//E2采用软件地址，需要设置E2地址
#define CHIP_IS_SOFT_ADDR(chip)	(!!((chip) & CHIP_SOFT_ADDR))	//E2是软件地址

#define CHIP_E2_BL_HARD			(CHIP_HARD_ADDR + 1)		//贝岭硬件地址
#define CHIP_E2_FM_SOFT			(CHIP_SOFT_ADDR + 2)		//复旦软件地址
#define CHIP_E2_ST_HARD			(CHIP_HARD_ADDR + 3)		//ST硬件地址
#define CHIP_E2_ST_SOFT			(CHIP_SOFT_ADDR + 3)		//ST软件地址

//不同的液晶定义
#define LCD_GW3PHASE_09		21		//09年8月国网三相表液晶
#define LCD_HT_SINGLE_METER 23

//液晶驱动芯片地址线宽度
#define LCD_CHIP_1623		7
#define LCD_CHIP_1622		6
#define LCD_CHIP_BU97950	5
#define LCD_CHIP_BU9792                 4
#define LCD_CHIP_PCF8576                3
#define LCD_CHIP_BU9794                 2
#define LCD_CHIP_BU9799                 1
#define LCD_CHIP_NO                     0

//时钟芯片类型
#define SOC_CLOCK						0//SOC集成时钟
#define OUT_CLOCK						1//外部独立时钟
                
//时钟芯片类型定义              
#define RTC_CHIP_8025T                  4
#define RTC_HT_INCLOCK                  1
#define RTC_VG_INCLOCK                  2

//预付费模式
#define	PREPAY_NO				0xff	//没有预付费功能
#define	PREPAY_LOCAL			0x01	//表计预付费（本地）
#define	PREPAY_REMOTE			0x02	//主站预付费(远程)

#define	PREPAY_GUOWANG_698	 	0x01	//国网698规约
#define	PREPAY_GUOWANG_13		0x02	//国网13规范
#define	PREPAY_NANWANG_15		0x03	//南网15规范
#define MULT_NANWANG_11			0x04	//南网11规范多功能

#define ENCRYPT_ESAM			0x01	//硬加密
#define ENCRYPT_SOFTWARE		0x02	//软加密

#define TYPE_SPI				0x01	//SPI通信方式
#define TYPE_7816				0x02	//7816通信方式

#define CARD_NO					0x01	//无卡
#define CARD_CONTACT			0x02	//接触式卡
#define CARD_RADIO				0x03	//射频卡



#define RELAY_NO	 			0x01	//无继电器
#define RELAY_INSIDE			0x02	//内置继电器
#define RELAY_OUTSIDE			0x03	//外置继电器

#define PPRECISION_A	 		0x01	//A级表-2级
#define PPRECISION_B			0x02	//B级表-1级
#define PPRECISION_C			0x03	//C级表-0.5s
#define PPRECISION_D			0x04	//D级表-0.2S

#define OSCILATOR_JG			0x01	//精工晶体
#define OSCILATOR_XTC			0x02	//西铁城晶体
#define OSCILATOR_XTC_3215		0x03	//西铁城晶体_3215(小封装晶体)
#define OSCILATOR_JG_3215		0x04	//精工晶体_3215

//板型定义（5位数，单相以1开头，三相以3开头）
#define BOARD_HT_SINGLE_78201602		11602//20规范 共用SPI 9600波特率
#define BOARD_HT_SINGLE_78202201		12201//20规范 独立SPI 115200波特率 碳膜按键
#define BOARD_HT_SINGLE_78202202		12202//20规范 独立SPI 115200波特率 高防护
#define BOARD_HT_SINGLE_78202303		12303//20规范 独立SPI 115200波特率 碳膜按键  双E2 128
#define BOARD_HT_SINGLE_78202401		12401//20规范 独立SPI 115200波特率 碳膜按键 双E2 128 开盖按键PA10 法拉电容硬件控制
#define BOARD_HT_SINGLE_20250819		12508//单相费控智能电能表（电动自行车充电自动识别）初版本

#define BOARD_RN_SINGLE_78201806		11806//20规范 锐能微RN8613 115200波特率 256E2
#define BOARD_RN_SINGLE_78202307		12307//20规范 锐能微RN8613 115200波特率 128 + 128 E2

#define BOARD_HT_THREE_0134566          34566//20规范三相表+背光控制老方式+6A小电流电阻为5欧
#define BOARD_HT_THREE_0131699			31699//20规范三相表+背光控制新方式+6A小电流电阻为5欧
#define BOARD_HT_THREE_0130347			30347//20规范三相表+背光控制新方式+小电流电阻加倍+支持115200波特率+新E2
#define BOARD_HT_THREE_0132515			32515//20规范三相表+背光控制新方式+小电流电阻加倍+支持115200波特率+新E2+开盖新方式


#define FLASH_PROTECT_OPEN			0X01	//开启FLASH读保护
#define FLASH_PROTECT_COLSE			0X02	//关闭FLASH读保护

//CPU类型
#define CPU_HT6015              5
#define CPU_HT6025              6
#define CPU_ST091               7
#define CPU_VG8530              8

//串口类型定义
#define SERIAL_HARD_SCI				0
#define SERIAL_SIM_SCI				1

//红外唤醒类型
#define IR_WAKEUP_NO_FUNC			0//无红外唤醒功能
#define IR_WAKEUP_PHOTO_SWITCH		1//光敏管唤醒
#define IR_WAKEUP_UART_COMM			2//uart接收唤醒

// ADC通道定义
#define SYS_POWER_AD		    		0//系统电压
#define	SYS_CLOCK_VBAT_AD		    	1//时钟电池电压
#define	SYS_READ_VBAT_AD	    		2//其他的电池
#define	SYS_TEMPERATURE_AD		    	3//温度AD采样

typedef void (*Fun)( BYTE Step );
#define 	OP_FUNCTION(Step)		((Fun)FunctionConst)(Step)

typedef enum
{
	ePowerOnMode = 0,	//上电模式
	ePowerDownMode,		//掉电模式
	ePowerWakeUpMode,	//低功耗唤醒模式
}ePOWER_MODE;

typedef enum
{
	ePHASE_ALL = 0,
	ePHASE_A,
	ePHASE_B,
	ePHASE_C,
}ePHASE_TYPE;

//SPI各器件类型
typedef enum
{
	eCOMPONENT_SPI_SAMPLE = 0,	//采样芯片
	eCOMPONENT_SPI_FLASH,		//外置FLASH
	eCOMPONENT_SPI_LCD,			//点阵液晶
	eCOMPONENT_SPI_ESAM,		//SPI接口ESAM
	eCOMPONENT_SPI_CPU,			//双芯通信SPI
	eCOMPONENT_TOTAL_NUM,
}eCOMPONENT_TYPE;


//SPI模式 0 1 2 3
typedef enum
{
	eSPI_MODE_0 = 0,      
	eSPI_MODE_1 = 1, 
	eSPI_MODE_2 = 2,
	eSPI_MODE_3 = 3, 
}eSPI_MODE;

typedef enum
{
	//这4个顺序不能动
	eRS485_I = 0,	//4851
	ePT_UART1_F415,// 串口（与F415通信）
	eCR,			//载波
	eBlueTooth,		//蓝牙
	eIR,			//红外
	eRS485_II,		//4852
	ePT_UART2_F415,// 串口（与F415通信）
}eSERIAL_TYPE;

#endif//对应文件最前面的#ifndef __DEFINE_INCLUDE

