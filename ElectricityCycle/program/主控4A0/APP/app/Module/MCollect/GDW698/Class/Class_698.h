#ifndef __CLASS_698_H_
#define __CLASS_698_H_

#define MAX_PARA_BUF_LEN	200
// BOOL grp_action(WORD oi,WORD attrib_no,BYTE *pData);
WORD Classx_1_get(WORD oi,BYTE *pData);
// WORD Class1_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class2_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class3_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class4_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class5_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class6_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class7_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class7_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class7_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class7_Get_rec(DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealNum);

// WORD Class8_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class8_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class8_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class9_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class9_Get_rec(DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealNum);
// BOOL Class9_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class9_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class10_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class10_Get_rec(BYTE Ch,DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealNum);
// BOOL Class10_Set(DWORD oadd,BYTE *pData,DAR *rc);
BOOL Class10_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
WORD Class11_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
WORD Class11_Get_rec(BYTE Ch,DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealNum);
BOOL Class11_Set(DWORD oadd,BYTE *pData,DAR *rc);
BOOL Class11_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class12_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class12_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class12_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class13_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class13_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class13_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// BOOL Class14_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class14_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class18_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class18_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class18_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class19_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class19_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class19_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class20_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class20_Set(DWORD oadd,BYTE *pData,DAR *rc);
// WORD Class21_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class21_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class21_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class22_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class22_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class22_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class23_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class23_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class23_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
// WORD Class24_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// WORD Class24_Get_rec(DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealNum);
// BOOL Class24_Set(DWORD oadd,BYTE *pData,DAR *rc);
// BOOL Class24_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);

// WORD Class25_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class25_Set(DWORD oadd,BYTE *pData,DAR *rc);

// WORD Class26_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext);
// BOOL Class26_Set(DWORD oadd,BYTE *pData,DAR *rc);
#endif