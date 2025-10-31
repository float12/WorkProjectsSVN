//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������    ������
//��������  2017��9��6��09:23:07
//����      645��Լ��ȡ����
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "dlt645_2007.h"

#define MAX_PARA_BUF_LENTH         (MAX_TIME_SEG*3*MAX_TIME_SEG_TABLE+30)//ʱ�α�Դ���ݳ���Ϊ������ݳ���

#if( SEL_DLT645_2007 == YES )

//����Ĭ����������
extern const BYTE ProduceDate[7];
//---------------------------------------------------------------
//��������: ��ʱ����
//
//����:
//		DI0[IN] -- ���ݱ�ʶDI0
// 		Buf -- �������
//
//����ֵ: 
//      Lenth -- ��ȡ���ݳ���
//
//��ע:
//---------------------------------------------------------------
static WORD ReadParaData0001XX( BYTE DI0, BYTE *Buf )
{
	BYTE i;
	WORD Lenth,Result;
	WORD wTemp,OI;
	WORD Sel_HEX2BCD; //�Ƿ���ҪHEXתBCD
	eTYPE_COVER_DATA CoverMode;	//����ת����ʽ
	TTwoByteUnion Year;
	TFourByteUnion dwTime;
	BYTE pBuf[sizeof(TSwitchTimePara)+10];
	
	Sel_HEX2BCD = YES;
	CoverMode= eBYTE_MODE;
	Lenth = 0;
	
	switch( DI0 )
	{
		case 0x01:	//���ڼ�����
		case 0x02:	//ʱ��
		case 0x0C:	//���ڡ�����(����0����������)��ʱ�� 
			if( DI0 == 0x01 )
			{
				wTemp = DATETIME_YYMMDDWW;
			}
			else if( DI0 == 0x02 )
			{
				wTemp = DATETIME_hhmmss;
			}
			else
			{
				wTemp = DATETIME_YYMMDDWWhhmmss;
			}
			
			Lenth = api_GetRtcDateTime(wTemp, Buf );
			if( Lenth != 0 )
			{
				CoverMode = eBYTE_MODE;
			}
			break;
		#if( MAX_PHASE == 3 )	
		case 0x03:	//�����������
			Buf[0] = FPara2->EnereyDemandMode.DemandPeriod;
			Lenth = 1;
			CoverMode = eBYTE_MODE;
			break;
		case 0x04:	//����ʱ��
			Buf[0] = FPara2->EnereyDemandMode.DemandSlip;
			Lenth = 1;
			CoverMode = eBYTE_MODE;
			break;
		#endif//#if( MAX_PHASE == 3 )	
		case 0x05:	//У��������
			Buf[0] = 0x00;
			Buf[1] = 0x80;
			Sel_HEX2BCD = NO;
			Lenth = 2;
			break; 
		case 0x06:	//����ʱ�����л�ʱ��
		case 0x07:	//������ʱ���л�ʱ��
		#if( PREPAY_MODE == PREPAY_LOCAL )
		case 0x08:	//���׷�ʱ�����л�ʱ��
		case 0x09:	//���׽����л�ʱ��
		#endif   
            Sel_HEX2BCD = NO;
            
            Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof(TSwitchTimePara), pBuf);
            if( Result == FALSE )
            {
                return 0;
            }
            
            memcpy((void *)&dwTime.b,pBuf+(DI0-6)*4,4);
            if(dwTime.d == 0)
            {
                memset(Buf,0x00,5);
            }
			else if(dwTime.d == 0x99999999)
			{
				memset(Buf,0x99,5);
			}
            else
            {
                Result = api_ToAbsTime( dwTime.d, (TRealTimer *)pBuf );
                if(Result == FALSE)
                {
                    return FALSE;
                }
                api_TimeFormat698To645( (TRealTimer *)pBuf, Buf );
            }
            
            Lenth = 5;
			break;
			
        default: 
            Lenth = 0;
	    break;
	}
	
	//ͳһ����HEXתBCD
	if( Sel_HEX2BCD == YES )
	{
		//�����Ƿ���ȷ ��ʹ����ҲҪ���ش�������
		ProBinToBCD( CoverMode, Lenth, Buf );
	}
	
    return Lenth;
}

//-----------------------------------------------
//--------------------------------------------------
//��������:  ��ȡ������
//����	:
//			pBuf[out]
//����ֵ:	TRUE	FALSE
//ע:����ʱ������ΪHEX
//--------------------------------------------------
WORD ReadSettlementDate645(BYTE DI0,BYTE *pBuf)
{		
	WORD Result;
    TBillingPara BillingParaBuf;

	Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE*)&BillingParaBuf);
	if( Result == FALSE )
	{
		return FALSE;
	}
	else
	{
		if((DI0<=(MAX_MON_CLOSING_NUM))&&(DI0 > 0))
		{
			*(pBuf++) = BillingParaBuf.MonSavePara[DI0-1].Day;
			*(pBuf++) = BillingParaBuf.MonSavePara[DI0-1].Hour;
		}
		else
		{
			return DLT645_ERR_ID_02;
		}
	
		return TRUE;
	}
}
#if(PREPAY_MODE == PREPAY_LOCAL)
//---------------------------------------------------------------
//��������: �����׷��ʵ�ۡ����׽��ݵ��
//
//����:
//		DI2[IN] -- ���ݱ�ʶDI2
//		DI1[IN] -- ���ݱ�ʶDI1
//		DI0[IN] -- ���ݱ�ʶDI0
// 		Buf -- �������
//
//����ֵ: 
//      Lenth -- ��ȡ���ݳ���
//
//��ע:
//---------------------------------------------------------------
static WORD ReadPriceParaData645( BYTE DI2,BYTE DI1,BYTE DI0, BYTE *Buf )
{
	BYTE i;
	WORD Lenth;
	TRatePrice				RatePrice;		//���ʵ�۱�
    TLadderPrice			LadderPrice;	//���ݵ�۱�
    TRatePrice   			RatePriceBCD;	
	TLadderPrice			LadderPriceBCD;
    Lenth = 0;
    
	if( DI2 == 0x05 )//���ʵ��
	{
		if( DI1 == 0x01 )
		{
			api_ReadPrePayPara(eCurRateTable, (BYTE*)RatePrice.Price);
		}
		else
		{
			api_ReadPrePayPara(eBackupRateTable, (BYTE*)RatePrice.Price);
		}

		for(i=0;i<(FPara1->TimeZoneSegPara.Ratio);i++)
		{
			RatePriceBCD.Price[i] = lib_DWBinToBCD(RatePrice.Price[i]);
		}
							
		if( DI0 == 0xff )
		{
			memcpy( Buf, (BYTE*)RatePriceBCD.Price, 4*(FPara1->TimeZoneSegPara.Ratio) );
			Lenth += (4*(FPara1->TimeZoneSegPara.Ratio));
		}
		else
		{	
			if( DI0 > (FPara1->TimeZoneSegPara.Ratio) )
			{
				return Lenth;
			}
			memcpy( Buf, (BYTE*)&RatePriceBCD.Price[DI0-1], 4 );
			Lenth += 4;
		}
	}
	else if( DI2 == 0x06 )//���ݵ��
	{
		if( DI1 == 0x04)
		{
			api_ReadPrePayPara(eCurLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
		}
		else
		{
			api_ReadPrePayPara(eBackupLadderTable, (BYTE*)LadderPrice.Ladder_Dl );
		}

		//����ۡ�����ֵ���������ת��ΪBCD��ʽ
		for(i=0;i<(MAX_LADDER);i++)
		{
			LadderPriceBCD.Ladder_Dl[i] = lib_DWBinToBCD(LadderPrice.Ladder_Dl[i]);
		}
		for(i=0;i<(MAX_LADDER+1);i++)
		{
			LadderPriceBCD.Price[i] = lib_DWBinToBCD(LadderPrice.Price[i]);
		}

		lib_MultipleHEXToBBCD((BYTE*)&LadderPrice.YearBill[0][0],(BYTE*)&LadderPriceBCD.YearBill[0][0],(3*MAX_YEAR_BILL));
		//������յ���
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			lib_InverseData( (BYTE*)&LadderPriceBCD.YearBill[i][0], 3 );
		}
		
		memcpy( Buf, (BYTE*)LadderPriceBCD.Ladder_Dl, (sizeof(TLadderPrice)-sizeof(DWORD)) );    
		Lenth += (sizeof(TLadderPrice)-sizeof(DWORD));
	}

	return Lenth;
}
#endif


//---------------------------------------------------------------
//��������: �����������ASCIIת��ΪBCD
//
//����:
//		InBuf[in] - ���뱸���� ASCII��ʽ
//		OutBuf[out] - ��������� BCD��ʽ
//����ֵ:
//
//��ע:
//---------------------------------------------------------------
void SwitchSoftRecordToBCD( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	
	memset( OutBuf, 0xAA, 8 ); //Ĭ��ֵ����Ϊ0xAA
	for( i = 0; i < 8; i++ )
	{
		//�Ͱ��ֽ�
		if( InBuf[2 * i] <= '9' )
		{
			OutBuf[i] = ((InBuf[2 * i] - '0') << 4);
		}
		else if( InBuf[2 * i] == 'A' )
		{
			OutBuf[i] = 0xA0; 
		}
		
		//�߰��ֽ�
		if( InBuf[2 * i + 1] <= '9' )
		{
			OutBuf[i] |= (InBuf[2 * i + 1] - '0');
		}
		else if( InBuf[2 * i + 1] == 'A' )
		{
			OutBuf[i] |= 0x0A;
		}
	}
}

//---------------------------------------------------------------
//��������: ����������HEXת��ΪASCII
//
//����:
//		InBuf[in] - �������� HEX��ʽ
//		OutBuf[out] - ������� ASCII��ʽ
//����ֵ:
//
//��ע:
//---------------------------------------------------------------
void SwitchProduceDataToAscii( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE i;
	TFourByteUnion FourTemp;
	BYTE Ascii[] = "0123456789";
	memcpy( (void *)FourTemp.b, InBuf, 4 );
	
	//ת����
	FourTemp.w[0] = lib_WBinToBCD( FourTemp.w[0] ); //��
	for( i = 0; i < 2; i++ )
	{
		OutBuf[2 * i] = Ascii[FourTemp.b[1 - i] >> 4];
		OutBuf[2 * i + 1] = Ascii[FourTemp.b[1 - i]&0x0F];
	}
	OutBuf[4] = '/';

	//ת����
	FourTemp.b[2] = lib_BBinToBCD( FourTemp.b[2] ); //��
	OutBuf[5] = Ascii[FourTemp.b[2] >> 4];
	OutBuf[6] = Ascii[FourTemp.b[2]&0x0F];
	OutBuf[7] = '/'; 
	
	//ת����
	FourTemp.b[3] = lib_BBinToBCD( FourTemp.b[3] ); //��
	OutBuf[8] = Ascii[FourTemp.b[3] >> 4];
	OutBuf[9] = Ascii[FourTemp.b[3]&0x0F];
}
//---------------------------------------------------------------
//��������: �����α�������
//
//����:
//                   PortType[in] - ͨ��
//		    ID[in]      - �������ݱ�ʶ
//		    OutBufLen   - ���뻺�泤��
//			OutBuf[out] - ��������
//
//����ֵ:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  �������
// 			DLT645_NO_RESPONSE - ����ظ�
// 			���� - �������ݳ���
//
//��ע:
//---------------------------------------------------------------
WORD GetPro645RequestMeterPara( eSERIAL_TYPE PortType,BYTE *DI, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE Buf[MAX_PARA_BUF_LENTH];
	BYTE TaggedByteBuf[1];
	BYTE i,Status;
	BYTE DataBuf[20];
	WORD wID1_2, Lenth;
	TFourByteUnion DataID;
	TFourByteUnion FourTemp;	//����4�ֽ����ݵ����⴦��
	BYTE Type;
	BYTE Sel_HEX2BCD; 			//�Ƿ���ҪHEXתBCD
	BYTE Sel_EXData;			//�Ƿ���Ҫ����
	eTYPE_COVER_DATA CoverMode;	//HEX��BCD����ת����ʽ

	BYTE ReceiveErrorNum;			//�ֲ�������6025��������415ʧ�ܴ���
	
	//Ĭ��BCDת�� ���赹��
	Sel_HEX2BCD = YES;
	Sel_EXData = YES;
	CoverMode = eBYTE_MODE;
	FourTemp.d = 0;
	Lenth = 0;
	Status = 0;
	Type = 0;
	memset( Buf, 0x00, sizeof(Buf) );
	memset( DataBuf, 0x00, sizeof(DataBuf) );
	
	if( OutBufLen == 0 )
	{
		return DLT645_ERR_ID_02;
	}

	memcpy((BYTE *)DataID.b, DI, 4);
	memcpy( (BYTE*)&wID1_2, DI+1, 2 );
	
	switch( wID1_2 )
	{
		case 0x0001:
			Lenth = ReadParaData0001XX( DI[0], Buf );
			Sel_HEX2BCD = NO;
			break;
		case 0x0002:
			if (DI[0] == 0x01)  //��ʱ����
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeZoneNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x02)  //��ʱ�α���
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeSegTableNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x03)  //��ʱ����
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.TimeSegNum;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x04)  //������
			{
				Lenth = 1;
				Buf[0] = FPara1->TimeZoneSegPara.Ratio;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x05)  //����������
			{
				Lenth = 2;
				Buf[0] = 0x00;
				Buf[1] = FPara1->TimeZoneSegPara.HolidayNum;
			}
			else if (DI[0] == 0x08)  //��Կ������
			{
				Lenth = 1;
				Buf[0] = ScretKeyNum;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			else
			{
				Lenth = 0;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			break;
		case 0x0003:
			if (DI[0] == 0x01)  //�Զ�ѭ����ʾ����
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DispItemNum[0];
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x02)	//ÿ����ʾʱ��
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.LoopDisplayTime;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x03)	//��ʾ����С��λ��
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.EnergyFloat;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x04)  //��ʾ���ʣ����������С��λ��
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DemandFloat;
				Sel_EXData = NO;
			}
			else if (DI[0] == 0x05)  //����ѭ����ʾ����
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.DispItemNum[1];
				Sel_EXData = NO;
			}
			else if( DI[0] <= 0x07 )  //������������� ��ѹ���������
			{
				Lenth = api_ReadPrePayPara( (ePrePayParaType) (DI[0]-0x05), FourTemp.b );

				if( Lenth == 4 )
				{
					Lenth = 3;
					FourTemp.d = lib_DWBinToBCD( FourTemp.d );
					memcpy( Buf, FourTemp.b, Lenth );
					Sel_HEX2BCD = NO;
					Sel_EXData = NO;
				}
			}
			else if (DI[0] == 0x08)  //�ϵ�ȫ��ʱ�� 
			{
				Lenth = 1;
				Buf[0] = FPara1->LCDPara.PowerOnDispTimer;
				Sel_EXData = NO;
			}
			else
			{
				Lenth = 0;
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			break;
		case 0x0004:
			//TMeterTypeEnumö���У�6֮�����2��698����������645������DI[0]����Ӧ
			Type = (DI[0] > 6) ? (DI[0] + 2) : DI[0];
			Lenth = api_ProcMeterTypePara( READ, Type, Buf );	
			//���峣���ڵ������16���ƴ洢 ��ҪתBCD ���赹��
			#if( MAX_PHASE == 1 )	
			if( DI[0] == 0x09 ) //����й�����
			#else
			if( (DI[0] == 0x09)||(DI[0] == 0x0A) ) //����޹�����
			#endif	
			{
				Lenth = 3;
				memcpy( (void *)FourTemp.b, Buf, 4 );
				FourTemp.d = lib_DWBinToBCD( FourTemp.d );
				memcpy( Buf, FourTemp.b, Lenth );
				Sel_HEX2BCD = NO;
				Sel_EXData = NO;
			}
			else if( DI[0] == 0x0B )	//����ͺ�
			{
				Lenth = 10;			//645������ͺ�10�ֽ�	698:32�ֽ�
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0] == 0x0C )	//��������
			{
				if( api_CheckClock((TRealTimer *)Buf)!=TRUE )
				{
               		memcpy(Buf,ProduceDate,7);
            	} 
				Lenth = 10;
				
				//HEXת��ΪASCII
				SwitchProduceDataToAscii( Buf, DataBuf );
				memcpy( Buf, DataBuf, 10 );
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0] == 0x0D ) //645Э��汾��
			{
				Lenth = 16; 
				memcpy( Buf, ProtocolVersionConst_645, 16 );
				Sel_HEX2BCD = NO; 
			}
			else if( DI[0] == 0x0F )  //�豸����λ�� ���ܱ�λ����Ϣ
            {
				memset( Buf, 0x00, 11 ); //645����λ�ù̶�Ϊ0
				
				Lenth = 11;
				Sel_HEX2BCD = NO;
                Sel_EXData = NO;
            }
			else
			{
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x0005:	//�������״̬�� ��Կ״̬��
			if( DI[0] <= 0x07 )
			{
				Lenth = api_GetMeterStatus( ePROTOCO_645, DI[0], 2, Buf );
				if( Lenth != 2 )
				{
					Lenth = 0;
				}
			}
			else if( DI[0] == 0x08 )
			{
				if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
				{	
					FourTemp.d = 0;						//��Կ����0
					memcpy( Buf, FourTemp.b, 4 );
				}
				else
				{
					FourTemp.d = 0xFFFF0F00;    		//˽Կ���ض�ֵ 0x000FFFFF
					memcpy( Buf, FourTemp.b, 4 );
				}
				Lenth = 4;
			}
			else if( DI[0] == 0xFF )  //���س��� Lenth Ϊ18�ֽ� �������״̬�ֹ�14�ֽڣ���Կ״̬��4�ֽ�
			{
				Lenth = api_GetMeterStatus( ePROTOCO_645, 0x00, 14, Buf );
				if( Lenth == 14 )
				{
					if((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)) == TRUE)
					{	
						FourTemp.d = 0;
						memcpy( Buf+14, FourTemp.b, 4 );
					}
					else
					{
						FourTemp.d = 0xFFFF0F00;
						memcpy( Buf+14, FourTemp.b, 4 );
					}
					Lenth += 4;
				}
				else
                {  
					Lenth = 0;
				}
              
				if( Lenth == 18 )
				{
                    for( i=0; i<7; i++ )
                    {
                        lib_InverseData( (Buf+(i*2)), 2 );
                    } 
                    lib_InverseData( Buf+14, 4 );
                }
                else
                {  
					Lenth = 0;
				}
				
				Sel_EXData = NO;
			}
			Sel_HEX2BCD = NO;
			break;
		case 0x0006:
			if( DI[0] == 0x01 )	//�й���Ϸ�ʽ������
			{
				Buf[0]=FPara2->EnereyDemandMode.byActiveCalMode;
				Lenth = 1;
			}
			#if(SEL_RACTIVE_ENERGY == YES)
			else if( DI[0] == 0x02 )	//�޹���Ϸ�ʽ1������
			{
				Buf[0]=FPara2->EnereyDemandMode.PReactiveMode;
				Lenth = 1;
			}
			else if( DI[0] == 0x03 )	//�޹���Ϸ�ʽ2������
			{
				Buf[0]=FPara2->EnereyDemandMode.NReactiveMode;
				Lenth = 1;
			}
			#endif//#if(SEL_RACTIVE_ENERGY == YES)
			Sel_HEX2BCD = NO;
			break;
		case 0x0007:
			if( DI[0] == 0x01 )
			{
				Buf[0] = 0x04;  //����̶�������1200
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			else if( (DI[0] >= 0x03) && (DI[0] <= 0x05) )
			{
				if( DI[0] == 0x03 )
				{
					Buf[0] = FPara2->CommPara.I485&0x0F;
				}
				else if( DI[0] == 0x04 )
				{
					Buf[0] = FPara2->CommPara.II485&0x0F;
				}
				else
				{
					Buf[0] = FPara2->CommPara.ComModule&0x0F;
				}

				if( Buf[0] < 0x05 )
				{
					Buf[0] = 0x01 << Buf[0];
				}
				else if( Buf[0] <= 0x07 ) //698��7200������ 645û�� ����Buf[0]Ҫ��һ
				{
					Buf[0] = 0x01 << (Buf[0] - 1);
				}
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			break; 
		case 0x0008:
			if( (DI[0] > 0x00) && (DI[0] <= 0x02) )   //������������ ��������õ���ʱ�α��
			{
				Buf[0] = *(&FPara1->TimeZoneSegPara.WeekStatus + (DI[0] - 0x01));
				Lenth = 1;
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x000a:    //�������߼��ʱ��
        	Lenth = api_ReadFreezeLpf645IntervalTime( DI, Buf );
        	Sel_HEX2BCD = NO;
        	Sel_EXData = NO;
        	break;	
		case 0x000B: //ÿ�µ�1������ ÿ�µ�2������ ÿ�µ�3������
			if( DI[0]<=0x03 )
			{
				if( ReadSettlementDate645( DI[0], Buf) == TRUE )
				{
					Lenth = 2;
					Sel_EXData = YES;
				}
			}
			break;
		case 0x0011:
			if( DI[0] == 0x01 )	//�������������1 
			{
                Status = 0;

			    if( api_GetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS ) == TRUE )
			    {
                    Status |= BIT2;
			    }

			    if( (FPara1->LCDPara.Meaning12) == 1 )
			    {
                    Status |= BIT1;
			    }
			    
                ReadRelayPara( 3, Buf);
			    if( Buf[0] == 0 )
			    {
                    Status |= BIT0;
			    }
			    
				Buf[0]= Status;
				Lenth = 1;
				Sel_EXData = NO;
				Sel_HEX2BCD = NO;
				
			}
			else if( DI[0] == 0x04 )//�����ϱ�ģʽ�� 
			{
				Lenth = api_ReadReportStatusMode( Buf );
				
				Sel_EXData = NO;
				Sel_HEX2BCD = NO;
			}
			break;
		case 0x0014:    
			switch( DI[0] )
			{
				case 0x01:  //��բ��ʱʱ�� ��բ��ʱʱ��
					if( ReadRelayPara( 2,  Buf ) == TRUE )
					{
						Lenth = 2;
						Sel_HEX2BCD = YES;
						CoverMode = eCOMB_MODE;
						Sel_EXData = NO;
					}
					break;
				case 0x02:  //�̵�����բ���Ƶ�������ֵ
					if( ReadRelayPara( 0,  FourTemp.b ) == TRUE )
					{
						Lenth = 3;
						FourTemp.d = lib_DWBinToBCD( FourTemp.d / 10 ); //д����ʱ�˹�10
						memcpy( Buf, FourTemp.b, Lenth );
						Sel_HEX2BCD = NO;
						Sel_EXData = NO; //
					}
					break;
				case 0x03:  //����բ���Ƶ������ޱ�����ʱʱ��
					if( ReadRelayPara( 1, Buf ) == TRUE )
					{
						Lenth = 2;
						Sel_EXData = NO;
						CoverMode = eCOMB_MODE;
					}
					break;
				case 0x04:  //������֤ʱЧ
					api_ReadPrePayPara( eIRTime, Buf ); //ǿ��ת��Ϊ1�ֽ�
					Lenth = 1;
					Sel_EXData = NO;
					Sel_HEX2BCD = YES;
					break;
				case 0x05:  //�����ϱ�״̬���Զ���λ��ʱʱ��
					Buf[0] = api_GetReportResetTime();
					Lenth = 1;
					Sel_EXData = NO;
					Sel_HEX2BCD = YES;
					break;
				default:
					break;
			}
			break;
        case 0x0015:    //�����ϱ�״̬��
            if( DI[0] == 0x01 ) //�������ϱ�״̬��
            {   
                Lenth = api_ReadReportStatusByte( PortType, eREAD_STATUS_PROC, Buf );
                Sel_EXData = NO;
                Sel_HEX2BCD = NO;
            }
            break;		
		case 0x0100:	//��һ��ʱ�������� ��һ����ʱ�α�����
		case 0x0200:	//�ڶ���ʱ�������� �ڶ�����ʱ�α�����
			if( DI[0] == 0x00 )	
			{
				Lenth  = FPara1->TimeZoneSegPara.TimeZoneNum * 3;
				FourTemp.b[0] = 0x01;
			}
			else if( DI[0] <= 0x08 )
			{
				Lenth = FPara1->TimeZoneSegPara.TimeSegNum * 3;
				FourTemp.b[0] = DI[0] + 0x02;
			}
			if(DI[2] == 0x02)   
            {
				FourTemp.b[0] |= 0x80;
            }

			if( api_ReadTimeTable( FourTemp.b[0], 0, Lenth, Buf ) == FALSE )
			{
				Lenth = 0;
			}
			//�����
			for( i = 0; i < Lenth / 3; i++ )
			{
				lib_InverseData( &Buf[3*i], 3 );
			}
			Sel_EXData = NO;
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )	//Sel_HEX2BCD\Sel_EXData��ѡNO���ں����ڲ��������ݸ�ʽ���ⲿ�Ĵ��������⣬��������Щ����
		case 0x0501:
		case 0x0502:
			Lenth = ReadPriceParaData645(0x05,DI[1],DI[0],Buf);
			Sel_HEX2BCD = NO;
			Sel_EXData = NO;
			break;
		case 0x0604:
		case 0x0605:
			Lenth = ReadPriceParaData645(0x06,DI[1],DI[0],Buf);
			Sel_HEX2BCD = NO;
            Sel_EXData = NO;
			break;
		#endif
		case 0x8000:
			switch( DI[0] )
			{
				case 0x01:	//��������汾��
					Status = eMeterSoftWareVersion;
                    break;
				case 0x02:  //����Ӳ���汾��
					Status = eMeterHardWareVersion;
                    break;
				case 0x03:  //���ұ��
					Status = eMeterFactoryCode;
					break;
				case 0x04:	//���������
					Status = eMeterSoftRecord;
					break;
				default:
					Status = 0xff;
					break;
			}
			if( Status != 0xff )
			{
				if(api_ProcMeterTypePara( READ, Status, DataBuf ) != FALSE)
				{
					if(DI[0] == 0x04)
					{
						SwitchSoftRecordToBCD( DataBuf, Buf ); 
	                    Lenth = 8;
					}
					else
					{
						memcpy( Buf, DataBuf, 4 );
						Lenth = 32;
					}


				}
				else
				{
					Lenth = 0;
				}
			}
			else
			{
				Lenth = 0;
			}
			Sel_EXData = YES;
			Sel_HEX2BCD = NO;
			break;
		#if (SEL_F415 == YES)
		case 0x000F:
			if( DI[0]==0x00 )		//��ȡ��ѹ����ϵ��
			{
				memcpy(Buf,(BYTE*)&TF415UIK,sizeof(TSampleUIK)-sizeof(DWORD) );
				Lenth = sizeof(TSampleUIK)-sizeof(DWORD);
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0x01 )	//������һ��·���ɱ�ʶ���
			{
				api_ReadIdentResult(0, Buf);

				Lenth = sizeof(TElectIdentify)*IDF_RES_NUM;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0x05 )	//�����ڶ���·���ɱ�ʶ���
			{
				api_ReadIdentResult(1, Buf);

				Lenth = sizeof(TElectIdentify)*IDF_RES_NUM;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0xB0 )	//��ȡ6025_to_415�ɼ�����Ŀ���״̬
			{
				Buf[0] = api_GetCollectionTaskStatus415();
				Lenth = 1;
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			else if( DI[0]==0xB1 )	//��ȡ415���ص�ʶ����
			{
				
				memcpy(Buf,(BYTE*)&MessageData.ucTime,sizeof(AMessage_T) );
				Lenth = sizeof(AMessage_T);
				
				//��Ҫ��������һ���꣬����+��λ��
				ProBinToBCD( eCOMB_MODE, sizeof(WORD), Buf );
				ProBinToBCD( CoverMode, sizeof(TRealTimer)-sizeof(WORD), Buf+2 );
				lib_InverseData(Buf,sizeof(WORD));
				lib_ExchangeData(Buf,Buf,sizeof(TRealTimer));
				Sel_HEX2BCD = NO;
				// Sel_EXData = YES;
				Sel_EXData = NO;
			}
			else if( DI[0]==0xB2 )	//��ȡ����415ʧ�ܴ���
			{
				//415�ĵ�һ·uartΪ��6025ͨ�ŵ���uart��
				ReceiveErrorNum =  TSendRec[0].Sec_Send_Success - TSendRec[0].Sec_Rec_Success;
				memcpy(Buf,(BYTE*)&ReceiveErrorNum,sizeof(ReceiveErrorNum) );
				Lenth = sizeof(ReceiveErrorNum);
				Sel_HEX2BCD = NO;
				Sel_EXData = YES;
			}
			break;
		#endif//#if (SEL_F415 == YES)
		default:
			break;
	}
	//���ݳ��ȳ���
	if( (Lenth == 0x00) || (Lenth > OutBufLen) )
	{
		return DLT645_ERR_ID_02;
	}
	
	//ͳһ����HEXתBCD
	if( Sel_HEX2BCD == YES )
	{
		//���з���ֵ ��ʹת������ ҲҪ����
		ProBinToBCD( CoverMode, Lenth, Buf );
	}
	
	//ͳһ������
	if( Sel_EXData == YES )
	{
		lib_ExchangeData( OutBuf, Buf, Lenth );
	}
	else
	{
		memcpy( OutBuf, Buf, Lenth );
	}
	
	return Lenth;
}


#endif//#if( SEL_DLT645_2007 == YES )
