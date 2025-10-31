//---------------------------------------------
// ���Ǵ����¼
//---------------------------------------------

#include "AllHead.h"

static const DWORD RelayErrOad[] =
{
#if( MAX_PHASE == 1)
	0x05,
	0xF2052201,    //���ɿ�������ʱ״̬
	0x00102201,    //���ɿ�������ǰ�����й��ܵ���
	0x00202201,    //���ɿ�������ǰ�����й��ܵ���
	0x00108201,    //���ɿ��������������й��ܵ���
	0x00208201     //���ɿ������������й��ܵ���
#else
	0x05,
	0xF2052201,    //���ɿ�������ʱ״̬
	0x00102201,    //���ɿ�������ǰ�����й��ܵ���
	0x00202201,    //���ɿ�������ǰ�����й��ܵ���
	0x00108201,    //���ɿ��������������й��ܵ���
	0x00208201     //���ɿ������������й��ܵ���

#endif
};

#if( SEL_EVENT_RELAY_ERR == YES )
void FactoryInitRelayErrPara( void )
{	
	api_WriteEventAttribute( 0x302B, 0xFF, (BYTE *)&RelayErrOad[0], sizeof(RelayErrOad)/sizeof(DWORD) );	
}
#endif
// ���ɿ����󶯴���������
#if( SEL_EVENT_RELAY_ERR == YES )

//------------------------------------------------------------------------------------------------------
// �жϸ��ɿ������Ƿ���
BYTE GetRelayErrStatus(BYTE Phase)
{
	//�ɼ̵���ģ���ṩ�����󶯵�״̬
	// �̵�������״̬ //���ƻ�·����
	if( api_GetSysStatus(eSYS_STATUS_RELAY_ERR) == TRUE )
	{
		//���β����̵������Ѿ���¼��һ�������ļ�¼��
		if(api_DealRelayErrFlag(READ,0x0000)& 0x00AA)
		{
			return 0;
		}
		return 1;
	}
	
	api_DealRelayErrFlag(WRITE,0x00AA);//�ڲ��б�����δ��ʼ����д�������ֻ�п�ʼ�Ĳſ���д�����			
	return 0;
}



#endif

