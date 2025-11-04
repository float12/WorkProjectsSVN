//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	
//创建日期	
//描述		MODBUS头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __TF_H
#define __TF_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
#define INFO_FILE_PATH 					"/sdcard0/Info.bin"		// 保存读写文件名称的文件的路径
#define MAX_FILE_NAME_LEN 				30
#define MAX_CHARGE_SAVE_NUM 			6900 					// 保存的最大充电次数为140
//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
typedef struct // 信息文件结构体头信息，防止每次读全部信息
{
	BYTE WriteDataFileName[MAX_FILE_NAME_LEN]; 	// 上一个波形数据文件的文件名
	WORD ReadOffset;							// 读偏移量
	WORD WriteOffset;							// 写偏移量
	WORD DeleteOffset;							//删除偏移位置
	WORD ChargeCnt;								//充电行为计数
} TInfoFileHead;

typedef struct
{
	BYTE FirstFileName[MAX_FILE_NAME_LEN];
	WORD CNT;			//目前方案为1
	BYTE Loop;			//识别当前充电来自哪个回路
} FileInfoOfOneCharge; // 1次充电行为的起始文件名和文件数

typedef struct // 信息文件结构体
{
	BYTE WriteDataFileName[MAX_FILE_NAME_LEN];					// 上一个波形数据文件的文件名
	WORD ReadOffset;											// 读偏移量
	WORD WriteOffset;											// 写偏移量
	WORD DeleteOffset;											//删除偏移位置
	WORD ChargeCnt;   
	FileInfoOfOneCharge FileInfoOfCharges[MAX_CHARGE_SAVE_NUM];	// 暂存待上传的充电行为，读文件名为全0表示无某次充电波形的数据文件，
																// 否则发生了某次充电行为，读到结束帧后将该文件名置为全0表示已上传完成
} TInfoFile;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE WaveQueueFullFlag;
extern BYTE RecLenExceed4096Flag; // 接收波形数据长度超过4096字节标志
extern BYTE RecLenNot815Flag; // 接收波形数据长度不是815字节
extern TInfoFile infoFile;
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
#endif //#ifndef __TF_H
//--------------------------------------------------
// 功能描述: 挂载TF卡
//
// 参数: 无
//
// 返回值: TRUE - 成功，FALSE - 失败
//
// 备注内容: 无
//--------------------------------------------------
BYTE api_CheckaAndMountSDCard(void);
//--------------------------------------------------
//功能描述:  删除TF卡内所有波形数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_DeleteAllWaveFile( void );