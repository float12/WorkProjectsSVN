//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部 
//创建人	低压台区软件-zhaohang
//创建日期	2023.02.07
//描述		头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef _DATADEAL_API_H
#define _DATADEAL_API_H
//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern DWORD  g_dwSoftWatDog; 		//软件计数，防止算法死机
extern WORD Last_frame_num;
extern float V_data[MAX_PHASE][G_SAMPLE_NUM];  //电压报文解析数据
extern float I_data[MAX_PHASE][G_SAMPLE_NUM];  //电流报文解析数据
extern float EU_factor[MAX_PHASE],EI_factor[MAX_PHASE];
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  存储冻结
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_SaveFreezeData( void );
//--------------------------------------------------
//功能描述:  /*应用程序主任务*/
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
int api_NILM_Task(void);

//--------------------------------------------------
//功能描述:  测试用，填充数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_TestComplementData( void );

//功能描述:  波形回放前，先初始化
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_SaveLicense(BYTE * pBuf );

BYTE  api_ReadLicense(BYTE * pBuf );

void  api_WatchTask(void);

void get_voltage(void *rawdate,float *voltagedatebuff);
void get_current(void *rawdate,float *currentdatebuff);
void GetActivePower(SDWORD *Act);
void SaveVIP( void );
void api_ClearSampBuf( void );
BOOL api_GetSmpPoints(void** new_data_ptr);
#endif //#ifndef _DATADEAL_API_H