//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
BYTE		SysUNMsgCounter;		//һ���ϵ��쳣�¼���¼��¼������

#define CONTINUE_SPACE_START_ADDR		0
#define GET_CONTINUE_ADDR( Addr_Name )		( (DWORD)&(((TConMem *)CONTINUE_SPACE_START_ADDR)->Addr_Name) )
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
TSysAbrEventConRom SysAbrEventConRom;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
const char event_name[64] = {"/sysevent.txt"};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  �����¼����ļ�ϵͳ(�˺���ֻ������¼�¼�)
//         
//����:      Operation 1д���� 0 ������
//           addr Ҫ�������ļ���ַ
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_OperateEventTable( BYTE Operation,int addr,BYTE*Buf,DWORD bLen)//�˺�������û����
{
	int fd;

	fd = nwy_sdk_fopen(event_name, NWY_RDWR);
	if (fd >= 0)
	{
		if(nwy_sdk_fseek(fd,addr,NWY_SEEK_SET) == addr)
		{
			if (Operation == WRITE)//д����
			{
				if(nwy_sdk_fwrite(fd,Buf,bLen) == bLen )
				{
					nwy_sdk_fclose(fd);
					// nwy_ext_echo("\r\n write file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
			else
			{
				if (nwy_sdk_fread(fd,Buf,bLen) == bLen)
				{
					nwy_sdk_fclose(fd);
					// nwy_ext_echo("\r\n read file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
		}
		else
		{
			nwy_sdk_fclose(fd);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
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
	static nwy_osiMutex_t *write_event_mutex = NULL;//���̶߳������д�¼����������Լ���
	//�ڶ�ջ�п�1���ṹ�Ŀռ�
	TSysUNMsg TempBuf;
	
	if(NULL == write_event_mutex)
	{
		write_event_mutex = nwy_create_mutex();
	}	
	if(NULL == write_event_mutex)
	{
		return;
	}
	nwy_lock_mutex(write_event_mutex, 0);
	//һ���ϵ�����¼120��
	if( SysUNMsgCounter < (MAX_SYS_UN_MSG-8) )
	{
		SysUNMsgCounter++;
	}
	else
	{
		nwy_unlock_mutex(write_event_mutex);  // ����
		return;
	}

	//��ָ��
	api_OperateEventTable(READ,GET_CONTINUE_ADDR( SysAbrEventConRom ),(BYTE *)&Point,sizeof(WORD));

	if( Point >= MAX_SYS_UN_MSG )
	{
		Point = 0;
	}

	TempBuf.EventType = Msg;
	//������6���ֽ������꣺�£��գ�ʱ���֣��루5��4��3��2��1��0��
	api_GetRtcDateTime( DATETIME_YYMMDDhhmmss|DATETIME_SECOND_FIRST|DATETIME_BCD, (BYTE *)&TempBuf.EventTime[0] );
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + Point*(sizeof(TSysUNMsg));
	api_OperateEventTable(WRITE,dwAddr,(BYTE *)&(TempBuf),sizeof(TSysUNMsg));

	Point ++;
	api_OperateEventTable(WRITE,GET_CONTINUE_ADDR( SysAbrEventConRom ), (BYTE *)&Point, sizeof(WORD));
	nwy_unlock_mutex(write_event_mutex);
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
	api_OperateEventTable(READ,GET_CONTINUE_ADDR( SysAbrEventConRom ),(BYTE *)&ReadPoint,sizeof(WORD));

	if( ReadPoint >= Num )
	{
		ReadPoint = ReadPoint - Num;
	}
	else
	{
		ReadPoint = ReadPoint+MAX_SYS_UN_MSG-Num;
	}
	
	dwAddr = GET_CONTINUE_ADDR( SysAbrEventConRom ) + sizeof(WORD) + ReadPoint*(sizeof(TSysUNMsg));
	api_OperateEventTable(READ,dwAddr,Buf,sizeof(TSysUNMsg));
	
	return sizeof(TSysUNMsg);
}
//--------------------------------------------------
//��������:  �ϵ紴�����ʱ���¼�ļ�//�������ϵ�Уʱ֮��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_PowerOnCreatSyseventTable( void )
{
    int fd;

	fd = nwy_sdk_fopen(event_name, NWY_RDWR);
	if(fd < 0)
	{
		fd = nwy_sdk_fopen(event_name, NWY_CREAT | NWY_RDWR);
    	if (fd >= 0)
    	{
			api_OperateEventTable(WRITE,0,(BYTE*)&SysAbrEventConRom,sizeof(SysAbrEventConRom));
    	}
	}
	else
	{
		nwy_sdk_fclose(fd);
	}
}
//--------------------------------------------------
//��������:  ɾ����¼�¼��ļ�
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL  api_DeleteSysEventFile( void )
{
	if (nwy_sdk_fexist(event_name) == TRUE)
	{
		nwy_ext_echo("\r\n exit fle");
		if(nwy_sdk_file_unlink(event_name) == NWY_SUCESS)
		{
			nwy_ext_echo("\r\n dele secc");
			return TRUE;
		}
		else
		{
			nwy_ext_echo("\r\ndelte fail is%d",nwy_sdk_file_unlink(event_name));
			return FALSE;
		}
	}		
	else
	{
		return FALSE;
	}
}
