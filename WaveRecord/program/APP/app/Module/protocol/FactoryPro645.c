//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.9.23
//����		���ڹ��ܣ�����Լ����
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "SaveMem.h"
#include "ff.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
TFourByteUnion randtemp;
const char keys[]="DYTQ";
extern BYTE TopoErrFlag;
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------



//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//�ж��·������Ƿ���ȷ
BOOL FactoryOpen(TFourByteUnion tOpens) 
{
	if (randtemp.d==0)
	{
		return FALSE;
	}
	
    for (BYTE i = 0; i < 4; i++)
	{
		tOpens.b[i]-=randtemp.b[0];
		tOpens.b[i]+=keys[i];
	}
	if (tOpens.d==randtemp.d)
	{
		return TRUE;
	}
	return FALSE;
}

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
	DWORD td,writtenBytes = 0;
	WORD Result;
	BYTE EEandFLASH_Test[40],txbuf[2][2048],TFTestbuf[20];
	static DWORD InstanDataNo = 0;
	FRESULT FResult = FAT32_FR_OK;
	BYTE TFTestStatus = 0;//0��ʾtf�������쳣��1��ʾ����
	FIL TestFile;
	FATFS FatFs;

	j = 0;
	k = 0;
	memset(TFTestbuf,0x00,sizeof(TFTestbuf));
	memset(&TestFile,0x00,sizeof(TestFile));
	memset(&FatFs,0x00,sizeof(FatFs));

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
			}
			else if(pBuf[0] == 0x02)//������о��һƬEEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x01, td, i, p ) + 2;
			
				if(SelEESoftAddrConst == YES)
				{
					if(api_CheckEEPRomAddr(1) == FALSE)//���E2��ַ
					{

						wReturnLen = 0;
					}
				}
			}
			else if(pBuf[0] == 0x03)//������о�ڶ�ƬEEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x02, td, i, p ) + 2;
				if(SelEESoftAddrConst == YES)
				{
					if(api_CheckEEPRomAddr(2) == FALSE)//���E2��ַ
					{

						wReturnLen = 0;
					}
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
		// case 0x0c:
		// 	// 0 -- �����̵���
		// 	// 1 -- LCDС�����������Ʋ���
		// 	// 2 -- Һ������
		// 	// 3 -- Һ��ERR��ʾ����
		// 	if(pBuf[0] < 0x04)
		// 	{
		// 		api_ProcLcdCtrlPara(READ,pBuf[0],8,p);
		// 		wReturnLen = 10;
		// 	}
		// 	break;
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
		case 0x11://��ҵ�����
			#if(SEL_EVENT_DI_CHANGE == YES)
			if(pBuf[0] == 0x23)//��ȡʵʱң��״̬
			{
				p[0] = api_ReadDIStatus();
				wReturnLen = 1;
			}
			else if( pBuf[0] == 0x50 )//ң�ŷ���ʱ��
			{
				wReturnLen = api_ReadDIPara(0,p);
			}
			else if (pBuf[0] == 0x52)
		    {
			    p[0] = 0;
			    // memset(txbuf, 0xA5, sizeof(txbuf[0]));
			    // w25nxx_block_erase((MAX_DATA_FLASH/SIZE_PAGE) + 1);
			    // api_Delayms(10);
			    // w25nxx_page_write((MAX_DATA_FLASH/SIZE_PAGE) + 1, txbuf[0]);
			    // api_Delayms(20);
			    // w25qxx_page_read((MAX_DATA_FLASH/SIZE_PAGE) + 1, txbuf[1]);
			    // api_Delayms(20);
			    // if(memcmp(&txbuf[0], &txbuf[1], 2048) != 0)
			    // {
				//     p[0] |= BIT0;
			    // }
			    // memset(EEandFLASH_Test, 0x02, 10);
			    // WriteEEPRom2(SINGLE_CHIP_SIZE - 10, 10, (BYTE *)EEandFLASH_Test);
			    // memset((BYTE *)EEandFLASH_Test + 12, 0x00, 10);
			    // ReadEEPRom2(SINGLE_CHIP_SIZE - 10, 10, (BYTE *)EEandFLASH_Test + 12);
			    // if(memcmp(EEandFLASH_Test, EEandFLASH_Test + 12, 10) != 0)
			    // {
				//     p[0] |= BIT1;
			    // }
			    if(TopoErrFlag >= 15)
			    {
				    p[0] |= BIT2;
			    }

			    wReturnLen = 1;
		    }
		    else if (pBuf[0] == 0x66)
			{
				srand(api_CalcInTimeRelativeSec( &RealTimer ));
				randtemp.d = rand();
				wReturnLen = sizeof(randtemp.d);
				memcpy(p,&randtemp.d,wReturnLen);
			}
			else if (pBuf[0] == 0x18)
			{
				p[0]=api_GetCanMod();
				wReturnLen = 1;
			}
			else if (pBuf[0] == 0x19)//�������չ�汾
			{
				tw = api_CheckCpuFlashSum(1);
				lib_ExchangeData(p,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(2);
				lib_ExchangeData(p+2,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(3);
				lib_ExchangeData(p+4,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(0xff);
				lib_ExchangeData(p+6,(BYTE*)&tw,2);
				wReturnLen = 8;
			}
			
			if (wReturnLen)
			{
				wReturnLen+=2;
			}
			#else
			if (pBuf[0] == 0x52)
		    {
			    p[0] = 0;
			    // memset(txbuf, 0xA5, sizeof(txbuf[0]));
			    // w25nxx_block_erase((MAX_DATA_FLASH/SIZE_PAGE) + 1);
			    // api_Delayms(10);
			    // w25nxx_page_write((MAX_DATA_FLASH/SIZE_PAGE) + 1, txbuf[0]);
			    // api_Delayms(20);
			    // w25qxx_page_read((MAX_DATA_FLASH/SIZE_PAGE) + 1, txbuf[1]);
			    // api_Delayms(20);
			    // if(memcmp(&txbuf[0], &txbuf[1], 2048) != 0)
			    // {
				//     p[0] |= BIT0;
			    // }
			    // memset(EEandFLASH_Test, 0x02, 10);
			    // WriteEEPRom2(SINGLE_CHIP_SIZE - 10, 10, (BYTE *)EEandFLASH_Test);
			    // memset((BYTE *)EEandFLASH_Test + 12, 0x00, 10);
			    // ReadEEPRom2(SINGLE_CHIP_SIZE - 10, 10, (BYTE *)EEandFLASH_Test + 12);
			    // if(memcmp(EEandFLASH_Test, EEandFLASH_Test + 12, 10) != 0)
			    // {
				//     p[0] |= BIT1;
			    // }
			    if(TopoErrFlag >= 15)
			    {
				    p[0] |= BIT2;
			    }

			    wReturnLen = 1;
		    }
		    else if (pBuf[0] == 0x66)
			{
				srand(api_CalcInTimeRelativeSec( &RealTimer ));
				randtemp.d = rand();
				wReturnLen = sizeof(randtemp.d);
				memcpy(p,&randtemp.d,wReturnLen);
			}
			// else if (pBuf[0] == 0x18)
			// {
			// 	p[0]=api_GetCanMod();
			// 	wReturnLen = 1;
			// }
			else if (pBuf[0] == 0x19)//�������չ�汾
			{
				tw = api_CheckCpuFlashSum(1);
				lib_ExchangeData(p,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(2);
				lib_ExchangeData(p+2,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(3);
				lib_ExchangeData(p+4,(BYTE*)&tw,2);
				tw = api_CheckCpuFlashSum(0xff);
				lib_ExchangeData(p+6,(BYTE*)&tw,2);
				wReturnLen = 8;
			}
			
			if (wReturnLen)
			{
				wReturnLen+=2;
			}
			#endif//#if(SEL_EVENT_DI_CHANGE == YES)	
			break;
		case 0x12:
			if (pBuf[0] == 0x00) // ��һ·485
			{
				p[0] = ((FPara2->CommPara.I485 & 0x60) >> 5) + 1;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x01) // �ڶ�·485
			{
				// p[0] = ((FPara2->CommPara.II485 & 0x60) >> 5) + 1;
				// wReturnLen = 3;
			}
			else if (pBuf[0] == 0x02) // �ز�
			{
				p[0] = ((FPara2->CommPara.ComModule & 0x60) >> 5) + 1;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x05) // ��һ·485 ͨ��������
			{
				p[0] = FPara2->CommPara.I485;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x06) // �ڶ�·485 ͨ��������
			{
				// p[0] = FPara2->CommPara.II485;
				// wReturnLen = 3;
			}
			else if (pBuf[0] == 0x07) // �ز�
			{
				p[0] = FPara2->CommPara.ComModule;
				wReturnLen = 3;
			}
			break;
		case 0x13://��ȡ�ϴ�˲ʱ��
			if (pBuf[0] == 0x07)
			{
	#if (MAX_PHASE != 1)
				for (i = 0; i < 3; i++)
	#else
				for (i = 0; i < 1; i++)
	#endif
				{
					Result = api_GetRemoteData(PHASE_A + (i * 0x1000) + REMOTE_U, DATA_HEX, 4, 4, p);
					if (Result == FALSE)
					{
						return 0;
					}
					p = p + 4;

					Result = api_GetRemoteData(PHASE_A + (i * 0x1000) + REMOTE_I, DATA_HEX, 4, 4, p);
					if (Result == FALSE)
					{
						return 0;
					}
					p = p + 4;

					Result = api_GetRemoteData(PHASE_A + (i * 0x1000) + REMOTE_P, DATA_HEX, 4, 4, p);
					if (Result == FALSE)
					{
						return 0;
					}
					p = p + 4;

					Result = api_GetRemoteData(PHASE_A + (i * 0x1000) + REMOTE_Q, DATA_HEX, 4, 4, p);
					if (Result == FALSE)
					{
						return 0;
					}
					p = p + 4;
					// ����г������
					memcpy(p, &Cur_harm_content[i][0], UPLOIAD_HARM_NUM * sizeof(float));
					p = p + UPLOIAD_HARM_NUM * sizeof(float);
					// UI����
					memcpy(p, &UI_Trait_data[i][0], UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float));
					p = p + UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float);
				}
				//���
				memcpy(p,&InstanDataNo, sizeof(InstanDataNo));
				InstanDataNo++;
				p = p + sizeof(InstanDataNo);
				//16Ϊ u��i��p��q
				wReturnLen = (16 * i +  sizeof(InstanDataNo)+MAX_RELAY_NUM * UPLOIAD_HARM_NUM * sizeof(float) + MAX_RELAY_NUM * UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float) + 2);
			}
			break;
		case 0x14://��ȡuik
			if (pBuf[0] == 0x07)
			{
				memcpy(p, &UIFactor,sizeof(UIFactor));
				wReturnLen = 2 + sizeof(UIFactor);
			}
			break;
		case 0x15://����tf��д�빦��
			if (pBuf[0] == 0x07)
			{
				if(isPressed == 0)//δ���°�����Ҫ����
				{
					memset(&FatFs, 0, sizeof(FatFs));
					f_mount(0, &FatFs); 
				}
				FResult = f_open(&TestFile, "test.txt", FA_OPEN_ALWAYS | FA_WRITE);
				if(FResult == FAT32_FR_OK)
				{
					FResult = f_write(&TestFile, TFTestbuf, sizeof(TFTestbuf), (UINT *)&writtenBytes);
					FResult = f_sync(&TestFile);
					if ((FResult == FAT32_FR_OK) && (writtenBytes == sizeof(TFTestbuf)))
					{
						TFTestStatus = 1;	
					}
				}
				f_close(&TestFile);
				f_unlink("test.txt");
				if(isPressed == 0)
				{
					f_mount(0, NULL); 
				}
				memcpy(p, &TFTestStatus, sizeof(TFTestStatus));
				wReturnLen = 2 + sizeof(TFTestStatus);
			}
			break;
		default:
			break;
	}
	return wReturnLen;
}

//--------------------------------------------------
// ��������:  ��Լ����ͨ����żУ��λ
//
// ����:
//	 Sub0[in]:	����ѡ��
//
//	 Sub1[in]��	������żУ��ֵ
// ����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
// ��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
// Sub0[in] 00-01 ͨ�ſ�1������������(��һ·485)    ͨ�ſ�2�����������֣��ڶ�·485��
// Sub1[in] 01��У�飻02��У�飻03żУ��
//----------------------------------------------------------
static WORD SetParabaudPari(BYTE Sub0, BYTE Sub1)
{
	BYTE RsBaud;
	WORD Result, Addr;
	BYTE i, Type;

	if (Sub0 == 0)
	{
			Type = eRS485_I;
			RsBaud = FPara2->CommPara.I485;
			Addr = GET_STRUCT_ADDR(TFPara2, CommPara.I485);
	}
	else if (Sub0 == 1)
	{
//			Type = eRS485_II;
//			RsBaud = FPara2->CommPara.II485;
//			Addr = GET_STRUCT_ADDR(TFPara2, CommPara.II485);
	}
	else if (Sub0 == 2)
	{
			Type = eCR;
			RsBaud = FPara2->CommPara.ComModule;
			Addr = GET_STRUCT_ADDR(TFPara2, CommPara.ComModule);
	}

	i = api_GetSerialIndex(Type);

	if (i >= MAX_COM_CHANNEL_NUM)
	{
			return 0x8002;
	}
	// Bps=0x40;//ǿ��żУ�飨ʵ����żУ�鶼�����ϣ�
	// Bps=0x20;//ǿ����У�飨ʵ����żУ�鶼�����ϣ�
	// Bps=0;//ǿ����У��
	RsBaud &= ~0x60; // bit5~bit6����

	RsBaud |= ((Sub1 - 1) << 5);

	Result = api_WritePara(1, Addr, 1, &RsBaud);

	if (Result != FALSE)
	{

			Serial[i].OperationFlag = 1;
			return 1;
	}
	else
	{
			return 0x8002;
	}
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
	BYTE i,j;
    BOOL Result; 
	TFourByteUnion tOpens;

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
//	if( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
//	{
//		//ʱ����󣬲��ж�ʱ��	
//	}
//	else
//	{
//		if( ((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) || (UP_COVER_IS_OPEN)) || (FactoryTime != 0) )
//		{
//			//��Կ�ж�ȫ8ʱ������ڣ���ǰʱ��~��ǰʱ��+10080��������
//			if(tdw != 0x88888888L)//0x054c5638
//			{
//				if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
//				{
//					return 0x8001;
//				}			
//			}
//		}
//		else
//		{
//			//˽Կ�жϣ���ǰʱ��~��ǰʱ��+10080��������
//			if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
//			{
//				return 0x8001;
//			}
//		}
//	}
	
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
					//���� �� ��Կ �� ����ʱ�䲻Ϊ��
					//if( (UP_COVER_IS_OPEN) 
					//	|| (api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) 
					//	|| (FactoryTime != 0) )
					//{
				    if(!api_GetRunHardFlag(eRUN_HARD_COMMON_KEY))
				    {
					    memcpy(tOpens.b, &pBuf[9], 4);
					    if(FactoryOpen(tOpens))
					    {
						    api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
						    api_WriteFreeEvent(EVENT_OPEN_FAC_MODE, FactoryTime);
						    wReturnLen = 1;
					    }
				    }
					else
				    {
					    api_SetSysStatus(eSYS_STATUS_INSIDE_FACTORY);
					    api_WriteFreeEvent(EVENT_OPEN_FAC_MODE, FactoryTime);
					    wReturnLen = 1;
				    }
				}
				else
				{
					api_ClrSysStatus(eSYS_STATUS_INSIDE_FACTORY);
					wReturnLen = 1;
				}
				randtemp.d = 0;
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
				// api_PowerDownEnergy();         // �������ת�洦��
				// Reset_CPU();				
				api_EnterLowPower(eFromOnRunEnterDownMode);
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
				//Result = api_WriteUpdateProgMessageNew(pBuf+8);
				//if(Result != TRUE)
				//{
				//	break;
				//}
				//wReturnLen = 1;
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
				//api_LcdQuitAllDisplay();

				wReturnLen = 1;
			}
			break;
		case 0x11:
			if ( pBuf[0] == 0x16)
			{
				if (pBuf[8])
				{
					setLedflag(eLedWaring,eLightAllOn);
				}
				else
				{
					setLedflag(eLedWaring,eLightAllNone);
				}
				
				wReturnLen = 1;
			}
			// else if (pBuf[0] == 0x18)
			// {
			// 	if (pBuf[8])
			// 	{
			// 		api_SetCanMod(1);
			// 	}
			// 	else
			// 	{
			// 		api_SetCanMod(0);
			// 	}
			// 	ChangeCanMod();
			// 	wReturnLen = 1;
			// }
			
			#if(SEL_EVENT_DI_CHANGE == YES)
			else if( pBuf[0] == 0x50 )//ң�ŷ���ʱ��
			{
				if(api_WriteDIPara(0,(BYTE*)&pBuf[8]) == TRUE)
				{
					wReturnLen = 1;
				}
			}	
			#endif
			break;
		case 0x12: // ��ҵ�����ò�����У��
			i = pBuf[0];
			if (i < 3) // 0-2 ͨ�ſ�1������������(��һ·485)    ͨ�ſ�2�����������֣��ڶ�·485���ز�������������
			{
				if ((pBuf[8] >= 1) && (pBuf[8] <= 3)) // 01��У�飻02��У�飻03żУ��
				{
					wReturnLen = SetParabaudPari(i, pBuf[8]);
				}
			}
			else if (i == 0x05) // ��һ·485 ͨ��������
			{
				j = api_GetSerialIndex(eRS485_I);
				if (j >= MAX_COM_CHANNEL_NUM)
				{
					return wReturnLen;
				}
				if (api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.I485), 1, &pBuf[8]) == TRUE)
				{
					Serial[j].OperationFlag = 1;
					wReturnLen = 1;
				}
			}
			else if (i == 0x06) // �ڶ�·485 ͨ��������
			{
//				j = api_GetSerialIndex(eRS485_II);
//				if (j >= MAX_COM_CHANNEL_NUM)
//				{
//					return wReturnLen;
//				}
//				if (api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.II485), 1, &pBuf[8]) == TRUE)
//				{
//					Serial[j].OperationFlag = 1;
//					wReturnLen = 1;
//				}
			}
			else if (i == 0x07) // �ز�
			{
				j = api_GetSerialIndex(eCR);
				if (j >= MAX_COM_CHANNEL_NUM)
				{
					return wReturnLen;
				}
				if (api_WritePara(1, GET_STRUCT_ADDR(TFPara2, CommPara.ComModule), 1, &pBuf[8]) == TRUE)
				{
					Serial[j].OperationFlag = 1;
					wReturnLen = 1;
				}
                api_FreshParaRamFromEeprom(1);
			}
			break;
		case 0xF0:
			if( pBuf[0] == 0x03 )//4Gģ�鸴λ����
		    {
				if(pBuf[8] == 0x01)
				{
					api_ResetModule4g();
					wReturnLen = 1;
				}
			}
			break;
		default:
			break;
	}
	
	return wReturnLen;
}




