#ifndef __DEFINE_INCLUDE
#define __DEFINE_INCLUDE

///////////////////////////////////////////////////
//数据类型定义
#define BYTE	unsigned char
#define WORD    unsigned short
#define SWORD   short
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
#define CLR_TYPE			2

#define UN_REPAIR_CRC		0		//不修复CRC
#define REPAIR_CRC			1		//修复CRC

//////////////////////////////////////////////////
//电表基本量定义
#define	PHASE_ALL	0x0000			//总
#define	PHASE_A		0x1000			//A相
#define	PHASE_B		0x2000			//B相
#define	PHASE_C		0x3000			//C相
#define	PHASE_N		0x4000			//零线

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
//60(400)A表
#define CURR_400A		5
//行业外表类型
#define CURR_NORMAL		6
//50(250)A表
#define CURR_250A		7

//////////////////
//通讯类型
//近红外
#define	COMM_NEAR		0
//远红外
#define COMM_FAR		1

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
#define CHIP_RN2026			9

//存储芯片定义
#define CHIP_AT45DB161		(5)
#define CHIP_AT45DB321		(7)
#define CHIP_24LC256		(8)
#define CHIP_24LC512		(9)
#define CHIP_GD25Q64C		(10)
#define CHIP_GD25Q32C		(11)
#define CHIP_GD25Q16C		(12)
#define CHIP_NO				(6)


//不同的液晶定义
#define LCD_GW3PHASE_09		21		//09年8月国网三相表液晶
#define LCD_HT_SINGLE_METER 23
#define LCD_HT_NO			25

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
#define RTC_NO							3
#define RTC_RN2026						4

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

#define PPRECISION_1	 		0x01	//1级表
#define PPRECISION_2			0x02	//2级表
#define PPRECISION_05S			0x03	//0.5S级表
#define PPRECISION_02S			0x04	//0.2S级表


//板型定义（5位数，单相以1开头，三相以3开头）
#define BOARD_HT_SINGLE_78201662      	11662//面向对象698规范单相表
#define BOARD_HT_SINGLE_78201702      	11702//面向对象698规范单相表 3.3v系统，带flash给江苏供货
#define BOARD_VG_SINGLE_78201802		11802//面向对象698规范单相表 国网芯

#define BOARD_HT_THREE_0131037      	31037//6025+7038+698+模块
#define BOARD_HT_THREE_0134566          34566//6025+7038+698+模块+ESAM可单独控制
#define BOARD_HT_THREE_0131766			31766//6025+7038+698+模块+ESAM可单独控制+一体化外置表
#define BOARD_HT_THREE_0131074			31074//6025+7038+698+（开关电源 或 485费控）
#define BOARD_HT_THREE_0131699			31699//6025+7038+698+（开关电源 或 485费控）ESAM可单独控制
#define BOARD_TI_THREE_78202317			31700//6025+2026
#define BOARD_TI_THREE_7820698      	36982//5438+7026+645（还未调试）
#define BOARD_ST_THREE_0131212          36983//面向对象698规范三相表 ST

#define BOARD_HC_MEASURINGSWITCH		52700//HC32F460 + RN7326	

//CPU类型
#define CPU_HT6015              5
#define CPU_HT6025              6
#define CPU_ST091               7
#define CPU_VG8530              8
#define CPU_HC32F460			9
#define CPU_HC32F4A0			10

//串口类型定义
#define SERIAL_HARD_SCI				0
#define SERIAL_SIM_SCI				1

// ADC通道定义
#define SYS_POWER_AD		    		0//系统电压
#define	SYS_CLOCK_VBAT_AD		    	1//时钟电池电压
#define	SYS_READ_VBAT_AD	    		2//其他的电池
#define	SYS_TEMPERATURE_AD		    	3//温度AD采样

//红外唤醒类型
#define IR_WAKEUP_NO_FUNC			0//无红外唤醒功能
#define IR_WAKEUP_PHOTO_SWITCH		1//光敏管唤醒
#define IR_WAKEUP_UART_COMM			2//uart接收唤醒

typedef void (*Fun)( BYTE Step );
#define 	OP_FUNCTION(Step)		((Fun)FunctionConst)(Step)

typedef enum
{
	ePowerOnMode = 0,	//上电模式
	ePowerDownMode,		//掉电模式
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
	eCOMPONENT_SPI_FLASH,		//外置FLASH
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
	eRS485_I = 0,	//485
	eIR,			//红外
	eCR,			//载波
	eRS485_II,		//第二路485
	eUART_SW,		//与开关本体通信
}eSERIAL_TYPE;

typedef enum
{
	//这4个顺序不能动
	ePT_4851 = 0,//4851（维护用）
	ePT_4852,	//4852（抄表用）
	ePT_SW,	  //串口（与本体通信）
	ePT_HPLC,	//串口（载波）
}ePORT_TYPE;
#endif//对应文件最前面的#ifndef __DEFINE_INCLUDE

