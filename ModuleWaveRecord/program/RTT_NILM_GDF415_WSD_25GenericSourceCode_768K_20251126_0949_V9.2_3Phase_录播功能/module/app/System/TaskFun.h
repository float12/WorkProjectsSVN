/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASKFUN_H
#define __TASKFUN_H


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if(!WIN32)
#include "gd32f4xx.h"
#endif



#ifdef __cplusplus
}
#endif

extern int xTaskCreate(void (*pvTaskCode)(void *parameter), 
				const char * const pcName,
				int usStackDepth,
				void *pvParameters,
				int uxPriority,
				int *pxCreatedTask
				);
				
#endif /* __FUN_H */

#if(PROTOCOL_VERSION == 25)
//--------------------------------------------------
//功能描述:  发送一个消息队列的消息
//         
//参数:      Flag：1代表发送紧急消息，0代表发送普通消息
//           Buf：消息内容  Mq：消息对象  Len：消息长度
//         
//返回值:    RT-EOK：发送成功  RT_EINVAL：参数非法
//         
//备注:  
//--------------------------------------------------
rt_err_t  api_QueueSend(rt_mq_t Mq, BYTE *Buf, rt_size_t Len, BYTE Flag );

//--------------------------------------------------
//功能描述:  接收一个消息队列的消息
//         
//参数:      Buf：消息内容  Mq：消息对象
//         
//返回值:    RT-EOK：发送成功  其他：失败
//         
//备注:  
//--------------------------------------------------
rt_err_t  api_QueueRecv(rt_mq_t Mq, BYTE *Buf, rt_size_t size);
#endif //#if(PROTOCOL_VERSION == 25)

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
