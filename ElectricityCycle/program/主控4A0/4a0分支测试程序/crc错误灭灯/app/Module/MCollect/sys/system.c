
#include "appcfg.h"
#include "cgy.h"
// extern TData_6203 *gpData_6203;

BOOL sys_File2FlashOffset(char *szFileName,DWORD *pdwMainOffset,DWORD *pdwBakOffset)
{//根据文件名获得FLASH文件偏移
	*pdwBakOffset = 0xFFFFFFFF;
	// if(strcmp(szFileName,FILE_SYSPARA) == 0)
	// {
	// 	*pdwMainOffset	= SYSCFG_OFFSET; 
	// 	*pdwBakOffset	= SYSCFG_OFFSET_BAK;
	// }	
	// else 
	if(strcmp(szFileName,FILE_MPPARA) == 0)
	{
		*pdwMainOffset	= MPCFG_OFFSET;
		*pdwBakOffset	= MPCFG_OFFSET_BAK;
	}	
// 	else if(strcmp(szFileName,FILE_NVRAM) == 0)
// 	{
// 		*pdwMainOffset	= NVRAM_OFFSET;
// 	}
// 	else if(strcmp(szFileName,FILE_CUVFRZ) == 0)
// 	{
// 		*pdwMainOffset	= CURV_OFFSET;
// 	}	
// 	else if(strcmp(szFileName,FILE_DAYFRZ) == 0)
// 	{
// 		*pdwMainOffset	= DFRZ_OFFSET;
// 	}	
// 	else if(strcmp(szFileName,FILE_MONFRZ) == 0)
// 	{
// 		*pdwMainOffset	= MFRZ_OFFSET;
// 	}
// 	else if(strcmp(szFileName,FILE_SETTLEDAYFRZ) == 0)
// 	{
// 		*pdwMainOffset	= SETTLEDAY_OFFSET;
// 	}
// 	else if(strcmp(szFileName,FILE_ALM) == 0)
// 	{
// 		*pdwMainOffset	= ALM_OFFSET;
// 	}
// 	else if(strcmp(szFileName,FILE_SNEWNODE) == 0)
// 	{
// 		*pdwMainOffset	= SNEWNODE_OFFSET;
// 	}
// //	else if(strcmp(szFileName,FILE_UPDATE) == 0)
// //	{
// //		*pdwMainOffset	= OFFSET_UPDATE;
// //	}
// 	else if(strcmp(szFileName,FILE_METERINFO) == 0)
// 	{
// 		*pdwMainOffset	= METERRUNINFO_OFFSET;
// 	}
// #if(MT_MODULE == MT_DF6203_FK_III)
// 	else if(strcmp(szFileName,FILE_CHINAINFO) == 0)
// 	{
// 		*pdwMainOffset	= CHINAINFO_OFFSET;
// 	}
// #endif
// 	else if(strcmp(szFileName,FILE_REAL) == 0)
// 	{
// 		*pdwMainOffset	= REAL_OFFSET;
// 	}
	else	
	{
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : bsp_GetFlashOft
//	函数功能 : 底层获得FLASH偏移
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2016年3月5日
//	返 回 值 : DWORD
//	参数说明 : DWORD dwNo 0- CACH块在FLASH中的物理地址,1-坏块替换的首地址
///////////////////////////////////////////////////////////////
DWORD bsp_GetFlashOft(DWORD dwNo)
{
	if(dwNo == 0)
	{
		return CACHE_BLOCK_OFFSET;
	}
	else if(dwNo == 1)	
		return OFFSET_BLOCK_BADBAK;
	return 0xFFFFFFFF;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : bsp_GetEROMOFT
//	函数功能 : 获得EROM空间中相关偏移
//	处理过程 : 
//	备    注 : file.c中调用
//	作    者 : jiangjy
//	时    间 : 2016年3月4日
//	返 回 值 : WORD
//	参数说明 : WORD wNo 0-坏块表1，1-坏块表2，2-页擦除标识
///////////////////////////////////////////////////////////////
WORD bsp_GetEROMOFT(WORD wNo)
{//获得EEPROM相关参数的起始偏移
#if(!MOS_LINUX)
	if(wNo == 0)
		return EROM_OFT_NANDBADTAB_1;
	else if(wNo == 1)
		return EROM_OFT_NANDBADTAB_2;
	else if(wNo == 2)
		return EROM_OFT_FLASHPAGE_OFT;
#endif
	return 0xFFFF;
}
