#ifndef _MGYDBASE_H_
#define _MGYDBASE_H_
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif	
void WriteDD(DWORD dwNo,DWORD dwValue);
void WriteDayDD(DWORD dwNo,DWORD dwValue);
void WriteMonDD(DWORD dwNo,DWORD dwValue);
void WriteTime(DWORD dwNo,BYTE *pBuf);
//用户数据接口
void WriteOther(DWORD dwNo,DWORD dwVal);
void WriteYC(DWORD dwNo,long nValue);

void WriteXLVal(DWORD dwNo,DWORD dwValue);
void WriteXLTime(DWORD dwNo,TMPTime *pMPTime);
void WriteDayXLVal(DWORD dwNo,DWORD dwValue);
void WriteDayXLTime(DWORD dwNo,TMPTime *pMPTime);
void WriteMonXLVal(DWORD dwNo,DWORD dwValue);
void WriteMonXLTime(DWORD dwNo,TMPTime *pMPTime);
//抄读曲线接口
BOOL GetCurvStartTime(TTime *pTime);
void WriteCurvYC(TTime *pTime,DWORD dwNo,long nYC);
void WriteCurvDD(TTime *pTime,DWORD dwNo,DWORD dwDD);
// void WritePre(WORD wNo,DWORD dwValue);
// void WriteStatus(WORD wNo,BYTE *pBuf,BYTE byLen);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif



