/*****************************************************************************/
/*
 *项目名称： 物联网表电能质量模块
 *创建人：   wjd
 *日  期：   2020年4月
 *修改人：
 *日  期：
 *描  述：
 *
 *版  本：
 *说  明:    数据库设计
 *
 */
/*****************************************************************************/

#include "AllHead.h"
//#include "SavePara.h"
//#include "task_stats.h"
#include "Dlt698_45.h"

//#include "task_vfrate.h"

extern WORD GetBasicTypeLen( BYTE DataType, BYTE *Tag );

PQ_JZ_RATE PQ_Para;// ;

static SYS_PARA_1808 BackupPara;    //最后一次参数的备份
/**************************************************************************
*
*  实时数据区
*
***************************************************************************/

// static PQ_DATA s_pq_data;
PQ_DATA s_pq_data;              // TODO: 仿真用，删除静态关键字

WORD api_GetExtremeStatData(  BYTE Index, BYTE Times, BYTE *pBuf );
WORD api_GetProbatyStatData(  BYTE Index, BYTE Times, BYTE *pBuf );
WORD api_GetExtremeStatPara( BYTE Index, BYTE Times, BYTE *pBuf );
WORD api_GetProbatyStatPara(  BYTE Index, BYTE Times, BYTE *pBuf );
/* 获取数据库指针 */
PQ_DATA * GetPqDataAddr()
{
    return &s_pq_data;
}

/*稳态数据时间戳*/
void SetTimeStamp(BYTE type, DSPTIME *val)
{
    switch(type)
    {
        case TIME_3S: /* 3秒数据时间戳 */
            memcpy(&s_pq_data.time.tm_3s, val, sizeof(DSPTIME));
            return ;
        case TIME_10S: /* 10秒数据时间戳 */
            memcpy(&s_pq_data.time.tm_10s, val, sizeof(DSPTIME));
            return ;
        case TIME_10M: /* 10分数据时间戳 */
            memcpy(&s_pq_data.time.tm_10m, val, sizeof(DSPTIME));
            return ;
        case TIME_2H: /* 2小时数据时间戳 */
            memcpy(&s_pq_data.time.tm_2h, val, sizeof(DSPTIME));
            return ;
        default:
            return ;
    }
}

/*稳态数据存放到数据库*/
void SetPqData(BYTE type, void *val)
{
    switch(type)
    {
        case PQ_U_RMS:  // 电压有效值
            memcpy(s_pq_data.vol.vi_mag, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_U_DC:   // 电压直流分量
            memcpy(s_pq_data.vol.vi_dc, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_U_SEQ_POS:
            memcpy(&s_pq_data.vol.vi_seq.pos, val, sizeof(DWORD));
            break;
        case PQ_U_SEQ_NEG:
            memcpy(&s_pq_data.vol.vi_seq.neg, val, sizeof(DWORD));
            break;
        case PQ_U_SEQ_ZERO:
            memcpy(&s_pq_data.vol.vi_seq.zero, val, sizeof(DWORD));
            break;
        case PQ_U_DEV:
            memcpy(s_pq_data.vol.vi_union.v.vol_dev, val, PH_NUM*sizeof(SDWORD));
            break;
        case PQ_U_UNBALANCE_NEG:
            memcpy(&s_pq_data.vol.vi_unbalance.neg, val, sizeof(DWORD));
            break;
        case PQ_U_UNBALANCE_ZERO:
            memcpy(&s_pq_data.vol.vi_unbalance.zero, val, sizeof(DWORD));
            break;
        case PQ_U_FREQ:/*频率*/
            memcpy(&s_pq_data.vol.vi_union.v.hz, val, sizeof(DWORD));
            break;
        case PQ_U_FREQ_DEV:
            memcpy(&s_pq_data.vol.vi_union.v.hz_dev, val, sizeof(SDWORD));
            break;
        case PQ_U_FLUC:/*电压波动*/
            memcpy(s_pq_data.vol.vi_union.v.fluc, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_U_FLUCF:/*波动频率*/
            memcpy(s_pq_data.vol.vi_union.v.fluc_f, val, PH_NUM*sizeof(DWORD));
            break;
		case PQ_U_FLUC2:/*电压波动*/
            memcpy(s_pq_data.vol.vi_union.v.fluc2, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_U_FLUCF2:/*波动频率*/
            memcpy(s_pq_data.vol.vi_union.v.fluc_f2, val, PH_NUM*sizeof(DWORD));
            break;

        case PQ_U_PST:/*短时闪变*/
            memcpy(s_pq_data.vol.vi_union.v.pst, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_U_PLT:/*长时闪变*/
            memcpy(s_pq_data.vol.vi_union.v.plt, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_I_RMS:  // 电流有效值
            memcpy(s_pq_data.cur.vi_mag, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_I_DC:   // 电流直流分量
            memcpy(s_pq_data.cur.vi_dc, val, PH_NUM*sizeof(DWORD));
            break;
        case PQ_I_SEQ_POS:
            memcpy(&s_pq_data.cur.vi_seq.pos, val, sizeof(DWORD));
            break;
        case PQ_I_SEQ_NEG:
            memcpy(&s_pq_data.cur.vi_seq.neg, val, sizeof(DWORD));
            break;
        case PQ_I_SEQ_ZERO:
            memcpy(&s_pq_data.cur.vi_seq.zero, val, sizeof(DWORD));
            break;
        case PQ_I_UNBALANCE_NEG:
            memcpy(&s_pq_data.cur.vi_unbalance.neg, val, sizeof(DWORD));
            break;
        case PQ_I_UNBALANCE_ZERO:
            memcpy(&s_pq_data.cur.vi_unbalance.zero, val, sizeof(DWORD));
            break;
        case PQ_PF:
            memcpy(s_pq_data.cur.vi_union.c.PF, val, (PH_NUM+1)*sizeof(INT));
            break;
        case PQ_P:      // 有功功率
            memcpy(s_pq_data.cur.vi_union.c.P, val, (PH_NUM+1)*sizeof(DWORD));
            break;
        case PQ_Q:      // 无功功率
            memcpy(s_pq_data.cur.vi_union.c.Q, val, (PH_NUM+1)*sizeof(DWORD));
            break;
        case PQ_S:      // 无功功率
            memcpy(s_pq_data.cur.vi_union.c.S, val, (PH_NUM+1)*sizeof(DWORD));
            break;
        case PQ_FUND_P:/*基波有功功率*/
            memcpy(s_pq_data.harm_data.fundpower.P, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_FUND_Q:/*基波无功功率*/
            memcpy(s_pq_data.harm_data.fundpower.Q, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_HARM_POS_P_TOTAL:/*2-50次总谐波正向有功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.posP, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_HARM_REV_P_TOTAL:/*2-50次总谐波反向有功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.revP, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_HARM_POS_Q_TOTAL:/*2-50次总谐波正向无功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.posQ, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_HARM_REV_Q_TOTAL:/*2-50次总谐波反向无功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.revQ, val, (PH_NUM+1)*sizeof(FLOAT));
            break;
        case PQ_HARM_P:/*2-50次谐波有功功率*/
            memcpy(s_pq_data.harm_data.harmpower.P, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            break;
        case PQ_HARM_Q:/*2-50次谐波无功功率*/
            memcpy(s_pq_data.harm_data.harmpower.Q, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            break;
        case PQ_IHARM_P:/*2-50次间谐波有功功率*/
            memcpy(s_pq_data.harm_data.iharmpower.P, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            break;
        case PQ_IHARM_Q:/*2-50次间谐波无功功率*/
            memcpy(s_pq_data.harm_data.iharmpower.Q, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            break;
        case PQ_EXTRERMS:
            memcpy(s_pq_data.vol.vi_union.v.ExtreRms, val, PH_NUM*(MaxRms+1)*sizeof(DWORD));
            break;
        default:
            break;
    }
}


/**************************************************************************
*
*  统计数据区
*
***************************************************************************/
PQ_STATSDATA s_pq_statsdata;

/* 获取统计数据库指针 */
PQ_STATSDATA * GetPqStatsDataAddr()
{
    return &s_pq_statsdata;
}


/**************************************************************************
*
*  系统参数数据区
*
***************************************************************************/
SYS_PARA_1808 SysPara;      // TODO: 仿真用，删除static

SYS_PARA_1808 *GetSysParaAddr()
{
    return &SysPara;
}

DWORD Get_Sys_Para(BYTE type)
{
    switch(type)
    {
    case PARA_3P3W_VOL: /* 电压是否3相3 */
        return (SysPara.Uis3P3W&0x1);
    case PARA_RATE_VOL: /* 额定电压*/
        return (SysPara.Uis3P3W) ? (DWORD)(((FLOAT)SysPara.vol_std)*1.73205081f) : (SysPara.vol_std);
    case PARA_RATE_CUR: /* 额定电流 */
        return SysPara.cur_std;
    }
    return 0;
}

/*校准参数设置部分*/
void Set_Adjust_Para(BYTE type ,BYTE phase,void *factor)
{
    BYTE ph;
	
	if( phase >= PH_NUM )
	{
		return;
	}
	
    switch(type)
    {
        case Factor_Big_Vol:
            memcpy((BYTE*)&SysPara.big_adjust_para.U_factor[phase], factor, sizeof(float));
            break;
        case Factor_Big_Cur:
            memcpy((BYTE*)&SysPara.big_adjust_para.I_factor[phase], factor, sizeof(float));
            break;
        case Factor_Big_Ang:
            memcpy((BYTE*)&SysPara.big_adjust_para.A_factor[phase], factor, sizeof(float));
            break;
        case Factor_P:
            memcpy((BYTE*)&SysPara.big_adjust_para.P_factor[phase], factor, sizeof(float));
            break;
        case Factor_Q:
            memcpy((BYTE*)&SysPara.big_adjust_para.Q_factor[phase], factor, sizeof(float));
            break;

        case Factor_Clr:
            for(ph=0;ph<PH_NUM;ph++)
            {
            	SysPara.big_adjust_para.U_factor[ph] = 1.0f;
                SysPara.big_adjust_para.I_factor[ph] = 1.0f;
                SysPara.big_adjust_para.A_factor[ph] = 0.0f;
                SysPara.big_adjust_para.P_factor[ph] = 1.0f;
                SysPara.big_adjust_para.Q_factor[ph] = 1.0f;
            }
            break;
				case EVol:
					memcpy((BYTE*)&SysPara.vol_std,factor,sizeof(DWORD));
					break;
				case ECur:
					memcpy((BYTE*)&SysPara.cur_std,factor,sizeof(DWORD));
					break;
			  case Is3P3W:
					//SysPara.Uis3P3W = ;
					break;
        default:
            return ;
    }
}

/* 反校表,由实际值 反推 采样值  暂态事件使用*/
int ICorrect(BYTE ph,BYTE type, float val)
{   /* 反校表 只有电压 和 电流*/
    if(type==PQ_Correct_U)
    {
    	return (int)(val/SysPara.big_adjust_para.U_factor[ph]);
    }
    else
    {
    	return (int)(val/SysPara.big_adjust_para.I_factor[ph]);
    }
}
/*获得频率*/
DWORD GetFreq()
{
	return s_pq_data.vol.vi_union.v.hz;
}

/*校准部分*/
int CorrectUIPQ(BYTE ph, BYTE type, void *data)
{
    int tmp32;
    SQWORD tmp64;
    switch(type)
    {
        case PQ_Correct_U:
        	memcpy(&tmp32,data,sizeof(int));
        	tmp32 *= (SysPara.big_adjust_para.U_factor[ph] * 10000);    // 保留4位小数，乘10000
        	return tmp32;
        case PQ_Correct_I:
            memcpy(&tmp32,data,sizeof(int));
            tmp32 *= (SysPara.big_adjust_para.I_factor[ph] * 10000);    // 保留4位小数，乘10000
            return tmp32;
        // case PQ_Correct_I_3s:
        //     memcpy(&tmp32,data,sizeof(int));
        //     tmp32 *= (SysPara.big_adjust_para.I_factor[ph]);
        //     return tmp32;
        case PQ_Correct_P://有功传入是long long型
            memcpy(&tmp64,data,sizeof(SQWORD));
            tmp64 *=(SysPara.big_adjust_para.P_factor[ph] * 10000);     // 保留4位小数，乘10000
            return (int)(tmp64);
        case PQ_Correct_Q://无功传入是long long型
            memcpy(&tmp64,data,sizeof(SQWORD));
            tmp64 *= (SysPara.big_adjust_para.Q_factor[ph] * 10000);    // 保留4位小数，乘10000
            return (int)(tmp64);
        default:
            return 0 ;
    }
}

//double型的校准
void Correct_UI_double(BYTE ph, BYTE type, double *data)
{
    double tmp_lf64;
    switch(type)
    {
        case PQ_Correct_U://电压传入是int型
            memcpy(&tmp_lf64,data,sizeof(double));
			tmp_lf64 *=SysPara.big_adjust_para.U_factor[ph];
            memcpy(data, &tmp_lf64,sizeof(double));
            return ; //电压采样带3位小数,传送给ARM时带4位小数,一共需要乘10,此外还要除掉校表系数
        case PQ_Correct_I://电流传入是int型
            memcpy(&tmp_lf64,data,sizeof(double));
            tmp_lf64 *= (SysPara.big_adjust_para.I_factor[ph]);
            memcpy(data, &tmp_lf64,sizeof(double));
            return ; //电流采样带2位小数,传送给ARM时带4位小数,一共需要乘100,此外还要除掉校表系数
        default:
            return ;
    }
}


/*角差校准*/
void CorrectAngle(int *p,int *q)
{
    char ph;
    int tp;
    float ang;
    for(ph=0;ph<PH_NUM;ph++)
    {
        ang = SysPara.big_adjust_para.A_factor[ph];
        tp=(int)(ang*q[ph]+p[ph]);
        q[ph]=(int)(q[ph]-ang*p[ph]);
        p[ph]=tp;
    }
}

/*系统参数初始化*/
void SysParaInit()
{
    BYTE k;
    memset(&s_pq_data, 0x00, sizeof(s_pq_data));
    //if(ReadParaFromSpiFlash((BYTE *)&SysPara, sizeof(SYS_PARA_1808))==FALSE)
    {

        /*事件门限*/
    	SysPara.evt_threshold[TRA_EVT_VOL_DIP][EVT_BEGIN]=90;//90%
        SysPara.evt_threshold[TRA_EVT_VOL_DIP][EVT_END]=95;//95%

        SysPara.evt_threshold[TRA_EVT_VOL_SWL][EVT_BEGIN]=110;//110%
        SysPara.evt_threshold[TRA_EVT_VOL_SWL][EVT_END]=105;//105%

        SysPara.evt_threshold[TRA_EVT_VOL_INT][EVT_BEGIN]=10;//10%
        SysPara.evt_threshold[TRA_EVT_VOL_INT][EVT_END]=15;//15%

        SysPara.Uis3P3W = FALSE ;/*电压3P4W*/
        SysPara.vol_std = DEFAULT_U;/*额定电压*/
        SysPara.cur_std=DEFAULT_I;

        /* 校表系数*/
        
        if( !api_VReadSafeMem(GET_CONTINUE_ADDR( EventConRom.big_adjust_para ), sizeof(BIG_ADJUST_PARA), (BYTE *)&SysPara.big_adjust_para) )
        {
              for(k=0;k<PH_NUM;k++)
              {
                      SysPara.big_adjust_para.U_factor[k] = DEFAULT_U_FACTOR;
              }

              for(k=0;k<PH_NUM;k++)
              {
                  SysPara.big_adjust_para.I_factor[k] = DEFAULT_I_FACTOR;
                  SysPara.big_adjust_para.A_factor[k] = DEFAULT_A_FACTOR;
                  SysPara.big_adjust_para.P_factor[k] = DEFAULT_P_FACTOR;
                  SysPara.big_adjust_para.Q_factor[k] = DEFAULT_Q_FACTOR;
              }
        }
    }
    memcpy(&BackupPara, &SysPara, sizeof(SYS_PARA_1808));   //参数备份
    memset(&s_pq_data, 0x00, sizeof(s_pq_data));
    
    PQ_Para.dwBdThreshold = 650;

}

//因为double型数组,无法使用memset函数赋0,所以创建此函数用于初始化double型数组为零
void memclr_f(void *ptr, int len)
{
    int i;
    double *fptr;
    fptr = (double *)ptr;
    len = len/sizeof(double);
    for(i=0;i<len;i++)
    {
        fptr[i] = 0.0f;
    }
}
/*
void Save_Para()
{
    if(memcmp(&BackupPara,&SysPara,sizeof(SYS_PARA_1808))!=0)
    {   //参数更新才写入到Flash内,否则不写
        WriteParaToSpiFlash((BYTE *)&SysPara, sizeof(SYS_PARA_1808));
        memcpy(&BackupPara, &SysPara, sizeof(SYS_PARA_1808));   //备份参数备份
    }
}
*/


WORD api_GetModuleData(BYTE ModuleType, BYTE Phase, BYTE Times, BYTE *pBuf )
{
    WORD len=0x8000;
    DWORD tempvalue=0;
    DWORD dwTemp = 0;
    SDWORD sdwTemp = 0;
    DWORD ModuleStatus;
	int nLen=0;
        DWORD dwData;
    WORD wTemp = 0;

//     if( (Phase > PH_NUM) ||(Times >= HAMONIC_ANYSIS_NUM) )
//     {
//         return 0;
//     }

    switch(ModuleType)
    {
        case U:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
			// sdwTemp = s_pq_data.vol.vi_mag[Phase] / 1000;
			wTemp = (WORD)(s_pq_data.vol.vi_mag[Phase]/ 1000);
            // memcpy(pBuf,&s_pq_data.vol.vi_mag[Phase],4);
            memcpy(pBuf,&wTemp,2);
            len=2;
            break;
        #if(MAX_PHASE == 3)
        case U_SEQ_POS:
            memcpy(pBuf,&s_pq_data.vol.vi_seq.pos,4);
            len=4;
            break;
        case U_SEQ_NEG:
            memcpy(pBuf,&s_pq_data.vol.vi_seq.neg,4);
            len=4;
            break;
        case U_SEQ_ZERO:
            memcpy(pBuf,&s_pq_data.vol.vi_seq.zero,4);
            len=4;
            break;
        #endif
        case U_DEV:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.vol_dev[Phase],2);
            len=2;
            break;
        case I:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            sdwTemp = (SDWORD)(s_pq_data.cur.vi_mag[Phase] / 10);
            //sdwTemp = s_pq_data.cur.vi_mag[Phase];
            if(s_pq_data.cur.vi_union.c.P[Phase+1] < 0)
            {
                sdwTemp *= -1;
            }
            memcpy(pBuf,&sdwTemp,4);
            len=4;
            break;
        case I_HIGHPREC:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            sdwTemp = s_pq_data.cur.vi_mag[Phase];
            if(s_pq_data.cur.vi_union.c.P[Phase] < 0)
            {
                sdwTemp *= -1;
            }
            memcpy(pBuf,&sdwTemp,4);
            len=4;
            break;
        #if(MAX_PHASE == 3)
        case I_SEQ_POS:
            dwTemp = s_pq_data.cur.vi_seq.pos / 10;
            memcpy(pBuf,&dwTemp,4);
            len=4;
            break;
        case I_SEQ_NEG:
            dwTemp = s_pq_data.cur.vi_seq.neg / 10;
            memcpy(pBuf,&dwTemp,4);
            len=4;
            break;
        case I_SEQ_ZERO:
            dwTemp = s_pq_data.cur.vi_seq.zero / 10;
            memcpy(pBuf,&dwTemp,4);
            len=4;
            break;
        #endif
		case NILM_P:
			if(Phase >= (PH_NUM+1))
            {
				return  0x8000;
            }
            sdwTemp = (SDWORD)(s_pq_data.cur.vi_union.c.P[Phase] / 1000);
            memcpy(pBuf,&sdwTemp,4);
            len=4;
            break;
        case PF:
            if(Phase > PH_NUM)
            {
                 return  0x8000;
            }
			if(Phase == 0)
				Phase = PH_NUM;
			else
				Phase = Phase-1;
            memcpy(pBuf,&s_pq_data.cur.vi_union.c.PF[Phase],2);
		    len=2;
            break;
        case U_THD:
            if(Phase > PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_THD[Phase],2);
            len=2;
            break;
        case I_THD:
            if(Phase > PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.cur.vi_THD[Phase],2);
            len=2;
            break;
        case HZ:
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.hz,4);
            len=4;
            break;
        case HZ_DEV:
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.hz_dev,4);
            len=4;
            break;
        case U_UNBALANCE_NEG:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_data.vol.vi_unbalance.neg,2);
            len=2;
            #endif
            break;
        case U_UNBALANCE_ZERO:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_data.vol.vi_unbalance.zero,2);
            len=2;
            #endif
            break;
        case I_UNBALANCE_NEG:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_data.cur.vi_unbalance.neg,2);
            len=2;
            #endif
            break;
        case I_UNBALANCE_ZERO:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_data.cur.vi_unbalance.zero,2);
            len=2;
            #endif
            break;
        case UChange:
			if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc[Phase],4);
			pBuf += 4;
			memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc_f[Phase],4);
            len=8;
           /*new 20210906 if(Phase >= PH_NUM)
            {
                 return 0;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc[Phase],4);
            len=4;*/
            break;
		case UChange3:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc2[Phase],4);
			pBuf += 4;
			memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc_f2[Phase],2);
            len=6;
			break;

		case UChangeFre:
			if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.pst[Phase],4);
			pBuf += 4;
			memcpy(pBuf,&s_pq_data.vol.vi_union.v.plt[Phase],4);
            len=8;
			/*new 20210906 if(Phase >= PH_NUM)
			{
				 return 0;
			}
			memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc_f[Phase],4);
			len=4;*/
            break;
        case UFlicker:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.pst[Phase],4);
            len=4;
            break;
		case UFlickerLong:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.plt[Phase],4);
            len=4;
			break;
        case MODULE_STATUS:
            ModuleStatus = 0;
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_U_THD_OVER) << 0);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_I_THD_OVER) << 1);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_METERING_FAULT) << 6);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_PST_OVER) << 8);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_PLT_OVER) << 9);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_U_DIP) << 10);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_U_SWL) << 11);
            ModuleStatus |= ((DWORD)api_GetSysStatus(eSYS_STATUS_U_INT) << 12);
            memcpy(pBuf,&ModuleStatus,4);
            len=4;
            break;
        case HarmonicU:
            if(Phase >= PH_NUM)
            {
                 return 0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_harm_mag[Phase][Times],4);
            len=4;
            break;
        case HarmonicI:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.cur.vi_harm_mag[Phase][Times],4);
            len=4;
            break;
        case HarmonicP:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
            tempvalue=(long)s_pq_data.harm_data.harmpower.P[Phase][Times];
            memcpy(pBuf,&tempvalue,4);
            len=4;
            break;
        case HarmonicPALL:
            dwTemp=(DWORD)s_pq_data.harm_data.harmpower_total.posP[PH_NUM];//wxy Phase
            memcpy(pBuf,&dwTemp,4);
            dwTemp=(DWORD)(-s_pq_data.harm_data.harmpower_total.revP[PH_NUM]);//wxy Phase
            memcpy(pBuf+4,&dwTemp,4);
            len=8;
            break;
        case IHarmonicU:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_intharm_mag[Phase][Times],4);
            len=4;
            break;
        case IHarmonicI:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.cur.vi_intharm_mag[Phase][Times],4);
            len=4;
            break;
        case IHarmonicP:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
            tempvalue=(long)s_pq_data.harm_data.iharmpower.P[Phase][Times];
            memcpy(pBuf,&tempvalue,4);
            len=4;
            break;
        case HarmonicUDayCount:
            if(Phase >= PH_NUM || Times >= STATS_HARM_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.p95_harm_vol[Phase][Times],4);
            len=4;
            break;
        case HarmonicIDayCount:
            if(Phase >= PH_NUM || Times >= STATS_HARM_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.p95_harm_cur[Phase][Times],4);
            len=4;
            break;
        case IHarmonicUDayCount:
            if(Phase >= PH_NUM || Times >= STATS_IHARM_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.p95_iharm_vol[Phase][Times],4);
            len=4;
            break;
        case IHarmonicIDayCount:
            if(Phase >= PH_NUM || Times >= STATS_IHARM_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.p95_iharm_cur[Phase][Times],4);
            len=4;
            break;
        case PSTDayCount:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.pst[Phase].max_data.d,4);
            memcpy(pBuf+4,&s_pq_statsdata.pst[Phase].max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.pst[Phase].min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.pst[Phase].min_time,7);
            len=22;
            break;
        case PLTDayCount:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.plt[Phase].max_data.d,4);
            memcpy(pBuf+4,&s_pq_statsdata.plt[Phase].max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.plt[Phase].min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.plt[Phase].min_time,7);
            len=22;
            break;
        case U_DEV_DayCount:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
           /* new memcpy(pBuf,&s_pq_statsdata.vol_dev[Phase].max_data.l,4);
            memcpy(pBuf+4,&s_pq_statsdata.vol_dev[Phase].max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.vol_dev[Phase].min_data.l,4);
            memcpy(pBuf+15,&s_pq_statsdata.vol_dev[Phase].min_time,7);
            len=22;*/
			memcpy(pBuf,&s_pq_statsdata.vol_dev[Phase].max_data.l,4);
			memcpy(pBuf+4,&s_pq_statsdata.vol_dev[Phase].min_data.l,4);
            memcpy(pBuf+8,&s_pq_statsdata.vol_dev[Phase].max_data.l,4);
			memcpy(pBuf+12,&s_pq_statsdata.vol_dev[Phase].min_data.l,4);
			memcpy(pBuf+16,&s_pq_statsdata.vol_dev[Phase].min_data.l,4);
            len=20;

            break;
        case HZ_DEV_DayCount:
            memcpy(pBuf,&s_pq_statsdata.hz_dev.max_data.l,4);
            memcpy(pBuf+4,&s_pq_statsdata.hz_dev.max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.hz_dev.min_data.l,4);
            memcpy(pBuf+15,&s_pq_statsdata.hz_dev.min_time,7);
            len=22;
            break;
        case U_SEQ_DayCount:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_statsdata.vol_unbalance.max_data.d,4);
            memcpy(pBuf+4,&s_pq_statsdata.vol_unbalance.max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.vol_unbalance.min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.vol_unbalance.min_time,7);
            len=22;
            #endif
            break;
        case I_SEQ_DayCount:
            #if(MAX_PHASE == 3)
            memcpy(pBuf,&s_pq_statsdata.cur_unbalance.max_data,4);
            memcpy(pBuf+4,&s_pq_statsdata.cur_unbalance.max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.cur_unbalance.min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.cur_unbalance.min_time,7);
            len=22;
            #endif
            break;
        case UChange_DayCount:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.fluc[Phase].max_data.d,4);
            memcpy(pBuf+4,&s_pq_statsdata.fluc[Phase].max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.fluc[Phase].min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.fluc[Phase].min_time,7);
            len=22;
            break;
        case UChangeFre_DayCount:
            if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_statsdata.fluc_f[Phase].max_data.d,4);
            memcpy(pBuf+4,&s_pq_statsdata.fluc_f[Phase].max_time,7);
            memcpy(pBuf+11,&s_pq_statsdata.fluc_f[Phase].min_data.d,4);
            memcpy(pBuf+15,&s_pq_statsdata.fluc_f[Phase].min_time,7);
            len=22;
			break;
		case ExtremStat:
			len  = api_GetExtremeStatData(Times,Phase,pBuf);
			break;
		case ExtremStat3:
			len  = api_GetExtremeStatPara(Times,Phase,pBuf);
			break;
		case ProbabilityStat:
			len  = api_GetProbatyStatData(Phase,Times,pBuf);
			break;
		case ProbabilityStat3:
			len  = api_GetProbatyStatPara(Phase,Times,pBuf);
			break;
		case HarmUContent://200E0200—A相谐波
			if(Phase >= PH_NUM)
            {
                 return  0x8000;
            }
            memcpy(pBuf,&s_pq_data.vol.vi_harm_content[Phase][Times],2);
            len=2;
			break;
		case HarmIContent://200E0200—A相谐波电流含有率
			if(Phase >= PH_NUM)
            {
                 return 0x8000;
            }
            memcpy(pBuf,&s_pq_data.cur.vi_harm_content[Phase][Times],2);
            len=2;
			break;
		case IHarmUContent://20490200—A相间谐波电压含有率
			if(Phase >= PH_NUM)
            {
                 return 0x8000;
            }
            dwData = s_pq_data.vol.vi_intharm_content[Phase][Times];
            memcpy(pBuf,&dwData,2);
            len=2;
			break;
		case IHarmIContent://204A0200—A相间谐波电流含有率
			if(Phase >= PH_NUM)
            {
                 return 0x8000;
            }
            memcpy(pBuf,&s_pq_data.cur.vi_intharm_content[Phase][Times],2);
            len=2;
			break;
		case HGStat_A:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
			
			break;
						#if(MAX_PHASE == 3)
		case HGStat_B:
			if(Phase >= PH_NUM)
            {
				return  0x8000;
            }
			
			break;
		case HGStat_C:
			if(Phase >= PH_NUM)
            {
				return 0x8000;
            }
    
			break;
			#endif
		case FATStat:
                        dwData = 0x200A0200;
                        memcpy(pBuf,&dwData,4);
                        lib_ExchangeData(&pBuf[0], &pBuf[0], 4);
                     
			break;
                case BodongPara:
                        memcpy(&pBuf[nLen],&PQ_Para.dwBdThreshold,2);
                        len = 2;
                        break;
		case FATStatPara:
			nLen = 0;
			
			break;
		case MD_LIST:
			break;
        default:
            len=0x8000;
            break;
    }
    return len;
}

#define DEBUG_TEST	0
#if(MAX_PHASE == 3)
#define MAX_EXTREME_NUM		12
#else
#define MAX_EXTREME_NUM		5
#endif

WORD AddExTag(BYTE Tag, BYTE* pBuf, BYTE byLen)
{
	WORD wLen=0;
	
	pBuf[wLen++] = Tag;
	
	if(byLen != 0)
		pBuf[wLen++] = byLen;
	
	return wLen;
}


WORD AddExTagToData(BYTE Tag, BYTE* InBuf, BYTE* OutBuf, WORD* pwInLen, WORD* pwOutLen)
{
	WORD wLen=0;
	WORD wTagLen=0;

	OutBuf[wLen++] = Tag;

	wTagLen = GetBasicTypeLen(eData, &Tag);

	memcpy( OutBuf + wLen, InBuf , wTagLen );
	*pwOutLen += wTagLen+wLen;
	*pwInLen += wTagLen;

	return wLen;
}
WORD api_GetExtremeStatDataLen(BYTE Index)
{
	WORD len=0;
//    DWORD tempvalue=0;
//	TRealTimer stime;
//	DWORD dwOad;
//	WORD InLen=0;
	BYTE byNo, byNum=0;
	BYTE bySubIndex;

	if(Index == 0)
	{
		byNum = MAX_EXTREME_NUM;
		Index=1;
	}
	else
	{
		byNum = Index;
	}
for(byNo=Index; byNo<byNum+1;byNo++)
{
	bySubIndex = byNo;
	#if(MAX_PHASE != 3)
	switch(byNo)
	{
	case 2: bySubIndex = 4; break;
	case 3: bySubIndex = 7; break;
	case 4: bySubIndex = 10; break;
	case 5: bySubIndex = 13; break;
	case 6: bySubIndex = 16; break;
	}
	#endif

	switch(bySubIndex)
	{
	case 1:
	#if(MAX_PHASE == 3)
	case 2:
	case 3:
	#endif
		len += 4+2+7+2+7;

		break;
	case 4:		//200F0400——电网频率偏差
		len += 4+4+7+4+7;
		break;
	#if(MAX_PHASE == 3)
	case 5: 
		len += 4+2+7+2+7;
		break;
	case 6:
		len += 4+2+7+2+7;
		break;
	#endif
	case 7:
	#if(MAX_PHASE == 3)
	case 8:
	case 9:
	#endif
		len += 4+4+2+7+4+2+7;
		break;
	case 10://20370200——电压短时闪变
	#if(MAX_PHASE == 3)
	case 11:
	case 12:
	#endif
		len += 4+4+7+4+7;
		break;
	case 13:		//20380200——电压长时闪变
	#if(MAX_PHASE == 3)
	case 14:
	case 15:
	#endif
		len += 4+4+7+4+7;
		break;
	default:
		len = 0x8000;
		break;
	}
}
	return len;
}
WORD api_GetExtremeStatData_AddTag( BYTE Index, BYTE* InBuf, BYTE *OutBuf )
{
	WORD len=0;
//    DWORD tempvalue=0;
//	TRealTimer stime;
//	DWORD dwOad;
	WORD InLen=0;
	BYTE byNo, byNum=0;
	BYTE bySubIndex;

	if(Index == 0)
	{
		byNum = MAX_EXTREME_NUM;
		len += AddExTag(Array_698, &OutBuf[len], byNum);
		Index=1;
	}
	else
	{
		byNum = Index;
	}
for(byNo=Index; byNo<byNum+1;byNo++)
{
	bySubIndex = byNo;
	#if(MAX_PHASE != 3)
	switch(byNo)
	{
	case 2: bySubIndex = 4; break;
	case 3: bySubIndex = 7; break;
	case 4: bySubIndex = 10; break;
	case 5: bySubIndex = 13; break;
	case 6: bySubIndex = 16; break;
	}
	#endif

	switch(bySubIndex)
	{
	case 1:
	#if(MAX_PHASE == 3)
	case 2:
	case 3:
	#endif
		len += AddExTag(Structure_698, &OutBuf[len], 5);
		
		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);//OAD_698,                                4,      //

		AddExTagToData(Long_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);


		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Long_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		break;
	case 4:		//200F0400——电网频率偏差
		len += AddExTag(Structure_698, &OutBuf[len], 5);

		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	#if(MAX_PHASE == 3)
	case 5: 
		len += AddExTag(Structure_698, &OutBuf[len], 5);

		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	case 6:
		len += AddExTag(Structure_698, &OutBuf[len], 5);
		
		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		
		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		
		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	#endif
	case 7:
	#if(MAX_PHASE == 3)
	case 8:
	case 9:
	#endif
		len += AddExTag(Structure_698, &OutBuf[len], 5);
	
		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		len += AddExTag(Structure_698, &OutBuf[len], 2);
		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		len += AddExTag(Structure_698, &OutBuf[len], 2);
		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(Long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	case 10:
	#if(MAX_PHASE == 3)
	case 11:
	case 12:
	#endif
		len += AddExTag(Structure_698, &OutBuf[len], 5);

		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);

		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	case 13:		//20370200——电压短时闪变
	#if(MAX_PHASE == 3)
	case 14:
	case 15:
	#endif
		len += AddExTag(Structure_698, &OutBuf[len], 5);
	
		AddExTagToData(OAD_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
	  
		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		
		AddExTagToData(Double_long_unsigned_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		AddExTagToData(DateTime_S_698, &InBuf[InLen], &OutBuf[len], &InLen, &len);
		break;
	default:
		len = 0x8000;
		break;
	}
}
	return len;

}
WORD api_GetExtremeStatData( BYTE Index, BYTE Times, BYTE *pBuf )
{
	WORD len=0;
//    DWORD tempvalue=12345;
//	TRealTimer stime;
	DWORD dwOad;

	#if(MAX_PHASE != 3)
	switch(Index)
	{
	case 2: Index = 4; break;
	case 3: Index = 7; break;
	case 4: Index = 10; break;
	case 5: Index = 13; break;
	case 6: Index = 16; break;
	}
	#endif

	switch(Index)
	{
	case 1:
	#if(MAX_PHASE == 3)
	case 2:
	case 3:
	#endif
		dwOad = 0x20000700 + Index;		//20000700——电压偏差
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.vol_dev[Index-1].max_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;
#if(DEBUG_TEST)
		get_sys_time(&stime);
		memcpy(pBuf+len,&stime,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
			
	//	tempvalue = 0;
		memcpy(pBuf+len,&tempvalue,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;
			
		get_sys_time(&stime);
		memcpy(pBuf+len,&stime,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
#else
		memcpy(pBuf+len,&s_pq_statsdata.vol_dev[Index-1].max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		
		memcpy(pBuf+len,&s_pq_statsdata.vol_dev[Index-1].min_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;
		
		memcpy(pBuf+len,&s_pq_statsdata.vol_dev[Index-1].min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
#endif
		break;
	case 4:		//200F0400——电网频率偏差
		dwOad = 0x200F0400;

		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.hz_dev.max_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.hz_dev.max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;

		memcpy(pBuf+len,&s_pq_statsdata.hz_dev.min_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.hz_dev.min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
	#if(MAX_PHASE == 3)
	case 5: 
		dwOad = 0x20260400;		//20260400——电压负序不平衡度（仅三相表模组支持）
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.vol_unbalance.max_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;

		memcpy(pBuf+len,&s_pq_statsdata.vol_unbalance.max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;

		memcpy(pBuf+len,&s_pq_statsdata.vol_unbalance.min_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;

		memcpy(pBuf+len,&s_pq_statsdata.vol_unbalance.min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
	case 6:
		dwOad = 0x20270400;		//20270400——电流负序不平衡度（仅三相表模组支持）
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.cur_unbalance.max_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;

		memcpy(pBuf+len,&s_pq_statsdata.cur_unbalance.max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		
		memcpy(pBuf+len,&s_pq_statsdata.cur_unbalance.min_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;
		
		memcpy(pBuf+len,&s_pq_statsdata.cur_unbalance.min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
	#endif
	case 7:
	#if(MAX_PHASE == 3)
	case 8:
	case 9:
	#endif
		dwOad = 0x20350300 + Index - 6;//20350200——电压波动量
	
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].max_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len,4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].max_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].min_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].min_data,2);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 2;

		memcpy(pBuf+len,&s_pq_statsdata.pst[Index-7].min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
	case 10:
	#if(MAX_PHASE == 3)
	case 11:
	case 12:
	#endif
		dwOad = 0x20370200 + Index - 9;	
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc[Index-10].max_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc[Index-10].max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;

		memcpy(pBuf+len,&s_pq_statsdata.fluc[Index-10].min_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc[Index-10].min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
	case 13:		//20380200——电压长时闪变
#if(MAX_PHASE == 3)
	case 14:
	case 15:
	#endif
		dwOad = 0x20380200 + Index - 12;
		memcpy(pBuf+len,&dwOad,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc_f[Index-13].max_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc_f[Index-13].max_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;

		memcpy(pBuf+len,&s_pq_statsdata.fluc_f[Index-13].min_data,4);
		lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		memcpy(pBuf+len,&s_pq_statsdata.fluc_f[Index-13].min_time,7);
		lib_ExchangeData( pBuf+len, pBuf+len, 2 );
		len += 7;
		break;
		
	default:
		len = 0x8000;
		break;
	}
	
	return len;
}
WORD api_GetExtremeStatPara( BYTE Index, BYTE Times, BYTE *pBuf )
{
	WORD len=0;
//    DWORD tempvalue=12345;
//	TRealTimer stime;
	DWORD dwOad;

	PQ_Para.byDayJZPeriod = 1;
	PQ_Para.dwDayJZTi = 3<<8;

	#if(MAX_PHASE != 3)
	switch(Index)
	{
	case 2: Index = 4; break;
	case 3: Index = 7; break;
	case 4: Index = 10; break;
	case 5: Index = 13; break;
	case 6: Index = 16; break;
	}
	#endif

	switch(Index)
	{
	case 1:
	#if(MAX_PHASE == 3)
	case 2:
	case 3:
	#endif
		dwOad = 0x20000700 + Index;		//20000700——电压偏差
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;

		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	case 4:		//200F0400——电网频率偏差
		dwOad = 0x200F0400;
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	#if(MAX_PHASE == 3)
	case 5: 
		dwOad = 0x20260400;		//20260400——电压负序不平衡度（仅三相表模组支持）
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	case 6:
		dwOad = 0x20270400;		//20270400——电流负序不平衡度（仅三相表模组支持）
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	#endif
	case 7:
	#if(MAX_PHASE == 3)
	case 8:
	case 9:
	#endif
		dwOad = 0x20350300 + Index - 6;//20350200——电压波动量
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	case 10:
	#if(MAX_PHASE == 3)
	case 11:
	case 12:
	#endif
		dwOad = 0x20370200 + Index - 9;	
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
	case 13:		//20380200——电压长时闪变
#if(MAX_PHASE == 3)
	case 14:
	case 15:
	#endif
		dwOad = 0x20380200 + Index - 12;
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		memcpy(pBuf+len,&PQ_Para.byDayJZPeriod,1);
		len += 1;
		
		memcpy(pBuf+len,&PQ_Para.dwDayJZTi,3);
		len += 3;
		break;
		
	default:
		len = 0x8000;
		break;
	}
	
	return len;
}

WORD api_GetProbatyStatData( BYTE Index, BYTE Times, BYTE *pBuf )
{
	WORD len=0;
//    DWORD tempvalue=0;
	int i;
//	TRealTimer stime;
	DWORD dwOad;

#if(MAX_PHASE != 3)
	Index = (((Index-1) * 3) + 1);
#endif
	
	switch(Index)
	{
	case 1:	 //20460200——A相谐波电压（2-25次）	20460300——B相谐波电压（2-25次）	20460400——C相谐波电压（2-25次）
#if(MAX_PHASE == 3)
	case 2:
	case 3:
#endif
		dwOad = 0x20460200 + ((Index-1)<<8);		
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		for(i=0; i<24; i++)
		{
			memcpy(pBuf+len,&s_pq_statsdata.p95_harm_vol[Index-1][i],4);
			len += 4;
		}
		
		break;
	case 4:		//20470200——A相谐波电流（2-25次）	20470300——B相谐波电流（2-25次）20470400——C相谐波电流（2-25次）
#if(MAX_PHASE == 3)
	case 5:
	case 6:
#endif
		dwOad = 0x20470200 + ((Index-4)<<8);
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;

		for(i=0; i<24; i++)
		{
			memcpy(pBuf+len,&s_pq_statsdata.p95_harm_cur[Index-4][Times],4);
			len += 4;
		}
	
		break;
	case 7: // 20600200——A相间谐波电压（0.5-20.5次）20600300——B相间谐波电压（0.5-20.5次）20600400——C相间谐波电压（0.5-20.5次）
#if(MAX_PHASE == 3)
	case 8:
	case 9:
#endif
		dwOad = 0x20600200 + ((Index-7)<<8);		
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		for(i=0; i<24; i++)
		{
			memcpy(pBuf+len,&s_pq_statsdata.p95_iharm_vol[Index-7][Times],4);
			len += 4;
		}
	
		break;
	case 10:		//20610200——A相间谐波电流（0.5-20.5次）20610300——B相间谐波电流（0.5-20.5次）20610400——C相间谐波电流（0.5-20.5次）
#if(MAX_PHASE == 3)
	case 11:
	case 12:
#endif
		dwOad = 0x20610200 + ((Index-10)<<8);		//
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		
		for(i=0; i<24; i++)
		{
			memcpy(pBuf+len,&s_pq_statsdata.p95_iharm_cur[Index-10][Times],4);
			len += 4;
		}
		break;
	default:
		len = 0x8000;
		break;
	}


	return len;
}

WORD api_GetProbatyStatPara( BYTE Index, BYTE Times, BYTE *pBuf )
{

	WORD len=0;
//    DWORD tempvalue=0;
//	int i;
//	TRealTimer stime;                                                      
	DWORD dwOad;

	PQ_Para.byDayTJPeriod = 1;
	PQ_Para.dwDayTJTi = 3<<8;
	PQ_Para.byDayTJConLevel = 95;
	PQ_Para.byDayTJType = 0;
#if(MAX_PHASE != 3)
	Index = (((Index-1) * 3) + 1);
#endif
	
	switch(Index)
	{
	case 1:	 //20460200——A相谐波电压（2-25次）	20460300——B相谐波电压（2-25次）	20460400——C相谐波电压（2-25次）
#if(MAX_PHASE == 3)
	case 2:
	case 3:
#endif
		dwOad = 0x20460200 + ((Index-1)<<8);		
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		memcpy(pBuf+len,&PQ_Para.byDayTJPeriod,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.dwDayTJTi,3);
		len += 3;
		memcpy(pBuf+len,&PQ_Para.byDayTJConLevel,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.byDayTJType,1);
		len += 1;
		break;
	
	case 4:		//20470200——A相谐波电流（2-25次）	20470300——B相谐波电流（2-25次）20470400——C相谐波电流（2-25次）
#if(MAX_PHASE == 3)
	case 5:
	case 6:
#endif
		dwOad = 0x20470200 + ((Index-4)<<8);
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		memcpy(pBuf+len,&PQ_Para.byDayTJPeriod,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.dwDayTJTi,3);
		len += 3;
		memcpy(pBuf+len,&PQ_Para.byDayTJConLevel,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.byDayTJType,1);
		len += 1;
		break;
	case 7: // 20600200——A相间谐波电压（0.5-20.5次）20600300——B相间谐波电压（0.5-20.5次）20600400——C相间谐波电压（0.5-20.5次）
#if(MAX_PHASE == 3)
	case 8:
	case 9:
#endif
		dwOad = 0x20600200 + ((Index-7)<<8);		
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		memcpy(pBuf+len,&PQ_Para.byDayTJPeriod,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.dwDayTJTi,3);
		len += 3;
		memcpy(pBuf+len,&PQ_Para.byDayTJConLevel,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.byDayTJType,1);
		len += 1;
		break;
	case 10:		//20610200——A相间谐波电流（0.5-20.5次）20610300——B相间谐波电流（0.5-20.5次）20610400——C相间谐波电流（0.5-20.5次）
#if(MAX_PHASE == 3)
	case 11:
	case 12:
#endif
		dwOad = 0x20610200 + ((Index-10)<<8);		//
		memcpy(pBuf+len,&dwOad,4);
                lib_ExchangeData( pBuf+len, pBuf+len, 4 );
		len += 4;
		memcpy(pBuf+len,&PQ_Para.byDayTJPeriod,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.dwDayTJTi,3);
		len += 3;
		memcpy(pBuf+len,&PQ_Para.byDayTJConLevel,1);
		len += 1;
		memcpy(pBuf+len,&PQ_Para.byDayTJType,1);
		len += 1;
		break;
	default:
		len = 0x8000;
		break;
	}

	return len;

}

