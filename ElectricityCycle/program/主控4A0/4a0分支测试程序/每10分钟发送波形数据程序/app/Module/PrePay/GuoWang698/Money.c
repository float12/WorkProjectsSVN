//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.17
//����		

//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "PrePay.h"

#if( PREPAY_MODE == PREPAY_LOCAL )

const DWORD DefRatePrice[MAX_RATIO] = {5000, 5000, 5000, 5000};
const DWORD DefLadderDl[MAX_LADDER] = {2880, 4800, 4800, 4800, 4800, 4800};
const DWORD DefLadderPrice[MAX_LADDER+1] = {0, 500, 3000, 3000, 3000, 3000, 3000};
const BYTE  DefCstYearBill[MAX_YEAR_BILL][3] = {1,1,0};//1��1��0ʱ

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------

typedef enum
{
	eNoLadderMode = 0,			//�޽���ģʽ
	eLadderMonMode = 0x55,     //�½���ģʽ
	eLadderYearMode = 0xaa,    //�����ģʽ
}eLadderMode; 

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------																						



//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
__no_init TCurMoneyBag  CurMoneyBag @ (0x20000000+2*sizeof(TEnergyRam)); //Ǯ���ļ����ѿ۷������� �̶���ַ ����EnergyRam�� ��������һ������ 
__no_init TRatePrice	CurRateTable;		//��ǰ���ʵ�۱�
__no_init TLadderPrice	CurLadderTable;		//��ǰ���ݵ�۱�
__no_init TCurPrice		CurPrice;			//��ǰ��� ���ڿ۷�
__no_init TLimitMoney	LimitMoney;         //������޲���
TCardInfo               CardInfo; //����Ϣ������״̬�������кţ�
eChangeCurPriceFlag  	UpdatePriceFlag;    	//���ˢ���жϱ�־

TLocalStatus			LocalStatus;		//����״̬

TLadderPrice			PowerdownLadderTable;	//����ǰ���ݵ�۱�

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
extern BYTE				PrePayPowerOnTimer;     //�ϵ��ʱ����30����
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void CheckCurMoneyBag( ePOWER_MODE Type );    //ʣ�������У��
//
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ����ʣ�������ݵ�ee
//
//����: 	��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
static BOOL SaveCurMoneyBag(void)
{
	BYTE Result;
	
	Result = FALSE;

	if( CurMoneyBag.CRC32 == lib_CheckCRC32( (BYTE*)&CurMoneyBag , sizeof(TCurMoneyBag)-sizeof(DWORD) ) )//�洢ʣ����
	{
		api_SetSysStatus(eSYS_STATUS_EN_WRITE_MONEY);
		Result = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE *)&CurMoneyBag );
		api_ClrSysStatus(eSYS_STATUS_EN_WRITE_MONEY);
	}
	
	return Result;
}

//--------------------------------------------------
//��������:  esam���ۿ��
//
//����:      BYTE* InBuf[in]//4�ֽڽ��  ����buf�������bufΪͬһָ�룬��Ҫע�����buf�ĳ���
//
//����ֵ:	OutBuf	2�ֽڳ���+4�ֽڿۿ���+4�ֽڽ��׽��
//
//��ע����:
//--------------------------------------------------
static BOOL ReduceEsamMoney( BYTE *InBuf, BYTE *OutBuf )
{
	BYTE TmpBuf[15];
	BOOL Result;
	WORD ReturnLen;

	api_ProcPrePayCommhead( TmpBuf, 0x80, 0x46, 0x00, 0x00, 0x00, 0x04 );
	lib_ExchangeData( TmpBuf + 6, InBuf, 4 ); //ȡ4�ֽڿۿ���
	Result = Link_ESAM_TxRx( 6, TmpBuf, 4, TmpBuf + 6,  OutBuf );
	if( Result == FALSE )
	{
		return FALSE;
	}

	lib_ExchangeData( (BYTE *)&ReturnLen, OutBuf, 2 );
	memmove( OutBuf, OutBuf + 2, ReturnLen );

	return TRUE;
}

//-----------------------------------------------
//��������: RAM��ESAM����Ǯ��ͬ��
//
//����:		��
//
//����ֵ:	��
//
//��ע:��ram��esam�й�����������ram������esamʱ����esam�ۼ���ֵ����֮��esam����ֵ����ram����ֵ
//-----------------------------------------------
static WORD SyncMoneyBagToEsam( void )
{
	BYTE MoneyBagBuf[20];
	TFourByteUnion ESAMMoney,ESAMBuyTimes,RamMoney;

	if( ReadEsamMoneyBag( MoneyBagBuf ) != TRUE ) //��Ǯ��ʣ����(1)�͹������(3)
	{
		return FALSE;
	}
	
	lib_ExchangeData((BYTE *)&ESAMMoney, MoneyBagBuf, 4);
	lib_ExchangeData((BYTE *)&ESAMBuyTimes, MoneyBagBuf+4, 4);
	
	if( CurMoneyBag.Money.Remain.ll > 0 )
	{
        RamMoney.d = (DWORD)(CurMoneyBag.Money.Remain.ll/10000);
	}
	else
	{
        RamMoney.d = 0;
	}
	
	//����������ͬʱʣ��ram<Ǯ��,��ramΪ׼
	if( (ESAMBuyTimes.d == CurMoneyBag.Money.BuyTimes)&&(ESAMMoney.d > RamMoney.d) )
	{		
		ESAMMoney.d -= RamMoney.d;		//Ǯ�� - ʣ��
		
		ReduceEsamMoney( ESAMMoney.b, MoneyBagBuf );
		lib_ExchangeData( ESAMMoney.b, MoneyBagBuf, 4 );
		
		if( ESAMMoney.d != RamMoney.d )//�ж��Ƿ�ۼ��ɹ�
		{
			return FALSE;
		}
	}
	else if( (ESAMBuyTimes.d != CurMoneyBag.Money.BuyTimes)||(ESAMMoney.d < RamMoney.d) )//��������Ȼ�esam���С��ram���ʱ��esamΪ׼
	{		
		CurMoneyBag.Money.Remain.ll = (long long)ESAMMoney.d*10000;
		CurMoneyBag.Money.BuyTimes = ESAMBuyTimes.d;	
		CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, (sizeof(TCurMoneyBag) -sizeof(DWORD)));
		
		SaveCurMoneyBag();//����ʣ�������ݵ�ee
		
		api_WriteSysUNMsg( SYSUN_FIX_MONEY );	
	}
		
	return TRUE;	
}

//-----------------------------------------------
//��������: ����Ǯ��ͬ��
//
//����:     ��
//                   
//����ֵ:   �� 
//
//��ע:   
//-----------------------------------------------
void MeteringUpdateMoneybag(void)
{
	if( SyncMoneyBagToEsam() != TRUE )
	{
		ASSERT( 0 != 0, 0 ),
		api_ResetEsamSpi();
		SyncMoneyBagToEsam();  //�ٴ�Ǯ������
	}
}

//-----------------------------------------------
//��������: ����ram�еĽ��ݵ���/���/�������/CRC
//
//����: 	Type[in]:ePowerOnMode/ePowerDownMode
//
//����ֵ:  	��
//
//��ע:   �����жԽ���ֵ������������
//-----------------------------------------------
static void CheckLadderPriceRamData( ePOWER_MODE Type )
{
	BYTE i, k, Flag, Crc_Flag;
	DWORD Change_Tmp;
	
	if( CurLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&CurLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )//�ڴ�У����� ���߷��������л�ʱ ��EEPROM�ָ����
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&CurLadderTable ) != TRUE )//������ǰ���ݱ�
		{
			//for debug @@@
			memcpy( (BYTE *)&CurLadderTable.Ladder_Dl,(BYTE *)&DefLadderDl,sizeof(DefLadderDl) );
			memcpy( (BYTE *)&CurLadderTable.Price, (BYTE *)&DefLadderPrice, sizeof(DefLadderPrice) );
			memcpy( CurLadderTable.YearBill, DefCstYearBill, sizeof(DefCstYearBill) );
		}
		
		if( Type == ePowerOnMode )
		{
			ProcLadderModeSwitch();
		}
	}
	else
	{
		return;
	}
	
	Crc_Flag = 0;
	for( i=MAX_LADDER-1; i>0; i-- )//�Խ���ֵ������������
	{
		Flag = 0;
		for(k=0;k<i;k++)
		{
			if( ((CurLadderTable.Ladder_Dl[k] > CurLadderTable.Ladder_Dl[k+1]) && (CurLadderTable.Ladder_Dl[k+1] != 0)) ||     //Ϊ0�򲻽���
                ((CurLadderTable.Ladder_Dl[k+1] > CurLadderTable.Ladder_Dl[k]) && (CurLadderTable.Ladder_Dl[k] == 0))        ) //��û�����õĽ���ֵ�ŵ�����
			{
				Change_Tmp = CurLadderTable.Ladder_Dl[k];
				CurLadderTable.Ladder_Dl[k] = CurLadderTable.Ladder_Dl[k+1];
				CurLadderTable.Ladder_Dl[k+1] = Change_Tmp;
				
				Flag = 1;
				Crc_Flag = 0x55; //���λ�õ��� ���¼���CRC
			}
		}
		if( Flag == 0 )
		{
			break;			// �޽���
		}
	}
    //��û�����õĽ���ֵ��ֵΪ���һ�����õĽ���ֵ
    for( k=1; k<MAX_LADDER; k++ )
    {
        if( CurLadderTable.Ladder_Dl[k] == 0 )
        {
            CurLadderTable.Ladder_Dl[k] = CurLadderTable.Ladder_Dl[k-1];
            Crc_Flag = 0x55; //���λ�õ��� ���¼���CRC
        }
    }
    
	if( Crc_Flag == 0x55 ) //���λ���й����� ���¼���CRC
	{
		CurLadderTable.CRC32 = lib_CheckCRC32((BYTE*)&CurLadderTable, sizeof(TLadderPrice)-sizeof(DWORD));
	}
}

//-----------------------------------------------
//��������: ����RAM�еķ��ʵ��
//
//����: 	��
//                   
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
static void CheckRatePriceRamData( void )
{
	if( CurRateTable.CRC32 != lib_CheckCRC32( (BYTE *)&CurRateTable, sizeof(TRatePrice) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentRate ), sizeof(TRatePrice), (BYTE *)&CurRateTable ) != TRUE )//������ǰ���ݱ�
		{
			memcpy( (BYTE *)CurRateTable.Price, (BYTE *)&DefRatePrice, sizeof(DefRatePrice) );//for debug @@@
		}
	}
}

//-----------------------------------------------
//��������: �����ۼƹ�����
//
//����:
//			lValue[in],������������������˷ѽ��
//����ֵ:		��
//
//��ע:
//-----------------------------------------------
void SaveTotalBuyMoney( long lValue )
{
	DWORD dwData;

	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dwData );
	dwData = dwData + lValue;
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dwData );
}


//-----------------------------------------------
//��������: ���湺��ǰʣ����͸֧��������
//
//����:  		
//
//				Type[in]
//����ֵ:		��
//		   
//��ע:
//-----------------------------------------------
static void SaveBuyMoneyPreData(eUpdataMoneyMode Type)
{
	//���ڱ���202C2201��������ǰʣ����  
	//202D2200��������ǰ͸֧��� 
	//202E2200��������ǰ�ۼƹ����� 
	//���淶�ౣ�湺��ǰ�Ĺ�������������û���չ������
	DWORD dwPreData[4];

	memset( (BYTE *)&dwPreData, 0X00, sizeof(dwPreData) );

	if( Type != eInitMoneyMode )//Ԥ�ý����ڲ���Ԥ�ÿ���Ǯ����ʼ������
	{	
		if( CurMoneyBag.Money.Remain.ll >= 0 )
		{
			dwPreData[0] = CurMoneyBag.Money.Remain.ll/10000;//WORD dwPreRemain;			//202C2201��������ǰʣ���� 
		}
		else
		{	
			dwPreData[1] = 	(-1)*CurMoneyBag.Money.Remain.ll/10000;//DWORD dwPreTickMoney;		//202D2200��������ǰ͸֧��� 
		}
		api_ReadTotalBuyMoney( (BYTE *)&dwPreData[2]);	//DWORD dwPreTotalBuyMoney;	//202E2200��������ǰ�ۼƹ����� 
		dwPreData[3] = CurMoneyBag.Money.BuyTimes;//���淶�ౣ�湺��ǰ�Ĺ�������������û���չ������
	}
	
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(dwPreData), (BYTE *)&dwPreData[0]);
}
//-----------------------------------------------
//��������: �����˷�ǰʣ����
//
//����:  		
//
//				Type[in]
//����ֵ:		��
//		   
//��ע:
//-----------------------------------------------
static void SaveReturnMoneyPreData(void)
{ 

	DWORD dwPreData;

	dwPreData= 0;

	if( CurMoneyBag.Money.Remain.ll >= 0 )
	{
		dwPreData = CurMoneyBag.Money.Remain.ll/10000;//WORD dwPreRemain;			//202C2201��������ǰʣ���� 
	}

	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( EventConRom.ProgramRecordRom.PreProgramData.Data[4] ), sizeof(dwPreData), (BYTE *)&dwPreData);
}

//-----------------------------------------------
//��������:     ��ֵ�����ʣ����
//
//����: 	    
//				Type[in] 			
// 					eReturnMoneyMode  - �˷Ѳ���
// 					eChargeMoneyMode  - ��ֵ���
// 					eInitMoneyMode     - Ԥ�ý��                
// 		        ReduceValue[in] 	�۳����                 
//����ֵ:    ��
//
//��ע:   
//-----------------------------------------------
BOOL api_UpdataRemainMoney( eUpdataMoneyMode Type, DWORD ReduceValue, DWORD BuyTimes )
{
	DWORD	dw;

	CheckCurMoneyBag( ePowerOnMode );   //ʣ�������У��

	if( Type > eInitMoneyMode )
	{
		return FALSE;
	}

    if( Type == eInitMoneyMode )//Ԥ�ý�Ǯ����ʼ��
    {
		//���湺��ǰʣ����͸֧��������
		SaveBuyMoneyPreData(eInitMoneyMode);
		//�õ�ǰǮ���ļ�
        CurMoneyBag.Money.Remain.ll = (long long)ReduceValue*10000;
        CurMoneyBag.Money.BuyTimes = 0;
        CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, sizeof(TCurMoneyBag)-sizeof(DWORD) );
		//���ۼƹ����������eeprom��
		dw = CurMoneyBag.Money.Remain.ll / 10000;
		api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), (BYTE *)&dw ); 

		api_InitRelay();
    }
	else //��ֵ���˷�
    {
		if( Type == eReturnMoneyMode )//�˷�
		{
			SaveReturnMoneyPreData();
			CurMoneyBag.Money.Remain.ll -= (long long)ReduceValue * 10000;

			api_WritePreProgramData( 1, ReduceValue ); //�����˷ѽ��
			api_SavePrgOperationRecord( ePRG_RETURN_MONEY_NO ); //���ܱ��˷Ѽ�¼

			SaveTotalBuyMoney( (long)(-1 * ReduceValue) ); //�����ۼƹ�����
		}
		else//��ֵ
		{
			SaveBuyMoneyPreData( eChargeMoneyMode ); //���湺��ǰʣ����͸֧��������
			CurMoneyBag.Money.Remain.ll += (long long)ReduceValue * 10000;
			CurMoneyBag.Money.BuyTimes = BuyTimes;
			SaveTotalBuyMoney( ReduceValue ); //������ǹ������ۼƽ��
			api_SavePrgOperationRecord( ePRG_BUY_MONEY_NO ); //�����¼�й���ǰ���ݡ���������ݣ��ܲ��ܲο�����������������ǰ�����������Ƚ���
		}

		CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );
		MeteringUpdateMoneybag(); 
    }
	
	if( SaveCurMoneyBag() == FALSE ) //����ʣ����
	{
		return FALSE;
	}

	JudgeLocalStatus( Type, 0xFF ); //����״̬ˢ��
	
    return TRUE;

}
//-----------------------------------------------
//��������: �жϽ��ݽ����ռ���Ƿ����½��ݣ�Ҫ����ı����ǽ��ݵ�1������
//
//����:  	��
//
//����ֵ:	eLadderMode:	eNoLadderMode/eLadderMonMode/eLadderYearMode
//
//��ע:   	Сʱ�仯����
//-----------------------------------------------
static eLadderMode JudgeLadderMode( void )
{
	BYTE i;

	//������ 0--�� 1--��  2--ʱ
	for( i = 0; i < MAX_YEAR_BILL; i++ )
	{
		if( (CurLadderTable.YearBill[i][1] <= 28)&&(CurLadderTable.YearBill[i][1] > 0) //��
			&&(CurLadderTable.YearBill[i][2] <= 23) ) //ʱ
		{
			if( (CurLadderTable.YearBill[i][0] <= 12)&&(CurLadderTable.YearBill[i][0] > 0) ) //��
			{
				return eLadderYearMode;//��ģʽ
			}
			else if( i == 0 ) //�����һ���ݽ����� ����Ч ˵�����½���ģʽ ֻ����ʱ����
			{
				return eLadderMonMode;//��ģʽ
			}
		}
	}

	return eNoLadderMode; //�Ȳ�������ݡ��ֲ����½���
}

//---------------------------------------------------------------
//��������: ��õ�ǰ�����õ���
//
//����: 	��
//
//����ֵ:	��ǰ�����õ���
//
//��ע:   �������ģʽ�ַ���
//---------------------------------------------------------------
DWORD GetCurLadderUserEnergy( void )
{
	DWORD dwUserEnergy;
	
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		//��EEProm�ָ�����
		if( api_FreshParaRamFromEeprom( 1 ) != TRUE )
		{
			api_SetError( ERR_FPARA2 );
		}
	}

	dwUserEnergy = 0;
	if( (FPara2->EnereyDemandMode.byActiveCalMode&0x03) == 0x01 )
	{
		dwUserEnergy += CurMoneyBag.UserEnergy.ForwardEnergy;
	}

	if( (FPara2->EnereyDemandMode.byActiveCalMode&0x0C) == 0x04 )
	{
		dwUserEnergy += CurMoneyBag.UserEnergy.BackwardEnergy;
	}
	
	return dwUserEnergy;
}

//-----------------------------------------------
//��������: ����ж�
//
//����:     ePOWER_MODE Type
//                   
//����ֵ:   ��
//
//��ע:   
//����ģʽ�л�
//����ģʽ�л� ����� �½���
//��Ҫ�������½���ģʽ�л������
//1�����������ն�ͨ���û����������õ�ǰ�׽��ݱ�ʱҪ�ж�
//2���ѿ�ģʽ�����л�ʱҪ�ж�
//3�����ñ����׽���ʱ����ʱ����Ҫ�жϲ��õ�ǰ�ڼ��׽��ݱ����ж����½����л�
//4�����ñ����׽��ݱ�ʱ����Ҫ�ж����½����л�
//5������ʱ�����ڲ������л�ʱ���ҵ���ǰ���ݲ������ױ����ж����½����л�
//6�����õ�ǰ�׽���ʱ����ʱ����Ҫ�ҵ���ǰ���ݲ������ױ����ж����½����л�
//7�����ñ����׽���ʱ���������׽��ݱ�ʱ�����ݴ�����������ж��Ƿ�������½����л�
//8: ������շ����仯����������ս����л�
//-----------------------------------------------
void JudgePrice( ePOWER_MODE Type )    //���ʽ��ݵ���ж�
{
	BYTE i,t;
	BYTE CurrentRateNum;	//��ǰ����
	DWORD TmpPTCT;
	DWORD dwUserEnergy;
		
	CheckLadderPriceRamData( Type ); //��������У��
		
	CheckRatePriceRamData(); //���ʵ�۱�У��
	
	t = 6;//6�����ݵ���
	for (i=0; i<5; i++)
	{
		if ( CurLadderTable.Ladder_Dl[5-i] == CurLadderTable.Ladder_Dl[4-i] ) 
		{
			t--;//��ͬ��������1
		}
		else 
		{
			break;
		}
	}
	
	if( (t>0) && (CurLadderTable.Ladder_Dl[t-1]==0) ) 
	{
		t--;//�����ݵ���Ϊ0,���������1
	}
	
	dwUserEnergy = GetCurLadderUserEnergy();

	if( t == 0 )	//�жϽ��ݵ����Ƿ�Ϊȫ0
	{
		CurPrice.Current_Ladder = 0xff;//�޽���
		CurPrice.Current_L.d = 0;//��ǰ���ݵ��Ĭ��Ϊ0
		CurPrice.Current_Lx.d = 0;
	}
	else
	{
		for(i=0; i<t; i++)
		{
			if( dwUserEnergy <= CurLadderTable.Ladder_Dl[i] )
			{
				break;//�ҵ�
			}
		}//i�ķ�Χ0~6
		
		CurPrice.Current_Ladder = i;//��ǰ����
		CurPrice.Current_L.d = CurLadderTable.Price[i];//���ݵ��
	}
		
	CurrentRateNum = api_GetCurRatio();//������ʵ��
	if( (CurrentRateNum==0)||(CurrentRateNum > MAX_RATIO) ) //��ǰ���ʵ�۲��ԣ�Ĭ��ʹ��ƽ���
	{
		CurrentRateNum = DefCurrRatioConst;
	}
	
	CurPrice.Current_F.d = CurRateTable.Price[CurrentRateNum-1];

	CurPrice.Current.d = (CurPrice.Current_F.d+CurPrice.Current_L.d);//���㵱ǰ���=���ݵ��+���ʵ��
	//������	
	TmpPTCT = PrePayPara.PTCoe*PrePayPara.CTCoe;//���hex
	if ( (TmpPTCT==0) || (TmpPTCT>=1000000) ) //�����Ƿ����!!!!!!
	{
		TmpPTCT = 1;
	}
	
	CurPrice.Current_Lx.d = CurPrice.Current_L.d*TmpPTCT;//���*��ǰ���ݵ��(hex)	
	CurPrice.Current_Fx.d = CurPrice.Current_F.d*TmpPTCT;//���*��ǰ���ʵ��(hex)

    UpdatePriceFlag = eNoUpdatePrice;
}

//-----------------------------------------------
//��������: ���ݽ����ռ����ܣ����ڱ�����ݽ���ʱ���ܵ��룬���ڼ�������õ���
//
//����:  	��
//	
//����ֵ:	TRUE--��ǰ�¡��� ��ʱ���ô��ڽ��ݽ����� FALSE---��ǰ�¡��� ��ʱ�����ڽ��ݽ�����
//
//��ע:   	Сʱ�仯����
//-----------------------------------------------
BOOL api_CheckLadderClosing( void )
{
	BYTE i,Mon,Day,Hour;
	TBillingPara Para;
	eLadderMode LadderMode;

	api_GetRtcDateTime( DATETIME_MM, &Mon );
	api_GetRtcDateTime( DATETIME_DD, &Day );
	api_GetRtcDateTime( DATETIME_hh, &Hour );

	if( CurLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&CurLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )//�ڴ�У����� ���߷��������л�ʱ ��EEPROM�ָ����
	{
		//�������ɿ��Թ淶�������ݲ����쳣���ܵõ�����ֵʱ���γ��¼���¼���ϱ����ȴ����裬�����ݻָ�����ǰ�����طѿ����ܵ��ܱ�Ӧ�ۼ����ݵ��
		JudgePrice( ePowerOnMode );
	}
	
	LadderMode = JudgeLadderMode();

	//�����һ���ݽ���������Ч,���ա�ʱ��Ч ˵�����½���ģʽ ֻ����ʱ����
	if( LadderMode == eLadderMonMode )
	{
		if( (Day == CurLadderTable.YearBill[0][1])&&(Hour == CurLadderTable.YearBill[0][2]) )
		{
			return TRUE;
		}
	}
	else if( LadderMode == eLadderYearMode ) //��ģʽ
	{
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			if( (Mon == CurLadderTable.YearBill[i][0])
				&&(Day == CurLadderTable.YearBill[i][1])
				&&(Hour == CurLadderTable.YearBill[i][2]) )
			{
				return TRUE;
			}
		}
	}
    else    //��ģʽ
    {
        
    }
	
    return FALSE;
}

//-----------------------------------------------
//��������:     ���õ��ˢ�±�־λ
//
//����: 	    PriceFlag[in]
//			    eNoUpdatePrice		        ���δ�����仯
//			    ePriceChangeEnergy,			���δ�����仯 ����Ҫˢ�µ�ǰ���
//			    eChangeCurRatePrice,	    �ı��˵�ǰ���ʵ��
//			    eChangeCurLadderPrice,		�ı��˵�ǰ���ݵ��
//
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void api_SetUpdatePriceFlag( eChangeCurPriceFlag  PriceFlag )
{
	//�ı�RAM�е��CRC �����EEPROM�ָ�
	switch( PriceFlag )
	{
		case eChangeCurRatePrice:
			CurRateTable.CRC32 += 1;
			break;
		case eChangeCurLadderPrice:
			CurLadderTable.CRC32 += 1;
			break;
		case ePriceChangeEnergy:
			CurRateTable.CRC32 += 1;
			CurLadderTable.CRC32 += 1;
			break;
		default:
			break;
	}
	//�ñ��������ڴ��У���ʱδ���µ�����߸�λҲ����ν���ϵ�������жϵ��
	UpdatePriceFlag = PriceFlag;    
}
//-----------------------------------------------
//��������: 	ʣ�����������У��
//
//����: 		ePOWER_MODE Type
//
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
static void CheckCurMoneyBag( ePOWER_MODE Type )    //ʣ�����������У��
{
	WORD Result;
	TFourByteUnion td;
	BYTE MoneyBagBuf[20];
	
	td.d = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );

	//���RAM��EEPROM�е����ݶ����� ����ESAM�е�Ϊ׼ �������Ƿ ���޷��ָ�
	if( CurMoneyBag.CRC32 != td.d )
	{
		api_WriteSysUNMsg( WATCH_MONEY_RAM_ERR );
		
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE*)&CurMoneyBag ) != TRUE )
		{
			if( Type != ePowerWakeUpMode )//�͹��Ļ��Ѳ���esam�ָ�
			{
				Result = ReadEsamMoneyBag( MoneyBagBuf ); //��Ǯ��ʣ����(1)�͹������(3)
				if(Result != TRUE) 
				{
					return;
				}
				
				memcpy( td.b, MoneyBagBuf, 4 );//Ǯ��ʣ����ת����ramʣ���� ����˳���Ƿ���Ҫ���� ������Ҫ��֤@@@@
				CurMoneyBag.Money.Remain.ll = (long long)td.d*10000;
				memcpy( td.b, MoneyBagBuf+4, 4 );//���������esamΪ׼
				CurMoneyBag.Money.BuyTimes = td.d;		
				CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE*)&CurMoneyBag, sizeof(TCurMoneyBag)-sizeof(DWORD) );
				SaveCurMoneyBag();//����ʣ�������ݵ�ee
				
				api_WriteSysUNMsg( SYSUN_FIX_MONEY );
			}
		}
	}
}

//-----------------------------------------------
//��������: Ǯ������У�� 
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void VerifyMoneyPara( void )    //��Ƿ���,�ڻ����Ȳ���У��
{	
	//���ض������У�� -- �ڻ���͸֧������ޡ���բ����������
	if( LimitMoney.CRC32 != lib_CheckCRC32( LimitMoney.TickLimit.b, sizeof(TLimitMoney) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LimitMoney ), sizeof(TLimitMoney),LimitMoney.TickLimit.b ) != TRUE )
		{
			//����Ĭ��ֵ---�����û�����
			LimitMoney.Alarm_Limit1.d = 0;
			LimitMoney.Alarm_Limit2.d = 0; 
			LimitMoney.CloseLimit.d = CloseLimitConst;
			LimitMoney.RegrateLimit.d = RegrateLimitConst;
			LimitMoney.TickLimit.d = TickLimitConst;
		}
	}
	
	//����״̬У��
	if( LocalStatus.CRC32 != lib_CheckCRC32( (BYTE*)&LocalStatus.CurLocalStatus, sizeof(TLocalStatus) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LocalStatus ), sizeof(TLocalStatus), (BYTE*)&LocalStatus.CurLocalStatus ) != TRUE )
		{
			LocalStatus.CurLocalStatus = eNoAlarm;
			LocalStatus.LastLocalStatus = eNoAlarm;
		}
	}

	if( CardInfo.CRC32 != lib_CheckCRC32( (BYTE*)&CardInfo.LocalFlag, sizeof(TCardInfo) - sizeof(DWORD) ) )
	{
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CardInfo ), sizeof(TCardInfo), (BYTE*)&CardInfo.LocalFlag ) != TRUE )
		{
		//�Ƿ����Ĭ��ֵ
		}
	}
}

//---------------------------------------------------------------
//��������: �嵱ǰ�õ���
//
//����:   ��
//
//����ֵ: ��
//
//��ע:  �������������½���ʱ���㵱ǰ�õ���
//---------------------------------------------------------------
void api_ClrCurLadderUseEnergy( void )
{
	//У���ѿ۷ѵ���
	CheckCurMoneyBag( ePowerOnMode );

	CurMoneyBag.UserEnergy.ForwardEnergy = 0;
	CurMoneyBag.UserEnergy.BackwardEnergy = 0;

	api_SetSysStatus( eSYS_STATUS_EN_WRITE_MONEY );
	api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.CurMoneyBag ), sizeof(TCurMoneyBag), (BYTE *)&CurMoneyBag );
	api_ClrSysStatus( eSYS_STATUS_EN_WRITE_MONEY );

	//ˢ�µ��
	//api_SetUpdatePriceFlag( ePriceChangeEnergy );
	JudgePrice( ePowerOnMode );
}
//-----------------------------------------------
//��������: �жϵ���Ƿ�������״̬
//
//����:    ��
//
//����ֵ:  TRUE/FALSE
//
//��ע:
//-----------------------------------------------
static BOOL JudgeNoAlarm( void )
{
	DWORD Alarm1, Alarm2; 
	long long RemainMoney;
	
	Alarm1 = LimitMoney.Alarm_Limit1.d;
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	//�ڻ���ESAM��֤���ᳬ��DWORD���Ʒ�Χ
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	//״̬�ж�	
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm1 > 0 )
	{
		//Alarm1>0
		if( RemainMoney > Alarm1 )
		{
			return TRUE;
		}
	}
	else
	{
		if( Alarm2 > 0 )	//Alarm1=0,Alarm2>0
		{
			if( RemainMoney > Alarm2 )
			{
				return TRUE;
			}
		}
		else if( RemainMoney > 0 )	//Alarm1=Alarm2=0
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//��������: �жϵ���Ƿ��ڱ������1״̬
//
//����: 	��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
static BOOL JudgeAlarm1( void )
{
	DWORD Alarm1, Alarm2; 
	long long RemainMoney;
	
	Alarm1 = LimitMoney.Alarm_Limit1.d;
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm1 > 0 )
	{
		if( (Alarm2 > 0)&&(Alarm1 > Alarm2) )
		{
			if( (RemainMoney > Alarm2)&&(RemainMoney <= Alarm1) )
			{
				return TRUE;
			}
		}
		else if( (RemainMoney <= Alarm1)&&(Alarm2 == 0) ) //�Ƿ���ҪRemainMoney ����0������������
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//��������: �жϵ���Ƿ��ڱ������2״̬
//
//����: 	��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
static BOOL JudgeAlarm2( void )
{
	DWORD Alarm2; 
	long long RemainMoney;
	
	Alarm2 = LimitMoney.Alarm_Limit2.d;
	
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	if( RemainMoney < 0 )
	{
		return FALSE;
	}
	if( Alarm2 > 0 )
	{
		if( (RemainMoney <= Alarm2)&&(RemainMoney > 0) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------
//��������: �жϵ���Ƿ��ڹ���״̬
//
//����: 	��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
static BOOL JudgeOvrZero( void )
{
	DWORD TickLimit;
	long long RemainMoney;
	
	TickLimit = LimitMoney.TickLimit.d;
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	
	if( TickLimit > 0 )
	{
        if( (RemainMoney <= 0)&&((RemainMoney *(-1)) < TickLimit) )
        {
            return TRUE;
        }    
	}
	return FALSE;
}

//-----------------------------------------------
//��������: �жϵ���Ƿ���͸֧״̬
//
//����: 	��
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   
//-----------------------------------------------
static BOOL JudgeOvrTick( void )//�������䣡����������
{
	DWORD TickLimit;
	long long RemainMoney;
	
	TickLimit = LimitMoney.TickLimit.d;
	RemainMoney = CurMoneyBag.Money.Remain.ll / 10000;
	
	if( TickLimit > 0 )
	{
		if( (RemainMoney < 0)&&((RemainMoney *(-1))>= TickLimit) )
		{
			return TRUE;
		}
	}
	else
	{
		if( RemainMoney <= 0)
		{
			return TRUE;
		} 
	}
	return FALSE;
}

//-----------------------------------------------
//��������: ����״̬�ж�
//
//����:    Type[in]
//           eReturnMoneyMode  -- �۷ѻ����˷Ѳ���
//           eChargeMoneyMode  -- ��ֵ����
//			 eSetParaMode	   -- ���Ĳ�������
//			 eInitMoneyMode     -- Ԥ�ò���
//         ParaType[in]        -- ����Ǹ��Ĳ��� ����������� �Ǹ��Ĳ�����������FF
//			 eTickLimit		   -- ͸֧�������
// 			 eAlarm_Limit1	   -- �������1
// 			 eAlarm_Limit2	   -- �������2
// 			 
// 			         
//����ֵ:  ����״̬
//
//��ע:   
//-----------------------------------------------
static eLOCAL_STATUS UpdateLocalStatus( eUpdataMoneyMode Type, BYTE ParaType )
{
	QWORD CloseLimit;
			
	if( Type > eInitMoneyMode)
	{
		ASSERT(FALSE,0); 
		return LocalStatus.CurLocalStatus; 
	}
	if( LocalStatus.CurLocalStatus == eOvrTick )
	{
		CloseLimit = (QWORD)LimitMoney.CloseLimit.d; 

		//��ֵ���� ���ı������1�ͱ������2 ��͸֧�������ں�բ������������բ
		if( (Type == eChargeMoneyMode)							//��ֵ����
		 ||((Type == eSetParaMode)&&((ParaType == eAlarm_Limit1) || (ParaType == eAlarm_Limit2))) )	//���ı������1�ͱ������2
		{
			if( (CurMoneyBag.Money.Remain.ll / 10000) <= (long long)CloseLimit )//�޷������з������㣬��ͳһΪ�޷���
			{
				return eOvrTick;
			}			
		}
		//�۷ѻ���͸֧
		else if( Type == eReturnMoneyMode )
		{
			return eOvrTick;
		}
		//Ԥ�ú͸���͸֧�����������ˢ��
		//����������
		
		//������Ĳ�������͸֧��� �ߵ�����˵������ ��ˢ��״̬
		if( (Type == eSetParaMode)&&(ParaType != eTickLimit) )
		{
			ASSERT(FALSE,0); 
			return LocalStatus.CurLocalStatus;
		}
	}
	
	if( JudgeOvrTick() == TRUE )
	{
		return eOvrTick;
	}

	if( JudgeOvrZero() == TRUE )
	{
		return eOvrZero;
	}
	
	if( JudgeAlarm2() == TRUE )
	{
		return eAlarm2;
	}
	
	if( JudgeAlarm1() == TRUE )
	{
		return eAlarm1;
	}
	
	if( JudgeNoAlarm()==TRUE )
	{
		return eNoAlarm;
	}
	
	ASSERT(FALSE,0); 
	return LocalStatus.CurLocalStatus; //�������״̬��û�ҵ� ����֮ǰ��״̬
}

//-----------------------------------------------
//��������: ����״̬�ж�
//
//����:    Type[in]
//           eReturnMoneyMode  -- �۷ѻ����˷Ѳ���
//			 eRChargeMoneyMode -- ��ֵ����
// 			 eSetParaMode      -- ���Ĳ�������
//			 eInitMoneyMode     -- Ԥ�ò���
//        ParaType[in]         -- ����Ǹ��Ĳ��� ����������� �Ǹ��Ĳ�����������FF
//			 eTickLimit		   -- ͸֧�������
// 			 eAlarm_Limit1	   -- �������1
// 			 eAlarm_Limit2	   -- �������2         
//����ֵ:  ����״̬
//
//��ע:   
//-----------------------------------------------
void JudgeLocalStatus( eUpdataMoneyMode Type, BYTE ParaType )
{
	eLOCAL_STATUS LocalStatusTmp;
    
	if( Type > eInitMoneyMode)
	{
		ASSERT(FALSE,1); 
		return;
	}
	
	LocalStatusTmp = UpdateLocalStatus( Type, ParaType );
	
	//͸֧״̬�ж�
	api_ClrSysStatus( eSYS_STATUS_PRE_USE_MONEY );
	if( LocalStatusTmp >= eOvrZero )
	{
		if( CurMoneyBag.Money.Remain.ll < 0 )
		{
			api_SetSysStatus( eSYS_STATUS_PRE_USE_MONEY );
			if( api_GetRunHardFlag(eRUN_HARD_TICK_FLAG) == FALSE )
			{
				api_SetRunHardFlag(eRUN_HARD_TICK_FLAG);
				api_SetFollowReportStatus(eSTATUS_TickFlag);
			}	
		}
		else
		{
			api_ClrRunHardFlag(eRUN_HARD_TICK_FLAG);
		}
	}
	else
	{
		api_ClrRunHardFlag(eRUN_HARD_TICK_FLAG);
	}
	
	//״̬�����仯 ���߳�ֵԤ�ò���
	if( (LocalStatusTmp != LocalStatus.CurLocalStatus)||(Type != eReturnMoneyMode) )
	{
		LocalStatus.LastLocalStatus = LocalStatus.CurLocalStatus;
		LocalStatus.CurLocalStatus = LocalStatusTmp;
		
		//״̬�����仯��ͱ��� Ӧ�ò���������
		if( api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.LocalStatus ), sizeof(TLocalStatus), (BYTE *)&LocalStatus.CurLocalStatus ) == TRUE )
		{
			api_Set_LocalRelayCmd( Type, LocalStatus.CurLocalStatus );
		}
		else//�������EEPROM���� �ָ�֮ǰ��״̬
		{
			LocalStatus.CurLocalStatus = LocalStatus.LastLocalStatus;
		}
	}
}

//-----------------------------------------------
//��������:  	�ϵ����
//
//����: 	    ePOWER_MODE Type
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void PowerUpMoney( ePOWER_MODE Type )
{
    VerifyMoneyPara();  //����У�������
    CheckCurMoneyBag( Type );//����У��ʣ�������ѿ۷ѵ���
    JudgePrice( Type );
    
    if( Type != ePowerWakeUpMode )//����״̬�²�ˢ�±���״̬
    {
		JudgeLocalStatus( eReturnMoneyMode, 0xFF ); //�ϵ�ˢ��һ�α���״̬
    }
}
//-----------------------------------------------
//��������: �������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void PowerDownMoney( void )
{
	SaveCurMoneyBag();
}

//-----------------------------------------------
//��������:     ����������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void MoneyMinuteTask( void )
{
    //һ����У��һ�α��ز���
    VerifyMoneyPara();
    CheckCurMoneyBag( ePowerOnMode );
}
//-----------------------------------------------
//��������:     ���������
//
//����: 	    ��
//                   
//����ֵ:  	��
//
//��ע:   	
//-----------------------------------------------
void MoneySecTask( void )
{
    //ˢ�µ�ǰ���
    if( UpdatePriceFlag != eNoUpdatePrice )
    {
        JudgePrice( ePowerOnMode );
    }

	//�������и����� �ϵ糬��30�����ű������ ����ϵ�ͬ��Ǯ��Ҳ������30�� ��Ȼ����ֿ۷Ѻ͵����޷���Ӧ�����
	//�����ж� PrePayPowerOnTimer == 0 ��Ϊ30���PrePayPowerOnTimer��һ����0�� �ᵼ��Ǯ��һ��ͬ��һ��
	if( PrePayPowerOnTimer == 1 )//�ϵ�30s��ͬ��Ǯ��
    {
		MeteringUpdateMoneybag();//ͬ��Ǯ��
    }
}

//-----------------------------------------------
//��������: ��ѿۼ�
//
//����: 	EnergyDirection[in]		���ܷ���
//			inTotalReduceEnergyNum[in]	���ۼ��ܵ��� ��λ0.01kwh
// 			inRatioReduceEnergyNum[in]	���ۼӵ�ǰ���ʵ��� ��λ0.01kwh
//����ֵ:   ��
//
//��ע:
//-----------------------------------------------
void api_ReduceRemainMoney( BYTE EnergyDirection, BYTE inTotalReduceEnergyNum, BYTE inRatioReduceEnergyNum )
{
	BYTE  	i;
	BYTE	tReduceTotalEnergyNum,tReduceRatioEnergyNum;
	DWORD	*pdw;

	if( (inTotalReduceEnergyNum == 0)&&(inRatioReduceEnergyNum == 0) )
	{
		return;
	}
	
	if( EnergyDirection == N_ACTIVE )
	{
		//��Ϊ������ܿۼ������ģʽ�����淴����ܲ�Ϊ�ӣ��򲻴���ֱ�ӷ���
		if( (FPara2->EnereyDemandMode.byActiveCalMode&0x0C) != 0x04 )
		{
			return;
		}
	}
	else
	{
		//��Ϊ������ܿۼ������ģʽ������������ܲ�Ϊ�ӣ��򲻴���ֱ�ӷ���
		if( (FPara2->EnereyDemandMode.byActiveCalMode&0x03) != 0x01 )
		{
			return;
		}
	}

	pdw = 0;
	
	//У���ѿ۷ѵ���
	CheckCurMoneyBag( ePowerOnMode );
	
	if( EnergyDirection == N_ACTIVE )
	{
		pdw = &CurMoneyBag.UserEnergy.BackwardEnergy; 
	}
	else
	{
		pdw = &CurMoneyBag.UserEnergy.ForwardEnergy; 
	}
	
	tReduceTotalEnergyNum = inTotalReduceEnergyNum;
	tReduceRatioEnergyNum = inRatioReduceEnergyNum;

	//���ݿ۷ѣ��޽���ģʽ�²��Ʒѣ�
	if( JudgeLadderMode() != eNoLadderMode )
	{
		//�������ɿ��Թ淶�������ݲ����쳣���ܵõ�����ֵʱ���γ��¼���¼���ϱ����ȴ����裬�����ݻָ�����ǰ�����طѿ����ܵ��ܱ�Ӧ�ۼ����ݵ�ۡ�
		for( i = 0; i < tReduceTotalEnergyNum; i++ )
		{
			//�õ����ȼ�һ �ٽ��е���ж� �������0.01���д����
			//�жϵ�� ���ݿ۷�ÿ�۷�һ��Ӧ���ж�һ�ε�� ��Ϊ�漰����ֵ�жϵ�����
			(*pdw)++;

			JudgePrice( ePowerOnMode );
			CurMoneyBag.Money.Remain.ll -= CurPrice.Current_Lx.d; //ʣ�������ȥ��λ���
		}
	}
	
	//�жϵ�� ���ʿ۷ѵ���ж�һ�μ���
	JudgePrice( ePowerOnMode );
	//���ʿۼ����� ���ֵ�жϴ��� ����ۼ��ܶ�� 20��
	for( i = 0; i < tReduceRatioEnergyNum; i++ )
	{
		CurMoneyBag.Money.Remain.ll -= CurPrice.Current_Fx.d; //ʣ�������ȥ��λ���
	}

	//ֻ�пۼ���Ӧ���жϱ���״̬
	JudgeLocalStatus( eReturnMoneyMode, 0xFF );
	//���¼���CRC
	CurMoneyBag.CRC32 = lib_CheckCRC32( (BYTE *)&CurMoneyBag, sizeof(TCurMoneyBag) - sizeof(DWORD) );
}

//���¶��Ǹ�����ݡ����½����л��йصĺ���
static WORD JudgeSubYearBillPara(BYTE *YearBill_Para)//�ж���������Ƿ���Ч
{
	if((YearBill_Para[2] > 23))//ʱ
	{
		return FALSE;
	}
	if(( YearBill_Para[1] > 28 ) || (YearBill_Para[1] == 0 ) ) //��
	{
		return FALSE;
	}
	if( (YearBill_Para[0] == 0) || (YearBill_Para[0] > 12) )//��
	{
		return FALSE;
	}

	return TRUE;	
}

static BYTE ExchangeYearBillPara(BYTE byOffset, BYTE *pBuf )
{
    BYTE Buf[3];
	
	if(   (pBuf[0] > pBuf[byOffset*3])//����´���
		||((pBuf[0] == pBuf[byOffset*3])&&(pBuf[1] > pBuf[byOffset*3+1]))//����µ��ڣ����մ���
		||((pBuf[0] == pBuf[byOffset*3])&&(pBuf[1] == pBuf[byOffset*3+1])&&(pBuf[2] > pBuf[byOffset*3+2]))//����¡��յ��ڣ���ʱ����
	)
	{
		memcpy( (void *)Buf, pBuf, 3 );
		memcpy( (void *)&(pBuf[0]), (void *)&(pBuf[byOffset*3]), 3 );
		memcpy( (void *)&(pBuf[byOffset*3]), (void *)Buf, 3 );
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------
//��������: ��������ǰ�ڴ����������
//
//����: 	Len�̶�Ϊ������ճ���MAX_YEAR_BILL*3 pBuf ������մ���buf
//                   
//����ֵ:   TRUE  �����˸ı�  FALSE û�����ı�
//
//��ע:     
//			
//-----------------------------------------------
static BYTE SortYearBillPara( BYTE Len, BYTE *pBuf)
{
	BYTE i, j;
	BYTE Result;
	
	Result = FALSE;
	
	if( Len != MAX_YEAR_BILL*3 )
	{
		return Result;
	}
	
	//�ж�ÿ��������յĺϷ��ԣ������Ϸ�������Ϊȫ99
	for( i=0; i<MAX_YEAR_BILL; i++ )
	{
		if( JudgeSubYearBillPara( (BYTE *)&(pBuf[i * 3]) ) == FALSE )
		{
			memset( (void *)&(pBuf[i*3]), 99, 3 );
			Result = TRUE;
		}	
	}
	
	//������������հ��մ�С�����˳������		
	for( i=0; i<MAX_YEAR_BILL; i++ )
	{
		for( j=(i+1); j<MAX_YEAR_BILL; j++ )
		{
			if( ExchangeYearBillPara( (j - i), (BYTE *)&(pBuf[i * 3]) ) )
			{
				Result = TRUE;
			}
		}
	}
	
	return Result;
}

//-----------------------------------------------
//��������: ��ʼ�����ݱ�����ģʽ
//
//����: 	��
//                   
//����ֵ:   ��
//
//��ע:     �����ϵ��ʼ����Ԥ�ÿ����õ�ǰ�׽����������
//			��������Ҫ�����ϵ��ʼ��PrepayPara�ĺ����У�����������У�����Ҫ����дee��ram�ĺ���
//-----------------------------------------------
void InitJudgeBillMode(void)
{
    eLadderMode LadderMode;
    
	//���»�õ�ǰ���ݱ�
	api_SetUpdatePriceFlag( eChangeCurLadderPrice );
	CheckLadderPriceRamData( ePowerOnMode );
	
    LadderMode = JudgeLadderMode();
	if( LadderMode == eLadderYearMode )
	{
		//��ģʽ
		api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
	}
	else if( LadderMode == eLadderMonMode )
	{
		//��ģʽ
		api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
	}
	else
	{
		//��ģʽ
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
	}
}
//-----------------------------------------------
//��������: �ϵ����ݱ����л����³�ʼ�����ݱ�����ģʽ
//
//����: 	��
//                   
//����ֵ:   
//			�½��㵽����� //ת�����õ��� //ȫ����������õ���
//			������ոı� 	 //ת�����õ���	
//			����㵽�½��� //ת�����õ���	//ת�����õ���			
//
//��ע:		����ģʽ�л� ����� �½���
//			��Ҫ�������½���ģʽ�л������
//			1�����������ն�ͨ���û����������õ�ǰ�׽��ݱ�ʱҪ�ж�
//			2���ѿ�ģʽ�����л�ʱҪ�ж�
//			3�����ñ����׽���ʱ����ʱ����Ҫ�жϲ��õ�ǰ�ڼ��׽��ݱ����ж����½����л�
//			4�����ñ����׽��ݱ�ʱ����Ҫ�ж����½����л�
//			5������ʱ�����ڲ������л�ʱ���ҵ���ǰ���ݲ������ױ����ж����½����л�
//			6�����õ�ǰ�׽���ʱ����ʱ����Ҫ�ҵ���ǰ���ݲ������ױ����ж����½����л�
//			7�����ñ����׽���ʱ���������׽��ݱ�ʱ�����ݴ�����������ж��Ƿ�������½����л�
//			8: ������շ����仯����������ս����л�
//-----------------------------------------------
void ProcLadderModeSwitch(void)//���㷽ʽ�任�жϼ�����
{
	TBackupYearBill  BackupYearBillBuf,CurYearBillBuf;
	eLadderMode LadderMode; 
	
	LadderMode = JudgeLadderMode();
	
	if( LadderMode == eLadderYearMode )
	{
		//�½��㵽�����
		if( api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == TRUE )
		{			
			api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
			api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );

			//����ǰ���ݽ����ձ����ڱ����У����´θ��Ľ��ݽ�����ʱ�������бȽ��Ƿ��б仯
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else if( 	(api_GetRunHardFlag(eRUN_HARD_YEAR_LADDER_MODE) == FALSE) 
				 && (api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == FALSE) )//��ģʽ�������
		{
			api_SetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
			
			//�嵱ǰ���ݽ����ձ���
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else //������ոı�
		{
			//����������ϴ�������գ��뵱ǰ�Ľ��бȽϣ�����һ����Ҫת����
		 	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&BackupYearBillBuf.YearBill ); 		
			//������յ��жϱ������� ��������һ�� ����˳��ͬ������
			SortYearBillPara( sizeof(TBackupYearBill) - sizeof(DWORD), (BYTE *)&BackupYearBillBuf.YearBill );

			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			SortYearBillPara( sizeof(TBackupYearBill) - sizeof(DWORD), (BYTE *)&CurYearBillBuf.YearBill );

			//����ǰ�׽��ݽ������뱸���׽��ݽ�������ͬ����ֱ�ӷ���
			if( memcmp( (BYTE *)&BackupYearBillBuf, (BYTE *)&CurYearBillBuf.YearBill, sizeof(TBackupYearBill) - sizeof(DWORD) ) == 0 )
			{
				return;
			}
			
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );					
		}
	}
	else if( LadderMode == eLadderMonMode )
	{
		//����㵽�½���
		if( api_GetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE ) == TRUE )
		{	
			//����ǰ���ݽ����ձ����ڱ����У����´θ��Ľ��ݽ�����ʱ�������бȽ��Ƿ��б仯
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );

			//���½���ģʽ
			api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
			api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		}
		else if( 	(api_GetRunHardFlag(eRUN_HARD_YEAR_LADDER_MODE) == FALSE) 
				 && (api_GetRunHardFlag(eRUN_HARD_MON_LADDER_MODE) == FALSE) )//��ģʽ�������
		{
			api_SetRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
			
			//�嵱ǰ���ݽ����ձ���
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
		else //�½����ոı�
		{
			//����������ϴ�������գ��뵱ǰ�Ľ��бȽϣ�����һ����Ҫת����
		 	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&BackupYearBillBuf.YearBill ); 
			
			//�½�����ֻ�жϵ�һ�������ռ���
			if( memcmp( (BYTE *)&BackupYearBillBuf, (BYTE *)&CurLadderTable.YearBill, sizeof(CurLadderTable.YearBill[0]) ) == 0 )
			{
				return;
			}
			memcpy((BYTE *)&CurYearBillBuf.YearBill,(BYTE *)&CurLadderTable.YearBill,sizeof(TBackupYearBill)-sizeof(DWORD));
			api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );
		}
	}
	else
	{
		if( (api_GetRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE ) == FALSE)
			&&(api_GetRunHardFlag( eRUN_HARD_MON_LADDER_MODE ) == FALSE) )
		{
			return;
		}

		//���޽���ģʽ
		api_ClrRunHardFlag( eRUN_HARD_YEAR_LADDER_MODE );
		api_ClrRunHardFlag( eRUN_HARD_MON_LADDER_MODE );
		
		//�嵱ǰ���ݽ����ձ���
		memset((BYTE *)&CurYearBillBuf.YearBill,0xFF,sizeof(TBackupYearBill)-sizeof(DWORD));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.BackupYearBill ), sizeof(TBackupYearBill), (BYTE *)&CurYearBillBuf.YearBill );	
	}

	api_SetFreezeFlag( eFREEZE_LADDER_CLOSING, 0 );
}

//-----------------------------------------------
//��������: ת���������ESAMǮ��ͬ����
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע: 
//-----------------------------------------------
void api_SwapMoney( void )
{
    CheckCurMoneyBag( ePowerOnMode );//У��ʣ�������ѿ۷ѵ���
    SaveCurMoneyBag();//����ʣ����
    MeteringUpdateMoneybag();//ͬ��Ǯ��
}

//-----------------------------------------------
//��������: ��ȡ��ǰ�Ľ��ݽ����ձ�
//
//����: 		LadderBillingPara[out]:���ݽ����ձ�
//
//����ֵ:		����ģʽ
//			eNoLadderMode	--�޽���ģʽ
//			eLadderMonMode	--�½���ģʽ
//			eLadderYearMode	--�����ģʽ
//
//��ע:
//-----------------------------------------------
BYTE api_GetCurLadderYearBillInfo(TLadderBillingPara* LadderBillingPara)
{
	BYTE i,j;
	eLadderMode LadderMode; 
		
	CheckLadderPriceRamData( ePowerOnMode );
	LadderMode = JudgeLadderMode();
	memset( LadderBillingPara, 0xff, sizeof(TLadderBillingPara) );
	
	//�����һ���ݽ���������Ч,���ա�ʱ��Ч ˵�����½���ģʽ ֻ����ʱ����
	if( LadderMode == eLadderMonMode )
	{
		//���ݽ����պϷ�
		if( ( CurLadderTable.YearBill[0][1] >= 1 ) && ( CurLadderTable.YearBill[0][1] <= 28)
		 &&( CurLadderTable.YearBill[0][2] >= 0 ) && ( CurLadderTable.YearBill[0][2] <= 23))
		{
			LadderBillingPara->LadderSavePara[0].Day = CurLadderTable.YearBill[0][1] ;
			LadderBillingPara->LadderSavePara[0].Hour = CurLadderTable.YearBill[0][2] ;
		}
	}
	else if( LadderMode == eLadderYearMode ) //��ģʽ
	{
		j = 0;
		for( i = 0; i < MAX_YEAR_BILL; i++ )
		{
			//���ݽ����պϷ�
			if( ( CurLadderTable.YearBill[i][0] >= 1 ) && ( CurLadderTable.YearBill[i][0] <= 12)
			 &&( CurLadderTable.YearBill[i][1] >= 1 ) && ( CurLadderTable.YearBill[i][1] <= 28)
			 &&( CurLadderTable.YearBill[i][2] >= 0 ) && ( CurLadderTable.YearBill[i][2] <= 23))
			{
				memcpy( (BYTE *)&LadderBillingPara->LadderSavePara[j], (BYTE *)&CurLadderTable.YearBill[i][0], sizeof(TLadderSavePara) );
				j++;
			}
		}
	}
    else    //��ģʽ
    {
        
    }
	//���ؽ��ݽ���ģʽ
    return LadderMode;  
}
//-----------------------------------------------
//��������: ��E2��ȡ��ǰ���ݱ�����ǰ���ݱ�
//
//����: 	
//
//����ֵ:	

//
//��ע:ֻ�������Y�ã��ڳ�ʼ������ǰ��ȡ����ֹ��������ݽ��㽫E2�е�ǰ���ݱ���
//-----------------------------------------------
void api_ReadPowerLadderTable(void)
{
	api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&PowerdownLadderTable );
}
//-----------------------------------------------
//��������: ��ȡ����ǰ�Ľ��ݽ����ձ�
//
//����: 		LadderBillingPara[out]:���ݽ����ձ�
//
//����ֵ:		����ģʽ
//			eNoLadderMode	--�޽���ģʽ
//			eLadderMonMode	--�½���ģʽ
//			eLadderYearMode	--�����ģʽ
//
//��ע:
//-----------------------------------------------
BYTE api_GetPowerDownYearBill(TLadderBillingPara* LadderBillingPara)
{
	BYTE i,j;
	eLadderMode LadderMode = eNoLadderMode;
	//�ڴ�У����� ,��EEPROM��ȡ��ǰ���ݱ�
	if( PowerdownLadderTable.CRC32 != lib_CheckCRC32((BYTE*)&PowerdownLadderTable,sizeof(TLadderPrice)-sizeof(DWORD)) )
	{
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( PrePaySafeRom.Price.CurrentLadder ), sizeof(TLadderPrice), (BYTE *)&PowerdownLadderTable );
	}
	memset( LadderBillingPara, 0xff, sizeof(TLadderBillingPara) );
	j = 0;
	//������ 0--�� 1--��  2--ʱ
	for( i = 0; i < MAX_YEAR_BILL; i++ )
	{
		if( (PowerdownLadderTable.YearBill[i][1] <= 28)&&(PowerdownLadderTable.YearBill[i][1] > 0) //��
			&&(PowerdownLadderTable.YearBill[i][2] <= 23) ) //ʱ
		{
			if( (PowerdownLadderTable.YearBill[i][0] <= 12)&&(PowerdownLadderTable.YearBill[i][0] > 0) ) //��
			{
				LadderMode = eLadderYearMode;//��ģʽ
				memcpy( (BYTE *)&LadderBillingPara->LadderSavePara[j], (BYTE *)&PowerdownLadderTable.YearBill[i][0], sizeof(TLadderSavePara) );
				j++;
			}
			else if( i == 0 ) //�����һ���ݽ����� ����Ч ˵�����½���ģʽ ֻ����ʱ����
			{
				LadderMode = eLadderMonMode;//��ģʽ
				LadderBillingPara->LadderSavePara[0].Day = PowerdownLadderTable.YearBill[0][1] ;
				LadderBillingPara->LadderSavePara[0].Hour = PowerdownLadderTable.YearBill[0][2] ;
				break;
			}
		}
	}

	//���ؽ��ݽ���ģʽ
    return LadderMode;  
}

//-----------------------------------------------
//��������: �õ���ǰ����״̬
//
//����: 	��
//
//����ֵ:  	���ر���״̬
//
//��ע:   	���ڼ̵�������״̬��ѯ
//-----------------------------------------------
eLOCAL_STATUS api_GetLocalStatus( void )
{
	return LocalStatus.CurLocalStatus;
}

//-----------------------------------------------
//��������: ���ۼƹ�����
//
//����:  		Buf[out] ����ۼƹ�����
//
//����ֵ:		��
//
//��ע:
//-----------------------------------------------
void api_ReadTotalBuyMoney( BYTE *Buf )
{
	api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( PrePayConMem.dwTotalBuyMoney ), sizeof(DWORD), Buf );
	//lib_InverseData(Buf, 4 );�����ݶ�������Э��㻹Ҫ���� Exchange ���˴����ø��ֽ���Buf[0]
}

//---------------------------------------------------------------
//��������: ��ȡ��ǰ���ڵڼ����ݡ���ǰ�Ƿ��б����׽��ݱ���ǰ�Ƿ��б����׷���
//
//����: 	Type
//            eInfoCurLadder - ��ȡ��ǰ��������
//            eInfoBackupRateFalg - �Ƿ��б����׷���
//            eInfoBackupLadderFlag - �Ƿ��б����׽���
//
//����ֵ:  	0 	���������ڵĽ���  0xff����ʾ���ݷ���
//			1  ����0xff����ʾ true������ FALSE ������
//          2  ����0xff����ʾ true������ FALSE ������
//
//
//��ע:   0xff��ʾԶ��ģʽ����ʾ���н��ݡ�������صķ���
//---------------------------------------------------------------
BYTE api_GetLocalInfo( eLocalInfo Type )
{
	BYTE ReturnData;

	switch( Type )
	{
		case eInfoCurLadder:    	//��ȡ��ǰ��������
			if( CurPrice.Current_Ladder != 0xff )
			{
				ReturnData = ( CurPrice.Current_Ladder + 1 );
			}
			else
			{
				ReturnData = 0xff;
			}
			break;
		case eInfoBackupRateFalg:   // �Ƿ��б����׷���
			if( api_GetRunHardFlag( eRUN_HARD_SWITCH_FLAG3 ) == TRUE )
			{
				ReturnData = TRUE;
			}
			else
			{
				ReturnData = FALSE;
			}
			break;
		case eInfoBackupLadderFlag: //�Ƿ��б����׽���
			if( CurPrice.Current_Ladder != 0xff ) //ֻ�е�ǰ���ݵ����Чʱ����ʾ
			{
				if( api_GetRunHardFlag( eRUN_HARD_SWITCH_FLAG4 ) == TRUE )
				{
					ReturnData =  TRUE;
				}
				else
				{
					ReturnData = FALSE;
				}
			}
			else
			{
				ReturnData = 0xff;
			}
			break;
	}
		
	return ReturnData;
}

#endif//#if( PREPAY_MODE == PREPAY_LOCAL )
