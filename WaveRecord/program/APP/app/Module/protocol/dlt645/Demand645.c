//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	李根科
//创建日期	2017.09.22
//描述		645协议读取需量
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt645_2007.h"
#if( SEL_DLT645_2007 == YES )
#if( MAX_PHASE == 3 )//只有三相表才打开需量功能
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
 
//-----------------------------------------------
//				本文件使用的变量，常量
typedef struct
{
	const WORD				ID32;			//645数据标识 ID2,ID3
	const BYTE				MaxDI1Value;	//ID1数据标识的最大值
	const BYTE      		MaxDI0Value;	//ID0数据标识的最大值
}TDemand645Obj_t;


//增加数据类型请查看源数据和加TAG是否支持
static const TDemand645Obj_t TDemand645Obj[] =
{
//	  ID3ID2	ID1最大值				ID0最大值				电能类型	
	{ 0x0101, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次正向有功总最大需量及发生时间
	{ 0x0102, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次反向有功总最大需量及发生时间

//不支持分相需量
//#if( SEL_REACTIVE_DEMAND == YES )//是否支持分相需量	
//	#if( SEL_REACTIVE_DEMAND == YES )//是否支持无功需量	
//	{ 0x0117, 0x00, 			DLT_CLOSEING_NO 	},//当前及上12次A相组合无功1需量
//	{ 0x0118, 0x00, 			DLT_CLOSEING_NO 	},//当前及上12次A相组合无功2需量
//	{ 0x0119, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相第一象限无功需量
//	{ 0x011A, 0x00, 			DLT_CLOSEING_NO 	},//当前及上12次A相第二象限无功需量
//	{ 0x011B, 0x00, 			DLT_CLOSEING_NO		},//当前及上12次A相第三象限无功需量
//	{ 0x011C, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相第四象限无功需量
//	{ 0x012B, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相组合无功1需量
//	{ 0x012C, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相组合无功2需量
//	{ 0x012D, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相第一象限无功需量
//	{ 0x012E, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相第二象限无功需量
//	{ 0x012F, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相第三象限无功需量
//	{ 0x0130, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相第四象限无功需量
//	{ 0x013F, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相组合无功1需量
//	{ 0x0140, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相组合无功2需量
//	{ 0x0141, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相第一象限无功需量
//	{ 0x0142, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相第二象限无功需量
//	{ 0x0143, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相第三象限无功需量
//	{ 0x0144, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相第四象限无功需量
//	#endif
//	#if( SEL_APPARENT_DEMAND == YES )//是否支持视在需量
//	{ 0x011D, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相正向视在需量
//	{ 0x011E, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相反向视在需量
//	{ 0x0131, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相正向视在需量
//	{ 0x0132, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相反向视在需量
//	{ 0x0145, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相正向视在需量
//	{ 0x0146, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相反向视在需量
//	#endif
//	{ 0x0115, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相正向有功需量
//	{ 0x0116, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次A相反向有功需量
//	{ 0x0129, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相正向有功需量
//	{ 0x012A, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次B相反向有功需量
//	{ 0x013D, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相正向有功需量
//	{ 0x013E, 0x00, 			DLT_CLOSEING_NO  	},//当前及上12次C相反向有功需量
//#endif

#if( SEL_REACTIVE_DEMAND == YES )//是否支持无功需量	
	{ 0x0103, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次组合无功1总最大需量及发生时间
	{ 0x0104, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次组合无功2总最大需量及发生时间
	{ 0x0105, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次第一象限无功总最大需量及发生时间
	{ 0x0106, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次第二象限无功总最大需量及发生时间
	{ 0x0107, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次第三象限无功总最大需量及发生时间
	{ 0x0108, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次第四象限无功总最大需量及发生时间
#endif

#if( SEL_APPARENT_DEMAND == YES )//是否支持视在需量
	{ 0x0109, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次正向视在总最大需量及发生时间
	{ 0x010A, MAX_RATIO, 	DLT_CLOSEING_NO 	},//当前及上12次反向视在总最大需量及发生时间
#endif
};								

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  根据表格查找需量数据标识         
//参     数:WORD ID[in]	数据标识         
//返回   值  :成功：具体索引；失败：返回0x80             
//备注内容:  BYTE
//--------------------------------------------------
BYTE Search645DemandID( BYTE *DI  )
{
	BYTE i,Num;//数组个数不能超过255
	WORD DataId;
	DataId = 0;

	DataId = (DI[3]<<8) + DI[2];
	Num = (sizeof(TDemand645Obj)/sizeof(TDemand645Obj[0]));
	if( Num >= 0x80 )		//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( DataId == TDemand645Obj[i].ID32 )
		{
			if( (DI[1]<=TDemand645Obj[i].MaxDI1Value) || ((DI[1] == 0xff)&&(TDemand645Obj[i].MaxDI1Value!=0)) )//ID1满足要求
			{
				if( (DI[0] <= TDemand645Obj[i].MaxDI0Value) || ((DI[0] == 0xff)&&(TDemand645Obj[i].MaxDI0Value!=0)) )//ID0满足要求
				{
					return i;
				}
			}
		}
	}

	if( i == (sizeof(TDemand645Obj)/sizeof(TDemand645Obj[0])) )		//未找到
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取普通需量数据,645入口       
//参     数:BYTE Dot[in]		小数位数       
//         BYTE *DI[in]		数据标识                       
//         WORD OutBuf[in]  返回数据        
//         BYTE Sch[in]	    数据标识索引       
//返回值     :static 获取的数据长度 
//返回值     :static  Lenth:长度，超过0x8000认为无请求数据 
//备注内容:  
//--------------------------------------------------
static WORD Get645CommonDemand( BYTE Dot, BYTE *DI, BYTE *OutBuf, BYTE Sch )
{
	BYTE Num[2],Ratio[2],Lenth;
	WORD EnergyType,Result,LenthRec;
	TFourByteUnion DataId,DemandData;
	
	Lenth = 0;
	DataId.l = 0;
	DemandData.l = 0;
	LenthRec = 0;
	Result = TRUE;
	
	memcpy( DataId.b, DI, 4 ); 
	
	if( DI[2] <= 0x0A )//当前需量
	{
		EnergyType = (0*0x1000)+(DI[2]-0x00);
	}
	else if( DI[2] <= 0x1E )//A相需量
	{
		EnergyType = (1*0x1000)+(DI[2]-0x14);
	}
	else if( DI[2] <= 0x32 )//B相需量
	{
		EnergyType = (2*0x1000)+(DI[2]-0x28);
	}
	else if( DI[2] <= 0x46 )//C相需量
	{
		EnergyType = (3*0x1000)+(DI[2]-0x3C);
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	Num[1] = ((DI[1] == 0xff) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	Ratio[1] = ((DI[1] == 0xff) ? 0 : DI[1]);
	Num[1] = (Num[1]+Ratio[1]);

	Num[0] = ((DI[0] == 0xff) ? (TDemand645Obj[Sch].MaxDI0Value) : 1);
	Ratio[0] = ((DI[0] == 0xff) ? 1 : DI[0]);		
	Num[0] = (Num[0]+Ratio[0]);
		
	if( DI[1]>(FPara1->TimeZoneSegPara.Ratio) && (DI[1] != 0xff) )//元素索引不能大于费率数
	{
		return DLT645_ERR_ID_02;
	}
	if( DI[0]>(TDemand645Obj[Sch].MaxDI0Value ) && (DI[0] != 0xff) )//元素索引不能大于费率数
	{
		return DLT645_ERR_DATA_01;
	}
	Lenth = 8; //最大需量及发生时间


	if( (DI[0]==0x00) || (DI[0]==0xFF) )//读当前需量
	{
		for( ; Ratio[1]<Num[1]; Ratio[1]++ )
		{
			Result = api_GetDemand(EnergyType, DATA_HEXCOMP, Ratio[1], Dot, OutBuf);
			if(Result == FALSE)
			{
				return DLT645_ERR_ID_02;
			}

			//转换最大需量XX.XXXX
			memcpy( DemandData.b, OutBuf,4 );

			ProBinToBCD645( eCOMB_MODE, 3, DemandData.b);	////数据返回错误 也要返回错误数据 不能返回无请求数据


			memcpy( OutBuf, DemandData.b, 3 );				
	
			DemandData.l = 0;
			//转换发生时间，YYMMDDhhmm
			memcpy( DemandData.b, OutBuf+4, 2 );
			Result = ProBinToBCD( eCOMB_MODE, 2, DemandData.b );
			if(Result == 0)
			{
				return DLT645_ERR_ID_02;
			}
			else
			{
				memcpy( OutBuf+3, DemandData.b, 1 );
			}

			DemandData.l = 0;
			memcpy( DemandData.b, OutBuf+6, 4 );
			Result = ProBinToBCD( eBYTE_MODE, 4, DemandData.b );
			if(Result == 0)
			{
				return DLT645_ERR_ID_02;
			}
			else
			{
				memcpy( OutBuf+4, DemandData.b, 4 );
			}
			lib_InverseData( OutBuf+3, 5 );
			OutBuf += Lenth;
			LenthRec += Lenth;
		}
	}
	// if( DI[0] != 0x00 ) //读结算日需量
	// {
	// 	for( ; Ratio[0]<Num[0]; Ratio[0]++ )
	// 	{
	// 		DataId.b[0] = Ratio[0];
	// 		Result = api_ReadClosingRecord645(DI, OutBuf);
	// 		if((Result == 0) || (Result >= 0x8000))
	// 		{
	// 			return DLT645_ERR_ID_02;
	// 		}
	// 		else
	// 		{
	// 			Lenth = Result;
	// 		}

	// 		if( DataId.b[1] == 0xff )
	// 		{
	// 			Lenth = 8*(FPara1->TimeZoneSegPara.Ratio + 1 );
	// 		}
	// 		OutBuf += Lenth;
	// 		LenthRec += Lenth;
			
	// 	}
	// }
	return LenthRec;
}

//--------------------------------------------------
//功能描述:  读取需量       
//参     数:
//			Dot[in]		   小数位数       
//          *DI[in]		   数据标识                       
//          OutBufLen[in]  读取的长度       
//          OutBuf[out]	   返回数据       
//返回    值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度    
//备注内容:  无
//--------------------------------------------------
WORD GetRequest645Demand( BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Sch;
	WORD Result,Lenth;
	BYTE EnergyOutBuf[200];	//开局部变量，保存数据

	Result = DLT645_OK_00;	
	memset( EnergyOutBuf, 0x00, 200 );

	
	//搜寻数据标识是否被支持 不支持返回
	if( DI[2]==0xff )	//id2不能为0xff
	{
		return DLT645_ERR_ID_02;
	}
	else if( (DI[0]==0xff) && (DI[1]==0xff) )	//id0与id1不能同时为0xff
	{
		return DLT645_ERR_ID_02;
	}
	
	Sch = Search645DemandID( DI );
	if( Sch == 0x80 )
	{
		Result = DLT645_ERR_ID_02;
		return Result;
	}
	
	
	Lenth = Get645CommonDemand( Dot, DI, EnergyOutBuf, Sch ); 

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
			memcpy(OutBuf, EnergyOutBuf, Lenth);
		}
	}
	return Result;
}

#endif//#if( MAX_PHASE == 3 )

#endif//#if ( SEL_DLT645_2007 == YES)


