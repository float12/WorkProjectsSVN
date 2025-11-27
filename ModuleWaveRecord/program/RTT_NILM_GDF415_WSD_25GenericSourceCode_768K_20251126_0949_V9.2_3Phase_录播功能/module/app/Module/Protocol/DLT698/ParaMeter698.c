//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//#include "task_vfrate.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern BYTE gDownFlagLen; //用于存储下载方标识，存储从flash读取出来的长度
extern BYTE gDownFlagFlash;//用于区分是否从flash读取的下载方式标识
//-----------------------------------------------
//				本文件使用的变量，常量

#define MAX_PARA_BUF_LENTH         4000//时段表源数据长度为最大数据长度

extern unsigned char devid[8];
extern TSafeModePara SafeModePara;
extern TReportMode gReportMode;
//extern VOL_QUAL_RATE VolHg;//zh
extern PQ_JZ_RATE PQ_Para;// ;
extern int gdwUartBps ;
extern BYTE IsUse5002;				//是否使用5002抄读4800
#if(PROTOCOL_VERSION == 25)
extern BYTE IsUse4801;				//是否使用309B抄读4801
extern BYTE IsUse4800;				//是否使用309A抄读4800
extern BYTE g_SpeDevSaveCount;		//用于存储特定设备保存事件次数
#endif
extern T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];
extern BYTE GetActiveFlag(void);
static const BYTE T_RemoteControl[]		= 
{ 
	Structure_698,				2,
	Long_unsigned_698,			2,//	Double_long_unsigned_698,	4,
		Long_unsigned_698,			2,
};

typedef struct TModulelist_t	//读取记录参数结构体
{
    char  Name[9];			//设备描述符
    BYTE  LogicAdrr;		//逻辑地址
    WORD  Class;			//模组类别
    BYTE  SerialNum[8];		//模组序列号
}TModulelistPara;
//不支持1字节对齐，不能直接使用
 TModulelistPara ModulelistPara=
{
	"type=NILM",
	0x03,
	0x0200,//实际对应bit1为1,:bit0--通信，bit1---计算，bit2--控制，bit3--存储，bit4--显示，其它位保留为0。
	{0x12,0x34,0x56,0x78,0x12,0x34,0x56,0x78},
};

static const BYTE T_Modulelist[]		= //模组列表
{ 
	Array_698,			1,
	Structure_698, 	 	5, 
	Visible_string_698,	9, 	//模组描述符
	Unsigned_698,		1,  //逻辑地址
	Bit_string_698,		16, //模组类别
	Octet_string_698,	8,  //模组序列号
	NULL_698,			1,
};


typedef struct TModuleVersion_t	//读取记录参数结构体
{
    BYTE  LogicAdrr;		//逻辑地址
    BYTE  Rsv1[4];			//厂商代码
	BYTE  Rsv2[16];			//软件版本号
	BYTE  Rsv3[6];			//软件版本日期
	BYTE  Rsv4[16];			//硬件版本号
	BYTE  Rsv5[6];			//硬件版本日期
	BYTE  Rsv6[16];			//厂商扩展信息
}TModuleVersion;
//不支持1字节对齐，不能直接使用
const TModuleVersion ModuleVersion=
{
	0x03,								//逻辑地址
	{0,0,0,0},                          //厂商代码
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //软件版本号
	{0,0,0,0,0,0},                      //软件版本日期
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //硬件版本号
	{0,0,0,0,0,0},                      //硬件版本日期
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //厂商扩展信息
};

//0x4031属性3的定义
static const BYTE T_ModuleVersion[]		= //模组列表
{ 
	Array_698,			1,
	Structure_698, 	 	7, 
	Unsigned_698,		1,  //逻辑地址
	Visible_string_698,	4,  //厂商代码
	Visible_string_698,	16, //软件版本号
	Visible_string_698,	6,  //软件版本日期
	Visible_string_698,	16, //硬件版本号
	Visible_string_698,	6,  //硬件版本日期
	Visible_string_698,	16, //厂商扩展信息
};
	
typedef struct TDevice_t	//读取记录参数结构体
{

    BYTE  Factory[4];			//厂商代码
	BYTE  SoftVer[8];			//软件版本
	BYTE  SoftTime[6];			//软件时间
	BYTE  HardVer[8];			//硬件版本
	BYTE  HardTime[6];			//硬件时间
	BYTE  FactoryEx[8];			//厂商扩展
}TDevice;
//不支持1字节对齐，不能直接使用
const TDevice DeviceVersion=
{
#if(USER_TEST_MODE)
	{'X','X','X',0},                          //
#else
#if(SEL_WSD_MODE)
	{'0','0','6','2'},							//wsd是0062
#else
	{'0','0','0','0'},							//鲁软是0000
#endif
#endif
        
	{0,0,0,0},							//
	{0,0,0,0,0,0},                      //
	{0,0,0,0},  //
	{0,0,0,0,0,0},                      //
	{0,0,0,0,0,0,0,0},  //
};

typedef struct wsdTDevice_t	//读取记录参数结构体
{

    BYTE  Factory[4];			//厂商代码
	BYTE  SoftVer[8];			//软件版本
	BYTE  SoftTime[6];			//软件时间
	BYTE  HardVer[8];			//硬件版本
	BYTE  HardTime[6];			//硬件时间
	BYTE  FactoryEx[8];			//厂商扩展
}wsdTDevice;

const wsdTDevice gDefaultPara=
{
#if(USER_TEST_MODE)
	{'X','X','X',0},                          //
#else

#if(SEL_WSD_MODE)
	{'0','0','6','2'},							//wsd是0062
#else
	{'0','0','0','0'},							//鲁软是0000
#endif
#endif
        
	{'v','1','.','1','.','0','0','1'},				//
	{'2','5','1','1','0','6'},                      //
	{'v','1','.','1','.','0','0','1'},				//
	{'2','4','0','8','3','1'},                      //
	{0,0,0,0,0,0,0,0},  //
};

static const BYTE T_Device[]		= //设备列表
{ 
	Structure_698, 	 	6, 
	
	Visible_string_698,	4,		//
	Visible_string_698,	4,		//
	Visible_string_698,	6,		//
	Visible_string_698,	4,		//
	Visible_string_698,	6,		//
	Visible_string_698,	8,		//
};

//static const BYTE T_File[]		= //设备列表
//{ 
//	Structure_698, 	 	6, 
//	
//	Visible_string_698,	4,		//
//	Visible_string_698,	4,		//
//	Visible_string_698,	6,		//
//	Visible_string_698,	4,		//
//	Visible_string_698,	6,		//
//	Visible_string_698,	8,		//
//};
static const BYTE T_FileStatus[]		= //
{ 
	Visible_string_698,	32, //软件版本号
		
//	Octet_string_698,	1,		//

};

static BYTE T_FileVer[]		= //
{ 
	Visible_string_698,	5, //软件版本号
		
		//	Octet_string_698,	1,		//
		
};

typedef struct TSoftCheckFlag_t	//读取记录参数结构体
{
    BYTE  SoftFlag[4];			//软件标识
    BYTE  SoftCheck[10];		//动态校验码
    BYTE  ModuleFlag[24];		//模组标识 //王选友 2023.4.6 10:20 根据国网送检公告修改
}TSoftCheckFlag;

#if(MAX_PHASE == 1)
//电科院用户
TSoftCheckFlag SoftCheckFlag=
{
	//"FF000000",属性5,软件标识
	{0xFF,0x00,0x00,0x00},
	{0x98,0x76,0x54,0x32,0x10,0xFF,0x08,0x14,0x03,0xB6},
	//"9876543210081403B6,,属性6，动态校验
	{0x01,0x02,0x9C,0x01,0xC1,0xFB,  0x60,    0x00,0x00,   0x00,0x00,    0x98,0x76,0x54,0x32,0x10,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	//"01029C01C1FB600000000098765432100000000000000000},,属性7，模组标识/硬件标识
	// 附加信息字段                 设备类别    硬件厂商      模组类型       序列号                        防伪校验码
};

//非电科院用户
TSoftCheckFlag SoftCheckFlag_bak=
{
	//"FF000000",属性5,软件标识
	{0xFF,0x00,0x00,0x00},
	{0x98,0x76,0x54,0x32,0x10,0xFF,0x08,0x14,0x03,0xB6},
	//"9876543210081403B6,,属性6，动态校验
	{0x01,0x02,0x9C,0x01,0xC1,0xFB, 0x60,    0x00,0x00, 0x00,0x00, 0x98,0x76,0x54,0x32,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	//"01029C01C1FB600000000098765432100000000000000000},,属性7，模组标识/硬件标识
	// 附加信息字段                 设备类别 硬件厂商   模组类型   序列号                     防伪校验码
};
#elif(MAX_PHASE == 3)
//电科院用户
TSoftCheckFlag SoftCheckFlag=
{
	//"FF000000",属性5,软件标识
	{0xFF,0x00,0x00,0x00},
	{0x98,0x76,0x54,0x32,0x10,0xFF,0x08,0x14,0x03,0xB6},
	//"9876543210081403B6,,属性6，动态校验
	{0x01,0x02,0x9C,0x01,0xC1,0xFB,  0x60,    0x00,0x00,   0x01,0x00,    0x98,0x76,0x54,0x32,0x10,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	//"01029C01C1FB600000000098765432100000000000000000},,属性7，模组标识/硬件标识
	// 附加信息字段                 设备类别    硬件厂商      模组类型       序列号                        防伪校验码
};

//非电科院用户
TSoftCheckFlag SoftCheckFlag_bak=
{
	//"FF000000",属性5,软件标识
	{0xFF,0x00,0x00,0x00},
	{0x98,0x76,0x54,0x32,0x10,0xFF,0x08,0x14,0x03,0xB6},
	//"9876543210081403B6,,属性6，动态校验
	{0x01,0x02,0x9C,0x01,0xC1,0xFB,  0x60,    0x00,0x00,   0x01,0x00,    0x98,0x76,0x54,0x32,0x10,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	//"01029C01C1FB600000000098765432100000000000000000},,属性7，模组标识/硬件标识
	// 附加信息字段                 设备类别    硬件厂商      模组类型       序列号                        防伪校验码
};
#endif


static const BYTE T_SoftCheck[]		= //模组列表
{ 
	Octet_string_698,	4, //模组描述符		属性5（软件标识，只读）∷= octet-string
	Octet_string_698,	10,  //模组序列号		属性6（软件标识动态校验码，只读）∷= octet-string

	Octet_string_698,	24, //属性7（模组标识，只读）∷= octet-string	模组标识为24个字节 是模组的唯一性标识，统一发行。

};

typedef struct TMyVer_t	
{
    WORD  Ver;		//版本
}TMyVer;

const BYTE T_Addr[]=
{
    Octet_string_698,  6,	

};

const BYTE T_MeterNum[]=
{
    Unsigned_698,  1,	

};

const BYTE T_MyVer[] =
{
	Structure_698,	    2,
	Long_unsigned_698, 2,                          //
	Long_unsigned_698, 2,
};

const BYTE T_44000200AppLink[] =
{
	Array_698,			1,           //对象列表∷=array 一个可访问对象
	Structure_698,	    2,			//一个可访问对象∷=structure {	对象标识  OI，	访问权限  structure}
	
	Long_unsigned_698,  2,			//对象标识  OI
	Structure_698,	    2,          //访问权限  structure

	Array_698,			1,			//属性访问权限  array 一个属性访问权限，
	Array_698,			1,			//方法访问权限  array 一个方法访问权限

	Structure_698,	    2,			//一个属性访问权限∷=structure
	Unsigned_698,		1,			//属性ID  			unsigned
	Unsigned_698,		1,			//属性访问权限类别  	enum
	
	Structure_698,	    2,			//一个方法访问权限∷=structure
	Unsigned_698,		1,			//方法ID  		unsigned，
	Unsigned_698,		1,			//方法访问权限	bool
};
//ESAM计数器
static const BYTE T_NowCount[]		= 
{ 
	Structure_698,				    3,
		Double_long_unsigned_698,		4,
		Double_long_unsigned_698,		4,
		Double_long_unsigned_698,		4,
};

//static const BYTE T_MeterCfgPara[]		= //设备列表
//{ 
//	Array_698,			1,
//	Structure_698, 	 	4, 
//	Region_698,			1,
//
//	Array_698,			3,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//
//	Array_698,			3,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//
//	Array_698,			3,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//	Double_long_unsigned_698,		4,
//
//
//	Visible_string_698,	4,		//
//	Visible_string_698,	4,		//
//	Visible_string_698,	6,		//
//	Visible_string_698,	4,		//
//	Visible_string_698,	6,		//
//	Visible_string_698,	8,		//
//};

static const BYTE T_HGPARA[]		= //
{ 
	Structure_698, 	 	4, 
	
	Long_unsigned_698, 2,  //
	Long_unsigned_698, 2,   //
	Long_unsigned_698, 2,  //
	Long_unsigned_698, 2,   //

};

static const BYTE T_FactoryBianHao[]		= //
{ 
	Visible_string_698,	32,		//
};

//0x4A00 属性2定义,模组握手完成标志
static const BYTE A_4A00_2_Status[] =
{
	Enum_698,			1,	//模组握手标志
};

//0xFF01 属性2定义,模组切换jlink和uart调试标志
static const BYTE A_FF01_2_Status[] =
{
	Enum_698,			1,	//模组握手标志
};

//0x4A01 属性2定义,模组运行状态字
static const BYTE A_4A01_2_ModStatus[] =
{
	Bit_string_698,			16,	//模组运行状态
};

//0x4800 属性2定义，用电设备类号
static const BYTE A_4800_2_ApplClsID[] =
{
	Array_698,			0xff,
	Long_unsigned_698,	2,	//设备类号
};

//0x4800 属性3定义，电器设备当前数据集
static const BYTE A_4800_3_ApplCurData[] =
{
	Array_698,			0xff,
	Structure_698, 	 	3,
	Long_unsigned_698,	2,	//设备类号
	Enum_698,			1,	//设备状态
	Double_long_698,	4,	//设备有功功率
};

#if(PROTOCOL_VERSION == 25)
//0x4801 属性2定义，电器设备当前数据单元
static const BYTE A_4801_2_ApplCurDataUnion[] =
{
	Structure_698, 	 	3,
	Long_unsigned_698,	2,	//设备类号
	Enum_698,			1,	//设备状态
	Double_long_698,	4,	//设备有功功率
};
#endif

//0x4800 属性4定义，电器设备冻结数据单元
const BYTE A_4800_4_ApplFrzData[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//设备类号
	Double_long_unsigned_698,	4,	//冻结周期内有功用电量
	Double_long_698,			4,	//冻结周期内设备运行平均功率
	Unsigned_698,				1,	//启动时间
	Unsigned_698,				1,	//停止时间
	Unsigned_698,				1,	//合并启停数
};

//0x4800 属性4定义，电器设备冻结，非电热类
const BYTE A_4800_4_ApplFrzData_NotElecHot[] =
{
	Array_698,					0xff,
	Structure_698, 	 			6,
	Long_unsigned_698,			2,	//设备类号
	Double_long_unsigned_698,	4,	//冻结周期内有功用电量
	Double_long_698,			4,	//冻结周期内设备运行平均功率
	NULL_698,					1,	//启动时间
	NULL_698,					1,	//停止时间
	NULL_698,					1,	//合并启停数
};

#if(PROTOCOL_VERSION == 25)
//0x4800 属性8定义,模组上传特征数据
static const BYTE A_4800_8_FeatureData[] =
{
	Octet_string_698,			0xff,	//模组特征数据,加入组报文时候改成256
};
#endif

//4800属性2的数据存这里
WORD tApplNumData[NILM_EQUIPMENT_TYPE_NO]=
{
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0005,
	0x0006,
	0x0007,
	0x0008,
	0x0009,
	0x000a,
	0x000b,
	0x000c,
	0x0021,
	0x0022,
	0x0023,
	0x0024,
	0x0025,
	0x0026,
	0x0031,
	0x00a1,
};

//4800属性3的数据存这里
T_ApplCurData tApplCurData[NILM_EQUIPMENT_MAX_NO]=
{
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
	{0xFFFF,0xFF,0xFFFFFFFF},
};

//4800属性4的数据存这里
T_ApplFrzData tApplFrzData[NILM_EQUIPMENT_MAX_NO]=
{
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
	{0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF},
};

#if(PROTOCOL_VERSION == 25)
/* 消息队列中用到的放置消息的内存池 */
BYTE g_UnDevFeature[FEATRUE_MSG_SIZE]; //此全局变量用于存放未知设备事件的特征数据
TDeviceInfoPool g_DevInof;			   //此全局变量用于存放特定设备信息
#endif
T_ApplFrzData tApplFrzDataFLASH[NILM_EQUIPMENT_MAX_NO];

//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj ParaMeterObj[] =
{
	{  0x4031,  &SU_00,   	T_Modulelist  },//模组列表
	{  0x4810,	&SU_00, 	T_Special  	  },//电能模块

	{  0x4300,	&SU_00, 	T_Device  	  },//电能模块

	{  0x4027,	&SU_00, 	T_Enum  	  },//
	{  0xF001,	&SU_00, 	T_FileStatus },//T_VisString32
	{  0xF402,	&SU_00, 	T_FileVer },//
	{  0x4800,	&SU_00, 	T_SoftCheck },
	{  0x4000,	&SU_00, 	T_DateTimS },

	{  0x4400,	&SU_00, 	T_44000200AppLink },

	{  0x4001,	&SU_00, 	T_Addr },

	{  0x4010,	&SU_00, 	T_MeterNum },

	{  0x4030,	&SU_00, 	T_HGPARA },
	{  0x4103,	&SU_00, 	T_FactoryBianHao },
	{  0x4A00,	&SU_00, 	A_4A00_2_Status },
	{  0x4A01,	&SU_00, 	A_4A01_2_ModStatus },
	#if(PROTOCOL_VERSION == 25)
	{  0x4801,	&SU_00, 	A_4801_2_ApplCurDataUnion },
	#endif
};	

// 支持的对象列表 可访问对象以及该对象属性和方法的访问权限
static const TObjList ObjList[] =
{
	{  0x4000,  2,		0x01,  	0,	0	},					//时间
	{  0x2000,  2,		0x01,  	0,	0 	},					//电压,分相数值数组
	{  0x2001,  2,	    0x01,  	0,	0 	},					//电流
	{  0x2004,  2,		0x01,  	0,	0	},					//有功功率
};	

static BYTE NumofNILM;
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
BOOL HexToChar(BYTE byHex, char* pOut)
{
//		if(byHex>=0 && byHex<=9)
  		if(byHex<=9)
		{
				*pOut = byHex + '0';
				return TRUE;
		}
		if(byHex>=0x0A && byHex<=0x0F)
		{
				*pOut = byHex + 'A'-10;
				return TRUE;
		}
		*pOut = 0;
		return FALSE;
		
}

WORD api_GetSoftCheckFlag(BYTE *Buffer )
{	
	TRealTimer time;
	
	get_sys_time(&time);
	
/*	SoftCheckFlag.SoftCheck[5] = time.Mon;   //lib_BBinToBCD(time.Mon);
	SoftCheckFlag.SoftCheck[6] = time.Day;   //lib_BBinToBCD(time.Day);
	SoftCheckFlag.SoftCheck[7] = time.Hour;  //lib_BBinToBCD(time.Hour);
	SoftCheckFlag.SoftCheck[8] = (SoftCheckFlag.SoftCheck[0] + SoftCheckFlag.SoftCheck[5] + SoftCheckFlag.SoftCheck[6]+ SoftCheckFlag.SoftCheck[7]) & 0xFF;
*/															

	SoftCheckFlag.SoftCheck[6] = time.Mon;
	SoftCheckFlag.SoftCheck[7] = time.Day;
	SoftCheckFlag.SoftCheck[8] = time.Hour;
	// SoftCheckFlag.SoftCheck[9] = ( lib_BBCDToBin(SoftCheckFlag.SoftCheck[0]) +lib_BBCDToBin(SoftCheckFlag.SoftCheck[5]) + time.Mon + time.Day +time.Hour) & 0xFF;
	SoftCheckFlag.SoftCheck[9] = ( SoftCheckFlag.SoftCheck[0] +SoftCheckFlag.SoftFlag[0] + SoftCheckFlag.SoftCheck[6] + SoftCheckFlag.SoftCheck[7] +SoftCheckFlag.SoftCheck[8]) & 0xFF;//潘辰台体检查不过，原因是这里对bcd转换hex，取的值也不对，电能质量和负荷辨识模组不一致

	memcpy(Buffer, &SoftCheckFlag.SoftCheck, sizeof(SoftCheckFlag.SoftCheck) );
	
	return sizeof(SoftCheckFlag.SoftCheck);
}
void copy_default_softflag()
{
   memcpy(&SoftCheckFlag, &SoftCheckFlag_bak, sizeof(SoftCheckFlag_bak) );
}

WORD api_GetSoftFlag(BYTE *Buffer )
{	
	memcpy(Buffer, &SoftCheckFlag.SoftFlag, sizeof(SoftCheckFlag.SoftFlag) );
	
	return sizeof(SoftCheckFlag.SoftFlag);
}

WORD api_GetSoftModuleFlag(BYTE *Buffer )
{	
  
#if(USER_TEST_MODE)
	memcpy(Buffer, &SoftCheckFlag_bak.ModuleFlag, sizeof(SoftCheckFlag_bak.ModuleFlag) );
#else
	memcpy(Buffer, &SoftCheckFlag.ModuleFlag, sizeof(SoftCheckFlag.ModuleFlag) );
#endif	
	return sizeof(SoftCheckFlag.ModuleFlag);
}

BOOL api_GetSoftFlagFromFlash(BYTE *Buffer)
{
		memcpy(&SoftCheckFlag,Buffer,sizeof(SoftCheckFlag));
	
		return TRUE;
}

BOOL api_WriteSoftFlagToFlash(BYTE *Buffer)
{
		memcpy(Buffer,&SoftCheckFlag,sizeof(SoftCheckFlag));
	
		return TRUE;
}

WORD GetApplianceCurrentData(BYTE Index, BYTE Count, BYTE *Buffer )
{	
	BYTE Num;//读取的个数
	BYTE i;//取数据起始点
	
	if(Index == 0)
	{
		Num = Count;
		i = 0;
	}
	else
	{
		Num = 1;
		i = Index - 1;
	}

	memcpy(Buffer, &tApplCurData[i], Num * sizeof(T_ApplCurData));
	
	return Num * sizeof(T_ApplCurData);
}

WORD GetApplianceClassID(BYTE Index, BYTE Count, BYTE *Buffer )
{	
	BYTE Num, i;
	
	if(Index == 0)
	{
		Num = Count;
		for(i = 0; i< Num; i++)
		{
			memcpy(Buffer, &tApplNumData[i], sizeof(WORD));
			Buffer += sizeof(WORD);
		}
	}
	else
	{
		Num = 1;
		memcpy(Buffer, &tApplNumData[Index - 1], sizeof(WORD));
	}
	return Num * sizeof(WORD);
}

WORD GetApplianceFreezData(BYTE Index, BYTE Count, BYTE *Buffer )
{	
	BYTE Num;//读取的个数
	BYTE i;//取数据起始点
	
	if(Index == 0)
	{
		Num = Count;
		i = 0;
	}
	else
	{
		Num = 1;
		i = Index - 1;
	}
	
	memcpy(Buffer, &tApplFrzData[i], Num * sizeof(T_ApplFrzData));
	
	return Num * sizeof(T_ApplFrzData);
}

WORD GetFrzorCurLen(BYTE flag )
{	
	BYTE i,res=0;
	T_ApplCurData tempBuf = {0xFFFF,0xFF,0xFFFFFFFF};
	T_ApplFrzData tempBuf1 = {0xFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF,0xFF,0xFF};
	
	for(i=0;i<NILM_EQUIPMENT_MAX_NO;i++)
	{
		if(flag == 2)
		{
			res = NILM_EQUIPMENT_TYPE_NO;
			return res;
		}
		else if(flag == 3)
		{
			if(memcmp(&tempBuf.ApplClassID, &tApplCurData[i].ApplClassID, sizeof(T_ApplCurData)) == 0)
			{
				continue;
			}
		}
		else if (flag == 4)
		{
			if(memcmp(&tempBuf1.ApplClassID, &tApplFrzData[i].ApplClassID, sizeof(T_ApplFrzData)) == 0)
			{
				continue;
			}
		}
		res++;
	}
	
	return res;
}

//--------------------------------------------------
// 功能描述:  读分钟冻结数据时，由于存电器个数不确定，需要读取时进行计算
//
// 参数:      [in]：Num
//
// 返回值:    无
//
// 备注内容:  无
//--------------------------------------------------
void api_SetTagArrayNum(BYTE Num)
{
	NumofNILM = Num;
}

//--------------------------------------------------
//功能描述:  根据表格查找参变量OAD
//         
//参数:      OI[in]：OI
//         
//返回值:    参变量索引
//         
//备注内容:  无
//--------------------------------------------------
static BYTE SearchParaMeterOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(ParaMeterObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
    
	for (i=0; i<Num; i++)
	{
		if( OI == ParaMeterObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取参变量（class8）数据
//         
//参数:     
//          *pOAD[in]：	OAD         
//          Sch[in]：   ParaMeterObj 中的数据索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 不含tag
//--------------------------------------------------
static WORD GetProMeterParaData( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex, MaxIndex;//,Num,i,Type;
	WORD Len;
	BYTE *pBufBak = pBuf;
	const BYTE *ObjType;
    TModuleVersion tModuleVersion;
	TDevice      tDevice;
	TRealTimer time;
	WORD wData;
	char szText[20];
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	ObjType = ParaMeterObj[Sch].Type2;
	Len = 0;
	
	switch( ClassAttribute )
	{
		case 8:
			switch( OI.w )
			{
			case 0x4300:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				pBuf[0] = gReportMode.g_EnableAutoReportFlag;
				Len = 1;
				break;
			case 0xF001:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetFileTransmitType(pBuf);
				pBuf += Len;
				break;
			#if(PROTOCOL_VERSION == 25)
			case 0x4800://属性8
				if( ClassIndex > 0 )
				{
					return 0x8000;
				}

				Len = api_GetFeartureData(pBuf);//获取特征数据
				pBuf += Len;
				break;
			#endif
			}
			break;
		case 7:
			switch( OI.w )
			{
			case 0xF001:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetFileExeTime(pBuf);
				pBuf += Len;
				break;
			case 0x4300:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				pBuf[0] = gReportMode.g_EnableFollowReportFlag;
				Len = 1;
				pBuf += 1;
				break;
			case 0x4800://属性7
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetSoftModuleFlag(pBuf);
				pBuf += Len;
				break;
			}
			break;
		case 6:
			switch( OI.w )
			{
			case 0xF001:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
// 				Len = GetTypeLen( eData, ObjType );
// 				if( Len == 0x8000 )
// 				{
// 					return 0x8000;
// 				}
				Len = api_GetFileBlockSize(pBuf);
				pBuf += Len;
				break;
			case 0x4800://属性6
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetSoftCheckFlag(pBuf);
				pBuf += Len;
				break;
			}
			break;
		case 5:
			switch( OI.w )
			{
			case 0xF001:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetFileLoadFlag2(pBuf);
				pBuf += Len;
				break;
			case 0x4800://属性5
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetSoftFlag(pBuf);
				pBuf += Len;
			
				break;
			}
			break;
		case 4:
			switch( OI.w )
			{
			case 0xF001:
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = api_GetFileStatus(pBuf);
				pBuf += Len;

				break;
			case 0x4800://属性4
				MaxIndex = GetFrzorCurLen(4);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}
				if(ClassIndex == 0)
				{
					TagPara.Array_Struct_Num = MaxIndex;
				}
				else
				{
					TagPara.Array_Struct_Num = 1;
				}
				ObjType = (BYTE *)&A_4800_4_ApplFrzData;
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = GetApplianceFreezData(ClassIndex, MaxIndex, pBuf);
				pBuf += Len;
				break;
			}
			break;
		case 2:
			switch( OI.w )
			{
				case 0x4000: //时间读取
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}
						get_sys_time(&time);
						pBuf[0] = time.wYear&0xFF;
						pBuf[1] = time.wYear>>8;
						pBuf[2] = time.Mon;
						pBuf[3] = time.Day;
						pBuf[4] = time.Hour;
						pBuf[5] = time.Min;
						pBuf[6] = time.Sec;
						pBuf += 7;
						
					break;
				case 0x4001:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}
					memcpy(pBuf, ManageComInfo.Address, 6 );//获取通信地址
					pBuf += 6;
					break;
				case 0x4010:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}
					pBuf[0] = ManageComInfo.SampleNO;
					pBuf += 1;
					break;
				case 0x4031://模组列表
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}
					memcpy(&ModulelistPara.SerialNum, devid, 8 );
						
					memcpy(pBuf, &ModulelistPara.Name, sizeof(ModulelistPara.Name) );
					pBuf += sizeof(ModulelistPara.Name);
	    			memcpy(pBuf, &ModulelistPara.LogicAdrr, sizeof(ModulelistPara.LogicAdrr) );
					pBuf += sizeof(ModulelistPara.LogicAdrr);	
					memcpy(pBuf, &ModulelistPara.Class, sizeof(ModulelistPara.Class) );
					pBuf += sizeof(ModulelistPara.Class);
					memcpy(pBuf, &ModulelistPara.SerialNum, sizeof(ModulelistPara.SerialNum) );
					pBuf += sizeof(ModulelistPara.SerialNum);
					//接收到管理芯命令后，开始读取管理芯信息
					ManageComInfo.Status = eGET_ADDR;
					ManageComInfo.byRetryCount = 0;
					#if(USER_TEST_MODE)
						ManageComInfo.DelayTime = 8;
					#else
						ManageComInfo.DelayTime = 7;                                    
					#endif
                                        
				break;
				case 0x4027://文件
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					ObjType = (BYTE *)&T_Enum;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					Len = api_GetFileTransmitStatus(pBuf);
					pBuf += Len;
					//memcpy(pBuf, &tModuleVersion.LogicAdrr, sizeof(tModuleVersion.LogicAdrr) );
					//pBuf += sizeof(tModuleVersion.LogicAdrr);	
				break;
				case 0xF402:
				
					ObjType = (BYTE *)&T_Enum;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					Len = api_GetFileVersion(pBuf,ClassIndex);
					pBuf += Len;
					break;
				case 0x4030:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
	    			}
//					memcpy(pBuf, &VolHg.Para.wVolExamUpper, sizeof(VolHg.Para.wVolExamUpper) );
//					pBuf += sizeof(VolHg.Para.wVolExamUpper);
//					memcpy(pBuf, &VolHg.Para.wVolExamDown, sizeof(VolHg.Para.wVolExamDown) );
//					pBuf += sizeof(VolHg.Para.wVolExamDown);
//					memcpy(pBuf, &VolHg.Para.wVolHGUpper, sizeof(VolHg.Para.wVolHGUpper) );
//					pBuf += sizeof(VolHg.Para.wVolHGUpper);
//					memcpy(pBuf, &VolHg.Para.wVolHGDown, sizeof(VolHg.Para.wVolHGDown) );
//					pBuf += sizeof(VolHg.Para.wVolHGDown);//zh

					break;
				case 0x4103:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
                    }
                    if(!api_VReadSafeMem(GET_CONTINUE_ADDR(EventConRom.szAssetsCode),36,(BYTE*)pBuf) )
                    {
						memset(pBuf, 0,32);
					}
                                        
					pBuf += 32;
					
					break;
				case 0x4400:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
	    			}
					// pBuf += 10;
					break;

				case 0x4800://属性2
					MaxIndex = GetFrzorCurLen(2);
					if( ClassIndex > MaxIndex)
					{
						return 0x8000;
					}
					if(ClassIndex == 0)
					{
						TagPara.Array_Struct_Num = MaxIndex;
					}
					else
					{
						TagPara.Array_Struct_Num = 1;
					}

					ObjType = (BYTE *)&A_4800_2_ApplClsID;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					Len = GetApplianceClassID(ClassIndex, MaxIndex, pBuf);
					pBuf += Len;
					break;

				#if(PROTOCOL_VERSION == 25)
				case 0x4801://属性2
					ObjType = (BYTE *)&A_4801_2_ApplCurDataUnion;
					if( ClassIndex > ObjType[1] )
					{
						return 0x8000;
					}

					Len = GetTypeLen( eData, (ObjType+ClassIndex*2) );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					
					if( api_GetSpecificData( pBuf,ClassIndex) == TRUE )
					{
						pBuf += Len;
					}
					else
					{
						return 0x8000;
					}
					break;
				#endif //#if(PROTOCOL_VERSION == 25)

				case 0x4A00://属性2
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					ObjType = (BYTE *)&A_4A00_2_Status;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
	    			}
					*pBuf = InitComplete;
					Len = 1;
					pBuf += Len;
					break;
				case 0x4A01://属性2
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					ObjType = (BYTE *)&A_4A01_2_ModStatus;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
	    			}
					memset(pBuf,0,2);
					GetMeterStatus1( pBuf );	
					Len = 2;
					pBuf += Len;
					break;
				default:
					return 0x8000;
			}
			break;

		case 3:
			switch( OI.w )
			{
			case 0x4400://  协议版本号
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				pBuf[0] = 0x17;
				pBuf[1] = 0x10;
				pBuf += 2;
				break;
			case 0xF001:
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
					Len = api_GetFileTransmitStatusF001(pBuf);
					pBuf += Len;
				break;
				case 0x4031://模组列表
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					ObjType = (BYTE *)&T_ModuleVersion;
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
					return 0x8000;
	    			}
//-------------------------------------------软件版本-------------------------------------------------//
					memcpy(&tModuleVersion, &ModuleVersion, sizeof(ModuleVersion));
					memset(&tModuleVersion.Rsv6, 0, sizeof(ModuleVersion.Rsv6));
#if(USER_TEST_MODE)//电科院模式
					tModuleVersion.Rsv6[0] = 0;
					tModuleVersion.Rsv6[1] = 0;
#else				//特殊用户模式/测试模式
					tModuleVersion.Rsv6[0] = (BYTE)(MODULE_VERSION);
					tModuleVersion.Rsv6[1] = (BYTE)(MODULE_VERSION >> 8);
#endif
					*(WORD *)&tModuleVersion.Rsv6[2] = api_CheckCpuFlashSum(0x01);
//-------------------------------------------逻辑地址-------------------------------------------------//					
					memcpy(pBuf, &tModuleVersion.LogicAdrr, sizeof(tModuleVersion.LogicAdrr) );
					pBuf += sizeof(tModuleVersion.LogicAdrr);	
					
			
					memset(&tDevice,0,sizeof(tDevice));
					memcpy(&tDevice, &DeviceVersion, sizeof(tDevice));

//-------------------------------------------厂商代码-------------------------------------------------//								
					#if(USER_TEST_MODE)
					sprintf((char *)tDevice.Factory,"%s","XXXX");
					memcpy(pBuf, &tDevice.Factory, sizeof(tDevice.Factory) );
					pBuf += sizeof(tDevice.Factory);

					#else
					sprintf((char *)tDevice.Factory,"%s",gDefaultPara.Factory);
					#endif

					sprintf((char *)tDevice.Factory,"%s",gDefaultPara.Factory);
					if(!api_VReadSafeMem(GET_CONTINUE_ADDR(EventConRom.szFactory),8,(BYTE*)pBuf) )
					{
						memcpy(pBuf, &tDevice.Factory, sizeof(tDevice.Factory) );
						pBuf += sizeof(tDevice.Factory);
					}
					else
					{
						pBuf += 4;
					}

//-------------------------------------------软件版本-------------------------------------------------//					
#if(USER_TEST_MODE)
					sprintf((char *)tDevice.SoftVer,"%s",MODULE_VERSION_T);
#else
					sprintf((char *)tDevice.SoftVer,"%s",gDefaultPara.SoftVer);//MODULE_VERSION
#endif
					memset(pBuf, 0, 16 );
					memcpy(pBuf, &tDevice.SoftVer, sizeof(tDevice.SoftVer) );
					pBuf += 16;//sizeof(tDevice.SoftVer);

//-------------------------------------------软件版本时间-------------------------------------------------//					
#if(USER_TEST_MODE)
					memset(pBuf, 'X', 6 );
					//memcpy(pBuf, &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
					pBuf += 6;//sizeof(tDevice.SoftTime);
#else
					sprintf((char *)tDevice.SoftTime,"%s",gDefaultPara.SoftTime);					
					//GetAppDateTimeString(tDevice.SoftTime);
					memset(pBuf, 0, 6 );
					memcpy(pBuf, &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
					pBuf += 6;//sizeof(tDevice.SoftTime);
#endif
					//sprintf(tDevice.HardVer,"%x",HARD_VERSION);
#if(!WIN32)
					//sprintf((char *)tDevice.HardVer,"%02x%02x",*((BYTE*)FLASH_ADDR_BOOT_VERSION + 1), *((BYTE*)FLASH_ADDR_BOOT_VERSION ));
//					sprintf((char *)tDevice.HardVer,"%s",gDefaultPara.szHardVer);//zh
#endif
//-------------------------------------------硬件版本-------------------------------------------------//
#if(USER_TEST_MODE)
					memset(pBuf, 'X', 16 );
					//memcpy(pBuf, &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
					pBuf += 16;//sizeof(tDevice.SoftTime);
					
					memset(pBuf, 'X', 6 );
					//memcpy(pBuf, &tDevice.HardTime, sizeof(tDevice.HardTime) );
					pBuf += 6;
					
					memset(pBuf, 'X', 16 );
					//memcpy(pBuf, &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
					pBuf += 16;//sizeof(tDevice.SoftTime);
					
#else
					//sprintf(tDevice.HardVer,"%x",HARD_VERSION);
					sprintf((char *)tDevice.HardVer,"%s",gDefaultPara.HardVer);
					memset(pBuf, 0, 16 );
					memcpy(pBuf, &tDevice.HardVer, sizeof(tDevice.HardVer) );
					pBuf += 16;//sizeof(tDevice.HardVer);

//-------------------------------------------软件版本时间-------------------------------------------------//					
					//memcpy(tDevice.HardTime,HARD_TIME,6);
					sprintf((char *)tDevice.HardTime,"%s",gDefaultPara.HardTime);
					memset(pBuf, 0, 6 );
					memcpy(pBuf, &tDevice.HardTime, sizeof(tDevice.HardTime) );
					pBuf += 6;//sizeof(tDevice.HardTime);
					
					
					memset(pBuf, 0, 16 );
					wData = api_CheckCpuFlashSum(0x01);
					
					memset(szText, 0, 16 );
					sprintf(szText,"%04x-%04x,B-%04x",MODULE_VERSION,wData,api_CheckCpuFlashSum(0x0));
					memcpy(pBuf, szText, 16 );
					pBuf += 16;//sizeof(tDevice.FactoryEx);
#endif
					//memset(pBuf, 0, 16 );
					//memcpy(pBuf, &tDevice.FactoryEx, sizeof(tDevice.FactoryEx) );
					//pBuf += 16;//sizeof(tDevice.FactoryEx);
					break;

			case 0x4300://
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					ObjType = (BYTE *)&T_Device;
					Len = GetTypeLen( eData, ObjType );
					if( Len == 0x8000 )
					{
						return 0x8000;
					}
			
					memset(&tDevice,0,sizeof(tDevice));
					memcpy(&tDevice, &DeviceVersion, sizeof(tDevice));

					#if(USER_TEST_MODE)
					sprintf((char *)tDevice.Factory,"%s","XXXX");
					#else
//					sprintf((char *)tDevice.Factory,"%s",gDefaultPara.szFactory);
					#endif

					memcpy(pBuf, &tDevice.Factory, sizeof(tDevice.Factory) );
					pBuf += 4;

                   	#if(USER_TEST_MODE)
					sprintf((char *)tDevice.SoftVer,"%s","XXXX");
					#else
//					sprintf((char *)tDevice.SoftVer,"%s",gDefaultPara.szSoftVer);
					#endif
					
					memcpy(pBuf, &tDevice.SoftVer, sizeof(tDevice.SoftVer) );
					pBuf += 4;
	                  	
					#if(USER_TEST_MODE)
					sprintf((char *)tDevice.SoftTime,"%s","XXXXXX");
					#else
//					sprintf((char *)tDevice.SoftTime,"%s",gDefaultPara.szSoftTime);
					#endif
					
					//GetAppDateTimeString(tDevice.SoftTime);
					memcpy(pBuf, &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
					pBuf += 6;

					#if(USER_TEST_MODE)
					sprintf((char *)tDevice.HardVer,"%s","XXXX");
					#else
//					sprintf((char *)tDevice.HardVer,"%s",gDefaultPara.szHardVer);
					#endif

					//sprintf((char *)tDevice.HardVer,"%s",HARD_VERSION_F);
					
					memcpy(pBuf, &tDevice.HardVer, sizeof(tDevice.HardVer) );
					pBuf += 4;

					#if(USER_TEST_MODE)
					sprintf((char *)tDevice.HardTime,"%s","XXXXXX");
					#else
//					sprintf((char *)tDevice.HardTime,"%s",gDefaultPara.szHardTime);
					#endif

				//	memcpy(tDevice.HardTime,HARD_TIME,6);
					memcpy(pBuf, &tDevice.HardTime, sizeof(tDevice.HardTime) );
					pBuf += 6;

					memcpy(pBuf, &tDevice.FactoryEx, sizeof(tDevice.FactoryEx) );
					pBuf += 8;
					break;
			case 0x4027://文件
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				ObjType = (BYTE *)&T_Enum;
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
			//	Len = api_GetFileTransmitStatus(pBuf);
				Len = api_GetFileTransmitMakeType(pBuf);
				pBuf += Len;
				break;
			
			case 0x4800://属性3
				MaxIndex = GetFrzorCurLen(3);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}
				if(ClassIndex == 0)
				{
					TagPara.Array_Struct_Num = MaxIndex;
				}
				else
				{
					TagPara.Array_Struct_Num = 1;
				}

				ObjType = (BYTE *)A_4800_3_ApplCurData;
				Len = GetTypeLen( eData, ObjType );
				if( Len == 0x8000 )
				{
					return 0x8000;
				}
				Len = GetApplianceCurrentData(ClassIndex, MaxIndex, pBuf);
				pBuf += Len;
			 	break;
			default:
					return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对参变量（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 参变量（class8）  OI=4XXX
//--------------------------------------------------
static WORD AddTagMeterPara(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex, MaxIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	BYTE T_BOOL[]={Boolean_698,1};
	BYTE TagEx[20]={0};
        char szText[32];
        
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	if((OI.w == 0x4800) && (ClassAttribute >= 5) && (ClassAttribute <= 7))
	{
		Tag = ParaMeterObj[Sch].Type2;
		switch(ClassAttribute)
		{
			case 5: 
			Tag += 0; 
			break;
			case 6: 
			Tag += 2; 
			break;
			case 7: 
			Tag += 4; 
			break;
			default: 
			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
			break;
			}
			Lenth = GetTypeLen( eTagData, Tag );
			if( Lenth == 0x8000 )
			{
				return 0x8000;
			}
			
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			return Lenth;
	}
	switch( ClassAttribute )
	{
		case 7:
			if( OI.w == 0xF001 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				TagEx[0] = DateTime_S_698;
				Lenth = GetTypeLen( eTagData, TagEx );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( TagEx, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
		case 8:
			if(OI.w == 0x4300)
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag =  T_BOOL;
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			if(OI.w == 0xF001)
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				TagEx[0] = Enum_698;
				Lenth = GetTypeLen( eTagData, TagEx );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( TagEx, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			#if(PROTOCOL_VERSION == 25)
			if(OI.w == 0x4800) //Tag属性8
			{
				if(IsUse4800 == 1)
				{
					if( ClassIndex > 0 )
					{
						return 0x8000;
					}
					
					Tag =  (BYTE *)&A_4800_8_FeatureData;
					TagPara.Lenth.w = FEATRUE_MSG_SIZE;
					
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
					return 0x8000;
					}
					Lenth = Result;
					
					return Lenth;
				}
				else
				{
					return 0x8000;
				}
			}
			#endif //#if(PROTOCOL_VERSION == 25)
			break;
		case 2://属性2
			if( OI.w == 0x4000 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
				if( OI.w == 0x4001 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			if( OI.w == 0x4010 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
		
			if( OI.w == 0x4027 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			if( OI.w == 0xF402 )//升级前后的版本
			{
				if( ClassIndex > 1 )
				{
					ClassIndex = 0;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                                
                                #if(USER_TEST_MODE)
                                      sprintf(szText,"%s",MODULE_VERSION_T);
                                #else
                                       sprintf(szText,"%x",MODULE_VERSION);
                                #endif
          
                               		
				T_FileVer[1] = strlen(szText);
				Tag = (BYTE *)&T_FileVer;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                                
                                if( Lenth == 0x8000 )
                                {
                                    return 0x8000;
                                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			if( OI.w == 0x4030 )//
			{
				if( ClassIndex > 1 )
				{
					ClassIndex = 0;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				
				return Lenth;
			}
			if( OI.w == 0x4400 || OI.w == 0x4103 )//
			{
				if( ClassIndex > 1 )
				{
					ClassIndex = 0;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				
				return Lenth;
			}

			if( OI.w == 0x4800 )//Tag属性2
			{
				MaxIndex = GetFrzorCurLen(2);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}

				Tag = (BYTE *)&A_4800_2_ApplClsID;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);

				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}

			#if(PROTOCOL_VERSION == 25)
			if( OI.w == 0x4801 )//Tag属性2
			{
				if(IsUse4801 == 1)
				{
					Tag = (BYTE *)&A_4801_2_ApplCurDataUnion;
					if( ClassIndex > Tag[1] )
					{
						return 0x8000;
					}
					
					Tag +=GetTagOffsetAddr( ClassIndex, Tag);
					Lenth = GetTypeLen( eTagData, Tag );
					if( Lenth == 0x8000 )
					{
						return 0x8000;
					}
						
					if( OutBufLen < Lenth )
					{
						return 0;
					}
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}

					return Lenth;	
				}
				else
				{
					return 0x8000;
				}
			}
			#endif //#if(PROTOCOL_VERSION == 25)


			if( OI.w == 0x4A00 )//Tag属性2
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4A00_2_Status;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}

			if( OI.w == 0x4A01 )//Tag属性2
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4A01_2_ModStatus;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			break;
		case 3://属性3
			if(OI.w == 0xF001)
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				TagEx[0] = Enum_698;
				Lenth = GetTypeLen( eTagData, TagEx );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( TagEx, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
	
			if( OI.w == 0x4400 )
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_MyVer;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_ModuleVersion;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			if( OI.w == 0x4300 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_Device;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
			if( OI.w == 0x4027 )//升级操作类型
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_Enum;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}

			if( OI.w == 0x4800 )//Tag属性3
			{
				MaxIndex = GetFrzorCurLen(3);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}

				Tag = (BYTE *)&A_4800_3_ApplCurData;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);

				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
			break;
		case 5:
                  if( OI.w == 0xF001 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                                if( Lenth == 0x8000 )
                                {
                                    return 0x8000;
                                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
		case 6://wxy
			if( OI.w == 0xF001 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				TagEx[0] = Double_long_unsigned_698;
				Lenth = GetTypeLen( eTagData, TagEx );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
				
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( TagEx, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				
				return Lenth;
			}
		case 4:
			if( OI.w == 0x4800 )//Tag属性4
			{
				if(IsUse5002 == 1)
				{
					TagPara.Array_Struct_Num = NumofNILM;
					if( ClassIndex > TagPara.Array_Struct_Num)
					{
						return 0x8000;
					}

					if(ClassIndex != 0)		//index非零情况
					{
						TagPara.Array_Struct_Num = 1;
					}

					Tag = (BYTE *)&A_4800_4_ApplFrzData;		//临时
					Tag +=GetTagOffsetAddr( ClassIndex, Tag);

					Lenth = GetTypeLen4804( eTagData, Tag, InBuf);//特殊处理
					if( Lenth == 0x8000 )
					{
						return 0x8000;
					}
						
					if( OutBufLen < Lenth )
					{
						return 0;
					}
					Result = GetRequestAddTag4804( Tag, InBuf, OutBuf );//特殊处理
					if( Result == 0x8000 )
					{
						return 0x8000;
					}

					return Lenth;
				}
				else
				{
					return 0x8000;
				}
			
			}
			else
			{
			if( ClassIndex > 1 )
			{
				return 0x8000;
			}
			
			Tag = ParaMeterObj[Sch].Type2;
			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
			Lenth = GetTypeLen( eTagData, Tag );
			if( Lenth == 0x8000 )
			{
				return 0x8000;
			}
			
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			return Lenth;
			}

		default:
			return 0x8000;
	}
	return 0x8000;
}

//--------------------------------------------------
//功能描述:  读取参变量（class8）数据 OI=4XXX
//         
//参数:  
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch;
	BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetProMeterParaData( pOAD, Sch, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			Lenth = AddTagMeterPara(pOAD, Sch, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		Lenth = AddTagMeterPara(pOAD, Sch, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth; 
}

WORD GetRequestFileParaF4027( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	// BYTE Sch;
	// BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Lenth = api_GetFileTransmitStatus(OutBuf);
	
	return Lenth; 
}

WORD GetRequestFilePara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	// BYTE Sch;
	// BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);

//	api_GetFileTransmitStatus(OutBuf);


	
	Lenth = api_GetFileStatus(OutBuf);
	
	return Lenth; 
}

WORD GetRequestFileFlag( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	// BYTE Sch;
	// BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	//	api_GetFileTransmitStatus(OutBuf);
	
	
	
	Lenth = api_GetFileLoadFlag( DataType,InBuf, OutBuf,OutBufLen);
	
	return Lenth; 
}
WORD GetRequestFileF402( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	// BYTE Sch;
	// BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Lenth = api_GetFileStatus(OutBuf);
	
	return Lenth; 
}

extern TFileInfo tFileInfo;
WORD GetRequestFileF402Ver2( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	// BYTE Sch;
	// BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	int nLen = 0;
	char szText[32];
	int ClassIndex;
	
	lib_ExchangeData(OI.b,pOAD,2);
//	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];

	memset(szText,0,20);

	OutBuf[nLen++] = Visible_string_698;

/*	if(ClassIndex == 4)
	{
#if(USER_TEST_MODE)
          sprintf(szText,"%s",MODULE_VERSION_T);
#else
	  sprintf(szText,"%x",MODULE_VERSION);
#endif		
		OutBuf[nLen++] = strlen(szText);
		memcpy(&OutBuf[nLen], szText, strlen(szText));
	
		nLen += strlen(szText);

	}
	else*/ if(ClassIndex == 2)
	{
		sprintf(szText,"%s","NILM");
		OutBuf[nLen++] = strlen(szText);
		memcpy(&OutBuf[nLen], szText, strlen(szText));
	
		nLen += strlen(szText);
	}
		
	return nLen;

}

WORD GetRequestFile_OtherLen( WORD wOad, BYTE byClass, BYTE byIndex )
{	
	char szText[32];

	if(wOad == 0xF001) //下载方标识
	{
		if(gDownFlagFlash == 0x00)//正常ram的值，如果升级后没有掉电，f0010500读取是dky或者本次启动传输传来的标识。如果掉电后再读取是空格的ascll
		{
			if(tFileInfo.szDownFlag[0] == 0)
			{
				sprintf(tFileInfo.szDownFlag,"%s"," ");
			}

			if(byClass == 5)
				return strlen(tFileInfo.szDownFlag)+2;
		}
		else if (gDownFlagFlash == 0x01)//读取3036事件时候调用的读取，此时是正常flash读取值的长度
		{
			gDownFlagFlash = 0x00;
			if(byClass == 5)
			{
				return 	gDownFlagLen+2;
			}
		}
	}

	if(wOad == 0xF402)
	{
		if(byIndex == 2)
		{
			sprintf(szText,"%s","NILM");
			return strlen(szText)+2;
		}
			
		if(byIndex == 4)
		{
#if(USER_TEST_MODE)
          sprintf(szText,"%s",MODULE_VERSION_T);
#else
	  sprintf(szText,"%x",MODULE_VERSION);
#endif

			return strlen(szText)+2;
		}

	}
	
	return 0;

}


//-----------------------------------------------
//函数功能: 获取电表安全芯片信息
//
//参数:		Type[in]	ff--全部信息
//			OutBuf[out]	放置要读取的长度及数据，长度两字节，高在前，低在后			
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_GetEsamInfo(BYTE Type,BYTE *OutBuf)
{
	BOOL Result=FALSE;
	DWORD dwCount=1;

	switch(Type)
	{
	case 2:			//ESAM序 列 号：51 01 00 00 00 04 C5 97
		OutBuf[0] = 0;
		OutBuf[1] = 8;
		memcpy(&OutBuf[2],devid,8);
		return TRUE;
	case 4:			//对称密钥版本：00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		OutBuf[0] = 0;
		OutBuf[1] = 16;
		if(sm4_GetStatus())
			memset(&OutBuf[2],0xFF,16);
		else
			memset(&OutBuf[2],0,16);
		return TRUE;
	case 6:			//单地址应用协商计数器：1821
		OutBuf[0] = 0;
		OutBuf[1] = 12;
		memcpy(&OutBuf[2],&dwCount,4);
		memset(&OutBuf[6],0,4);
		memset(&OutBuf[10],0,4);

		return TRUE;
	default:
		break;
	}
	
	return Result;
}
//--------------------------------------------------
//功能描述: 获取ESAM参数（F100）（class21）属性数据（不带Tag）
//         
//参数:
//			Dot[in]：	小数点 （未用到）    
//          *pOAD[in]：	OAD         
//          Sch[in]：   OtherObj中的事件索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 
//--------------------------------------------------
static WORD GetOtherClass21Data( BYTE Dot, BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{	
	BYTE ClassAttribute,ClassIndex;
	BYTE Tag[8] = {0,0,2,1,4,5,5,6};
	WORD Result;
	TTwoByteUnion OI,DataLen;
	BYTE *pBufBak = pBuf;		
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	
	switch( ClassAttribute )
	{
		case 0x02: //获取ESAM序列号
		case 0x03: //获取ESAM版本号
		case 0x04: //获取对称密钥版本
		case 0x05: //获取会话时效门限
	    case 0x06: //获取会话剩余时间
		case 0x07: //获取计数器
			if( ClassAttribute == 7 )
			{
				if( ClassIndex > 3 )
				{
					return 0x8000;
				}
			}
			else
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
			}
			
			Result = api_GetEsamInfo(Tag[ClassAttribute],pBuf);
			if(Result == FALSE)
			{
				return 0x8000;
			}
			lib_ExchangeData(DataLen.b,pBuf,2);
			
			if( (ClassAttribute == 5) || (ClassAttribute == 6) )
			{
			    if( /*(APPConnect.Flag == FALSE) &&*/ (ClassAttribute == 6) )//未建立应用链接时 会话剩余时间为0
    			{
    				memset( pBuf, 0x00, 4 );
    			}
    			else
    			{
    				//此处使用exchange是因为数据标识位doublelong，加tag时会进行倒序，为防止数据不变在数据源处进行倒序
    				lib_ExchangeData( pBuf, pBuf+2+4*(ClassAttribute-5), 4 );
    			}
				pBuf += 4;
			}
			else
			{
				memcpy( pBuf, pBuf+2, DataLen.w );
				pBuf += DataLen.w;
			}
			break;
			default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

WORD GetRequestESamPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;
	WORD Lenth,Result;
	const BYTE *Tag;
	BYTE Buf[300];

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetOtherClass21Data( Dot, pOAD, 0, Buf );
	}
	switch( ClassAttribute )
	{
	case 0x02: //获取ESAM序列号
	case 0x03: //获取ESAM版本号
	case 0x04: //获取对称密钥版本
		if( ClassIndex != 0 )
		{
			return 0x8000;
		}
		
		if( ClassAttribute == 2 )
		{
			Tag = T_OctetString8;
		}
		else if( ClassAttribute == 3 )
		{
			Tag = T_OctetString5;
		}
		else
		{
			Tag = T_OctetString16;
		}
		
		Lenth = GetTypeLen( eTagData, Tag );
		if( Lenth == 0x8000 )
		{
			return 0x8000;
		}
		
		if( OutBufLen < Lenth )
		{
			return 0;
		}
		
		Result = GetRequestAddTag( Tag, Buf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		
		break;
	case 0x05: //获取会话时效门限
	case 0x06: //获取会话剩余时间
	case 0x0E: //获取红外认证时效
	case 0x0F: //获取红外认证时效剩余时间
		if( ClassIndex != 0 )
		{
			return 0x8000;
		}
		
		Tag = T_UNDoubleLong;
		Lenth = GetTypeLen( eTagData, Tag );
		if( Lenth == 0x8000 )
		{
			return 0x8000;
		}
		
		if( OutBufLen < Lenth )
		{
			return 0;
		}
		
		Result = GetRequestAddTag( Tag, Buf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		break;
	case 0x07: //获取计数器
		if( ClassIndex > T_NowCount[1] )
		{
			return 0x8000;
		}
		Tag = T_NowCount;
		Tag +=GetTagOffsetAddr( ClassIndex, Tag);
		Lenth = GetTypeLen( eTagData, Tag );
		if( Lenth == 0x8000 )
		{
			return 0x8000;
		}
		
		if( OutBufLen < Lenth )
		{
			return 0;
		}
		
		Result = GetRequestAddTag( Tag, Buf, OutBuf );
		if( Result == 0x8000 )
		{
			return 0x8000;
		}
		break;
		
	default:
		return 0x8000;
	}	
	

	return Lenth;
}

WORD GetObjNum(void)
{
	WORD wNo;
	WORD wOldOI=0;
	WORD wTotal=0;

	for(wNo=0; wNo<sizeof(ObjList)/sizeof(ObjList[0]); wNo++)
	{
		if(ObjList[wNo].OI != wOldOI)
		{
			wTotal++;
			wOldOI = ObjList[wNo].OI;
		}
		
	}
	return wTotal;
}

WORD GetObjOI(int nIndex)
{
	WORD wNo;
	WORD wOldOI=0;
	WORD wTotal=0;
	
	for(wNo=0; wNo<sizeof(ObjList)/sizeof(ObjList[0]); wNo++)
	{
		if(ObjList[wNo].OI != wOldOI)
		{
			if(nIndex == wTotal)
				return ObjList[wNo].OI ;

			wTotal++;
			wOldOI = ObjList[wNo].OI;
		}
	}
	return 0;
}

WORD GetObjPropertyNum(WORD wOI)
{
	WORD wNo;
	WORD wTotal=0;
	
	for(wNo=0; wNo<sizeof(ObjList)/sizeof(ObjList[0]); wNo++)
	{
		if(ObjList[wNo].OI == wOI && ObjList[wNo].Property != 0)
		{
			wTotal++;
		}
	}
	return wTotal;
}

WORD GetObjMothodNum(WORD wOI)
{
	WORD wNo;
	WORD wTotal=0;
	
	for(wNo=0; wNo<sizeof(ObjList)/sizeof(ObjList[0]); wNo++)
	{
		if(ObjList[wNo].OI == wOI && ObjList[wNo].Mathod != 0)
		{
			wTotal++;
		}
	}
	return wTotal;
}


//波特率  ENUMERATED
//{
//	300bps（0），   600bps（1），     1200bps（2），
//		2400bps（3），  4800bps（4），    7200bps（5），
//		9600bps（6），  19200bps（7），   38400bps（8），
//		57600bps（9）， 115200bps（10）， 230400bps（11），
//		460800bps（12），自适应（255）
//}，
int TBps_Enumrate[] = {300, 600, 1200, 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200, 230400, 460800};

BYTE GetPortBpsToIndex(int nBps)
{
	int i;
	for(i=0; i < sizeof(TBps_Enumrate); i++)
	{
		if(TBps_Enumrate[i] == nBps)
			return i;
	}
	return 6;
}

int GetPortBpsFromIndex(int nBps)
{

	if(nBps >= sizeof(TBps_Enumrate))
		return UART_DEFAULT_BAUD_SUB;

	return TBps_Enumrate[nBps];
}

WORD GetRequestF20DPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	TTwoByteUnion OI;	
	WORD Lenth;
//	BYTE *pBufBak = InBuf;
//	const BYTE *Tag;
	TDevice      tDevice;

	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	if( ClassAttribute == 0x02 )
	{
		if( ClassIndex != 0 )
		{
			return 0x8000;
		}
		Lenth = 0;

		OutBuf[Lenth++] = Array_698;
		OutBuf[Lenth++] = 1;

		OutBuf[Lenth++] = Structure_698;
		OutBuf[Lenth++] = 9;

		//模组序号	         unsigned
		OutBuf[Lenth++] = Unsigned_698;
		OutBuf[Lenth++] = 1;
	
		//模块设备型号		visible-string，
		OutBuf[Lenth++] = Visible_string_698;
		OutBuf[Lenth++] = 20;
		memcpy(&OutBuf[Lenth], &ModulelistPara.Name, sizeof(ModulelistPara.Name) );
		Lenth += sizeof(ModulelistPara.Name);

		//模块设备ID		visible-string，
		OutBuf[Lenth++] = Octet_string_698;
		OutBuf[Lenth++] = 8;
		memcpy(&OutBuf[Lenth], &ModulelistPara.SerialNum, sizeof(ModulelistPara.SerialNum) );
		Lenth += sizeof(ModulelistPara.SerialNum);

		memset(&tDevice,0,sizeof(tDevice));
		memcpy(&tDevice, &DeviceVersion, sizeof(tDevice));

		//软件版本			double-long
		OutBuf[Lenth++] = Double_long_698;
		OutBuf[Lenth++] = 0;
		OutBuf[Lenth++] = 0;
#if(USER_TEST_MODE)
                OutBuf[Lenth++] = 1;
                OutBuf[Lenth++] = 0;
#else
		OutBuf[Lenth++] = (BYTE)(MODULE_VERSION);
                OutBuf[Lenth++] = (BYTE)(MODULE_VERSION >> 8);
#endif
		//软件发布日期		visible-string，
		OutBuf[Lenth++] = Visible_string_698;
		OutBuf[Lenth++] = 6;
		GetAppDateTimeString(tDevice.SoftTime);
		memset(&OutBuf[Lenth], 0, 6 );
		memcpy(&OutBuf[Lenth], &tDevice.SoftTime, sizeof(tDevice.SoftTime) );
		Lenth += 6;//sizeof(tDevice.SoftTime);

		//硬件版本			double-long，
		OutBuf[Lenth++] = Double_long_698;
		OutBuf[Lenth++] = 0;
		OutBuf[Lenth++] = 0;
		OutBuf[Lenth++] = (BYTE)(HARD_VERSION);
        OutBuf[Lenth++] = (BYTE)(HARD_VERSION >> 8);
		
		//硬件发布日期		visible-string
		OutBuf[Lenth++] = Visible_string_698;
		OutBuf[Lenth++] = 6;
		memcpy(tDevice.HardTime,HARD_TIME,6);
		memset(&OutBuf[Lenth], 0, 6 );
		memcpy(&OutBuf[Lenth], &tDevice.HardTime, sizeof(tDevice.HardTime) );
		Lenth += 6;//sizeof(tDevice.SoftTime);

		//厂商编码			visible-string，
		OutBuf[Lenth++] = Visible_string_698;
		OutBuf[Lenth++] = 8;
		memcpy(&tDevice, &DeviceVersion, sizeof(tDevice));
		memcpy(&OutBuf[Lenth], &tDevice.FactoryEx, sizeof(tDevice.FactoryEx) );
		Lenth += sizeof(tDevice.FactoryEx);

		//通道配置			array通道信息
		OutBuf[Lenth++] = Array_698;
		OutBuf[Lenth++] = 1;
		OutBuf[Lenth++] = Structure_698;
		OutBuf[Lenth++] = 2;
		OutBuf[Lenth++] = Unsigned_698;
		OutBuf[Lenth++] = 1;
		OutBuf[Lenth++] = Unsigned_698;
		OutBuf[Lenth++] = 2;
					
	}
	else if( ClassAttribute == 0x03 )
	{
		Lenth = 0;
		
		OutBuf[Lenth++] = Array_698;
		OutBuf[Lenth++] = 1;
		
		OutBuf[Lenth++] = Structure_698;
		OutBuf[Lenth++] = 2;
		
		//端口号    OAD，
		OutBuf[Lenth++] = OAD_698;
		OutBuf[Lenth++] = 0xF2;
		OutBuf[Lenth++] = 0x0D;
		OutBuf[Lenth++] = 0x02;
		OutBuf[Lenth++] = 0;

		//通信参数  COMDCB
		OutBuf[Lenth++] = COMDCB_698;
		OutBuf[Lenth++] = GetPortBpsToIndex(gdwUartBps);
		OutBuf[Lenth++] = 2;
		OutBuf[Lenth++] = 8;
		OutBuf[Lenth++] = 1;
		OutBuf[Lenth++] = 0;

	}
	else
	{
		return 0x8000;
	}
	return Lenth;

}

WORD GetRequestAppLink( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num,j;
	TTwoByteUnion OI;	
	WORD Lenth;
//	BYTE *pBufBak = InBuf;
//	const BYTE *Tag;
	int nLen=0;
	WORD wOI;
        WORD wData;
        BYTE byBuffer64[8]={0x71,0x8C,0x7C,0,0,0,0,0};
        BYTE byBuffer128[16]={0x0,0x0D,0x80,0x08,0,0,0,0};
        
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	if( ClassAttribute == 0x02 )
	{
		if( ClassIndex > GetObjNum() )
		{
			return 0x8000;
		}
		
		i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
		Num = (ClassIndex == 0) ? GetObjNum() : 1;
		Num += i;
		
		//Num = GetObjNum();

		OutBuf[nLen++] = Array_698;
		OutBuf[nLen++] = Num-i;

		for( ; i<Num; i++ )//ObjList
		{	
			OutBuf[nLen++] = Structure_698;
			OutBuf[nLen++] = 2;

			OutBuf[nLen++] = Long_unsigned_698;

			wOI = GetObjOI(i);
			OutBuf[nLen++] = wOI/0x100;//OI的高字节 OI先传高字节，
			OutBuf[nLen++] = wOI&0xff;//OI的低字节

			OutBuf[nLen++] = Structure_698;
			OutBuf[nLen++] = 2;

			OutBuf[nLen++] = Array_698;
			OutBuf[nLen++] = GetObjPropertyNum(wOI);
			for(j=0; j<GetObjPropertyNum(wOI); j++ )
			{
				OutBuf[nLen++] = Structure_698;
				OutBuf[nLen++] = 2;
				OutBuf[nLen++] = Unsigned_698;
				OutBuf[nLen++] = ObjList[i+j].Property;
				OutBuf[nLen++] = Enum_698;
				OutBuf[nLen++] = ObjList[i+j].ProPermission;
			}

			OutBuf[nLen++] = Array_698;
			OutBuf[nLen++] = GetObjMothodNum(wOI);
			for(j=0; j<GetObjMothodNum(wOI); j++ )
			{
				OutBuf[nLen++] = Structure_698;
				OutBuf[nLen++] = 2;
				OutBuf[nLen++] = Unsigned_698;
				OutBuf[nLen++] = ObjList[i+j].Mathod;
				OutBuf[nLen++] = Boolean_698;
				OutBuf[nLen++] = ObjList[i+j].ModPermission;
			}

		}
	}
	else if(ClassAttribute == 0x03)
	{
            OutBuf[nLen++] = Structure_698;
            OutBuf[nLen++] = 7;
            
            OutBuf[nLen++] = Long_unsigned_698;
			#if(PROTOCOL_VERSION == 25)
            wData = 0x1715; //25规范负荷辨识扩展协议还没经协议组评审呢，或者用0x1715。25年之前用0x1710
			#elif(PROTOCOL_VERSION == 24)
			wData = 0x1710; //25规范负荷辨识扩展协议还没经协议组评审呢，或者用0x1715。25年之前用0x1710
			#endif
            memcpy(&OutBuf[nLen], &wData, 2);
            lib_ExchangeData(&OutBuf[nLen],&OutBuf[nLen],2);
            nLen+=2;
            
            OutBuf[nLen++] = Long_unsigned_698;
            wData = MAX_PRO_BUF;
            memcpy(&OutBuf[nLen], &wData, 2);
            lib_ExchangeData(&OutBuf[nLen],&OutBuf[nLen],2);
            nLen+=2;
            
            OutBuf[nLen++] = Long_unsigned_698;
            wData = MAX_PRO_BUF;
            memcpy(&OutBuf[nLen], &wData, 2);
            lib_ExchangeData(&OutBuf[nLen],&OutBuf[nLen],2);
            nLen+=2;            
            
            OutBuf[nLen++] = Long_unsigned_698;
            wData = MAX_PRO_BUF;
            memcpy(&OutBuf[nLen], &wData, 2);
            lib_ExchangeData(&OutBuf[nLen],&OutBuf[nLen],2);
            nLen+=2;
            
            OutBuf[nLen++] = Bit_string_698;
            OutBuf[nLen++] = 64;
            memcpy(&OutBuf[nLen], &byBuffer64, 8);
            nLen+=8;
            
            
            OutBuf[nLen++] = Bit_string_698;
            OutBuf[nLen++] = 0x81;
            OutBuf[nLen++] = 128;
            memcpy(&OutBuf[nLen], &byBuffer128, 16);
            nLen+=16;
            
            OutBuf[nLen++] = NULL;
            
            Lenth = nLen;
            
            return Lenth;
	}
	else
        {
            return 0x8000;
        }
        
	 if( ClassAttribute == 0x02 )
	{
		if( ClassIndex >  GetObjNum() )
		{
			return 0x8000;
		}
		
		TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;

		Lenth = 0;
	/*	for(i=0;i<TagPara.Array_Struct_Num;i++)
		{
			Tag = T_MyVer;
			//Tag[1]=0xFF;
			Tag += GetTagOffsetAddr( ClassIndex, Tag);
			Lenth += GetTypeLen( DataType, Tag );
			if( Lenth == 0x8000 )
			{
				return 0x8000;
			}
		}*/
			Lenth = nLen;//23*3+2;
	}
	
	
	return Lenth;
}

WORD GetRequestSafetyPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,i,Num;
	TTwoByteUnion OI;	
	WORD Lenth;
//	BYTE *pBufBak = InBuf;
	const BYTE *Tag;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	int ret=0;

	if( ClassAttribute == 0x02 )
	{
		if( ClassIndex != 0 )
		{
			return 0x8000;
		}
		*(OutBuf++) = Enum_698;
		*(OutBuf++) = SafeModePara.Flag;//USE_SAFETY_MODE;
	}
	else if( ClassAttribute == 0x03 )
	{
 		if( ClassIndex > SafeModePara.Num )
 		{
 			return 0x8000;
 		}
 		
 		i = (ClassIndex == 0) ? 0 : (ClassIndex-1);
 		Num = (ClassIndex == 0) ? SafeModePara.Num : 1;
 		TagPara.Array_Struct_Num = Num;
 		Num += i;
 		
		*(OutBuf++) = Array_698;
		*(OutBuf++) = Num;
	
 		for( ; i<Num; i++ )
		{	
			*(OutBuf++) = Structure_698;
			*(OutBuf++) = 2;

			*(OutBuf++) = OI_698;
 			*(OutBuf++) = SafeModePara.Sub[i].OI/0x100;//OI的高字节 OI先传高字节，
 			*(OutBuf++) = SafeModePara.Sub[i].OI&0xff;//OI的低字节

			*(OutBuf++) = Long_unsigned_698;
 			*(OutBuf++) = SafeModePara.Sub[i].wSafeMode&0xff;//safemode的低字节  安全模式先传低字节，符合EEPROM中存储规律，jwh那边根据协议倒序下 wlk
 			*(OutBuf++) = SafeModePara.Sub[i].wSafeMode/0x100;//safemode的高字节
 		}
	}
        else if(ClassAttribute == 0x09)
        {
          	if( ClassIndex != 0 )
		{
			return 0x8000;
		}
                ret = sm4_InitKey();		// 设备 ID 写入成功 
           
        
		*(OutBuf++) = Enum_698;
		*(OutBuf++) = ret;
                return 2;
        }
	else
	{
		return 0x8000;
	}

	if( ClassAttribute == 0x02 )
	{
		if( ClassIndex != 0 )
		{
			return 0x8000;
		}
		
		Lenth = GetTypeLen( DataType, T_Enum );
		if( Lenth == 0x8000 )
		{
			return 0x8000;
		}
	}
	else if( ClassAttribute == 0x03 )
	{
 		if( ClassIndex > SafeModePara.Num )
 		{
 			return 0x8000;
 		}
		
 		TagPara.Array_Struct_Num = (ClassIndex == 0) ? SafeModePara.Num : 1;

		Lenth = 2;
		for(i=0;i<TagPara.Array_Struct_Num;i++)
		{
 			Tag = T_RemoteControl;
 			Tag += GetTagOffsetAddr( ClassIndex, Tag);
 			Lenth += GetTypeLen( DataType, Tag );
 			if( Lenth == 0x8000 )
 			{
 				return 0x8000;
 			}
		}
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}

WORD GetRequestChannelPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,byChannel;
	TTwoByteUnion OI;	
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;

	if( ClassAttribute == 0x02 )
	{
		if(OI.w == 0xFF01)//属性2
		{
			if( ClassIndex > 1 )
			{
				return 0x8000;
			}
			Lenth = GetTypeLen( eData, (BYTE *)&A_FF01_2_Status);
			if( Lenth == 0x8000 )
			{
				return 0x8000;
			}
			*(OutBuf++) = Enum_698;
			byChannel = (BYTE)GetActiveFlag();
			if(byChannel == 1)
			{
				*(OutBuf) = 0; //pbuf的0代表jlink通道
			}
			else if(byChannel == 0)
			{
				*(OutBuf) = 1; //pbuf的1代表uart通道
			}

			Lenth = 2; //tag+data
			OutBuf += Lenth;
		}
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}


//--------------------------------------------------
//功能描述: 读取参变量（class8）数据长度 OI=4XXX
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	*pOAD[in]：		OAD   
//         
//返回值:   返回数据长度 
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,Sch,ClassIndex,MaxIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Tag;
	// BYTE BUFF[20];
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	switch( ClassAttribute )
	{
		case 2://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}	
			if( OI.w == 0x4027 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			if( OI.w == 0xF402 )//下载方标志
			{
				if( ClassIndex > 1 )
				{
				//	ClassIndex = 0;
				}
				
				//Tag = ParaMeterObj[Sch].Type2;
				//Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				//Len.w = GetTypeLen( eTagData, Tag );
				Len.w = GetRequestFile_OtherLen( OI.w, ClassAttribute, ClassIndex );
			}
			if( OI.w == 0x4800 )//Len属性2
			{
				MaxIndex = GetFrzorCurLen(2);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4800_2_ApplClsID;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			#if(PROTOCOL_VERSION == 25)
			if( OI.w == 0x4801 )//Len属性2
			{
				Tag = (BYTE *)&A_4801_2_ApplCurDataUnion;
				if( ClassIndex > Tag[1] )
				{
					return 0x8000;
				}
				
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			#endif //#if(PROTOCOL_VERSION == 25)

			if( OI.w == 0x4A00 )//Len属性2
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4A00_2_Status;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			if( OI.w == 0x4A01 )//Len属性2
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4A01_2_ModStatus;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			break;
		
		case 3://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_ModuleVersion;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			if( OI.w == 0x4300 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			if( OI.w == 0x4027 )//
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}

			if( OI.w == 0x4800 )//Len属性3
			{
				MaxIndex = GetFrzorCurLen(3);
				if( ClassIndex > MaxIndex)
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4800_3_ApplCurData;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			break;
		case 4:
			if( OI.w == 0x4800 )//Len属性4
			{
				TagPara.Array_Struct_Num = NumofNILM;
				if( ClassIndex > TagPara.Array_Struct_Num)
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&A_4800_4_ApplFrzData;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen4804( eTagData, Tag ,(BYTE*)&tApplFrzDataFLASH[0]);
			}
			break;

		case 5:
			if( OI.w == 0xF001 )//下载方式标识
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				//Tag = ParaMeterObj[Sch].Type2;
				//Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				//Len.w = GetTypeLen( eTagData, Tag );
                Len.w = GetRequestFile_OtherLen( OI.w, ClassAttribute, ClassIndex );
			}
			break;
		#if(PROTOCOL_VERSION == 25)		
		case 8:
			if( OI.w == 0x4800 )//Len属性8
			{
				if( ClassIndex > 0)
				{
					return 0x8000;
				}
				Tag =  (BYTE *)&A_4800_8_FeatureData;
				TagPara.Lenth.w = FEATRUE_MSG_SIZE;
				Len.w = FEATRUE_MSG_SIZE+1+3;//256字节+4字节tag
			}	
			break;
		#endif //#if(PROTOCOL_VERSION == 25)
		default:
			return 0x8000;
	}	

	return Len.w;
}

// WORD GetRequestFileParaLen( BYTE DataType, BYTE* pOAD)
// {
// 	BYTE ClassAttribute,Sch,ClassIndex;
// 	TTwoByteUnion OI,Len;	
// 	const BYTE *Tag;
	
// 	Len.w = 0;
// 	lib_ExchangeData(OI.b,pOAD,2);
// 	ClassAttribute = (pOAD[2]&0x0f);
// 	ClassIndex = pOAD[3];
// 	Sch = SearchParaMeterOAD( OI.w );
// 	if( Sch == 0x80 )
// 	{
// 		return 0x8000;
// 	}
	
// 	switch( ClassAttribute )
// 	{
// 	case 2://属性2
// 		Len.w = 2;
// 		break;
// 		if( OI.w == 0xF001)//模组列表
// 		{
// 			if( ClassIndex > 1 )
// 			{
// 				return 0x8000;
// 			}
			
// 			Tag = ParaMeterObj[Sch].Type2;
// 			Tag +=GetTagOffsetAddr( ClassIndex, Tag);
// 			Len.w = GetTypeLen( eTagData, Tag );
// 		}
// 		break;
		
// 	case 3://属性2
// 	case 5:
// 		Len.w = 2;
// 		break;			
// 	default:
// 		return 0x8000;
// 	}	
	
// 	return Len.w;
// }

