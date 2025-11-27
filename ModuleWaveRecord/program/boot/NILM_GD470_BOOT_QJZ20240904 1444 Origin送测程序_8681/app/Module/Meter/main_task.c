/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjd
 *日  期：   2020年5月
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
#include "Dlt698_45.h"
#include "task_update.h"

/*初始化应用*/
void InitApp()
{
	// dsp下的配置
    // SysParaInit();
    /* 初始化Main */
    // Init_Sample();
    /* 初始化时间 */
    // Init_Time();
	/* 时间校准初始化 */
    // api_PowerUpRtc();
    /* 模块信息初始化 */
    // api_PowerUpManageCOM();
    /*预处理*/
    // Init_Freq();
    /* 电参量量配置 */
    // Init_Mmxu();
    /* 事件配置 */
    // Init_Evt();
    /*谐波配置*/
    // Init_Harm();
    /* 闪变配置 */
    // Init_Flick();
    /* 电压波动 */
    // Init_fluc();
    /* 统计数据初始化 */
    // Init_Stats();
    /* 冻结上电初始化 */
    // api_PowerUpFreeze();
    /* 事件上电初始化 */
    // api_PowerUpEvent();
}


/*应用主任务*/
void task_main(void)
{
	BYTE i, j, ID;
	WORD Sum;
	DWORD Len;

	if(api_GetSysStatus(eSYS_STATUS_UPDATE_REC_COMPLETE) == TRUE)
	{
		api_ClrSysStatus(eSYS_STATUS_UPDATE_REC_COMPLETE);

		if(api_GetSysStatus(eSYS_STATUS_UPDATE_RECBUF_FULL) == TRUE)
		{
			api_ClrSysStatus(eSYS_STATUS_UPDATE_RECBUF_FULL);

			if(UpdateBuf_pos >= 2)
			{ 
				api_WriteDataToCodeBuffer(UpdateCodeBufferID, UpdateDataLen-UpdateBuf_pos, UpdateBuf_pos-2, UpdateBuffer[UpdateBuf_No]);
			}
		}

		ID = UpdateCodeBufferID;
		Len = UpdateDataLen - 2;
		if(UpdateBuf_pos >= 2)
		{
			Sum = *(WORD *)(&UpdateBuffer[UpdateBuf_No][UpdateBuf_pos-2]);
		}
		else
		{
			Sum = 0;
		}
		//ID = 1;
		api_SetCodeBufferLen(ID, Len);
		api_SetCodeBufferSum(ID, Sum);

		for(i=0;i<6;i++)
		{
			CLEAR_WATCH_DOG;

			// 检查缓冲区校验，重试3次
			if(api_CheckCodeBufferSum(ID) == TRUE)
			{
				for(j=0;j<3;j++)
				{
					CLEAR_WATCH_DOG;

					// 拷贝FLASH数据到Rom
					api_CopyCodeBufferToApp(ID);
					// 检查RAM校验，重试3次
					if(api_CheckAppCodeSum(ID) == TRUE)
					{
						__disable_irq();
						api_SetCodeBufferFlag(1, ID);
						api_SetUpdateFlag(FALSE);
						api_SetCodeBufferFlag(0, api_GetCodeBufferFlag(1));
						api_JumpToApp();//wxy api_JumpToRAM();
					}
				}

				// RAM校验重试三次都不对，软复位
				__set_FAULTMASK(1);
				NVIC_SystemReset();
				
			}

			// 缓冲区校验重试三次都不对，使用另一份缓冲区
			if(i == 2)
			{
				ID = ID ? 0 : 1;
			}
		}

		// 两份缓冲区校验都不对，重新升级
		if(i == 6)
		{
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
	}
	else
	{
		if(api_GetSysStatus(eSYS_STATUS_UPDATE_RECBUF_FULL) == TRUE)
		{
			api_ClrSysStatus(eSYS_STATUS_UPDATE_RECBUF_FULL);

			api_WriteDataToCodeBuffer(UpdateCodeBufferID, UpdateDataLen-UPDATEBUFFERLEN, UPDATEBUFFERLEN, UpdateBuffer[UpdateBuf_No ? 0 : 1]);
		}
	}



}


