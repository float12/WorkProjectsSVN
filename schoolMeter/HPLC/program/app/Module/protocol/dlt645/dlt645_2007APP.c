//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	κ����
//��������	2016.9.2
//����		����DL/T645-2007��ԼԴ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

#if( SEL_DLT645_2007 == YES )
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

//-----------------------------------------------
//				���ļ�ʹ�õı���������
TDLT645APPFollow 			DLT645APPFollow[MAX_COM_CHANNEL_NUM];
T_DLT645 					gPr645[MAX_COM_CHANNEL_NUM];
TDLT645Sendbuf 				DLT645Sendbuf[MAX_COM_CHANNEL_NUM];

__no_init BYTE 				g_645PassWordErrCounter[2];//����������03 04 �ϵ粻��ʼ��

//��֤��ͨ����ֻ�ܶ�����š�ͨ�ŵ�ַ�������š���ǰ���ڡ���ǰʱ�䡢��ǰ����ʱ�䡢��ǰ���ܡ���ǰʣ���������֤����ѯ���
const DWORD g_dwHongwaiID[] = {0x04000402,0x04000401,0x04800004,0x04000101,0x04000102,0x0400010c,0x00900200,0x078003FF,0x070003FF};
const DWORD FirstDataID[] = {0x04000108,0x04000109,0x04000306,0x04000307,0x04000402,0x0400040E,0x04001001,0x04001002,0x040502ff,0x040605FF,0x04060AFF,0x04060BFF};//ȷ��һ��songchen

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//--------------------------------------------------
//��������:  698.45�ϵ纯��
//         
//����  :   ��
//
//����ֵ:    ��  
//         
//��ע����:  ��
//--------------------------------------------------
void PowerUpDlt645(void)
{
    api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
}

//--------------------------------------------------
//��������:�Ͽ������֤
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void api_Release645_Auth( void )
{
    api_ClrSysStatus(eSYS_STATUS_ID_645AUTH);
}

//-----------------------------------------------
//��������  :   ��ʼ��645����
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void FactoryInitDLT645(void)
{
	memset( g_645PassWordErrCounter, 0x00, sizeof(g_645PassWordErrCounter) );
}

WORD Judge645AddrCanDo( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_07 )//����07��ԼĬ�Ϸ��سɹ�
	{
		return DLT645_OK_00;
	}
	
	if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )//ȫ99��ַ
	{
		switch( gPr645[PortType].byReceiveControl )//֧�ֵ�����
		{
			case eCONTROL_RECE_08://�㲥Уʱ
			case eCONTROL_RECE_16://����
			case eCONTROL_RECE_1D://���ö๦�ܶ���				
			case 0x1f://�����ز�
			case eCONTROL_RECE_11://������(Һ���鿴)
				return DLT645_OK_00;
			default:
			
			return DLT645_ERR_DATA_01;
		}
	}

	switch( gPr645[PortType].byReceiveControl )//����ʹ���ͨ���ַ��֧�ֵ�����
	{
		case eCONTROL_RECE_08://�㲥Уʱ
		case eCONTROL_RECE_16://����
		case eCONTROL_RECE_14://д����
		case eCONTROL_RECE_17://���Ĳ�����
		case eCONTROL_RECE_19://��������
		case eCONTROL_RECE_1A://����������
		case eCONTROL_RECE_1B://�¼�����
		case eCONTROL_RECE_1C://��������
			if( (gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_ALLAA)//ȫAA��ַ
			  ||(gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_AA) )//ͨ���ַ
			{
				return DLT645_ERR_DATA_01;
			}
			else if( api_GetSysStatus( eSYS_STATUS_POWER_ON ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		break;

		default:
			return DLT645_OK_00;	
	}

	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_13 )//����ַ��֧��ȫAA��ַ
	{
		if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_ALLAA )//ȫAA��ַ
		{
			return DLT645_ERR_DATA_01;
		}
	}
	
	return DLT645_OK_00;
}

//---------------------------------------------------------------
//��������: �ж��Ƿ���һ������
//
//����: 	dwID[in]  ���ݱ�ʶ
//                   
//����ֵ:   TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
BYTE api_JudgeFirstData( DWORD dwID )
{
	BYTE i;
	for( i=0; i<(sizeof(FirstDataID)/sizeof(DWORD)); i++ )
	{
		if( dwID == FirstDataID[i] )
		{
			return TRUE;
		}
	}
	return FALSE;	

}

//--------------------------------------------------
//��������:  645����Ȩ���ж�
//
//����:      eSERIAL_TYPE PortType[in]��ControlByte[in]
//
//����ֵ:
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			
//
//��ע����:  ��
//--------------------------------------------------

WORD api_JudgeAuthority(eSERIAL_TYPE PortType)
{
	BYTE i;
	BYTE ControlByte;
	BYTE *ProBuf;
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	ControlByte = gPr645[PortType].byReceiveControl;

	if( ControlByte == eCONTROL_RECE_03 )
	{
		if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
            for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
			{
				if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
				{
					return DLT645_OK_00;
				}
			}
			return DLT645_ERR_DATA_01;//����01û��
		}	
	}
	else if( ControlByte == eCONTROL_RECE_08 )//�㲥Уʱ��Ҫ�жϺ���Ȩ��
	{
		if( (PortType == eIR) && (api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
			if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
			{
				return DLT645_NO_RESPONSE;
			}
			else
			{
				return DLT645_ERR_PASSWORD_04;
			}
		}
	}
	else if( ControlByte == eCONTROL_RECE_11 )
	{
	    if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )
	    {
            return DLT645_OK_00;
	    }
		else if( (PortType == eIR)&&(api_GetSysStatus( eSYS_IR_ALLOW_PRG ) == FALSE) )
		{
            for( i=0; i<(sizeof(g_dwHongwaiID)/sizeof(DWORD)); i++ )
			{
				if( gPr645[PortType].dwDataID.d == g_dwHongwaiID[i] )
				{
					return DLT645_OK_00;
				}
			}
	
			if(( gPr645[PortType].dwDataID.b[0] == 0x00 )&&( gPr645[PortType].dwDataID.b[3] == 0x00 ))//����ǰ����
			{
				return DLT645_OK_00;
			}
			
			return DLT645_ERR_PASSWORD_04;
		}	
	}
	else if( ControlByte == eCONTROL_RECE_14 )
	{
		if( !((gPr645[PortType].dwDataID.b[3])&0x04 ) )
		{
			//�û��Բ�֧��д������������(0x04XXXXXX֮�������)��̣�Ӧ��"�����/δ��Ȩ"
			return DLT645_ERR_PASSWORD_04;
		}

        if(gPr645[PortType].dwDataID.d == 0x04001503)//��λ�����ϱ�״̬�ֲ��ж��ܼ�������
        {
            return DLT645_OK_00;
        }
        
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )//02 ��
		{
			if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//�ǳ��ڲ�������
			{
			    if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )//��������سɹ�
				{
					return DLT645_OK_00;
				}
				else
				{
                    return DLT645_ERR_PASSWORD_04;
				}
			}
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )//04 ��
		{
			return DLT645_ERR_PASSWORD_04;//������������
		}
		else if( (gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )||( gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL ) )//98 99 ��
		{
			//�����֤�������Ѿ��й��������ظ�
			//MAC����
			if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
			{
				return DLT645_ERR_PASSWORD_04;
			}
				
			if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)//98��
			{
				if(gPr645[PortType].dwDataID.d == 0x04001503)
				{
					return DLT645_OK_00;
				}
				if( api_JudgeFirstData(gPr645[PortType].dwDataID.d) == TRUE )//�����һ�����ݣ�����99H���������÷��������
				{
					return DLT645_ERR_PASSWORD_04;
				}	
			}
			else//99��
			{
				if(gPr645[PortType].dwDataID.d == 0x04001503)
				{
					return DLT645_ERR_PASSWORD_04;
				}
			}
		}
	}
	else if( ControlByte == eCONTROL_RECE_1C )
	{
		//����Ϊ98��
		if(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL)
		{
			//MAC����
			if( api_GetRunHardFlag(eRUN_HARD_MAC_ERR_FLAG) == TRUE )
			{
				return R_ERR_MAC;
			}
		}
		else if( (gPr645[PortType].bLevel==MINGWEN_H_PASSWORD_LEVEL) || (gPr645[PortType].bLevel==MINGWEN_L_PASSWORD_LEVEL) )
		{	
			memmove( ProBuf+22, ProBuf+18, 8 );	
			lib_InverseData( ProBuf+24, 6 );
			if( (ProBuf[22]==0x1a) ||(ProBuf[22]==0x1b) || (ProBuf[22]==0x1c) || (api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY)==TRUE) )
			{
				//֧��02,04������������բ����բ����,������������֤Ҳ���Ժ�բ
			}
			else
			{
				return R_ERR_PASSWORD;
			}
		}
		else
		{
			return R_ERR_PASSWORD;
		}
	}

	return DLT645_OK_00;
}

//---------------------------------------------------------------
//��������: �ж��Ƿ��к���֡
//
//����: 	eSERIAL_TYPE PortType
//
//����ֵ:
//
//��ע:
//---------------------------------------------------------------
static WORD JudgeFollowFlag( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_12 )//����Ƕ�ȡ����֡
	{
		if( DLT645APPFollow[PortType].FollowFlag != eNO_FOLLOW )//����к���֡��֡�����ڷ�֡������ȷ
		{
			return DLT645_OK_00;
		}
		else
		{
			return DLT645_ERR_ID_02;//���طǷ�����
		}
	}
	else
	{
		return DLT645_OK_00;
	}
}
//---------------------------------------------------------------
//��������: �жϽ�������
//
//����: 	eSERIAL_TYPE PortType
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
static BOOL CheckUnlockPassword( eSERIAL_TYPE PortType )
{	
	BYTE i;
	TMuchPassword Password;

	//���������������״̬����Ч
	if(	(api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == FALSE) &&
		(api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == FALSE) &&
		(api_GetRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS) == FALSE ))
	{
		return FALSE;	
	}

	for( i=0; i<3; i++ )
	{
		if( gPr645[PortType].bPassword[i] != 0xAB )
		{
			break;
		}
	}
	
	if( i != 3 ) //�����жϲ���ȷ
	{
		return FALSE;
	}
	else
	{
		g_645PassWordErrCounter[0] = 0;	//02��������������0
		g_645PassWordErrCounter[1] = 0;	//04��������������0
		api_ClrRunHardFlag(eRUN_HARD_PASSWORD_ERR_STATUS);//����698��������־
		api_ClrRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);//����02����������־
		api_ClrRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);//����04����������־
		return TRUE;
	}
}

//---------------------------------------------------------------
//��������: �ж�����!!!!!!!!����
//
//����: 	eSERIAL_TYPE PortType
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//---------------------------------------------------------------
static BOOL CheckPassword( eSERIAL_TYPE PortType )
{
	BYTE i;
	TMuchPassword Password;

	if(gPr645[PortType].bLevel == SYSTEM_UNLOCK_PASSWORD_LEVEL)
	{
		//��������������溯���д�����ɣ�Ĭ�Ϸ���FALSE
		CheckUnlockPassword(PortType);
		return FALSE;
	}
	
	//��EEPROM��ȡ�������� ���EEPROM���ݴ��� �������� ��ֹEEPROM�������δ��ʼ���޷��򿪳���ģʽ
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.MuchPassword), sizeof(TMuchPassword),(BYTE *)Password.Password645) != TRUE )
	{
		return TRUE;
	}

	//�ж��Ƿ�Ϊ02�����볧�ڹ�Լ
	if( (gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY) && (gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		//�ж��Ƿ�Ϊ�򿪻�رճ���ģʽ��ʶ
		if( (gPr645[PortType].pDataAddr[0] == 0x01)&&
			(gPr645[PortType].pDataAddr[1] == 0x02)&&
			(gPr645[PortType].pDataAddr[2] == 0xDF)&&
			(gPr645[PortType].pDataAddr[3] == 0xDB)  )
		{
			return TRUE;
		}
	}

	for( i=0; i<3; i++ )
	{
		if( gPr645[PortType].bPassword[i] != Password.Password645[gPr645[PortType].bLevel][i] )
		{
			break;
		}
	}
	
	if( i != 3 ) //�����жϲ���ȷ
	{
		//������֤���� �ۼӴ������
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
		{
			if( (++g_645PassWordErrCounter[0]) >= MAX_PASSWORD_ERR_TIMES )
			{			
				api_SetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS);
			    api_WriteFreeEvent( EVENT_PROGRAMKEY_LOCK, 0 );
			}
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
		{
			if( (++g_645PassWordErrCounter[1]) >= MAX_PASSWORD_ERR_TIMES )
			{
				api_SetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS);
			}
		}
		return FALSE;
	}
	//������֤��ȷʱ�������������
	else if( i == 3 )
	{
		if( gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL )
		{
			g_645PassWordErrCounter[0] = 0;
		}
		else if( gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL )
		{
			g_645PassWordErrCounter[1] = 0;
		}
	}

	//����ǰΪMINGWEN_H_PASSWORD_LEVEL����״̬����ʹMINGWEN_H_PASSWORD_LEVEL������֤��ȷ��ҲҪ����FALSE
	if( (api_GetRunHardFlag(eRUN_HARD_H_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		return FALSE;
	}
	//����ǰΪMINGWEN_L_PASSWORD_LEVEL����״̬����ʹMINGWEN_L_PASSWORD_LEVEL������֤��ȷ��ҲҪ����FALSE
	else if( (api_GetRunHardFlag(eRUN_HARD_L_PASSWORD_ERR_STATUS) == TRUE) && (gPr645[PortType].bLevel == MINGWEN_L_PASSWORD_LEVEL) )
	{
		return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------
//��������  :   ��ʼ��698��־
//
//����  : 	��
//
//����ֵ:      ��
//
//��ע����  :   ��
//-----------------------------------------------
void InitDLT645Flag( eSERIAL_TYPE PortType )
{
    if( DLT645APPFollow[PortType].FollowFlag != eREPORT_FOLLOW )
    {
        DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
    }
}

//--------------------------------------------------
//��������:  �ж����뺯��
//
//����  : ��
//
//����ֵ:    static
//
//��ע����:  ��
//--------------------------------------------------
static WORD JudgePasswordLevel( eSERIAL_TYPE PortType )
{	
	//����������ж�����ȼ������� �Ų��Ƿ��Ѿ����@@@@@
	//G���ź�ǿ�� ��λ�����ϱ�״̬�� 
	//ģ�����չ���� κ�����ṩ�ĵ�@@@@

	//���á�����������㡢������㡢�¼����㡢�ѿز���(����բ������������)
	if( (gPr645[PortType].byReceiveControl != eCONTROL_RECE_14)//ֻ����5������������
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_19)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1A)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1B)
	  &&(gPr645[PortType].byReceiveControl != eCONTROL_RECE_1C) )	
	{
		return DLT645_OK_00;
	}
	
	//��λ�����ϱ�״̬�� �������벻�еȼ�ֱ�ӷ�����ȷ д�����ź�ǿ��
	if( gPr645[PortType].dwDataID.d == 0x04001503 )
	{
		return DLT645_OK_00;
	}

	if( (gPr645[PortType].bLevel != CLASS_1_PASSWORD_LEVEL) //ֻ֧����4������
	  &&(gPr645[PortType].bLevel != CLASS_2_PASSWORD_LEVEL)
	  &&(gPr645[PortType].bLevel != MINGWEN_H_PASSWORD_LEVEL) //����ȼ��ú궨���ʾ @@@@
	  &&(gPr645[PortType].bLevel != MINGWEN_L_PASSWORD_LEVEL)
	  &&(gPr645[PortType].bLevel != SYSTEM_UNLOCK_PASSWORD_LEVEL) )//������������	
	{
		return DLT645_ERR_PASSWORD_04;
	}

	if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == TRUE )//����ģʽ�� Ĭ��Ȩ����ȷ
	{
        return DLT645_OK_00;
	}
	else
	{
        if( (gPr645[PortType].bLevel == CLASS_1_PASSWORD_LEVEL)//�����ܼ�������������ж�,ֻ�ж��Ƿ������֤ͨ��
        ||(gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL) )
        {
            if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == TRUE )
            {
                return DLT645_OK_00;
            }
            else
            {
               return DLT645_ERR_PASSWORD_04;
            }
        }
        else
        {
            if( CheckPassword( PortType ) == TRUE )//�ж������Ƿ���ȷ
            {
                return DLT645_OK_00;
            }
        }
	}
	
	return DLT645_ERR_PASSWORD_04;
}

//--------------------------------------------------
//��������:  645���ݸ�ʽ�ж�
//         
//����:      TYPE_PORT PortType[in]
//         
//           WORD Len[in]
//         
//           BYTE *pBuf[in]
//         
//����ֵ:    WORD
//         
//��ע����:  ��
//--------------------------------------------------
static WORD Proc645FrameData( eSERIAL_TYPE PortType )
{
	WORD wResult;
	
	wResult = Judge645AddrCanDo( PortType );
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = JudgePasswordLevel( PortType );
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = api_JudgeAuthority(PortType);
	if( wResult != DLT645_OK_00 )
	{
		return wResult;
	}
	
	wResult = JudgeFollowFlag( PortType );
    
	return wResult;
}
//--------------------------------------------------
//��������:  645������
//         
//����:      eSERIAL_TYPE PortType[in]
//         
//����ֵ:
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
//         
//��ע����:  ��
//--------------------------------------------------
WORD ReadData0X11( eSERIAL_TYPE PortType )
{
	DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;//����յ���������������������ݱ�־

	return( GetPro645IDData( PortType, 0xFF, gPr645[PortType].dwDataID.b, MAX_APDU_SIZE, DLT645Sendbuf[PortType].DataBuf ) );
}
//---------------------------------------------------------------
//��������: ��ȡ����֡
//
//����:    eSERIAL_TYPE PortType[in] - ���ں�
//                   
//����ֵ:  
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 
//��ע:   
//---------------------------------------------------------------
WORD ReadContinueData0X12( eSERIAL_TYPE PortType )
{
	if( gPr645[PortType].bSEQ == (DLT645APPFollow[PortType].bLastNum + 1) )
	{
		if( DLT645APPFollow[PortType].FollowFlag == eNORMAL_FOLLOW )
		{
			return DLT645_OK_00;
		}
		else if( DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW )//���ɼ�¼����֡ ��Ҫ���⴦��			
		{
			return GetRequest645Lpf( PortType, (gPr645[PortType].pMessageAddr+10), MAX_APDU_SIZE, DLT645Sendbuf[PortType].DataBuf );
		}
	}
    
    return DLT645_OK_00;
}

//--------------------------------------------------
//��������:  645-2007Э����������ܱ�ͨ�ŵ�ַ����֧�ֵ�Ե�ͨ�š�
//         
//����:      BYTE *ProBuf[in]
//         		ProBuf: ����ͨ�Ż���	
//         
//����ֵ:    0x0000 - 0x7FFF : ���ݳ���
//			0x8000 + ��������
//         
//��ע����:  ��Ӧ645-2007 �е� ���£�
//	7.4.1	��վ����֡
//	���ܣ���������ܱ�ͨ�ŵ�ַ����֧�ֵ�Ե�ͨ�š�
//	��ַ��AA��AAH
//	�����룺C=13H
//	�����򳤶ȣ�L=00H
//7.4.2	��վ����Ӧ��֡
//	�����룺C=93H
//��	�����򳤶ȣ�L=06H
//--------------------------------------------------
WORD DoDlt645_2007ReadCommAddr0X13( eSERIAL_TYPE PortType )
{
	lib_ExchangeData( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&FPara1->MeterSnPara.CommAddr[0], 6 );

	return 6;
}

//--------------------------------------------------
//��������:  645-2007Э����������ܱ�ͨ�ŵ�ַ����֧�ֵ�Ե�ͨ�š�
//         
//����:      BYTE *ProBuf[in]
//         		ProBuf: ����ͨ�Ż���	
//         
//����ֵ:    ��
//         
//��ע����:  1��������������̼����ʹ�ã����ޱ�̼�����֧�ָ����� 
//			2����վ�쳣��Ӧ��        
//--------------------------------------------------
WORD DoDlt645_2007WriteCommAddr0X15( eSERIAL_TYPE PortType )
{
	return DLT645_ERR_ID_02;
}

//---------------------------------------------------------------
//��������: ��������
//
//����: 	eSERIAL_TYPE PortType
//                   
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 - �������
//
//��ע:   
//---------------------------------------------------------------
WORD ProClrCurrentMaxDemand( eSERIAL_TYPE PortType )
{
	WORD wResult,wTmpClearMeterType;
	BYTE *ProBuf;
	
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	
	wResult = DLT645_ERR_DATA_01;
	
	if( api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == FALSE )
	{		
		return DLT645_ERR_PASSWORD_04;
	}
	
	if( api_GetRunHardFlag( eRUN_HARD_MAC_ERR_FLAG ) == TRUE )
	{ 
		return DLT645_ERR_PASSWORD_04;
	}
	
	#if( MAX_PHASE == 3) //�����û����������
	if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )
	{
		if( ProBuf[9] < 28 )//16����4mac+4����4�����ߴ���
		{
			return DLT645_ERR_DATA_01;
		}
		//����
		lib_InverseData( ProBuf + 22, ProBuf[9] - 12 );
		//MAC
		lib_ExchangeData( ProBuf + 22 + 16, ProBuf + 18,  4 );
		
		if( (api_DecipherSecretData( 0x85, ProBuf[9] - 12 + 4, ProBuf + 22, ProBuf )&0x8000) != 0 )
		{
			return DLT645_ERR_PASSWORD_04;
		}
	}
	
	//ʱ�䵹��	
	//lib_InverseData( ProBuf+20, 6 );
	
	if( api_JudgeClock645( ProBuf + 2 ) == FALSE )
	{
		return DLT645_ERR_PASSWORD_04;
	}      
	
	wResult = api_SetClearFlag( eCLEAR_DEMAND, 2 );
	if( wResult == FALSE )
	{
		wResult = DLT645_ERR_DATA_01;
	}
	api_SavePrgOperationRecord( ePRG_CLEAR_MD_NO ); //��¼���������¼�
	#endif	//#if( MAX_PHASE == 3)
	return wResult;
}

//---------------------------------------------------------------
//��������: �������
//
//����: 	eSERIAL_TYPE PortType[in]
//			Type
// 				- 0x55 �¼�����
//
// 				- 0xAA �������
//
//����ֵ:
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 - �������
//
//��ע:   
//---------------------------------------------------------------
WORD ProClrMeter( eSERIAL_TYPE PortType, BYTE Type )
{
	BYTE i;
	BYTE bTmpClearMeterTimes;
	TFourByteUnion CurEnergy[2], EvenID;
	BYTE *ProBuf, File;
	eCLEAR_METER_TYPE ClearFlag;
	eEVENT_INDEX EventOI;
	TFourByteUnion OMD;
	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;

	#if( PREPAY_MODE == PREPAY_LOCAL )
	if(( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE ) && (Type == 0xAA))
	{	
		return DLT645_ERR_PASSWORD_04;//������ģʽ������Զ�����㣬���ⲻ����������������
	}
	#endif

	if( (api_GetSysStatus( eSYS_STATUS_ID_645AUTH ) == FALSE)
	 && (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE) )
	{		
		return DLT645_ERR_PASSWORD_04;
	}
	
	if( api_GetRunHardFlag( eRUN_HARD_MAC_ERR_FLAG ) == TRUE )
	{ 
		return DLT645_ERR_PASSWORD_04;
	}
	//˽Կ�²�����������
	if( (api_GetRunHardFlag( eRUN_HARD_COMMON_KEY ) == FALSE) && (Type == 0xAA) )
	{
		return DLT645_ERR_PASSWORD_04; 
	}
	
	if( Type == 0x55 )
	{
		File = 0x85;
		ClearFlag = eCLEAR_EVENT;
		EventOI = ePRG_CLEAR_EVENT_NO;
	}
	else if( Type == 0xAA )
	{
		File = 0x84; 
		ClearFlag = eCLEAR_METER;
		EventOI = ePRG_CLEAR_METER_NO;
	}
	else
	{
		return DLT645_ERR_DATA_01;
	}
	if( (api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE)
		&&(gPr645[PortType].bLevel == MINGWEN_H_PASSWORD_LEVEL) )
	{
		//ʱ�䵹��	
		lib_InverseData( ProBuf+20, 6 );
		memcpy( ProBuf + 2, ProBuf + 20, 6 );
	}
	else if( gPr645[PortType].bLevel == CLASS_2_PASSWORD_LEVEL )
	{
		if( ProBuf[9] < 28 )//16����4mac+4����4�����ߴ���
		{
			return DLT645_ERR_DATA_01;
		}
		//����
		lib_InverseData( ProBuf + 22, ProBuf[9] - 12 );
		//MAC
		lib_ExchangeData( ProBuf + 22 + 16, ProBuf + 18,  4 );
		
		if( (api_DecipherSecretData( File, ProBuf[9] - 12 + 4, ProBuf + 22, ProBuf )&0x8000) != 0 )
		{
			return DLT645_ERR_PASSWORD_04;
		}
	}
	else//�������뼶�𷵻ش���
    {
		return DLT645_ERR_PASSWORD_04;
    }
	
	if( api_JudgeClock645( ProBuf + 2 ) == FALSE )
	{
		return DLT645_ERR_PASSWORD_04;
	}

	//�¼�����
	if( Type == 0x55 )
	{
		// memcpy( EvenID.b, ProBuf + 8, 4 );			//�˴�ȡ�¼��������ݱ�ʶλ�������⣬У԰��˴����иĶ���
		memcpy( EvenID.b, ProBuf + 26, 4 );	
		OMD.d = 0x00050043;
		if( EvenID.d != 0xFFFFFFFF )
		{
			return DLT645_ERR_DATA_01;
		}
		api_WritePreProgramData( 1, OMD.d );
	}
	//��λ��������־ Ĭ����ʱ1�룬����ʵ����ʱ��Χ500~1500ms,1200bps ��������20�ֽ�ʱ��Ϊ 183ms
	if( api_SetClearFlag( ClearFlag, 2 ) == FALSE )
	{
		return DLT645_ERR_DATA_01;
	}
	api_SavePrgOperationRecord( EventOI ); //��¼��������¼�
	
	return DLT645_OK_00;
}

//---------------------------------------------------------------
//��������: Զ�̷ѿ�����
//
//����: 	eSERIAL_TYPE PortType
//                   
//����ֵ:  
//
//��ע:   
//---------------------------------------------------------------
WORD ProcRemoteRelayCommand645( eSERIAL_TYPE PortType )
{
	WORD wResult;
	
 	wResult = api_ProcRemoteRelayCommand645( gPr645[PortType].pMessageAddr);// !!!!!!!!gPr645[PortType].byOperatorCode �����ߴ���δ����
 	
	//���󷵻�
	if( wResult & 0x8000 )
	{
		api_WriteFreeEvent(EVENT_RELAY_ERR, wResult);

		if(  gPr645[PortType].bPassword[2] != 0x01 ) //����ظ�1���ֽ�
		{
			return DLT645_ERR_PASSWORD_04;
		}
		else //����ظ�2���ֽ�
		{
			return wResult;
		}
	}
	else //��ȷ����
	{
		memcpy( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&wResult, 2 );
			
		return 2;
	}
}

//---------------------------------------------------------------
//��������: �㲥Уʱ
//
//����: 	 eSERIAL_TYPE PortType
//                   
//����ֵ:  
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 - �������
//			DLT645_NO_RESPONSE - ����ظ�
//
//��ע:   
//---------------------------------------------------------------
WORD ReviseTime0X08( eSERIAL_TYPE PortType )
{
	BYTE i;
	DWORD AbsMeterSecTime,AbsSetSecTime;
	TRealTimer TmpRealTimer;
	WORD wResult;
	
	//Ĭ�ϴ���
	wResult = DLT645_ERR_DATA_01;
	
	if( api_GetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG ) == FALSE )//�㲥Уʱһ��һ��
	{
		api_GetRtcDateTime( DATETIME_20YYMMDDhhmmss, (BYTE *)&TmpRealTimer ); //��ȡ��ǰʱ��
		AbsMeterSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//���㵱ǰʱ���������
	
		//BCDתHEX
		for( i = 0; i < 6; i++ )
		{
			gPr645[PortType].pDataAddr[i] = lib_BBCDToBin( gPr645[PortType].pDataAddr[i] );
		}
		
		//������
		lib_InverseData( gPr645[PortType].pDataAddr, 6 );
		
		TmpRealTimer.wYear = 2000 + gPr645[PortType].pDataAddr[0];
		memcpy( &TmpRealTimer.Mon, gPr645[PortType].pDataAddr + 1, 5 );
		AbsSetSecTime = api_CalcInTimeRelativeSec((TRealTimer*)&TmpRealTimer);//���㵱ǰʱ���������
	
		//���ǰ��5�����ڲ�����㲥Уʱ
		if( ((RealTimer.Hour == 23) && (RealTimer.Min >= 55)) ||((RealTimer.Hour == 0) && (RealTimer.Min < 5 )))
		{
			wResult = DLT645_ERR_DATA_01;
		}
		//������ǰ��5���Ӳ���Уʱ �·�ʱ������ʵʱ�䲻����5����
		else if( (JudgeBroadcastMeterTime( 5*60 ) == TRUE) && (labs(AbsMeterSecTime-AbsSetSecTime) < (5*60)) )
		{
			wResult = DLT645_OK_00;
		}
		
		//Уʱ����
		if( wResult == DLT645_OK_00 )
		{
			if( api_WriteMeterTime(&TmpRealTimer) == TRUE )
			{
				api_SetRunHardFlag( eRUN_HARD_ALREADY_BROADCAST_FLAG );
				wResult = DLT645_OK_00;

				//��¼645�㲥Уʱ�����¼�
				api_WriteFreeEvent(EVENT_BROADCAST_WRITE_TIME, 0x0645);
			}
			else
			{
				wResult = DLT645_ERR_DATA_01;
			}
		}
	}

	//�жϷ���
	if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
	{
        return DLT645_NO_RESPONSE;
	}
	else
	{
		return wResult;
	}
}
//---------------------------------------------------------------
//��������: �๦�ܶ�������
//
//����: 	eSERIAL_TYPE PortType
//                   
//����ֵ: 
//			DLT645_OK_00 - ��ȷ����
//��ע:   
//---------------------------------------------------------------
WORD MultiFunPortCtrl_645( eSERIAL_TYPE PortType )
{
	if (gPr645[PortType].pDataAddr[0]<3)//���ö๦�ܶ��ӣ�00-ʱ�������壬01-�������ڣ�02-ʱ��Ͷ��
	{
	    api_MultiFunPortSet( gPr645[PortType].pDataAddr[0] );
	
		if( gPr645[PortType].eAddrType == eTYPE_ADDR_TYPE_99 )
		{
			return DLT645_NO_RESPONSE;
		}
		else
		{
			memcpy( DLT645Sendbuf[PortType].DataBuf, (BYTE *)&GlobalVariable.g_byMultiFunPortType, 1 );
			return 0x01;
		}
	}
	else
	{
		return DLT645_ERR_DATA_01;
	}
	
}
	
//---------------------------------------------------------------
//��������: ����Ԥ����
//
//����:      eSERIAL_TYPE PortType[in]
//
//����ֵ:
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
//
//��ע:   
//---------------------------------------------------------------
WORD DealProcPrePay( eSERIAL_TYPE PortType )
{
	WORD ReturnLen = 0;
	ReturnLen = api_ProcProPrePay(PortType, gPr645[PortType].pMessageAddr, DLT645Sendbuf[PortType].DataBuf );
	
	return ReturnLen;
}

//--------------------------------------------------
//��������:  645_07Ӧ�ò㺯��
//
//����:      eSERIAL_TYPE PortType[in]
//
//����ֵ:
//			DLT645_OK_00 - ��ȷ����
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
//
//��ע����:  ��
//--------------------------------------------------
static WORD DealDLT645_2007( eSERIAL_TYPE PortType )
{
	WORD wResult;

	wResult = DLT645_ERR_ID_02;
	
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_07 )
	{
		return 0xffff;
	}
	switch ( gPr645[PortType].byReceiveControl )
	{
		case eCONTROL_RECE_03:
			wResult = DealProcPrePay(PortType);
			break;
		case eCONTROL_RECE_08:
			wResult = ReviseTime0X08(PortType);
			break;
		case eCONTROL_RECE_11:
			wResult = ReadData0X11(PortType);
			break;
		case eCONTROL_RECE_12:
			wResult = ReadContinueData0X12(PortType);
			break;
		case eCONTROL_RECE_13:
			wResult = DoDlt645_2007ReadCommAddr0X13(PortType);
			break;	
		case eCONTROL_RECE_14:
			wResult = DealSetDlt645_2007Data0X14(PortType);
			break;
		case eCONTROL_RECE_15:
			wResult = DLT645_ERR_ID_02;//DoDlt645_2007WriteCommAddr0X15(PortType);
			break;
		case eCONTROL_RECE_19://��������
			wResult = ProClrCurrentMaxDemand( PortType );
			break;
		case eCONTROL_RECE_1A://�������
			wResult = ProClrMeter( PortType, 0xAA );
			break;
		case eCONTROL_RECE_1B://�¼�����   ��֧������
			wResult = ProClrMeter( PortType, 0x55 );
			break;
		case eCONTROL_RECE_1C:
			wResult = ProcRemoteRelayCommand645(PortType);
			break;
		case eCONTROL_RECE_1D:
			if ((gPr645[PortType].dwDataID.d > 0x0F0F0F00) && (gPr645[PortType].dwDataID.d < 0x0F0F0F04)) // ��������
			{
				wResult = ProIap(PortType);
			}
			else if ((gPr645[PortType].wDataLen == 1) && (gPr645[PortType].pDataAddr[0] < eMAX_OUTPUT)) // �๦�ܶ�������
			{
				wResult = MultiFunPortCtrl_645(PortType);
			}
			break;
		default:
			break;
	}
	return wResult;
}
//--------------------------------------------------
//��������:  ������չӦ�ò㺯��
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:    
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//         
//��ע����:  ��
//--------------------------------------------------
static WORD DealDLT645_Factory( eSERIAL_TYPE PortType )
{
	WORD wResult;
	       
	wResult = 0;
	if( gPr645[PortType].eProtocolType != ePROTOCOL_DLT645_FACTORY )
	{
		return 0xffff;
	}
	
	switch( gPr645[PortType].byReceiveControl )
	{
		case eCONTROL_RECE_11:			
			wResult = ReadFactoryExtPro( gPr645[PortType].pMessageAddr + 14 );
			if( wResult != 0 )
			{
				memcpy( DLT645Sendbuf[PortType].DataBuf, gPr645[PortType].pMessageAddr + 14, wResult );
			}
			else
			{
				wResult = DLT645_ERR_DATA_01;
			}
			break;
		case eCONTROL_RECE_14:
			wResult = WriteFactoryExtPro( gPr645[PortType].pMessageAddr + 22 );
			if( wResult == 0 )
			{
				wResult = DLT645_ERR_DATA_01;
			}
			else if( wResult > 0x8000 )
			{
				return wResult;//�õ�ʧ��ԭ��
			}
			else
			{
				DLT645Sendbuf[PortType].DataBuf[0] = 0x00;
				wResult = 1;
			}
			break;
		default:
			break;
	}
	
	return wResult;	
}

//--------------------------------------------------
//��������:  645Ӧ�ò㺯��
//         
//����:      TYPE_PORT PortType[in]
//         
//����ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - ���ݳ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD DLT645APPPro(eSERIAL_TYPE PortType)
{
	WORD wResult;
	
	DLT645Sendbuf[PortType].DataBuf = AllocResponseBuf( PortType, ePROTOCO_645 );
    wResult = Proc645FrameData( PortType );
    //�������������ϲ�Proc645FrameData�д����ҷ���FALSE��������������жϣ������ٴ���
	if( (wResult == DLT645_OK_00) && (DLT645APPFollow[PortType].FollowFlag != eNORMAL_FOLLOW) )//�������������Ƕ�ȡ�������ɼ�¼
	{
    	if(gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_07 )
    	{
    		wResult = DealDLT645_2007(PortType);//�������� DealDLT645_2007
    	}
    	else if( gPr645[PortType].eProtocolType == ePROTOCOL_DLT645_FACTORY )
    	{
    		wResult = DealDLT645_Factory(PortType);//�������� DealDLT645_Factory
    	}
    	else
    	{
    		//����������Լ	
    	}
	}

	return wResult;
}

//--------------------------------------------------
//��������:  645��Լ����������
//         
//����:      TYPE_PORT PortType[in]
//         
//           BYTE *pBuf[in]
//         
//����ֵ:    WORD
//         
//��ע����:  ��
//--------------------------------------------------
void ProMessage_Dlt645( TSerial *p )
{
	WORD wResult;
	BYTE i;
	
	for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
	{
		if( p == &Serial[i] )
		{
        	Proc645LinkDataRequest((eSERIAL_TYPE)i, Serial[i].ProBuf);    //��·�����ݴ���
        	wResult = DLT645APPPro((eSERIAL_TYPE)i);                      //Ӧ�ò����ݽ���
        	Proc645LinkDataResponse( (eSERIAL_TYPE)i, wResult );          //��·��������֡
			break;
		}
	}
}
//-----------------------------------------------
// ��������  : 645��ԼԤ����
//
// ����  :    TSerial *p[in]
//
// ����ֵ:     BOOL ��TRUE-��Ҫ��ת�����߱������鴦��
// ��ע����  :  ��
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p)
{
	BYTE i, jlNo, byGy;
	BOOL bRc = FALSE;
	WORD wLen;

	#if(SEL_F415 == YES)
	for (i = 0; i < MAX_COM_CHANNEL_NUM; i++)
	{
		if (p == &Serial[i])
		{
			if (i == eRS485_I)
			{
				if (IsNeedTransmit(i, PRO_NO_SPECIAL, Serial[i].ProBuf, &jlNo))
				{
					JLWrite(jlNo, Serial[i].ProBuf, (DWORD)((Serial[i].RXWPoint + MAX_PRO_BUF - Serial[i].BeginPosDlt645) % MAX_PRO_BUF), PRO_NO_SPECIAL, i);
					bRc = TRUE;
				}
				break;
			}
			else if(i == ePT_UART1_F415)// ePT_UART1_F415
			{
				if (IsJLing(i, &byGy, &jlNo))
				{
					if (jlNo == eRS485_I)
					{
						Serial[i].RXWPoint = (Serial[i].RXWPoint + MAX_PRO_BUF - Serial[i].BeginPosDlt645) % MAX_PRO_BUF;
						memset((void *)Serial[jlNo].ProBuf, 0xfe, 4);
						memcpy((Serial[jlNo].ProBuf + 4), (void *)Serial[i].ProBuf, Serial[i].RXWPoint);

						//ʹ�÷��ظ���λ���ı����У�ͨ�ŵ�ַ����λ�����ĵ�ַ��һ��
						memcpy((Serial[jlNo].ProBuf + 5), (BYTE *)&Address_645[0], 6);

						Serial[jlNo].SendLength = (Serial[i].RXWPoint + 4);
						Serial[jlNo].TXPoint = 0;

						// ���¼���֡У��
						wLen = Serial[jlNo].SendLength ;
						Serial[jlNo].ProBuf[wLen-2] = lib_CheckSum( (BYTE *)&(Serial[jlNo].ProBuf[4]), wLen-6);
						// ִ����ʱ
						//	Serial[jlNo].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
						SerialMap[jlNo].SCIDisableRcv(SerialMap[jlNo].SCI_Ph_Num);
						SerialMap[jlNo].SCIEnableSend(SerialMap[jlNo].SCI_Ph_Num);
						bRc = TRUE;
					}
				}
				else
				{
					//415�ڲ��ظ�6025
					bRc = GyRxMonitor(i, PRO_NO_SPECIAL);
				}
				break;
			}
		}
	}
	// 2������ת��ʱ��/�ڲɼ���ģʽ�¶Ե�ַ����ȷ�ı��Ľ�������
	if (bRc == FALSE)
	{
		if (i == eRS485_I)
		{
			if (IsWorkInCllMode())
			{
				// 2.1 ��ַ���ԣ���������մ�����
				if (JudgeBroadcastMeterNo(&p->ProBuf[1]) == FALSE)
				{
					if (JudgeRecMeterNo_645(&p->ProBuf[1]) == FALSE)
					{
						bRc = TRUE;
					}
				}
			}
		}
	}
	#endif//#if(SEL_F415 == YES)
	return bRc;
}
#endif//#if( SEL_DLT645_2007 == YES )

