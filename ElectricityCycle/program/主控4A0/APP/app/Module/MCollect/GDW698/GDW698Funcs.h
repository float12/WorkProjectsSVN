#ifndef _GDW698FUNCS_H_
#define _GDW698FUNCS_H_

typedef struct _TDATATYPE_LEN{
	OBJ_DATA_TYPE objType;
	WORD wLen;
}TDATATYPE_LEN;

typedef struct _TOad_data_info_tab{
	DWORD oadd;
	BYTE  data_type;			//MTP_AI、MTP_CI...
	BYTE  start_no;				//起始点号
	BYTE  data_num;				//个数
	BYTE  datalen;				//数据长度	
}TOad_data_info_tab;


extern WORD fcs16(BYTE *cp, WORD len);
BYTE GetGDW698UpType(TPort *pPort);
void SetGDW698UpType(TPort *pPort,BYTE byType);


BOOL DTBinToShortTime(DATETIME_S *pType, TTime *pTm, TRANS_DIR td);
WORD objGetDataLen(OBJ_DATA_TYPE objDataType,BYTE *pInBuf,BYTE byMax);
WORD getAResultNomalLen(BYTE byNum,BYTE *pBuf);
WORD getAResultRecordLen(BYTE *pBuf);
BYTE CINo2DataType(BYTE byNo);
BOOL get_oad_data_type_len(DWORD oadd,BYTE *pbyDatalen,BYTE *pbyType,BYTE *pbyStartNo,BYTE *pbyNum);
BOOL get_X_data_of_buf(BYTE *pInbuf,BYTE byoffset,BYTE *poutbuf,WORD *pwoutlen);
BOOL set_X_data_of_buf(BYTE *pInbuf,BYTE byoffset,BYTE *poutbuf);
BOOL get_oad_of_type(BYTE byType,BYTE byStartNo,BOOL bHaveFv,DWORD *poadd);
WORD GetMeterNumOfMS(MS *ms);
WORD GetMeterEXNumOfMS(MS *ms);
BOOL GetNoMeterOfMS(WORD No,MS *ms,TMetCfg_Base *pCfg);
BOOL AddrIsInMS(TSA *sa,MS *ms);
DWORD PortID2OAD(DWORD dwPortID);
BOOL oi2class(WORD oi,BYTE *pbyclass);
WORD get_max_m698rx_apdu_len(TPort *pPort);
WORD get_max_OAD_reponse_len(DWORD dwOAD);
WORD GetTimePointNum(DATETIME_S *pStartTime,DATETIME_S *pEndTime,TI *pti,BYTE byChioce);
BOOL AddTime(DATETIME_S *pStartTime,WORD wtiNum,TI *pti,DATETIME_S *pRealTime);
BOOL get_real_task_data(TSA *saa,TTime *pTime,DWORD dwOAD,BYTE *pData,WORD *pwDataLen,WORD wIntel);
BOOL get_curv_task_data(TSA *saa,TTime *pTime,DWORD dwOAD,BYTE *pData,WORD *pwDataLen);
WORD get_csd_num_of_RCSD(BYTE *pRCSD);
BOOL get_no_csd_of_RCSD(BYTE *pRCSD,WORD wNo,CSD *pcsd);
BOOL get_no_csd_of_CSDS(BYTE *pCSDS,WORD wNo,CSD *pcsd);
BOOL get_no_road_of_ROADS(BYTE *pROADS,WORD wNo,ROAD *proad);
BYTE get_dfrz_save_offset(void);
BOOL IsCurvSch(TOad60150200 *pSch);
WORD TI2Mins(TI *pTI);
WORD GetMeterNumOfTranScheme(TOad60190200 *psch);
BOOL addrIsInTask(TSA *sa,TPort *pPort,BYTE byTaskNo);
BYTE get_dfrz_save_time_type(void);
BYTE get_Mfrz_save_time_type(void);
BOOL is_need_cj_curv(CSD *pcsd);
BOOL set_chg_sch_mins(BYTE sch_no,BYTE task_type,DWORD mins);
void chg_sch_init(void);
BOOL schNo2Index(BYTE bySchNo,BYTE byTaskType,BYTE *pbyIndex);
#endif						