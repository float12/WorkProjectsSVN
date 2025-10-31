//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2018.08.13
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt645_2007.h"
#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef struct
{
	const WORD				ID21_32;		//645数据标识 ID2,ID3
	const TFourByteUnion	OAD;	        //698数据标识
	const BYTE				TableNum;	    //Table个数
	const DWORD*      		pRelationTable;	//对应的关联对象属性表
}TEvent645Obj_t;


//-----------------------------------------------
//				全局使用的变量，常量

//-----------------------------------------------
 
//-----------------------------------------------
//				本文件使用的变量，常量
static const DWORD Start_EndTime_Table[] =
{
    //2,          //个数
    0x00021e20, //事件发生时间
    0x00022020,  //事件结束时间
};


static const DWORD Adjust_Time_Table[] =
{
	//3,          //个数
	0x55555555,  //操作者代码
	//0x00220040, //校时前时间
	0x00021E20,//用事件发生前时间来代替校时前时间
	//0x00820040,//校时后时间
	0x00022020,//用事件结束时间来代替校时后时间
};
static const DWORD Relay_Table[] =
{
    //8,            //个数
    0x00021E20,   //事件发生时间
    0x55555555,   //操作者代码
    0x01221000,   //正向有功总电能    
    0x01222000,   //反向有功总电能    
    0x01225000,   //第一象限无功总电能
    0x01226000,   //第二象限无功总电能
    0x01227000,   //第三象限无功总电能
    0x01228000,   //第四象限无功总电能
};

static const DWORD Prog_Table[] =
{
    0x00021E20,   //事件发生时间
    0x55555555,   //操作者代码
    0x66666666,   //编程记录内容
};
static const DWORD Clear_Event_Table[] =
{
    0x00021E20,   //事件发生时间
    0x55555555,   //操作者代码
    0x55555555,   //事件清零数据标识码
};
static const DWORD Cover_Table[] =
{
    //14,            //个数
    0x00021E20,   //事件发生时间
    0x00022020,   //事件结束时间
    0x01221000,   //事件发生时刻正向有功总电能    
    0x01222000,   //事件发生时刻反向有功总电能    
    0x01225000,   //事件发生时刻第一象限无功总电能
    0x01226000,   //事件发生时刻第二象限无功总电能
    0x01227000,   //事件发生时刻第三象限无功总电能
    0x01228000,   //事件发生时刻第四象限无功总电能
    0x01821000,   //事件结束时刻正向有功总电能    
    0x01822000,   //事件结束时刻反向有功总电能    
    0x01825000,   //事件结束时刻第一象限无功总电能
    0x01826000,   //事件结束时刻第二象限无功总电能
    0x01827000,   //事件结束时刻第三象限无功总电能
    0x01828000,   //事件结束时刻第四象限无功总电能
};

static const DWORD Magnetic_PowerErr_Table[] =
{
    //6,            //个数
    0x00021E20,   //事件发生时间
    0x00022020,   //事件结束时间
    0x01221000,   //正向有功总电能    
    0x01222000,   //反向有功总电能    
    0x01821000,   //正向有功总电能    
    0x01822000,   //反向有功总电能    
};
static const DWORD Realy_Err_Table[] =
{
    //7,            //个数
    0x00021E20,   //事件发生时间
    0x00022020,   //事件结束时间
    0x00020F40,	  //继电器的状态-采用密钥条数-占位用-1字节
    0x01221000,   //正向有功总电能    
    0x01222000,   //反向有功总电能    
    0x01821000,   //正向有功总电能    
    0x01822000,   //反向有功总电能    
};

static const DWORD ClosingDay_Prog_Table[] =
{
	//5,            //个数
	0x00021E20,   //事件发生时间
	0x55555555,   //操作者代码
	0x01221641,   //结算日编程前每月第1结算日数据
	0x02221641,   //结算日编程前每月第2结算日数据
	0x03221641,   //结算日编程前每月第3结算日数据
};
static const DWORD Over_I_Table[] = 
{
    //49,              //个数
    0x00021E20,      //事件发生时间
    0x01221000,      //事件发生时刻正向有功总电能    
    0x01222000,      //事件发生时刻反向有功总电能    
    0x01223000,      //事件发生时刻组合无功1总电能   
    0x01224000,      //事件发生时刻组合无功2总电能   
    0x01221100,      //事件发生时刻A相正向有功电能   
    0x01222100,      //事件发生时刻A相反向有功电能   
    0x01223100,      //事件发生时刻A相组合无功1电能  
    0x01224100,      //事件发生时刻A相组合无功2电能  
    0x01220020,      //事件发生时刻A电压              
    0x01220120,      //事件发生时刻A电流              
    0x02220420,      //事件发生时刻A有功功率          
    0x02220520,      //事件发生时刻A无功功率          
    0x02220A20,      //事件发生时刻A功率因数          
    0x01221200,      //事件发生时刻B相正向有功电能   
    0x01222200,      //事件发生时刻B相反向有功电能   
    0x01223200,      //事件发生时刻B相组合无功1电能  
    0x01224200,      //事件发生时刻B相组合无功2电能  
    0x02220020,      //事件发生时刻B电压              
    0x02220120,      //事件发生时刻B电流              
    0x03220420,      //事件发生时刻B有功功率          
    0x03220520,      //事件发生时刻B无功功率          
    0x03220A20,      //事件发生时刻B功率因数          
    0x01221300,      //事件发生时刻C相正向有功电能   
    0x01222300,      //事件发生时刻C相反向有功电能   
    0x01223300,      //事件发生时刻C相组合无功1电能  
    0x01224300,      //事件发生时刻C相组合无功2电能  
    0x03220020,      //事件发生时刻C电压              
    0x03220120,      //事件发生时刻C电流              
    0x04220420,      //事件发生时刻C有功功率          
    0x04220520,      //事件发生时刻C无功功率          
    0x04220A20,      //事件发生时刻C功率因数          
    0x00022020,      //事件结束时间
    0x01821000,      //事件结束时刻正向有功总电能    
    0x01822000,      //事件结束时刻反向有功总电能    
    0x01823000,      //事件结束时刻组合无功1总电能   
    0x01824000,      //事件结束时刻组合无功2总电能   
    0x01821100,      //事件结束时刻A相正向有功电能   
    0x01822100,      //事件结束时刻A相反向有功电能   
    0x01823100,      //事件结束时刻A相组合无功1电能  
    0x01824100,      //事件结束时刻A相组合无功2电能         
    0x01821200,      //事件结束时刻B相正向有功电能   
    0x01822200,      //事件结束时刻B相反向有功电能   
    0x01823200,      //事件结束时刻B相组合无功1电能  
    0x01824200,      //事件结束时刻B相组合无功2电能          
    0x01821300,      //事件结束时刻C相正向有功电能   
    0x01822300,      //事件结束时刻C相反向有功电能   
    0x01823300,      //事件结束时刻C相组合无功1电能  
    0x01824300,      //事件结束时刻C相组合无功2电能        
};

static const DWORD Lost_V_Table[] = 
{
    //53,              //个数
    0x00021E20,      //事件发生时间
    0x01221000,      //事件发生时刻正向有功总电能    
    0x01222000,      //事件发生时刻反向有功总电能    
    0x01223000,      //事件发生时刻组合无功1总电能   
    0x01224000,      //事件发生时刻组合无功2总电能   
    0x01221100,      //事件发生时刻A相正向有功电能   
    0x01222100,      //事件发生时刻A相反向有功电能   
    0x01223100,      //事件发生时刻A相组合无功1电能  
    0x01224100,      //事件发生时刻A相组合无功2电能  
    0x01220020,      //事件发生时刻A电压              
    0x01220120,      //事件发生时刻A电流              
    0x02220420,      //事件发生时刻A有功功率          
    0x02220520,      //事件发生时刻A无功功率          
    0x02220A20,      //事件发生时刻A功率因数          
    0x01221200,      //事件发生时刻B相正向有功电能   
    0x01222200,      //事件发生时刻B相反向有功电能   
    0x01223200,      //事件发生时刻B相组合无功1电能  
    0x01224200,      //事件发生时刻B相组合无功2电能  
    0x02220020,      //事件发生时刻B电压              
    0x02220120,      //事件发生时刻B电流              
    0x03220420,      //事件发生时刻B有功功率          
    0x03220520,      //事件发生时刻B无功功率          
    0x03220A20,      //事件发生时刻B功率因数          
    0x01221300,      //事件发生时刻C相正向有功电能   
    0x01222300,      //事件发生时刻C相反向有功电能   
    0x01223300,      //事件发生时刻C相组合无功1电能  
    0x01224300,      //事件发生时刻C相组合无功2电能  
    0x03220020,      //事件发生时刻C电压              
    0x03220120,      //事件发生时刻C电流              
    0x04220420,      //事件发生时刻C有功功率          
    0x04220520,      //事件发生时刻C无功功率          
    0x04220A20,      //事件发生时刻C功率因数
    0x01622920,      //事件发生期间总安时值
    0x02622920,      //事件发生期间A安时值
    0x03622920,      //事件发生期间B安时值
    0x04622920,      //事件发生期间C安时值
    0x00022020,      //事件结束时间
    0x01821000,      //事件结束时刻正向有功总电能    
    0x01822000,      //事件结束时刻反向有功总电能    
    0x01823000,      //事件结束时刻组合无功1总电能   
    0x01824000,      //事件结束时刻组合无功2总电能   
    0x01821100,      //事件结束时刻A相正向有功电能   
    0x01822100,      //事件结束时刻A相反向有功电能   
    0x01823100,      //事件结束时刻A相组合无功1电能  
    0x01824100,      //事件结束时刻A相组合无功2电能         
    0x01821200,      //事件结束时刻B相正向有功电能   
    0x01822200,      //事件结束时刻B相反向有功电能   
    0x01823200,      //事件结束时刻B相组合无功1电能  
    0x01824200,      //事件结束时刻B相组合无功2电能          
    0x01821300,      //事件结束时刻C相正向有功电能   
    0x01822300,      //事件结束时刻C相反向有功电能   
    0x01823300,      //事件结束时刻C相组合无功1电能  
    0x01824300,      //事件结束时刻C相组合无功2电能        
};

static const DWORD Clear_Meter_Table[] = 
{
    //26,           //个数
    0x00021E20,     //事件发生时间
    0x55555555,		//操作者代码
    0x01221000,     //事件发生时刻正向有功总电能    
    0x01222000,     //事件发生时刻反向有功总电能        
    0x01225000,   	//事件发生时刻第一象限无功总电能
    0x01226000,  	//事件发生时刻第二象限无功总电能
    0x01227000,   	//事件发生时刻第三象限无功总电能
    0x01228000,  	//事件发生时刻第四象限无功总电能
    0x01221100,     //事件结束时刻A相正向有功电能   
    0x01222100,     //事件结束时刻A相反向有功电能 
    0x01225100,   	//事件发生时刻A相第一象限无功总电能
    0x01226100,  	//事件发生时刻A相第二象限无功总电能
    0x01227100,   	//事件发生时刻A相第三象限无功总电能
    0x01228100,  	//事件发生时刻A相第四象限无功总电能
    0x01221200,     //事件结束时刻B相正向有功电能   
    0x01222200,     //事件结束时刻B相反向有功电能 
    0x01225200,   	//事件发生时刻B相第一象限无功总电能
    0x01226200,  	//事件发生时刻B相第二象限无功总电能
    0x01227200,   	//事件发生时刻B相第三象限无功总电能
    0x01228200,  	//事件发生时刻B相第四象限无功总电能
    0x01221300,     //事件结束时刻C相正向有功电能   
    0x01222300,     //事件结束时刻C相反向有功电能 
    0x01225300,   	//事件发生时刻C相第一象限无功总电能
    0x01226300,  	//事件发生时刻C相第二象限无功总电能
    0x01227300,   	//事件发生时刻C相第三象限无功总电能
    0x01228300,  	//事件发生时刻C相第四象限无功总电能
};

//增加数据类型请查看源数据和加TAG是否支持
#define EVENT645_03NUM          SearchEvent645DI3Is03Num()

static const TEvent645Obj_t Event645Obj[] =
{
    //ID2ID1        698数据标识	      Table个数   对应的关联对象属性表
    #if( SEL_EVENT_LOST_POWER == YES )//掉电事件
	{ 0x1100,       0x30110200,		sizeof(Start_EndTime_Table)/sizeof(DWORD),		Start_EndTime_Table		},
	#endif
	#if( SEL_PRG_INFO_CLEAR_METER == YES )//电表清零
	{ 0x3001,		0x30130200,		sizeof(Clear_Meter_Table)/sizeof(DWORD), 		Clear_Meter_Table		},
	#endif
	#if( SEL_PRG_INFO_ADJUST_TIME == YES )//校时事件
	{ 0x3004,		0x30160200,		sizeof(Adjust_Time_Table)/sizeof(DWORD), 		Adjust_Time_Table		},
	#endif
	#if( SEL_EVENT_METERCOVER == YES )//开表盖事件
	{ 0x300D,       0x301B0200,     sizeof(Cover_Table)/sizeof(DWORD),    			Cover_Table             },
	#endif
    #if( SEL_EVENT_BUTTONCOVER == YES )//开端纽盖事件 三相
    { 0x300E,       0x301C0200,     sizeof(Cover_Table)/sizeof(DWORD),    			Cover_Table             },
	#endif
    #if( SEL_EVENT_MAGNETIC_INT == YES )//恒定磁场干扰 三相
    { 0x3500,       0x302A0200,     sizeof(Magnetic_PowerErr_Table)/sizeof(DWORD),  Magnetic_PowerErr_Table	},
	#endif
    #if( SEL_EVENT_POWER_ERR == YES )//电源异常 三相
    { 0x3700,       0x302C0200,     4,												Magnetic_PowerErr_Table	},
	#endif
	#if( SEL_EVENT_RELAY_ERR == YES )//负荷开关误动作
	{0x3600,		0x302B0200,		sizeof(Realy_Err_Table)/sizeof(DWORD),			Realy_Err_Table			},
	#endif
	#if( SEL_PRG_INFO_CLEAR_EVENT == YES )//事件清零事件
	{0x3003,		0x30150200,		sizeof(Clear_Event_Table)/sizeof(DWORD),		Clear_Event_Table		},
	#endif
	#if( SEL_PRG_INFO_CLOSING_DAY == YES )//结算日编程事件
	{0x300C,		0x301A0200,		sizeof(ClosingDay_Prog_Table)/sizeof(DWORD),	ClosingDay_Prog_Table	},
	#endif

    {0xFFFF,		0xFFFFFFFF,		sizeof(Start_EndTime_Table)/sizeof(DWORD),	    Start_EndTime_Table	    },
	//以上事件是DI3为03的、不支持分项抄读的事件，数据与 EVENT645_03NUM 严格对应 增减数量需要与 EVENT645_03NUM 同步
    
    //ID3ID2        698数据标识	       Table个数   对应的关联对象属性表
    #if( SEL_EVENT_LOST_V == YES )//失压事件 三相
    { 0x1000,       0x30000700,     sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //A相失压事件	
    { 0x1001,       0x30000700,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //B相失压事件
    { 0x1002,       0x30000800,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
    //C相失压事件
    { 0x1003,       0x30000900,		sizeof(Lost_V_Table)/sizeof(DWORD),    			Lost_V_Table            },
	#endif
	
    #if( SEL_EVENT_LOST_I == YES ) //A相失流事件 三相
    { 0x1801,       0x30040700,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //B相失流事件
    { 0x1802,       0x30040800,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //C相失流事件
    { 0x1803,       0x30040900,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
	#endif
	
	#if( SEL_EVENT_OVER_I == YES )
    //A相过流事件
    { 0x1901,       0x30050700,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //B相过流事件
    { 0x1902,       0x30050800,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
    //C相过流事件
    { 0x1903,       0x30050900,		sizeof(Over_I_Table)/sizeof(DWORD),    			Over_I_Table            },
	#endif

    //跳闸事件
    { 0x1D00,       0x301F0200,		sizeof(Relay_Table)/sizeof(DWORD),				Relay_Table             },
    //合闸事件
    { 0x1E00,       0x30200200,		sizeof(Relay_Table)/sizeof(DWORD),				Relay_Table              },

};								

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern BOOL RecordJudgePhase( WORD OI, BYTE ClassAttribute, TDLT698RecordPara *DLT698RecordPara);


//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述: 获得Event645Obj 中， DI3 = 03开头的事件个数
//
//参数: 无
//
//返回值: 获得Event645Obj 中， DI3 = 03开头的事件个数 
//
//备注: DI3 = 03开头的事件用DI2DI1填充Event645Obj中的ID21_32
//      其他用DI3DI2填充，因此ID21_32可能会有重复，需要区分搜索区间
//--------------------------------------------------
static BYTE SearchEvent645DI3Is03Num(void)
{
    BYTE i, Ret = 0;

    for (i = 0; i < (sizeof(Event645Obj) / sizeof(Event645Obj[0])); i++)
    {
        if(Event645Obj[i].OAD.d == 0xFFFFFFFF)
        {
            break;
        }
        Ret++;
    }

    return Ret;
}

//--------------------------------------------------
//功能描述:  根据表格查找事件数据标识         
//参     数:	BYTE Mode[in] 0x00:0x03数据标识的事件       0x55:0x10开头的数据标识事件
//			WORD ID[in]	数据标识         
//返回   值  :成功：具体索引；失败：返回0x80             
//备注内容:  BYTE
//--------------------------------------------------
BYTE Search645EventID( BYTE Mode, WORD DataId )
{
	BYTE i,Num,Index;//数组个数不能超过255

	if( Mode == 0x00 )
	{
		Num = EVENT645_03NUM;
		if( Num >= 0x80 )		//避免死循环
		{
			return 0x80;
		}

		for(i=0; i<Num; i++)
		{
			if( DataId == Event645Obj[i].ID21_32 )
			{
		        return i;
			}
		}
	}
	else
	{
		Num = (sizeof(Event645Obj)/sizeof(Event645Obj[0]));
		if( (Num >= 0x80) ||(Num < EVENT645_03NUM) )//避免死循环
		{
			return 0x80;
		}
		
		for (i=EVENT645_03NUM; i<Num; i++)
		{
			if( DataId == Event645Obj[i].ID21_32 )
			{
		        return i;
			}
		}
	}

	return 0x80;
}

//--------------------------------------------------
//功能描述:     获取关联对象属性表     
//参     数:
//          Sch[in]	    表格偏移   
//          Index[in]   关系对象属性偏移
//          pBuf[out]   回填数据
//返回值    
//备注内容:     WORD  数据长度 0x8000代表数据出错
//--------------------------------------------------
WORD GetRelationTable( BYTE Sch, BYTE Index, BYTE* pBuf )
{
    BYTE Num;
    WORD Len;
    
    Num = (sizeof(Event645Obj)/sizeof(Event645Obj[0]));
    if( Sch > Num )
    {
        return 0x8000;
    }

    if( Index == 0xff )
    {
        Len = sizeof(DWORD)*Event645Obj[Sch].TableNum;
        if( Len > ((MAX_EVENT_OAD_NUM+40)*4+50))//风险防护
        {
			return 0x8000;
        }
        
        memcpy( pBuf, (BYTE*)Event645Obj[Sch].pRelationTable, Len );//填充开始关联对象属性表
    }
    else
    {
        Len = 4;
        
        //Index判断 
        if( (Index > Event645Obj[Sch].TableNum) ||( Index == 0) )
        {
            return 0x8000;
        }
        
        memcpy( pBuf, (BYTE*)&Event645Obj[Sch].pRelationTable[Index-1], sizeof(DWORD) );

    }
   
    return (Len/4);
}

//--------------------------------------------------
//功能描述:     读取645事件接口函数     
//参     数:
//          pDI[in]	    数据标识   
//          OutBuf[in]  输入Buf
//返回值    
//备注内容:     WORD  数据长度 0x8000代表数据出错
//--------------------------------------------------
WORD api_ReadEventRecord645( BYTE* pDI , BYTE* OutBuf )
{
    TTwoByteUnion DI21_32,OI;
    BYTE Sch,ClassAttribute,Type;
    TDLT698RecordPara DLT698RecordPara;
	BYTE Buf[(MAX_EVENT_OAD_NUM+40)*4+50];
	WORD Result,Len;
	
    Len = 0;
    
    if( pDI[3] == 0X03 )
    {
    	Type = 0x00;
        DI21_32.w = ((WORD)(pDI[2]<<8)+pDI[1]);
        #if(SEL_PRG_RECORD645 == YES)
        //645编程记录没有采用698的数据源，所以在此单独处理
        if( (pDI[2] == 0x30)&&(pDI[1] == 0x00) )
        {
            Len = api_ReadProgramRecord645( pDI, OutBuf );
            return Len; 
        }
        #endif
    }
    else
    {
    	Type = 0x55;
        DI21_32.w = ((WORD)(pDI[3]<<8)+pDI[2]);
    }

    Sch = Search645EventID( Type, DI21_32.w );//进行表格查找
    if( Sch == 0x80 )//未找到数据
    {
        return 0x8000;
    }
    
	OI.w = Event645Obj[Sch].OAD.w[1];
	ClassAttribute = Event645Obj[Sch].OAD.b[1];     //获取属性索引
    Result = RecordJudgePhase( OI.w, ClassAttribute, &DLT698RecordPara);
    if( Result == FALSE )
    {
        return 0x8000;
    }

    if( pDI[3] == 0x03 )
    {
        if( pDI[0] == 0x00 )//事件总次数
        {
           Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 1, OutBuf );
        }
        else//读取记录
        {
            if( pDI[0] == 0xff )
            {
                return 0x8000;
            }
            
            Result = GetRelationTable( Sch, 0xff, Buf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }
            
            DLT698RecordPara.OI = OI.w;                       //OI数据
            DLT698RecordPara.Ch = 0;                        //不清上报 固定通道传输
            DLT698RecordPara.pOAD = Buf;                    //pOAD指针
            DLT698RecordPara.OADNum = Result;               //OAD个数
            DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
            DLT698RecordPara.TimeOrLast = pDI[0];           //绝对时间数
            
            Len = api_ReadEvent645RecordByNo((TDLT698RecordPara *)&DLT698RecordPara, MAX_APDU_SIZE, OutBuf );

        }
    }
    else//0x1x开头的事件
    {
        if( DI21_32.w == 0x1000 )//失压事件
        {
            DLT698RecordPara.Phase = ePHASE_ALL;
            
            if( pDI[1] == 0x00 )//失压总次数、失压累计时间
            {
                if( (pDI[0] == 0x01) ||(pDI[0] == 0x02))
                {
                    Len = api_ReadCurrentRecordTable645(OI.w, DLT698RecordPara.Phase, pDI[0], OutBuf );
                }
                else
                {
                    return 0x8000;
                }
            }
            else if( (pDI[1] == 0x01) || (pDI[1] == 0x02) )//最近1次失压发生时刻、结束时刻 
            {
                if( pDI[0] == 0x01 )
                {
                    Len = api_ReadCurrentRecordTable645(OI.w, DLT698RecordPara.Phase, pDI[1]+2, OutBuf );
                }
            }
            else
            {
                return 0x8000;
            }
        }
        else if( pDI[1] == 0x00 )//事件总次数、累计时间
        {
            if( pDI[0] == 1 )//读取事件总次数
            {
                Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 1, OutBuf );
            }
            else if( pDI[0] == 2 )//读取累计时间
            {
                Len = api_ReadCurrentRecordTable645( OI.w,DLT698RecordPara.Phase, 2, OutBuf );
            }
            else
            {
                Len = 0x8000;
            } 
        }
        else//读取记录
        {
            if( pDI[0] == 0xff )
            {
                return 0x8000;
            }
            
            Result = GetRelationTable( Sch, pDI[1], Buf );
            if( Result == 0x8000 )
            {
                return 0x8000;
            }
            
            DLT698RecordPara.OI = OI.w;                       //OI数据
            DLT698RecordPara.Ch = 0;                        //不清上报 固定通道传输
            DLT698RecordPara.pOAD = Buf;                    //pOAD指针
            DLT698RecordPara.OADNum = Result;               //OAD个数
            DLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;   //选择次数
            DLT698RecordPara.TimeOrLast = pDI[0];           //绝对时间数

            Len = api_ReadEvent645RecordByNo((TDLT698RecordPara *)&DLT698RecordPara, MAX_APDU_SIZE, OutBuf );
            
        }
    }

    return Len;
} 

#endif//#if( SEL_DLT645_2007 == YES )


