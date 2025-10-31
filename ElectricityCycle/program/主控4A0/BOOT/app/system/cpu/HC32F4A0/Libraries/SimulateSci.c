//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	2021.09.17
//����		ģ�⴮��(2400,��У�飬һλֹͣλ)
//�޸ļ�¼	�޸Ĳ��������޸ĺ궨��SCI_SIM_BPS���޸�У��λ��ֹͣλ����Ҫ�޸�BuildSendData()
//----------------------------------------------------------------------
#include "AllHead.h"
#include "hc32f4xx_conf.h"
#include "hc32_ddl.h"
#include "cpuHC32F4A0.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
// #define EVENTOUT_IRQ				Int001_IRQn

#define SEND_DATA_BIT				12			//�����ֽ���(1λ��ʼλ+8λ����+1λУ��λ+1λֹͣλ+1λ�հ�)
#define REC_DATA_BIT				9			//�����ֽ���(8λ����+1λУ��λ(û�ж�))
#define SCI_SIM_BPS					2400		//ģ�⴮�ڲ�����(�Ѳ���:1200/2400/4800/9600/19200)
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

typedef struct 
{
    BYTE DataBitCnt;
    WORD DataRegister;
    WORD SciOneBitTime;//us
}TSimulateSci;

TSimulateSci SimulateSci;
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static void SimulateSciHandler(void);
//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������:����żУ��
//
//����:	
//		pAddr[in]: Ҫ���͵ĵ�ַ
//����ֵ:TRUE  1�ĸ���Ϊ����
//       FALSE 1�ĸ���Ϊż��
//	��
//
//��ע:
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
//��������:��һ���ֽڷ�������
//
//����:	Data[in]: Ҫ��������
//		
//����ֵ:����
//
//��ע:1λ��ʼλ��8λ����λ��һλУ��λ��һλֹͣλ(������λ��У��λ��������������)
//-----------------------------------------------
static WORD BuildSendData( BYTE Data )
{
	WORD TmpData;
	//WORD Verify;
	
	TmpData = Data;
	TmpData <<= 1;
	TmpData |= 0xfe00;

	//żУ������
	//Verify = CalcVerify( Data );
	
	//if( Verify == FALSE )
	//{
	//	TmpData &= ~(0x0200);
	//}
	
	return TmpData;
}
//-----------------------------------------------
//��������:��ʱ���жϷ�����
//
//����:	
//		
//����ֵ:
//
//��ע:�����շ�����
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
//��������:�ⲿ�ж��жϷ�����
//
//����:	
//		
//����ֵ:
//
//��ע:�����жϽ�����ʼ��
//-----------------------------------------------
static void EIRQ7_IrqHandler(void)
{
	// if(EXINT_IrqFlgGet(ExtiCh07) == 1)
	// {
	// 	EXINT_IrqFlgClr(ExtiCh07);
	// 	//�յ���ʼλ���ر��ⲿ�ж�
	// 	NVIC_EnableIRQ(EVENTOUT_IRQ);
		
	// 	//�ź�ͨ�������������������
	// 	//Ϊ��ֹ�źŻ��䵼�µ����룬������Ҫ��ʱһ��ʱ�䣬ȷ���źŵ�ƽ����������
	// 	//��ǰ�����£���СȡֵΪ256���ɱ�֤300~9600�����ʿ���
	// 	for(uint16_t n=0; n<358; n++)
	// 		__ASM("NOP");
		
	// 	//�򿪶�ʱ������������
	// 	M4_TMR02->CNTBR = 0x00000000ul;
	// 	TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
	// }
}
//-----------------------------------------------
//��������:��ʼ��ģ�⴮��ʹ�ö�ʱ��
//
//����:	bpstimer[in]:������
//		
//����ֵ:
//
//��ע:
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
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_01);//����Ϊ����ռ���ȼ����������ȼ�
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    // /*start timer0*/
    // TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);

    // //�����ⲿ�жϣ��������ź�
	
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
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);//����Ϊ����ռ���ȼ����������ȼ�
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
}

//-----------------------------------------------
//��������:��ʼ��ģ�⴮��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
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
    // bpstimer = 1000000/SCI_SIM_BPS;	//�˴��������Ƕ��پ������

	// //�򿪵�Դ����
    // OPEN_BLUETOOTH_POWER;
    
    // //ģ�⴮�ڳ�ʼ��
    // InitSciTimer(bpstimer);
    
    // api_InitSubProtocol(ePT_BLUETOOTH);
    // //���ڼ��Ӷ�ʱ��
    // Serial[ ePT_BLUETOOTH ].WatchSciTimer = WATCH_SCI_TIMER;

    // api_splx(IntStatus);
    return TRUE;
}
//-----------------------------------------------
//��������:����ʹ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
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
	
	// //��RX�ж�
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
	// u32EncodePriority = NVIC_EncodePriority(NVIC_PriorityGroup_1,HIGH_PREEMPTION_PRIORITY,DDL_IRQ_PRIORITY_00);//����Ϊ����ռ���ȼ����������ȼ�
	// NVIC_SetPriority(stcIrqRegiConf.enIRQn, u32EncodePriority);
    // /* ENABLE NVIC */
    // NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    return TRUE;
}
//-----------------------------------------------
//��������:����ʧ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
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
//��������:����ʹ��
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
BYTE SimulateSciSendEnable(BYTE SciPhNum)
{
    return TRUE;
}
//-----------------------------------------------
//��������:����SCI��һ���ֽ�
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
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
    // //׼����һ���ַ�
    // SimulateSci.DataRegister = BuildSendData(Serial[ePT_BLUETOOTH].ProBuf[Serial[ePT_BLUETOOTH].TXPoint++]);
    // //����12λ��
    // SimulateSci.DataBitCnt = 0;
	// //�򿪶�ʱ���ж�
	// M4_TMR02->CNTBR = 0x00000000ul;
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
    
    return TRUE;
}
//-----------------------------------------------
//��������:����SCI��һ���ֽ�
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:
//-----------------------------------------------
static void SimulateSciTxNextByte(BYTE SerialNo)
{
    // BYTE IntStatus;
    
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);
	
    // SimulateSci.DataBitCnt = 0;
    // SimulateSci.DataRegister = 0;
	// //������ɺ��ʼ������
	// if( Serial[ePT_BLUETOOTH].TXPoint >= Serial[ePT_BLUETOOTH].SendLength )
	// {
    //     //��ת����״̬
    //     DealSciFlag(ePT_BLUETOOTH);
    //     SerialMap[ePT_BLUETOOTH].SCIEnableRcv(SerialMap[ePT_BLUETOOTH].SCI_Ph_Num);
    //     IntStatus = api_splx(0);
	//     api_InitSubProtocol(ePT_BLUETOOTH);
    //     api_splx(IntStatus);
	// 	return;
	// }
	
    // //׼��һ���ַ�
    // SimulateSci.DataRegister = BuildSendData(Serial[ePT_BLUETOOTH].ProBuf[Serial[ePT_BLUETOOTH].TXPoint++]);
    
	// //�򿪶�ʱ���ж�
	// M4_TMR02->CNTBR = 0x00000000ul;
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,ENABLE);
}
//-----------------------------------------------
//��������:����SCI��һ���ֽ�
//
//����:	SciPhNum[in]:���ں�
//		
//����ֵ:�ɹ�/ʧ��
//
//��ע:��������ط�������һ���յ�������
//-----------------------------------------------
static void SimulateSciRxNextByte(BYTE SerialNo)
{	
	// //�رն�ʱ���ж�
	// TIMER0_Cmd(M4_TMR02,Tim0_ChannelB,DISABLE);
	
	// //��RX�ж�
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
//��������:����/�����жϺ���
//
//����:	
//		
//����ֵ:
//
//��ע:Ƕ���ڶ�ʱ���ж���ʹ��
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
