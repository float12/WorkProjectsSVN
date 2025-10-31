
#include "appcfg.h"
#include "cgy.h"
// extern TData_6203 *gpData_6203;

BOOL sys_File2FlashOffset(char *szFileName,DWORD *pdwMainOffset,DWORD *pdwBakOffset)
{//�����ļ������FLASH�ļ�ƫ��
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
//	�� �� �� : bsp_GetFlashOft
//	�������� : �ײ���FLASHƫ��
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2016��3��5��
//	�� �� ֵ : DWORD
//	����˵�� : DWORD dwNo 0- CACH����FLASH�е������ַ,1-�����滻���׵�ַ
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
//	�� �� �� : bsp_GetEROMOFT
//	�������� : ���EROM�ռ������ƫ��
//	������� : 
//	��    ע : file.c�е���
//	��    �� : jiangjy
//	ʱ    �� : 2016��3��4��
//	�� �� ֵ : WORD
//	����˵�� : WORD wNo 0-�����1��1-�����2��2-ҳ������ʶ
///////////////////////////////////////////////////////////////
WORD bsp_GetEROMOFT(WORD wNo)
{//���EEPROM��ز�������ʼƫ��
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
