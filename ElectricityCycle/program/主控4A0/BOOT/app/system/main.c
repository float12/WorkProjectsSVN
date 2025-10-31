//------------------------------------------------------------------------------------------
//	名    称： main.c
//	功    能:  IAP
//	作    者:  lq
//	创建时间:  2021.9.1
//	更新时间:
//	备    注:
//	修改记录:
//------------------------------------------------------------------------------------------

#include "AllHead.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MAX_SIZE_FOR_IAP						(512 * 1024)									// 用来升级的空间  
#define LEN_OF_ONE_FRAME						200												// 一帧报文的长度
#define FRAMENUM_OF_ONE_SECTOR					(SECTOR_SIZE / LEN_OF_ONE_FRAME)				//一个扇区能够存的报文数20
#define FRAMESIZE_OF_ONE_SECTOR					(FRAMENUM_OF_ONE_SECTOR * LEN_OF_ONE_FRAME) 	//一个扇区能够存字节数
#define SECTOR_NUM_FOR_IAP						((MAX_SIZE_FOR_IAP / FRAMESIZE_OF_ONE_SECTOR)+1)// 存MAX_SIZE_FOR_IAP的升级数据，需要多少个扇区132
#define IAP_CODE_ADDR							((2048 - SECTOR_NUM_FOR_IAP) * SECTOR_SIZE)		// 扇区号 0~2047
#define IAP_INFO_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 1) * SECTOR_SIZE )	// 扇区号 0~2047  多用一个扇区用来存放升级信息
#define IAP_JUDGE_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 2) * SECTOR_SIZE)	// 扇区号 0~2047   存放第一帧判断信息
#define IAP_FLAG								0xA55AA55A
#define IAP_ALL									0x5AA5											// 全部升级标志
#define PPPINITFCS16							0xffff 		/* Initial FCS value */
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct TIap_t
{
	BYTE bDestFile[10];
	BYTE bSourceFile[10];
	DWORD dwFileSize;		// 程序文件大小（Byte）
	BYTE bFileAttr;			// 文件属性
	BYTE bFileVer[10];		// 文件版本
	BYTE bFileType;			// 文件类别
	WORD wFrameSize;		// 传输块大小，默认200字节
	WORD wAllFrameNo;		// 程序文件分割后的总帧数
	WORD wFrameOfOneSector; // 一个扇区可以存放的数据帧数
	WORD wCrc16;			// 整个程序文件的CRC16校验码
	WORD wAllUpdate;		// 1:全部升级  0：部分升级
	DWORD dwIapFlag;		// 升级标志位  0xA55AA55A 有效，其他无效
	DWORD dwCrc32;			// 结构体校验
} T_Iap;
#pragma pack()
typedef void (*iapfun)(void); //定义一个函数类型的参数.
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
//FCS lookup table as calculated by the table generator.
static const WORD fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


iapfun jump2app;
BYTE RunLedFlag;
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述: 设置栈顶地址
//
//参数:addr:栈顶地址
//
//返回值:
//
//备注内容:
//--------------------------------------------------
void MSR_MSP(uint32_t addr)
{
	__asm("MSR MSP, r0 \n"); //set Main Stack value
	__asm("BX r14 \n");
}

//--------------------------------------------------
//功能描述: 跳转到应用程序段
//
//参数:appxaddr:用户代码起始地址.
//
//返回值:
//
//备注内容:
//--------------------------------------------------
void iap_load_app(uint32_t appxaddr)
{
	uint32_t Cnt;
	//	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{
		for (Cnt = 0; Cnt < INT143_IRQn; Cnt++)
		{
			INTC_IrqSignOut((IRQn_Type)Cnt);
		}

		jump2app = (iapfun) * (uint32_t *)(appxaddr + 4); //用户代码区第二个字为程序开始地址(复位地址)
		MSR_MSP(*(uint32_t *)appxaddr);					  //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
//        __set_MSP(*(__IO uint32_t *)appxaddr);
		jump2app();										  //跳转到APP.
	}
}

//-----------------------------------------------
// 函数功能: 根据之前的fcs校验值，继续计算fsc校验
//
// 参数:		BYTE *cp[in]	数据指针
//			WORD len[in] 	数据长度
//			LastFcs[in]		上一次计算的校验值，用于本次计算的初始值
//			IsEnd[in]		是否是最后一段数据 TRUE/FALSE 是/否
//
// 返回值:	WORD
//
// 备注:
//-----------------------------------------------
WORD ContinueFcs16(BYTE *cp, WORD len, WORD LastFcs, BOOL IsEnd)
{
	WORD cpLen;
	WORD fcs;

	fcs = LastFcs;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ((fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	if (IsEnd) // 计算最后一段crc校验时执行
	{
		fcs ^= 0xffff; // complement  //此为FCS值
	}

	return fcs;
}
//--------------------------------------------------
//功能描述:  是否升级判断+升级
//
//参数:
//
//返回值:    CRC16校验
//
//备注内容:
//--------------------------------------------------
#pragma location = 0x7ff0
__root static const BYTE IapFacCode[16] = {"WisdomWaveRecord"};
void DealIap( void )
{
	T_Iap tIap;
	DWORD dwCrc,dwExFlashAddr,dwInFlashAddr,dwAddr;
	WORD w,wLen,wCRC16;
	BOOL boResult;
	BYTE *pData;
	BYTE IapBuf[SECTOR_SIZE+20];
	BYTE LastLen=0;
	WORD LastSize;

	api_ReadMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);

	dwCrc = lib_CheckCRC32((BYTE *)&tIap.bDestFile[0], sizeof(tIap) - sizeof(DWORD)); // 计算校验

	// 检查升级合法性
	if( dwCrc != tIap.dwCrc32 )
	{
		return;// 跳转到App
	}

	if (tIap.dwIapFlag != IAP_FLAG)
	{
		return; // 跳转到App
	}

	// 先点亮RUN LED
	LIGHT_RUN_OPEN;

	if (tIap.dwFileSize % tIap.wFrameSize)
	{
		LastSize = tIap.dwFileSize % tIap.wFrameSize;
	}
	else
	{
		LastSize = tIap.wFrameSize;
	}

	wCRC16 = PPPINITFCS16; // 赋初值

	for (w = 0; w < tIap.wAllFrameNo; w++)
	{
		dwAddr = IAP_CODE_ADDR;
		dwAddr += (w / tIap.wFrameOfOneSector) * SECTOR_SIZE + (w % tIap.wFrameOfOneSector) * tIap.wFrameSize;
		api_ReadMemRecordData(dwAddr, tIap.wFrameSize, IapBuf);

		if (w == (tIap.wAllFrameNo - 1))
		{
			wCRC16 = ContinueFcs16(IapBuf, LastSize, wCRC16, TRUE); // 计算校验
		}
		else
		{
			wCRC16 = ContinueFcs16(IapBuf, tIap.wFrameSize, wCRC16, FALSE); // 计算校验
		}

		// 计算校验，运行灯闪烁
		if( (w %50) == 0 )
		{
			if (RunLedFlag == 0)
			{
				RunLedFlag = 1;
				LIGHT_RUN_OPEN;
			}
			else
			{
				RunLedFlag = 0;
				LIGHT_RUN_STOP;
			}
		}

		CLEAR_WATCH_DOG;
	}

	if (wCRC16 != tIap.wCrc16)
	{
		return; // 跳转到App
	}

	LIGHT_RUN_STOP;
	WARN_ALARM_LED_OFF;
	CLEAR_WATCH_DOG;
	api_Delayms(500);
	LIGHT_RUN_OPEN;

	// 擦除App
	bsp_EraseAppFlash();

	// 升级App
	w = 0;
	dwExFlashAddr = IAP_CODE_ADDR+sizeof(IapFacCode);
	dwInFlashAddr = FLASH_APP_START_ADDR;

	while (w < tIap.wAllFrameNo) // 升级总帧数需要减去前2帧
	{
		CLEAR_WATCH_DOG;

		// 写内部flash，告警灯闪烁
		if (RunLedFlag == 0)
		{
			RunLedFlag = 1;
			WARN_ALARM_LED_ON;
		}
		else
		{
			RunLedFlag = 0;
			WARN_ALARM_LED_OFF;
		}

		if ((w + tIap.wFrameOfOneSector) < tIap.wAllFrameNo)
		{
			wLen = (SECTOR_SIZE / tIap.wFrameSize) * tIap.wFrameSize; // 每次读取外部flash一个扇区的数据（可能不满一个扇区）
		}
		else
		{
			wLen = (tIap.wAllFrameNo - w) * tIap.wFrameSize; // 最后读取不满一个扇区的数据  

			if (tIap.wAllUpdate == IAP_ALL)
			{
				wLen -= tIap.wFrameSize;// 最后一帧
				wLen += LastSize;
			}
			memset(((BYTE *)&IapBuf[0]+LastLen),0xff,sizeof(IapBuf)-LastLen);
		}
		if (w == 0)
		{
			wLen -= sizeof(IapFacCode);
		}

		api_ReadMemRecordData(dwExFlashAddr, wLen, (BYTE *)&IapBuf[0]+LastLen);
		wLen+=LastLen;
		if ((wLen % 4) != 0)
		{
			LastLen = wLen % 4;
			wLen -= LastLen;
			if ((w + tIap.wFrameOfOneSector) >= tIap.wAllFrameNo) // 最后一次搬运
			{
				wLen += 4;
				LastLen = 0;
			}
		}
		else
		{
			LastLen = 0;
		}

		boResult = bsp_WriteCpuFlash(dwInFlashAddr, (BYTE *)&IapBuf[0], wLen);
		if (LastLen != 0)
		{
			memcpy((BYTE *)&IapBuf[0], (BYTE *)&IapBuf[wLen], LastLen);
		}
		if( boResult == FALSE )
		{
			return; // 重新升级
		}
        if (w == 0)
		{
			dwExFlashAddr -= sizeof(IapFacCode);
		}
		w += tIap.wFrameOfOneSector;
		dwExFlashAddr += SECTOR_SIZE;				// 外部一次跨flash一个扇区
		dwInFlashAddr += wLen; // 内部flash

		
	}

	// 计算升级后ROM中的crc校验，判断是否跳转到app
	pData = (BYTE *)FLASH_APP_START_ADDR-sizeof(IapFacCode);
	wCRC16 = PPPINITFCS16; // 赋初值
	for (w = 0; w < (tIap.wAllFrameNo); w++)
	{
		if (w == (tIap.wAllFrameNo - 1))
		{
			wCRC16 = ContinueFcs16(pData, LastSize, wCRC16, TRUE); // 计算校验
		}
		else
		{
			wCRC16 = ContinueFcs16(pData, tIap.wFrameSize, wCRC16, FALSE); // 计算校验
		}
		pData += tIap.wFrameSize;
		CLEAR_WATCH_DOG;
	}

	if (wCRC16 != tIap.wCrc16)
	{
		return;// 重新升级
	}

	// 升级工程，将升级标志置为无效
	tIap.dwIapFlag = 0;
	tIap.dwCrc32 = 0;
	api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.bDestFile[0]);
}

void main(void)
{
	DISABLE_CPU_INT;		// 关闭中断

	LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
	
	api_InitCPU();

	InitBoard();

	LL_PERIPH_WP(EXAMPLE_PERIPH_WP);

	//系统电压自检
	if (api_CheckSysVol(eOnInitDetectPowerMode) == TRUE)
	{
		DealIap();// 升级判断流程
	}

	iap_load_app(FLASH_APP_START_ADDR);
}


