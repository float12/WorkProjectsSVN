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


static SYS_PARA_1808 BackupPara;    //最后一次参数的备份
/**************************************************************************
*
*  实时数据区
*
***************************************************************************/

// static PQ_DATA s_pq_data;
PQ_DATA s_pq_data;              // TODO: 仿真用，删除静态关键字

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
            return ;
        case PQ_U_DC:   // 电压直流分量
            memcpy(s_pq_data.vol.vi_dc, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_I_RMS:  // 电流有效值
            memcpy(s_pq_data.cur.vi_mag, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_I_DC:   // 电流直流分量
            memcpy(s_pq_data.cur.vi_dc, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_P:      // 有功功率
            memcpy(s_pq_data.cur.vi_union.c.P, val, PH_NUM*sizeof(DWORD));
            #if(MAX_PHASE==3)
            s_pq_data.cur.vi_union.c.P[PH_NUM] = s_pq_data.cur.vi_union.c.P[PH_A]+s_pq_data.cur.vi_union.c.P[PH_B]+s_pq_data.cur.vi_union.c.P[PH_C];
            #elif(MAX_PHASE==1)
            s_pq_data.cur.vi_union.c.P[PH_NUM] = s_pq_data.cur.vi_union.c.P[PH_A];
            #endif
            return ;
        case PQ_Q:      // 无功功率
            memcpy(s_pq_data.cur.vi_union.c.Q, val, PH_NUM*sizeof(DWORD));
            #if(MAX_PHASE==3)
            s_pq_data.cur.vi_union.c.Q[PH_NUM] = s_pq_data.cur.vi_union.c.Q[PH_A]+s_pq_data.cur.vi_union.c.Q[PH_B]+s_pq_data.cur.vi_union.c.Q[PH_C];
            #elif(MAX_PHASE==1)
            s_pq_data.cur.vi_union.c.Q[PH_NUM] = s_pq_data.cur.vi_union.c.Q[PH_A];
            #endif
            return ;
        case PQ_U_FREQ:/*频率*/
            memcpy(&s_pq_data.vol.vi_union.v.hz, val, sizeof(DWORD));
            return ;
        case PQ_U_PST:/*短时闪变*/
            memcpy(s_pq_data.vol.vi_union.v.pst, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_U_PLT:/*长时闪变*/
            memcpy(s_pq_data.vol.vi_union.v.plt, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_U_FLUC:/*电压波动*/
            memcpy(s_pq_data.vol.vi_union.v.fluc, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_U_FLUCF:/*波动频率*/
            memcpy(s_pq_data.vol.vi_union.v.fluc_f, val, PH_NUM*sizeof(DWORD));
            return ;
        case PQ_FUND_P:/*基波有功功率*/
            memcpy(s_pq_data.harm_data.fundpower.P, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_FUND_Q:/*基波无功功率*/
            memcpy(s_pq_data.harm_data.fundpower.Q, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_HARM_POS_P_TOTAL:/*2-50次总谐波正向有功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.posP, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_HARM_REV_P_TOTAL:/*2-50次总谐波反向有功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.revP, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_HARM_POS_Q_TOTAL:/*2-50次总谐波正向无功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.posQ, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_HARM_REV_Q_TOTAL:/*2-50次总谐波反向无功功率*/
            memcpy(s_pq_data.harm_data.harmpower_total.revQ, val, (PH_NUM+1)*sizeof(FLOAT));
            return ;
        case PQ_HARM_P:/*2-50次谐波有功功率*/
            memcpy(s_pq_data.harm_data.harmpower.P, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            return ;
        case PQ_HARM_Q:/*2-50次谐波无功功率*/
            memcpy(s_pq_data.harm_data.harmpower.Q, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            return ;
        case PQ_IHARM_P:/*2-50次间谐波有功功率*/
            memcpy(s_pq_data.harm_data.iharmpower.P, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            return ;
        case PQ_IHARM_Q:/*2-50次间谐波无功功率*/
            memcpy(s_pq_data.harm_data.iharmpower.Q, val, (PH_NUM+1)*(HAMONIC_ANYSIS_NUM)*sizeof(FLOAT));
            return ;
        default:
            return ;
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
    long long tmp64;
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
            memcpy(&tmp64,data,sizeof(long long));
            tmp64 *=(SysPara.big_adjust_para.P_factor[ph] * 10000);     // 保留4位小数，乘10000
            return (int)(tmp64);
        case PQ_Correct_Q://无功传入是long long型
            memcpy(&tmp64,data,sizeof(long long));
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
    memcpy(&BackupPara, &SysPara, sizeof(SYS_PARA_1808));   //参数备份
    memset(&s_pq_data, 0x00, sizeof(s_pq_data));

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

WORD api_GetModuleData( BYTE ModuleType, BYTE Phase, BYTE Times, BYTE *pBuf )
{
    WORD len=0x8000;
    DWORD tempvalue=0;

    if( (Phase >= PH_NUM) ||(Times >= HAMONIC_ANYSIS_NUM) )
    {
        return 0;
    }

    switch(ModuleType)
    {
        case U:
            memcpy(pBuf,&s_pq_data.vol.vi_mag[Phase],4);
            len=4;
            break;
        case I:
            memcpy(pBuf,&s_pq_data.cur.vi_mag[Phase],4);
            len=4;
            break;
        case UChange:
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.fluc[Phase],4);
            memcpy(pBuf+4,&s_pq_data.vol.vi_union.v.fluc_f[Phase],4);
            len=8;
            break;
        case UFlicker:
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.pst[Phase],4);
            memcpy(pBuf+4,&s_pq_data.vol.vi_union.v.plt[Phase],4);
            len=8;
            break;
        case HZ:
            memcpy(pBuf,&s_pq_data.vol.vi_union.v.hz,4);
            len=4;
            break;
        case UDayFlickerCount:
            memcpy(pBuf,&s_pq_statsdata.pst[Phase],20);
            memcpy(pBuf+20,&s_pq_statsdata.plt[Phase],20);
            len=40;
            break;
        case HarmonicU:
            memcpy(pBuf,&s_pq_data.vol.vi_harm_mag[Phase][Times],4);
            len=4;
            break;
        case HarmonicI:
            memcpy(pBuf,&s_pq_data.cur.vi_harm_mag[Phase][Times],4);
            len=4;
            break;
        case HarmonicP:
            tempvalue=(long)s_pq_data.harm_data.harmpower.P[Phase][Times];
            memcpy(pBuf,&tempvalue,4);
            len=4;
            break;
        case IHarmonicU:
            memcpy(pBuf,&s_pq_data.vol.vi_intharm_mag[Phase][Times],4);
            len=4;
            break;
        case IHarmonicI:
            memcpy(pBuf,&s_pq_data.cur.vi_intharm_mag[Phase][Times],4);
            len=4;
            break;
        case IHarmonicP:
            tempvalue=(long)s_pq_data.harm_data.iharmpower.P[Phase][Times];
            memcpy(pBuf,&tempvalue,4);
            len=4;
            break;
        case HarmonicUDayCount:
            memcpy(pBuf,&s_pq_statsdata.harm_vol[Phase][Times],20);
            len=20;
            break;
        case HarmonicIDayCount:
            memcpy(pBuf,&s_pq_statsdata.harm_cur[Phase][Times],20);
            len=20;
            break;
        case IHarmonicUDayCount:
            memcpy(pBuf,&s_pq_statsdata.iharm_vol[Phase][Times],20);
            len=20;
            break;
        case IHarmonicIDayCount:
            memcpy(pBuf,&s_pq_statsdata.iharm_cur[Phase][Times],20);
            len=20;
            break;
        case HarmonicPALL:
            tempvalue=(DWORD)s_pq_data.harm_data.harmpower_total.posP[Phase];
            memcpy(pBuf,&tempvalue,4);
            tempvalue=(DWORD)s_pq_data.harm_data.harmpower_total.revP[Phase];
            memcpy(pBuf+4,&tempvalue,4);
            len=8;
            break;
        default:
            len=0x8000;
            break;
    }
    return len;
}
