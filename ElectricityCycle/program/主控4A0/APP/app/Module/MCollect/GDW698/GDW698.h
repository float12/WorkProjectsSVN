#ifndef GDW698_H__
#define GDW698_H__
#include "appcfg.h"

#if(MD_BASEUSER	== MD_GDW698)
#define MIN_RXFM_SIZE   10		//最小接收帧长
#include "GDW698DataDef.h"
#include "GDW698Funcs.h"
#include "cgy.h"

#define MAX_SAPDU_SIZE		4096//服务器端最大APDU长度	
#define	MAX_HEAD_TAIL_LEN	37	//帧头帧尾的最大长度（含10字节APDU头信息）
#define MAX_OBJ_DATA_LEN	50	//单个APDU及数据的最大长度
#define MAX_DWN_485FRAME_LEN	512	//最大下行帧长度(透抄)	
#define MAX_DWN_ZBFRAME_LEN		256	//最大下行帧长度(透抄)	
	
#define MAX_NUM_NEST	10
#define MAX_GDW698_MEMSIZE	16*1024	//698共用内存	
#define OBJ_START_FLAG	0x68	//起始字符
#define CTRL_DIR	0x80		//传输方向位 0-下行;1-上行
#define CTRL_SC		0x08		//扰码标识位
#define PRM			0x40		//启动标志位 0-来自服务器;1-来自客户端
#define PRM_BIT		14

#define ACD			0x40		//要求访问位
#define ADDR_BITS	0x0F		//地址掩码

#define CTRL_SP		0x020		//分帧标识位
#define CTRLCODE	0x0F		//功能码

//功能码
#define CC_PRM1_NO_ANSWER	0	//0 发送∕无回答 用户数据								
#define CC_PRM1_SEND		1	//1 链路管理
#define CC_PRM1_DATA		3	//3 用户数据

//数据单元
#define OBJ_NO_DATA		0x00	//无数据
#define OBJ_END_FLAG	0x16	//结束字符

//安全传输中的加密方式
// typedef enum _OBJ_SECURITY_TYPE{
// 	Plain_Text = 0, //明文
// 		Plain_Text_RN = 1, //明文+随机数
// 		Clipher_Text = 2, //密文
// 		Clipher_Text_MAC = 3, //密文+SID_MAC
// 		Plain_Text_MAC = 4, //明文+SID_MAC  （安全级别等同于 明文+随机数）
// }OBJ_SECURITY_TYPE;


// typedef struct _TRxAPDU{
// 	HPARA	hMem;				//内存指针
// 	WORD	wMemSize;			//内存大小
// 	WORD	wRPtr;				//当前处理指针
// 	WORD	wOffset;			//读数据的数据个数偏移
// 	WORD	wOldReq;
// 	BYTE	byRxCtrl;			//接收控制字
// 	WORD	wApduWptr;
// 	DWORD	dwRxTime;			//接收时间
// 	BYTE	byPIID;
// 	BYTE	byRxAddrType;		//地址类型
// 	BYTE    byCtrl;				//BIT4-有无时间标签，BIT0-时间标签是否有效，BIT1-是否为分帧传输,BIT2--本帧有效,BIT3-1有后续帧
// 	TimeTag time_tag;
// 	BYTE	byPrivate;			//安全类型
// 	BYTE	abyRound[32];
// 	WORD	tx_seq_no;
// }TRxAPDU;

#pragma pack(1)
typedef struct _TClass_get{
	BYTE class_no;				//类号
	FW_DWHWH func_get;	
	FW_DHWHWHH func_get_rec;	
}TClass_get;

// typedef struct _TClass_set{
// 	BYTE class_no;				//类号
// 	FB_DHH func_set;
// }TClass_set;

// typedef struct _TClass_action{
// 	BYTE class_no;				//类号
// 	FB_DHHH func_action;
// }TClass_action;

typedef struct OBJ_TAIL{
	WORD wCheckCS;				//校验和
	BYTE byEnd;					//结束符
}OBJ_TAIL;

//不带帧尾的帧定义
typedef struct OBJ_EXCEPT_TAIL{
	BYTE  byStart;				//启动字符0X68
	WORD wFmLen;				//帧长
	BYTE  byCtrl;				//功能码C
	BYTE  byAddrF;
	BYTE  _byAddrStart;
}OBJ_EXCEPT_TAIL;

typedef struct _TGDW698TaskHead{
	BYTE task_no;				//任务号
	WORD type_len;				//数据类型长度（BIT15-1数据源于电表冻结0终端冻结,BIT14-(1_698表0-645) ）
	DWORD start_time;			//任务启动时间
	DWORD save_time;			//任务存贮时间
	DWORD success_time;			//任务成功时间
}TGDW698TaskHead;
#pragma pack()

#define	MAX_FRAME_HEAD_LEN		(28) //最大帧头长度

typedef struct _TFrame698_FULL_BASE{
	OBJ_EXCEPT_TAIL *pFrame;
	BYTE *pDataAPDU;
	TSA  *pObjTsa;
	OBJ_TAIL *pTail;
}TFrame698_FULL_BASE;

//数据壳基类
// typedef struct OBJ_DATA_SHELL_BASE{
// 	TFrame698_FULL_BASE *_pFullFrame; //与此shell关联的frame指针
// 	BYTE* _pDataFirst;			//数据头
// 	void* _pDataEnd;			//数据尾
// 	void* _pCursor;				//当前指针
// }OBJ_DATA_SHELL_BASE;

typedef struct _TTask698RunInfo{
	BYTE  byStatus;				//(0-未执行，1-执行中，2-已执行)
	WORD total_num;				//总个数 	
	WORD wCj_No;				//采集个数	
	WORD success_num;			//采集成功数量
	WORD tx_packet_num;			//已发送报文条数
	WORD rx_packet_num;			//已接收报文条数
	BYTE byRetryNum[MAX_DWNPORT_NUM]; //
	DWORD dwTaskTime;			//任务时间	
	DWORD startTask_time[MAX_DWNPORT_NUM];			//数据起始时间
	DWORD success_time;			//任务成功时间
    BYTE success_mps[(MAX_MP_NUM+7)/8];    //采集成功的测量点
}TTask698RunInfo;
// BYTE get_client_addr(void);
// BYTE get_logic_addr(void);
// void MakeServerFrame(BYTE byCtrl,BYTE *pAPDU,WORD wAPDULen);
//基本规约功能
#include "GDW698Base.h"
//#include "APDU/Get.h"
//#include "APDU/Set.h"
//#include "APDU/Action.h"
//#include "APDU/Report.h"
//#include "APDU/Proxy.h"
//#include "APDU/Security.h"
//#include "APDU/Link.h"
//#include "APDU/Connect.h"
//#include "APDU/Release.h"
//#include "GDW698Alm.h"
#include "GDW698_PARA.h"
//#include "DBase/fun/dbFrz_698.h"
#include "Class/Class_698.h"
#endif
#endif