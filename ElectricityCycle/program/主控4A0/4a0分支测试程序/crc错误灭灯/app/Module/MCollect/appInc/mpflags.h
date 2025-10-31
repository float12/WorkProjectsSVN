#ifndef __MPFLAGS_H__
#define __MPFLAGS_H__

// #define FLAG_MP_ALL			0xFFFFFFFF	//所有

// #define MAX_SYSFRZ_MP_NUM	128			//终端冻结的日月测量点最大个数(必须是8的整数倍)	

typedef struct _TMPFlagsCtrl{//参数	
	BYTE Flag_Valid[(MAX_MP_NUM+7)/8];	//测量点有效标识
	BYTE  PortMPFlag[MAX_DWNPORT_NUM][(MAX_MP_NUM+7)/8];
	WORD  wJCMPNo;						//交流采样测量点号	
}TMPFlagsCtrl;

void InitMPFlags(void);
BYTE *_pGetPortMPFlag(BYTE byNo);
TMPFlagsCtrl *pGetMPFlag(void);
BYTE *pGetPortMPFlag(BYTE byPortNo);
// void ClearMPFlag(WORD wMPNo,DWORD dwMask);
// void SetMPFlag(WORD wMPNo,DWORD dwMask);
// BOOL GetMPFlag(WORD wMPNo,DWORD dwMask);
// void SetCJFlagByNo(BYTE byNo,BYTE byCjType);
// void ClrCJFlagByNo(BYTE byNo,BYTE byCjType);
// void SetCJFlagByPort(TPort *pPort,BYTE byCjType);
// void ClrCJFlagByPort(TPort *pPort,BYTE byCjType);
// BOOL DataCJIsOK(BYTE byNo,BYTE byType);
// BOOL PortCJIsOK(TPort *pPort,BYTE byType);
// void SetDataOKFlag(BYTE byNo,BYTE byType,BYTE byOffset);
void  Fre2RealFlag(void);
#endif

