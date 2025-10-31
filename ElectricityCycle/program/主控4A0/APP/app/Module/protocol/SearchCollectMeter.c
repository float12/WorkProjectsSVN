//----------------------------------------------------------------------
//Copyright (C) 2003-2021 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	刘骞
//创建日期	2021.7.27
//描述		缩位搜表
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"
#include "./GDW698/GDW698.h"
#include "./GDW698/GDW698DataDef.h"

#if (SEL_SEARCH_METER == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define COLLECT_METER_RETRY_TIMES	  2       // 采表重试次数
#define EXEPCT_MESSAGE_LENGTH_64507   (24+4)
#define EXEPCT_MESSAGE_LENGTH_698     (39+4)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

TSchedMeterInfo  tSearchedMeter[SEARCH_METER_MAX_NUM];
#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
FindUnknownMeterEventData_t  gFindUnknownMeterEventDataRAM = {0};
BOOL FindUnknownMeterEventFlag = FALSE;
#endif
TSKMeter SKMeter;                                       // 搜表过程变量
TCycleSInfoRom tCycleSInfoRom;                          // 周期搜表参数
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
// 函数功能: 效验当前周期搜表控制字RAM参数
//
// 参数:
//
// 返回值:  	无
//
// 备注: 每小时效验RAM参数
//-----------------------------------------------
void CheckCycleSInfo(void)
{
	// 监视当前参数
	if (lib_CheckSafeMemVerify((BYTE *)&tCycleSInfoRom, sizeof(TCycleSInfoRom), UN_REPAIR_CRC) == FALSE)
	{
		// 由EEProm恢复参数
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
//函数功能: 判断地址是否有效
//
//参数: addr[in]
//
//返回值: TRUE FALE
//	无
//
//备注:
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
//函数功能: 判断波特率是否有效
//
//参数: byBaud[in]
//
//返回值: TRUE FALE
//	无
//
//备注:
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
//函数功能: 判断协议是否有效
//
//参数: byProtocolType[in]；协议类型
//
//返回值: TRUE FALE
//
//备注:
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
//函数功能: 判断指定的搜表结果是否有效
//
//参数: byNo[in]:搜表结果索引
//
//返回值: TRUE FALE
//
//备注:
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
// //功能描述:  获取搜表结果第一个空闲位置的索引
// //         
// //参数:      
// //         
// //返回值:  索引  
// //         
// //备注:  
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
// 	return byI;//没搜到就返回最大值了
// }

//--------------------------------------------------
//功能描述:  通过表地址获取index
//         
//参数: pbuf[in];表地址
//         
//返回值:   索引
//         
//备注:  
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
	return byI;//没搜到就返回最大值了
}

// //-----------------------------------------------
// //函数功能: 返回系统第N个有效的电表序号（数组下标）
// //
// //参数: byN - 第 N 个
// //
// //返回值: BYTE 
// //
// //备注:
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
//函数功能: 判断该表地址是否在搜表结果中
//
//参数: pAddr[IN] 表地址
//     pBaud[OUT] 通讯速率
//
//返回值: BOOL
//
//备注:
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

    //更新档案
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
                    //搜表结果对应位置为0
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

    //添加档案
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
//函数功能: 将发现的未知电能表转换为698协议的搜表结果
//
//参数: InBuf[in]: 搜表结果
//      OutBuf[out]: 输出Buf
//返回值: 数据长度
//
//备注: 
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
        //通信地址
        OutBuf[wLen++] = TSA_698;
        OutBuf[wLen++] = 0x07;
        OutBuf[wLen++] = 0x05;
        lib_ExchangeData( OutBuf+wLen, InBuf->SearchResult[i].Addr, 6 );
        wLen += 6;
        //采集器地址
        OutBuf[wLen++] = TSA_698;
        OutBuf[wLen++] = 0x07;
        OutBuf[wLen++] = 0x05;
        memcpy( OutBuf+wLen, FPara1->MeterSnPara.CommAddr, 6 );
        wLen += 6;
        //规约类型
        OutBuf[wLen++] = Enum_698;
        OutBuf[wLen++] = InBuf->SearchResult[i].byProtocolType;
        //相位
        OutBuf[wLen++] = Enum_698;
        OutBuf[wLen++] = 0;
        //信号品质
        OutBuf[wLen++] = Unsigned_698;
        OutBuf[wLen++] = 0;
        //搜到的时间
        OutBuf[wLen++] = DateTime_S_698;
        memcpy( OutBuf+wLen, (BYTE*)&InBuf->SearchResult[i].Time, sizeof(InBuf->SearchResult[i].Time));
        lib_InverseData(OutBuf+wLen,2); //年
        wLen += sizeof(InBuf->SearchResult[i].Time);
        //附加信息
        OutBuf[wLen++] = Array_698;
        OutBuf[wLen++] = 0x00;
	}

	return wLen;
}
#endif
//-----------------------------------------------
//函数功能: 保存指定RAM中的搜表结果到FLASH
//
//参数: byMetIndex[in]：索引
//
//返回值: TRUE FALSE
//
//备注:
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
//函数功能: 验证之前搜到过的表地址
//
//参数: 
//
//返回值: FALSE 验证完成；TRUE 验证中 
//
//备注:
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
    
    //验证搜表结果
	if ( (SKMeter.CheckStep & BIT15 ) == 0)
	{
        if(SKMeter.CheckStep == 0x00FF)
        {
            //验证第一块搜表结果
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
            //验证搜表结果完成
            SKMeter.CheckStep = BIT15;
        }
        else
        {
            for(; SKMeter.CheckStep < SEARCH_METER_MAX_NUM; SKMeter.CheckStep++)
            {
				//不验证CAN通道的监听搜表结果
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
                //验证搜表结果完成
                SKMeter.CheckStep = BIT15;
            }
        }
	}

	if ( (SKMeter.CheckStep & BIT15 ) != 0)//验证搜表通道下的档案
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
                    &&(GetMeterIndexByAddr(cfg.basic.tsa.addr) >= SEARCH_METER_MAX_NUM) //搜表结果里没有这个地址才搜索
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
            //搜表完成
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
//函数功能: 切换到下一个波特率进行搜表
//
//参数: 
//
//返回值：
//
//备注:
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
//函数功能: 切换到下一个协议进行搜表
//
//参数: 
//
//返回值：
//
//备注:
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
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 上电从EEP读取搜表结果和搜表参数到RAM
//
//参数: 
//
//返回值：
//
//备注:
//-----------------------------------------------
void api_PowerUpSchClctMeter(void)
{
    BYTE Buf[((SUB_EVENT_INDEX_MAX_NUM+7)/8)+20];
	WORD Len;
	DWORD dwAddr;

	// 从EE中读取搜表信息
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(tSchMtInfoRom[0]), sizeof(TSchedMeterInfo)*SEARCH_METER_MAX_NUM, (BYTE *)&tSearchedMeter[0]);

	memset((BYTE *)&SKMeter, 0x00, sizeof(TSKMeter));

    CheckCycleSInfo();

    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    Len = GET_STRUCT_MEM_LEN( TPowerDownFlag, InstantEventFlag );
	dwAddr = GET_CONTINUE_ADDR( EventConRom.api_PowerDownFlag.InstantEventFlag[0] );
	api_ReadFromContinueEEPRom( dwAddr, Len, Buf );
    // 发现未知电能表事件结束
	api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
    //掉电前事件未开始，但有未知电能表，清空
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
//函数功能: 掉电保存
//
//参数: 
//
//返回值：
//
//备注:
//-----------------------------------------------
void api_PowerDownSchClctMeter(void)
{
    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    if( (tCycleSInfoRom.SearchMeterEventFlag != TRUE) || (gFindUnknownMeterEventDataRAM.MeterNum == 0) )
    {
        memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
    }
    // 保存掉电时的未生成发现未知电能表事件数据
	api_WriteToContinueEEPRom(GET_CONTINUE_ADDR(FindUnknownMeterEventData), sizeof(FindUnknownMeterEventData_t), (BYTE *)&gFindUnknownMeterEventDataRAM);
    #endif
}
//-----------------------------------------------
//函数功能: 发现未知电能表事件初始化
//
//参数: 
//
//返回值：
//
//备注:
//-----------------------------------------------
void api_FindUnknownMeterEventPara(void)
{
    DWORD Inpara = 0;
    api_WriteEventAttribute(0x3111, 0xff, (BYTE *)&Inpara, 0);
}
//-----------------------------------------------
//函数功能: 清除搜表结果
//
//参数: 
//
//返回值：
//
//备注:
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
//函数功能: 搜表初始化
//
//参数: 
//
//返回值：
//
//备注:
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
//函数功能: 启动搜表流程
//
//参数: SearchMaxtime[in]:本次搜表最长时间（min） 0表示不限时
//
//返回值:
//
//-----------------------------------------------
void api_StartSearchMeter(WORD SearchMaxtime)
{	
	//上次搜表未结束，不再启动搜表
	if(SKMeter.byAutoCheck != 0)
	{
		return;
	}
    
    if(tCycleSInfoRom.ClearResultOption == 2)   //每次搜表前清空搜表结果
    {
        api_ClearSchMeter();
    }

	memset(SKMeter.AA,0,sizeof(SKMeter.AA));
    #if (cSPECIAL_CLIENTS == SPECIAL_GW_AN_HUI)
    SKMeter.byAutoCheck = BIT1;//BIT0-698规约识别，BIT1--07规约识别，BIT2--97规约识别
    #else
	SKMeter.byAutoCheck = BIT0|BIT1;//BIT0-698规约识别，BIT1--07规约识别，BIT2--97规约识别
    #endif
	SKMeter.byAATry     = 1;//先用全AA通配符识别
	SKMeter.byAATryNum  = 0;
	SKMeter.byFirst     = 1;
	SKMeter.byRxOK      = 0;
	SKMeter.byComplete  = 0;
	SKMeter.byAutoBaud  = 0x1F;
	SKMeter.byAutoAck   = 0;
	SKMeter.byAutoLevel = 0;//自动识别级别 0-最低字节地址，1-此低字节地址...
    SKMeter.CheckStep = 0x00FF; 
    SKMeter.dwRxCnt     = 0;
    SKMeter.dwSearchMaxTime = SearchMaxtime*60;
    SKMeter.dwSearchTimeCnt = 0;
    FindUnknownMeterEventFlag = FALSE;
}
//-----------------------------------------------
//函数功能: 停止搜表
//
//参数: 
//
//返回值:
//
//备注:
//-----------------------------------------------
void api_StopSchMeter(void)
{
	SKMeter.byAutoCheck = 0;
    //搜表结束更新档案
    UpdateArchives();
    //发现未知电能表事件结束
	#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    FindUnknownMeterEventFlag = FALSE;
	#endif
}
//-----------------------------------------------
//函数功能: 获取系统中有效的电表个数
//
//参数: 
//
//返回值:   个数
//
//备注:
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
//函数功能: 把搜到的表地址存储到合适的位置
//
//参数: pAddr[in]:地址
//      byPro[in]:协议
//      byBaudRate[in]:波特率
//      bAutoLearn[in]:是否允许添加未知的表地址
//
//返回值: 存储位置的索引
//
//备注:
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
    //存完事件之后再搜表
    if( (FindUnknownMeterEventFlag == FALSE) && (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_FIND_UNKNOWN_METER ) == TRUE) )
    {
        return FALSE;
    }
    #endif

	//查找已有的
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
	//不用学习表地址
	if(!bAutoLearn) 
		return 0xFF;
	
	//首先检查采集器是否学习到过此表号，是则存储在原来的位置
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
    //然后检查采集器中是否还有空位未存储过表地址，有则存储
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
	//最后检查是否有可用位置存储
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
        //新增表地址
		#if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
        if(tCycleSInfoRom.SearchMeterEventFlag == TRUE)
        {
            if(gFindUnknownMeterEventDataRAM.MeterNum>=FIND_UNKNOWN_METER_MAX_NUM)
            {
                memset((BYTE*)&gFindUnknownMeterEventDataRAM,0x00,sizeof(FindUnknownMeterEventData_t));
                FindUnknownMeterEventFlag = FALSE;
                //生成异常事件
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
                // 发现未知电能表事件结束
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
    
    //应该放到周期搜表启动的前面，否则周期搜表启动后dwSearchTimeCnt立刻加 1
    if( (SKMeter.byAutoCheck) && (SKMeter.dwSearchMaxTime != 0))
    {
        SKMeter.dwSearchTimeCnt++;
        //判断搜表是否超时
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
        //上电搜表
        if(tCycleSInfoRom.IsPowerUpSearch)
        {
            api_StartSearchMeter(tCycleSInfoRom.PowerUpSearchTime);
        }
    }

    // 判断周期搜表 上电搜表结束后再进行周期搜表判断
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

                    if( (tCycleSInfoRom.ClearResultOption == 1) && (i == 0) )   //每天周期搜表前清空
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
//函数功能: 搜表主函数
//
//参数: 
//
//返回值:   TRUE 有报文需要发送
//
//备注:
//-----------------------------------------------
BOOL api_ProSearchMeter( BYTE SerialNo )
{
	BYTE Addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
	BYTE byI, byMeterNum;
	

	//判断FPara2的校验
	if( lib_CheckSafeMemVerify( (BYTE *)(FPara2), sizeof(TFPara2), UN_REPAIR_CRC ) == FALSE )
	{
		return FALSE;
	}
	
	if (SKMeter.byAutoCheck == 0)
	{
		return FALSE;
	}
    
    #if(SEL_EVENT_FIND_UNKNOWN_METER == YES)
    //存完事件之后再搜表
    if( (FindUnknownMeterEventFlag == FALSE) && (api_DealEventStatus( eGET_EVENT_STATUS, eSUB_FIND_UNKNOWN_METER ) == TRUE) )
    {
        return FALSE;
    }
    #endif

	//首先验证已存在的表地址
	if (CheckFlashMeter(SerialNo))
	{
		return TRUE;
	}

	//系统中有效表数
	if (api_GetMeterNumInSys() >= SEARCH_METER_MAX_NUM)
	{
		api_StopSchMeter();
		return FALSE;
	}

    //收到的报文长度不符合预期，认为有混叠报文(防护字节数大于预期的)
    if( ((SKMeter.byGyType == ePROTOCOL_645_07)&&(SKMeter.dwRxCnt > EXEPCT_MESSAGE_LENGTH_64507)) 
       ||((SKMeter.byGyType == ePROTOCOL_698)&&(SKMeter.dwRxCnt > EXEPCT_MESSAGE_LENGTH_698))
       )
    {
        SKMeter.byAutoAck = 1;
        SKMeter.byRxOK = 0;
    }
    SKMeter.dwRxCnt = 0;

	//规约自动识别
	if (SKMeter.byAutoAck)
	{ //上次发送有模糊应答(通过收到数据，但无法通过校验判断)
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
			if (SKMeter.byRxOK) //全AA搜表，收到完整帧，也需要继续搜表
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
					// 全AA尝试，无应答，切换波特率
                    SwitchNextBaud();
					// 波特率切换后，仍无应答，则698切换至645
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
            //表示报文有混叠时，会出现SKMeter.byAutoLevel > 1 
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
                        //698规约检查结束
                        SwitchNextProtocol();
					}
				}
				else if (SKMeter.byAutoCheck & BIT1)
				{
					if ( !SKMeter.byAutoBaud )
					{
                        //07规约检查结束
                        SwitchNextProtocol();
					}
				}
				else if (SKMeter.byAutoCheck & BIT2)
				{//97规约检查结束
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
//函数功能: 读取搜表结果
//
//参数: ClassIndex[in]: 搜表结果索引
//      pBuf[out]: 输出Buf
//      bNum[out]: 结果数量
//返回值: 数据长度
//
//备注: 目前最多支持32只表
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
            //读全部搜索结果或者找到了需要的单个搜索结果
            if( (ClassIndex == 0) || (ClassIndex == *bNum))
            {
                //通信地址
				pBuf[wLen++] = 0x05;
				lib_ExchangeData( pBuf+wLen, tSearchedMeter[i].Addr, 6 );
				wLen += 6;
                //采集器地址
				pBuf[wLen++] = 0x05;
				memcpy( pBuf+wLen, FPara1->MeterSnPara.CommAddr, 6 );
				wLen +=6;
                //规约类型
				pBuf[wLen++] = tSearchedMeter[i].byProtocolType;
                //相位
				pBuf[wLen++] = 0;
                //信号品质
				pBuf[wLen++] = 0;
                //搜到的时间
				memcpy( pBuf+wLen, (BYTE*)&tSearchedMeter[i].Time, sizeof(tSearchedMeter[i].Time) );
				wLen += sizeof(tSearchedMeter[i].Time);
                //附加信息
				// pBuf[wLen++] = 0x01;
				// pBuf[wLen++] = 0x00;
			}

            //读到了单个搜表结果
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
//函数功能: 读取搜表参数
//
//参数: ClassIndex[in]: 搜表参数索引
//      pData[out]: 输出Buf
//返回值: 数据长度
//
//备注: 
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
            //每天周期搜表启用标志 
            pData[wLen++] = tCycleSInfoRom.CycleSearchFlag;
            //自动更新采集档案
            pData[wLen++] = tCycleSInfoRom.AutoUpdateArchives;
            //搜表事件生成标志
            pData[wLen++] = tCycleSInfoRom.SearchMeterEventFlag;
            //清空搜表结果选项
            pData[wLen++] = tCycleSInfoRom.ClearResultOption;
            break;
        case 1:
            //每天周期搜表启用标志 
            pData[wLen++] = tCycleSInfoRom.CycleSearchFlag;
            break;
        case 2:
            //自动更新采集档案
            pData[wLen++] = tCycleSInfoRom.AutoUpdateArchives;
            break;
        case 3:
            //搜表事件生成标志
            pData[wLen++] = tCycleSInfoRom.SearchMeterEventFlag;
            break;
        case 4:
            //清空搜表结果选项
            pData[wLen++] = tCycleSInfoRom.ClearResultOption;
            break;
        default:
            wLen = 0;
            break;
    }

	return wLen;
}
//-----------------------------------------------
//函数功能: 读取搜表策略
//
//参数: ClassIndex[in]: 搜表策略索引
//      pData[out]: 输出Buf
//返回值: 数据长度
//
//备注: 
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
            //搜表模式  
            pData[wLen++] = tCycleSInfoRom.SearchMode;
            //上电是否启动搜表 
            pData[wLen++] = tCycleSInfoRom.IsPowerUpSearch;
            //上电搜表时长
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime&0xFF);
            pData[wLen++] = (BYTE)(tCycleSInfoRom.PowerUpSearchTime>>8);
            break;
        case 1:
            //搜表模式 
            pData[wLen++] = tCycleSInfoRom.SearchMode;
            break;
        case 2:
            //上电是否启动搜表
            pData[wLen++] = tCycleSInfoRom.IsPowerUpSearch;
            break;
        case 3:
            //上电搜表时长
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
//函数功能: 读取搜表周期
//
//参数: ClassIndex[in]: 搜表周期索引
//      pData[out]: 输出Buf
//      bNum[in]:搜表周期参数个数
//返回值: 数据长度
//
//备注: 
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
        //开始时间
        memcpy(pBuf+wLen,tCycleSInfoRom.Cycle[i].Time,3);
        wLen += 3;
        //搜表时长
        memcpy(pBuf+wLen,(BYTE*)&tCycleSInfoRom.Cycle[i].Duration,2);
        wLen += 2;
	}

    *bNum = i;
	return wLen;
}

//-----------------------------------------------
//函数功能: 设置搜表参数
//
//参数: ClassIndex[in]: 搜表参数索引
//      pData[in]: 参数
//返回值: 设置结果
//
//备注: 
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
            
            //每天周期搜表启用标志 
            tCycleSInfoRom.CycleSearchFlag = pData[3];
            //自动更新采集档案
            tCycleSInfoRom.AutoUpdateArchives = pData[5];
            //搜表事件生成标志 表示是否产生搜表事件
            tCycleSInfoRom.SearchMeterEventFlag = pData[7];
            //清空搜表结果选项
            tCycleSInfoRom.ClearResultOption = pData[9];
            Dar = DAR_Success;
            break;
		case 1: //每天周期搜表启用标志 
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
        case 2: //自动更新采集档案
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
        case 3: //搜表事件生成标志 表示是否产生搜表事件
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
        case 4: //清空搜表结果选项
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
//函数功能: 设置搜表策略
//
//参数: ClassIndex[in]: 搜表策略索引
//      pData[in]: 参数
//返回值: 设置结果
//
//备注: 
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

            //搜表模式 
            tCycleSInfoRom.SearchMode = pData[3];
            //上电是否启动搜表
            tCycleSInfoRom.IsPowerUpSearch = pData[5];
            //上电搜表时长
            tCycleSInfoRom.PowerUpSearchTime = (WORD)(pData[7]<<8) | pData[8] ;
            Dar = DAR_Success;
            break;
		case 1: //搜表模式 
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
        case 2: //上电是否启动搜表
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
        case 3: //上电搜表时长
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
//函数功能: 设置搜表周期
//
//参数: ClassIndex[in]: 搜表周期索引
//      pData[out]: 参数
//返回值: 设置结果
//
//备注: 
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

    //只允许所有周期一起设
    if(ClassIndex != 0)
    {
        return DAR_Undefined;
    }

    //格式判断
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

    //读取RAM中的搜表周期
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

    //比较新的搜表周期是否符合升序排列
   for(i = 0;i<ArrayLen;i++)
    {
        SourceSecond = Cycle[i].tStartTime.hour*60;
        SourceSecond = (SourceSecond+Cycle[i].tStartTime.minute)*60;
        SourceSecond += Cycle[i].tStartTime.second;

        //确保写入的开始时间大于前一个的开始时间 //确保写入的开始时间大于等于前一个的停止搜表时间
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
        //确保写入的开始时间小于后一个的开始时间 //确保写入的结束时间小于等于后一个的开始时间
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

    //符合升序排列
    if(i>=ArrayLen)
    {
        //写入的元素个数不足，补齐
        if(ArrayLen < SEARCH_CYCLE_TIMES)
        {
            for(i = ArrayLen; i<SEARCH_CYCLE_TIMES;i++)
            {
                memcpy((BYTE*)&Cycle[i],(BYTE*)&Cycle[ArrayLen - 1],sizeof(TOad60020900));
            }
        }
        memcpy((BYTE*)tCycleSInfoRom.Cycle,(BYTE*)Cycle,sizeof(tCycleSInfoRom.Cycle));
        tCycleSInfoRom.CycleNum = ArrayLen;   //更新搜表周期数
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
//函数功能: 设置搜表
//
//参数: ClassAttribute[in]: 属性
//      ClassIndex[out]: 索引
//      pData[in]:参数
//返回值: 数据长度
//
//备注: 
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
