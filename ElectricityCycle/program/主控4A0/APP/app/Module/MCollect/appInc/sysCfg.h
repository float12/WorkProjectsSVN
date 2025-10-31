#ifndef __SYSCFG_H__
#define __SYSCFG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define	FLAG_SYS			BIT0
#define	FLAG_METER			BIT1
#define	FLAG_MP_EX			BIT2
#define	FLAG_FKSYS			BIT3
#define	FLAG_TASK			BIT4
#define FLAG_DATASET		BIT5
#define FLAG_SCHEME			BIT7
	
typedef struct _TMetCfg_Base{
	BYTE	byUserType;		//�û�����ż��û�С���D7~D4�������ʾ�����ܱ��������û�����ţ���ֵ��Χ0~15�����α�ʾ16���û�����š�
 							//D3~D0�������ʾ�����ܱ��������û�С��ţ���ֵ��Χ0~15�����α�ʾ16��1���2�������������
	BYTE    byProtocolNo;	//ͨ��Э���	
	WORD	wMPNo;			//��698 �������

	BYTE	byPortNo;
	BYTE	byBaud;			//������300��600��1200��2400��4800��7200��9600��19200
	TSA		meter_addr;
	// TSA		cll_addr;
}TMetCfg_Base;


///////////////////////////////////////////////////////////////
//	�� �� �� : sys_GetMPCfg
//	�������� : ���ָ����Ų���������
//	������� : 
//	��    ע : 
//	��    �� : jiangjy
//	ʱ    �� : 2016��1��19��
//	�� �� ֵ : BOOL
//	����˵�� : WORD wNo,
//				TMetCfg_Base *pCfg
///////////////////////////////////////////////////////////////
BOOL sys_GetMPCfg(WORD wNo,TMetCfg_Base *pCfg);
BOOL JudgeCanID(WORD wMPSeq);
BOOL sys_InitMPCfg(WORD wMPSeq,TMetCfg_Base *pCfg);
BOOL IsJCMP(TMetCfg_Base *pCfg);
// BOOL sys_ReadTermNo(DWORD *pdwAreaCode,DWORD *pdwAddress);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
