/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjs
 *日  期：   2021年5月
 *修改人：
 *日  期：
 *描  述：
 *
 *版  本：
 *说  明:    升级程序
 *
 */
/*****************************************************************************/
#ifndef _TASK_UPDATE_H_
#define _TASK_UPDATE_H_

#define RAMCODE_START_ADDR  0x24000000
#define RAMCODE_END_ADDR    0x24040000

#define UPDATEBUFFERLEN     (1024)
#define UPDATERECWAITMS     (1000)

typedef void (*pFunction)(void);


void api_SetCodeBufferLen(BYTE ID, DWORD Len);
DWORD api_GetCodeBufferLen(BYTE ID);
void api_SetCodeBufferSum(BYTE ID, WORD Sum);
WORD api_GetCodeBufferSum(BYTE ID);
void api_SetUpdateFlag(BOOL state);
BOOL api_GetUpdateFlag(void);
void api_SetCodeBufferFlag(BYTE type, BYTE ID);
BYTE api_GetCodeBufferFlag(BYTE type);
BOOL api_WriteDataToCodeBuffer(BYTE ID, DWORD Offset, WORD Len, BYTE *pBuf);
WORD api_CalcCodeBufferSum(BYTE ID);
BOOL api_CheckCodeBufferSum(BYTE ID);
BOOL api_CheckRAMCodeSum(BYTE ID);
BOOL api_CopyCodeBufferToRAM(BYTE ID);
BYTE api_GetCodeNextBufferFlag(BYTE type);
void api_ResetCpu(void);
BOOL api_ReadDataFromCodeBuffer(BYTE ID, DWORD Offset, WORD Len, BYTE *pBuf);
void InitUpFileStatus(void);
BOOL CompareUpFile(TFileInfo* tFile);
BOOL WriteUpFileStatus(void);
BOOL IsVipUserUpdate(BYTE* pBuf, WORD wLen);
#endif

