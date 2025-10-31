#ifndef __MP_H__
#define __MP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
// #define DAYS_DFRZ_SAVE		62			
// #define BLOCK_OF_DFRZ		(512*1024)	
// #define DAYS_CURV_SAVE		16				
// #define BLOCK_OF_CURV		(128*1024)

// BOOL IsMeter(WORD wMPNo);
WORD GetJCMPNo(void);
// WORD mp_GetFrzSize(BYTE byType);

// BOOL IsVipMP(WORD wMPNo);
// BYTE GetVipMPNum(void);
//获得指定端口的测量点个数
WORD GetMPNum(BYTE byPortNo);		
WORD GetAllMeterSum(void);	
BOOL No2MP(WORD wNo,WORD *pwMPNo);
BOOL MP2No(WORD wMPNo,WORD *pwSeq);
BOOL MP_IsValid(WORD wMPNo);

// BOOL SetMPCJFlag(WORD wMPNo,BYTE byCJType,BYTE *pOutFlag);
// void mp_Delete(WORD wMPNo);
int comp_TSA_addr(TSA *s1,TSA *s2);
BOOL Addr2MP(BYTE byPortNo,HPARA hAddr,BOOL bDlt645,WORD *pwMPNo);
BOOL GetMPAddr(WORD wMPSeqNo,BYTE *pAddr);
//698用
// BOOL TSA2MPSeqNo(TSA *sa,BYTE byType,WORD *pwMPSeqNo);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
