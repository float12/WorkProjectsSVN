#ifndef __SP_I_UNBALANCE
#define __SP_I_UNBALANCE

// 单相表电流不平衡记录 
#if( SEL_EVENT_SP_I_UNBALANCE == YES )
typedef struct TSPIUnbalanceData_t
{
	// 发生时刻电流1、2值 
	WORD I[2];
	
}TSPIUnbalanceData;
#endif
#if(SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
//-----------------------------------------------
//函数功能: 初始化事件门限值,关联对象属性表
//
//参数: 无
//                    
//返回值:  	无
//
//备注:
//-----------------------------------------------
void FactoryInitNeutralCurrentErrPara( void );
//-----------------------------------------------
//函数功能: 判断是否零线电流不平衡
//
//参数:  无
//                    
//返回值:	掉电状态		FALSE：正常
//					TRUE：零线电流不平衡状态
//
//备注: 
//-----------------------------------------------
BYTE GetNeutralCurrentErrStatus(BYTE Phase);
#endif

#endif
