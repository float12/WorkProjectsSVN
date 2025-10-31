#ifndef __MID_PORT_H__
#define __MID_PORT_H__
#include "_defA.h"
WORD api_RequestBaseMeterA(BYTE *APDU, BYTE APDULen, WORD BufLen);
WORD api_GetNorm(OAD s_OAD,BYTE *pAPDU);//获取数据
void dataToBuff();//更新数据
void ModuleALoopTask(void);//循环任务
void ModuleASecTask(void);//秒任务
void ModuleAMinTask(void);//分钟任务
void ModuleAHourTask(void);//小时任务
void api_Electric(BYTE *pAPDU);//获取电能量
void ModuleADayTask(void);//日任务
void api_GetTime(BYTE *pAPDU);//扩展模块获取导轨表时间
void api_Electricity(BYTE *pAPDU);//扩展模块获取导轨表时电流
void api_ApparentPower(BYTE *pAPDU);//扩展模块获取导轨表视在功率
void api_ReactivePower(BYTE *pAPDU);//扩展模块获取导轨表无功功率
void api_ActivePower(BYTE *pAPDU);//扩展模块获取导轨表有功功率
void api_Voltage(BYTE *pAPDU);//扩展模块获取导轨表电压
void api_Record(BYTE *pAPDU);//扩展模块更新档案
void api_Record2(BYTE *pAPDU);//更新第二套档案
void api_PowerFactor(BYTE *pAPDU);//功率因素
void api_SeriaNet(BYTE *pAPDU , OAD s_OAD ,BYTE a);//透传数据
void api_OprationIncident(BYTE *pAPDU , SWORD OI , BYTE Happen,BYTE *DataBuff , BYTE DataLength );//扩展模块触发或结束一次事件
void api_SaveParameter(BYTE *pAPDU , BYTE *ExModelNameBuff ,  BYTE ExModelNameLength ,  SWDORD  Addr , SWORD  DataLength , BYTE *DataBuff);//扩展应用自定义参数保存
void api_GetAdd(BYTE *APDU, BYTE APDULen, WORD BufLen);//获取表地址
#endif
