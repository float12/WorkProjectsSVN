
#include "appcfg.h"
#if(MD_MDLT645_07)
#include "mdlt645.h"
#include "../GDW698/GDW698DataDef.h"

// extern BYTE GetReal2FrzFlg(void);
// extern BOOL GetCurvStartTimeEx(TTime *pTime,BYTE *pbyStepNum);

// static void _Tx_645_07_Read(DWORD dwDI,BYTE *pData,BYTE byDataLen)
// {
// TPort *pPort=(TPort*)GethPort();	
// TTx *pTx=GetTx();

// 	_Tx_Fm645Read(TRUE,dwDI,pData,byDataLen,pPort->GyRunInfo.Addr,pTx);
// 	SetSunitemFlag(TRUE,dwDI,TRUE);
// }

// static void Tx_645_07_Read(DWORD dwDI)
// {
// 	_Tx_645_07_Read(dwDI,NULL,0);
// }

// static BOOL MonDD_Tx(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_MON_APR))
// 	{//上1结算日组合有功
// 		Tx_645_07_Read(0x0000ff01);
// 		return TRUE;				
// 	}
// 	if(CheckClearFlag(MSF_MON_AP))
// 	{//正向有功
// 		Tx_645_07_Read(0x0001FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_AR))
// 	{//反向有功
// 		Tx_645_07_Read(0x0002FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_RP))
// 	{//正向无功
// 		Tx_645_07_Read(0x0003FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_RR))
// 	{//反向无功
// 		Tx_645_07_Read(0x0004FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R1))
// 	{//一象限无功
// 		Tx_645_07_Read(0x0005FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R2))
// 	{//二象限无功
// 		Tx_645_07_Read(0x0006FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R3))
// 	{//三象限无功
// 		Tx_645_07_Read(0x0007FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R4))
// 	{//四象限无功
// 		Tx_645_07_Read(0x0008FF01);
// 		return TRUE;
// 	}
// 	//新扩
// 	if(CheckClearFlag(TBD))
// 	{//铜损补偿电能
// 		Tx_645_07_Read(0x00850001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(TBD))
// 	{//铁损补偿电能
// 		Tx_645_07_Read(0x00860001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_A))
// 	{//A相正向有功
// 		if(Sunitem(TRUE,0x00150001,MSF_MON_A,3,HL,0x16,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_MON_B))
// 	{//B相
// 		if(Sunitem(TRUE,0x00290001,MSF_MON_B,3,HL,0x2A,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_MON_C))
// 	{//C相
// 		if(Sunitem(TRUE,0x003D0001,MSF_MON_C,3,HL,0x3E,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

// static BOOL MonXL_Tx(void)
// {
// 	if(CheckClearFlag(MSF_MONXLAP))
// 	{//上月正向有功需量
// 		Tx_645_07_Read(0x0101FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLAR))
// 	{//上月反向有功需量
// 		Tx_645_07_Read(0x0102FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLRP))
// 	{//上月正向无功需量
// 		Tx_645_07_Read(0x0103FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLRR))
// 	{//上月反向无功需量
// 		Tx_645_07_Read(0x0104FF01);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// static BOOL MeterTime_Tx(void)
// {
// 	if(CheckClearFlag(MSF_DATE))
// 	{//日期		
// 		Tx_645_07_Read(0x04000101);		
// 		return TRUE;
// 	}	
// 	if(CheckClearFlag(MSF_TIME))
// 	{//时间 
// 		Tx_645_07_Read(0x04000102);
// 		return TRUE;
// 	}	
// 	return FALSE;
// }

// static BOOL MonData_Tx(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_AUTODAY))
// 	{//第一自动抄表日/*山东第二、三自动抄表日*/
// 		if(Sunitem(TRUE,0x04000B01,MSF_AUTODAY,2,LL,2,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	//月冻结电量
// 	if(MonDD_Tx())
// 		return TRUE;
// 	//月冻结需量
// 	if(MonXL_Tx())
// 		return TRUE;
// 	return FALSE;
// }

// static BOOL DayData_Tx(void)
// {
// TPort *pPort=(TPort *)GethPort();
// BYTE byOffset=pPort->GyRunInfo.byHisDataOffset&0x7F;		

// 	if(CheckClearFlag(MSF_DAY_TM))
// 	{//上一次冻结时间
// 		pPort->GyRunInfo.byTxIDFlag = MSF_DAY_TM;
// 		Tx_645_07_Read(0x05060001+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_AP))
// 	{//上1次日冻结正向有功
// 		Tx_645_07_Read(0x05060101+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_AR))
// 	{//上1次日冻结反向有功
// 		Tx_645_07_Read(0x05060201+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_RP))
// 	{//上1次日冻结组合无功1
// 		Tx_645_07_Read(0x05060301+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_RR))
// 	{//上1次日冻结组合无功2
// 		Tx_645_07_Read(0x05060401+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R1))
// 	{//上1次一象限无功
// 		Tx_645_07_Read(0x05060501+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R2))
// 	{//上1次二象限无功
// 		Tx_645_07_Read(0x05060601+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R3))
// 	{//上1次三象限无功
// 		Tx_645_07_Read(0x05060701+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R4))
// 	{//上1次四象限无功
// 		Tx_645_07_Read(0x05060801+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYXLAP))
// 	{//上一次日冻结正有需量
// 		Tx_645_07_Read(0x05060901+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYXLAR))
// 	{//上一次日冻结反有需量
// 		Tx_645_07_Read(0x05060A01+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYLEFTPAY))
// 	{
// 		Tx_645_07_Read(0x05080201+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAYNUM))
// 	{//上1次购电后总购电次数
// 		Tx_645_07_Read(0x03330201);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAYSUM))
// 	{//上1次购电后累计购电金额
// 		Tx_645_07_Read(0x03330601);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_LEFTDD))
// 	{//剩余金额
// 		Tx_645_07_Read(0x00900200);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_OVERDRAFT))
// 	{//透支金额
// 		Tx_645_07_Read(0x00900201);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYP07))
// 	{
// 		Tx_645_07_Read(0x05061001+byOffset);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Cure_Tx
// //	函数功能 : 曲线数据招测
// //	处理过程 : 
// //	备    注 : 
// //	作    者 : 张东
// //	时    间 : 2016年2月17日
// //	返 回 值 : BOOL
// //	参数说明 : void
// ///////////////////////////////////////////////////////////////
// static BOOL Cure_Tx(void)
// {//曲线
// BYTE bystep =1;	
// TPort *pPort=(TPort *)GethPort();
// 	if(CheckClearFlag(MSF_CURE))
// 	{
// 		TTime Time;		
// 		if(GetCurvStartTime(&Time))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= 1;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06000001,Buf,6);
// 			SetFlag(MSF_CURE);
// 			return TRUE;
// 		}		
// 	}
// 	if(pPort->GyRunInfo.byTxIDFlag == 0xFF)
// 		pPort->GyRunInfo.byTxIDFlag = MSF_CURE1;//此处先置，若return FALSE，在退出前清掉 
// 	if(CheckClearFlag(MSF_CURE1))
// 	{//电压
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100101,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE2))
// 	{//电流
// 		TTime Time;	
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100201,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE3))
// 	{//功率
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100300,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE4))
// 	{//功率因数
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100500,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE5))
// 	{//正有
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100601,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE6))
// 	{//正有
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100602,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_CURE1)
// 		pPort->GyRunInfo.byTxIDFlag = 0xFF;
// 	return FALSE;
// }

///////////////////////////////////////////////////////////////
//	函 数 名 : DD_Txd
//	函数功能 : 电能量数据招测
//	处理过程 : 
//	备    注 : OK
//	作    者 : 张东
//	时    间 : 2016年2月17日
//	返 回 值 : BOOL
//	参数说明 : void
///////////////////////////////////////////////////////////////
// static BOOL DD_Txd(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_APR))
// 	{//组合有功
// 		Tx_645_07_Read(0x0000ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_AP))
// 	{//正向有功	
// 		if(Sunitem(TRUE,0x0001ff00,MSF_AP,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_AR))
// 	{//反向有功	
// 		if(Sunitem(TRUE,0x0002ff00,MSF_AR,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_RP))
// 	{//正向无功
// 		if(Sunitem(TRUE,0x0003ff00,MSF_RP,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_RR))
// 	{//反向无功	
// 		if(Sunitem(TRUE,0x0004ff00,MSF_RR,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_R1))
// 	{//一象限无功	
// 		Tx_645_07_Read(0x0005ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R2))
// 	{//二象限无功	
// 		Tx_645_07_Read(0x0006ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R3))
// 	{//三象限无功	
// 		Tx_645_07_Read(0x0007ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R4))
// 	{//四象限无功	
// 		Tx_645_07_Read(0x0008ff00);
// 		return TRUE;
// 	}

// 	if(CheckClearFlag(MSF_A_ARP))
// 	{//A相点能量
// 		if(Sunitem(TRUE,0x00150000,MSF_A_ARP,3,HL,0x16,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}		
// 	if(CheckClearFlag(MSF_B_ARP))
// 	{//B相
// 		if(Sunitem(TRUE,0x00290000,MSF_B_ARP,3,HL,0x2A,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_C_ARP))
// 	{//C相
// 		if(Sunitem(TRUE,0x003D0000,MSF_C_ARP,3,HL,0x3E,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : XL_Txd
// //	函数功能 : 需量数据招测
// //	处理过程 : 
// //	备    注 : ok
// //	作    者 : 张东
// //	时    间 : 2016年2月17日
// //	返 回 值 : BOOL
// //	参数说明 : void
// ///////////////////////////////////////////////////////////////
// static BOOL XL_Txd(void)
// {
// 	if(CheckClearFlag(MSF_APXL))
// 	{
// 		Tx_645_07_Read(0x0101ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_ARXL))
// 	{
// 		Tx_645_07_Read(0x0102ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_RPXL))
// 	{
// 		Tx_645_07_Read(0x0103ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_RRXL))
// 	{
// 		Tx_645_07_Read(0x0104ff00);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : SS_Txd
// //	函数功能 : 瞬时量数据招测
// //	处理过程 : 
// //	备    注 : ok
// //	作    者 : 张东
// //	时    间 : 2016年2月17日
// //	返 回 值 : BOOL
// //	参数说明 : void
// ///////////////////////////////////////////////////////////////
// static BOOL SS_Txd(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_VT))
// 	{//电压
// 		if(Sunitem(TRUE,0x0201FF00,MSF_VT,3,LH,1,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_CT))
// 	{//电流
// 		if(Sunitem(TRUE,0x0202ff00,MSF_CT,3,LH,1,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PA))
// 	{//有功功率
// 		if(Sunitem(TRUE,0x0203ff00,MSF_PA,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PR))
// 	{//无功功率
// 		if(Sunitem(TRUE,0x0204ff00,MSF_PR,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAP))
// 	{//视在功率
// 		if(Sunitem(TRUE,0x0205FF00,MSF_PAP,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PE))
// 	{//功率因数
// 		if(Sunitem(TRUE,0x0206ff00,MSF_PE,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_VIANGLE))
// 	{//相位角
// 		Tx_645_07_Read(0x0207FF00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_I0))
// 	{//零序电流
// 		Tx_645_07_Read(0x02800001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_FREQ))
// 	{//频率
// 		Tx_645_07_Read(0x02800002);
// 		return TRUE;
// 	}	
// 	if(CheckClearFlag(MSF_STSMET))
// 	{//电表状态字
// 		Tx_645_07_Read(0x040005FF);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_STSMET1))
// 	{//电表状态字1
// 		Tx_645_07_Read(0x04000501);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_STSREST))
// 	{//周休日状态字
// 		Tx_645_07_Read(0x04000801);
// 		return TRUE;
// 	}

// 	return FALSE;
// }


// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Rx_VCV 
// //	函数功能 : 新645规约电压接收处理
// //	处理过程 : 
// //	备    注 : 电压(0.1V)
// //	作    者 : 蒋剑跃
// //	时    间 : 2009年1月17日
// //	返 回 值 : void
// //	参数说明 : WORD wNo 
// ///////////////////////////////////////////////////////////////
// static void Rx_VCV(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// WORD wVCV;

// 	for(byNo=0;(byNo<(byLen/2))&&(byNo<3);byNo++)
// 	{//将BCD转化为二进制
// 		wVCV = MW(pBuf[1],pBuf[0]);
// 		if(IsBCD(wVCV))
// 		{
// 			wVCV = WBcd2Bin(wVCV)*10;
// 			//保存电压
// 			WriteYC(byNo+wNo,wVCV);						
// 		}
// 		pBuf += 2;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Rx_VCI 
// //	函数功能 : 新645规约电流接收处理
// //	处理过程 : 
// //	备    注 : 电流(0.001A)
// //	作    者 : 蒋剑跃
// //	时    间 : 2009年1月17日
// //	返 回 值 : void
// //	参数说明 : BYTE byNo
// ///////////////////////////////////////////////////////////////
// static void Rx_VCI(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {
// BYTE byI;
// DWORD dwVCI;
	
// 	for(byI=0;(byI<(byLen/3))&&(byI<3);byI++)
// 	{
// 		//将BCD转化为二进制
// 		dwVCI = MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwVCI))
// 		{
// 			dwVCI = DWBcd2Bin(dwVCI);			
// 			if(pBuf[2]&0x80)	//取符号					
// 				WriteYC(byNo+byI,(long)(0-dwVCI));
// 			else
// 				WriteYC(byNo+byI,dwVCI);
// 		}
// 		//指向下一数据块
// 		pBuf += 3;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Rx_P07
// //	函数功能 : 新645规约功率接收处理(有功，无功，视在)
// //	处理过程 : 
// //	备    注 : 0.0001kW
// //	作    者 : 蒋剑跃
// //	时    间 : 2009年1月17日
// //	返 回 值 : void
// //	参数说明 : WORD wNo
// ///////////////////////////////////////////////////////////////
// static void Rx_P07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// DWORD dwData;
// BYTE byNo;

// 	for(byNo=0;(byNo<(byLen/3))&&(byNo<4);byNo++)
// 	{
// 		//将BCD转化为二进制
// 		dwData		= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwData))
// 		{
// 			dwData		= DWBcd2Bin(dwData);			
// 			//保存功率
// 			if(pBuf[2]&0x80)		//取符号
// 				WriteYC(byNo+wNo,(long)(0-dwData));
// 			else
// 				WriteYC(byNo+wNo,dwData);
// 		}
// 		//指向下一数据块
// 		pBuf += 3;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Rx_PE
// //	函数功能 : 新645规约功率因素接收处理
// //	处理过程 : 
// //	备    注 : 
// //	作    者 : 蒋剑跃
// //	时    间 : 2009年1月17日
// //	返 回 值 : void
// //	参数说明 : WORD wNo
// ///////////////////////////////////////////////////////////////
// static void Rx_PE07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// DWORD dwPEVal;//改成DWORD 防止越界
// BYTE byNo;
		
// 	for(byNo=0;(byNo<(byLen/2))&&(byNo<4);byNo++)
// 	{
// 		//将BCD转化为二进制
// 		dwPEVal = MDW(0,0,(BYTE)(pBuf[1]&0x7f),pBuf[0]);
// 		if(IsBCD(dwPEVal))
// 		{
// 			dwPEVal = DWBcd2Bin(dwPEVal)*10;			
// 			//保存功率因素
// 			if(pBuf[1]&0x80)	//取符号
// 				WriteYC(byNo+wNo,(long)(0-dwPEVal));
// 			else
// 				WriteYC(byNo+wNo,dwPEVal);
// 		}
// 		//指向下一数据块
// 		pBuf += 2;
// 	}
// }

// static void Rx_MetStatus07(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {

		
// }

// static void Rx_XB(BYTE *pBuf,BYTE byLen,WORD wNo)
// {

// }

// ///////////////////////////////////////////////////////////////
// //	函 数 名 : Rx_XL07
// //	函数功能 : 新645需量及发生时间数据接收处理
// //	处理过程 : 
// //	备    注 : 4位小数点(0.0001kW)
// //	作    者 : 蒋剑跃
// //	时    间 : 2009年1月17日
// //	返 回 值 : void
// //	参数说明 : WORD wNo  需量数据起始点号
// //				TXL *pXL 需量值的指针
// ///////////////////////////////////////////////////////////////
// static void Rx_XL07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{	

// 		if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//无功需量且为负数
// 		{
// 			Data.dwVal  = MDW(0,pBuf[2]&0x7F,pBuf[1],pBuf[0]);
// 		}
// 		else
// 		{
// 			Data.dwVal	= MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		}
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//无功需量且为负数
// 				Data.dwVal = 0-Data.dwVal;
// 			WriteXLVal((wNo+byNo),Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);			
// 			Data.Time.Buf[0]	= 0;
// 			WriteXLTime((wNo+byNo),&Data.Time);
// 		}
// 		//指向下一数据块
// 		pBuf += 8;
// 	}
// }


// static void Rx_MonXL07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{	
// 		if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//无功需量且为负数
// 		{
// 			Data.dwVal  = MDW(0,pBuf[2]&0x7F,pBuf[1],pBuf[0]);
// 		}
// 		else
// 		{
// 			Data.dwVal	= MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		}
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//无功需量且为负数
// 				Data.dwVal = 0-Data.dwVal;
// 			WriteMonXLVal((wNo+byNo),Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);			
// 			Data.Time.Buf[0]	= 0;
// 			WriteMonXLTime((wNo+byNo),&Data.Time);
// 		}
// 		//指向下一数据块
// 		pBuf += 8;
// 	}
// }
// static void Rx_DayP07(BYTE *pBuf,BYTE byLen,BYTE byStartNo,DWORD dwSeq)
// {
// 	DWORD dwData;
// 	BYTE byNo;
	
// 	for(byNo=0;(byNo<(byLen/3))&&(byNo<8);byNo++)
// 	{
// 		//将BCD转化为二进制
// 		dwData		= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwData))
// 		{
// 			dwData		= DWBcd2Bin(dwData);			
// 			//保存功率
// 			if(pBuf[2]&0x80)		//取符号
// 				WriteYC(byNo+6,(long)(0-dwData));
// 			else
// 				WriteYC(byNo+6,dwData);
// 		}
// 		//指向下一数据块
// 		pBuf += 3;
// 	}
// }
// static void Rx_DayDD(BYTE *pBuf,BYTE byLen,BYTE byStartNo,DWORD dwSeq)
// {
// BYTE byNo;//减数据标识
// DWORD dwDD;
// #if(MD_BASEUSER	== MD_GDW698)
// long CI;
// #endif

// 	for(byNo=0;(byNo<(byLen/4))&&(byNo<5);byNo++)
// 	{
// 		//将BCD转化为二进制
// 		dwDD = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 		if ((byStartNo==10) || (byStartNo==15))
// 		{
// 			if (pBuf[3]&BIT7)
// 				dwDD = MDW((BYTE)(pBuf[3]&0x7F),pBuf[2],pBuf[1],pBuf[0]);
// 		}
		
// 		if(IsBCD(dwDD))
// 		{
// 			dwDD = DWBcd2Bin(dwDD);
// #if(MD_BASEUSER	== MD_GDW698)
// 			if ((byStartNo==10) || (byStartNo==15))//组合无功1 组合无功2最高位为符号位
// 			{
// 				if (pBuf[3]&BIT7)
// 				{
// 					dwDD = (DWORD)(0-dwDD);
// 					CI = (long)dwDD;
// 					CI *= 10;
// 					dwDD = (DWORD)CI;
// 				}
// 				else
// 					dwDD *= 10;
// 			}
// 			else
// 				dwDD *= 10;
// #else
// 			dwDD *= 10;
// #endif		
// 			WriteDayDD((DWORD)(byStartNo+byNo),dwDD);
// 		}		
// 		//指向下一数据块
// 		pBuf += 4;
// 	}
// }

// static void Rx_DayTM(BYTE *pBuf,BYTE byLen)
// {
// TTime Time,Time2;
// DWORD dwTime,dwTime2;
// TPort *pPort=(TPort *)GethPort();
// BYTE byOffset;
    
//     memset(&Time,0,sizeof(TTime));
//     Time.wYear= (WORD)(Bcd2Bin(pBuf[4])+2000);
// 	Time.Mon = Bcd2Bin(pBuf[3]);
// 	Time.Day = Bcd2Bin(pBuf[2]);
// 	Time.Hour= Bcd2Bin(pBuf[1]);
// 	Time.Min = Bcd2Bin(pBuf[0]);
// 	if(!TimeIsValid(&Time))
// 	{//冻结时间无效，直接进入下一天
// 		if (GetReal2FrzFlg() == 0x5A)	//实时转日冻结
// 			Fre2RealFlag();
// 		else
// 			ClearAllFlag();		
// 		pPort->GyRunInfo.byReqDataValid	= 0x55;		
//         return;
// 	}	
// 	byOffset	= pPort->GyRunInfo.byHisDataOffset&0x7F;	
// 	dwTime	= Time2Min(&Time);
// 	WriteOther(1,dwTime);
// 	dwTime = (dwTime/1440)*1440+byOffset*1440;
// 	if((pPort->GyRunInfo.byHisDataOffset & 0x80) == 0)
// 	{//需要判断时间
// 		GetTime(&Time2);
// 		dwTime2 = (Time2Min(&Time2)/1440)*1440;
// 		if(dwTime != dwTime2)
// 		{//时间不对不抄冻结
// 			if (GetReal2FrzFlg() == 0x5A)	//实时转日冻结
// 				Fre2RealFlag();
// 			else
// 				ClearAllFlag();
// 			pPort->GyRunInfo.byReqDataValid	= 0;	
// 		}
// 	}	
// }

// static void Rx_DayXL(BYTE *pBuf,BYTE byLen,WORD wNo,DWORD dwSeq)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{
// 		Data.dwVal = MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			WriteDayXLVal(wNo+byNo,Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);
// 			Data.Time.Buf[0]	= 0;
// 			WriteDayXLTime(wNo+byNo,&Data.Time);
// 		}
// 		//指向下一数据块
// 		pBuf += 8;
// 	}
// }

// static BYTE _SwitchToNextDataBlock(BYTE *pBuf)
// {
// BYTE byI;

// 	for(byI=0;byI<100;byI++)
// 	{
// 		if(pBuf[byI] == 0xAA)
// 		{
// 			break;
// 		}
// 	}
// 	return (BYTE)(byI+1);
// }

// static void Rx_Curve(BYTE *pBuf,BYTE byLen)
// {
// BYTE byInd=0,byCurvLen,byI,byCheckSum=0,byStep=0,byTmp;
// WORD wTmp;
// TTime TCTime;
// DWORD dwTmp;
// long nTmp;
// #if(MD_BASEUSER	== MD_GDW698)
// long CI;
// #endif

// 	ClearFlag(MSF_CURE1);ClearFlag(MSF_CURE2);ClearFlag(MSF_CURE3);
// 	ClearFlag(MSF_CURE4);ClearFlag(MSF_CURE5);ClearFlag(MSF_CURE6);
// 	//1、开始标识判断
// 	if((pBuf[0] != 0xA0)||(pBuf[1] != 0xA0))
// 	{
// 		return;
// 	}
// 	byCurvLen	= pBuf[2];
// 	if(byCurvLen > byLen)
// 		return;
// 	//2、计算校验
// #if(!MOS_MSVC)
// 	for(byI=0;byI<(BYTE)(byCurvLen+3);byI++)
// 	{
// 		byCheckSum	+= pBuf[byI];
// 	}
// 	if(byCheckSum != pBuf[byI])
// 		return;
// #endif
// 	pBuf	+= 3;
// 	while(byInd<byCurvLen)
// 	{
// 		byTmp	= _SwitchToNextDataBlock(&pBuf[0]);
// 		switch(byStep)
// 		{
// 		case 0: //时间			
// 			TCTime.wMSec = 0;
// 			TCTime.Sec	 = 0;
// 			TCTime.Min	 = Bcd2Bin(pBuf[0]);
// 			TCTime.Hour	 = Bcd2Bin(pBuf[1]);
// 			TCTime.Day	 = Bcd2Bin(pBuf[2]);
// 			TCTime.Mon	 = Bcd2Bin(pBuf[3]);
// 			TCTime.wYear = Bcd2Bin(pBuf[4])+2000;	
// 			//.if(!TimeIsValid(&TCTime))
// 			//.	return;
// 			byStep	= 1;
// 		case 1: //电压、电流、频率
// 			if(byTmp == 23)
// 			{
// 				for( byI=0; byI<3; byI ++)
// 				{//电压
// 					wTmp	= MW(pBuf[byI*2+1+5],pBuf[byI*2+5]);
// 					if(IsBCD(wTmp))
// 					{
// 						nTmp	= (long)(WBcd2Bin(wTmp)*10);					
// 						WriteCurvYC(&TCTime,byI,nTmp);
// 					}				
// 					dwTmp	= MDW(0,(BYTE)(pBuf[6+3*byI+2+5]&0x7f),pBuf[6+3*byI+1+5],pBuf[6+3*byI+5]);
// 					if(IsBCD(dwTmp))
// 					{
// 						nTmp	= (long)DWBcd2Bin(dwTmp);
// 						if(pBuf[6+3*byI+2+5] & 0x80)
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,byI+3,nTmp);
// 					}					
// 				}			
// 			}
// 			break;
// 		case 2: //有无功功率
// 			if(byTmp == 25)
// 			{
// 				for( byI=0; byI<8; byI ++)
// 				{
// 					dwTmp	= MDW(0,(BYTE)(pBuf[byI*3+2]&0x7f),pBuf[byI*3+1],pBuf[byI*3]);
// 					if(IsBCD(dwTmp))
// 					{				
// 						nTmp	= (long)DWBcd2Bin(dwTmp);
// 						if((pBuf[3*byI+2]&0x80)==0x80)//取符号
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,6+byI,nTmp);
// 					}
// 				}
// 			}
// 			break;
// 		case 3: //功率因素
// 			if(byTmp == 9)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp	= MW((BYTE)(pBuf[byI*2+1]&0x7f),pBuf[byI*2]);
// 					if(IsBCD(dwTmp))
// 					{
// 						nTmp	= (long)WBcd2Bin((WORD)dwTmp)*10;
// 						if((pBuf[byI*2+1]&0x80)==0x80)//取符号
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,byI+14,nTmp);
// 					}
// 				}
// 			}
// 			break;
// 		case 4: //有无功总电能
// 			if(byTmp == 17)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp = MDW(pBuf[byI*4+3],pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4+0]);
// 					if ((byI==2) || (byI==3))				//组合无功1 组合无功2最高位为符号位
// 					{
// 						if (pBuf[byI*4+3]&BIT7)
// 							dwTmp = MDW((BYTE)(pBuf[byI*4+3]&0x7F),pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4+0]);
// 					}
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp);
// #if(MD_BASEUSER	== MD_GDW698)
// 						if ((byI==2) || (byI==3))			//组合无功1 组合无功2最高位为符号位
// 						{
// 							if (pBuf[byI*4+3]&BIT7)
// 							{
// 								dwTmp = (DWORD)(0-dwTmp);
// 								CI = (long)dwTmp;
// 								CI *= 10;
// 								dwTmp = (DWORD)CI;
// 							}
// 							else
// 								dwTmp *= 10;
// 						}
// 						else
// 							dwTmp *= 10;
// #else
// 						dwTmp *= 10;
// #endif
// 						WriteCurvDD(&TCTime,byI*5,dwTmp);
// 					}					
// 				}
// 			}
// 			break;
// 		case 5: //四象限无功电能
// 			if(byTmp == 17)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp = MDW(pBuf[byI*4+3],pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4]);
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp)*10;
// 						WriteCurvDD(&TCTime,byI*5+20,dwTmp);
// 					}

// 				}
// 			}
// 			break;
// 		case 6: //当前需量
// 			if(byTmp == 7)
// 			{//均有符号位
// 				for( byI=0; byI<2; byI++)
// 				{
// 					dwTmp = MDW(0,(pBuf[byI*3+2]&0x7f),pBuf[byI*3+1],pBuf[byI*3]);			
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp);
// #if(MD_BASEUSER == MD_GDW698)//376规约用不到，舍去符号
// 						if((pBuf[byI*3+2]&0x80)==0x80)//负数
// 							dwTmp = 0-dwTmp;							
// #endif
// 						WriteCurvXL(&TCTime,byI,dwTmp);
// 					}
// 				}
// 			}
// 			break;
// 		default:
// 			byTmp	= 100;
// 			break;
// 		}
// 		if(byTmp >= 100)
// 				{
// 			break;
// 		}	
// 		pBuf	+= byTmp;
// 		byInd	+= byTmp;
// 		byStep++;
// 	}
// }
// static void Rx_CurveEx(BYTE no,BYTE *pBuf,BYTE byLen)
// {
// BYTE byInd=0,byCurvLen,byStep=0,byTmp;
// WORD wTmp;
// TTime TCTime;
// DWORD dwTmp;
// long nTmp;
// 	byCurvLen	= byLen;
// 	TCTime.wMSec = 0;
// 	TCTime.Sec	 = 0;
// 	TCTime.Min	 = Bcd2Bin(pBuf[0]);
// 	TCTime.Hour	 = Bcd2Bin(pBuf[1]);
// 	TCTime.Day	 = Bcd2Bin(pBuf[2]);
// 	TCTime.Mon	 = Bcd2Bin(pBuf[3]);
// 	TCTime.wYear = Bcd2Bin(pBuf[4])+2000;
// 	pBuf += 5;
// 	byInd += 5;
// 	while(byInd<byCurvLen)
// 	{
// 		switch(no)
// 		{
// 		case 1: //电压
// 			wTmp	= MW(pBuf[1],pBuf[0]);
// 			if(IsBCD(wTmp))
// 			{
// 				nTmp	= (long)(WBcd2Bin(wTmp)*10);					
// 				WriteCurvEx(&TCTime,byStep,0,nTmp);
// 			}	
// 			byTmp = 2;
// 			break;
// 		case 2: //电流
// 			dwTmp	= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{				
// 				nTmp	= (long)DWBcd2Bin(dwTmp);
// 				if((pBuf[2]&0x80)==0x80)//取符号
// 				{
// 					nTmp	= 0-nTmp;
// 				}
// 				WriteCurvEx(&TCTime,byStep,3,nTmp);
// 			}
// 			byTmp = 3;
// 			break;
// 		case 3: //功率
// 			dwTmp	= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{
// 				nTmp	= (long)WBcd2Bin((WORD)dwTmp);
// 				if((pBuf[2]&0x80)==0x80)//取符号
// 				{
// 					nTmp	= 0-nTmp;
// 				}
// 				WriteCurvEx(&TCTime,byStep,6,nTmp);
// 			}
// 			byTmp = 3;
// 			break;
// 		case 5: //功率因素
// 			dwTmp = MDW(0,0,(BYTE)(pBuf[1]&0x7f),pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{
// 				nTmp = ((long)DWBcd2Bin(dwTmp))*10;
// 				if((pBuf[1]&0x80)==0x80)//取符号
// 				{
// 					nTmp	= 0-nTmp;
// 				}				
// 				WriteCurvEx(&TCTime,byStep,14,nTmp);
// 			}	
// 			byTmp = 2;
// 			break;
// 		case 6: //有功总电能
// 			dwTmp = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 			dwTmp = DWBcd2Bin(dwTmp)*10;
// 			WriteCurvEx(&TCTime,byStep,40,dwTmp);
// 			byTmp = 4;
// 			break;
// 		case 7: //反有总电能
// 			dwTmp = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 			dwTmp = DWBcd2Bin(dwTmp)*10;
// 			WriteCurvEx(&TCTime,byStep,45,dwTmp);
// 			byTmp = 4;
// 			break;
// 		default:
// 			byTmp	= 100;
// 			break;
// 		}
// 		if(byTmp >= 100)
// 		{
// 			break;
// 		}	
// 		pBuf	+= byTmp;
// 		byInd	+= byTmp;
// 		byStep++;
// 	}
// }

// static void Rx_MetStatusUP(BYTE *pBuf,BYTE byLen)
// {//主动上报状态字12字节

// 	UData Data;
// 	Data.dwVal	= (DWORD)pBuf;
// //	WriteMeterRunInfo(DT_F31,byLen,Data);	
	
// }


void MDlt645_2007_RxMonitor(HPARA hPort)
{
	TPort *pPort=(TPort *)hPort;
#if(MD_BASEUSER	== MD_GDW698)
	TOad60170200 *pSch=(TOad60170200 *)pPort->GyRunInfo.psch698;
#endif
	BYTE *pBuf=(BYTE*)GetRxFm();
	TMDlt645Fm *pRxFm=(TMDlt645Fm *)pBuf;
	BYTE *pRxData=(BYTE *)&pRxFm[1];
	BYTE *pRead=&pRxData[4];
	DWORD dwDataType;
	BYTE byErr;
	BYTE byRLen;
	
	byRLen = MDlt645_RxMonitor(TRUE,pBuf,&dwDataType,&byErr);
	//无所请求的数据
	// if(pRxFm->byCtrl&NODATA)
	// {
	// 	if( GetFlag(MSF_CURE) || (pPort->GyRunInfo.byReqType == REQ_CURV) 
	// 		|| (pPort->GyRunInfo.byReqType == REQ_CURVEX))
	// 	{
	// 		ClearFlag(MSF_CURE);
	// 		if((pPort->GyRunInfo.byTxIDFlag != MSF_CURE1) && ((gVar.Cfg.byCurvSource == 0x55)||(pPort->GyRunInfo.byTxCurvSource == 0x55)))
	// 		{
	// 			if( (pPort->GyRunInfo.CurvCjCfg.wNo < pPort->GyRunInfo.CurvCjCfg.wNum 
	// 				&& pPort->GyRunInfo.CurvCjCfg.wNum > 1 ) 
	// 				|| ((pPort->GyRunInfo.CurvCjCfg.wNum==1) && (pPort->GyRunInfo.byReqType == REQ_CURV))  )
	// 			{
	// 				SetFlag(MSF_CURE1);SetFlag(MSF_CURE2);SetFlag(MSF_CURE3);
	// 				SetFlag(MSF_CURE4);SetFlag(MSF_CURE5);SetFlag(MSF_CURE6);
	// 			}
	// 		}
	// 	}
	// 	if((pPort->GyRunInfo.byReqType == REQ_DFRZ)
	// 		||(pPort->GyRunInfo.byReqType == REQ_CURV) 
	// 		|| (pPort->GyRunInfo.byReqType == REQ_CURVEX))
	// 	{//没有所请求的数据  
	// 		if(pPort->GyRunInfo.byReqDataValid == 0)
	// 		{
	// 			//.ClearAllFlag(); 
	// 			pPort->GyRunInfo.byReqDataValid	= 0xAA;	
	// 		}
	// 	}	
	// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_DAY_TM)
	// 	{//上一次日冻结时间回否认时
	// 		if (GetReal2FrzFlg() == 0x5A)	//实时转日冻结
	// 			Fre2RealFlag();
	// 		else
	// 			ClearAllFlag();
	// 		pPort->GyRunInfo.byReqDataValid	= 0xAA;	//为了实现补抄增加。如果是实时转日冻结，此处增加无影响因为在存储数据时会将valid置成1
	// 	}

	// }
	// if((byErr != 0)||(byRLen == 0))
	// {
	// 	return;
	// }

	// SetSunitemFlag(TRUE,dwDataType,FALSE);
	// switch(dwDataType) 
	// {
	// case 0x04000101: Rx_Date(pRead);					return;//日期
	// case 0x04000102: Rx_Time(pRead);					return;//时间
	// case 0x0000ff00: Rx_DD(pRead,byRLen,52);			return;//组合有功
	// case 0x0001ff00: //正向有功
	// case 0x00010000:
	// case 0x0002ff00:
	// case 0x00020000: Rx_DD_Ex(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;
	// case 0x0003ff00:
	// case 0x00030000:
	// case 0x0004ff00:
	// case 0x00040000:
	// case 0x0005ff00:
	// case 0x0006ff00:
	// case 0x0007ff00:
	// case 0x0008ff00: Rx_DD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;
	// //分相计量数据
	// case 0x00150000:
	// case 0x00160000:
	// case 0x00170000:
	// case 0x00180000:
	// case 0x00290000:
	// case 0x002A0000:
	// case 0x002B0000:
	// case 0x002C0000:
	// case 0x003D0000:
	// case 0x003E0000:
	// case 0x003F0000:
	// case 0x00400000: Rx_DD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)%0x14+(HLBYTE(dwDataType)/0x14-1)*4+39)) ;return;
	
	// case 0x00850000: Rx_DD(pRead,byRLen,52);			return;
	// case 0x00860000: Rx_DD(pRead,byRLen,53);			return;

	// case 0x0201ff00: Rx_VCV(pRead,byRLen,0);			return;
	// case 0x02010100:
	// case 0x02010200:
	// case 0x02010300: Rx_VCV(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)-1));	return;
	// case 0x0202ff00: Rx_VCI(pRead,byRLen,3);			return;
	// case 0x02020100:
	// case 0x02020200:
	// case 0x02020300: Rx_VCI(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+2));	return;	//电流序号为3、4、5 
	// case 0x0203ff00: Rx_P07(pRead,byRLen,6);			return;
	// case 0x02030000: 
	// case 0x02030100:
	// case 0x02030200:
	// case 0x02030300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+6));	return;
	// case 0x0204ff00: Rx_P07(pRead,byRLen,10);			return;
	// case 0x02040000: 
	// case 0x02040100: 
	// case 0x02040200: 
	// case 0x02040300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+10));return;

	// case 0x0205FF00: Rx_P07(pRead,byRLen,25);			return;
	// case 0x02050000: 
	// case 0x02050100: 
	// case 0x02050200: 
	// case 0x02050300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+25));return;
	// case 0x0206ff00: Rx_PE07(pRead,byRLen,14);			return;
	// case 0x02060000:
	// case 0x02060100:
	// case 0x02060200:
	// case 0x02060300: Rx_PE07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+14));return;
		
	// case 0x0207FF00: Rx_VCV(pRead,byRLen,19);			return;//MSF_VIANGLE
	// case 0x02800001: Rx_VCI(pRead,byRLen,18);			return;//MSF_I0
	// //case 0x02800002: Rx_FREQ(pRead,byRLen);				return;//MSF_FREQ
	// case 0x0208FF00: Rx_XB(pRead,byRLen,0);				return;//电压谐波失真度
	// case 0x0209FF00: Rx_XB(pRead,byRLen,3);				return;//电流谐波失真度
	// case 0x04000501:
	// case 0x040005FF: Rx_MetStatus07(pRead,byRLen,0);	return;//MSF_STSMET
	// case 0x04000801: return;//MSF_STSREST

	// case 0x0101ff00: 
	// case 0x0102ff00: 
	// case 0x0103ff00: 
	// case 0x0104ff00: Rx_XL07(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));	return;//MSF_RRXL

	// //月
	// case 0x04000B01: 
	// case 0x04000B02: 
	// case 0x04000B03: Rx_AutoDay(pRead,byRLen,(BYTE)(LLBYTE(dwDataType)-1));	return;//自动第一抄表日
	// case 0x0000FF01: Rx_MonDD(pRead,byRLen,0);								return;//(上月)上1结算日组合有功
	// case 0x0001FF01: //上月正向有功
	// case 0x0002FF01: //上月反向有功
	// case 0x0003FF01: //上月正向无功
	// case 0x0004FF01: //上月反向无功
	// case 0x0005FF01: //一象限无功
	// case 0x0006FF01: //二象限无功	
	// case 0x0007FF01: //三象限无功
	// case 0x0008FF01: Rx_MonDD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));	return;//四象限无功
	// case 0x00150001:				
	// case 0x00160001:				
	// case 0x00170001:				
	// case 0x00180001:				
	// case 0x00290001:				
	// case 0x002A0001:				
	// case 0x002B0001:				
	// case 0x002C0001:				
	// case 0x003D0001:				
	// case 0x003E0001:				
	// case 0x003F0001:				
	// case 0x00400001: Rx_MonDD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)%0x14+(HLBYTE(dwDataType)/0x14-1)*4+39)) ;return;

	// case 0x0101FF01: //上月正向有功需量
	// case 0x0102FF01: //上月反向有功需量	
	// case 0x0103FF01: //上月正向无功需量
	// case 0x0104FF01: Rx_MonXL07(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;//上月反向无功需量
	// case 0x0105FF01: return;//上月一象限无功需量
	// case 0x0106FF01: return;//上月二象限无功需量	
	// case 0x0107FF01: return;//上月三象限无功需量
	// case 0x0108FF01: return;//上月四象限无功需量	
	// //曲线
	// case 0x06000001: Rx_Curve(pRead,byRLen);			return;//指定时间一个记录块

	// case 0x06100101: Rx_CurveEx(1,pRead,byRLen);			return;//A电压
	// case 0x06100201: Rx_CurveEx(2,pRead,byRLen);			return;//A电流
	// case 0x06100300: Rx_CurveEx(3,pRead,byRLen);			return;//总功率
	// case 0x06100500: Rx_CurveEx(5,pRead,byRLen);			return;//总功率因数
	// case 0x06100601: Rx_CurveEx(6,pRead,byRLen);			return;//正有电能
	// case 0x06100602: Rx_CurveEx(7,pRead,byRLen);			return;//反有电能
	// }
	// switch((dwDataType&0xFFFFFF00)+1)
	// {//日
	// case 0x05060001: Rx_DayTM(pRead,byRLen);return;
	// case 0x05060101: 
	// case 0x05060201: 
	// case 0x05060301: 
	// case 0x05060401: 
	// case 0x05060501: 
	// case 0x05060601: 
	// case 0x05060701: 
	// case 0x05060801: Rx_DayDD(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)*5-5),(dwDataType&0x0f));return;//MSF_DAY_R4
		
	// case 0x05060901: Rx_DayXL(pRead,byRLen,0,(dwDataType&0x0f));	return;//MSF_DAYXLAP
	// case 0x05060A01: Rx_DayXL(pRead,byRLen,5,(dwDataType&0x0f));	return;//MSF_DAYXLAR
	// //case 0x05080201: Rx_DayLeftPay(pRead, byRLen,0,(dwDataType&0x0f));return;
	// case 0x05061001: Rx_DayP07(pRead,byRLen,0,(dwDataType&0x0f));return;
	// }
}


// BOOL MDlt645_2007_TxMonitor(HPARA hPort)
// {
// TPort *pPort = (TPort*)hPort;
	
// 	if(pPort->GyRunInfo.wMPSeqNo==0xFFFF)
// 		return FALSE;
// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_DAY_TM)
// 	{//如果抄收日冻结时间没有应答时，后面数据不再抄读
// 		if(pPort->GyRunInfo.wRxCount < pPort->GyRunInfo.wTxCount)
// 		{//抄收日冻结时间没有应答
// 			if (GetReal2FrzFlg() == 0x5A)	//实时转日冻结
// 				Fre2RealFlag();
// 			else
// 				ClearAllFlag();
// 			pPort->GyRunInfo.byReqDataValid	= 0xAA;	
// 			pPort->GyRunInfo.byTxIDFlag = 0xFF;
// 			return FALSE;	//由于此处return FALSE,所以对于上1次日冻结时间不应答的不再抄。
// 		}	
// 	}
// 	pPort->GyRunInfo.byTxIDFlag = 0xFF;
	
// 	//电表当前时间
// 	if(MeterTime_Tx())
// 		return TRUE;
// 	//电表月冻结数据
// 	if(MonData_Tx())//ok
// 		return TRUE;
// 	//上小时上日数据
// 	if(DayData_Tx())//OK
// 		return TRUE;
// 	if(Cure_Tx())
// 		return TRUE;
// 	//检查电度是否结束
// 	if(DD_Txd())
// 		return TRUE;
// 	//检查瞬时量是否结束
// 	if(SS_Txd())//OK
// 		return TRUE;
// 	//检查需量是否结束
// 	if(XL_Txd())//0k
// 		return TRUE;	
// 	return FALSE;
// }

#endif

