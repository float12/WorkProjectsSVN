
#ifndef _GDW698_BASE_H_
#define _GDW698_BASE_H_
#if(MD_BASEUSER	== MD_GDW698)

typedef enum WRITE_UP_RESULT{ WR_ERR, WR_OK, WR_NOT_NEED_ANSWER } WRITE_UP_RESULT;
typedef enum FRAME_RESULT{ 
	FR_ERR, 
	FR_NOT_START_FLAG, 
	FR_LEN_NOT_EQUAL, 
	FR_NOT_END_FLAG,
	FR_LESS, 
	FR_OK,
	FR_FIX_OK
} FRAME_RESULT;

FRAME_RESULT objSmockRead( TFrame698_FULL_BASE *pAfnBase, BYTE *Buf );
// WORD GetClientAPDUDatalen(BYTE *pDataAPDU,WORD wAPDUSize);
// BOOL TpIsOk(TimeTag *pTimeTag);
WORD objGetFrameLength( TFrame698_FULL_BASE *pAfnBase );
WORD objGetAPDUSize(TFrame698_FULL_BASE *pAfnBase);
WORD objGetFrameSize( TFrame698_FULL_BASE *pAfnBase );
DWORD gdw698_SearchOneFrame(BYTE byCtrl_Dir,WORD wMaxFmSize,HPARA hBuf,DWORD dwLen);
DWORD gdw698_SearchOneFrame2(BYTE byCtrl_Dir,WORD wMaxFmSize,HPARA hBuf,DWORD dwLen);
WORD MakeFrameHead(TSA *sa,BYTE *pBuf,BYTE byClientAddr);
WORD Add698FrameTail(BYTE byCtrl,WORD wFrameLen,BYTE *pBuf);
void MakeFrameTail(BYTE byCtrl,WORD wFrameLen);
// void MakeFrame(BYTE byCtrl,TSA *sa,BYTE *pAPDU,WORD wAPDULen);
// WORD GetTxMaxApduSize(void);
void DATETIME_Time(DATETIME *pdatatime,TTime *pTime,TRANS_DIR td);
void DATETIME_S_Time(DATETIME_S *pdatatime,TTime *pTime,TRANS_DIR td);
WORD gdw698buf_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td,BOOL bAddHead);
WORD gdw698bufLink_TSA(BYTE *pBuf,TSA *psa,TRANS_DIR td);
WORD gdw698bufLink_TSA_Oct_Str(BYTE *pBuf,TSA *psa,TRANS_DIR td);
WORD gdw698buf_WORD(BYTE *pBuf,WORD *pword,TRANS_DIR td);
WORD gdw698buf_DWORD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td);
WORD gdw698buf_float32(BYTE *pBuf,float *pfloat,TRANS_DIR td);
WORD gdw698buf_OAD(BYTE *pBuf,DWORD *pdword,TRANS_DIR td);
WORD gdw698buf_OMD(BYTE *pBuf,OMD *pomd,TRANS_DIR td);
WORD gdw698buf_DATE(BYTE *pBuf,Date *pdate,TRANS_DIR td);
WORD gdw698buf_TIME(BYTE *pBuf,Time *ptime,TRANS_DIR td) ;
BOOL TimeHMSIsValid(Time *pTime);
WORD gdw698buf_DATETIME(BYTE *pBuf,DATETIME *pdatatime,TRANS_DIR td);
WORD gdw698buf_DATETIME_S(BYTE *pBuf,DATETIME_S *pdatatime,TRANS_DIR td);
WORD gdw698buf_TI(BYTE *pBuf,TI *pti,TRANS_DIR td);
WORD gdw698buf_OI(BYTE *pBuf,OI *poi,TRANS_DIR td);
WORD gdw698buf_TimeTag(BYTE *pBuf,TimeTag *ptimetag,TRANS_DIR td);
WORD gdw698buf_DAR(BYTE *pBuf,DAR *pdar,TRANS_DIR td);
WORD gdw698buf_ROAD(BYTE *pBuf,ROAD *proad,TRANS_DIR td);
WORD gdw698buf_CSD(BYTE *pBuf,CSD *pcsd,TRANS_DIR td);
WORD gdw698buf_Bit8Str(BYTE *pBuf,BYTE *pbyFlag,TRANS_DIR td);
WORD gdw698buf_ComDCB(BYTE *pBuf,ComDCB *pcomdcb,TRANS_DIR td);
WORD gdw698Buf_MS(BYTE *pBuf,MS *pms,TRANS_DIR td);
WORD gdw698Buf_Num(BYTE *pBuf,WORD *pw,TRANS_DIR td);
WORD gdw698Buf_boolean(BYTE *pbuf,BOOL *pbool,TRANS_DIR td);
WORD gdw698Buf_double_long(BYTE *pbuf,long *pvalue,TRANS_DIR td);
WORD gdw698Buf_double_long_unsigned(BYTE *pbuf,DWORD *pvalue,TRANS_DIR td);
WORD gdw698Buf_Integer(BYTE *pbuf,char *pvalue,TRANS_DIR td);
WORD gdw698Buf_long(BYTE *pbuf,short *pvalue,TRANS_DIR td);
WORD gdw698Buf_unsigned(BYTE *pbuf,BYTE *pvalue,TRANS_DIR td);
WORD gdw698Buf_long_unsigned(BYTE *pbuf,WORD *pvalue,TRANS_DIR td);
WORD gdw698Buf_long64(BYTE *pbuf,DLONG *pvalue,TRANS_DIR td);
WORD gdw698buf_float64(BYTE *pBuf,double *pdouble,TRANS_DIR td);
WORD gdw698Buf_long64_unsigned(BYTE *pbuf,UDLONG *pvalue,TRANS_DIR td);
WORD gdw698buf_RSD(BYTE *pBuf,RSD *prsd,TRANS_DIR td);
// int set_data_obj(Variant *val, BYTE *pBuf );
BOOL GetMP698Addr(TSA *sa);
// void str_IP(char *pstr,BYTE bystrLen,DWORD *pdwIP,TRANS_DIR td);
// DWORD vstr2IP(char *pstr);
// BYTE UpPort2No(TPort *pPort);
#endif
#endif

