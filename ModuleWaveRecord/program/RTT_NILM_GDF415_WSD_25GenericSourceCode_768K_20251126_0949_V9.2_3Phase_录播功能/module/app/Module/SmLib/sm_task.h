/*
//	文 件 名 :sm_task.c
//	文件功能 :
//	作    者 : 智能电表组
//	创建时间 : 2021年6月25日
//	项目名称 ：QunlityVS
//	操作系统 : 
//	备    注 :
//	历史记录： : 
*/
#ifndef _SM_TASK_H_
#define _SM_TASK_H_

#include "AllHead.h"

#include "light_sm.h"

#define SM4_SIK_DATA_ADDR			(0xCC00 - HALFRMN_MEM_SPACE)
#define SM4_IK_MAGIC				0x5AA5B66B
#define SM4_SIK_MAGIC				0x12348765
#define SM4_ID_MAGIC				0x12348789


#pragma pack(1)

typedef struct TSm4SikSave_t
{
		DWORD dwIDMagic;
		BYTE  DevID[8];
		WORD  wIDCrc;
	
	
		DWORD dwIKMagic;
		BYTE  IKBuf[96];
		WORD  wIKCrc;
	
		DWORD	dwSIKMagic;
		BYTE	SIKBuf[74];
		WORD  wSIKCrc;
	
		BYTE  SoftFlag[50];
                
		DWORD InitFlag;
		WORD  wSoftCrc;
	
}TSm4SavePara;

#pragma pack()

int smTask_main(void);
uint8_t sm4_GetStatus(void);
boolean_t sm4_SetIKey(uint8_t* inBuf);

extern WORD ReadDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf1, BYTE * Buf2 );
extern WORD WriteDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf );

extern BOOL api_VWriteSafeMem(DWORD Addr, WORD Length, BYTE* pBuf);
extern BOOL api_VReadSafeMem(DWORD Addr, WORD Length, BYTE* pBuf);

extern BOOL WriteEEPRom2(DWORD Addr, WORD Length, BYTE* pBuf);
extern BOOL ReadEEPRom2(DWORD Addr, WORD Length, BYTE* pBuf);

// 初始化组网
// 认证阶段
///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_Authent1
//	函数功能 : 下发 16 字节密文数据，执行初始认证
//	处理过程 : 输入电表下发的密文数据，若认证通过，则返回 32 字节密文数据。
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 :  uint8_t* inBuf16,     // 假设 inBuf16 为对方发送数据  16字节
//				  uint8_t* outBuf32   // outBuf32 若认证通过，则返回 32 字节密文数据
///////////////////////////////////////////////////////////////
boolean_t sm4_InitNet_Authent1( uint8_t* inBuf16,  uint8_t* outBuf32);

///////////////////////////////////////////////////////////////
//	函 数 名 : sm4_InitNet_Authent2
//	函数功能 : 输入 16 字节密文数据，若返回成功，则初始认证成功；否则，初始认证失败。
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月24日
//	返 回 值 : boolean_t
//	参数说明 : uint8_t* inBuf16   16 字节密文数据
///////////////////////////////////////////////////////////////
boolean_t sm4_InitNet_Authent2(uint8_t* inBuf16);
uint8_t sm4_Mac(uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen);
int sm4_InitKey(void);
uint8_t sm4_MacVerify(uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t nOutLen);
boolean_t sm4_Encrypt(uint8_t auth, uint8_t* inBuf, uint32_t nInLen, uint8_t* outBuf, uint32_t* nOutLen);
boolean_t Sm4_Reset(void);
BOOL sm4_UnVoidAuthent(void);
boolean_t sm4_SetID(uint8_t* inID);
boolean_t sm4_InitNet_Keydistribut(uint8_t* inBufSik, uint8_t nLen);
BOOL ReadSm4Para(int nType);
uint8_t sm4_GetStatus(void);
boolean_t sm4_WriteSoftFlag(uint8_t* SoftBuffer);
#endif
