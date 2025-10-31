//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	马亮
//创建日期	2017-02-06
//描述		单相表液晶显示处理
//修改记录	显示程序中的OAD都是按照小端模式保存处理，调用规约函数时将OAD倒序后调用
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Lcd_HT_SINGLE.h"
#include "LCD_115200_SINGLE.H"

#if( LCD_TYPE == LCD_HT_SINGLE_METER )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//显示单位枚举
enum
{
	eLcdUnit_KWH=0,		
	eLcdUnit_KVARH,		
	eLcdUnit_KW,		
	eLcdUnit_KVAR,		
	eLcdUnit_VA,		
	eLcdUnit_V,			
	eLcdUnit_A,			
	eLcdUnit_KVAH,		
	eLcdUnit_KVA,		
	eLcdUnit_AH,		
	eLcdUnit_WAN,		
	eLcdUnit_YUAN,		
	eLcdUnit_WANYUAN,	
	eLcdUnit_INVALID,
};

enum
{
	eLcdPowerOnMode=0,			// 上电全显模式
	eLcdLoopMode,				// 循环显示模式
	eLcdKeyMode,				// 按键显示模式
	eLcdUpDownKeyMode,			// 双按键显示模式
	eLcdEchoInfoMode,			// 插入交互信息模式
};

enum
{
	eLcdStatusSegFlag=0,		//液晶下部状态段标志
	eLcdWarnFlag,				//表内状态字标志
};


typedef struct TLcdCtrl_t
{
	BYTE	Mode;				//显示模式
	BYTE	Timer;				//显示定时器
	BYTE	Screen;				//显示屏号
	WORD	ErrNo;				//错误序号  大于0x100，扩展类	小于0x100，标准类 0xffff没有错误
}TLcdCtrl;

typedef struct TLcdDispTimer_t
{
	BYTE Led;					//led灯点亮时间
	BYTE CommFlag[MAX_COM_CHANNEL_NUM];	//通信符号点亮时间
	BYTE InitDriver;			//初始化液晶驱动定时器
	BYTE BackLight;				//背光点亮时间
	BYTE ReadCard[2];				//读卡结果
}TLcdDispTimer;


typedef struct TLcdItemOAD_t
{
	TFourByteUnion	MainOAD;	//主OAD
	TFourByteUnion	SubOAD;		//第二个OAD
	BYTE    		OADNum;		//OAD的个数
	BYTE    		ScreenOrder;//显示屏序号

}TLcdItemOAD;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
#if( (BOARD_TYPE == BOARD_HT_SINGLE_78201602 )||( BOARD_TYPE == BOARD_HT_SINGLE_20250819 ))
//数码管7段的控制pin口(0),位数(1)	数码管数量 从0到6 a,b,c,d,e,f,g
// const static BYTE DigTubCtrl[2][LCD_DIGITAL_TUBE_NUM][7] = 
// {	//数码管7段的控制pin口
// 	{
// 		{20,19,19,20,21,21,20},		//1	pin口
// 		{ 9,10,10, 9, 8, 8, 9},		//2
// 		{23,22,22,23,23,23,22},		//3
// 		{21,20,20,21,21,21,20},		//4
// 		{19,18,18,19,19,19,18},		//5
// 		{ 0, 1, 1, 0, 0, 0, 1},		//6
// 		{ 2, 3, 3, 2, 2, 2, 3},		//7
// 		{ 4, 5, 5, 4, 4, 4, 5},		//8
// 		{ 6, 7, 7, 6, 6, 6, 7},		//9
// 		{ 8, 9, 9, 8, 8, 8, 9},		//10
// 		{16,17,17,16,16,16,17},		//11
// 		{14,15,15,14,14,14,15},		//12
// 	},
// 	//数码管7段的控制位数(1) a b c d e f g
// 	{
// 		{0x20,0x20,0x10,0x08,0x10,0x20,0x10},	//1
// 		{0x20,0x20,0x10,0x08,0x10,0x20,0x10},	//2
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//3
// 		{0x08,0x04,0x01,0x01,0x02,0x04,0x02},	//4
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//5
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//6
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//7	
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//8
// 		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},	//9	
// 		{0x08,0x04,0x01,0x01,0x02,0x04,0x02},	//10
// 		{0x04,0x04,0x10,0x20,0x10,0x08,0x08},	//11
// 		{0x20,0x20,0x08,0x04,0x08,0x10,0x10},	//12
// 	}
// };
const static BYTE DigTubCtrl[2][LCD_DIGITAL_TUBE_NUM][7] = 
{	//数码管7段的控制pin口
	{
		{SEG20, SEG19, SEG19, SEG20, SEG21, SEG21, SEG20},    // 1号数码管
		{SEG10, SEG11, SEG11, SEG10, SEG09, SEG09, SEG10},    // 2号数码管
		{SEG23, SEG22, SEG22, SEG23, SEG23, SEG23, SEG22},    // 3号数码管
		{SEG21, SEG20, SEG20, SEG21, SEG21, SEG21, SEG20},    // 4号数码管
		{SEG19, SEG18, SEG18, SEG19, SEG19, SEG19, SEG18},    // 5号数码管
		{SEG01, SEG02, SEG02, SEG01, SEG01, SEG01, SEG02},    // 6号数码管
		{SEG03, SEG04, SEG04, SEG03, SEG03, SEG03, SEG04},    // 7号数码管
		{SEG05, SEG06, SEG06, SEG05, SEG05, SEG05, SEG06},    // 8号数码管
		{SEG07, SEG08, SEG08, SEG07, SEG07, SEG07, SEG08},    // 9号数码管
		{SEG09, SEG10, SEG10, SEG09, SEG09, SEG09, SEG10},    // 10号数码管
		{SEG16, SEG17, SEG17, SEG16, SEG16, SEG16, SEG17},    // 11号数码管
		{SEG14, SEG15, SEG15, SEG14, SEG14, SEG14, SEG15},    // 12号数码管
	},
	//数码管7段的控制位数(1) a b c d e f g
	{
		{COM6, COM6, COM5, COM4, COM5, COM6, COM5},           // 1号数码管
		{COM6, COM6, COM5, COM4, COM5, COM6, COM5},           // 2号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 3号数码管
		{COM4, COM3, COM1, COM1, COM2, COM3, COM2},           // 4号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 5号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 6号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 7号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 8号数码管
		{COM4, COM4, COM2, COM1, COM2, COM3, COM3},           // 9号数码管
		{COM4, COM3, COM1, COM1, COM2, COM3, COM2},           // 10号数码管
		{COM3, COM3, COM5, COM6, COM5, COM4, COM4},           // 11号数码管
		{COM6, COM6, COM4, COM3, COM4, COM5, COM5},           // 12号数码管
	}
};

//芯片和液晶的连接关系	
const static BYTE CPULCDCONNECT[24] = {30,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
#else
//数码管7段的控制pin口(0),位数(1)	数码管数量 从0到6 a,b,c,d,e,f,g
const static BYTE DigTubCtrl[2][LCD_DIGITAL_TUBE_NUM][7] = 
{	//数码管7段的控制pin口
	{
		{ SEG19, SEG18, SEG18, SEG19, SEG20, SEG20, SEG19 },      //1	pin口
		{ SEG09, SEG10, SEG10, SEG09, SEG08, SEG08, SEG09 },      //2
		{ SEG22, SEG21, SEG21, SEG22, SEG22, SEG22, SEG21 },      //3
		{ SEG20, SEG19, SEG19, SEG20, SEG20, SEG20, SEG19 },      //4
		{ SEG18, SEG17, SEG17, SEG18, SEG18, SEG18, SEG17 },      //5
		{ SEG00, SEG01, SEG01, SEG00, SEG00, SEG00, SEG01 },      //6
		{ SEG02, SEG03, SEG03, SEG02, SEG02, SEG02, SEG03 },      //7
		{ SEG04, SEG05, SEG05, SEG04, SEG04, SEG04, SEG05 },      //8
		{ SEG06, SEG07, SEG07, SEG06, SEG06, SEG06, SEG07 },      //9
		{ SEG08, SEG09, SEG09, SEG08, SEG08, SEG08, SEG09 },      //10
		{ SEG15, SEG16, SEG16, SEG15, SEG15, SEG15, SEG16 },      //11
		{ SEG13, SEG14, SEG14, SEG13, SEG13, SEG13, SEG14 },      //12
	},
	//数码管7段的控制位数(1) a b c d e f g
	{
		{ COM5, COM5, COM4, COM3, COM4, COM5, COM4 },   //1
		{ COM5, COM5, COM4, COM3, COM4, COM5, COM4 },   //2
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //3
		{ COM3, COM2, COM0, COM0, COM1, COM2, COM1 },   //4
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //5
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //6
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //7
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //8
		{ COM3, COM3, COM1, COM0, COM1, COM2, COM2 },   //9
		{ COM3, COM2, COM0, COM0, COM1, COM2, COM1 },   //10
		{ COM2, COM2, COM4, COM5, COM4, COM3, COM3 },   //11
		{ COM5, COM5, COM3, COM2, COM3, COM4, COM4 },   //12
	}
};
#endif//#if ( BOARD_TYPE == BOARD_HT_SINGLE_78202201 )
//符号对应数码管段码数组	BYTE8位中0x80闲置,0x40->g,0x20->f……0x01->a
//目前只支持0-f，再增加可以直接在这里改
//						  				  		  0    1    2    3     4    5    6    7    8    9 
const static BYTE DigTubCode[LCD_SIGN_NUM] = {	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7f,0x6f,
//						  				 		 10A  11B  12C   13D  14E  15F 16空白  17J  18U  19t   20-   21r  22P	 23_
												0x77,0x7C,0x39,0x5E,0x79,0x71,0x00,0x0E,0x3E,0x78,0x40,0x50,0x73,0x08};
//可显示小数点数极限值							0位小数	1		2		3
const static QWORD DigTubNumRange[] = {9999999,999999,99999,9999};

static BYTE LCDKey;

//键显项目默认设置
const BYTE LcdItemTable[200][10] =                
{
//	  0		1		2	3		4	5		6	7		8	9
	{0x00, 0x00, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第0项	不要设置此项！！！！！！
	{0x20, 0x2C, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第1项	当前剩余金额
	{0x00, 0x00, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第2项	当前组合有功总电量
	{0x00, 0x00, 0x02, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第3项	当前组合有功尖电量
	{0x00, 0x00, 0x02, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第4项	当前组合有功峰电流
	{0x00, 0x00, 0x02, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第5项	当前组合有功平电量
	{0x00, 0x00, 0x02, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第6项	当前组合有功谷电量	
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x02, 0x00},//第7项	上1月组合有功总电量 
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x02, 0x02, 0x00},//第8项	上1月组合有功尖电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x03, 0x02, 0x00},//第9项	上1月组合有功峰电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x04, 0x02, 0x00},//第10项	上1月组合有功平电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x05, 0x02, 0x00},//第11项	上1月组合有功谷电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x02, 0x01, 0x02, 0x00},//第12项	上2月组合有功总电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x02, 0x02, 0x02, 0x00},//第13项	上2月组合有功尖电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x02, 0x03, 0x02, 0x00},//第14项	上2月组合有功峰电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x02, 0x04, 0x02, 0x00},//第15项	上2月组合有功平电量 
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x02, 0x05, 0x02, 0x00},//第16项	上2月组合有功谷电量
	{0x20, 0x1A, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第17项	当前电价
	{0x40, 0x03, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第18项	用户户号低8位
	{0x40, 0x03, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x01},//第19项	用户户号高4位
	{0x40, 0x01, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第20项	通信地址低8位		40010200
	{0x40, 0x01, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x01},//第21项	通信地址高4位
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x01},//第22项	当前日期			40000200
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第23项	当前时间
	{0x20, 0x00, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第24项	瞬时电压			20000201
	{0x20, 0x01, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第25项	火线电流			20010201
	{0x20, 0x04, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第26项	瞬时功率			20040201
	{0x20, 0x0A, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第27项	功率因数			200A0201
	{0x00, 0x10, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第28项	当前正向有功总电量
	{0x00, 0x10, 0x02, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第29项	当前正向有功尖电量
	{0x00, 0x10, 0x02, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第30项	当前正向有功峰电流
	{0x00, 0x10, 0x02, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第31项	当前正向有功平电量
	{0x00, 0x10, 0x02, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第32项	当前正向有功谷电量
	{0x00, 0x20, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第33项	当前反向有功总电量
	{0x00, 0x20, 0x02, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第34项	当前反向有功尖电量
	{0x00, 0x20, 0x02, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第35项	当前反向有功峰电流
	{0x00, 0x20, 0x02, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第36项	当前反向有功平电量
	{0x00, 0x20, 0x02, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第37项	当前反向有功谷电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x02, 0x01, 0x02, 0x00},//第38项	上1月正向有功总电量 
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x02, 0x02, 0x02, 0x00},//第39项	上1月正向有功尖电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x02, 0x03, 0x02, 0x00},//第40项	上1月正向有功峰电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x02, 0x04, 0x02, 0x00},//第41项	上1月正向有功平电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x02, 0x05, 0x02, 0x00},//第42项 上1月正向有功谷电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x02, 0x01, 0x02, 0x00},//第43项 上1月反向有功总电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x02, 0x02, 0x02, 0x00},//第44项 上1月反向有功尖电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x02, 0x03, 0x02, 0x00},//第45项 上1月反向有功峰电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x02, 0x04, 0x02, 0x00},//第46项 上1月反向有功平电量 
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x02, 0x05, 0x02, 0x00},//第47项 上1月反向有功谷电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x02, 0x01, 0x02, 0x00},//第48项 上2月正向有功总电量 
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x02, 0x02, 0x02, 0x00},//第49项	上2月正向有功尖电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x02, 0x03, 0x02, 0x00},//第50项	上2月正向有功峰电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x02, 0x04, 0x02, 0x00},//第51项	上2月正向有功平电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x02, 0x05, 0x02, 0x00},//第52项	上2月正向有功谷电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x02, 0x01, 0x02, 0x00},//第53项	上2月反向有功总电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x02, 0x02, 0x02, 0x00},//第54项	上2月反向有功尖电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x02, 0x03, 0x02, 0x00},//第55项	上2月反向有功峰电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x02, 0x04, 0x02, 0x00},//第56项	上2月反向有功平电量 
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x02, 0x05, 0x02, 0x00},//第57项	上2月反向有功谷电量
	{0x20, 0x01, 0x04, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第58项	零线电流
	{0x41, 0x16, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第59项 	第1结算日
	{0x40, 0x02, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x01},//第60项	表号高4位
	{0x40, 0x02, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第61项	表号低8位
	//下面为高精度电能
	{0x00, 0x00, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第62项	当前组合有功总高精度电量
	{0x00, 0x00, 0x04, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第63项	当前组合有功尖高精度电量
	{0x00, 0x00, 0x04, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第64项	当前组合有功峰高精度电量
	{0x00, 0x00, 0x04, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第65项 当前组合有功平高精度电量
	{0x00, 0x00, 0x04, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第66项 当前组合有功谷高精度电量	
	{0x00, 0x10, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第67项 当前正向有功总高精度电量
	{0x00, 0x10, 0x04, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第68项 当前正向有功尖高精度电量
	{0x00, 0x10, 0x04, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第69项	当前正向有功峰高精度电量
	{0x00, 0x10, 0x04, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第70项 当前正向有功平高精度电量
	{0x00, 0x10, 0x04, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第71项	当前正向有功谷高精度电量
	{0x00, 0x20, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第72项	当前反向有功总高精度电量
	{0x00, 0x20, 0x04, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第73项	当前反向有功尖高精度电量
	{0x00, 0x20, 0x04, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第74项	当前反向有功峰高精度电量
	{0x00, 0x20, 0x04, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第75项	当前反向有功平高精度电量
	{0x00, 0x20, 0x04, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第76项	当前反向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x01, 0x02, 0x00},//第77项	上1月组合有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x02, 0x02, 0x00},//第78项	上1月组合有功尖高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x03, 0x02, 0x00},//第79项	上1月组合有功峰高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x04, 0x02, 0x00},//第80项	上1月组合有功平高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x05, 0x02, 0x00},//第81项	上1月组合有功谷高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第82项	上1月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x02, 0x02, 0x00},//第83项	上1月正向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x03, 0x02, 0x00},//第84项	上1月正向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x04, 0x02, 0x00},//第85项	上1月正向有功平高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x05, 0x02, 0x00},//第86项	上1月正向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第87项	上1月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x02, 0x02, 0x00},//第88项	上1月反向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x03, 0x02, 0x00},//第89项	上1月反向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x04, 0x02, 0x00},//第90项	上1月反向有功平高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x05, 0x02, 0x00},//第91项	上1月反向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x01, 0x02, 0x00},//第92项	上2月组合有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x02, 0x02, 0x00},//第93项	上2月组合有功尖高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x03, 0x02, 0x00},//第94项	上2月组合有功峰高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x04, 0x02, 0x00},//第95项	上2月组合有功平高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x05, 0x02, 0x00},//第96项	上2月组合有功谷高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第97项	上2月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x02, 0x02, 0x00},//第98项	上2月正向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x03, 0x02, 0x00},//第99项	上2月正向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x04, 0x02, 0x00},//第100项	上2月正向有功平高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x05, 0x02, 0x00},//第101项	上2月正向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第102项	上2月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x02, 0x02, 0x00},//第103项	上2月反向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x03, 0x02, 0x00},//第104项	上2月反向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x04, 0x02, 0x00},//第105项	上2月反向有功平高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x05, 0x02, 0x00},//第106项	上2月反向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x03, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第107项	上3月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x03, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第108项	上3月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x04, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第109项	上4月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x04, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第110项	上4月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x05, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第111项	上5月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x05, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第112项	上5月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x06, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第113项	上6月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x06, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第114项	上6月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x07, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第115项	上7月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x07, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第116项	上7月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x08, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第117项	上8月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x08, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第118项	上8月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x09, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第119项	上9月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x09, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第120项	上9月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0A, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第121项	上10月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0A, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第122项	上10月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0B, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第123项	上11月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0B, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第124项	上11月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0C, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第125项	上12月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x0C, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第126项	上12月反向有功总高精度电量
	{0x41, 0x09, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第127项	有功脉冲常数
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第128项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第129项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第130项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第131项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第132项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第133项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第134项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第135项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第136项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第137项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第138项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第139项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第140项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第141项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第142项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第143项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第144项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第145项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第146项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第147项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第148项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第149项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第150项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第151项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第152项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第153项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第154项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第155项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第156项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第157项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第158项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第159项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第160项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第161项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第162项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第163项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第164项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第165项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第166项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第167项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第168项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第169项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第170项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第171项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第172项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第173项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第174项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第175项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第176项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第177项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第178项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第179项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第180项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第181项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第182项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第183项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第184项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第185项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第186项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第187项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第188项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第189项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第190项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第191项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第192项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第193项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第194项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第195项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第196项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第197项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第198项	当前时间
	{0x40, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02},//第199项	当前时间
};

//异常报警事件列表( 64个 ),顺序不变
static const BYTE EventWarnErrorTab[]=
{	
	// -- 负荷开关误动			err -- 1
	#if( SEL_EVENT_RELAY_ERR == YES )
	eSUB_EVENT_RELAY_ERR,
	#else
	0xff,
	#endif
	
	// -- ESAM错误			err -- 2
	0xff,
	// -- 内卡初始化错误			err -- 3
	0xff,
	// -- 时钟电池电压低			err -- 4
	0xff,
	// -- 内部程序错误			err -- 5
	0xff,
	// -- 存储器故障或损坏			err -- 6
	0xff,
	// -- 控制回路错误			err -- 7
	0xff,
	// -- 时钟故障				err -- 8
	0xff,
	
	// -- 停电抄表电池欠压			err -- 9
	0xff,
	// -- 透支状态				err -- 10
	0xff,						//目前没有
	
	// -- 开表盖				err -- 11
	#if( SEL_EVENT_METERCOVER == YES )
	eSUB_EVENT_METER_COVER,					
	#else
	0xff,
	#endif
	
	// -- 开端钮盖				err -- 12
	#if( SEL_EVENT_BUTTONCOVER == YES )
	eSUB_EVENT_BUTTON_COVER,					
	#else
	0xff,
	#endif
	
	// -- 恒定磁场干扰			err -- 13
	#if( SEL_EVENT_MAGNETIC_INT == YES )
	eSUB_EVENT_MAGNETIC_INT, 				
	#else
	0xff,
	#endif
	
	// -- 电源异常				err -- 14
	#if( SEL_EVENT_POWER_ERR == YES )
	eSUB_EVENT_POWER_ERR,					
	#else
	0xff,
	#endif
	
	// -- 跳闸					err -- 15
	// -- 合闸					err -- 16
	0xff,
	0xff,
	
	// -- 失压					err -- 17
	#if ( SEL_EVENT_LOST_V == YES )
	eSUB_EVENT_LOSTV_A,
	#else
	0xff,
	#endif
	
	// -- 欠压					err -- 18
	#if( SEL_EVENT_WEAK_V == YES )
	eSUB_EVENT_WEAK_V_A,			
	#else
	0xff,
	#endif
		
	// -- 过压					err -- 19
	#if( SEL_EVENT_OVER_V == YES )
	eSUB_EVENT_OVER_V_A,						
	#else
	0xff,
	#endif

	// -- 失流					err -- 20
	#if( SEL_EVENT_LOST_I == YES )
	eSUB_EVENT_LOSTI_A,						
	#else
	0xff,
	#endif
	
	// -- 过流					err -- 21
	#if( SEL_EVENT_OVER_I == YES )
	eSUB_EVENT_OVER_I_A,						
	#else
	0xff,
	#endif
	
	// -- 过载					err -- 22
	#if( SEL_EVENT_OVERLOAD == YES )
	eSUB_EVENT_OVERLOAD_A, 					
	#else
	0xff,
	#endif
	
	// -- 功率反向					err -- 23
	#if( SEL_EVENT_BACKPROP == YES )
	eSUB_EVENT_POW_BACK_A, 					
	#else
	0xff,
	#endif
	
	// -- 断相					err -- 24
	#if( SEL_EVENT_BREAK == YES )
	eSUB_EVENT_BREAK_A,						
	#else
	0xff,
	#endif
	
	// -- 断流					err -- 25
	#if( SEL_EVENT_BREAK_I == YES )
	eSUB_EVENT_BREAK_I_A,						
	#else
	0xff,
	#endif
	
	// -- 备用					err -- 26
	0xff,
	
	// -- 备用					err -- 27
	0xff,
	// -- 备用					err -- 28
	0xff,
	// -- 备用					err -- 29
	0xff,
	// -- 备用					err -- 30
	0xff,
	// -- 备用					err -- 31
	0xff,
	// -- 备用					err -- 32
	0xff,

	// -- 电压逆相序				err -- 33
	#if( SEL_EVENT_V_REVERSAL == YES )
	eSUB_EVENT_V_REVERSAL,					
	#else
	0xff,
	#endif
	
	// -- 电流逆相序				err -- 34
	#if( SEL_EVENT_I_REVERSAL == YES )
	eSUB_EVENT_I_REVERSAL,					
	#else
	0xff,
	#endif
	
	// -- 电压不平衡				err -- 35
	#if( SEL_EVENT_V_UNBALANCE == YES )
	eSUB_EVENT_V_UNBALANCE,					
	#else
	0xff,
	#endif
	
	// -- 电流不平衡				err -- 36
	#if( SEL_EVENT_I_UNBALANCE == YES )
	eSUB_EVENT_I_UNBALANCE,					
	#else
	0xff,
	#endif
	
	// -- 辅助电源失电			err -- 37
	#if( SEL_EVENT_LOST_SECPOWER == YES )
	eSUB_EVENT_LOST_SECPOWER,				
	#else
	0xff,
	#endif
	
	// -- 掉电（能工作时）			err -- 38
	#if( SEL_EVENT_LOST_POWER == YES )
	eSUB_EVENT_LOST_POWER,					
	#else
	0xff,
	#endif
	
	// -- 正向和反向有功需量超限				err -- 39
	0xff,
	
	// -- 总功率因数超下限					err -- 40
	#if( SEL_EVENT_COS_OVER == YES )
	eSUB_EVENT_COS_OVER_ALL, 				
	#else
	0xff,
	#endif
	
	// -- 电流严重不平衡					err -- 41
	#if( SEL_EVENT_I_S_UNBALANCE == YES )
	eSUB_EVENT_S_I_UNBALANCE,				
	#else
	0xff,
	#endif
	
	// -- 总有功功率反向					err -- 42
	#if( SEL_EVENT_BACKPROP == YES )
	eSUB_EVENT_POW_BACK_ALL,				
	#else
	0xff,
	#endif
	
	// -- 全失压（能工作时）					err -- 43
	#if( SEL_EVENT_LOST_ALL_V == YES )
	eSUB_EVENT_LOST_ALL_V,					
	#else
	0xff,
	#endif
	
	// -- 备用					err -- 44
	0xff,
	// -- 有功电能方向改变		err -- 45
	0xff,
	// -- 备用					err -- 46
	0xff,
	// -- 备用					err -- 47
	0xff,
	// -- 备用					err -- 48
	0xff,
	
	// -- 编程					err -- 49
	0xff,
	
	// -- 电表清零					err -- 50
	0xff,
	
	// -- 需量清零					err -- 51
	0xff,
	
	// -- 事件清零					err -- 52
	0xff,
	
	// -- 校时					err -- 53
	0xff,
	
	// -- 时段表编程					err -- 54
	0xff,
	
	// -- 时区表编程					err -- 55
	0xff,
	
	// -- 周休日编程					err -- 56
	0xff,
	
	// -- 节假日编程					err -- 57
	0xff,
	
	// -- 有功组合方式编程				err -- 58
	0xff,
	
	// -- 无功组合方式1编程 			err -- 59
	0xff,
	
	// -- 无功组合方式2编程 			err -- 60	      
	0xff,
	
	// -- 结算日编程					err -- 61
	0xff,
	
	// -- 费率参数表编程				err -- 62
	0xff,
	// -- 阶梯表编程					err -- 63
	0xff,
	// -- 密钥更新					err -- 64
	0xff,

};

//显示读数据使用的缓冲
BYTE lcd[LCD_BUF_LENGTH];        				
//报警模式字
TWarnDispPara LcdWarnModeWord;
//报警状态字
static BYTE LcdWarnStatusWord[MAX_WARN_ERR_NUM];
//状态位显示状态字
static BYTE g_LcdStatusSegFlag[(eLcdFlagT+7)/8];
//按键检测计数器
TLcdKeyDetTimer LcdKeyDetTimer;
//显示控制寄存器
TLcdCtrl	LcdCtrl;
//当前显示项
TLcdItemOAD	LcdItemOAD;
//闪烁显示标志位 TRUE 显示 FALSE 熄灭  
static BOOL LcdFlashFlagSec;		//秒闪烁
static BOOL LcdFlashFlagHalfSec;	//0.5秒闪烁				

TLcdDispTimer LcdDispTimer;

//无线信号强度，0-无信号，1~4强度递增
static BYTE SignalIntensity;
//显示缓冲区，用于存放待显示的数据
static TFourByteUnion LcdDData;	
//借位小数点数
static __no_init BYTE BorrDotNum;
#if(PREPAY_MODE == PREPAY_LOCAL)
//蜂鸣器插卡状态标志
static WORD BeepReadCardFlag = 0;
//插卡前剩余金额
static TFourByteUnion ReadCardBeforeBalance;
//插卡前剩余金额负号标志
static BYTE BalanceNegativeFlag;
//蜂鸣器插卡计时器
static WORD BeepTimeFlag = 0;
#endif

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void api_DisplayDF01(void);
static void DisplayVersion(BYTE Type);
static BOOL DigTubCtrlShow(BYTE ShowNum,BYTE DigTubNum);
static void CLRDigTub(BYTE DigTubNum);
static void DisplayStatus(void);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 读卡中显示8个"-"
//
//参数: 	无
//                    
//返回值:	无
//
//备注:
//-----------------------------------------------
void DoReadingCard(void)
{
	BYTE i;
	//直接修改lcd缓存数组插入1屏，所以写缓存前把缓存清掉
	memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
	DisplayStatus();		//要求保留状态符号
	for(i = 2; i < 10; i++)	//在中间显示8个“-”
	{
		DigTubCtrlShow(20,i);
	}	
}
#endif

//-----------------------------------------------
//函数功能: 把要显示的数据写到液晶驱动芯片
//
//参数: 	len[in]		要写入的数据长度
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
static void WriteLcdBufToDriver( BYTE len )
{
	BYTE i;
	
	for (i=0; i<len; i++)
	{
		HT_LCD->LCDBUF[i] = lcd[i];
	}
}


//-----------------------------------------------
//函数功能:	电能数据辅助处理函数
//
//参数:		TmpOAD	电能类型
//			Buf		数组地址
//返回值:  	
//		
//备注:	入参Buf大小最小8,必须注意，否则可能会冲内存
//-----------------------------------------------
void HandleEnergyData(TFourByteUnion TmpOAD ,BYTE *Buf)
{
	TEightByteUnion TempSavData;
	BYTE tPoint,tPoint1,type;

	TempSavData.ll = 0x00;
	type = TmpOAD.b[1] & 0x1f;
	
	memcpy(TempSavData.b,Buf,sizeof(TempSavData));//获取数据

	//只有组合电能有符号
	if((TmpOAD.w[1] == 0x0000) 
	|| ((TmpOAD.w[1] >= 0x0030) && (TmpOAD.w[1] <= 0x0033))
	|| ((TmpOAD.w[1] >= 0x0040) && (TmpOAD.w[1] <= 0x0043))) 
	{
		//判断正负数，取消负号，显示负号
		if(type == 0x02)//普通电能
		{
			if(TempSavData.l[0] < 0)
			{
				TempSavData.l[0] *= -1;
				SHOW_DATA_NEGATIVE; 				//LCD显示负号
			}
		}		
		else if(type == 0x04)//高精度电能
		{	
			if(TempSavData.ll < 0)
			{
				TempSavData.ll *= -1;
				SHOW_DATA_NEGATIVE; 				//LCD显示负号
			}		
		}
		else
		{	
		}

	}

	//小数点数处理，把数据统一处理成小数点数为借位功能小数点数，方便后续借位功能处理
	if(type == 0x04) //高精度电能
	{
		tPoint1 = 4;	//冻结高精度电能为4位小数
	}
	else	//普通电能
	{
		//电能类型出错，不是普通也不是高精度时默认2位小数
		tPoint1 = 2;	//冻结普通电能为2位小数
	}
	//得到所需小数点数的电能数据，多余的部分除掉
	if(tPoint1 > BorrDotNum)
	{
		TempSavData.ll = TempSavData.ll / lib_MyPow10(tPoint1 - BorrDotNum);
	}
	else if(tPoint1 < BorrDotNum)
	{
		TempSavData.ll = TempSavData.ll * lib_MyPow10(BorrDotNum - tPoint1);
	}
	else	//小数点数一样就不处理了
	{}
	
	memcpy(Buf,TempSavData.b,sizeof(TempSavData));//写回数据
}

//-----------------------------------------------
//函数功能: 获取四种显示报警状态及液晶下方状态位状态
//
//参数: 	Type[in]	eLcdStatusSegFlag	液晶下方状态位
//						eLcdWarnFlag		四种显示报警状态
// 			Order[in]	枚举号
//                    
//返回值:	TRUE	发生此种状态
//			FALSE	没发生此种状态
//备注:   
//-----------------------------------------------
static BOOL GetLcdStatusFlag(BYTE Type,BYTE Order)
{
	BYTE i,j;
	
	i = Order/8;
	j = Order%8;	
	
	if(Type == eLcdStatusSegFlag)
	{
		//错误编号不能太大
		if(Order > eLcdFlagT)
		{
			return FALSE;
		}
		
		if( (g_LcdStatusSegFlag[i] & (0x01<<j)) == 0 )
		{
			return FALSE;
		}
	}
	else// if(Type == eLcdWarnFlag)
	{
		//错误编号不能太大
		if(Order > (MAX_WARN_ERR_NUM*8))
		{
			return FALSE;
		}
		
		if( (LcdWarnStatusWord[i] & (0x01<<j)) == 0 )
		{
			return FALSE;
		}	
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 置四种显示报警状态及液晶下方状态位状态
//
//参数: 	Type[in]	eLcdStatusSegFlag	液晶下方状态位
//						eLcdWarnFlag		四种显示报警状态
// 			Order[in]	枚举号
//                    
//返回值:	TRUE/FALSE
//备注:   
//-----------------------------------------------
static BOOL SetLcdStatusFlag(BYTE Type,BYTE Order)
{
	BYTE i,j;
	
	i = Order/8;
	j = Order%8;	
	
	if(Type == eLcdStatusSegFlag)
	{
		//错误编号不能太大
		if(Order > eLcdFlagT)
		{
			return FALSE;
		}
		
		g_LcdStatusSegFlag[i] |= (0x01<<j);	
	}
	else// if(Type == eLcdWarnFlag)
	{
		//错误编号不能太大
		if(Order > (MAX_WARN_ERR_NUM*8))
		{
			return FALSE;
		}
		
		LcdWarnStatusWord[i] |= (0x01<<j);		
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 清四种显示报警状态及液晶下方状态位状态
//
//参数: 	Type[in]	eLcdStatusSegFlag	液晶下方状态位
//						eLcdWarnFlag		四种显示报警状态
// 			Order[in]	枚举号
//                    
//返回值:	TRUE/FALSE
//备注:   
//-----------------------------------------------
static BOOL ClrLcdStatusFlag(BYTE Type,BYTE Order)
{
	BYTE i,j;
	
	i = Order/8;
	j = Order%8;	
	
	if(Type == eLcdStatusSegFlag)
	{
		//错误编号不能太大
		if(Order > eLcdFlagT)
		{
			return FALSE;
		}
		
		g_LcdStatusSegFlag[i] &= ~(0x01<<j);
	}
	else// if(Type == eLcdWarnFlag)
	{
		//错误编号不能太大
		if(Order > (MAX_WARN_ERR_NUM*8))
		{
			return FALSE;
		}
		
		LcdWarnStatusWord[i] &= ~(0x01<<j);	
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 获取电能显示小数位数
//
//参数: 	无
//                   
//返回值:	小数位数
//
//备注:   
//-----------------------------------------------
static BYTE GetEnergyDot( void )
{
	BYTE tPoint;
	
	tPoint = FPara1->LCDPara.EnergyFloat;
	if( tPoint > 4 )
	{
		tPoint = 2;
	}
	return tPoint;
}

//-----------------------------------------------
//函数功能: 获取需量/功率显示小数位数
//
//参数: 	无
//                    
//返回值:	小数位数
//
//备注:   
//-----------------------------------------------
static BYTE GetDemandDot( void )
{
	BYTE tPoint;
	
	tPoint = FPara1->LCDPara.DemandFloat;
	if( tPoint > 4 )
	{
		tPoint = 4;
	}
	return tPoint;
}

//-----------------------------------------------
//函数功能: 小数点借位处理，判断输入数据在LCD上最多允许的小数点位数
//
//参数:		InData[in]	输入数据,请不要输入带符号数
//
//返回值:  	小数点位数
//		
//备注:		小数点最多借到1位
//			返回0表示不需要考虑小数点处理
//-----------------------------------------------
BYTE DealBorrowPoint(QWORD InData)
{
	BYTE i;
	for(i = 0;i < 4;i++)
	{
		//其他情况根据界限值判断
		if(InData > DigTubNumRange[i])
		{
			return i;
		}
		else
		{}
	}
	return 0xFF;
}

//-----------------------------------------------
//函数功能: 判断当前电压打开背光
//
//参数: 	无
//                    
//返回值:	TRUE 可以刷新背光持续时间 FALSE 不刷新背光持续时间
//
//备注:   
//-----------------------------------------------
static BOOL Drv_OpenBKLight(void)
{
	TTwoByteUnion DataUA;
	DWORD LimitVoltage = 0;
	
	api_GetRemoteData(PHASE_A + REMOTE_U, DATA_HEX, 1, 2, DataUA.b);	

	LimitVoltage = (DWORD)wStandardVoltageConst * 75 / 100; 
	
	if(    (LcdCtrl.Mode == eLcdPowerOnMode)
		|| (DataUA.w >= LimitVoltage) )
	{
		if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
		{
			OPEN_BACKLIGHT;
			return TRUE;
		}
	}
	return FALSE;	
}

//-----------------------------------------------
//函数功能: 驱动管脚关闭背光
//
//参数: 	无
//                    
//返回值:	
//
//备注:   
//-----------------------------------------------
static void Drv_CloseBKLight(void)
{
	CLOSE_BACKLIGHT;
}

//-----------------------------------------------
//函数功能: 设置背光点亮时间
//
//参数: 	Sec[in]      点亮时间 单位 秒
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void SetLcdBackLightTimer(BYTE Sec)
{
	if( LcdDispTimer.BackLight < Sec )
	{
		LcdDispTimer.BackLight = Sec;
	}
}

//-----------------------------------------------
//函数功能: 液晶全显
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void LcdDispAll(void)
{
	memset(lcd, 0xFF, LCD_BUF_LENGTH);
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 清除全部液晶显示
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void LcdClearAll(void)
{
	memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 重置借位功能
//
//参数:		Type		0：重置后从第一屏开始   
//						1：重置电能小数点数
//
//返回值:	
//		   	TRUE/FALSE
//备注:返回两个OAD，若只有一个，则第二个OAD置为ff，后面为OadNum及屏号
//-----------------------------------------------
void api_ResetBorrDotNum(BYTE type)
{
	if(type < 2)
	{
		//更改显示项、显示项数
		if(type == 0)
		{
			//下一秒循显第一屏
			LcdCtrl.Screen = FPara1->LCDPara.DispItemNum[0];
			LcdCtrl.Timer = 1;
		}
		//更改电能小数点数从当前屏显示
		//重置借位功能
		if(FPara1->LCDPara.EnergyFloat > 4)
		{
			BorrDotNum = LCDParaDef.EnergyFloat;
		}
		else
		{
			BorrDotNum = FPara1->LCDPara.EnergyFloat;
		}
	}
}


//-----------------------------------------------
//函数功能: 设置显示项目
//
//参数:		Type[in]		0--循显   1--键显
//			ItemOrder[in]   显示项目序号
//			OadNum        	OAD的个数(1 or 2)
//			*OADBuf		    OAD数据指针(字节顺序 高在前，低在后)
//			SubIndex		分屏号  1--第一屏  2--第二屏					
//返回值:	
//		   	TRUE/FALSE
//备注:返回两个OAD，若只有一个，则第二个OAD置为ff，后面为OadNum及屏号
//-----------------------------------------------
BOOL api_WriteLcdItem(BYTE Type, WORD ItemOrder, BYTE OadNum, BYTE *OADBuf, BYTE SubIndex)
{
	BYTE Buf[10];
	DWORD ItemAddr;	

	CLEAR_WATCH_DOG;//清看门狗	
	
	//参数的安全检查，出现错误直接返回 0
	if(Type >= 2)
	{
		return FALSE;
	}
	if( (Type==0) && (ItemOrder>=MAX_LCD_LOOP_ITEM) )	   //设置循显超过限制
	{
		return FALSE;
	}	
	if( (Type==1) && (ItemOrder>=MAX_LCD_KEY_ITEM) )	   //设置键显超过限制
	{
		return FALSE;
	}
	if( (OadNum!=1) && (OadNum!=2) )	
	{
		return FALSE;
	}
	api_ResetBorrDotNum(0);
		
	//规约传过来按照大端模式传输
	lib_ExchangeData(Buf, OADBuf, 4);
	lib_ExchangeData(Buf+4, OADBuf+4, 4);
	if(OadNum == 1)
	{		
		memset(Buf+4, 0xff, 4);
	}
	Buf[8] = OadNum;
	Buf[9] = SubIndex;

	if(Type == 0)	   //设置循显项目
	{
		ItemAddr = GET_CONTINUE_ADDR(LcdConRom.DisplayItem.LoopItem[0][0]);
		ItemAddr = ItemAddr + ItemOrder*10;
	}
	else		//设置键显项目
	{
		ItemAddr = GET_CONTINUE_ADDR(LcdConRom.DisplayItem.KeyItem[0][0]);
		ItemAddr = ItemAddr + ItemOrder*10;
	}
	
	api_WriteToContinueEEPRom(ItemAddr,10,Buf );
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取显示项目
//
//参数:		Type[in]		0--循显   1--键显
//			ItemOrder[in]   显示项目序号
//			*OADBuf		    OAD数据指针(字节顺序 高在前，低在后)					
//返回值:	
//		   	TRUE/FALSE
//备注:返回两个OAD，若只有一个，则第二个OAD置为ff，后面为OadNum及屏号
//-----------------------------------------------
BOOL api_ReadLcdItem( BYTE Type, WORD ItemOrder, BYTE *OADBuf )
{
	BYTE Buf[10];
	DWORD ItemAddr;	

	CLEAR_WATCH_DOG;//清看门狗	

	if(Type >= 2)
	{
		return FALSE;
	}	
	if( (Type == 0) && (ItemOrder >= MAX_LCD_LOOP_ITEM) )	   //设置循显超过限制
	{
		return FALSE;
	}	
	if( (Type == 1) && (ItemOrder >= MAX_LCD_KEY_ITEM) )	   //设置键显超过限制
	{
		return FALSE;
	}
	
	if(Type == 0)	   //设置循显项目
	{
		ItemAddr = GET_CONTINUE_ADDR(LcdConRom.DisplayItem.LoopItem[0][0]);
		ItemAddr = ItemAddr + ItemOrder*10;
	}
	else		//设置键显项目
	{
		ItemAddr = GET_CONTINUE_ADDR(LcdConRom.DisplayItem.KeyItem[0][0]);
		ItemAddr = ItemAddr + ItemOrder*10;
	}

	api_ReadFromContinueEEPRom(ItemAddr,10,Buf );
	
	lib_ExchangeData(OADBuf,Buf,4);
	lib_ExchangeData(OADBuf+4,Buf+4,4);
	memcpy(OADBuf+8,Buf+8,2);
	
	//OadNum
	if(Buf[8] == 1)
	{
		memset(OADBuf+4,0xff,4);
	}
	else if(Buf[8] == 2)
	{	
	}
	else 
	{
		return FALSE;
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 置位通讯标志
//
//参数: Type[in]	eRS485_I       	485通道
//					eIR				红外通道
//					eCR				载波通道
//					eRS485_II		第二路485通道                   
//返回值:	无
//			
//备注:   
//-----------------------------------------------
void api_SetLcdCommunicationFlag(BYTE Type)
{
	WORD IRBackLightTimer ; 
 	switch( Type )
	{
		case eRS485_I:
		  	LcdDispTimer.CommFlag[eRS485_I] = 3;
			SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommu485);
			break;
		case eIR:
			// LcdDispTimer.CommFlag[eIR] = 3;
			// SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuRf);
			// if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE ) 
			// {
			// 	IRBackLightTimer = ((FPara1->LCDPara.LoopDisplayTime)*(FPara1->LCDPara.DispItemNum[0])*2);
			// 	if(IRBackLightTimer > 255)
			// 	{
			// 		IRBackLightTimer = 255;
			// 	}
			// 	SetLcdBackLightTimer( IRBackLightTimer );
			// }
			break;
		case eCR:
		  	LcdDispTimer.CommFlag[eCR] = 3;
			SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuCr);
			break;

		default:
			break;
	} 
}

//-----------------------------------------------
//函数功能: 处理通讯标志延时计数器
//
//参数: 	无
//                    
//返回值:	无
//
//备注:   秒任务调用
//-----------------------------------------------
static void ProcCommunicationFlag(void)
{
	BYTE i;
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( LcdDispTimer.CommFlag[i] != 0 )
		{
			LcdDispTimer.CommFlag[i]--;
			if( LcdDispTimer.CommFlag[i] == 0 )
			{
				ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCommu485+i);
			}
		}
	}

}

//-----------------------------------------------
//函数功能: 显示各个通信口的状态
//
//参数: 	无
//                    
//返回值:	无
//
//备注:
//-----------------------------------------------
static void DisplayCommStatus(void)
{
	//清除通信标志
	LCD_STATUS_COMMU_485_RF_CR_CLR;
	
	//载波
	if(	GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuCr) == TRUE )
	{
		LCD_STATUS_COMMU_CR;
	}

    //第一路485
	if( GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommu485) == TRUE )
	{
		LCD_STATUS_COMMU_485;
	}

	//第二路485
	if( GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommu2_485) == TRUE )
	{
		LCD_STATUS_COMMU2_485;
	}

	//红外
	if( GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuRf) == TRUE )
	{
		LCD_STATUS_COMMU_RF;
	}

}

//-----------------------------------------------
//函数功能: 小数点显示
//
//参数: 	Point[in]
//				1～7：	小数点
//				10:		nnXX:XX:XX，时间，nn为空格；YY:MM:DD，需量时间
//				11:		nnXX.XX.XX，日期，n为空格，W为星期
//				12:		YY.MM.DD:NN，公共假日
//				13:		MM.DD:NN，时区表
//						hh.mm:NN，时段表
//				14:		HH:mm.NN            
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void DisplayPoint( BYTE Point )
{
	switch(Point)
	{
	case 1:
		SET_POINT_1;
		break;
	case 2:
		SET_POINT_2;
		break;
	case 3:
		SET_POINT_3;
		break;
	case 4:
		SET_POINT_4;
		break;
	case 5:
		SET_POINT_5;
		break;
	case 6:
		SET_POINT_6;
		break;
	case 7:
		SET_POINT_7;
		break;
	case 10:
		SHOW_DOT_TIME;
		break;
	case 11:
		SHOW_DOT_DATE;
		break;
	case 12:
		SHOW_DOT_HOLIDAY_TIME;
		break;
	case 13:
		SHOW_DOT_AREA_ZONE;
		break;
	case 14:
		 SHOW_DOT_CUR_TIME_RATIO;
		break;
	default:
		break;
	}
}

//-----------------------------------------------
//函数功能: 显示单位
//
//参数: 	Unit[in]
//				0:  eLcdUnit_KWH
//				1:  eLcdUnit_KVARH
//				2:  eLcdUnit_KW
//				3:  eLcdUnit_KVAR
//				4:  eLcdUnit_VA
//				5:  eLcdUnit_V
//				6:  eLcdUnit_A
//				7:  eLcdUnit_KVAH
//				8:  eLcdUnit_KVA
//				9:  eLcdUnit_AH
//				10: eLcdUnit_WAN
//				11: eLcdUnit_YUAN
//				12: eLcdUnit_WANYUAN
//				13：eLcdUnit_INVALID
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayUnit( BYTE Unit )
{
	if( Unit == eLcdUnit_INVALID )
	{
		return;
	}

	switch(Unit)
	{
	case eLcdUnit_KWH:
		SHOW_UNIT_KWH;
		break;
	case eLcdUnit_KW:
		SHOW_UNIT_KW;
		break;
	case eLcdUnit_V:
		SHOW_UNIT_V;
		break;
	case eLcdUnit_A:
		SHOW_UNIT_A;
		break;
	case eLcdUnit_YUAN:
		SHOW_UNIT_YUAN;
		break;
	default:
		break;
	}
}

//-----------------------------------------------
//函数功能: 显示费率
//
//参数: Type[in]  	0－当前运行费率显示
//            		1－数据显示费率显示
//		Ratio[in] 	费率号(0~4)
//                    
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayRatio( BYTE Type, BYTE Ratio )
{
	if(Ratio > MAX_RATIO)
	{
		return;
	}
	if( Type == 0 )
	{
		//清除当前运行费率指示
		LCD_STATUS_RATIO_CLR;
		//显示下方“T”
		LCD_CURR_RATIO_T;
		if(Ratio < 10)
		{
			//在下方“T ”数码管上显示费率
			DigTubCtrlShow(Ratio,11);
		}
		else
		{
			//显示数码管前面的“1”
			LCD_CURR_RATIO_1;
			Ratio = Ratio - 10;			
			DigTubCtrlShow(Ratio,11);
		}
	}
	else
	{
		//清除数据显示费率显示
		LCD_DATA_RATIO_CLR;
		if(Ratio == 0)
		{
			SHOW_CHINESE_ZONG;		//总
		}
		else
		{
			//上方费率数码管，顺便把“费率”点亮
			LCD_DATA_RATIO_T;
			if(Ratio < 10)
			{
				//在上方“费率 ”数码管显示费率
				DigTubCtrlShow(Ratio,1);
			}
			else
			{
				//显示数码管前面的“1”
				LCD_DATA_RATIO_1;
				Ratio = Ratio - 10;
				//在上方“费率 ”数码管显示费率
				DigTubCtrlShow(Ratio,1);
			}
		}
	}
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 显示当前阶梯电价，当前使用阶梯电价套显示
//
//参数:      无
//
//返回值:
//
//备注:每次刷新显示时调用此函数
//-----------------------------------------------
static void DispLadder(void)
{
	BYTE TempCurLad;
	TempCurLad = api_GetLocalInfo( eInfoCurLadder );
	//阶梯电价数从1开始所以是小于等于
	if((TempCurLad != 0) && (TempCurLad <= (MAX_LADDER+1)))
	{
		//显示下方“L”
		LCD_CURR_LADDOR_L; 
		//在下方“L ”数码管显示当前阶梯电价
		DigTubCtrlShow(TempCurLad,10);
	}
	else
	{
		LCD_CURR_LADDOR_L_CLR;
		CLRDigTub(10);
	}
	
	switch(api_GetLocalInfo( eInfoBackupLadderFlag ))
    {
       case 0: 
           LCD_STATUS_GRADE_TABLE_1;
           break;
       case 1: 
           LCD_STATUS_GRADE_TABLE_1;
           LCD_STATUS_GRADE_TABLE_2;
           break;
       default:
           LCD_STATUS_GRADE_TABLE_CLR;
           break;
    }
}
//-----------------------------------------------
//函数功能: 查询透支状态
//
//参数:      无
//
//返回值:
//		  0x5AA5--闪烁（过零但是没有到透支门限或到了透支门限处于保电状态）
//        0xA55A--常显（已经透支到透支门限）
//        0xFFFF--不显示
//
//备注:每次刷新显示时调用此函数
//-----------------------------------------------
static WORD InquiryOverdraftStatus(void)
{
	eLOCAL_STATUS Local_Status;
	BYTE Result;

	Local_Status = api_GetLocalStatus();

	//数据非法 不显示透支
	if(Local_Status > eOvrTick)
	{
		return 0xFFFF;//不显示
	}

    if( api_GetSysStatus( eSYS_STATUS_PRE_USE_MONEY ) == TRUE ) 
    {
        if( api_GetRelayStatus(2) == TRUE ) //保电
        {
            return 0x5AA5;//闪烁
        }

        if( Local_Status == eOvrTick ) //过透支门限
        {
            return 0xA55A;//常显
        }
        else
        {
            return 0x5AA5;//闪烁
        }
    }

    return 0xFFFF;//不显示
}
#endif

//-----------------------------------------------
//函数功能: 显示状态子程序
//
//参数: 
// 			无                 
//返回值:	
//			无
//备注:每次刷新显示时调用此函数   
//-----------------------------------------------
static void DisplayStatus(void)
{
	BYTE Flag;
	
	//低功耗下显示密钥状态即红外通信标志
	//密钥状态显示
	#if( PREPAY_MODE != PREPAY_NO )
	if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
	{
		LCD_STATUS_PUBLIC_KEY;
	}
	else
	{
		LCD_STATUS_PUBLIC_KEY_CLR;
	}

	if( (api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS)==TRUE) //698密码闭锁
	|| (api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS)==TRUE)//645-02级密码闭锁
	|| (api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS)==TRUE)//645-04级密码闭锁
	|| (api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG)==TRUE) )//MAC错误
	{
		LCD_STATUS_SYS_LOCK;
	}
	else
	{
		LCD_STATUS_SYS_LOCK_CLR;
	}
	#else
    LCD_STATUS_PUBLIC_KEY;
	#endif


	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )		    //电压正常
	{
		return;
	}	

	//厂内模式或红外认证通过
	if((api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE) 
	|| (api_GetSysStatus(eSYS_IR_ALLOW_PRG)==TRUE) 
	|| (api_GetSysStatus(eSYS_STATUS_LOW_INSIDE_FACTORY)==TRUE) )
	{
	    if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE )//厂内模式进行闪烁
	    {
    		if( LcdFlashFlagSec == TRUE )
    		{
    			LCD_STATUS_ALLOC_PRG;
    		}
    		else
    		{
    			LCD_STATUS_ALLOC_PRG_CLR;
    		}	
	    }
	    else if( api_GetSysStatus(eSYS_STATUS_LOW_INSIDE_FACTORY)==TRUE )//低级厂内模式进行闪烁
	    {
            if( LcdFlashFlagHalfSec == TRUE )
    		{
    			LCD_STATUS_ALLOC_PRG;
    		}
    		else
    		{
    			LCD_STATUS_ALLOC_PRG_CLR;
    		}	
	    }
	    else
	    {
            LCD_STATUS_ALLOC_PRG;
	    }
	}
	else
	{
		LCD_STATUS_ALLOC_PRG_CLR;
	}
	
	//功率反向
	if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A) == TRUE)
	{
		LCD_STATUS_OP_POWER;
	}
	else
	{
		LCD_STATUS_OP_POWER_CLR;
	}

	//判时钟电池
	if(api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE)	
	{
		LCD_STATUS_LOW_BATT;
	}
	else
	{
		LCD_STATUS_LOW_BATT_CLR;
	}
	
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//囤积标志
	if(	GetLcdStatusFlag(eLcdStatusSegFlag,eLcdHoard) == TRUE )
	{
		LCD_STATUS_HOARD;
	}
	else
	{
		LCD_STATUS_HOARD_CLR;
	}
	#if(PREPAY_MODE == PREPAY_LOCAL)
	//读卡
	if( GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess) == TRUE )
	{
		LCD_CPUCARD_READ_OK;
	}
	else if( GetLcdStatusFlag(eLcdStatusSegFlag,eLcdCardFailure) == TRUE )
	{
        LCD_CPUCARD_READ_BAD;
	}
	else
	{
        LCD_CPUCARD_CLR;
	}

	#endif
	#else

	#endif	
	
	//请购电标志
	if( api_GetRelayStatus(3) == TRUE )	
	{
		SetLcdBackLightTimer(2);
		if( LcdFlashFlagSec == TRUE )
		{
			LCD_STATUS_BUY_POWER;
		}
		else
		{
			LCD_STATUS_BUY_POWER_CLR;
		}
	}
	else
	{
		LCD_STATUS_BUY_POWER_CLR;
	}
	
	if(RelayTypeConst != RELAY_NO)
	{
		//判断液晶是否显示 "拉闸"
		Flag = api_GetRelayStatus( 0 );
		if(Flag == 1) //命令跳闸
		{
			LCD_STATUS_CUT;     // 显示系统跳闸标志
		}
		else if(Flag == 2)    //命令跳闸延时或者大电流延时
		{
			SetLcdBackLightTimer( 2 );
			if(LcdFlashFlagSec == TRUE)
			{
				LCD_STATUS_CUT;
			}
			else
			{
				LCD_STATUS_CUT_CLR;
			}
		}
		else if(Flag == 0)    //命令合闸
		{
			LCD_STATUS_CUT_CLR; // 清除显示系统跳闸标志
		}

		//跳闸灯，继电器实际状态
		Flag = api_GetRelayStatus( 1 );
		if(Flag == 0) //合闸状态
		{
			RELAY_LED_OFF;
		}
		else if(Flag == 1)    //跳闸状态
		{
			RELAY_LED_ON;
		}
		else if(Flag == 2)    //合闸允许状态
		{
			if(RelayTypeConst == RELAY_INSIDE)    //内置继电器
			{
				if(LcdFlashFlagSec == TRUE)
				{
					RELAY_LED_ON;
				}
				else
				{
					RELAY_LED_OFF;
				}
				LCD_STATUS_CUT_CLR;
			}
			else    //外置继电器
			{
				RELAY_LED_OFF;
				LCD_STATUS_CUT_CLR;
			}
		}
	}

	//判断通讯状态
	DisplayCommStatus();

	//显示当前运行费率
	DisplayRatio( 0, api_GetCurRatio() );

    #if( PREPAY_MODE == PREPAY_LOCAL )
    //当前运行的阶梯，当前阶梯电价套显示
    DispLadder();
    #endif

	//显示当前时段
	LCD_STATUS_SEG_CLR;
	LCD_STATUS_SEG_1;
	if( FPara1->LCDPara.Meaning12 == 0 )//显示时段
	{
    	if( api_GetRunHardFlag(eRUN_HARD_SWITCH_FLAG2) != FALSE )
    	{
    		LCD_STATUS_SEG_2;
    	}
	}
	else//显示费率
	{
        if( api_GetRunHardFlag(eRUN_HARD_SWITCH_FLAG3) != FALSE )
        {
            LCD_STATUS_SEG_2;
        }
	}

}

//-----------------------------------------------
//函数功能: 显示液晶上方的数字1 ~ 12
//
//参数: 		dig	上方“上 月”0号数码管显示数字
//                    
//返回值:		无
//
//-----------------------------------------------
static void Decode_SubLine(BYTE dig)
{
	if (dig <= 19)
	{
		if (dig >= 10)
		{
			dig -= 10;
			LCD_FORMER_MONTH_1;		//显示左边的“1”
		}
		DigTubCtrlShow(dig,0);
	}
	else
	{//大于19不显示，如果协议要求显示则另行更改
	}
}

//-----------------------------------------------
//函数功能: 显示上N月或上N次
//
//参数: 
//			Type[in]：    0--上月，1--上次
//			Order[in]：   次数                   
//返回值:
//			无
//备注:
//-----------------------------------------------
static void ShowLcdHistory( BYTE Type, BYTE Order )
{
	if( Order == 0 )
	{
		SHOW_CHINESE_DANGQIAN;
	}
	else
	{
		if( Order < 20 )
		{
			if( Type == 0 )
			{
				SHOW_CHINESE_SHANGYUE;
			}
			else
			{
				SHOW_CHINESE_SHANGCI;
			}
			Decode_SubLine(Order);     //显示上月的数字
		}
	}
}

#if( PREPAY_MODE == PREPAY_LOCAL )
//-----------------------------------------------
//函数功能: 数字前显示负号
//
//参数: 	 Data[in]:        要显示的数字 
//		 PointNo[in]: 	  小数点位数
//       DataLength[in]:  要显示的数字长度
//						     
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void ShowNegative( DWORD Data, BYTE PointNo, BYTE DataLength )
{
    BYTE i, j, k;
    TFourByteUnion DataBuf;
    
    DataBuf.d = Data;
	
    //数字
    //是否甩掉高位零的标志
    k = 0;
    
    for(i=0; i<DataLength; i++)
    {
        // 取最高一位数字
        j = (DataBuf.b[ (DataLength/2)-1-(i/2) ] >> 4);
        
		if( (i==(DataLength-1-PointNo)) && (k==0) )
		{
			k = 1;
		}
	    // 需要甩掉高位0
    	if( k == 0 )
    	{
    		// 高位为0
    		if( j == 0 )
    		{
                DataBuf.b[ (DataLength/2)-1-(i/2) ] <<= 4;
    			continue;
    		}
    		else
    		{
    			k = 1;
    		}
    	}
    	if( k == 1 )
    	{
    	    if( i == 0 )
			{
                SHOW_DATA_NEGATIVE;
                break;
			}
			DigTubCtrlShow(20,(9-DataLength+i));
            break;
    	}
    }
}
#endif

//-----------------------------------------------
//函数功能: 显示液晶中部数字的子程序
//
//参数:
//  	DataLength -- 要显示的数字长度
//  	PointNo -- 小数点位数，最高位为0表示BCD码，否则为16进制数
//  	Type -- 0：显示为十进制数，1：显示为十六进制数
// 		Mode -- 0：高位0不显示，1：全显
//
//备注: 此函数如果显示 HEX 编码，必须设置成为全显模式(Type=1,Mode=1),
//-----------------------------------------------
static void ShowData(BYTE PointNo, BYTE DataLength, BYTE Type, BYTE Mode)
{
    BYTE i, j, k;
    BYTE m = 0xff;

    //如果为16进制数，则转化
    if( (PointNo & 0x80) )
    {
        LcdDData.d = lib_DWBinToBCD(LcdDData.d);
    }
    
    PointNo &= 0x7f;

    //数字
    //是否甩掉高位零的标志，为“1”，显示
    k = 0;
    
    if( (DataLength == 0) || (DataLength == 2) || (DataLength == 4)
     || (DataLength == 6)|| (DataLength == 8) )//数据长度必须为0、2、4、6、8
    {
        for(i=0; i<DataLength; i++)
        {
            // 取最高一位数字
            j = (LcdDData.b[(DataLength / 2) - 1 - (i / 2)] >> 4);
        	LcdDData.b[(DataLength / 2) - 1 - (i / 2)] <<= 4;
			
            if( Mode == 1 )
            {
                k = 1;
            }
            else
            {
                if( (i == (DataLength - 1 - PointNo) ) && (k == 0) )
                {
                    k = 1;
                    m = i-1;
                }
            }

            // 需要甩掉高位0
            if( k == 0 )
            {
                // 高位为0
                if( j == 0 )
                {
                    j = 0x0f;
                }
                else
                {
                    k = 1;
                    m = i-1;
                }
            }

            if( Type == 0 )
            {
	        #if(LCD_DRIVE_CHIP == LCD_CHIP_NO)            //单相表液晶
	        	if(j == 15)
	        	{	//把所有符号集合到一个数组
	        		j = 16;		//符号“ ”
	        	}  
	        	DigTubCtrlShow(j,(10 - DataLength + i));
	        #endif
            }
            else
            {
        	#if(LCD_DRIVE_CHIP == LCD_CHIP_NO)            //单相表液晶
	        	      
	        	DigTubCtrlShow(j,(10 - DataLength + i));
        	#endif
            }
        }

        // 显示小数点
        DisplayPoint( PointNo );
    }
    else
    {
        ASSERT(0, 0);
    }
    
}

//-----------------------------------------------
//函数功能: 获取记录或者冻结里面的数据
//
//参数: 
//			Len[in]--显示版本信息编号
//			Buf[out]--输出缓冲
//返回值:	无
//			
//备注:当现实两个OAD情况时，读取附OAD的方法 
//-----------------------------------------------
static WORD GetDispRecordData(BYTE Len,BYTE *Buf)
{
	TDLT698RecordPara  DLT698RecordPara;
	TFourByteUnion tOAD;
	WORD ReturnLen;
	
	//接口类24 属性索引6、7、8、9代表总\A\B\C相
	if( ( ((LcdItemOAD.MainOAD.w[1]>=0x3000)&&(LcdItemOAD.MainOAD.w[1]<=0x3008))
		|| (LcdItemOAD.MainOAD.w[1]==0x300b)|| (LcdItemOAD.MainOAD.w[1]==0x303B) )
		&&	((LcdItemOAD.MainOAD.b[1]>=6)&&(LcdItemOAD.MainOAD.b[1]<=9)) )
	{
		DLT698RecordPara.Phase = (ePHASE_TYPE)(LcdItemOAD.MainOAD.b[1] - 6);
	}
	else
	{
		DLT698RecordPara.Phase = ePHASE_ALL;
	}
	//规约接口函数OI按照大端模式传输
	DLT698RecordPara.OI = LcdItemOAD.MainOAD.w[1];
	DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;
	DLT698RecordPara.TimeOrLast = LcdItemOAD.MainOAD.b[0];
	DLT698RecordPara.OADNum = 0x01;
	lib_ExchangeData(tOAD.b, LcdItemOAD.SubOAD.b, 4);
	//规约接口函数OAD按照大端模式传输
	DLT698RecordPara.pOAD = tOAD.b;
	ReturnLen = api_ReadProRecordData( 0x02, &DLT698RecordPara, Len , Buf);
	
	return ReturnLen;
}

//-----------------------------------------------
//函数功能: 显示电能数据
//
//参数: 	无
//		                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayEnergyData( void )
{
	BYTE UnitIndex,tPoint,tPoint1,tbTemp;
	WORD ReturnLen,tw;	
	TFourByteUnion tOAD;
	TEightByteUnion TempSavData;
	
	UnitIndex = eLcdUnit_INVALID;
	TempSavData.ll = 0x00;
	tPoint1 = 0x00;
	LcdDData.d = 0x00;

	if(LcdItemOAD.OADNum == 0x02)
	{
		tw = LcdItemOAD.SubOAD.w[1];
	}
	else
	{
		tw = LcdItemOAD.MainOAD.w[1];
	}
	//无功显示 "组合"
	if( (tw>=0x0030) && (tw<=0x0043) )
	{
		SHOW_CHINESE_ZUHE;
	}
	
	//显示"正向"
	if( 	((tw>=0x0010)&&(tw<=0x0013)) 
		|| 	((tw>=0x0090)&&(tw<=0x0093))
		|| 	((tw>=0x0110)&&(tw<=0x0113)) 
		|| 	((tw>=0x0210)&&(tw<=0x0213))  )
	{
		SHOW_CHINESE_ZHENGXIANG;	
	}

	//显示"反向"
	if( 	((tw>=0x0020)&&(tw<=0x0023)) 
		|| 	((tw>=0x00A0)&&(tw<=0x00A3))
		|| 	((tw>=0x0120)&&(tw<=0x0123)) 
		|| 	((tw>=0x0220)&&(tw<=0x0223))  )
	{
		SHOW_CHINESE_FANXIANG;
	}

	UnitIndex = eLcdUnit_KWH;

	//显示费率 : 尖峰平谷
	if(LcdItemOAD.OADNum == 0x02)
	{
		tbTemp = LcdItemOAD.SubOAD.b[0];
	}
	else
	{
		tbTemp = LcdItemOAD.MainOAD.b[0];
	}
	if( (tbTemp>=0x01) && (tbTemp<=(MAX_RATIO+1)) )
	{
		DisplayRatio( 1, tbTemp-1);		//尖峰平谷
	}
	
	//显示电量
	SHOW_CHINESE_DIANLIANG;
		
	//借位小数点数不能超过限值
	if(BorrDotNum > 4)
	{
		if(FPara1->LCDPara.EnergyFloat > 4)
		{
			//出错取默认值，等待每分钟从EEprom恢复
			BorrDotNum = LCDParaDef.EnergyFloat;
		}
		else
		{
			BorrDotNum = FPara1->LCDPara.EnergyFloat;
		}
	}
	//发生借位后所有电能数据都借位，保持相同小数点数
	tPoint = BorrDotNum;
	
	//两个OAD显示电能的情况 : 冻结电能、事件记录电能
	if(LcdItemOAD.OADNum == 0x02)
	{	
		if( (LcdItemOAD.MainOAD.w[1]==0x5005) || (LcdItemOAD.MainOAD.w[1] == 0x5006))//月冻结与结算冻结显示月
	    {
	        SHOW_CHINESE_YUE;
	    }
		//获取数据，获取的普通电能小数点2位，高精度小数点4位
		ReturnLen = GetDispRecordData(8 , TempSavData.b);
	}
	else	//当前电能
	{
		SHOW_CHINESE_DANGQIAN;
		
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);		
		ReturnLen = api_GetProOadData( eGetNormalData, eData, 0xFF, tOAD.b, 0X00, 8, TempSavData.b );
	}
	//读取正常
	if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
	{
		//两个OAD显示电能的情况 : 冻结电能、事件记录电能
		if(LcdItemOAD.OADNum == 0x02)
		{	
			HandleEnergyData(LcdItemOAD.SubOAD, TempSavData.b);
		}
		else//当前电能
		{
			HandleEnergyData(LcdItemOAD.MainOAD, TempSavData.b);
		}
		//参数小数位0没必要借位
		//协议要求启用借位时所有电能显示全部借位相同位数
		if(tPoint != 0)
		{	
			//DealBorrowPoint入参必须是整数部分
			tPoint1 = DealBorrowPoint(TempSavData.ll / lib_MyPow10(tPoint));
			//仅需要借位时，取已浏览电能显示屏最小电能小数点数
			if((tPoint1 != 0xFF) && (tPoint > tPoint1)) 	//需要借位处理
			{
				TempSavData.ll = TempSavData.ll / lib_MyPow10(tPoint - tPoint1);
				tPoint = tPoint1;
				BorrDotNum = tPoint;
			}
			else
			{}
		}
		else
		{}
		TempSavData.ll = lib_QWBinToBCD(TempSavData.ll);		
		memcpy(LcdDData.b,TempSavData.b,4);
	}
	else
	{
		LcdDData.d = 0x00;
	}

	ShowData( tPoint, 8, 0, 0 );
	DisplayUnit( UnitIndex );
}

//-----------------------------------------------
//函数功能: 显示变量数据
//
//参数: 	无
//		                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayVariableData( void )
{
	BYTE UnitIndex, tPoint, tLength, tMode, DataType, tbPhase;
	WORD tw,tw0,ReturnLen;
	TFourByteUnion tOAD;
	BYTE Buf[20];
	BYTE SignIndex,SignFlag;
	
	UnitIndex = eLcdUnit_INVALID;
	tPoint = 0;
	tLength = 8;	
	tMode = 0;
	DataType = 0;	//0 正常数据  1 日期  2 时间 3需要显示负号
	//变量数据符号显示不同，新增符号索引和符号标志
	//SignIndex为1显示反向，为2显示负号，SignFlag为1表示负数，需要显示对应符号
	SignIndex = 0;
	SignFlag = 0;
	
	if(LcdItemOAD.OADNum == 0x02)
	{
		tw = LcdItemOAD.SubOAD.w[1];
		tw0 = LcdItemOAD.MainOAD.w[0];
		tbPhase = LcdItemOAD.SubOAD.b[0]; 
	}
	else
	{
		tw = LcdItemOAD.MainOAD.w[1];
		tw0 = LcdItemOAD.MainOAD.w[0]; 
		tbPhase = LcdItemOAD.MainOAD.b[0]; 
	}

	switch(tw)
	{
	case 0x2000:		//电压
		tPoint = 1;
		tLength = 4;
		UnitIndex = eLcdUnit_V;
		break;

	case 0x2001:		//电流
		tPoint = 3;
		tLength = 8;
		UnitIndex = eLcdUnit_A;
	    DataType = 3;
		SignIndex = 2;		//电流为负显示负号
		if(tw0 == 0x0201)
		{
			LCD_STATUS_L;	//火线
		}
		else if((tw0/0x100) == 0x04)		//零线电流
		{
			LCD_STATUS_N;	//零线
		}
		break;

	case 0x2002:		//电压相角
		tPoint = 1;
		tLength = 4;
		break;
		
	case 0x2003:		//电压电流相角
		tPoint = 1;
		tLength = 4;
		break;
		
	case 0x2004:		//有功功率
	case 0x2007:		//一分钟平均有功功率
	    DataType = 3;
		tPoint = GetDemandDot();
		tLength = 8;
		UnitIndex = eLcdUnit_KW;
		//LCD_STATUS_P;	//显示P
		SignIndex = 2;		//电流为负显示负号
		break;

	case 0x200A:		//功率因数
	    DataType = 3;
		tPoint = 3;
		tLength = 4;
		SignIndex = 2;		//电流为负显示负号
		SHOW_CHINESE_COS;
		break;

	case 0x200B:		//电压波形失真度
	case 0x200C:		//电流波形失真度
	case 0x200D:		//电压谐波含有量（总及2…n次）
	case 0x200E:		//电流谐波含有量（总及2…n次）
		//不支持
		tPoint = 0;
		tLength = 8;
		break;

	#if( SEL_MEASURE_FREQ == YES )
	case 0x200F:		//电网频率
		tPoint = 2;
		tLength = 4;
		break;
	#endif	//SEL_MEASURE_FREQ

	case 0x2010:		//表内温度
		tPoint = 1;
		tLength = 4;
		SignIndex = 2;		//温度为负显示负号
		break;

	case 0x2011:		//时钟电池电压
	case 0x2012:		//停电抄表电池电压
		tPoint = 2;
		tLength = 4;
		UnitIndex = eLcdUnit_V;	
		break;

	case 0x2013:		//时钟电池工作时间
		tPoint = 0;
		tLength = 8;
		SHOW_CHINESE_ZONG;
		SHOW_CHINESE_SHIJIAN;
		break;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	case 0x201A:	//当前电价
	case 0x201B:	//当前费率电价
		tPoint  = 4;
		tLength = 8;
		UnitIndex = eLcdUnit_YUAN;
		if(tw == 0x201B)
		{
			LCD_DATA_RATIO_T;
		}
		SHOW_CHINESE_DANGQIAN;
		SHOW_CHINESE_DIANJIA;
		break;
	case 0x201C:	//当前阶梯电价
    	tPoint  = 4;
    	tLength = 8;
    	UnitIndex = eLcdUnit_YUAN;
    	SHOW_CHINESE_DANGQIAN;
    	SHOW_CHINESE_DIANJIA;
    	SHOW_CHINESE_JIETI;
    	break;
	#endif
	case 0x201E:	//事件发生时间
	case 0x2020:	//事件结束时间
	case 0x2021:	//数据冻结时间
		if(LcdItemOAD.ScreenOrder == 0x01)		//显示日期
		{
			DataType = 1;
			tPoint = 11;
			tLength = 6;
			tMode = 1;
			SHOW_CHINESE_SHIJIAN;
		}
		else if(LcdItemOAD.ScreenOrder == 0x02)		//显示时间
		{
			DataType = 2;
			tPoint = 10;
			tLength = 6;
			tMode = 1;
			SHOW_CHINESE_SHIJIAN;
		}
		break;

	case 0x2024:	//事件发生源
		break;		//暂时不支持
	#if( PREPAY_MODE == PREPAY_LOCAL )
	case 0x202C:	
		//剩余金额显示不在这里处理
		if(LcdItemOAD.MainOAD.d == 0x202c0201)//剩余金额
		{
			tPoint  = 2;
			tLength = 8;
			UnitIndex = eLcdUnit_YUAN;
			//这里没有用到，但还是加上保持统一
			SignIndex = 2;		//剩余金额为负显示负号
			SHOW_CHINESE_SHENGYU;
			SHOW_CHINESE_JINE;
		}
		break;
	case 0x202D:	
		//透支金额显示不在这里处理
		if(LcdItemOAD.MainOAD.d == 0x202D0200)//透支金额
		{
			tPoint  = 2;
			tLength = 8;
			UnitIndex = eLcdUnit_YUAN;
			//这里没有用到，但还是加上保持统一
			SignIndex = 2;		//透支金额为负显示负号
			SHOW_CHINESE_TOUZHI;
			SHOW_CHINESE_JINE;
		}
		break;
	#endif

	case 0x2031:	//月度用电量
	case 0x2032:	//阶梯结算用电量
		if(tw ==0x2032)
		{
			SHOW_CHINESE_JIETI;	//显示阶梯
		}
		tPoint  = 2;
		tLength = 8;
		UnitIndex = eLcdUnit_KWH;
		SHOW_CHINESE_YONG;
		SHOW_CHINESE_YUE;
		SHOW_CHINESE_DIANLIANG;	//电量
		if(LcdItemOAD.OADNum == 0x01)//显示当前
		{
            SHOW_CHINESE_DANGQIAN;
            Decode_SubLine(1);     //显示上1月的数字
		}
		break;

	default:		//不支持的OAD 和 一些不需要显示汉字、单位、小数点的OAD
		tPoint = 0;
		tLength = 8;
		break;
	}
	
	memset(Buf,0x00,sizeof(Buf));

	if(LcdItemOAD.OADNum == 0x02)
	{
		//冻结数据、事件记录发生记录数据
		ReturnLen = GetDispRecordData(sizeof(Buf), Buf);
	}
	else
	{
		//当前的瞬时变量数值
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
		ReturnLen = api_GetProOadData( eGetNormalData, eData, tPoint, tOAD.b, 0X00,  sizeof(Buf), Buf );
	}

	if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
	{
		if((DataType == 0) || (DataType == 3) )	//正常数据
		{
			if( (ReturnLen>=1) && (ReturnLen<=4) )
			{
				memcpy( LcdDData.b, Buf, ReturnLen );
			}
			if(ReturnLen == 4)
			{
				if(LcdDData.l < 0)
				{
					LcdDData.l *= -1;
					SignFlag = 1;		//显示对应符号(负号或反向)
				}
			}
			else if(ReturnLen == 2)
			{
				if(LcdDData.s[0] < 0)
				{
					LcdDData.s[0] *= -1;
					SignFlag = 1;		//显示对应符号(负号或反向)
				}
			}
			else if(ReturnLen == 1)
			{
				if(LcdDData.c[0] < 0)
				{					
					LcdDData.c[0] *= -1;
					SignFlag = 1;		//显示对应符号(负号或反向)
				}
			}
			if( SignFlag == 1 )
			{
				if(SignIndex == 1)
				{
					SHOW_CHINESE_FANXIANG;		//反向
				}
				else if(SignIndex == 2)
				{
					SHOW_DATA_NEGATIVE;			//负号
				}
			}
			LcdDData.d = lib_DWBinToBCD(LcdDData.d);
		}
		else if(DataType == 1)		//日期
		{
			tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
			Buf[0] = tw /100;
			Buf[1] = tw%100;	
			memcpy(LcdDData.b, Buf+1, 3);
			LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
			LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
			LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
			LcdDData.b[3] = lib_BBinToBCD(LcdDData.b[3]);
			lib_InverseData( LcdDData.b, 3);
		}
		else if(DataType == 2)		//时间
		{
			tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
			Buf[0] = tw /100;
			Buf[1] = tw%100; 
			memcpy(LcdDData.b, Buf+4, 3);
			LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
			LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
			LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
			LcdDData.b[3] = lib_BBinToBCD(LcdDData.b[3]);
			lib_InverseData( LcdDData.b, 3);
		}
	}
	else		//无数据或错误，显示“--------”
	{
		LcdDData.d = 0x00;
	}

	ShowData( tPoint, tLength, 0, tMode );
	DisplayUnit( UnitIndex );
}

//-----------------------------------------------
//函数功能: 显示参变量数据
//
//参数: 	无
//		                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplaySysParaData( void )
{
	BYTE DataType, UnitIndex, tPoint, tLength, tType, tMode;
	WORD tw,ReturnLen;
	TFourByteUnion tOAD;
	BYTE Buf[64];
	
	LcdDData.d = 0xeeeeeeee;
	UnitIndex = eLcdUnit_INVALID;
	tLength = 8;
	tPoint = 0;
	tType = 0;	
	tMode = 0;
	DataType = 0;

	tw = LcdItemOAD.MainOAD.w[1];
	
	if(LcdItemOAD.OADNum == 1)
	{
		switch(tw)
		{
		case 0x4000:	//日期时间
			SHOW_CHINESE_DANGQIAN;
		case 0x4008:	//两套时区表切换时间
		case 0x4009:	//两套日时段切换时间
		case 0x400A:	//两套分时费率切换时间
		case 0x400B:	//两套阶梯电价切换时间
			if(LcdItemOAD.ScreenOrder == 0x01)		//显示日期
			{
				DataType = 1;
				tPoint = 11;
				tLength = 6;
				tMode = 1;
				SHOW_CHINESE_SHIJIAN;
			}
			else if(LcdItemOAD.ScreenOrder == 0x02)		//显示时间
			{
				DataType = 2;		
				tPoint = 10;
				tLength = 6;
				tMode = 1;
				SHOW_CHINESE_SHIJIAN;
			}
			break;
	
		case 0x4001:	//通信地址
		case 0x4002:	//表号
		case 0x4003:	//客户编号
		case 0x4005:	//组地址
			if( LcdItemOAD.ScreenOrder == 0x02 )
			{
				DataType = 3;
				tMode = 1;
				tType = 1;
				tPoint = 0;
				tLength = 8;
			}
			else if( LcdItemOAD.ScreenOrder	== 0x01 )
			{
				DataType = 4;
				tMode = 1;
				tType = 1;
				tPoint = 0;
				tLength = 4;
			}
			tMode = 1;
			break;
	
		case 0x4109:	//电表有功常数
			tPoint = 0;
			tLength = 8;
			SHOW_CHINESE_CHANGSHU;
			break;
			
		case 0x400C:	//时区时段数，
		case 0x400D:	//阶梯数，
		case 0x400E:	//谐波分析次数，
		case 0x400F:	//密钥总条数,
		case 0x4010:	//计量元件数, 
		case 0x4013:	//周休日釆用的日时段表号
		case 0x4102:	//校表脉冲宽度
			tLength = 2;
			tMode = 1;
			break;
	
		case 0x4030:	//电压参考(  合格) 上下限
			UnitIndex = eLcdUnit_V;
			tPoint = 1;
			tLength = 4;
			break;
		
		case 0x401C:	//电流互感器变比
		case 0x401D:	//电压互感器变比
			tPoint= 2;
			tLength = 8;
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x401E:	//报警金额限值
		case 0x401F:	//其它金额限值
            UnitIndex = eLcdUnit_YUAN;
            tLength = 8;
            tPoint = 2;
            DataType = 9;
            if( tw == 0x401E )
            {
                Decode_SubLine( LcdItemOAD.MainOAD.b[0] );
            }
			break;
		#endif
		case 0x4116:	//结算日
			DataType = 5;
			UnitIndex = eLcdUnit_INVALID;
			tPoint = 2;		//hh.mm
			tLength = 4;
			tMode = 1;
			break;
	
		case 0x4016:	//当前套日时段表
		case 0x4017:	//备用套日时段表
			DataType = 7;
			UnitIndex = eLcdUnit_INVALID;
			tPoint = 14;//HH:mm.NN
			tLength = 6;
			tMode = 1;
			break;
		case 0x4014:	//当前套时区表
		case 0x4015:	//备用套时区表
			DataType = 8;
			UnitIndex = eLcdUnit_INVALID;
			tPoint = 11;		//HH.mm.NN
			tLength = 6;
			tMode = 1;
			break;
			
		case 0x4011:	//公共节假日
			DataType = 6;
			UnitIndex = eLcdUnit_INVALID;
			tPoint = 12;		//YY,MM.DD:NN
			tLength = 8;
			tMode = 1;
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )
	    case 0x4018: //当前套费率
        case 0x4019: //备用套费率
            DataType = 0;
            UnitIndex = eLcdUnit_YUAN;
            tPoint = 4;
            tLength = 8;
            tMode = 0;
            SHOW_CHINESE_DIANJIA;
            if( (LcdItemOAD.MainOAD.b[0]>=0x01) && (LcdItemOAD.MainOAD.b[0]<=MAX_RATIO) )
            {
                    DisplayRatio( 1, LcdItemOAD.MainOAD.b[0]);		//尖峰平谷
            }
            break;

        case 0x401A: //当前套阶梯参数
        case 0x401B: //备用套阶梯参数
            if( (LcdItemOAD.ScreenOrder>=1) && (LcdItemOAD.ScreenOrder<=7) )
            {
                Decode_SubLine(LcdItemOAD.ScreenOrder);
            }
            
            if( LcdItemOAD.MainOAD.b[0] == 0x01 )   //当前套阶梯值1~6 //备用套阶梯值1~6
            {
                UnitIndex = eLcdUnit_KWH;
                tPoint = 2;
                tLength = 8;
                DataType = 9;
                SHOW_CHINESE_JIETI;
                SHOW_CHINESE_DIANLIANG;//显示“电量”
            }
            else if( LcdItemOAD.MainOAD.b[0] == 0x02 )  //当前套阶梯电价1~7 //备用套阶梯电价1~7
            {
                UnitIndex = eLcdUnit_YUAN;
                tPoint = 4;
                tLength = 8;
                DataType = 9;
                SHOW_CHINESE_DIANJIA;//显示“电价”
                SHOW_CHINESE_JIETI;
            }
            else if( LcdItemOAD.MainOAD.b[0] == 0x03 )     //当前套第1~4阶梯结算日    //备用套第1~4阶梯结算日
            {
                tPoint = 13;
                tLength = 6;
                tMode = 1;
                DataType = 9;
            }
            break;
        #endif    
		case 0x4012:	//周休日特征字
		case 0x4110:	//电表运行特征字
		case 0x4111:	//软件备案号
		case 0x4112:	//有功组合方式特征字
		case 0x4113:	//无功组合方式1特征字
		case 0x4114:	//无功组合方式2特征字
			UnitIndex = eLcdUnit_INVALID;
			tPoint = 0;		
			tLength = 8;
			tMode = 1;
			tType = 1;
			break;
	
		default:
			tPoint = 0;
			tLength = 8;
			break;
		}
		//单相表特殊汉字显示处理
		switch(tw)
		{
		case 0x4002:
			SHOW_CHINESE_BIAOHAO;
			break;
		case 0x4003:
			SHOW_CHINESE_YONG;
			SHOW_CHINESE_HUHAO;
			break;
		default:
			break;
		}
	
		memset(Buf,0x00,sizeof(Buf));
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
		ReturnLen = api_GetProOadData( eGetNormalData, eData, 0, tOAD.b, 0X00, sizeof(Buf), Buf );
	
		if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
		{
			if(DataType == 0)	//正常数据
			{
				if( (ReturnLen>=1) && (ReturnLen<=4) )
				{
					memcpy( LcdDData.b, Buf, ReturnLen );
				}
				LcdDData.d = lib_DWBinToBCD(LcdDData.d);
			}
			else if(DataType == 1)		//日期
			{
				tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
				Buf[0] = tw/100;
				Buf[1] = tw%100;	
				memcpy(LcdDData.b, Buf+1, 3);
				LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
				LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
				LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
				lib_InverseData( LcdDData.b, 3);
			}
			else if(DataType == 2)		//时间
			{
				tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
				Buf[0] = tw/100;
				Buf[1] = tw%100; 
				memcpy(LcdDData.b, Buf+4, 3);
				LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
				LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
				LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
				lib_InverseData( LcdDData.b, 3);
			}
			else if( DataType == 3 )	//地址低8位
			{
				memcpy(LcdDData.b, Buf+2, 4);	
				lib_InverseData( LcdDData.b, 4);
			}
			else if( DataType == 4 )	//地址高4位
			{
				memcpy(LcdDData.b, Buf, 2);	
				lib_InverseData( LcdDData.b, 2);
			}
			else if(DataType == 5)		//结算日、时区表
			{
				if( (ReturnLen>=1) && (ReturnLen<=4) )
				{
					memcpy( LcdDData.b, Buf, ReturnLen );
					lib_InverseData( LcdDData.b, ReturnLen);
					LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
					LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
				}			
			}
			else if(DataType == 6)		//公共节假日
			{				
				tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
				Buf[0] = tw/100;
				Buf[1] = tw%100;	
				memcpy(LcdDData.b, Buf+1, 3);
				LcdDData.b[3] = Buf[5];		//舍弃星期
				lib_InverseData( LcdDData.b, 4);
				LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
				LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
				LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
				LcdDData.b[3] = lib_BBinToBCD(LcdDData.b[3]);
			}
			else if(DataType == 7)		//日时段表
			{
				if( (LcdItemOAD.ScreenOrder>=1) && (LcdItemOAD.ScreenOrder<=MAX_TIME_SEG) )
				{
					memcpy( LcdDData.b, Buf+(LcdItemOAD.ScreenOrder-1)*3, 3 );
					lib_InverseData( LcdDData.b, 3);
					LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
					LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);
					LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]);
				}
			}
			else if(DataType == 8)//时区表
			{
                if( (ReturnLen>=1) && (ReturnLen<=4) )
                {
                    memcpy( LcdDData.b, Buf, ReturnLen );
                    lib_InverseData( LcdDData.b, ReturnLen);
                    LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[0]);
                    LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[1]);   
                    LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[2]); 
                }       
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if( DataType == 9 )//阶梯参数
			{
			    if( LcdItemOAD.MainOAD.b[0] == 0x03  ) //阶梯结算日
			    {
                    memcpy( LcdDData.b, Buf+(LcdItemOAD.ScreenOrder-1)*3, 3 );
                    lib_InverseData( (BYTE *)&LcdDData.b[0], sizeof(LcdDData) );
                    LcdDData.b[0] = lib_BBinToBCD(LcdDData.b[1]);
                    LcdDData.b[1] = lib_BBinToBCD(LcdDData.b[2]);
                    LcdDData.b[2] = lib_BBinToBCD(LcdDData.b[3]);
			    }
			    else
			    {
					if( (tw == 0x401a)||(tw == 0x401b) )
					{
						memcpy( LcdDData.b, Buf + (LcdItemOAD.ScreenOrder-1) * 4, 4 );
					}
					else
					{
						memcpy( LcdDData.b, Buf + LcdItemOAD.ScreenOrder * 4, 4 );
					}
                    LcdDData.d = lib_DWBinToBCD(LcdDData.d);
			    }
			}
			#endif
			
		}
		else
		{
            LcdDData.d = 0x00;
		}
	}
	
	ShowData( tPoint, tLength, tType, tMode );
	DisplayUnit( UnitIndex );
}

//-----------------------------------------------
//函数功能: 显示事件记录数据
//
//参数: 	无
//		                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayEventRecordData( void )
{
}

//-----------------------------------------------
//函数功能: 显示事件和冻结的汉字显示
//
//参数: 
// 			Type[in]: 	0  显示事件汉字
//				  		1  显示冻结汉字
//返回值:	无
//
//备注: 使用举例:
//		显示事件汉字	失压 失流 逆相序 上N
//		显示冻结汉字	上N月
//-----------------------------------------------
static void DisplayCharForEventOrFreeze(BYTE Type)
{
	
	//事件显示相 A/B/C
	if(Type == 0)
	{
		
	}
	
	//月冻结
	if( (LcdItemOAD.MainOAD.w[1]==0x5005) || (LcdItemOAD.MainOAD.w[1] == 0x5006))
	{
		if( (LcdItemOAD.MainOAD.b[0]>=1) && (LcdItemOAD.MainOAD.b[0]<=12) )
		{
			//上XX月
			ShowLcdHistory( 0, LcdItemOAD.MainOAD.b[0] );
		}
	}
	else
	{
		//上XX次
		ShowLcdHistory( 1, LcdItemOAD.MainOAD.b[0] );
	}
}

//-----------------------------------------------
//函数功能: 显示输入输出设备信息函数
//
//参数: 
// 			无
//
//返回值:	无
//
//备注: 目前只支持电表波特率的显示
//-----------------------------------------------
static void DisplayOutputDeviceData( void )
{
	BYTE BaudRate;
	
	if( (LcdItemOAD.OADNum==1) && (LcdItemOAD.MainOAD.w[1]==0xF201) )//不判断屏序号
	{
		BaudRate = FPara2->CommPara.I485;
		switch(BaudRate&0x0F)
		{
			case 0x00:
				LcdDData.d = 0x0300;
				break;
			case 0x01:
				LcdDData.d = 0x0600;
				break;				
			case 0x02:
				LcdDData.d = 0x1200;
				break;				
			case 0x03:
				LcdDData.d = 0x2400;
				break;
			case 0x04:
				LcdDData.d = 0x4800;
				break;				
			case 0x05:
				LcdDData.d = 0x7200;
				break;
			case 0x06:
				LcdDData.d = 0x9600;
				break;
			case 0x07:
				LcdDData.d = 0x019200;
				break;
			case 0x08:
				LcdDData.d = 0x038400;
				break;
			case 0x09:
				LcdDData.d = 0x057600;
				break;
			case 0x0A:
				LcdDData.d = 0x115200;
				break;	
			default:
				break;
		}
	}
	else
	{
		LcdDData.d = 0xeeeeeeee;
	}	

	ShowData( 0, 8, 0, 0 );
	DisplayUnit( 0xff );
}

//-----------------------------------------------
//函数功能: 显示不能识别OAD的数据
//
//参数: 	OADNum[in]	 1  一个OAD	2  两个OAD
// 			
//返回值:	无
//
//备注:   	通过GetRequestNormal_Type_Data()函数和Get_Record_Data()函数
//		读取OAD数据，如果能读取出来就显示，不能就显示--------
//-----------------------------------------------
static void DisplayOtherData(BYTE OADNum)
{
	WORD ReturnLen;
	TFourByteUnion tOAD;
	BYTE Buf[20];
	
	if(OADNum == 2)
	{
		ReturnLen = GetDispRecordData(sizeof(Buf), Buf);
	}
	else
	{
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
		ReturnLen = api_GetProOadData( eGetNormalData, eData, 0, tOAD.b, 0X00, sizeof(Buf), Buf );
	}

	if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
	{
		memcpy( LcdDData.b,Buf,4);
	}
	else		//无数据或错误，显示“--------”
	{
		LcdDData.d = 0xeeeeeeee;
	}
	
	ShowData( 0, 8, 0, 0 );
	DisplayUnit( 0xff );
}

//-----------------------------------------------
//函数功能: 显示错误类信息
//
//参数: 
//			Index[in]	错误信息代号
//
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
static void LcdShowErrMsg( WORD Index )
{
	if( Index < 0x100 )
	{
		LcdDData.w[0] = lib_WBinToBCD( Index );
		SHOW_Err_XX;
		ShowData( 0, 2, 0, 1 );
	}
	else
	{
		LcdDData.w[0] = lib_WBinToBCD(Index-0x100);
		LcdDData.w[0] += 0x100;

		SHOW_Err_01XX;
		ShowData( 0, 4, 0, 1 );
	}
}
//-----------------------------------------------
//函数功能: 读写无线信号强度
//
//参数:		Operation[in]	READ/WRITE
//			Buf[in]			信号强度，1字节
//返回值:	
//			TRUE/FALSE
//备注:  
//-----------------------------------------------
BOOL api_ProcLcdWirelessPara(BYTE Operation,BYTE *Buf)
{	
	return FALSE;
}


//-----------------------------------------------
//函数功能: 根据规约标识符取数据
//
//参数: 	无
//		
//
//返回值:	无	
//			
//备注: 这个函数只有在Buildscene()函数中调用
//-----------------------------------------------
static void DisplayData(void)
{
	WORD twMainOAD,twSubOAD, Result;
	TFourByteUnion Balance, Overdraft, tOAD;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TFourByteUnion TmpMoney;
	#endif
	BYTE tPoint;

	//清除全局变量的数据
	LcdDData.d = 0x00;
	
	twMainOAD = LcdItemOAD.MainOAD.w[1];
	twSubOAD = LcdItemOAD.SubOAD.w[1];
	#if( PREPAY_MODE == PREPAY_LOCAL ) 
	if( (LcdCtrl.ErrNo>0) && (LcdCtrl.ErrNo<=0x1ff) && ( (BeepReadCardFlag == 0x5AA5) || (LcdCtrl.Mode != eLcdEchoInfoMode) ) )
	{
		//显示错误号
		LcdShowErrMsg( LcdCtrl.ErrNo );
		return;
    }

    
    if( BeepReadCardFlag == 0x555A ) //用户卡显示插卡前金额
    {
        memcpy( LcdDData.b, ReadCardBeforeBalance.b, 4 );
        if( BalanceNegativeFlag == 0x01 )
        {
			SHOW_NEGATIVE;
            SHOW_CHINESE_TOUZHI;
        }
        SHOW_CHINESE_SHENGYU;
        SHOW_CHINESE_JINE;
        DisplayUnit( eLcdUnit_YUAN );
        ShowData( 2, 8, 0, 0 );
        return;
    }
    #else
    if( (LcdCtrl.ErrNo>0) && (LcdCtrl.ErrNo<=0x1ff) )
	{
		//显示错误号
		LcdShowErrMsg( LcdCtrl.ErrNo );
		return;
	}
    #endif  //#if( PREPAY_MODE == PREPAY_LOCAL )

	//电能类：		0x0000----0x0503
	//最大需量: 	0x1010----0x11A3
	//变量：		0x2000----0x2506
	//事件：		0x3000----0x3203
	//参变量：		0x4000----0x4517
	if(LcdItemOAD.OADNum == 0x01)
	{
	    if( (twMainOAD>=0x0000) && (twMainOAD<=0x0503) )			//电能类
		{
			DisplayEnergyData();
		}
		else if( (twMainOAD>=0x2000) && (twMainOAD<=0x2506) )		//变量
		{
			#if( PREPAY_MODE == PREPAY_LOCAL )
			if( LcdItemOAD.MainOAD.d == 0x202C0201 )
			{
				//显示剩余金额
	            SHOW_CHINESE_SHENGYU;
				SHOW_CHINESE_JINE;

	            tPoint = 2;
	            DisplayUnit( eLcdUnit_YUAN );

				//规约接口函数OAD按照大端模式传输
				//如果剩余金额为负，则剩余金额显示透支。
				Result = api_ReadPrePayPara(eRemainMoney,(BYTE *)TmpMoney.b);

				if( Result == 4 )
				{
					LcdDData.d = lib_DWBinToBCD( labs( TmpMoney.l ));
					if( TmpMoney.l < 0 )	//剩余金额要求大于等于0
					{
						SHOW_NEGATIVE;
	                    SHOW_CHINESE_TOUZHI;						
					}
				}
				else
	            {
	                LcdDData.d = 0xeeeeeeee;
	                tPoint = 0;
	            }
	            ShowData( tPoint, 8, 0, 0 );
			}
			else if( LcdItemOAD.MainOAD.d == 0x202D0200 )
			{
	            SHOW_CHINESE_TOUZHI;
	            SHOW_CHINESE_JINE;

	            //规约接口函数OAD按照大端模式传输
				lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
	            Result = api_GetProOadData( eGetNormalData, eData, 2 , tOAD.b, 0X00, sizeof(LcdDData), LcdDData.b );
	            if( Result != 0x8000 )
	            {
	                tPoint = 2;
	                DisplayUnit( eLcdUnit_YUAN );
					LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					if( LcdDData.d != 0 )
					{
						SHOW_NEGATIVE;
					}
	            }
	            else
	            {
	                LcdDData.d = 0xeeeeeeee;
	                tPoint = 0;
	            }
	            ShowData( tPoint, 8, 0, 0 );
	        }
			else
			#endif
			{
				DisplayVariableData();
			}
		}
		else if( (twMainOAD>=0x4000) && (twMainOAD<=0x4517) )		//参变量
		{
			DisplaySysParaData();
		}
		else if( (twMainOAD>=0xF200) && (twMainOAD<=0xF20B) )		//输入输出设备
		{
			DisplayOutputDeviceData();
		}
		else if(twMainOAD == 0xFF00)	//参照三相双键模式，显示版本号等厂内信息
		{
			DisplayVersion( LcdItemOAD.MainOAD.b[0] );
		}
		else//未识别的OAD
		{
			DisplayOtherData(1);
		}
	}
	else if(LcdItemOAD.OADNum == 0x02)
	{
		if( (twMainOAD>=0x3000) && (twMainOAD<=0x3040) )			//事件
		{
			DisplayCharForEventOrFreeze(0);
			
			if( (twSubOAD>=0x0000) && (twSubOAD<=0x0503) )		//电能类
			{
				DisplayEnergyData();
			}
			else if( (twSubOAD>=0x2000) && (twSubOAD<=0x2506) )	//变量
			{
				DisplayVariableData();
			}
			else	//未识别的OAD
			{
				DisplayOtherData(2);
			}
		}
		if( (twMainOAD>=0x5000) && (twMainOAD<=0x5011) )			//冻结
		{
			DisplayCharForEventOrFreeze(1);
			
			if( (twSubOAD>=0x0000) && (twSubOAD<=0x0503) )		//电能类
			{
				DisplayEnergyData();
			}
			else if( (twSubOAD>=0x2000) && (twSubOAD<=0x2506) )	//变量
			{
				DisplayVariableData();
			}
			else	//未识别的OAD
			{
				DisplayOtherData(2);
			}			
		}
	}
}

//-----------------------------------------------
//函数功能: 组屏
//
//参数: 	无
//		
//                    
//返回值:	无	
//
//备注:
//-----------------------------------------------
static void BuildScene(void)
{
	if( LcdCtrl.Mode != eLcdPowerOnMode )
	{
		memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
	}
	
	if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
	{
		if( LcdItemOAD.MainOAD.d == 0xffffffff )
		{
			return;
		}
		
		//遥控全显
		if((LcdItemOAD.SubOAD.d==0xFFFFFFFF) && (LcdItemOAD.MainOAD.d==0xdfdfdfdf))	
		{
			LcdItemOAD.SubOAD.d = 0xFFFFFFFF;
			LcdItemOAD.MainOAD.d = 0xFFFFFFFF;	
			LcdItemOAD.ScreenOrder = 0x00;
			LcdItemOAD.OADNum = 0x01;	
			
			LcdDispAll();
			return;
		}
		//送样时 电压影响量开启SelVolCorrectConst为YES 按键长按10s显示DF01
		//供货时 电压影响量关闭SelVolCorrectConst为NO 防止按键损坏后显示DF01
		if( SelVolCorrectConst == YES )
		{
			if((LcdItemOAD.SubOAD.d == 0xFFFFFFFF) && (LcdItemOAD.MainOAD.d == 0xdf01df01))
			{
				LcdItemOAD.SubOAD.d = 0xFFFFFFFF;
				LcdItemOAD.MainOAD.d = 0xFFFFFFFF;	
				LcdItemOAD.ScreenOrder = 0x00;
				LcdItemOAD.OADNum = 0x01;
				api_DisplayDF01();
				return;
			}
		}
	}
	
	DisplayStatus();
	
	DisplayData();
	
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 通过屏号获取显示项
//
//参数: 	Type[in]		显示模式
//			ScreenNo[in]	第几屏
//		
//返回值:	
//		
//备注:   
//-----------------------------------------------
static void GetDispCode(BYTE Type,BYTE ScreenNo)
{	
	BYTE DispOADInfo[10];
	WORD AddrOffset;
	DWORD ItemAddr;
	
	if( (LcdCtrl.ErrNo>0) && (LcdCtrl.ErrNo<=0x1ff) )
	{
		return;
	}
	
	switch(Type)
	{
		case eLcdLoopMode:
			if( (ScreenNo!=0) && (ScreenNo<=MAX_LCD_LOOP_ITEM) )
			{
				ItemAddr =	GET_CONTINUE_ADDR(LcdConRom.DisplayItem.LoopItem[0][0]);
				AddrOffset = (ScreenNo-1)*10 ;
				//读取循显的两个OAD 以及两个显示信息
				if( api_ReadFromContinueEEPRom(ItemAddr+AddrOffset, 10, DispOADInfo) != TRUE )
				{
					//读EE错误，使用默认显示项
					#if (SLE_THIRD_MEM_BACKUP == YES)
					if (LoopItemDef[ScreenNo - 1] == 0)
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[2][0]), 10);
					}
					else if (LoopItemDef[ScreenNo - 1] < 200)
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[LoopItemDef[ScreenNo - 1]][0]), 10);
					}
					else if (LoopItemDef[ScreenNo - 1] < 220)
					{
						memcpy(DispOADInfo, (BYTE *)&(ExpandLcdItemTable[LoopItemDef[ScreenNo - 1] - 200][0]), 10);
					}
					else
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[2][0]), 10);
					}

					lib_InverseData(DispOADInfo, 4);
					lib_InverseData(DispOADInfo + 4, 4);
					#endif
				}
				memcpy(LcdItemOAD.MainOAD.b,DispOADInfo,4);
				memcpy(LcdItemOAD.SubOAD.b,DispOADInfo+4,4);
				LcdItemOAD.OADNum = DispOADInfo[8];
				LcdItemOAD.ScreenOrder = DispOADInfo[9];
			}
			break;
		case eLcdKeyMode:
			if( (ScreenNo!=0) && (ScreenNo<=MAX_LCD_KEY_ITEM) )
			{
				ItemAddr =  GET_CONTINUE_ADDR(LcdConRom.DisplayItem.KeyItem[0][0]);
				AddrOffset = (ScreenNo-1)*10 ;
				//读取循显的两个OAD 以及两个显示信息
				if( api_ReadFromContinueEEPRom(ItemAddr+AddrOffset, 10, DispOADInfo) != TRUE )
				{
					//读EE错误，使用默认显示项
					#if (SLE_THIRD_MEM_BACKUP == YES)
					if (KeyItemDef[ScreenNo - 1] == 0)
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[2][0]), 10);
					}
					else if (KeyItemDef[ScreenNo - 1] < 200)
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[KeyItemDef[ScreenNo - 1]][0]), 10);
					}
					else if (KeyItemDef[ScreenNo - 1] < 220)
					{
						memcpy(DispOADInfo, (BYTE *)&(ExpandLcdItemTable[KeyItemDef[ScreenNo - 1] - 200][0]), 10);
					}
					else
					{
						memcpy(DispOADInfo, (BYTE *)&(LcdItemTable[2][0]), 10);
					}

					lib_InverseData(DispOADInfo, 4);
					lib_InverseData(DispOADInfo + 4, 4);
					#endif
				}
				memcpy(LcdItemOAD.MainOAD.b,DispOADInfo,4);
				memcpy(LcdItemOAD.SubOAD.b,DispOADInfo+4,4);
				LcdItemOAD.OADNum = DispOADInfo[8];
				LcdItemOAD.ScreenOrder = DispOADInfo[9];
			}
			break;
		case eLcdPowerOnMode:
			LcdItemOAD.MainOAD.d = 0xFFFFFFFF;		
			LcdItemOAD.SubOAD.d = 0xdfdfdfdf;
			LcdItemOAD.OADNum = 0x01;
			LcdItemOAD.ScreenOrder = 0x00;
			break;
		case eLcdEchoInfoMode:
			break;
		default:
			//无效时显示当前组合有功总电能
			LcdItemOAD.MainOAD.d = 0x00000201;		
			LcdItemOAD.SubOAD.d = 0xFFFFFFFF;
			LcdItemOAD.OADNum = 0x01;
			LcdItemOAD.ScreenOrder = 0x00;
			break;
	}
}

//-----------------------------------------------
//函数功能: 获取下一个错误编号
//
//参数: 	Type[in]		eUpKey/eDownKey
//			LastErrNo[in]	上次显示错误编号
//返回值:	查找下一个要显示的错误编号，FFFF代表没有查到下一个错误
//			
//备注: 
//-----------------------------------------------
static WORD GetNextErrNo( BYTE Type, WORD LastErrNo )
{
	WORD i,j;
	WORD ErrNo,StartErrNo,FirstErrNo;
	
	ErrNo = 0xffff;
	StartErrNo = LastErrNo;
	FirstErrNo = 0;
	
	//上翻键查找下一个错误，只有在键显时才用
	if(Type == eUpKey)
	{
		if( (StartErrNo>=(0x100+1)) && (StartErrNo<=(0x100+MAX_ERROR)) )
		{
			for( i=StartErrNo; i>=0x100+1; i-- )
			{
				j = (i-0x100-1) % MAX_ERROR;
				//自用类错误不可能为0
				if( (api_CheckError(j)==TRUE) && (j!=0) )
				{
					ErrNo = j+0x100;
					return ErrNo;
				}
			}						
		}
		
		if( StartErrNo > LCD_ERR_END_NO )
		{
			StartErrNo = LCD_ERR_END_NO;	
		}			
		
		for( i=StartErrNo-1; i>=1; i-- )
		{
			j = (i-1) % LCD_ERR_END_NO;
			
			//如果相应的错误状态标志位有效
			if( (LcdWarnStatusWord[j/8]&(0x01<<(j%8))) != 0 )
			{
				//如果相应的错误需要显示
				if( (LcdWarnModeWord.WarnLcdDisp[3][j/8]&(0x01<<(j%8))) != 0 )
				{
					ErrNo = j+1;
					break;
				}
			}
		}
	}
	else
	{
		//下翻时分键显和循显两种情况，循显往下查找不到错误时要置第一个错误，所以要全部查一遍
		for( i=0; i<LCD_ERR_END_NO; i++ )
		{
			j = i % LCD_ERR_END_NO;
			
			//如果相应的错误状态标志位有效
			if( (LcdWarnStatusWord[j/8]&(0x01<<(j%8))) != 0 )
			{
				//如果相应的错误需要显示
				if( (LcdWarnModeWord.WarnLcdDisp[3][j/8]&(0x01<<(j%8))) != 0 )
				{
					ErrNo = j+1;
					if(FirstErrNo == 0)
					{
						FirstErrNo = ErrNo;
					}
					if(ErrNo > StartErrNo)
					{
						return ErrNo;
					}
				}
			}
		}
		if( LcdWarnModeWord.WarnLcdDisp[3][3]&(0x80) != 0 )//使用保留未用的位作为厂内错误的开关
		{
          	for( i=0x101; i<=(0x100+MAX_ERROR); i++ )
			{
				j = (i-0x100) % MAX_ERROR;
				if( api_CheckError(j) == TRUE )
				{
					ErrNo = j+0x100;
					if(FirstErrNo == 0)
					{
						FirstErrNo = ErrNo;
					}
					if(ErrNo > StartErrNo)
					{
						break;
					}				
				}
			}
		}
		
		if( i > (0x100+MAX_ERROR) )
		{
			ErrNo = 0xffff;
		}
		
		//循显没找到错误置第一个错误
		if( (LcdCtrl.Mode==eLcdLoopMode)&& (FirstErrNo!=0) )
		{
			ErrNo = FirstErrNo;
		}
		else if(ErrNo == StartErrNo)
		{
  
            ErrNo = 0xffff;

		}
	}

	return ErrNo;
}

//-----------------------------------------------
//函数功能: 检测当前错误编号对应错误是否继续存在
//
//参数: 	ErrNo[in]	当前错误编号是否继续存在
//			
//返回值:	YES--继续存在	
//			NO--当前现实的错误已经消失
//备注:  
//-----------------------------------------------
static BOOL CheckLcdCurrentErrDisappear(WORD ErrNo)
{
	WORD tErrNo;
	
	if( (ErrNo==0) || (ErrNo>0x1ff) )
	{
		//若没有错误，则不作处理
		return FALSE;
	}
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( (ErrNo >= 31)&&(ErrNo <= 36) )
	{
		//卡表的ERR显示不做处理
		return FALSE;
	}
	#endif//#if( PREPAY_MODE == PREPAY_LOCAL )

	if( ErrNo >= 0x100 ) 
	{
		tErrNo = ErrNo;
	}
	else
	{
		tErrNo = ErrNo - 1;
	}
	
	if( tErrNo >= 0x100 ) 
	{
		tErrNo = (tErrNo-0x100)%MAX_ERROR;
		if( api_CheckError(tErrNo) == FALSE )
		{
			return TRUE;
		}
	}
	else if( tErrNo < LCD_ERR_END_NO )
	{
		if( (LcdWarnStatusWord[tErrNo/8]&(0x01<<(tErrNo%8))) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------
//函数功能: 循显处理
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
static void LoopProcess(void)
{
	LcdCtrl.Timer = FPara1->LCDPara.LoopDisplayTime;
	
	if( LcdCtrl.Mode != eLcdLoopMode )
	{
		LcdCtrl.Mode = eLcdLoopMode;
		LcdCtrl.Screen = 0;
		LcdCtrl.ErrNo = 0;
	}
	
	if( LcdCtrl.Screen == 0 )
	{
		//现在这种错误方法，若错误马上消失后，需要下个显示周期
		LcdCtrl.ErrNo = GetNextErrNo(eDownKey,LcdCtrl.ErrNo);
		if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
		{
			//显示此屏号
			LcdCtrl.Screen = 1;
		}
		else
		{
			//显示LcdCtrl.ErrNo 错误号
		}
	}
	else
	{
		LcdCtrl.Screen++;
		if( LcdCtrl.Screen > FPara1->LCDPara.DispItemNum[0] )
		{
			LcdCtrl.Screen = 0;
			LcdCtrl.ErrNo = GetNextErrNo(eDownKey,0);
			if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
			{
				//显示此屏号
				LcdCtrl.Screen = 1;
			}
			else
			{
				//显示LcdCtrl.ErrNo 错误号
			}
		}
		else
		{
			//显示LcdCtrl.Screen 屏号
			LcdCtrl.ErrNo = 0xffff;			
		}		
	}
	
	GetDispCode(LcdCtrl.Mode,LcdCtrl.Screen);
}

//-----------------------------------------------
//函数功能: 键显处理
//
//参数: 	Keys[in]	eUpDownKey/eUpKey/eDownKey
//			
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
static void KeyProcess(BYTE Keys)
{	
	BYTE KeyMaxNum;

	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )//掉电状态
	{
		LcdCtrl.Timer = FPara1->LCDPara.LcdSwitchTime;//使用无电按键屏幕驻留时间
		MaxWakeupTimer = FPara1->LCDPara.LcdSwitchTime;//重新进行赋值
	}
	else
	{
		LcdCtrl.Timer = FPara1->LCDPara.KeyDisplayTime;//使用键显时间
	}
	
	#if( PREPAY_MODE == PREPAY_LOCAL )
	BeepReadCardFlag = 0x0000;
	BalanceNegativeFlag = 0x00;
	ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess);
    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardFailure);
    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdHoard);
	#endif
	
	//仅全显模式使用的参数切换到键显模式应该初始化掉
	if(LcdCtrl.Mode == eLcdPowerOnMode)
	{
		//跳闸灯点亮时间
		LcdDispTimer.Led = 0;
	}

	//切换到键显
	if(LcdCtrl.Mode!=eLcdKeyMode) 
	{
		LcdCtrl.Mode = eLcdKeyMode;
		LcdCtrl.Screen = 0;
		LcdCtrl.ErrNo = 0;
	}

	KeyMaxNum = FPara1->LCDPara.DispItemNum[1];
	
	
	if( Keys == eUpKey )	//上翻键
	{
		if( LcdCtrl.Screen == 0 )
		{
			LcdCtrl.ErrNo = GetNextErrNo(eUpKey,LcdCtrl.ErrNo);
			if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
			{
				//显示此屏号
				LcdCtrl.Screen = KeyMaxNum;
			}
			else
			{
				//显示LcdCtrl.ErrNo 错误号
			}
		}
		else
		{
			LcdCtrl.Screen--;
			if( LcdCtrl.Screen == 0 )
			{
				LcdCtrl.ErrNo = GetNextErrNo(eUpKey,0x100+MAX_ERROR);
				if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
				{
					//显示此屏号
					LcdCtrl.Screen = KeyMaxNum;
				}
				else
				{
					//显示LcdCtrl.ErrNo 错误号
				}
			}
			else
			{
				LcdCtrl.ErrNo = 0xffff;
				//显示LcdCtrl.Screen 屏号
			}			
		}
	}
	else if( Keys == eDownKey )	//下翻键
	{
		if( LcdCtrl.Screen == 0 )
		{
			LcdCtrl.ErrNo = GetNextErrNo(eDownKey,LcdCtrl.ErrNo);
			if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
			{
				//显示此屏号
				LcdCtrl.Screen = 1;
			}
			else
			{
				//显示LcdCtrl.ErrNo 错误号
			}
		}
		else
		{
			LcdCtrl.Screen++;
			if( LcdCtrl.Screen > KeyMaxNum )
			{
				LcdCtrl.Screen = 0;
				LcdCtrl.ErrNo = GetNextErrNo(eDownKey,0);
				if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
				{
					//显示此屏号
					LcdCtrl.Screen = 1;
				}
				else
				{
					//显示LcdCtrl.ErrNo 错误号
				}
			}
			else
			{
				LcdCtrl.ErrNo = 0xffff;
				//显示LcdCtrl.Screen 屏号
			}			
		}
	}
	else
	{
		//无效按键 无操作
	}
	
	GetDispCode(LcdCtrl.Mode,LcdCtrl.Screen);
	//防止低功耗唤醒不断按键情况下，还是进入低功耗
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )
	{
		WakeUpTimer = 0;
	}
}

//-----------------------------------------------
//函数功能: 规约控制液晶显示的切换
//
//参数: 	CommKeyType[in]	eUpKey/eDownKey
//			
//返回值:	无	
//			
//备注:  只支持键显的上翻和下翻查看(和魏工确认过了)
//-----------------------------------------------
void api_ProcLcdCommKey(BYTE CommKeyType)
{
	BYTE i;
	
	if( (CommKeyType!=eUpKey) && (CommKeyType!=eDownKey) )
	{
		return;
	}
	KeyProcess(CommKeyType);
	SetLcdBackLightTimer(FPara1->LCDPara.BackLightPressKeyTimer);
}

//-----------------------------------------------
//函数功能: 按键检测
//
//参数: 	无
//			
//返回值:	无	
//			
//备注:  检测上键 下键 双键
//-----------------------------------------------
static void DetectLcdKey(void) 
{
	BYTE i,KeyType;
	TFourByteUnion tempOAD;
	
	KeyType = eKeyInValid;

	if( DOWN_KEY_PRESSED  )	//下键
	{
		if(LcdKeyDetTimer.DownKey < 10)
		{
			LcdKeyDetTimer.DownKey++;
			if(LcdKeyDetTimer.DownKey == 4 )
			{
				KeyType = eDownKey;
			}
		}
		if(LcdKeyDetTimer.UpKey < 801)//单相表长按8s液晶显示DF01
		{
			LcdKeyDetTimer.UpKey++;
			if(LcdKeyDetTimer.UpKey>800)
			{
				//送样时 电压影响量开启SelVolCorrectConst为YES 按键长按显示厂家编号
				//供货时 电压影响量关闭SelVolCorrectConst为NO 防止按键损坏后显示"EEEEEEEE" 
				if (SelVolCorrectConst == YES)
				{
					tempOAD.d = 0x01df01df;//直接赋值了就不倒序了，0xdf01df01
					api_WriteLcdEchoInfo( 1,tempOAD.b,0,5);
				}	
			}
		}
	}	
	else if(!DOWN_KEY_PRESSED)	//没有按下按键
	{			
		LcdKeyDetTimer.DownKey = 0;
		LcdKeyDetTimer.UpKey = 0;	
	}

	if( KeyType != eKeyInValid )
	{
		KeyProcess( KeyType );
		SetLcdBackLightTimer(FPara1->LCDPara.BackLightPressKeyTimer);
		Drv_OpenBKLight();
		BuildScene();
	}
}

//-----------------------------------------------
//函数功能: 刷新LCD系统报警异常状态字
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 每秒刷新一次
//-----------------------------------------------
static void UpdateLcdWarnStatusWord(void) 
{
	BYTE i,EventStatus;

	for(i=0;i<64;i++)
	{
		if(EventWarnErrorTab[i] != 0xff)
		{
			if( (i>=16) && (i<=24) )
			{
				if(		(api_DealEventStatus(eGET_EVENT_STATUS,EventWarnErrorTab[i]) == 1)
					||	(api_DealEventStatus(eGET_EVENT_STATUS,EventWarnErrorTab[i]+1) == 1)
					||	(api_DealEventStatus(eGET_EVENT_STATUS,EventWarnErrorTab[i]+2) == 1)	)
				{
					//当前正在发生对应事件
					SetLcdStatusFlag(eLcdWarnFlag,i);
				}
				else
				{
					ClrLcdStatusFlag(eLcdWarnFlag,i);
				}
			}
			else
			{
				EventStatus = api_DealEventStatus(eGET_EVENT_STATUS,EventWarnErrorTab[i]);
				if(EventStatus == 1)
				{
					SetLcdStatusFlag(eLcdWarnFlag,i);
				}
				else
				{
					ClrLcdStatusFlag(eLcdWarnFlag,i);
				}
			}
		}
	}

	// 继电器故障状态 //控制回路错误
	if( api_GetSysStatus(eSYS_STATUS_RELAY_ERR) == TRUE )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_CTR_CURRENT_ERR);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_CTR_CURRENT_ERR);
	}

	//ESAM错误
	if( api_GetRunHardFlag(eRUN_HARD_ESAM_ERR) == TRUE )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_ESAM_ERR);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_ESAM_ERR);
	}

	//时钟电池电压低
	if( api_GetRunHardFlag(eRUN_HARD_CLOCK_BAT_LOW) == TRUE )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_RTC_BATT_LOW);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_RTC_BATT_LOW);
	}

	//存储器故障或损坏 第1、2片EEPROM坏，或 45DB161/45DB321 坏
	if( api_CheckError(ERR_WRITE_EEPROM1) || api_CheckError(ERR_WRITE_EEPROM2) || api_CheckError(ERR_WRITE_EEPROM3) )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_EEPROM_ERR);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_EEPROM_ERR);
	}

	//时钟故障
	if(( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
	|| ( api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE ))
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_RTC_ERR);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_RTC_ERR);
	}

	//当前是否已透支
	#if( PREPAY_MODE == PREPAY_LOCAL )
	if( api_GetSysStatus(eSYS_STATUS_PRE_USE_MONEY) == TRUE )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_OVER_DRAFT);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_OVER_DRAFT);
	}
	#endif
	
	//停电抄表电池欠压
	if( api_GetRunHardFlag(eRUN_HARD_READ_BAT_LOW) == TRUE )
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_RUN_BATT_LOW);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_RUN_BATT_LOW);
	}

	//有功电能方向反向
	if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P) == TRUE)
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_ALL_POWER_BACK);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_ALL_POWER_BACK);
	}

	//有功需量超限，正向及反向有功
	#if(SEL_DEMAND_OVER == YES)
	if((api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_PA_DEMAND_OVER) == TRUE)//正向有功
	||(api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_NA_DEMAND_OVER) == TRUE)//反向有功
	||(api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_QI_DEMAND_OVER) == TRUE)//第一象限无功
	||(api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_QII_DEMAND_OVER) == TRUE)//第二象限无功
	||(api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_QIII_DEMAND_OVER) == TRUE)//第三象限无功
	||(api_DealEventStatus(eGET_EVENT_STATUS,eSUB_EVENT_QIV_DEMAND_OVER) == TRUE)//第四象限无功
	)
	{
		SetLcdStatusFlag(eLcdWarnFlag,LCD_P_DEMAND_OVER);
	}
	else
	{
		ClrLcdStatusFlag(eLcdWarnFlag,LCD_P_DEMAND_OVER);
	}
	#endif

	//合闸、跳闸
	if(RelayTypeConst != RELAY_NO)
	{
		if(api_GetRelayStatus( 1 ) != 0)//跳闸加合闸允许均算跳闸
		{
			SetLcdStatusFlag(eLcdWarnFlag,LCD_RELAY_OPEN);
			ClrLcdStatusFlag(eLcdWarnFlag,LCD_RELAY_CLOSE);
		}
		else 										  //合闸
		{
			SetLcdStatusFlag(eLcdWarnFlag,LCD_RELAY_CLOSE);
			ClrLcdStatusFlag(eLcdWarnFlag,LCD_RELAY_OPEN);		
		}	
	}

}

//-----------------------------------------------
//函数功能: 根据LCD系统报警异常状态字及模式字决定是否打开背光/报警继电器/小铃铛/ERR
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 每秒刷新一次
//-----------------------------------------------
static void JudgeLcdWarnStatusSecTask(void)
{
	BYTE i;
	BOOL LcdBackLightFlag;
	BOOL CurrErrFlag;
	
	LcdBackLightFlag = FALSE;

	UpdateLcdWarnStatusWord();

	//根据系统报警状态判断是否需要光报警
	for(i=0;i<MAX_WARN_ERR_NUM;i++)
	{	
		//液晶背光
		if( (LcdWarnStatusWord[i] & LcdWarnModeWord.WarnLcdDisp[2][i]) != 0 )
		{
			LcdBackLightFlag = TRUE;
		}		
	}

	//判断错误,若当前显示的错误号已消失，做一次按键处理
	CurrErrFlag = CheckLcdCurrentErrDisappear(LcdCtrl.ErrNo);
	if( (CurrErrFlag==TRUE) && (LcdCtrl.Screen==0) )
	{
		KeyProcess( eDownKey );
	}

	//置背光计数器
	if( LcdBackLightFlag == TRUE )
	{
		SetLcdBackLightTimer(2);
	}
	
	if( LcdDispTimer.BackLight )
	{
		Drv_OpenBKLight();
	}
	else
	{	
		Drv_CloseBKLight();
	}	

    if( LcdDispTimer.BackLight )
	{
		LcdDispTimer.BackLight--;
	}
}

//-----------------------------------------------
//函数功能: 显示校表ADJUST
//
//参数: 	无
//                    
//返回值:	无
//
//备注:
//-----------------------------------------------
void api_DisplayAdjust(void)
{
	//校表时先遥控显示，点亮背光，再调用此函数，熄灭背光，待校表结束后会继续调用遥控显示的背光计数器
	Drv_OpenBKLight();

	LcdClearAll();
	DigTubCtrlShow(10,3);	//显示A
	DigTubCtrlShow(13,4);	//显示d
	DigTubCtrlShow(17,5);	//显示J
	DigTubCtrlShow(18,6);	//显示U
	DigTubCtrlShow( 5,7);	//显示s
	DigTubCtrlShow(19,8);	//显示t
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 在液晶上显示断言 A-XXXXXX  
//
//参数: 
//			LineNum[in]--断言所在的行号
// 				   
//返回值:	 
//
//使用举例:	 api_DisplayAssert(12345); 		//显示"  A-012345  "
//-----------------------------------------------
void api_DisplayAssert(WORD LineNum)
{
	LcdDData.d = lib_DWBinToBCD(LineNum);
	
	memset(lcd , 0x00 , LCD_BUF_LENGTH);
	DigTubCtrlShow(10,2);	//显示 A
	DigTubCtrlShow(20,3);	//显示-
	ShowData( 0, 6, 1, 1 );
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 显示电表信息
//
//参数: 
//			Type[in]:	显示版本信息编号
//
//返回值:	无
//			
//备注: 参考：三相表双按键显示电表版本信息
//-----------------------------------------------
static void DisplayVersion(BYTE Type)
{
	if(Type < MAX_VERSION)
	{
		LcdClearAll();
		LcdDData.w[0] = api_GetMeterVersion(Type);	//0xdf01;
		ShowData( 0, 4, 1, 1 );
	}
	else
	{
		LcdDData.d = 0xeeeeeeee;	//显示--------
		ShowData( 0, 8, 0, 0 );
	}
}

//-----------------------------------------------
//函数功能: 单相表显示DF01
//
//参数: 	无
//                    
//返回值:	无
//
//备注:  不显示DF01，显示DF+厂家编号
//-----------------------------------------------
static void api_DisplayDF01(void)
{
	//LcdClearAll();
	//DigTubCtrlShow(13,4);
	//DigTubCtrlShow(15,5);
	//DigTubCtrlShow(0,6);
	//DigTubCtrlShow(1,7);

	DisplayVersion(6);

	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 插入显示信息
//
//参数:		InOADNum[in]	OAD个数
//			OADBuf[in]		OAD指针
//			SubIndex[in]	0--不需要分屏  1--第一屏  2--第二屏	
//			EchoTimer[in]	遥控液晶显示时间
//返回值:	TRUE
//		   
//备注:
//-----------------------------------------------
WORD api_WriteLcdEchoInfo(BYTE InOADNum, BYTE *OADBuf, BYTE SubIndex ,WORD EchoTimer)
{	
	LcdCtrl.Mode = eLcdEchoInfoMode;
	lib_ExchangeData(LcdItemOAD.MainOAD.b,OADBuf,4);
	lib_ExchangeData(LcdItemOAD.SubOAD.b,OADBuf+4,4);
	LcdItemOAD.OADNum = InOADNum;
	LcdItemOAD.ScreenOrder = SubIndex;
	
	if(LcdItemOAD.OADNum == 1)
	{
		LcdItemOAD.SubOAD.d = 0xFFFFFFFF;
	}

	// 刷新显示时间,对显示时间有一个限制
	if( EchoTimer < 255 )
	{
		LcdCtrl.Timer = EchoTimer+1;
		SetLcdBackLightTimer(EchoTimer);
	}
	else
	{
		LcdCtrl.Timer = 11;
		SetLcdBackLightTimer(10);
	}
	if((LcdItemOAD.SubOAD.d==0xFFFFFFFF) && (LcdItemOAD.MainOAD.d==0xdfdfdfdf))
	{
		if( EchoTimer < 255 )
		{
			LcdDispTimer.Led = EchoTimer;
		}
		else
		{
			LcdDispTimer.Led = 10;
		}
	}
	
	BuildScene();
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 读取设置液晶的显示、小铃铛、ERR显示、继电器控制参数
//
//参数:		Operation[in]	READ/WRITE
//			Type[in]		0--报警继电器
// 							1--LCD小铃铛报警控制参数
// 							2--液晶背光
// 							3--液晶ERR显示报警
//			Len[in]			数据长度
//			pBuf[in]		读或写的数据缓存
//返回值:	TRUE/False
//	
//备注: 
//-----------------------------------------------
BOOL api_ProcLcdCtrlPara(BYTE Operation,BYTE Type,BYTE Len,BYTE * pBuf)
{	
	BOOL Result;
	
	if( Len != MAX_WARN_ERR_NUM )
	{
		return FALSE;
	}
	
	if( Operation == READ )
	{
		memcpy(pBuf,(BYTE *)&(LcdWarnModeWord.WarnLcdDisp[Type]),Len);
	}
	else
	{
		memcpy(&LcdWarnModeWord.WarnLcdDisp[Type][0],pBuf,Len);
		Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(LcdSafeRom.WarnDispPara),sizeof(TWarnDispPara),&LcdWarnModeWord.WarnLcdDisp[0][0]);
		if(Result == FALSE)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//-----------------------------------------------
//函数功能: 检查RAM中LCD模块参数是否正确
//
//参数:		无 
//			
//返回值: 	无
//		    
//备注:
//-----------------------------------------------
static void CheckLcdRamPara( void )
{
	WORD Result;
	
	if(LcdWarnModeWord.CRC32 != lib_CheckCRC32((BYTE *)&LcdWarnModeWord, (sizeof(LcdWarnModeWord) - sizeof(DWORD)) ) )
	{
		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(LcdSafeRom.WarnDispPara),sizeof(TWarnDispPara), (BYTE*)&LcdWarnModeWord.WarnLcdDisp[0][0]);
		if( Result != TRUE )
		{
			memcpy(&LcdWarnModeWord.WarnLcdDisp[0][0],(void*)&WarnDispParaConst,sizeof(LcdWarnModeWord));
			LcdWarnModeWord.CRC32 = lib_CheckCRC32((BYTE *)&LcdWarnModeWord, (sizeof(TWarnDispPara) - sizeof(DWORD)) );
		}
	}
}

//-----------------------------------------------
//函数功能: 借位功能小数点数判断
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 用于借位功能刷新小数点数
//-----------------------------------------------
static void LCDBorrDotJudge(void)
{
	TEightByteUnion TempSavData[2];
	SQWORD MaxEnergy;
	WORD ReturnLen;
	BYTE tpoint,tpoint1;
	TFourByteUnion tOAD[2];
	memset(TempSavData[0].b,0x00,sizeof(TempSavData));
	if(FPara1->LCDPara.EnergyFloat > 4)
	{
		tpoint = LCDParaDef.EnergyFloat;
	}
	else
	{
		tpoint = FPara1->LCDPara.EnergyFloat;
	}
	//写入对比的OAD
	tOAD[0].d = 0x01020000;	//组合有功电能
	tOAD[1].d = 0x01021000;	//正向有功电能
	//读取当前组合有功电能
	ReturnLen = api_GetProOadData( eGetNormalData, eData, 0, tOAD[0].b, 0X00, 8, TempSavData[0].b );
	if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
	{
		//读取当前正向有功电能
		ReturnLen = api_GetProOadData( eGetNormalData, eData, 0, tOAD[1].b, 0X00, 8, TempSavData[1].b );
		if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
		{
			//组合有功电能要判断正反
			TempSavData[0].ll = llabs(TempSavData[0].ll);
			//认为组合有功和正向有功电能中最大者电能小数点数为电能小数点数最大值
			if(TempSavData[0].ll < TempSavData[1].ll)
			{
				MaxEnergy = TempSavData[1].ll;
			}
			else
			{
				MaxEnergy = TempSavData[0].ll;
			}
			//获取电能小数位数
			tpoint1 = DealBorrowPoint( MaxEnergy );
			//借位功能小数点数出错则刷新
			if(BorrDotNum < tpoint1)
			{
				BorrDotNum = tpoint;
			}
		}
	}
}

//-----------------------------------------------
//函数功能: 液晶500ms任务
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
static void LCD500msTask(void)
{
	if( LcdFlashFlagHalfSec == TRUE )
	{
		LcdFlashFlagHalfSec = FALSE;
	}
	else
	{
		LcdFlashFlagHalfSec = TRUE;
	}
	
	BuildScene();
}

//-----------------------------------------------
//函数功能: 液晶秒任务
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
static void LCDSecTask(void)
{
	BYTE NewSta;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TFourByteUnion TempMoney; 
	#endif

	api_GetRtcDateTime(DATETIME_ss,&NewSta);
	if( NewSta == (eTASK_LCD_ID*3+3) )
	{
		CheckLcdRamPara();
	}
	
	ProcCommunicationFlag();
	
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
	{
		if( LcdFlashFlagSec == TRUE )
		{
			LcdFlashFlagSec = FALSE;
		}
		else
		{
			LcdFlashFlagSec = TRUE;
		}
		
		JudgeLcdWarnStatusSecTask();
			
		//每隔5秒钟初始化一次液晶驱动，确保可靠
		if( LcdDispTimer.InitDriver == 0 )
		{		
			InitLCDDriver();
			LcdDispTimer.InitDriver = 5;
		}
		else
		{
			LcdDispTimer.InitDriver --;
		}
	
		if( LcdDispTimer.Led != 0 )
		{
			RELAY_LED_ON;
			LcdDispTimer.Led--;
		}
	}
	#if  ((BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303))
	else
	{
		// 低功耗唤醒每秒刷新液晶对比度
		InitLCDDriver();
	}
	#endif//#if ((BOARD_TYPE == BOARD_HT_SINGLE_78202201))

	//考虑需量清零
	
	if( LcdCtrl.Timer != 0 )
	{
		LcdCtrl.Timer--;
		if( LcdCtrl.Timer == 0 )
		{
			#if( PREPAY_MODE == PREPAY_LOCAL )
		    if( BeepReadCardFlag == 0x555A )
		    { 
                LcdCtrl.Timer = 3;
		        LcdItemOAD.MainOAD.d = 0x202C0201;
        		LcdItemOAD.ScreenOrder = 0;
        		LcdItemOAD.OADNum = 0x01;
                BeepReadCardFlag = 0x0000;
                BalanceNegativeFlag = 0x00;
		    }
			else
            #endif //#if( PREPAY_MODE == PREPAY_LOCAL )
			{
			    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess);
			    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardFailure);
			    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdHoard);
				#if( PREPAY_MODE == PREPAY_LOCAL )
			    BeepReadCardFlag = 0x0000;
           		#endif //#if( PREPAY_MODE == PREPAY_LOCAL )
			    LoopProcess();
				//如果为上电状态
				if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
				{
					//判断背光是否熄灭
					if( LcdDispTimer.BackLight == 0 )
					{
						Drv_CloseBKLight();
					}
				}
			}
		}
	}
	
	BuildScene();
}

//-----------------------------------------------
//函数功能: 液晶大循环任务
//
//参数: 	无
//			
//返回值:	无	
//			
//备注: 
//-----------------------------------------------
void api_LcdTask(void)
{
	//10ms任务，显示按键检测
	if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_10_MS) == TRUE)	
	{
		DetectLcdKey();
		api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_10_MS );
	}

	#if (BOARD_TYPE == BOARD_HT_SINGLE_78202202)
	if(SelVolCorrectConst == YES)	//送样程序 先执行秒任务再执行 500毫秒任务--- 高防护表为了降低送样时的整机功耗！--20240723
	{
		//秒任务
		if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_SECOND) == TRUE)
		{ 	
			if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_500_MS) == TRUE)	
			{
				LCDSecTask();		
				api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_SECOND );
			}
		}
		
		//500毫秒任务
		if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
		{
			if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_500_MS) == TRUE)	
			{
				LCD500msTask();
				api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_500_MS );
			}
		}
	}
	else
	#endif
	{
		//500毫秒任务
		if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
		{
			if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_500_MS) == TRUE)	
			{
				LCD500msTask();
				api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_500_MS );
			}
		}
		
		//秒任务
		if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_SECOND) == TRUE)
		{ 		 
			LCDSecTask();
			api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_SECOND );
		}
	}

	//小时任务
	if(api_GetTaskFlag(eTASK_LCD_ID, eFLAG_HOUR) == TRUE)
	{ 		 
		LCDBorrDotJudge();		
		api_ClrTaskFlag( eTASK_LCD_ID, eFLAG_HOUR );
	}
}

//-----------------------------------------------
//函数功能: 液晶显示初始化
//
//参数:    
//Type  	Type[in]	ePowerOnMode	上电初始化LCD
//	     				ePowerDownMode	低功耗处理LCD                   
//返回值:	无
//
//备注:  
//-----------------------------------------------
void api_PowerUpLcd(ePOWER_MODE Type)
{
	BYTE uci;
	
	LcdCtrl.ErrNo = 0xffff;
	
	InitLCDDriver();
	CheckLcdRamPara();

	if(Type == ePowerOnMode)
	{
		RELAY_LED_ON;
		Drv_OpenBKLight();//背光点亮以前在主循环控制，上电会导致背光点亮慢2s 现在提前控制后，会导致背光熄灭慢2s左右-jwh-！！！！！！
//		DisplayVersion();
//        for (uci = 0; uci < 150; uci++)
//		{
//			api_Delayms(10); //延时秒
//			CLEAR_WATCH_DOG;
//		}
		LcdDispAll();
	}
	else if(Type == ePowerDownMode)
	{
		//按键唤醒从第一屏开始
		LcdCtrl.Mode = eLcdLoopMode;
		LcdCtrl.Screen = 0;
		LcdCtrl.Timer = FPara1->LCDPara.LoopDisplayTime;//按键唤醒后默认使用循显显示时间//FPara1->LCDPara.PowerOnDispTimer;
		LcdCtrl.ErrNo = GetNextErrNo(eDownKey,LcdCtrl.ErrNo);
		if( (LcdCtrl.ErrNo==0) || (LcdCtrl.ErrNo>0x1ff) )
		{
			//显示此屏号
			LcdCtrl.Screen = 1;
		}
		//借位功能刷新
		LCDBorrDotJudge();
		GetDispCode( eLcdLoopMode,LcdCtrl.Screen );
		BuildScene();
	}
	else
	{
		return;
	}

	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE )
	{
		LcdCtrl.Mode = eLcdPowerOnMode;
		SetLcdBackLightTimer(FPara1->LCDPara.PowerOnDispTimer);
		LcdCtrl.Timer = FPara1->LCDPara.PowerOnDispTimer;
		LcdDispTimer.Led = FPara1->LCDPara.PowerOnDispTimer;
		LcdDispTimer.BackLight = FPara1->LCDPara.PowerOnDispTimer;
		if( (FPara1->LCDPara.PowerOnDispTimer<5) || (FPara1->LCDPara.PowerOnDispTimer>30) )
		{			
			LcdCtrl.Timer = 5;
			LcdDispTimer.Led = 5;
			LcdDispTimer.BackLight = 5;
		}
		//借位功能刷新
		LCDBorrDotJudge();
	}
	else
	{

	}

	SignalIntensity = 0x00;
	LcdDispTimer.InitDriver = 5;
	
	if( Type == ePowerOnMode )//低功耗模式不清LcdItemOAD参数，避免唤醒后第一屏数据不刷新
	{
	    memset((void*)&LcdItemOAD, 0xff, sizeof(LcdItemOAD));
	}


	MaxWakeupTimer = (WORD)( FPara1->LCDPara.LoopDisplayTime * FPara1->LCDPara.DispItemNum[0]);//计算循显一周时间
}

//-----------------------------------------------
//函数功能: 退出全显模式
//
//参数:		无
//                    
//返回值:	无
//
//备注:  厂内退出全显，进入键显，并且关闭背光灯
//		方便车间自动化流水线生产
//-----------------------------------------------
void api_LcdQuitAllDisplay(void)
{	
	KeyProcess(eDownKey);
	BuildScene();

	Drv_CloseBKLight();
	LcdDispTimer.BackLight = 0;		//SetLcdBackLightTimer()无法设置0S时间
}

//-----------------------------------------------
//函数功能: 掉电液晶处理
//
//参数:		无
//                    
//返回值:	无
//
//备注:  
//-----------------------------------------------
void api_PowerDownLcd(void)
{
	Drv_CloseBKLight();
	LcdDispTimer.BackLight = 0;
	LcdDispTimer.Led = 0;
	LcdClearAll();
	//ShutOff_Lcd();
}

//-----------------------------------------------
//函数功能: 奇偶屏检查
//
//参数:		Type:0-CPU管脚第一屏，1-CPU管脚第二屏
//               2-LCD管脚第一屏，3-LCD管脚第二屏
//			Timer:显示时间，单位秒，最大255
//返回值:	无
//
//备注:用于检查CPU或LCD相邻管脚是否有焊锡导致短接
//-----------------------------------------------
void api_CheckDispPin(BYTE Type, BYTE Timer)
{
	BYTE i;
	const BYTE LcdPinData[4][12] = 
	{ 
		#if( (BOARD_TYPE == BOARD_HT_SINGLE_78202201) || (BOARD_TYPE == BOARD_HT_SINGLE_78202303) || (BOARD_TYPE == BOARD_HT_SINGLE_78202401)|| (BOARD_TYPE == BOARD_HT_SINGLE_20250819))
		//CPU管脚第一屏 COM1 COM3 COM5 SEG2 SEG4 SEG6 SEG8 SEG9 SEG11 SEG13 SEG15 SEG17 SEG19 SEG21 SEG23 
		//CPU管脚第二屏 COM0 COM2 COM4 SEG0 SEG1 SEG3 SEG5 SEG7 SEG10 SEG12 SEG14 SEG16 SEG18 SEG20 SEG22
		//LCD管脚第一屏 COM0 COM2 COM4 SEG0 SEG2 SEG4 SEG6 SEG8 SEG10 SEG12 SEG14 SEG16 SEG18 SEG20 SEG22
		//LCD管脚第二屏 COM1 COM3 COM5 SEG1 SEG3 SEG5 SEG7 SEG9 SEG11 SEG13 SEG15 SEG17 SEG19 SEG21 SEG23
		{SEG02, SEG04, SEG06, SEG08, SEG09, SEG11, SEG13, SEG15, SEG17, SEG19, SEG21, SEG23},//CPU管脚第一屏
		{SEG00, SEG01, SEG03, SEG05, SEG07, SEG10, SEG12, SEG14, SEG16, SEG18, SEG20, SEG22},//CPU管脚第二屏
		{SEG00, SEG02, SEG04, SEG06, SEG08, SEG10, SEG12, SEG14, SEG16, SEG18, SEG20, SEG22},//LCD管脚第一屏
		{SEG01, SEG03, SEG05, SEG07, SEG09, SEG11, SEG13, SEG15, SEG17, SEG19, SEG21, SEG23} //LCD管脚第二屏
		#else
		//BOARD_HT_SINGLE_78201602  9600波特率板型未做
		{SEG02, SEG04, SEG06, SEG08, SEG09, SEG11, SEG13, SEG15, SEG17, SEG19, SEG21, SEG23},//CPU管脚第一屏
		{SEG00, SEG01, SEG03, SEG05, SEG07, SEG10, SEG12, SEG14, SEG16, SEG18, SEG20, SEG22},//CPU管脚第二屏
		{SEG00, SEG02, SEG04, SEG06, SEG08, SEG10, SEG12, SEG14, SEG16, SEG18, SEG20, SEG22},//LCD管脚第一屏
		{SEG01, SEG03, SEG05, SEG07, SEG09, SEG11, SEG13, SEG15, SEG17, SEG19, SEG21, SEG23} //LCD管脚第二屏
		#endif
	};

	if(Type >= 4)
	{
		return;
	}

	memset(lcd, 0x00, LCD_BUF_LENGTH);

	for(i=0; i<sizeof(LcdPinData[0]); i++)
	{
		lcd[LcdPinData[Type][i]] |= ((Type == 0) || (Type == 3)) ? 0x2A : 0x15;
	}

	if( Timer == 0 )
	{
		Timer = 10;
	}
	
	SetLcdBackLightTimer(Timer);
	LcdCtrl.Timer = Timer;
	LcdItemOAD.MainOAD.d = 0xffffffff;
	WriteLcdBufToDriver(LCD_BUF_LENGTH);
}

//-----------------------------------------------
//函数功能: 厂内电表初始化任务
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_FactoryInitLcd(void)
{
	WORD i,j;
	BYTE Buf[10];

	//设置Lcdpara里面的显示参数( 主要是显示的时间 )
	api_WritePara(0, GET_STRUCT_ADDR(TFPara1,LCDPara.PowerOnDispTimer), sizeof(TLCDPara), (BYTE *)&LCDParaDef );

	//设置异常报警配置参数(小铃铛、背光、ERR显示、报警继电器)
	memcpy(&LcdWarnModeWord.WarnLcdDisp[0][0],(void*)&WarnDispParaConst,sizeof(LcdWarnModeWord));
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(LcdSafeRom.WarnDispPara),sizeof(TWarnDispPara),&LcdWarnModeWord.WarnLcdDisp[0][0]);
	memset((void*)&LcdWarnStatusWord[0],0,sizeof(LcdWarnStatusWord));
	
	//设置借位显示小数点数
	BorrDotNum = LCDParaDef.EnergyFloat;
	//设置循显项
	j = 0;
	for(i=0; i<sizeof(LoopItemDef); i++)
	{
		if(LoopItemDef[i] == 0)
		{
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[2][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[2][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[2][8]),2);
		}
		else if( LoopItemDef[i] < 200 )
		{
			j = LoopItemDef[i];
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[j][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[j][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[j][8]),2);
		}
		else if(  LoopItemDef[i] < 220 )
		{
			j = LoopItemDef[i] - 200;
			lib_ExchangeData( Buf, (BYTE *)&(ExpandLcdItemTable[j][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(ExpandLcdItemTable[j][4]),4);
			memcpy(Buf+8,(BYTE *)&(ExpandLcdItemTable[j][8]),2);
		}	
		else
		{
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[2][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[2][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[2][8]),2);
		}
					
		//fe时不设置显示项，由规约进行设置
		if( LoopItemDef[i] != 0xfe )
		{
			api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( LcdConRom.DisplayItem.LoopItem[i][0] ), 10, Buf );
		}
	}
	for(i=sizeof(LoopItemDef); i<MAX_LCD_LOOP_ITEM; i++)
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LcdConRom.DisplayItem.LoopItem[i][0]), 10, Buf );	
	}	
	
	//设置键显项
	for(i=0; i<sizeof(KeyItemDef); i++)
	{		
		if(KeyItemDef[i] == 0)
		{
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[2][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[2][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[2][8]),2);
		}
		else if( KeyItemDef[i] < 200 )
		{
			j = KeyItemDef[i];
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[j][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[j][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[j][8]),2);
		}
		else if(  KeyItemDef[i] < 220  )
		{
			j = KeyItemDef[i] - 200;
			lib_ExchangeData( Buf, (BYTE *)&(ExpandLcdItemTable[j][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(ExpandLcdItemTable[j][4]),4);
			memcpy(Buf+8,(BYTE *)&(ExpandLcdItemTable[j][8]),2);
		}	
		else
		{
			lib_ExchangeData( Buf, (BYTE *)&(LcdItemTable[2][0]), 4 );
			lib_ExchangeData(Buf+4,(BYTE *)&(LcdItemTable[2][4]),4);
			memcpy(Buf+8,(BYTE *)&(LcdItemTable[2][8]),2);
		}
					
		//fe时不设置显示项，由规约进行设置
		if( KeyItemDef[i] != 0xfe )
		{
			api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( LcdConRom.DisplayItem.KeyItem[i][0] ), 10, Buf );
		}
	}
	for(i=sizeof(KeyItemDef); i<MAX_LCD_KEY_ITEM; i++)
	{
		api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(LcdConRom.DisplayItem.KeyItem[i][0]), 10, Buf );	
	}	
}

//-----------------------------------------------
//函数功能: 数码管控制显示函数
//
//参数:		ShowNum		显示符号的编号，0->0x00,a->0x0a,f->0x0f诸如此类
//						通过DigTubCode数组对应编码和符号
//			DigTubNum	在哪个数码管上显示，中间的8位数码管从左到右编号为2-9，上方费率编号1
//						上方的“上 月”编号0，下方阶梯号“L ”编号10，下方时段(费率)号“T ”编号11
//						
//返回值:	无
//		   
//备注:	这个函数是写入缓存来控制数码管的最下层函数，要求控制数码管的话调用这个函数
//		由于控制数码管的管脚太混乱了，所以把管脚分配列成表格，符号显示做成数组
//		控制的时候a,b,c,d,e,f一个个写进液晶数据缓存，后面再改也方便
//-----------------------------------------------
BOOL DigTubCtrlShow(BYTE ShowNum,BYTE DigTubNum)
{
	BYTE i;
	//超过极限直接跳出
	if((ShowNum >= LCD_SIGN_NUM) || (DigTubNum >= LCD_DIGITAL_TUBE_NUM))
	{
		return FALSE;
	}
	//a,b,c……f一个个写进缓存
	for(i = 0;i < 7;i++)
	{
		//如果显示字符要求点亮数码管该段
		if(DigTubCode[ShowNum]&(0x01<<i))
		{
			//lcd是显示缓存，显示驱动将缓存写入LCD
			//DigTubCtrl是LCD真值表中数码管控制管脚数([0])及其对应位数([1])
			//向缓存写入对应位
			#if ( BOARD_TYPE == BOARD_HT_SINGLE_78201602 )
			lcd[ CPULCDCONNECT[ DigTubCtrl[0][DigTubNum][i] ] ] |= DigTubCtrl[1][DigTubNum][i];
			#else
			lcd[ DigTubCtrl[0][DigTubNum][i] ] |= DigTubCtrl[1][DigTubNum][i];
			#endif
		}
	}
	return TRUE;
}
//-----------------------------------------------
//函数功能: 数码管清除函数
//
//参数:			DigTubNum	在哪个数码管上显示，中间的8位数码管从左到右编号为2-9，上方费率编号1
//							上方的“上 月”编号0，下方阶梯号“L ”编号10，下方时段(费率)号“T ”编号11
//						
//返回值:	无
//		   
//备注:	这个函数用于给指定数码管写空
//-----------------------------------------------

void CLRDigTub(BYTE DigTubNum)
{
	BYTE i;	
	if(DigTubNum >= LCD_DIGITAL_TUBE_NUM)
	{
		return;
	}
	//数码管清空
	for(i = 0;i < 7;i++)
	{
		//向缓存写入对应位
		lcd[DigTubCtrl[0][DigTubNum][i]] &= ~DigTubCtrl[1][DigTubNum][i];
	}
}

#if( PREPAY_MODE == PREPAY_LOCAL )

//-----------------------------------------------
//函数功能: 读卡前调用，清除当前读卡状态
//
//参数:		无
//
//返回值:	    无
//
//备注:
//-----------------------------------------------
static void ClrReadCardStatus(void)
{
    BEEP_OFF;
    BeepTimeFlag = 0;
    BeepReadCardFlag = 0x0000;
	BalanceNegativeFlag = 0x00;
	ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess);
    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdCardFailure);
    ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdHoard);
}

//-----------------------------------------------
//函数功能: 读卡成功,蜂鸣器长鸣630ms;失败短鸣130ms三次，间隔130ms
//
//参数:		无
//
//返回值:	    插卡成功或失败2s内执行一次
//
//备注:
//-----------------------------------------------
void api_BeepReadCard( void )
{
	if( (BeepReadCardFlag==0x555A) || (BeepReadCardFlag==0xAA5A) )
	{
		if( (++BeepTimeFlag) >= 63 )//630ms
		{
			BEEP_OFF;
		}
	}
	else if( BeepReadCardFlag == 0x5AA5 )
	{
		++BeepTimeFlag;

        switch( (BeepTimeFlag) / 13 )
        {
            case 0:
                break;
            case 2:
            case 4:
                BEEP_ON;
                break;
            default:
                BEEP_OFF;
                break;
        }
	}
    if( (BeepTimeFlag > 400) || (BeepReadCardFlag==0x00) )
    {
        BeepTimeFlag = 0;
        BEEP_OFF;
    }
}

//-----------------------------------------------
//函数功能: 读卡状态、剩余金额、错误信息显示
//
//参数:       Step[IN]     00:读卡中   03:用户卡    02:预置卡
//
//          ErrReg[IN]   00:插卡成功       其他为失败
//          Balance[IN]  插卡前剩余金额
//返回值:
//			无
//备注:
//-----------------------------------------------
void api_ReadCardDisplay(BYTE Step, BYTE ErrReg, long Balance)
{

	if( Step == 0x00 )
	{
		LcdCtrl.Timer = 3;
		ClrReadCardStatus();
		Drv_OpenBKLight();
		SetLcdBackLightTimer(60);
		LCD_CPUCARD_READING;		//直接更改lcd缓存数组，插入1屏
		WriteLcdBufToDriver(LCD_BUF_LENGTH);	//写液晶驱动
	}
	else if( (Step == USER_CARD_COMMAND) && (ErrReg == OK_CARD_PROC) )
	{
	    BEEP_ON;
		SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess);

		BeepReadCardFlag = 0x555A;//用户卡读卡成功

		if( Balance < 0 )
		{
			Balance = (~Balance) + 1;
			BalanceNegativeFlag = 0x01;
        }
        ReadCardBeforeBalance.d = lib_DWBinToBCD(Balance);//显示剩余金额,用户卡插卡成功

        LcdCtrl.Timer = 3;
		LcdCtrl.ErrNo = 0;
		LcdItemOAD.MainOAD.d = 0x202C0201;
		LcdItemOAD.ScreenOrder = 0;
		LcdItemOAD.OADNum = 0x01;
		//插入1屏后lcd缓存发生变更，全清把变更去掉，再进屏显函数
		memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
		BuildScene();	//屏显函数
	}
	else if( (Step == PRESET_CARD_COMMAND) && (ErrReg == OK_CARD_PROC) )
	{
	    BEEP_ON;
		SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCardSuccess);

		BeepReadCardFlag = 0xAA5A;//预置卡读卡成功

		LcdCtrl.Timer = 4;  //显示剩余金额，预置卡插卡成功
		LcdCtrl.ErrNo = 0;
		LcdItemOAD.MainOAD.d = 0x202C0201;
		LcdItemOAD.ScreenOrder = 0;
		LcdItemOAD.OADNum = 0x01;
		//插入1屏后lcd缓存发生变更，全清把变更去掉，再进屏显函数
		memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
		BuildScene();	//屏显函数
    }
	else if( ErrReg != OK_CARD_PROC )
	{
	    BEEP_ON;
		SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCardFailure);

		BeepReadCardFlag = 0x5AA5;//读卡失败

		LcdCtrl.Timer = 4;  //显示ERR-XX--用户卡、预置卡插卡失败--5s
		LcdCtrl.Mode = eLcdEchoInfoMode;
		LcdCtrl.Screen = 0;
		ErrReg = lib_BBCDToBin( ErrReg );
		LcdCtrl.ErrNo = ErrReg;
		if( ErrReg == 36 )
		{
            SetLcdStatusFlag(eLcdStatusSegFlag,eLcdHoard);
		}
		//插入1屏后lcd缓存发生变更，全清把变更去掉，再进屏显函数
		memset( (void*)&lcd[0], 0, LCD_BUF_LENGTH );
		BuildScene();	//屏显函数
	}
}






#endif //#if( PREPAY_MODE == PREPAY_LOCAL )
#endif  //#if( LCD_TYPE == LCD_HT_SINGLE_METER )
