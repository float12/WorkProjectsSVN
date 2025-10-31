#ifndef __SYSCFG_H__
#define __SYSCFG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define	FLAG_SYS			BIT0
#define	FLAG_METER			BIT1
#define	FLAG_MP_EX			BIT2
#define	FLAG_FKSYS			BIT3
#define	FLAG_TASK			BIT4
#define FLAG_DATASET		BIT5
#define FLAG_SCHEME			BIT7
	
typedef struct _TMetCfg_Base{
	BYTE	byUserType;		//用户大类号及用户小类号D7~D4：编码表示本电能表所属的用户大类号，数值范围0~15，依次表示16个用户大类号。
 							//D3~D0：编码表示本电能表所属的用户小类号，数值范围0~15，依次表示16套1类和2类数据项的配置
	BYTE    byProtocolNo;	//通信协议号	
	WORD	wMPNo;			//对698 代表序号

	BYTE	byPortNo;
	BYTE	byBaud;			//波特率300、600、1200、2400、4800、7200、9600、19200
	TSA		meter_addr;
	// TSA		cll_addr;
}TMetCfg_Base;


///////////////////////////////////////////////////////////////
//	函 数 名 : sys_GetMPCfg
//	函数功能 : 获得指定序号测量点配置
//	处理过程 : 
//	备    注 : 
//	作    者 : jiangjy
//	时    间 : 2016年1月19日
//	返 回 值 : BOOL
//	参数说明 : WORD wNo,
//				TMetCfg_Base *pCfg
///////////////////////////////////////////////////////////////
BOOL sys_GetMPCfg(WORD wNo,TMetCfg_Base *pCfg);
BOOL JudgeCanID(WORD wMPSeq);
BOOL sys_InitMPCfg(WORD wMPSeq,TMetCfg_Base *pCfg);
BOOL IsJCMP(TMetCfg_Base *pCfg);
// BOOL sys_ReadTermNo(DWORD *pdwAreaCode,DWORD *pdwAddress);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
