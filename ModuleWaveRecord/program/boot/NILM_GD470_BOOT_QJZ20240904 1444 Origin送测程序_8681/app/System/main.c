//------------------------------------------------------------------------------------------
//	名    称： main.c
//	功    能:  系统入口函数
//	作    者:  wangjunsheng
//	创建时间:  2020.10.28
//	更新时间:
//	备    注:
//	修改记录:
//------------------------------------------------------------------------------------------
#include "AllHead.h"
#include "main_task.h"
#include "task_update.h"

volatile DWORD BufferLen=0;

#define ApplicationAddress    0x08010000

void CheckSysLimit( void )
{
	#if(FLASH_UPDATEDATA_END_ADDR > FLASH_EVENT_BASE)
	FLASH中事件区域与升级区域冲突
	#endif
}

 void api_JumpToApp(void)
{
	pFunction Jump_To_Application;
	DWORD JumpAddress;

	//api_splx(0);

	//vControlSwitch(ApplicationAddress,ApplicationAddress+4 );

	JumpAddress = ((*(__IO uint32_t*)ApplicationAddress) );

	JumpAddress += 4;

	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FF00000 ) == 0x20000000) 
	{
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_MSP(*(__IO DWORD*) ApplicationAddress);
		SCB->VTOR = ApplicationAddress;
		Jump_To_Application();
	}
}


#if(INIT_CODEBUFFER == YES)
void InitCodeBuffer(void)
{
	DWORD i, len;
	BYTE tempdata[4096];
	TUpdatePara tempUpdatePara;

	len = sizeof(tempdata);
	memset((void *)tempdata, 0xff, sizeof(tempdata));
	memset((void *)&tempUpdatePara, 0xff, sizeof(tempUpdatePara));

	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(UpdatePara), sizeof(TUpdatePara), (BYTE *)&tempUpdatePara);

	CLEAR_WATCH_DOG;

	for(i=0;i<CODEBUFFER_SIZE;i+=len)
	{
		CLEAR_WATCH_DOG;

		api_WriteDataToCodeBuffer(0, i, len, tempdata);
		api_WriteDataToCodeBuffer(1, i, len, tempdata);
}

for(;;)
{
	CLEAR_WATCH_DOG;
	api_Delayms(1000);
}

}
#endif

int main(void)
{
	BYTE i, j, ID;
	WORD version[16];
	DWORD UID_ADDR;

	BYTE Buffer[100];
	//wxy FLASH_OBProgramInitTypeDef OptionsBytesStruct;

	//SystemInit();

	/* 检查编译系统正确性 */
	CheckSysLimit();

	/* 初始化CPU */
	api_InitCPU();

	/* 初始化板件 */
	InitBoard();

	CLEAR_WATCH_DOG;

	#if(INIT_CODEBUFFER == YES)
	InitCodeBuffer();
	#endif
	// 首次运行写入版本号和UID
	//HAL_FLASH_Unlock();


	memset(version, 0, sizeof(version));
	version[0] = *((WORD *)FLASH_ADDR_BOOT_VERSION);
	if(version[0] == 0xffff)
	{
		version[0] = MODULE_VERSION;
		UID_ADDR = UID_BASE;
		memcpy((void *)&version[1], (void *)&UID_ADDR, 4);
		memcpy((void *)&version[3], (void *)UID_BASE, 12);
			
		api_Flash_write(FLASH_ADDR_BOOT_VERSION, 32, version);
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, FLASH_ADDR_BOOT_VERSION, (DWORD)version);
			
		api_SetCodeBufferLen(0, 0);
		api_SetCodeBufferSum(0, -1);
				
		api_SetCodeBufferLen(1, 0);
		api_SetCodeBufferSum(1, -1);
	}

//测试用：屏蔽后rtt功能不能正常使用，正式程序也无需打开
//    if(ob_spc_get() != SET)
//     {
// 	ob_unlock();
// 	ob_security_protection_config(FMC_LSPC);
// 	ob_start();
// 	ob_lock();
//     }
	
	// 是否需要升级
	if(api_GetUpdateFlag() == FALSE)
	{
		// 获取当前使用的缓冲区ID
		ID = api_GetCodeBufferFlag(0);

		if(ID == 0xff)
		{
			ID = 0;
		}

		for(i=0;i<6;i++)
		{
			CLEAR_WATCH_DOG;

			// 检查缓冲区校验，重试3次
			if(api_CheckCodeBufferSum(ID) == TRUE)//对比外部flash和eep存储的sum，如果一致，返回true
			{
				for(j=0;j<3;j++)
				{
					CLEAR_WATCH_DOG;
					// 检查RAM校验，重试3次
					if(api_CheckAppCodeSum(ID) == TRUE)//对比片内flash和eep存储的sum，如果一致，返回true
					{
						__disable_irq();

						api_SetCodeBufferFlag(1, ID);
						
						api_SetUpdateFlag(FALSE);
											
						api_JumpToApp();
					}
						
					// 拷贝FLASH数据到APP1
					api_CopyCodeBufferToApp(ID);//片内app程序从flash中恢复，此时sum不一致,拷贝大约需要10s左右
					
				}//wxy 
				// RAM校验重试三次都不对，软复位
				goto LABLE;
				__set_FAULTMASK(1);
				NVIC_SystemReset();
			}
			else//对比外部flash和eep存储的sum，此时不一致
			{
				// if(api_CheckAppCodeSum(ID) == TRUE)
				{
					__disable_irq();

					WriteAppToFlashBuffer(ID);			//从内部flash拷贝到外部flash
					api_SetCodeBufferFlag(1, ID);		//设置类型为第二份缓存
					api_SetUpdateFlag(FALSE);			//清零升级标志
								
					api_JumpToApp(); //起始rom的值不是0x20000000，跳转app失败
				}
			}
			
			// 缓冲区校验重试三次都不对，使用另一份缓冲区
			if(i == 2)
			{
				ID = ID ? 0 : 1;
			}
		}
		api_ClrSysStatus(eSYS_STATUS_LED_UPDATING);
		api_ClrSysStatus(eSYS_STATUS_LED_READY_UPDATE);
		api_SetSysStatus(eSYS_STATUS_LED_NOCODE);
	}
	else
	{
		api_SetUpdateFlag(FALSE);

		api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
		api_SetSysStatus(eSYS_STATUS_START_UPDATE);
		InitUSART1(115200);

		api_ClrSysStatus(eSYS_STATUS_LED_UPDATING);
		api_ClrSysStatus(eSYS_STATUS_LED_NOCODE);
		api_SetSysStatus(eSYS_STATUS_LED_READY_UPDATE);
	}
	LABLE:
	api_InitCodeBudder();

	#if(!WIN32)
	__enable_irq();     // 需要提前开启，否则HAL_Delay()进入死循环
	#endif
	api_SetUpdateFlag(FALSE);

	api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
	api_SetSysStatus(eSYS_STATUS_START_UPDATE);
	InitUSART1(115200);

	api_ClrSysStatus(eSYS_STATUS_LED_UPDATING);
	api_ClrSysStatus(eSYS_STATUS_LED_NOCODE);
	api_SetSysStatus(eSYS_STATUS_LED_READY_UPDATE);
		
	for(;;)
	{
		CLEAR_WATCH_DOG;

		/*主任务*/
		task_main();

		api_ProtocolTask();
		api_RtcTask();
	}
}


