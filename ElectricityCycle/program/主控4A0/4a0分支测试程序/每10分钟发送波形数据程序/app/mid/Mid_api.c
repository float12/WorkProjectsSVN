//----------------------------------------------------------------------
//Copyright (C) 2003-2023 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	    
//创建日期	    2023.04.04
//描述         导轨表定义中间件及扩展功能地址C文件
//修改记录
//---------------------------------------------------------------------- 
#ifdef __cplusplus
 extern "C" {
#endif

#include "mid.h"

//--------------------------------------------中间件相关定义-----------------------------------------------------//

const MidEnterFunction MidEnterFunctionConst  = MidFunction;

//--------------------------------------------扩展功能相关定义-----------------------------------------------------//

const ExpandOneEnterFunction ExpandFunctionList[ Expand_functin_Max_Num + 1 ] =
{
	(ExpandOneEnterFunction)0,					//扩展功能个数
	//(ExpandOneEnterFunction)ExpandFunction,		//扩展功能
	//ExpandFunction2,							//扩展功能
};

#ifdef __cplusplus
}
#endif
































































