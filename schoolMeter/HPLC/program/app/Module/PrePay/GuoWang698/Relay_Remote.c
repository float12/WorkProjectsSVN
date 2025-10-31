//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.3
//����		�̵��������ļ�
//				���ļ�Ҫ���� �������ü̵��� �������,���йܽż������
//							 �������ü̵��� �������,���йܽż������
//							 �������ü̵��� �������,���йܽż���ƽ
//							 �������ü̵���	����/��ƽ����,�޹ܽż��
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )
#if(PREPAY_MODE == PREPAY_REMOTE)

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//�̵���ʵ��״̬����
#define ST_RELAY_OPEN				0xaa	//��բ״̬
#define ST_RELAY_CLOSE				0x55	//��բ״̬
#define CMD_RELAY_OPEN				0xaa	//��բ����
#define CMD_RELAY_CLOSE				0x55	//��բ����

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct TRelayTaskFlag_t
{
	BOOL	OpFlag;				//�����̵�����־
	BYTE	KeyDown3sTimer;		//��������ʱ������� 
}TRelayTaskFlag;

//�̵��������
typedef enum 
{
    eCMD_Relay_NotReport = 0x0000,				//���ϱ�
	eCMD_RelayOn_Report  = 0x5555,				//��բ�ϱ�
	eCMD_RelayOff_Report = 0xAAAA,				//��բ�ϱ�			
}e_Relay_Report;

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
#if( SEL_EVENT_RELAY_ERR == YES  )
TRelayErrFlag RelayErrFlag;
#endif


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static				BYTE	g_RelayFlg;				//ˢ�¼̵���ʵ��״̬��־
static	__no_init	BYTE	g_RelaySta;				//�̵���ʵ��״̬��=ST_RELAY_OPEN:��բ/=ST_RELAY_CLOSE:��բ��
static	__no_init	DWORD	g_RelayTmr;				//�̵���״̬��ʱ��
static				BOOL	g_RelayBlk;				//����������־
static				BYTE	g_RelayErrCounter;		//�̵��������������
static	__no_init	TRelayCmd		RelayCmd;
static				TRelayTaskFlag	RelayTaskFlag;
static				WORD			RelayOkStatus;
static				WORD			RelayErrStatus;
static	__no_init	BOOL			bRelayDetectFlag;//���ü̵�����բʱ�Ƿ�������̵���״̬��־
static  __no_init	e_Relay_Report	g_RelayReportflag;		//���������ϱ���־
static  __no_init   e_Relay_Report  RelayOperateNeedRepoted;//����բ�Ƿ���Ҫ�ϱ���־��0x00-�����ϱ�����բ��բ��0x55��Ҫ�ϱ���բ\0xAA��Ҫ�ϱ���բ��

//�̵���״̬ת������������Ϊ����������Ϊ��ǰ״̬������Ԫ��Ϊת����״̬
//����,Զ�̱�
static const BYTE TAB_RelayStaInside[8][13] =
{
//	eST_RlyOffR		eST_AdvOff1R	eST_AdvOff2R	eST_AllowOn		eST_AllowOnKp	eST_RelayOn		eST_RelayOnKp	eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R	eST_RlyOffGdR	eST_AdvOff1GdR	eST_AdvOff2GdR	
//	Զ����բ		Զ��Ԥ��բ1		Զ��Ԥ��բ2		��բ����		��բ������	��բ			��բ����		Զ�̵ȴ���բ	Ԥ��բ1�ȴ���բ	Ԥ��բ2�ȴ���բ	Զ����բ����	Զ��Ԥ��բ1����	 Զ��Ԥ��բ2����
	{eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	eST_Error,		(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	},	//��բ����
	{eST_AllowOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//�����բ����
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//ֱ�Ӻ�բ����
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//Ԥ��բ1����
	{eST_Error,		(eST_AdvOff2R),	(eST_AdvOff2R),	eST_Error,		eST_Error,		(eST_AdvWtOff2R),eST_Error,		eST_Error,		(eST_AdvWtOff2R),(eST_AdvWtOff2R),eST_Error,	(eST_AdvWtOff2R),(eST_AdvWtOff2R),},//Ԥ��բ2����
	{eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_AllowOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	},	//��������
	{eST_RlyOffR,	eST_AdvOff1R,	eST_AdvOff2R,	eST_AllowOn,	eST_AllowOn,	eST_RelayOn,	eST_RelayOn,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff2R,	eST_RlyOffGdR,	eST_AdvOff1GdR,	eST_AdvOff2GdR,	},	//�����������
	{eST_RlyOffR,	eST_RelayOn,	eST_AllowOn,	eST_AllowOn,	eST_AllowOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	},	//�ϵ�
};
//��������ִ��״̬�� �����������״̬��
static const WORD TAB_RelayReturnStatusInside[7][13] =
{
//		eST_RlyOffR		eST_AdvOff1R		eST_AdvOff2R		eST_AllowOn			eST_AllowOnKp		    eST_RelayOn			eST_RelayOnKp		eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R		eST_RlyOffGdR		eST_AdvOff1GdR		eST_AdvOff2GdR	
//		Զ����բ		Զ��Ԥ��բ1			Զ��Ԥ��բ2			��բ����			��բ������		    ��բ				��բ����			Զ�̵ȴ���բ		Ԥ��բ1�ȴ���բ		Ԥ��բ2�ȴ���բ		Զ����բ����		Զ��Ԥ��բ1����		Զ��Ԥ��բ2����
	{R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_ERR_OFF_PROTECT,	    (R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_OK_OVER_I_OFF,	R_OK_OVER_I_OFF,	R_OK_OVER_I_OFF,	},//��բ����
	{R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		    R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		R_OK_ALLOW_ON,		},//�����բ����
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		    R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		},//ֱ�Ӻ�բ����
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,R_ERR_AUTOON_OFF,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	},//Ԥ��բ1����
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,R_ERR_AUTOON_OFF,	(R_OK_DELAY_OFF),	(R_OK_DELAY_OFF),	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	},//Ԥ��բ2����
	{R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		    R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		},//��������
	{R_OK_UNPROTECT,	R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		    R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		},//�����������
};

//����,Զ�̱�
static const BYTE TAB_RelayStaOutside[8][13] =
{
//	���ñ�֧�ֵ�״̬���մ�״ִ̬��eST_AdvOff1R	eST_RelayOn		eST_RelayOnKp																					eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R
//	eST_RlyOffR		eST_AdvOff1R	eST_AdvOff2R	eST_AllowOn		eST_AllowOnKp	eST_RelayOn		eST_RelayOnKp	eST_WaitOffR	eST_AdvWtOff1R	eST_AdvWtOff2R  eST_RlyOffGdR	eST_AdvOff1GdR	eST_AdvOff2GdR	                   
//	Զ����բ		Զ��Ԥ��բ1		Զ��Ԥ��բ2		��բ����		��բ������	��բ			��բ����		�ȴ���բ		Ԥ��բ1�ȴ���բ	Ԥ��բ2�ȴ���բ Զ����բ����	Զ��Ԥ��բ1����	 Զ��Ԥ��բ2����                   
	{eST_RlyOffR,	eST_RlyOffR,	eST_RlyOffR,	(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	eST_Error,		(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	(eST_WaitOffR),	},	//��բ����     
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//�����բ���� 
	{eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	},	//ֱ�Ӻ�բ���� 
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//Ԥ��բ1����  
	{eST_Error,		(eST_AdvOff1R),	(eST_AdvOff1R),	(eST_AdvOff1R),	eST_Error,		(eST_AdvOff1R),	eST_Error,		eST_Error,		(eST_AdvWtOff1R),(eST_AdvWtOff1R),eST_Error,	(eST_AdvWtOff1R),(eST_AdvWtOff1R),},//Ԥ��բ2����  
	{eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	eST_RelayOnKp,	},	//��������     
	{eST_RlyOffR,	eST_AdvOff1R,	eST_AdvOff1R,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff1R,	eST_WaitOffR,	eST_AdvWtOff1R,	eST_AdvWtOff1R,	},	//����������� 
	{eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RelayOn,	eST_RelayOnKp,	eST_RelayOn,	eST_RelayOnKp,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	eST_RlyOffR,	eST_RelayOn,	eST_RelayOn,	},	//�ϵ�         
};
//��������ִ��״̬�� �����������״̬��
static const WORD TAB_RelayReturnStatusOutside[7][13] =
{
//	���ñ�֧�ֵ�״̬���մ�״ִ̬��		eST_AdvOff1R		eST_RelayOn			eST_RelayOnKp																													eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R
//		eST_RlyOffR		eST_AdvOff1R		eST_AdvOff2R		eST_AllowOn			eST_AllowOnKp			eST_RelayOn			eST_RelayOnKp			eST_WaitOffR		eST_AdvWtOff1R		eST_AdvWtOff2R		eST_RlyOffGdR		eST_AdvOff1GdR		eST_AdvOff2GdR	
//		Զ����բ		Զ��Ԥ��բ1			Զ��Ԥ��բ2			��բ����			��բ������			��բ				��բ����				Զ�̵ȴ���բ		Ԥ��բ1�ȴ���բ		Ԥ��բ2�ȴ���բ		Զ����բ����		Զ��Ԥ��բ1����		Զ��Ԥ��բ2����
	{R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	R_OK_DIRECT_OFF,	(R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,		(R_OK_DELAY_OFF),	R_ERR_OFF_PROTECT,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,		R_OK_DELAY_OFF,	},//��բ����    
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_ALLOW_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,	},//�����բ����
	{R_OK_DIRECT_ON,	R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,			R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,		R_OK_DIRECT_ON,	},//ֱ�Ӻ�բ����
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,},//Ԥ��բ1����
	{R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_PROTECT,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,	R_ERR_AUTOON_OFF,	R_OK_OFF_AUTO_ON,	R_OK_OFF_AUTO_ON,},//Ԥ��բ2����
	{R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,			R_OK_PROTECT,		R_OK_PROTECT,			R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,		R_OK_PROTECT,	},//��������    
	{R_OK_UNPROTECT,	R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,			R_OK_UNPROTECT,		R_OK_UNPROTECT,			R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,		R_OK_UNPROTECT,	},//�����������
};

//���ü̵���������Ч��״̬����Ϊ����Ĭ��״̬
static const BYTE ST_OutSideRelay[] =
{
	0,
	eST_RlyOffR,	//Զ����բ
	eST_AdvOff1R,	//Զ��Ԥ��բ1��ʱ�䵽ֱ�Ӻ�բ��
	eST_AdvOff1R,	//Զ��Ԥ��բ2��ʱ�䵽��բ����	���ñ�	ǿ��Ϊ Զ��Ԥ��բ1
	eST_RelayOn,	//��բ����						���ñ�	ǿ��Ϊ ��բ
	eST_RelayOnKp,	//��բ������					���ñ�	ǿ��Ϊ ��բ����
	eST_RelayOn,	//��բ
	eST_RelayOnKp,	//��բ����
	eST_WaitOffR,	//Զ�̵ȴ���բ
	eST_AdvWtOff1R,	//Զ��Ԥ��բ1�ȴ���բ
	eST_AdvWtOff2R,	//Զ��Ԥ��բ2�ȴ���բ
	eST_WaitOffR,	//Զ����բ����					���ñ�	ǿ��Ϊ Զ�̵ȴ���բ
	eST_AdvWtOff1R,	//Զ��Ԥ��բ1����				���ñ�	ǿ��Ϊ Զ��Ԥ��բ1�ȴ���բ
	eST_AdvWtOff2R,	//Զ��Ԥ��բ2����				���ñ�	ǿ��Ϊ Զ��Ԥ��բ2�ȴ���բ
	eST_Error,		//����״̬
};


//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static WORD ProcRelayCmdR(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime);
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �̵����������� �պϼ̵���
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
static void Drv_CloseRelay(void)
{
	if(RelayTypeConst == RELAY_INSIDE)
	{
		CLOSE_POWER_RELAY_PLUS_INSIDE;
	}
	else if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.CtrlMode == 0 )//���巽ʽ
		{
		}
		else
		{
			CLOSE_POWER_RELAY_LEVEL_OUTSIDE;
		}
	}
}


//-----------------------------------------------
//��������: �̵����������� �Ͽ��̵���
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
static void Drv_OpenRelay(void)
{
	if(RelayTypeConst == RELAY_INSIDE)
	{
		OPEN_POWER_RELAY_PLUS_INSIDE;
	}
	else if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.CtrlMode == 0 )//���巽ʽ
		{
			OPEN_POWER_RELAY_PLUS_OUTSIDE;
		}
		else
		{
			OPEN_POWER_RELAY_LEVEL_OUTSIDE;
		}
	}
	
	CLEAR_WATCH_DOG;
}


//-----------------------------------------------
//��������: ��ü̵�������
//
//����:		Type[in]--�����
//						
//����ֵ:	CMD_RELAY_OPEN(0XAA)/CMD_RELAY_CLOSE(0X55)
//		   
//��ע:
//-----------------------------------------------
static BYTE GetRelayCMD(BYTE Type)
{
	BYTE CloseRelayNo;
	
	if(RelayTypeConst == RELAY_INSIDE)
	{
		CloseRelayNo = eST_RelayOn;
	}
	else
	{
		CloseRelayNo = eST_AllowOn;
	}
	
	if(Type < CloseRelayNo)
	{
		return CMD_RELAY_OPEN;
	}
	
	return CMD_RELAY_CLOSE;
}

//---------------------------------------------------------------
//��������: У��̵�������Ͳ���
//
//����:   ��
//                   
//����ֵ:  ��
//
//��ע:   
//---------------------------------------------------------------
void CheckRelayCmd( void )
{
	WORD wResult;
	
	//У��̵�������
	if( RelayCmd.CRC32 != lib_CheckCRC32( (BYTE *)&RelayCmd, sizeof(TRelayCmd) - sizeof(DWORD)) )
	{
		wResult = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( RelaySafeRom.RelayCmd ), sizeof(TRelayCmd), (BYTE *)&RelayCmd );
		
		if( wResult == FALSE )
		{
			RelayCmd.Command = eST_RelayOn;
			RelayCmd.Alarm = FALSE;
			#if( SEL_DLT645_2007 == YES )
			RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
			#endif
			RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
			RelayCmd.CtrlMode = RelayCtrlMode_Def;
			RelayCmd.dwRelayProtectI = RelayProtectI_Def;
		}
	}
	
	//Command��Alarm��CtrlMode����һ���ǷǷ�ֵ ����ΪĬ��ֵ
	if( (RelayCmd.Command >= eST_Error) 
	 || (RelayCmd.Alarm > TRUE)		//����״ֻ̬��ΪTRUE����FALSE
	 || (RelayCmd.CtrlMode > 1) )	//�̵������Ʒ�ʽֻ��Ϊ0��1
	{
		RelayCmd.Command = eST_RelayOn;
		RelayCmd.Alarm = FALSE;
		#if( SEL_DLT645_2007 == YES )
		RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
		#endif
		RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
		RelayCmd.CtrlMode = RelayCtrlMode_Def;
		RelayCmd.dwRelayProtectI = RelayProtectI_Def;
		//����У�黹�Ǵ��� ��ΪEEPRPOM��ֵҲ������ �ָ�CRC ��ֹһֱ��ȡEEPROM
		RelayCmd.CRC32 = lib_CheckCRC32( (BYTE *)&RelayCmd, sizeof(TRelayCmd) - sizeof(DWORD) ); 
	}
}
//-----------------------------------------------
//��������: �жϼ̵�������״̬�Ϸ���
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:�������ü̵�����Щ��Ч��״̬����Ϊ��Ч��״̬
//-----------------------------------------------
void JudgeRelayCmd(void)
{
	if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.Command != ST_OutSideRelay[RelayCmd.Command])
		{
			ASSERT(0,0);
			RelayCmd.Command = ST_OutSideRelay[RelayCmd.Command];
			RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
		}
	}
}

//-----------------------------------------------
//��������: ���̵����������������Ƚϣ��ж��Ƿ����������̵���
//
//����:		NewSta[in]	�̵���������
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
static void JudgeRelayCmdChange(BYTE NewSta)
{		
	TRelayDetectFlag RelayDetectFlag;
	
	if(NewSta != RelayCmd.Command)
	{
		//�̵����������ת ��ԭ�����բ->��������բ��&&��ԭ������բ->�������բ��
		if( ((GetRelayCMD(RelayCmd.Command)==CMD_RELAY_CLOSE)&&(GetRelayCMD(NewSta)==CMD_RELAY_OPEN))	
		|| ((GetRelayCMD(RelayCmd.Command)==CMD_RELAY_OPEN)&&(GetRelayCMD(NewSta)==CMD_RELAY_CLOSE)) )
		{
			if(GetRelayCMD(NewSta) == CMD_RELAY_OPEN)			
			{
				//aa����Ҫ����4�βų���1111��������4������жϳ��Ǽ̵�����բ״̬��
				//	��Ҫ����3�γ���0000��������3������жϳ��Ǽ̵�����բ״̬��
				g_RelayFlg = 0xAA;
				RelayOperateNeedRepoted = eCMD_RelayOff_Report;
				api_SavePrgOperationRecord( ePRG_OPEN_RELAY_NO );
			}
			else
			{
				//55����Ҫ����4�βų���0000��������4������жϳ��Ǽ̵�����բ״̬��
				//	��Ҫ����3�γ���1111��������3������жϳ��Ǽ̵�����բ״̬��
				g_RelayFlg = 0x55;
				RelayOperateNeedRepoted = eCMD_RelayOn_Report;
				api_SavePrgOperationRecord( ePRG_CLOSE_RELAY_NO );
				#if( MAX_PHASE == 3)
				if(RelayTypeConst == RELAY_OUTSIDE)
				{
					if(bRelayDetectFlag != TRUE)//if(bRelayDetectFlag == FALSE)
					{
						bRelayDetectFlag = TRUE;
						RelayDetectFlag.Flag = bRelayDetectFlag;
						api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
					}
				}
				#endif
			}
			RelayTaskFlag.OpFlag = TRUE;
		}
		//������¾�Ҫ��¼
		RelayCmd.Command = NewSta;		
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	}
}
#if( SEL_EVENT_RELAY_ERR == YES  )

//-----------------------------------------------
//��������: д�롢��ȡ���ɿ�����״̬��־
//
//����:		Option READ��ȡWRITEд��CLEAR���	Status Ҫд��ĸ��ɿ�����״̬��־
//						
//����ֵ:	��ȡ�Ż᷵�أ�����״̬�����ء�д�����ҪStatus����������Ҫ
//		   
//��ע:		XXAA�����Ѿ���¼�˽���--������һ�������������¼��ˣ��´β����̵���֮ǰ���ټ�¼���ϱ�
//			0000δ��ʼ
//			A000������բ����
//			0A00������բ����
//-----------------------------------------------
WORD api_DealRelayErrFlag(BYTE Operate,WORD Status)
{
	WORD Result;
	Result = 0x0000;

	//��ȡ�͸���״̬ʱ��ҪУ��һ���Ƿ���ȷ������ȷ���EE�ָ����ָ�ʧ�ܾ�����
	if((Operate == READ)||(Operate == WRITE))
	{
		if( RelayErrFlag.CRC32 != lib_CheckCRC32((BYTE*)&RelayErrFlag,sizeof(TRelayErrFlag)-sizeof(DWORD)) )
		{
			if( api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag) == FALSE )
			{
				RelayErrFlag.EventRelayErrRecordFlag = 0;
				RelayErrFlag.CRC32 = lib_CheckCRC32((BYTE*)&RelayErrFlag, sizeof(TRelayErrFlag)-sizeof(DWORD)) ;
			}
		}
	}

	if(Operate == READ)//��ȡ
	{
		Result = RelayErrFlag.EventRelayErrRecordFlag;
	}
	else if(Operate == WRITE)//д��
	{
		if(Status == 0x00AA)
		{
			if((RelayErrFlag.EventRelayErrRecordFlag == 0xA000)||(RelayErrFlag.EventRelayErrRecordFlag == 0x0A00))
			{	//����Ѿ���ʼ��������Ϊ����
				RelayErrFlag.EventRelayErrRecordFlag = (RelayErrFlag.EventRelayErrRecordFlag|Status);
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
			}
			else
			{
				//�޿�ʼ��־������д�������־;��д��������û��Ҫ����д��
			}
		}
		else if((Status == 0xA000)||(Status == 0x0A00))
		{
			if(	RelayErrFlag.EventRelayErrRecordFlag != 0x0000)
			{
				//�ѿ�ʼ��һ�󶯣�����д�롣�ѿ�ʼ���󶯣�û��Ҫ����д�롣�ѽ�������д��
			}
			else
			{
				//δ��ʼ����������Ϊ��ʼ
				RelayErrFlag.EventRelayErrRecordFlag = Status;
				api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
			}
		}
	}
	else if(Operate == CLEAR)//����--������´μ̵���������Ҫ����
	{
		RelayErrFlag.EventRelayErrRecordFlag = 0;
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayErrFlag),sizeof(TRelayErrFlag),(BYTE*)&RelayErrFlag);
	}

	return Result;
}
#endif

//-----------------------------------------------
//��������: �̵��������ϵ��ʼ��
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void PowerUpRelay(void)
{	
	WORD wResult;
	TRelayDetectFlag RelayDetectFlag;
	
	//�ϵ�Ч��̵����������
	CheckRelayCmd();
	
	#if( MAX_PHASE == 3)
	if(RelayTypeConst == RELAY_OUTSIDE)
	{
		wResult = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
		if( wResult == FALSE )
		{
			bRelayDetectFlag = TRUE;
		}
		else
		{
			bRelayDetectFlag = RelayDetectFlag.Flag;
		}
	}
	#endif
	
	if(RelayTypeConst == RELAY_NO)
	{
		if(RelayCmd.Command != eST_RelayOn)
		{
			RelayCmd.Command = eST_RelayOn;
		}
	}
	
	//�ж����ü̵���״̬����Ч��
	JudgeRelayCmd();
	
	//�鿴�̵���״̬�Ƿ�Ϊ�Ƿ�ֵ,����Ϊ�Ƿ�ֵ���������õ���֮ǰ��״̬�������ж�ˢ�¡�
	if( 	((g_RelaySta!=ST_RELAY_CLOSE)&&(g_RelaySta!=ST_RELAY_OPEN)) 
		|| 	(api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR)==TRUE) )
	{
		//��Ϊ�Ƿ�ֵ���ϵ�ʱ���̵������״̬��Ϊ����״̬
		if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )	
		{
			g_RelaySta = ST_RELAY_OPEN;
		}
		else
		{
			g_RelaySta = ST_RELAY_CLOSE;
		}
		
		g_RelayTmr = 0;
	}
	
	//�ϵ紦��̵���״̬
	ProcRelayCmdR(eCMD_PowerOn, 0, 0);


    if(((RelayOperateNeedRepoted != eCMD_RelayOff_Report)
    &&(RelayOperateNeedRepoted != eCMD_RelayOn_Report)
    &&(RelayOperateNeedRepoted != eCMD_Relay_NotReport))//RelayOperateNeedRepotedΪ�Ƿ�ֵ
    || (api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR) == TRUE) )//�ϵ�RAM��
    {
        RelayOperateNeedRepoted = eCMD_Relay_NotReport;
    }
	//�ϵ��ʼ���˱�־λ55,3~4���ܹ��жϳ��̵�����ʵ��״̬
	g_RelayFlg = 0x55;		
	//������־:������
	g_RelayBlk = FALSE;		
	//�̵��������������
	g_RelayErrCounter = 0;												

}


//-----------------------------------------------
//��������: ��ȡ�̵�����ѹ�����ж�״̬
//
//����:		Type[in]	REMOTE_I
//						REMOTE_U
//����ֵ:	TRUE/FALSE
//		    REMOTE_I	TRUE--��һ��������ڼ̵�����բ��������ֵ
//			REMOTE_U	TRUE--�����ѹ��һ�����0.75Un
//��ע:
//-----------------------------------------------
static BOOL GetRemoteValueRelayStatus(WORD Type)
{
	WORD i,wMaxPhase;
	DWORD tU;
	DWORD tdw;
	
	#if( MAX_PHASE == 3)
	wMaxPhase = 3;
	#else
	wMaxPhase = 1;
	#endif
	
	if(Type == REMOTE_I)
	{
		for(i=0;i<wMaxPhase;i++)
		{
			if( RelayCmd.dwRelayProtectI != 0x0 )
			{
				api_GetRemoteData(PHASE_A+REMOTE_I+0x1000*i, DATA_HEX, 4, 4, (BYTE*)&tdw);
				if(tdw > RelayCmd.dwRelayProtectI)	
				{
				    break;
				}
			}
		}
		
		//��һ��������ڼ̵�����բ��������ֵ������TRUE
		if(i < wMaxPhase)
		{
			return TRUE;
		}
	}
	else if(Type == REMOTE_U)
	{
		//�����ѹ��һ�����0.75Un������TRUE
		tU = wStandardVoltageConst * 75 / 100;
		
		for(i=0;i<wMaxPhase;i++)
		{
			api_GetRemoteData(PHASE_A+REMOTE_U+0x1000*i, DATA_HEX, 1, 4, (BYTE*)&tdw);
			
            if(tdw > tU) //0.75Un
			{
				break;
			}
		}
		
		if(i < wMaxPhase)
		{
			return TRUE;	
		}
	}
	
	return FALSE;	
}


//-----------------------------------------------
//��������: �̵���������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
static void Task_Sec_Relay(void)
{
	BYTE 	i,j,k,NewSta,RelayErrCheckFlag;
	WORD	wResult;
	BOOL 	RelayProtectIFlag;
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	
	FunctionConst(RELAY_TASK);

	RelayErrCheckFlag = FALSE;
	
	
	if(RelayTypeConst == RELAY_NO)
	{
		//û�м̵����ı� ���й����л���һ���Ӽ��һ�²���
		api_GetRtcDateTime(DATETIME_ss,&NewSta);
		if( NewSta == (eTASK_RELAY_ID*3+3) )
		{	
			CheckRelayCmd(); 
		}
		if(RelayCmd.Command != eST_RelayOn)
		{
			RelayCmd.Command = eST_RelayOn;
		}
		
		return;
	}
	
	CheckRelayCmd(); 
	
	//�ж����ü̵���״̬����Ч��
	JudgeRelayCmd();
	
	//�������³���3s
	if( (DOWN_KEY_PRESSED) || (UP_KEY_PRESSED) )
	{
		if( RelayTaskFlag.KeyDown3sTimer < 5 )
		{
			RelayTaskFlag.KeyDown3sTimer++;
		}
		
		//���°�����������
		if( RelayTaskFlag.KeyDown3sTimer == 3 )
		{
			NewSta = RelayCmd.Command;
			if(RelayCmd.Command == eST_AllowOn)
			{
				NewSta = eST_RelayOn;
			}
			else if(RelayCmd.Command == eST_AllowOnKp)
			{
				NewSta = eST_RelayOnKp;
			}
			
			JudgeRelayCmdChange(NewSta);			
		}
	}
	else
	{
		RelayTaskFlag.KeyDown3sTimer = 0;
	}
		
	//����������־,���������жϼ̵�����־		
	if(g_RelayBlk == TRUE)											
	{
		if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )
		{
			g_RelayFlg = 0xAA;
		}
		else
		{
			g_RelayFlg = 0x55;
		}
		RelayTaskFlag.OpFlag = TRUE;
	}
			
	//���̵���ʵ��״̬
	{
		g_RelayFlg <<= 1;
		
		j = 0;
		k = 0;
		
		#if(MAX_PHASE == 1)
		for(i=0;i<240;i++)//Լ668��ʱ�ӣ�32768��20.4ms �͸ߵ͵�ƽ״̬�޹�
		{
			if(RELAY_STATUS_OPEN)
			{
				j++;
			}
			else
			{
				k++;
			}
			api_Delay100us(1);
		}
		#else
		if(RelayTypeConst == RELAY_INSIDE)
		{
			if(!RELAY_STATUS_OPEN)
			{
				j = 70;
			}
		}
		else if( (RelayTypeConst == RELAY_OUTSIDE) && (MeterTypesConst == METER_ZT) )//ֱͨ���ñ�
		{
			for( i = 0; i < 240; i++ )//Լ668��ʱ�ӣ�32768��20.4ms �͸ߵ͵�ƽ״̬�޹�
			{
				if(RELAY_STATUS_OPEN)
				{
					j++;
				}
				else
				{
					k++;
				}
				api_Delay100us(1);
			}
		}
		else
		{
			//�������ü̵���
			if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )
			{
				j = 0;				
			}
			else
			{
				if( bRelayDetectFlag == TRUE )
				{
					j = 0;
				}
				else
				{
					j = 70;
				}
			}
		}
		#endif

		//0x20--��Ǳ��״̬	1--Ǳ��		0--��
		if( (api_GetSampleStatus(0x40)==0) || (j>30) )
		{
			g_RelayFlg |= 0x01;
		}
		if( (g_RelayFlg&0x0F) == 0x0F )			//4��
		{
			g_RelaySta = ST_RELAY_CLOSE;		//ʵ�ʺ�բ
			RelayErrCheckFlag = TRUE;
		}
		else if( (g_RelayFlg&0x0F) == 0x00 )	//4��
		{			
			if( RelayTypeConst == RELAY_INSIDE )
			{
				g_RelaySta = ST_RELAY_OPEN;		//ʵ����բ
				RelayErrCheckFlag = TRUE;
			}
			else
			{
				if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )	//������բ
				{
					g_RelaySta = ST_RELAY_OPEN;		//ʵ����բ
					RelayErrCheckFlag = TRUE;
				}
				else
				{
					#if (SEL_EVENT_RELAY_ERR == YES)
					if( (api_DealRelayErrFlag(READ,0x0000) & 0x00AA) == 0x00AA )
					{
						api_DealRelayErrFlag(CLEAR,0x0000);//�帺�ɿ�����
					}
					#endif
				}
			}					
		}
		else
		{
		}
				
		#if(MAX_PHASE == 3)
		if(RelayTypeConst == RELAY_OUTSIDE)
		{
			if(g_RelaySta == ST_RELAY_CLOSE)
			{
			    if( GetRelayCMD(RelayCmd.Command) == CMD_RELAY_CLOSE )//�����բ
			    {
    				if( bRelayDetectFlag != FALSE )//if( bRelayDetectFlag == TRUE )
    				{
    					bRelayDetectFlag = FALSE;
    					RelayDetectFlag.Flag = bRelayDetectFlag;
    					api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
    				} 
			    }
			}
		}
		#endif
	}

	if( RelayErrCheckFlag != FALSE )//ˢ�³�ʵ��״̬����м̵����󶯼��
	{
        //���ɿ����󶯼��� ��60s��120s��180�����������̵������Σ��������ٴ���
        if( GetRelayCMD(RelayCmd.Command) != g_RelaySta )
        {
            if(g_RelayErrCounter < 230)
            {
                g_RelayErrCounter++;
            }
            
            if( (g_RelayErrCounter%60) == 0 )
            {
                RelayTaskFlag.OpFlag = TRUE;
            }
            #if( SEL_EVENT_RELAY_ERR == YES  )
			//������β����̵���֮��δ��¼\�ϱ������ɿ����󶯣�����Ҫ��¼���ϱ���ͬʱ���Ѽ�¼�¼���־
			if((api_DealRelayErrFlag(READ,0x0000) == 0x0000)&&(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_RELAY_ERR )== TRUE ))
			{
				if(GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN )//��բ����ĸ��ɿ�����--���
				{
					api_DealRelayErrFlag(WRITE,0x0A00);
            	}
				else//��բ����ĸ��ɿ�����--���
				{
					api_DealRelayErrFlag(WRITE,0xA000);
				}
			}
			#endif
            api_SetSysStatus(eSYS_STATUS_RELAY_ERR);
        }
        else
        {
            g_RelayErrCounter = 0;
            api_ClrSysStatus(eSYS_STATUS_RELAY_ERR);
    
    
            //�ϱ�����բ�ɹ�
            if( (g_RelayReportflag == eCMD_RelayOn_Report) && (g_RelaySta == 0x55) ) 
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//�̵����쳣����բ���𣬺�բ�����״̬��־
				if(api_DealRelayErrFlag(READ,0x0000) & 0x0A00)
				{
					api_DealRelayErrFlag(CLEAR,0x0000);
				}
				#endif
		
				api_SetFollowReportStatus(eSTATUS_CloseRelaySucceed);
                g_RelayReportflag = eCMD_Relay_NotReport;
            }
            else if( (g_RelayReportflag == eCMD_RelayOff_Report) && (g_RelaySta == 0xAA) )
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//�̵����쳣�ɺ�բ������բ�����״̬��־
				if(api_DealRelayErrFlag(READ,0x0000) & 0xA000)
				{
					api_DealRelayErrFlag(CLEAR,0x0000);
				}
				#endif
				
				api_SetFollowReportStatus(eSTATUS_OpenRelaySucceed);
                g_RelayReportflag = eCMD_Relay_NotReport;
            }
            else
            {
            }
        }
	}
			
	//����ʱ������բ��ʱ���������ʱ����բ�Իָ���ʱ
	{
		NewSta = RelayCmd.Command;	
		//��ǰ����
		RelayProtectIFlag = GetRemoteValueRelayStatus(REMOTE_I);
		
		switch (RelayCmd.Command)
		{
		case eST_WaitOffR://����״̬
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//��բ����״̬������բ��ʱ���ҵ���>����ֵ��
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)											
					{
						NewSta = eST_RlyOffGdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//��բ״̬
							NewSta = eST_RlyOffR;								
						}
					}
					else
					{
						//��բ״̬������բ��ʱ���ҵ���<����ֵ��
						NewSta = eST_RlyOffR;									
					}
				}
			}
			break;
		case eST_AdvWtOff1R://Զ��Ԥ��բ1�ȴ���բ
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//��բ����״̬������բ��ʱ���ҵ���>����ֵ��
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)
					{
						NewSta = eST_AdvOff1GdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//Ԥ��բ״̬
							NewSta = eST_AdvOff1R;								
						}
					}
					else
					{
						//Ԥ��բ״̬����Ԥ��բ��ʱ���ҵ���<����ֵ��
						NewSta = eST_AdvOff1R;							
    				}
    				
    				if( NewSta == eST_AdvOff1R )
    				{
    				    api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                        if( RelayPara.dwOffWarnTime == 0 )
                        {
                            RelayPara.dwOffWarnTime = 300;    //����ЧֵʱĬ�ϰ���5����
                        }
    					g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
				}
			}
			break;
		case eST_AdvOff1R://Ԥ��բ1״̬ 
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			//��Ҫ������ó��� ��ֹg_RelayTmr���0����һ���޷���բ
			if( g_RelayTmr == 0 )
			{
				NewSta = eST_RelayOn;
			}
			break;
		case eST_AdvWtOff2R://Զ��Ԥ��բ2�ȴ���բ
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					//��բ����״̬������բ��ʱ���ҵ���>����ֵ��
					if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)					
					{
						NewSta = eST_AdvOff2GdR;									
						g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
						if(g_RelayTmr == 0)
						{
							//Ԥ��բ״̬
							NewSta = eST_AdvOff2R;								
						}
					}
					else
					{
						//Ԥ��բ״̬����Ԥ��բ��ʱ���ҵ���<����ֵ��
						NewSta = eST_AdvOff2R;	
						
    				}
    				if( NewSta == eST_AdvOff2R )
    				{
    				    //�����ݴ����բ�Իָ�ʱ�� ��λ����
    					api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                        if( RelayPara.dwOffWarnTime == 0 )
                        {
                            RelayPara.dwOffWarnTime = 300;    //����ЧֵʱĬ�ϰ���5����
                        }
    					g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
				}
			}
			break;
		case eST_AdvOff2R://Ԥ��բ2״̬
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			//��Ҫ������ó��� ��ֹg_RelayTmr���0����һ���޷���բ
			if( g_RelayTmr == 0 )
			{
				NewSta = eST_AllowOn;
			}
			break;
		case eST_RlyOffGdR://��բ����״̬
		case eST_AdvOff1GdR://Ԥ��բ1����״̬
		case eST_AdvOff2GdR://Ԥ��բ2����״̬
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			if( (g_RelayTmr==0) || (RelayProtectIFlag==FALSE) )//24Сʱ����ʱ�䵽��С����բ��������				
			{
				if( RelayCmd.Command == eST_RlyOffGdR )
				{
					NewSta = eST_RlyOffR;
				}
				else if( RelayCmd.Command == eST_AdvOff1GdR )
				{
					NewSta = eST_AdvOff1R;
				}
				else
				{
					NewSta = eST_AdvOff2R;
				}
				
				if ( (NewSta==eST_AdvOff1R) || (NewSta==eST_AdvOff2R) )			
				{
					//�����ݴ����բ�Իָ�ʱ�� ��λ����
					api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
                    if( RelayPara.dwOffWarnTime == 0 )
                    {
                        RelayPara.dwOffWarnTime = 300;    //����ЧֵʱĬ�ϰ���5����
                    }
					g_RelayTmr = RelayPara.dwOffWarnTime;
				}
			}
			break;
		default:
			break;
		}
		
		JudgeRelayCmdChange(NewSta);
	}
}

//-----------------------------------------------
//��������: �̵���Сʱ����
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
static void Task_Hour_Relay(void)
{
	//ÿСʱֱ�������̵�������ʱ�����󶯼�������55~220֮�䲻����������ֹ��ʱ����Ƶ��������
	if( (g_RelayErrCounter<55) || (g_RelayErrCounter>220) )
	{
		RelayTaskFlag.OpFlag = TRUE;
	}
}

//-----------------------------------------------
//��������: �̵�����ѭ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void RelayTask(void)
{	
	BOOL RelayVolFlag;
	
	//�����ж�
	if( api_GetSysStatus(eSYS_STATUS_POWER_ON) == FALSE )	
	{
		return;
	}
	
	//������
	if( api_GetTaskFlag(eTASK_RELAY_ID,eFLAG_SECOND) == TRUE )
	{		
		Task_Sec_Relay();
		api_ClrTaskFlag(eTASK_RELAY_ID,eFLAG_SECOND);
	}
	
	if(RelayTypeConst != RELAY_NO)	
	{	
		//Сʱ����
		if( api_GetTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR) == TRUE )
		{
			Task_Hour_Relay();
			api_ClrTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR);
		}
			
		//�յ��̵���������Ϣ
		if(RelayTaskFlag.OpFlag == TRUE)
		{
			RelayVolFlag = GetRemoteValueRelayStatus(REMOTE_U);
			
			//((ģ��δ��||ֹͣͨѶ)&&����65%Un)
			if( (RelayVolFlag==TRUE) || (RelayTypeConst == RELAY_OUTSIDE) )						
			{
				if(GetRelayCMD(RelayCmd.Command) == CMD_RELAY_OPEN)	
				{
					Drv_OpenRelay();
					g_RelayFlg = 0xAA;//10101010
                    //�Ƿ��ϱ���־
                    if(RelayOperateNeedRepoted == eCMD_RelayOff_Report)
                    {
					    //�ǳ���ģʽ���ϱ�,��ֹ���ڳ�ʼ��������ϱ�
					    if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
					
						{
							g_RelayReportflag = eCMD_RelayOff_Report;//Ҫ�ϱ���բ  
						}	
					}
					RelayOperateNeedRepoted = eCMD_Relay_NotReport;
				}
				else
				{
					Drv_CloseRelay();
					g_RelayFlg = 0x55;//01010101
					//�Ƿ��ϱ���־
					if(RelayOperateNeedRepoted == eCMD_RelayOn_Report)
                    {
					    //�ǳ���ģʽ���ϱ�,��ֹ���ڳ�ʼ��������ϱ�
					    if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )
					
						{
							g_RelayReportflag = eCMD_RelayOn_Report;//Ҫ�ϱ���բ  
						}	
					}
					RelayOperateNeedRepoted = eCMD_Relay_NotReport;
				}
				
				//������
				g_RelayBlk = FALSE;											
			}
			else
			{
				//����
				g_RelayBlk = TRUE;											
			}
			
			RelayTaskFlag.OpFlag = FALSE;
		}	
	}	
}


//-----------------------------------------------
//��������: �����������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void Task_ClearMeter_Relay(void)
{	
	WORD wResult;
	TRelayDetectFlag RelayDetectFlag;
	
	//Ч��̵�������
	CheckRelayCmd(); 

	//�̵�������״̬
	if(RelayCmd.Command == eST_AllowOnKp)
	{
		RelayCmd.Command = eST_RelayOnKp;
	}
	else if(RelayCmd.Command != eST_RelayOnKp)
	{
		RelayCmd.Command = eST_RelayOn;	
	}
	
	//���ü̵�����բʱ�Ƿ�������̵���״̬��־
	bRelayDetectFlag = TRUE;
	RelayDetectFlag.Flag = TRUE;
	//�̵�������״̬	TRUE:������FASLE��������
	RelayCmd.Alarm = FALSE;	
	//��������״̬
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	//�����Ƿ�������̵���״̬��־
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);

	RelayTaskFlag.OpFlag = TRUE;
	RelayTaskFlag.KeyDown3sTimer = 0;
	g_RelayReportflag = eCMD_Relay_NotReport;		//�ϱ���־����
	RelayOperateNeedRepoted = eCMD_Relay_NotReport;          //��ʼ������Ҫ�ϱ���ʶ
	g_RelayFlg = 0x55;				//����־����բ	
	g_RelaySta = ST_RELAY_CLOSE;	//ʵ��״̬����բ
	g_RelayBlk = FALSE;				//������־:������	
	g_RelayTmr = 0;
	g_RelayErrCounter = 0;
	#if( SEL_EVENT_RELAY_ERR == YES  )
	api_DealRelayErrFlag(CLEAR,0x0000);
	#endif
	//���㸺�ɿ����𶯱�־
	api_ClrSysStatus(eSYS_STATUS_RELAY_ERR);
}


//-----------------------------------------------
//��������: ���ڵ���ʼ������
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void FactoryInitRelay(void)
{
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	
	//�̵�������״̬
	if( RelayKeepPowerFlag == TRUE )//���ñ���
	{
		RelayCmd.Command = eST_RelayOnKp;
	}
	else
	{
		RelayCmd.Command = eST_RelayOn;
	}
	
	//�̵�������״̬	TRUE:������FASLE��������
	RelayCmd.Alarm = FALSE;	
	//��բ��ʱʱ�䣨���ӣ�
	#if( SEL_DLT645_2007 == YES )
	RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;	
	#endif
	//���������ޱ�����ʱʱ��	��λ������
	RelayCmd.wOverIProtectTime = OverIProtectTime_Def;	
	//�̵������Ʒ�ʽ	�̵�������������� 0--���� 1--����
	RelayCmd.CtrlMode = RelayCtrlMode_Def;	
	//�̵�����բ��������ֵ	��λ��A������-4	
	RelayCmd.dwRelayProtectI = RelayProtectI_Def;
	//��������״̬
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	//�ݴ���բ�Իָ�ʱ�� ��λ����	
	RelayPara.dwOffWarnTime = 300;	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
}


//-----------------------------------------------
//��������: Զ��������ú���
//
//����:		Cmd[in]	0��բ 1�����բ 2ֱ�Ӻ�բ 3Ԥ��բ1(ʱ�䵽ֱ�Ӻ�բ) 4Ԥ��բ2(ʱ�䵽��բ����)
//					5���� 6������ 7�ϵ� 8���� 9�������	
//			RelayWaitOffTime[in] �澯��ʱ unsigned����λ�����ӣ����㣺0��,Ϊ����645���˴�ΪWORD
//			RelayOffWarnTime[in] �޵���ʱ long-unsigned����λ�����ӣ����㣺0��ֵΪ0��ʾ�����޵磩				
//����ֵ:	���ع�ԼҪ���Ӧ��״̬�֣���bit15Ϊ1ʱ���������״̬��
//		   
//��ע:
//-----------------------------------------------
static WORD ProcRelayCmdR(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime)
{
	BYTE	NewSta;
	WORD	wReturnStatus;
	BOOL 	RelayProtectIFlag;
	TRelayPara	RelayPara;
	
	wReturnStatus = R_OK_WARN;
	
	if (Cmd >= eCMD_RemoteNum)
	{	
		//��Ч����
		return R_ERR_PASSWORD;
	}
	
	CheckRelayCmd(); 
		
	//���������
	if( (Cmd==eCMD_InAlarm) || (Cmd==eCMD_OutAlarm) )												//�����������������
	{		
		if(Cmd == eCMD_InAlarm)
		{
			RelayCmd.Alarm = TRUE;
			wReturnStatus = R_OK_WARN;
		}
		else
		{
			RelayCmd.Alarm = FALSE;
			wReturnStatus = R_OK_UNWARN;
		}
		
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
		
		return wReturnStatus;
	}
	
	if(RelayTypeConst == RELAY_NO)
	{
		return R_ERR_PASSWORD;
	}
	
	//�ж����ü̵���״̬����Ч��
	JudgeRelayCmd();	
	
	//��ȡ�̵���ת��״̬
	if(RelayTypeConst == RELAY_INSIDE)
	{
		NewSta = TAB_RelayStaInside[Cmd][RelayCmd.Command-1];
	}
	else
	{
		NewSta = TAB_RelayStaOutside[Cmd][RelayCmd.Command-1];
	}
	//���Զ�̷���״̬
	if(Cmd < eCMD_PowerOn)
	{
		if(RelayTypeConst == RELAY_INSIDE)
		{
			wReturnStatus = TAB_RelayReturnStatusInside[Cmd][RelayCmd.Command-1];
		}
		else
		{
			wReturnStatus = TAB_RelayReturnStatusOutside[Cmd][RelayCmd.Command-1];
			//��Ϊ���ñ����ͺ�բ��������ҷ���Ϊֱ�Ӻ�բ״̬��ʱ����Ϊ���������բ״̬
			if((Cmd == eCMD_IndirOn)&&(wReturnStatus == R_OK_DIRECT_ON))
			{
				wReturnStatus = R_OK_ALLOW_ON;
			}
		}
	}

    //�������״̬���ش����ڴ˴�ֱ�ӷ���
    if( (NewSta == 0) || (NewSta >= eST_Error) )
    {
        return wReturnStatus;
    }
    
	//��ǰ����
	RelayProtectIFlag = GetRemoteValueRelayStatus(REMOTE_I);
	
	//���д����ŵ�״̬��Ҫ��һ���ж�
	if( Cmd == eCMD_RelayOff )												
	{
		if( NewSta == eST_WaitOffR )											
		{
			g_RelayTmr = (DWORD)RelayWaitOffTime*60;
			
			if( g_RelayTmr == 0 )
			{				
				//��բ����״̬������բ��ʱ���ҵ���>����ֵ��
				if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)					
				{
					NewSta = eST_RlyOffGdR;									
					g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
					if (g_RelayTmr == 0)
					{
						//��բ״̬
						NewSta = eST_RlyOffR;								
					}
				}
				else
				{
					//��բ״̬������բ��ʱ���ҵ���<����ֵ��
					NewSta = eST_RlyOffR;									
				}
			}
		}
		
		//������״̬ ����״̬Ҳ��Ҫ��һ���ж�
		if( NewSta == eST_RlyOffGdR )
		{
			wReturnStatus = R_OK_OVER_I_OFF;
		}
		else if( NewSta == eST_RlyOffR )
		{
			wReturnStatus = R_OK_DIRECT_OFF;
		}
		else
		{
		}
	}
	else if( (Cmd==eCMD_AdvOff1) || (Cmd==eCMD_AdvOff2) )
	{
		//�յ�Ԥ��բ1����,Ԥ��բ2����
		//645����delayΪ0���ش���������Ϊ0ʱӦ���·�������բ���� 
        if( RelayOffWarnTime == 0 )
        {
            wReturnStatus = R_ERR_AUTOON_TIME;
            return wReturnStatus;
        }
//		RelayPara.dwOffWarnTime = (DWORD)RelayOffWarnTime*5*60;   //��5����Ϊʱ�䵥λ-645��Ҫ��698�޴�Ҫ��

		RelayPara.dwOffWarnTime = (DWORD)RelayOffWarnTime*60;  

        if( (NewSta == eST_AdvWtOff1R) || (NewSta == eST_AdvWtOff2R) )
		{
		    g_RelayTmr = (DWORD)RelayWaitOffTime*60;       
		    if( g_RelayTmr == 0 )
            {
    			//��բ����״̬������բ��ʱ���ҵ���>����ֵ��
    			if( (RelayTypeConst==RELAY_INSIDE) && (RelayProtectIFlag==TRUE)	)						
    			{
    			    if( NewSta == eST_AdvWtOff1R )
    			    {
    				    NewSta = eST_AdvOff1GdR;
    				}
    				else
    				{
    				    NewSta = eST_AdvOff2GdR;
    				}

    				g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
    				if(g_RelayTmr == 0)
    				{
    					//Ԥ��բ״̬
    					if( NewSta == eST_AdvOff1GdR )
        			    {
        				    NewSta = eST_AdvOff1R;
        				}
        				else
        				{
        				    NewSta = eST_AdvOff2R;
        				}
        				g_RelayTmr = RelayPara.dwOffWarnTime;
    				}
    			}
    			else
    			{
    				//Ԥ��բ״̬����Ԥ��բ��ʱ���ҵ���<����ֵ��
    				if( NewSta == eST_AdvWtOff1R )
    				{
    				    NewSta = eST_AdvOff1R;
    				}
    				else
    				{
    				    NewSta = eST_AdvOff2R;
    				}
    				g_RelayTmr = RelayPara.dwOffWarnTime;
    			}
    		}
    		else
    		{
    		}
		}
		else
		{
			g_RelayTmr = RelayPara.dwOffWarnTime;
		}

		//������״̬ ����״̬Ҳ��Ҫ��һ���ж�
		if( (NewSta == eST_AdvWtOff1R) || (NewSta == eST_AdvWtOff2R) )
		{
			wReturnStatus = R_OK_DELAY_OFF;
		}
		else if( (NewSta == eST_AdvOff1GdR) || (NewSta == eST_AdvOff2GdR) )
		{
			wReturnStatus = R_OK_OVER_I_OFF;
		}
		else
		{
		}
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
	}
	else if( Cmd == eCMD_PowerOn )//�ϵ粻����״̬ת�����ж�
	{
		RelayTaskFlag.OpFlag = TRUE;
	}
		
	//״̬��ת��
	JudgeRelayCmdChange(NewSta);
		
	return wReturnStatus;
}
	
//-----------------------------------------------
//��������: Զ��������ú���
//
//����:		Cmd[in]	0��բ 1�����բ 2ֱ�Ӻ�բ 3Ԥ��բ1(ʱ�䵽ֱ�Ӻ�բ) 4Ԥ��բ2(ʱ�䵽��բ����)
//					5���� 6������ 7�ϵ� 8���� 9�������	
//			RelayWaitOffTime[in] �澯��ʱ unsigned����λ�����ӣ����㣺0��,Ϊ����645���˴�ΪWORD
//			RelayOffWarnTime[in] �޵���ʱ long-unsigned����λ�����ӣ����㣺0��ֵΪ0��ʾ�����޵磩				
//����ֵ:	���ع�ԼҪ���Ӧ��״̬�֣���bit15Ϊ1ʱ���������״̬��
//		   
//��ע:
//-----------------------------------------------
WORD api_Set_RemoteRelayCmd(BYTE Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime)
{
	WORD tw;
	
	RelayOkStatus = 0;
	RelayErrStatus = 0;

	//�޵���ʱ��Ч ����բ��ʱʧЧ
	if( RelayOffWarnTime )
	{
		RelayWaitOffTime = 0;
	}
	tw = ProcRelayCmdR(Cmd, RelayWaitOffTime, RelayOffWarnTime);
	
	if(tw & 0x8000)
	{
		RelayErrStatus = ( tw&(~0x8000) );
	}
	else
	{
		RelayOkStatus = tw;
	}
	
	return tw;
}

//-----------------------------------------------
//��������: ��ü̵���״̬
//
//����:		Type[in]	0--�̵�������״̬	���� 0-FALSE--�����բ	1-TRUE--������բ  2--������բ��ʱ���ߴ������ʱ
//						1--�̵���ʵ��״̬	���� 0-FALSE--��բ״̬	1-TRUE--��բ״̬  2--��բ����״̬
//						2--����״̬			���� 0-FALSE--�Ǳ���		1-TRUE--����״̬
//						3--����״̬			���� 0-FALSE--��Զ�̱���	1-TRUE--Զ�̱���
//����ֵ:	0/1/2 WORD��
//��ע:
//-----------------------------------------------
WORD api_GetRelayStatus(BYTE Type)
{
	WORD wStatus;	
	wStatus = 0;
	
	switch(Type)
	{
		case 0x00:
			//�̵�������״̬	0-FALSE--�����բ	1-TRUE--������բ  2--��բ��ʱ�ʹ������ʱ
			if( 	(RelayCmd.Command==eST_RlyOffR)
				||	(RelayCmd.Command==eST_AdvOff1R) 
				||	(RelayCmd.Command==eST_AdvOff2R) )
			{
				wStatus = 1;
			}
			else if( 	(RelayCmd.Command==eST_WaitOffR)
					||	(RelayCmd.Command==eST_AdvWtOff1R)
					||	(RelayCmd.Command==eST_AdvWtOff2R)
					||	(RelayCmd.Command==eST_RlyOffGdR)
					||	(RelayCmd.Command==eST_AdvOff1GdR)
					||	(RelayCmd.Command==eST_AdvOff2GdR) )
			{
			    wStatus = 2;
			}
			else
			{
				wStatus = 0;
			}
			break;
		case 0x01:
			//�̵���ʵ��״̬	0-FALSE--��բ״̬	1-TRUE--��բ״̬  2-��բ����״̬
			if(g_RelaySta == ST_RELAY_OPEN)
			{
			    if((RelayCmd.Command==eST_AllowOn) || (RelayCmd.Command==eST_AllowOnKp))
    			{
    				wStatus = 2;
    			}
				else
				{
				    wStatus = 1;
				}
			}
			else
			{
			    wStatus = 0;
			}
			break;
		case 0x02:
			//����״̬			FALSE--�Ǳ���		TRUE--����״̬
			if( (RelayCmd.Command==eST_RelayOnKp) || (RelayCmd.Command==eST_AllowOnKp) )
			{
				wStatus = 1;
			}
			break;
		case 0x03:
			//����״̬			FALSE--��Զ�̱���	TRUE--Զ�̱���
			if(RelayCmd.Alarm == TRUE)
			{
				wStatus = 1;
			}
			break;
		default:
			break;
	}
		
	return wStatus;
}


//-----------------------------------------------
//��������: ��ȡ��������ִ�С�����״̬��
//
//����:		Type[in] 	0--��������ִ��״̬��
//						1--�����������״̬�� 
//									
//����ֵ:	״̬��
//		   
//��ע:
//-----------------------------------------------
WORD GetRelayReturnStatus(BYTE Type)
{
	if(Type == 0)
	{
		return RelayOkStatus;
	}
	else
	{
		return RelayErrStatus;
	}
}


//-----------------------------------------------
//��������: �����̵�������
//
//����:		Type[in] 	0--�̵�����բ��������ֵ	
//						1--���������ޱ�����ʱʱ��   
//						2--��բ��ʱʱ��
//						3--�̵�������������� 0--���� 1--����
//			Buf[out] �������ݻ���			
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD ReadRelayPara(BYTE Type,BYTE *Buf)
{
	if(Type == 0)
	{
		//�̵�����բ��������ֵ    double-long-unsigned����λ��A������-4��
		memcpy(Buf,(BYTE*)&RelayCmd.dwRelayProtectI,4);
	}
	else if(Type == 1)
	{
		//���������ޱ�����ʱʱ��  long-unsigned����λ�����ӣ�����0��
		memcpy(Buf,(BYTE*)&RelayCmd.wOverIProtectTime,2);
	}
	#if( SEL_DLT645_2007 == YES )
	else if( Type == 2 )
	{
		Buf[0] = RelayCmd.wRelayWaitOffTime;//��բ��ʱʱ��
		Buf[1] = RelayCmd.wRelayWaitOffTime/0x100;
	}
	#endif	
	else if( Type == 3 )
	{
		Buf[0] = RelayCmd.CtrlMode;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
	
}

//-----------------------------------------------
//��������: д�̵�������
//
//����:		Type[in] 	0--�̵�����բ��������ֵ	
//						1--���������ޱ�����ʱʱ��   
//						2--��բ��ʱʱ��
//						3--�̵�������������� 0--���� 1--����
//			Buf[in] д������			
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
WORD WriteRelayPara(BYTE Type,BYTE *Buf)
{
	if(Type == 0)
	{
		//�̵�����բ��������ֵ    double-long-unsigned����λ��A������-4��
		memcpy((BYTE*)&RelayCmd.dwRelayProtectI,Buf,4);
	}
	else if(Type == 1)
	{
		//���������ޱ�����ʱʱ��  long-unsigned����λ�����ӣ�����0��
		memcpy((BYTE*)&RelayCmd.wOverIProtectTime,Buf,2);
	}
	#if( SEL_DLT645_2007 == YES )
	else if( Type == 2 )
	{
		RelayCmd.wRelayWaitOffTime = Buf[1]*0x100+Buf[0]; 
	}
	#endif	
	else if( Type == 3 )
	{
		if(Buf[0] == 1)
		{
			RelayCmd.CtrlMode = 1;
		}
		else
		{
			RelayCmd.CtrlMode = 0;
		}
	}
	else
	{
		return FALSE;
	}
	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	
	return TRUE;
}

//-----------------------------------------------
//��������: ��Լ�ü̵�������״̬��
//
//����:		Type[in] 	0x01--������
//						0x02--MAC����
//						0x04--�������/δ��Ȩ
//						0x08--��ȫ��֤��ʱ
//						0x10--ʱ���ǩ��Ч
//����ֵ:	TRUE/FALSE
//
//��ע:��Щ����״̬�ڹ�Լ��ͷ����ˣ������ڹ�Լ����Ҫ���ã������ܵ�Set_RelayCmdR������ǰ���Ҳ�Ϳ��Բ�������
//-----------------------------------------------
void api_SetRelayRelayErrStatus(BYTE Type)
{
	RelayErrStatus = 0;
	RelayOkStatus = 0;
	
	if(Type == 0x01)
	{
		RelayErrStatus = R_ERR_HUNGUP; //������
	}
	else if(Type == 0x02)
	{
		RelayErrStatus = R_ERR_MAC; //MAC����
	}
	else if(Type == 0x04)
	{
		RelayErrStatus = R_ERR_PASSWORD; //�������/δ��Ȩ
	}
	else if(Type == 0x08)
	{
		RelayErrStatus = R_ERR_AUTO_OVERTIME; //��ȫ��֤��ʱ
	}
	else if(Type == 0x10)
	{
		RelayErrStatus = R_ERR_TIME; //ʱ���ǩ��Ч
	}
	
	RelayErrStatus &= ~(0x8000);
}
#endif
#endif//#if( PREPAY_STANDARD == PREPAY_GUOWANG_698 )


