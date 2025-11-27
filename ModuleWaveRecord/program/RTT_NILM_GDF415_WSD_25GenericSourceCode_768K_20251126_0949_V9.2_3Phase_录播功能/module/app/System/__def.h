#ifndef __DEFINE_INCLUDE
#define __DEFINE_INCLUDE


// #define __no_init __attribute__((section("NO_INIT"),zero_init))

///////////////////////////////////////////////////
//数据类型定义
#define BOOL	unsigned char
#define BYTE	unsigned char
#define WORD    unsigned short
#define SWORD   short
#define INT	    int
#define DWORD	unsigned long
#define SDWORD	long
#if(!WIN32)
#define	QWORD   unsigned long long
#define SQWORD	long long
#else
#define	QWORD   unsigned __int64
#define SQWORD	__int64
#endif

#define FLOAT	float
#define DOUBLE	double

//#define NULL	0

// 位定义      
#define BIT0    (0x00000001)
#define BIT1    (0x00000002)
#define BIT2    (0x00000004)
#define BIT3    (0x00000008)
#define BIT4    (0x00000010)
#define BIT5    (0x00000020)
#define BIT6    (0x00000040)
#define BIT7    (0x00000080)
#define BIT8    (0x00000100)
#define BIT9    (0x00000200)
#define BIT10   (0x00000400)
#define BIT11   (0x00000800)
#define BIT12   (0x00001000)
#define BIT13   (0x00002000)
#define BIT14   (0x00004000)
#define BIT15   (0x00008000)
#define BIT16   (0x00010000)
#define BIT17   (0x00020000)
#define BIT18   (0x00040000)
#define BIT19   (0x00080000)
#define BIT20   (0x00100000)
#define BIT21   (0x00200000)
#define BIT22   (0x00400000)
#define BIT23   (0x00800000)
#define BIT24   (0x01000000)
#define BIT25   (0x02000000)
#define BIT26   (0x04000000)
#define BIT27   (0x08000000)
#define BIT28   (0x10000000)
#define BIT29   (0x20000000)
#define BIT30   (0x40000000)
#define BIT31   (0x80000000)

#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

#define SQUARE(a)	((a)*(a))		// 平方

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

/***********************************
 * CPU频率 
*/
#define CPU_FREQ_160M       160
#define CPU_FREQ_200M       200
#define CPU_FREQ_320M       320
#define CPU_FREQ_400M       400
#define CPU_FREQ_480M       480

#define CPU_FREQ_240M       240

/* 相数 */
typedef enum{
	PH_A = 0,
    #if(MAX_PHASE==3)
	PH_B,
	PH_C,
    #endif
	PH_NUM// 默认3相
}PHARSE;	

typedef enum
{
    C2U2I_U0A_1U3I_U0A = 0,
    #if(MAX_PHASE==3)
    C2U2I_U0B_1U3I_U0B,
    C2U2I_U0C_1U3I_U0C,
    #endif
    C2U2I_I0A_1U3I_I0A,
    #if(MAX_PHASE==3)
    C2U2I_I0B_1U3I_I0B,
    C2U2I_I0C_1U3I_I0C,
    #endif
    CHMAX
}CHNUM_INFO;

/*半周波极值*/
typedef enum
{
    MinRms = 0,
    MaxRms = 1
}HALFCYCRMSINFO;

/* 暂态事件类别 */
typedef enum{
     TRA_EVT_VOL_DIP = 0,// 电压暂降
     TRA_EVT_VOL_SWL, // 电压暂升
     TRA_EVT_VOL_INT, // 电压中断
     TRA_EVT_MAX
}TRA_EVT_TYPE;


typedef enum{
	 EVT_BEGIN = 0,/*开始阈值*/
	 EVT_END,/*结束阈值*/
	 EVT_PARA_MAX
}EVT_PARA;



//存储芯片定义
#define CHIP_AT45DB161		(5)
#define CHIP_AT45DB321		(7)
#define CHIP_24LC256		(8)
#define CHIP_24LC512		(9)
#define CHIP_GD25Q64C		(10)
#define CHIP_GD25Q32C		(11)
#define CHIP_GD25Q16C		(12)
#define CHIP_NO				(6)
#define CHIP_M24512_RMN6TP	(14)


//板型定义
#define BOARD_HT_SINGLE_78201662      	11662//面向对象698规范单相表
#define BOARD_HT_SINGLE_78201702      	11702//面向对象698规范单相表 3.3v系统，带flash给江苏供货
#define BOARD_VG_SINGLE_78201802		11802//面向对象698规范单相表 国网芯

#define BOARD_HT_THREE_0131037      	31037//6025+7038+698+模块
#define BOARD_HT_THREE_0134566          34566//6025+7038+698+模块+ESAM可单独控制
#define BOARD_HT_THREE_0131766			31766//6025+7038+698+模块+ESAM可单独控制+一体化外置表
#define BOARD_HT_THREE_0131074			31074//6025+7038+698+（开关电源 或 485费控）
#define BOARD_HT_THREE_0131699			31699//6025+7038+698+（开关电源 或 485费控）ESAM可单独控制
#define BOARD_TI_THREE_7820698      	36982//5438+7026+645（还未调试）
#define BOARD_ST_THREE_0131212          36983//面向对象698规范三相表 ST
#define BOARD_ST_THREE_0132112          32112//面向对象698规范三相表 ST-esam电源可控-ESAMSPI独立

#define BOARD_ST_PQM                    70001// ST版的电能质量模块


//CPU类型
#define CPU_HT6015              5
#define CPU_HT6025              6
#define CPU_ST091               7
#define CPU_VG8530              8
#define CPU_STH7                9
#define CPU_GD32F415_256K       10
#define CPU_GD32F415_768K       11


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
	ePowerDownMode		//掉电模式
}ePOWER_MODE;

typedef enum
{
	ePHASE_ALL = 0,
	ePHASE_A,
	ePHASE_B,
	ePHASE_C
}ePHASE_TYPE;

//SPI各器件类型
typedef enum
{
	eCOMPONENT_SPI_SAMPLE = 0,	//计量芯
	eCOMPONENT_SPI_FLASH,		//外置FLASH
	eCOMPONENT_TOTAL_NUM
}eCOMPONENT_TYPE;


//SPI模式 0 1 2 3
typedef enum
{
	eSPI_MODE_0 = 0,      
	eSPI_MODE_1 = 1, 
	eSPI_MODE_2 = 2,
	eSPI_MODE_3 = 3
}eSPI_MODE;

#define CS_SPI_EEPROM					16  //指EEPROM芯片
#define CS_SPI_EEPROM2 					17  //第二片EEPROM
#define CS_SPI_FLASH                    12  //指Flash芯片
#define CS_SPI_SAMPLE               	23  //指计量芯

typedef enum
{
	//这个顺序不能动
	eUSART_I = 0,

}eSERIAL_TYPE;


#endif// #ifndef __DEFINE_INCLUDE

