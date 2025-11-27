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
__no_init TNAddr_t NAddr;

/*typedef enum
{	
	eGET_ADDR = 0,
	eGET_SAMPLENUM,
	eGET_SAMPLEINFO,
	eGET_RTC,
	eMAXCOMNO
}eCOMStatus;//与表格一一对应，单调递增*/

//读取1个对象属性
BYTE ManageComMsgPkg[][8] =
{
   //顺序不可改变  --与枚举一一对应-枚举格式一一对应
	//GetRequestNormal
	{0x05, 0x01, 0x00, 0x40, 0x01, 0x02, 0x00,	0x00 	},//eGET_ADDR 		读通信地址
	{0x05, 0x01, 0x00, 0x40, 0x10, 0x02, 0x00,	0x00 	},//eGET_SAMPLENUM	读计量元件数
	{0x05, 0x01, 0x00, 0x48, 0x04, 0x02, 0x00,	0x00	},//eGET_SAMPLENUM	计量配置参数
	{0x05, 0x01, 0x00, 0x41, 0x04, 0x02, 0x00,	0x00	},//eGET_V
	{0x05, 0x01, 0x00, 0x41, 0x05, 0x02, 0x00,	0x00	},//eGET_I
	
	{0x05, 0x01, 0x00, 0x40, 0x00, 0x02, 0x00,	0x00	},//eGET_RTC		读时间----常态化同步，最后执行的步骤
};


BYTE ManageComMsgPkgList1[][21] =
{
	{0x05, 0x02, 0x00, 0x04,  0x40, 0x01, 0x02, 0x00,	0x40, 0x10, 0x02, 0x00,		0x48, 0x04, 0x02, 0x00,	  0x40, 0x00, 0x02, 0x00,	0x00 	},//eGET_ADDR 		
};

//读取多个对象属性
BYTE ManageComMsgPkgList[][29] =
{
	{0x05, 0x02, 0x00, 0x06, 	//get-request,getrequestnomallist,PIID,6个OAD
		
	0x40, 0x01, 0x02, 0x00,		//通信地址
	0x40, 0x10, 0x02, 0x00,		//计量元件数
	0x48, 0x04, 0x02, 0x00,	  	//计量配置参数
	0x40, 0x00, 0x02, 0x00,		//日期时间
	0x41, 0x04, 0x02, 0x00,		//额定电压
	0x41, 0x05, 0x02, 0x00,		//额定电流
	
	0x00						//时间标签无 	
	},//eGET_ADDR 		
};

//操作一个对象方法请求
BYTE RequestLink[][9] = 
{
	{0x07, 0x01, 0x00, 0x40, 0x31, 0xFE,0x00, 0x00}, 
};

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern WORD api_MakeSecurityApdu(BYTE byType, BYTE* InBuff, WORD wInLen, BYTE* OutBuffer);


void api_PowerUpManageCOM( void )
{
	memset( &ManageComInfo, 0x00, sizeof(ManageComInfo) );
	ManageComInfo.DelayTime = 0xFFFF;//填充无效值
	ManageComInfo.EventReportOverTime = 0XFFFF;
	
	ManageComInfo.Status =eGET_ADDR;
	
	ManageComInfo.byRecOkFlag = 0;
        
	ManageComInfo.byRetryCount = 0;
}
extern TReportMode gReportMode;
extern TReportPara ReportPara;

void request_link(void)
{
	api_ActiveReportResponse( (BYTE*)&RequestLink[0], sizeof(RequestLink[0]), 0x00);
}

BOOL api_ManageCOM( void )
{
	WORD ReportAPDULen;
	BYTE ReportAPDU[1000];

	if( ManageComInfo.DelayTime != 0 )
	{
		//上报事件
		ReportAPDULen = api_GetReportEventAPDU( &ReportAPDU[0] );

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
		ManageComInfo.Status = eMAXCOMNO;//eGET_RTC;
	}

	//主动抄读管理芯数据
	if(ManageComInfo.Status == eGET_ALL)
	{
		if((ManageComInfo.byRecOkFlag & 0x1E ) == 0x1E)
		{
			ManageComInfo.Status = eGET_RTC;
			ManageComInfo.DelayTime = 21600;//每6小时同步一次
			return TRUE;
		}
		api_ActiveReportResponse( (BYTE*)&ManageComMsgPkgList[0], sizeof(ManageComMsgPkgList[0]), 0x00 );
	}
	else
	{
		#if(USER_TEST_MODE)
		if(ManageComInfo.byRetryCount<5)
		{
			ManageComInfo.byRetryCount++;
		}
		else
		return FALSE;
		#endif

		if( ManageComInfo.Status != eMAXCOMNO )
		{
			ReportAPDULen = api_MakeSecurityApdu(0, (BYTE*)&ManageComMsgPkg[ManageComInfo.Status-1], sizeof(ManageComMsgPkg[ManageComInfo.Status-1]), &ReportAPDU[0]);

			api_ActiveReportResponse( &ReportAPDU[0], ReportAPDULen, 0x00 );
		}
	}

	if( ManageComInfo.Status != eMAXCOMNO )
	{
		#if(USER_TEST_MODE)
		ManageComInfo.DelayTime = 1;//5;//每5秒进行同步一次
		#else
		ManageComInfo.DelayTime = 1;//5;//每5秒进行同步一次
		#endif

		if(ManageComInfo.Status == eGET_I)
		{
			api_DealManageCOMStatus(eGET_I);
		}
		
		if(ManageComInfo.Status == eGET_V )
		{
			api_DealManageCOMStatus(eGET_V);
		}
	}
	else
	{
		ManageComInfo.DelayTime = 21600;//每6小时同步一次
		ManageComInfo.Status = eGET_RTC;
	}

	return TRUE;

}

void api_DealManageCOMStatus( eCOMStatus  COMStatus )
{
	ManageComInfo.byRecOkFlag |= (0x01<<COMStatus);
	
	if( (COMStatus == ManageComInfo.Status) && (COMStatus != eMAXCOMNO))
	{
		ManageComInfo.Status++;
	}
			
	ManageComInfo.byRetryCount = 0;
}

//-----------------------------------------------
//				函数定义
//-----------------------------------------------



