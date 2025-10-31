#ifndef GDW698_PARA_H
#define GDW698_PARA_H

typedef struct _TOAD_LEN{
	DWORD	obj;
	WORD	wLen;
}TOAD_LEN;
// TsysCfgOther *pGetSysCfgOther(void);
// void FreshSysCfgOther(void);
HPARA pGetFrz698(DWORD oad_index);
// BOOL sys_Para698_Read(DWORD oad_index,HPARA hPARA);
// BOOL sys_Para698_Write(DWORD oad_index,HPARA hPARA);
// void sys_ClrStatus(DWORD dwNo,BYTE byFlag);
BOOL add_mp_cfg(WORD wMPSeqNo,TOad60010200 *pPara);
void sys_set_task_cfg(BYTE byTaskID,TOad60130200 *ptkcfg);
void sys_del_task_cfg(BYTE byTaskID,BOOL ball);
BOOL GetMP698Para(WORD wMPSeqNo,TOad60010200 *pPara);
void del_mp_cfg(WORD mp_seqno);
// BYTE get_scheme_num(BYTE sch_type);
void set_task_status(BYTE byTaskID,BYTE task_status);
// TOad60130200 *pGet698NoTaskCfg(BYTE byNo);
WORD get_taskcfg_num(BYTE byMask);
// void sys_SetStatus(DWORD dwNo,DWORD dwArg0);
// void add_safemode(TEsamSafeMode *psafemode);
// void del_safemode(OI oii);
void _freshPara(WORD flag);
BOOL api_GetMeterNo(const BYTE* mailAddr,BYTE* meterNo);
// TTask698ParaCfg *pGet698TaskPara(void);
DWORD getParaFileOffset(WORD wOffsetPage,BYTE byType);
// BOOL sys_cj_scheme_read(BYTE sch_type,BYTE scheme_no,BYTE *schinfo);
BOOL GetschInfo(BYTE bytaskType,BYTE byShemeNo,BYTE *psch698);
// BOOL add_cjscheme(BYTE sch_type,BYTE scheme_no,BYTE *schinfo);
void del_cjscheme(BYTE sch_type,BYTE sch_no);
// DWORD getLcdPass(void);
// BYTE setLcdPass(DWORD dwPass);
// void sys_AddFrzCfg(DWORD oad_index,TFrz698Cfg *pFrzcfg);
// void sys_DelFrzCfg(DWORD oad_index,DWORD frzOAD);
// void sys_ClrFrzCfg(DWORD oad_index);
DWORD get_dmfrztask_save_time(DWORD dwTaskTime,TOad60150200 *psch);
BYTE get_frztype_rcsd(BYTE *pRcsd);
BOOL IsUseSafeMode(WORD wMPSeqNo);
BOOL SafeModeIsInit(WORD wMPSeqNo);
void set_mp_safemode(WORD wMPSeqNo,BOOL bUse);
void write_mp_meterno(WORD wMPSeqNo,BYTE *meterno);
// BOOL IsMACVerifyFlag(void);
#endif