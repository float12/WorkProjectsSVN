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
#if(PREPAY_MODE == PREPAY_LOCAL)
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
	eRELAY_OPERATE	OpFlag;				//�����̵�����־
	BYTE			KeyDown3sTimer;		//��������ʱ������� 
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
TRelayErrFlag RelayErrFlag;


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//static	__no_init	BYTE			g_RelayInitFlag;        //�̵�����ʼ����־λ ����Ǯ����ʼ����Զ��������й��ʺͷ��� ֱ�ӹر���բ�� 0x55-�ѳ�ʼ������δִ�� 0xAA-������ִ�У���Ϊ0xAA�����жϲ��ϱ���բ�ɹ� 0x00-�޳�ʼ������
static				BYTE			g_RelayFlg;				//ˢ�¼̵���ʵ��״̬��־
static	__no_init	BYTE			g_RelaySta;				//�̵���ʵ��״̬��=ST_RELAY_OPEN:��բ/=ST_RELAY_CLOSE:��բ��
//static  __no_init	BYTE			g_RecordRelayErrFlag;	//��¼���ɿ���������־λ ���ɿ���һ������բֻ��¼һ�� 0x55-���Լ�¼���ɿ�������
static	__no_init	DWORD			g_RelayTmr;				//�̵���״̬��ʱ��
static				BOOL			g_RelayBlk;				//����������־
static				BYTE			g_RelayErrCounter;		//�̵��������������
static  __no_init   TRelayCmd       RelayCmd;
static				TRelayTaskFlag	RelayTaskFlag;
static				WORD			RelayOkStatus;
static				WORD			RelayErrStatus;
static	__no_init	BOOL			bRelayDetectFlag;//���ü̵�����բʱ�Ƿ�������̵���״̬��־ TRUE-δ���ĺ�բ FALSE-�Ѿ���⵽��բ �������ں�բ��������բ����˸
static  __no_init	e_Relay_Report	g_RelayReportflag;		//���������ϱ���־
static  __no_init   e_Relay_Report  RelayOperateNeedRepoted;//����բ�Ƿ���Ҫ�ϱ���־��0x00-�����ϱ�����բ��բ��0x55��Ҫ�ϱ���բ\0xAA��Ҫ�ϱ���բ��
//���ر���ݱ���״̬�Լ̵����Ĳ���
static const eRELAY_CMD TAB_RelayLocalCmd[5][5] =
{
	//����״̬           �������1             �������2              ����״̬              ͸֧״̬
	//eNoAlarm          eAlarm1              eAlarm2               eOvrZero             eOvrTick
	{eCMD_RelayOn_L,    eCMD_RelayOn_L,     (eCMD_AdvOff_L),     (eCMD_AdvOff_L),     (eCMD_AdvOff_L), }, //����״̬ 
	{eCMD_RelayOn_L,    eCMD_RelayOn_L,     (eCMD_AdvOff_L),     (eCMD_AdvOff_L),     (eCMD_AdvOff_L), }, //�������1
	{eCMD_AdvOff_L,     eCMD_AdvOff_L,      (eCMD_AdvOff_L),     (eCMD_AdvOff_L),     (eCMD_AdvOff_L), }, //�������2 
	{eCMD_AdvOff_L,     eCMD_AdvOff_L,      eCMD_AdvOff_L,       (eCMD_AdvOff_L),     (eCMD_AdvOff_L), }, //����״̬
	{eCMD_RelayOff_L,   eCMD_RelayOff_L,    eCMD_RelayOff_L,     eCMD_RelayOff_L,     eCMD_RelayOff_L, }, //͸֧״̬
};
//�̵���״̬ת������������Ϊ����������Ϊ��ǰ״̬������Ԫ��Ϊת����״̬
//����,Զ�̱�
static const eRELAY_ST TAB_RelayStatus[11][15] =
{
//    eST_RlyOffR        eST_AdvOff1R        eST_AdvOff2R        eST_RlyOff_L        eST_AdvOff_L        eST_AllowOn        eST_AllowOnKp        eST_RelayOn            eST_RelayOnKp        eST_WaitOffR          eST_RlyOffGdR        eST_AdvOff1GdR         eST_AdvOff2GdR         eST_RlyOffGd_L         eST_AdvOffGd_L    
//    Զ����բ            Զ��Ԥ��բ1           Զ��Ԥ��բ2          ������բ             ����Ԥ��բ            ��բ����            ��բ������         ��բ                ��բ����               Զ�̵ȴ���բ           Զ����բ����           Զ��Ԥ��բ1����         Զ��Ԥ��բ2����          ������բ�����           ����Ԥ��բ�����
    {eST_RlyOffR,       eST_RlyOffR,        eST_RlyOffR,        eST_RlyOffR,        eST_RlyOffR,        eST_RlyOffR,        eST_Error,          (eST_WaitOffR),       eST_Error,            (eST_WaitOffR),       (eST_WaitOffR),       (eST_WaitOffR),        (eST_WaitOffR),        (eST_WaitOffR),        (eST_WaitOffR),  },    //��բ����
    {eST_AllowOn,       eST_AllowOn,        eST_AllowOn,        eST_AllowOn,        eST_AllowOn,        eST_AllowOn,        eST_AllowOnKp,      eST_RelayOn,          eST_RelayOnKp,        (eST_RelayOn),        (eST_RelayOn),        eST_RelayOn,           eST_RelayOn,            eST_RlyOffGd_L,        eST_RelayOn,    },    //�����բ����
    {eST_RelayOn,       eST_RelayOn,        eST_RelayOn,        eST_RelayOn,        eST_RelayOn,        eST_RelayOn,        eST_RelayOnKp,      eST_RelayOn,          eST_RelayOnKp,        (eST_RelayOn),        (eST_RelayOn),        eST_RelayOn,           eST_RelayOn,            eST_RlyOffGd_L,        eST_RelayOn,    },    //ֱ�Ӻ�բ����
    {eST_Error,         (eST_AdvOff1R),     (eST_AdvOff1R),     eST_Error,          eST_Error,          eST_Error,          eST_Error,          (eST_AdvOff1GdR),     eST_Error,            eST_Error,            eST_Error,            (eST_AdvOff1GdR),      (eST_AdvOff1GdR),       eST_Error,             eST_Error,      },    //Ԥ��բ1����
    {eST_Error,         (eST_AdvOff2R),     (eST_AdvOff2R),     eST_Error,          eST_Error,          eST_Error,          eST_Error,          (eST_AdvOff2GdR),     eST_Error,            eST_Error,            eST_Error,            (eST_AdvOff2GdR),      (eST_AdvOff2GdR),       eST_Error,             eST_Error,      },    //Ԥ��բ2����
    {eST_AllowOnKp,     eST_AllowOnKp,      eST_AllowOnKp,      eST_AllowOnKp,      eST_AllowOnKp,      eST_AllowOnKp,      eST_AllowOnKp,      eST_RelayOnKp,        eST_RelayOnKp,        eST_RelayOnKp,        eST_RelayOnKp,        eST_RelayOnKp,         eST_RelayOnKp,          eST_RelayOnKp,         eST_RelayOnKp,  },    //��������
    {eST_RlyOffR,       eST_AdvOff1R,       eST_AdvOff2R,       eST_RlyOff_L,       eST_AdvOff_L,       eST_AllowOn,        eST_AllowOn,        eST_RelayOn,          (eST_RelayOn),        eST_WaitOffR,         eST_RlyOffGdR,        eST_AdvOff1GdR,        eST_AdvOff2GdR,         eST_RlyOffGd_L,        eST_AdvOffGd_L, },    //�����������
    {eST_RlyOffR,       eST_AdvOff_L,       eST_AdvOff_L,       eST_AdvOff_L,       eST_AdvOff_L,       eST_AdvOff_L,       eST_AllowOnKp,      (eST_AdvOffGd_L),     eST_RelayOnKp,        (eST_RlyOffGdR),      eST_RlyOffGdR,        eST_AdvOffGd_L,        eST_AdvOffGd_L,         eST_AdvOffGd_L,        eST_AdvOffGd_L, },    //����Ԥ��բ
    {eST_RlyOffR,       eST_RlyOff_L,       eST_RlyOff_L,       eST_RlyOff_L,       eST_RlyOff_L,       eST_RlyOff_L,       eST_AllowOnKp,      (eST_RlyOffGd_L),     eST_RelayOnKp,        (eST_RlyOffGdR),      eST_RlyOffGdR,        eST_RlyOffGd_L,        eST_RlyOffGd_L,         eST_RlyOffGd_L,        eST_RlyOffGd_L, },    //������բ
    {eST_RlyOffR,       eST_AdvOff1R,       eST_AdvOff2R,       eST_RelayOn,        eST_RelayOn,        eST_AllowOn,        eST_AllowOnKp,      eST_RelayOn,          eST_RelayOnKp,        eST_WaitOffR,         eST_RlyOffGdR,        eST_AdvOff1GdR,        eST_AdvOff2GdR,         eST_RelayOn,           eST_RelayOn,    },    //���غ�բ
    {eST_RlyOffR,       eST_RelayOn,        eST_AllowOn,        eST_RlyOff_L,       eST_AdvOff_L,       eST_AllowOn,        eST_AllowOnKp,      eST_RelayOn,          eST_RelayOnKp,        eST_RlyOffR,          eST_RlyOffR,          eST_RelayOn,           eST_RelayOn,            eST_RlyOff_L,          eST_RlyOff_L,   },    //�ϵ�
};
//��������ִ��״̬�� �����������״̬��
static const WORD TAB_RelayReturnStatus[7][15] =
{
//    eST_RlyOffR         eST_AdvOff1R         eST_AdvOff2R         eST_RlyOff_L         eST_AdvOff_L        eST_AllowOn           eST_AllowOnKp          eST_RelayOn          eST_RelayOnKp            eST_WaitOffR          eST_RlyOffGdR        eST_AdvOff1GdR        eST_AdvOff2GdR     eST_RlyOffGd_L     eST_AdvOffGd_L    
//    Զ����բ             Զ��Ԥ��բ1           Զ��Ԥ��բ2            ������բ              ����Ԥ��բ            ��բ����              ��բ������                ��բ                ��բ����             Զ�̵ȴ���բ            Զ����բ����           Զ��Ԥ��բ1����        Զ��Ԥ��բ2����      ������բ�����        ����Ԥ��բ�����
	{R_OK_DIRECT_OFF,     R_OK_DIRECT_OFF,     R_OK_DIRECT_OFF,     R_OK_DIRECT_OFF,     R_OK_DIRECT_OFF,     R_OK_DIRECT_OFF,     R_ERR_OFF_PROTECT,    (R_OK_DELAY_OFF),     R_ERR_OFF_PROTECT,      (R_OK_DELAY_OFF),     R_OK_OVER_I_OFF,     R_OK_OVER_I_OFF,     R_OK_OVER_I_OFF,    R_OK_OVER_I_OFF,    R_OK_OVER_I_OFF,    },//��բ����
	{R_OK_ALLOW_ON,       R_OK_ALLOW_ON,       R_OK_ALLOW_ON,       R_OK_ALLOW_ON,		 R_OK_ALLOW_ON,       R_OK_ALLOW_ON,       R_OK_ALLOW_ON,         R_OK_ALLOW_ON,       R_OK_ALLOW_ON,           R_OK_ALLOW_ON,       R_OK_ALLOW_ON,       R_OK_ALLOW_ON,       R_OK_ALLOW_ON,      R_OK_ALLOW_ON,      R_OK_ALLOW_ON,      },//�����բ����
	{R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,        R_OK_DIRECT_ON,      R_OK_DIRECT_ON,          R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,      R_OK_DIRECT_ON,     R_OK_DIRECT_ON,     R_OK_DIRECT_ON,     },//ֱ�Ӻ�բ����
	{R_ERR_AUTOON_OFF,    R_OK_OFF_AUTO_ON,    R_OK_OFF_AUTO_ON,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_PROTECT,  R_OK_OFF_AUTO_ON,    R_ERR_AUTOON_PROTECT,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_OK_OFF_AUTO_ON,    R_OK_OFF_AUTO_ON,   R_OK_OVER_I_OFF,    R_OK_OVER_I_OFF,    },//Ԥ��բ1����
	{R_ERR_AUTOON_OFF,    R_OK_OFF_AUTO_ON,    R_OK_OFF_AUTO_ON,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_PROTECT,  R_OK_OFF_AUTO_ON,    R_ERR_AUTOON_PROTECT,    R_ERR_AUTOON_OFF,    R_ERR_AUTOON_OFF,    R_OK_OFF_AUTO_ON,    R_OK_OFF_AUTO_ON,   R_OK_OVER_I_OFF,    R_OK_OVER_I_OFF,    },//Ԥ��բ2����
	{R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,          R_OK_PROTECT,        R_OK_PROTECT,            R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,        R_OK_PROTECT,       R_OK_PROTECT,       R_OK_PROTECT,       },//��������
	{R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,        R_OK_UNPROTECT,      R_OK_UNPROTECT,          R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,      R_OK_UNPROTECT,     R_OK_UNPROTECT,     R_OK_UNPROTECT      },//�����������
};

//���ü̵���������Ч��״̬����Ϊ����Ĭ��״̬
eRELAY_ST ST_OutSideRelay[] =
{
	eST_NULL,
	eST_RlyOffR,	//Զ����բ
	eST_AdvOff1R,	//Զ��Ԥ��բ1��ʱ�䵽ֱ�Ӻ�բ��
	eST_AdvOff1R,	//Զ��Ԥ��բ2��ʱ�䵽��բ����	���ñ�	ǿ��Ϊ Զ��Ԥ��բ1
	eST_RlyOff_L,	//������բ
	eST_AdvOff_L,	//����Ԥ��բ
	eST_RelayOn,	//��բ����			  ���ñ�	ǿ��Ϊ ��բ
	eST_RelayOnKp,	//��բ������		  ���ñ�	ǿ��Ϊ ��բ����
	eST_RelayOn,	//��բ
	eST_RelayOnKp,	//��բ����
	eST_WaitOffR,	//Զ�̵ȴ���բ
	eST_WaitOffR,	//Զ����բ����		  ���ñ�	ǿ��Ϊ Զ�̵ȴ���բ
	eST_AdvOff1R,	//Զ��Ԥ��բ1����     ���ñ�	ǿ��Ϊ Զ��Ԥ��բ1
	eST_AdvOff1R,	//Զ��Ԥ��բ2����     ���ñ�	ǿ��Ϊ Զ��Ԥ��բ1
	eST_RlyOff_L,	//������բ�����      ���ñ�    ǿ��Ϊ ������բ
	eST_AdvOff_L,	//����Ԥ��բ�����    ���ñ�    ǿ��Ϊ ����Ԥ��բ
	eST_Error,		//����״̬
};

//�������״̬ [0]-֮ǰ״̬  [1]-֮��״̬
eRELAY_ST ST_LocalRelay[][2] =
{
	{ eST_RlyOff_L,   eST_RlyOffR }, //������բ//Զ����բ
	{ eST_AdvOff_L,   eST_RlyOffR }, //����Ԥ��բ//Զ����բ
	{ eST_RlyOffGd_L, eST_RelayOn }, //������բ�����//��բ
	{ eST_AdvOffGd_L, eST_RelayOn }, //����Ԥ��բ�����//��բ
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static WORD ProcRelayCmdR(eRELAY_CMD Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime);
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
static BYTE GetRelayCMD(eRELAY_ST Type)
{
	eRELAY_ST CloseRelayNo;
	
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


//-----------------------------------------------
//��������: �жϼ̵�������״̬�Ϸ���
//
//����:		��
//						
//����ֵ:	��
//		   
//��ע:�������ü̵�����Щ��Ч��״̬����Ϊ��Ч��״̬
//-----------------------------------------------
static void JudgeRelayCmd(void)
{
	if(RelayTypeConst == RELAY_OUTSIDE)
	{
		if(RelayCmd.Status != ST_OutSideRelay[RelayCmd.Status])
		{
			ASSERT(0,0);
			RelayCmd.Status = ST_OutSideRelay[RelayCmd.Status];
			RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
		}
	}
}

//---------------------------------------------------------------
//��������: �жϱ���״̬����Ч��
//
//����: 
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
#if(PREPAY_MODE == PREPAY_REMOTE)
static void JudgeLocalRelayStatus( void )
{
	BYTE Flag, i, Len;
	
	Flag = 0;
	//�Ǳ���ģʽ �������������Ҫת��	
	Len = sizeof(ST_LocalRelay) / sizeof(ST_LocalRelay[0]);
	for(i = 0; i < Len; i++)
	{
		if(RelayCmd.Status == ST_LocalRelay[i][0])
		{
			Flag = 1;
			RelayCmd.Status = ST_LocalRelay[i][1];
			break;
		}
	}
	if(Flag == 1)
	{
		RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
	}		
}
#endif

//-----------------------------------------------
//��������: ���̵����������������Ƚϣ��ж��Ƿ����������̵���
//
//����:
//			Cmd[in]     �̵���������
//			NewSta[in]	�̵�����״̬
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
static void JudgeRelayCmdChange( eRELAY_CMD Cmd, eRELAY_ST NewSta )
{		
	TRelayDetectFlag RelayDetectFlag;
	
	if((NewSta != RelayCmd.Status)||(Cmd != RelayCmd.Command))
	{
		if(NewSta != RelayCmd.Status)
		{
			//�̵����������ת ��ԭ�����բ->��������բ��&&��ԭ������բ->�������բ��
			if( ((GetRelayCMD(RelayCmd.Status)==CMD_RELAY_CLOSE)&&(GetRelayCMD(NewSta)==CMD_RELAY_OPEN))	
			|| ((GetRelayCMD(RelayCmd.Status)==CMD_RELAY_OPEN)&&(GetRelayCMD(NewSta)==CMD_RELAY_CLOSE)) )
			{
				if(GetRelayCMD(NewSta) == CMD_RELAY_OPEN)			
				{
					//aa����Ҫ����4�βų���1111��������4������жϳ��Ǽ̵�����բ״̬��
					//	��Ҫ����3�γ���0000��������3������жϳ��Ǽ̵�����բ״̬��
					g_RelayFlg = ST_RELAY_OPEN;
					RelayOperateNeedRepoted = eCMD_RelayOff_Report;
					api_SavePrgOperationRecord( ePRG_OPEN_RELAY_NO );
				}
				else
				{
					//55����Ҫ����4�βų���0000��������4������жϳ��Ǽ̵�����բ״̬��
					//	��Ҫ����3�γ���1111��������3������жϳ��Ǽ̵�����բ״̬��
					g_RelayFlg = ST_RELAY_CLOSE;
					RelayOperateNeedRepoted = eCMD_RelayOn_Report;
					api_SavePrgOperationRecord( ePRG_CLOSE_RELAY_NO );
					#if( MAX_PHASE == 3)
					if(RelayTypeConst == RELAY_OUTSIDE)
					{
						if(bRelayDetectFlag != TRUE)//
						{
							bRelayDetectFlag = TRUE;
							RelayDetectFlag.Flag = bRelayDetectFlag;
							api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
						}
					}					
					else
					#endif
					{
						//g_RecordRelayErrFlag = 0x55; //���ñ��բ������� ����ֱ�Ӽ�¼���ɿ�������
					}
				}
				RelayTaskFlag.OpFlag = eCMD_OPERATE;
			}
		}
		
		//������¾�Ҫ��¼
		RelayCmd.Status = NewSta;
		if(Cmd != eCMD_PowerOn) //�ϵ粻��¼����
		{
			RelayCmd.Command = Cmd;
		}
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	}
	if( Cmd == eCMD_PowerOn ) //�ϵ�ǿ������һ�μ̵���
	{
		if( RelayTaskFlag.OpFlag != eCMD_OPERATE )//������δִ�� �������ϵ���� ���򲻼�¼�¼���¼
		{
			RelayTaskFlag.OpFlag = ePOWERON_OPERATE;
		}
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
	else if(Operate == CLRDT)//����--������´μ̵���������Ҫ����
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
	if( RelayCmd.CRC32 != lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD)) )
	{
		wResult = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
		if( wResult == FALSE )
		{
			RelayCmd.Status = eST_RelayOn;
			RelayCmd.Alarm = FALSE;
			#if( SEL_DLT645_2007 == YES )
			RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
			#endif
			RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
			RelayCmd.CtrlMode = RelayCtrlMode_Def;
			RelayCmd.dwRelayProtectI = RelayProtectI_Def;
		}				
	}	
	
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
	
	if(RelayTypeConst == RELAY_NO)//!!!!!
	{
		if(RelayCmd.Status != eST_RelayOn)
		{
			RelayCmd.Status = eST_RelayOn;
			RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
		}
	}
	
	//�ж����ü̵���״̬����Ч��
	JudgeRelayCmd();
	
	//�鿴�̵���״̬�Ƿ�Ϊ�Ƿ�ֵ,����Ϊ�Ƿ�ֵ���������õ���֮ǰ��״̬�������ж�ˢ�¡�
	if( 	((g_RelaySta!=ST_RELAY_CLOSE)&&(g_RelaySta!=ST_RELAY_OPEN)) 
		|| 	(api_GetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR)==TRUE) )
	{
		//��Ϊ�Ƿ�ֵ���ϵ�ʱ���̵������״̬��Ϊ����״̬
		if( GetRelayCMD(RelayCmd.Status) == CMD_RELAY_OPEN )	
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
	g_RelayFlg = ST_RELAY_CLOSE;
												
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
	BYTE 	i,j,k,Sec,RelayErrCheckFlag;;
	WORD	wResult;
	BOOL 	RelayProtectIFlag;
	eRELAY_ST 	NewSta;
	TRelayPara	RelayPara;
	TRelayDetectFlag RelayDetectFlag;
	
	FunctionConst(RELAY_TASK);
	
	RelayErrCheckFlag = FALSE;
	
	//ÿ���ӵĵ�33sЧ��̵���״̬
	//��ȡ��	
	api_GetRtcDateTime(DATETIME_ss,&Sec);
	if( Sec == (eTASK_RELAY_ID*3+3) )
	{	
		if( RelayCmd.CRC32 != lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD)) )
		{
			wResult = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
			if( wResult == FALSE )
			{
				RelayCmd.Status = eST_RelayOn;
				RelayCmd.Alarm = FALSE;
				#if( SEL_DLT645_2007 == YES )
				RelayCmd.wRelayWaitOffTime = wRelayWaitOffTime_Def;
				#endif
				RelayCmd.wOverIProtectTime = OverIProtectTime_Def;
				RelayCmd.CtrlMode = RelayCtrlMode_Def;
				RelayCmd.dwRelayProtectI = RelayProtectI_Def;
			}
		}	
		
		if(RelayTypeConst == RELAY_NO)
		{			
			if( RelayCmd.Status != eST_RelayOn )
			{
				RelayCmd.Status = eST_RelayOn;
				RelayCmd.CRC32 = lib_CheckCRC32((BYTE*)&RelayCmd,sizeof(TRelayCmd)-sizeof(DWORD));
			}
		}
	}
	
	if(RelayTypeConst == RELAY_NO)
	{
		return;
	}

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
			NewSta = RelayCmd.Status;
			if(RelayCmd.Status == eST_AllowOn)
			{
				NewSta = eST_RelayOn;
			}
			else if(RelayCmd.Status == eST_AllowOnKp)
			{
				NewSta = eST_RelayOnKp;
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if((RelayCmd.Status == eST_AdvOff_L) || (RelayCmd.Status == eST_AdvOffGd_L))
			{
				NewSta = eST_RelayOn;
			}
			#endif
			JudgeRelayCmdChange(RelayCmd.Command, NewSta);			
		}
	}
	else
	{
		RelayTaskFlag.KeyDown3sTimer = 0;
	}
		
	//����������־,���������жϼ̵�����־		
	if(g_RelayBlk == TRUE)											
	{
		if( GetRelayCMD(RelayCmd.Status) == CMD_RELAY_OPEN )
		{
			g_RelayFlg = ST_RELAY_OPEN;
		}
		else
		{
			g_RelayFlg = ST_RELAY_CLOSE;
		}
		if( RelayTaskFlag.OpFlag != eCMD_OPERATE )//������δִ�� �������ϵ���� ���򲻼�¼�¼���¼
		{
			RelayTaskFlag.OpFlag = ePOWERON_OPERATE;
		}
	}
			
	//���̵���ʵ��״̬ ֻ�е�ѹ����0.75Un���ܽ��м̵���״̬��� ��Ȼ��բ����µ������������բ����һ�µ�����
	if( GetRemoteValueRelayStatus( REMOTE_U ) == TRUE )
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
			if( GetRelayCMD(RelayCmd.Status) == CMD_RELAY_OPEN )
			{
				j = 0;				
			}
			else
			{
				if( bRelayDetectFlag == TRUE ) //֮ǰδ��⵽��բ
				{
					j = 0;//��բ
				}
				else
				{
					j = 70;//��բ
				}
			}
		}
		#endif

		//0x20--��Ǳ��״̬	1--Ǳ��		0--��
		if( (api_GetSampleStatus(0x40)==0) || (j>30) )
		{
			g_RelayFlg |= 0x01;
		}
		if( (g_RelayFlg&0x0F) == 0x0F )			//3�� ����̨����Լ̵�����բ״̬ˢ���� ���²��Բ���
		{
			g_RelaySta = ST_RELAY_CLOSE;		//ʵ�ʺ�բ
			RelayErrCheckFlag = TRUE;
		}
		
		else if( (g_RelayFlg&0x0F) == 0x00 )	//3��
		{			
			if( RelayTypeConst == RELAY_INSIDE )
			{
				g_RelaySta = ST_RELAY_OPEN;		//ʵ����բ
				RelayErrCheckFlag = TRUE;
			}
			else
			{
				if( GetRelayCMD(RelayCmd.Status) == CMD_RELAY_OPEN )	//������բ
				{
					g_RelaySta = ST_RELAY_OPEN;		//ʵ����բ
					RelayErrCheckFlag = TRUE;
				}
				else
				{
					#if( SEL_EVENT_RELAY_ERR == YES )
					if( (api_DealRelayErrFlag(READ,0x0000) & 0x00AA) == 0x00AA )
					{
						api_DealRelayErrFlag(CLRDT,0x0000);//�帺�ɿ�����
					}
					#endif
				}
			}					
		}
		else
		{
		}
				
		if(RelayTypeConst == RELAY_OUTSIDE)
		{
			if(g_RelaySta == ST_RELAY_CLOSE)
			{
				if(GetRelayCMD( RelayCmd.Status ) == CMD_RELAY_CLOSE) //�����բ
				{
					if(bRelayDetectFlag != FALSE)
					{
						bRelayDetectFlag = FALSE;
						RelayDetectFlag.Flag = bRelayDetectFlag;
						api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayDetectFlag),sizeof(TRelayDetectFlag),(BYTE*)&RelayDetectFlag);
					} 
				}
			}
		}
	}
	
	if(RelayErrCheckFlag != FALSE) //ˢ�³�ʵ��״̬����м̵����𶯼��
	{
		//���ɿ����󶯼��� ��60s��120s��180�����������̵������Σ��������ٴ���
		if( GetRelayCMD(RelayCmd.Status) != g_RelaySta )
		{
			if(g_RelayErrCounter < 230)
			{
				g_RelayErrCounter++;
			}
			
			if( (g_RelayErrCounter%60) == 0 )
			{
				if( RelayTaskFlag.OpFlag != eCMD_OPERATE )//������δִ�� �������ϵ���� ���򲻼�¼�¼���¼
				{
					RelayTaskFlag.OpFlag = ePOWERON_OPERATE;
				}
			}
			
			#if( SEL_EVENT_RELAY_ERR == YES  )
			//������β����̵���֮��δ��¼\�ϱ������ɿ����󶯣�����Ҫ��¼���ϱ���ͬʱ���Ѽ�¼�¼���־
			if((api_DealRelayErrFlag(READ,0x0000) == 0x0000)&&(api_DealEventStatus( eGET_EVENT_STATUS, eSUB_EVENT_RELAY_ERR )== TRUE ))
			{
				if(GetRelayCMD( RelayCmd.Status ) == CMD_RELAY_OPEN) //��բ����ĸ��ɿ�����--���
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
			if((g_RelayReportflag == eCMD_RelayOn_Report)&&(g_RelaySta == ST_RELAY_CLOSE))
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//�̵����쳣����բ���𣬺�բ�����״̬��־
				if(api_DealRelayErrFlag(READ,0x0000) & 0x0A00)
				{
					api_DealRelayErrFlag(CLRDT,0x0000);
				}
				#endif
		
				api_SetFollowReportStatus(eSTATUS_CloseRelaySucceed);
                g_RelayReportflag = eCMD_Relay_NotReport;
            }
			else if((g_RelayReportflag == eCMD_RelayOff_Report)&&(g_RelaySta == ST_RELAY_OPEN))
            {
				#if( SEL_EVENT_RELAY_ERR == YES  )
				//�̵����쳣�ɺ�բ������բ�����״̬��־
				if(api_DealRelayErrFlag(READ,0x0000) & 0xA000)
				{
					api_DealRelayErrFlag(CLRDT,0x0000);
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
		NewSta = RelayCmd.Status;	
		//��ǰ����
		RelayProtectIFlag = GetRemoteValueRelayStatus(REMOTE_I);
		
		switch (RelayCmd.Status)
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
		case eST_AdvOff1R://Ԥ��բ1״̬ 
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					NewSta = eST_RelayOn;
				}
			}
			break;
		case eST_AdvOff2R://Ԥ��բ2״̬
			if( g_RelayTmr )
			{
				g_RelayTmr--;
				if( g_RelayTmr == 0 )
				{
					NewSta = eST_AllowOn;
				}
			}
			break;
		case eST_RlyOffGdR://Զ����բ���������״̬
		case eST_RlyOffGd_L://������բ���������״̬
		case eST_AdvOffGd_L://����Ԥ��բ���������״̬
		case eST_AdvOff1GdR://Ԥ��բ1���������״̬
		case eST_AdvOff2GdR://Ԥ��բ2���������״̬
			if( g_RelayTmr )
			{
				g_RelayTmr--;
			}
			if( (g_RelayTmr==0) || (RelayProtectIFlag==FALSE) )//24Сʱ����ʱ�䵽��С����բ��������				
			{
				g_RelayTmr = 0;
				
				switch(RelayCmd.Status)
				{
					case eST_RlyOffGdR:
						NewSta = eST_RlyOffR;
						break;
					case eST_RlyOffGd_L:
						NewSta = eST_RlyOff_L;
						break;
					case eST_AdvOffGd_L:
						NewSta = eST_AdvOff_L;
						break;
					case eST_AdvOff1GdR:
						NewSta = eST_AdvOff1R;
						break;
					case eST_AdvOff2GdR:
						NewSta = eST_AdvOff2R;
						break;
					default:
						break;
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
		
		JudgeRelayCmdChange(RelayCmd.Command, NewSta);
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
		if( RelayTaskFlag.OpFlag != eCMD_OPERATE )//������δִ�� �������ϵ���� ���򲻼�¼�¼���¼
		{
			RelayTaskFlag.OpFlag = ePOWERON_OPERATE;
		}
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
	
	if(RelayTypeConst == RELAY_NO)
	{
		return;
	}

	//Сʱ����
	if( api_GetTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR) == TRUE )
	{
		Task_Hour_Relay();
		api_ClrTaskFlag(eTASK_RELAY_ID,eFLAG_HOUR);
	}
		
	//�յ��̵���������Ϣ
	if(RelayTaskFlag.OpFlag != eNO_OPERATE)
	{
		RelayVolFlag = GetRemoteValueRelayStatus(REMOTE_U);
		
		//��ѹ����65%Un)
		if( (RelayVolFlag==TRUE) || (RelayTypeConst == RELAY_OUTSIDE) )						
		{
			if(GetRelayCMD(RelayCmd.Status) == CMD_RELAY_OPEN)	
			{
				Drv_OpenRelay();
				g_RelayFlg = ST_RELAY_OPEN; //10101010
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
				g_RelayFlg = ST_RELAY_CLOSE; //01010101
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
		
		RelayTaskFlag.OpFlag = eNO_OPERATE;	
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
	g_RelayErrCounter = 0;		//������բ��¼���󶯼�¼���ϱ�
	g_RelayReportflag = eCMD_Relay_NotReport;		//�ϱ���־����
	RelayOperateNeedRepoted = eCMD_Relay_NotReport;  //��Ҫ�ϱ���־����
	
	//���ü̵�����բʱ�Ƿ�������̵���״̬��־
	bRelayDetectFlag = FALSE;			//���ټ��̵���״̬
	g_RelayFlg 		 = 0x0F;			//����־����բ����ֹ3��״̬���
	g_RelaySta 		 = ST_RELAY_CLOSE;	//ʵ��״̬����բ
	#if( SEL_EVENT_RELAY_ERR == YES  )
	api_ClrSysStatus(eSYS_STATUS_RELAY_ERR);
	api_DealRelayErrFlag(CLRDT,0x0000);
	#endif
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
	
	//�̵�������״̬
	if( RelayKeepPowerFlag == TRUE )//���ñ���
	{
		RelayCmd.Status = eST_RelayOnKp;
	}
	else
	{
		RelayCmd.Status = eST_RelayOn;
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
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);
	
	//�ݴ���բ�Իָ�ʱ�� ��λ����	
	RelayPara.dwOffWarnTime = 300;	
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
	
	RelayTaskFlag.OpFlag = ePOWERON_OPERATE;
	RelayTaskFlag.KeyDown3sTimer = 0;
	RelayOperateNeedRepoted = eCMD_Relay_NotReport;          //��ʼ������Ҫ�ϱ���ʶ

	g_RelayBlk = FALSE;				//������־:������	
	g_RelayTmr = 0;
	g_RelayErrCounter = 0;
	#if( SEL_EVENT_RELAY_ERR == YES  )
	api_DealRelayErrFlag(CLRDT,0x0000);
	#endif
}


//-----------------------------------------------
//��������: Զ��������ú���
//
//����:		Cmd[in]	0��բ 1�����բ 2ֱ�Ӻ�բ 3Ԥ��բ1(ʱ�䵽ֱ�Ӻ�բ) 4Ԥ��բ2(ʱ�䵽��բ����)
//					5���� 6������ 7�ϵ� 8���� 9�������	
//			RelayWaitOffTime[in] �澯��ʱ unsigned����λ�����ӣ����㣺0��,Ϊ����645���˴�ΪWORD - ��բ��ʱ
//			RelayOffWarnTime[in] �޵���ʱ long-unsigned����λ�����ӣ����㣺0��ֵΪ0��ʾ�����޵磩 - ��բ�Իָ���ʱ			
//����ֵ:	���ع�ԼҪ���Ӧ��״̬�֣���bit15Ϊ1ʱ���������״̬��
//		   
//��ע:
//-----------------------------------------------
static WORD ProcRelayCmdR(eRELAY_CMD Cmd, WORD RelayWaitOffTime, WORD RelayOffWarnTime)
{
	BYTE    i, Len;
	WORD	wReturnStatus;
	BOOL 	RelayProtectIFlag;
	eRELAY_ST	NewSta;
	TRelayPara	RelayPara;
	eLOCAL_STATUS LocalStatus;
	
	wReturnStatus = R_OK_WARN;
	
	if(Cmd >= eCMD_RemoteNum)
	{	
		//��Ч����
		return R_ERR_PASSWORD;
	}
		
	//���������
	if( (Cmd==eCMD_InAlarm) || (Cmd==eCMD_OutAlarm) )		//�����������������
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
	else if(RelayTypeConst == RELAY_OUTSIDE) //��բ�����Ԥ��բ���� 
	{
		if(Cmd == eCMD_IndirOn)
		{
			Cmd = eCMD_DirectOn;
		}
		else if(Cmd == eCMD_AdvOff2)
		{
			Cmd = eCMD_AdvOff1;
		}
	}

	//�ж����ü̵���״̬����Ч��
	JudgeRelayCmd();
	
	#if(PREPAY_MODE == PREPAY_REMOTE)	
	JudgeLocalRelayStatus();
	#endif
	
	//��ȡ�̵���ת��״̬
	NewSta = TAB_RelayStatus[Cmd][RelayCmd.Status-1];

	//���Զ�̷���״̬
	if(Cmd < (sizeof(TAB_RelayReturnStatus) / sizeof(TAB_RelayReturnStatus[0])) )
	{
		wReturnStatus = TAB_RelayReturnStatus[Cmd][RelayCmd.Status-1];		
	}

    //�������״̬���ش����ڴ˴�ֱ�ӷ���
    if( (NewSta == eST_NULL) || (NewSta >= eST_Error) )
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
		
		RelayPara.dwOffWarnTime = (DWORD)RelayOffWarnTime*60; 
		
		//������״̬ ����״̬Ҳ��Ҫ��һ���ж�
		if( (NewSta == eST_AdvOff1GdR) || (NewSta == eST_AdvOff2GdR) )
		{
			if( RelayTypeConst == RELAY_INSIDE )
			{
				g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;
				
				if((RelayProtectIFlag == TRUE)&&(g_RelayTmr != 0))
				{
					//�������ʱ
					wReturnStatus = R_OK_OVER_I_OFF;
				}
				else
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
			else if( RelayTypeConst == RELAY_OUTSIDE )
			{
				NewSta = eST_AdvOff1R;
				g_RelayTmr = RelayPara.dwOffWarnTime;
			}
		}
		else
		{
			g_RelayTmr = RelayPara.dwOffWarnTime;
		}
		api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayPara),sizeof(TRelayPara),(BYTE*)&RelayPara);
	}
	else if(Cmd == eCMD_InKeep)
	{
		if((RelayTypeConst == RELAY_OUTSIDE)&&(NewSta == eST_AllowOnKp))
		{
			NewSta = eST_RelayOnKp;
		}
	}
#if( PREPAY_MODE == PREPAY_LOCAL )
	else if((Cmd == eCMD_AdvOff_L)||(Cmd == eCMD_RelayOff_L))
	{
		//������բ��Ԥ��բ��Ҫ���ݵ��������ж��Ƿ������բ
		if((NewSta == eST_RlyOffGd_L)||(NewSta == eST_AdvOffGd_L)||(NewSta == eST_RlyOffGdR))
		{
			if((RelayTypeConst == RELAY_INSIDE)&&(RelayProtectIFlag == TRUE))
			{
				g_RelayTmr = (DWORD)RelayCmd.wOverIProtectTime*60;	
				if (g_RelayTmr == 0)
				{
					//��բ״̬
					if(NewSta == eST_RlyOffGdR)//Զ����բ��ʱֱ��ת��Ϊֱ����բ
					{
						NewSta = eST_RlyOffR;
					}
					else
					{
						NewSta = (eRELAY_ST)(eST_RlyOff_L + (NewSta - eST_RlyOffGd_L));
					}
				}
			}
			else
			{
				if(NewSta == eST_RlyOffGdR)//Զ����բ��ʱֱ��ת��Ϊֱ����բ
				{
					NewSta = eST_RlyOffR;
				}
				else
				{
					NewSta = (eRELAY_ST)(eST_RlyOff_L + (NewSta - eST_RlyOffGd_L));
				}
			}
		}
	}
	else if((Cmd == eCMD_IndirOn)||(Cmd == eCMD_DirectOn))
	{
		ASSERT( FALSE, 0 );
		//��բ���ߺ�բ��������� ״̬���ɱ����ж� ����������� �������е�����ط� ��ֹ����״̬δˢ��
		if((NewSta == eST_AllowOn)||(NewSta == eST_RelayOn))
		{
			LocalStatus = api_GetLocalStatus();
			
			if(LocalStatus == eOvrTick)
			{
				NewSta = eST_RlyOff_L;
			}
		}
	}
	else if(Cmd == eCMD_OutKeep)
	{
		if((NewSta == eST_AllowOn)||(NewSta == eST_RelayOn))
		{
			LocalStatus = api_GetLocalStatus();
			
			//����Ԥ���󷢱����Ԥ��ǰ������ ���������״̬�ǲ�һ���� �����Ԥ��ǰӦ����բ �����բ
			if((LocalStatus == eAlarm2)||(LocalStatus == eOvrZero))
			{
				//�����һ�����Ǳ���Ԥ��բ ˵�����ȱ�������ı�2�͹��� ��������Ӧ����բ
				if(RelayCmd.Command == eCMD_AdvOff_L)
				{
					NewSta = eST_AdvOff_L;
				}
			}
			if(LocalStatus == eOvrTick)
			{
				NewSta = eST_RlyOff_L;
			}
		}
	}
#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
		
	//״̬��ת��
	JudgeRelayCmdChange( Cmd, NewSta );
		
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
	eRELAY_CMD tCmd;
	
	RelayOkStatus = 0;
	RelayErrStatus = 0;
	tCmd = (eRELAY_CMD)Cmd;

	//�޵���ʱ��Ч ����բ��ʱʧЧ
	if( RelayOffWarnTime )
	{
		RelayWaitOffTime = 0;
	}
	tw = ProcRelayCmdR(tCmd, RelayWaitOffTime, RelayOffWarnTime);
	
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

//---------------------------------------------------------------
//��������: ���ؼ̵�������
//
//����: 	
//			Type[in] 			
// 			  eReturnMoneyMode  - �۷ѻ����˷Ѳ���
// 			  eChargeMoneyMode  - ��ֵ���� 
// 			  eSetParaMode      - ���Ĳ�������
// 			  eInitMoneyMode     - Ԥ�ò���
//          LocalStatus[in]
// 			  eNoAlarm		   -����״̬
//			  eAlarm1		   -���ڱ������1״̬
//			  eAlarm2      	   -���ڱ������2״̬
//			  eOvrZero    	   -���ڹ���״̬
//			  eOvrTick    	   -����͸֧״̬       
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
void api_Set_LocalRelayCmd( eUpdataMoneyMode Type, eLOCAL_STATUS CurLocalStatus )
{
	eRELAY_CMD NewCMD;
	
	if((Type > eInitMoneyMode)||(CurLocalStatus > eOvrTick))
	{	
		//��Ч����
		return;
	}
	
	NewCMD = TAB_RelayLocalCmd[CurLocalStatus][RelayCmd.LastLocalStatus];
	
	//��ֵ����
	if( Type == eChargeMoneyMode )
	{
		if( (NewCMD == eCMD_AdvOff_L) || (NewCMD == eCMD_RelayOn_L) )
		{
			//��ֵ����Ҫ�ж������Ƿ��Ǻ�բ״̬ ����Ǻ�բ״̬ �����Ϊ���غ�բ ����Ϊ��բ����
			//����������Զ�̳�ֵ���Ǳ��س�ֵ ���Ǻ�բ
			if( (GetRelayCMD( RelayCmd.Status ) == CMD_RELAY_CLOSE) || (RelayTypeConst == RELAY_OUTSIDE))
			{
				NewCMD = eCMD_RelayOn_L;
			}
			else
			{
				NewCMD = eCMD_AdvOff_L;
			}
		}
	}
	//Ԥ�ò���
	else if( Type == eInitMoneyMode )
	{
		//Ԥ�ò���������Ǻ�բ���� ֱ����Ϊ��բ
		if( NewCMD == eCMD_AdvOff_L )
		{
			NewCMD = eCMD_RelayOn_L;
		}
	}
	//���Ĳ�������
	else if(Type == eSetParaMode)
	{
		//״̬�����ƶ� ˵���μӸ�С 
		//���֮ǰ�̵���Ϊ��բ ���ñ���Ϊ����Ԥ��բ ���ñ���Ϊֱ�Ӻ�բ 
		//���֮ǰ�̵���Ϊ�Ǻ�բ ����Ϊ���غ�բ
		if(CurLocalStatus < RelayCmd.LastLocalStatus)
		{
			//����Ǳ���Ԥ��բ ���̵���״̬ ����Ǻ�բ״̬��Ϊ���غ�բ
			if(NewCMD == eCMD_AdvOff_L)
			{
				if( GetRelayCMD( RelayCmd.Status ) == CMD_RELAY_CLOSE )
				{
					NewCMD = eCMD_RelayOn_L;
				}
				else if(RelayTypeConst == RELAY_OUTSIDE)//���ñ�ֱ�Ӻ�բ
				{
					NewCMD = eCMD_RelayOn_L;
				}
			}
			else
			{
				//���غ�բ��������
			}
		}
		//״̬���� ���� �������κβ���
		else if(CurLocalStatus == RelayCmd.LastLocalStatus)
		{
			return;
		}
		else //CurLocalStatus > RelayCmd.LastLocalStatus
		{
			//״̬�����ƶ� ˵���μӸĴ� ���ı�����״̬
		}
	}
	//�����ǰ�����״̬��֮ǰһ�� ��ִ��
	if( (NewCMD == RelayCmd.Command) && (CurLocalStatus == RelayCmd.LastLocalStatus) )
	{
		return;
	}
	
	RelayCmd.LastLocalStatus = CurLocalStatus;
	//״̬�����仯Ҫ��EEPROM ����У�鲻�� �ᱻ�ָ� ֻ��У��Ҳ���� �������Ҳ�����
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(RelaySafeRom.RelayCmd),sizeof(TRelayCmd),(BYTE*)&RelayCmd);

	//�����̵���
	ProcRelayCmdR( NewCMD, 0, 0 );
}

//---------------------------------------------------------------
//��������: Զ�̹����բ����
//
//����: 	OldMoney Զ�̹���ǰ�Ľ��
//                   
//����ֵ:   ��
//
//��ע:    ��բ�Իָ���ʱ�����У�����ɹ�Ҫֱ�Ӻ�բ���˺���רΪ����������
//---------------------------------------------------------------
void api_RemoteChargeCloseRelay( long OldMoney )
{
	eRELAY_ST NewSta;
	TFourByteUnion  NowMoney;
    
    NewSta = RelayCmd.Status;

	api_ReadPrePayPara(eRemainMoney,(BYTE*)NowMoney.b);
	if( NowMoney.l>OldMoney )
	{
		if(RelayCmd.Status == eST_AdvOff1R)
		{
			NewSta = eST_RelayOn;
		}
		else if(RelayCmd.Status == eST_AdvOff2R)
		{
			NewSta = eST_RelayOn;
		}
	}

	//״̬��ת��
	JudgeRelayCmdChange( RelayCmd.Command, NewSta );

}

//---------------------------------------------------------------
//��������: �忨��բ����
//
//����: 	OldMoney �忨ʱ���õ����忨ǰʣ����
//                   
//����ֵ:   ��
//
//��ע:   	��բ�Իָ���ʱ�����У�����ɹ�Ҫֱ�Ӻ�բ��
//			api_ReadPrePayPara(eRemainMoney,(BYTE*)NowMoney.b);֮��Ϊ���˴����¼�����
//---------------------------------------------------------------
void api_CardCloseRelay( long OldMoney )
{
	eRELAY_ST NewSta;
	TFourByteUnion  NowMoney;
	
	NewSta = RelayCmd.Status;
	if(RelayCmd.Status == eST_AllowOn)
	{
		NewSta = eST_RelayOn;
	}
	else if(RelayCmd.Status == eST_AllowOnKp)
	{
		NewSta = eST_RelayOnKp;
	}
	else if(RelayCmd.Status == eST_AdvOff_L)
	{
		NewSta = eST_RelayOn;
	}
	
	api_ReadPrePayPara(eRemainMoney,(BYTE*)NowMoney.b);
	if( NowMoney.l>OldMoney )
	{
		if(RelayCmd.Status == eST_AdvOff1R)
		{
			NewSta = eST_RelayOn;
		}
		else if(RelayCmd.Status == eST_AdvOff2R)
		{
			NewSta = eST_RelayOn;
		}
	}

	//״̬��ת��
	JudgeRelayCmdChange( RelayCmd.Command, NewSta );

}
//---------------------------------------------------------------
//��������: �̵�����ʼ������
//
//����:
//
//����ֵ:
//
//��ע: ���ڵ�����㡢����Ԥ�ÿ���ģʽ�л�����
//---------------------------------------------------------------
void api_InitRelay( void )
{
	//g_RelayInitFlag = 0x55;
	
	//�̵����ı���ģʽ��Ϊ���� ����ĺ������EEPROM
	RelayCmd.LastLocalStatus = eNoAlarm;
	
	//�����������
	ProcRelayCmdR( eCMD_OutAlarm, 0, 0 );
	
	//Զ��������Ϊ��բ
	ProcRelayCmdR( eCMD_DirectOn, 0, 0 );
}
//-----------------------------------------------
//��������: ��ü̵���״̬
//
//����:		Type[in]	
//				0--�̵�������״̬	���� 0--�����բ	1--������բ  2--������բ��ʱ���ߴ������ʱ
//				1--�̵���ʵ��״̬	���� 0--��բ״̬	1--��բ״̬  2--��բ����״̬
//				2--����״̬			���� 0-FALSE--�Ǳ���		1-TRUE--����״̬
//				3--����״̬			���� 0-FALSE--��Զ�̱���   1-TRUE--Զ�̱���
//				4--��բ�����Զ�̺�բδ���ɹ�		���� 0-FALSE--�Ǵ�״̬ 1-TRUE--��״̬ ��բ����˸
// 				5--�̵���Զ������״̬  ���� 0-FALSE--�����բ    1-TRUE--������բ -��״̬��3��
// 				6--�Ƿ������ƻ�·���� ���� 0-FALSE--�Ǵ�״̬ 1-TRUE--��״̬
// 				7--�̵���Զ������״̬ ���� 0-FALSE--�����բ	1-TRUE--������բ
//����ֵ:	0/1/2 WORD��
//��ע:
//-----------------------------------------------
WORD api_GetRelayStatus( BYTE Type )
{
	WORD wStatus;
	TRelayDetectFlag RelayDetectFlag;
	wStatus = 0;

	switch(Type)
	{
		case 0x00:
			//�̵�������״̬	0-FALSE--�����բ	1-TRUE--������բ  2--��բ��ʱ�ʹ������ʱ
			if((RelayCmd.Status == eST_RlyOffR)
			   ||(RelayCmd.Status == eST_AdvOff1R)
			   ||(RelayCmd.Status == eST_AdvOff2R)
			   ||(RelayCmd.Status == eST_RlyOff_L)
			   ||(RelayCmd.Status == eST_AdvOff_L))
			{
				wStatus = 1;
			}
			else if((RelayCmd.Status == eST_WaitOffR)
					||(RelayCmd.Status == eST_RlyOffGdR)
					||(RelayCmd.Status == eST_AdvOff1GdR)
					||(RelayCmd.Status == eST_AdvOff2GdR)
					||(RelayCmd.Status == eST_RlyOffGd_L)
					||(RelayCmd.Status == eST_AdvOffGd_L))
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
				if((RelayCmd.Status == eST_AllowOn)||(RelayCmd.Status == eST_AllowOnKp))
				{
					wStatus = 2;
				}
				else
				{
					//��ʱ�����Ժ�Ҫ��
                    if((RelayTypeConst == RELAY_OUTSIDE)&&(g_RelaySta == ST_RELAY_CLOSE))//���ã�����Ϊ0����Ϊ�ж��˼̵���ʵ��״̬���������е�������բ�Ƹ���̵���ʵ��״̬
					{
						wStatus = 0;
					}
					else
					{
						wStatus = 1;
					}
				}
			}
			else
			{
				wStatus = 0;
			}
			break;
		case 0x02:
			//����״̬			FALSE--�Ǳ���		TRUE--����״̬
			if((RelayCmd.Status == eST_RelayOnKp)||(RelayCmd.Status == eST_AllowOnKp))
			{
				wStatus = 1;
			}
			break;
		case 0x03:
			//����״̬			FALSE--��Զ�̣����أ�����	TRUE--Զ�̣����أ�����
			if(RelayCmd.Alarm == TRUE)
			{
				wStatus = 1;
			}
#if( PREPAY_MODE == PREPAY_LOCAL )
			else if(api_GetLocalStatus() >= eAlarm1)
			{
				wStatus = 1;
			}
#endif
			break;
		case 0x04:
			wStatus = 0;
			if((RelayCmd.Status == eST_AllowOn)
			   ||(RelayCmd.Status == eST_AllowOnKp)
			   ||(RelayCmd.Status == eST_AdvOff_L))
			{
				wStatus = 1;
			}

			break;
		case 0x05:
			wStatus = 0;
			if(((RelayCmd.Status >= eST_RlyOffR)&&(RelayCmd.Status <= eST_AdvOff2R))
			   ||((RelayCmd.Status >= eST_WaitOffR)&&(RelayCmd.Status <= eST_AdvOff2GdR)))
			{
				wStatus = 1;
			}
			break;
		case 0x06:  //���ƻ�·����͸��ɿ����󶯲�һ�� ֻҪ�̵���״̬������״̬��һ�¾��ǿ��ƻ�·����
			wStatus = 0;
			if(g_RelayErrCounter != 0)
			{
				wStatus = 1;
			}
			break;
		case 0x07:
			if((RelayCmd.Status == eST_RlyOffR)
			   ||(RelayCmd.Status == eST_AdvOff1R)
			   ||(RelayCmd.Status == eST_AdvOff2R)
			   ||(RelayCmd.Status == eST_WaitOffR)
			   ||(RelayCmd.Status == eST_RlyOffGdR)
			   ||(RelayCmd.Status == eST_AdvOff1GdR)
			   ||(RelayCmd.Status == eST_AdvOff2GdR))
			{
				wStatus = 1;
			}
			else
			{
				wStatus = 0;
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
WORD WriteRelayPara(BYTE Type, const BYTE *Buf)
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


