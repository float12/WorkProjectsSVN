//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���
//��������	2016.11.14
//����		��Χ�����쳣Դ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if( (SEL_EVENT_RTC_ERR == YES)||(SEL_EVENT_SAMPLECHIP_ERR == YES) )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
static const DWORD RtcErrDefaultOadConst[] =
{
	0x04,//��������ĸ���
	0x00102201, 	   //�¼�����ʱ�������й��ܵ���
	0x00202201, 	   //�¼�����ʱ�̷����й��ܵ���
	0x00108201, 	   //�¼������������й��ܵ���
	0x00208201, 	   //�¼����������й��ܵ���
};
#endif

#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
static const DWORD SampleChipErrDefaultOadConst[] =
{
	0x02,//��������ĸ���
	0x00102201, 	   //�¼�����ʱ�������й��ܵ���
	0x00202201, 	   //�¼�����ʱ�̷����й��ܵ���
};
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��ȡ��ǰʱ�ӹ���״̬
//
//����:  
//  Phase[in]: ��Ч          
//   
//����ֵ:	1   �й���
//			0	�޹���
//
//��ע: 
//-----------------------------------------------
#if( SEL_EVENT_RTC_ERR == YES )
BYTE GetRtcErrStatus(BYTE Phase)
{
	//ʱ��оƬ�𻵻����ϵ�ʱ�����
	if((api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
	|| (api_GetRunHardFlag(eRUN_HARD_BROADCAST_ERR_FLAG) == TRUE ))	
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------------
//��������: ʱ�ӹ����¼����ϵ紦��
//
//����:  ��
//                    
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
void RtcErrPowerOn( void )
{
	BYTE TmpOADBuf[10];
	WORD Len;
	TRealTimer TmpRealTimer,TmpRealTimer2;
	TDLT698RecordPara TmpDLT698RecordPara;
	
	memset( (BYTE *)&TmpRealTimer2, 0, sizeof(TRealTimer) );
	memset( (BYTE *)&TmpRealTimer, 0xff, sizeof(TRealTimer) );	 
	TmpOADBuf[0] = 0x20;
	TmpOADBuf[1] = 0x20;
	TmpOADBuf[2] = 0x02;
	TmpOADBuf[3] = 0x00;
	TmpDLT698RecordPara.pOAD = TmpOADBuf;//pOADָ��
	TmpDLT698RecordPara.OADNum = 1; 	//OAD����
	TmpDLT698RecordPara.eTimeOrLastFlag = eNUM_FLAG;//ѡ�����
	TmpDLT698RecordPara.TimeOrLast = 0x01;	//��һ��
	TmpDLT698RecordPara.Phase = ePHASE_ALL;
	TmpDLT698RecordPara.OI = 0x302e;					
	Len = api_ReadProRecordData( 0, &TmpDLT698RecordPara, sizeof(TRealTimer), (BYTE*)&TmpRealTimer );
	if( (memcmp( (BYTE *)&TmpRealTimer, (BYTE *)&TmpRealTimer2, sizeof(TRealTimer) ) == 0)
	||(Len == 1) )//ʱ�ӹ����¼����һ����¼����ʱ��Ϊnull
	{
	
		//��λʱ�ӹ����¼���־
		EventStatus.CurrentStatus[eSUB_EVENT_RTC_ERR/8] |= (BYTE)(0x01<<(eSUB_EVENT_RTC_ERR%8));
		api_DealEventStatus( eSET_EVENT_STATUS, eSUB_EVENT_RTC_ERR );
	}

}


//-----------------------------------------------
//��������: ��ʼ��ʱ�ӹ����¼�����
//
//����:  ��
//   
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void api_FactoryInitRtcErrPara( void )
{
	api_WriteEventAttribute( 0x302e, 0xff, (BYTE *)&RtcErrDefaultOadConst[0], sizeof(RtcErrDefaultOadConst)/sizeof(DWORD) );
}
#endif

//-----------------------------------------------
//��������: ��ȡ��ǰ����оƬ����״̬
//
//����:  
//  Phase[in]: ��Ч          
//   
//����ֵ:	1   �й���
//			0	�޹���
//
//��ע: 
//-----------------------------------------------
#if( SEL_EVENT_SAMPLECHIP_ERR == YES )
BYTE GetSampleChipErrStatus(BYTE Phase)
{
	if( api_CheckError(ERR_CHECK_5460_1) == TRUE )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//-----------------------------------------------
//��������: ��ʼ������оƬ�����¼�����
//
//����:  ��
//   
//����ֵ:	��
//
//��ע: 
//-----------------------------------------------
void FactoryInitSampleChipErrPara( void )
{
	api_WriteEventAttribute( 0x302f, 0xff, (BYTE *)&SampleChipErrDefaultOadConst[0], sizeof(SampleChipErrDefaultOadConst)/sizeof(DWORD) );
}
#endif

#endif

