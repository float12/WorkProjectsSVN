#include "../GDW698.h"
#if(MD_BASEUSER	== MD_GDW698)
WORD Classx_1_get(WORD oi,BYTE *pData)
{
WORD wLen=0;

	pData[wLen++]	= dt_octet_str;
	pData[wLen++]	= 2;
	wLen	+= gdw698buf_WORD(&pData[wLen],&oi,TD21);
	return wLen;
}

// BOOL Class_x_Set(DWORD oadd,BYTE *pData,DAR *rc)
// {
// 	if((LHBYTE(oadd) == 0) 
// 		&& ((LHBYTE(oadd) > 3))
// 		)	
// 		*rc	= dar_object_class_inconsistent;
// 	else
// 		*rc	= dar_read_write_denied;
// 	return FALSE;
// }

// BOOL Class_x_action(DWORD oadd,BYTE *pData)
// {
// 	if((LHBYTE(oadd) == 0) 
// 		&& ((LHBYTE(oadd) > 2))
// 		)	
// 		return FALSE;
// 	return TRUE;
// }


#endif