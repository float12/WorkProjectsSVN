
#include "GDW698.h"
#include "CommFunc_API.h"
#include "AllHead.h"

#if(MD_BASEUSER	== MD_GDW698)
#define SECTOR_SIZE		4096
// static DWORD dwParaFreshTime;
extern TMetCfg_Base gMetCfg[];
extern BOOL OAD2PortNo(DWORD *pdwOAD,BYTE *pbyPortNo,BOOL bDirect);

extern BOOL DeleteChargingMap(BYTE);
extern BOOL ADDChargingMap(BYTE);
TTask698RunInfo *g_pTaskRunInfo=NULL;
static TTask698RunInfo _s_TaskRunInfos[MAX_698_TASK_NUM];
static TSysParaCfg _s_SysParaCfg;

// typedef struct _TSchIndexTab{
// 	BYTE	GSchTab[MAX_SCH_GENERAL];
// 	BYTE	Flag_GSchTab[(MAX_SCH_GENERAL+7)/8];
// 	BYTE	ESchTab[MAX_SCH_EVENT];
// 	BYTE	Flag_ESchTab[(MAX_SCH_EVENT+7)/8];
// 	BYTE	RSchTab[MAX_SCH_REPORT];
// 	BYTE	Flag_RSchTab[(MAX_SCH_REPORT+7)/8];
// 	BYTE	TSchTab[MAX_SCH_TRANS];
// 	BYTE	Flag_TSchTab[(MAX_SCH_TRANS+7)/8];
// }TSchIndexTab;

// TSchIndexTab s_SchIndexTab={0};
// const TOAD_LEN s_OADLen[]={	
// 	{0	,sizeof(TsysCfgOther)},
// 	{0x45000200,sizeof(TOad45000200)},		//����ͨѶģ��
// 	{0x45000300,sizeof(TOad45000300)},		//��վ����
// 	{0x45000400,sizeof(TOad45000400)},		//���Ų���
// 	{0x45100200,sizeof(TOad45100200)},		//��̫��ͨ��ģ��
// 	{0x45100300,sizeof(TOad45100300)},      //��վ����
// 	{0x45100400,sizeof(TOad45100400)},		//�ն�IP
// 	{0x45100500,sizeof(TOad45100500)},      //mac
// 	{0x40010200,sizeof(TOad40010200)},		//�ն˵�ַ
// 	{0x40000300,sizeof(TOad40000300)},		//Уʱģʽ
// 	{0x40000400,sizeof(TOad40000400)},		//��׼Уʱ����	
// 	{0x42040200,sizeof(TOad42040200)},		//�ն˹㲥Уʱ����
// 	{0x42040300,sizeof(TOad42040300)},		//�ն˵���ַ�㲥Уʱ����	
// 	{0x40040200,sizeof(TOad40040200)},		//�豸����λ��
// 	{0x40050200,sizeof(TOad40050200)},		//�ն����ַ
// 	{0x40070200,sizeof(TOad40070200)},		//LCD����
// //.	{0x40240200,sizeof(TOad40240200)},		//�ն��޳�
// //.	{0x40250200,sizeof(TOad40250200)},		//�ɼ����������Ʋ��� ����������Լ�г�ͻ
// 	{0x40300200,sizeof(TOad40300200)},		//��ѹ�ϸ��ʲ���
// //.	{0x300F0600,sizeof(TOad300F0600)},		//���ܱ��ѹ����������6
// 	{0x31050600,sizeof(TOad31050600)},		//���ܱ�ʱ�ӳ���������6
// 	{0x31060600,sizeof(TOad31060600)},		//�ն�ͣ/�ϵ��¼����ò���
// 	{0x310B0600,sizeof(TOad310B0600)},		//���ܱ�ʾ���½����ò���
// 	{0x310C0600,sizeof(TOad310C0600)},		//�����������¼����ò���
// 	{0x310D0600,sizeof(TOad310D0600)},		//���ܱ�����¼�
// 	{0x310E0600,sizeof(TOad310E0600)},		//���ܱ�ͣ���¼�
// 	{0x310F0600,sizeof(TOad310F0600)},		//�ն˳���ʧ���¼�
// 	{0x31100600,sizeof(TOad31100600)},		//��ͨ�����������¼�
// 	{0x311C0600,sizeof(TOad311C0600)},		//���ܱ����ݱ����ؼ�¼
// //.	{0x42020200,sizeof(TOad42020200)},		//����ͨ�Ų���	
// 	{0xF1010200,sizeof(TOadF1010200)},		//��ȫģʽѡ��
// 	{0xF2050200,sizeof(TOadF2050200)},		//�̵�������
// 	{0xF2060400,sizeof(TOadF2060400)},		//�澯ʱ��
// 	{0xF1010300,sizeof(TOadF1010300)},		//��ʽ��ȫģʽ����
// 	{0xF2030400,sizeof(TOadF2030400)},		//����������
// 	{0xF2010200,sizeof(TOadF2010200)},		//���3·485
// 	{0x24010200,sizeof(TOad24010200)},		//��1�����������
// 	{0x24020200,sizeof(TOad24010200)},		//��2�����������
// 	{0x31010300,sizeof(TOad31010300)},		//�ն˰汾����¼�
// 	{0x31040300,sizeof(TOad31040300)},		//�ն�״̬����λ�¼�
// 	{0x31090300,sizeof(TOad31090300)},		//��Ϣ��֤�����¼
// 	{0x310B0300,sizeof(TOad310B0300)},		//���ܱ�ʾ���½�
// 	{0x310C0300,sizeof(TOad310C0300)},		//����������
// 	{0x310D0300,sizeof(TOad310D0300)},		//���ܱ����
// 	{0x310E0300,sizeof(TOad310E0300)},		//���ܱ�ͣ��
// 	{0x310F0300,sizeof(TOad310F0300)},		//����ʧ��
// 	{0x31100300,sizeof(TOad31100300)},		//�ն�����վͨ������������
// 	{0x31140300,sizeof(TOad31140300)},		//��ʱ�¼���¼
// 	{0x31190300,sizeof(TOad31190300)},		//�ն˵�����·�쳣�¼�
// 	{0x32000300,sizeof(TOad32000300)},		//������բ��¼
// 	{0x32010300,sizeof(TOad32010300)},		//�����բ��¼
// 	{0x32020300,sizeof(TOad32020300)},		//����������ü�¼

// 	{0x30000200,sizeof(TOad30000200)},		//���ܱ�ʧѹ�¼�
// 	{0x30010200,sizeof(TOad30010200)},		//���ܱ�Ƿѹ�¼�
// 	{0x30020200,sizeof(TOad30020200)},		//���ܱ��ѹ�¼�
// 	{0x30030200,sizeof(TOad30030200)},		//���ܱ�����¼�
// 	{0x30040200,sizeof(TOad30040200)},		//���ܱ�ʧ���¼�
// 	{0x30050200,sizeof(TOad30050200)},		//���ܱ�����¼�
// //.	{0x30060200,sizeof(TOad30060200)},		//���ܱ�����¼�
// 	{0x42020200,sizeof(TOad42020200)},		//��������
// 	{0x41030200,sizeof(TOad41030200)}		//�ʲ��������
// };

// static const WORD wChargingPileIO[]=
// {
// 	0x2700,0x2701,0x2702,0x2704,0x2705,0x2706,0x2707,0x2708,0x2709,0x4600,
// 	0x4601,0x4602,0x4603,0x4702
// };

// WORD get_sys_oad_para_size(DWORD objOAD)
// {
// DWORD dwInd;

// 	for(dwInd=0;dwInd<ArraySize(s_OADLen);++dwInd) 
// 	{
// 		if( objOAD == s_OADLen[dwInd].obj )
// 		{
// 			if (s_OADLen[dwInd].wLen != INVALID_LEN )
// 				return s_OADLen[dwInd].wLen;
// 			break;
// 		}
// 	}
// 	if(dwInd == ArraySize(s_OADLen))
// 	{//û�ж�Ӧ��Fn 
// 		return 0;
// 	}
// //	Trace("��Ӧ�õ���oad_para_size %x", objOAD);
// 	return 0;
// }

DWORD getParaFileOffset(WORD wOffsetPage,BYTE byType)
{
	switch(byType)
	{
	//"SYSPARA.DAT" ,ƫ�Ƽ�ABC_XXXX_XXX.H
	// case FLAG_SYS:		return ( SYSCFG_ADDR ); //4K �ռ�
	// case FLAG_FKSYS:	return (FKCFG_ADDR+ (wOffsetPage*SIZE_PAGE));//4k�ռ�
	// case FLAG_SCHEME:	return ( SYSCFG_SCHEME_ADDR + (wOffsetPage*SIZE_PAGE) );	//160K
	// case FLAG_TASK:		return ( TASKCFG_ADDR );//8K
	// case FLAG_SYS_EX:	return SYSCFG_EX_ADDR;	//2K
	//"MPPARA.DAT"	
	// case FLAG_METER:	return ( MPCFG_ADDR   +(wOffsetPage*SECTOR_SIZE) );	//128K(���1536��������)
	case FLAG_METER:	return ( FLASH_FREEZE_END_ADDR   +(wOffsetPage*SECTOR_SIZE) );	//128K(���1536��������)
	default:	break;
	}
	return 0xFFFFFFFF;
}

// WORD getSys698ParaIndex(DWORD oadObj )
// {//�����ڿ��ڵ�ƫ��
// WORD wInd = 0;
// WORD wSumOffset=0;

// 	for (wInd=0;wInd<ArraySize(s_OADLen); ++wInd)
// 	{
// 		if(s_OADLen[wInd].obj == oadObj)
// 			break;		
// 		wSumOffset += s_OADLen[wInd].wLen;
// 	}
// 	if (wInd >= ArraySize(s_OADLen))
// 		return 0xFFFF;
// 	return wSumOffset;
// }


// TBase698ParaCfg *pGet698BasePara(void)
// {
//    return &gpSysPara->base_para_cfg;
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : sys_Para698_Read
//	�������� : ����������(ϵͳ)
//	������� : 
//	��    ע : 
//	��    �� : ����Ծ
//	ʱ    �� : 2017��8��11��
//	�� �� ֵ : BOOL
//	����˵�� : DWORD oad_index,
//				HPARA hPARA
///////////////////////////////////////////////////////////////
// BOOL sys_Para698_Read(DWORD oad_index,HPARA hPARA)
// {
// WORD wOffset=getSys698ParaIndex(oad_index);
// WORD  wSize=get_sys_oad_para_size(oad_index);
// TBase698ParaCfg *pSysCfg=pGet698BasePara();	
// BYTE byClass;

// 	if(0xFFFF == wOffset)
// 		return FALSE;
// 	if((WORD)(wOffset+wSize) > sizeof(pSysCfg->base_para_buf))
// 		return FALSE;
// 	if(oi2class(HIWORD(oad_index),&byClass))
// 	{
// 		if(((byClass == 7)&&(LHBYTE(oad_index) == 3))
// 			||((byClass == 24)&&(LHBYTE(oad_index) == 2)))
// 		{//�¼�������⴦��
// 			//return sys_Event698_Read(oad_index,hPARA);
// 			return FALSE;
// 		}
// 	}	
// 	memcpy(hPARA,&pSysCfg->base_para_buf[wOffset],wSize);	
// 	return TRUE;
// }

// BOOL sys_Para698_Write(DWORD oad_index,HPARA hPARA)
// {
// WORD wOffset=getSys698ParaIndex(oad_index);
// WORD  wSize=get_sys_oad_para_size(oad_index);
// TBase698ParaCfg *pSysCfg=pGet698BasePara();	

// 	if(0xFFFF == wOffset)
// 		return FALSE;
// 	if((WORD)(wOffset+wSize) > sizeof(pSysCfg->base_para_buf))
// 		return FALSE;
// 	memcpy(&pSysCfg->base_para_buf[wOffset],hPARA,wSize);
// 	_freshPara(FLAG_SYS);

// 	return TRUE;
// }

TMP698ParaCfg *pGet698MPPara(void)
{
   return &gpSysPara->mp_para_cfg;
}

static WORD _freshGetMPBaseOff698(WORD wMPSeqNo)
{
TMP698ParaCfg *pPara = pGet698MPPara();
BYTE byOffsetPage = 0;
BYTE byMPMaxSize = SECTOR_SIZE/sizeof(TOad60010200);
WORD wOffset=0;

	byOffsetPage= ( (wMPSeqNo-1)/byMPMaxSize );
	wOffset		= ( (wMPSeqNo-1)%byMPMaxSize );
	if(pPara->nowPageNum != byOffsetPage)
	{//ҳ�벻һ�»�ҳ		
		TFileRead FR;		
		if(pPara->saveflag)
		{//�������������
			TFileWrite FW;			
			FW.idwBufLen	= SECTOR_SIZE;
			FW.idwOffset	= getParaFileOffset(pPara->nowPageNum,FLAG_METER);
			FW.ipvBuf		= pPara->rtu_para_buf;
			FW.iszName		= FILE_MPPARA;
			FileWrite(&FW);
			pPara->saveflag = FALSE;			
		}		
		FR.idwBufLen	= SECTOR_SIZE;
		FR.idwOffset	= getParaFileOffset(byOffsetPage,FLAG_METER);
		FR.ipvBuf		= pPara->rtu_para_buf;
		FR.iszName		= FILE_MPPARA;
		FileRead(&FR);
		pPara->nowPageNum = byOffsetPage;
	}	
	return wOffset*sizeof(TOad60010200);
}

///////////////////////////////////////////////////////////////
//	�� �� �� : GetMP698Para
//	�������� : ��õ������������
//	������� : 
//	��    ע : 
//	��    �� : ����Ծ
//	ʱ    �� : 2017��7��3��
//	�� �� ֵ : BOOL
//	����˵�� : WORD wMPSeqNo,
//				TOad60010200 *pPara
///////////////////////////////////////////////////////////////
BOOL GetMP698Para(WORD wMPSeqNo,TOad60010200 *pPara)
{
TMP698ParaCfg *pmpPara=pGet698MPPara();
WORD wOffset;
BOOL bRc=FALSE;
	
	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
		return FALSE;
	// SM_P(MPV_PARA_MP);
	wOffset	= _freshGetMPBaseOff698(wMPSeqNo);
	if(wOffset < SECTOR_SIZE) 
	{
		bRc	= TRUE;
		memcpy(pPara,&pmpPara->rtu_para_buf[wOffset],sizeof(TOad60010200));
	}		
	// SM_V(MPV_PARA_MP);
	return bRc;
}

// void set_mp_safemode(WORD wMPSeqNo,BOOL bUse)
// {
// TData_6203 *pSys=pGetDF6203();

// 	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
// 		return;
// 	if(bUse)
// 		pSys->safmodeValid[wMPSeqNo>>3]	|= 1<<(wMPSeqNo&0x07);
// 	else
// 		pSys->safmodeValid[wMPSeqNo>>3]	&= ~(1<<(wMPSeqNo&0x07));
// }

// BOOL SafeModeIsInit(WORD wMPSeqNo)
// {
// TData_6203 *pSys=pGetDF6203();

// #if(MOS_MSVC)
// 	return TRUE;
// #endif
// 	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
// 		return FALSE;
// 	return	((pSys->safmodeInit[wMPSeqNo>>3] & (1<<(wMPSeqNo&0x07))) != 0);
// }

// BOOL IsUseSafeMode(WORD wMPSeqNo)
// {
// TData_6203 *pSys=pGetDF6203();

// 	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
// 		return FALSE;
// 	return	((pSys->safmodeValid[wMPSeqNo>>3] & (1<<(wMPSeqNo&0x07))) != 0);
// }

// void write_mp_meterno(WORD wMPSeqNo,BYTE *meterno)
// {
// TMP698ParaCfg *pmpPara=pGet698MPPara();
// WORD wOffset;
// TOad60010200 *pCfg;
// // TData_6203 *pSys=pGetDF6203();
	
// 	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
// 		return;
// 	// SM_P(MPV_PARA_MP);
// 	wOffset	= _freshGetMPBaseOff698(wMPSeqNo);
// 	if(wOffset < SIZE_PAGE) 
// 	{
// 		pCfg	= (TOad60010200*)&pmpPara->rtu_para_buf[wOffset];		
// 		if(memcmp(pCfg->MeterNo,meterno,8))
// 		{
// 			memcpy(pCfg->MeterNo,meterno,8);
// 			_freshPara(FLAG_METER);
// 		}
// 		pSys->safmodeInit[wMPSeqNo>>3]	|= 1<<(wMPSeqNo&0x07);
// 	}		
// 	SM_V(MPV_PARA_MP);
// }

BOOL add_mp_cfg(WORD wMPSeqNo,TOad60010200 *pPara)
{
	TMP698ParaCfg *pmpPara=pGet698MPPara();
	WORD wOffset;
	BOOL bRc=FALSE;
	BYTE i,bCanID;
	
	if((wMPSeqNo == 0)||( wMPSeqNo>= MAX_MP_NUM))
		return FALSE;
	// SM_P(MPV_PARA_MP);
	wOffset	= _freshGetMPBaseOff698(wMPSeqNo);
	if(wOffset < SECTOR_SIZE) 
	{
		bRc	= TRUE;
        //Ĭ�ϱ�ŵ���ͨ�ŵ�ַ
        lib_ExchangeData(pPara->MeterNo,pPara->basic.tsa.addr,8);
		memcpy(&pmpPara->rtu_para_buf[wOffset],pPara,sizeof(TOad60010200));
		_freshPara(FLAG_METER);
	}		
	// SM_V(MPV_PARA_MP);
	{
		TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
		BYTE *pFlag;
		WORD wI;
		BYTE byPortNo;
		// TData_6203 *pSys=pGetDF6203();
			
		// pSys->safmodeValid[wMPSeqNo>>3]	&= ~(1<<(wMPSeqNo&0x07));
		// pSys->safmodeInit[wMPSeqNo>>3]	&= ~(1<<(wMPSeqNo&0x07));
		sys_InitMPCfg(wMPSeqNo,&gMetCfg[wMPSeqNo]);
        //����˿ڲ�������Ч��ʶ
		if(pMPFlagsCtrl->Flag_Valid[wMPSeqNo>>3] & 1<<(wMPSeqNo&0x07))
		{
			for(wI=0;wI<MAX_DWNPORT_NUM;wI++)
			{
				pFlag	= _pGetPortMPFlag((BYTE)wI);
								
				pFlag[wMPSeqNo>>3]	&= ~(1<<(wMPSeqNo&0x07));
			}	
		}
		pMPFlagsCtrl->Flag_Valid[wMPSeqNo>>3]	|= 1<<(wMPSeqNo&0x07);
		if(HIWORD(pPara->basic.port.value) == 0xF208)
		{//����
	
			pMPFlagsCtrl->wJCMPNo	= wMPSeqNo;
		}
		else if(OAD2PortNo(&pPara->basic.port.value,&byPortNo,TRUE))
		{
			if(pMPFlagsCtrl->wJCMPNo == wMPSeqNo)
				pMPFlagsCtrl->wJCMPNo = 0xFFFF;
			pFlag	= pGetPortMPFlag(byPortNo);			
		
			if(NULL != pFlag)
				pFlag[wMPSeqNo>>3]	|= (1<<(wMPSeqNo&0x07));
		}
	}
	if(bRc == TRUE)
	{
		if( pPara->annex.nNum != 0 )
		{
			for(i = 0; i < pPara->annex.nNum; i++)
			{
				if( ( pPara->annex.annexObj[i].oad.value == 0xF2210600 )
				&& ( pPara->annex.annexObj[i].value[0] == 0x11))
				{
					bCanID = pPara->annex.annexObj[i].value[1];
					ADDChargingMap(bCanID);
				}
			}
		}
	}
	return bRc;
}

void del_mp_cfg(WORD mp_seqno)
{
	TMP698ParaCfg *pmpPara=pGet698MPPara();
	WORD wOffset;
	TOad60010200 pPara;
	BYTE i,bCanID;

	GetMP698Para(mp_seqno,&pPara);
	
	if((mp_seqno == 0)||( mp_seqno>= MAX_MP_NUM))
		return;
	// SM_P(MPV_PARA_MP);
	wOffset	= _freshGetMPBaseOff698(mp_seqno);
	if(wOffset < SECTOR_SIZE) 
	{		
		memset(&pmpPara->rtu_para_buf[wOffset],0,sizeof(TOad60010200));
		_freshPara(FLAG_METER);
	}			
	// SM_V(MPV_PARA_MP);	

	TMPFlagsCtrl *pMPFlagsCtrl=pGetMPFlag();
	BYTE byI;
	// TData_6203 *pSys=pGetDF6203();
	// pSys->safmodeValid[mp_seqno>>3]	&= ~(1<<(mp_seqno&0x07));
	// pSys->safmodeInit[mp_seqno>>3]	&= ~(1<<(mp_seqno&0x07));
	pMPFlagsCtrl->Flag_Valid[mp_seqno>>3]	&= ~(1<<(mp_seqno&0x07));
	sys_InitMPCfg(mp_seqno,&gMetCfg[mp_seqno]);
	for(byI=0;byI<MAX_DWNPORT_NUM;byI++)
	{		
		pMPFlagsCtrl->PortMPFlag[byI][mp_seqno>>3] &= ~(1<<(mp_seqno&0x07));
	}	
	if(pMPFlagsCtrl->wJCMPNo == mp_seqno)
		pMPFlagsCtrl->wJCMPNo = 0xFFFF;
	if( pPara.annex.nNum != 0 )
	{
		for(i = 0; i < pPara.annex.nNum; i++)
		{
			if( ( pPara.annex.annexObj[i].oad.value == 0xF2210600 )
			&& ( pPara.annex.annexObj[i].value[0] == 0x11))
			{
				bCanID = pPara.annex.annexObj[i].value[1];
				DeleteChargingMap(bCanID);
			}
		}
	}
}
BYTE api_GetChargingPileCanID(BYTE* bAddr)
{
	TOad60010200 mpcfg;
	BYTE i,j,s;
	for (i = 1; i < MAX_MP_NUM; i++)
	{
		if (GetMP698Para(i,&mpcfg))
		{
			if ( memcmp(mpcfg.basic.tsa.addr,bAddr,6) == 0) 
			{
				for ( j = 0; j < mpcfg.annex.nNum; j++)
				{
					if (mpcfg.annex.annexObj[j].oad.value==0x40360200)
					{
						if ((mpcfg.annex.annexObj[j].value[2]== 6)||(mpcfg.annex.annexObj[j].value[2]== 7))//�ǳ��׮ҵ��ģ����ַ
						{
							for ( s = 0; s < mpcfg.annex.nNum; s++)
							{
								if (mpcfg.annex.annexObj[s].oad.value==0xf2210600)
								{
									return mpcfg.annex.annexObj[s].value[1];
								}
							}
						}
					}
				}
			}
		}
	}
	return 0xFF;
}

//-----------------------------------------------
//��������:	��ȡ���ܱ���
//	
//����:	mailAddr[in]	ͨ�ŵ�ַ
//	
//		meterNo[out]	���
//����ֵ:	TRUE FALSE
//	
//��ע:   
//-----------------------------------------------
BOOL api_GetMeterNo(const BYTE* mailAddr,BYTE* meterNo)
{
	TOad60010200 mpcfg = {0};
	BYTE i = 0;

	if((mailAddr == NULL)
	||(meterNo == NULL))
	{
		return FALSE;
	}
	
	for (i = 1; i < MAX_MP_NUM; i++)
	{
		if (GetMP698Para(i,&mpcfg))
		{
			if ( memcmp(mpcfg.basic.tsa.addr,mailAddr,6) == 0) 
			{
				memcpy(meterNo,mpcfg.MeterNo,8);
                return TRUE;
			}
		}
	}
    return FALSE;
}
// TTask698ParaCfg *pGet698TaskPara(void)
// {
// 	return &gpSysPara->task_para_cfg;
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : *pGet698NoTaskCfg
//	�������� : ����ڴ��е�x����������
//	������� : 
//	��    ע : 
//	��    �� : ����Ծ
//	ʱ    �� : 2017��11��8��
//	�� �� ֵ : TOad60130200
//	����˵�� : BYTE byNo
///////////////////////////////////////////////////////////////
// TOad60130200 *pGet698NoTaskCfg(BYTE byNo)
// {
// TTask698ParaCfg *p=pGet698TaskPara();
// TOad60130200 *ptkcfg=(TOad60130200 *)(&p->para_task_buf[byNo*sizeof(TOad60130200)]);
	
// 	if(byNo >= MAX_698_TASK_NUM)
// 		return NULL;
// 	return ptkcfg;
// }

// TOad60130200 *pGet698TaskCfg(BYTE task_no)
// {
// 	return pGet698NoTaskCfg(task_no);
// }

// WORD get_taskcfg_num(BYTE byMask)
// {
// TTask698ParaCfg *p=pGet698TaskPara();
// TOad60130200 *ptaskCfg=(TOad60130200 *)&p->para_task_buf[0];
// WORD wI,task_num=0;

// 	for(wI=0;wI<MAX_698_TASK_NUM;wI++,ptaskCfg++)
// 	{
// 		if(ptaskCfg->validflag == 0x5aa5)	
// 		{
// 			if(byMask & (1<<ptaskCfg->byTaskType))
// 				task_num++;
// 		}	
// 	}
// 	return task_num;
// }

/**************************************************************
 *  �� �� �� : check_clear_day_task_time
 *  �������� : ����ն���ɼ�����ʱ�䣬����ʱ���á�    
 *  ������� : 
 *  ��    ע : 
 *  ��    �� : lsg
 *  ʱ    �� : 2020��6��18�� 
 *  �� �� ֵ : void
 *  ����˵�� : void
**************************************************************/
// void check_clear_day_task_time(void)
// {
// TData_6203 *pDF6203=pGetDF6203();
// TOad60130200 *pCfg;
// BYTE byI,byJ;

// 	for (byI=0;byI<MAX_698_TASK_NUM;byI++)
// 	{
// 		pCfg = pGet698NoTaskCfg(byI);
// 		if (pCfg&&pCfg->validflag == 0x5aa5)
// 		{
// 			if ((pCfg->byTaskType == 1)&&(pCfg->intelTI.unit==3)&&(pCfg->intelTI.value == 1))
// 			{//�ն���ɼ����������
// 				for(byJ=0;byJ<MAX_DWNPORT_NUM;byJ++)
// 					pDF6203->OldTaskTime[byJ][pCfg->byTaskID] = 0;
// 			}
// 		}
// 	}
// }

// void sys_set_task_cfg(BYTE byTaskID,TOad60130200 *ptkcfg)
// {
// TTask698ParaCfg *p=pGet698TaskPara();
// TData_6203 *pDF6203=pGetDF6203();

// 	if(byTaskID >= MAX_698_TASK_NUM)
// 		return;
// 	ptkcfg->validflag  = 0x5aA5;
// 	memcpy(&p->para_task_buf[byTaskID*sizeof(TOad60130200)],ptkcfg,sizeof(TOad60130200));
// 	if(NULL != pDF6203)
// 	{
// 		BYTE byI;
// 		for(byI=0;byI<MAX_DWNPORT_NUM;byI++)
// 			pDF6203->OldTaskTime[byI][byTaskID] = 0;
// 	}	
// 	_freshPara(FLAG_TASK);
// }

// void sys_del_task_cfg(BYTE byTaskID,BOOL ball)
// {
// TTask698ParaCfg *p=pGet698TaskPara();
// TOad60130200 *ptkcfg=(TOad60130200 *)p->para_task_buf;
// WORD i;

// 	if(ball)
// 	{
// 		for(i=0;i<MAX_698_TASK_NUM;i++)
// 			ptkcfg[i].validflag	= 0;
// 	}
// 	else 
// 	{
// 		if(byTaskID >= MAX_698_TASK_NUM)
// 			return;
// 		ptkcfg[byTaskID].validflag	= 0;
// 	}	
// 	_freshPara(FLAG_TASK);
// }

// void set_task_status(BYTE byTaskID,BYTE task_status)
// {
// TOad60130200 *p=pGet698TaskCfg(byTaskID);

// 	if(NULL == p)
// 		return;
// 	p->byStuts	= task_status;
// 	_freshPara(FLAG_TASK);
// }				

// TScheme698ParaCfg *pGet698ShemePara(void)
// {//������������
// 	return &gpSysPara->sheme_para_cfg;//��������
// }

// BOOL schNo2Index(BYTE bySchNo,BYTE byTaskType,BYTE *pbyIndex)
// {
// WORD wI;
// BOOL bRc=FALSE;

// 	*pbyIndex	= 0xFF;
// 	switch(byTaskType)
// 	{
// 	case TT_DATA_CJ://��ͨ�ɼ����� 
// 		for(wI=0;wI<MAX_SCH_GENERAL;wI++)
// 		{
// 			if(s_SchIndexTab.Flag_GSchTab[wI>>3] & (1<<(wI&0x07)))
// 			{
// 				if(s_SchIndexTab.GSchTab[wI] == bySchNo)
// 				{
// 					*pbyIndex	= (BYTE)wI;
// 					bRc	= TRUE;
// 					break;
// 				}
// 			}
// 		}
// 		break;
// 	case TT_EVENT_CJ://�¼��ɼ�����
// 		for(wI=0;wI<MAX_SCH_EVENT;wI++)
// 		{
// 			if(s_SchIndexTab.Flag_ESchTab[wI>>3] & (1<<(wI&0x07)))
// 			{
// 				if(s_SchIndexTab.ESchTab[wI] == bySchNo)
// 				{
// 					*pbyIndex	= (BYTE)wI;
// 					bRc	= TRUE;
// 					break;
// 				}
// 			}
// 		}		
// 		break;
// 	case TT_TRANS_CJ://͸������
// 		for(wI=0;wI<MAX_SCH_TRANS;wI++)
// 		{
// 			if(s_SchIndexTab.Flag_TSchTab[wI>>3] & (1<<(wI&0x07)))
// 			{
// 				if(s_SchIndexTab.TSchTab[wI] == bySchNo)
// 				{
// 					*pbyIndex	= (BYTE)wI;
// 					bRc	= TRUE;
// 					break;
// 				}
// 			}
// 		}				
// 	    break;
// 	case TT_REPORT://�ϱ�����
// 		for(wI=0;wI<MAX_SCH_REPORT;wI++)
// 		{
// 			if(s_SchIndexTab.Flag_RSchTab[wI>>3] & (1<<(wI&0x07)))
// 			{
// 				if(s_SchIndexTab.RSchTab[wI] == bySchNo)
// 				{
// 					*pbyIndex	= (BYTE)wI;
// 					bRc	= TRUE;
// 					break;
// 				}
// 			}
// 		}						
// 	    break;
// 	default:
// 		return FALSE;
// 	}
// 	return bRc;
// }
//--------------------------------------------------
//��������:  ��ȡ�����͵ķ�����ʣ���ٸ��ռ�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
// BYTE  api_GetSchHaveNum( BYTE sch_type)
// {
// 	WORD wI;
// 	BYTE Num=0;
// 	switch(sch_type)
// 	{
// 	case TT_DATA_CJ://��ͨ�ɼ����� 		
// 		for(wI=0;wI<MAX_SCH_GENERAL;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_GSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				Num++;
// 			}
// 		}	
// 		return Num;	
// 		break;
// 	case TT_EVENT_CJ://�¼��ɼ�����
// 		for(wI=0;wI<MAX_SCH_EVENT;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_ESchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				Num++;
// 			}
// 		}	
// 		return Num;	
// 		break;
// 	case TT_TRANS_CJ://͸������
// 		for(wI=0;wI<MAX_SCH_TRANS;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_TSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				Num++;
// 			}
// 		}	
// 		return Num;	
// 		break;
// 	case TT_REPORT://�ϱ�����
// 		for(wI=0;wI<MAX_SCH_REPORT;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_RSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				Num++;
// 			}
// 		}	
// 		return Num;	
// 		break;
// 	default:
// 		break;
// 	}
// 	return 0;
// }
// BYTE AddSchIndex(BYTE sch_type,BYTE scheme_no)
// {
// WORD wI;

// 	switch(sch_type)
// 	{
// 	case TT_DATA_CJ://��ͨ�ɼ����� 		
// 		for(wI=0;wI<MAX_SCH_GENERAL;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_GSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				s_SchIndexTab.GSchTab[wI] = scheme_no;
// 				s_SchIndexTab.Flag_GSchTab[wI>>3] |= (1<<(wI&0x07));
// 				return (BYTE)wI;
// 			}
// 		}
// 		break;
// 	case TT_EVENT_CJ://�¼��ɼ�����
// 		for(wI=0;wI<MAX_SCH_EVENT;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_ESchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				s_SchIndexTab.ESchTab[wI] = scheme_no;
// 				s_SchIndexTab.Flag_ESchTab[wI>>3] |= (1<<(wI&0x07));
// 				return (BYTE)wI;
// 			}
// 		}		
// 		break;
// 	case TT_TRANS_CJ://͸������
// 		for(wI=0;wI<MAX_SCH_TRANS;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_TSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				s_SchIndexTab.TSchTab[wI] = scheme_no;
// 				s_SchIndexTab.Flag_TSchTab[wI>>3] |= (1<<(wI&0x07));
// 				return (BYTE)wI;
// 			}
// 		}				
// 	    break;
// 	case TT_REPORT://�ϱ�����
// 		for(wI=0;wI<MAX_SCH_REPORT;wI++)
// 		{
// 			if((s_SchIndexTab.Flag_RSchTab[wI>>3] & (1<<(wI&0x07))) == 0)
// 			{
// 				s_SchIndexTab.RSchTab[wI] = scheme_no;
// 				s_SchIndexTab.Flag_RSchTab[wI>>3] |= (1<<(wI&0x07));
// 				return (BYTE)wI;
// 			}
// 		}						
// 	    break;
// 	default:
// 		break;
// 	}
// 	return 0xFF;
// }

// void delSchIndex(BYTE sch_type,BYTE byIndex)
// {
// 	switch(sch_type)
// 	{
// 	case TT_DATA_CJ://��ͨ�ɼ����� 
// 		if(byIndex < MAX_SCH_GENERAL)
// 			s_SchIndexTab.Flag_GSchTab[byIndex>>3] &= ~(1<<(byIndex&0x07));
// 		break;
// 	case TT_EVENT_CJ://�¼��ɼ�����
// 		if(byIndex < MAX_SCH_EVENT)
// 			s_SchIndexTab.Flag_ESchTab[byIndex>>3] &= ~(1<<(byIndex&0x07));		
// 		break;
// 	case TT_TRANS_CJ://͸������
// 		if(byIndex < MAX_SCH_TRANS)
// 			s_SchIndexTab.Flag_TSchTab[byIndex>>3] &= ~(1<<(byIndex&0x07));
// 		break;
// 	case TT_REPORT://�ϱ�����
// 		if(byIndex < MAX_SCH_REPORT)
// 			s_SchIndexTab.Flag_RSchTab[byIndex>>3] &= ~(1<<(byIndex&0x07));		
// 	    break;
// 	default:
// 		break;
// 	}
// }

//static WORD _freshGetSchemeOff698(BYTE bySchIdex,BYTE byTaskType)
//{
//TScheme698ParaCfg *pPara=pGet698ShemePara();
//BYTE byOffsetPage = 0;
//WORD wOffset=0;
//	
//	switch(byTaskType)
//	{
//	case TT_DATA_CJ://��ͨ�ɼ����� 
//		if(bySchIdex < MAX_SCH_GENERAL)
//			byOffsetPage = bySchIdex;
//		else return 0xFFFF;
//		break;
//	case TT_EVENT_CJ://�¼��ɼ�����
//		if(bySchIdex < MAX_SCH_EVENT)
//			byOffsetPage = (BYTE)(MAX_SCH_GENERAL+bySchIdex);
//		else return 0xFFFF;
//		break;	
//	case TT_REPORT://�ϱ�����
//		if(bySchIdex < MAX_SCH_REPORT)
//			byOffsetPage = (BYTE)(MAX_SCH_GENERAL+MAX_SCH_EVENT+bySchIdex);
//		else return 0xFFFF;
//	    break;
//	case TT_TRANS_CJ://͸������
//		if(bySchIdex < MAX_SCH_TRANS)
//			byOffsetPage = (BYTE)(MAX_SCH_GENERAL+MAX_SCH_EVENT+MAX_SCH_REPORT+bySchIdex);
//		else return 0xFFFF;
//	    break;
//	default:
//		return 0xFFFF;
//	}	
//	if(pPara->nowPageNum != byOffsetPage)
//	{//ҳ�벻һ�»�ҳ		
//		TFileRead FR;
//		if(pPara->saveflag)
//		{//�������������
//			TFileWrite FW;
//			FW.idwBufLen	= SIZE_PAGE;
//			FW.idwOffset	= getParaFileOffset(pPara->nowPageNum,FLAG_SCHEME);
//			FW.ipvBuf		= pPara->para_scheme_buf;
//			FW.iszName		= FILE_SYSPARA;
//			FileWrite(&FW);
//			pPara->saveflag = FALSE;
//		}		
//		FR.idwBufLen	= SIZE_PAGE;
//		FR.idwOffset	= getParaFileOffset(byOffsetPage,FLAG_SCHEME);
//		FR.ipvBuf		= pPara->para_scheme_buf;
//		FR.iszName		= FILE_SYSPARA;
//		FileRead(&FR);
//		pPara->nowPageNum = byOffsetPage;
//	}	
//	return wOffset;
//}

//BOOL sys_cj_scheme_read(BYTE sch_type,BYTE scheme_no,BYTE *schinfo)
//{
//TScheme698ParaCfg *p=pGet698ShemePara();
//WORD offset=0xFFFF;
//BYTE byIndex;
//BOOL bRc=FALSE;
//	
//	// SM_P(MPV_PARA_MPB);
//	if(schNo2Index(scheme_no,sch_type,&byIndex))
//		offset	= _freshGetSchemeOff698(byIndex,sch_type);
//	if(offset != 0xFFFF)
//	{
//		switch(sch_type)
//		{
//		case TT_DATA_CJ://��ͨ
//			{
//				TOad60150200 *psch=(TOad60150200 *)&p->para_scheme_buf[offset];
//				WORD wLen=sizeof(TOad60150200);
//				memcpy(schinfo,&p->para_scheme_buf[offset],wLen);
//				bRc	= (psch->valid_flag	== 0x5AA5);
//			}
//			break;
//		case TT_EVENT_CJ://�¼�
//			{
//				TOad60170200 *psch=(TOad60170200 *)&p->para_scheme_buf[offset];
//				memcpy(schinfo,&p->para_scheme_buf[offset],sizeof(TOad60170200));
//				bRc	= (psch->valid_flag	== 0x5AA5);
//			}
//			break;
//		case TT_TRANS_CJ://͸��
//			{
//				TOad60190200 *psch=(TOad60190200 *)&p->para_scheme_buf[offset];
//				memcpy(schinfo,&p->para_scheme_buf[offset],sizeof(TOad60190200));
//				bRc	= (psch->valid_flag	== 0x5AA5);
//			}
//			break;
//		case TT_REPORT://�ϱ�
//			{
//				TOad601D0200 *psch=(TOad601D0200 *)&p->para_scheme_buf[offset];
//				memcpy(schinfo,&p->para_scheme_buf[offset],sizeof(TOad601D0200));
//				bRc	= (psch->valid_flag	== 0x5AA5);
//			}
//			break;
//		default:
//			bRc	= FALSE;
//			break;
//		}
//	}
//	SM_V(MPV_PARA_MPB);
//	return bRc;
//}

//BOOL add_cjscheme(BYTE sch_type,BYTE scheme_no,BYTE *schinfo)
//{
//TScheme698ParaCfg *p=pGet698ShemePara();
//WORD offset=0xFFFF;
//BYTE byIndex;
//BOOL bRc=TRUE;
//	
//	// SM_P(MPV_PARA_MPB);
//	if(!schNo2Index(scheme_no,sch_type,&byIndex))
//		byIndex	= AddSchIndex(sch_type,scheme_no);
//	offset	= _freshGetSchemeOff698(byIndex,sch_type);
//	if(offset != 0xFFFF)
//	{
//		switch(sch_type)
//		{
//		case TT_DATA_CJ://��ͨ
//			{
//				TOad60150200 *pschNew = NULL;
//				TOad60150200 *psch=(TOad60150200 *)&p->para_scheme_buf[offset];
//				pschNew = (TOad60150200 *)schinfo;
//				if( memcmp(psch->csdBuf,pschNew->csdBuf,512 ) == 0 )
//				{					
//					pschNew->dwValidTime = psch->dwValidTime;	
//				}
//				else
//				{
//					TTime time;
//					GetTime(&time);					
//					pschNew->dwValidTime = Time2Sec(&time);
//					set_chg_sch_mins(scheme_no,sch_type,pschNew->dwValidTime/60);
//					pschNew->dwValidTime = 0xfefefefe;//��֤�˷���ִ��ǰ�������ٲⲻ����
//				}
//				memcpy(&p->para_scheme_buf[offset],schinfo,sizeof(TOad60150200));
//				psch->valid_flag	= 0x5AA5;
//			}
//			break;
//		case TT_EVENT_CJ://�¼�
//			{
//				TOad60170200 *psch=(TOad60170200 *)&p->para_scheme_buf[offset];
//				memcpy(&p->para_scheme_buf[offset],schinfo,sizeof(TOad60170200));
//				psch->valid_flag	= 0x5AA5;
//			}
//			break;
//		case TT_TRANS_CJ://͸��
//            {
//				TOad60190200 *psch=(TOad60190200 *)&p->para_scheme_buf[offset];
//				memcpy(&p->para_scheme_buf[offset],schinfo,sizeof(TOad60190200));
//				psch->valid_flag	= 0x5AA5;
//			}
//			break;
//		case TT_REPORT://�ϱ�
//			{
//				TOad601D0200 *psch=(TOad601D0200 *)&p->para_scheme_buf[offset];
//				memcpy(&p->para_scheme_buf[offset],schinfo,sizeof(TOad601D0200));
//				psch->valid_flag	= 0x5AA5;
//			}
//			break;
//		}
//		_freshPara(FLAG_SCHEME);
//	}
//	SM_V(MPV_PARA_MPB);
//	return bRc;
//}

//void del_cjscheme(BYTE sch_type,BYTE sch_no)
//{
//TScheme698ParaCfg *p=pGet698ShemePara();
//WORD offset=0xFFFF;
//BYTE byIndex;
//
//	// SM_P(MPV_PARA_MPB);
//	if(schNo2Index(sch_no,sch_type,&byIndex))
//		offset	= _freshGetSchemeOff698(byIndex,sch_type);
//	if(offset != 0xFFFF)
//	{
//		switch(sch_type)
//		{
//		case 1://��ͨ
//			{
//				TOad60150200 *psch=(TOad60150200 *)&p->para_scheme_buf[offset];
//				if(psch->valid_flag == 0x5aa5)
//				{
//					psch->valid_flag	= 0;
//					psch->dwValidTime = 0xfefefefe;
//					_freshPara(FLAG_SCHEME);
//				}
//			}
//			break;
//		case 2://�¼�
//			{
//				TOad60170200 *psch=(TOad60170200 *)&p->para_scheme_buf[offset];
//				if(psch->valid_flag == 0x5aa5)
//				{
//					psch->valid_flag	= 0;
//					_freshPara(FLAG_SCHEME);
//				}
//			}
//			break;
//		case 3://͸��
//			{
//				TOad60190200 *psch=(TOad60190200 *)&p->para_scheme_buf[offset];
//				if(psch->valid_flag == 0x5aa5)
//				{
//					psch->valid_flag	= 0;
//					_freshPara(FLAG_SCHEME);
//				}
//			}
//			break;
//		case 4://�ϱ�
//			{
//				TOad601D0200 *psch=(TOad601D0200 *)&p->para_scheme_buf[offset];
//				if(psch->valid_flag == 0x5aa5)
//				{
//					psch->valid_flag	= 0;
//					_freshPara(FLAG_SCHEME);
//				}
//			}
//			break;
//		}
//		delSchIndex(sch_type,byIndex);
//	}
//	SM_V(MPV_PARA_MPB);
//}

DWORD get_dmfrztask_save_time(DWORD dwTaskTime,TOad60150200 *psch)
{
DWORD dwTime=dwTaskTime;
BYTE byLast=1;
CSD csdd;

	get_no_csd_of_CSDS(psch->csdBuf,0,&csdd);
	if(csdd.choice)
	{
		if(csdd.road.oadMain.value == 0x50040200)	
		{//����
			if(psch->bySaveFlag == 4)
			{
				byLast	= 2;
				dwTime	= (dwTaskTime/1440-byLast+1)*1440;
			}
			else if(psch->bySaveFlag == 3)
			{
				byLast	= 2;
				dwTime	= (dwTaskTime/1440-byLast+1)*1440+1439;
			}
			else
				dwTime	= (dwTaskTime/1440-byLast+1)*1440;

		}
		else if(csdd.road.oadMain.value == 0x50050200)
		{
			if(psch->bySaveFlag == 4)
			{
				byLast	= 2;
				dwTime	= (dwTaskTime/1440-byLast+1)*1440;
			}
			else if(psch->bySaveFlag == 3)
			{
				byLast	= 2;
				dwTime	= (dwTaskTime/1440-byLast+1)*1440+1439;
			}
			else
				dwTime	= (dwTaskTime/1440-byLast+1)*1440;
		}
		else if(csdd.road.oadMain.value == 0x50060200)		
		{
			TTime tTime;
			dwTime	= (dwTaskTime/1440)*1440;
			Min2Time(dwTime,&tTime);
			tTime.Day	= 1;			
			dwTime = Time2Min(&tTime);
			if(psch->bySaveFlag == 7)
				dwTime = dwTime - 1;
		}
	}	
	else
	{
		if(psch->bySaveFlag == 1)
		{
			TTime tTime;
			GetTime(&tTime);
			dwTime =Time2Min(&tTime);
		}
	}	
	return dwTime;
}

WORD get_max_sch_size(void)
{
WORD wRc=sizeof(TOad60150200);

	if(wRc < sizeof(TOad60150200))
		wRc = sizeof(TOad60150200);
	if(wRc < sizeof(TOad60170200))
		wRc = sizeof(TOad60170200);
	if(wRc < sizeof(TOad60190200))
		wRc = sizeof(TOad60190200);
	if(wRc < sizeof(TOad601D0200))
		wRc = sizeof(TOad601D0200);
	return wRc;
}

///////////////////////////////////////////////////////////////
//	�� �� �� : get_frztype_rcsd
//	�������� : ����RCSD ����ٻ����ݵ�����
//	������� : 
//	��    ע : 
//	��    �� : ����Ծ
//	ʱ    �� : 2017��11��17��
//	�� �� ֵ : BYTE
//	����˵�� : BYTE *pRcsd
///////////////////////////////////////////////////////////////
BYTE get_frztype_rcsd(BYTE *pRcsd)
{
WORD wI,wNum;
BYTE byFrzType=0,classno=0;
CSD csdd;

	wNum	= get_csd_num_of_RCSD(pRcsd);
	for(wI=0;wI<wNum;wI++)
	{
		if(get_no_csd_of_RCSD(pRcsd,wI,&csdd))
		{
			if(csdd.choice)
			{
				if((csdd.road.oadMain.value	== 0x50020200)
					||(csdd.road.oadMain.value	== 0x50030200))
				{
					byFrzType	= REQ_CURV;
					break;
				}
				else if(csdd.road.oadMain.value	== 0x50040200)				
				{
					byFrzType	= REQ_DFRZ;
					break;
				}
				if(csdd.road.oadMain.value == 0x50050200)
				{
					byFrzType	= REQ_SETTLEDAY;
					break;
				}
				else if(csdd.road.oadMain.value	== 0x50060200)
				{
					byFrzType	= REQ_MFRZ;
					break;
				}
				if( oi2class(HIWORD(csdd.road.oadMain.value),&classno) )
				{
					if((classno==7) || (classno ==24))
						byFrzType = REQ_EVENT_0;
					break;
				}
			}
		}		
	}
	return byFrzType;
}

// BYTE get_scheme_num(BYTE sch_type)
// {
// BYTE byNum=0,i;

// 	// SM_P(MPV_PARA_MPB);
// 	switch(sch_type)
// 	{
// 	case TT_DATA_CJ://��ͨ
// 		for(i=0;i<MAX_SCH_GENERAL;i++)
// 		{
// 			if(s_SchIndexTab.Flag_GSchTab[i>>3] & (1<<(i&0x07)))
// 				byNum++;
// 		}
// 		break;
// 	case TT_EVENT_CJ://�¼�
// 		for(i=0;i<MAX_SCH_EVENT;i++)
// 		{
// 			if(s_SchIndexTab.Flag_ESchTab[i>>3] & (1<<(i&0x07)))
// 				byNum++;			
// 		}
// 		break;
// 	case TT_TRANS_CJ://͸��
//         for(i=0;i<MAX_SCH_TRANS;i++)
// 		{
// 			if(s_SchIndexTab.Flag_TSchTab[i>>3] & (1<<(i&0x07)))
// 				byNum++;			
// 		}
// 		break;
// 	case TT_REPORT://�ϱ�
// 		for(i=0;i<MAX_SCH_REPORT;i++)
// 		{
// 			if(s_SchIndexTab.Flag_RSchTab[i>>3] & (1<<(i&0x07)))
// 				byNum++;			
// 		}
// 		break;
// 	}
// 	SM_V(MPV_PARA_MPB);
// 	return byNum;
// }

//void set_event_up_status(BYTE sch_no,BYTE status)
//{
//TScheme698ParaCfg *p=pGet698ShemePara();
//WORD offset=0xFFFF;
//BYTE byIndex;
//
//	// SM_P(MPV_PARA_MPB);
//	if(schNo2Index(sch_no,TT_EVENT_CJ,&byIndex))
//		offset	= _freshGetSchemeOff698(byIndex,TT_EVENT_CJ);
//	if(0xFFFF != offset)
//	{
//		TOad60170200 *psch=(TOad60170200 *)&p->para_scheme_buf[offset];
//		if(status)
//			psch->bReport	= TRUE;
//		else
//			psch->bReport	= FALSE;
//		_freshPara(FLAG_SCHEME);
//	}	
//	SM_V(MPV_PARA_MPB);
//}

//BOOL GetschInfo(BYTE bytaskType,BYTE byShemeNo,BYTE *psch698)
//{
//TScheme698ParaCfg *pPara = pGet698ShemePara();
//WORD offset=0xFFFF,wSchLen=get_max_sch_size();
//BYTE byIndex;
//
//	if(bytaskType == TT_DATA_CJ)
//		wSchLen	= sizeof(TOad60150200);
//	else if(bytaskType == TT_EVENT_CJ)
//		wSchLen	= sizeof(TOad60170200);
//	else if(bytaskType == TT_TRANS_CJ)
//		wSchLen = sizeof(TOad60190200);
//	else if(bytaskType == TT_REPORT)
//		wSchLen = sizeof(TOad601D0200);
//	// SM_P(MPV_PARA_MPB);
//	if(schNo2Index(byShemeNo,bytaskType,&byIndex))
//		offset	= _freshGetSchemeOff698(byIndex,bytaskType);
//	if(0xFFFF != offset)
//		memcpy(psch698,&pPara->para_scheme_buf[offset],wSchLen);
//	SM_V(MPV_PARA_MPB);
//	if(0xFFFF != offset)
//	{
//		if(bytaskType == TT_DATA_CJ)
//		{
//			TOad60150200 *pCfg=(TOad60150200 *)psch698;
//			if(pCfg->byColNum > 100)
//				pCfg->byColNum	= 0;
//			return (pCfg->valid_flag == 0x5AA5);
//		}
//		else if(bytaskType == TT_EVENT_CJ)
//		{
//			TOad60170200 *pCfg=(TOad60170200 *)psch698;
//			if(pCfg->byCJNum > 100)
//				pCfg->byCJNum	= 0;		
//			return (pCfg->valid_flag == 0x5AA5);
//		}
//		else if(bytaskType == TT_TRANS_CJ)
//		{
//			TOad60190200 *pCfg=(TOad60190200 *)psch698;
//		
//			return (pCfg->valid_flag == 0x5AA5);
//		}
//		else if(bytaskType == TT_REPORT)
//		{
//			TOad601D0200 *pCfg=(TOad601D0200 *)psch698;
//		
//			return (pCfg->valid_flag == 0x5AA5);
//		}
//	}	
//	return FALSE;
//}

//TCtrl698ParaCfg *pGet698CtrlPara(void)
//{
//	return &gpSysPara->ctrl_para_cfg;//���Ʋ�������
//}

const TOAD_LEN s_OADLenEx[]={	
	{0x50020300,sizeof(TOad50020300)},		//���Ӷ���
	{0x50030300,sizeof(TOad50030300)},		//Сʱ����
	{0x50040300,sizeof(TOad50040300)},		//�ն���
	{0x50060300,sizeof(TOad50060300)}		//�¶���
};

// WORD get_sys_frz_oad_para_size(DWORD objOAD)
// {
// DWORD dwInd;

// 	for(dwInd=0;dwInd<ArraySize(s_OADLenEx);++dwInd) 
// 	{
// 		if( objOAD == s_OADLenEx[dwInd].obj )
// 		{
// 			if (s_OADLenEx[dwInd].wLen != INVALID_LEN )
// 				return s_OADLenEx[dwInd].wLen;
// 			break;
// 		}
// 	}
// 	return 0;
// }

WORD getSysFrz698ParaIndex(DWORD oadObj )
{//�����ڿ��ڵ�ƫ��
WORD wInd = 0;
WORD wSumOffset=0;

	for (wInd=0;wInd<ArraySize(s_OADLenEx); ++wInd)
	{
		if(s_OADLenEx[wInd].obj == oadObj)
			break;		
		wSumOffset += s_OADLenEx[wInd].wLen;
	}
	if (wInd >= ArraySize(s_OADLenEx))
		return 0xFFFF;
	return wSumOffset;
}

//TFrz698ParaCfg *pGet698FrzPara(void)
//{
//	return &gpSysPara->frz_para_cfg;//���Ʋ�������
//}

//HPARA pGetFrz698(DWORD oad_index)
//{
//WORD wOffset=getSysFrz698ParaIndex(oad_index);
//TFrz698ParaCfg *pCfg=pGet698FrzPara();	
//
//	if(0xFFFF == wOffset)
//		return NULL;
//	return &pCfg->para_frz_buf[wOffset];
//}


///////////////////////////////////////////////////////////////
//	�� �� �� : sys_GetMPCfg
//	�������� : ���ָ��(���)��ȡ������(���ֻ�����Ϣ)
//	������� : 
//	��    ע : 
//	��    �� : wangbw
//	ʱ    �� : 2015��12��30��
//	�� �� ֵ : BOOL
//	����˵�� : WORD wMPSeq, ���
//			   TMetCfg_Base ������(���ֻ�����Ϣ)
///////////////////////////////////////////////////////////////
BOOL sys_GetMPCfg(WORD wMPSeq,TMetCfg_Base *pCfg)	
{
	if((wMPSeq == 0)||( wMPSeq>= MAX_MP_NUM))
		return FALSE;
	memcpy(pCfg, &gMetCfg[wMPSeq],sizeof(TMetCfg_Base));
	if(	pCfg->wMPNo!=0 )
		return TRUE;
	return FALSE;
}
extern void api_SetCanID(BYTE bType,BYTE bTar);
BOOL JudgeCanID(WORD wMPSeq)
{
	TOad60010200 tmp={0};
	BYTE bRc = FALSE,i,bCanID;	
	
	bRc	= GetMP698Para(wMPSeq,&tmp);

	if(bRc)
	{
		if( tmp.annex.nNum != 0 )
		{
			for(i = 0; i < tmp.annex.nNum; i++)
			{
				if( ( tmp.annex.annexObj[i].oad.value == 0xF2210600 )
				&& ( tmp.annex.annexObj[i].value[0] == 0x11))
				{
					bCanID = tmp.annex.annexObj[i].value[1];
					api_SetCanID(1,bCanID);
				}
			}
		}
	}
	return FALSE;
}
BOOL sys_InitMPCfg(WORD wMPSeq,TMetCfg_Base *pCfg)
{
	BOOL bRc = FALSE;
	TOad60010200 tmp={0};
	
	bRc	= GetMP698Para(wMPSeq,&tmp);
	if(bRc)
	{
		if(tmp.nIndex == wMPSeq)
		{
			bRc	= TRUE;
			pCfg->wMPNo	= wMPSeq;
			pCfg->byUserType	= tmp.basic.userType;
			pCfg->byProtocolNo	= tmp.basic.protocol;
			memcpy(&pCfg->meter_addr,&tmp.basic.tsa,sizeof(TSA));
			// memcpy(&pCfg->cll_addr,&tmp.extend.add,sizeof(TSA));
			pCfg->byBaud	= tmp.basic.baud;
			OAD2PortNo(&tmp.basic.port.value,&pCfg->byPortNo,TRUE);
		}
		else
		{
			pCfg->wMPNo = 0;
			bRc	= FALSE;
		}
	}
	return bRc;
}
extern WORD JudgeRecMeterNo_645(BYTE *pBuf);
BOOL IsInSysMeters(BYTE *pMeterAddr)
{
	TMetCfg_Base Cfg;
	WORD i;

	if (!JudgeRecMeterNo_645(pMeterAddr))
	{
		for (i = 1; i < MAX_MP_NUM; i++)
		{
			if (sys_GetMPCfg(i,&Cfg))
			{
				if (memcmp(Cfg.meter_addr.addr,pMeterAddr,6) == 0)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//���ݱ�Ų��ҵ�����
BYTE FindMeterMPNo(BYTE *pMeterAddr)
{
	TMetCfg_Base Cfg;
	WORD i;
	for (i = 1; i < MAX_MP_NUM; i++)
	{
		if (sys_GetMPCfg(i,&Cfg))
		{
			if (memcmp(Cfg.meter_addr.addr,pMeterAddr,6) == 0)
			{
				return i;
			}
		}
	}
	return 0;
}

BOOL IsChargeMeters(BYTE *pMeterAddr)
{
	TOad60010200 mpcfg={0};
	WORD i,j;
	
	for (i = 1; i < MAX_MP_NUM; i++)
	{
		if (GetMP698Para(i,&mpcfg))
		{
			if ( memcmp(mpcfg.basic.tsa.addr,pMeterAddr,6) == 0) 
			{
				for ( j = 0; j < mpcfg.annex.nNum; j++)
				{
					if (mpcfg.annex.annexObj[j].oad.value==0x40360200)
					{
						if ((mpcfg.annex.annexObj[j].value[2]== 6)||(mpcfg.annex.annexObj[j].value[2]== 7))//�ǳ��׮ҵ��ģ����ַ
						{
							return TRUE;
						}
					}
					
					
					
				}
				
				
			}
		}
	}
	return FALSE;
}

//�Ƿ�Ϊ���׮������ı���֡
BOOL IsChargingPileFrame(BYTE *pBuf)
{
	WORD OI;
	BYTE i;
	if (pBuf[0] == GET_Request)
	{
		//OI = MW(pBuf[3],pBuf[4]);
		//for( i = 0; i < (sizeof(wChargingPileIO) / sizeof(WORD)); i++)
		//{
		//	if(OI == wChargingPileIO[i])
		//	{
				return TRUE;
		//	}
		//}
	}
    return FALSE;
}

// TsysCfgOther *pGetSysCfgOther(void)
// {
// 	return (TsysCfgOther*) &gpSysPara->base_para_cfg.base_para_buf[0];
// }

// void FreshSysCfgOther(void)
// {
// 	// SM_P(MPV_PARA_SYS);
// 	gpSysPara->base_para_cfg.saveflag	= TRUE;
// 	SM_V(MPV_PARA_SYS);
// }

// BYTE GetReal2FrzFlg(void)
// {
// #if(MT_MODULE == MT_DF6203_JZQ_II)
// TsysCfgOther *pSys=pGetSysCfgOther();
// 	if(GetUserType() == MAR_HE_NAN)//����Ҫ��ʵʱת�ն���
// 		return 0x5A;
// 	else if (GetUserType() == MAR_SHAN_DONG)
// 		return LLBYTE(pSys->dwMins_BD);
// 	else 
// 		return 0;
// #else
// 	return 0;
// #endif
// }
// void SetReal2FrzFlg(BYTE byFlag)
// {
// #if(MT_MODULE == MT_DF6203_JZQ_II)
// TsysCfgOther *pSys=pGetSysCfgOther();
// BYTE byOld;

// 	if ((GetUserType() != MAR_SHAN_DONG)&&(GetUserType() != MAR_HE_NAN))
// 		return;

// 	byOld = LLBYTE(pSys->dwMins_BD);
// 	if (byOld == byFlag)
// 		return;

// 	pSys->dwMins_BD &= 0xFFFFFF00;
// 	pSys->dwMins_BD |= byFlag;
// 	FreshSysCfgOther();
// //	Trace("���ö��᷽ʽ =%x",pSys->dwMins_BD);
// #endif
// }

// void add_safemode(TEsamSafeMode *psafemode)
// {
// TsysCfgOther *pSys=pGetSysCfgOther();
// WORD i;

// 	del_safemode(psafemode->oii);
// 	for(i=0;i<ESAM_SAFE_NUM;i++)
// 	{
// 		if((pSys->valid_safemode[i>>3] & (1<<(i&7))) == 0)
// 		{
// 			memcpy(&pSys->safeMode[i],psafemode,sizeof(TEsamSafeMode));
// 			pSys->valid_safemode[i>>3] |= 1<<(i&7);
// 			break;
// 		}
// 	}
// 	FreshSysCfgOther();
// }

// void del_safemode(OI oii)
// {
// TsysCfgOther *pSys=pGetSysCfgOther();
// WORD i,wFlag=0;

// 	for(i=0;i<ESAM_SAFE_NUM;i++)
// 	{
// 		if(pSys->valid_safemode[i>>3] & (1<<(i&7)))
// 		{
// 			if(pSys->safeMode[i].oii == oii)
// 			{
// 				wFlag	= 1;
// 				pSys->valid_safemode[i>>3] &= ~(1<<(i&7));
// 			}	
// 		}
// 	}
// 	if(wFlag)
// 		FreshSysCfgOther();
// }


// BOOL sys_ReadTermNo(DWORD *pdwAreaCode,DWORD *pdwAddress)
// {
// DWORD tmp=0,i;
// TOad40010200 cfg;

// 	if(sys_Para698_Read(0x40010200,&cfg))
// 	{
// 		if(pdwAddress != NULL)
// 		{
// 			tmp	= 0;
// 			for(i=0;((i<4)&&(i<cfg.addr.len));i++)
// 			{
// 				tmp	+= cfg.addr.addr[i]<<(i*8);
// 			}
// 			*pdwAddress	= tmp;
// 		}
// 		if(pdwAreaCode != NULL)
// 		{
// 			tmp	= 0;
// 			for(i=4;((i<8)&&(i<cfg.addr.len));i++)
// 			{				
// 				tmp	+= (cfg.addr.addr[i]<<((i-4)*8));
// 			}
// 			*pdwAreaCode = tmp;
// 		}
// 	}
// 	return FALSE;
// }

void ParaSaveDaemon(BOOL bFlag,TTime *pTime)
{
TSysParaCfg *p=gpSysPara;
TFileWrite FW;
//#if(!MOS_MSVC)
//DWORD dwDelay,dwTime;
//	if(bFlag)
//	{//��ʱ����(1���������ݲ��������)
//		if(TimeIsValid(pTime))
//		{
//			dwTime	= Time2Sec(pTime);
//			dwDelay	= dwTime-dwParaFreshTime;
//			if(dwDelay < 10)
//				return;
//		}
//	}
//#endif	
	// if(p->base_para_cfg.saveflag)
	// {//ϵͳ���ò����ڴ�
	// 	// SM_P(MPV_PARA_SYS);
	// 	FW.iszName	= FILE_SYSPARA;
	// 	FW.idwBufLen= SIZE_PAGE*2;
	// 	FW.ipvBuf	= p->base_para_cfg.base_para_buf;
	// 	FW.idwOffset= getParaFileOffset(0,FLAG_SYS);	
	// 	FileWrite(&FW);
	// 	p->base_para_cfg.saveflag	= FALSE;
	// 	SM_V(MPV_PARA_SYS);
	// }
	// if(p->ctrl_para_cfg.saveflag)
	// {
	// 	// SM_P(MPV_PARA_SYS);
	// 	FW.iszName	= FILE_SYSPARA;
	// 	FW.idwBufLen= SIZE_PAGE;
	// 	FW.ipvBuf	= p->ctrl_para_cfg.para_ctrl_buf;
	// 	FW.idwOffset= getParaFileOffset(0,FLAG_FKSYS);	
	// 	FileWrite(&FW);
	// 	p->ctrl_para_cfg.saveflag	= FALSE;
	// 	SM_V(MPV_PARA_SYS);
	// }
	// if(p->task_para_cfg.saveflag)
	// {//ϵͳ���ò����ڴ�
	// 	// SM_P(MPV_PARA_TK);
	// 	FW.iszName	= FILE_SYSPARA;
	// 	FW.idwBufLen= sizeof(p->task_para_cfg.para_task_buf);
	// 	FW.ipvBuf	= p->task_para_cfg.para_task_buf;
	// 	FW.idwOffset= getParaFileOffset(0,FLAG_TASK);	
	// 	FileWrite(&FW);
	// 	p->task_para_cfg.saveflag	= FALSE;
	// 	SM_V(MPV_PARA_TK);
	// }
	if(p->mp_para_cfg.saveflag)
	{//�����㵵������2K����
		// SM_P(MPV_PARA_MP);
		FW.iszName	= FILE_MPPARA;
		FW.idwBufLen= SECTOR_SIZE;
		FW.idwOffset= getParaFileOffset(p->mp_para_cfg.nowPageNum,FLAG_METER);
		FW.ipvBuf	= p->mp_para_cfg.rtu_para_buf;
		FileWrite(&FW);
		p->mp_para_cfg.saveflag		= FALSE;
		// SM_V(MPV_PARA_MP);
	}
	// if(p->sheme_para_cfg.saveflag)
	// {
	// 	// SM_P(MPV_PARA_MPB);
	// 	FW.iszName	= FILE_SYSPARA;		
	// 	FW.idwBufLen= SIZE_PAGE;
	// 	FW.ipvBuf	= p->sheme_para_cfg.para_scheme_buf;
	// 	FW.idwOffset= getParaFileOffset(p->sheme_para_cfg.nowPageNum,FLAG_SCHEME);	
	// 	FileWrite(&FW);
	// 	p->sheme_para_cfg.saveflag	= FALSE;
	// 	SM_V(MPV_PARA_MPB);
	// }
	// if(p->frz_para_cfg.saveflag)
	// {//
	// 	// SM_P(MPV_PARA_USER);
	// 	FW.iszName	= FILE_SYSPARA;		
	// 	FW.idwBufLen= SIZE_PAGE;
	// 	FW.ipvBuf	= p->frz_para_cfg.para_frz_buf;
	// 	FW.idwOffset= getParaFileOffset(0,FLAG_SYS_EX);	
	// 	FileWrite(&FW);
	// 	p->frz_para_cfg.saveflag	= FALSE;
	// 	SM_V(MPV_PARA_USER);
	// }
}

// void _eeromPara2Ram(BYTE byType)
// {
// TSysParaCfg *p=gpSysPara;
// const DWORD Para1[]={0x45000200,0x45000300,0x45000400,0x45100200,0x45100300,0x45100400,0x45100500,0x40010200};
// DWORD dwOffset,dwFnLen,dwOffset2=EROM_OFT_INITPARA+5;
// BYTE byI;
	
// 	if((byType == 1)||(gVar.Cfg.wInitParaValid != 0x5AA5))
// 	{//2��������ʼ��(����ͨ�Ų���)
// 		for(byI=0;byI<ArraySize(Para1);byI++)
// 		{
// 			dwOffset= getSys698ParaIndex(Para1[byI]);	
// 			dwFnLen	= get_sys_oad_para_size(Para1[byI]);
// 			if( (dwFnLen >= SIZE_PAGE) 
// 				|| ((dwOffset+dwFnLen)>=SIZE_PAGE)
// 				|| (dwFnLen == 0) 
// 				|| (dwOffset==0xFFFF) ) 
// 			{
// 				continue;
// 			}
// 			bspEEPROMRead((WORD)(dwOffset+EROM_OFT_FLASHPARA),(WORD)dwFnLen,&p->base_para_cfg.base_para_buf[dwOffset]);			
// 		}		
// 	}
// 	else if(byType == 0) //ά�������������ó�������
// 	{//1��������ʼ��(�ָ���������)//��FLASH ��˳��ȡ���� EROM_OFT_INITPARA
// 		for(byI=0;byI<ArraySize(Para1);byI++)
// 		{
// 			dwOffset= getSys698ParaIndex(Para1[byI]);	
// 			dwFnLen	= get_sys_oad_para_size(Para1[byI]);
// 			if( (dwFnLen >= SIZE_PAGE) 
// 				|| ((dwOffset+dwFnLen)>=SIZE_PAGE)
// 				|| (dwFnLen == 0) 
// 				|| (dwOffset==0xFFFF) ) 
// 			{
// 				continue;
// 			}
// 			bspEEPROMRead((WORD)dwOffset2,(WORD)dwFnLen,&p->base_para_cfg.base_para_buf[dwOffset]);			
// 			dwOffset2	+= dwFnLen;
// 		}		
// 	}
// 	// SM_P(MPV_PARA_SYS);
// 	p->base_para_cfg.saveflag = TRUE;
// 	SM_V(MPV_PARA_SYS);
// }

void InitPara(BYTE byType)
{//�ն˲�����ʼ��

	TFileClean FC;
	TSysParaCfg *p=gpSysPara;
	// TsysCfgOther *pSys=pGetSysCfgOther();
	// BYTE Buf[5]={0x12,0x34,0x56,0x78,0};
	// TFileWrite FW;	
	// BYTE byI,byJ;
	// WORD oi = 0x300F;
	// WORD ioReport[] = {0x3100,0x3101,0x3104,0x3105,0x3106,0x3109,0x310B,0x310D,0x310E,0x310F,0x3111,0x3114,0x3119,0x311c};
	// DWORD dwLcdPass = 0;
	// DWORD dwTmp;

	// bspEEPROMWrite(EROM_OFT_FLASHPARA,p->base_para_cfg.base_para_buf,2048);
	// Buf[4]	= byType;
	// bspEEPROMWrite(EROM_OFT_INITPARA,Buf,5);

// #if(MOS_UCOSII)
	// ResetBadBlockTab();
// #endif
	memset(p,0,sizeof(TSysParaCfg));
	FC.iszName	= FILE_MPPARA;
	FC.bClearBak= FALSE;
	FC.idwCleanLen= SECTOR_SIZE*(MAX_MP_NUM - 1)/(SECTOR_SIZE/sizeof(TOad60010200));
	FC.idwOffset= FLASH_FREEZE_END_ADDR;
	FileClean(&FC);
	//������ز���
	// for(byI=0;byI<2;byI++)
	// {
	// 	FW.iszName	= FILE_SYSPARA;
	// 	FW.idwBufLen= SIZE_PAGE;
	// 	FW.ipvBuf	= p->base_para_cfg.base_para_buf;
	// 	FW.idwOffset= getParaFileOffset(byI,FLAG_FKSYS);	
	// 	FileWrite(&FW);
	// }
	InitMPFlags();
	// sys_SetStatus(4,1);            //Ĭ�ϴ������ϱ�
	// memset(pSys->EventFlag,0,88);  //�ر��¼���Ч���ϱ�

	// FreshSysCfgOther();
	//��EEPROM������
	// _eeromPara2Ram(byType);	


	//�����û�����Ĭ�ϲ���
	// sys_del_task_cfg(0,TRUE);

	ParaSaveDaemon(FALSE,NULL);
	// memset(Buf,0,sizeof(Buf));	
	// bspEEPROMWrite(EROM_OFT_INITPARA,Buf,5);

	memset(gMetCfg,0x00,sizeof(TMetCfg_Base)*MAX_MP_NUM );
}

// static void InitSchTab(void)
// {
// TScheme698ParaCfg *pPara=pGet698ShemePara();
// WORD wI,wBaseOffset=0;
// TFileRead FR;

// 	{//1����ͨ�ɼ����� 
// 		TOad60150200 *psch=(TOad60150200 *)&pPara->para_scheme_buf[0];
// 		for(wI=0;wI<MAX_SCH_GENERAL;wI++)
// 		{			
// 			FR.idwBufLen	= SIZE_PAGE;
// 			FR.idwOffset	= getParaFileOffset((BYTE)(wI+wBaseOffset),FLAG_SCHEME);
// 			FR.ipvBuf		= pPara->para_scheme_buf;
// 			FR.iszName		= FILE_SYSPARA;
// 			FileRead(&FR);
// 			if((psch->valid_flag == 0x5AA5))
// 			{
// 				s_SchIndexTab.GSchTab[wI] = psch->byShemeNo;
// 				s_SchIndexTab.Flag_GSchTab[wI>>3] |= (1<<(wI&0x07));
// 			}	
// 			psch->valid_flag	= 0xFFFF;
// 		}
// 	}
// 	{//2���¼��ɼ����� 
// 		TOad60170200 *psch=(TOad60170200 *)&pPara->para_scheme_buf[0];
// 		wBaseOffset	= MAX_SCH_GENERAL;
// 		for(wI=0;wI<MAX_SCH_EVENT;wI++)
// 		{			
// 			FR.idwBufLen	= SIZE_PAGE;
// 			FR.idwOffset	= getParaFileOffset((BYTE)(wI+wBaseOffset),FLAG_SCHEME);
// 			FR.ipvBuf		= pPara->para_scheme_buf;
// 			FR.iszName		= FILE_SYSPARA;
// 			FileRead(&FR);
// 			if((psch->valid_flag == 0x5AA5))
// 			{
// 				s_SchIndexTab.ESchTab[wI] = psch->byEventNo;
// 				s_SchIndexTab.Flag_ESchTab[wI>>3] |= (1<<(wI&0x07));
// 			}
// 			psch->valid_flag	= 0xFFFF;
// 		}
// 	}
// 	{//3���ϱ����� 
// 		TOad601D0200 *psch=(TOad601D0200 *)&pPara->para_scheme_buf[0];
// 		wBaseOffset	= MAX_SCH_GENERAL+MAX_SCH_EVENT;
// 		for(wI=0;wI<MAX_SCH_REPORT;wI++)
// 		{			
// 			FR.idwBufLen	= SIZE_PAGE;
// 			FR.idwOffset	= getParaFileOffset((BYTE)(wI+wBaseOffset),FLAG_SCHEME);
// 			FR.ipvBuf		= pPara->para_scheme_buf;
// 			FR.iszName		= FILE_SYSPARA;
// 			FileRead(&FR);
// 			if((psch->valid_flag == 0x5AA5))
// 			{
// 				s_SchIndexTab.RSchTab[wI] = psch->byReportNo;
// 				s_SchIndexTab.Flag_RSchTab[wI>>3] |= (1<<(wI&0x07));
// 			}
// 			psch->valid_flag	= 0xFFFF;
// 		}
// 	}
// 		{//4��͸������ 
// 		TOad60190200 *psch=(TOad60190200 *)&pPara->para_scheme_buf[0];
// 		wBaseOffset	= MAX_SCH_GENERAL+MAX_SCH_EVENT+MAX_SCH_REPORT;
// 		for(wI=0;wI<MAX_SCH_TRANS;wI++)
// 		{			
// 			FR.idwBufLen	= SIZE_PAGE;
// 			FR.idwOffset	= getParaFileOffset((BYTE)(wI+wBaseOffset),FLAG_SCHEME);
// 			FR.ipvBuf		= pPara->para_scheme_buf;
// 			FR.iszName		= FILE_SYSPARA;
// 			FileRead(&FR);
// 			if((psch->valid_flag == 0x5AA5))
// 			{
// 				s_SchIndexTab.TSchTab[wI] = psch->byTransNo;
// 				s_SchIndexTab.Flag_TSchTab[wI>>3] |= (1<<(wI&0x07));
// 			}
// 			psch->valid_flag	= 0xFFFF;
// 		}
// 	}
// 	pPara->nowPageNum	= 0xFF;
// }

void InitGDW698Cfg(void)
{//����698��ʼ��(�����õĲ������������ڴ�)
 TSysParaCfg *p;
DWORD dwTmp;
TFileRead FR;
BYTE Buf[5]={0};



	//p	= MemAlloc(sizeof(TSysParaCfg));
	 gpSysPara	= &_s_SysParaCfg;
	 p = gpSysPara;
	// bspEEPROMRead(EROM_OFT_INITPARA,5,Buf);
	// dwTmp	= MDW(Buf[0],Buf[1],Buf[2],Buf[3]);
	// if(dwTmp == 0x12345678)
	// {//��������ʼ��;�жϵ�
	// 	TFileWrite FW;
	// 	_eeromPara2Ram(Buf[4]);	
	// 	FW.iszName	= FILE_SYSPARA;
	// 	FW.idwOffset= getParaFileOffset(0,FLAG_SYS);
	// 	FW.ipvBuf	= p->base_para_cfg.base_para_buf;
	// 	FW.idwBufLen= SIZE_PAGE;
	// 	FileWrite(&FW);	
	// 	memset(Buf,0,sizeof(Buf));	
	// 	bspEEPROMWrite(EROM_OFT_INITPARA,Buf,5);
	// }
	//��ʼ��ϵͳ���ò���		
	// FR.iszName	= FILE_SYSPARA;
	// FR.idwOffset= getParaFileOffset(0,FLAG_SYS);
	// FR.ipvBuf	= p->base_para_cfg.base_para_buf;
	// FR.idwBufLen= sizeof(p->base_para_cfg.base_para_buf);
	// FileRead(&FR);	
	//����������
	// FR.iszName	= FILE_SYSPARA;
	// FR.idwOffset= getParaFileOffset(0,FLAG_SYS_EX);
	// FR.ipvBuf	= p->frz_para_cfg.para_frz_buf;
	// FR.idwBufLen= sizeof(p->frz_para_cfg.para_frz_buf);
	// FileRead(&FR);
	// FR.iszName	= FILE_SYSPARA;
	// FR.idwBufLen= sizeof(p->task_para_cfg.para_task_buf);
	// FR.ipvBuf	= p->task_para_cfg.para_task_buf;
	// FR.idwOffset= getParaFileOffset(0,FLAG_TASK);	
	// FileRead(&FR);			
	// // SM_P(MPV_PARA_SYS);
	// p->base_para_cfg.saveflag	= FALSE;
	// SM_V(MPV_PARA_SYS);	
	// SM_P(MPV_PARA_USER);
	// p->frz_para_cfg.saveflag	= FALSE;	
	// SM_V(MPV_PARA_USER);
	//��Ч�����㼰������������
	p->mp_para_cfg.nowPageNum	= 0xFF;	//��Ч����ҳ
	// p->sheme_para_cfg.nowPageNum= 0xFF;	//��Ч����ҳ
	// p->ctrl_para_cfg.nowPageNum	= 0xFF;	//��Ч����ҳ
	// dwParaFreshTime	= gVar.dwStartTime;	
	{//���ϵͳ��������		
		// TOad50020300 *pFrz=pGetFrz698(0x50020300);
		// TOad50030300 *pHourFrz=pGetFrz698(0x50030300);//�������ñ�
		// if(pFrz && (pFrz->byNum > MAX_FRZ_CFG_NUM) )
		// 	pFrz->byNum	= 0;
		// if(pHourFrz && (pHourFrz->byNum > MAX_FRZ_CFG_NUM) )
		// 	pHourFrz->byNum	= 0;
	}
	// dwTmp	= getSys698ParaIndex(s_OADLen[ArraySize(s_OADLen)-1].obj)+s_OADLen[ArraySize(s_OADLen)-1].wLen;
//	if(dwTmp >= sizeof(p->base_para_cfg.base_para_buf))
//		Trace("ϵͳ���ò����ռ䳬!!!");
	//��ʼ������������Ӧ��
	// InitSchTab();
	// check_clear_day_task_time();
}

void _InitTaskRuninfo(void) 
{
	g_pTaskRunInfo = &_s_TaskRunInfos[0];
	memset(g_pTaskRunInfo,0,sizeof(TTask698RunInfo)*MAX_698_TASK_NUM);
}

// void InitDase698Ex(void)
// {//��ʼ���澯����
// TData_6203 *pSys=pGetDF6203();

// 	_InitTaskRuninfo();
// 	//��ʼ�����ȫģʽ��
// 	memset(pSys->safmodeInit,0,sizeof(pSys->safmodeInit));
// 	memset(pSys->safmodeValid,0,sizeof(pSys->safmodeValid));
// 	chg_sch_init();	//��ʼ�����ķ���ʱ���
// }

void _freshPara(WORD flag)
{
TTime Time;

	GetTime(&Time);
	// dwParaFreshTime	= Time2Sec(&Time);
	// if(FLAG_SYS == flag)
	// {//����ֻˢ�±�ʶ
	// 	TBase698ParaCfg *pSysCfg=pGet698BasePara();
	// 	// SM_P(MPV_PARA_SYS);
	// 	pSysCfg->saveflag = TRUE;
	// 	SM_V(MPV_PARA_SYS);
	// }
	// else 
	if(FLAG_METER == flag)
	{
		TMP698ParaCfg *pmp698paracfg=pGet698MPPara();		
		pmp698paracfg->saveflag = TRUE;
	}
	// else if(FLAG_TASK == flag)
	// {
	// 	TTask698ParaCfg *ptaskCfg=pGet698TaskPara();
	// 	// SM_P(MPV_PARA_TK);
	// 	ptaskCfg->saveflag = TRUE;
	// 	SM_V(MPV_PARA_TK);
	// }
	// else if(FLAG_SCHEME == flag)
	// {
	// 	TScheme698ParaCfg *p698schemecfg=pGet698ShemePara();		
	// 	p698schemecfg->saveflag = TRUE;
	// }
//	else if(FLAG_FKSYS == flag)
//	{
//		TCtrl698ParaCfg *p698ctrlcfg=pGet698CtrlPara();		
//		p698ctrlcfg->saveflag = TRUE;
//	}
//	else if(FLAG_SYS_EX == flag)
//	{
//		TFrz698ParaCfg *pFrzcfg=pGet698FrzPara();		
//		// SM_P(MPV_PARA_USER);
//		pFrzcfg->saveflag	= TRUE;
//		SM_V(MPV_PARA_USER);
//	}
}

// WORD get_mins_of_tick(BYTE byType)
// {
// WORD wInterval;

// 	if(byType == MPT_376_3)
// 	{
// 		TOad45000200 cfg;
// 		sys_Para698_Read(0x45000200,&cfg);
// 		wInterval	= cfg.wSecs_Tick/60;
// 	}
// 	else //.if(HHBYTE(pPort->dwID) == MPT_ETHERNET)
// 	{
// 		TOad45100200 cfg;
// 		sys_Para698_Read(0x45100200,&cfg);
// 		wInterval	= cfg.wSecs_Tick/60;
// 	}
// 	if(wInterval == 0)
// 		wInterval	= 1;
// 	else if(wInterval > 30)
// 		wInterval	= 30;
// 	return wInterval;
// }

// void fresh_sch_time(BYTE sch_no,BYTE task_type)
// {
// 	BYTE index;
// 	WORD offset = 0xffff;
// 	TScheme698ParaCfg *pPara=pGet698ShemePara();
// 	TTime time_now;
	
// 	// SM_P(MPV_PARA_MPB);
// 	if(schNo2Index(sch_no,task_type,&index))
// 		offset	= _freshGetSchemeOff698(index,task_type);
// 	if(0xFFFF != offset)
// 	{
// 		TOad60150200 *p_sch = (TOad60150200 *)&pPara->para_scheme_buf[offset];
// 		GetTime(&time_now);
// 		if (p_sch->dwValidTime == 0xfefefefe)
// 		{
// 			p_sch->dwValidTime = Time2Sec(&time_now);
// 			_freshPara(FLAG_SCHEME);
// 		}
// 	}		
// 	SM_V(MPV_PARA_MPB);
// }


// BOOL IsMACVerifyFlag(void)
// {
// 	if(gVar.Cfg.bySysCtrlFlag & BIT1)
// 		return TRUE;
// 	else
// 		return FALSE;
// }

#endif