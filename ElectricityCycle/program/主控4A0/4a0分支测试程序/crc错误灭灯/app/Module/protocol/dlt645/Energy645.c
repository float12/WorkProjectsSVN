//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2017.03.29
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
typedef enum{
	eCOMMON_ENERGY=0,	//普通电能(包含结算电能)
	eHIGH_ENERGY,  		//高精度电能
	eACCU_ENERGY,		//累计电能
	eMETER_MONEY		//电表电费
}eEnergyClass;


//-----------------------------------------------
//				全局使用的变量，常量

//-----------------------------------------------
 
//-----------------------------------------------
//				本文件使用的变量，常量

typedef struct
{
	const WORD				ID32;			//645数据标识 ID2,ID3
	const BYTE				MaxDI1Value;	//ID1数据标识的最大值
	const BYTE      		MaxDI0Value;	//ID0数据标识的最大值
	const eEnergyClass     	EnergyClass;	//电能类别
}TEnergy645Obj_t;


//增加数据类型请查看源数据和加TAG是否支持
static const TEnergy645Obj_t Energy645Obj[] =
{
//	  ID3ID2	ID1最大值				ID0最大值				电能类型
	{ 0x0000, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次组合有功总及费率电能
	{ 0x0001, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次正向有功总及费率电能
	{ 0x0002, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次反向有功总及费率电能
	{ 0x000B, 0x00, 			0x01, 				eACCU_ENERGY  	},//当前上1结算周期组合有功总累计用电量
	{ 0x000C, 0x00, 			0x01, 				eACCU_ENERGY  	},//当前上1月度组合有功总累计用电量
	{ 0x00FE, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  }, //结算时间
#if( SEL_SEPARATE_ENERGY == YES )//是否支持分相电能	
	#if( SEL_RACTIVE_ENERGY == YES )//是否支持无功电能	
	{ 0x0017, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相组合无功1电能
	{ 0x0018, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相组合无功2电能
	{ 0x0019, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相第一象限无功电能
	{ 0x001A, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相第二象限无功电能
	{ 0x001B, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相第三象限无功电能
	{ 0x001C, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相第四象限无功电能
	{ 0x002B, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相组合无功1电能
	{ 0x002C, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相组合无功2电能
	{ 0x002D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相第一象限无功电能
	{ 0x002E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相第二象限无功电能
	{ 0x002F, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相第三象限无功电能
	{ 0x0030, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相第四象限无功电能
	{ 0x003F, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相组合无功1电能
	{ 0x0040, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相组合无功2电能
	{ 0x0041, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相第一象限无功电能
	{ 0x0042, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相第二象限无功电能
	{ 0x0043, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相第三象限无功电能
	{ 0x0044, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相第四象限无功电能
	#endif
	#if( SEL_APPARENT_ENERGY == YES )//是否支持视在电能
	{ 0x001D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相正向视在电能
	{ 0x001E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相反向视在电能
	{ 0x0031, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相正向视在电能
	{ 0x0032, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相反向视在电能
	{ 0x0045, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相正向视在电能
	{ 0x0046, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相反向视在电能
	#endif
	{ 0x0015, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相正向有功电能
	{ 0x0016, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次A相反向有功电能
	{ 0x0029, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相正向有功电能
	{ 0x002A, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次B相反向有功电能
	{ 0x003D, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相正向有功电能
	{ 0x003E, 0x00, 			DLT_CLOSEING_NO, 	eCOMMON_ENERGY  	},//当前及上12次C相反向有功电能
#endif

#if( SEL_RACTIVE_ENERGY == YES )//是否支持无功电能	
	{ 0x0003, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次组合无功1总及费率电能
	{ 0x0004, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次组合无功2功总及费率电能
	{ 0x0005, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次第一象限无功总及费率电能
	{ 0x0006, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次第二象限无功总及费率电能
	{ 0x0007, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次第三象限无功总及费率电能
	{ 0x0008, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次第四象限无功总及费率电能
#endif

#if( SEL_APPARENT_ENERGY == YES )//是否支持视在电能
	{ 0x0009, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次正向视在总及费率电能
	{ 0x000A, MAX_RATIO, 		DLT_CLOSEING_NO, 	eCOMMON_ENERGY 	},//当前及上12次反向视在总及费率电能
#endif

#if( MAX_PHASE != 1 )	//单相不支持高精度电能
	{ 0x0060, MAX_RATIO, 		0x00, 				eHIGH_ENERGY  	},//当前正向有功总及费率精确电能
	{ 0x0061, MAX_RATIO, 		0x00, 				eHIGH_ENERGY  	},//当前反向有功总及费率精确电能
#endif

#if(PREPAY_MODE == PREPAY_LOCAL)//本地表需要支持剩余金额和透支金额
	{ 0x0090, 0x02,				0x01,				eMETER_MONEY	},//当前正向有功总及费率精确电能
		
#endif

};								

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  根据表格查找电能数据标识         
//参     数:WORD ID[in]	数据标识         
//返回   值  :成功：具体索引；失败：返回0x80             
//备注内容:  BYTE
//--------------------------------------------------
BYTE Search645EnergyID( BYTE *DI  )
{
	BYTE i,Num;//数组个数不能超过255
	WORD DataId;
	DataId = 0;

	DataId = (DI[3]<<8) + DI[2];
	Num = (sizeof(Energy645Obj)/sizeof(Energy645Obj[0]));
	if( Num >= 0x80 )		//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( DataId == Energy645Obj[i].ID32 )
		{
			if( (DI[1]<=Energy645Obj[i].MaxDI1Value) || ((DI[1] == 0xff)&&(Energy645Obj[i].MaxDI1Value!=0)) )//ID1满足要求
			{
				if( (DI[0] <= Energy645Obj[i].MaxDI0Value) || ((DI[0] == 0xff)&&(Energy645Obj[i].MaxDI0Value!=0)) )//ID0满足要求
				{
					return i;
				}
			}
		}
	}
	
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取普通电能数据,645入口       
//参     数:BYTE Dot[in]		小数位数       
//         BYTE *DI[in]		数据标识                       
//         WORD OutBuf[in]  返回数据        
//         BYTE Sch[in]	    数据标识索引       
//返回值     :static 获取的数据长度      
//备注内容:  
//--------------------------------------------------
static WORD Get645CommonEnergy( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	BYTE i,Num[2],Ratio[2],Lenth,MinusFlag;
	WORD EnergyType,Result,LenthRec;
	TFourByteUnion DataId,OutData;
	
	Lenth = 0;
	DataId.l = 0;
	LenthRec = 0;
	Result = TRUE;
	MinusFlag = 0;
	
	memcpy( DataId.b, DI, 4 ); 
	
	if( DI[2] <= 0x0A )//当前电能
	{
		EnergyType = (0*0x1000)+(DI[2]-0x00);
	}
	else if( DI[2] <= 0x1E )//A相电能@@hy这里的判断条件并不准确，会将DI[2]==0x0B开始的非A相电能数据标识包含，程序执行时依赖上上层函数GetRequest645Energy中的Search645EnergyID( DI )进行筛选
	{
		EnergyType = (1*0x1000)+(DI[2]-0x14);
	}
	else if( DI[2] <= 0x32 )//B相电能@@同上
	{
		EnergyType = (2*0x1000)+(DI[2]-0x28);
	}
	else if( DI[2] <= 0x46 )//C相电能@@同上
	{
		EnergyType = (3*0x1000)+(DI[2]-0x3C);
	}
	else if( (DI[2] == 0x60) || (DI[2] == 0x61) )
	{
		EnergyType = (0*0x1000)+(DI[2]-0x5F);
	}
	else if( DI[2] == 0xfe )//结算日时间
	{
		
	}
	else
	{
		return DLT645_ERR_ID_02;//其他暂不支持，返回无数据
	}
	Num[1] = ((DI[1] == 0xff) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio[1] = ((DI[1] == 0xff) ? 0 : DI[1]);
	Num[1] = (Num[1]+Ratio[1]);

	Num[0] = ((DI[0] == 0xff) ? (Energy645Obj[Sch].MaxDI0Value) : 1);
	Ratio[0] = ((DI[0] == 0xff) ? 1 : DI[0]);
	Num[0] = (Num[0]+Ratio[0]);
		
	if( DI[1]>(FPara1->TimeZoneSegPara.Ratio) && (DI[1] != 0xff) )//元素索引不能大于费率数
	{
		return DLT645_ERR_ID_02;
	}
	if( DI[0]>(Energy645Obj[Sch].MaxDI0Value ) && (DI[0] != 0xff) )//元素索引不能大于费率数
	{
		return DLT645_ERR_ID_02;
	}
	Lenth =4;
	
	if( Energy645Obj[Sch].EnergyClass == eHIGH_ENERGY )
	{
		EnergyType = ( EnergyType | 0x8000 );
		Lenth = 5;	//698高精度电能采用8字节,4位小数;645采用5字节,4位小数
	}

	if( DI[2] == 0xfe ) //1-12月结算日结算时间
	{
		if( Ratio[0] == 0x00 )
		{
			return DLT645_ERR_ID_02;
		}
		else
		{
			for(; Ratio[0] < Num[0]; Ratio[0]++ )
			{
				DataId.b[0] = Ratio[0];
				Result = api_ReadClosingRecord645( DataId.b, OutBuf );
				if( (Result == 0) || (Result >= 0x8000) )
				{
					return DLT645_ERR_ID_02;
				}
				
				OutBuf += 4; 
				LenthRec += 4; //YYMMDDHH 保留4个字节
			}
		}
	}
	else //读结算日电能
	{
		if( (DI[0] == 0x00) || (DI[0] == 0xFF) ) //读当前电能
		{
			for( ; Ratio[1]<Num[1]; Ratio[1]++ )
			{
				Result = api_GetCurrEnergyData(EnergyType, DATA_BCD, Ratio[1], Dot, OutBuf);
				if(Result == FALSE)
				{
					return DLT645_ERR_ID_02;
				}
	
				if( (Lenth != 4) && (Lenth != 5) )
				{
					return DLT645_ERR_ID_02;
				}
				
				OutBuf += Lenth;
				LenthRec += Lenth;
			}
		}
		if( DI[0] != 0x00 ) //读结算日电能 冻结处理时已经转换为BCD，此处无需转换
		{
			for( ; Ratio[0]<Num[0]; Ratio[0]++ )
			{
				DataId.b[0] = Ratio[0];
	
				Result = api_ReadClosingRecord645( DataId.b, OutBuf );
	
				if((Result == 0) || (Result >= 0x8000))
				{
					return DLT645_ERR_ID_02;
				}
				
				if( DataId.b[1] == 0xff )
				{
					Lenth = 4*( FPara1->TimeZoneSegPara.Ratio + 1 );
				}
				
				//只有Dot不等于2 才需要特殊处理
				if( Dot < 2 )
				{
					for( i = 0; i < Lenth / 4; i++ )
					{
						memcpy( OutData.b, OutBuf + 4 * i, 4 );
						
						if( OutData.b[3]&0x80 )
						{
							MinusFlag = 1;
							OutData.b[3] &= 0x7F;
						}
						//BCD一个数占4个bit位
						OutData.d >>= 4 * (2 - Dot);
						
						if( (MinusFlag == 1)&&(OutData.d != 0) )
						{
							OutData.b[3] |= 0x80;
						}
						
						memcpy( OutBuf + 4 * i, OutData.b, 4 );
					}
				}
				OutBuf += Lenth;
				LenthRec += Lenth;	
			}
		}
	}
	return LenthRec;
}
//--------------------------------------------------
//功能描述:  获取电能数据,645入口       
//参     数:    
//			Dot[in]		   小数位数       
//          *DI[in]		   数据标识                       
//          OutBufLen[in]  读取的长度       
//          OutBuf[out]	   返回数据     
//          Sch[in]	       数据标识索引       
//返回值     :static  Lenth:长度，超过0x8000认为无请求数据     
//备注内容:  
//--------------------------------------------------
static WORD Get645EnergyData( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	WORD Lenth;
	BOOL Result;

	#if( PREPAY_MODE == PREPAY_LOCAL )
	TFourByteUnion TmpMoney;
	#endif

	Lenth = 0;
	Result = TRUE;
	
	if( Energy645Obj[Sch].EnergyClass == eACCU_ENERGY) 
	{	//累计电能
		if( DI[2] == 0x0B )//当前结算周期及上1结算周期组合有功总累计用电量
		{
			Result = api_GetAccuPeriEnergy( DI[0], DATA_BCD, Dot, OutBuf );
			Lenth = 4;
		}
		else if( DI[2] == 0x0C )//当前月度及上1月度组合有功总累计用电量
		{
			Result = api_GetAccuEnergyData( DI[0], DATA_BCD, Dot, OutBuf );
			Lenth = 4;
		}
		else if( DI[2] == 0x0D )//当前及上6次年结算日记录
		{
			if( (DI[1] == 0)&&(DI[0] <= 7) )
			{
				return DLT645_ERR_ID_02;//暂时不支持
			}
			else
			{
				return DLT645_ERR_ID_02;
			}
		}
		else if( ( DI[2] == 0x60 )||( DI[2] == 0x61 ) )
		{
			Lenth = DLT645_ERR_ID_02;
		}
		
		//返回数据长度处理：组合有共总用电量4字节；年结算记录4字节结算时间+4字节电能
		if( Result == FALSE )
		{
			return DLT645_ERR_ID_02;
		}
		
	}
	else if( Energy645Obj[Sch].EnergyClass == eMETER_MONEY )
	{	
		#if(PREPAY_MODE == PREPAY_LOCAL)
		if((DI[2]==0x90)&&(DI[1]==0x02)&&((DI[0]==0x00)||(DI[0]==0x01)))//DI3 肯定等于00，只需要判断DI2~DI0即可
		{
			Lenth = api_ReadPrePayPara(eRemainMoney, (BYTE *)TmpMoney.b);
			if( Lenth == 0 )//数据长度为0，说明抄读失败.正确情况应该为4
			{	
				return DLT645_ERR_ID_02;
			}
			
			if(DI[0]==0x00)//剩余金额
			{
				if(TmpMoney.l>=0)
				{
					TmpMoney.d =lib_DWBinToBCD(TmpMoney.d);
					memcpy(OutBuf,TmpMoney.b,Lenth);
				}
				else
				{
					memset(OutBuf,0x00,Lenth);
				}
			}
			else//透支金额
			{
				if(TmpMoney.l>=0)
				{
					memset(OutBuf,0x00,Lenth);
				}
				else
				{
					TmpMoney.l *=(-1);
					TmpMoney.d =lib_DWBinToBCD(TmpMoney.d);
					memcpy(OutBuf,TmpMoney.b,Lenth);
				}
			}	
		}
		else
		{	
			return DLT645_ERR_ID_02;
		}
		#else
		return DLT645_ERR_ID_02;
		#endif
	}
	else
	{	
        //普通电能(结算电能)、高精度电能
		Lenth = Get645CommonEnergy( Dot, DI, OutBuf, Sch );				
	}
	return Lenth;
}

//--------------------------------------------------
//功能描述:  读取电能       
//参     数:    
//			Dot[in]		   小数位数       
//          *DI[in]		   数据标识                       
//          OutBufLen[in]  读取的长度       
//          OutBuf[out]	   返回数据       
//返回    值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 数据长度
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequest645Energy( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Sch;
	WORD Result,Lenth;
	BYTE EnergyOutBuf[100];	//开局部变量，保存数据

	Result = DLT645_ERR_ID_02;	
	memset( EnergyOutBuf, 0x00, 100 );

	
	//搜寻数据标识是否被支持 不支持返回
	if( DI[2]==0xff )	//id2不能为0xff
	{
		return DLT645_ERR_ID_02;
	}
	else if( (DI[0]==0xff) && (DI[1]==0xff) )	//id0与id1不能同时为0xff
	{
		return DLT645_ERR_ID_02;
	}
	
	Sch = Search645EnergyID( DI );
	if( Sch == 0x80 )
	{
		Result = DLT645_ERR_ID_02;
		return Result;
	}
	
	Lenth = Get645EnergyData( Dot, DI, EnergyOutBuf, Sch );

	//根据返回长度，确定应该返回的状态字节
	if( Lenth > 0x8000 )
	{
		Result = Lenth;		
	}
	else
	{
		if( Lenth > OutBufLen )
		{
			Result = DLT645_ERR_ID_02;
		}
		else
		{
			Result = Lenth;
			memcpy(OutBuf, EnergyOutBuf,Lenth);
		}
	}
	return Result;
}

#endif//#if( SEL_DLT645_2007 == YES )


