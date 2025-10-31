//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.9
//����		�����¼����쳣�¼���¼
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

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
BYTE		SysUNMsgCounter;		//һ���쳣�¼���¼��¼������
WORD		SysFREEMsgCounter;		//һ�������¼���¼��¼������

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ֻ��ȡʱ�䣬��ˢ��RealTimer
//
//����: 
//        	pBuf[in]		���ʱ��buf
//����ֵ:  	��
//
//��ע:  �������¼����ã�������͹����¸�λ���������¼��޷���¼ʱ������
//-----------------------------------------------
void SysFreeEventGetRtcTime(BYTE *pBuf)
{
	api_GetRtcDateTime(DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, pBuf);
	if( lib_IsAllAssignNum(pBuf, 0x00, 6) == TRUE )
	{
		api_FreeEventReadClockHara(pBuf);
	}
}

//-----------------------------------------------
//��������: ��¼�����¼�
//
//����: 
//			EventType[in]		�����¼�����
//        	EventSub[in]		��������
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_WriteFreeEvent(WORD EventType, WORD EventSub)
{
	DWORD dwAddr;
	WORD Point;
	//�ڶ�ջ�п�1���ṹ�Ŀռ�
	TFreeEvent TempBuf;
	
	//һ������¼512��
	if( SysFREEMsgCounter < (MAX_SYS_FREE_MSG) )
	{
		SysFREEMsgCounter++;
	}
	else
	{
		return;
	}

	//��ָ��
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_FREE_EVENT )
	{
		Point = 0;
	}
	
	TempBuf.EventType = EventType;
	//��������
	TempBuf.SubEvent = EventSub;

	//������6���ֽ������꣺�£��գ�ʱ���֣��루5��4��3��2��1��0��
	// api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	SysFreeEventGetRtcTime( (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + Point*(sizeof(TFreeEvent));
	api_WriteToContinueEEPRom(dwAddr, sizeof(TFreeEvent), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&Point);
}


//-----------------------------------------------
//��������: �������¼�
//
//����: 
//			Num[in]		��Num�������¼���¼
//          Buf[out]	�������	          
//����ֵ:  	�������ݳ���
//
//��ע:   
//-----------------------------------------------
WORD api_ReadFreeEvent(WORD Num,BYTE *Buf)
{
	WORD ReadPoint;
	DWORD dwAddr;
	
	if( (Num>MAX_FREE_EVENT) || (Num==0) )
	{
		return 0;
	}
	
	//��ָ��
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( SysFreeEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_FREE_EVENT-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysFreeEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TFreeEvent));
	api_ReadFromContinueEEPRom(dwAddr, sizeof(TFreeEvent), Buf );
	
	return sizeof(TFreeEvent);
}

//-----------------------------------------------
//��������: ��¼�쳣�¼�
//
//����: 
//			Msg[in]		�쳣�¼�����
//                    
//����ֵ:  	��
//
//��ע: 
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg)
{
	DWORD dwAddr;
	WORD Point;

	//�ڶ�ջ�п�1���ṹ�Ŀռ�
	TSysUNMsg TempBuf;
	
	//һ���ϵ�����¼120��
	if( SysUNMsgCounter < (MAX_SYS_UN_MSG-8) )
	{
		SysUNMsgCounter++;
	}
	else
	{
		return;
	}

	//��ָ��
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point);
	if( Point >= MAX_SYS_UN_MSG )
	{
		Point = 0;
	}

	TempBuf.EventType = Msg;
	//������6���ֽ������꣺�£��գ�ʱ���֣��루5��4��3��2��1��0��
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + Point*(sizeof(TSysUNMsg));
	api_WriteToContinueEEPRom(dwAddr, sizeof(TSysUNMsg), (BYTE *)&(TempBuf) );

	Point ++;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&Point );
}


//-----------------------------------------------
//��������: ���쳣�¼�
//
//����: 
//			Num[in]		��Num���쳣�¼���¼
//          Buf[out]	�������	          
//����ֵ:  	�������ݳ���
//
//��ע:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf)
{
	WORD ReadPoint;
	DWORD dwAddr;
	
	if( (Num==0) || (Num>MAX_SYS_UN_MSG) )
	{
		return 0;
	}
	
	//��ָ��
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( SysAbrEventConRom ), sizeof(WORD), (BYTE *)&ReadPoint);	
	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_SYS_UN_MSG-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TSysUNMsg));
	api_ReadFromContinueEEPRom(dwAddr, sizeof(TSysUNMsg), Buf );
	
	return sizeof(TSysUNMsg);
}


