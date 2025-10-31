//----------------------------------------------------------------------
//Copyright (C) 2003-2021 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���
//��������	2021.7.27
//����		��λ�ѱ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"
#include "./GDW698/GDW698.h"
#include "./GDW698/GDW698DataDef.h"

#if (SEL_SEARCH_METER == YES )
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define COLLECT_METER_RETRY_TIMES	  2       // �ɱ����Դ���
#define EXEPCT_MESSAGE_LENGTH_64507   (24+4)
#define EXEPCT_MESSAGE_LENGTH_698     (39+4)
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

TSchedMeterInfo  tSearchedMeter[SEARCH_METER_MAX_NUM];
#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
FindUnknownMeterEventData_t  gFindUnknownMeterEventDataRAM = {0};
BOOL FindUnknownMeterEventFlag = FALSE;
#endif
TSKMeter SKMeter;                                       // �ѱ���̱���
TCycleSInfoRom tCycleSInfoRom;                          // �����ѱ����
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
//-----------------------------------------------
// ��������: Ч�鵱ǰ�����ѱ������RAM����
//
// ����:
//
// ����ֵ:  	��
//
// ��ע: ÿСʱЧ��RAM����
//-----------------------------------------------
void CheckCycleSInfo(void)
{
	// ���ӵ�ǰ����
	if (lib_CheckSafeMemVerify((BYTE *)&tCycleSInfoRom, sizeof(TCycleSInfoRom), UN_REPAIR_CRC) == FALSE)
	{
		// ��EEProm�ָ�����
		if (api_VReadSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) != TRUE)
		{
			memcpy((BYTE *)&tCycleSInfoRom, (BYTE *)&CycleSInfoRomConst, sizeof(TCycleSInfoRom));
			api_SetError(ERR_CYCLESINFO_DATA);
		}
	}
	else
	{
		api_ClrError(ERR_CYCLESINFO_DATA);
	}
}
//-----------------------------------------------
//��������: �жϵ�ַ�Ƿ���Ч
//
//����: addr[in]
//
//����ֵ: TRUE FALE
//	��
//
//��ע:
//-----------------------------------------------
static BOOL IsAddrVaild(BYTE *Addr)
{
    if( (Addr == NULL)
        || lib_IsAllAssignNum(Addr,0x88,6) 
        || lib_IsAllAssignNum(Addr,0x99,6) 
        || (!lib_IsMultiBcd(Addr,6)
        )
    )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//-----------------------------------------------
//��������: �жϲ������Ƿ���Ч
//
//����: byBaud[in]
//
//����ֵ: TRUE FALE
//	��
//
//��ע:
//-----------------------------------------------
static BOOL IsBaudValid(BYTE byBaud)
{
	switch(byBaud)
	{
        // case 0: //300
        // case 1: //600
        // case 2: //1200
        case 3: //2400
        case 4: //4800
        // case 5: //7200
        case 6: //9600
        // case 7: //19200
        // case 8: //38400
        // case 9: //57600
        case 10://115200
        //CAN


            return TRUE;
            break;
        default: 
            return FALSE;
            break;
	}
}
//-----------------------------------------------
//��������: �ж�Э���Ƿ���Ч
//
//����: byProtocolType[in]��Э������
//
//����ֵ: TRUE FALE
//
//��ע:
//-----------------------------------------------
static BOOL IsProtocolValid(BYTE byProtocolType)
{
	switch(byProtocolType)
	{
        // case ePROTOCOL_645_97:
        case ePROTOCOL_645_07:
        case ePROTOCOL_698:
            return TRUE;
            break;
        default: 
            return FALSE;
            break;
	}

}
static BOOL IsPortValid(DWORD ChOad)
{
    switch(ChOad)
	{
        case SEARCH_METER_CH_OAD:
        // case CAN_CH_OAD:
            return TRUE;
            break;
        default: 
            return FALSE;
            break;
	}
}
//-----------------------------------------------
//��������: �ж�ָ�����ѱ����Ƿ���Ч
//
//����: byNo[in]:�ѱ�������
//
//����ֵ: TRUE FALE
//
//��ע:
//-----------------------------------------------
static BOOL IsMeterValid(BYTE byNo)
{
	BYTE byI;
	
    if (byNo >= SEARCH_METER_MAX_NUM)
    {
		return FALSE;
    }

    if( (IsAddrVaild(tSearchedMeter[byNo].Addr) == FALSE) || 
        (IsProtocolValid(tSearchedMeter[byNo].byProtocolType) == FALSE) ||
        (IsBaudValid(tSearchedMeter[byNo].byBaud) == FALSE) ||
        (IsPortValid(tSearchedMeter[byNo].ChOad) == FALSE) ||
        (api_CheckClock(&tSearchedMeter[byNo].Time) == FALSE)
        )
    {
        return FALSE;
    }

	return TRUE;
}

// //--------------------------------------------------
// //��������:  ��ȡ�ѱ�����һ������λ�õ�����
// //         
// //����:      
// //         
// //����ֵ:  ����  
// //         
// //��ע:  
// //--------------------------------------------------
// static BYTE GetMeterIndexFirstFree(void)
// {
// 	BYTE byI;
	
// 	for(byI=0;byI<SEARCH_METER_MAX_NUM;byI++)
// 	{
// 		if( IsMeterValid(byI)  == FALSE )
// 		{
// 			return byI;
// 		}
// 	}
// 	return byI;//û�ѵ��ͷ������ֵ��
// }

//--------------------------------------------------
//��������:  ͨ�����ַ��ȡindex
//         
//����: pbuf[in];���ַ
//         
//����ֵ:   ����
//         
//��ע:  
//--------------------------------------------------
static BYTE GetMeterIndexByAddr(BYTE* pbuf)
{
	BYTE byI;
	
    if(pbuf == NULL)
    {
        return SEARCH_METER_MAX_NUM;
    }

	for(byI=0;byI<SEARCH_METER_MAX_NUM;byI++)
	{
		if( IsMeterValid(byI) && (memcmp(pbuf,(BYTE*)&tSearchedMeter[byI].Addr,6) == 0) )
		{
			return byI;
		}
	}
	return byI;//û�ѵ��ͷ������ֵ��
}

// //-----------------------------------------------
// //��������: ����ϵͳ��N����Ч�ĵ����ţ������±꣩
// //
// //����: byN - �� N ��
// //
// //����ֵ: BYTE 
// //
// //��ע:
// //-----------------------------------------------
// static BYTE GetValidMeterIndex(BYTE byN)
// {
//     BYTE byI,byNum = 0;
	
//     for (byI = 0; byI <SEARCH_METER_MAX_NUM; byI++)
// 	{
// 		if ( IsMeterValid(byI) )
// 		{
// 			if (++byNum == byN)
// 			{
// 				return byI;
// 			}
// 		}
// 	}
//     return 0xFF;	
// }

// static BYTE SwitchToNext(BYTE byStartNo, TSchedMeterInfo *pMetInfo)
// {
//     BYTE byI;

// 	if(pMetInfo == NULL)
//     {
//         return 0xFF;
//     }

//     for (byI = byStartNo; byI <SEARCH_METER_MAX_NUM; byI++)
// 	{
// 		if ( IsMeterValid(byI) )
// 		{
// 			memcpy(pMetInfo, &tSearchedMeter[byI], sizeof(TSchedMeterInfo));
// 			return byI;
// 		}
// 	}
//     return 0xFF;
// }

//-----------------------------------------------
//��������: �жϸñ��ַ�Ƿ����ѱ�����
//
//����: pAddr[IN] ���ַ
//     pBaud[OUT] ͨѶ����
//
//����ֵ: BOOL
//
//��ע:
//-----------------------------------------------
// static BOOL IsExistMeter(BYTE *pAddr,BYTE *pBaud)
// {
// 	BYTE byI;
	
//     if((pAddr == NULL) || (pBaud == NULL))
//     {
//         return FALSE;
//     }

// 	for(byI=0;byI<SEARCH_METER_MAX_NUM;byI++)
// 	{
// 		if( IsMeterValid(byI) )
// 		{
// 			if ( memcmp(pAddr, tSearchedMeter[byI].Addr,6) == 0 )
// 			{
// 				if (pBaud != NULL)
// 				{
// 					*pBaud = tSearchedMeter[byI].byBaud;
// 				}
// 				return TRUE;
// 			}
// 		}
// 	}
// 	return FALSE;
// }
static void UpdateArchives(void)
{
    TOad60010200 cfg ={0};
    BYTE i,j;
    BOOL updateFlag;
    DWORD MeterVaildFlag = 0;
    
    if(tCycleSInfoRom.AutoUpdateArchives != TRUE)
    {
        return;
    }

    if(api_GetMeterNumInSys() == 0)
    {
        return;
    }

    for(i = 0;i < SEARCH_METER_MAX_NUM;i++)
    {
        if( IsMeterValid(i) )
        {
            MeterVaildFlag |= (1<<i);
        }
    }

    //���µ���
    for(j = 1;j < MAX_MP_NUM;j++)
    {
        if(MP_IsValid(j))
        {
            GetMP698Para(j,&cfg);

            for(i = 0;i < SEARCH_METER_MAX_NUM;i++)
            {
                if( MeterVaildFlag & (1<<i) )
                {
                    if(memcmp(cfg.basic.tsa.addr,tSearchedMeter[i].Addr,6) != 0)
                    {
                        continue;
                    }
                    //�ѱ�����Ӧλ��Ϊ0
                    MeterVaildFlag &= ~(1<<i);

                    if(cfg.basic.port.value != tSearchedMeter[i].ChOad)
                    {
                        cfg.basic.port.value= tSearchedMeter[i].ChOad;
                        updateFlag = TRUE;
                    }

                    if(cfg.basic.protocol != tSearchedMeter[i].byProtocolType)
                    {
                        cfg.basic.protocol = tSearchedMeter[i].byProtocolType;
                        updateFlag = TRUE;
                    }

                    if(cfg.basic.baud != tSearchedMeter[i].byBaud)
                    {
                        cfg.basic.baud = tSearchedMeter[i].byBaud;
                        updateFlag = TRUE;
                    }
					
                    #if (cSPECIAL_CLIENTS == SPECIAL_GW_AN_HUI)
                    if(cfg.basic.userType != 99)
                    {
                        cfg.basic.userType  = 99;
                        updateFlag = TRUE;
                    }
					#endif
					
                    if(updateFlag)
                    {
                        add_mp_cfg(cfg.nIndex,&cfg);
                    }

                    break;
                }
            }
        }
    }

    //��ӵ���
    j = 1;
    for(i = 0;i < SEARCH_METER_MAX_NUM;i++)
    {
        if( MeterVaildFlag & (1<<i) )
        {
            for(;j < MAX_MP_NUM;j++)
            {
                if(MP_IsValid(j) == FALSE)
                {
                    // MeterVaildFlag &= ~(1<<i);
                    memset((BYTE*)&cfg,0x00,sizeof(TOad60010200));
                    cfg.nIndex = j;
                    cfg.basic.tsa.AF = 5;
                    memcpy(cfg.basic.tsa.addr,tSearchedMeter[i].Addr,6);
                    cfg.basic.baud = tSearchedMeter[i].byBaud;
                    cfg.basic.protocol = tSearchedMeter[i].byProtocolType;
                    cfg.basic.port.value = tSearchedMeter[i].ChOad;
                    #if (cSPECIAL_CLIENTS == SPECIAL_GW_AN_HUI)
                    cfg.basic.userType  = 99;
					#endif
                    add_mp_cfg(cfg.nIndex,&cfg);
                    j++;
                    break;
                }
            }
        }
    }
}

#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
BOOL api_GetFindUnknownMeterEventStatus(BYTE Phase)
{
    if(tCycleSInfoRom.SearchMeterEventFlag == TRUE)
    {
        return FindUnknownMeterEventFlag;
    }
    return FALSE;
}
//-----------------------------------------------
//��������: �����ֵ�δ֪���ܱ�ת��Ϊ698Э����ѱ���
//
//����: InBuf[in]: �ѱ���
//      OutBuf[out]: ���Buf
//����ֵ: ���ݳ���
//
//��ע: 
//-----------------------------------------------
WORD api_GetFindUnknownMeter(FindUnknownMeterEventData_t* InBuf,BYTE* OutBuf)
{
    BYTE i;
    WORD wLen = 0;

    if( (InBuf == NULL) || (OutBuf == NULL) || (InBuf->MeterNum>FIND_UNKNOWN_METER_MAX_NUM))
    {
        return 0;
    }

    OutBuf[wLen++] = Array_698;
    OutBuf[wLen++] = InBuf->MeterNum;

	for (i = 0;  i < InBuf->MeterNum; i++)
	{
        OutBuf[wLen++] = Structure_698;
        OutBuf[wLen++] = 0x07;
        //ͨ�ŵ�ַ
        OutBuf[wLen++] = TSA_698;
        OutBuf[wLen++] = 0x07;
        OutBuf[wLen++] = 0x05;
        lib_ExchangeData( OutBuf+wLen, InBuf->SearchResult[i].Addr, 6 );
        wLen += 6;
        //�ɼ�����ַ
        OutBuf[wLen++] = TSA_698;
        OutBuf[wLen++] = 0x07;
        OutBuf[wLen++] = 0x05;
        memcpy( OutBuf+wLen, FPara1->MeterSnPara.CommAddr, 6 );
        wLen += 6;
        //��Լ����
        OutBuf[wLen++] = Enum_698;
        OutBuf[wLen++] = InBuf->SearchResult[i].byProtocolType;
        //��λ
        OutBuf[wLen++] = Enum_698;
        OutBuf[wLen++] = 0;
        //�ź�Ʒ��
        OutBuf[wLen++] = Unsigned_698;
        OutBuf[wLen++] = 0;
        //�ѵ���ʱ��
        OutBuf[wLen++] = DateTime_S_698;
        memcpy( OutBuf+wLen, (BYTE*)&InBuf->SearchResult[i].Time, sizeof(InBuf->SearchResult[i].Time));
        lib_InverseData(OutBuf+wLen,2); //��
        wLen += sizeof(InBuf->SearchResult[i].Time);
        //������Ϣ
        OutBuf[wLen++] = Array_698;
        OutBuf[wLen++] = 0x00;
	}

	return wLen;
}
#endif
//-----------------------------------------------
//��������: ����ָ��RAM�е��ѱ�����FLASH
//
//����: byMetIndex[in]������
//
//����ֵ: TRUE FALSE
//
//��ע:
//-----------------------------------------------
static BOOL api_SaveSchedMeter(BYTE byMetIndex)
{
	if (byMetIndex >= SEARCH_METER_MAX_NUM)
    {
		return FALSE;
    }
    
	return api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[byMetIndex]), sizeof(TSchedMeterInfo), (BYTE *)&tSearchedMeter[byMetIndex]);
}

//-----------------------------------------------
//��������: ��֤֮ǰ�ѵ����ı��ַ
//
//����: 
//
//����ֵ: FALSE ��֤��ɣ�TRUE ��֤�� 
//
//��ע:
//-----------------------------------------------
static BOOL CheckFlashMeter( BYTE SerialNo )
{
    TOad60010200 cfg ={0};
    static TSchedMeterInfo TmpMeterResult;
	BYTE i;
	
	if (SKMeter.byComplete)
    {
		return FALSE;
    }

    SKMeter.byAutoAck = 0;
    SKMeter.dwRxCnt = 0;
    
    //��֤�ѱ���
	if ( (SKMeter.CheckStep & BIT15 ) == 0)
	{
        if(SKMeter.CheckStep == 0x00FF)
        {
            //��֤��һ���ѱ���
            SKMeter.CheckStep = 0;
        }
        else if(SKMeter.CheckStep < SEARCH_METER_MAX_NUM)
        {
            if(!SKMeter.byRxOK)
            {
                tSearchedMeter[SKMeter.CheckStep].byBaud = 0;
                tSearchedMeter[SKMeter.CheckStep].byProtocolType = 0xFF;
                tSearchedMeter[SKMeter.CheckStep].ChOad = 0x00000000;
                memset((BYTE*)&tSearchedMeter[SKMeter.CheckStep].Time,0xFF,sizeof(tSearchedMeter[SKMeter.CheckStep].Time));
                api_SaveSchedMeter(SKMeter.CheckStep);
            }
            SKMeter.CheckStep++;
        }

        if( SKMeter.CheckStep >= SEARCH_METER_MAX_NUM)
        {
            //��֤�ѱ������
            SKMeter.CheckStep = BIT15;
        }
        else
        {
            for(; SKMeter.CheckStep < SEARCH_METER_MAX_NUM; SKMeter.CheckStep++)
            {
				//����֤CANͨ���ļ����ѱ���
                if( IsMeterValid(SKMeter.CheckStep) && (tSearchedMeter[SKMeter.CheckStep].ChOad == SEARCH_METER_CH_OAD) )
                {
                    break;
                }
            }

            if(SKMeter.CheckStep < SEARCH_METER_MAX_NUM)
            {
                memcpy(&TmpMeterResult,&tSearchedMeter[SKMeter.CheckStep],sizeof(TSchedMeterInfo));
            }
            else
            {
                //��֤�ѱ������
                SKMeter.CheckStep = BIT15;
            }
        }
	}

	if ( (SKMeter.CheckStep & BIT15 ) != 0)//��֤�ѱ�ͨ���µĵ���
    {
        if( (SKMeter.CheckStep >= (BIT15|MAX_MP_NUM) ) || (api_GetMeterNumInSys() == SEARCH_METER_MAX_NUM))
        {
            SKMeter.byComplete = 1;
            SKMeter.CheckStep = 0x00FF;
            SKMeter.byRxOK = 0;
            return FALSE;
        }

        i = SKMeter.CheckStep & (~BIT15);

        if( (i != 0) && (SKMeter.byRxOK != 0) )
        {
            api_Addr2MeterNo(TmpMeterResult.Addr,TmpMeterResult.byProtocolType,TmpMeterResult.byBaud,SEARCH_METER_CH_OAD,TRUE);
        }

        i++;

        for( ;i < MAX_MP_NUM;i++)
        {
            if(MP_IsValid(i))
            {
                GetMP698Para(i,&cfg);
                if( (cfg.basic.port.value == SEARCH_METER_CH_OAD)
                    &&(IsAddrVaild(cfg.basic.tsa.addr) == TRUE)  
                    &&(IsProtocolValid(cfg.basic.protocol) == TRUE) 
                    &&(IsBaudValid(cfg.basic.baud) == TRUE )
                    &&(GetMeterIndexByAddr(cfg.basic.tsa.addr) >= SEARCH_METER_MAX_NUM) //�ѱ�����û�������ַ������
                )
                {
                    memcpy(TmpMeterResult.Addr,cfg.basic.tsa.addr,6);
                    TmpMeterResult.byProtocolType = cfg.basic.protocol;
                    TmpMeterResult.byBaud = cfg.basic.baud;
					break;
                }
                else
                {
                    continue;
                }
            }
        }

        if( i < MAX_MP_NUM)
        {
            SKMeter.CheckStep = BIT15|i;
        }
        else
        {
            //�ѱ����
            SKMeter.byComplete = 1;
            SKMeter.CheckStep = 0x00FF; 
            SKMeter.byRxOK = 0;
		    return FALSE;
        }
    }
	
    if(TmpMeterResult.byProtocolType == ePROTOCOL_698)
	{
		if(!IsBaudValid(TmpMeterResult.byBaud))
		{
			TmpMeterResult.byBaud = 6; //9600
		}
        Tx_Collect_Meter(SerialNo,ePROTOCOL_698,TmpMeterResult.Addr,SEARCH_METER_698_OAD,TmpMeterResult.byBaud);
	}
	else if (TmpMeterResult.byProtocolType == ePROTOCOL_645_07)
	{
		if(!IsBaudValid(TmpMeterResult.byBaud))
		{
			TmpMeterResult.byBaud = 3; //2400
		}
		Tx_Collect_Meter(SerialNo,ePROTOCOL_645_07,TmpMeterResult.Addr,SEARCH_METER_654_07_OAD,TmpMeterResult.byBaud);
	}
	else
	{
		if(!IsBaudValid(TmpMeterResult.byBaud))
		{
			TmpMeterResult.byBaud = 2; //1200
		}
		Tx_Collect_Meter(SerialNo,ePROTOCOL_645_97,TmpMeterResult.Addr,SEARCH_METER_654_97_OAD,TmpMeterResult.byBaud);
	}
    SKMeter.byRxOK = 0;
	return TRUE; 
}
//-----------------------------------------------
//��������: �л�����һ�������ʽ����ѱ�
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
static void SwitchNextBaud(void)
{
    for(BYTE byI=0; byI<5; byI++)
    {
        if(SKMeter.byAutoBaud & (1<<byI))
        {
            SKMeter.byAutoBaud &= ~(1<<byI);
            break;
        }
    }

    memset(SKMeter.AA,0,6);

    SKMeter.byAATry    = 1;
	SKMeter.byAATryNum = 1;
	SKMeter.byFirst    = 1;
}
//-----------------------------------------------
//��������: �л�����һ��Э������ѱ�
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
static void SwitchNextProtocol(void)
{
    for(BYTE byI=0; byI<3; byI++)
    {
        if(SKMeter.byAutoCheck & (1<<byI))
        {
            SKMeter.byAutoCheck &= ~(1<<byI);
            break;
        }
    }

    if(SKMeter.byAutoCheck != 0)
    {
        SKMeter.byAutoBaud = 0x1F;
    }
    else
    {
        SKMeter.byAutoBaud = 0;
    }

    memset(SKMeter.AA,0,6);
    SKMeter.byAATry    = 1;
	SKMeter.byAATryNum = 1;
	SKMeter.byFirst    = 1;
}
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �ϵ��EEP��ȡ�ѱ������ѱ������RAM
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
void api_PowerUpSchClctMeter(void)
{
    BYTE Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20];
	WORD Len;
	DWORD dwAddr;

	// ��EE�ж�ȡ�ѱ���Ϣ
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);

	memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));

    CheckCycleSInfo();

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );
    // ����δ֪���ܱ��¼�����
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
    //����ǰ�¼�δ��ʼ������δ֪���ܱ����
    if( ( ( Buf[eSUB_FIND_UNKNOWN_METER/8] & (0x01<<(eSUB_FIND_UNKNOWN_METER%8)) ) == 0 )
      &&(gFindUnknownMeterEventDataRAM.MeterNum != 0)
    )
    {
        memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
        api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
    }
	#endif
}

//-----------------------------------------------
//��������: ���籣��
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
void api_PowerDownSchClctMeter(void)
{
    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    if( (tCycleSInfoRom.SearchMeterEventFlag != TRUE) || (gFindUnknownMeterEventDataRAM.MeterNum == 0) )
    {
        memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
    }
    // �������ʱ��δ���ɷ���δ֪���ܱ��¼�����
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
    #endif
}
//-----------------------------------------------
//��������: ����δ֪���ܱ��¼���ʼ��
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
void api_FindUnknownMeterEventPara(void)
{
    DWORD Inpara = 0;
    api_WriteEventAttribute(0x3111, 0xff, (BYTE *)&Inpara, 0);
}
//-----------------------------------------------
//��������: ����ѱ���
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
void api_ClearSchMeter(void)
{
    memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));

	memset((BYTE *)&tSearchedMeter[0],0xff,sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM);
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    FindUnknownMeterEventFlag = FALSE;
	#endif

}
//-----------------------------------------------
//��������: �ѱ��ʼ��
//
//����: 
//
//����ֵ��
//
//��ע:
//-----------------------------------------------
void api_InitSchClctMeter(void)
{
	api_ClearSchMeter();

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
    api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
	#endif

	memcpy((BYTE *)&tCycleSInfoRom,(BYTE *)&CycleSInfoRomConst,sizeof(TCycleSInfoRom));
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom);
}

//-----------------------------------------------
//��������: �����ѱ�����
//
//����: SearchMaxtime[in]:�����ѱ��ʱ�䣨min�� 0��ʾ����ʱ
//
//����ֵ:
//
//-----------------------------------------------
void api_StartSearchMeter(WORD SearchMaxtime)
{	
	//�ϴ��ѱ�δ���������������ѱ�
	if(SKMeter.byAutoCheck != 0)
	{
		return;
	}
    
    if(tCycleSInfoRom.ClearResultOption == 2)   //ÿ���ѱ�ǰ����ѱ���
    {
        api_ClearSchMeter();
    }

	memset(SKMeter.AA,0,sizeof(SKMeter.AA));
    #if (cSPECIAL_CLIENTS == SPECIAL_GW_AN_HUI)
    SKMeter.byAutoCheck = BIT1;//BIT0-698��Լʶ��BIT1--07��Լʶ��BIT2--97��Լʶ��
    #else
	SKMeter.byAutoCheck = BIT0|BIT1;//BIT0-698��Լʶ��BIT1--07��Լʶ��BIT2--97��Լʶ��
    #endif
	SKMeter.byAATry     = 1;//����ȫAAͨ���ʶ��
	SKMeter.byAATryNum  = 0;
	SKMeter.byFirst     = 1;
	SKMeter.byRxOK      = 0;
	SKMeter.byComplete  = 0;
	SKMeter.byAutoBaud  = 0x1F;
	SKMeter.byAutoAck   = 0;
	SKMeter.byAutoLevel = 0;//�Զ�ʶ�𼶱� 0-����ֽڵ�ַ��1-�˵��ֽڵ�ַ...
    SKMeter.CheckStep = 0x00FF; 
    SKMeter.dwRxCnt     = 0;
    SKMeter.dwSearchMaxTime = SearchMaxtime*60;
    SKMeter.dwSearchTimeCnt = 0;
    FindUnknownMeterEventFlag = FALSE;
}
//-----------------------------------------------
//��������: ֹͣ�ѱ�
//
//����: 
//
//����ֵ:
//
//��ע:
//-----------------------------------------------
void api_StopSchMeter(void)
{
	SKMeter.byAutoCheck = 0;
    //�ѱ�������µ���
    UpdateArchives();
    //����δ֪���ܱ��¼�����
	#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    FindUnknownMeterEventFlag = FALSE;
	#endif
}
//-----------------------------------------------
//��������: ��ȡϵͳ����Ч�ĵ�����
//
//����: 
//
//����ֵ:   ����
//
//��ע:
//-----------------------------------------------
BYTE api_GetMeterNumInSys(void)
{
	BYTE byI,byNum=0;
	
	for(byI=0;byI<SEARCH_METER_MAX_NUM;byI++)
	{
		if( IsMeterValid(byI) )
        {
			byNum++;
        }
	}

	return byNum;
}

//-----------------------------------------------
//��������: ���ѵ��ı��ַ�洢�����ʵ�λ��
//
//����: pAddr[in]:��ַ
//      byPro[in]:Э��
//      byBaudRate[in]:������
//      bAutoLearn[in]:�Ƿ��������δ֪�ı��ַ
//
//����ֵ: �洢λ�õ�����
//
//��ע:
//-----------------------------------------------
BYTE api_Addr2MeterNo(BYTE *pAddr, BYTE byPro, BYTE byBaudRate,DWORD ChOad,BOOL bAutoLearn)
{
	BYTE byNo;
	BYTE IsNeedSave=0;
	

    if( (IsAddrVaild(pAddr) == FALSE)  
      || (IsProtocolValid(byPro) == FALSE) 
      || (IsBaudValid(byBaudRate) == FALSE) 
      || (IsPortValid(ChOad) == FALSE) 
     )
    {
        return 0xFF;
    }

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    //�����¼�֮�����ѱ�
    if( (FindUnknownMeterEventFlag == FALSE) && (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_FIND_UNKNOWN_METER ) == TRUE) )
    {
        return FALSE;
    }
    #endif

	//�������е�
	for(byNo=0;byNo<SEARCH_METER_MAX_NUM;byNo++)
	{	
		if(memcmp(tSearchedMeter[byNo].Addr,pAddr,6) == 0)
		{
			if(IsMeterValid(byNo))
			{
                #if (cSPECIAL_CLIENTS == SPECIAL_GW_AN_HUI)
                if(byPro != tSearchedMeter[byNo].byProtocolType)
                #else
				if( ((byPro != tSearchedMeter[byNo].byProtocolType) && (byPro == ePROTOCOL_698)) 
				  || ( (ChOad == CAN_CH_OAD) && (tSearchedMeter[byNo].byBaud != byBaudRate))
				)
                #endif
				{
                    tSearchedMeter[byNo].byProtocolType =  byPro;
                    tSearchedMeter[byNo].byBaud	= byBaudRate;
                    tSearchedMeter[byNo].ChOad = ChOad;
                    memcpy((BYTE*)&tSearchedMeter[byNo].Time,(BYTE*)&RealTimer,sizeof(tSearchedMeter[byNo].Time));
                    api_SaveSchedMeter(byNo);
				}
				return byNo;
			}
		}	
	}
	//����ѧϰ���ַ
	if(!bAutoLearn) 
		return 0xFF;
	
	//���ȼ��ɼ����Ƿ�ѧϰ�����˱�ţ�����洢��ԭ����λ��
	for(byNo=0; byNo<SEARCH_METER_MAX_NUM; byNo++)
	{
		if(memcmp(tSearchedMeter[byNo].Addr,pAddr,6) == 0)
		{
			IsNeedSave = 1;
			tSearchedMeter[byNo].byProtocolType =  byPro;
			tSearchedMeter[byNo].byBaud	= byBaudRate;
            tSearchedMeter[byNo].ChOad = ChOad;
			break;
		}	        
	}
    //Ȼ����ɼ������Ƿ��п�λδ�洢�����ַ������洢
	if(IsNeedSave != 1)
	{
		for(byNo=0;byNo<SEARCH_METER_MAX_NUM;byNo++)
		{
			if(IsAddrVaild(tSearchedMeter[byNo].Addr) == FALSE)
			{
				memcpy(tSearchedMeter[byNo].Addr,pAddr,6);
				tSearchedMeter[byNo].byProtocolType =  byPro;
				tSearchedMeter[byNo].byBaud	= byBaudRate;
                tSearchedMeter[byNo].ChOad = ChOad;
				IsNeedSave = 1;
				break;
			}
		}
	}	
	//������Ƿ��п���λ�ô洢
	if(IsNeedSave != 1)
	{
		for(byNo=0;byNo<SEARCH_METER_MAX_NUM;byNo++)
		{ 
			if( (IsProtocolValid(tSearchedMeter[byNo].byProtocolType) == FALSE) || (IsBaudValid(tSearchedMeter[byNo].byBaud) == FALSE) )
			{
				memcpy(tSearchedMeter[byNo].Addr,pAddr,6);
				tSearchedMeter[byNo].byProtocolType =  byPro;
				tSearchedMeter[byNo].byBaud	= byBaudRate;
                tSearchedMeter[byNo].ChOad = ChOad;
				IsNeedSave = 1;
				break;
			}
		}
	}
    if(IsNeedSave)
	{	
        memcpy((BYTE*)&tSearchedMeter[byNo].Time,(BYTE*)&RealTimer,sizeof(tSearchedMeter[byNo].Time));
		api_SaveSchedMeter(byNo);
        //�������ַ
		#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
        if(tCycleSInfoRom.SearchMeterEventFlag == TRUE)
        {
            if(gFindUnknownMeterEventDataRAM.MeterNum>=FIND_UNKNOWN_METER_MAX_NUM)
            {
                memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
                FindUnknownMeterEventFlag = FALSE;
                //�����쳣�¼�
                api_WriteSysUNMsg(FIND_UNKNOWN_METER_EVENT_ERR);
            }
            else
            {
                FindUnknownMeterEventFlag = TRUE;
            }
            
            memcpy((BYTE*)&gFindUnknownMeterEventDataRAM.SearchResult[gFindUnknownMeterEventDataRAM.MeterNum],(BYTE*)&tSearchedMeter[byNo],sizeof(TSchedMeterInfo));
            gFindUnknownMeterEventDataRAM.MeterNum++;
            if(gFindUnknownMeterEventDataRAM.MeterNum == FIND_UNKNOWN_METER_MAX_NUM)
            {
                // ����δ֪���ܱ��¼�����
                FindUnknownMeterEventFlag = FALSE;
            }
            
            api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
        }
		#endif
		return byNo;	
	}	
	return 0xFF;
}

void api_SearchMeterSecondTask(void)
{
    BYTE i = 0;
    static BYTE UpdateArchivesFlag = 0;
    
    //Ӧ�÷ŵ������ѱ�������ǰ�棬���������ѱ�������dwSearchTimeCnt���̼� 1
    if( (SKMeter.byAutoCheck) && (SKMeter.dwSearchMaxTime != 0))
    {
        SKMeter.dwSearchTimeCnt++;
        //�ж��ѱ��Ƿ�ʱ
        if( SKMeter.dwSearchTimeCnt>=SKMeter.dwSearchMaxTime )
        {
            api_StopSchMeter();
        }
    }

    if(UpdateArchivesFlag == 0)
    {
        UpdateArchives();
        UpdateArchivesFlag = 1;
    }
    
    if ( (POWER_UP_TIMER - g_PowerOnSecTimer) == SEARCH_METER_POWER_TIME)
    {
        //�ϵ��ѱ�
        if(tCycleSInfoRom.IsPowerUpSearch)
        {
            api_StartSearchMeter(tCycleSInfoRom.PowerUpSearchTime);
        }
    }

    // �ж������ѱ� �ϵ��ѱ�������ٽ��������ѱ��ж�
    if ((POWER_UP_TIMER - g_PowerOnSecTimer) > SEARCH_METER_POWER_TIME)
    {
        if (tCycleSInfoRom.CycleSearchFlag)
        {
            for (i = 0; i < tCycleSInfoRom.CycleNum; i++)
            {
                if ((tCycleSInfoRom.Cycle[i].Time[2] == RealTimer.Sec) && (tCycleSInfoRom.Cycle[i].Time[1] == RealTimer.Min) && (tCycleSInfoRom.Cycle[i].Time[0] == RealTimer.Hour))
                {
                    if(SKMeter.byAutoCheck != 0)
                    {
                        api_StopSchMeter();
                    }

                    if( (tCycleSInfoRom.ClearResultOption == 1) && (i == 0) )   //ÿ�������ѱ�ǰ���
                    {
                        api_ClearSchMeter();
                    }
                    api_StartSearchMeter(tCycleSInfoRom.Cycle[i].Duration);
                    break;
                }
            }
            
        }
    }
}
//-----------------------------------------------
//��������: �ѱ�������
//
//����: 
//
//����ֵ:   TRUE �б�����Ҫ����
//
//��ע:
//-----------------------------------------------
BOOL api_ProSearchMeter( BYTE SerialNo )
{
	BYTE Addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
	BYTE byI, byMeterNum;
	

	//�ж�FPara2��У��
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		return FALSE;
	}
	
	if (SKMeter.byAutoCheck == 0)
	{
		return FALSE;
	}
    
    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    //�����¼�֮�����ѱ�
    if( (FindUnknownMeterEventFlag == FALSE) && (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_FIND_UNKNOWN_METER ) == TRUE) )
    {
        return FALSE;
    }
    #endif

	//������֤�Ѵ��ڵı��ַ
	if (CheckFlashMeter(SerialNo))
	{
		return TRUE;
	}

	//ϵͳ����Ч����
	if (api_GetMeterNumInSys() >= SEARCH_METER_MAX_NUM)
	{
		api_StopSchMeter();
		return FALSE;
	}

    //�յ��ı��ĳ��Ȳ�����Ԥ�ڣ���Ϊ�л������(�����ֽ�������Ԥ�ڵ�)
    if( ((SKMeter.byGyType == ePROTOCOL_645_07)&&(SKMeter.dwRxCnt > EXEPCT_MESSAGE_LENGTH_64507)) 
       ||((SKMeter.byGyType == ePROTOCOL_698)&&(SKMeter.dwRxCnt > EXEPCT_MESSAGE_LENGTH_698))
       )
    {
        SKMeter.byAutoAck = 1;
        SKMeter.byRxOK = 0;
    }
    SKMeter.dwRxCnt = 0;

	//��Լ�Զ�ʶ��
	if (SKMeter.byAutoAck)
	{ //�ϴη�����ģ��Ӧ��(ͨ���յ����ݣ����޷�ͨ��У���ж�)
		if (SKMeter.byAATry)
		{
			SKMeter.byAATry = 0;
			SKMeter.byAutoAck = 0;
		}
		else
		{
			if(SKMeter.byAutoLevel < 5)
			{
				SKMeter.byAutoLevel++;
				SKMeter.AA[SKMeter.byAutoLevel] = 0x00;
			}
			else
			{
				SKMeter.AA[SKMeter.byAutoLevel]++;
			}
		}
	}
	else
	{
		if (SKMeter.byAATry)
		{
			if (SKMeter.byRxOK) //ȫAA�ѱ��յ�����֡��Ҳ��Ҫ�����ѱ�
			{
				SKMeter.byAATry = 0;
				SKMeter.byAutoAck = 0;
			}
		}
	}
	while (1)
	{
		if (SKMeter.byAutoAck == 0)
		{
			if (SKMeter.byAATry)
			{
				if(++SKMeter.byAATryNum > COLLECT_METER_RETRY_TIMES) 
				{	
					// ȫAA���ԣ���Ӧ���л�������
                    SwitchNextBaud();
					// �������л�������Ӧ����698�л���645
					if(!SKMeter.byAutoBaud)
					{
                        SwitchNextProtocol();
					}
				}
			}
			else
			{
				if (!SKMeter.byFirst)
				{
					SKMeter.AA[SKMeter.byAutoLevel]++;
				}
				else
				{
					SKMeter.byFirst = 0;
				}
			}
		}
		else
		{
			SKMeter.byAutoAck = 0;
		}
		
		if(SKMeter.byAATry)
		{
			break;
		}

		if (SKMeter.AA[SKMeter.byAutoLevel] > 99)
		{
			SKMeter.AA[SKMeter.byAutoLevel] = 0;
            //��ʾ�����л��ʱ�������SKMeter.byAutoLevel > 1 
			if (SKMeter.byAutoLevel)
			{
				SKMeter.byAutoLevel--;
				continue;
			}
			else
			{
                SwitchNextBaud();

				if (SKMeter.byAutoCheck & BIT0)
				{
					if ( !SKMeter.byAutoBaud )
					{
                        //698��Լ������
                        SwitchNextProtocol();
					}
				}
				else if (SKMeter.byAutoCheck & BIT1)
				{
					if ( !SKMeter.byAutoBaud )
					{
                        //07��Լ������
                        SwitchNextProtocol();
					}
				}
				else if (SKMeter.byAutoCheck & BIT2)
				{//97��Լ������
					api_StopSchMeter();
					return FALSE;
				}
				else
				{
					api_StopSchMeter();
					return FALSE;
				}
				break;
			}
		}
		else
		{
			break;
		}
    }
	if (SKMeter.byAATry == 0)
	{
		for (byI = 0; byI <= SKMeter.byAutoLevel; byI++)
		{
			Addr[byI] = lib_BBinToBCD(SKMeter.AA[byI]);
		}
	}

    if (SKMeter.byAutoCheck & BIT0)
    { //698
		SKMeter.byAutoBaud &= ~(BIT0);

		if (SKMeter.byAutoBaud & BIT1)
		{
			SKMeter.byBaud = 3;     //2400
		}
        else if (SKMeter.byAutoBaud & BIT2)
		{
			SKMeter.byBaud = 4;     //4800
		}
        else if (SKMeter.byAutoBaud & BIT3)
		{
			SKMeter.byBaud = 6;     //9600
		}
        else if (SKMeter.byAutoBaud & BIT4)
		{
			SKMeter.byBaud = 10;     //115200
		}
        else
        {
			SKMeter.byAutoBaud = 0;
            SKMeter.byRxOK = 0;
            return FALSE;
        }

		SKMeter.byGyType = ePROTOCOL_698;
        Tx_Collect_Meter(SerialNo,ePROTOCOL_698,Addr,SEARCH_METER_698_OAD,SKMeter.byBaud);
    }
    else if (SKMeter.byAutoCheck & BIT1)
    { // 07
	
		SKMeter.byAutoBaud &= ~(BIT0);
			
		if (SKMeter.byAutoBaud & BIT1)
		{
			SKMeter.byBaud = 3;     //2400
		}
        else if (SKMeter.byAutoBaud & BIT2)
		{
			SKMeter.byBaud = 4;     //4800
		}
        else if (SKMeter.byAutoBaud & BIT3)
		{
			SKMeter.byBaud = 6;     //9600
		}
        else if (SKMeter.byAutoBaud & BIT4)
		{
			SKMeter.byBaud = 10;     //115200
		}
        else
        {
			SKMeter.byAutoBaud = 0;
            SKMeter.byRxOK = 0;
            return FALSE;
        }

        SKMeter.byGyType = ePROTOCOL_645_07;
		Tx_Collect_Meter(SerialNo,ePROTOCOL_645_07,Addr,SEARCH_METER_654_07_OAD,SKMeter.byBaud);
    }
    else if (SKMeter.byAutoCheck & BIT2)
    { //97
		SKMeter.byAutoBaud &= ~(BIT1|BIT2|BIT3|BIT4);
        SKMeter.byBaud = 2;     //1200
        SKMeter.byGyType = ePROTOCOL_645_97;
		Tx_Collect_Meter(SerialNo,ePROTOCOL_645_97,Addr,SEARCH_METER_654_97_OAD,SKMeter.byBaud);
    }
    else
    {
        api_StopSchMeter();
    }
    SKMeter.byRxOK = 0;
    return TRUE;
}

//-----------------------------------------------
//��������: ��ȡ�ѱ���
//
//����: ClassIndex[in]: �ѱ�������
//      pBuf[out]: ���Buf
//      bNum[out]: �������
//����ֵ: ���ݳ���
//
//��ע: Ŀǰ���֧��32ֻ��
//-----------------------------------------------
WORD api_GetSchedMeter698(BYTE ClassIndex, BYTE *pBuf ,BYTE* bNum)
{
	BYTE i;
    WORD wLen = 0;

    *bNum = 0;

    if( (pBuf == NULL) || (bNum == NULL))
    {
        return 0;
    }

	for (i = 0; i < SEARCH_METER_MAX_NUM; i++)
	{
		if (IsMeterValid(i))
		{
			(*bNum)++;
            //��ȫ��������������ҵ�����Ҫ�ĵ����������
            if( (ClassIndex == 0) || (ClassIndex == *bNum))
            {
                //ͨ�ŵ�ַ
				pBuf[wLen++] = 0x05;
				lib_ExchangeData( pBuf+wLen, tSearchedMeter[i].Addr, 6 );
				wLen += 6;
                //�ɼ�����ַ
				pBuf[wLen++] = 0x05;
				memcpy( pBuf+wLen, FPara1->MeterSnPara.CommAddr, 6 );
				wLen +=6;
                //��Լ����
				pBuf[wLen++] = tSearchedMeter[i].byProtocolType;
                //��λ
				pBuf[wLen++] = 0;
                //�ź�Ʒ��
				pBuf[wLen++] = 0;
                //�ѵ���ʱ��
				memcpy( pBuf+wLen, (BYTE*)&tSearchedMeter[i].Time, sizeof(tSearchedMeter[i].Time) );
				wLen += sizeof(tSearchedMeter[i].Time);
                //������Ϣ
				// pBuf[wLen++] = 0x01;
				// pBuf[wLen++] = 0x00;
			}

            //�����˵����ѱ���
            if( (ClassIndex != 0) && (ClassIndex == *bNum))
            {
                *bNum = 1;
                break;
            }
		}
	}
	return wLen;
}
//-----------------------------------------------
//��������: ��ȡ�ѱ����
//
//����: ClassIndex[in]: �ѱ��������
//      pData[out]: ���Buf
//����ֵ: ���ݳ���
//
//��ע: 
//-----------------------------------------------
WORD api_GetSchedMeterPara698(BYTE ClassIndex,BYTE *pData)
{
    WORD wLen = 0;

    if(pData == NULL)
    {
        return 0;
    }

	switch(ClassIndex)
	{
        case 0:
            //ÿ�������ѱ����ñ�־ 
            pData[wLen++] = tCycleSInfoRom.CycleSearchFlag;
            //�Զ����²ɼ�����
            pData[wLen++] = tCycleSInfoRom.AutoUpdateArchives;
            //�ѱ��¼����ɱ�־
            pData[wLen++] = tCycleSInfoRom.SearchMeterEventFlag;
            //����ѱ���ѡ��
            pData[wLen++] = tCycleSInfoRom.ClearResultOption;
            break;
        case 1:
            //ÿ�������ѱ����ñ�־ 
            pData[wLen++] = tCycleSInfoRom.CycleSearchFlag;
            break;
        case 2:
            //�Զ����²ɼ�����
            pData[wLen++] = tCycleSInfoRom.AutoUpdateArchives;
            break;
        case 3:
            //�ѱ��¼����ɱ�־
            pData[wLen++] = tCycleSInfoRom.SearchMeterEventFlag;
            break;
        case 4:
            //����ѱ���ѡ��
            pData[wLen++] = tCycleSInfoRom.ClearResultOption;
            break;
        default:
            wLen = 0;
            break;
    }

	return wLen;
}
//-----------------------------------------------
//��������: ��ȡ�ѱ����
//
//����: ClassIndex[in]: �ѱ��������
//      pData[out]: ���Buf
//����ֵ: ���ݳ���
//
//��ע: 
//-----------------------------------------------
WORD api_GetSchedMeterStrategy698(BYTE ClassIndex,BYTE *pData)
{
    WORD wLen = 0;

    if(pData == NULL)
    {
        return 0;
    }

	switch(ClassIndex)
	{
        case 0:
            //�ѱ�ģʽ  
            pData[wLen++] = tCycleSInfoRom.SearchMode;
            //�ϵ��Ƿ������ѱ� 
            pData[wLen++] = tCycleSInfoRom.IsPowerUpSearch;
            //�ϵ��ѱ�ʱ��
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime&0xFF);
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime>>8);
            break;
        case 1:
            //�ѱ�ģʽ 
            pData[wLen++] = tCycleSInfoRom.SearchMode;
            break;
        case 2:
            //�ϵ��Ƿ������ѱ�
            pData[wLen++] = tCycleSInfoRom.IsPowerUpSearch;
            break;
        case 3:
            //�ϵ��ѱ�ʱ��
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime&0xFF);
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime>>8);
            break;
        default:
            wLen = 0;
            break;
    }

	return wLen;
}
//-----------------------------------------------
//��������: ��ȡ�ѱ�����
//
//����: ClassIndex[in]: �ѱ���������
//      pData[out]: ���Buf
//      bNum[in]:�ѱ����ڲ�������
//����ֵ: ���ݳ���
//
//��ע: 
//-----------------------------------------------
WORD api_GetSchedMeterCycle698( BYTE ClassIndex,BYTE *pBuf,BYTE* bNum)
{
	BYTE i,j;
    WORD wLen = 0;

    if( (ClassIndex > tCycleSInfoRom.CycleNum) || (pBuf == NULL) || (bNum == NULL) )
    {
        return 0;
    }
    if(ClassIndex == 0)
    {
        i = 0;
        j = tCycleSInfoRom.CycleNum;
    }
    else
    {
        i = ClassIndex - 1;
        j = ClassIndex;
    }

	for (; i < j; i++)
	{
        //��ʼʱ��
        memcpy(pBuf+wLen,tCycleSInfoRom.Cycle[i].Time,3);
        wLen += 3;
        //�ѱ�ʱ��
        memcpy(pBuf+wLen,(BYTE*)&tCycleSInfoRom.Cycle[i].Duration,2);
        wLen += 2;
	}

    *bNum = i;
	return wLen;
}

//-----------------------------------------------
//��������: �����ѱ����
//
//����: ClassIndex[in]: �ѱ��������
//      pData[in]: ����
//����ֵ: ���ý��
//
//��ע: 
//-----------------------------------------------
BYTE api_SetSchedMeterPara698(BYTE ClassIndex,BYTE *pData)
{
    BYTE Dar = DAR_Undefined;

    if(pData == NULL)
    {
        return DAR_TempFail;
    }

    switch(ClassIndex)
    {
        case 0:
            if( (pData[0] != Structure_698) 
              ||(pData[1] != 4)
              ||(pData[2] != Boolean_698) 
              ||(pData[4] != Boolean_698)
              ||(pData[6] != Boolean_698)
              ||(pData[8] != Enum_698)
              )
            {
                return DAR_WrongType;
            }

            if(  ( (pData[3] != 0) && (pData[3] != 1) )
                || ( (pData[5] != 0) && (pData[5] != 1) )
                || ( (pData[7] != 0) && (pData[7] != 1) )
                || ( (pData[9] != 0) && (pData[9] != 1) && (pData[9] != 2) )
            )
            {
                return DAR_OverRegion;
            }
            
            //ÿ�������ѱ����ñ�־ 
            tCycleSInfoRom.CycleSearchFlag = pData[3];
            //�Զ����²ɼ�����
            tCycleSInfoRom.AutoUpdateArchives = pData[5];
            //�ѱ��¼����ɱ�־ ��ʾ�Ƿ�����ѱ��¼�
            tCycleSInfoRom.SearchMeterEventFlag = pData[7];
            //����ѱ���ѡ��
            tCycleSInfoRom.ClearResultOption = pData[9];
            Dar = DAR_Success;
            break;
		case 1: //ÿ�������ѱ����ñ�־ 
            if(pData[0] != Boolean_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.CycleSearchFlag = pData[1];
            Dar = DAR_Success;
            break;
        case 2: //�Զ����²ɼ�����
            if(pData[0] != Boolean_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.AutoUpdateArchives = pData[1];
            Dar = DAR_Success;
            break;
        case 3: //�ѱ��¼����ɱ�־ ��ʾ�Ƿ�����ѱ��¼�
            if(pData[0] != Boolean_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.SearchMeterEventFlag = pData[1];
            Dar = DAR_Success;
            break;
        case 4: //����ѱ���ѡ��
            if(pData[0] != Enum_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) && (pData[1] != 2) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.ClearResultOption = pData[1];
            Dar = DAR_Success;
            break;
        default:
            Dar = DAR_OverRegion;
            break;
    }

    if(Dar == DAR_Success)
    {
        if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) == TRUE)
        {
            return DAR_Success;
        }
        else
        {
            return DAR_HardWare;
        }
    }
    else
    {
        return Dar;
    }
}
//-----------------------------------------------
//��������: �����ѱ����
//
//����: ClassIndex[in]: �ѱ��������
//      pData[in]: ����
//����ֵ: ���ý��
//
//��ע: 
//-----------------------------------------------
BYTE api_SetSchedMeterStrategy698(BYTE ClassIndex,BYTE *pData)
{
    BYTE Dar = DAR_Undefined;

    if(pData == NULL)
    {
        return DAR_TempFail;
    }

    switch(ClassIndex)
    {
        case 0:
            if( (pData[0] != Structure_698) 
              ||(pData[1] != 3)
              ||(pData[2] != Enum_698) 
              ||(pData[4] != Boolean_698)
              ||(pData[6] != Long_unsigned_698)
              )
            {
                return DAR_WrongType;
            }

            if(  ( (pData[3] != 0) && (pData[3] != 1) )
              || ( (pData[5] != 0) && (pData[5] != 1) )
            )
            {
                return DAR_OverRegion;
            }

            //�ѱ�ģʽ 
            tCycleSInfoRom.SearchMode = pData[3];
            //�ϵ��Ƿ������ѱ�
            tCycleSInfoRom.IsPowerUpSearch = pData[5];
            //�ϵ��ѱ�ʱ��
            tCycleSInfoRom.PowerUpSearchTime = (WORD)(pData[7]<<8) | pData[8] ;
            Dar = DAR_Success;
            break;
		case 1: //�ѱ�ģʽ 
            if(pData[0] != Enum_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.SearchMode = pData[1];
            Dar = DAR_Success;
            break;
        case 2: //�ϵ��Ƿ������ѱ�
            if(pData[0] != Boolean_698)
            {
                return DAR_WrongType;
            }

            if( (pData[1] != 0) && (pData[1] != 1) )
            {
                return DAR_OverRegion;
            }

            tCycleSInfoRom.IsPowerUpSearch = pData[1];
            Dar = DAR_Success;
            break;
        case 3: //�ϵ��ѱ�ʱ��
           if( pData[0] != Long_unsigned_698 )
            {
                return DAR_WrongType;
            }
            tCycleSInfoRom.PowerUpSearchTime = (WORD)(pData[1]<<8) | pData[2] ;
            Dar = DAR_Success;
            break;
        default:
            Dar = DAR_OverRegion;
            break;
    }

    if(Dar == DAR_Success)
    {
        if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) == TRUE)
        {
            return DAR_Success;
        }
        else
        {
            return DAR_HardWare;
        }
    }
    else
    {
        return Dar;
    }
}
//-----------------------------------------------
//��������: �����ѱ�����
//
//����: ClassIndex[in]: �ѱ���������
//      pData[out]: ����
//����ֵ: ���ý��
//
//��ע: 
//-----------------------------------------------
BYTE api_SetSchedMeterCycle698(BYTE ClassIndex,BYTE *pData)
{
    TOad60020900 Cycle[SEARCH_CYCLE_TIMES];
    BYTE i,ArrayLen;
    DWORD SourceSecond,DestSecond;

    if(pData == NULL)
    {
        return DAR_TempFail;
    }

    //ֻ������������һ����
    if(ClassIndex != 0)
    {
        return DAR_Undefined;
    }

    //��ʽ�ж�
    if( pData[0] != Array_698 )
    {
        return DAR_WrongType;
    }
    else
    {
        ArrayLen = pData[1];
        if(ArrayLen == 0)
        {
            return DAR_WrongType;
        }

        if(ArrayLen>SEARCH_CYCLE_TIMES)
        {
            return DAR_OverRegion;
        }
        pData += 2;
    }

    //��ȡRAM�е��ѱ�����
    memcpy((BYTE*)Cycle,(BYTE*)tCycleSInfoRom.Cycle,sizeof(Cycle));

    for(i = 0;i<ArrayLen;i++)
    {
        if( (pData[0] != Structure_698) || 
            (pData[1] != 2) || 
            (pData[2] != Time_698) || 
            (pData[6] != Long_unsigned_698) 
        )
        {
            return DAR_WrongType;
        }

        memcpy( (BYTE*)&Cycle[i].tStartTime.hour ,&pData[3],3);
        lib_ExchangeData((BYTE*)&Cycle[i].wSearchMin ,&pData[7],2);
        if( (Cycle[i].tStartTime.hour>23) || (Cycle[i].tStartTime.minute>59) ||(Cycle[i].tStartTime.second>59) 
            )
        {
            return DAR_OverRegion;
        }

        pData += 9;
    }

    //�Ƚ��µ��ѱ������Ƿ������������
   for(i = 0;i<ArrayLen;i++)
    {
        SourceSecond = Cycle[i].tStartTime.hour*60;
        SourceSecond = (SourceSecond+Cycle[i].tStartTime.minute)*60;
        SourceSecond += Cycle[i].tStartTime.second;

        //ȷ��д��Ŀ�ʼʱ�����ǰһ���Ŀ�ʼʱ�� //ȷ��д��Ŀ�ʼʱ����ڵ���ǰһ����ֹͣ�ѱ�ʱ��
        if(i != 0)
        {
            DestSecond = Cycle[i-1].tStartTime.hour*60;
            DestSecond = (DestSecond+Cycle[i-1].tStartTime.minute)*60;
            DestSecond += Cycle[i-1].tStartTime.second;
            if(SourceSecond<DestSecond)
            {
                break;
            }
        }
        //ȷ��д��Ŀ�ʼʱ��С�ں�һ���Ŀ�ʼʱ�� //ȷ��д��Ľ���ʱ��С�ڵ��ں�һ���Ŀ�ʼʱ��
        if( i != (ArrayLen-1) )
        {
            DestSecond = Cycle[i+1].tStartTime.hour*60;
            DestSecond = (DestSecond+Cycle[i+1].tStartTime.minute)*60;
            DestSecond += Cycle[i+1].tStartTime.second;
            if(SourceSecond>DestSecond)
            {
                break;
            }
        }
    }

    //������������
    if(i>=ArrayLen)
    {
        //д���Ԫ�ظ������㣬����
        if(ArrayLen < SEARCH_CYCLE_TIMES)
        {
            for(i = ArrayLen; i<SEARCH_CYCLE_TIMES;i++)
            {
                memcpy((BYTE*)&Cycle[i],(BYTE*)&Cycle[ArrayLen - 1],sizeof(TOad60020900));
            }
        }
        memcpy((BYTE*)tCycleSInfoRom.Cycle,(BYTE*)Cycle,sizeof(tCycleSInfoRom.Cycle));
        tCycleSInfoRom.CycleNum = ArrayLen;   //�����ѱ�������
        if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(CycleSearchRom.CycleSInfoRom), sizeof(TCycleSInfoRom), (BYTE *)&tCycleSInfoRom) == TRUE)
        {
            return DAR_Success;
        }
        else
        {
            return DAR_HardWare;
        }
    }
    else
    {
        return DAR_OverRegion;
    }

}
//-----------------------------------------------
//��������: �����ѱ�
//
//����: ClassAttribute[in]: ����
//      ClassIndex[out]: ����
//      pData[in]:����
//����ֵ: ���ݳ���
//
//��ע: 
//-----------------------------------------------
BYTE api_SetSchedMeter698(BYTE ClassAttribute,BYTE ClassIndex,BYTE *pData)
{
    BYTE Dar;

    if( pData == NULL)
    {
        return DAR_TempFail;
    }

    switch(ClassAttribute)
    {
        case 8:
            Dar = api_SetSchedMeterPara698(ClassIndex,pData);
            break;
        case 9:
            Dar = api_SetSchedMeterCycle698(ClassIndex,pData);
            break;
        case 11:
            Dar = api_SetSchedMeterStrategy698(ClassIndex,pData);
            break;
        default:
            Dar = DAR_Undefined;
            break;
    }

    return Dar;
}
#endif //#if (SEL_SEARCH_METER == YES )
