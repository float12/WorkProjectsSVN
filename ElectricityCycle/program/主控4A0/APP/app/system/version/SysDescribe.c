//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.9.5
//描述		用户获取电表的版本信息
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//下标[0--MAX_VERSION-1]依次为 功能描述1 功能描述2
//功能描述3  FLASH校验  软件版本号
//功能描述  功能描述   功能描述
WORD Version[MAX_VERSION];


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
//函数功能: 版本管理模块上电初始化函数
//
//参数: 	无
//                    
//返回值:  	无
//
//备注: 只是给显示用，是否需要挪到显示里面或者代码信息缩减一下？!!!!!!  
//-----------------------------------------------
void api_PowerUpVersion( void )
{
	//初始化版本号
	//表类型及功能

	if( MeterTypesConst == METER_3P4W )
	{
		if( MeterVoltageConst == METER_220V )
		{
			Version[0] = 0xC220;
		}
		else
		{
			Version[0] = 0xC057;
		}
	}
    else if( MeterTypesConst == METER_3P3W )
    {
		if( MeterVoltageConst == METER_380V )
		{
			Version[0] = 0xC380;
		}
		else
		{
			Version[0] = 0xC100;
		}
    }
	else//METER_ZT
	{
		Version[0] = 0xC080;
	}

	//功能
	Version[1] = MeterCurrentTypesConst;//电流类型
	//
	Version[2] = (WORD)RelayTypeConst;
	
	Version[3] = api_CheckCpuFlashSum(0xff);

	//软件版本
	Version[4] = (WORD)SoftVersionConst;

	Version[5] = api_CheckCpuFlashSum(0);//程序区校验码

	Version[6] = 0xDF00; //VERSION7; 高位常显DF，低位显示厂内保留地址1处的一个字节。
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(FactoryRcvArea[4]), 1, (BYTE *)&Version[6]);

	Version[7] = (wStandardVoltageConst/10);//(WORD)VERSION8;

	Version[8] = (MeterCurrentTypesConst<<8)|(RelayTypeConst);//VERSION9;

	Version[9] = 0;//VERSION10 Bit1:选择连续空间,这位信息要传，维护软件要用;

	Version[10] = 0;//VERSION11;

	Version[11] = 0;//(WORD)VERSION12;// 显示光报警原因  ;

	Version[12] = 0;//VERSION13;// 显示光报警原因  ;

	Version[13] = 0;//VERSION14;

	//以下两个用于标识模块了
	Version[14] = 0;

	Version[15] = 0;
}


//-----------------------------------------------
//函数功能: 获取对应类型版本
//
//参数: 	Type[in]	对应类型
//                    
//返回值:  	版本信息
//
//备注: 
//-----------------------------------------------
WORD api_GetMeterVersion(BYTE Type)
{
	if( Type >= MAX_VERSION )
	{
		return 0;
	}
	
	return Version[Type];
}



//-----------------------------------------------
//函数功能: 读电表版本
//
//参数:		Buf[out] 输出缓冲
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
WORD api_ReadMeterVersion(BYTE *Buf)
{
	WORD i;
	WORD tw;
	
	i = 0;
	//起始符
	Buf[i++] = 0xaa;
	//数据说明采用 Type+Len结构，Type+Len为1字节，Type占高5位，Len占低3位
	//00001 001    表类型（1字节）
	Buf[i++] = 0x09;
	Buf[i++] = MeterTypesConst;
	//00010 001    电压（1字节）
	Buf[i++] = 0x11;

	Buf[i++] = wStandardVoltageConst/10;

	//00011 001    电流（1字节）
	Buf[i++] = 0x19;
	Buf[i++] = MeterCurrentTypesConst;

	//00101 010    版本（2字节）
	Buf[i++] = 0x2A;
	Buf[i++] = (BYTE)SoftVersionConst;
	Buf[i++] = (WORD)SoftVersionConst>>8;
	//00110 010    用户（2字节）
	Buf[i++] = 0x32;
	Buf[i++] = (BYTE)SpecialClientsConst;
	Buf[i++] = (WORD)SpecialClientsConst>>8;
	//00111 010    全部FLASH效验码（2字节）
	Buf[i++] = 0x3a;
	tw = api_CheckCpuFlashSum(0xff);
	lib_ExchangeData(Buf+i,(BYTE*)&tw,2);
	i += 2;
	//01000 010    板号（2字节）
	Buf[i++] = 0x42;	
	tw = SelThreeBoard;
	memcpy(Buf+i,(BYTE*)&tw,2);
	i += 2;
	//01001 001    计量CPU（1字节）
	Buf[i++] = 0x49;	
	Buf[i++] = CPU_TYPE;
	//01010 001    采样芯片（1字节）
	Buf[i++] = 0x51;	
	Buf[i++] = SAMPLE_CHIP;
	//01011 100    存储芯片（4字节）
	Buf[i++] = 0x5c;	
	Buf[i++] = MASTER_MEMORY_CHIP;
	Buf[i++] = SLAVE_MEM_CHIP;
	Buf[i++] = THIRD_MEM_CHIP;
	Buf[i++] = CHIP_NO;
	//01110 001    液晶驱动（1字节）
	Buf[i++] = 0x71;	
	Buf[i++] = LCD_DRIVE_CHIP;
	//01111 001    ESAM（1字节）
	Buf[i++] = 0x79;	
	Buf[i++] = PREPAY_MODE;
	//10000 001    cpu卡（1字节）
	Buf[i++] = 0x81;	
	Buf[i++] = CARD_COM_TYPE;
	//10001 001    继电器（1字节）
	Buf[i++] = 0x89;
	Buf[i++] = RelayTypeConst;
	
	//10010 100    软件功能（4字节）
	Buf[i++] = 0x94;	
	memset(Buf+i,0x00,4);
	if( SEL_DEBUG_VERSION == YES )
	{
		Buf[i] |= 0x01;
	}
	if( SEL_DLT645_2007 == YES )
	{
		Buf[i] |= 0x02;
	}
	if( SEL_DLT698_2016_FUNC == YES )
	{
		Buf[i] |= 0x04;
	}
	//if( SelCorrectVrefgain == YES ) 已删除自热手动补偿功能
	//{
	//	Buf[i] |= 0x10;
	//}
	if( SelDivAdjust == YES )
	{
		Buf[i] |= 0x20;
	}
	if( SelMChipVrefgain == YES )
	{
		Buf[i] |= 0x40;
	}
	
	if(SelZeroCurrentConst == YES )
	{
		Buf[i+1] |= 0x01;
	}
	if( SelSecPowerConst == YES )
	{
		Buf[i+1] |= 0x02;
	}
/*	if( SelSwitchPowerSupplyConst == YES )
	{
		Buf[i+1] |= 0x04;
	}*/	
	#if( SEL_CONTINUS_FRAM_WAKEUP != IR_WAKEUP_NO_FUNC )
	Buf[i+1] |= 0x08;
	#endif
	
	#if( SEL_24LC256_COMMUNICATION_MOD == YES )
	Buf[i+1] |= 0x10;
	#endif
	
	i = i+4;
	
	//10011 010    程序FLASH效验码（2字节）
	Buf[i++] = 0x9a;
	tw = api_CheckCpuFlashSum(0x00);
	lib_ExchangeData(Buf+i,(BYTE*)&tw,2);
	i += 2;
	
	//结束符
	Buf[i++] = 0xaa;
	
	return i;
}


