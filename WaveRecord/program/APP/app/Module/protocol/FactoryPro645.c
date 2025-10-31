//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	张玉猛
//创建日期	2016.9.23
//描述		厂内功能，供规约调用
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "SaveMem.h"
#include "ff.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
TFourByteUnion randtemp;
const char keys[]="DYTQ";
extern BYTE TopoErrFlag;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------



//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//判断下发报文是否正确
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
   0x0201,//厂内模式
   0x0206,//低级厂内模式
   0x0205,//需量进位
   0x0C00,//报警继电器
   0x0C01,//lcd小铃铛报警控制参数
   0x0C02,//液晶背光
   0x0C03,//液晶err报警
};


//-----------------------------------------------
//函数功能: 扩展规约读函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						
//返回值:	返回读取数据长度
//		   
//备注:规约见《电表软件平台扩展规约》:抄读时在04dfdfdb后面扩展4个字节，设置时在数据域扩展8个字节（扩展字节以下简称扩展标识）。
//	第1,2字节为厂内扩展数据标识，分别对应DI0，DI1.
//	第3,4字节固定为0xdf，0xdb。
//	第5,6,7,8字节为当前时间与00年1月1日0时0分的相对分钟数（hex码，传输时高在后，低在前）（抄读时无此数据）。
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
	BYTE TFTestStatus = 0;//0表示tf卡测试异常，1表示正常
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
	//返回数据要添加两个字节的扩展数据标识
	wReturnLen = 0;
		
	switch(pBuf[1])
	{
		case 0x00://自由事件记录		
		case 0x01://异常事件记录			
			if(tw == 0)
			{
				return 0;
			}
			
			for(i=1;i<=6;i++)
			{
				if( pBuf[1] == 0x00 )//自由事件
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
			if( pBuf[0] == 0x01 )//厂内模式
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
			else if( pBuf[0] == 0x03 )//自热是否补偿相角 0-不补偿 1-补偿
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
				//所有阶梯结算日无效情况，且阶梯电价非零情况下，阶梯电价如何扣费：0--不扣费，1-按月阶梯扣费并在每月第1结算日转存阶梯用电量
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
			else if( pBuf[0] == 0x05 ) //需量最低位是否要进位 0—进位 1—不进位（下一位不足0.5的也进位加1）
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
		case 0x03://厂内保留存储区
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
				//获取电能地址，测试电能转存使用
				wReturnLen = api_GetFactoryEnergyPara(p);
				wReturnLen += 2;
			}
			break;
		case 0x04:
			if(( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//未打开厂内模式
			&&( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == FALSE ))//未打开低级厂内模式
			{
				break;
			}
			
			memcpy((BYTE*)&td,pBuf+4,4);
			i = pBuf[8];
			
			if( (pBuf[0]==0x00) || (pBuf[0]==0x01) )
			{
				//读内部ram及内部flash
				wReturnLen = api_ReadCpuRamAndInFlash( pBuf[0], td, i, p ) + 2;
			}
			else if(pBuf[0] == 0x02)//读管理芯第一片EEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x01, td, i, p ) + 2;
			
				if(SelEESoftAddrConst == YES)
				{
					if(api_CheckEEPRomAddr(1) == FALSE)//检查E2地址
					{

						wReturnLen = 0;
					}
				}
			}
			else if(pBuf[0] == 0x03)//读管理芯第二片EEPROM
			{
				wReturnLen = api_ReadSaveMem( 0x02, td, i, p ) + 2;
				if(SelEESoftAddrConst == YES)
				{
					if(api_CheckEEPRomAddr(2) == FALSE)//检查E2地址
					{

						wReturnLen = 0;
					}
				}
			}
			else if(pBuf[0] == 0x04)//读管理芯FLASH
			{
				wReturnLen = api_ReadSaveMem( 0x11, td, i, p ) + 2;
			}
			else if(pBuf[0]==0x06)
			{
				//读连续空间
				wReturnLen = api_ReadSaveMem( 0x31, td, i, p ) + 2;
			}
			break;
		case 0x06://读取采样系数
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
		case 0x08://读电表版本
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
		// 	// 0 -- 报警继电器
		// 	// 1 -- LCD小铃铛报警控制参数
		// 	// 2 -- 液晶背光
		// 	// 3 -- 液晶ERR显示报警
		// 	if(pBuf[0] < 0x04)
		// 	{
		// 		api_ProcLcdCtrlPara(READ,pBuf[0],8,p);
		// 		wReturnLen = 10;
		// 	}
		// 	break;
		case 0x0D://读取大盖、端钮盖状态 0扣盖 1合盖（单相表第二字节端钮盖固定传0）
			if(pBuf[0] == 0x00)
			{
				memset( p, 0x00, 2 );
				if( UP_COVER_IS_OPEN )//上盖开盖
				{
					p[0] = 1;
				}

				#if( MAX_PHASE != 1)
				if( END_COVER_IS_OPEN )//端纽盖开盖
				{
					p[1] = 1;
				}
				#endif
				wReturnLen = 2+2;
			}
			break;
		case 0x0e://读取功率
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
			if(pBuf[0] == 0x00)//电源异常检查
			{
				#if(MAX_PHASE == 1)
	//			text[0] = HT_TMR4->TMRCNT;
				for(i=0;i<240;i++)//约668个时钟（32768）20.4ms 和高低电平状态无关
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
			else if( pBuf[0] == 0x01 ) //检查掉电保存是否完整
			{
				p[0] = api_GetPowerDownCompleteFlag();
				wReturnLen = 2+1;
			}
			#if( PREPAY_MODE == PREPAY_LOCAL )
			else if( pBuf[0] == 0x02 )//esam版本检测
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
			else if( pBuf[0] == 0x04)//晶体类型
			{
				p[0] = SelOscTypeConst;
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 6 )//读第一片E2地址
			{
				api_GetEEPRomAddr( 0, p );
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 7 )//读第二片E2地址
			{
				api_GetEEPRomAddr( 1, p );
				wReturnLen = 2+1 ;
			}
			else if( pBuf[0] == 8 )//电流
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
			else if( pBuf[0] == 9 )//EE版本
			{
				wReturnLen = sizeof(SelEESoftAddrConst);
				memcpy( p, (BYTE*)&SelEESoftAddrConst, wReturnLen );
				wReturnLen += 2;
			}
			break;
		case 0x10:
			if(pBuf[0] == 0x00)//表内错误信息字
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
		case 0x11://行业外参数
			#if(SEL_EVENT_DI_CHANGE == YES)
			if(pBuf[0] == 0x23)//获取实时遥信状态
			{
				p[0] = api_ReadDIStatus();
				wReturnLen = 1;
			}
			else if( pBuf[0] == 0x50 )//遥信防抖时间
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
			else if (pBuf[0] == 0x19)//导轨表拓展版本
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
			else if (pBuf[0] == 0x19)//导轨表拓展版本
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
			if (pBuf[0] == 0x00) // 第一路485
			{
				p[0] = ((FPara2->CommPara.I485 & 0x60) >> 5) + 1;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x01) // 第二路485
			{
				// p[0] = ((FPara2->CommPara.II485 & 0x60) >> 5) + 1;
				// wReturnLen = 3;
			}
			else if (pBuf[0] == 0x02) // 载波
			{
				p[0] = ((FPara2->CommPara.ComModule & 0x60) >> 5) + 1;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x05) // 第一路485 通信特征字
			{
				p[0] = FPara2->CommPara.I485;
				wReturnLen = 3;
			}
			else if (pBuf[0] == 0x06) // 第二路485 通信特征字
			{
				// p[0] = FPara2->CommPara.II485;
				// wReturnLen = 3;
			}
			else if (pBuf[0] == 0x07) // 载波
			{
				p[0] = FPara2->CommPara.ComModule;
				wReturnLen = 3;
			}
			break;
		case 0x13://读取上传瞬时量
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
					// 电流谐波含量
					memcpy(p, &Cur_harm_content[i][0], UPLOIAD_HARM_NUM * sizeof(float));
					p = p + UPLOIAD_HARM_NUM * sizeof(float);
					// UI特征
					memcpy(p, &UI_Trait_data[i][0], UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float));
					p = p + UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float);
				}
				//序号
				memcpy(p,&InstanDataNo, sizeof(InstanDataNo));
				InstanDataNo++;
				p = p + sizeof(InstanDataNo);
				//16为 u、i、p、q
				wReturnLen = (16 * i +  sizeof(InstanDataNo)+MAX_RELAY_NUM * UPLOIAD_HARM_NUM * sizeof(float) + MAX_RELAY_NUM * UPLOIAD_VI_CHARACTERISTIC_NUM * sizeof(float) + 2);
			}
			break;
		case 0x14://读取uik
			if (pBuf[0] == 0x07)
			{
				memcpy(p, &UIFactor,sizeof(UIFactor));
				wReturnLen = 2 + sizeof(UIFactor);
			}
			break;
		case 0x15://测试tf卡写入功能
			if (pBuf[0] == 0x07)
			{
				if(isPressed == 0)//未按下按键需要挂载
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
// 功能描述:  规约设置通信奇偶校验位
//
// 参数:
//	 Sub0[in]:	串口选择
//
//	 Sub1[in]：	设置奇偶校验值
// 返回值：成功：设置数据长度（字节）； 失败：0x8000+Err
//
// 备注内容:设置如成功返回数据长度,如失败,返回 0x8000+Err
// Sub0[in] 00-01 通信口1波特率特征字(第一路485)    通信口2波特率特征字（第二路485）
// Sub1[in] 01无校验；02奇校验；03偶校验
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
	// Bps=0x40;//强制偶校验（实测奇偶校验都能连上）
	// Bps=0x20;//强制奇校验（实测奇偶校验都能连上）
	// Bps=0;//强制无校验
	RsBaud &= ~0x60; // bit5~bit6清零

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
//函数功能: 扩展规约写函数
//
//参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB	
//						4,5,6,7--相对分钟数
//返回值:	返回值bit15为1为错误，其他为正确
//		   
//备注:规约见《电表软件平台扩展规约》
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
	
	if( api_GetSysStatus( eSYS_STATUS_INSIDE_FACTORY ) == FALSE )//未打开厂内模式
	{
		memcpy( (BYTE*)&OI, pBuf, sizeof(WORD) );
		
		if( api_GetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY ) == TRUE )//打开低级厂内模式
		{
			if( (OI == 0x0201 )//厂内模式
			|| (OI == 0x0206 )//低级厂内模式
			|| (OI == 0x0205 )//需量进位
			|| (OI == 0x0C00 )//报警继电器
			|| (OI == 0x0C01 )//lcd小铃铛报警控制参数
			|| (OI == 0x0C02 )//液晶背光
			|| (OI == 0x0C03 )//液晶err报警
			|| (OI == 0x0603 )//直流偶次谐波判断时间
			)
			{
				//可以执行
			}
			else
			{
				return 0x8001;
			}
		}
		else
		{
			if( (OI != 0x0201 ) && (OI != 0x0206 ))//只允许打开厂内模式和低级厂内模式
			{
				return 0x8001;
			}
		}
	}
	
	//判断时标
	memcpy( (BYTE*)&tdw, pBuf+4, 4 );
//	if( api_GetRunHardFlag(eRUN_HARD_ERR_RTC_FLAG) == TRUE )
//	{
//		//时间错误，不判断时标	
//	}
//	else
//	{
//		if( ((api_GetRunHardFlag(eRUN_HARD_COMMON_KEY)==TRUE) || (UP_COVER_IS_OPEN)) || (FactoryTime != 0) )
//		{
//			//公钥判断全8时标或者在（当前时间~当前时间+10080）窗口内
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
//			//私钥判断（当前时间~当前时间+10080）窗口内
//			if( (g_RelativeMin>tdw) || (tdw>(g_RelativeMin+10080)) )//1440*7=10080
//			{
//				return 0x8001;
//			}
//		}
//	}
	
	//厂内规约扩展写操作自由事件
	api_WriteFreeEvent( EVENT_FACTORY_WRITE, (pBuf[1] << 8) + pBuf[0] );
	
	wReturnLen = 0;
	
	switch(pBuf[1])
	{
		case 0x02:
			if(pBuf[0] == 0x01)
			{
				//厂内模式
				if(pBuf[8] == 0x01)
				{
					//开盖 或 公钥 或 厂内时间不为零
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
				//自热是否补偿相角 0-不补偿 1-补偿
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
				//所有阶梯结算日无效情况，且阶梯电价非零情况下，阶梯电价如何扣费：0--不扣费，1-按月阶梯扣费并在每月第1结算日转存阶梯用电量
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
				//需量最低位是否要进位 0—进位 1—不进位（下一位不足0.5的也进位加1）
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
				//低等级厂内模式-延时时间30min
				if( pBuf[8] == 0x01 )
				{
					api_SetSysStatus( eSYS_STATUS_LOW_INSIDE_FACTORY );
					LowFactoryTime = (30*60);//30分钟时效
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
		case 0x03://写厂内保留区
			if(pBuf[0] != 0x01)
			{
				break;
			}
			//偏移+长度
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
			if(pBuf[0] == 0x01)//写修正系数等
			{
				memcpy( (BYTE*)&tw, pBuf+8, 2 );
				if( api_ProcSampleCorr( WRITE, tw, pBuf[10], pBuf + 12 ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x02)//校表
			{
				//AA  HF常数  AA  A相瞬时量  AA  B相瞬时量  AA  C相瞬时量  AA
				if( api_ProcSampleAdjust( pBuf[9], pBuf + 10 ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if( pBuf[0] == 0x03 )//直流偶次谐波时间参数
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
				//电表复位操作
				wReturnLen = 1;
				// api_PowerDownEnergy();         // 掉电电能转存处理
				// Reset_CPU();				
				api_EnterLowPower(eFromOnRunEnterDownMode);
			}
			else if(pBuf[0] == 0xdb)
			{
				//电表清零及电表初始化
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
			if(pBuf[0] == 0x00)//RTC常温调校值
			{
				memcpy( (BYTE*)&tw, pBuf+8, 2 );
				if( api_CaliRtcError( tw ) != TRUE )
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0xAA)//写InfoFlash中RTC的A、B、C、D、E校正系数
			{
				ProcInfoRtcAdjustPara( WRITE, pBuf + 8 );
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0xff)//写高低温调校系数
			{
				api_ProcRtcPara(WRITE,pBuf + 10);
				//写入错误的校验和
				WriteInfoRTCErrorCheckSum();
				wReturnLen = 1;
			}
			break;
		#endif
		case 0x0a://程序升级
			if(pBuf[0] == 0x00)//开始接收升级报文
			{
				memcpy((BYTE*)&tdw,pBuf+8+6,4);
				if(tdw != 0xdfdb0001)
				{
					break;
				}
				
				api_WriteUpdateProgHead(pBuf+8);
				
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x01)//接收升级数据
			{
				Result = api_WriteUpdateProgMessage(pBuf+8);
				if(Result != TRUE)
				{
					break;
				}
				wReturnLen = 1;
			}
			else if(pBuf[0] == 0x03)//接收升级数据NEW
			{
				//Result = api_WriteUpdateProgMessageNew(pBuf+8);
				//if(Result != TRUE)
				//{
				//	break;
				//}
				//wReturnLen = 1;
			}
			else if(pBuf[0] == 0x02)//启动电表程序升级
			{
				memcpy((BYTE*)&tdw,pBuf+8+6,4);
				if(tdw != 0xa55a5aa5)
				{
					break;
				}
				
				//此函数不会有返回，在里面有个复位操作
				api_StartUpdateProgMessage(pBuf+8);
				
				wReturnLen = 1;
			}
			break;
		case 0x0c:
			// 0 -- 报警继电器
			// 1 -- LCD小铃铛报警控制参数
			// 2 -- 液晶背光
			// 3 -- 液晶ERR显示报警
			if(pBuf[0] < 0x04)
			{
				api_ProcLcdCtrlPara(WRITE,pBuf[0],8,pBuf+8);
                wReturnLen = 1;
			}
			break;
		case 0x0D:
			//校准电池电压
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
			else if( pBuf[0] == 0x50 )//遥信防抖时间
			{
				if(api_WriteDIPara(0,(BYTE*)&pBuf[8]) == TRUE)
				{
					wReturnLen = 1;
				}
			}	
			#endif
			break;
		case 0x12: // 行业外设置波特率校验
			i = pBuf[0];
			if (i < 3) // 0-2 通信口1波特率特征字(第一路485)    通信口2波特率特征字（第二路485）载波波特率特征字
			{
				if ((pBuf[8] >= 1) && (pBuf[8] <= 3)) // 01无校验；02奇校验；03偶校验
				{
					wReturnLen = SetParabaudPari(i, pBuf[8]);
				}
			}
			else if (i == 0x05) // 第一路485 通信特征字
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
			else if (i == 0x06) // 第二路485 通信特征字
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
			else if (i == 0x07) // 载波
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
			if( pBuf[0] == 0x03 )//4G模块复位命令
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




