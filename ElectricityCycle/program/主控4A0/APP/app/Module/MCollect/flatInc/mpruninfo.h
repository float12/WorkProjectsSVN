#ifndef __MPRUNINFO_H__
#define __MPRUNINFO_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef enum MPDT_TYPE{
	DT_NUL = 0, DT_F1, DT_F2, DT_F3, DT_F4, DT_F5,DT_F6,DT_F7,DT_F8, DT_F9, DT_F10,
	DT_F11, DT_F12, DT_F13, DT_F14, DT_F15,DT_F16,DT_F17,DT_F18, DT_F19,DT_F20,
	DT_F21, DT_F22, DT_F23, DT_F24, DT_F25,DT_F26,DT_F27,DT_F28, DT_F29,DT_F30,
	DT_F31,DT_F32,DT_F33,DT_F34,DT_F35,DT_MAX, DT_END //不包含MAX，DT_END为列表使用
}MPDT_TYPE;

#pragma pack(1)
typedef struct _UData{
	TMPTime	Time;
	DWORD	dwVal;
}UData;

//测量点自动抄表日
typedef struct _MPDT_F1{
	WORD	AutoDay[3];			//16进制
}MPDT_F1;

//断相
typedef struct _MPDT_F2{
	DWORD	Num[4];				//总及A、B、C断相次数
	DWORD	Time[4];			//断相累计时间
	TMPTime	STime[4];			//最近一次断相起始时间
	TMPTime	ETime[4];			//最近一次断相结束时间
	DWORD	DD[4];				//断相发生时正向有功电度
	long	AI[4][6];			//断相发生时电压、电流
	DWORD	EDD[4];				//断相结束时正向有功电度
}MPDT_F2;

//失压
typedef MPDT_F2		MPDT_F3;

//编程时间、次数
typedef struct _MPDT_F4{
	DWORD	dwNum;				//次数
	TMPTime Time;				
}MPDT_F4;

//需量清0
typedef MPDT_F4		MPDT_F5;
//时段表编程	
typedef MPDT_F4		MPDT_F6;

//电池工作时间
typedef struct _MPDT_F7{
	DWORD	dwMins;				
}MPDT_F7;

//最近一次磁场干扰记录 
typedef struct _MPDT_F8{
	DWORD	dwNum;				//次数
	TMPTime STime;		
	TMPTime ETime;		
}MPDT_F8;

//最近一次开盖次数
typedef MPDT_F8		MPDT_F9;

//清零记录 
typedef MPDT_F4		MPDT_F10;	
//事件清零记录 
typedef MPDT_F4		MPDT_F11;	

//校时记录 
typedef MPDT_F8		MPDT_F12;	

//断流次数
typedef struct _MPDT_F13{
	DWORD	Num[3];			
}MPDT_F13;

//脉冲常数
typedef struct _MPDT_F14{
	DWORD	Const[2];			
}MPDT_F14;

//时段参数
typedef struct _MPDT_F15{
	DWORD	Seg[8];			
}MPDT_F15;

//PT、CT参数
typedef struct _MPDT_F16{
	WORD	PCT[2];
}MPDT_F16;

//开表盖记录
typedef struct _MPDT_F17{
	DWORD	dwNum;				//次数
	TMPTime STime;		
	TMPTime ETime;	
	DWORD	DD[12];				//6个发生前，6个发生后
}MPDT_F17;

//开尾盖记录
typedef MPDT_F17	MPDT_F18;

//最近10次掉电记录
typedef struct _MPDT_F19{
	DWORD	dwNum;				//次数
	TMPTime STime[10];		
	TMPTime ETime[10];		
}MPDT_F19;

//最近一次跳闸记录
typedef MPDT_F4	MPDT_F20;	

//最近一次合闸记录
typedef MPDT_F4	MPDT_F21;	
//过流次数
typedef MPDT_F13 MPDT_F22;	
//失流次数
typedef MPDT_F13 MPDT_F23;	
//电源异常
typedef struct _MPDT_F24{
	DWORD	dwNum;			
}MPDT_F24;
//电压逆相序
typedef MPDT_F24 MPDT_F25;	
//电压不平衡
typedef MPDT_F24 MPDT_F26;	
//欠压次数
typedef MPDT_F13 MPDT_F27;	
//过压次数
typedef MPDT_F13 MPDT_F28;
//负荷开关动作次数
typedef MPDT_F24 MPDT_F29;	
//电能表状态字及变更标识
typedef struct _MPDT_F30{
	WORD	Status[7];			
	WORD	BWStatus[7];			
}MPDT_F30; 	
//电能表事件采集状态字
typedef struct _MPDT_F31{
	BYTE	byFramLen;					//帧长
	BYTE	UpStatus[12];				//主动上报状态字	
	BYTE	EventIncNum[62];			//事件增加的个数(顺序见主动上报状态字和事件对应表,序号0代表是否需主动上报状态字)
}MPDT_F31; 	

typedef struct _MPDT_F32{				//事件增量状态字
	BYTE	EventIncNum[62];			//事件增加的个数(顺序见主动上报状态字和事件对应表,序号0代表是否需主动上报状态字)
}MPDT_F32; 	

typedef struct _MPDT_F33{				
	DWORD	EventNum[62];				//事件的个数(顺序见主动上报状态字和事件对应表,序号0代表是否需主动上报状态字)
	BYTE	ValidFlag[8];
}MPDT_F33; 

typedef struct _MPDT_F34{				
	BYTE	byFmLen;					//040015数据帧长度
	BYTE	FmData[60];
}PACK MPDT_F34; 

typedef struct _MPDT_F35{
	DWORD	EventNum[62];				//事件的个数(顺序见主动上报状态字和事件对应表,序号0代表是否需主动上报状态字)
	BYTE	ValidFlag[8];
}PACK MPDT_F35; 

#pragma pack()

typedef struct _MPDT_LEN{
	MPDT_TYPE Fn;
	WORD wLen;
}MPDT_LEN;

// BOOL IsEventInfo(DWORD dwDI);
// BOOL GetMPFnSize(MPDT_TYPE ft,DWORD *pdwSize);
// BOOL GetMPFnIndex(MPDT_TYPE ft,DWORD *pdwOffset);
// void WriteMeterRunInfo(MPDT_TYPE ft,WORD wNo,UData Val);
// void InitMPAllRunInfo(WORD wMPNo,HPARA hInfo);
// BOOL GetMPAllRunInfo(WORD wMPNo,HPARA hInfo);
// BOOL WriteMPAllRunInfo(WORD wMPNo,HPARA hInfo,DWORD dwFreshFlag);
// BOOL GetMPRunInfo(WORD wMPNo,MPDT_TYPE ft,HPARA hInfo);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
