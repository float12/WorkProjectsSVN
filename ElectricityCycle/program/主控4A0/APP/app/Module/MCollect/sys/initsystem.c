#include "appcfg.h"

// TData_6203 *gpData_6203;
extern void CheckClearFrzBlock(BYTE byType,DWORD dwNewTime);
extern void CheckClearCurvBlock(DWORD dwNewTime);
extern void InitGDW698Cfg(void);

// #define DF6203_SIZE ( ((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE )

// BYTE df_DataBuffer[DF6203_SIZE];

// static void _InitSysBaseCfg(void)
// {
	// TVar *pVar=&gVar;

//	memset(&pVar->Cfg,0,sizeof(TERomSysCfg));
//	
//	pVar->Cfg.byUserType	= MAR_STANDARD;	//用户类型
	
// #if(!MOS_MSVC)		
	// pVar->Cfg.byHaveESAM	= 0x55;		//有ESAM
// #endif
//	pVar->Cfg.byCurvSource	= 0x55;
	// pVar->Cfg.wDefaultJCMPNo	= 1;
	//交采类型设置
	// pVar->Cfg.byJCType		= 0x34;		
	
	//确保645-97和07规约必须包含
	// 	pVar->Cfg.GyNo[17][0]	= 10;			//645-1997
	// 	pVar->Cfg.GyNo[17][1]	= 1;			//主站规约号645-1997
	// 	pVar->Cfg.GyNo[18][0]	= 15;			//645_2007
	// 	pVar->Cfg.GyNo[18][1]	= 2;			//主站规约号645_2007
	// 	pVar->Cfg.GyNo[19][0]	= 16;			//698.45
	// 	pVar->Cfg.GyNo[19][1]	= 3;			//主站规约号698.45

// }

// TData_6203 *pGetDF6203(void)
// {
// 	return gpData_6203;
// }

static void _HooK_Reset(void)
{
	//保存历史缓冲区中的数据（日月冻结、实时、历史曲线冻结）
	// FrzBuf2File(REQ_DFRZ);
	// FrzBuf2File(REQ_MFRZ);
	// FrzBuf2File(REQ_CURV);
	// FrzBuf2File(REQ_SETTLEDAY);
	//参数保存
	ParaSaveDaemon(FALSE,NULL);
	// gpData_6203->dwImagicB = IMAGIC_START;
	// gpData_6203->dwImagicE = IMAGIC_END;
	// //保存NVRAM区域
	// SaveNvRam_DF6203();	
}

//todo 掉电或复位时需调用
void api_PowerDownSave(void)
{
	_HooK_Reset();
}

// void SaveNvRam_DF6203(void)
// {
// TData_6203 *pDF6203=pGetDF6203();
// TFileWrite FW;

// 	pDF6203->dwImagicB = IMAGIC_START;
// 	pDF6203->dwImagicE = IMAGIC_END;
// 	//4、保存NVRAM区域
// 	//清除相应的的数据（NANDFLASH需先擦后写，统一擦除，后面直接写入可以提高效率）
// #if MOS_UCOSII
// 	{
// 		TFileClean FC;
// 		FC.iszName	= FILE_NVRAM;
// 		FC.bClearBak= FALSE;
// 		//整块擦除
// 		FC.idwCleanLen = (((sizeof(TData_6203)+SIZE_FLASH_BLOCK-1)/SIZE_FLASH_BLOCK)*SIZE_FLASH_BLOCK);
// 		FC.idwOffset= 0;
// 		FileClean(&FC);
// 	} 	
// #endif
// 	//保存NVRAM数据到FLASH中
// 	FW.iszName	= FILE_NVRAM;
// 	FW.idwOffset= 0;	
// 	//底层的读写以页的方式进行
// 	FW.idwBufLen= (((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE);
// 	FW.ipvBuf	= pDF6203;
// 	FileWrite(&FW);
// }

// void InitDF6203(void)
// {//关联复位函数(复位时会调用相应的函数)	
// DWORD dwFlashSaveSize=((sizeof(TData_6203)+SIZE_PAGE-1)/SIZE_PAGE)*SIZE_PAGE;
// BYTE byBootType;

// 	//gpData_6203		= NvAlloc(dwFlashSaveSize);//分配整数个页
//     gpData_6203 = (TData_6203*)&df_DataBuffer[0];
// 	if((gpData_6203->dwImagicB != IMAGIC_START)
// 		||(gpData_6203->dwImagicE != IMAGIC_END))
// 	{//发生了掉电行为
// 		TFileRead FR;		
// 		FR.iszName	= FILE_NVRAM;
// 		FR.idwOffset= 0;
// 		FR.ipvBuf	= gpData_6203;
// 		FR.idwBufLen= dwFlashSaveSize;
// 		FileRead(&FR);	
// 	#if(!MOS_MSVC)	
// 		if((gpData_6203->dwImagicB != IMAGIC_START)
// 			||(gpData_6203->dwImagicE != IMAGIC_END))
// 	#endif
// 		{	
// 			memset(gpData_6203,0,sizeof(TData_6203));
// 		}	
// 	}
// 	else
// 	// gVar.byStart_reset	= 1;	//当前启动为终端复位(NVRAM没有变化)
// 	gpData_6203->dwImagicB	= IMAGIC_START;
// 	gpData_6203->dwImagicE	= IMAGIC_END;

// 	gpData_6203->dwLastRunTime	= gVar.dwStartTime/60;
// 	Min2Time(gpData_6203->dwLastRunTime,&gVar.GyTime);

// 	//对长期停运后启机的先清除对应的数据块
// 	//1、初始化块擦除日月
// 	CheckClearFrzBlock(REQ_DFRZ,gpData_6203->dwLastRunTime);
// 	CheckClearFrzBlock(REQ_MFRZ,gpData_6203->dwLastRunTime);
// 	//2、检查清除曲线块
// 	CheckClearCurvBlock(gpData_6203->dwLastRunTime);
// }

void InitSysCfg(void)
{
//	InitDF6203();

	// _InitSysBaseCfg();

	InitGDW698Cfg();
}

///////////////////////////////////////////////////////////////
//	函 数 名 : PortID2No
//	函数功能 : 根据端口ID获得实际的内存中的端口序号
//	处理过程 : 
//	备    注 : (只对下行抄表口)
//	返 回 值 : BYTE
//	参数说明 : DWORD dwPortID
///////////////////////////////////////////////////////////////
BYTE PortID2No(DWORD dwPortID)
{//根据端口ID获得实际的内存中的端口序号
	if(HHBYTE(dwPortID) == MPT_SERIAL)
	{
		BYTE port_no=(BYTE)(HLBYTE(dwPortID)-1);
		
		return port_no;
	}
	return 0;
}

