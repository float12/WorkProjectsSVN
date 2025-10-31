#include "__define.h"
#include "paradef.h"
//#include "os.h"
#include "usrtime.h"
#include "configs/DF6203Cfg.h"
#include "AllHead.h"
#define DEBUG_UCOSII	YES

#define SIZE_PAGE 2048
#define SECTOR_SIZE		4096
// void InitBadBlockTab(void);
#if(MOS_UCOSII || (MOS_MSVC && DEBUG_UCOSII))
extern DWORD bsp_GetFlashOft(DWORD dwNo);
extern WORD bsp_GetEROMOFT(WORD wNo);
extern BOOL sys_File2FlashOffset(char *szFileName,DWORD *pdwMainOffset,DWORD *pdwBakOffset);
extern void WatchDog(void);
// static BYTE TmpFileBuf[2048];
// static BYTE TmpFileBuf2[2048];
static WORD _GetCacheBlock(void);
#endif
// static DWORD _FileRead(TFileRead *pR);
// BOOL Flash_Block_Erase(DWORD dwPage);
// BOOL Flash_Page_Read(DWORD dwPage, BYTE *pBuf);
// BOOL Flash_Page_Write(DWORD dwPage, BYTE *pBuf);

// BOOL bspEEPROMRead(WORD addr,WORD readlen,BYTE *pdata)
// {
// 	return api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(GatherConROM[addr]), readlen, pdata);
// }
// BOOL bspEEPROMWrite(WORD addr,BYTE *pdata,WORD writelen)
// {
// 	return api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(GatherConROM[addr]), writelen, pdata);
// }

#if(MOS_UCOSII || (MOS_MSVC && (DEBUG_UCOSII)))



///////////////////////////////////////////////////////////////
//	函 数 名 : _BlockCopy
//	函数功能 : 块拷贝（带数据更新）
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2016年6月28日
//	返 回 值 : BOOL
//	参数说明 : WORD wSrcBlockNo,
//				WORD wObjBlockNo,
//				BYTE *pBuf,
///////////////////////////////////////////////////////////////
// static BOOL _BlockCopy(WORD wSrcBlockNo,WORD wObjBlockNo,BYTE *pBuf)
// {
//     BYTE byI;
//     BYTE TmpFileBuf[2048];

// 	if(!Flash_Block_Erase(wObjBlockNo*64))
// 		return FALSE;
// 	for(byI=0;byI<64;byI++)	
// 	{		
// 		if((pBuf[byI>>3] & (1<<((byI&0x07)))) == 0)
// 		{
// 			Flash_Page_Read(wSrcBlockNo*64+byI,TmpFileBuf);		
// 			if(!Flash_Page_Write(wObjBlockNo*64+byI,TmpFileBuf))
// 				return FALSE;
// 		}	
// 	}
// 	return TRUE;
// }
#endif

#if(MOS_UCOSII || (MOS_MSVC && DEBUG_UCOSII))
// BOOL Flash_Page_Read(DWORD dwPage, BYTE *pBuf)
// {
// DWORD dwRealPage;
// BOOL bRc;

// 	// dwRealPage	= _GetRealPages(dwPage);
// 	// bRc		= w25qxx_page_read(dwRealPage,pBuf);
// 	return bRc;
// }


// BOOL Flash_Page_Write(DWORD dwPage, BYTE *pBuf)
// {//FLASH写(dwPage 只能输入数据页及CACHE页)
// DWORD dwRealPage;
// BOOL bRc;

// 	// dwRealPage=_GetRealPages(dwPage);
// 	// bRc = w25nxx_page_write(dwRealPage,pBuf);
// 	return bRc;

// }

// BOOL Flash_Block_Erase(DWORD dwPage)
// {
// DWORD dwRealPage;

// 	// dwRealPage=_GetRealPages(dwPage);
// 	// return w25nxx_block_erase(dwRealPage);
// }
#endif

///////////////////////////////////////////////////////////////
//	函 数 名 : GetFileIdlePageAddr
//	函数功能 : 获得空闲页偏移
//	处理过程 : 
//	备    注 : 连续2个页标志空闲才为空闲(避免因为某一位变位，无法跳过),最大查找空间为10M
//	作    者 : jiangjy
//	时    间 : 2016年5月13日
//	返 回 值 : BOOL
//	参数说明 : char *szName,
//				DWORD dwOffset,
//				DWORD dwMaxLen,
//				DWORD *pdwIdleOffset
///////////////////////////////////////////////////////////////
// BOOL GetFileIdlePageAddr(char *szName,DWORD dwOffset,DWORD dwMaxLen,DWORD *pdwIdleOffset)
// {
// DWORD dwMainwOffset,dwBakOffset,dwI,dwNum;
// WORD wEROMOft,wBlockNo,wI,wOffset=0xFFFF,wStartOffset;
// BYTE Buf[136];
	
// 	if(!sys_File2FlashOffset(szName,&dwMainwOffset,&dwBakOffset))
// 		return FALSE;
	
// 	if((DWORD)(dwMainwOffset+dwOffset+dwMaxLen) >= CACHE_BLOCK_OFFSET)
// 		return FALSE;
// 	if(dwMaxLen > 0x200000)
// 		dwMaxLen	= 0x200000;
// 	//获得页擦除标识起始偏移
// 	wEROMOft	= bsp_GetEROMOFT(2);
// 	dwMainwOffset += dwOffset; 
// 	if(dwMainwOffset%2048)
// 		return FALSE;
// 	dwNum		= dwMaxLen/2048;
// 	wBlockNo	= (WORD)(dwMainwOffset/0x20000);
// 	wStartOffset= (WORD)((dwMainwOffset%0x20000)/2048);	
// 	bspEEPROMRead((WORD)(wEROMOft+wBlockNo*8),sizeof(Buf),Buf);
// 	for(dwI=0,wI=wStartOffset;dwI<dwNum;dwI++,wI++)
// 	{		
// 		if(Buf[wI>>3] & (1<<(wI&0x07)))
// 		{
// 			wOffset	= wI-wStartOffset;
// 			if(Buf[(wI+1)>>3] & (1<<((wI+1)&0x07)))
// 			{
// 				*pdwIdleOffset	= wOffset*2048;
// 				return TRUE;
// 			}
// 			else//可能是EEPROM损坏
// 				wOffset	= 0xFFFF;
// 		}
// 	}
// 	*pdwIdleOffset	= wOffset*2048;//最后一页
// 	return (wOffset	!= 0xFFFF);
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : FileClean
//	函数功能 : 清除文件中指定位置的数据(数据变成0xFF)
//	处理过程 : 
//	备    注 : 清除是以块为单位0x20000
//	作    者 : jiangjy
//	时    间 : 2016年3月7日
//	返 回 值 : DWORD
//	参数说明 : TFileClean *pC
///////////////////////////////////////////////////////////////
DWORD FileClean(TFileClean *pC)
{
	TFileWrite FW;
	DWORD dwLen;
    BYTE TmpFileBuf[SECTOR_SIZE];

	memset(TmpFileBuf,0xFF,sizeof(TmpFileBuf));
	FW.iszName = pC->iszName;
	FW.ipvBuf = TmpFileBuf;
	FW.idwOffset = pC->idwOffset;

	for(dwLen = 0; dwLen < pC->idwCleanLen;dwLen += FW.idwBufLen)
	{	
		if( (pC->idwCleanLen - dwLen) < SECTOR_SIZE)
		{
			FW.idwBufLen = pC->idwCleanLen - dwLen;
		}
		else
		{
			FW.idwBufLen =SECTOR_SIZE;
		}
		if( FileWrite(&FW) != 0)
		{
			break;
		}
		FW.idwOffset += FW.idwBufLen;
	}

	if(dwLen < pC->idwCleanLen)
	{
		return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : FileWrite
//	函数功能 : 更新文件数据
//	处理过程 : 
//	备    注 : (以页为单位，读写的偏移、大小需以页为单位)
//	作    者 : jiangjy
//	时    间 : 2016年3月15日
//	返 回 值 : DWORD
//	参数说明 : TFileWrite *pW
///////////////////////////////////////////////////////////////
// DWORD FileWrite(TFileWrite *pW)
// {
//     DWORD dwMainwOffset,dwBakOffset;
	
// 	if( (pW == NULL)
// 	 || (pW->ipvBuf == NULL)
// 	 || (pW->iszName == NULL)
// 	 || ( (pW->idwOffset+pW->idwBufLen) > (FLASH_FREEZE_END_ADDR + 4*SECTOR_SIZE)) 
// 	 || (pW->idwOffset < FLASH_FREEZE_END_ADDR)
// 	 || (pW->idwBufLen > SECTOR_SIZE)
// 	 || (sys_File2FlashOffset(pW->iszName,&dwMainwOffset,&dwBakOffset) == FALSE)
// 	)
// 	{
// 		return 1;
// 	}

// 	api_WriteMemRecordData(pW->idwOffset,pW->idwBufLen,pW->ipvBuf);
	
// 	return 0;
// }

//DWORD FileRead(TFileRead *pR)
//{
//	DWORD dwMainwOffset,dwBakOffset;
//	
//	if( (pR == NULL)
//	 ||	(pR->ipvBuf == NULL)
//	 ||	(pR->iszName == NULL)
//	 || ( (pR->idwOffset+pR->idwBufLen) > (FLASH_FREEZE_END_ADDR + 4*SECTOR_SIZE)) 
//	 || (pR->idwOffset < FLASH_FREEZE_END_ADDR)
//	 || (pR->idwBufLen > SECTOR_SIZE)
//	 || (sys_File2FlashOffset(pR->iszName,&dwMainwOffset,&dwBakOffset) == FALSE)
//	)
//	{
//		return 1;
//	}
//	
//	
//	api_ReadMemRecordData(pR->idwOffset,pR->idwBufLen,pR->ipvBuf);
//
//	return 0;	
//}