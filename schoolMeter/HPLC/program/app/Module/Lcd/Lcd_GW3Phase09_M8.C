//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	马亮 张玉猛
//创建日期	2016-10-08
//描述		三相表液晶显示处理
//修改记录	显示程序中的OAD都是按照小端模式保存处理，调用规约函数时将OAD倒序后调用
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Lcd_GW3Phase09_M8.h"

#if( LCD_TYPE == LCD_GW3PHASE_09 )

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
//数码管7段的控制pin口(0),位数(1)	数码管数量 从0到6 a,b,c,d,e,f,g
const static BYTE DigTubCtrl[2][LCD_DIGITAL_TUBE_NUM][7] =
{	//数码管7段的控制pin口
	{
		{30,30,30,30,31,31,31},		//1
		{21,21,21,21,22,22,22},		//2
		{32,32,32,32,33,33,33},		//3
		{30,30,30,30,31,31,31},		//4
		{28,28,28,28,29,29,29},		//5
		{26,26,26,26,27,27,27},		//6
		{24,24,24,24,25,25,25},		//7
		{11,11,11,11,10,10,10},		//8
		{13,13,13,13,12,12,12},		//9
		{15,15,15,15,14,14,14},		//10
		{17,17,17,17,16,16,16},		//11
		{23,23,23,23,18,18,18},		//12
		{ 0, 1, 1, 0, 0, 0, 1},		//13
		{ 2, 3, 3, 2, 2, 2, 3},		//14
		{ 8, 7, 7, 8, 8, 8, 7},		//15
		{ 6, 5, 5, 6, 6, 6, 5},		//16
		{ 5, 6, 6, 5, 5, 5, 6},		//17
		{ 7, 8, 8, 7, 7, 7, 8},		//18
		{ 9,10,10, 9, 9, 9,10},		//19
		{11,12,12,11,11,11,12},		//20
		{18,18,18,18,17,17,17},		//21
		{19,19,19,19,20,20,20},		//22
		{ 0, 1, 1, 0, 0, 0, 1},		//23
		{ 2, 3, 3, 2, 2, 2, 3},		//24
	},
	{
		{0x01,0x02,0x04,0x08,0x04,0x01,0x02},		//1
		{0x01,0x02,0x04,0x08,0x04,0x01,0x02},		//2
		{0x10,0x20,0x40,0x80,0x80,0x20,0x40},		//3
		{0x10,0x20,0x40,0x80,0x80,0x20,0x40},		//4
		{0x10,0x20,0x40,0x80,0x80,0x20,0x40},		//5
		{0x10,0x20,0x40,0x80,0x80,0x20,0x40},		//6
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//7
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//8
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//9
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//10
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//11
		{0x10,0x20,0x40,0x80,0x40,0x10,0x20},		//12
		{0x80,0x80,0x20,0x10,0x20,0x40,0x40},		//13
		{0x80,0x80,0x20,0x10,0x20,0x40,0x40},		//14
		{0x80,0x80,0x20,0x10,0x20,0x40,0x40},		//15
		{0x80,0x80,0x20,0x10,0x20,0x40,0x40},		//16
		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},		//17
		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},		//18
		{0x80,0x08,0x02,0x02,0x04,0x08,0x04},		//19
		{0x08,0x04,0x01,0x01,0x02,0x04,0x02},		//20
		{0x08,0x04,0x02,0x01,0x02,0x08,0x04},		//21
		{0x80,0x40,0x20,0x10,0x20,0x80,0x40},		//22
		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},		//23
		{0x08,0x08,0x02,0x01,0x02,0x04,0x04},		//24
	}
};

//符号对应数码管段码数组	BYTE8位中0x80闲置,0x40->g,0x20->f……0x01->a
//						  				  		  0    1    2    3     4    5    6    7    8    9 
const static BYTE DigTubCode[LCD_SIGN_NUM] = {	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7f,0x6f,
//						  				 		 10A  11B  12C   13D  14E  15F 16空白  17J  18U  19t   20-   21r  22P	 23_
												0x77,0x7C,0x39,0x5E,0x79,0x71,0x00,0x0E,0x3E,0x78,0x40,0x50,0x73,0x08};
//可显示小数点数极限值							0位小数		1		2		3
const static QWORD DigTubNumRange[] = {999999999,99999999,9999999,999999};


//0~3 主OAD	4~7附OAD	8 OAD个数	9屏序号
static const BYTE LcdItemTable[200][10] =                
{
//	  0		1		2	3		4	5		6	7		8	9
	{0x00, 0x00, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第0项 不要设置此项！！！！！！
	{0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第1项	40000200--当前日期
	{0X40, 0X00, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第2项	40000200--当前时间
	{0x20, 0x2C, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第3项 	202c0201--当前剩余金额
	{0X00, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第4项	00000201--当前组合有功总电量
	{0X00, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第5项	00100201--当前正向有功总电量
	{0X00, 0X10, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第6项	00100202--当前正向有功尖电量
	{0X00, 0X10, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第7项	00100203--当前正向有功峰电量
	{0X00, 0X10, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第8项	00100204--当前正向有功平电量
	{0X00, 0X10, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第9项	00100205--当前正向有功谷电量
	{0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第10项	10100201--当前正向有功总最大需量
	{0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第11项	10100201--当前正向有功总最大需量发生日期
	{0X10, 0X10, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03},//第12项	10100201--当前正向有功总最大需量发生时间
	{0X00, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第13项	00200201--当前反向有功总电量
	{0X00, 0X20, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第14项	00200202--当前反向有功尖电量
	{0X00, 0X20, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第15项	00200203--当前反向有功峰电量
	{0X00, 0X20, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第16项	00200204--当前反向有功平电量
	{0X00, 0X20, 0X02, 0X05, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第17项	00200205--当前反向有功谷电量
	{0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第18项	10200201--当前反向有功总最大需量
	{0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第19项	10200201--当前反向有功总最大需量发生日期
	{0X10, 0X20, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03},//第20项	10200201--当前反向有功总最大需量发生时间
	{0X00, 0X30, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第21项	00300201--当前组合无功1总电量
	{0X00, 0X40, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第22项	00400201--当前组合无功2总电量
	{0X00, 0X50, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第23项	00500201--当前第1象限无功总电量
	{0X00, 0X60, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第24项	00600201--当前第2象限无功总电量
	{0X00, 0X70, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第25项	00700201--当前第3象限无功总电量
	{0X00, 0X80, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第26项	00800201--当前第4象限无功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X01, 0x02, 0X00},//第27项	50050201 00100201--上1月正向有功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X02, 0x02, 0X00},//第28项	50050201 00100202--上1月正向有功尖电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X03, 0x02, 0X00},//第29项	50050201 00100203--上1月正向有功峰电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X04, 0x02, 0X00},//第30项	50050201 00100204--上1月正向有功平电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X10, 0X02, 0X05, 0x02, 0X00},//第31项	50050201 00100205--上1月正向有功谷电量
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X01},//第32项	50050201 10100201--上1月正向有功总最大需量
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X02},//第33项	50050201 10100201--上1月正向有功总最大需量发生日期
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X10, 0X02, 0X01, 0x02, 0X03},//第34项	50050201 10100201--上1月正向有功总最大需量发生时间
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X01, 0x02, 0X00},//第35项	50050201 00200201--上1月反向有功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X02, 0x02, 0X00},//第36项	50050201 00200202--上1月反向有功尖电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X03, 0x02, 0X00},//第37项	50050201 00200203--上1月反向有功峰电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X04, 0x02, 0X00},//第38项	50050201 00200204--上1月反向有功平电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X20, 0X02, 0X05, 0x02, 0X00},//第39项	50050201 00200205--上1月反向有功谷电量
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X01},//第40项	50050201 10200201--上1月反向有功总最大需量
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X02},//第41项	50050201 10200201--上1月反向有功总最大需量发生日期
	{0X50, 0X05, 0X02, 0X01, 0X10, 0X20, 0X02, 0X01, 0x02, 0X03},//第42项	50050201 10200201--上1月反向有功总最大需量发生时间
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X50, 0X02, 0X01, 0x02, 0X00},//第43项	50050201 00500201--上1月第1象限无功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X60, 0X02, 0X01, 0x02, 0X00},//第44项	50050201 00600201--上1月第2象限无功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X70, 0X02, 0X01, 0x02, 0X00},//第45项	50050201 00700201--上1月第3象限无功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X80, 0X02, 0X01, 0x02, 0X00},//第46项	50050201 00800201--上1月第4象限无功总电量
	{0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第47项	40010200--通信地址低8位
	{0X40, 0X01, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第48项	40010200--通信地址高4位
	{0XF2, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第49项	F2010201--通信波特率
	{0X41, 0X09, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第50项	41090200--有功脉冲常数
	{0X41, 0X0A, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第51项	410A0200--无功脉冲常数
	{0X20, 0X13, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第52项	20130200--时钟电池使用时间
	{0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X01},//第53项	30120201 33020202--最近一次编程日期
	{0X30, 0X12, 0X02, 0X01, 0X20, 0X1e, 0X02, 0X00, 0x02, 0X02},//第54项	30120201 33020202--最近一次编程时间
	{0X30, 0X00, 0X0D, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第55项	30000D01--总失压次数、对象增加属性12
	{0X30, 0X00, 0X0D, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第56项	30000D02--总失压累计时间、对象增加属性12
	{0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第57项	30000D03--最近一次失压起始日期
	{0X30, 0X00, 0X0D, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第58项	30000D03--最近一次失压起始时间
	{0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第59项	30000D04--最近一次失压结束日期
	{0X30, 0X00, 0X0D, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第60项	30000D04--最近一次失压结束时间
	{0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00},//第61项	30000701 00102201--最近一次A相失压起始时刻正向有功电量
	{0X30, 0X00, 0X07, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00},//第62项	30000701 00108201--最近一次A相失压结束时刻正向有功电量
	{0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00},//第63项	30000701 00202201--最近一次A相失压起始时刻反向有功电量
	{0X30, 0X00, 0X07, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00},//第64项	30000701 00208201--最近一次A相失压结束时刻反向有功电量
	{0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00},//第65项	30000801 00102201--最近一次B相失压起始时刻正向有功电量
	{0X30, 0X00, 0X08, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00},//第66项	30000801 00108201--最近一次B相失压结束时刻正向有功电量
	{0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00},//第67项	30000801 00202201--最近一次B相失压起始时刻反向有功电量
	{0X30, 0X00, 0X08, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00},//第68项	30000801 00208201--最近一次B相失压结束时刻反向有功电量
	{0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X22, 0X01, 0x02, 0X00},//第69项	30000901 00102201--最近一次C相失压起始时刻正向有功电量
	{0X30, 0X00, 0X09, 0X01, 0X00, 0X10, 0X82, 0X01, 0x02, 0X00},//第70项	30000901 00108201--最近一次C相失压结束时刻正向有功电量
	{0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X22, 0X01, 0x02, 0X00},//第71项	30000901 00202201--最近一次C相失压起始时刻反向有功电量、
	{0X30, 0X00, 0X09, 0X01, 0X00, 0X20, 0X82, 0X01, 0x02, 0X00},//第72项	30000901 00208201--最近一次C相失压结束时刻反向有功电量
	{0X20, 0X00, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第73项	20000201--A相电压
	{0X20, 0X00, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第74项	20000202--B相电压
	{0X20, 0X00, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第75项	20000203--C相电压
	{0X20, 0X01, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第76项	20010201--A相电流
	{0X20, 0X01, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第77项	20010202--B相电流
	{0X20, 0X01, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第78项	20010203--C相电流
	{0X20, 0X04, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第79项	20040201--瞬时总有功功率
	{0X20, 0X04, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第80项	20040202--瞬时A相有功功率
	{0X20, 0X04, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第81项	20040203--瞬时B相有功功率
	{0X20, 0X04, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第82项	20040204--瞬时C相有功功率
	{0X20, 0X0A, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第83项	200A0201--瞬时总功率因数
	{0X20, 0X0A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第84项	200A0202--瞬时A相功率因数
	{0X20, 0X0A, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第85项	200A0203--瞬时B相功率因数
	{0X20, 0X0A, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第86项	200A0204--瞬时C相功率因数	
	{0X40, 0X18, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第87项	40180201--当前尖费率电价
	{0X40, 0X18, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第88项	40180202--当前峰费率电价
	{0X40, 0X18, 0X02, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第89项	40180203--当前平费率电价
	{0X40, 0X18, 0X02, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第90项	40180204--当前谷费率电价
	{0X40, 0X1A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第91项	401A0202--阶梯1电价
	{0X40, 0X1A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第92项	401A0202--阶梯2电价
	{0X40, 0X1A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03},//第93项	401A0202--阶梯3电价
	{0X40, 0X1A, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X04},//第94项	401A0202--阶梯4电价
	{0X20, 0X1A, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第95项	201A0200--当前电价
	{0X40, 0X1E, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第96项	401E0201--报警金额1
	{0X40, 0X1E, 0X02, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第97项	401E0202--报警金额2
	{0X20, 0X2D, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第98项	401F0201--透支金额
	{0X41, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第99项	41160201--结算日
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第100项 40160201--时段参数1
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第101项 40160201--时段参数2
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X03},//第102项 40160201--时段参数3
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X04},//第103项 40160201--时段参数4
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X05},//第104项 40160201--时段参数5
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X06},//第105项 40160201--时段参数6
	{0X40, 0X16, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X07},//第106项 40160201--时段参数7
	{0X30, 0X00, 0X0A, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第107项 30000A02--A失压次数
	{0X30, 0X00, 0X0A, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第108项 30000A03--B失压次数
	{0X30, 0X00, 0X0A, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第109项 30000A04--C失压次数
	{0X30, 0X04, 0X0A, 0X02, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第110项 30040A02--A失流次数
	{0X30, 0X04, 0X0A, 0X03, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第111项 30040A03--B失流次数
	{0X30, 0X04, 0X0A, 0X04, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第112项 30040A04--C失流次数
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X10, 0X02, 0X01, 0x02, 0X00},//第113项 50050202 00100201--上2月正向有功总电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X10, 0X02, 0X02, 0x02, 0X00},//第114项 50050202 00100202--上2月正向有功尖电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X10, 0X02, 0X03, 0x02, 0X00},//第115项 50050202 00100203--上2月正向有功峰电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X10, 0X02, 0X04, 0x02, 0X00},//第116项 50050202 00100204--上2月正向有功平电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X10, 0X02, 0X05, 0x02, 0X00},//第117项 50050202 00100205--上2月正向有功谷电量
	{0X50, 0X05, 0X02, 0X02, 0X10, 0X10, 0X02, 0X01, 0x02, 0X01},//第118项 50050202 10100201--上2月正向有功总最大需量
	{0X50, 0X05, 0X02, 0X02, 0X10, 0X10, 0X02, 0X01, 0x02, 0X02},//第119项 50050202 10100201--上2月正向有功总最大需量发生日期
	{0X50, 0X05, 0X02, 0X02, 0X10, 0X10, 0X02, 0X01, 0x02, 0X03},//第120项 50050202 10100201--上2月正向有功总最大需量发生时间
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X20, 0X02, 0X01, 0x02, 0X00},//第121项 50050202 00200201--上2月反向有功总电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X20, 0X02, 0X02, 0x02, 0X00},//第122项 50050202 00200202--上2月反向有功尖电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X20, 0X02, 0X03, 0x02, 0X00},//第123项 50050202 00200203--上2月反向有功峰电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X20, 0X02, 0X04, 0x02, 0X00},//第124项 50050202 00200204--上2月反向有功平电量
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X20, 0X02, 0X05, 0x02, 0X00},//第125项 50050202 00200205--上2月反向有功谷电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X30, 0X02, 0X01, 0x02, 0X00},//第126项 50050101 00300201--上1月正向无功总电量 
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X30, 0X02, 0X01, 0x02, 0X00},//第127项 50050202 00300201--上2月正向无功总电量
	{0X50, 0X05, 0X02, 0X01, 0X00, 0X40, 0X02, 0X01, 0x02, 0X00},//第128项 50050101 00400201--上1月反向无功总电量 
	{0X50, 0X05, 0X02, 0X02, 0X00, 0X40, 0X02, 0X01, 0x02, 0X00},//第129项 50050202 00400201--上2月反向无功总电量
	{0x20, 0x01, 0x04, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第130项 40020200--零线电流
	{0x20, 0x01, 0x06, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第131项 40020200--零序电流
	{0X40, 0X02, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第132项	40020200--表号低8位
	{0X40, 0X02, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第133项	40020200--表号低8位
	{0X40, 0X03, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X02},//第134项	40030200--客户编号低8位
	{0X40, 0X03, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X01},//第135项	40030200--客户编号高4位
	{0X00, 0X11, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第136项	00110201--当前A 相正向有功电能
	{0X00, 0X12, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第137项	00120201--当前B 相正向有功电能
	{0X00, 0X13, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第138项	00130201--当前C 相正向有功电能
	{0X00, 0X21, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第139项	00210201--当前A 相反向有功电能
	{0X00, 0X22, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第140项	00220201--当前B 相反向有功电能
	{0X00, 0X23, 0X02, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第141项	00230201--当前C 相反向有功电能
	{0X20, 0X31, 0X02, 0X00, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第142项	20310201 --当前月度组合有功总累计用电量
	//以下为高精度电
	{0X00, 0X11, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第143项	00110401--当前A 相正向有功高精度电量
	{0X00, 0X12, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第144项	00120401--当前B 相正向有功高精度电量
	{0X00, 0X13, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第145项	00130401--当前C 相正向有功高精度电量
	{0X00, 0X21, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第146项	00210401--当前A 相反向有功高精度电量
	{0X00, 0X22, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第147项	00220401--当前B 相反向有功高精度电量
	{0X00, 0X23, 0X04, 0X01, 0Xff, 0Xff, 0Xff, 0Xff, 0x01, 0X00},//第148项	00230401--当前C 相反向有功高精度电量
	{0x00, 0x00, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第149项 00000401--当前组合有功总高精度电量
	{0x00, 0x10, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第150项 00100401--当前正向有功总高精度电量
	{0x00, 0x10, 0x04, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第151项 00100402--当前正向有功尖高精度电量
	{0x00, 0x10, 0x04, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第152项 00100403--当前正向有功峰高精度电量
	{0x00, 0x10, 0x04, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第153项 00100404--当前正向有功平高精度电量
	{0x00, 0x10, 0x04, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第154项 00100405--当前正向有功谷高精度电量
	{0x00, 0x20, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第155项 00200401--当前反向有功总高精度电量
	{0x00, 0x20, 0x04, 0x02, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第156项 00200402--当前反向有功尖高精度电量
	{0x00, 0x20, 0x04, 0x03, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第157项 00200403--当前反向有功峰高精度电量
	{0x00, 0x20, 0x04, 0x04, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第158项 00200404--当前反向有功平高精度电量
	{0x00, 0x20, 0x04, 0x05, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第159项 00200405--当前反向有功谷高精度电量
	{0x00, 0x30, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第160项 00300401--当前组合无功1总高精度电量
	{0x00, 0x40, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第161项 00400401--当前组合无功2总高精度电量
	{0x00, 0x50, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第162项 00500401--当前第一象限无功总高精度电量
	{0x00, 0x60, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第163项 00600401--当前第二象限无功总高精度电量
	{0x00, 0x70, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第164项 00700401--当前第三象限无功总高精度电量
	{0x00, 0x80, 0x04, 0x01, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00},//第165项 00800401--当前第四象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x00, 0x04, 0x01, 0x02, 0x00},//第166项 50050201 00000401--上1月组合有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第167项 50050201 00100401--上1月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x02, 0x02, 0x00},//第168项 50050201 00100402--上1月正向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x03, 0x02, 0x00},//第169项 50050201 00100403--上1月正向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x04, 0x02, 0x00},//第170项 50050201 00100404--上1月正向有功平高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x10, 0x04, 0x05, 0x02, 0x00},//第171项 50050201 00100405--上1月正向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第172项 50050201 00200401--上1月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x02, 0x02, 0x00},//第173项 50050201 00200402--上1月反向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x03, 0x02, 0x00},//第174项 50050201 00200403--上1月反向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x04, 0x02, 0x00},//第175项 50050201 00200404--上1月反向有功平高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x20, 0x04, 0x05, 0x02, 0x00},//第176项 50050201 00200405--上1月反向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x30, 0x04, 0x01, 0x02, 0x00},//第177项 50050201 00300401--上1月组合无功1总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x40, 0x04, 0x01, 0x02, 0x00},//第178项 50050201 00400401--上1月组合无功2总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x50, 0x04, 0x01, 0x02, 0x00},//第179项 50050201 00500401--上1月第一象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x60, 0x04, 0x01, 0x02, 0x00},//第180项 50050201 00600401--上1月第二象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x70, 0x04, 0x01, 0x02, 0x00},//第181项 50050201 00700401--上1月第三象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x01, 0x00, 0x80, 0x04, 0x01, 0x02, 0x00},//第182项 50050201 00800401--上1月第四象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x00, 0x04, 0x01, 0x02, 0x00},//第183项 50050202 00000401--上2月组合有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x01, 0x02, 0x00},//第184项 50050202 00100401--上2月正向有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x02, 0x02, 0x00},//第185项 50050202 00100402--上2月正向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x03, 0x02, 0x00},//第186项 50050202 00100403--上2月正向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x04, 0x02, 0x00},//第187项 50050202 00100404--上2月正向有功平高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x10, 0x04, 0x05, 0x02, 0x00},//第188项 50050202 00100405--上2月正向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x01, 0x02, 0x00},//第189项 50050202 00200401--上2月反向有功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x02, 0x02, 0x00},//第190项 50050202 00200402--上2月反向有功尖高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x03, 0x02, 0x00},//第191项 50050202 00200403--上2月反向有功峰高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x04, 0x02, 0x00},//第192项 50050202 00200404--上2月反向有功平高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x20, 0x04, 0x05, 0x02, 0x00},//第193项 50050202 00200405--上2月反向有功谷高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x30, 0x04, 0x01, 0x02, 0x00},//第194项 50050202 00300401--上2月组合无功1总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x40, 0x04, 0x01, 0x02, 0x00},//第195项 50050202 00400401--上2月组合无功2总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x50, 0x04, 0x01, 0x02, 0x00},//第196项 50050202 00500401--上2月第一象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x60, 0x04, 0x01, 0x02, 0x00},//第197项 50050202 00600401--上2月第二象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x70, 0x04, 0x01, 0x02, 0x00},//第198项 50050202 00700401--上2月第三象限无功总高精度电量
	{0x50, 0x05, 0x02, 0x02, 0x00, 0x80, 0x04, 0x01, 0x02, 0x00},//第199项 50050202 00800401--上2月第四象限无功总高精度电量
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
	// -- 有功电能方向改变				err -- 45
	0xff,
	// -- 备用					err -- 46
	0xff,
	// -- 备用					err -- 47
	0xff,
	// -- 备用					err -- 48
	0xff,
	
	// -- 编程					err -- 49
	0xff,
	
	// -- 电表清零				err -- 50
	0xff,
	
	// -- 需量清零				err -- 51
	0xff,
	
	// -- 事件清零				err -- 52
	0xff,
	
	// -- 校时				err -- 53
	0xff,
	
	// -- 时段表编程				err -- 54
	0xff,
	
	// -- 时区表编程				err -- 55
	0xff,
	
	// -- 周休日编程				err -- 56
	0xff,
	
	// -- 节假日编程				err -- 57
	0xff,
	
	// -- 有功组合方式编程			err -- 58
	0xff,
	
	// -- 无功组合方式1编程 		err -- 59
	0xff,
	
	// -- 无功组合方式2编程 		err -- 60	      
	0xff,
	
	// -- 结算日编程				err -- 61
	0xff,
	
	// -- 费率参数表编程			err -- 62
	0xff,
	// -- 阶梯表编程				err -- 63
	0xff,
	// -- 密钥更新				err -- 64
	0xff,

};

//显示读数据使用的缓冲
BYTE lcd[LCD_BUF_LENGTH]; 

//报警模式字
TWarnDispPara LcdWarnModeWord;
//报警状态字
static BYTE LcdWarnStatusWord[MAX_WARN_ERR_NUM];
//备份当前报警继电器输出状态字
static BYTE WarnRelayStatusBackup[MAX_WARN_ERR_NUM];
//在清报警继电器输出时已经存在的报警项
static BYTE WarnRelayStatusAlready[MAX_WARN_ERR_NUM];
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
//10位数码管高两位
static BYTE DigTubHig = 0;
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
	for(i = 3; i < 11; i++)	//在中间显示8个“-”
	{
		DigTubCtrlShow(20,i);
	}	
	for(i = 12; i < 20; i++)
	{	//OAD置全F
		DigTubCtrlShow(15,i);
	}	
	DigTubCtrlShow(15,22);	//索引置ff	
	DigTubCtrlShow(15,23);
}
#endif

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
		else if(type == 0x06)//尾数电能
		{	
			//尾数电能不会超过100，bit7不为0就是负数
			if(TempSavData.b[0] & 0x80)
			{
				TempSavData.b[0] *= -1;
				SHOW_DATA_NEGATIVE; 				//LCD显示负号
			}
		}
	}

	//小数点数处理，把数据统一处理成小数点数为借位功能小数点数，方便后续借位功能处理
	if(type == 0x04) //高精度电能
	{
		tPoint1 = 4;	//冻结高精度电能为4位小数
	}
	else if(type == 0x06)  //电能尾数
	{
		//电能尾数小数点数不变，不进行借位处理
		memcpy(Buf,TempSavData.b,sizeof(TempSavData));//写回数据
		return;
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

	if(tPoint > 6)
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
	TTwoByteUnion DataUA,DataUB,DataUC;
	DWORD LimitVoltage = 0;
	
	api_GetRemoteData(PHASE_A + REMOTE_U, DATA_HEX, 1, 2, DataUA.b);
	api_GetRemoteData(PHASE_B + REMOTE_U, DATA_HEX, 1, 2, DataUB.b);
	api_GetRemoteData(PHASE_C + REMOTE_U, DATA_HEX, 1, 2, DataUC.b);
	
	if( SelSecPowerConst == YES )//有辅助电源，背光不进行电压判断
	{
		LimitVoltage = 0;
	}
	else
	{
		LimitVoltage = (DWORD)wStandardVoltageConst * 75 / 100;
	}
	
	if(    (LcdCtrl.Mode == eLcdPowerOnMode)
		|| (DataUA.w >= LimitVoltage)
		|| (DataUB.w >= LimitVoltage)
		|| (DataUC.w >= LimitVoltage) )
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
	if(Operation == READ)
	{
		Buf[0] = SignalIntensity;
	}
	else
	{
		SignalIntensity = Buf[0];
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
			LcdDispTimer.CommFlag[eIR] = 3;
			SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuRf);
			if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == TRUE ) 
			{
				IRBackLightTimer = ((FPara1->LCDPara.LoopDisplayTime)*(FPara1->LCDPara.DispItemNum[0])*2);
				if(IRBackLightTimer > 255)
				{
					IRBackLightTimer = 255;
				}
				SetLcdBackLightTimer( IRBackLightTimer );
			}
			break;
		case eCR:
		  	LcdDispTimer.CommFlag[eCR] = 3;
			SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommuCr);
			break;
		case eRS485_II:
	  		LcdDispTimer.CommFlag[eRS485_II] = 3;
			SetLcdStatusFlag(eLcdStatusSegFlag,eLcdCommu2_485);
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
//函数功能: 显示象限
//
//参数: 	Type[in]		0--当前运行象限显示	1--数据显示象限显示
//          Quadrant[in]	1-4 -- I-IV象限          	
//返回值:	无
//
//备注:
//-----------------------------------------------
static void DisplayQuadrant( BYTE Type, BYTE Quadrant )
{
	if( Type == 0 )
	{
		switch( Quadrant )
		{
		case 1:
			CURR_QUADRANT_I;
			break;
		case 2:
			CURR_QUADRANT_II;
			break;
		case 3:
			CURR_QUADRANT_III;
			break;
		case 4:
			CURR_QUADRANT_IV;
			break;
		default:
			CURR_QUADRANT_CLR;
			break;
		}
	}
	else
	{
		switch( Quadrant )
		{
		case 1:
			TEXT_QUADRANT_I;
			break;
		case 2:
			TEXT_QUADRANT_II;
			break;
		case 3:
			TEXT_QUADRANT_III;
			break;
		case 4:
			TEXT_QUADRANT_IV;
			break;
		default:
			break;
		}
	}
}

//-----------------------------------------------
//函数功能: 显示无线模块信息
//
//参数:		SignalInfo[in]:		信号强度1-4
//
//返回值:	无
//			
//备注: 
//-----------------------------------------------
static void DisplayWirelessInfo( BYTE SignalInfo )
{
	SIGNAL_NO;
	
	if( SignalInfo & 0x07 )
	{
		SIGNAL_INTENSITY_0;
	}
	
	switch( SignalInfo & 0x07 )		//无线信号强度
	{
	case 1:
		SIGNAL_INTENSITY_1;
		break;
	case 2:
		SIGNAL_INTENSITY_2;
		break;
	case 3:
		SIGNAL_INTENSITY_3;
		break;
	case 4:
		SIGNAL_INTENSITY_4;
		break;
	default:
		break;
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
//函数功能: 闪烁电压电流标志
//
//参数: 	Type[in]	0~2	Ua Ub Uc
//                  	3~5 Ia Ib Ic  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void LcdFlashUI( BYTE Type )
{
    switch( Type )
	{
	case 0:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			 SHOW_UA;
		}
		else
		{
			 HIDE_UA;
		}
		break;
	case 1:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			 SHOW_UB;
		}
		else
		{
			 HIDE_UB;
		}
		break;
	case 2:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			 SHOW_UC;
		}
		else
		{
			 HIDE_UC;
		}
		break;
	case 3:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			SHOW_IA_P;
		}
		else
		{
			HIDE_IA;
		}
		break;
	case 4:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			SHOW_IB_P;
		}
		else
		{
			HIDE_IB;
		}
		break;
	case 5:
		if( LcdFlashFlagHalfSec == TRUE )
		{
			SHOW_IC_P;
		}
		else
		{
			HIDE_IC;
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------
//函数功能: 判断三相电流全断流
//
//参数: 	无
//            
//返回值:	TRUE --	三相电流全断流
//			FALSE-- 三相电流不全断流
//备注:   
//-----------------------------------------------
static BOOL JudgeAllCurrentBreak(void)
{
	BYTE i, Flag = 0;

	for( i=0; i<3; i++ )
	{
		//如果三相电流都断流，则认为三相全无流
	  	#if( SEL_EVENT_BREAK_I == YES )
		if( api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_BREAK_I_A+i) ) )
		{
			Flag |= (0x01 << i);
		}
		#endif
	}

	if( Flag == 0x07 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------
//函数功能: 显示电压、电流实时状态
//
//参数: 	无
//            
//返回值:	无
//			
//备注:包括失压、失流、断相及实时电流方向（各相功率方向）Ua Ub Uc -Ia-Ib-Ic
//-----------------------------------------------
static void DisplayUIStatus(void)
{
	BYTE i;

	//清除电压电流状态
	HIDE_U_ALL;
	HIDE_I_ALL;

	//如果处于按键唤醒或红外唤醒状态下(低功耗)，不判断失压、失流、断相
	if(api_GetSysStatus(eSYS_LOW_POWER_WAKEUP) == TRUE) 
	{
		return;
	}
	//如果处于掉电，不判断失压、失流、断相
	//掉电且全失压，按照全失压显示
	if(	(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_POWER))
		&&(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V ) != TRUE))
	{
		return;
	}

	for(i=0; i<3; i++)
	{
		if( (MeterTypesConst==METER_3P3W) && (i==1) )
		{
			continue;
		}

		#if( (SEL_EVENT_BREAK==YES) && (SEL_EVENT_LOST_ALL_V==YES) )
		if( 	(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_BREAK_A+i) ) 
			&& 	(api_DealEventStatus(eGET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V ) != TRUE) )
		{
			continue;
		}
		else
		{
			//失压或全失压
			#if( SEL_EVENT_LOST_V == YES )
			if( 	(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOSTV_A+i )) 
				|| 	(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_LOST_ALL_V )) )
			{
				LcdFlashUI(i);
			}
			else
			{
				if( i == 0 )
				{
					SHOW_UA;
				}
				else if( i == 1 )
				{
					SHOW_UB;
				}
				else if( i == 2 )
				{
					SHOW_UC;
				}
			}
			#endif

			if( JudgeAllCurrentBreak() == FALSE )	//判断是否三相电流全无
			{
				#if( SEL_EVENT_LOST_I == YES )
				if( api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_LOSTI_A+i) ) )	//失流
				{
					LcdFlashUI(3 + i);
				}
				else
				{
					#if( (SEL_EVENT_BREAK_I == YES) && (SEL_EVENT_BREAK == YES))
					if( api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_BREAK_I_A+i) )||api_DealEventStatus( eGET_EVENT_STATUS, (eSUB_EVENT_BREAK_A+i)))	//断流 或 断相 表示这相没有电流。不显示电流符号
					{
						continue;
					}
					#endif
					
					if( i == 0 )		//Ia
					{
						if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + i) == TRUE)
						{
							SHOW_IA_N;
						}
						else
						{
							SHOW_IA_P;
						}
					}
					else if( i == 1 )	//Ib
					{
						if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + i) == TRUE)
						{
							SHOW_IB_N;
						}
						else
						{
							SHOW_IB_P;
						}
					}
					else if( i == 2 )	//Ic
					{
						if(api_GetSysStatus(eSYS_STATUS_OPPOSITE_P_A + i) == TRUE)
						{
							SHOW_IC_N;
						}
						else
						{
							SHOW_IC_P;
						}
					}
				}
				#endif
			}
		}
		#endif
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
	case 15:
		SHOW_DOT_Demand;
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
	case eLcdUnit_KVARH:
		SHOW_UNIT_KVARH;
		break;
	case eLcdUnit_KW:
		SHOW_UNIT_KW;
		break;
	case eLcdUnit_KVAR:
		SHOW_UNIT_KVAR;
		break;
	case eLcdUnit_VA:
		SHOW_UNIT_VA;
		break;
	case eLcdUnit_V:
		SHOW_UNIT_V;
		break;
	case eLcdUnit_A:
		SHOW_UNIT_A;
		break;
	case eLcdUnit_KVAH:
		SHOW_UNIT_KVAH;
		break;
	case eLcdUnit_KVA:
		SHOW_UNIT_KVA;
		break;
	case eLcdUnit_AH:
		SHOW_UNIT_AH;
		break;
	case eLcdUnit_WAN:
		SHOW_UNIT_WAN;
		break;
	case eLcdUnit_YUAN:
		SHOW_UNIT_YUAN;
		break;
	case eLcdUnit_WANYUAN:		//13修订版新液晶删除了“万”，程序中没有调用
		SHOW_UNIT_WANYUAN;		//这个宏定义是个";"
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
		DigTubCtrlShow(TempCurLad,20);
	}
	else
	{
		LCD_CURR_LADDOR_L_CLR;
		CLRDigTub(20);
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

	//判断通讯状态(低功耗下支持显示)
	DisplayCommStatus();

	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )		    //电压正常
	{
		return;
	}	

	//厂内模式或红外认证通过
	if((api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE) 
	|| (api_GetSysStatus(eSYS_IR_ALLOW_PRG)==TRUE) 
	||(api_GetSysStatus(eSYS_STATUS_LOW_INSIDE_FACTORY)==TRUE) )
	{
	    if( api_GetSysStatus(eSYS_IR_ALLOW_PRG)==TRUE )//红外认证常显
	    {
	    	LCD_STATUS_ALLOC_PRG;
    		
	    }
	    else
	    {
	    	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE )//厂内模式1s闪烁
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
	    	else//低级厂内模式0.5s闪烁
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

	    }
	}
	else
	{
		LCD_STATUS_ALLOC_PRG_CLR;
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

	//判停电抄表电池
	if(api_GetRunHardFlag( eRUN_HARD_READ_BAT_LOW ) == TRUE)
	{
		LCD_STATUS_LOW_BATT2;
	}
	else
	{
		LCD_STATUS_LOW_BATT2_CLR;
	}

		
	//判逆相序     
	#if( (SEL_EVENT_V_REVERSAL==YES) && (SEL_EVENT_I_REVERSAL==YES) )
	if( 	(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_V_REVERSAL )) 
		|| 	(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_I_REVERSAL ))	) 
	{
		LCD_STATUS_PHASE_REVERSE;
	}
	else
	{
		LCD_STATUS_PHASE_REVERSE_CLR;
	}
	#endif

	//显示实时电压、电流状态
	DisplayUIStatus();

	//告警显示
	if(	GetLcdStatusFlag(eLcdStatusSegFlag,eLcdAlarm) == TRUE )
	{
		if( LcdFlashFlagHalfSec == TRUE )
		{
			LCD_STATUS_ALARM;
		}
		else
		{
			LCD_STATUS_ALARM_CLR;
		}
	}
	else
	{
		LCD_STATUS_ALARM_CLR;
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
	
	if( RelayTypeConst != RELAY_NO )
	{
		//判断液晶是否显示 "拉闸"
		Flag = api_GetRelayStatus( 0 );
		if( Flag == 1 ) //命令跳闸
		{
			LCD_STATUS_CUT;     // 显示系统跳闸标志
		}
		else if( Flag == 2 )    //命令跳闸延时或者大电流延时
		{
			SetLcdBackLightTimer( 2 );
			if( LcdFlashFlagSec == TRUE )
			{
				LCD_STATUS_CUT;
			}
			else
			{
				LCD_STATUS_CUT_CLR;
			}
		}
		else if( Flag == 0 )    //命令合闸
		{
			LCD_STATUS_CUT_CLR; // 清除显示系统跳闸标志
		}

		//跳闸灯，继电器实际状态
		Flag = api_GetRelayStatus( 1 );
		if( Flag == 0 ) //合闸状态
		{
			RELAY_LED_OFF;
		}
		else if( Flag == 1 )    //跳闸状态
		{
			RELAY_LED_ON;
		}
		else if( Flag == 2 )    //合闸允许状态
		{
			if( RelayTypeConst == RELAY_INSIDE )    //内置继电器
			{
				if( LcdFlashFlagSec == TRUE )
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

	//GPRS模块信号强度显示
	DisplayWirelessInfo( SignalIntensity );

	//显示当前运行费率
	DisplayRatio( 0, api_GetCurRatio() );

	//显示电表运行所在象限
	CURR_QUADRANT_CLR;
	if( api_GetSysStatus( eSYS_LOW_POWER_WAKEUP ) == FALSE)
	{
		CURR_QUADRANT_AXES;
		//显示当前运行象限
		DisplayQuadrant( 0, api_GetSampleStatus(0x30) );
	}
	
    #if( PREPAY_MODE == PREPAY_LOCAL )
    //当前运行的阶梯，当前阶梯电价套显示
    DispLadder();
    #endif//#if( PREPAY_MODE == PREPAY_LOCAL )

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
			SHOW_CHINESE_MON_NUM(Order);
		}
	}
}

//-----------------------------------------------
//函数功能: 显示相别
//
//参数: 	Phase[in]: 0~4 总/A/B/C/N
//			               
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void DisplayPhase( BYTE Phase )
{
	switch( Phase )
	{
	case 0:			//总
		SHOW_CHINESE_ZONG;
		break;
	case 1:			//A
		TEXT_PAHSE_A;
		break;
	case 2:			//B
		TEXT_PAHSE_B;
		break;
	case 3:			//C
		TEXT_PAHSE_C;
		break;
	case 4:			//N
		TEXT_PAHSE_N;
		break;
	default:
		break;
	}
}

//-----------------------------------------------
//函数功能: 显示主索引
//
//参数: 	Index[in]: OAD
//			               
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void DisplayIndex( DWORD Index )
{
	BYTE i;
	
	for( i=0; i<8; i++ )
	{
		DigTubCtrlShow((Index & 0x0f),(19-i));
		Index >>= 4;
	}
	SHOW_INDEX_POINT;
}

//-----------------------------------------------
//函数功能: 显示子索引
//
//参数: 	Index[in]: ScreenOrder
//			               
//返回值:	无
//			
//备注:
//-----------------------------------------------
static void DisplaySubIndex( BYTE SubIndex )
{
	if(SubIndex >= 99)
	{
		return;
	}
	
	SubIndex = lib_BBinToBCD(SubIndex);
	DigTubCtrlShow((SubIndex & 0x0f),23);
	DigTubCtrlShow(((SubIndex & 0xf0) >> 4),22);
}

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
    
		if((DataLength == 10)&&(i < 2))
		{
			j = (DigTubHig >> 4);
			DigTubHig <<= 4;
		}
		else
		{
	        // 取最高一位数字
	        j = (DataBuf.b[ (DataLength/2)-1-(i/2) ] >> 4);
			//左移4位，把低位数字移动到高位
            DataBuf.b[ (DataLength/2)-1-(i/2) ] <<= 4;
        }
        //到小数点前一位
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
    			continue;
    		}
    		else
    		{
    			k = 1;
    		}
    	}
    	if( k == 1 )
    	{
    	    if( i == 0 )	//第一位不为0
			{
                SHOW_DATA_NEGATIVE;		//显示最前面的负号
                break;
			}
			DigTubCtrlShow(20,(11-DataLength+i));
            break;
    	}
    }
}

//-----------------------------------------------
//函数功能: 显示液晶中部数字的子程序
//
//参数: 	PointNo[in]: 	小数点位数
//			DataLength[in]:	要显示的数字长度
//			DataType[in]:	DATA_BCD/DATA_HEX
//			Mode[in]:  		0不显示，1：全显      
//返回值:	无
//			
//备注:此函数如果显示 HEX 编码，必须设置成为全显模式(DataType=DATA_HEX,Mode=1)
//-----------------------------------------------
static void ShowData( BYTE PointNo, BYTE DataLength, BYTE DataType, BYTE Mode )
{
	BYTE i, j, k;
	
    //数字
    //是否甩掉高位零的标志，为“1”，显示
    k = 0;

    if( (DataLength == 0) || (DataLength == 2) || (DataLength == 4) 
    || (DataLength == 6) || (DataLength == 8) || (DataLength == 10))//数据长度必须为0、2、4、6、8
    {
        for(i=0; i<DataLength; i++)
        {
        	if((DataLength == 10)&&(i < 2))
        	{
        		//显示10位液晶数据高两位写DigTubHig，静态全局变量
        		j = (DigTubHig >> 4);
        		DigTubHig <<= 4;
        	}
        	else
        	{
	        	// 取最高一位数字
	        	j = (LcdDData.b[ (DataLength/2)-1-(i/2) ] >> 4);
	        	//把低位移动到高位，这里需要测试一下是否可行
        		LcdDData.b[(DataLength/2)-1-(i/2)] <<= 4;
        	}

        	if( Mode == 1 )
        	{
        		k = 1;
        	}
        	else
        	{
        		if( (i==(DataLength-1-PointNo)) && (k==0) )
        		{
        			k = 1;
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
        		}
        	}

        	if( DataType == DATA_BCD )
        	{
        		//所有符号整合到一个数组里面
        		//BCD码有“ ”、“_”、“-”的要求
        		if(j == 13)			//	“_”
        		{
        			j = 23;
        		}
        		else if(j ==14)		//	“-”
        		{
        			j = 20;
        		}
        		else if(j == 15)	//	“ ”
        		{	
        			j = 16;
        		}
				DigTubCtrlShow(j , (12 - DataLength + i));
        	}                         
        	else                      
        	{                         
				DigTubCtrlShow(j , (12 - DataLength + i));
        	}

        }

        DisplayPoint( PointNo );		// 显示小数点
    }
    else
    {
        ASSERT( 0, 0 );
    }
}

//-----------------------------------------------
//函数功能: 获取电表导致小铃铛闪烁的前三个错误编号
//
//参数: 无
//
//返回值: 前三个错误编码号
//
//备注: 如果存在，识别出来前三个错误编号，举例: 如果存在错误编号3, 15, 26，则显示" 03015026 "
//-----------------------------------------------
static DWORD GetFirstThreeLEDWarn(void)
{
	DWORD Temp;
	BYTE i,j,k;
	
	Temp = 0;
	k = 0;
	//根据系统报警状态判断是否需要光报警
	for(i=0;i<MAX_WARN_ERR_NUM;i++)
	{
		//判断光报警是否打开
		for(j=0;j<8;j++)
		{
			if( ( (LcdWarnStatusWord[i]&(0x01<<j)) & (LcdWarnModeWord.WarnLcdDisp[1][i]&(0x01<<j)) ) != 0 )
			{
				Temp += lib_BBinToBCD(i*8+j+1);
				k++;
				if(k == 3)
				{
					return Temp;
				}
				Temp <<= 12;
			}
		}
	}
	
	return Temp;
}

//-----------------------------------------------
//函数功能: 三相表双按键显示电表版本信息
//
//参数: 
//			Type[in]:	显示版本信息编号
//
//返回值:	无
//			
//备注: 
//-----------------------------------------------
static void DisplayVersion(BYTE Type)
{
	if( Type == 14 )
	{
		LcdDData.d = GetFirstThreeLEDWarn();
		ShowData( 0, 8, 0, 1 );
	}
	else
	{
		if(Type < MAX_VERSION)
		{
			LcdDData.w[0] = api_GetMeterVersion(Type);	//0xdf01;
			ShowData( 0, 4, 1, 1 );
		}
		else
		{
			LcdDData.d = 0xeeeeeeee;	//显示--------
			ShowData( 0, 8, 0, 0 );
		}
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
	|| (LcdItemOAD.MainOAD.w[1]==0x300b) || (LcdItemOAD.MainOAD.w[1]==0x303b) )
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
	DigTubHig = 0x00;

	if(LcdItemOAD.OADNum == 0x02)
	{
		tw = LcdItemOAD.SubOAD.w[1];
	}
	else
	{
		tw = LcdItemOAD.MainOAD.w[1];
	}
	//无功显示 "   组合 "
	if( (tw>=0x0030) && (tw<=0x0043) )
	{
		SHOW_CHINESE_ZUHE;
	}
	
	//显示"   有功"
	if( ((tw>=0x0000)&&(tw<=0x0023)) || ((tw>=0x0110)&&(tw <= 0x0403)) )
	{
		SHOW_CHINESE_YOUGONG;
	}

	//显示"   无功"
	if( (tw>=0x0030 ) && (tw<=0x0083) )
	{
		SHOW_CHINESE_WUGONG;
	}

	//显示"   正向"
	if( 	((tw>=0x0010)&&(tw<=0x0013)) 
		|| 	((tw>=0x0090)&&(tw<=0x0093))
		|| 	((tw>=0x0110)&&(tw<=0x0113)) 
		|| 	((tw>=0x0210)&&(tw<=0x0213))  )
	{
		SHOW_CHINESE_ZHENGXIANG;	
	}

	//显示"   反向"
	else if(	((tw>=0x0020)&&(tw<=0x0023)) 
		||	((tw>=0x00A0)&&(tw<=0x00A3))
		|| 	((tw>=0x0120)&&(tw<=0x0123)) 
		|| 	((tw>=0x0220)&&(tw<=0x0223))  )
	{
		SHOW_CHINESE_FANXIANG;
	}

	else if( ((tw>=0x0030)&&(tw<=0x0033)) || ((tw>=0x0050)&&(tw<=0x0053)) )	
	{
		TEXT_QUADRANT_I;	//I	
	}
	else if( ((tw>=0x0040)&&(tw<=0x0043)) || ((tw>=0x0060)&&(tw<=0x0063)) )	
	{
		TEXT_QUADRANT_II;	//II
	}
	else if( (tw>=0x0070) && (tw<=0x0073) )		
	{
		TEXT_QUADRANT_III;	//III
	}
	else if( (tw>=0x0080) && (tw<=0x0083) )		
	{
		TEXT_QUADRANT_IV;	//IV
	}

	//显示单位:   kWh  / kvarh  / kVAh
	if( ((tw>=0x0000)&&(tw<=0x0023)) || ((tw>=0x0110)&&(tw<=0x0503)) )
	{
		UnitIndex = eLcdUnit_KWH;
	}
	else if( (tw>=0x0030) && (tw<=0x0083) )
	{
		UnitIndex = eLcdUnit_KVARH;	
	}
	else if( (tw>=0x0090) && (tw<=0x00A3) )
	{
		UnitIndex = eLcdUnit_KVAH;
	}

	//显示总 A  / B  / C 总不显示
	tbTemp = tw%0x0010;
	if( (tbTemp!=0) && (tbTemp<0x04) )
	{
		DisplayPhase(tbTemp);
	}

	//显示费率 : 尖峰平谷
	if(LcdItemOAD.OADNum == 0x02)
	{
		tbTemp = LcdItemOAD.SubOAD.b[0];
	}
	else
	{
		tbTemp = LcdItemOAD.MainOAD.b[0];
	}
	if( (tbTemp>=0x01) && (tbTemp<=(MAX_RATIO+1)) )//根据698.45电能OAD，属性索引1--总 2--尖 3--峰 4--平 5--谷
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
			if(LcdItemOAD.SubOAD.b[1] == 0x06)  //电能尾数
			{
				tPoint = 6;	//电能尾数电能为6位小数
			}
		}
		//当前电能
		else
		{
			HandleEnergyData(LcdItemOAD.MainOAD, TempSavData.b);
			if(LcdItemOAD.MainOAD.b[1] == 0x06)  //电能尾数
			{
				tPoint = 6;	//电能尾数电能为6位小数
			}
		}
		//参数小数位0没必要借位
		//协议要求启用借位时所有电能显示全部借位相同位数
		if((tPoint != 0) && (tPoint != 6))
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
		DigTubHig = TempSavData.b[4];
		memcpy(LcdDData.b,TempSavData.b,4);
	}
	else
	{
		DigTubHig = 0x00;
		LcdDData.d = 0x00;
	}

	ShowData( tPoint, 10, 0, 0 );
	DisplayUnit( UnitIndex );
}

//-----------------------------------------------
//函数功能: 显示需量数据
//
//参数: 	无
//		                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
static void DisplayDemandData( void )
{	
	BYTE UnitIndex,i, tPoint, tLength, tMode;
	BYTE DataType;
	BYTE tb,tbTemp;
	WORD YearTemp;
	TFourByteUnion tOAD;
	BYTE Buf[20] = {0};

	UnitIndex = eLcdUnit_INVALID;
	tPoint = 0;
	tLength = 8;
	tMode = 0;
	
	if(LcdItemOAD.OADNum == 0x02)
	{
		tb = LcdItemOAD.SubOAD.b[3];
	}
	else
	{
		tb = LcdItemOAD.MainOAD.b[3];
	}
	
	//需量都是以10/11开头
	if( (tb!=0x10) && (tb!=0x11) )
	{
		return;
	}
	
	if(LcdItemOAD.OADNum == 0x02)
	{
		tb = LcdItemOAD.SubOAD.b[2];
	}
	else
	{
		tb = LcdItemOAD.MainOAD.b[2];
	}		
	
	//需量只能为总A/B/C相
	if( (tb&0x0f) > 0x03 )
	{
		return;
	}	
	
	if( (tb>=0x0030) && (tb<=0x0043) )
	{
	    SHOW_CHINESE_ZUHE;
	}

	//显示有功or  无功
	if( (tb>=0x10) && (tb<=0x23) )
	{
		SHOW_CHINESE_YOUGONG;
	}
	else if( (tb>=0x30) && (tb<=0x83) )
	{
		SHOW_CHINESE_WUGONG;
	}
	//显示正向or  反向
	if( ((tb>=0x10)&&(tb<=0x13)) || ((tb>=0x90)&&(tb<=0x93)) )
	{
		SHOW_CHINESE_ZHENGXIANG;
	}
	else if( ((tb>=0x20)&&(tb<=0x23)) || ((tb>=0xA0)&&(tb<=0xA3)) )
	{
		SHOW_CHINESE_FANXIANG;
	}
	
	if( ((tb>=0x30)&&(tb<=0x33)) || ((tb>=0x50)&&(tb<=0x53)) )	
	{
		TEXT_QUADRANT_I;	//I	
	}
	if( ((tb>=0x40)&&(tb<=0x43)) || ((tb>=0x60)&&(tb<=0x63)) )	
	{
		TEXT_QUADRANT_II;	//II
	}
	if( (tb>=0x70) && (tb<=0x73) )		
	{
		TEXT_QUADRANT_III;	//III
	}
	if( (tb>=0x80) && (tb<=0x83) )		
	{
		TEXT_QUADRANT_IV;	//IV
	}
	
	//显示总 A  / B  / C 总不显示
	tbTemp = tb%0x10;
	if( (tbTemp!=0) && (tbTemp<0x04) )
	{
		DisplayPhase(tbTemp);
	}

	//显示“需量”
	SHOW_CHINESE_XULIANG;

	//显示费率 : 尖峰平谷
	if(LcdItemOAD.OADNum == 0x02)
	{
		tbTemp = LcdItemOAD.SubOAD.b[0];
	}
	else
	{
		tbTemp = LcdItemOAD.MainOAD.b[0];
	}
	if( (tbTemp>=0x01) && (tbTemp<=(MAX_RATIO+1)) )//根据698.45电能OAD，属性索引1--总 2--尖 3--峰 4--平 5--谷
	{
		DisplayRatio( 1, tbTemp-1 );
	}

	if(LcdItemOAD.OADNum == 0x02)
	{
		GetDispRecordData(sizeof(Buf), Buf);
	}
	else
	{
		SHOW_CHINESE_DANGQIAN;
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
		api_GetProOadData( eGetNormalData, eData, GetDemandDot(), tOAD.b, 0X00, sizeof(Buf), Buf );
	}
	DataType = DATA_BCD;
	{
		if( LcdItemOAD.ScreenOrder == 0x01 )
		{
			if( (tb>=0x10) && (tb<=0x23) )
			{
				UnitIndex = eLcdUnit_KW;
			}
			else if( (tb>=0x30) && (tb<=0x83) )
			{
				UnitIndex = eLcdUnit_KVAR;
			}
			else if( (tb>=0x90) && (tb<=0xA3) )
			{
				UnitIndex = eLcdUnit_KVA;	
			}
		}
		
		if(LcdItemOAD.OADNum == 0x02)
		{
			if( LcdItemOAD.ScreenOrder == 0x01 )//上一最大需量
			{
				memcpy( LcdDData.b, Buf, 4 );			
				if( LcdDData.d == 0xffffffff )
				{
					DataType = DATA_HEX;
				}
				else
				{	
					//组合无功1,2最大需量、冻结周期内组合无功1,2最大需量
					if( (LcdItemOAD.SubOAD.w[1] == 0x1030) || (LcdItemOAD.SubOAD.w[1] == 0x1040)
					|| (LcdItemOAD.SubOAD.w[1] == 0x1130) || (LcdItemOAD.SubOAD.w[1] == 0x1140) )
					{
						if(LcdDData.l < 0)
						{
							LcdDData.l *= -1;
							SHOW_DATA_NEGATIVE;
						}
					}
					LcdDData.d = lib_DWBinToBCD(LcdDData.d);							
				}

				tPoint = 4;
				tLength = 8;
			}
			else//上一月最大需量发生时间和日期
			{
				//显示“时间” 
				SHOW_CHINESE_SHIJIAN;

				switch(LcdItemOAD.ScreenOrder)
				{
				case 0x02:	//显示日期(年月日)
					YearTemp = (WORD)(Buf[4]) + (WORD)(Buf[5]) * (WORD)(0x100);
					if( (YearTemp == 0xffff)&&(Buf[6] == 0xff)&&(Buf[7] == 0xff) )
					{
						LcdDData.d = 0xffffffff;
						DataType = DATA_HEX;
					}
					else
					{		
						Buf[4] = YearTemp /100;
						Buf[5] = YearTemp%100;	
	
						memcpy(LcdDData.b, Buf+5, 3);

						for( i=0; i<4; i++ )
						{
							LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
						}
						lib_InverseData( LcdDData.b , 3);
					}
					
					tPoint = 11;
					tLength = 6;
					tMode = 1;
					break;
				case 0x03:	//显示时间(时分)
					memcpy(LcdDData.b, Buf+8, 2);
					if( (LcdDData.d & 0xffffff) == 0xffffff )
					{
						DataType = DATA_HEX;
					}
					else
					{	
						for( i=0; i<2; i++ )
						{
							LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
						}
						lib_InverseData( LcdDData.b , 2);
					}
					
					tPoint = 15;
					tLength = 4;
					tMode = 1;
					break;
				}
			}
		}
		else//显示当前最大需量 / 发生时间
		{
			if( LcdItemOAD.ScreenOrder == 0x01 )	//当前最大需量
			{
				memcpy( LcdDData.b, Buf, 4 );
				
				//组合无功1,2最大需量、冻结周期内组合无功1,2最大需量
				if( (LcdItemOAD.MainOAD.w[1] == 0x1030) || (LcdItemOAD.MainOAD.w[1] == 0x1040)
				|| (LcdItemOAD.MainOAD.w[1] == 0x1130) || (LcdItemOAD.MainOAD.w[1] == 0x1140) ) 
				{
					if(LcdDData.l < 0)
					{
						LcdDData.l *= -1;
						SHOW_DATA_NEGATIVE;
					}
				}
				
				LcdDData.d = lib_DWBinToBCD(LcdDData.d);

				tPoint = GetDemandDot();
				tLength = 8;
			}
			else//当前最大需量发生时间和日期
			{
				//显示“时间” 
				SHOW_CHINESE_SHIJIAN;

				switch(LcdItemOAD.ScreenOrder)
				{
				case 0x02:		//日期(  年月日 )
					YearTemp = (WORD)(Buf[4]) + (WORD)(Buf[5]) * (WORD)(0x100);
					Buf[4] = YearTemp /100;
					Buf[5] = YearTemp%100;	
					memcpy(LcdDData.b, Buf+5, 3);
					tPoint = 11;

					for( i=0; i<4; i++ )
					{
						LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
					}
					
					lib_InverseData( LcdDData.b , 3);
					
					tLength = 6;
					tMode = 1;
					break;
				case 0x03:		//时间(  时分 )
					memcpy(LcdDData.b, Buf+8, 2);
					tPoint = 15;
					
					for( i=0; i<2; i++ )
					{
						LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
					}
					
					lib_InverseData( LcdDData.b , 2);
					
					tLength = 4;
					tMode = 1;
				}
			}
		}
		
		ShowData( tPoint, tLength, DataType, tMode );		
	}	
	
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
	BYTE UnitIndex,i, tPoint, tLength, tMode, DataType, tbPhase,Mon = 1;
	WORD tw,ReturnLen;
	TFourByteUnion tOAD;
	BYTE Buf[20];
	BYTE SignIndex,SignFlag;
	
	UnitIndex = eLcdUnit_INVALID;
	tPoint = 0;
	tLength = 8;	
	tMode = 0;
	DataType = 0;	//0 正常数据  1 日期  2 时间
	//变量数据符号显示不同，新增符号索引和符号标志
	//SignIndex为1显示反向，为2显示负号，SignFlag为1表示负数，需要显示对应符号
	SignIndex = 0;
	SignFlag = 0;
	
	if(LcdItemOAD.OADNum == 0x02)
	{
		tw = LcdItemOAD.SubOAD.w[1];
		tbPhase = LcdItemOAD.SubOAD.b[0]; 
	}
	else
	{
		tw = LcdItemOAD.MainOAD.w[1];
		tbPhase = LcdItemOAD.MainOAD.b[0]; 
	}

	switch(tw)
	{
	case 0x2000:		//电压
	case 0x2002:		//电压相角
	case 0x2003:		//电压电流相角
		tPoint = 1;
		tLength = 4;
		if( tw == 0x2003 )
		{
			SHOW_CHINESE_ANGLE;
		}
		else if( tw == 0x2000 )
		{
			UnitIndex = eLcdUnit_V;
			SHOW_CHINESE_DIANYA;
		}
		else
		{}
		
		DisplayPhase( tbPhase );
		break;

	case 0x2001:		//电流
		tPoint = 3;
		tLength = 8;
		UnitIndex = eLcdUnit_A;
		SHOW_CHINESE_DIANLIU;
		SignIndex = 1;		//电流为负显示反向
	    if( ( LcdItemOAD.MainOAD.b[1] == 4 )
		|| ( LcdItemOAD.MainOAD.b[1] == 6 ))//显示零线电流
		{
            tbPhase = 4;
		}
		DisplayPhase( tbPhase );
		break;
		
	case 0x2004:		//有功功率
	case 0x2007:		//一分钟平均有功功率
	case 0x2005:		//无功功率
	case 0x2008:		//一分钟平均无功功率
	case 0x2006:		//视在功率
	case 0x2009:		//一分钟平均视在功率	
		tPoint = GetDemandDot();
		tLength = 8;
		if( (tw ==0x2004) || (tw ==0x2007) )
		{
			UnitIndex = eLcdUnit_KW;
			SHOW_CHINESE_YOUGONG;

		}
		else if( (tw ==0x2005) || (tw ==0x2008) )
		{
			UnitIndex = eLcdUnit_KVAR;
			SHOW_CHINESE_WUGONG;
		}
		else
		{
			UnitIndex = eLcdUnit_KVA;
		}
		SignIndex = 1;		//功率为负显示反向
		SHOW_CHINESE_GONGLV;
		DisplayPhase( tbPhase - 1);
		break;

	case 0x200A:		//功率因数
		tPoint = 3;
		tLength = 4;
		SignIndex = 1;		//功率因数为负显示反向
		SHOW_CHINESE_COS;
		DisplayPhase( tbPhase - 1);
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

	case 0x2017:		//当前有功需量
	case 0x2018:		//当前无功需量
	case 0x2019:		//当前视在需量
		tPoint = GetDemandDot();
		tLength = 8;
		if( tw == 0x2017 )
		{
			UnitIndex = eLcdUnit_KW;
			SHOW_CHINESE_YOUGONG;

		}
		else if( tw == 0x2018 )
		{
			UnitIndex = eLcdUnit_KVAR;
			SHOW_CHINESE_WUGONG;
		}
		else
		{
			UnitIndex = eLcdUnit_KVA;
		}
		SignIndex = 2;		//需量为负显示负号
		SHOW_CHINESE_DANGQIAN;
		SHOW_CHINESE_XULIANG;
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
		tPoint  = 2;
		tLength = 10;
		UnitIndex = eLcdUnit_YUAN;
		//这里没有用到，但还是加上保持统一
		SignIndex = 2;		//剩余金额为负显示负号
		SHOW_CHINESE_SHENGYU;
		SHOW_CHINESE_JINE;
		break;
	case 0x202E:
		tPoint  = 2;
		tLength = 10;
		UnitIndex = eLcdUnit_YUAN;
		//这里没有用到，但还是加上保持统一
		SignIndex = 2;		//剩余金额为负显示负号
		SHOW_CHINESE_JINE;
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
		SHOW_CHINESE_YUE;
		SHOW_CHINESE_DIANLIANG;	//电量
		if(LcdItemOAD.OADNum == 0x01)//显示当前
		{
		    Mon = 1;
            SHOW_CHINESE_DANGQIAN;    
            SHOW_CHINESE_MON_NUM(Mon);     //显示上月的数字
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
		if(((tw >= 0x2004)&&(tw <= 0x2009))||((tw >= 0x2017)&&(tw <= 0x2019)))      //冻结中的功率和需量按照固定4位小数显示
		{
			tPoint = 4;
		}
		//冻结数据、事件记录发生记录数据
		ReturnLen = GetDispRecordData(sizeof(Buf), Buf);
		if( (LcdItemOAD.MainOAD.w[1]==0x5005) || (LcdItemOAD.MainOAD.w[1] == 0x5006))//月冻结与结算冻结显示月
		{
			SHOW_CHINESE_YUE;
		}
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
		if( DataType == 0 )	//正常数据
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
			
			if(tLength == 10)
			{
				DigTubHig = LcdDData.d/100000000;
			}
			LcdDData.d = lib_DWBinToBCD(LcdDData.d);
		}
		else if( (DataType == 1) || (DataType == 2) )		//日期
		{
			tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
			Buf[0] = tw /100;
			Buf[1] = tw%100;
			if( DataType == 1 )
			{
				memcpy(LcdDData.b, Buf+1, 3);
			}
			else
			{
				memcpy(LcdDData.b, Buf+4, 3);
			}

			for( i=0; i<4; i++ )
			{
				LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
			}
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
			SHOW_CHINESE_YOUGONG;
			//SHOW_CHINESE_CHANGSHU;
			break;
			
		case 0x410A:	//电表无功常数
			tPoint = 0;
			tLength = 8;
			SHOW_CHINESE_WUGONG;
			//SHOW_CHINESE_CHANGSHU;
			break;
	
		case 0x400C:	//时区时段数，
		case 0x400D:	//阶梯数，
		case 0x400E:	//谐波分析次数，
		case 0x400F:	//密钥总条数,
		case 0x4010:	//计量元件数, 
		case 0x4013:	//周休日釆用的日时段表号, 
		case 0x4100:	//最大需量周期, 
		case 0x4101:	//滑差时间, 
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
                SHOW_CHINESE_MON_NUM( LcdItemOAD.MainOAD.b[0] );
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
                SHOW_CHINESE_MON_NUM(LcdItemOAD.ScreenOrder);
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
			else if(DataType == 5)		//结算日
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
	BYTE DataType,i, UnitIndex, tPoint, tLength, tMode, DataPoint;
	WORD tw,ReturnLen;
	TFourByteUnion tOAD;
	BYTE Buf[20];
	
	DataType = 0;
	UnitIndex = eLcdUnit_INVALID;
	tPoint = 0;
	tLength = 8;
	tMode = 0;	
	DataPoint = 0;	
	
	if(LcdItemOAD.OADNum == 1)
	{
		DataType = 0;			//正常数据
		tw = LcdItemOAD.MainOAD.w[1];

		if( (( tw >=0x3000) && ( tw <= 0x3008)) 
		|| ( tw == 0x300b) || ( tw == 0x303b )) //class_id=24
		{
			if(LcdItemOAD.MainOAD.b[1] == 0x0A)//属性10．当前值记录表
			{
				switch(LcdItemOAD.MainOAD.b[0])
				{
				case 0X01://事件记录表1
					if (LcdItemOAD.ScreenOrder==1)//事件发生次数   double-long-unsigned
					{
						tLength = 8;
						SHOW_CHINESE_ZONG;
					}
					else if(LcdItemOAD.ScreenOrder==2)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						tLength = 8;
						SHOW_CHINESE_ZONG;
						SHOW_CHINESE_SHIJIAN;
					}
					break;
				case 0X02://事件记录表2
					if (LcdItemOAD.ScreenOrder==1)//事件发生次数   double-long-unsigned
					{
						tLength = 8;
						TEXT_PAHSE_A;
					}
					else if(LcdItemOAD.ScreenOrder==2)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						tLength = 8;
						TEXT_PAHSE_A;
						SHOW_CHINESE_SHIJIAN;
					}
					break;
				case 0X03://事件记录表3
					if (LcdItemOAD.ScreenOrder==1)//事件发生次数   double-long-unsigned
					{
						tLength = 8;
						TEXT_PAHSE_B;
					}
					else if(LcdItemOAD.ScreenOrder==2)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						tLength = 8;
						TEXT_PAHSE_B;
						SHOW_CHINESE_SHIJIAN;
					}
					break;
				case 0X04://事件记录表4
					if (LcdItemOAD.ScreenOrder==1)//事件发生次数   double-long-unsigned
					{
						tLength = 8;
						TEXT_PAHSE_C;
					}
					else if(LcdItemOAD.ScreenOrder==2)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						tLength = 8;
						TEXT_PAHSE_C;
						SHOW_CHINESE_SHIJIAN;
					}
					break;
				default:
					break;
				}
			}
			
			if( tw == 0x3000)		//失压事件
			{
				SHOW_CHINESE_SHIYA;
				
				if(LcdItemOAD.MainOAD.b[1] == 0x05)		//属性5
				{
					switch(LcdItemOAD.MainOAD.b[0])		//元素索引
					{
					case 0x01:	//电压触发上限  long-unsigned（单位：V，换算：-1）
					case 0x02:	//电压恢复下限  long-unsigned（单位：V，换算：-1）
						tPoint = 1;
						DataPoint = 1;
						tLength = 4;
						UnitIndex = eLcdUnit_V;
						break;
					case 0x03:	//电流触发下限  double-long（单位：A，换算：-4）
						tPoint = 4;
						DataPoint = 4;
						tLength = 8;
						UnitIndex = eLcdUnit_A;	
						break;
					case 0x04:	//判定延时时间  unsigned（单位：s，换算：0）
						tLength = 8;
						break;
					default:
						break;
					}
				}
				else if(LcdItemOAD.MainOAD.b[1] == 0x0D)//属性13
				{
					switch(LcdItemOAD.MainOAD.b[0])
					{
					case 0x01:	//事件发生总次数   double-long-unsigned
						tLength = 8;
						SHOW_CHINESE_ZONG;
						break;
					case 0x02:	//事件总累计时间   double-long-unsigned（单位：秒，无换算）
						tLength = 8;
						SHOW_CHINESE_ZONG;
						SHOW_CHINESE_SHIJIAN;
						break;
					case 0x03:	//最近一次失压发生时间 date_time_s
					case 0x04:	//最近一次失压结束时间 date_time_s
						SHOW_CHINESE_ZONG;
						SHOW_CHINESE_SHIJIAN;
						ShowLcdHistory( 1, 1 );
						if(LcdItemOAD.ScreenOrder == 1)		//显示日期
						{
							DataType = 1;
							tPoint = 11;
							tLength = 6;
							tMode = 1;
						}
						else if(LcdItemOAD.ScreenOrder == 2)//显示时间
						{
							DataType = 2;
							tPoint = 10;
							tLength = 6;
							tMode = 1;
						}
						break;
					default:
						break;
					}
				}
			}
			else if( (tw==0x3001) || (tw==0x3002) )		//欠压事件,过压事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0501)	//电压触发上限  long-unsigned（单位：V，换算：-1）
				{
					tPoint = 1;
					DataPoint = 1;
					tLength = 4;
					UnitIndex = eLcdUnit_V;	
					SHOW_CHINESE_DIANYA;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0502)//判定延时时间  unsigned（单位：s，换算：0）
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			//断相事件,失流事件,断流事件
			else if( (tw==0x3003) || (tw==0x3004) || (tw==0x3006) )		
			{
				if( tw == 0x3004 )	//失流事件
				{
					SHOW_CHINESE_SHILIU;
				}
				
				if(LcdItemOAD.MainOAD.w[0] == 0x0501) 		//电压触发上限
				{
					tPoint = 1;
					DataPoint = 1;
					tLength = 4;
					UnitIndex = eLcdUnit_V;
					SHOW_CHINESE_DIANYA;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0502) 	//电流触发上限
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_A;
					SHOW_CHINESE_DIANLIU;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0503) 	//延时时间
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}
			}
			else if( tw == 0x3005 )		//过流事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0501)		//0501	电流触发下限
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_A;
					SHOW_CHINESE_DIANLIU;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0502)		//0502	延时时间
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			else if( (tw==0x3007) || (tw==0x3008) )
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0501) 	//0501	有功功率触发下限
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_KW;
					SHOW_CHINESE_YOUGONG;
					SHOW_CHINESE_GONGLV;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0502)		//0502	延时时间
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}	
			else if(tw == 0x300B)		//无功需量超限事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0501) 	
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_KVAR;	
					SHOW_CHINESE_WUGONG;
					SHOW_CHINESE_XULIANG;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0502)		
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			else
			{
			  if(LcdItemOAD.MainOAD.b[1] != 0x0A)
			  {
				UnitIndex = eLcdUnit_INVALID;
				tLength = 8;
				tPoint = 0;
				tMode = 0;
				DataPoint = 0;
			  }
			}
		}
		else//class_id=7
		{
			if(LcdItemOAD.MainOAD.b[1] == 0x07)//属性7
			{
				switch(LcdItemOAD.ScreenOrder)//屏序号
				{
				case 0x01:	//事件发生源
				case 0x02:	//事件发生次数   double-long-unsigned
					tLength = 8;
					break;
				case 0x03:	//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
					break;
				default:
					break;
				}
			}
			
			if(tw == 0x3009)		//正向有功需量超限事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0601) 	//0501	有功功率触发下限
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_KW;
					SHOW_CHINESE_ZHENGXIANG;
					SHOW_CHINESE_YOUGONG;
					SHOW_CHINESE_XULIANG;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0602)		//0502	延时时间
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			else if(tw == 0x300A)		//反向有功需量超限事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0601) 	
				{
					tPoint = 4;
					DataPoint = 4;
					tLength = 8;
					UnitIndex = eLcdUnit_KW;
					SHOW_CHINESE_FANXIANG;
					SHOW_CHINESE_YOUGONG;
					SHOW_CHINESE_XULIANG;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0602)		
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			else if(tw == 0x303B)		//功率因数超下限事件
			{
				if(LcdItemOAD.MainOAD.w[0] == 0x0601) 	
				{
					tPoint = 1;
					DataPoint = 1; 
					tLength = 4;
					SHOW_CHINESE_COS;
					SHOW_CHINESE_GONGLV;
				}
				else if(LcdItemOAD.MainOAD.w[0] == 0x0602)		
				{
					tLength = 8;
					SHOW_CHINESE_SHIJIAN;
				}			
			}
			else if( 	((tw>=0x300E)&&(tw<=0x3011)) 
					||	((tw>=0x302A)&&(tw<=0x302C)) 
			     	||	((tw>=0x302E)&&(tw<=0x3030))  )
			{		
				if(tw == 0x300F)
				{
					//电能表电压逆相序事件
					SHOW_CHINESE_DIANYA;
					LCD_STATUS_PHASE_REVERSE;
				}
				else if(tw == 0x3010)
				{
					//电能表电流逆相序事件
					SHOW_CHINESE_DIANLIU;
					LCD_STATUS_PHASE_REVERSE;
				}
				
				//属性6 :  判定延时（单位：s）
				//0x300E----0x3011
				//0x302A----0x302C
				//0x302E----0x3030
				if(LcdItemOAD.MainOAD.w[0] == 0x0601)
				{
					tLength = 8;
					tMode = 0;
					SHOW_CHINESE_SHIJIAN;
				}
			}		
			else if( (tw==0x301D ) || (tw==0x301E) || (tw==0x302D)  )
			{
				//判定延时时间  （单位：s）
				//0x301D    0x301E    0x302D
				if(LcdItemOAD.MainOAD.w[0] == 0x0602)
				{
					tLength = 8;
					tMode = 0;
					SHOW_CHINESE_SHIJIAN;
				}
			}
			else
			{
			  	if(LcdItemOAD.MainOAD.b[1] != 0x07)
				{
					UnitIndex = eLcdUnit_INVALID;
					tLength = 8;
					tPoint = 0;
					tMode = 0;
					DataPoint = 0;
				}
			}
		}

		//读取数据
		memset(Buf,0x00,sizeof(Buf));
		//规约接口函数OAD按照大端模式传输
		lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
		ReturnLen = api_GetProOadData( eGetNormalData, eData, DataPoint, tOAD.b, 0X00, sizeof(Buf), Buf );

		if( (ReturnLen!=0) && (ReturnLen!=0x8000) )
		{
			if(DataType == 0)		//正常数据
			{
				if( (ReturnLen>=1) && (ReturnLen<=4) )
				{
					memcpy( LcdDData.b, Buf, ReturnLen );
					LcdDData.d = lib_DWBinToBCD(LcdDData.d);
				}
				else if(ReturnLen==8)//class_id=24 属性10
				{
					if (LcdItemOAD.ScreenOrder==1)//事件发生次数   double-long-unsigned
					{
						memcpy( LcdDData.b, Buf, 4 );
						LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					}
					else if(LcdItemOAD.ScreenOrder==2)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						memcpy( LcdDData.b, Buf+4, 4 );
						LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					}
					else
					{
						LcdDData.d=0x00;
					}
				}
				else if(ReturnLen==9)//class_id=7 属性7
				{
					if (LcdItemOAD.ScreenOrder==1)//事件发生源
					{
						memcpy( LcdDData.b, Buf, 1 );
						LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					}
					if (LcdItemOAD.ScreenOrder==2)//事件发生次数   double-long-unsigned
					{
						memcpy( LcdDData.b, Buf+1, 4 );
						LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					}
					else if(LcdItemOAD.ScreenOrder==3)//事件总累计时间   double-long-unsigned（单位：秒，无换算）
					{
						memcpy( LcdDData.b, Buf+5, 4 );
						LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					}
					else
					{
						LcdDData.d=0x00;
					}
				}
				else
				{
					LcdDData.d=0x00;
				}
			}
			else if( (DataType == 1) || (DataType == 2) )		//日期
			{
				tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
				Buf[0] = tw /100;
				Buf[1] = tw%100;
				if( DataType == 1 )
				{
					memcpy(LcdDData.b, Buf+1, 3);
				}
				else
				{
					memcpy(LcdDData.b, Buf+4, 3);
				}

				for( i=0; i<4; i++ )
				{
					LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
				}
				lib_InverseData( LcdDData.b, 3);
			}
		}
		else
		{
            LcdDData.d = 0x00;
		}
		ShowData( tPoint, tLength, 0, tMode );
		DisplayUnit( UnitIndex );
	}
	else if(LcdItemOAD.OADNum == 2)
	{	
		tw = LcdItemOAD.SubOAD.w[1];
		
		//记录单元
		if( (tw>=0x3301) && (tw<=0x3311) )
		{
			if(LcdItemOAD.SubOAD.b[1] == 0x02)		//属性2
			{
				switch(LcdItemOAD.SubOAD.b[0])
				{
				case 0x02:	//事件发生时间  date_time_s
				case 0x03:	//事件结束时间  date_time_s
					if(LcdItemOAD.ScreenOrder == 1)	//显示日期
					{
						DataType = 1;
						tPoint = 11;
						tLength = 6;
						tMode = 1;
						SHOW_CHINESE_SHIJIAN;
					}
					else if(LcdItemOAD.ScreenOrder == 2)//显示时间
					{
						DataType = 2;
						tPoint = 10;
						tLength = 6;
						tMode = 1;
						SHOW_CHINESE_SHIJIAN;
					}
					break;
					
				default:
					DataType = 0;
					tPoint = 0;
					tLength = 8;
					break;
				}
			}
		}
		
		//电能表需量超限事件单元->超限期间需量最大值发生时间  date_time_s
		if(LcdItemOAD.SubOAD.d == 0x33080207)
		{
			if(LcdItemOAD.ScreenOrder == 1)		//显示日期
			{
				DataType = 1;
				tPoint = 11;
				tLength = 6;
				tMode = 1;
				SHOW_CHINESE_SHIJIAN;
			}
			else if(LcdItemOAD.ScreenOrder == 2)		//显示时间
			{
				DataType = 2;
				tPoint = 10;
				tLength = 6;
				tMode = 1;
				SHOW_CHINESE_SHIJIAN;
			}
		}

		//冻结数据、事件记录发生记录数据
		ReturnLen = GetDispRecordData(sizeof(Buf), Buf);		
		
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
			else if( (DataType == 1) || (DataType == 2) )		//日期
			{
				tw = (WORD)(Buf[0]) + (WORD)(Buf[1]) * (WORD)(0x100);
				Buf[0] = tw/100;
				Buf[1] = tw%100;
				if( DataType == 1 )
				{
					memcpy(LcdDData.b, Buf+1, 3);
				}
				else
				{
					memcpy(LcdDData.b, Buf+4, 3);
				}

				for( i=0; i<4; i++ )
				{
					LcdDData.b[i] = lib_BBinToBCD(LcdDData.b[i]);
				}
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
        if( LcdItemOAD.MainOAD.w[1] == 0x3000 )
        {
            SHOW_CHINESE_SHIYA;
        }
        else if( LcdItemOAD.MainOAD.w[1] == 0x3004 )
        {  
            SHOW_CHINESE_SHILIU;
        }

        if( LcdItemOAD.MainOAD.w[0] == 0x0701 )
        {
            TEXT_PAHSE_A;
        }
        else if( LcdItemOAD.MainOAD.w[0] == 0x0801 )
        {
            TEXT_PAHSE_B;
        }
        else if( LcdItemOAD.MainOAD.w[0] == 0x0901 )
        {
            TEXT_PAHSE_C;
        }
	}
	
	//月冻结与结算冻结显示月
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
	BYTE BaudRate=0xff;
	LcdDData.d = 0xeeeeeeee;
	 //判断OAD个数,读取是否为
	if((LcdItemOAD.OADNum==1) && ((LcdItemOAD.MainOAD.b[1]&0x1f) == 0x02))
	{
		switch(LcdItemOAD.MainOAD.w[1])
		{
			case 0xF201://RS485
				if(LcdItemOAD.MainOAD.b[0] == 0x01)
				{
					BaudRate=FPara2->CommPara.I485;
				}
				else if(LcdItemOAD.MainOAD.b[0] == 0x02)
				{
					BaudRate=FPara2->CommPara.II485;
				}
				else if(LcdItemOAD.MainOAD.b[0] == 0x00)
				{
					BaudRate=FPara2->CommPara.I485;
				}
				break;
			case 0xF202://红外
				if(LcdItemOAD.MainOAD.b[0] <= 0x01)
				{
					BaudRate=0x02;//红外波特率固定为1200
				}
				break;
			case 0xF209://载波
				if(LcdItemOAD.MainOAD.b[0] <= 0x01)
				{
					BaudRate=FPara2->CommPara.ComModule;
				}
				break;
			default:
				break;
		}
		//判断屏序号与数据长度正确返回且有数据
		if(BaudRate != 0xff)
		{
			switch(BaudRate&0x0F)
			{
				case 0:
					LcdDData.d = 0x0300;
					break;
				case 1:
					LcdDData.d = 0x0600;
					break;				
				case 2:
					LcdDData.d = 0x1200;
					break;				
				case 3:
					LcdDData.d = 0x2400;
					break;
				case 4:
					LcdDData.d = 0x4800;
					break;				
				case 5:
					LcdDData.d = 0x7200;
					break;
				case 6:
					LcdDData.d = 0x9600;
					break;
				case 7:
					LcdDData.d = 0x019200;
					break;	
				case 8:
					LcdDData.d = 0x038400;
					break;
				case 9:
					LcdDData.d = 0x057600;
					break;
				case 10:
					LcdDData.d = 0x115200;
				break;
				default:
					break;
			}
		}
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
		LcdDData.w[0] = lib_WBinToBCD(Index);
		//显示Err-XX
		DigTubCtrlShow(14,6);
		DigTubCtrlShow(21,7);
		DigTubCtrlShow(21,8);
		DigTubCtrlShow(20,9);
		ShowData( 0, 2, 0, 1 );
	}
	else
	{
		LcdDData.w[0] = lib_WBinToBCD(Index-0x100);
		LcdDData.w[0] += 0x100;

		//显示Err-XXXX
		DigTubCtrlShow(14,4);
		DigTubCtrlShow(21,5);
		DigTubCtrlShow(21,6);
		DigTubCtrlShow(20,7);
		ShowData( 0, 4, 0, 1 );
	}
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
	TFourByteUnion tOAD;
	#if( PREPAY_MODE == PREPAY_LOCAL )
	TFourByteUnion TmpMoney;
	#endif
	BYTE tPoint;

	//清除全局变量的数据
	LcdDData.d = 0x00;
	DigTubHig = 0x00;
	
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
            ShowNegative( LcdDData.d, 2, 8 );
        }
        SHOW_CHINESE_SHENGYU;
        SHOW_CHINESE_DIANFEI;
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
    #endif //#if( PREPAY_MODE == PREPAY_LOCAL )


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
		else if( (twMainOAD>=0x1010) && (twMainOAD<=0x11A3) )		//最大需量
		{
			DisplayDemandData();
		}
		else if( (twMainOAD>=0x2000) && (twMainOAD<=0x2506) )		//变量
		{
			#if( PREPAY_MODE == PREPAY_LOCAL )
			if( LcdItemOAD.MainOAD.d == 0x202C0201 )
			{
				//显示剩余金额
				SHOW_CHINESE_SHENGYU;
				SHOW_CHINESE_DIANFEI;
        
				tPoint = 2;
				DisplayUnit( eLcdUnit_YUAN );

				//规约接口函数OAD按照大端模式传输
				//如果剩余金额为负，则剩余金额显示0。
				Result = api_ReadPrePayPara(eRemainMoney,(BYTE *)TmpMoney.b);

				if( Result == 4 )
				{
					if( TmpMoney.l < 0 )	//剩余金额小于0时赋0
					{
						//等于0没必要转换BCD码
						LcdDData.d = 0;
					}
					else
					{
						LcdDData.d = lib_DWBinToBCD( TmpMoney.d );
						DigTubHig = lib_BBinToBCD((BYTE)(TmpMoney.d/100000000));
					}
				}
				else
				{
					LcdDData.d = 0xeeeeeeee;
					tPoint = 0;
				}
				ShowData( tPoint, 10, 0, 0 );
			}
			else if( LcdItemOAD.MainOAD.d == 0x202D0200 )
			{
				//显示透支金额
				SHOW_CHINESE_SHENGYU;
				SHOW_CHINESE_DIANFEI;

				//规约接口函数OAD按照大端模式传输
				lib_ExchangeData(tOAD.b,LcdItemOAD.MainOAD.b,4);
				Result = api_GetProOadData( eGetNormalData, eData, 2 , tOAD.b, 0X00, sizeof(LcdDData), LcdDData.b );
				if( Result != 0x8000 )
				{
					tPoint = 2;
					DisplayUnit( eLcdUnit_YUAN );
					DigTubHig = lib_BBinToBCD((BYTE)(LcdDData.d/100000000));
					LcdDData.d = lib_DWBinToBCD(LcdDData.d);
					if( LcdDData.d != 0 )
					{
						SHOW_DATA_NEGATIVE;
					}
				}
				else
				{
					LcdDData.d = 0xeeeeeeee;
					tPoint = 0;
				}
				ShowData( tPoint, 10, 0, 0 );
			}
			else
			#endif
			{
				DisplayVariableData();
			}
		}
		else if( (twMainOAD>=0x3000) && (twMainOAD<=0x3040) )		//事件记录
		{
			DisplayEventRecordData();
		}
		else if( (twMainOAD>=0x4000) && (twMainOAD<=0x4517) )		//参变量
		{
			DisplaySysParaData();
		}
		else if( (twMainOAD>=0xF200) && (twMainOAD<=0xF20B) )		//输入输出设备
		{
			DisplayOutputDeviceData();
		}
		else if(twMainOAD == 0xFF00) 
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
		if( (twMainOAD>=0x3000) && (twMainOAD<=0x3040) )		//事件
		{
			DisplayCharForEventOrFreeze(0);
			
			if( (twSubOAD>=0x0000) && (twSubOAD<=0x0503) )		//电能类
			{
				DisplayEnergyData();
			}
			else if( (twSubOAD>=0x1010) && (twSubOAD<=0x11A3) )	//最大需量
			{
				DisplayDemandData();
			}
			else if( (twSubOAD>=0x2000) && (twSubOAD<=0x2506) )	//变量
			{
				DisplayVariableData();
			}
			else if( (twSubOAD>=0x3300) && (twSubOAD<=0x3311) )	//事件记录单元，两个OAD的OI都是事件的
			{
				DisplayEventRecordData();
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
			else if( (twSubOAD>=0x1010) && (twSubOAD<=0x11A3) )	//最大需量
			{
				DisplayDemandData();
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
	
		DisplayIndex( LcdItemOAD.MainOAD.d );	
		DisplaySubIndex(LcdItemOAD.ScreenOrder);
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
				api_ReadFromContinueEEPRom(ItemAddr+AddrOffset, 10, DispOADInfo);					
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
				api_ReadFromContinueEEPRom(ItemAddr+AddrOffset, 10, DispOADInfo);		
				memcpy(LcdItemOAD.MainOAD.b,DispOADInfo,4);
				memcpy(LcdItemOAD.SubOAD.b,DispOADInfo+4,4);
				LcdItemOAD.OADNum = DispOADInfo[8];
				LcdItemOAD.ScreenOrder = DispOADInfo[9];
			}
			break;
		case eLcdUpDownKeyMode:
			LcdItemOAD.MainOAD.d = 0xFF000000+LcdCtrl.Screen-1;		
			LcdItemOAD.SubOAD.d = 0xFFFFFFFF;
			LcdItemOAD.OADNum = 0x01;
			LcdItemOAD.ScreenOrder = LcdCtrl.Screen;	
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
	if( Keys == eUpDownKey )	//双键
	{
		if( LcdCtrl.Mode != eLcdUpDownKeyMode )
		{
			LcdCtrl.Mode = eLcdUpDownKeyMode;
			LcdCtrl.Screen = 1;
			LcdCtrl.ErrNo = 0xffff;
		}
	}
	else	//上键 或 下键
	{
		if( (LcdCtrl.Mode!=eLcdKeyMode) && (LcdCtrl.Mode!=eLcdUpDownKeyMode) )
		{
			LcdCtrl.Mode = eLcdKeyMode;
			LcdCtrl.Screen = 0;
			LcdCtrl.ErrNo = 0;
		}
	}
	
	if( LcdCtrl.Mode == eLcdUpDownKeyMode )
	{
		KeyMaxNum = 16;
	}
	else
	{
		KeyMaxNum = FPara1->LCDPara.DispItemNum[1];
	}
	
	if( Keys == eUpKey )	//上翻键
	{
		if( LcdCtrl.Screen == 0 )
		{
			if( LcdCtrl.Mode == eLcdUpDownKeyMode )
			{
				LcdCtrl.Screen = 16;
				LcdCtrl.ErrNo = 0xffff;
			}
			else
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
		}
		else
		{
			LcdCtrl.Screen--;
			if( LcdCtrl.Screen == 0 )
			{
				if( LcdCtrl.Mode == eLcdUpDownKeyMode )
				{
					LcdCtrl.Screen = 16;
					LcdCtrl.ErrNo = 0xffff;
				}
				else
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
			if( LcdCtrl.Mode == eLcdUpDownKeyMode )
			{
				LcdCtrl.Screen = 1;
				LcdCtrl.ErrNo = 0xffff;
			}
			else
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
		}
		else
		{
			LcdCtrl.Screen++;
			if( LcdCtrl.Screen > KeyMaxNum )
			{
				if( LcdCtrl.Mode == eLcdUpDownKeyMode )
				{
					LcdCtrl.Screen = 1;
					LcdCtrl.ErrNo = 0xffff;
				}
				else
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
		//双键或者无效按键 无操作
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
	
	for(i=0;i<MAX_WARN_ERR_NUM;i++)
	{
		WarnRelayStatusAlready[i] = 0;
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
	
	KeyType = eKeyInValid;

	if( DOWN_KEY_PRESSED && (!UP_KEY_PRESSED) )	//下键
	{
		if(LcdKeyDetTimer.DownKey < 10)
		{
			LcdKeyDetTimer.DownKey++;
			if(LcdKeyDetTimer.DownKey == 4 )
			{
				KeyType = eDownKey;
			}
		}
	}	
	else if( UP_KEY_PRESSED && (!DOWN_KEY_PRESSED))	//上键
	{
		if(LcdKeyDetTimer.UpKey < 10)
		{
			LcdKeyDetTimer.UpKey++;
			if(LcdKeyDetTimer.UpKey == 4 )
			{
				KeyType = eUpKey;
			}
		}
	}	
	else if( UP_KEY_PRESSED && DOWN_KEY_PRESSED )	//双键
	{
		if(LcdKeyDetTimer.UpDownKey < 10)
		{
			LcdKeyDetTimer.UpDownKey++;
			if(LcdKeyDetTimer.UpDownKey == 4 )
			{
				KeyType = eUpDownKey;
			}			
		}
	}
	else if((!UP_KEY_PRESSED) && (!DOWN_KEY_PRESSED))	//没有按下按键
	{			
		LcdKeyDetTimer.DownKey = 0;
		LcdKeyDetTimer.UpKey = 0;	
		LcdKeyDetTimer.UpDownKey = 0;	
	}

	if( KeyType != eKeyInValid )
	{
		KeyProcess( KeyType );
		SetLcdBackLightTimer(FPara1->LCDPara.BackLightPressKeyTimer);
		Drv_OpenBKLight();
		for(i=0;i<MAX_WARN_ERR_NUM;i++)
		{
			WarnRelayStatusAlready[i] = 0;
		}
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
	|| (api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE ))
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


	//有功需量超限，正向、反向有功、无功
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
	if( RelayTypeConst != RELAY_NO )
	{
		if( api_GetRelayStatus(1) != 0 )
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
	BOOL LcdWarnRelayFlag,LcdBellFlag,LcdBackLightFlag;
	BOOL CurrErrFlag;
	BYTE TmpEventStatus;
	
	LcdWarnRelayFlag = FALSE;
	LcdBellFlag = FALSE;
	LcdBackLightFlag = FALSE;

	UpdateLcdWarnStatusWord();

	//根据系统报警状态判断是否需要光报警	
	for(i=0;i<MAX_WARN_ERR_NUM;i++)
	{	
		//报警继电器报警事件状态获取
		TmpEventStatus = LcdWarnStatusWord[i] & LcdWarnModeWord.WarnLcdDisp[0][i];
		//判断是否有新发生事件
		WarnRelayStatusAlready[i] |= (~WarnRelayStatusBackup[i]) & TmpEventStatus;
		//清除已结束事件状态字
		WarnRelayStatusAlready[i] &= TmpEventStatus ;
		//有新发生事件即报警继电器输出，不管事件是否已经结束，只有按键才能结束报警继电器输出
		if(WarnRelayStatusAlready[i])
		{
			LcdWarnRelayFlag = TRUE;
		}
		//备份当前状态，供下次判断是否有新发生事件用
		WarnRelayStatusBackup[i] = TmpEventStatus;
		
		//LCD小铃铛
		if( (LcdWarnStatusWord[i] & LcdWarnModeWord.WarnLcdDisp[1][i]) != 0 )
		{
			LcdBellFlag = TRUE;
		}
		//液晶背光
		if( (LcdWarnStatusWord[i] & LcdWarnModeWord.WarnLcdDisp[2][i]) != 0 )
		{
			LcdBackLightFlag = TRUE;
		}		
	}

	// 报警（远程报警状态）
	if( api_GetRelayStatus(3) == TRUE )
	{
		LcdWarnRelayFlag = TRUE;	//报警继电器
		LcdBellFlag = TRUE;			//需要 LCD小铃铛报警
	}
	
	//判断错误,若当前显示的错误号已消失，做一次按键处理
	CurrErrFlag = CheckLcdCurrentErrDisappear(LcdCtrl.ErrNo);
	if( (CurrErrFlag==TRUE) && (LcdCtrl.Screen==0) )
	{
		KeyProcess( eDownKey );
	}

	// 设置欠费标志，本地费控报警状态，对应 BUG_VERSION -- 6

	//操作报警继电器
	if(LcdWarnRelayFlag == TRUE)
	{		
		WARN_RELAY_OPEN;
	}
	else
	{
		WARN_RELAY_CLOSE;
	}

	//置小铃铛标志
	if( LcdBellFlag == TRUE )
	{
		SetLcdStatusFlag(eLcdStatusSegFlag,eLcdAlarm);
	}
	else
	{
		ClrLcdStatusFlag(eLcdStatusSegFlag,eLcdAlarm);
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
	Drv_CloseBKLight();

	LcdClearAll();
	DigTubCtrlShow(10,4);
	DigTubCtrlShow(13,5);
	DigTubCtrlShow(17,6);
	DigTubCtrlShow(18,7);
	DigTubCtrlShow( 5,8);
	DigTubCtrlShow(19,9);		//“A d J U S t”
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

	memset( lcd, 0x00, LCD_BUF_LENGTH );		//清除液晶显示缓存数组
	
	DigTubCtrlShow(10,4);	//显示“A”
	DigTubCtrlShow(20,5);	//显示“-”

	ShowData( 0, 6, 1, 1 );
	
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
	LcdCtrl.ErrNo = 0xffff;
	
	InitLCDDriver();
	CheckLcdRamPara();

	if(Type == ePowerOnMode)
	{
		RELAY_LED_ON;
		Drv_OpenBKLight();//背光点亮以前在主循环控制，上电会导致背光点亮慢2s 现在提前控制后，会导致背光熄灭慢2s左右-jwh-！！！！！！
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
//备注:  场内退出全显，进入键显，并且关闭背光灯
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
	ShutOff_Lcd();
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
		else if(  LoopItemDef[i] < 220  )
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
//函数功能: Lcd电表清零
//
//参数:		无
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_ClrLcd( BYTE Type )
{
	
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
			lcd[DigTubCtrl[0][DigTubNum][i]] |= DigTubCtrl[1][DigTubNum][i];
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

#endif//if( PREPAY_MODE == PREPAY_LOCAL )

#endif	//#if( LCD_TYPE == LCD_GW3PHASE_09 )

