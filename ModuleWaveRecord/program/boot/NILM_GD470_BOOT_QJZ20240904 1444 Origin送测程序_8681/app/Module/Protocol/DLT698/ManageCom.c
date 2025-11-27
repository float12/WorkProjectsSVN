//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	2016.9.3
//描述		
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TManageComInfo ManageComInfo;

/*typedef enum
{	
	eGET_ADDR = 0,
	eGET_SAMPLENUM,
	eGET_SAMPLEINFO,
	eGET_RTC,
	eMAXCOMNO
}eCOMStatus;//与表格一一对应，单调递增*/

BYTE ManageComMsgPkg[][8] =
{
    //顺序不可改变  --与枚举一一对应-枚举格式一一对应
    {0x05, 0x01, 0x00, 0x40, 0x01, 0x02, 0x00,	0x00 	},//eGET_ADDR 		读通信地址
	{0x05, 0x01, 0x00, 0x40, 0x10, 0x02, 0x00,	0x00 	},//eGET_SAMPLENUM	读计量元件数
    {0x05, 0x01, 0x00, 0x48, 0x04, 0x02, 0x00,	0x00	},//eGET_SAMPLENUM	计量配置参数
	{0x05, 0x01, 0x00, 0x40, 0x00, 0x02, 0x00,	0x00	},//eGET_RTC		读时间----常态化同步，最后执行的步骤
};

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

void api_PowerUpManageCOM( void )
{
	memset( &ManageComInfo, 0x00, sizeof(ManageComInfo) );
	ManageComInfo.DelayTime = 0xFFFF;//填充无效值
	ManageComInfo.EventReportOverTime = 0XFFFF;
}

BOOL api_ManageCOM( void )
{
	WORD ReportAPDULen;
	BYTE ReportAPDU[1000];
	
	if( ManageComInfo.DelayTime != 0 )
	{
		//上报事件
		// ReportAPDULen = api_GetReportEventAPDU( &ReportAPDU[0] );
        ReportAPDULen = 0x8000;

		if(ReportAPDULen == 0x8000)
		{
			return FALSE;
		}
		else
		{
			api_ActiveReportResponse( &ReportAPDU[0], ReportAPDULen, 0x55 );
			ManageComInfo.EventReportOverTime = 10;//10秒后重试
			return TRUE;
		}
	}

	if( ManageComInfo.Status >= eMAXCOMNO )//极限值判断
	{
		ManageComInfo.Status = eGET_RTC;
	}

	//主动抄读管理芯数据
	api_ActiveReportResponse( (BYTE*)&ManageComMsgPkg[ManageComInfo.Status], sizeof(ManageComMsgPkg[ManageComInfo.Status]), 0x00 );

	if( ManageComInfo.Status != eGET_RTC )
	{
		ManageComInfo.DelayTime = 5;//每5秒进行同步一次
	}
	else
	{
		ManageComInfo.DelayTime = 21600;//每6小时同步一次
	}

	return TRUE;

}

void api_DealManageCOMStatus( eCOMStatus  COMStatus )
{
	if( (COMStatus == ManageComInfo.Status) && (COMStatus != eGET_RTC) )
	{
		ManageComInfo.Status++;
	}
}

//-----------------------------------------------
//				函数定义
//-----------------------------------------------



