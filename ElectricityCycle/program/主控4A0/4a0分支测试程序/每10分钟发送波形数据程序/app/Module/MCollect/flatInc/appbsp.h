
#ifndef __APPBSP_H__
#define __APPBSP_H__
#ifdef __cplusplus
	#if __cplusplus
	extern "C" {
	#endif
#endif

// BOOL bspEEPROMRead(WORD addr,WORD readlen,BYTE *pdata);
// BOOL bspEEPROMWrite(WORD addr,BYTE *pdata,WORD writelen);

DWORD GetTicks(void);

#ifdef __cplusplus
	#if __cplusplus
	}
	#endif
#endif	
#endif
