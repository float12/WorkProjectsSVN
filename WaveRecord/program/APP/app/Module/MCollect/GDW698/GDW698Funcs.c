///////////////////////////////////////////////////////////////
//	文 件 名 :GDW698Funcs.c
///////////////////////////////////////////////////////////////
#include "GDW698.h"

extern void Init_cj_task(WORD wNo,DWORD dwTaskTime,DWORD dwMins,BYTE byExeStatus);
// extern BYTE GetReal2FrzFlg(void);
extern void fresh_sch_time(BYTE sch_no,BYTE task_type);
//extern TTask698RunInfo *g_pTaskRunInfo;

#if(MD_BASEUSER	== MD_GDW698)

typedef struct _TGDW_Mems{
	DWORD	dwValid;
	DWORD   dwAllocTime[MAX_GDW698_MEMSIZE/1024];
	BYTE	Mems[MAX_GDW698_MEMSIZE];
}TGDW_Mems;



// static const WORD fcstab[256] = {
// 	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
// 	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
// 	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
// 	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
// 	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
// 	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
// 	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
// 	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
// 	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
// 	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
// 	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
// 	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
// 	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
// 	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
// 	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
// 	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
// 	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
// 	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
// 	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
// 	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
// 	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
// 	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
// 	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
// 	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
// 	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
// 	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
// 	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
// 	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
// 	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
// 	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
// 	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
// 	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
// };
typedef struct _TChg_sch_info{
	DWORD	dwImagicB;
	DWORD   chg_sch_mins[MAX_SCH_GENERAL];
	DWORD	dwImagicE;
}TChg_sch_info;

static TChg_sch_info _SchChgInfo;
TChg_sch_info *p_chg_sch_info;



// WORD fcs16(BYTE *cp, WORD len)
// {//68K模式输出
// WORD fcs=0xffff;

// 	while (len--)
// 		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

// 	fcs ^= 0xffff; // complement  //此为FCS值
// 	return fcs;
// }




void chg_sch_init(void)
{
	p_chg_sch_info = &_SchChgInfo;
	if (p_chg_sch_info == NULL)
		return;
	if((p_chg_sch_info->dwImagicB != IMAGIC_START)
		||(p_chg_sch_info->dwImagicE != IMAGIC_END))
	{//停电
		memset(p_chg_sch_info,0,sizeof(TChg_sch_info));
		p_chg_sch_info->dwImagicB =	IMAGIC_START;
		p_chg_sch_info->dwImagicE =	IMAGIC_END;
	}
}
///////////////////////////////////////////////////////////////
//	函 数 名 : get_chg_sch_mins
//	函数功能 : 获取更改方案的时间
///////////////////////////////////////////////////////////////
DWORD get_chg_sch_mins(BYTE sch_no,BYTE task_type)
{
BYTE index = 0;

	if (p_chg_sch_info == NULL)
		return FALSE;
	if(task_type != TT_DATA_CJ)
		return FALSE;

	if(!schNo2Index(sch_no,task_type,&index))
		return FALSE;
	if (index >= MAX_SCH_GENERAL)
		return FALSE;
	return p_chg_sch_info->chg_sch_mins[index];
}

///////////////////////////////////////////////////////////////
//	函 数 名 : set_chg_sch_mins
//	函数功能 : 设置更改方案的分钟数
///////////////////////////////////////////////////////////////
BOOL set_chg_sch_mins(BYTE sch_no,BYTE task_type,DWORD mins)
{
BYTE index = 0;
			
	if (p_chg_sch_info == NULL)
		return FALSE;
	if(task_type != TT_DATA_CJ)
		return FALSE;
	if(!schNo2Index(sch_no,task_type,&index))
		return FALSE;
	if (index >= MAX_SCH_GENERAL)
		return FALSE;
	p_chg_sch_info->chg_sch_mins[index] = mins;
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : is_task_delay
//	函数功能 : 任务延时函数
///////////////////////////////////////////////////////////////
BOOL is_task_delay(BYTE sch_no,BYTE task_type)
{
TTime time_now;
DWORD chg_mins,now_mins;
	
	chg_mins = get_chg_sch_mins(sch_no,task_type);
	if(0 == chg_mins)
		return FALSE;
	if(!GetTime(&time_now))
		return FALSE;		
	now_mins = Time2Min(&time_now);
	if (now_mins > chg_mins)
	{//已经延时1分钟,可以执行方案了。
		set_chg_sch_mins(sch_no,task_type,0);
		return FALSE;
	}
	if( now_mins < chg_mins)
	{//此处是防止对时 （对时后 再延时一分钟）
		set_chg_sch_mins(sch_no,task_type,now_mins);
	}
	//至此认为需要延时 当前时间小于等于记录时间时需要延时
	return TRUE;	
}

// BYTE GetGDW698UpType(TPort *pPort)
// {
// TData_6203 *pDF6203=pGetDF6203();

// 	if(HHBYTE(pPort->dwID) == MPT_ETHERNET)
// 		return pDF6203->UpType[0];
// 	else if(HHBYTE(pPort->dwID) == MPT_376_3)
// 		return pDF6203->UpType[1];
// 	return pDF6203->UpType[2];
// }

// void SetGDW698UpType(TPort *pPort,BYTE byType)
// {
// TData_6203 *pDF6203=pGetDF6203();

// 	if(HHBYTE(pPort->dwID) == MPT_ETHERNET)
// 		pDF6203->UpType[0]	= byType;
// 	else if(HHBYTE(pPort->dwID) == MPT_376_3)
// 		pDF6203->UpType[1]	= byType;
// 	else
// 		pDF6203->UpType[2]	= byType;
// }


static const TDATATYPE_LEN data_type_Len[]={
	{dt_null,				0},
	{dt_array,				INVALID_LEN},
	{dt_structure,			INVALID_LEN},
	{dt_boolean,			1},
	{dt_bit_string,			INVALID_LEN},
	{dt_double_long,		4},
	{dt_double_long_unsigned,4},
	{dt_octet_str,			INVALID_LEN},
	{dt_visible_string,		INVALID_LEN},
	{dt_UTF8_string,		INVALID_LEN},
	{dt_bcd,			 	1},
	{dt_Integer,			1},
	{dt_long,				2},
	{dt_unsigned,			1},
	{dt_long_unsigned,	 	2},
	{dt_long64,		     	8},
	{dt_long64_unsigned, 	8},
	{dt_enum,		     	1},
	{dt_float32,		 	4},
	{dt_float64,		 	8},
	{dt_date_time,			10},
	{dt_date,			 	5},
	{dt_time,			 	3},
	{dt_datetime_s,		 	7},
	{dt_OI,				 	2},
	{dt_OAD,			 	4},
	{dt_OMD,		 	 	4},
	{dt_TI,				 	3},
	{dt_TSA,			 	INVALID_LEN},
	{dt_MAC,			 	INVALID_LEN},
	{dt_Rand,				INVALID_LEN},
	{dt_Region,				INVALID_LEN},
	{dt_Scaler_Unit,		2},
	{dt_RSD,				INVALID_LEN},
	{dt_ROAD,				INVALID_LEN},
	{dt_CSD,				INVALID_LEN},
	{dt_MS,					INVALID_LEN},
	{dt_RCSD,				INVALID_LEN},
	{dt_ESAM_SID,			INVALID_LEN},
	{dt_SID_MAC,			INVALID_LEN},
	{dt_COMDCB,				5},
	{dt_PIID,				1}, //PIID数据类型
	{dt_PIID_ACD,			1},
	{dt_DAR,				1},
	{dt_FN,					1}
};

WORD getOBJ_MSLen(BYTE *pBuf,BYTE byMax)
{//获得电表集合的长度
WORD wLen=1,wDataNum=0,i;

	if((pBuf[0] >= 2) && (pBuf[0]<8))
		wLen	+= gdw698Buf_Num(&pBuf[wLen],&wDataNum,TD12);
	switch(pBuf[0])
	{
	default:	break;
	case 2: //一组用户类型		
		wLen	+= wDataNum;
		break;
	case 3: //一组用户地址
		for(i=0;i<wDataNum;i++)
			wLen += objGetDataLen(dt_TSA,&pBuf[wLen],byMax);					
		break;
	case 4: //一组配置序号--指定的若干电能表配置序号的那些电能表
		wLen	+= wDataNum*sizeof(WORD);
		break;
	case 5: //一组用户类型区间
	case 6: //一组用户地址区间
	case 7: //一组用户序号区间
		for(i=0;i<wDataNum;i++)
			wLen	+= objGetDataLen(dt_Region,&pBuf[wLen],byMax);
		break;
	}
	return wLen;
}

WORD getOBJ_RSDLen(BYTE *pData,BYTE byMax)
{//获得记录选择描述符RSD的数据长度
BYTE byChioce=pData[0];
WORD wLen=1,wDataNum,j;
	
 	switch(byChioce)
 	{
  	case 1: //对象属性描述符  OAD +  数值            Data
		wLen	+= objGetDataLen(dt_OAD,&pData[wLen],byMax);
		wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;
 		break;
 	case 2: //对象属性描述符 OAD，起始值 Data，结束值 Data，数据间隔 Data
		wLen	+= objGetDataLen(dt_OAD,&pData[wLen],byMax);
		wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;
		wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;
		wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;		
 	    break;
 	case 3:		
		wLen	+= gdw698Buf_Num(&pData[wLen],&wDataNum,TD12);
		for(j=0;j<wDataNum;j++)
		{
			wLen	+= objGetDataLen(dt_OAD,&pData[wLen],byMax);
			wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;
			wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;
			wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],byMax)+1;		
		}
 	    break;
 	case 4: 	
	case 5:
		wLen	+= objGetDataLen(dt_datetime_s,&pData[wLen],byMax);
		wLen	+= getOBJ_MSLen(&pData[wLen],byMax);
 		break;
 	case 6:
	case 7:
	case 8:
		wLen	+= objGetDataLen(dt_datetime_s,&pData[wLen],byMax);
		wLen	+= objGetDataLen(dt_datetime_s,&pData[wLen],byMax);
		wLen	+= objGetDataLen(dt_TI,&pData[wLen],byMax);
		wLen	+= getOBJ_MSLen(&pData[wLen],byMax);
		break;
 	case 9:
		wLen	+= objGetDataLen(dt_unsigned,&pData[wLen],byMax);
 	    break;
 	case 10:
		wLen	+= objGetDataLen(dt_unsigned,&pData[wLen],byMax);
		wLen	+= getOBJ_MSLen(&pData[wLen],byMax);
 	    break;
 	}
	return wLen;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : objGetDataLen
//	函数功能 : 获得指定缓冲区数据的长度
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年6月20日
//	返 回 值 : WORD
//	参数说明 : OBJ_DATA_TYPE objDataType,
//				BYTE *pInBuf,
//				BYTE byMax(最大嵌套级别)
///////////////////////////////////////////////////////////////
WORD objGetDataLen(OBJ_DATA_TYPE objDataType,BYTE *pInBuf,BYTE byMax)
{
BYTE i;
WORD wLen_Type=0,wDataNum,j;

	//保护，防止报文异常，嵌套出错
	if(byMax == 0)
		return 0;
	byMax--;
	for (i=0; i<ArraySize(data_type_Len); ++i)
	{
		if(objDataType == data_type_Len[i].objType)
		{
			if (data_type_Len[i].wLen == INVALID_LEN )
			{	
				if( NULL == pInBuf ) 
					return 0;
				switch(objDataType)
				{
				case dt_array		: //数量 数据类型 ...										
				case dt_structure	: //ok
					wLen_Type	+= gdw698Buf_Num(pInBuf,&wDataNum,TD12);					
					for(j=0;j<wDataNum;j++)
						wLen_Type	+= objGetDataLen((OBJ_DATA_TYPE)pInBuf[wLen_Type],&pInBuf[wLen_Type+1],byMax)+1;																
					break;
				case dt_bit_string	: //位串			
					wLen_Type	+= gdw698Buf_Num(pInBuf,&wDataNum,TD12);					
					wLen_Type	+= (wDataNum+7)/8;			
					break;
				case dt_octet_str	: 
				case dt_visible_string:
				case dt_UTF8_string	:
				case dt_MAC	: 	
				case dt_Rand:
					wLen_Type	+= gdw698Buf_Num(pInBuf,&wDataNum,TD12);					
					wLen_Type	+= wDataNum;
					break;
				case dt_TSA			: 
					wLen_Type	+= (pInBuf[wLen_Type]+1);
					break;
				case dt_Region		:															
					wLen_Type	+= objGetDataLen((OBJ_DATA_TYPE)pInBuf[wLen_Type+1],&pInBuf[wLen_Type+2],byMax)+2;
					wLen_Type	+= objGetDataLen((OBJ_DATA_TYPE)pInBuf[wLen_Type],&pInBuf[wLen_Type+1],byMax)+1;					
					break;
				case dt_ESAM_SID	:  //标识       double-long-unsigned，+  附加数据   octet-string
					wLen_Type	+= objGetDataLen(dt_double_long_unsigned,&pInBuf[wLen_Type],byMax);
					wLen_Type	+= objGetDataLen(dt_octet_str,&pInBuf[wLen_Type],byMax);
					break;
				case dt_SID_MAC		: 
					wLen_Type	+= objGetDataLen(dt_ESAM_SID,&pInBuf[wLen_Type],byMax);
					wLen_Type	+= objGetDataLen(dt_MAC,&pInBuf[wLen_Type],byMax);
					break;
				case dt_RSD		:
					wLen_Type	+= getOBJ_RSDLen(&pInBuf[wLen_Type],byMax);
					break;
				case dt_ROAD	:
					wLen_Type	+= objGetDataLen(dt_OAD,&pInBuf[wLen_Type],byMax);
					wLen_Type	+= gdw698Buf_Num(&pInBuf[wLen_Type],&wDataNum,TD12);										
					wLen_Type	+= wDataNum*objGetDataLen(dt_OAD,&pInBuf[wLen_Type],byMax);					
					break;
				case dt_CSD		: //列选择描述符CSD
					if(pInBuf[wLen_Type] == 0)
						wLen_Type	+= objGetDataLen(dt_OAD,&pInBuf[wLen_Type+1],byMax);					
					else if(pInBuf[wLen_Type] == 1)
						wLen_Type	+= objGetDataLen(dt_ROAD,&pInBuf[wLen_Type+1],byMax);						
					wLen_Type++;
					break;
				case dt_MS		: //电表集合
					wLen_Type	+= getOBJ_MSLen(pInBuf,byMax);
					break;
				case dt_RCSD	:
					wLen_Type	+= gdw698Buf_Num(&pInBuf[wLen_Type],&wDataNum,TD12);															
					for(j=0;j<wDataNum;j++)
						wLen_Type	+= objGetDataLen(dt_CSD,&pInBuf[wLen_Type],byMax);											
					break;
				}				
			}
			else//定长数据类型
				wLen_Type =  data_type_Len[i].wLen;
			break;
		}
	}
	return wLen_Type;
}

WORD getAResultNomalLen(BYTE byNum,BYTE *pBuf)
{
	WORD wLen=0;
	BYTE *pData = pBuf;
	BYTE i;
	
	for (i = 0; i < byNum; i++)
	{
		wLen += 4; 
		if(pData[wLen] == 0)
		{
			wLen += 2;
		}
		else
		{
			wLen += 1;
			wLen += objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],MAX_NUM_NEST)+1;
		}
	}
	
	return wLen;
}

WORD getAResultRecordLen(BYTE *pBuf)
{
	WORD wLen=0;
	WORD i,j,wCSDNum,wTMMPP2;
	BYTE *pData = pBuf;
	
	wLen += 4; //跳过OAD
	
	wCSDNum	= get_csd_num_of_RCSD(&pData[wLen]);
	wLen += objGetDataLen(dt_RCSD,&pData[wLen],MAX_NUM_NEST);
	if(pData[wLen++] == 1)
	{
		wLen	+= gdw698Buf_Num(&pData[wLen],&wTMMPP2,TD12);
		for(j=0;j<wTMMPP2;j++)
		{
			for(i=0;i<wCSDNum;i++)
				wLen	+= objGetDataLen((OBJ_DATA_TYPE)pData[wLen],&pData[wLen+1],MAX_NUM_NEST)+1;	
		}
	}
	else
		wLen++;
	
	return wLen;
}

BOOL DTBinToShortTime(DATETIME_S *pType, TTime *pTm, TRANS_DIR td)
{
	CHECKPTR( pType, FALSE );
	CHECKPTR( pTm, FALSE );
	switch( td )
	{
	case TD12:
		pTm->Sec	= (pType->second);
		pTm->Min	= (pType->minute);
		pTm->Hour	= (pType->hour);
		pTm->Day	= (pType->day);
		pTm->Mon	= (pType->month);
		pTm->wYear	= pType->year;
		pTm->wMSec	= 0;
		break;
	case TD21:
		pType->second = ( pTm->Sec );
		pType->minute = ( pTm->Min );
		pType->hour = ( pTm->Hour );
		pType->day  = ( pTm->Day );
		pType->month= ( pTm->Mon );
		pType->year = pTm->wYear;
		break;
	}
	return TRUE;
}

static const TOad_data_info_tab Oad_data_info_tab[]={
//1、正有
	{0x00100200,	MTP_CI	,	0,	5,20},
	{0x00100201,	MTP_CI	,	0,	1,4 },
	{0x00100202,	MTP_CI	,	1,	1,4 },
	{0x00100203,	MTP_CI	,	2,	1,4 },
	{0x00100204,	MTP_CI	,	3,	1,4 },
	{0x00100205,	MTP_CI	,	4,	1,4 },
	{0x00200200,	MTP_CI	,	5,	5,20},
	{0x00200201,	MTP_CI	,	5,	1,4 },
	{0x00200202,	MTP_CI	,	6,	1,4 },
	{0x00200203,	MTP_CI	,	7,	1,4 },
	{0x00200204,	MTP_CI	,	8,	1,4 },
	{0x00200205,	MTP_CI	,	9,	1,4 },
	{0x00300200,	MTP_CI	,	10,	5,20},
	{0x00300201,	MTP_CI	,	10,	1,4 },
	{0x00300202,	MTP_CI	,	11,	1,4 },
	{0x00300203,	MTP_CI	,	12,	1,4 },
	{0x00300204,	MTP_CI	,	13,	1,4 },
	{0x00300205,	MTP_CI	,	14,	1,4 },
	{0x00400200,	MTP_CI	,	15,	5,20},
	{0x00400201,	MTP_CI	,	15,	1,4 },
	{0x00400202,	MTP_CI	,	16,	1,4 },
	{0x00400203,	MTP_CI	,	17,	1,4 },
	{0x00400204,	MTP_CI	,	18,	1,4 },
	{0x00400205,	MTP_CI	,	19,	1,4 },
	{0x00500200,	MTP_CI	,	20,	5,20},
	{0x00500201,	MTP_CI	,	20,	1,4 },
	{0x00500202,	MTP_CI	,	21,	1,4 },
	{0x00500203,	MTP_CI	,	22,	1,4 },
	{0x00500204,	MTP_CI	,	23,	1,4 },
	{0x00500205,	MTP_CI	,	24,	1,4 },
	{0x00600200,	MTP_CI	,	25,	5,20},
	{0x00600201,	MTP_CI	,	25,	1,4 },
	{0x00600202,	MTP_CI	,	26,	1,4 },
	{0x00600203,	MTP_CI	,	27,	1,4 },
	{0x00600204,	MTP_CI	,	28,	1,4 },
	{0x00600205,	MTP_CI	,	29,	1,4 },
	{0x00700200,	MTP_CI	,	30,	5,20},
	{0x00700201,	MTP_CI	,	30,	1,4 },
	{0x00700202,	MTP_CI	,	31,	1,4 },
	{0x00700203,	MTP_CI	,	32,	1,4 },
	{0x00700204,	MTP_CI	,	33,	1,4 },
	{0x00700205,	MTP_CI	,	34,	1,4 },
	{0x00800200,	MTP_CI	,	35,	5,20},
	{0x00800201,	MTP_CI	,	35,	1,4 },
	{0x00800202,	MTP_CI	,	36,	1,4 },
	{0x00800203,	MTP_CI	,	37,	1,4 },
	{0x00800204,	MTP_CI	,	38,	1,4 },
	{0x00800205,	MTP_CI	,	39,	1,4 },
//分相电能
	{0x00110200,	MTP_CI	,	40,	1,4},//..A相正有
	{0x00110201,	MTP_CI	,	40,	1,4}, //..A相正有

	{0x00210200,	MTP_CI	,	41,	1,4},//..A相反向
	{0x00210201,	MTP_CI	,	41,	1,4},//..A相反向

	{0x00310200,	MTP_CI	,	42,	1,4},//..A相无功
	{0x00310201,	MTP_CI	,	42,	1,4},//..A相无功

	{0x00410200,	MTP_CI	,	43,	1,4},//..A相无功
	{0x00410201,	MTP_CI	,	43,	1,4},//..A相无功

// 	{0x00120200,	MTP_CI	,	44,	5,20},//..B相正有
// 	{0x00220200,	MTP_CI	,	45,	5,20},
// 	{0x00320200,	MTP_CI	,	46,	5,20},
//	{0x00420200,	MTP_CI	,	47,	5,20},

	{0x00120200,	MTP_CI	,	44,	1,4},//..B相正有
	{0x00120201,	MTP_CI	,	44,	1,4}, //..b相正有

	{0x00220200,	MTP_CI	,	45,	1,4},//..b相反向
	{0x00220201,	MTP_CI	,	45,	1,4},//..b相反向

	{0x00320200,	MTP_CI	,	46,	1,4},//..b相无功
	{0x00320201,	MTP_CI	,	46,	1,4},//..b相无功

	{0x00420200,	MTP_CI	,	47,	1,4},//..b相无功
	{0x00420201,	MTP_CI	,	47,	1,4},//..b相无功
	
	{0x00130200,	MTP_CI	,	48,	1,4},//..C相正有
	{0x00130201,	MTP_CI	,	48,	1,4}, //..C相正有

	{0x00230200,	MTP_CI	,	49,	1,4},//..C相反向
	{0x00230201,	MTP_CI	,	49,	1,4},//..C相反向

	{0x00330200,	MTP_CI	,	50,	1,4},//..C相无功
	{0x00330201,	MTP_CI	,	50,	1,4},//..C相无功

	{0x00430200,	MTP_CI	,	51,	1,4},//..C相无功
	{0x00430201,	MTP_CI	,	51,	1,4},//..C相无功
	
	{0x00000200,	MTP_CI	,	52,	5,20},
	{0x00000201,	MTP_CI	,	52,	1,4 },
	{0x00000202,	MTP_CI	,	53,	1,4 },
	{0x00000203,	MTP_CI	,	54,	1,4 },
	{0x00000204,	MTP_CI	,	55,	1,4 },
	{0x00000205,	MTP_CI	,	56,	1,4 },
//2、遥测
	{0x20000200,	MTP_AI	,	0,	3,6},
	{0x20000201,	MTP_AI	,	0,	1,2},
	{0x20000202,	MTP_AI	,	1,	1,2},
	{0x20000203,	MTP_AI	,	2,	1,2},
	{0x20010200,	MTP_AI	,	3,	3,12},
	{0x20010201,	MTP_AI	,	3,	1,4},
	{0x20010202,	MTP_AI	,	4,	1,4},
	{0x20010203,	MTP_AI	,	5,	1,4},

	{0x20040200,	MTP_AI	,	6,	4,16},
	{0x20040201,	MTP_AI	,	6,	1,4},
	{0x20040202,	MTP_AI	,	7,	1,4},
	{0x20040203,	MTP_AI	,	8,	1,4},
	{0x20040204,	MTP_AI	,	9,	1,4},
	
	{0x20050200,	MTP_AI	,	10,	4,16},
	{0x20050201,	MTP_AI	,	10,	1,4},
	{0x20050202,	MTP_AI	,	11,	1,4},
	{0x20050203,	MTP_AI	,	12,	1,4},
	{0x20050204,	MTP_AI	,	13,	1,4},

	{0x200A0200,	MTP_AI	,	14,	4,8},
	{0x200A0201,	MTP_AI	,	14,	1,2},
	{0x200A0202,	MTP_AI	,	15,	1,2},
	{0x200A0203,	MTP_AI	,	16,	1,2},
	{0x200A0204,	MTP_AI	,	17,	1,2},

	{0x20010400,	MTP_AI	,	18,	1,4},
//电压角
	{0x20020200,	MTP_AI	,	19,	3,6},
	{0x20020201,	MTP_AI	,	19,	1,2},
	{0x20020202,	MTP_AI	,	20,	1,2},
	{0x20020203,	MTP_AI	,	21,	1,2},
	{0x20030200,	MTP_AI	,	22,	3,6},
	{0x20030201,	MTP_AI	,	22,	1,2},
	{0x20030202,	MTP_AI	,	23,	1,2},
	{0x20030203,	MTP_AI	,	24,	1,2},

	{0x20060200,	MTP_AI	,	25,	4,16},
	{0x20060201,	MTP_AI	,	25,	1,4},
	{0x20060202,	MTP_AI	,	26,	1,4},
	{0x20060203,	MTP_AI	,	27,	1,4},
	{0x20060204,	MTP_AI	,	28,	1,4},
//3、需量
	{0x10100200,	MTP_XL	,	0,	5,50},
	{0x10100201,	MTP_XL	,	0,	1,10},
	{0x10100202,	MTP_XL	,	1,	1,10},
	{0x10100203,	MTP_XL	,	2,	1,10},
	{0x10100204,	MTP_XL	,	3,	1,10},
	{0x10100205,	MTP_XL	,	4,	1,10},
	{0x10200200,	MTP_XL	,	5,	5,50},
	{0x10200201,	MTP_XL	,	5,	1,10},
	{0x10200202,	MTP_XL	,	6,	1,10},
	{0x10200203,	MTP_XL	,	7,	1,10},
	{0x10200204,	MTP_XL	,	8,	1,10},
	{0x10200205,	MTP_XL	,	9,	1,10},

	{0x10300200,	MTP_XL	,	10,	5,50},
	{0x10300201,	MTP_XL	,	10,	1,10},
	{0x10300202,	MTP_XL	,	11,	1,10},
	{0x10300203,	MTP_XL	,	12,	1,10},
	{0x10300204,	MTP_XL	,	13,	1,10},
	{0x10300205,	MTP_XL	,	14,	1,10},
	{0x10400200,	MTP_XL	,	15,	5,50},
	{0x10400201,	MTP_XL	,	15,	1,10},
	{0x10400202,	MTP_XL	,	16,	1,10},
	{0x10400203,	MTP_XL	,	17,	1,10},
	{0x10400204,	MTP_XL	,	18,	1,10},
	{0x10400205,	MTP_XL	,	19,	1,10},

	{0x10500200,	MTP_XL	,	20,	5,50},
	{0x10500201,	MTP_XL	,	20,	1,10},
	{0x10500202,	MTP_XL	,	21,	1,10},
	{0x10500203,	MTP_XL	,	22,	1,10},
	{0x10500204,	MTP_XL	,	23,	1,10},
	{0x10500205,	MTP_XL	,	24,	1,10},
	{0x10600200,	MTP_XL	,	25,	5,50},
	{0x10600201,	MTP_XL	,	25,	1,10},
	{0x10600202,	MTP_XL	,	26,	1,10},
	{0x10600203,	MTP_XL	,	27,	1,10},
	{0x10600204,	MTP_XL	,	28,	1,10},
	{0x10600205,	MTP_XL	,	29,	1,10},

	{0x10700200,	MTP_XL	,	30,	5,50},
	{0x10700201,	MTP_XL	,	30,	1,10},
	{0x10700202,	MTP_XL	,	31,	1,10},
	{0x10700203,	MTP_XL	,	32,	1,10},
	{0x10700204,	MTP_XL	,	33,	1,10},
	{0x10700205,	MTP_XL	,	34,	1,10},
	{0x10800200,	MTP_XL	,	35,	5,50},
	{0x10800201,	MTP_XL	,	35,	1,10},
	{0x10800202,	MTP_XL	,	36,	1,10},
	{0x10800203,	MTP_XL	,	37,	1,10},
	{0x10800204,	MTP_XL	,	38,	1,10},
	{0x10800205,	MTP_XL	,	39,	1,10},
//4、电表时间
	{0x40000200,	MTP_STA	,	0,	1,4},
	{0x202C0200,	MTP_PAYINFO	,0,	2,8},
	{0x202C0201,	MTP_PAYINFO	,	0,	1,4},
	{0x202C0202,	MTP_PAYINFO	,	1,	1,4},
	{0x202E0200,	MTP_PAYINFO	,	2,	1,4},
	{0x202D0200,	MTP_PAYINFO	,	3,	1,4},
	{0x20140200,	MTP_PAYINFO	,	4,	7,14},
//需量 有功、无功
	{0x20170200,	MTP_CI	,	57,	1,4},
	{0x20180200,	MTP_CI	,	58,	1,4},

	{0x25000200,	MTP_MULTI,	0,	1,4},
	{0x25010200,	MTP_MULTI,	0,	1,4},
	{0x25020200,	MTP_MULTI,	0,	1,4},
	{0x25030200,	MTP_MULTI,	1,	1,4},
	{0x25040200,	MTP_MULTI,	2,	1,4},
	{0x25050200,	MTP_MULTI,	3,	1,8},
};

BYTE CINo2DataType(BYTE byNo)
{//根据CI的点号获得数据的类型
BYTE byType=dt_double_long_unsigned;

	if(((byNo>=10)&&(byNo<20))
		||((byNo==42)||(byNo==43))
		||((byNo==46)||(byNo==47))
		||((byNo==50)||(byNo==51))
		||((byNo>=52)&&(byNo<57)))
	{
		byType=dt_double_long;
	}
	return byType;
}

BOOL get_oad_data_type_len(DWORD oadd,BYTE *pbyDatalen,BYTE *pbyType,BYTE *pbyStartNo,BYTE *pbyNum)
{
WORD i;

	for(i=0;i<ArraySize(Oad_data_info_tab);i++)
	{
		if(Oad_data_info_tab[i].oadd == oadd)
		{
			if(NULL != pbyDatalen)
				*pbyDatalen	= Oad_data_info_tab[i].datalen;
			if(NULL != pbyType)
				*pbyType	= Oad_data_info_tab[i].data_type;
			if(NULL != pbyStartNo)
				*pbyStartNo	= Oad_data_info_tab[i].start_no;
			if(NULL != pbyNum)
				*pbyNum		= Oad_data_info_tab[i].data_num;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL get_oad_of_type(BYTE byType,BYTE byStartNo,BOOL bHaveFv,DWORD *poadd)
{
WORD i;

	for(i=0;i<ArraySize(Oad_data_info_tab);i++)
	{
		if((Oad_data_info_tab[i].data_type == byType)
			&&(byStartNo == Oad_data_info_tab[i].start_no))
		{
			if(bHaveFv)
			{
				if(Oad_data_info_tab[i].data_num > 1)
				{
					*poadd	= Oad_data_info_tab[i].oadd;
					return TRUE;
				}
			}
			else if(Oad_data_info_tab[i].data_num == 1)
			{
				*poadd	= Oad_data_info_tab[i].oadd;
				return TRUE;
			}			
		}
	}
	return FALSE;
}

WORD TI2Mins(TI *pTI)
{
WORD wInterval=0;
	
	if( pTI == NULL ) 
		return 0;
	switch(pTI->unit)
	{
	case 0: //秒
		wInterval	= pTI->value/60;
		break;
	case 1: //分
		wInterval	= pTI->value;
		break;
	case 2: //时
		wInterval	= pTI->value*60;
		break;
	case 3: //日
		wInterval	= pTI->value*1440;
		break;
	case 4: //月
		wInterval	= 0xFFFF;
		break;
	}
	return wInterval;
}

// BOOL IsTaskWorkTime(BYTE byTaskNo, DWORD dwCheckTime)
// {
// TOad60130200 *pCfg=pGet698NoTaskCfg(byTaskNo);
// WORD wI;
// DWORD dwTime=dwCheckTime%1440;

//     if(NULL == pCfg)
// 		return FALSE;
// 	for(wI=0;(wI<pCfg->bySegNum)&&(wI<MAX_TASK_TIMEREG_NUM);wI++)
// 	{
// 		if((dwTime >= (DWORD)(pCfg->segTime[wI].byBeginHour*60+pCfg->segTime[wI].byBeginMin))
// 			&&(dwTime < (DWORD)(pCfg->segTime[wI].byEndHour*60+pCfg->segTime[wI].byEndMin)))
// 			return TRUE;
// 	}	
// 	return FALSE;
// }

// BOOL task_start(TPort *pPort,BYTE task_no,DWORD dwCheckTime,DWORD *pdwStartTime,BYTE byType)
// {
// TData_6203 *pDF6203=pGetDF6203();
// TOad60130200 *pCfg=pGet698NoTaskCfg(task_no);
// WORD wInterval,delay;
// DWORD dwStartTime;
// BYTE no=PortID2No(pPort->dwID);	

// //	if(	gVar.byInitOK == 0)//..初始化未结束不启动任务！
// //		return FALSE;
	
// 	if(NULL == pCfg)
// 		return FALSE;
// 	//判断任务有效性
// 	if(pCfg->byStuts != 1)
// 		return FALSE;
// 	wInterval	= TI2Mins(&pCfg->intelTI);	
// 	if(wInterval == 0)
// 		wInterval = 60;
// 	delay	= TI2Mins(&pCfg->dealyTI);	
// 	if(delay == 0xFFFF)
// 		return FALSE;	
// 	//判断时段是否该任务启动
// 	if(!((dwCheckTime >= (pCfg->dwBeginTime/60+delay))
// 		&&(dwCheckTime < pCfg->dwEndTime/60)))
// 		return FALSE;
// 	if(wInterval == 0xFFFF)
// 	{//月间隔
// 		TTime tTime,tTime2;
// 		Min2Time(dwCheckTime,&tTime);
// 		Min2Time(pDF6203->OldTaskTime[no][task_no],&tTime2);
// 		if((tTime.Mon+tTime.wYear*12) == (tTime2.Mon+tTime2.wYear*12))
// 		{
// 			return FALSE;
// 		}
// 		else
// 		{
// 			if(pDF6203->OldTaskTime[no][task_no] != 0)
// 			{//初始默认执行一次任务 	
// 				if(((tTime.Day-1)*1440+tTime.Hour*60+tTime.Min) < delay)
// 					return FALSE;
// 			}	
// 		}
// 		if(!IsTaskWorkTime(task_no,dwCheckTime))
// 		{
// 			return FALSE;
// 		}
// 		tTime.Day	= 1;
// 		tTime.Hour	= 0;
// 		tTime.Min	= 0;
// 		tTime.Sec	= 0;
// 		dwStartTime	= Time2Min(&tTime);	
// 		*pdwStartTime	= dwStartTime;	
// 		return TRUE;
// 	}
// 	else
// 	{	
// 		dwStartTime	= ((pCfg->dwBeginTime/60)/wInterval)*wInterval+((dwCheckTime-pCfg->dwBeginTime/60-delay)/wInterval)*wInterval;
// 	}
// 	if(pDF6203->OldTaskTime[no][task_no] == dwStartTime)
// 	{
// 		return FALSE;		
// 	}
// 	if(!IsTaskWorkTime(task_no,dwCheckTime))
// 	{
// 		return FALSE;
// 	}

// 	*pdwStartTime	= dwStartTime;	
// 	return TRUE;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : cjtask_start
//	函数功能 : 采集任务启动的时间
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年12月8日
//	返 回 值 : BOOL
//	参数说明 : TPort *pPort,
//				BYTE task_no,
//				DWORD dwCheckTime,
//				DWORD *pdwStartTime
///////////////////////////////////////////////////////////////
// BOOL cjtask_start(TPort *pPort,BYTE task_no,DWORD dwCheckTime,DWORD *pdwStartTime)
// {
// TOad60130200 *pCfg=pGet698NoTaskCfg(task_no);
// TData_6203 *pDF6203=pGetDF6203();
// BYTE no=PortID2No(pPort->dwID);
// TTime tTime;

//     if(NULL == pCfg)
// 		return FALSE;
// 	if( TI2Mins(&pCfg->intelTI)>1 && (dwCheckTime%1440==1439) )
// 		return FALSE;
// 	if((pCfg->byTaskType != TT_DATA_CJ) 
// 		&& (pCfg->byTaskType != TT_EVENT_CJ)
// 		&& (pCfg->byTaskType != TT_TRANS_CJ))
// 		return FALSE;
// 	/* 每个小时的59分55秒到下一小时的0分1秒之间不执行任务 */		
// 	GetTime(&tTime); 		
// 	if ( (tTime.Min==59)&&(tTime.Sec >= 55) )		
// 	{			
// 		return FALSE;		
// 	}
// 	if(task_start(pPort,task_no,dwCheckTime,pdwStartTime,0))
// 	{
// 		pDF6203->OldTaskTime[no][task_no] = *pdwStartTime;
// 		return TRUE;
// 	}
// 	else	return FALSE;
// }

// BYTE get_dfrz_save_offset(void)
// {
// WORD wI,j;
// TOad60130200 *pCfg;
// BYTE byRc=0;
// TOad60150200 sch;
// CSD csdd;

// 	for(wI=0;wI<MAX_698_TASK_NUM;wI++)
// 	{
// 		pCfg=pGet698NoTaskCfg((BYTE)wI);
// 		if((pCfg->validflag == 0x5aa5)
// 			&&(pCfg->byTaskType == TT_DATA_CJ)
// 			&&(pCfg->byStuts == 1))
// 		{
// 			if(GetschInfo(TT_DATA_CJ,pCfg->byShemeNo,(BYTE*)&sch))
// 			{
// 				for(j=0;j<sch.byColNum;j++)
// 				{
// 					if(get_no_csd_of_CSDS(sch.csdBuf,j,&csdd))
// 					{
// 						if(csdd.choice)
// 						{
// 							if(HIWORD(csdd.road.oadMain.value) == 0x5004)
// 							{
// 								if(sch.bySaveFlag	== 4)//存为上上日0点0分
// 									return 1;
// 							}
// 						}							
// 					}
// 				}				
// 			}
// 		}
// 	}	
// 	return byRc;
// }

// BYTE get_dfrz_save_time_type(void)
// {
// 	WORD wI,j;
// 	TOad60130200 *pCfg;
// 	BYTE byRc=0,num=0,first_flag=0xFF;
// 	TOad60150200 sch;
// 	CSD csdd;
// 	for(wI=0;wI<MAX_698_TASK_NUM;wI++)
// 	{
// 		pCfg=pGet698NoTaskCfg((BYTE)wI);
// 		if((pCfg->validflag == 0x5aa5)
// 			&&(pCfg->byTaskType == TT_DATA_CJ)
// 			&&(pCfg->byStuts == 1))
// 		{
// 			if(GetschInfo(TT_DATA_CJ,pCfg->byShemeNo,(BYTE*)&sch))
// 			{
// 				for(j=0;j<sch.byColNum;j++)
// 				{
// 					if(get_no_csd_of_CSDS(sch.csdBuf,j,&csdd))
// 					{
// 						if(csdd.choice)
// 						{
// 							if((HIWORD(csdd.road.oadMain.value) == 0x5004)&&(pCfg->intelTI.unit==3)&&(pCfg->intelTI.value==1))
// 							{
// 								if(first_flag == 0xFF)
// 								{//第一个日冻结任务不是3,可以直接返回
// 									if(sch.bySaveFlag != 3)
// 										return sch.bySaveFlag;
// 									first_flag = sch.bySaveFlag;
// 								}
// 								num++;
// 							}
// 						}							
// 					}
// 				}				
// 			}
// 		}
// 	}	
// 	if((num == 7)&&(first_flag == 3))
// 	{//为了过台体分组抄表，台体会设7个日冻结方案
// 		return 6;
// 	}
// 	else
// 		return byRc;
// }
// BYTE get_Mfrz_save_time_type(void)
// {
// 	WORD wI,j;
// 	TOad60130200 *pCfg;
// 	BYTE byRc=0;
// 	TOad60150200 sch;
// 	CSD csdd;
// 	for(wI=0;wI<MAX_698_TASK_NUM;wI++)
// 	{
// 		pCfg=pGet698NoTaskCfg((BYTE)wI);
// 		if((pCfg->validflag == 0x5aa5)
// 			&&(pCfg->byTaskType == TT_DATA_CJ)
// 			&&(pCfg->byStuts == 1)
// 			&&(pCfg->intelTI.unit==4)
// 			&&(pCfg->intelTI.value==1)
// 			)
// 		{
// 			if(GetschInfo(TT_DATA_CJ,pCfg->byShemeNo,(BYTE*)&sch))
// 			{
// 				for(j=0;j<sch.byColNum;j++)
// 				{
// 					if(get_no_csd_of_CSDS(sch.csdBuf,j,&csdd))
// 					{
// 						if(csdd.choice)
// 						{
// 							if((HIWORD(csdd.road.oadMain.value) == 0x5006))
// 							{
// 								return sch.bySaveFlag;
// 							}
// 						}							
// 					}
// 				}				
// 			}
// 		}
// 	}	
// 	return byRc;//和类型0冲突;
// }

// BOOL addrIsInTask(TSA *sa,TPort *pPort,BYTE byTaskNo)
// {
// TOad60130200 *pCfg;
	
// 	pCfg=pGet698NoTaskCfg(byTaskNo);
// 	if(NULL != pCfg)
// 	{
// 		if(pCfg->byTaskType == 1)
// 		{//普通采集
// 			TOad60150200 *pSch=(TOad60150200 *)pPort->GyRunInfo.psch698;
// 			if(AddrIsInMS(sa,&pSch->objMS))
// 				return TRUE;
// 		}
// 		else if(pCfg->byTaskType == 2)
// 		{//事件采集
// 			TOad60170200 *pSch=(TOad60170200 *)pPort->GyRunInfo.psch698;
// 			if(AddrIsInMS(sa,&pSch->objMS))
// 				return TRUE;
// 		}
// 		else if(pCfg->byTaskType == 3)
// 		{//透明方案

// 		}
// 	}	
// 	return FALSE;
// }

// DWORD get_last_curv_time(WORD mp_no)
// {
// TData_6203 *pDF6203=pGetDF6203();

// 	if((mp_no == 0)||( mp_no>= MAX_MP_NUM))
// 		return FALSE;
// 	return pDF6203->dwLastCurvTime[mp_no];
// }

// void mark_last_curv_time(MS *p_ms,DWORD time)
// {
// TData_6203 *pDF6203=pGetDF6203();
// WORD i;
// TSA sa;
	
// 	CHECKPTR0(p_ms);
// 	for (i=1;i<MAX_MP_NUM;i++)
// 	{
// 		if(!MP_IsValid(i))
// 			continue;
// 		if(!GetMPAddr(i,(BYTE*)&sa))
// 			continue;
// 		if(!AddrIsInMS(&sa,p_ms))
// 			continue;
// 		pDF6203->dwLastCurvTime[i] = time;
// 	}	
// }
///////////////////////////////////////////////////////////////
//	函 数 名 : get_max_m698rx_apdu_len
//	函数功能 : 获得最大不分帧可接收下行报文长度
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年11月23日
//	返 回 值 : WORD
//	参数说明 : TPort *pPort
///////////////////////////////////////////////////////////////
WORD get_max_m698rx_apdu_len(TPort *pPort)
{
WORD wRc=MAX_DWN_485FRAME_LEN-200;//预留200字节用于跟随上报

#if(MT_MODULE == MT_DF6203_JZQ_I)
	if(HHBYTE(pPort->dwID) == MPT_376_2)	
		wRc	= 256-50;
#else
		wRc = 256-66;
#endif
	wRc	-= MAX_HEAD_TAIL_LEN;
	return wRc;
}

BOOL IsCurvSch(TOad60150200 *pSch)
{//是否是曲线采集方案
CSD csdd;
WORD j;

	if(pSch->byCJType == 3)
		return TRUE;
	else if(pSch->byCJType == 2)
	{
		for(j=0;j<pSch->byColNum;j++)
		{
			get_no_csd_of_CSDS(pSch->csdBuf,j,&csdd);
			if(csdd.choice)
			{
				if((HIWORD(csdd.road.oadMain.value) == 0x5002)
					||(HIWORD(csdd.road.oadMain.value) == 0x5003))
					break;
			}
		}		
		return (j<pSch->byColNum);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : SwitchToNextTask
//	函数功能 : 切换到指定端口，指定电表、指定抄表类型的
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年7月14日
//	返 回 值 : BOOL
//	参数说明 : 
//				BYTE *pbyTaskNo,--任务序号，非实际任务号
///////////////////////////////////////////////////////////////
// BOOL SwitchToNextTask(HPARA hPort,DWORD dwCheckTime,BYTE *pbyTaskNo)
// {
// TPort *pPort=(TPort *)hPort; 
// DWORD dwTime,dwStart;
// TOad60130200 *pCfg;
// // TData_6203 *pDF6203=pGetDF6203();	
// BYTE i,no,j,byTaskNo=0xFF;
// WORD k;

// 	no = PortID2No(pPort->dwID);
// 	if(no >= MAX_DWNPORT_NUM)
// 		return FALSE;
// 	//判断是不是曲线采集任务
// 	pCfg=pGet698NoTaskCfg((*pbyTaskNo));
// 	if(NULL != pCfg)
// 	{
// 		if(pCfg->byTaskType == TT_DATA_CJ)
// 		{
// 			if(GetschInfo(pCfg->byTaskType,pCfg->byShemeNo,pPort->GyRunInfo.psch698))
// 			{					
// 				TOad60150200 *pSch=(TOad60150200 *)pPort->GyRunInfo.psch698;
// 				if((pSch->byCJType == 3)&&(pPort->GyRunInfo.CurvCjCfg.byInit == 0x5A))
// 				{
// 					pPort->GyRunInfo.CurvCjCfg.wNo	+= pPort->GyRunInfo.CurvCjCfg.byCJStep;
// 					dwStart	= pPort->GyRunInfo.CurvCjCfg.dwStart+pPort->GyRunInfo.CurvCjCfg.wNo*pPort->GyRunInfo.CurvCjCfg.wInterval;
// 					for(i=1;i<4;i++)
// 					{
// 						if(((dwStart+i*pPort->GyRunInfo.CurvCjCfg.wInterval)/60)*60 != (dwStart/60)*60)
// 						{
// 							pPort->GyRunInfo.CurvCjCfg.byCJStep	= i;
// 							break;
// 						}
// 					}
// 					if(i>= 4)
// 						pPort->GyRunInfo.CurvCjCfg.byCJStep	= 4;
// 					if(pPort->GyRunInfo.CurvCjCfg.wNo < pPort->GyRunInfo.CurvCjCfg.wNum)
// 					{//曲线没有采集结束
// 						pPort->GyRunInfo.CurvCjCfg.byCJInc	= 0;
					
// 						return TRUE;						
// 					}
// 				}
// 				if((pSch->byCJType == 4)&&(pPort->GyRunInfo.ReBuCJCfg.byInit == 0x5A))
// 				{
// 					if(pPort->GyRunInfo.ReBuCJCfg.wReCJStep==0) pPort->GyRunInfo.ReBuCJCfg.wReCJStep = 1;
// 					pPort->GyRunInfo.ReBuCJCfg.wNo	+= pPort->GyRunInfo.ReBuCJCfg.wReCJStep;
// 					dwStart	= pPort->GyRunInfo.ReBuCJCfg.dwStart+pPort->GyRunInfo.ReBuCJCfg.wNo*pPort->GyRunInfo.ReBuCJCfg.wInterval;
// 					if(pPort->GyRunInfo.ReBuCJCfg.wNo < pPort->GyRunInfo.ReBuCJCfg.wNum)
// 					{//补采没有采集结束
// 						pPort->GyRunInfo.ReBuCJCfg.byReCJInc	= 0;
					
// 						return TRUE;						
// 					}
// 				}
// 			}
// 		}
// 	}
// 	memset(&pPort->GyRunInfo.CurvCjCfg,0,sizeof(TCurvCjCfg));
// 	memset(&pPort->GyRunInfo.ReBuCJCfg,0,sizeof(TReCjCfg));

	
// 	for(k=0;k<256;k++)
// 	{
// 		for(i=0;i<MAX_698_TASK_NUM;i++)
// 		{			
// 			pCfg=pGet698NoTaskCfg(i);
// 			if(NULL == pCfg)
// 				continue;
// 			if(pCfg->validflag != 0x5AA5)
// 				continue;
// 			if(pCfg->byPrior != (BYTE)k)
// 				continue;			
// 			if(is_task_delay(pCfg->byShemeNo,pCfg->byTaskType))
// 				continue;
// 			if(cjtask_start(pPort,i,dwCheckTime,&dwTime))
// 			{			
// 				if(GetschInfo(pCfg->byTaskType,pCfg->byShemeNo,pPort->GyRunInfo.psch698))
// 				{
// 					if(pCfg->byTaskType == TT_DATA_CJ)
// 					{//普通采集		
// 						TOad60150200 *pschNo=(TOad60150200 *)pPort->GyRunInfo.psch698;
// 						WORD wStep=0,wCurveIntel,wNum=1;
// 						DWORD dwEnd,dwTmp;
// 						WORD wCJStep = 0;
// 						pPort->GyRunInfo.dwTaskTime	= dwTime;
// 						if (pschNo->dwValidTime == 0xfefefefe)
// 						{
// 							pschNo->dwValidTime =	dwCheckTime*60;	//更新一下
// 							fresh_sch_time(pschNo->byShemeNo,pCfg->byTaskType);	
// 						}
// 						Init_cj_task(i,dwTime,dwCheckTime,1);
// 						g_pTaskRunInfo[i].total_num	= GetMeterEXNumOfMS(&pschNo->objMS);					
// 						//.if(pschNo->byCJType == 3)
// 						if(IsCurvSch(pschNo))
// 						{						
// 							pPort->GyRunInfo.CurvCjCfg.byInit = 0x5A;
// 							if(pschNo->byCJType == 3)
// 							{//按间隔采集
// 								switch(pschNo->CJContent.tii.unit)
// 								{
// 								case 0: wStep = pschNo->CJContent.tii.value/60;		break;	//秒
// 								case 1: wStep = pschNo->CJContent.tii.value;		break;	//分
// 								case 2: wStep = pschNo->CJContent.tii.value*60;		break;	//时	
// 								default: break;
// 								}
						
// 								switch(pCfg->intelTI.unit)
// 								{							
// 								case 1: //分
// 									wCurveIntel = pCfg->intelTI.value;
// 									if(wCurveIntel)
// 									{
// 										dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 										dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 										dwStart	= dwEnd-wCurveIntel;	
// 										if(wStep)
// 											wCJStep= wCurveIntel/wStep;
// 										else
// 											wCJStep= 1;
// 									}
// 									break;	
// 								default:
// 								case 2: //时
// 									wCurveIntel = pCfg->intelTI.value*60;	
// 									if(wCurveIntel)
// 									{
// 										dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 										dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 										dwStart	= dwEnd-wCurveIntel;
// 										if(wStep)
// 											wCJStep= wCurveIntel/wStep;
// 										else
// 											wCJStep= 1;
// 									}
// 									break;	
// 								case 3: //日
// 									wCurveIntel = pCfg->intelTI.value*1440;
// 									if(wCurveIntel)
// 									{

// 										dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 										dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 										dwStart	= dwEnd-wCurveIntel;
// 										if(wStep)
// 											wCJStep= wCurveIntel/wStep;
// 										else
// 											wCJStep= 1;
// 									}	
// 									break;	
// 								}
// 								if((GetUserType() == MAR_SHAN_DONG)&&(wCurveIntel))
// 								{
// 									if(wCurveIntel != 1440) //日采集不抄当前（为了通过流水线)
// 										dwStart += wCurveIntel;//山东采集当前周期
// 								}
// 							}
// 							else if(pschNo->byCJType == 2)
// 							{
// 								wCJStep= 1; wStep = 0;
// 								switch(pCfg->intelTI.unit)
// 								{							
// 								case 1: //分
// 									wCurveIntel = pCfg->intelTI.value;	
// 									if(wCurveIntel==0) 
// 										wCurveIntel = 15;
// 									break;	
// 								default:
// 								case 2: //时
// 									wCurveIntel = pCfg->intelTI.value*60;	
// 									if(wCurveIntel==0) 
// 										wCurveIntel = 60;
// 									break;	
// 								case 3: //日
// 									wCurveIntel = pCfg->intelTI.value*1440;	
// 									if(wCurveIntel==0) 
// 										wCurveIntel = 1440;
// 									break;
// 								}
// 								if(wCurveIntel)
// 									dwStart	= (pPort->GyRunInfo.dwTaskTime/wCurveIntel)*wCurveIntel;
// 								else
// 									dwStart	= pPort->GyRunInfo.dwTaskTime;
// 							}
// 							//异常防护
// 							{
// 								if(wCurveIntel == 0)
// 									wCurveIntel	= 60;
// 								if(wStep == 0)//对采用冻结方式抄读的曲线wStep=0
// 									wStep = wCurveIntel;
// 							}							
// 							if(wStep && wCurveIntel)
// 							{
// 								if(wCJStep == 0)
// 									wCJStep = 1;//一次采集一个点
// 								wNum	= wCurveIntel/wStep;
// 								dwTmp	= ((dwStart/wStep)*wStep);
// 								if(wNum == 0)							
// 									wNum	= 1;							
// 								dwStart	= dwTmp;						
// 								if(wCJStep > 4)
// 								{//一包最多抄4个时间点
// 									for(j=1;j<4;j++)
// 									{
// 										if(((dwTmp+j*wStep)/60)*60 != (dwTmp/60)*60)
// 										{
// 											wCJStep	= j;
// 											break;
// 										}
// 									}
// 									if(j>= 4)
// 										wCJStep	= 4;
// 								}
// 							}
// 							pPort->GyRunInfo.CurvCjCfg.wNum	= wNum;
// 							pPort->GyRunInfo.CurvCjCfg.wNo	= 0;
// 							pPort->GyRunInfo.CurvCjCfg.byCJStep	= (BYTE)wCJStep;
// 							pPort->GyRunInfo.CurvCjCfg.wInterval= wStep;
// 							pPort->GyRunInfo.CurvCjCfg.dwStart	= dwStart;
// 							pPort->GyRunInfo.CurvCjCfg.byCJInc	= 0;

// 							mark_last_curv_time((MS*)&pschNo->objMS,dwStart+(wNum-1)*wStep);

// 						}
// 						else if(pschNo->byCJType == 1)
// 						{//采集上x次
// 							CSD csdd;
// 							pPort->GyRunInfo.byHisDataOffset	= 0;
// 							get_no_csd_of_CSDS(pschNo->csdBuf,0,&csdd);
// 							if(csdd.road.oadMain.value == 0x50040200)
// 							{
// 								pPort->GyRunInfo.byHisDataOffset	= pschNo->CJContent._N;
// 								if((pPort->GyRunInfo.byHisDataOffset&0x7F) > 0)
// 									pPort->GyRunInfo.byHisDataOffset	-= 1;
// 							//	pPort->GyRunInfo.byHisDataOffset	|= 0x80; //WKC
// 							}	
// 						}
// 						else if(pschNo->byCJType == 2)
// 						{//按冻结时间采集
// 							pPort->GyRunInfo.byHisDataOffset	=  0;//0x80;WKC
// 						}
// 						else if(pschNo->byCJType == 4)
// 						{//按补采方案采集
// 							pPort->GyRunInfo.ReBuCJCfg.byInit = 0x5A;
// 							if(TI2Mins(&pschNo->CJContent.retryMeter.tii)!=0)
// 								wCJStep = TI2Mins(&pCfg->intelTI)/TI2Mins(&pschNo->CJContent.retryMeter.tii);
// 							else
// 								wCJStep = 1;
// 							if( wCJStep==0 ) wCJStep = 1;
// 							if( wCJStep >4 ) wCJStep = 4;
// 							pPort->GyRunInfo.ReBuCJCfg.wNum	= pschNo->CJContent.retryMeter.lastNums*wCJStep;
// 							pPort->GyRunInfo.ReBuCJCfg.wNo	= 0;
// 							pPort->GyRunInfo.ReBuCJCfg.wReCJStep	= wCJStep;
// 							pPort->GyRunInfo.ReBuCJCfg.wInterval = TI2Mins(&pschNo->CJContent.retryMeter.tii);
// 							if(pPort->GyRunInfo.ReBuCJCfg.wInterval==0 || pPort->GyRunInfo.ReBuCJCfg.wInterval==0xFFFF) 
// 								continue;
// 							dwStart =  pPort->GyRunInfo.dwTaskTime - (pschNo->CJContent.retryMeter.lastNums*(TI2Mins(&pCfg->intelTI)));
// 							pPort->GyRunInfo.ReBuCJCfg.dwStart	 = dwStart;
// 							pPort->GyRunInfo.ReBuCJCfg.byReCJInc	= 0;

// 						}
// 						else if(pschNo->byCJType == 0)
// 						{//实时数据

// 						}
// 						*pbyTaskNo	= i;										
// 						return TRUE;	
// 					}
// 					else if(pCfg->byTaskType == TT_EVENT_CJ)
// 					{//事件采集			
// 	 					TOad60170200 *pschNo=(TOad60170200 *)pPort->GyRunInfo.psch698;
// 	 					pPort->GyRunInfo.dwTaskTime	= dwTime;
// 						Init_cj_task(i,dwTime,dwCheckTime,1);
// 						g_pTaskRunInfo[i].total_num	= GetMeterEXNumOfMS(&pschNo->objMS);	
// 						*pbyTaskNo	= i;
// 						if(GetUserType() == MAR_SHAN_DONG)
// 						{
// 							if((g_pTaskRunInfo[i].total_num > 50)&&(TI2Mins(&pCfg->intelTI)) < 15)
// 							{
// 								g_pTaskRunInfo[i].byStatus = 2;
// 								continue;
// 							}
// 						}
// 						return TRUE;
// 					}				
// 				}			
// 			}		
// 		}	
// 	}

// 	if(k>=256)
// 	{
// 		WORD wIntel = 2;
// 		for(i=((BYTE)(*pbyTaskNo+1));i<MAX_698_TASK_NUM;i++)
// 		{	
// 			TTask698RunInfo *pRunInfo;
// 			pCfg=pGet698NoTaskCfg(i);
// 			if(NULL == pCfg)
// 				continue;
// 			if(pCfg->validflag != 0x5AA5)
// 				continue;
// 			if(pCfg->byStuts!=1)
// 				continue;
// 			if((pCfg->byTaskType != TT_DATA_CJ) 
// 				&& (pCfg->byTaskType != TT_EVENT_CJ)
// 				&& (pCfg->byTaskType != TT_TRANS_CJ))
// 				continue;
// 			pRunInfo=&g_pTaskRunInfo[i];
// 			if( pRunInfo->byRetryNum[no] > 10 )
// 				wIntel = 60;
// 			else
// 				wIntel = 2;
// 			if( pRunInfo->byStatus==1 && ((DWORD)(dwCheckTime-pRunInfo->startTask_time[no])>=wIntel) ) //执行中
// 			{
// 				pRunInfo->startTask_time[no] = dwCheckTime;
// 				pRunInfo->byRetryNum[no]++;
// 				if(GetschInfo(pCfg->byTaskType,pCfg->byShemeNo,pPort->GyRunInfo.psch698))
// 				{

// 					if( pRunInfo->byRetryNum[no] > 20 ) 
// 						continue;
// 					if(pCfg->byTaskType == TT_DATA_CJ)
// 					{//普通采集		
// 						TOad60150200 *pschNo=(TOad60150200 *)pPort->GyRunInfo.psch698;
// 						WORD wStep=0,wCurveIntel,wNum=1;
// 						DWORD dwEnd,dwTmp;
// 						WORD wCJStep = 0;
// 						if(pschNo->byCJType == 4)
// 						{//补采的补采不做 
// 							if( pRunInfo->tx_packet_num==pRunInfo->rx_packet_num )
// 							{
// 								pRunInfo->byStatus = 2;
// 								pRunInfo->success_num = pRunInfo->total_num;
// 								pRunInfo->tx_packet_num = pRunInfo->total_num;
// 								pRunInfo->rx_packet_num = pRunInfo->total_num;
// 							}
// 							continue;
// 						}
// 						pPort->GyRunInfo.dwTaskTime	= pRunInfo->dwTaskTime;
// 						if (pschNo->dwValidTime == 0xfefefefe)
// 						{
// 							pschNo->dwValidTime =	dwCheckTime*60;	//更新一下
// 							fresh_sch_time(pschNo->byShemeNo,pCfg->byTaskType);	
// 						}
// 						if(IsCurvSch(pschNo))
// 						{						
// 							pPort->GyRunInfo.CurvCjCfg.byInit = 0x5A;
// 							if(pschNo->byCJType == 3)
// 							{//按间隔采集
// 								switch(pschNo->CJContent.tii.unit)
// 								{
// 									case 0: wStep = pschNo->CJContent.tii.value/60;		break;	//秒
// 									case 1: wStep = pschNo->CJContent.tii.value;		break;	//分
// 									case 2: wStep = pschNo->CJContent.tii.value*60;		break;	//时	
// 									default: break;
// 								}
// 								switch(pCfg->intelTI.unit)
// 								{							
// 									case 1: //分
// 										wCurveIntel = pCfg->intelTI.value;	
// 										if(wCurveIntel)
// 										{
// 											dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 											dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 											dwStart	= dwEnd-wCurveIntel;	
// 											if(wStep)
// 												wCJStep= wCurveIntel/wStep;
// 											else
// 												wCJStep= 1;
// 										}
// 										break;	
// 									default:
// 									case 2: //时
// 										wCurveIntel = pCfg->intelTI.value*60;	
// 										if(wCurveIntel)
// 										{
// 											dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 											dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 											dwStart	= dwEnd-wCurveIntel;
// 											if(wStep)
// 												wCJStep= wCurveIntel/wStep;
// 											else
// 												wCJStep= 1;
// 										}
// 										break;	
// 									case 3: //日
// 										wCurveIntel = pCfg->intelTI.value*1440;
// 										if(wCurveIntel)
// 										{

// 											dwEnd	= pPort->GyRunInfo.dwTaskTime;
// 											dwEnd	= (dwEnd/wCurveIntel)*wCurveIntel;	
// 											dwStart	= dwEnd-wCurveIntel;
// 											if(wStep)
// 												wCJStep= wCurveIntel/wStep;
// 											else
// 												wCJStep= 1;
// 										}	
// 										break;	
// 								}
// 								if((GetUserType() == MAR_SHAN_DONG)&&(wCurveIntel))
// 								{
// 									if(wCurveIntel != 1440) //日采集不抄当前（为了通过流水线)
// 										dwStart += wCurveIntel;//山东采集当前周期
// 								}
// 							}
// 							else if(pschNo->byCJType == 2)
// 							{
// 								wCJStep= 1; wStep = 0;
// 								switch(pCfg->intelTI.unit)
// 								{							
// 									case 1: //分
// 										wCurveIntel = pCfg->intelTI.value;	
// 										if(wCurveIntel==0) 
// 											wCurveIntel = 15;
// 										break;	
// 									default:
// 									case 2: //时
// 										wCurveIntel = pCfg->intelTI.value*60;	
// 										if(wCurveIntel==0) 
// 											wCurveIntel = 60;
// 										break;	
// 									case 3: //日
// 										wCurveIntel = pCfg->intelTI.value*1440;	
// 										if(wCurveIntel==0) 
// 											wCurveIntel = 1440;
// 										break;
// 								}
// 								if(wCurveIntel)
// 									dwStart	= (pPort->GyRunInfo.dwTaskTime/wCurveIntel)*wCurveIntel;
// 								else
// 									dwStart	= pPort->GyRunInfo.dwTaskTime;
// 							}							
// 							//异常防护
// 							{
// 								if(wCurveIntel == 0)
// 									wCurveIntel	= 60;
// 								if(wStep == 0)//对采用冻结方式抄读的曲线wStep=0
// 									wStep = wCurveIntel;
// 							}							
// 							if(wStep && wCurveIntel)
// 							{
// 								if(wCJStep == 0)
// 									wCJStep = 1;//一次采集一个点
// 								wNum	= wCurveIntel/wStep;
// 								dwTmp	= ((dwStart/wStep)*wStep);
// 								if(wNum == 0)							
// 									wNum	= 1;							
// 								dwStart	= dwTmp;						
// 								if(wCJStep > 4)
// 								{//一包最多抄4个时间点
// 									for(j=1;j<4;j++)
// 									{
// 										if(((dwTmp+j*wStep)/60)*60 != (dwTmp/60)*60)
// 										{
// 											wCJStep	= j;
// 											break;
// 										}
// 									}
// 									if(j>= 4)
// 										wCJStep	= 4;
// 								}
// 							}
// 							pPort->GyRunInfo.CurvCjCfg.wNum	= wNum;
// 							pPort->GyRunInfo.CurvCjCfg.wNo	= 0;
// 							pPort->GyRunInfo.CurvCjCfg.byCJStep	= (BYTE)wCJStep;
// 							pPort->GyRunInfo.CurvCjCfg.wInterval= wStep;
// 							pPort->GyRunInfo.CurvCjCfg.dwStart	= dwStart;
// 							pPort->GyRunInfo.CurvCjCfg.byCJInc	= 0;
// 						}
// 						else if(pschNo->byCJType == 1)
// 						{//采集上x次
// 							CSD csdd;
// 							pPort->GyRunInfo.byHisDataOffset	= 0;
// 							get_no_csd_of_CSDS(pschNo->csdBuf,0,&csdd);
// 							if(csdd.road.oadMain.value == 0x50040200)
// 							{
// 								pPort->GyRunInfo.byHisDataOffset	= pschNo->CJContent._N;
// 								if((pPort->GyRunInfo.byHisDataOffset&0x7F) > 0)
// 									pPort->GyRunInfo.byHisDataOffset	-= 1;
// 							//	pPort->GyRunInfo.byHisDataOffset	|= 0x80; //WKC
// 							}	
// 						}
// 						else if(pschNo->byCJType == 2)
// 						{//按冻结时间采集
// 							pPort->GyRunInfo.byHisDataOffset	=  0;//0x80;WKC
// 						}
// 						else if(pschNo->byCJType == 4)
// 						{//补采的补采不做 
// 							return FALSE;
// 						}
// 						*pbyTaskNo	= i;										
// 						return TRUE;
// 					}
// 					else if(pCfg->byTaskType == TT_EVENT_CJ)
// 					{//事件采集

// #if 0
// 						if(TI2Mins(&pCfg->intelTI) >= 1440)
// 							continue;
// 						if( pRunInfo->byRetryNum[no] > 2) 	
// 							continue;
// 						pPort->GyRunInfo.dwTaskTime	= pRunInfo->dwTaskTime;
// 						*pbyTaskNo	= i;
// 						return TRUE;
// #else
// 						continue;	//事件不再补采
// #endif

// 					}
// 					else if(pCfg->byTaskType == TT_TRANS_CJ)
// 					{
// 						if( pRunInfo->byRetryNum[no] > 1) 	
// 							continue;
// 						pPort->GyRunInfo.dwTaskTime	= pRunInfo->dwTaskTime;
// 						*pbyTaskNo	= i;
// 						return TRUE;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return FALSE;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : GetMeterNumOfMS
//	函数功能 : 获得MS集合中电表个数
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月4日
//	返 回 值 : WORD
//	参数说明 : MS *ms
///////////////////////////////////////////////////////////////
WORD GetMeterNumOfMS(MS *ms)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
TMetCfg_Base Cfg;
WORD wNum=0,wNo,i,byValid;
	
	for(wNo=1;wNo<MAX_MP_NUM;wNo++)
	{
		if(pMPFlagsCtrl->Flag_Valid[wNo>>3] & (1<<(wNo&0x07)))
		{
			switch(ms->choice)
			{
			case 1:	//全部用户地址
				wNum++;
				break;	
			case 2: //一组用户类型
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->mst.size;i++)
					{
						if(Cfg.byUserType == ms->mst.type[i])
						{
							wNum++;
							break;
						}
					}
				}
				break;
			case 3: //一组用户地址
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msa.size;i++)
					{
						if(comp_TSA_addr(&Cfg.meter_addr,&ms->msa.add[i]) == 0)
						{
							wNum++;
							break;
						}
					}
				}
				break;
			case 4: //一组配置序号
				for(i=0;i<ms->msi.size;i++)
				{
					if(wNo == ms->msi.idx[i])
					{
						wNum++;
						break;
					}
				}
				break;
			case 5: //一组用户类型区间
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msrt.size;i++)
					{
						byValid	= 0;
						switch(ms->msrt.region[i].nType)
						{
						case 0: //前闭后开    （0），
							if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 1: //前开后闭    （1），
							if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 2: //前闭后闭    （2），
							if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 3: // 前开后开    （3）
							if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
								byValid	= 1;
							break;
						}
						if(byValid)
						{
							wNum++;
							break;
						}	
					}
				}
				break;
			case 6: //一组用户地址区间	
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msra.size;i++)
					{
						byValid	= 0;
						switch(ms->msra.region[i].nType)
						{
						case 0: //前闭后开    （0），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) < 0))
								byValid	= 1;
							break;
						case 1: //前开后闭    （1），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) <= 0))
								byValid	= 1;
							break;
						case 2: //前闭后闭    （2），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) <= 0))				
								byValid	= 1;
							break;
						case 3: // 前开后开    （3）
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) < 0))				
								byValid	= 1;
							break;
						}
						if(byValid)
						{
							wNum++;
							break;
						}	
					}
				}
				break;
			case 7: //一组配置序号区间	
				for(i=0;i<ms->msri.size;i++)
				{
					byValid	= 0;
					switch(ms->msri.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((wNo >= ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 1: //前开后闭    （1），
						if((wNo > ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 2: //前闭后闭    （2），
						if((wNo >= ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 3: // 前开后开    （3）
						if((wNo > ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					}				
					if(byValid)
					{
						wNum++;
						break;
					}	
				}
				break;
			}
		}	
	}	
	return wNum;
}

WORD GetMeterEXNumOfMS(MS *ms)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
TMetCfg_Base Cfg;
WORD wNum=0,wNo,i,byValid;
	
	for(wNo=1;wNo<MAX_MP_NUM;wNo++)
	{
		if((pMPFlagsCtrl->PortMPFlag[0][wNo>>3] & (1<<(wNo&0x07)))
			||(pMPFlagsCtrl->PortMPFlag[1][wNo>>3] & (1<<(wNo&0x07)))
		#if(MT_MODULE != MT_DF6203_FK_III)
			||(pMPFlagsCtrl->PortMPFlag[2][wNo>>3] & (1<<(wNo&0x07)))
		#endif
			)
		{
			switch(ms->choice)
			{
			case 1:	//全部用户地址
				wNum++;
				break;	
			case 2: //一组用户类型
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->mst.size;i++)
					{
						if(Cfg.byUserType == ms->mst.type[i])
						{
							wNum++;
							break;
						}
					}
				}
				break;
			case 3: //一组用户地址
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msa.size;i++)
					{
						if(comp_TSA_addr(&Cfg.meter_addr,&ms->msa.add[i]) == 0)
						{
							wNum++;
							break;
						}
					}
				}
				break;
			case 4: //一组配置序号
				for(i=0;i<ms->msi.size;i++)
				{
					if(wNo == ms->msi.idx[i])
					{
						wNum++;
						break;
					}
				}
				break;
			case 5: //一组用户类型区间
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msrt.size;i++)
					{
						byValid	= 0;
						switch(ms->msrt.region[i].nType)
						{
						case 0: //前闭后开    （0），
							if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 1: //前开后闭    （1），
							if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 2: //前闭后闭    （2），
							if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
								byValid	= 1;
							break;
						case 3: // 前开后开    （3）
							if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
								byValid	= 1;
							break;
						}
						if(byValid)
						{
							wNum++;
							break;
						}	
					}
				}
				break;
			case 6: //一组用户地址区间	
				if(sys_GetMPCfg(wNo,&Cfg))
				{
					for(i=0;i<ms->msra.size;i++)
					{
						byValid	= 0;
						switch(ms->msra.region[i].nType)
						{
						case 0: //前闭后开    （0），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) < 0))
								byValid	= 1;
							break;
						case 1: //前开后闭    （1），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) <= 0))
								byValid	= 1;
							break;
						case 2: //前闭后闭    （2），
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) <= 0))				
								byValid	= 1;
							break;
						case 3: // 前开后开    （3）
							if((comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&Cfg.meter_addr, &ms->msra.region[i].end) < 0))				
								byValid	= 1;
							break;
						}
						if(byValid)
						{
							wNum++;
							break;
						}	
					}
				}
				break;
			case 7: //一组配置序号区间	
				for(i=0;i<ms->msri.size;i++)
				{
					byValid	= 0;
					switch(ms->msri.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((wNo >= ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 1: //前开后闭    （1），
						if((wNo > ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 2: //前闭后闭    （2），
						if((wNo >= ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 3: // 前开后开    （3）
						if((wNo > ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					}				
					if(byValid)
					{
						wNum++;
						break;
					}	
				}
				break;
			}
		}	
	}	
	return wNum;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GetNoMeterOfMS
//	函数功能 : 获得MS列表中第NO块表的信息
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月3日
//	返 回 值 : BOOL
//	参数说明 : WORD No,（1---xx）
//				MS *ms,
//				TMetCfg_Base *pCfg
///////////////////////////////////////////////////////////////
BOOL GetNoMeterOfMS(WORD No,MS *ms,TMetCfg_Base *pCfg)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
WORD wNum=0,wNo,i,byValid;
	
	for(wNo=1;wNo<MAX_MP_NUM;wNo++)
	{
		if(pMPFlagsCtrl->Flag_Valid[wNo>>3] & (1<<(wNo&0x07)))
		{
			if(!sys_GetMPCfg(wNo,pCfg))
				continue;
			switch(ms->choice)
			{
			case 1:	wNum++;		break;	//全部用户地址
			case 2: //一组用户类型
				for(i=0;i<ms->mst.size;i++)
				{
					if(pCfg->byUserType == ms->mst.type[i])
					{
						wNum++;
						break;
					}
				}
				break;
			case 5: //一组用户类型区间
				for(i=0;i<ms->msrt.size;i++)
				{
					byValid	= 0;
					switch(ms->msrt.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((pCfg->byUserType >= ms->msrt.region[i].start) && (pCfg->byUserType < ms->msrt.region[i].end))
							byValid	= 1;
						break;
					case 1: //前开后闭    （1），
						if((pCfg->byUserType > ms->msrt.region[i].start) && (pCfg->byUserType <= ms->msrt.region[i].end))
							byValid	= 1;
						break;
					case 2: //前闭后闭    （2），
						if((pCfg->byUserType >= ms->msrt.region[i].start) && (pCfg->byUserType <= ms->msrt.region[i].end))
							byValid	= 1;
						break;
					case 3: // 前开后开    （3）
						if((pCfg->byUserType > ms->msrt.region[i].start) && (pCfg->byUserType < ms->msrt.region[i].end))
							byValid	= 1;
						break;
					}
					if(byValid)
					{
						wNum++;
						break;
					}	
				}
				break;
			case 6: //一组用户地址区间	
				for(i=0;i<ms->msra.size;i++)
				{
					byValid	= 0;
					switch(ms->msra.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].end) < 0))
							byValid	= 1;
						break;
					case 1: //前开后闭    （1），
						if((comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].end) <= 0))
							byValid	= 1;
						break;
					case 2: //前闭后闭    （2），
						if((comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].end) <= 0))				
							byValid	= 1;
						break;
					case 3: // 前开后开    （3）
						if((comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(&pCfg->meter_addr, &ms->msra.region[i].end) < 0))				
							byValid	= 1;
						break;
					}
					if(byValid)
					{
						wNum++;
						break;
					}	
				}
				break;
			case 7: //一组配置序号区间	
				for(i=0;i<ms->msri.size;i++)
				{
					byValid	= 0;
					switch(ms->msri.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((wNo >= ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 1: //前开后闭    （1），
						if((wNo > ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 2: //前闭后闭    （2），
						if((wNo >= ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
							byValid	= 1;
						break;
					case 3: // 前开后开    （3）
						if((wNo > ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
							byValid	= 1;
						break;
					}				
					if(byValid)
					{
						wNum++;
						break;
					}	
				}
				break;
			}
			if(No == wNum)
				return TRUE;
		}	
	}
	switch(ms->choice)
	{
	case 3: //一组用户地址
		if(No < ms->msa.size)
		{
			WORD wMPReqNo;
			Addr2MP(0,&ms->msa.add[No-1],FALSE,&wMPReqNo);
			return sys_GetMPCfg(wMPReqNo,pCfg);
		}		
		break;
	case 4: //一组配置序号
		if(No < ms->msa.size)
			return sys_GetMPCfg(ms->msi.idx[No-1],pCfg);
		break;	
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : AddrIsInMS
//	函数功能 : 指定地址是否在MS范围内
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月4日
//	返 回 值 : BOOL
//	参数说明 : TSA *sa,
//				MS *ms
///////////////////////////////////////////////////////////////
BOOL AddrIsInMS(TSA *sa,MS *ms)
{
WORD wNo,i;

	switch(ms->choice)
	{
	default:
		return FALSE;
	case 1:	return TRUE;	//全部用户地址
	case 2: //一组用户类型
		if(Addr2MP(0,sa,FALSE,&wNo))
		{
			TMetCfg_Base Cfg;
			if(sys_GetMPCfg(wNo,&Cfg))
			{
				for(i=0;i<ms->mst.size;i++)
				{
					if(Cfg.byUserType == ms->mst.type[i])
						return TRUE;
				}
			}
		}
		return FALSE;
	case 3: //一组用户地址
		for(i=0;i<ms->msa.size;i++)
		{
			if(comp_TSA_addr(sa,&ms->msa.add[i]) == 0)
				return TRUE;
		}
		return FALSE;
	case 4: //一组配置序号
		if(Addr2MP(0,sa,FALSE,&wNo))
		{
			for(i=0;i<ms->msi.size;i++)
			{
				if(wNo == ms->msi.idx[i])
					return TRUE;
			}
		}
		return FALSE;
	case 5: //一组用户类型区间
		if(Addr2MP(0,sa,FALSE,&wNo))
		{
			TMetCfg_Base Cfg;
			if(sys_GetMPCfg(wNo,&Cfg))
			{
				for(i=0;i<ms->msrt.size;i++)
				{
					switch(ms->msrt.region[i].nType)
					{
					case 0: //前闭后开    （0），
						if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
							return TRUE;
						break;
					case 1: //前开后闭    （1），
						if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
							return TRUE;
						break;
					case 2: //前闭后闭    （2），
						if((Cfg.byUserType >= ms->msrt.region[i].start) && (Cfg.byUserType <= ms->msrt.region[i].end))
							return TRUE;
						break;
					case 3: // 前开后开    （3）
						if((Cfg.byUserType > ms->msrt.region[i].start) && (Cfg.byUserType < ms->msrt.region[i].end))
							return TRUE;
						break;
					}					
				}
			}
		}
		return FALSE;
	case 6: //一组用户地址区间	
		for(i=0;i<ms->msra.size;i++)
		{
			switch(ms->msra.region[i].nType)
			{
			case 0: //前闭后开    （0），
				if((comp_TSA_addr(sa, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(sa, &ms->msra.region[i].end) < 0))
					return TRUE;
				break;
			case 1: //前开后闭    （1），
				if((comp_TSA_addr(sa, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(sa, &ms->msra.region[i].end) <= 0))
					return TRUE;
				break;
			case 2: //前闭后闭    （2），
				if((comp_TSA_addr(sa, &ms->msra.region[i].start) >= 0)  && (comp_TSA_addr(sa, &ms->msra.region[i].end) <= 0))				
					return TRUE;
				break;
			case 3: // 前开后开    （3）
				if((comp_TSA_addr(sa, &ms->msra.region[i].start) > 0)  && (comp_TSA_addr(sa, &ms->msra.region[i].end) < 0))				
					return TRUE;
				break;
			}				
		}
		return FALSE;
	case 7: //一组配置序号区间	
		if(Addr2MP(0,sa,FALSE,&wNo))
		{
			for(i=0;i<ms->msri.size;i++)
			{
				switch(ms->msri.region[i].nType)
				{
				case 0: //前闭后开    （0），
					if((wNo >= ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
						return TRUE;
					break;
				case 1: //前开后闭    （1），
					if((wNo > ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
						return TRUE;
					break;
				case 2: //前闭后闭    （2），
					if((wNo >= ms->msri.region[i].start) && (wNo <= ms->msri.region[i].end))
						return TRUE;
					break;
				case 3: // 前开后开    （3）
					if((wNo > ms->msri.region[i].start) && (wNo < ms->msri.region[i].end))
						return TRUE;
					break;
				}				
			}
		}
		return FALSE;
	}
}


WORD GetMeterNumOfTranScheme(TOad60190200 *psch)
{
TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
TMetCfg_Base Cfg;
WORD wNum=0,wNo,i;
	
	for(wNo=1;wNo<MAX_MP_NUM;wNo++)
	{
		if(pMPFlagsCtrl->Flag_Valid[wNo>>3] & (1<<(wNo&0x07)))
		{
			if(sys_GetMPCfg(wNo,&Cfg))
			{
				for(i=0;i<psch->byContentNum;i++)
				{
					if(comp_TSA_addr(&Cfg.meter_addr,&psch->Contents[i].tsa) == 0)
					{
						wNum++;
						break;
					}
				}
			}
		}
	}	
	return wNum;	
}
///////////////////////////////////////////////////////////////
//	函 数 名 : GetTimePointNum
//	函数功能 : 根据时间范围确定有多少个时间点
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年10月14日
//	返 回 值 : WORD
//	参数说明 : DATETIME_S *pStartTime,
//				DATETIME_S *pEndTime,
//				TI *pti
///////////////////////////////////////////////////////////////
WORD GetTimePointNum(DATETIME_S *pStartTime,DATETIME_S *pEndTime,TI *pti,BYTE byChioce)
{
TTime tTimeS,tTimeE;
DWORD dwTime,dwTime2,dwOffset=0xFFFFFFFF,dwTime3;
WORD wNum=0;
TTime time;

	DATETIME_S_Time(pStartTime,&tTimeS,TD12);
	DATETIME_S_Time(pEndTime,&tTimeE,TD12);
	dwTime	= Time2Min(&tTimeS);
	dwTime2	= Time2Min(&tTimeE);
	GetTime(&time);
	dwTime3	= Time2Min(&time);
	dwTime2 = min(dwTime2,dwTime3+1);
	// if((GetUserType()==MAR_HE_NAN)
	//  	&& ((pti->unit==3) && (pti->value==1)))
	// {
	// 	if( (dwTime2+1)==dwTime )
	// 		wNum = 1;
	// }
	if (byChioce == 7)  //只对主站下发方法7的抄读特殊处理
	{
		if ((pti->unit==3) && (pti->value==1)&&(get_dfrz_save_time_type() == 3)) //日冻结采集时标，且存储时标是相对上日23:59
		{
			if( (dwTime2+1)==dwTime )//这个条件可以满足结束时间是23:59的采集
				wNum = 1;
			else
			{
				dwTime -= 1440;//这个条件用来满足采集、结束时标都是00:00或者多天采集的情况
			}
		}
		if((pti->unit==4) && (pti->value==1)&&(get_Mfrz_save_time_type() == 7))
		{
			if( (dwTime2+1)==dwTime )
				wNum = 1;
			else
				dwTime -= 1;//这个条件用来满足采集、结束时标都是00:00
		}
	}
	while(dwTime < dwTime2)
	{
		switch(pti->unit)
		{	
		case 0://秒
			dwOffset	= pti->value/60;
			break;
		case 1://分
			dwOffset	= pti->value;
			break;
		case 2:
			dwOffset	= pti->value*60;
			break;
		case 3:
			dwOffset	= pti->value*1440;
			break;
		case 4://月
//..			dwOffset	= 0;
//..			Min2Time(dwTime,&tTimeS);
			dwOffset  = 0x7FFFFFFF;
			tTimeS.Mon++;
			if(tTimeS.Mon > 12)
			{
				tTimeS.Mon	= 1;
				tTimeS.wYear++;				
			}
			dwTime	= Time2Min(&tTimeS);
			break;
		default:
			dwOffset	= 0xFFFFFFFF;
			break;
		}
		if(dwOffset == 0xFFFFFFFF)
			break;
		if(dwOffset == 0)
		{
			wNum	= 1;
			break;
		}
		if(dwOffset == 0x7FFFFFFF)
		{
			wNum++;
			if(wNum>12) 
				break;
			continue;
		 }
		wNum++;
		dwTime	+= dwOffset;
	}
	if(wNum > 288)
		wNum = 288;
	return wNum;
}

BOOL AddTime(DATETIME_S *pStartTime,WORD wtiNum,TI *pti,DATETIME_S *pRealTime)
{
TTime tTimeS;
DWORD dwTime,dwOffset;

	DATETIME_S_Time(pStartTime,&tTimeS,TD12);
	dwTime	= Time2Min(&tTimeS);
	switch(pti->unit)
	{		
	case 1://分
		dwOffset	= pti->value;
		if(dwOffset == 0)
			dwOffset	= 60;
		break;
	case 2:
		dwOffset	= pti->value*60;
		if(dwOffset == 0)
			dwOffset	= 60;
		break;
	case 3:
		dwOffset	= pti->value*1440;
		if(dwOffset == 0)
			dwOffset	= 1440;
		break;
	case 4://月
		dwOffset	= 0;
		while(wtiNum--)
		{
			Min2Time(dwTime,&tTimeS);
			tTimeS.Mon++;
			if(tTimeS.Mon > 12)
			{
				tTimeS.Mon	= 1;
				tTimeS.wYear++;				
			}
			dwTime	= Time2Min(&tTimeS);
		}
		break;
	default:
		memset(pRealTime,0,sizeof(DATETIME_S));
		return FALSE;
	}	
	dwTime	+= wtiNum*dwOffset;
	Min2Time(dwTime,&tTimeS);
	DATETIME_S_Time(pRealTime,&tTimeS,TD21);
	return TRUE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : PortID2OAD
//	函数功能 : 将端口ID转换成上行的OAD信息
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月7日
//	返 回 值 : DWORD
//	参数说明 : DWORD dwPortID
///////////////////////////////////////////////////////////////
DWORD PortID2OAD(DWORD dwPortID)
{
	if(HHBYTE(dwPortID) == MPT_376_2)
	{
		return 0xF2090201;
	}
	else if(HHBYTE(dwPortID) == MPT_ETHERNET)
	{
		return 0x45100200;	
	}
	else if(HHBYTE(dwPortID) == MPT_376_3)
	{
		return 0x45000200;	
	}
	else
	{
		return (0xF2010200 | LHBYTE(dwPortID));	
	}
}

///////////////////////////////////////////////////////////////
//	函 数 名 : get_X_data_of_buf
//	函数功能 : 获得数据数据组的第X个数据
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月11日
//	返 回 值 : BOOL
//	参数说明 : BYTE *pInbuf,
//				BYTE byoffset,
//				BYTE *poutbuf,
//				WORD *pwoutlen
///////////////////////////////////////////////////////////////
BOOL get_X_data_of_buf(BYTE *pInbuf,BYTE byoffset,BYTE *poutbuf,WORD *pwoutlen)
{
WORD num,wLen=0,i,tmp,j;

	if(pInbuf[0] == dt_null) 
	{//对组合为空的特殊处理
		poutbuf[0]	= dt_null;
		*pwoutlen	= 1;
		return TRUE;
	}
	if((pInbuf[0] != dt_structure) 
		&& (pInbuf[0] != dt_array))
	{
		*pwoutlen	= 0;
		return FALSE;
	}
	wLen++;
	wLen	+= gdw698Buf_Num(&pInbuf[wLen],&num,TD12);
	for(i=0;i<num;i++)
	{
		if((byoffset-1) == i)
		{
			tmp	= objGetDataLen((OBJ_DATA_TYPE)pInbuf[wLen],&pInbuf[wLen+1],MAX_NUM_NEST)+1;
			for(j=0;j<tmp;j++)
				poutbuf[j]	= pInbuf[wLen+j];
			*pwoutlen	= tmp;
			return TRUE;
		}
		else
			wLen	+= objGetDataLen((OBJ_DATA_TYPE)pInbuf[wLen],&pInbuf[wLen+1],MAX_NUM_NEST)+1;
	}
	*pwoutlen	= 0;
	return FALSE;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : set_X_data_of_buf
//	函数功能 : 更新一个结构或数组的第x个元素
//	处理过程 : 
//	备    注 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月11日
//	返 回 值 : BOOL
//	参数说明 : BYTE *pInbuf,
//				BYTE byoffset,
//				BYTE *poutbuf
///////////////////////////////////////////////////////////////
BOOL set_X_data_of_buf(BYTE *pInbuf,BYTE byoffset,BYTE *poutbuf)
{
WORD num,wLen=0,i,tmp,j,wtotal,tmp2;
WORD len=objGetDataLen((OBJ_DATA_TYPE)pInbuf[0],&pInbuf[1],MAX_NUM_NEST)+1;
BOOL bRc=FALSE;	

	wtotal=objGetDataLen((OBJ_DATA_TYPE)poutbuf[0],&poutbuf[1],MAX_NUM_NEST);
	wLen++;
	wLen	+= gdw698Buf_Num(&poutbuf[wLen],&num,TD12);
	for(i=0;i<num;i++)
	{
		if((byoffset-1) == i)
		{
			tmp	= objGetDataLen((OBJ_DATA_TYPE)poutbuf[wLen],&poutbuf[wLen+1],MAX_NUM_NEST)+1;
			if(tmp == len)	
				memcpy(&poutbuf[wLen],pInbuf,len);
			else if(tmp > len)
			{//后面数据需前移
				tmp2	= wLen+tmp;
				memcpy(&poutbuf[wLen],pInbuf,len);
				wLen	+= len;
				for(j=tmp2;j<wtotal;j++)
					poutbuf[wLen++]	= poutbuf[j];
			}
			else
			{//后面数据需后移
				tmp2	= wLen+tmp;												
				for(j=0;j<(wtotal-tmp2);j++)
					poutbuf[wtotal-tmp2+len-1-j]	= poutbuf[wtotal-1-j];
				memcpy(&poutbuf[wLen],pInbuf,len);
			}
			bRc	= TRUE;
			break;
		}
		else
			wLen	+= objGetDataLen((OBJ_DATA_TYPE)poutbuf[wLen],&poutbuf[wLen+1],MAX_NUM_NEST)+1;
	}	
	return bRc;
}

///////////////////////////////////////////////////////////////
//	类    名 : (WORDoi,BYTE*pbyclass)
//	基    类 : 
//	描    述 : 根据OI找到对应的基类号
//	功    能 : 
//	历史记录 : 
//	使用说明 : 
//	作    者 : 蒋剑跃
//	时    间 : 2017年8月13日
//	备    注 : 
///////////////////////////////////////////////////////////////
BOOL oi2class(WORD oi,BYTE *pbyclass)
{
BYTE classno=(BYTE)((HIBYTE(oi)>>4)&0x0F);

	switch(classno)
	{
	case 0: //电量类
		if(((HIBYTE(oi)&0x0F) > 5)
			||(((oi&0xF0)>>4) > 0x0A)||(((HIBYTE(oi)&0x0F) >= 3)&&(oi&0xF0))
			||((oi&0x0F) > 3))
			break;
		*pbyclass	= 1;
		return TRUE;
	case 1://需量类
		if(((HIBYTE(oi)&0x0F) > 1)
			||(((oi&0xF0)>>4) > 0x0A)||((oi&0x0F) > 3))
			break;
		*pbyclass	= 2;
		return TRUE;
	case 2://变量类
		if(((oi >= 0x2000) && (oi < 0x2004))
			||(oi == 0x200B)
			||(oi == 0x200C))
		{//变量类
			*pbyclass	= 3;
			return TRUE;
		}
		else if((oi >= 0x2004) && (oi <= 0x200A))
		{//功率类
			*pbyclass	= 4;
			return TRUE;
		}
		else if((oi == 0x200D) || (oi == 0x200E))
		{//谐波类
			*pbyclass	= 5;
			return TRUE;
		}
		else if(((oi >= 0x200F) && (oi <= 0x2014))
			||((oi >= 0x2017) && (oi <= 0x201C))
			||((oi >= 0x2026) && (oi <= 0x2029))
			||((oi >= 0x202D) && (oi <= 0x202E))
			||((oi >= 0x2031) && (oi <= 0x2032))
			||((oi >= 0x2040) && (oi <= 0x2041))
			||((oi >= 0x2131) && (oi <= 0x2133))
			||((oi >= 0x2203) && (oi <= 0x2204))
			||((oi >= 0x2500) && (oi <= 0x2506))
			||(oi == 0x2200)
			)
		{//变量类
			*pbyclass	= 6;
			return TRUE;
		}
		else if((oi == 0x201E) 
			|| (oi == 0x202A)
			|| (oi == 0x202C)
			||((oi >= 0x2020) && (oi <= 0x2025)))
		{//变量类
			*pbyclass	= 8;
			return TRUE;
		}
		else if((oi >= 0x2100) && (oi <= 0x2104))
		{//区间统计
			*pbyclass	= 14;
			return TRUE;
		}
		else if((oi >= 0x2110) && (oi <= 0x2114))
		{//平均
			*pbyclass	= 15;
			return TRUE;
		}
		else if((oi >= 0x2120) && (oi <= 0x2124))
		{//极值
			*pbyclass	= 15;
			return TRUE;
		}
		else if((oi >= 0x2140) && (oi <= 0x2141))
		{//需量类
			*pbyclass	= 2;
			return TRUE;
		}
		else if((oi >= 0x2301) && (oi <= 0x2308))
		{//总加组类
			*pbyclass	= 23;
			return TRUE;
		}
		else if((oi >= 0x2401) && (oi <= 0x2408))
		{//脉冲计量类
			*pbyclass	= 12;
			return TRUE;
		}		
		break;
	case 3:
		if(oi <= 0x3008)
		{
			*pbyclass	= 24;
			return TRUE;
		}
		else if((oi <= 0x3030)
			||((oi >= 0x3100) && (oi <= 0x3101))
			||((oi >= 0x3104) && (oi <= 0x311C))
			||(oi == 0x3140)
			||((oi >= 0x3200) && (oi <= 0x3203))
			)
		{
			*pbyclass	= 7;
			return TRUE;
		}
		else if(((oi >= 0x3300) && (oi <= 0x3316))
			||(oi == 0x3320))
		{
			*pbyclass	= 8;
			return TRUE;
		}
		break;
	case 4:
		if((oi <= 0x4021)
			||((oi >= 0x4024) && (oi <= 0x4026))
			||(oi == 0x4030)||(oi == 0x4202)||(oi == 0x4204)
			||((oi >= 0x4100) && (oi <= 0x4117))
			||(oi == 0x4520)
			||(oi == 0x4401)
			)
		{
			*pbyclass	= 8;
			return TRUE;
		}
		else if(oi == 0x4022)
		{
			*pbyclass	= 6;
			return TRUE;
		}
		else if((oi == 0x4300)
			||((oi >= 0x4307) && (oi <= 0x4309))
			)
		{
			*pbyclass	= 19;
			return TRUE;
		}
		else if(oi == 0x4400)
		{
			*pbyclass	= 20;
			return TRUE;
		}
		else if((oi == 0x4500)||(oi == 0x4501))
		{
			*pbyclass	= 25;
			return TRUE;
		}
		else if((oi >= 0x4510) && (oi <= 0x4517))
		{
			*pbyclass	= 26;
			return TRUE;
		}
		break;
	case 5:
		if((oi <= 0x500B) || (oi == 0x5011))
		{
			*pbyclass	= 9;
			return TRUE;
		}
		break;
	case 6:
		if((oi == 0x6000)
			|| (oi == 0x6002)
			|| (oi == 0x6014)
			|| (oi == 0x6016)
			|| (oi == 0x6018)
			|| (oi == 0x601A)
			|| (oi == 0x601C)
			|| (oi == 0x601E)
			|| (oi == 0x6032)
			|| (oi == 0x6034)
			)
		{
			*pbyclass	= 11;
			return TRUE;
		}
		else if((oi == 0x6001)
			|| (oi == 0x6003)
			|| (oi == 0x6004)
			|| (oi == 0x6013)
			|| (oi == 0x6015)
			|| (oi == 0x6017)
			|| (oi == 0x6019)
			|| (oi == 0x601B)
			|| (oi == 0x601D)
			|| (oi == 0x601F)
			|| (oi == 0x6033)
			|| (oi == 0x6035)
			|| (oi == 0x6040)|| (oi == 0x6041)|| (oi == 0x6042)
			)
		{
			*pbyclass	= 8;
			return TRUE;
		}
		else if((oi == 0x6012)
			)
		{
			*pbyclass	= 10;
			return TRUE;
		}
		break;
	case 7:
		if((oi == 0x7000)
			|| (oi == 0x7010)
			|| (oi == 0x7012)
			|| (oi == 0x7100)
			|| (oi == 0x7101)
			)
		{
			*pbyclass	= 11;
			return TRUE;
		}
		else if((oi == 0x7001)
			|| (oi == 0x7011)
			|| (oi == 0x7013)

			)
		{
			*pbyclass	= 8;
			return TRUE;
		}
		break;
	case 8:
		if(((oi >= 0x8000)&&(oi <= 0x8002))
			||((oi >= 0x8100)&&(oi <= 0x8102))
			||((oi >= 0x8109)&&(oi <= 0x8110))
			)
		{
			*pbyclass	= 8;
			return TRUE;
		}
		else if(((oi >= 0x8003)&&(oi <= 0x8004))
			)
		{
			*pbyclass	= 11;
			return TRUE;
		}
		else if(((oi >= 0x8103)&&(oi <= 0x8108))
			)
		{
			*pbyclass	= 13;
			return TRUE;
		}
		break;
	case 0x0F:
		if(((oi >= 0xF000)&&(oi <= 0xF002))
			)
		{
			*pbyclass	= 18;
			return TRUE;
		}
		else if(oi == 0xF100)
		{
			*pbyclass	= 21;
			return TRUE;
		}
		else if((oi == 0xF101)||(oi == 0xF210)||(oi == 0xFF02))
		{
			*pbyclass	= 8;
			return TRUE;
		}
		else if(((oi >= 0xF200)&&(oi <= 0xF20C))
			)
		{
			*pbyclass	= 22;
			return TRUE;
		}
#if(FUNC_SAFETY_PROTECTION == YES)
		else if(oi == 0xF20E)
		{
			*pbyclass = 22;
			return TRUE;
		}
#endif
		else if(((oi >= 0xF300)&&(oi <= 0xF301))
			)
		{
			*pbyclass	= 17;
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

WORD get_max_OAD_reponse_len(DWORD dwOAD)
{
WORD wRc=27;	
BYTE byclass,byTmp;

	if(oi2class(HIWORD(dwOAD),&byclass))
	{
		switch(byclass)
		{
		case 1://电量
			if(LHBYTE(dwOAD) == 4)	//高精度
				byTmp	= 9;
			else
				byTmp	= 5;
			if(LLBYTE(dwOAD))
				wRc	= byTmp;			
			else
				wRc	= byTmp*5+2;			
			break;
		case 2://需量			
			byTmp	= 15;
			if(LLBYTE(dwOAD))
				wRc	= byTmp;
			else
				wRc	= byTmp*5+2;			
			break;
		case 3:
		case 4:
			byTmp	= 5;
			if(LLBYTE(dwOAD))
				wRc	= byTmp;
			else
				wRc	= byTmp*byclass+2;			
			break;
		case 5://谐波类
			byTmp	= 3;
			if(LLBYTE(dwOAD))
				wRc	= byTmp;
			else
				wRc	= byTmp*19+2;			
			break;
		case 8://参数变量接口类
			if(dwOAD == 0x20210200)
				wRc = 8;
			break;
		}
	}
	return wRc;
}

WORD get_csd_num_of_RCSD(BYTE *pRCSD)
{
WORD wNum;

	 gdw698Buf_Num(pRCSD,&wNum,TD12);
	 return wNum;
}

BOOL get_no_csd_of_RCSD(BYTE *pRCSD,WORD wNo,CSD *pcsd)
{
WORD wNum,wLen=0,wI;

	wLen	+= gdw698Buf_Num(pRCSD,&wNum,TD12);	
	for(wI=0;wI<wNum;wI++)
	{
		if(wNo == wI)
		{
			gdw698buf_CSD(&pRCSD[wLen],pcsd,TD12);
			return TRUE;
		}
		else
			wLen	+= objGetDataLen(dt_CSD,&pRCSD[wLen],MAX_NUM_NEST);
	}
	return FALSE;
}

BOOL get_no_csd_of_CSDS(BYTE *pCSDS,WORD wNo,CSD *pcsd)
{
WORD wNum=500,wLen=0,wI;

	for(wI=0;wI<wNum;wI++)
	{
		if(wNo == wI)
		{
			gdw698buf_CSD(&pCSDS[wLen],pcsd,TD12);
			return TRUE;
		}
		else
			wLen	+= objGetDataLen(dt_CSD,&pCSDS[wLen],MAX_NUM_NEST);
	}
	return FALSE;
}

BOOL get_no_road_of_ROADS(BYTE *pROADS,WORD wNo,ROAD *proad)
{
WORD wNum=500,wLen=0,wI;

	for(wI=0;wI<wNum;wI++)
	{
		if(wNo == wI)
		{
			gdw698buf_ROAD(&pROADS[wLen],proad,TD12);
			return TRUE;
		}
		else
			wLen	+= objGetDataLen(dt_ROAD,&pROADS[wLen],MAX_NUM_NEST);
	}
	return FALSE;
}


//如果曲线中有正有/反有电能，则抄表内;否则实时转曲线（山东主站瞬时量和电能曲线分开下发）
// BOOL is_need_cj_curv(CSD *pcsd)
// {
// BYTE i;
// WORD OI;

// 	CHECKPTR(pcsd,FALSE);

// 	if(GetUserType() != MAR_SHAN_DONG)
// 		return FALSE;
// 	for (i=0; i<pcsd->road.oadCols.nNum; i++)
// 	{
// 		OI = HIWORD(pcsd->road.oadCols.oad[i].value);

// 		if((OI == 0x0010)||(OI == 0x0020))
// 			return TRUE;
// 	}
// 	return FALSE;
// }

#endif
