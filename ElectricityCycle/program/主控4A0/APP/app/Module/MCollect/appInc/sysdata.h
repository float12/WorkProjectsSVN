#ifndef __SYSDATA_H__
#define __SYSDATA_H__
#ifdef __cplusplus
 #if __cplusplus
	extern "C" {
 #endif
#endif

	
//保存在EEPROM中的系统配置文件
//!!!!!!!!!!!!!!!!不能随意加入配置，否则影响后面电能的偏移，需要时使用结构中保留的Bak字段	
//修改影响升级后EEPROM中所有数据		
#pragma pack(1)
//参数配置双备份，必须校验对才可以
// typedef struct _TERomSysCfg{
// 	// BYTE byUserType;			//用户类型
// //	BYTE GyNo[20][2];			//外部规约号
// 	// BYTE byDays_BC;				//日冻结补采天数
// 	// BYTE byDFrz_MaxCJHour;		//日冻结最多补采小时数	
// 	// BYTE byJCType;				//0x33-三相三线,0x55-无交采,0x34-三相四线
// 	// BYTE bySourceType;			//1-电表冻结、其他-电表没有冻结时采用终端冻结	
// 	// BYTE byCurvSource;			//0x55-电表冻结、其他-采用终端冻结	
// 	// BYTE byMFRZSource;			//0x55-月数据冻结来源于电表冻结，其他是来源于电表冻结1	
// 	// BYTE byHaveESAM;			//有硬件ESAM
// //特殊
// 	// BYTE byRealFromMeter;		//实时数据直接取自电表 0x55
// 	//需要放到04Fx参数中
// 	WORD wInitParaValid;		//0x5AA5 初始化参数区数据有效		
// 	// WORD wDefaultJCMPNo;		//缺省交采测量点号
// 	// BYTE bySysCtrlFlag;         //BIT0 -(1)不判断376.1规约中CON标识位,BIT1-1 (698采表MAC验证有效)
// 	DWORD dwCheck;
// } TERomSysCfg;

#pragma pack()

//求结构偏移
//#define offsetof(structTest,e) (size_t)&(((structTest*)0)->e)		

// typedef struct _TVar{
// 	//内存临时状态	
// 	DWORD	dwStartTime;			//系统启动时间
// 	// DWORD   dwLastOnTime;			//最近一次上电时间
// 	// DWORD   dwLastStopTime;			//上次停电时间
// 	// BYTE	bySavePara;				//是否立即存贮参数
// 	// BYTE	byInitOK;				//是否初始化结束
// 	// BYTE	byStart_reset;			//系统是否为复位启动
// 	TTime	GyTime;
// 	// BYTE	bResetSystem;				//..
// 	//EROM映射(基本配置信息)
// 	// TERomSysCfg Cfg;
// }TVar;

// #define GetUserType()	(gVar.Cfg.byUserType)
// #define GetDataSource()	(gVar.Cfg.bySourceType)
// #define GetJCType()		(gVar.Cfg.byJCType)
// #define GetCurveSource() (gVar.Cfg.byCurvSource)


// extern TVar gVar;
void SaveNvRam_DF6203(void);
void ParaSaveDaemon(BOOL bFlag,TTime *pTime);
BOOL IsDlt698_45(WORD wDrvID);
BYTE PortID2No(DWORD dwPortID);
//采集信息


 #ifdef __cplusplus
  #if __cplusplus
}
  #endif
 #endif
#endif
