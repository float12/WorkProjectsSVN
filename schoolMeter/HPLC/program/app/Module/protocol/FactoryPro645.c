//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.23
//����		���ڹ��ܣ�����Լ����
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



//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

static const WORD FactoryOI_Table[] = 
{
   0x0201,//����ģʽ
   0x0206,//�ͼ�����ģʽ
   0x0205,//������λ
   0x0C00,//�����̵���
   0x0C01,//lcdС�����������Ʋ���
   0x0C02,//Һ������
   0x0C03,//Һ��err����
};


//-----------------------------------------------
//��������: ��չ��Լ������
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						
//����ֵ:	���ض�ȡ���ݳ���
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��:����ʱ��04dfdfdb������չ4���ֽڣ�����ʱ����������չ8���ֽڣ���չ�ֽ����¼����չ��ʶ����
//	��1,2�ֽ�Ϊ������չ���ݱ�ʶ���ֱ��ӦDI0��DI1.
//	��3,4�ֽڹ̶�Ϊ0xdf��0xdb��
//	��5,6,7,8�ֽ�Ϊ��ǰʱ����00��1��1��0ʱ0�ֵ���Է�������hex�룬����ʱ���ں󣬵���ǰ��������ʱ�޴����ݣ���
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf)
{
	BYTE i,j,k;
	BYTE *p;
	WORD tw,wLen,wReturnLen;
	DWORD td;
	WORD Result;
	WORD Buf[10];

	j = 0;
	k = 0;
	if( (pBuf[2]!=0xdf) || (pBuf[3]!=0xdb) )
	{
		return 0;
	}
	
	p = pBuf + 2;
	tw = pBuf[0];
	wLen = 0;
	//��������Ҫ��������ֽڵ���չ���ݱ�ʶ
	wReturnLen = 0;
		
	switch(pBuf[1])
	{
		case 0x00://�����¼���¼		
		case 0x01://�쳣�¼���¼			
			if(tw == 0)
			{
				return 0;
			}
			
			for(i=1;i<=6;i++)
			{
				if( pBuf[1] == 0x00 )//�����¼�
				{
					wLen = api_ReadFreeEvent((tw-1)*6+i,p);
				}
				else
				{
					wLen = api_ReadSysUNMsg((tw-1)*6+i,p);
				}
				
				p += wLen;
				wReturnLen += wLen;
			}
			
			wReturnLen += 2;
			break;
			
		case 0x02:
			if( pBuf[0] == 0x01 )//����ģʽ
			{
				if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == TRUE )
				{
					pBuf[2] = 0x01;
				}
				else
				{
					pBuf[2] = 0x00;
				}
				
				wReturnLen = 3;
			}
			else if( pBuf[0] == 0x03 )//�����Ƿ񲹳���� 0-������ 1-����
			{
				if( api_GetProHardFlag( ePRO_HARD_PHASE_HEAT ) == TRUE )
				{
					pBuf[2] = 0x01;
				}
				else
				{
					pBuf[2] = 0x00;
				}
				
				wReturnLen = 3;
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if(pBuf[0] == 0x04)
			{
				//���н��ݽ�������Ч������ҽ��ݵ�۷�������£����ݵ����ο۷ѣ�0--���۷ѣ�1-���½��ݿ۷Ѳ���ÿ�µ�1������ת������õ���
				if( api_GetProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) == TRUE )
				{
					pBuf[2] = 0x01;
				}
				else
				{
					pBuf[2] = 0x00;
				}
				
				wReturnLen = 3;
			}
			#endif
			else if( pBuf[0] == 0x05 ) //�������λ�Ƿ�Ҫ��λ 0����λ 1������λ����һλ����0.5��Ҳ��λ��1��
			{
				if( api_GetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) == TRUE )
				{
					pBuf[2] = 0x01;
				}
				else
				{
					pBuf[2] = 0x00;
				}

				wReturnLen = 3;
			}
			break;
		case 0x03://���ڱ����洢��
			if( pBuf[0] == 0x01 )
			{
				wLen = pBuf[5];
				tw = pBuf[4] + wLen;//pBuf[5];
				if(tw > GET_STRUCT_MEM_LEN(TConMem,FactoryRcvArea))
				{
					break;
				}
				if( api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( FactoryRcvArea ) + pBuf[4], wLen, p ) == TRUE )
				{
					wReturnLen = wLen + 2;
				}
			}
			else if(pBuf[0] == 0x03)
			{
				//��ȡ���ܵ�ַ�����Ե���ת��ʹ��
				wReturnLen = api_GetFactoryEnergyPara(p);
				wReturnLen += 2;
			}
			break;
		case 0x04:
			if(( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//δ�򿪳���ģʽ
			&&( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == FALSE ))//δ�򿪵ͼ�����ģʽ
			{
				break;
			}
			
			memcpy((BYTE*)&td,pBuf+4,4);
			i = pBuf[8];
			
			if( (pBuf[0]==0x00) || (pBuf[0]==0x01) )
			{
				//���ڲ�ram���ڲ�flash
				wReturnLen = api_ReadCpuRamAndInFlash( pBuf[0], td, i, p ) + 2;
				
				#if ( CPU_TYPE == CPU_HT6025 )
				//�·����Ȳ�����40���������ж���
				if(i != 40)
				{
					break;
				}
				//�������ͼ�����ģʽ�£���CPU�汾ʱ���һ��InfoFlash��RTCϵ��
				if( (td == 0x00040104) && (api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == FALSE) )
				{
					//����һ�����ݣ�ʹ֮��TAB_DFx_K��Ӧ
					for (j = 0; j < 10; j++)
					{
						Buf[j] = (pBuf[j*4 + 3] << 8) + pBuf[j*4+2];
					}
					//�����constֵ�Ƿ�һ�£����Ƚ�Aϵ��
					if(memcmp((BYTE *)&Buf[2], (BYTE *)&TAB_DFx_K[2], 16) != 0)
					{
						//��һ�·���ȫ0
						memset(p, 0x00, i);
					}
				}
				#endif
			}
			else if(pBuf[0] == 0x02)//������о��һƬEEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x01, td, i, p ) + 2;
			
				if(api_CheckEEPRomAddr(1) == FALSE)//���E2��ַ
				{
					wReturnLen = 0;
				}
			}
			else if(pBuf[0] == 0x03)//������о�ڶ�ƬEEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x02, td, i, p ) + 2;
				
				if(api_CheckEEPRomAddr(2) == FALSE)//���E2��ַ
				{
					wReturnLen = 0;
				}
			}
			else if(pBuf[0] == 0x04)//������оFLASH
			{
				wReturnLen = api_ReadSaveMem( 0x11, td, i, p ) + 2;
			}
			else if(pBuf[0]==0x06)
			{
				//�������ռ�
				wReturnLen = api_ReadSaveMem( 0x31, td, i, p ) + 2;
			}
			break;
		case 0x06://��ȡ����ϵ��
			memcpy((BYTE*)&tw,pBuf+4,2);
			i = pBuf[6];
			if(i > 200)
			{
				return 0;
			}
			if( api_ProcSampleCorr( READ, tw, i, p ) == TRUE )
			{
				wReturnLen = i + 2;
			}
			break;
		case 0x08://�����汾
			if(pBuf[0] == 0xdb)
			{
				wReturnLen = api_ReadMeterVersion(p);
				wReturnLen += 2;
			}
			break;
		#if(RTC_CHIP == RTC_HT_INCLOCK)
		case 0x09:
			if(pBuf[0] == 0xff)
			{
				wReturnLen = api_ProcRtcPara(READ, p);
				wReturnLen += 2;
			}
			else if( pBuf[0] == 0xAA )
			{
				ProcInfoRtcAdjustPara( READ, p );
				wReturnLen = 22;
			}
			break;
		#endif
		case 0x0c:
			// 0 -- �����̵���
			// 1 -- LCDС�����������Ʋ���
			// 2 -- Һ������
			// 3 -- Һ��ERR��ʾ����
			if(pBuf[0] < 0x04)
			{
				api_ProcLcdCtrlPara(READ,pBuf[0],8,p);
				wReturnLen = 10;
			}
			break;
		case 0x0D://��ȡ��ǡ���ť��״̬ 0�۸� 1�ϸǣ������ڶ��ֽڶ�ť�ǹ̶���0��
			if(pBuf[0] == 0x00)
			{
				memset( p, 0x00, 2 );
				if( UP_COVER_IS_OPEN )//�ϸǿ���
				{
					p[0] = 1;
				}

				#if( MAX_PHASE != 1)
				if( END_COVER_IS_OPEN )//��Ŧ�ǿ���
				{
					p[1] = 1;
				}
				#endif
				wReturnLen = 2+2;
			}
			break;
		case 0x0e://��ȡ����
			if( (pBuf[0]==0x00) || (pBuf[0]==0x01) || (pBuf[0]==0x02) || (pBuf[0]==0x03) )
			{
				Result = api_GetRemoteData(PHASE_ALL+(pBuf[0]*0x1000)+REMOTE_P, DATA_BCD, 6, 4, p );
				if( Result == FALSE )
				{
					return 0;
				}
				else
				{
					wReturnLen = 4+2;
				}
			}
			else if( pBuf[0] == 0xff )
			{
				#if( MAX_PHASE != 1)
				for( i=0; i< 4; i++)
				#else
				for( i=0; i< 2; i++)
				#endif
				{
					Result = api_GetRemoteData(PHASE_ALL+(i*0x1000)+REMOTE_P, DATA_BCD, 6, 4, p );
					if( Result == FALSE )
					{
						return 0;
					}
					p = p+4;
				}
				
				wReturnLen = ( 4*i + 2 );
			}
			break;

		case 0x0f:
			if(pBuf[0] == 0x00)//��Դ�쳣���
			{
				#if(MAX_PHASE == 1)
	//			text[0] = HT_TMR4->TMRCNT;
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
	//			text[1] = HT_TMR4->TMRCNT;
				p[0] = j;
				p[1] = 0;
				wReturnLen = 2+2;
				#else
				return 0;
				#endif
			}
			else if( pBuf[0] == 0x01 ) //�����籣���Ƿ�����
			{
				p[0] = api_GetPowerDownCompleteFlag();
				wReturnLen = 2+1;
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if( pBuf[0] == 0x02 )//esam�汾���
			{
			   if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == TRUE ) 
			   {
                    p[0] = api_TestEsamCard( );
			   }
			   else
			   {
                    p[0] = FALSE;
			   }
               
               wReturnLen = 2+1;
			}
			#endif
			else if( pBuf[0] == 0x04)//��������
			{
				p[0] = SelOscTypeConst;
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 6 )//����һƬE2��ַ
			{
				api_GetEEPRomAddr( 0, p );
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 7 )//���ڶ�ƬE2��ַ
			{
				api_GetEEPRomAddr( 1, p );
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 8 )//����
			{
				#if( MAX_PHASE != 1)
				for( i=0; i< 3; i++)
				#else
				for( i=0; i< 1; i++)
				#endif
				{
					Result = api_GetRemoteData(PHASE_A+(i*0x1000)+REMOTE_I, DATA_BCD, 4, 4, p );
					if( Result == FALSE )
					{
						return 0;
					}
					p = p+4;
				}
				
				wReturnLen = ( 4*i + 2 );
			}
			else if( pBuf[0] == 9 )//EE�汾
			{
				wReturnLen = sizeof(SelEESoftAddrConst);
				memcpy( p, (BYTE*)&SelEESoftAddrConst, wReturnLen );
				wReturnLen += 2;
			}
			#if( MAX_PHASE == 3 )
			else if( pBuf[0] == 0x0D )//����OAD��Ӧ�ĵ�ַ
			{
				tw = pBuf[4];
				wReturnLen = (WORD)api_GetDefaultConstParaAdder(tw, p);
				wReturnLen += 2;
			}
			#endif
			break;
		case 0x10:
			if(pBuf[0] == 0x00)//���ڴ�����Ϣ��
			{
				memset( p, 0x00, 8 );
				for(i = 0; i < 64; i++)
				{
					if(api_CheckError( i ) == TRUE)
					{
						p[i / 8] |= (0x01 << (i % 8));
					}
				}
				wReturnLen = 2 + 8;
			}
			break;
			
		default:
			break;
	}
	
	return wReturnLen;
}


//-----------------------------------------------
//��������: ��չ��Լд����
//
//����:		pBuf[in]	0--��չDI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--��Է�����
//����ֵ:	����ֵbit15Ϊ1Ϊ��������Ϊ��ȷ
//		   
//��ע:��Լ����������ƽ̨��չ��Լ��
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf)
{
	DWORD tdw;
	WORD tw,wReturnLen,OI;
	BYTE i;
    BOOL Result; 
   
	if( (pBuf[2]!=0xdf) || (pBuf[3]!=0xdb) )
	{
		return 0;
	}
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//δ�򿪳���ģʽ
	{
		memcpy( (BYTE*)&OI, pBuf, sizeof(WORD) );
		
		if( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == TRUE )//�򿪵ͼ�����ģʽ
		{
			if( (OI == 0x0201 )//����ģʽ
			|| (OI == 0x0206 )//�ͼ�����ģʽ
			|| (OI == 0x0205 )//������λ
			|| (OI == 0x0C00 )//�����̵���
			|| (OI == 0x0C01 )//lcdС�����������Ʋ���
			|| (OI == 0x0C02 )//Һ������
			|| (OI == 0x0C03 )//Һ��err����
			|| (OI == 0x0603 )//ֱ��ż��г���ж�ʱ��
			)
			{
				//����ִ��
			}
			else
			{
				return 0x8001;
			}
		}
		else
		{
			if( (OI != 0x0201 ) && (OI != 0x0206 ))//ֻ����򿪳���ģʽ�͵ͼ�����ģʽ
			{
				return 0x8001;
			}
		}
	}
	
	//�ж�ʱ��
	memcpy( (BYTE*)&tdw, pBuf+4, 4 );
	// !!!!!! zh����
	// if( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
	// {
		//ʱ����󣬲��ж�ʱ��	
	// }
	// else
	// {
	// 	if( ((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) || (UP_COVER_IS_OPEN)) || (FactoryTime != 0) )
	// 	{
			//��Կ�ж�ȫ8ʱ������ڣ���ǰʱ��~��ǰʱ��+10080��������
	// 		if(tdw != 0x88888888L)//0x054c5638
	// 		{
	// 			if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
	// 			{
	// 				return 0x8001;
	// 			}			
	// 		}
	// 	}
	// 	else
	// 	{
			//˽Կ�жϣ���ǰʱ��~��ǰʱ��+10080��������
	// 		if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
	// 		{
	// 			return 0x8001;
	// 		}
	// 	}
	// }
	
	//���ڹ�Լ��չд���������¼�
	api_WriteFreeEvent( EVENT_FACTORY_WRITE, (pBuf[1] << 8) + pBuf[0] );
	
	wReturnLen = 0;
	
	switch(pBuf[1])
	{
		case 0x02:
			if(pBuf[0] == 0x01)
			{
				//����ģʽ
				if(pBuf[8] == 0x01)
				{
					// //���� �� ��Կ �� ����ʱ�䲻Ϊ��
					// !!!!!!zh
					// if( (UP_COVER_IS_OPEN) 
					// 	|| (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) 
					// 	|| (FactoryTime != 0) )
					{
						api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
						
						api_WriteFreeEvent( EVENT_OPEN_FAC_MODE, FactoryTime );
						wReturnLen = 1;
					}
				}
				else
				{
					api_ClrSysStatus(eSYS_STATUS_INSIDE_FACTORY);
					wReturnLen = 1;
				}
			}
			else if(pBuf[0] == 0x03)
			{
				//�����Ƿ񲹳���� 0-������ 1-����
				if(pBuf[8] == 0x01)
				{
					if( api_SetProHardFlag( ePRO_HARD_PHASE_HEAT ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
				else
				{
					if( api_ClrProHardFlag( ePRO_HARD_PHASE_HEAT ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if(pBuf[0] == 0x04)
			{
				//���н��ݽ�������Ч������ҽ��ݵ�۷�������£����ݵ����ο۷ѣ�0--���۷ѣ�1-���½��ݿ۷Ѳ���ÿ�µ�1������ת������õ���
				if(pBuf[8] == 0x01)
				{
					if( api_SetProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
				else
				{
					if( api_ClrProHardFlag( ePRO_HARD_LADDER_SAVE_PARA_INVALID_DEAL ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
			}
			#endif
			else if( pBuf[0] == 0x05 )
			{
				//�������λ�Ƿ�Ҫ��λ 0����λ 1������λ����һλ����0.5��Ҳ��λ��1��
				if( pBuf[8] == 0x01 )
				{
					if( api_SetProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
				else
				{
					if( api_ClrProHardFlag( ePRO_HARD_DEMAND_DATA_CARRY ) != TRUE )
					{
						break;
					}
					wReturnLen = 1;
				}
			}
			else if( pBuf[0] == 0x06 )
			{
				//�͵ȼ�����ģʽ-��ʱʱ��30min
				if( pBuf[8] == 0x01 )
				{
					api_SetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
					LowFactoryTime = (30*60);//30����ʱЧ
					wReturnLen = 1;
				}
				else
				{
					api_ClrSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
					LowFactoryTime = 0;
					wReturnLen = 1;
				}
			}
			else
			{}
			break;
		case 0x03://д���ڱ�����
			if(pBuf[0] != 0x01)
			{
				break;
			}
			//ƫ��+����
			tw = pBuf[8] + pBuf[9];
			if(tw > GET_STRUCT_MEM_LEN(TConMem,FactoryRcvArea) )
			{
				break;
			}
			if( api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( FactoryRcvArea ) + pBuf[8], pBuf[9], pBuf + 10 ) != TRUE )
			{
				break;
			}
			wReturnLen = 1;
			break;
		case 0x06:
			if(pBuf[0] == 0x01)//д����ϵ����
			{
				memcpy( (BYTE*)&tw, pBuf+8, 2 );
				if( api_ProcSampleCorr( WRITE, tw, pBuf[10], pBuf + 12 ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x02)//У��
			{
				//AA  HF����  AA  A��˲ʱ��  AA  B��˲ʱ��  AA  C��˲ʱ��  AA
				if( api_ProcSampleAdjust( pBuf[9], pBuf + 10 ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if( pBuf[0] == 0x03 )//ֱ��ż��г��ʱ�����
			{
				memcpy( (BYTE*)&tw, pBuf+8, 2 );
				if( tw != 0xA13F )
				{
					break;
				}
				if( api_ProcSampleCorr( WRITE, tw, pBuf[10], pBuf + 12 ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			break;
		case 0x07:
			if(pBuf[0] == 0x00)
			{
				//���λ����
				wReturnLen = 1;
				api_PowerDownEnergy();         // �������ת�洦��
				Reset_CPU();				
			}
			else if(pBuf[0] == 0xdb)
			{
				//������㼰����ʼ��
				//ClearMeter();			//0X1111
				//api_FactoryInitTask();	//0X2222
				api_SetClearFlag( eCLEAR_FACTORY, 2 );
				wReturnLen = 1;
			}
			else if( pBuf[0] == 0xdf )
			{
				api_SetClearFlag( eCLEAR_PARA, 2 );
				wReturnLen = 1;
			}
			break;
		#if(RTC_CHIP == RTC_HT_INCLOCK)	
		case 0x09:
			if(pBuf[0] == 0x00)//RTC���µ�Уֵ
			{
				memcpy( (BYTE*)&tw, pBuf+8, 2 );
				if( api_CaliRtcError( tw ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0xAA)//дInfoFlash��RTC��A��B��C��D��EУ��ϵ��
			{
				ProcInfoRtcAdjustPara( WRITE, pBuf + 8 );
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0xff)//д�ߵ��µ�Уϵ��
			{
				api_ProcRtcPara(WRITE,pBuf + 10);
				//д������У���
				WriteInfoRTCErrorCheckSum();
				wReturnLen = 1;
			}
			break;
		#endif
		case 0x0a://��������
			if(pBuf[0] == 0x00)//��ʼ������������
			{
				memcpy((BYTE*)&tdw,pBuf+8+6,4);
				if(tdw != 0xdfdb0001)
				{
					break;
				}
				
				api_WriteUpdateProgHead(pBuf+8);
				
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x01)//������������
			{
				Result = api_WriteUpdateProgMessage(pBuf+8);
				if(Result != TRUE)
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x03)//������������NEW
			{
				Result = api_WriteUpdateProgMessageNew(pBuf+8);
				if(Result != TRUE)
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x02)//��������������
			{
				memcpy((BYTE*)&tdw,pBuf+8+6,4);
				if(tdw != 0xa55a5aa5)
				{
					break;
				}
				
				//�˺��������з��أ��������и���λ����
				api_StartUpdateProgMessage(pBuf+8);
				
				wReturnLen = 1;
			}
			break;
		case 0x0c:
			// 0 -- �����̵���
			// 1 -- LCDС�����������Ʋ���
			// 2 -- Һ������
			// 3 -- Һ��ERR��ʾ����
			if(pBuf[0] < 0x04)
			{
				api_ProcLcdCtrlPara(WRITE,pBuf[0],8,pBuf+8);
                wReturnLen = 1;
			}
			break;
		case 0x0D:
			//У׼��ص�ѹ
			if(pBuf[0] == 0x01)
			{
				if( api_AdjustBatteryVoltage() != TRUE )
				{
					break;
				}

				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x02)
			{
				api_LcdQuitAllDisplay();

				wReturnLen = 1;
			}
			#if( MAX_PHASE == 1 )
			else if(pBuf[0] == 0x05)
			{
				api_CheckDispPin(pBuf[8], pBuf[9]);

				wReturnLen = 1;
			}
			#endif
			break;	
		default:
			break;
	}
	
	return wReturnLen;
}




