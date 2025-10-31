//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.10
//描述		矩泉CPU内置时钟接口文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

#if (RTC_CHIP == RTC_HT_INCLOCK)


//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define	C_Toff			0x0000			//温度偏置寄存器
#define	C_MCON01		0x2000			//控制系数01
#define	C_MCON23		0x0588			//控制系数23
#define	C_MCON45		0x4488			//控制系数45
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------


//-----------------------------------------------
//函数功能: 上电将info flash中的系数搬到rtc补偿寄存器中
//
//参数: 	无
//			
//                    
//返回值:  	无
//
//备注:以后放在分钟里面，每分钟也要效验一下!!!!!!   
//-----------------------------------------------
void api_Load_RtcInfoData(void)
{
	BYTE	i;
	DWORD	chksum = 0;
	DWORD	toff;
	short	temp, code1;
	
	EnWr_WPREG();
	
	for(i=0; i<14; i++)
	{
		chksum += HT_INFO->DataArry[i];
	}
       
    if(chksum == HT_INFO->DataArry[14])		//已设置
	{		
		if (HT_RTC->DFAH    != HT_INFO->Muster.iDFAH)   HT_RTC->DFAH    = HT_INFO->Muster.iDFAH;
        if (HT_RTC->DFAL    != HT_INFO->Muster.iDFAL)   HT_RTC->DFAL    = HT_INFO->Muster.iDFAL;
        if (HT_RTC->DFBH    != HT_INFO->Muster.iDFBH)   HT_RTC->DFBH    = HT_INFO->Muster.iDFBH;
        if (HT_RTC->DFBL    != HT_INFO->Muster.iDFBL)   HT_RTC->DFBL    = HT_INFO->Muster.iDFBL;
        if (HT_RTC->DFCH    != HT_INFO->Muster.iDFCH)   HT_RTC->DFCH    = HT_INFO->Muster.iDFCH;
        if (HT_RTC->DFCL    != HT_INFO->Muster.iDFCL)   HT_RTC->DFCL    = HT_INFO->Muster.iDFCL;
        if (HT_RTC->DFDH    != HT_INFO->Muster.iDFDH)   HT_RTC->DFDH    = HT_INFO->Muster.iDFDH;
        if (HT_RTC->DFDL    != HT_INFO->Muster.iDFDL)   HT_RTC->DFDL    = HT_INFO->Muster.iDFDL;
        if (HT_RTC->DFEH    != HT_INFO->Muster.iDFEH)   HT_RTC->DFEH    = HT_INFO->Muster.iDFEH;
        if (HT_RTC->DFEL    != HT_INFO->Muster.iDFEL)   HT_RTC->DFEL    = HT_INFO->Muster.iDFEL;
        if (HT_RTC->Toff    != HT_INFO->Muster.iToff)   HT_RTC->Toff    = HT_INFO->Muster.iToff;
        if (HT_RTC->MCON01  != HT_INFO->Muster.iMCON01) HT_RTC->MCON01  = HT_INFO->Muster.iMCON01;
        if (HT_RTC->MCON23  != HT_INFO->Muster.iMCON23) HT_RTC->MCON23  = HT_INFO->Muster.iMCON23;
        if (HT_RTC->MCON45  != HT_INFO->Muster.iMCON45) HT_RTC->MCON45  = HT_INFO->Muster.iMCON45;

	}
    else//补偿系数未设置，则写入默认值
    {
		if (HT_RTC->DFAH	!= TAB_DFx_K[0])	HT_RTC->DFAH	= TAB_DFx_K[0];
		if (HT_RTC->DFAL	!= TAB_DFx_K[1])	HT_RTC->DFAL	= TAB_DFx_K[1];
		if (HT_RTC->DFBH	!= TAB_DFx_K[2])	HT_RTC->DFBH	= TAB_DFx_K[2];
		if (HT_RTC->DFBL	!= TAB_DFx_K[3])	HT_RTC->DFBL	= TAB_DFx_K[3];
		if (HT_RTC->DFCH	!= TAB_DFx_K[4])	HT_RTC->DFCH	= TAB_DFx_K[4];
		if (HT_RTC->DFCL	!= TAB_DFx_K[5])	HT_RTC->DFCL	= TAB_DFx_K[5];
		if (HT_RTC->DFDH	!= TAB_DFx_K[6])	HT_RTC->DFDH	= TAB_DFx_K[6];
		if (HT_RTC->DFDL	!= TAB_DFx_K[7])	HT_RTC->DFDL	= TAB_DFx_K[7];
		if (HT_RTC->DFEH	!= TAB_DFx_K[8])	HT_RTC->DFEH	= TAB_DFx_K[8];
		if (HT_RTC->DFEL	!= TAB_DFx_K[9])	HT_RTC->DFEL	= TAB_DFx_K[9];

        toff = *(DWORD*)0x4012C;				//Toff低位
        temp = *(short*)0x4015E;				//温度
        code1 = *(short*)0x4015C;				//TPS code
        if( ( (toff>0x0BB8) && (toff<0xF448) )
        	|| (temp < 2000) 
        	|| (temp > 3000)
        	|| (code1 < -7000) 
        	|| (code1 > -1000) )
        {
            toff = C_Toff;
        }
        if (HT_RTC->Toff	!= toff)		HT_RTC->Toff	= toff;
        if (HT_RTC->MCON01	!= C_MCON01)	HT_RTC->MCON01	= C_MCON01;
        if (HT_RTC->MCON23	!= C_MCON23)	HT_RTC->MCON23	= C_MCON23;
        if (HT_RTC->MCON45	!= C_MCON45)	HT_RTC->MCON45	= C_MCON45;
    }
    
    DisWr_WPREG();
}


//-----------------------------------------------
//函数功能: 写info flash中的数据
//
//参数: 	pBuff[in]	要写入的数据
//			addr[in]	偏移地址
//          len[in]     写入数据长度    
//返回值:  	无
//
//备注:   	*info：      接收数据指针
//          pBuff[0]  = 系数A高7位
//          pBuff[1]  = 系数A低16位
//          pBuff[2]  = 系数B高7位
//          pBuff[3]  = 系数B低16位
//          pBuff[4]  = 系数C高7位
//          pBuff[5]  = 系数C低16位
//          pBuff[6]  = 系数D高7位
//          pBuff[7]  = 系数D低16位
//          pBuff[8]  = 系数E高7位
//          pBuff[9]  = 系数E低16位
//          pBuff[10] = Toff温度校准
//          pBuff[11] = MCON01控制系数01
//          pBuff[12] = MCON23控制系数23
//          pBuff[13] = MCON45控制系数45
//          pBuff[14] = 累加和; 
//-----------------------------------------------
void Prog_InfoData(const BYTE *pBuff, WORD addr, BYTE len)
{
	WORD	i;
	BYTE	info[1024];//6015--256字节1页	6025--1k字节1页
	
	//整页读取
	for (i=0; i<1024; i++)
	{
		info[i] = *((BYTE*)(HT_INFO_BASE +i));								
	}
	//更新RTC参数
	for (i=0; i<len; i++)
	{
		info[(addr & 0x03FF) + i] = pBuff[i];								
	}
	//擦除InfoData
	EnWr_WPREG();
	HT_CMU->FLASHLOCK = 0x7A68;					//unlock flash memory
	HT_CMU->INFOLOCK  = 0xF998;					//unlock information flash memory
	HT_CMU->FLASHCON  = 0x02;					//page erase
	*((DWORD*)HT_INFO_BASE) = 0xFFFFFFFF;		//data
	while (HT_CMU->FLASHCON & 0x04)				//FLASH_BSY
		;
	HT_CMU->FLASHCON  = 0x00;					//read only
	
	api_Delayms(2);
	//更新InfoData
	HT_CMU->FLASHCON  = 0x01;					//word write
	for (i=0; i<1024; i++)
	{
		*((BYTE*)(HT_INFO_BASE + i)) = info[i];	//program word
		while (HT_CMU->FLASHCON  & 0x04)		//FLASH_BSY
		{
			__NOP();
		}
	}
	HT_CMU->FLASHLOCK = ~0x7A68;				//lock flash memory
	HT_CMU->INFOLOCK  = ~0xF998;				//lock information flash memory
	HT_CMU->FLASHCON  = 0x00;					//read only
	DisWr_WPREG();
	
	api_Delayms(2);
	
	api_Load_RtcInfoData();
}


//-----------------------------------------------
//函数功能: 读取温度
//
//参数: 	无
//			
//                    
//返回值:  	温度值
//
//备注:   
//-----------------------------------------------
signed short  ADC_TempVolt(void)
{
	signed short 	temp, toff;
	
	toff = HT_RTC->Toff;
	temp = HT_TBS->TMPDAT;
	temp -= toff;
	return temp;
}



//-----------------------------------------------
//函数功能: RTC误差校准
//
//参数:		err[in]   秒脉冲误差(每天多少秒，单位0.001)  最高位为1表示负值
//						
//返回值:	TRUE/FALSE
//		   
//备注:
//-----------------------------------------------
BOOL api_CaliRtcError( WORD err )
{
	BYTE	i, j, num;
	BYTE	info[60];
	TRtcTemperature RtcTemperature;
	DWORD	chksum;
	signed long	ratio[5];				//当前补偿系数
	signed short error, tps[3];
	signed long long	tmp64s;			//中间变量
	double	FN, tmp;
	double	add[3];						//修正补偿系数
    double	TT13, TT23, T13, T23;

	//RTC误差及温度采样
	if( err & 0x8000 )
    {
    	error = (err & ~0x8000);
    	error *= (-1);
    }
    else
    {
    	error = err;
    }
    
    //RTC误差及温度采样 -30.0s/d ~ +30.0s/d
    if( (error<-30000) || (error>30000) )                                
    {
        return FALSE;
    }
	
	tps[0] = ADC_TempVolt();							//当前TpsCode
	for (i=0; i<3; i++)
	{
		if ( (tps[0]<=TAB_Temperature[i*2+0])
			&& (tps[0]>=TAB_Temperature[i*2+1]) )		//-50C~0C; +15C~+35C; +50C~+100C
		{
			break;
		}
	}
	if (i >= 3)
	{
		return FALSE;
	}
	j = i;												//当前采样点
	num = 1;											//1个有效点
	//已采样数据收集 //读取温度采样数据
	if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == FALSE )
	{
		return FALSE;
	}

	for (i=0; i<3; i++)
	{
		if (i != j)										//不同点
		{
			tps[num] = (RtcTemperature.Temperature[i*2] | (RtcTemperature.Temperature[i*2+1]<<8) );
			if ( (tps[num]<=TAB_Temperature[i*2+0])
				&& (tps[num]>=TAB_Temperature[i*2+1]) )	//-50C~0C; +15C~+35C; +50C~+100C
			{
				num++;									//有效点
			}
		}
		else											//同一点
		{
			RtcTemperature.Temperature[i*2]   = tps[0];
			RtcTemperature.Temperature[i*2+1] = tps[0]>>8;
		}
	}
	
	//更新温度采样数据
	if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == FALSE )
	{
		return FALSE;
	}
	
	//读出InfoData
	ratio[0] = HT_RTC->DFAH<<16 | HT_RTC->DFAL;
	ratio[1] = HT_RTC->DFBH<<16 | HT_RTC->DFBL;
	ratio[2] = HT_RTC->DFCH<<16 | HT_RTC->DFCL;
	ratio[3] = HT_RTC->DFDH<<16 | HT_RTC->DFDL;
	ratio[4] = HT_RTC->DFEH<<16 | HT_RTC->DFEL;

	for (i=0; i<5; i++)
	{
		if (ratio[i] & 0x00400000)
		{
			ratio[i] |= 0xFF800000;
		}
	}
	
	tmp64s  =  ratio[4];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[3];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[2];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[1];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[0];
	tmp64s +=  2;
	tmp64s >>= 2;
	
	FN  = (double)tmp64s/512.0 +32768;
	tmp = error / 1000.0;									//转换为double型
	FN  = FN*tmp/(86400-tmp);
	//曲线拟合
	if (num == 3)											//3点拟合
	{
		TT13 = (double)(tps[0]+tps[2]) * (tps[0]-tps[2]);	//dTps[0]^2 - dTps[2]^2
		TT23 = (double)(tps[1]+tps[2]) * (tps[1]-tps[2]);	//dTps[1]^2 - dTps[2]^2
		
		T13  = (double)(tps[0]-tps[2]);						//dTps[0] - dTps[2]
		T23  = (double)(tps[1]-tps[2]);						//dTps[0] - dTps[2]
		
		TT23 *= T13;
		TT23 /= T23;
		TT23 = TT13 - TT23;
		
		add[2] = FN/TT23;									//2次系数修正
		add[1] = (FN-add[2]*TT13)/T13;						//1次系数修正
		add[0] = FN -add[2]*tps[0]*tps[0] -add[1]*tps[0];	//0次系数修正

		add[2] = add[2] *128 *16 *65536 *65536;
		add[1] = add[1] *128 *16 *65536;
		add[0] = add[0] *128 *16;
	}
	else if (num == 2)										//2点拟合
	{
		T23  = (double)(tps[1]-tps[0]);
		
		add[1] = -FN/T23;
		add[0] = -add[1]*tps[1];
		
		add[2] = 0;
		add[1] = add[1] *128 *16 *65536;
		add[0] = add[0] *128 *16;
	}
	else													//1点拟合
	{
		add[2] = 0;
		add[1] = 0;
		add[0] = FN*128*16;
	}
    ratio[0] += (signed long)add[0];						//0次补偿系数
    ratio[1] += (signed long)add[1];						//1次补偿系数
    ratio[2] += (signed long)add[2];						//2次补偿系数
	//写入InfoData数据
	chksum = 0;
	for (i=0; i<5; i++)
	{
		chksum += (ratio[i]>>16) & 0x007F;
		chksum += ratio[i]       & 0xFFFF;
	}
	chksum += HT_RTC->Toff;
	chksum += HT_RTC->MCON01;
	chksum += HT_RTC->MCON23;
	chksum += HT_RTC->MCON45;
	
	for (i=0; i<5; i++)
	{
		info[i*8+0]  = (ratio[i]>>16) & 0x7F;
		info[i*8+1]  = 0x00;
		info[i*8+2]  = 0x00;
		info[i*8+3]  = 0x00;
		info[i*8+4]  = ratio[i]       & 0xFF;
		info[i*8+5]  = (ratio[i]>>8)  & 0xFF;
		info[i*8+6]  = 0x00;
		info[i*8+7]  = 0x00;
	}
	info[40]  = HT_RTC->Toff & 0xFF;
	info[41]  = (HT_RTC->Toff>>8) & 0xFF;
	info[42]  = 0x00;
	info[43]  = 0x00;
	info[44]  = HT_RTC->MCON01 & 0xFF;
	info[45]  = (HT_RTC->MCON01>>8) & 0xFF;
	info[46]  = 0x00;
	info[47]  = 0x00;
	info[48]  = HT_RTC->MCON23 & 0xFF;
	info[49]  = (HT_RTC->MCON23>>8) & 0xFF;
	info[50]  = 0x00;
	info[51]  = 0x00;
	info[52]  = HT_RTC->MCON45 & 0xFF;
	info[53]  = (HT_RTC->MCON45>>8) & 0xFF;
	info[54]  = 0x00;
	info[55]  = 0x00;
	
	info[56]  = chksum & 0xFF;
	info[57]  = (chksum>>8) & 0xFF;
	info[58]  = (chksum>>16) & 0xFF;
	info[59]  = (chksum>>24) & 0xFF;
	
	Prog_InfoData(&info[0], 0x0104, 60);
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: RTC时钟脉冲输出配置
//
//参数:		Type[in]   秒脉冲误差(每天多少秒，单位0.001)  最高位为1表示负值
//				Type -- 0			1HZ脉冲输出
//				Type -- 不为零	禁止秒脉冲输出					
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void api_InitOutClockPuls(BYTE Type)
{
    if( Type == 0 )
    {
      	//由高频补偿得到的1Hz
      	HT_RTC->RTCCON = 0x0006;
    }
    else
    {
      	//关闭秒脉冲输出
      	HT_RTC->RTCCON = 0;
    }
}


//-----------------------------------------------
//函数功能: 写外部时钟
//
//参数: 
//			t[in]		要写入的时钟指针,时钟结构内的数据要求是hex码
//                    
//返回值:  	TRUE:正确写入  FALSE:错误写入
//
//备注:   
//-----------------------------------------------
BOOL WriteOutClock(TRealTimer * t)
{
	DWORD tdw;
	
    tdw = api_CalcInTimeRelativeSec( t );
    if( tdw == ILLEGAL_VALUE_8F )
    {
    	return FALSE;
    }
    
    tdw = ((tdw/60/1440)+6)%7;//计算结果为0时，表示周日
    if( tdw == 0 )//6025 HT_RTC->WEEKR寄存器允许设置值范围1~7，对应周一~周日
    {
        tdw = 7;
    }

    
    EnWr_WPREG();
	HT_RTC->RTCWR = 0x0000;
	
	HT_RTC->YEARR = (t->wYear%100);//年是20XX Hex码 按照年、月、日、时、分、秒、周的顺序
	HT_RTC->MONTHR= (t->Mon);
	HT_RTC->DAYR  = (t->Day);                   
	HT_RTC->HOURR = (t->Hour);
	HT_RTC->MINR  = (t->Min);
	HT_RTC->SECR  = (t->Sec);
	HT_RTC->WEEKR = tdw; 
    
	HT_RTC->RTCWR = 0x0001;
//	while (HT_RTC->RTCWR & 0x0001)
//		;
	DisWr_WPREG();

	api_WriteFreeEvent( EVENT_WRITE_HARD_TIME, 0 );
	
	return TRUE;
}


//-----------------------------------------------
//函数功能: 读外部时钟的底层函数，模块内函数
//
//参数: 
//			Type[in]		
//                    
//返回值:  	TRUE:正确写入  FALSE:错误写入
//
//备注:   
//-----------------------------------------------
BOOL ReadOutClockHara( WORD Type )
{
    TRealTimer TmpRealTimer;
    //用于秒同步
	BYTE SecSync;
	
    HT_RTC->RTCRD = 0x0001;
	while (HT_RTC->RTCRD & 0x0001)
		;
    
	if(Type == 1)
	{
        SecSync=HT_RTC->SECR;
		return TRUE;
	}
	
	TmpRealTimer.wYear = 2000+(HT_RTC->YEARR);
	TmpRealTimer.Mon = (HT_RTC->MONTHR);
	TmpRealTimer.Day = (HT_RTC->DAYR);
	TmpRealTimer.Hour = (HT_RTC->HOURR);
	TmpRealTimer.Min = (HT_RTC->MINR);
	TmpRealTimer.Sec = (HT_RTC->SECR);

    DISABLE_CLOCK_INT;
    memcpy((BYTE*)&RealTimer,(BYTE*)&TmpRealTimer, sizeof(TRealTimer) );
    ENABLE_CLOCK_INT;
    
    if( api_CheckClock( (TRealTimer*)&TmpRealTimer ) == TRUE )
	{	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//---------------------------------------------------------------
//函数功能: 上电复位RTC模块
//
//参数: 	无
//                   
//返回值:   无
//
//备注:  HT6025H 如果VRTC电源在0.7V左右 上电可能导致RTC模块复位失败
//		出现较大的日计时误差 因此上电前如果电池欠压或者产生了掉电和上电复位
// 		上电软复位一次RTC 
// 		复位RTC会影响RTC和TBS相关寄存器 需要重新配置！！！！
//---------------------------------------------------------------
void PowerOnResetRTC( void )
{
	//如果上电前电池欠压或者产生了LBOR或者POR复位
	if ((api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE)
		|| (HT_PMU->RSTSTA&0x0003))
	{
		HT_RTC->RTCRSTSET = 0xAAAA; //RTC 软复位
		HT_RTC->RTCRSTSET = 0x5555;
		api_Delayms( 2 ); //等待复位完成 钜泉程序有1ms延时 此处放到2ms
	}
}

//-----------------------------------------------
//函数功能: 外部时钟初始化，模块内函数
//
//参数: 	无
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void InitRtc( void )
{
	PowerOnResetRTC();	//复位后需重新配置RTC和TBS相关寄存器 该函数不可随意调整位置
	
	api_Load_RtcInfoData();
	
	api_MCU_RTCInt( );		//打开秒 分 时中断
	
	api_InitOutClockPuls(0);	//打开时钟芯片脉冲输出
}

//-----------------------------------------------
//函数功能: 规约读写rtc调校系数
//
//参数: 	Type[in]	READ/WRITE
//			Buf[in/out] 缓冲                  
//返回值:  	Buf长度
//
//备注:   
//-----------------------------------------------
BYTE api_ProcRtcPara(BYTE Type, BYTE *Buf)
{
	if( Type == READ )
	{
		if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), Buf) == FALSE )
		{
			return 0;
		}
	}
	else
	{
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), Buf) == FALSE )
		{
			return 0;
		}
	}
	
	return 6;
}

//---------------------------------------------------------------
//函数功能: 读写InfoFlash中RTC的A、B、C、D、E校正系数
//
//参数: 	pBuff: 校正系数数据
//                   
//返回值:  	pBuff长度
//
//备注:   
//---------------------------------------------------------------
void ProcInfoRtcAdjustPara( BYTE Type, BYTE *pBuff )
{
	BYTE i = 0;
	BYTE Info[60];
	DWORD chksum = 0;
	
	if( Type == WRITE )
	{
		//计算A、B、C、D、E系数
		for(i = 0; i < 5; i++)
		{
			//高位
			Info[i*8+0] = pBuff[i*4+2] & 0x7F;
			Info[i*8+1] = 0x00;
			Info[i*8+2] = 0x00;
			Info[i*8+3] = 0x00;
			//低位
			Info[i*8+4] = pBuff[i*4+0] & 0xFF;
			Info[i*8+5] = pBuff[i*4+1] & 0xFF;
			Info[i*8+6] = 0x00;
			Info[i*8+7] = 0x00;
		}
		//计算温度偏差和控制字
		for(i = 0; i < 4; i++)
		{
			Info[i*4+40] = HT_INFO->DataArry[i+10] & 0xFF;
			Info[i*4+41] = (HT_INFO->DataArry[i+10] >> 8) & 0xFF;
			Info[i*4+42] = 0x00;
			Info[i*4+43] = 0x00;
		}
		
		//计算CheckSum校验和
		for (i = 0; i < 5; i++)
		{
			//系数高位
			chksum += Info[i*8+0];
			//系数低位
			chksum += ((Info[i*8+5] << 8) + Info[i*8+4]);
		}
		chksum += HT_INFO->DataArry[10];//iToff
		chksum += HT_INFO->DataArry[11];//iMCON01
		chksum += HT_INFO->DataArry[12];//iMCON23
		chksum += HT_INFO->DataArry[13];//iMCON45
		
		Info[56] = chksum & 0xFF;
		Info[57] = (chksum>>8) & 0xFF;
		Info[58] = (chksum>>16) & 0xFF;
		Info[59] = (chksum>>24) & 0xFF;
		
		//写入InfoFlash
		Prog_InfoData( Info, 0x0104, sizeof(Info) );
	}
	else
	{
		//读取A、B、C、D、E系数
		for (i = 0; i < 5; i++)
		{
			pBuff[i*4+0] = HT_INFO->DataArry[i*2+1] & 0xFF;
			pBuff[i*4+1] = (HT_INFO->DataArry[i*2+1] >> 8) & 0xFF;
			pBuff[i*4+2] = HT_INFO->DataArry[i*2+0] & 0xFF;
			pBuff[i*4+3] = (HT_INFO->DataArry[i*2+0] >> 8) & 0xFF;
		}
	}
}

//---------------------------------------------------------------
//函数功能: 写InfoFlash中RTC校正参数错误校验和
//
//参数: 	无
//                   
//返回值:  	无
//
//备注:   校验和错误会采用默认参数, 不使用InfoFlash中的参数
//---------------------------------------------------------------
void WriteInfoRTCErrorCheckSum( void )
{
	BYTE i = 0;
	BYTE Info[4];
	DWORD chksum = 0;
	
	//计算正确校验和
	for(i = 0; i < 14; i++)
	{
		chksum += HT_INFO->DataArry[i];
	}
	//校验和加1
	chksum += 1;
	Info[0] = chksum & 0xFF;
	Info[1] = (chksum>>8) & 0xFF;
	Info[2] = (chksum>>16) & 0xFF;
	Info[3] = (chksum>>24) & 0xFF;
	//写入错误校验和
	Prog_InfoData( Info, 0x013C, sizeof(Info) );
}

//-----------------------------------------------
//函数功能: 时钟补偿电表初始化
//
//参数: 	无
//                    
//返回值:  	无
//
//备注: 清补偿系数  
//-----------------------------------------------
void api_FactoryInitRtc(void)
{
	TRtcTemperature RtcTemperature;
	
	if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == TRUE )
	{
		return;
	}
	
	//初始化温度值为非法值, 保证RTC补偿曲线为1点拟合
	memset( RtcTemperature.Temperature, 0x00, sizeof(RtcTemperature.Temperature) );
	
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature);
}

#endif//#if (RTC_CHIP == RTC_HT_INCLOCK)
