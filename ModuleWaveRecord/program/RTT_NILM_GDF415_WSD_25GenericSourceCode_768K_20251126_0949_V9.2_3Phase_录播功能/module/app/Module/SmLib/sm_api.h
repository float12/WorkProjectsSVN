#ifndef _SM_API_H_
#define _SM_API_H_

#include "light_sm.h"

///////////////////////////////////////////////////////////////
//	函 数 名 : InitSm4Lib
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : void
///////////////////////////////////////////////////////////////
boolean_t InitSm4Lib(void);
boolean_t ClearSm4Lib(void);
boolean_t my_getRand(uint32_t *ranL, uint32_t len) ;
boolean_t my_getRand(uint32_t *ranL, uint32_t len) ;
int sm4_InitKey(void);
#endif

