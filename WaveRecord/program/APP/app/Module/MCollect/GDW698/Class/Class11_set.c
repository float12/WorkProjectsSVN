#include "AllHead.h"
#include "Dlt698_45.h"
//extern WORD JudgeTaskAuthority( BYTE Ch, eRequestMode eType, BYTE *pOI );
extern BOOL JudgeTimeTagValid(BYTE Ch);
#include "../GDW698.h"
#if(MD_BASEUSER	== MD_GDW698)
extern BOOL sys_GetTaskRunInfo(BYTE no,BYTE *pbyTaskNo,TTask698RunInfo *pRunInfo);
static BOOL _Class11_action_128(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);
static BOOL _Class11_action_127(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen);

WORD gdw698Buf_MP_basic(BYTE *pBuf,Meter_Basic *pBasic,TRANS_DIR td)
{
DWORD dwTMMP;  
WORD wRc=0,wTMMP;
BOOL bRc=FALSE;

	switch(td)
	{
	case TD12:
		if((pBuf[wRc] != dt_structure) || (pBuf[wRc+1] != 10))
			break;
		wRc	+= 2;
		if(pBuf[wRc++] != dt_TSA)
			break;
		wRc	+= gdw698buf_TSA(&pBuf[wRc],&pBasic->tsa,TD12,TRUE);
		if( (pBasic->tsa.type != 0)	//单地址
		   )
		{
			break;
		}
		// 波特率
		if(pBuf[wRc++] != dt_enum)
			break;
		if( ((pBuf[wRc] > 10)&&(pBuf[wRc] < 13))	//can总线速率（13-16),485及载波小于10
		    ||(pBuf[wRc] > 16) 
		  )
			break;
		pBasic->baud	= pBuf[wRc++];
		// 规约类型
		if(pBuf[wRc++] != dt_enum)
			break;
		if(pBuf[wRc] > 5)
			break;
		pBasic->protocol	= pBuf[wRc++];
		// 端口
		if(pBuf[wRc++] != dt_OAD)
			break;
		wRc	+= gdw698buf_OAD(&pBuf[wRc],&dwTMMP,TD12);
        //判断端口合法性
        if( //(dwTMMP == 0xF2010200)  //485
		   //||
           (dwTMMP == 0xF2010201)
           ||(dwTMMP == 0xF2090200) //载波 
           ||(dwTMMP == 0xF2090201)   
        //    ||(dwTMMP == 0xF20B0200) //蓝牙
        //    ||(dwTMMP == 0xF20B0201)  
		   ||(dwTMMP == 0xF2020200) //红外
           ||(dwTMMP == 0xF2020201)  
           ||(dwTMMP == 0xF2210200) //CAN
           ||(dwTMMP == 0xF2210201))
        {
            pBasic->port.value = dwTMMP;
        }
        else
        {
            break;
        }
		
		if((dwTMMP == 0xF2210200)||(dwTMMP == 0xF2210201)) //can总线速率（13-16)
        {
			if( (pBasic->baud < 13) || (pBasic->baud > 16) )
			{
				break;
			}
		}
		else if((dwTMMP == 0xF2020200)||(dwTMMP == 0xF2020201)) //红外固定1200
        {
			if(pBasic->baud != 2)
			{
				break;
			}
		}
		else
		{
			if( (pBasic->baud < 2) 
			|| (pBasic->baud > 10) 
			||(pBasic->baud == 5) //不支持7200
			)
			{
				break;
			}
		}
		
		// 通信密码
		if(pBuf[wRc++] != dt_octet_str)
			break;
		pBasic->pwdLen	= pBuf[wRc];
		if(pBasic->pwdLen > OBJ_PWD_LEN)
			pBasic->pwdLen = OBJ_PWD_LEN;
		memcpy(pBasic->password,&pBuf[wRc+1],pBasic->pwdLen);
		wRc	+= pBuf[wRc]+1;
		// 费率个数
		if(pBuf[wRc++] != dt_unsigned)
			break;
		pBasic->rateNum	= pBuf[wRc++];			
		// 用户类型
		if(pBuf[wRc++] != dt_unsigned)
			break;
		pBasic->userType	= pBuf[wRc++];
		// 接线方式
		if(pBuf[wRc++] != dt_enum)
			break;
		if(pBuf[wRc] > 3)
			break;
		pBasic->conType	= pBuf[wRc++];
		// 额定电压
		if(pBuf[wRc++] != dt_long_unsigned)
			break;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD12);
                pBasic->stVolt  = wTMMP;						
		// 额定电流
		if(pBuf[wRc++] != dt_long_unsigned)
			break;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD12);
                pBasic->stI     = wTMMP;						
		bRc	= TRUE;
		break;
	case TD21:
		pBuf[wRc++] = dt_structure;
		pBuf[wRc++] = 10;
		pBuf[wRc++] = dt_TSA;
		wRc	+= gdw698buf_TSA(&pBuf[wRc],&pBasic->tsa,TD21,TRUE);
		// 波特率
		pBuf[wRc++] = dt_enum;
		pBuf[wRc++]	= pBasic->baud;
		// 规约类型
		pBuf[wRc++] = dt_enum;
		pBuf[wRc++]	= pBasic->protocol;
		// 端口
		pBuf[wRc++] = dt_OAD;
                dwTMMP  = pBasic->port.value;
		wRc	+= gdw698buf_OAD(&pBuf[wRc],&dwTMMP,TD21);
		// 通信密码
		pBuf[wRc++] = dt_octet_str;
		if(pBasic->pwdLen > OBJ_PWD_LEN)
			pBasic->pwdLen = OBJ_PWD_LEN;
		pBuf[wRc]	= pBasic->pwdLen;
		memcpy(&pBuf[wRc+1],pBasic->password,pBasic->pwdLen);
		wRc	+= pBuf[wRc]+1;
		// 费率个数
		pBuf[wRc++] = dt_unsigned;
		pBuf[wRc++]	= pBasic->rateNum;			
		// 用户类型
		pBuf[wRc++] = dt_unsigned;
		pBuf[wRc++]	= pBasic->userType;
		// 接线方式
		pBuf[wRc++] = dt_enum;
		pBuf[wRc++]	= pBasic->conType;
		// 额定电压
		pBuf[wRc++] = dt_long_unsigned;
                wTMMP   = pBasic->stVolt;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD21);						
		// 额定电流
		pBuf[wRc++] = dt_long_unsigned;
                wTMMP   = pBasic->stI;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD21);						
		bRc	= TRUE;
		break;
	}
	if(!bRc)
		wRc	= 0;
	return wRc;
}

WORD gdw698Buf_MP_Extended(BYTE *pBuf,Meter_Extended *pExtend,TRANS_DIR td)
{
WORD wRc=0,wTMMP;
BOOL bRc=FALSE;

	switch(td)
	{
	case TD12:
		if((pBuf[wRc] != dt_structure) || (pBuf[wRc+1] != 4))
			break;
		wRc	+= 2;
		//采集器地址
		if(pBuf[wRc++] != dt_TSA)
			break;		
		wRc	+= gdw698buf_TSA(&pBuf[wRc],&pExtend->add,TD12,TRUE);
		//资产号      octet-string，
		if(pBuf[wRc++] != dt_octet_str)
			break;
		pExtend->astLen	= pBuf[wRc];
		if(pExtend->astLen > sizeof(pExtend->assetNo))
			pExtend->astLen = sizeof(pExtend->assetNo);
		memcpy(pExtend->assetNo,&pBuf[wRc+1],pExtend->astLen);
		wRc	+= pBuf[wRc]+1;
		// PT
		if(pBuf[wRc++] != dt_long_unsigned)
			break;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD12);
                pExtend->PT     = wTMMP;						
		// CT
		if(pBuf[wRc++] != dt_long_unsigned)
			break;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD12);	
                pExtend->CT     = wTMMP;					
		bRc	= TRUE;
		break;
	case TD21:
		pBuf[wRc++] = dt_structure;
		pBuf[wRc++] = 4;
		//采集器地址
		pBuf[wRc++] = dt_TSA;
		wRc	+= gdw698buf_TSA(&pBuf[wRc],&pExtend->add,TD21,TRUE);
		//资产号      octet-string，
		pBuf[wRc++] = dt_octet_str;
		
		if(pExtend->astLen > sizeof(pExtend->assetNo))
			pExtend->astLen = sizeof(pExtend->assetNo);
		pBuf[wRc++]	= pExtend->astLen;
		memcpy(&pBuf[wRc],pExtend->assetNo,pExtend->astLen);
		wRc	+= pExtend->astLen;
		// PT
		pBuf[wRc++] = dt_long_unsigned;
                wTMMP   = pExtend->PT;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD21);						
		// CT
		pBuf[wRc++] = dt_long_unsigned;
                wTMMP   = pExtend->CT;
		wRc	+= gdw698Buf_long_unsigned(&pBuf[wRc],&wTMMP,TD21);						
		bRc	= TRUE;
		break;
	}
	if(!bRc)
		wRc	= 0;
	return wRc;
}

WORD gdw698Buf_MP_Annex(BYTE *pBuf,Meter_Annex *pAnnex,TRANS_DIR td)
{
DWORD dwTMMP;  
WORD wRc=0,i,tmp;
BYTE num;
BOOL bRc=FALSE;

	switch(td)
	{
	case TD12:
		if(pBuf[wRc++] != dt_array) 
			break;
		num	= pBuf[wRc++];
		pAnnex->nNum	= num;
		if(pAnnex->nNum > OBJ_MAX_ANNEX_NUM)
			return FALSE;
			//pAnnex->nNum = OBJ_MAX_ANNEX_NUM;
		for(i=0;i<pAnnex->nNum;i++)
		{
			if((pBuf[wRc] != dt_structure) || (pBuf[wRc+1] != 2))
				break;
			wRc	+= 2;
			if(pBuf[wRc++] != dt_OAD)
				break;
			wRc	+= gdw698buf_OAD(&pBuf[wRc],&dwTMMP,TD12);
                        pAnnex->annexObj[i].oad.value   = dwTMMP;
			tmp	= objGetDataLen((OBJ_DATA_TYPE)pBuf[wRc],&pBuf[wRc+1],MAX_NUM_NEST);
			if(tmp > OBJ_MAX_ANNEX_LEN)
				return FALSE;
				//break;
			tmp	+= 1;
			pAnnex->annexObj[i].nLen	= (BYTE)tmp;
			memcpy(pAnnex->annexObj[i].value,&pBuf[wRc],tmp);
			wRc	+= tmp;
		}
		if(i >= pAnnex->nNum)
			bRc	= TRUE;
		break;
	case TD21:
		pBuf[wRc++] = dt_array; 
		if(pAnnex->nNum > OBJ_MAX_ANNEX_NUM)
			return FALSE;
			//pAnnex->nNum = OBJ_MAX_ANNEX_NUM;
		pBuf[wRc++]	= pAnnex->nNum;
		for(i=0;i<pAnnex->nNum;i++)
		{
			pBuf[wRc++] = dt_structure;
			pBuf[wRc++] = 2;
			pBuf[wRc++] = dt_OAD;
                        dwTMMP  = pAnnex->annexObj[i].oad.value;
			wRc	+= gdw698buf_OAD(&pBuf[wRc],&dwTMMP,TD21);
			tmp	= objGetDataLen((OBJ_DATA_TYPE)pAnnex->annexObj[i].value[0],&pAnnex->annexObj[i].value[1],MAX_NUM_NEST);
			if((BYTE)(tmp-1) >= OBJ_MAX_ANNEX_LEN)
				return FALSE;
				//pBuf[wRc++] = 0;
			else
			{
				tmp	+= 1;
				memcpy(&pBuf[wRc],&pAnnex->annexObj[i].value[0],tmp);
				wRc	+= tmp;
			}	
		}
		bRc	= TRUE;
		break;
	}
	if(!bRc)
		wRc	= 0;
	return wRc;
}

WORD gdw698Buf_meter_cfg(BYTE *pBuf,TOad60010200 *pcfg,TRANS_DIR td)
{
WORD offset=0,tmp,wTMMP;

	switch(td)
	{
	case TD12:
		if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 4))	
			return 0;
		offset	+= 2;
		//序号
		if(pBuf[offset++] != dt_long_unsigned)	
			return 0;
		offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD12);		
		pcfg->nIndex    = wTMMP;
		if((pcfg->nIndex == 0)||( pcfg->nIndex>= MAX_MP_NUM))
			return 0;
		//基本信息
		tmp	= gdw698Buf_MP_basic(&pBuf[offset],&pcfg->basic,TD12);
		if(tmp == 0)
			return 0;
		offset	+= tmp;
		//扩展信息		
		tmp	= gdw698Buf_MP_Extended(&pBuf[offset],&pcfg->extend,TD12);
		if(tmp == 0)
			return 0;
		//附录信息
		offset	+= tmp;		
		tmp	= gdw698Buf_MP_Annex(&pBuf[offset],&pcfg->annex,TD12);
		if(tmp == 0)
			return 0;
		offset	+= tmp;	
		break;
	case TD21:
		pBuf[offset++] = dt_structure;
		pBuf[offset++] = 4;
		//序号
		pBuf[offset++] = dt_long_unsigned;	
                wTMMP   = pcfg->nIndex;
		offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD21);		
		//基本信息
		offset	+= gdw698Buf_MP_basic(&pBuf[offset],&pcfg->basic,TD21);
		//扩展信息
		offset	+= gdw698Buf_MP_Extended(&pBuf[offset],&pcfg->extend,TD21);		
		//附录信息		
		offset	+= gdw698Buf_MP_Annex(&pBuf[offset],&pcfg->annex,TD21);
		break;
	}
	return offset;
}

static WORD _get_cj_cfg(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext)
{
WORD wLen=0,i,wOffset2=0,wNum=0,wNumOffset=1;
WORD wMaxNum=GetAllMeterSum(),wCfgLen;
TMPFlagsCtrl *pMPFlag=pGetMPFlag();
TOad60010200 cfg;
BYTE bbuf[1024];
BYTE *pBuf=bbuf;

	*bNext	= FALSE;
	if(NULL == pBuf)
		return 0;
	if(LLBYTE(oadd))
	{//召单个配置
		wMaxNum	= 1;
		wOffset	= LLBYTE(oadd)-1;
	}
	else
	{
		if(wMaxNum >= wOffset)
			wMaxNum	-= wOffset;
		else
			wMaxNum	= 0;
		pData[wLen++]	= 1;
		wNumOffset		= wLen;
		wLen++;
	}	
	for(i=1;i<MAX_MP_NUM;i++)
	{
		if(pMPFlag->Flag_Valid[i>>3] & (1<<(i&0x07)))		
		{
			if(wOffset2 >= wOffset)
			{
				GetMP698Para(i,&cfg);				
				wCfgLen	= gdw698Buf_meter_cfg(pBuf,&cfg,TD21);
				if((wLen+wCfgLen) > wMaxBufLen)
				{
					*bNext	= TRUE;
					break;
				}	
				memcpy(&pData[wLen],pBuf,wCfgLen);
				wLen	+= wCfgLen;
				wNum++;
				if((wNum >= wMaxNum)||LLBYTE(oadd))
					break;
				// if(wNum >= 10)
				// {
				// 	*bNext	= TRUE;
				// 	break;
				// }
			}
			wOffset2++;
		}
	}	
	if(LLBYTE(oadd) == 0)//召合相
		pData[wNumOffset]	= (BYTE)wNum;
	return wLen;
}

DAR _update_meter_cfg1(BYTE *pBuf)
{
TOad60010200 mpcfg={0};	
WORD offset=0,tmp,no;
	
	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 2))	
		return dar_object_unavailable;
	offset	+= 2;
	if(pBuf[offset++] != dt_long_unsigned)	
		return dar_object_unavailable;
	//序号
	offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&no,TD12);
	if ((no>=MAX_MP_NUM)||(no==0))//序号不对
	{
		return dar_object_unavailable;
	}
	GetMP698Para(no,&mpcfg);
	if(mpcfg.nIndex != no)
		return dar_object_unavailable;
	mpcfg.nIndex	= no;
	//基本信息
	tmp	= gdw698Buf_MP_basic(&pBuf[offset],&mpcfg.basic,TD12);
	if(tmp == 0)
		return dar_object_unavailable;
	offset	+= tmp;
	add_mp_cfg(mpcfg.nIndex,&mpcfg);
	return dar_success;
}

DAR _update_meter_cfg2(BYTE *pBuf)
{
TOad60010200 mpcfg={0};	
WORD offset=0,tmp,no;
	
	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 3))	
		return dar_object_unavailable;
	offset	+= 2;
	if(pBuf[offset++] != dt_long_unsigned)	
		return dar_object_unavailable;
	//序号
	offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&no,TD12);
	GetMP698Para(no,&mpcfg);
	if ((no>=MAX_MP_NUM)||(no==0))//序号不对
	{
		return dar_object_unavailable;
	}
	if(mpcfg.nIndex != no)
		return dar_object_unavailable;
	mpcfg.nIndex	= no;
	tmp	= gdw698Buf_MP_Extended(&pBuf[offset],&mpcfg.extend,TD12);
	if(tmp == 0)
		return dar_object_unavailable;
	offset	+= tmp;
	//附录信息
	tmp	= gdw698Buf_MP_Annex(&pBuf[offset],&mpcfg.annex,TD12);
	if(tmp == 0)
		return dar_object_unavailable;
	offset	+= tmp;
	add_mp_cfg(mpcfg.nIndex,&mpcfg);
	return dar_success;
}

DAR _update_meter_cfg3(BYTE *pBuf)
{
	TOad60010200 mpcfg={0};	
	WORD offset=0,tmp,no;
	TSA sa;
	
	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 2))	
		return dar_object_unavailable;
	offset	+= 2;
	if(pBuf[offset++] != dt_TSA)	
		return dar_object_unavailable;

	//地址
	offset	+= gdw698buf_TSA(&pBuf[offset],&sa,TD12,TRUE);
	
	for(no=1;no<MAX_MP_NUM;no++)
	{
		if(MP_IsValid(no))	
		{
			GetMP698Para(no,&mpcfg);
			if( comp_TSA_addr(&sa,&mpcfg.basic.tsa) == 0 )
			{	
				//附录信息		
				gdw698Buf_MP_Annex(&pBuf[offset],&mpcfg.annex,TD12);			
				break;
			}
		}
	}	
	
	if(no == MAX_MP_NUM)
	{
		return dar_scope_of_access_violated;
	}	
	
	//序号
	add_mp_cfg(mpcfg.nIndex,&mpcfg);
	return dar_success;
}

DAR _del_meter(BYTE *pBuf)
{
	WORD offset=0,no;
	TOad60010200 mpcfg = {0};
	
	if(pBuf[offset++] != dt_long_unsigned)
		return dar_object_unavailable;
	//序号
	offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&no,TD12);

	GetMP698Para(no,&mpcfg);
	if ((no>=MAX_MP_NUM)||(no==0))//序号不对
	{
		return dar_object_unavailable;
	}
	if(mpcfg.nIndex != no)
	{
		return dar_scope_of_access_violated;
	}	
	
	del_mp_cfg(no);
	return dar_success;
}

DAR _del_meter_bycfg1(BYTE *pBuf)
{
TOad60010200 mpcfg={0};	
Meter_Basic basiccfg={0};	
WORD offset=0,tmp,no;
	//基本信息
	tmp	= gdw698Buf_MP_basic(&pBuf[offset],&basiccfg,TD12);
	if(tmp == 0)
		return dar_object_unavailable;
	for(no=1;no<MAX_MP_NUM;no++)
	{
		if(MP_IsValid(no))	
		{
			GetMP698Para(no,&mpcfg);
			if(memcmp(&mpcfg.basic,&basiccfg,sizeof(Meter_Basic)) == 0)
			{
				del_mp_cfg(no);
				break;
			}
		}
	}	
	if(no == MAX_MP_NUM)
	{
		return dar_scope_of_access_violated;
	}
	
	return dar_success;
}

DAR _del_meter_byport(BYTE *pBuf)
{
TOad60010200 mpcfg={0};	
TSA sa;
OAD oadd;
DWORD dwTMMP;
WORD offset=0,no;
	
	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 2))	
		return dar_object_unavailable;
	offset	+= 2;
	if(pBuf[offset++] != dt_TSA)
		return dar_object_unavailable;
	offset	+= gdw698buf_TSA(&pBuf[offset],&sa,TD12,TRUE);
	if(pBuf[offset++] != dt_OAD)
		return dar_object_unavailable;
	offset	+= gdw698buf_OAD(&pBuf[offset],&dwTMMP,TD12);
    oadd.value      = dwTMMP;
	for(no=1;no<MAX_MP_NUM;no++)
	{
		if(MP_IsValid(no))	
		{
			GetMP698Para(no,&mpcfg);
			if((comp_TSA_addr(&sa,&mpcfg.basic.tsa) == 0) 
				&& (oadd.value == mpcfg.basic.port.value))
			{	
				del_mp_cfg(no);
				break;
			}
		}
	}	
	
	if(no == MAX_MP_NUM)
	{
		return dar_scope_of_access_violated;
	}
	
	return dar_success;
}

DAR _del_all_meters(BYTE *pBuf)
{
WORD no;

	if(pBuf[0] != dt_null)
		return dar_type_unmatched;
	for(no=1;no<MAX_MP_NUM;no++)
	{
		if(MP_IsValid(no))	
			del_mp_cfg(no);
	}	
	return dar_success;
}

WORD gdw698Buf_general_cjscheme(BYTE *pBuf,TOad60150200 *pcjsch,TRANS_DIR td)
{
WORD offset=0,i,num,wTMMP,wCSDDatalen=0,wTmmp2;
BOOL bRc=FALSE;

	switch(td)	
	{
	case TD12:
		memset(pcjsch,0,sizeof(TOad60150200));
		if((pBuf[offset] != dt_structure)||(pBuf[offset+1] != 6))
			break;
		offset	+= 2;
		//方案编号  unsigned，
		if(pBuf[offset++] != dt_unsigned)
			break;
		offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->byShemeNo,TD12);	
		//存储深度  long-unsigned，
		if(pBuf[offset++] != dt_long_unsigned)
			break;
		offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD12);
		pcjsch->wSaveDeep = wTMMP;
		if(pcjsch->wSaveDeep==0)
		{
			break;
		}
		//采集方式  structure{采集类型  unsigned，  采集内容  Data}，
		if((pBuf[offset] != dt_structure)||(pBuf[offset+1] != 2))
			break;
		offset	+= 2;
		if(pBuf[offset++] != dt_unsigned)
			break;
		offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->byCJType,TD12);	
		if(pcjsch->byCJType > 4)
			break;
		switch(pcjsch->byCJType)
		{
		case 0: //采集当前数据
		case 2: //按冻结时标采集
			offset	+= objGetDataLen((OBJ_DATA_TYPE)pBuf[offset],&pBuf[offset+1],MAX_NUM_NEST)+1;
			break;
		case 1: //采集上第N次
			if(pBuf[offset++] != dt_unsigned)
				break;
			offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->CJContent._N,TD12);	
			break;		
		case 3: //按时标间隔采集
			if(pBuf[offset++] != dt_TI)
				break;
			offset	+= gdw698buf_TI(&pBuf[offset],&pcjsch->CJContent.tii,TD12);	
			break;
		case 4: //补抄
			if((pBuf[offset] != dt_structure) ||(pBuf[offset+1] != 2))
				break;
			offset += 2;
			if(pBuf[offset++] != dt_TI)
				break;
			offset	+= gdw698buf_TI(&pBuf[offset],&pcjsch->CJContent.retryMeter.tii,TD12);	
			if(pBuf[offset++] != dt_long_unsigned)
				break;
			offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD12);	
                        pcjsch->CJContent.retryMeter.lastNums   = wTMMP;
			break;
		}
		//记录列选择  array CSD，
		if(pBuf[offset++] != dt_array)
			break;
		offset	+= gdw698Buf_Num(&pBuf[offset],&num,TD12);
		wCSDDatalen	= 0;
		for(i=0;i<num;i++)
		{
			if(pBuf[offset++] != dt_CSD)
				break;
			wTmmp2	= objGetDataLen(dt_CSD,&pBuf[offset],MAX_NUM_NEST);
			if((WORD)(wCSDDatalen+wTmmp2) < sizeof(pcjsch->csdBuf))
			{
				memcpy(&pcjsch->csdBuf[wCSDDatalen],&pBuf[offset],wTmmp2);
				wCSDDatalen	+= wTmmp2;
				pcjsch->byColNum++;
			}	
			offset	+= wTmmp2;			
		}
		//电能表集合  MS，
		if(pBuf[offset++] != dt_MS)
			break;
		offset	+= gdw698Buf_MS(&pBuf[offset],&pcjsch->objMS,TD12);
		//不支持8，9
		if(pcjsch->objMS.choice>7)
		{
			break;
		}
		//存储时标选择 enum
		if(pBuf[offset++] != dt_enum)
			break;
		pcjsch->bySaveFlag	= pBuf[offset++];
		if(pcjsch->bySaveFlag > 7)
			break;
		bRc	= TRUE;
		break;
	case TD21:
		pBuf[offset++] = dt_structure;
		pBuf[offset++] = 6;
		
		//方案编号  unsigned，
		pBuf[offset++] = dt_unsigned;
		offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->byShemeNo,TD21);	
		//存储深度  long-unsigned，
		pBuf[offset++] = dt_long_unsigned;
                wTMMP   = pcjsch->wSaveDeep;
		offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD21);
		//采集方式  structure{采集类型  unsigned，  采集内容  Data}，
		pBuf[offset++] = dt_structure;
		pBuf[offset++] = 2;
		pBuf[offset++] = dt_unsigned;
		offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->byCJType,TD21);	
		switch(pcjsch->byCJType)
		{
		case 0: //采集当前数据
		case 2: //按冻结时标采集
			pBuf[offset++] = dt_null;			
			break;
		case 1: //采集上第N次
			pBuf[offset++] = dt_unsigned;
			offset	+= gdw698Buf_unsigned(&pBuf[offset],&pcjsch->CJContent._N,TD21);	
			break;		
		case 3: //按时标间隔采集
			pBuf[offset++] = dt_TI;
			offset	+= gdw698buf_TI(&pBuf[offset],&pcjsch->CJContent.tii,TD21);	
			break;
		case 4: //补抄
			pBuf[offset++] = dt_structure;
			pBuf[offset++] = 2;
			pBuf[offset++] = dt_TI;
			offset	+= gdw698buf_TI(&pBuf[offset],&pcjsch->CJContent.retryMeter.tii,TD21);	
			pBuf[offset++] = dt_long_unsigned;
                        wTMMP   = pcjsch->CJContent.retryMeter.lastNums;
			offset	+= gdw698Buf_long_unsigned(&pBuf[offset],&wTMMP,TD21);	
			break;
		}
		//记录列选择  array CSD，
		pBuf[offset++] = dt_array;
		if(pcjsch->byColNum > 100)
			pcjsch->byColNum = 100;
		num = pcjsch->byColNum;
		offset	+= gdw698Buf_Num(&pBuf[offset],&num,TD21);
		wCSDDatalen	= 0;
		for(i=0;i<num;i++)
		{
			pBuf[offset++] = dt_CSD;
			wTmmp2	= objGetDataLen(dt_CSD,&pcjsch->csdBuf[wCSDDatalen],MAX_NUM_NEST);
			memcpy(&pBuf[offset],&pcjsch->csdBuf[wCSDDatalen],wTmmp2);
			offset	+= wTmmp2;
			wCSDDatalen	+= wTmmp2;			
		}
		//电能表集合  MS，
		pBuf[offset++] = dt_MS;
		offset	+= gdw698Buf_MS(&pBuf[offset],&pcjsch->objMS,TD21);
		//存储时标选择 enum
		pBuf[offset++] = dt_enum;
		pBuf[offset++]	= pcjsch->bySaveFlag;
		bRc	= TRUE;
		break;
	}
	if(!bRc)
		offset	= 0;
	return offset;
}
extern BYTE api_GetSchHaveNum(BYTE sch_type);
// DAR add_general_schemes(BYTE *pBuf)
// {
// 	WORD offset = 0, i, num, offsetT = 0;
// 	TOad60150200 sch = {0};
// 	BOOL bRc = FALSE;
// 	BYTE byIndex, Neednum = 0;

// 	if(pBuf[offset++] != dt_array)
// 		return dar_object_unavailable;
// 	offset += gdw698Buf_Num(&pBuf[offset], &num, TD12);
// 	offsetT = offset;
// 	for(i = 0; i < num; i++)   // 报文内容都合理才允许设置
// 	{
// 		if(!gdw698Buf_general_cjscheme(&pBuf[offsetT], &sch, TD12))
// 		{
// 			return dar_object_unavailable;
// 		}
// 		if(!schNo2Index(sch.byShemeNo, TT_DATA_CJ, &byIndex))   // 计算是否需要新增空间
// 		{
// 			Neednum++;
// 		}
// 		offsetT += objGetDataLen((OBJ_DATA_TYPE)pBuf[offsetT], &pBuf[offsetT + 1], MAX_NUM_NEST) + 1;
// 	}
// 	if(Neednum > api_GetSchHaveNum(TT_DATA_CJ))
// 	{
// 		return dar_object_unavailable;
// 	}
// 	for(i = 0; i < num; i++)
// 	{
// 		if(gdw698Buf_general_cjscheme(&pBuf[offset], &sch, TD12))
// 		{
// 			add_cjscheme(TT_DATA_CJ, sch.byShemeNo, (BYTE *)&sch);
// 			bRc = TRUE;
// 		}
// 		offset += objGetDataLen((OBJ_DATA_TYPE)pBuf[offset], &pBuf[offset + 1], MAX_NUM_NEST) + 1;
// 	}
// 	if(bRc)
// 		return dar_success;
// 	else
// 		return dar_type_unmatched;
// }

// static WORD _get_scheme_6014(BYTE scheme_no,BYTE *pBuf)
// {
// 	TOad60150200 cfg;
// 	if(!sys_cj_scheme_read(TT_DATA_CJ,scheme_no,(BYTE*)&cfg))
// 		return 0;
// 	if(cfg.valid_flag != 0x5AA5)
// 		return 0;
// 	return gdw698Buf_general_cjscheme(pBuf,&cfg,TD21);
// }

// static WORD _get_scheme(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext)
// {
// WORD wLen=0,i,wOffset2=0,wNum=0,wNumOffset=1;
// WORD wMaxNum=0,wCfgLen;
// BYTE bbuf[1024];
// BYTE *pBuf=bbuf;

// 	*bNext	= FALSE;
// 	if(HIWORD(oadd) == 0x6014)
// 		wMaxNum	= get_scheme_num(1);
// 	else if(HIWORD(oadd) == 0x6016)
// 		wMaxNum	= get_scheme_num(2);
// 	else if(HIWORD(oadd) == 0x6018)
// 		wMaxNum = get_scheme_num(3);
// 	else if(HIWORD(oadd) == 0x601C)
// 		wMaxNum	= get_scheme_num(4);
// 	if(NULL == pBuf)
// 		return 0;
// 	if(LLBYTE(oadd))
// 	{//召单个配置
// 		wMaxNum	= 1;
// 		wOffset	= LLBYTE(oadd)-1;
// 	}
// 	else
// 	{
// 		if(wMaxNum >= wOffset)
// 			wMaxNum	-= wOffset;
// 		else
// 			wMaxNum	= 0;		
// 		pData[wLen++]	= dt_array;
// 		wNumOffset	= wLen;
// 		wLen++;
// 	}
// 	for(i=0;i<256;i++)//限制byte而不是个数
// 	{
// 		wCfgLen	= 0;
// 		if(HIWORD(oadd) == 0x6014)
// 		{
// 			wCfgLen = _get_scheme_6014((BYTE)i,pBuf);
// 			if(wCfgLen == 0)
// 				continue;
// 		}
// 		else continue;
// 		if(wOffset2 < wOffset)
// 		{
// 			wOffset2++;
// 			continue;
// 		}
// 		if((wLen+wCfgLen) > wMaxBufLen)
// 		{
// 			*bNext	= TRUE;
// 			break;
// 		}		
// 		memcpy(&pData[wLen],pBuf,wCfgLen);
// 		wLen	+= wCfgLen;
// 		wNum++;
// 		if((wNum >= wMaxNum)||LLBYTE(oadd))
// 			break;
// 		// if(wNum >= 10)
// 		// {
// 		// 	*bNext	= TRUE;
// 		// 	break;
// 		// }		
// 	}
// 	if(LLBYTE(oadd) == 0)//召合相
// 		pData[wNumOffset]	= (BYTE)wNum;	
// 	return wLen;
// }

// WORD gdw698Buf_TaskRunInfo(BYTE *pData,BYTE byTaskNo,TTask698RunInfo *pRunInfo)
// {
// WORD wLen=0;
// TTime tTime;
// DATETIME_S dtime;
// 	TOad60130200 *pCfg=pGet698NoTaskCfg(byTaskNo);
	
// 	if((NULL!=pCfg) && (pCfg->byTaskType==TT_TRANS_CJ))
// 	{
// 		if((pRunInfo->byStatus == 1) && (pRunInfo->total_num == pRunInfo->success_num))
// 		{
// 			GetTime(&tTime);
// 			if((DWORD)(Time2Min(&tTime) - pRunInfo->success_time) >= 2)
// 			{
// 				pRunInfo->byStatus = 2;
// 			}
// 		}
// 	}
// 	pData[wLen++]	= dt_structure;
// 	pData[wLen++]	= 8;
// 	pData[wLen++]	= dt_unsigned;
// 	wLen	+= gdw698Buf_unsigned(&pData[wLen],&byTaskNo,TD21);
// 	pData[wLen++]	= dt_enum;
// 	pData[wLen++]	= pRunInfo->byStatus;
// 	pData[wLen++]	= dt_datetime_s;
// 	Min2Time(pRunInfo->startTask_time[0],&tTime);
// 	DATETIME_S_Time(&dtime,&tTime,TD21);
// 	wLen	+= gdw698buf_DATETIME_S(&pData[wLen],&dtime,TD21);
    
// 	if((NULL!=pCfg) && (pCfg->byTaskType==TT_TRANS_CJ))
// 	{
// 		if((pRunInfo->success_time == 0) || (pRunInfo->byStatus == 1))
// 			pData[wLen++] = dt_null;
// 		else 
// 		{
// 			pData[wLen++]	= dt_datetime_s;
// 			Min2Time(pRunInfo->success_time,&tTime);
// 			DATETIME_S_Time(&dtime,&tTime,TD21);
// 			wLen	+= gdw698buf_DATETIME_S(&pData[wLen],&dtime,TD21);
// 		}
// 	}
// 	else
// 	{
// 		pData[wLen++]	= dt_datetime_s;
// 		Min2Time(pRunInfo->success_time,&tTime);
// 		DATETIME_S_Time(&dtime,&tTime,TD21);
// 		wLen	+= gdw698buf_DATETIME_S(&pData[wLen],&dtime,TD21);
// 	}
// 	pData[wLen++]	= dt_long_unsigned;
// 	wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&pRunInfo->total_num,TD21);
// 	pData[wLen++]	= dt_long_unsigned;
// 	wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&pRunInfo->success_num,TD21);
// 	pData[wLen++]	= dt_long_unsigned;
// 	wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&pRunInfo->tx_packet_num,TD21);
// 	pData[wLen++]	= dt_long_unsigned;
// 	wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&pRunInfo->rx_packet_num,TD21);
// 	return wLen;
// }

// static WORD _get_cj_info(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext)
// {
// WORD wLen=0;
// WORD wNumOffset = 0;
// WORD wTaskLen = 0;
// WORD wValidNum = 0;
// WORD wMaxNum = 0;
// BYTE i,byTaskNo,byTaskNum=0;
// TOad60130200 *pCfg;
// TTask698RunInfo RunInfo;
// BYTE bbuf[1024];
// BYTE *pBuf=bbuf;

// 	*bNext = FALSE;
// 	//获取任务总数
// 	for (i=0;i<MAX_698_TASK_NUM;i++)
// 	{
// 		pCfg	= pGet698NoTaskCfg(i);
// 		if(pCfg->validflag != 0x5AA5)
// 			continue;
// 		if((pCfg->byTaskType != TT_DATA_CJ) 
// 			&& (pCfg->byTaskType != TT_EVENT_CJ))
// 		{
// 			continue;
// 		}
// 		wMaxNum ++;
// 	}

// 	if(LLBYTE(oadd))
// 	{//召单个配置
// 		wMaxNum	= 1;
// 		wOffset	= LLBYTE(oadd)-1;
// 	}
// 	else
// 	{
// 		if(wMaxNum >= wOffset)
// 			wMaxNum	-= wOffset;
// 		else
// 			wMaxNum	= 0;		
// 		pData[wLen++]	= dt_array;
// 		wNumOffset	= wLen;
// 		wLen++;
// 	}

// 	for(i=0;i<MAX_698_TASK_NUM;i++)
// 	{
// 		pCfg	= pGet698NoTaskCfg(i);
// 		if(pCfg->validflag != 0x5AA5)
// 			continue;
// 		if((pCfg->byTaskType != TT_DATA_CJ) 
// 			&& (pCfg->byTaskType != TT_EVENT_CJ))
// 		{
// 			continue;
// 		}
// 		wValidNum++;
// 		if(wValidNum <= wOffset)
// 			continue;		
// 		if(!sys_GetTaskRunInfo(i,&byTaskNo,&RunInfo))
// 		{
// 			memset(&RunInfo,0,sizeof(TTask698RunInfo));
// 			byTaskNo	= 0;
// 		}		
// 		wTaskLen = gdw698Buf_TaskRunInfo(pBuf,byTaskNo,&RunInfo);
// 		if ((wLen+wTaskLen)>wMaxBufLen)
// 		{
// 			*bNext = TRUE;
// 			break;
// 		}
// 		memcpy(&pData[wLen],pBuf,wTaskLen);
// 		wLen +=wTaskLen;
// 		byTaskNum++;
// 		if(byTaskNum >= wMaxNum)
// 			break;
// 		// if ((byTaskNum >=30))
// 		// {
// 		// 	*bNext = TRUE;
// 		// 	break;
// 		// }		
// 	}
// 	if(LLBYTE(oadd) == 0)
// 		pData[wNumOffset]	= (BYTE)byTaskNum;
// 	return wLen;
// }

WORD _Class11_2_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext)
{
WORD oi=HIWORD(oadd);
WORD wLen=0;

	switch(oi)
	{
	case 0x6000://采集档案配置
		wLen	= _get_cj_cfg(oadd,wOffset,pData,wMaxBufLen,bNext);
		break;
	// case 0x6014://普通采集方案配置
	// case 0x6016:
	// case 0x6018:
	// case 0x601C:
	// 	wLen	=  _get_scheme(oadd,wOffset,pData,wMaxBufLen,bNext);
	// 	break;
	// case 0x6034://采集任务监控集
	// 	wLen	= _get_cj_info(oadd,wOffset,pData,wMaxBufLen,bNext);
	// 	break;
	default:
		break;
	}
	return wLen;
}

WORD Class11_Get(DWORD oadd,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext)
{
	WORD oi=HIWORD(oadd);
	WORD wLen=0;
	
	*bNext	= FALSE;
	switch(LHBYTE(oadd))
	{
	case 1://逻辑名
		wLen	= Classx_1_get(oi,pData);
		break;
	case 2://集合
		wLen	= _Class11_2_Get(oadd,wOffset,pData,wMaxBufLen,bNext);
		return wLen;
	case 3://当前元素个数
		{
			WORD wNum=0;
			switch(oi)
			{
			case 0x6000:	wNum	= GetAllMeterSum();	break;
			// case 0x6014:	wNum	= get_scheme_num(1);break;
			// case 0x6016:	wNum	= get_scheme_num(2);break;
			// case 0x6018:	wNum	= get_scheme_num(3);break;
			// case 0x601A:    wNum    = get_scheme_num(3);break;
			// case 0x601C:	wNum	= get_scheme_num(4);break;
			// case 0x601E:	wNum	= 0;	break;
			// case 0x6034:	wNum	= get_taskcfg_num(BIT1|BIT2);break;
			}
			pData[wLen++]	= dt_long_unsigned;
			wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&wNum,TD21);
		}
		break;
	case 4://最大元素个数
		{
			WORD wMaxNum=0;
			switch(oi)
			{
			case 0x6000:	wMaxNum	= MAX_MP_NUM - 1;	break;
			// case 0x6014:	wMaxNum	= MAX_SCH_GENERAL;	break;
			// case 0x6016:	wMaxNum	= MAX_SCH_EVENT;	break;
			// case 0x6018:	wMaxNum	= MAX_SCH_TRANS;	break;
			// case 0x601A:    wMaxNum = MAX_SCH_TRANS;    break;
			// case 0x601C:	wMaxNum	= MAX_SCH_REPORT;	break;
			// case 0x601E:	wMaxNum	= 0;	break;
			// case 0x6034:	wMaxNum	= MAX_SCH_GENERAL;	break;
			}
			pData[wLen++]	= dt_long_unsigned;
			wLen	+= gdw698Buf_long_unsigned(&pData[wLen],&wMaxNum,TD21);
		}		
		break;
	}
	if(wLen && LLBYTE(oadd))
	{
		if(!get_X_data_of_buf(pData,LLBYTE(oadd),pData,&wLen))
			wLen	= 0;
	}
	return wLen;
}

static WORD _get_rec_6000(BYTE Ch,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealOffset)
{
	WORD wI,wLen=0,wParaOffset=0,wCfgLen,j,wNum=0,wNum2=0;
	RSD rsdd={0};
	WORD wNo=0xFFFF,wTmp,wOldLen,wNumOffset;
	BYTE bbuf[1024];
	BYTE *Buf=bbuf;
	CSD csdd;
	BYTE *pRCSD;
	WORD wCSDNum;
	TOad60010200 mpcfg;
	WORD wMaxNum=GetAllMeterSum();
	TMPFlagsCtrl *pMPFlag=pGetMPFlag();
	DAR dar = dar_success;
	TTwoByteUnion OI;
		
	OI.w = 0x0060;
	//1、选择方法
	wParaOffset	+= gdw698buf_RSD(pPara,&rsdd,TD12);		
	//2、选择内容
	pRCSD	= &pPara[wParaOffset];
	wCSDNum	= get_csd_num_of_RCSD(pRCSD);		
	wCfgLen	= objGetDataLen(dt_RCSD,&pPara[wParaOffset],MAX_NUM_NEST);

	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
	{
		dar = dar_timeflag_invalid;
	}
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, OI.b) == FALSE )//判断安全模式参数
	{			
		dar = dar_password_failure;
	}
	
	if((rsdd.choice == 0) 
	   ||(rsdd.choice == 1)
	   ||(rsdd.choice == 2) 
	   ||(rsdd.choice == 3))		
	{
		wNo	= 0;
	}
	
	if(wCSDNum == 0)
	{
		DWORD oadd2=0x60010200;
		wCSDNum = 1;
		pRCSD	= &pData[wLen];
		pData[wLen++]	= 1;
		pData[wLen++]	= 0;	//choice
		wLen	+= gdw698buf_OAD(&pData[wLen],&oadd2,TD21);
	}
	else
	{
		memcpy(&pData[wLen],&pPara[wParaOffset],wCfgLen);
		wLen	+= wCfgLen;
	}
	
	if((wNo == 0xFFFF) || dar)
	{
		if(wNo == 0xFFFF)
		{
			dar = dar_other_reason;
		}
		
		pData[wLen++]	= 0;
		wLen	+= gdw698buf_DAR(&pData[wLen],&dar,TD21);
		return wLen;
	}
	pData[wLen++]	= 1;		
	wNumOffset		= wLen++;
	if(NULL != Buf)
	{
		for(wI=1;wI<MAX_MP_NUM;wI++)
		{
			if((pMPFlag->Flag_Valid[wI>>3] & (1<<(wI&0x07))) == 0)	
				continue;
			GetMP698Para(wI,&mpcfg);				
			if(rsdd.choice == 1)
			{
				if((rsdd.sel1.oad.value == 0x60010201)&& (rsdd.sel1.value.type == dt_long_unsigned))
				{
					if(rsdd.sel1.value.nVal != mpcfg.nIndex)
						continue;
				}					
				else continue;
			}
			else if((rsdd.choice == 2) || (rsdd.choice == 3))
			{
				Selector2 *psel2=NULL;
				WORD wJ,wIndex;
				WORD wRegNum=1;
				
				if(rsdd.choice == 3)
					wRegNum = min(rsdd.sel3.nNum,MAX_SELECTOR2_NUM);
				for(wJ=0; wJ<wRegNum; wJ++)
				{
					if(rsdd.choice == 2)
						psel2 = &rsdd.sel2;
					else
						psel2 = &rsdd.sel3.item[wJ];
					
					if((NULL != psel2) && (psel2->oad.value == 0x60010201))
					{
						WORD nIntval=1;
						
						if((psel2->from.type != dt_long_unsigned)||(psel2->to.type != dt_long_unsigned))
							continue;
						if(psel2->span.type == dt_long_unsigned)
							nIntval = psel2->span.wVal;
						for(wIndex=psel2->from.wVal; wIndex<psel2->to.wVal; wIndex+=nIntval)
						{
							if(wIndex == mpcfg.nIndex)
								break;
						}
						if(wIndex >= psel2->to.wVal)
							continue;
						else
							break;
					}
					else continue;
				}
				if(wJ >= wRegNum)
					continue;
			}
			if(wNum >= wOffset)
			{
				wOldLen	= gdw698Buf_meter_cfg(Buf,&mpcfg,TD21);
				if((wNum2 == 0) || ((wMaxBufLen-wLen)>800))
				{
					for(j=0;j<wCSDNum;j++)
					{
						get_no_csd_of_RCSD(pRCSD,j,&csdd);
						if(csdd.choice)
							continue;
						if(LLBYTE(csdd.oad.value))
						{
							get_X_data_of_buf(Buf,LLBYTE(csdd.oad.value),&pData[wLen],&wTmp);
							wLen	+= wTmp;
						}
						else
						{
							memcpy(&pData[wLen],Buf,wOldLen);
							wLen	+= wOldLen;
						}
					}
					wNum2++;	
				}
				else
				{
					*bNext	= TRUE;							
					break;
				}	
			}
			wNum++;			
		}
	}
	pData[wNumOffset]	= (BYTE)wNum2;
	*pwRealOffset = wNum;
	return wLen;
}

static WORD _get_rec_6014(BYTE Ch,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealOffset)
{
	WORD wI,wLen=0,wParaOffset=0,wCfgLen,j,wNum=0,wNum2=0;
	RSD rsdd={0};
	WORD wNo=0xFFFF,wTmp,wOldLen,wNumOffset;
	BYTE bbuf[1024];
	BYTE *Buf=bbuf;
	CSD csdd;
	BYTE *pRCSD;
	WORD wCSDNum;
	TOad60150200 gensch;
	DAR dar = dar_success;
	TTwoByteUnion OI;
	
	OI.w = 0x1460;
	//1、选择方法
	wParaOffset	+= gdw698buf_RSD(pPara,&rsdd,TD12);		
	//2、选择内容
	pRCSD	= &pPara[wParaOffset];
	wCSDNum	= get_csd_num_of_RCSD(pRCSD);		
	wCfgLen	= objGetDataLen(dt_RCSD,&pPara[wParaOffset],MAX_NUM_NEST);
	
	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
	{
		dar = dar_timeflag_invalid;
	}
	else if( JudgeTaskAuthority( Ch, eREAD_MODE, OI.b) == FALSE )//判断安全模式参数
	{			
		dar = dar_password_failure;
	}
	
	if((rsdd.choice == 0) 
	   ||(rsdd.choice == 1)
		)		
	{
		wNo	= 0;
	}

	if(wCSDNum == 0)
	{
		DWORD oadd2=0x60150200;
		wCSDNum = 1;
		pRCSD	= &pData[wLen];
		pData[wLen++]	= 1;
		pData[wLen++]	= 0;	//choice
		wLen	+= gdw698buf_OAD(&pData[wLen],&oadd2,TD21);
	}
	else
	{
		memcpy(&pData[wLen],&pPara[wParaOffset],wCfgLen);
		wLen	+= wCfgLen;
	}

	if((wNo == 0xFFFF) || dar)
	{
		if(wNo == 0xFFFF)
		{
			dar = dar_other_reason;
		}
		
		pData[wLen++]	= 0;
		wLen	+= gdw698buf_DAR(&pData[wLen],&dar,TD21);
		return wLen;
	}
	
	pData[wLen++]	= 1;		
	wNumOffset		= wLen++;
	if(NULL != Buf)
	{
		for(wI=0;wI<256;wI++)
		{
			gensch.valid_flag	= 0;	
			if(!GetschInfo(TT_DATA_CJ,(BYTE)wI,(BYTE*)&gensch))
				continue;
			if(gensch.valid_flag != 0x5AA5)
				continue;
			if(rsdd.choice == 1)
			{
				if( (rsdd.sel1.oad.value == 0x60150201) && (rsdd.sel1.value.type == dt_unsigned))
				{
					if(rsdd.sel1.value.byVal != gensch.byShemeNo)
						continue;
				}
				else if( (rsdd.sel1.oad.value == 0x60150206) && (rsdd.sel1.value.type == dt_enum) )
				{//存贮时标选择
					if(rsdd.sel1.value.byVal != gensch.bySaveFlag)
						continue;
				}
				else 
				{
					continue;
				}
			}
			if(wNum >= wOffset)
			{				
				wOldLen	= gdw698Buf_general_cjscheme(Buf,&gensch,TD21);				
				if((wNum2 == 0) || ((wMaxBufLen-wLen)>800))
				{
					for(j=0;j<wCSDNum;j++)
					{
						get_no_csd_of_RCSD(pRCSD,j,&csdd);
						if(csdd.choice)
							continue;
						if(LLBYTE(csdd.oad.value))
						{
							get_X_data_of_buf(Buf,LLBYTE(csdd.oad.value),&pData[wLen],&wTmp);
							wLen	+= wTmp;
						}
						else
						{
							memcpy(&pData[wLen],Buf,wOldLen);
							wLen	+= wOldLen;
						}
					}
					wNum2++;	
				}
				else
				{
					*bNext	= TRUE;							
					break;
				}	
			}
			wNum++;			
		}
	}
	pData[wNumOffset]	= (BYTE)wNum2;
	*pwRealOffset = wNum;
	return wLen;
}

// static WORD _get_rec_6034(BYTE Ch,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealOffset)
// {
// 	WORD wI,wLen=0,wParaOffset=0,wCfgLen,j,wNum=0,wNum2=0;
// 	RSD rsdd={0};
// 	WORD wNo=0xFFFF,wTmp,wOldLen,wNumOffset;
// 	BYTE bbuf[1024];
// 	BYTE *Buf=bbuf;
// 	CSD csdd;
// 	BYTE *pRCSD;
// 	WORD wCSDNum;		
// 	TOad60130200 *pCfg;
// 	TTask698RunInfo RunInfo;
// 	DAR dar = dar_success;
// 	TTwoByteUnion OI;
	
// 	OI.w = 0x3460;
// 	//1、选择方法
// 	wParaOffset	+= gdw698buf_RSD(pPara,&rsdd,TD12);		
// 	//2、选择内容
// 	pRCSD	= &pPara[wParaOffset];
// 	wCSDNum	= get_csd_num_of_RCSD(pRCSD);		
// 	wCfgLen	= objGetDataLen(dt_RCSD,&pPara[wParaOffset],MAX_NUM_NEST);
	
// 	if( APPData[Ch].TimeTagFlag == eTime_Invalid )//时间标签错误
// 	{
// 		dar = dar_timeflag_invalid;
// 	}
// 	else if( JudgeTaskAuthority( Ch, eREAD_MODE, OI.b) == FALSE )//判断安全模式参数
// 	{			
// 		dar = dar_password_failure;
// 	}
	
// 	if((rsdd.choice == 0)	
// 	||(rsdd.choice == 1)	
// 		)
// 	{
// 		wNo	= 0;
// 	}
// 	if(wCSDNum == 0)
// 	{
// 		DWORD oadd2=0x60350200;
// 		wCSDNum	= 1;
// 		pRCSD	= &pData[wLen];
// 		pData[wLen++]	= 1;
// 		pData[wLen++]	= 0;//choice
// 		wLen	+= gdw698buf_OAD(&pData[wLen],&oadd2,TD21);
// 	}
// 	else 
// 	{
// 		memcpy(&pData[wLen],&pPara[wParaOffset],wCfgLen);
// 		wLen	+= wCfgLen;
// 	}	

// 	if((wNo == 0xFFFF) || dar)
// 	{
// 		if(wNo == 0xFFFF)
// 		{
// 			dar = dar_other_reason;
// 		}
		
// 		pData[wLen++]	= 0;
// 		wLen	+= gdw698buf_DAR(&pData[wLen],&dar,TD21);
// 		return wLen;
// 	}
	
// 	pData[wLen++]	= 1;		
// 	wNumOffset		= wLen++;
// 	if(NULL != Buf)
// 	{
// 		BYTE byTaskNo;
// 		for(wI=0;wI<MAX_698_TASK_NUM;wI++)
// 		{			
// 			pCfg	= pGet698NoTaskCfg((BYTE)wI);
// 			if(pCfg->validflag != 0x5AA5)
// 				continue;
// 			if(rsdd.choice == 1)
// 			{
// 				if((rsdd.sel1.oad.value == 0x60350201) && (rsdd.sel1.value.type == dt_unsigned))
// 				{						
// 					if(rsdd.sel1.value.byVal != pCfg->byTaskID)
// 						continue;
// 				}					
// 				else continue;
// 			}
// 			sys_GetTaskRunInfo((BYTE)wI,&byTaskNo,&RunInfo);
// 			if(wNum >= wOffset)
// 			{				
// 				wOldLen	= gdw698Buf_TaskRunInfo(Buf,byTaskNo,&RunInfo);				
// 				if((wNum2 == 0) || ((wMaxBufLen-wLen)>wOldLen))
// 				{
// 					for(j=0;j<wCSDNum;j++)
// 					{
// 						get_no_csd_of_RCSD(pRCSD,j,&csdd);
// 						if(csdd.choice)
// 							continue;
// 						if(LLBYTE(csdd.oad.value))
// 						{
// 							get_X_data_of_buf(Buf,LLBYTE(csdd.oad.value),&pData[wLen],&wTmp);
// 							wLen	+= wTmp;
// 						}
// 						else
// 						{
// 							memcpy(&pData[wLen],Buf,wOldLen);
// 							wLen	+= wOldLen;
// 						}
// 					}
// 					wNum2++;	
// 				}
// 				else
// 				{
// 					*bNext	= TRUE;							
// 					break;
// 				}	
// 			}
// 			wNum++;			
// 		}
// 	}
// 	pData[wNumOffset]	= (BYTE)wNum2;
// 	*pwRealOffset = wNum;
// 	return wLen;
// }

WORD Class11_Get_rec(BYTE Ch,DWORD oadd,BYTE *pPara,WORD wOffset,BYTE *pData,WORD wMaxBufLen,BOOL *bNext,WORD *pwRealOffset)
{
	*bNext	= FALSE;
	switch(oadd)
	{
	case 0x60000200: return _get_rec_6000(Ch,pPara,wOffset,pData,wMaxBufLen,bNext,pwRealOffset);
	// case 0x60140200: return _get_rec_6014(Ch,pPara,wOffset,pData,wMaxBufLen,bNext,pwRealOffset);
	//case 0x60340200: return _get_rec_6034(Ch,pPara,wOffset,pData,wMaxBufLen,bNext,pwRealOffset);
	default: return 0;
	}
}

BOOL Class11_Set(DWORD oadd,BYTE *pData,DAR *rc)
{
WORD oi=HIWORD(oadd);
WORD wOffset=0,wNum,wI,wTmp;
BYTE byBNo;
	
	switch(LHBYTE(oadd))
	{
	case 2:
//		if(oi == 0x6000)
//		{
//			BYTE OutBuf[10];
//			WORD wTmpOutLen;
//			if(LLBYTE(oadd) == 0)
//			{
//				if(_Class11_action_128(MDW2(oi,0x8000),&pData[wOffset],OutBuf,&wTmpOutLen))
//					*rc	= dar_success;
//			}
//			else
//			{
//				if(_Class11_action_127(MDW2(oi,0x8000),&pData[wOffset],OutBuf,&wTmpOutLen))
//					*rc	= dar_success;
//			}
//		}
//		else 
        // if(oi == 0x6014)
		// {
		// 	if(LLBYTE(oadd) == 0)
		// 		*rc	= add_general_schemes(&pData[wOffset]);
		// 	else
		// 	{
		// 		TOad60150200 sch={0};
		// 		if(gdw698Buf_general_cjscheme(&pData[wOffset],&sch,TD12))
		// 		{
		// 			add_cjscheme(TT_DATA_CJ,sch.byShemeNo,(BYTE*)&sch);
		// 			*rc	= dar_success;
		// 		}	
		// 	}
		// }
		// else
			*rc	= dar_no_long_set_in_progress;
		break;
	case 8://搜表配置
			*rc	= dar_success;//为了过台体
			break;
	case 9://每天周期搜表的配置
	default:
		*rc	= dar_no_long_set_in_progress;
		break;
	}		
	return ((*rc)	== dar_success);
}

DAR set_ex_cjRule(BYTE *pBuf)
{
	return dar_success;
}

// DAR _set_gen_sch_CSD(BYTE *pBuf)
// {
// TOad60150200 cfg={0};	
// WORD offset=0,tmp,i,wCSDDatalen,wTmmp2;
// BYTE sch_no,byIndex;
	
// 	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 2))	
// 		return dar_object_unavailable;
// 	offset	+= 2;
// 	if(pBuf[offset++] != dt_unsigned)
// 		return dar_object_unavailable;
// 	offset	+= gdw698Buf_unsigned(&pBuf[offset],&sch_no,TD12);
// 	if(schNo2Index(sch_no,TT_DATA_CJ,&byIndex) == FALSE)//合法性判断
// 	{
// 		return dar_temporary_failure;
// 	}
// 	if(pBuf[offset++] != dt_array)
// 		return dar_object_unavailable;
// 	offset	+= gdw698Buf_Num(&pBuf[offset],&tmp,TD12);	
// 	if(!sys_cj_scheme_read(TT_DATA_CJ,sch_no,(BYTE*)&cfg))		
// 		memset(&cfg,0,sizeof(TOad60150200));
// 	wCSDDatalen	= 0;
// 	cfg.byColNum= 0;
// 	for(i=0;i<tmp;i++)
// 	{
// 		if(pBuf[offset++] != dt_CSD)
// 			return dar_object_unavailable;
// 		wTmmp2	= objGetDataLen(dt_CSD,&pBuf[offset],MAX_NUM_NEST);
// 		if((WORD)(wCSDDatalen+wTmmp2) < sizeof(cfg.csdBuf))
// 		{
// 			memcpy(&cfg.csdBuf[wCSDDatalen],&pBuf[offset],wTmmp2);
// 			wCSDDatalen	+= wTmmp2;
// 			cfg.byColNum++;
// 		}	
// 		offset	+= wTmmp2;			
// 	}		
// 	cfg.byShemeNo	= sch_no;
// 	add_cjscheme(TT_DATA_CJ,sch_no,(BYTE*)&cfg);
// 	return dar_success;
// }

// DAR _set_gen_sch_MS(BYTE *pBuf)
// {
// TOad60150200 cfg={0};	
// WORD offset=0;
// BYTE sch_no;
	
// 	if((pBuf[offset] != dt_structure) || (pBuf[offset+1] != 2))	
// 		return dar_object_unavailable;
// 	offset	+= 2;
// 	if(pBuf[offset++] != dt_unsigned)
// 		return dar_object_unavailable;
// 	offset	+= gdw698Buf_unsigned(&pBuf[offset],&sch_no,TD12);

// 	if(pBuf[offset++] != dt_MS)
// 		return dar_object_unavailable;
// 	sys_cj_scheme_read(1,sch_no,(BYTE*)&cfg);
// 	offset	+= gdw698Buf_MS(&pBuf[offset],&cfg.objMS,TD12);			
// 	cfg.byShemeNo	= sch_no;
// 	add_cjscheme(1,sch_no,(BYTE*)&cfg);
// 	return dar_success;
// }

// DAR del_general_schemes(BYTE *pBuf)
// {
// WORD offset=0,i,num;
// BYTE sch_no,byIndex;

// 	if(pBuf[offset++] != dt_array)
// 		return dar_object_unavailable;
// 	offset	+= gdw698Buf_Num(&pBuf[offset],&num,TD12);
// 	for(i=0;i<num;i++)
// 	{
// 		offset++;
// 		offset	+= gdw698Buf_unsigned(&pBuf[offset],&sch_no,TD12);
// 		if(schNo2Index(sch_no,TT_DATA_CJ,&byIndex) == FALSE)//合法性判断
// 		{
// 			return dar_temporary_failure;
// 		}
// 		del_cjscheme(TT_DATA_CJ,sch_no);
// 	}
// 	return dar_success;
// }

// DAR clear_general_schemes(BYTE *pBuf)
// {
// WORD i;

// 	for(i=0;i<256;i++)
// 	{		
// 		del_cjscheme(TT_DATA_CJ,(BYTE)i);
// 	}
// 	return dar_success;
// }

BOOL _Class11_action_127(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen)
{
WORD oi=HIWORD(oadd),wTmp;
BOOL bRc=TRUE;
WORD wOffset=0;

	if(oi == 0x6000)
	{
		TOad60010200 cfg ={0};
		wTmp	= gdw698Buf_meter_cfg(pData,&cfg,TD12);
		if(wTmp)
			bRc = add_mp_cfg(cfg.nIndex,&cfg);
		else
			bRc	= FALSE;
	}
	// else if(oi == 0x6014)
	// {
	// 	DAR ar;
	// 	ar	= add_general_schemes(pData);
	// 	bRc	= (ar == dar_success);
	// }
	// else if(oi == 0x601E)
	// {
	// 	DAR ar;
	// 	ar	= set_ex_cjRule(pData);
	// 	bRc	= (ar == dar_success);
	// }
	return bRc;
}

BOOL _Class11_action_128(DWORD oadd, BYTE *pData, BYTE *pOutData, WORD *poutLen)
{
	WORD oi = HIWORD(oadd), wTmp, wLen = 0, wLenT = 0;
	BOOL bRc = TRUE;

	if(oi == 0x6000)
	{   // 批量添加
		WORD num, i;
		TOad60010200 cfg = {0};
		if(pData[wLen++] != dt_array)
			return FALSE;
		bRc = FALSE;
		wLen += gdw698Buf_Num(&pData[wLen], &num, TD12);
		wLenT = wLen;
		for(i = 0; i < num; i++)   // 先判断报文内容合理
		{
			wTmp = gdw698Buf_meter_cfg(&pData[wLenT], &cfg, TD12);
			if( (wTmp == 0) 
            || (cfg.nIndex == 0)
            ||(cfg.nIndex >= MAX_MP_NUM) )
			{
				return FALSE;   // 不合理 拒绝添加
			}
			wLenT += objGetDataLen((OBJ_DATA_TYPE)pData[wLenT], &pData[wLenT + 1], MAX_NUM_NEST) + 1;
		}

		for(i = 0; i < num; i++)//添加
		{
			wTmp = gdw698Buf_meter_cfg(&pData[wLen], &cfg, TD12);
			if(wTmp)
			{
				bRc = TRUE;
				add_mp_cfg(cfg.nIndex, &cfg);
			}
			else
			{
				return FALSE;
			}
			wLen += objGetDataLen((OBJ_DATA_TYPE)pData[wLen], &pData[wLen + 1], MAX_NUM_NEST) + 1;
		}
	}
	// else if(oi == 0x6014)
	// {
	// 	DAR ar;
	// 	ar = del_general_schemes(pData);
	// 	bRc = (ar == dar_success);
	// }
	return bRc;
}

BOOL _Class11_action_129(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen)
{
WORD oi=HIWORD(oadd);
BOOL bRc=TRUE;

	if(oi == 0x6000)
	{
		if(_update_meter_cfg1(pData) != dar_success)
			bRc	= FALSE;			
	}
	// else if(oi == 0x6014)
	// {
	// 	DAR ar;
	// 	ar	= clear_general_schemes(pData);
	// 	bRc	= (ar == dar_success);
	// }
	return bRc;
}

BOOL Class11_action(DWORD oadd,BYTE *pData,BYTE *pOutData,WORD *poutLen)
{
WORD oi=HIWORD(oadd);
BOOL bRc=TRUE;

	*poutLen	= 0;
	switch(LHBYTE(oadd))
	{
	case 127: 
		bRc	= _Class11_action_127(oadd,pData,pOutData,poutLen);
		break;
	case 128: 
		bRc	= _Class11_action_128(oadd,pData,pOutData,poutLen);
		break;		
	case 129: 
		bRc	= _Class11_action_129(oadd,pData,pOutData,poutLen);
		break;
	case 130: 
		if(oi == 0x6000)
		{
			if(_update_meter_cfg2(pData) != dar_success)
				bRc	= FALSE;			
		}
		// else if(oi == 0x6014)
		// {
		// 	if(_set_gen_sch_CSD(pData) != dar_success)
		// 		bRc	= FALSE;			
		// }
		break;
	case 131:
		if(oi == 0x6000)
		{
			if(_del_meter(pData) != dar_success)
				bRc	= FALSE;
		}
		break;
	case 132: 
		if(oi == 0x6000)
		{
			if(_del_meter_bycfg1(pData) != dar_success)
				bRc	= FALSE;
		}				
		break;
	case 133: 
		if(oi == 0x6000)
		{
			if(_del_meter_byport(pData) != dar_success)
				bRc	= FALSE;
		}				
		break;
	case 134: 
		if(oi == 0x6000)
		{			
			if(_del_all_meters(pData) != dar_success)
				bRc	= FALSE;
		}				
		break;
	case 135: 
		if(oi == 0x6000)
		{
			if(_update_meter_cfg3(pData) != dar_success)
				bRc	= FALSE;			
		}
		break;

	// case 200:
	// 	if(oi == 0x6014)
	// 	{
	// 		if(GetUserType() == MAR_HE_NAN)
	// 		{
	// 			if(_set_gen_sch_MS(pData) != dar_success)
	// 				bRc = FALSE;
	// 		}
	// 	}
	// 	break;
	}	
	return bRc;
}
#endif