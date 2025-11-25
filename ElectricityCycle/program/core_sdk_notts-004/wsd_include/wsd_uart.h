//----------------------------------------------------------------------
// 版权所有 (C) 2003-20XX ?????????????????????????????з???
// 功能描述
// 参数说明
// 返回值说明
// 修改记录
//----------------------------------------------------------------------
#ifndef __UART_H
#define __UART_H

//-----------------------------------------------
//				定义常量
//-----------------------------------------------
#define MAX_COM_CHANNEL_NUM					2										//三路串口 缓存用到两个	
#define SINGLE_LOOP_ITEM 					27 										// 一个周期内的数据帧数目
// 波形数据的相关常量		
#define WAVE_UART_QUEUE_SPACE				50
#define WAVE_CYCLE_CNT 						10										// 一个波形周期内的数据帧数目
#define WAVE_DATA_FRAME_SIZE 				815										// 一个数据帧的大小
#define SEND_WAVE_DATA_LEN 					(WAVE_DATA_FRAME_SIZE*2)
// 实时数据的相关常量，目前格式写死，未来可根据需求修改
#define MAX_PHASE_NUM 						3										// 最大相位数3个
#define DATA_ITEM_NUM_PER_PHASE 			26										// 每个相位的数据项数
#define INSTANT_DATA_LEN_PER_PHASE 			(DATA_ITEM_NUM_PER_PHASE * 4)			// 一个相位的实时数据大小，每个数据项4个字节
#define MAX_DATA_LEN_PER_FRAME 				194										// 645个字节一帧，最大有效数据大小
#define INSTANT_DATA_698_FRAME_LEN 			432										// 波形数据上传698帧的大小
//-----------------------------------------------
//				定义结构体，枚举类型
//-----------------------------------------------
typedef struct
{
	BYTE buf[SEND_WAVE_DATA_LEN];
	BYTE type;	  // 0:数据帧，1：请求上传帧
	BYTE LoopNum; // 波形系数属于哪个回路
} WaveDatatoTcp; // 波形数据或请求上传数据



typedef enum
{
	eUpgradeExeSuc = 0,			 // 执行升级成功
	eUpgrdeFrameTimeoutErr,		 // 接收超时
	eUpgradeCheckResultErr,		 // 校验结果错误
	eUpgradeVeriErr,			 // 验证错误
	eUpgradeDARErr,				 // DAR错误
	eUpgradeWholeProTimeoutErr,	 // 全过程超时
	eFtpLoginSuccess,			 // 登录ftp成功
	eFtpDownLoadSuccess,		 // 下载文件成功
	eFtpModuleUpgradeFail,		 // 模块升级失败
	eFtpModuleUpgradeVerifyFail, // 模块升级验证失败
	eFtpFailExit,				 // 模块异常退出ftp传输
} eUpgradeResult;

//-----------------------------------------------
//				定义全局变量
//-----------------------------------------------
extern int UART_HD_LTO;
extern int UART_HD_RECVWAVE;
extern int UART_HD_BASEMETER;
extern int IntervalTime; // 采样间隔时间
extern TRealTimer tTimer;
extern BYTE PublishInstantData[MAX_PHASE_NUM * INSTANT_DATA_LEN_PER_PHASE + 4]; // 4个字节
extern TSerial Serial[MAX_COM_CHANNEL_NUM];
//-----------------------------------------------
// 				定义函数
//-----------------------------------------------
void Uart_Task(void *parameter);
void InitPoint(TSerial *p);
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial *p);
DWORD SendDataToModule(BYTE *buf, DWORD len);
void api_DoReceMoveData(TSerial *p, BYTE ProtocolType);
#endif // #ifndef __UART_H
