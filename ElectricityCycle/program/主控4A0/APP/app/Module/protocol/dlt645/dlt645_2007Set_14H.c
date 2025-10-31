//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	魏灵坤
//创建日期	2016.9.2
//描述		国标DL/T645-2007规约源文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if( SEL_DLT645_2007 == YES )

//最大数量 0:年时区数 1日时段表数 2 日时段数 3费率数 4 公共假日数
const BYTE TimeSegMaxNum[] = {MAX_TIME_AREA,MAX_TIME_SEG_TABLE,MAX_TIME_SEG,MAX_RATIO,MAX_HOLIDAY};
//下标 0--年时区数超 1--日时段表数超 2--时段数超 3--费率数超 4--公共假日数超
const WORD TimeSegParaOverError[] = { DLT645_ERR_OVER_AREA_10, DLT645_ERR_DATA_01, DLT645_ERR_OVER_SEG_20, DLT645_ERR_OVER_RATIO_40, DLT645_ERR_DATA_01 };

//-----------------------------------------------
//函数功能: 将时区表或时段表进行排序
//
//参数: 
//			AreaAndSegmentNum[in]	时区表内时区个数 或 时段表内时段个数
//          pAreaAndSegment[in]		时区表或时段表
//         
//返回值:  	无
//备注: 
//-----------------------------------------------
void api_SortAreaAndSegment( BYTE AreaAndSegmentNum, BYTE *pAreaAndSegment )
{
	BYTE i,j,i0,i1,i2,j0,j1,j2;
	BYTE Temp;

	for(i=0; i<AreaAndSegmentNum; i++)
	{
		for(j=i; j<AreaAndSegmentNum; j++)
		{
			i0 = i*3+0;
			j0 = j*3+0;
			i1 = i*3+1;
			j1 = j*3+1;
			i2 = i*3+2;
			j2 = j*3+2;

			//如果日大于，或日等于但时大于
			if( (pAreaAndSegment[i0] > pAreaAndSegment[j0])||
				( (pAreaAndSegment[i0] == pAreaAndSegment[j0])&&(pAreaAndSegment[i1] > pAreaAndSegment[j1]) ) )
			{
				Temp = pAreaAndSegment[i0];
				pAreaAndSegment[i0] = pAreaAndSegment[j0];
				pAreaAndSegment[j0] = Temp;

				Temp = pAreaAndSegment[i1];
				pAreaAndSegment[i1] = pAreaAndSegment[j1];
				pAreaAndSegment[j1] = Temp;

				Temp = pAreaAndSegment[i2];
				pAreaAndSegment[i2] = pAreaAndSegment[j2];
				pAreaAndSegment[j2] = Temp;
			}
		}
	}
}

//DealSetPassword(): 成功返回 DLT_PRG_OK_00，不成功返回 DLT_PRG_ERR_PASSWORD_04
WORD DealSetPassword( BYTE byLevel, BYTE *pBuf )
{
	BYTE byUseLevel;
	TMuchPassword TmpMuchPassword;

	TSerial *pSerial = GetSerial(pBuf);
	if( pSerial == FALSE )
	{
		//为降低出错概率,一般没有这种情况，如果不是规约操作，默认使用的密码级别是02
		byUseLevel = MINGWEN_H_PASSWORD_LEVEL;
	}
	else
	{
		byUseLevel = pSerial->ProBuf[MAX_PRO_BUF-1];//保存使用的密码级别
	}

	if( ( byLevel != MINGWEN_H_PASSWORD_LEVEL )&&( byLevel != 0x04 ) )
	{
		return DLT645_ERR_ID_02;//功能规范宣贯要求仅支持02、04级密码 
	}

	if( ( (byLevel >= byUseLevel)||(byUseLevel == CLASS_2_PASSWORD_LEVEL) )
		&&( byLevel <= (MAX_645PASSWORD_LEVEL - 1) ) )
	{
		// 改写时只写密码，不写密级，密级靠下标区分

		//读出EEPROM中保存的密码 不管读出是否成功后面都写，这样只有一次写不成功
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
		
		pBuf[3] = pBuf[2];
		memcpy( (void*)&(TmpMuchPassword.Password645[byLevel][0]), (void*)&(pBuf[0]), 4 );

		//写入密码，自己计算校验
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
		
		return DLT645_OK_00;
	}
	else
	{
		//DL/T745_2007备案文件中Err的D2: 1--密码错/未授权 0--无密码错/未授权
		//用户抄读电表密码，应答"密码错/未授权"
		return DLT645_ERR_PASSWORD_04;
	}
}

//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:040001XX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err

//04000101	YYMMDDWW	 4	 年月日星期	 日期及星期(其中0代表星期天)
//04000102  hhmmss       3   时分秒      时间
//04000103  NN           1   分          最大需量周期
//04000104  NN           1   分          滑差时间
//04000105  XXXX         2   毫秒        校表脉冲宽度 (只读)
//04000106  YYMMDDhhmm   5   年月日时分  两套时区表切换时间
//04000107	YYMMDDhhmm	 5	 年月日时分	 两套日时段切换时间
//04000108  YYMMDDhhmm 5	 两套分时费率切换时间
//04000109	YYMMDDhhmm 5 * * 两套梯度切换时间
//0400010A	YYMMDDhhmm 5 * * 两套阶梯时区切换时间
//0400010B	YYMMDDhhmm 5 * * 两套费控模式切换时间
//0400010C  YYMMDDWWhhmmss 7 同时写日期、星期、时间
//----------------------------------------------------------
const BYTE Para040001XXLen[] = {4,3,1,1,2,5,5,5,5,5,5,7};
WORD SetPara040001XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i, j;
	BYTE Buf[6];
	WORD Result;
	TRealTimer TmpRealTimer;//时间结构体
	DWORD RelativeTime;
	TSwitchTimePara TmpSwitchTimePara;
	
	//04000101	YYMMDDWW	   4	 年月日星期	 日期及星期(其中0代表星期天)
	//04000102  hhmmss         3     时分秒      时间
	//0400010C  YYMMDDWWhhmmss 7     同时写日期、星期、时间
	if( ( SubID0 == 0x01 )||( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
	{
		//取得缓冲是TSerial中的哪个串口
		//返回: FALSE--不是串口缓冲 其它--返回的是串口Serial[]的地址
		if( GetSerial(pBuf) == FALSE )
		{
			//要求pBuf必须是规约缓冲，这主要为保存校时记录 SaveAdjustTimeInfo()时节省堆栈
			return DLT645_ERR_DATA_01;
		}
	}

	if( ( SubID0 == 0x01 )||( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
	{
		api_ReadMeterTime( 0 );//先读一下电表日期时间
		memcpy( (void *)&TmpRealTimer, (void *)&RealTimer, sizeof(TRealTimer) );
     
        if( ( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
		{
			TmpRealTimer.Sec = lib_BBCDToBin(pBuf[0]);
            TmpRealTimer.Min = lib_BBCDToBin(pBuf[1]);
            TmpRealTimer.Hour = lib_BBCDToBin(pBuf[2]);
			
			//时、分、秒合法性判断
			if( (TmpRealTimer.Hour > 23) && (TmpRealTimer.Hour != 0xff) )
			{
				return DLT645_ERR_DATA_01;
			}
			if( (TmpRealTimer.Min > 59) && (TmpRealTimer.Min != 0xff) )	
			{
				return DLT645_ERR_DATA_01;
			}
			if( (TmpRealTimer.Sec> 59) && (TmpRealTimer.Sec != 0xff) )	
			{
				return DLT645_ERR_DATA_01;
			}
			if( SubID0 == 0x0c )
			{
				pBuf += 3;
			}				
		}
		if( ( SubID0 == 0x01 )||( SubID0 == 0x0c ) )
		{
			TmpRealTimer.Day = lib_BBCDToBin(pBuf[1]);
			TmpRealTimer.Mon = lib_BBCDToBin(pBuf[2]);
			TmpRealTimer.wYear = 2000+lib_BBCDToBin(pBuf[3]);
			
			//年、月、日、合法性判断
			if( api_CheckMonDay( TmpRealTimer.wYear, TmpRealTimer.Mon, TmpRealTimer.Day ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		}
		api_WritePreProgramData( 0, 0x00020040 );  //校时记录 OAD = 40000200
        api_WriteMeterTime(&TmpRealTimer);
		api_SetAllTaskFlag( eFLAG_TIME_CHANGE );
		api_SavePrgOperationRecord( ePRG_ADJUST_TIME_NO );  //校时记录
		#if( SEL_DEMAND_2022 == NO )
		#if( MAX_PHASE != 1)		
		api_InitDemand( );//设置成功后，重新进行需量的计算
		#endif
		#endif
		return Para040001XXLen[ SubID0-1 ];
	}
	//04000103  NN           1   分          最大需量周期
	//04000104  NN           1   分          滑差时间
	#if( MAX_PHASE == 3 )
	else if( (SubID0 == 0x03) || (SubID0 == 0x04) )
	{
		pBuf[0] = lib_BBCDToBin(pBuf[0]);
		if( api_WritePara( 1,  GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.DemandPeriod)+(SubID0-0x03), 1, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		else
		{
			#if( MAX_PHASE != 1)
            api_InitDemand();//设置成功后，重新进行需量的计算	-功率和电能需量法都重新计算
            #endif
            
			return Para040001XXLen[ SubID0-1 ];
		}
	}
	#endif//#if( MAX_PHASE == 3 )
	else if( SubID0 == 0x05 )
	{
		return DLT645_ERR_PASSWORD_04;
	}

	//04000106	YYMMDDhhmm	 5	 年月日时分  两套时区表切换时间
	//04000107	YYMMDDhhmm	 5	 年月日时分  两套日时段切换时间
	//04000108  YYMMDDhhmm   5	 两套分时费率切换时间
	//04000109	YYMMDDhhmm   5   两套梯度切换时间
	#if( PREPAY_MODE == PREPAY_LOCAL )//本地费控表
	else if( (SubID0 >= 0x06) && (SubID0 <= 0x09) )  
	#else
	else if( (SubID0 == 0x06) || (SubID0 == 0x07) )
	#endif	
	{
		if( lib_IsAllAssignNum(pBuf, 0x00, 5) == TRUE ) //判断是否为00
		{
			RelativeTime = 0;
		}
		else if( lib_IsAllAssignNum(pBuf, 0x99, 5) == TRUE ) //判断是否为99
		{
			RelativeTime = 0x99999999;
		}
		else
		{
			api_ConvertYYMMDDhhmm_TRealTimer( (TRealTimer *)&TmpRealTimer, pBuf );
			RelativeTime = api_CalcInTimeRelativeMin( &TmpRealTimer );//计算绝对时间
		}

		if( RelativeTime == ILLEGAL_VALUE_8F )//非法数据
		{
			return DLT645_ERR_DATA_01;
		}

		if( (RelativeTime == 0) || (RelativeTime == 0x99999999) )
        {
        	//清除备用套时间有效标志
        	if( SubID0 == 0x06 )
        	{
        	    api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG1);
        	}
       	 	else if( SubID0 == 0x07 )
       	 	{
       	     	api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG2);
       	 	}
        	else
        	{

        	}
		}

		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof(TmpSwitchTimePara), (BYTE *)&TmpSwitchTimePara);
		TmpSwitchTimePara.dwSwitchTime[ SubID0 - 0x06 ] = RelativeTime;
		Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof( TmpSwitchTimePara ), (BYTE *)&TmpSwitchTimePara );

		if( Result == TRUE )
		{
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			return Para040001XXLen[ SubID0-1 ];
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
	
	}	
	else
	{
		return DLT645_ERR_ID_02;
	}
}

//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:040002XX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
//04000201	NN	  1	   个	   年时区数p≤14
//04000202	NN    1    个      日时段表数q≤8
//04000203	NN    1    个      日时段数(每日切换数)m≤14
//04000204	NN    1    个      费率数k≤63
//04000205	NNNN  2    个      公共假日数n≤254
//04000206	NN	  1	   次	   谐波分析次数
//04000207	NN	  1	   个	*  梯度数 阶梯数	---这两个暂时都不支持--songchen
//04000208	NN	  1	   个	*  密钥总条数 HEX	---这两个暂时都不支持--songchen
//----------------------------------------------------------
WORD SetPara040002XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	    
    if( SubID0 != 0x08 )
    {	
	    if( lib_CheckBCD( pBuf[0] ) == FALSE )
	    {
			return DLT645_ERR_DATA_01;
		}
	}
	
	i = SubID0 - 1;

	if( i <= 4 )
	{
		if( pBuf[0] != 0xff )
		{
			pBuf[0] = lib_BBCDToBin(pBuf[0]);	
		}
		if( ( i == 4 )&&( pBuf[0] == 0xff ) )//安徽要求：公共假日数设置为FF，表示不采用
		{
		}
		//年时区数，BCD 日时段表数，BCD 日时段数，BCD 费率数，BCD
		//年时区数不能起过最大,且数据长度要对
		else if( pBuf[0] > TimeSegMaxNum[i] )
		{
			//出错原因
			return TimeSegParaOverError[i];
		}
		#if( MAX_HOLIDAY < 100 )
		else if( ( i == 4 )&&( pBuf[1] != 0x00 ) )//设置公共假日数时, if( MAX_HOLIDAY < 100 ) 高字节必须是0
		{
			return TimeSegParaOverError[4];
		}
		#endif

		if( api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum) +i), 1, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	//返回：0--没有设置进数据 其它--表示设置进多少数据
	return 1;
}

//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:040003XX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
//04000301	NN	 1	个	自动循环显示屏数
//04000302	NN   1  秒  每屏显示时间
//04000303	NN   1  位  显示电能小数位数
//04000304	NN   1  位  显示功率（最大需量）小数位数
//04000305	NN	 1	个	按键循环显示屏数
//04000306  NNNNNNNN	CT变比
//04000307  NNNNNNNN	PT变比
//04000308  NN 	  1 秒 * * 上电全显时间
//----------------------------------------------------------
const BYTE Para040003XXLen[] = {1,1,1,1,1,3,3,1};
WORD SetPara040003XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	WORD wAddr;
	DWORD dwData;

	i = SubID0 - 1;

	//如果pBuf缓冲不够读下要读的数据，则退出
	if( Para040003XXLen[i] > Len )
	{
		return DLT645_ERR_DATA_01;
	}	
	//PT CT 数据处理与其他几个不同--暂时不支持所以删除
	pBuf[0] = lib_BBCDToBin(pBuf[0]);	

	if( (i < 5)||(i == 7) )
	{
		if( i == 0 )
		{	
			if( pBuf[0] > MAX_LCD_LOOP_ITEM )//04000301 自动循环显示屏数
			{
				return DLT645_ERR_DATA_01;
			}
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DispItemNum[0]);
		}
		else if( i == 1 )
		{
		    if( (pBuf[0] > 20) || (pBuf[0] < 5) ) //循显显示时间在5-20s
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.LoopDisplayTime);
		}
		else if( i == 2 )
		{
			#if( MAX_PHASE == 1 )
			if( pBuf[0] > 4 ) //单相表电能显示位数最多支持4位，山东要求0-4可设，
			{
				return DLT645_ERR_DATA_01;
			}
			#endif//#if( MAX_PHASE == 1 )
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.EnergyFloat);
		}
		else if( i == 3 )
		{
		    if( pBuf[0] > 5 ) //功率小数位最大支持5位
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DemandFloat);
		}
		else if( i == 4 )
		{	
			if( pBuf[0] > MAX_LCD_KEY_ITEM )
			{
				return DLT645_ERR_DATA_01;
			}
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DispItemNum[1]);
		}
		else if( i == 7 )
		{
		    if( (pBuf[0] < 5) || (pBuf[0] > 30) )  //上电全显时间5~30可设
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer);
		}	
		else
		{
            return DLT645_ERR_DATA_01;
		}
		
		if( api_WritePara( 0,  wAddr, 1, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		if( i == 2 )
		{
			//api_ResetBorrDotNum( 0 );
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	return Para040003XXLen[i];
}
//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:040004XX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
//04000301	NN	 通信地址
//040003XX	NN   其他常用参数--暂不可设置
//----------------------------------------------------------
WORD SetPara040004XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	BYTE Buf[6];

	if( SubID0 == 0x01 )
	{
		lib_ExchangeData( Buf, pBuf, 6 );
		if(api_ProcMeterTypePara( WRITE, eMeterCommAddr, Buf)== FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		
		return 6;
	}
	//表号
	else if( SubID0 == 0x02)
	{
		lib_ExchangeData( Buf, pBuf, 6 );
		if(api_ProcMeterTypePara( WRITE, eMeterMeterNo, Buf)== FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		
		return 6;
	}
	//资产管理编码暂不支持设置
	else if( SubID0 == 0x03)
	{
		return DLT645_ERR_PASSWORD_04;
	}
	//只读参数不支持设置
	else if(( SubID0 >= 0x02 ) && (SubID0 <= 0x0D))
	{
		return DLT645_ERR_PASSWORD_04;
	}
	else if(SubID0 == 0x1F)		// 扩展规约，设置地理位置，4G模块使用
	{
		// lib_InverseData(pBuf,18);

		if( (pBuf[0] > 1) || (pBuf[7] > 1))	//经纬度方位只能是0或1
		{
			return DLT645_ERR_DATA_01;
		}
		
		if(api_ProcMeterTypePara( WRITE, eMeterMeterPosition, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		return 18;
	}
	else if(SubID0 == 0x20)		// 扩展规约，设置透传报文参数
	{
		if( ( (pBuf[0]!=FALSE) && (pBuf[0]!=TRUE) )
		 || ( (pBuf[1]==5) || (pBuf[1]<2) || (pBuf[1]>10) ) // 不支持7200波特率
		 || ( (pBuf[2]<5) || (pBuf[2]>100) )
		)
		{
			return DLT645_ERR_DATA_01;
		}

		MessageTransParaRam.AllowUnknownAddrTrans = pBuf[0];
		MessageTransParaRam.Baud = pBuf[1];
		MessageTransParaRam.Timeout = pBuf[2];
		if(api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.MessageTransPara), sizeof(MessageTransPara_t), (BYTE*)&MessageTransParaRam ) == FALSE)
		{
			return DLT645_ERR_ID_02;
		}
		return 2;
	}
	else//不支持项返回数据标识码错
	{
		return DLT645_ERR_ID_02;
	}

}

//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:040006XX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
//04000601 有功组合方式特征字
//04000602 04000603 无功组合方式1、2特征字
//----------------------------------------------------------
WORD SetPara040006XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;

	i = SubID0 - 1;
	
	#if(SEL_RACTIVE_ENERGY == YES)
	if( i > 2 )// 0 1 2
	{
		return DLT645_ERR_ID_02;
	}
	#else
	if( i > 0 )
	{
		return DLT645_ERR_ID_02;
	}
	#endif

	//04000601 有功组合方式特征字 组合有功
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//保留	保留		保留	保留	反向有功	反向有功	正向有功	正向有功
	//									0不减1减	0不加1加	0不减1减	0不加1加
	if( api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.byActiveCalMode)+i, 1, pBuf) == FALSE )
	{
		return DLT645_ERR_DATA_01;
	}

	#if( PREPAY_MODE == PREPAY_LOCAL )
	//设置有功组合方式特征字，需要刷新一下阶梯值和阶梯电价
	//api_SetUpdatePriceFlag( ePriceChangeEnergy );
	#endif

	return 1;
}

//--------------------------------------------------
//功能描述:  规约设置函数:设置参变量标识码范围:04000BXX
//         
//参数:
//	 SubID0[in]:	规约标识DL/T645-2007中最低字节DI0（四个字节0xDI3DI2DI1DI0）
//
//	 Len[in] :	pBuf缓冲中要设置的数据的长度
//
//	 pBuf[in]：	传来要设置的数据
//返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
//备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
//04000B01		DDhh	2	日时	每月第1结算日
//04000B02      DDhh    2   日时    每月第2结算日
//04000B03	    DDhh	2	日时    每月第3结算日
//----------------------------------------------------------
WORD SetPara04000BXX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE byNeedSwitchSave;
	BYTE i,Status;
	BYTE Buf[6];
	DWORD dwID;
	
	TBillingPara TmpBillingPara;

	i = SubID0 - 1;


	//如果pBuf缓冲不够读下要读的数据，则退出
	if( 2 > Len )
	{
		return DLT645_ERR_DATA_01;
	}

	if( i < 3 )
	{
		pBuf[0] = lib_BBCDToBin(pBuf[0]);
		pBuf[1] = lib_BBCDToBin(pBuf[1]);
		//04000B01	DDhh	每月第1结算日
		//04000B02  DDhh    每月第2结算日
		//04000B03  DDhh    每月第3结算日
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		
		Buf[0] = TmpBillingPara.MonSavePara[0].Hour;
		Buf[1] = TmpBillingPara.MonSavePara[0].Day;
		
		
		byNeedSwitchSave = 0;
		if( i == 0 )
		{									
			if( (TmpBillingPara.MonSavePara[0].Hour != pBuf[0])||(TmpBillingPara.MonSavePara[0].Day != pBuf[1]) )
			{
				byNeedSwitchSave = 1;//如果改变第一结算日，则要转存月度用电量、结算日电量
			}
		}
		TmpBillingPara.MonSavePara[i].Hour = pBuf[0];
		TmpBillingPara.MonSavePara[i].Day = pBuf[1];

		if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )//非厂内模式下 
		{	
			//必须确保第一个结算日是有效的
			Status = TRUE;
	
			//日时必须都为99 否则判断时间是否合法
			if( (TmpBillingPara.MonSavePara[i].Day == 99)&&(TmpBillingPara.MonSavePara[i].Hour == 99) )
			{
				
			}
			else
			{
				if( (TmpBillingPara.MonSavePara[i].Day > 28)||(TmpBillingPara.MonSavePara[i].Day == 0) )
				{
					Status = FALSE;
				}
				
				if( TmpBillingPara.MonSavePara[i].Hour > 23  )
				{
					Status = FALSE;
				}
			}
	
			if( Status == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		}

		api_WritePreProgramData( 0, 0x00021641 );  //41160200 结算日
		Status = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		if( Status == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		else
		{
			api_SavePrgOperationRecord( ePRG_CLOSING_DAY_NO ); 
			if( byNeedSwitchSave != 0 )//如果改变第一结算日，则要转存月度用电量、结算日电量 
			{
				api_SetFreezeFlag( eFREEZE_CLOSING, 0 ); //结算冻结
			}
		}
		return 2;
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
}


//04000C01		NNNNNNNN	4		0级密码
//		...     ...    		...     ...
//04000C0A	    NNNNNNNN	4		9级密码
//设置参变量标识码范围:04000CXX
WORD SetPara04000CXX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	WORD wResult;
	if( Len < 4 )
	{
		return DLT645_ERR_DATA_01;
	}

	//SubID0-1后得到新密码级别
	//密码只有3个字节，但协议中用14H写密码是4个字节，按协议报文习惯，认为第一字节是密码级别，取后3个字节，2010-1-9
	//DealSetPassword(): 成功返回 DLT_PRG_OK_00，不成功返回 DLT_PRG_ERR_PASSWORD_04
	wResult = DealSetPassword( (SubID0-1), (BYTE *)&(pBuf[1]) );
	if( wResult == DLT645_OK_00 )
	{
		return 4;//数据长度
	}
	else
	{
		return wResult;//0x8000+失败原因
	}

}

//设置参变量标识码范围:040011XX
WORD SetPara040011XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE Buf[30];
	WORD wAddr,Result;
	// 04001101	 NN	电表运行特征字1
	//Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	Bit0
	//保留	保留	保留	保留	保留	保留	保留	外置开关控制方式
	//														（0电平，1脉冲）
	if( SubID0 == 0x01 )
	{
		//Bit2：主动上报模式
		//Bit1：液晶(1)(2)字样意义
		//Bit0：外置开关控制方式
		if( pBuf[0] & BIT2 )//启用主动上报
		{
            api_SetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
		}
		else
		{
            api_ClrRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
		}

		if( pBuf[0] & BIT1 )//液晶显示字样
		{
		    Buf[0] = 1;
            Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+9, 1, Buf);
		}
		else
		{
            Buf[0] = 0;
            Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+9, 1, Buf);
		}

        if( Result == FALSE )
        {
            return DLT645_ERR_DATA_01;
        }
        
		if( pBuf[0] & BIT0 )//外置开关控制方式 电平
		{
		    Buf[0] = 0;
            Result = WriteRelayPara( 3, Buf );
		}
		else//脉冲
		{
            Buf[0] = 1;
            Result = WriteRelayPara( 3, Buf );
		}
		
		if( Result == FALSE )
        {
            return DLT645_ERR_DATA_01;
        }
        
		return 1;
	}
    else
    {
		return DLT645_ERR_ID_02;
    }
}

WORD SetPara040013XX( BYTE SubID0, BYTE BufLen, BYTE *pBuf )
{
	BYTE Sum,i,Lenth;
	Sum = 0;
	if(SubID0 == 0xD1)
	{		
		for(i = 0; i < 22; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[22] = Sum;
		if( api_ProcGprsTypePara(WRITE, eTCPIP_IP_Port_APN, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 23;
	}
	else if(SubID0 == 0xD5)
	{		
		for(i = 0; i < 4; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[4] = Sum;
		if( api_ProcGprsTypePara(WRITE, eTCPIP_AreaCode_Ter, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 5;
	}
	else if(SubID0 == 0xD6)
	{
		for(i = 0; i < 64; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[64] = Sum;
		if( (api_ProcGprsTypePara(WRITE, eTCPIP_APN_UseName, pBuf) == FALSE) || (api_ProcGprsTypePara(WRITE, eTCPIP_APN_PassWord, pBuf+32) == FALSE))
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 65;
	}
	else if(SubID0 == 0xE1)
	{		
		for(i = 0; i < 22; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[22] = Sum;
		if( api_ProcGprsTypePara(WRITE, eMQTT_IP_Port_APN, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 23;
	}
	else if(SubID0 == 0xE5)
	{		
		for(i = 0; i < 4; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[4] = Sum;
		if( api_ProcGprsTypePara(WRITE, eMQTT_AreaCode_Ter, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 5;
	}
	else if(SubID0 == 0xE6)
	{
		for(i = 0; i < 64; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[64] = Sum;
		if( (api_ProcGprsTypePara(WRITE, eMQTT_APN_UseName, pBuf) == FALSE) || (api_ProcGprsTypePara(WRITE, eMQTT_APN_PassWord, pBuf+32) == FALSE))
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 65;
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	return Lenth;
}

WORD SetPara040014XX( BYTE SubID0, BYTE BufLen, BYTE *pBuf )
{
	BYTE Type;
	WORD wDataLen;
	TTwoByteUnion DataBuf;

	wDataLen = 0;

	if( SubID0 == 0x01 )
	{
		memcpy( DataBuf.b, pBuf, 2 );
		DataBuf.w = lib_WBCDToBin( DataBuf.w );
		WriteRelayPara( 2, DataBuf.b );
		
		wDataLen = 2;
	}
	else if( SubID0 == 0x05 )
	{
        pBuf[0] = lib_BBCDToBin( pBuf[0] );  
        api_SetReportResetTime(pBuf[0]);

		wDataLen = 1;	
	}	
	else
	{
		return DLT645_ERR_ID_02;
	}

	return wDataLen;
}

//设置参变量标识码范围:0400XXXX
WORD SetPara0400XXXX( eSERIAL_TYPE PortType, BYTE SubID1, BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	WORD wResult;

	wResult = DLT645_ERR_ID_02;

	switch( SubID1 )
	{
		case 0x01:
			wResult = SetPara040001XX(SubID0,  Len, pBuf);
			break;
		case 0x02:
			wResult = SetPara040002XX(SubID0,  Len, pBuf);
			break;
		case 0x03:
			wResult = SetPara040003XX(SubID0,  Len, pBuf);
			break;
		case 0x04:
			wResult = SetPara040004XX(SubID0,  Len, pBuf);
			break;
		case 0x05://电表运行状态字只读
			wResult = DLT645_ERR_PASSWORD_04;
			break;
		case 0x06:
			wResult = SetPara040006XX(SubID0,  Len, pBuf);
			break;
		case 0x07: //波特率特征字 有专用的设置命令，此命令也用，此命令允许从一个口改别的口速率
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			wResult = SetPara04000BXX(SubID0,  Len, pBuf);
			break;
		case 0x0c://04000CXX 是0--9级密码，不可读
			wResult = SetPara04000CXX(SubID0,  Len, pBuf);
			break;
		case 0x0e:
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )//本地费控表
		case 0x10:
			break;
		#endif		
		case 0x11:
			wResult = SetPara040011XX(SubID0,  Len, pBuf);
			break;
		case 0x12:
			break;
		case 0x13:
			wResult = SetPara040013XX( SubID0, Len, pBuf);
			break;
		case 0x14:
			wResult = SetPara040014XX( SubID0, Len, pBuf);
			break;		
		case 0x15://04001501	XX…XX	12+1+1+..+1		*		主动上报状态字,主动上报事件1新增次数,主动上报事件2新增次数...主动上报事件N新增次数
			if( SubID0 == 0x01 )
			{
				wResult = (DLT645_ERR_PASSWORD_04);
			}
			//0x04001503 复位主动上报状态字
			else if( SubID0 == 0x03 )
			{
				wResult = api_ResetReportStatusByte( PortType, pBuf);

				if( wResult == FALSE)
				{
					wResult = DLT645_ERR_DATA_01;
				}
				else
				{
					wResult = DLT645_OK_00;
				}
			}	
			break;
		default:
			break;
	}
	//返回设置进电表的数据长度
	return wResult;
}


//-----------------------------------------------
//函数功能: 设置645-2007中的数据标识为040100XX的时区表、时段表
//
//参数: 
//	SubID2[in]		645-2007中的数据标识的DI2
//	SubID0[in]		645-2007中的数据标识的DI0
//	Len[in]			协议传来设置数据长度
//  pBuf[in]		设置数据的缓冲
//                    
//返回值:    成功：设置数据长度（字节）； 失败：0x8000+Err
//备注: 
//04010000:第一套时区表数据  04020000:第二套时区表数据
//04010001:第一套第1日时段表数据 04020001:第二套第1日时段表数据
//04010008:第一套第8日时段表数据 04020008:第二套第8日时段表数据
//--------------------------------------------------
WORD SetTimeSegPara040100XX( BYTE SubID2, BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE TimeTableType, i;
	WORD wAddr;
	WORD wDataLen;
	DWORD dwID;
	BYTE Buf[MAX_TIME_AREA*3*2];
	BYTE TmpBuf[3];

	wDataLen = 0;

	// 设置长度减去对3的余数，每个时区、时段都是3字节 设置数据长度 Len 要是3的整数倍

	if( Len < 3 )
	{
		return DLT645_ERR_DATA_01;
	}
	for( i=0; i<Len; i++ )
	{
		if( lib_CheckBCD(pBuf[i]) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}	
		pBuf[i] = lib_BBCDToBin(pBuf[i]);//把645上位机发来的BCD码转为HEX，698.45电表EEPROM保存是HEX
	}

	//倒序操作:3个字节为一组，组内进行倒序
	for(i = 0; i < Len ; i += 3)
	{
		lib_InverseData(&pBuf[i], 3);
	}

	//校验时区时段表是否正确，且读出，放在FlashBuf中
	//Type
	//D7:	0--第一套时区时段表 1--第二套时区时段表  做新规约( SEL_DLT645_2007 ) 时改动 2008-7-19
	//D6--D0:
	//		1 -- TSubTimeTable1
	//		2 -- TSubTimeTable3
	//		3 - MAX_TIME_SEG_TABLE+2 -- TSubTimeTable2
	//WORD DbReadTimeTable(WORD Type, WORD Addr, WORD Length, BYTE * Buf)

	if( SubID0 == 0x00 )//04010000:第一套时区表数据  04020000:第二套时区表数据
	{
		if( Len > (MAX_TIME_AREA*3) )
		{
			return DLT645_ERR_OVER_AREA_10;
		}

		// 设置长度减去对3的余数，每个时区、时段都是3字节 设置数据长度 Len 要是3的整数倍
		//Len -= (Len % 3 );
		if( (Len % 3 ) != 0 )
		{
			return DLT645_ERR_DATA_01;
		}

		wDataLen = ( MAX_TIME_AREA *3 );
		wAddr = GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]);
		TimeTableType= 1;


		for( i=0; i<Len; i+=3 )
		{
			//698.45表时区月：日：时段表号（0、1、2）、时段时：分：费率号（0、1、2）,注意和以前顺序相反
			if( api_CheckMonDay( RealTimer.wYear, pBuf[i], pBuf[i+1] ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
			else if( ( pBuf[i+2] == 0x00 )||( pBuf[i+2] > FPara1->TimeZoneSegPara.TimeSegTableNum ) )//MAX_TIME_SEG_TABLE )
			{
				return DLT645_ERR_DATA_01;
			}
		}

	}
	// 04010001:第一套第1日时段表数据 04020001:第二套第1日时段表数据
	// 04010008:第一套第8日时段表数据 04020008:第二套第8日时段表数据
	// 新规约中最多8套日时段表
	else if( SubID0 <= MAX_TIME_SEG_TABLE )
	{
		if( Len > (MAX_TIME_SEG*3) )
		{
			return DLT645_ERR_OVER_SEG_20;
		}

		// 设置长度减去对3的余数，每个时区、时段都是3字节 设置数据长度 Len 要是3的整数倍
		//Len -= (Len % 3 );
		if( (Len % 3 ) != 0 )
		{
			return DLT645_ERR_DATA_01;
		}

		wDataLen = ( MAX_TIME_SEG * 3 );
		wAddr = GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]);
		TimeTableType = ( 2 + SubID0 );//时段表


		memcpy( (void *)Buf, (void *)pBuf, Len );
		api_SortAreaAndSegment( (Len/3), Buf);			
		Buf[Len+2]=Buf[2];
		Buf[Len+1]=Buf[1];//后面补上第1时段，且小时加24，方便判断最后一个时段和第一个时段的间隔
		Buf[Len]=Buf[0]+24;
						
        for( i=0; i<Len; i+=3 )
		{
			//检查每个时段数据的合法性
			{
				if( i <= (Len-3) )//最后一个时段和第一个时段的间隔也要判断 2014-5-8
				{
					if(( ( api_JudgeTimeSegInterval((BYTE *)&(Buf[i])) == FALSE ))
						&&(api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE) )
					{
						return DLT645_ERR_DATA_01;
					}
				}
				//698.45表时区月：日：时段表号（0、1、2）、时段时：分：费率号（0、1、2）,注意和以前顺序相反
				if( pBuf[i] > 23 )//时
				{
					return DLT645_ERR_DATA_01;
				}
				else if( pBuf[i+1] > 59 )//分
				{
					return DLT645_ERR_DATA_01;
				}
				else if( ( pBuf[i+2] == 0x00 )||( pBuf[i+2] > FPara1->TimeZoneSegPara.Ratio ) )//MAX_RATIO )
				{
					return DLT645_ERR_OVER_RATIO_40;
				}
			}
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	// 如果要设置数据超出电表支持最大时区时段长度，取电表支持最大时区时段长度
	if( Len > wDataLen )
	{
		Len = wDataLen;
	}
	
	if( SubID2 == 0x02 )
	{
		TimeTableType |= 0x80;
	}

	if( wDataLen > Len )
	{
		//把时段参数多余的时区、时段用最后一个有效的补上
		api_AddTimeSegParaExtDataWithLastValid( Len, wDataLen, pBuf );
		Len = wDataLen;
	}

	api_WriteTimeTable(TimeTableType, wAddr, wDataLen, pBuf);

	return wDataLen;
}
//--------------------------------------------------
//功能描述:  规约设置函数DL/T645-2007SetDlt645_2007Data()
//         
//参数:
//	 dwID:	规约标识DL/T645-2007（四个字节0xDI3DI2DI1DI0）
//	 Len :	pBuf缓冲中要设置的数据的长度
//	 pBuf：	传来要设置的数据
//返回值：成功：读取数据长度（字节）； 失败：0
//备注内容:SetDlt645_2007Data()供 规约、预付费、CPU卡等需要设置参数的程序都可以调用此函数
//		   :SetDlt645_2007Data()如成功返回数据长度,如失败,返回 0x8000+Err
//----------------------------------------------------------
WORD SetDlt645_2007Data( eSERIAL_TYPE PortType, BYTE Len, BYTE *pBuf )
{
	BYTE SubID2,SubID1, SubID0;
	WORD wDataLen;
	TFourByteUnion EventID;

	wDataLen = DLT645_ERR_ID_02;

	SubID0 = gPr645[PortType].dwDataID.b[0];
	SubID1 = gPr645[PortType].dwDataID.b[1];
	SubID2 = gPr645[PortType].dwDataID.b[2];
	//以下设置函数，设置成功，返回设置进电表的数据长度，如果设置不成功，返回0表示没有数据设置到电表中去
	switch( SubID2 )
	{
		case 0x00:
			wDataLen = SetPara0400XXXX( PortType, SubID1,  SubID0,  Len, pBuf);
			break;

		case 0x01://第一套时区表，第一套第1--8日时段表
			wDataLen = DLT645_ERR_PASSWORD_04;
			break;
		case 0x02://第二套时区表，第二套第1--8日时段表
			wDataLen = SetTimeSegPara040100XX( SubID2, SubID0,  Len, pBuf );
			break;
			
		#if( PREPAY_MODE == PREPAY_LOCAL )//本地费控表
		case 0x05:
			break;
		case 0x06:
			break;
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )//本地费控表
		case 0x09://09-09-06老魏发来的国网补充文件中规定事件触发阀值
			break;
		case 0x80:
			break;
		case 0xdf://东方扩展参数 wlk 2008-8-22
			break;
			
		default:
			break;
	}

	return wDataLen;
}

//--------------------------------------------------
//功能描述:  645-2007协议请求读电能表通信地址，仅支持点对点通信。
//         
//参数:      BYTE *ProBuf[in]
//         		ProBuf: 串口通信缓冲
//         
//返回值:    DLT645_OK_00--成功 其它--返回失败类型
//         
//备注内容: 
//--------------------------------------------------
WORD DealSetDlt645_2007Data0X14( eSERIAL_TYPE PortType )
{
	WORD i;
	WORD wDataLen,ReturnStatus;
	BYTE *ProBuf;
	WORD wResult;

	// 低功耗运行模式
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		return DLT645_ERR_ID_02;
	}

	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	
	wDataLen = DLT645_ERR_ID_02;
	ReturnStatus = 0;//这样如果密码不是98级则 Inverse 时长度是0，不会冲内存 wlk
	
	//#define CLASS_2_PASSWORD_LEVEL				0x98//二类数据加密操作密码级别国网为98H
	//#define CLASS_1_PASSWORD_LEVEL				0x99//一类数据加密操作密码级别国网为99H
	
	if( (ProBuf[14] == CLASS_2_PASSWORD_LEVEL)||(ProBuf[14] == CLASS_1_PASSWORD_LEVEL) )
	{
		if( ProBuf[14] == CLASS_2_PASSWORD_LEVEL )
		{
			if( ProBuf[9] < 16 )
			{
				return DLT645_ERR_DATA_01;
			}
			//密文
			lib_InverseData( ProBuf+22,ProBuf[9]-16 );
			//MAC
			lib_InverseData( ProBuf+22+ProBuf[9]-16, 4 );
			ReturnStatus = api_DecipherSecretData(0x83,ProBuf[9]-12,ProBuf+22, ProBuf+22 );//为兼容02级密码，数据还是恢复成02级那种倒序方式
			if(ReturnStatus & 0x8000)
			{
				return DLT645_ERR_PASSWORD_04;
			}	
			
			//时区表、时段表 数据块需要分项倒序特别处理 04010000-04010008   04020000-04020008
			if(((gPr645[PortType].dwDataID.d>=0x04010000)&&(gPr645[PortType].dwDataID.d<=0x04010008))||((gPr645[PortType].dwDataID.d>=0x04020000)&&(gPr645[PortType].dwDataID.d<=0x04020008)))
			{
				lib_InverseData(ProBuf+22,ReturnStatus);
				memmove( ProBuf+22, ProBuf+22+4, ReturnStatus-4 );//把数据标识去掉
				
				for( i=0; i<((ReturnStatus-4)/3); i++)
				{
					lib_InverseData(ProBuf+22+(i*3),3);//698.45表时区月：日：时段表号（0、1、2）、时段时：分：费率号（0、1、2）,注意和以前顺序相反
				}
			}

		}
		else
		{	
			return DLT645_ERR_PASSWORD_04;
		}
	}
	
	//68 A0...A5 68 14 L DIODI1DI2DI3+PAP0P1P2+C0C1C2C3+DATA
	//wDataLen:设置进电表的数据长度，如果==0，表示没有数据设置到电表中去，返回错误无请求数据
	if( wDataLen == DLT645_ERR_ID_02 )
	{
		if(ProBuf[14] == CLASS_2_PASSWORD_LEVEL)
		{	
			wDataLen = SetDlt645_2007Data( PortType, (ReturnStatus-4), (ProBuf+22) );
		}
		else
		{
			wDataLen = SetDlt645_2007Data( PortType, (ProBuf[9] - 12), (ProBuf+22) );
		}
	}
	if( wDataLen < 0xff )
    {
		#if(SEL_PRG_RECORD645 == YES)
		//记录645编程记录，645独立的数据源，存储在eeprom中
		api_SaveProgramRecord645( EVENT_START, gPr645[PortType].byOperatorCode, gPr645[PortType].dwDataID.b );
		#endif
		return DLT645_OK_00;        
    }   
	
	return wDataLen;
}

#endif

