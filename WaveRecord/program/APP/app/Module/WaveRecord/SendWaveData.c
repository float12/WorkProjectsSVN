//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "hc32_ddl.h"
#include "SendWaveData.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define UART_DMA_UNIT						(M4_DMA2)
#define UART_DMA_CH							(DmaCh0)
#define UART_DMA_TRG_SEL					(EVT_USART2_TI)//EVT_USART4_TCI

/* USART channel definition */
#define USART_CH							(M4_USART2)

/* DMA block transfer complete interrupt */
#define UART_DMA_TC_INT_NUM                 (INT_DMA2_TC0)  
#define UART_DMA_TC_INT_IRQn                (Int018_IRQn)   //(UART2_DMA2_IrqNo)
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
//----------------------------------------------
//-----------------------------------------------
//��������:��������DMA��������
//
//����: Buf ������    len ����
//                     
//����ֵ:
//
//��ע:   
//-----------------------------------------------
void UartSendDmaConfig(BYTE* Buf,WORD len)
{
	DMA_SetSrcAddress(UART_DMA_UNIT, UART_DMA_CH, (uint32_t)(&Buf[0]));
	DMA_SetTransferCnt(UART_DMA_UNIT, UART_DMA_CH, len);
	/* Enable DMA channel */
	DMA_ChannelCmd(UART_DMA_UNIT, UART_DMA_CH, Enable);
}
/**
 *******************************************************************************
 ** \brief DMA block transfer complete irq callback function.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void DmaTcIrqCallback(void)
{
	while(Reset == USART_GetStatus(USART_CH, UsartTxComplete))
	{
	}//���dma���������һ�ֽ�����
	USART_FuncCmd(USART_CH, UsartTx, Disable);
	DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, TrnCpltIrq);
	DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, BlkTrnCpltIrq);
}
/**
 *******************************************************************************
 ** \brief DMA��������жϻص�����
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void DmaErrIrqCallback(void)
{
	// USART_FuncCmd(USART_CH, UsartTx, Disable);
	// DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, TrnCpltIrq);
	// DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, BlkTrnCpltIrq);
	api_Delay100us(1);
}

//-----------------------------------------------
//��������: ���� dma��������жϳ�ʼ��
//
//����:		
//						
//����ֵ:	
//		   
//��ע:
//-----------------------------------------------
void DmaIrqInit(void)
{
	stc_irq_regi_conf_t stcIrqRegiCfg;
	/* Set DMA block transfer complete IRQ */
	stcIrqRegiCfg.enIRQn = UART_DMA_TC_INT_IRQn;
	stcIrqRegiCfg.pfnCallback = &DmaTcIrqCallback;
	stcIrqRegiCfg.enIntSrc = UART_DMA_TC_INT_NUM;
	enIrqRegistration(&stcIrqRegiCfg);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_00);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

	/* Set DMA block transfer complete IRQ */
	stcIrqRegiCfg.enIRQn = Int019_IRQn;
	stcIrqRegiCfg.pfnCallback = &DmaErrIrqCallback;
	stcIrqRegiCfg.enIntSrc = INT_DMA2_ERR;
	enIrqRegistration(&stcIrqRegiCfg);
	NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_00);
	NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
	NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
}
/**
 *******************************************************************************
 ** \brief ���Ͳ������ݴ���dma��ʼ��
 **
 ** \param [in] Buf ����; len ����
 **
 ** \retval None
 **
 ******************************************************************************/
void UartSendDmaInit(void)
{
	stc_dma_config_t stcDmaInit;
	stc_irq_regi_conf_t stcIrqRegiCfg;

	/* Enable peripheral clock */
	PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1 | PWC_FCG0_PERIPH_DMA2,Enable);

	/* Enable DMA. */
	DMA_Cmd(UART_DMA_UNIT,Enable);

	/* Initialize DMA. */
	MEM_ZERO_STRUCT(stcDmaInit);
	stcDmaInit.u16BlockSize = 1u; /* 1 */
	// stcDmaInit.u16TransferCnt = len;
	// stcDmaInit.u32SrcAddr = (uint32_t)(&Buf[0]); /* Set source address. */
	stcDmaInit.u32DesAddr = (uint32_t)(&USART_CH->DR);     /* Set destination address. */
	stcDmaInit.stcDmaChCfg.enSrcInc = AddressIncrease;  /* Set source address mode. */
	stcDmaInit.stcDmaChCfg.enDesInc = AddressFix;  /* Set destination address mode. */
	stcDmaInit.stcDmaChCfg.enIntEn = Enable;       /* Enable interrupt. */
	stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;   /* Set data width 8bit. */
	DMA_InitChannel(UART_DMA_UNIT, UART_DMA_CH, &stcDmaInit);

	/* Enable the specified DMA channel. */
	DMA_ChannelCmd(UART_DMA_UNIT, UART_DMA_CH, Enable);

	/* Clear DMA flag. */
	DMA_ClearIrqFlag(UART_DMA_UNIT, UART_DMA_CH, TrnCpltIrq);

	/* Enable peripheral circuit trigger function. */
	PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS,Enable);

	/* Set DMA trigger source. */
	DMA_SetTriggerSrc(UART_DMA_UNIT, UART_DMA_CH, UART_DMA_TRG_SEL);

	DmaIrqInit();
}
//-----------------------------------------------
//��������: ���� dma���Ͳ�������
//
//����:		Buf ����; len ����
//						
//����ֵ:	
//		   
//��ע:
//-----------------------------------------------
void UartSendWaveDataByDma(BYTE* Buf,WORD len)
{
	// toggleTestPin();
	UartSendDmaConfig(Buf,len);
	USART_CH->DR = 0;
	USART_FuncCmd(USART_CH, UsartTx, Enable); // ����ʹ��
        
}

