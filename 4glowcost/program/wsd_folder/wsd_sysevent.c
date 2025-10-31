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
const char event_name[64] = {"sysevent.txt"};

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

	fd = nwy_file_open(event_name, NWY_RB_PLUS_MODE);
	if (fd >= 0)
	{
		if(nwy_file_seek(fd,addr,NWY_SEEK_SET) == addr)
		{
			if (Operation == WRITE)//д����
			{
				if(nwy_file_write(fd,Buf,bLen) == bLen )
				{
					nwy_file_close(fd);
					nwy_ext_echo("\r\n write file success!!!");
					return TRUE;
				}
				else
				{
					nwy_ext_echo("\r\n write sysevent file fail,len:%d,addr:%d", bLen,addr);
					nwy_file_close(fd);
					return FALSE;
				}
			}
			else
			{
				if (nwy_file_read(fd,Buf,bLen) == bLen)
				{
					nwy_file_close(fd);
					nwy_ext_echo("\r\n read file success!!!");
					return TRUE;
				}
				else
				{
					nwy_ext_echo("\r\n read sysevent file fail,addr:%d", addr);
					nwy_file_close(fd);
					return FALSE;
				}
			}
		}
		else
		{
			nwy_ext_echo("\r\n fseek sysevent file fail, addr:%d", addr);
			nwy_file_close(fd);
			return FALSE;
		}
	}
	else
	{
		nwy_ext_echo("\r\n open sysevent file fail,event_name:%s, ret:%d", event_name, fd);
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
	BYTE flag;

	flag = nwy_file_exist(event_name);
	if(flag == FALSE)
	{
		fd = nwy_file_open(event_name, NWY_AB_PLUS_MODE);
    	if (fd >= 0)
    	{
			nwy_file_close(fd);
			api_OperateEventTable(WRITE,0,(BYTE*)&SysAbrEventConRom,sizeof(SysAbrEventConRom));
    	}
	else
	{
			nwy_ext_echo("\r\n create sysevent file fail,ret:%d", fd);
		}
	}
}
