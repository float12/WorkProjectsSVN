//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	2021.09.17
//描述		模拟串口(2400,无校验，一位停止位)
//修改记录	修改波特率需修改宏定义SCI_SIM_BPS，修改校验位，停止位等需要修改BuildSendData()
//----------------------------------------------------------------------
#include "AllHead.h"
#include "hc32f4xx_conf.h"
#include "hc32_ddl.h"
#include "cpuHC32F4A0.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// #define EVENTOUT_IRQ				Int001_IRQn

#define SEND_DATA_BIT				12			//发送字节数(1位起始位+8位数字+1位校验位+1位停止位+1位空白)
#define REC_DATA_BIT				9			//接收字节数(8位数字+1位校验位(没判断))
#define SCI_SIM_BPS					2400		//模拟串口波特率(已测试:1200/2400/4800/9600/19200)
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

typedef struct 
{
    BYTE DataBitCnt;
    WORD DataRegister;
    WORD SciOneBitTime;//us
}TSimulateSci;

TSimulateSci SimulateSci;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static void SimulateSciHandler(void);
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能:计算偶校验
//
//参数:	
//		pAddr[in]: 要发送的地址
//返回值:TRUE  1的个数为奇数
//       FALSE 1的个数为偶数
//	无
//
//备注:
//-----------------------------------------------
static WORD CalcVerify( WORD Data )
{
	WORD i;
	WORD j;

	j = 0;

	for(i=0; i<8; i++)
	{
		if( (Data & 0x0001) == 0x0001 )
		{
			j ++;
		}

		Data >>= 1;
	}

	if( j & 0x0001 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//-----------------------------------------------
//函数功能:组一个字节发送数据
//
//参数:	Data[in]: 要发送数据
//		
//返回值:数据
//
//备注:1位起始位，8位数据位，一位校验位，一位停止位(改数据位，校验位，则需改这个函数)
//-----------------------------------------------
static WORD BuildSendData( BYTE Data )
{
	WORD TmpData;
	//WORD Verify;
	
	TmpData = Data;
	TmpData <<= 1;
	TmpData |= 0xfe00;

	//偶校验配置
	//Verify = CalcVerify( Data );
	
	//if( Verify == FALSE )
	//{
	//	TmpData &= ~(0x0200);
	//}
	
	return TmpData;
}
//-----------------------------------------------
//函数功能:定时器中断服务函数
//
//参数:	
//		
//返回值:
//
//备注:用于收发数据
//-----------------------------------------------
// static void TMR02_IrqHandler(void)
// {
// 	if(TIMER0_GetFlag(M4_TMR02,Tim0_ChannelB) == 1)
// 	{
// 		TIMER0_ClearFlag(M4_TMR02,Tim0_ChannelB);
// 		SimulateSciHandler();
// 	}
// }
//-----------------------------------------------
//函数功能:外部中断中断服务函数
//
//参数:	
//		
//返回值:
//
//备注:用于判断接收起始符
//-----------------------------------------------
static void EIRQ7_IrqHandler(void)
{
	// if(EXINT_IrqFlgGet(ExtiCh07) == 1)
	// {
	// 	EXINT_IrqFlgClr(ExtiCh07);
	// 	//收到起始位，关闭外部中断
	// 	NVIC_EnableIRQ(EVENTOUT_IRQ);
		
	// 	//信号通过光耦器件会引起畸变
	// 	//为防止信号畸变导致的误码，这里需要延时一定时间，确保信号电平已正常建立
	// 	//当前条件下，最小取值为256，可保证300~9600波特率可用
	// 	for(uint16_t n=0; n<358; n++)
	// 		__ASM("NOP");
		
	// 	//打开定时器，接收数据
	// 	M4_TMR02->CNTBR = 0x00000000ul;
	// 	TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
	// }
}
//-----------------------------------------------
//函数功能:初始化模拟串口使用定时器
//
//参数:	bpstimer[in]:波特率
//		
//返回值:
//
//备注:
//-----------------------------------------------
static void InitSciTimer(WORD bpstimer)
{
    // stc_tim0_base_init_t stcTimerCfg;
    // stc_irq_regi_conf_t stcIrqRegiConf;
    // stc_exint_config_t stcExintCfg;
    // uint32_t u32EncodePriority;

    // MEM_ZERO_STRUCT(stcTimerCfg);
    // MEM_ZERO_STRUCT(stcIrqRegiConf);

    // /* ENABLE XTAL32 */
    // CLK_Xtal32Cmd(ENABLE);

    // /* Timer0 peripheral enable */
    // PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM02, ENABLE);

    // TIMER0_DeInit(M4_TMR02,Tim0_ChannelB);

    // /*config register for channel B */
    // stcTimerCfg.Tim0_CounterMode = Tim0_Sync;
    // stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;
    // stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv4;
    // stcTimerCfg.Tim0_CmpValue = (uint16_t)bpstimer * 3 + 1;
    // TIMER0_BaseInit(M4_TMR02,Tim0_ChannelB,&stcTimerCfg);

    // /* ENABLE channel B interrupt */
    // TIMER0_IntCmd(M4_TMR02,Tim0_ChannelB,ENABLE);
    // /* Register TMR_INI_GCMB Int to Vect.No.002 */
    // stcIrqRegiConf.enIRQn = Int000_IRQn;
    // /* Select I2C Error or Event interrupt function */
    // stcIrqRegiConf.enIntSrc = INT_TMR02_GCMB;
    // /* Callback function */
    // stcIrqRegiConf.pfnCallback = &TMR02_IrqHandler;
    // /* Registration IRQ */
    // enIrqRegistration(&stcIrqRegiConf);
    // /* Clear Pending */
    // NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    // /* Set priority */
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_01);//设置为高抢占优先级，高子优先级
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    // /*start timer0*/
    // TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);

    // //开启外部中断，检测接收信号
	
    // MEM_ZERO_STRUCT(stcExintCfg);
    
    // PWC_Fcg2PeriphClockCmd(PWC_STOPWKUPEN_EIRQ7, ENABLE);

    // stcExintCfg.enExitCh = ExtiCh07;
    // stcExintCfg.enFilterEn = DISABLE;
    // stcExintCfg.enFltClk = Pclk3Div1;
    // stcExintCfg.enExtiLvl = ExIntFallingEdge;
    
    // EXINT_Init(&stcExintCfg);
    
    // /* Register TMR_INI_GCMB Int to Vect.No.002 */
    // stcIrqRegiConf.enIRQn = EVENTOUT_IRQ;
    // /* Select I2C Error or Event interrupt function */
    // stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ7;
    // /* Callback function */
    // stcIrqRegiConf.pfnCallback = &EIRQ7_IrqHandler;
    // /* Registration IRQ */
    // enIrqRegistration(&stcIrqRegiConf);
    // /* Clear Pending */
    // NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    // /* Set priority */
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);//设置为高抢占优先级，高子优先级
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
}

//-----------------------------------------------
//函数功能:初始化模拟串口
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateInitSci(BYTE SciPhNum)
{
    // BYTE BpsBak, IntStatus, Bps;
    // DWORD bpstimer = 0;

    // if(SciPhNum != SCI_SIMULATION1_NUM)
    // {
    // 	return FALSE;
    // }
    
    // SimulateSci.DataBitCnt = 0;
    // SimulateSci.DataRegister = 0;
    // SimulateSci.SciOneBitTime = 0;
    
	// IntStatus = api_splx(0);
    // bpstimer = 1000000/SCI_SIM_BPS;	//此处波特率是多少就填多少

	// //打开电源控制
    // OPEN_BLUETOOTH_POWER;
    
    // //模拟串口初始化
    // InitSciTimer(bpstimer);
    
    // api_InitSubProtocol(ePT_BLUETOOTH);
    // //串口监视定时器
    // Serial[ ePT_BLUETOOTH ].WatchSciTimer = WATCH_SCI_TIMER;

    // api_splx(IntStatus);
    return TRUE;
}
//-----------------------------------------------
//函数功能:接收使能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciRcvEnable(BYTE SciPhNum)
{
    // stc_exint_config_t stcExintCfg;
	// stc_irq_regi_conf_t stcIrqRegiConf;
    // uint32_t u32EncodePriority;

    // if(SciPhNum != SCI_SIMULATION1_NUM)
    // {
    // 	return FALSE;
    // }
	
	// //打开RX中断
    // MEM_ZERO_STRUCT(stcExintCfg);
    
    // PWC_Fcg2PeriphClockCmd(PWC_STOPWKUPEN_EIRQ7, ENABLE);

    // stcExintCfg.enExitCh = ExtiCh07;
    // stcExintCfg.enFilterEn = DISABLE;
    // stcExintCfg.enFltClk = Pclk3Div1;
    // stcExintCfg.enExtiLvl = ExIntFallingEdge;
    
    // EXINT_Init(&stcExintCfg);
    
    // /* Register TMR_INI_GCMB Int to Vect.No.002 */
    // stcIrqRegiConf.enIRQn = EVENTOUT_IRQ;
    // /* Select I2C Error or Event interrupt function */
    // stcIrqRegiConf.enIntSrc = INT_PORT_EIRQ7;
    // /* Callback function */
    // stcIrqRegiConf.pfnCallback = &EIRQ7_IrqHandler;
    // /* Registration IRQ */
    // enIrqRegistration(&stcIrqRegiConf);
    // /* Clear Pending */
    // NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    // /* Set priority */
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);//设置为高抢占优先级，高子优先级
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    return TRUE;
}
//-----------------------------------------------
//函数功能:接收失能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciRcvDisable(BYTE SciPhNum)
{
	// if(SciPhNum != SCI_SIMULATION1_NUM)
    // {
    // 	return FALSE;
    // }
    // /* DISABLE NVIC */
    // NVIC_DisableIRQ(EVENTOUT_IRQ);

	// Serial[ePT_BLUETOOTH].WatchSciTimer = WATCH_SCI_TIMER;
    
    return TRUE;
}
//-----------------------------------------------
//函数功能:发送使能
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciSendEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//函数功能:发送SCI第一个字节
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
BYTE SimulateSciBeginSend(BYTE SciPhNum)
{
    // if(SciPhNum != SCI_SIMULATION1_NUM)
    // {
    // 	return FALSE;
    // }
    
	// if(Serial[ePT_BLUETOOTH].TXPoint != 0)
	// {
	// 	return FALSE;
	// }
    // //准备第一个字符
    // SimulateSci.DataRegister = BuildSendData(Serial[ePT_BLUETOOTH].ProBuf[Serial[ePT_BLUETOOTH].TXPoint++]);
    // //发送12位，
    // SimulateSci.DataBitCnt = 0;
	// //打开定时器中断
	// M4_TMR02->CNTBR = 0x00000000ul;
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
    
    return TRUE;
}
//-----------------------------------------------
//函数功能:发送SCI下一个字节
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:
//-----------------------------------------------
static void SimulateSciTxNextByte(BYTE SerialNo)
{
    // BYTE IntStatus;
    
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);
	
    // SimulateSci.DataBitCnt = 0;
    // SimulateSci.DataRegister = 0;
	// //发送完成后初始化串口
	// if( Serial[ePT_BLUETOOTH].TXPoint >= Serial[ePT_BLUETOOTH].SendLength )
	// {
    //     //立转接收状态
    //     DealSciFlag(ePT_BLUETOOTH);
    //     SerialMap[ePT_BLUETOOTH].SCIEnableRcv(SerialMap[ePT_BLUETOOTH].SCI_Ph_Num);
    //     IntStatus = api_splx(0);
	//     api_InitSubProtocol(ePT_BLUETOOTH);
    //     api_splx(IntStatus);
	// 	return;
	// }
	
    // //准备一个字符
    // SimulateSci.DataRegister = BuildSendData(Serial[ePT_BLUETOOTH].ProBuf[Serial[ePT_BLUETOOTH].TXPoint++]);
    
	// //打开定时器中断
	// M4_TMR02->CNTBR = 0x00000000ul;
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
}
//-----------------------------------------------
//函数功能:接收SCI下一个字节
//
//参数:	SciPhNum[in]:串口号
//		
//返回值:成功/失败
//
//备注:会在这个地方处理上一个收到的数据
//-----------------------------------------------
static void SimulateSciRxNextByte(BYTE SerialNo)
{	
	// //关闭定时器中断
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);
	
	// //打开RX中断
	// NVIC_EnableIRQ(EVENTOUT_IRQ);
	
    // if(Serial[ePT_BLUETOOTH].RXWPoint < MAX_PRO_BUF)
    // {
    //     Serial[ePT_BLUETOOTH].ProBuf[Serial[ePT_BLUETOOTH].RXWPoint++] = SimulateSci.DataRegister;
    // }
	// if( Serial[ePT_BLUETOOTH].RXWPoint >= MAX_PRO_BUF )
	// {
	// 	Serial[ePT_BLUETOOTH].RXWPoint = 0;
	// }
    
    // SimulateSci.DataBitCnt = 0;
    // SimulateSci.DataRegister = 0;
}
//-----------------------------------------------
//函数功能:接收/发送中断函数
//
//参数:	
//		
//返回值:
//
//备注:嵌套于定时器中断中使用
//-----------------------------------------------
static void SimulateSciHandler(void)
{
    // if( Serial[ePT_BLUETOOTH].SendLength == 0x00 )
    // {
    //     if(SIM_RX_GET_DATA)
    //     {
    //         SimulateSci.DataRegister |= (uint16_t)(1 << SimulateSci.DataBitCnt);
    //     }
    //     else
    //     {
    //         SimulateSci.DataRegister &= ~(uint16_t)(1 << SimulateSci.DataBitCnt);
    //     }

    //     SimulateSci.DataBitCnt++;
    //     if( SimulateSci.DataBitCnt >= REC_DATA_BIT )
    //     {
    //         SimulateSciRxNextByte( ePT_BLUETOOTH );
    //     }
    // }
    // else
    // {
    //     if( SimulateSci.DataBitCnt >= SEND_DATA_BIT )
    //     {
    //         SimulateSciTxNextByte( ePT_BLUETOOTH );
    //     }
    //     else
    //     {
    //         if(SimulateSci.DataRegister & (1 << SimulateSci.DataBitCnt))
    //         {
    //             SIM_TX_HIGH_DATA;
    //         }
    //         else
    //         {
    //             SIM_TX_LOW_DATA;
    //         }
    //         SimulateSci.DataBitCnt++;
    //     }
    // }
}
