//---------------------------------------------
// 罗氏线圈变更、终端抄表失败记录
//---------------------------------------------

#include "AllHead.h"
#include "Rogowski.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
BYTE bRogowskiChangeFlag=0,bRogowskiFailFlag=0;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )

// 互感器线圈变更 0x3053  的关联对象属性表 oad采用正序输入
static const DWORD RogowskiChangeDefaultOadConst[] =
{
	2,//关联对象的个数
	0x40332200,	// 变更前的罗氏线圈识别结果
	0x40338200,	// 变更后的罗氏线圈识别结果
};
#endif

//终端抄表失败 0x310F
#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
static const DWORD RogowskiFailDefaultOadConst[] =
{
	0//关联对象的个数	
};
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------


//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitRogowskiPara( void )
{
	#if( SEL_EVENT_ROGOWSKI_CHANGE == YES )
	api_WriteEventAttribute( 0x3053, 0xff, (BYTE *)&RogowskiChangeDefaultOadConst[0], sizeof(RogowskiChangeDefaultOadConst)/sizeof(DWORD) );
	#endif
	
	#if( SEL_EVENT_ROGOWSKI_COMM_FAIL == YES )
	api_WriteEventAttribute( 0x310f, 0xff, (BYTE *)&RogowskiFailDefaultOadConst[0], sizeof(RogowskiFailDefaultOadConst)/sizeof(DWORD) );
	#endif
}
BYTE api_GetRogowskiChangEventStatus( BYTE Phase )
{
	return bRogowskiChangeFlag;
}
BYTE api_GetRogowskiFailEventStatus( BYTE Phase )
{
	return bRogowskiFailFlag;
}
