/*****************************************************************************/
/*
 *项目名称： 物联网负荷辨识模组
 *创建人：   低压台区软件
 *日  期：   2023年1月
 *修改人：
 *日  期：
 *描  述：
 *
 *版  本：
 *说  明:    计算主程序 采样中断入口
 *
 */
/*****************************************************************************/
#include "AllHead.h"
#include "main_task.h"
#include "task_sample.h"
#include "Dlt698_45.h"


/*初始化应用*/
void InitApp()
{
	// dsp下的配置
	SysParaInit();
	/* 初始化Main */
	Init_Sample();
	/* 初始化时间 */
	// Init_Time();
	/* 时间校准初始化 */
	api_PowerUpRtc();
	/* 模块信息初始化 */
	api_PowerUpManageCOM();

	/* 冻结上电初始化 */
	api_PowerUpFreeze();

	api_ReadHisLastTime();
	
	/* 事件上电初始化 */
	api_PowerUpEvent();
}


