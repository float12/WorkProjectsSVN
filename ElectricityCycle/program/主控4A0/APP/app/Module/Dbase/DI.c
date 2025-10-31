//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人   
//创建日期 
//描述        遥信文件
//修改记录  
//----------------------------------------------------------------------
#include "AllHead.h"

#if(SEL_EVENT_DI_CHANGE == YES)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define DI_BUFFER						64
#define MAX_DI_NUM						1
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static BYTE bDIStatusBuf[DI_BUFFER];		//遥信缓冲区,存遥信状态
static BYTE bDIWPoint;	  					//遥信缓冲区写指针
static BYTE bDIRPoint[MAX_DI_NUM];			//遥信缓冲区读指针
static BYTE bDIStatus;						//当前遥信状态(处理后)
static BYTE bDIChange;						//规约读取后遥信状态是否变位
TDIPara  DIPara;							//当前遥信防抖动时间及校验
static BYTE bDIEventFlag;					//存储遥信698事件标志

//终端状态量变位事件 0x3104
static const DWORD DIChangeOadConst[] =
{
	0x00,//关联对象的个数
};

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 校验遥信防抖动时间是否正确
//
//参数: 		
//			
// 			                  
//返回值:	无
//
//备注:   
//-----------------------------------------------
void CheckDIPara(void)
{
	if(lib_CheckCRC32((BYTE *)&DIPara.AntiShakeTime,sizeof(DIPara.AntiShakeTime)) != DIPara.CRC32)
	{
		if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara), (BYTE *)&DIPara) == FALSE)
		{
			memcpy((BYTE *)&DIPara.AntiShakeTime,(BYTE *)&DIParaconst,sizeof(TDIPara));
		}
	}
}

//-----------------------------------------------
//函数功能: 遥信上电初始化
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	
//-----------------------------------------------
void api_PowerUpDI(void)
{
	CheckDIPara();
	memset((void*)bDIStatusBuf,0,sizeof(bDIStatusBuf));
	bDIWPoint = 0;
	memset((void*)bDIRPoint,0,sizeof(bDIRPoint));
	bDIStatus = 0;
	bDIChange = 0;
	bDIEventFlag = 0;
}
//-----------------------------------------------
//函数功能: 遥信大循环任务
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	
//-----------------------------------------------
void api_DIProcTask(void)
{
	BYTE uci,ucj;
	BYTE ucStatus,ucStatusN;
	BYTE ucStatus_old;
	DWORD ucCount;

	if(DIPara.bAccessState == 0)
	{
		bDIStatus = 0;
		bDIChange = 0;
		bDIEventFlag = 0;
		return;
	}

	ucStatus_old = bDIStatus;
	for(uci = 0;uci < MAX_DI_NUM;uci++)
	{
		ucStatus = (bDIStatusBuf[bDIRPoint[uci]]&(0x01<<uci));
		ucCount = 0;
		ucj = bDIRPoint[uci];
		while(ucj!=bDIWPoint)
		{
			ucStatusN = (bDIStatusBuf[ucj]&(0x01<<uci));
			if(ucStatus != ucStatusN)
			{//遥信发生变化
				bDIRPoint[uci] = ucj;
				ucStatus = ucStatusN;
				ucCount = 0;
			}
			else
			{
				ucCount += 1;//缓冲两毫秒更新一次
			}
			ucj++;
			if(ucj>=DI_BUFFER)
			{
				ucj = 0;
			}
		}
		if(ucCount >= DIPara.AntiShakeTime)
		{
			if(ucStatusN)
			{
				bDIStatus |= (0x01<<uci);			
			}
			else
			{
				bDIStatus &= (~(0x01<<uci));
			}
			bDIRPoint[uci] = bDIWPoint;
		}
	}
	
	if( ucStatus_old != bDIStatus )
	{
		bDIChange = 1;
		bDIEventFlag = 1;
	}
}
//-----------------------------------------------
//函数功能: 获取存储遥信698事件标志
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	
//-----------------------------------------------
BYTE api_GetDIEventStatus( BYTE Phase )
{
	BYTE bTemp = bDIEventFlag;	
	
	bDIEventFlag = 0;	//获取状态后，变位状态清零
	
	return bTemp;
}
//-----------------------------------------------
//函数功能: 获取当前遥信状态
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	
//-----------------------------------------------
BYTE api_ReadDIStatus( void )
{
	return bDIStatus;
}

//-----------------------------------------------
//函数功能: 获取当前遥信变位状态
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	
//-----------------------------------------------
BYTE api_ReadDIChange( void )
{
	BYTE bTemp = bDIChange;	
	
	bDIChange = 0;	//规约读取后，变位状态清零
	
	return bTemp;
}

//-----------------------------------------------
//函数功能: 实时遥信状态获取
//
//参数: 		无
//			             
//返回值:	无
//
//备注:   	10ms中断调用
//-----------------------------------------------
void api_DISample(void)
{
	BYTE ucTmp = 0xff;

	if(DIGITAL_INPUT0_STATUS)
	{
		ucTmp &= 0xfe;//清零Bit0位
	}
	
	bDIStatusBuf[bDIWPoint] = ucTmp;
	bDIWPoint++;
	if(bDIWPoint >= DI_BUFFER)
	{
		bDIWPoint = 0;
	}  
}

//--------------------------------------------------
//功能描述:  读取遥信防抖动时间
//参数	:	DINO:路数
//			pBuf[out]
//返回值:	Lenth
//注:
//--------------------------------------------------
WORD api_ReadDIPara(BYTE bType,BYTE *pBuf )
{	
	if(bType == 0)
	{
		memcpy(pBuf, (BYTE *)&DIPara.AntiShakeTime,sizeof(DIPara.AntiShakeTime));
		return sizeof(DIPara.AntiShakeTime);	
	}
	else if(bType == 1)
	{
		memcpy(pBuf, (BYTE *)&DIPara.bAccessState,sizeof(DIPara.bAccessState));
		return sizeof(DIPara.bAccessState);			
	}
	else if(bType == 2)
	{
		memcpy(pBuf, (BYTE *)&DIPara.bAttributeState,sizeof(DIPara.bAttributeState));
		return sizeof(DIPara.bAttributeState);			
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  设置遥信防抖动时间
//参数	:	DINO:路数
//			pBuf[in]
//返回值:	BOOL
//注:
//--------------------------------------------------
BOOL api_WriteDIPara(BYTE bType, BYTE *pBuf )
{
	DWORD AntiShakeTime = 0;

	if(bType == 0)
	{
		memcpy((BYTE *)&AntiShakeTime, pBuf, 4);
		DIPara.AntiShakeTime = AntiShakeTime;
	}
	else if(bType == 1)
	{
		if(pBuf[0] > 1)
		{
			return FALSE;
		}
		DIPara.bAccessState = pBuf[0];
	}
	else if(bType == 2)
	{
		if(pBuf[0] > 1)
		{
			return FALSE;
		}
		DIPara.bAttributeState = pBuf[0];		
	}
	if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara),(BYTE *)&DIPara) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------
//函数功能: 初始化遥信防抖动时间
//
//参数: 
//	无
//	
//  返回值:	
//  无
//
//备注:	  初始化调用
//-----------------------------------------------
void api_FactoryInitDIPara(void)
{
	memcpy((BYTE *)&DIPara.AntiShakeTime,(BYTE *)&DIParaconst,sizeof(WORD)*MAX_DI_NUM);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara),(BYTE *)&DIPara);
}

//-----------------------------------------------
//函数功能: 初始化终端状态量变位事件参数
//
//参数: 	无
//	
//  返回值:	  无
//
//备注:	  初始化调用
//-----------------------------------------------
void api_FactoryInitDIChangePara(void)
{
	api_WriteEventAttribute( 0x3104, 0xff, (BYTE *)&DIChangeOadConst[0], sizeof(DIChangeOadConst)/sizeof(DWORD) );
}
#endif//#if(SEL_EVENT_DI_CHANGE == YES)