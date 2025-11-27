//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人    尹西科
//创建日期  2017年9月6日09:23:07
//描述      645规约获取数据
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#if( SEL_DLT645_2007 == YES )
//---------------------------------------------------------------
//函数功能: 通信专用HEX转BCD
//
//参数: 
//		  Type[in]
// 			  eBYTE_MODE - 单字节模式
//			  eCOMB_MODE - 组合模式 比如长度为2 两个BYTE组合成一个WORD
// 		  Lenth[in] -  传入数据长度
// 		  Buf[in] - 输入缓存
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL ProBinToBCD( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf )
{
	BYTE i;
	TFourByteUnion CoverBuf;
	
    CoverBuf.l = 0;
    
	if( Type == eCOMB_MODE )
	{
		if(  Lenth > 4  ) //组合模式最大支持4个字节
		{
			return FALSE;
		}
		
		memcpy( CoverBuf.b, Buf, Lenth );
		
		switch( Lenth )
		{
			case 1:
				if( CoverBuf.b[0] > 99 )
				{
					return FALSE;
				}
				CoverBuf.b[0] = lib_BBinToBCD( CoverBuf.b[0] );
				break;
			case 2:
				if( CoverBuf.w[0] > 9999 )
				{
					return FALSE;
				}
				CoverBuf.w[0] = lib_WBinToBCD( CoverBuf.w[0] );
				break;
			case 4:
				if( CoverBuf.d > 99999999 )
				{
					return FALSE;
				}
				CoverBuf.d = lib_DWBinToBCD( CoverBuf.d );
				break;
			default:
				return FALSE;		
		}
		memcpy( Buf, CoverBuf.b, Lenth );
	}
	else
	{
		for( i = 0; i < Lenth; i++ )
		{
			if( Buf[i] > 99 )
			{
				return FALSE;
			}
			Buf[i] = lib_BBinToBCD( Buf[i] );
		}
	}
	
	return TRUE;
}

//---------------------------------------------------------------
//函数功能: 通信专用HEX转BCD 一般是组合模式
//
//参数: 
//		  Type[in]
// 			  eBYTE_MODE - 单字节模式
//			  eCOMB_MODE - 组合模式 比如长度为2 两个BYTE组合成一个WORD
// 		  Lenth[in] -  传出数据长度
// 		  Buf[in] - 输入缓存
//                   
//返回值:  TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
BOOL ProBinToBCD645( eTYPE_COVER_DATA Type, BYTE Lenth, BYTE *Buf )
{
	BOOL Result;
	// BYTE i,BackFlag;
	BYTE BackFlag;
	TFourByteUnion CoverBuf;
	TEightByteUnion llCoverBuf;
	
	BackFlag = 0;
	
	if( Lenth > 4 )	//高精度电能
	{
		llCoverBuf.ll = 0;
		CoverBuf.l = 0;
		
		memcpy( llCoverBuf.b, Buf, 8);
		if( llCoverBuf.ll < 0 )
		{
			BackFlag = 1;
			llCoverBuf.ll *= -1;
		}
		
		CoverBuf.l = (llCoverBuf.ll % 100000000);

		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			memcpy( Buf, CoverBuf.b, 4);
		}

		CoverBuf.l = (llCoverBuf.ll/100000000);		
		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			if( BackFlag == 1 )
			{
				CoverBuf.b[Lenth-1-4] |= 0x80;
			}
			memcpy( Buf+4, CoverBuf.b, Lenth-4);
		}
		
	}
	else
	{
	    if( Lenth == 0 )
	    {
            return FALSE;
	    }
	    
		CoverBuf.l = 0;
		memcpy( CoverBuf.b, Buf,4 );
		
		if( CoverBuf.l < 0 )
		{
			BackFlag = 1;
			CoverBuf.l *= -1;
		}
		
		Result = ProBinToBCD( eCOMB_MODE, 4, CoverBuf.b);
		if(Result == 0)
		{
			return FALSE;
		}
		else
		{
			if( BackFlag == 1 )
			{
				CoverBuf.b[Lenth-1] |= 0x80;
			}
			memcpy( Buf, CoverBuf.b, Lenth );				
		}
	}
	
	return TRUE;
}

//---------------------------------------------------------------
//函数功能: 获取负荷记录数据
//
//参数: 
//			PortType[in]-通讯口
//		    DI[in]     - 传入数据标识
//		    OutBufLen  - 传入缓存长度	
// 			OutBuf[out]- 输出数据缓存                  
//返回值:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//备注:   
//---------------------------------------------------------------
// WORD GetRequest645Lpf( eSERIAL_TYPE PortType, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
// {
// 	WORD Result,Lenth;
// 	BYTE FollowFlag;
	
// 	FollowFlag =0;
// 	Result = DLT645_OK_00;	
// 	Lenth = api_ReadLpfRecord645( PortType, gPr645[PortType].byReceiveControl , DI, gPr645[PortType].pMessageAddr+15, gPr645[PortType].pMessageAddr[14], OutBuf );

// 	//根据返回长度，确定应该返回的状态字节
// 	if( Lenth == 0x8000 ) //返回0x8000 说明数据出错
// 	{
// 		Result = DLT645_ERR_ID_02;
// 	}
// 	else
// 	{
// 	    if(Lenth&BIT14)
// 	    {
//             FollowFlag = 0X55;
// 	    }
	    
// 	    Lenth = Lenth&(~BIT14);
	    
// 		if( Lenth > OutBufLen )
// 		{
// 			Result = DLT645_ERR_ID_02;			
// 		}
// 		else
// 		{
// 			Result = Lenth;
// 			if( gPr645[PortType].pMessageAddr[8] == 0x11 )//读数据命令11H
// 			{	
// 			    if( FollowFlag == 0X55 )
// 			    {
// 			        DLT645APPFollow[PortType].FollowFlag = eLOAD_FOLLOW;
// 			    }
// 			}
// 			else
// 			{
//                 if( FollowFlag == 0 )
//                 {
//                     DLT645APPFollow[PortType].FollowFlag = eLOAD_FOLLOW_END;
//                 }

// 			}
// 		}
// 	}
// 	return Result;
// }

//---------------------------------------------------------------
//函数功能: 获取事件类数据
//
//参数: 
//		    DI[in]      - 传入数据标识
//		    OutBufLen   - 传入缓存长度	 
// 			OutBuf[out] - 输出数据缓存                 
//返回值:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//备注:    
//---------------------------------------------------------------
// WORD GetRequest645EventData( BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
// {
// 	WORD Result,Lenth;
// 	TFourByteUnion DataID;
	
// 	Result = DLT645_OK_00;	

// 	//复制数据标识
// 	memcpy( DataID.b, DI, 4 );

// 	Lenth = api_ReadEventRecord645( DataID.b, OutBuf );

// 	//根据返回长度，确定应该返回的状态字节
// 	if( Lenth == 0x8000 ) //返回0x8000 说明数据出错
// 	{
// 		Result = DLT645_ERR_ID_02;
// 	}
// 	else
// 	{
// 		if( Lenth > OutBufLen )
// 		{
// 			Result = DLT645_ERR_ID_02;
// 		}
// 		else
// 		{
// 			Result = Lenth;
// 		}
// 	}
// 	return Result;
// }

//---------------------------------------------------------------
//函数功能: 获取冻结类数据
//
//参数: 
//		    DI[in]     - 传入数据标识
//		    OutBufLen  - 传入缓存长度	 
// 			OutBuf	   - 输出数据缓存                 
//返回值:  
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//备注:   
//---------------------------------------------------------------
// WORD GetRequest645FreezeData( BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
// {
// 	WORD Lenth;
	
// 	Lenth = api_ReadFreezeRecord645( DI, OutBuf );
// 	if( Lenth >= 0x8000 )
// 	{
// 		return DLT645_ERR_ID_02;
// 	}
// 	else
// 	{
// 		return Lenth;
// 	}
// }

//---------------------------------------------------------------
//函数功能: 获取某个数据标识对应的数据
//
//参数:
//			Dot[in]        - 获取电能小数位数
//		    ID[in]         - 传入数据标识
//		    OutBufLen[in]  - 传入缓存长度
// 			OutBuf[out]    - 传入缓存
//
//返回值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 返回数据长度
//
//备注:
//---------------------------------------------------------------
// WORD GetPro645IDData( eSERIAL_TYPE PortType,BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
// {
// 	WORD wResult;
	
// 	if( OutBuf == NULL )
// 	{
// 		return DLT645_ERR_ID_02;
// 	}

// 	wResult = DLT645_ERR_ID_02;
	
// 	switch( DI[3] )
// 	{
// 		case DLT645_ENERGY_00H://电能类
// 			wResult = GetRequest645Energy( Dot, DI, OutBufLen, OutBuf );
// 			break;
// 		#if( MAX_PHASE == 3 )
// 		case DLT645_DEMAND_01H://需量类
// 			wResult = GetRequest645Demand( Dot, DI, OutBufLen, OutBuf );
// 			break;
// 		#endif//#if( MAX_PHASE == 3 )
// 		case DLT645_VARIABLE_02H://变量类
// 			wResult = Get645RequestVariable( Dot, DI, OutBufLen, OutBuf );
// 			break;
		
// 		case DLT645_PARA_04H://参变量类
// 			wResult = GetPro645RequestMeterPara( PortType,DI, OutBufLen, OutBuf );
// 			break;
		
// 		case DLT645_FREEZE_05H://冻结类
// 			wResult = GetRequest645FreezeData( DI, OutBufLen, OutBuf );
// 			break;
			
// 		#if( SEL_DLT645_2007 == YES )
// 		case DLT645_LOAD_06H://负荷记录类 !!!!!!暂未调试
// 			wResult = GetRequest645Lpf( PortType, DI, OutBufLen, OutBuf );
// 			break;
		
// 		case DLT645_EVENT_03H://密钥更新记录、异常插卡事件、编程类类等 !!!!!!!暂时未写
// 		case DLT645_EVENT_19H://过流事件
// 		case DLT645_EVENT_1DH://跳闸事件
// 		case DLT645_EVENT_1EH://合闸事件
// 		case DLT645_EVENT_10H://失压事件
// 		case DLT645_EVENT_18H://失流事件
// 			wResult = GetRequest645EventData( DI, OutBufLen, OutBuf );
// 			break;
// 		#endif//#if( (SEL_DLT645_2007 == YES)	
// 		default:
// 			break;
// 	}
// 	return wResult;
// }


//---------------------------------------------------------------
//函数功能: 获取某个数据标识对应的数据
//
//参数:     
//			Dot[in]    - 获取电能小数位数
//		    ID[in]     - 传入数据标识
//		    OutBufLen  - 传入缓存长度
// 			OutBuf[in] - 传入缓存
//
//返回值:	TRUE/FALSE
//
//备注:
//---------------------------------------------------------------
// BOOL api_GetPro645IDData( eSERIAL_TYPE PortType, BYTE Dot, BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
// {
// 	WORD Result;
	
// 	Result = GetPro645IDData( PortType, Dot, DI, OutBufLen, OutBuf );
	
// 	if( Result >= DLT645_ERR_DATA_01 )
// 	{
// 		return FALSE;
// 	}

// 	return TRUE;
// }

#endif//#if( SEL_DLT645_2007 == YES )

