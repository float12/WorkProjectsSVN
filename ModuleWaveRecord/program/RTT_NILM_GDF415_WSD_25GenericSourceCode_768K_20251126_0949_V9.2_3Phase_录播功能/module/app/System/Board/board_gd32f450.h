//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		支持ST芯片的电能质量模块印制板驱动文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __BOARD_STH7_PQM_H
#define __BOARD_STH7_PQM_H

#if( BOARD_TYPE == BOARD_ST_PQM )

// 定义CPU类型
#define CPU_TYPE                CPU_GD32F415_768K

// 外部flash类型
#define THIRD_MEM_CHIP          CHIP_GD25Q64C

//外挂EEPROM型号
#define MASTER_MEMORY_CHIP		CHIP_M24512_RMN6TP
//第二片存储芯片
#define SLAVE_MEM_CHIP				CHIP_NO
// 串口总数
#define MAX_COM_CHANNEL_NUM     1


////////////////////////////////////////////////////////////////
//                         LED管脚定义
////////////////////////////////////////////////////////////////
#define LED_GPIO            GPIOB
#define LED_RED1_PIN        GPIO_PIN_15
#define LED_GREEN1_PIN      GPIO_PIN_14
#define LED_RED2_PIN        GPIO_PIN_13
#define LED_GREEN2_PIN      GPIO_PIN_12

#define LED_RED             LED_GREEN1_PIN// 2022.02.12 把原来的红色灯改为绿色 LED_RED1_PIN
#define LED_GREEN           LED_GREEN2_PIN
#define LED_RED_OPEN        gpio_bit_reset(LED_GPIO, LED_RED);//HAL_GPIO_WritePin(LED_GPIO, LED_RED, GPIO_PIN_RESET);
#define LED_RED_CLOSE       gpio_bit_set(LED_GPIO, LED_RED);//HAL_GPIO_WritePin(LED_GPIO, LED_RED, GPIO_PIN_SET);
#define LED_RED_STATUS()    gpio_output_bit_get(LED_GPIO, LED_RED)//HAL_GPIO_ReadPin(LED_GPIO, LED_RED)
#define LED_GREEN_OPEN      gpio_bit_reset(LED_GPIO, LED_GREEN);//HAL_GPIO_WritePin(LED_GPIO, LED_GREEN, GPIO_PIN_RESET);
#define LED_GREEN_CLOSE     gpio_bit_set(LED_GPIO, LED_GREEN);//HAL_GPIO_WritePin(LED_GPIO, LED_GREEN, GPIO_PIN_SET);
#define LED_GREEN_STATUS()  gpio_output_bit_get(LED_GPIO, LED_GREEN)//HAL_GPIO_ReadPin(LED_GPIO, LED_GREEN)


#define LED_TX		//HAL_GPIO_WritePin(LED_GPIO, LED_RED2_PIN, GPIO_PIN_RESET);
#define LED_RX		//HAL_GPIO_WritePin(LED_GPIO, LED_GREEN2_PIN, GPIO_PIN_RESET);
////////////////////////////////////////////////////////////////
#define IICPort        			 GPIOB
#define IIC_SCL_Pin    			 GPIO_PIN_8
#define IIC_SDA_Pin    			 GPIO_PIN_9
#define IIC_SCL_H      			 gpio_bit_set(IICPort,IIC_SCL_Pin);//HAL_GPIO_WritePin(IICPort,IIC_SCL_Pin,GPIO_PIN_SET)
#define IIC_SCL_L      			 gpio_bit_reset(IICPort,IIC_SCL_Pin);//HAL_GPIO_WritePin(IICPort,IIC_SCL_Pin,GPIO_PIN_RESET)
#define IIC_SDA_H      			 gpio_bit_set(IICPort,IIC_SDA_Pin);//HAL_GPIO_WritePin(IICPort,IIC_SDA_Pin,GPIO_PIN_SET)
#define IIC_SDA_L      			 gpio_bit_reset(IICPort,IIC_SDA_Pin);//HAL_GPIO_WritePin(IICPort,IIC_SDA_Pin,GPIO_PIN_RESET)
#define IIC_ACK() 			 gpio_input_bit_get(IICPort, IIC_SDA_Pin)//HAL_GPIO_ReadPin(IICPort, IIC_SDA_Pin)
//SDA配置为输入方式
#define SDA_READ_Direction       gpio_mode_set(IICPort, GPIO_MODE_INPUT, GPIO_PUPD_NONE, IIC_SDA_Pin);//IICPort->MODER &= ~(3 << (2 * 9)); \
								 IICPort->MODER |= (0 << (2 * 9));  \
								 IICPort->PUPDR &= ~(3 << (2 * 9));\
								 IICPort->PUPDR |= (0 << (2 * 9))
//SDA配置为输出方式
#define SDA_WRITE_Direction     gpio_mode_set(IICPort, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, IIC_SDA_Pin);// IICPort->MODER &= ~(3 << (2 * 9)); \
								 IICPort->MODER |= (1 << (2 * 9));  \
								 IICPort->PUPDR &= ~(3 << (2 * 9)); \
								 IICPort->PUPDR |= (1 << (2 * 9))



// 串口驱动结构定义
typedef struct
{
	BYTE SerialType;//串口类型 0: 485_I 1:IR 2:CR 3:485_II
	BYTE SCI_Ph_Num;								// 物理串口号
	BYTE (*SCIInit)( BYTE SCI_Ph_Num );				// 初始化串口
	BYTE (*SCIEnableRcv)( BYTE SCI_Ph_Num );		// 允许接收  
	BYTE (*SCIDisableRcv)( BYTE SCI_Ph_Num );		// 禁止接收接收
	BYTE (*SCIEnableSend)( BYTE SCI_Ph_Num );		// 允许发送
	BYTE (*SCIBeginSend)( BYTE SCI_Ph_Num );		// 开始发送
}TypeDef_Pulic_SCI;

typedef struct
{
    DWORD RxCount;
    DWORD TxBegCount;
    DWORD TxCptCount;
    DWORD TxDMAErrorCount;
    DWORD ErrorCallbackCount;
} USARTCOUNT;

#define FLASH_CS_ENABLE     gpio_bit_reset(GPIOB,GPIO_PIN_6);//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
#define FLASH_CS_DISABLE    gpio_bit_set(GPIOB,GPIO_PIN_6);//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);


extern const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM];
extern volatile USARTCOUNT USART_Count;

void InitBoard(void);
void DoSPICS(WORD No, WORD Do);

extern void InitLED(void);
extern void api_DeInitTIM2(void);

#endif//#if( (BOARD_TYPE == BOARD_HT_THREE_0134566) )

#endif//#ifndef __BOARD_HT_698_THREE_H


