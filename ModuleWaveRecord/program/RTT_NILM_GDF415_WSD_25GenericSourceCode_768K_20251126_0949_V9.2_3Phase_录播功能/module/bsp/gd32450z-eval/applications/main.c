/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-18     tanek        first implementation
 */
#include "AllHead.h"
#include <stdio.h>
#include <rtthread.h>
//-----------------------------------------------
//				函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:main函数
//         
//参数:      无
//         
//返回值:    无
//         
//备注内容:   9.2版本
//--------------------------------------------------
int main(void)
{  
	api_InitRoot();
	
	//模组核心功能任务
	api_CreateRootTask();

	//算法核心任务
	api_CreateNILMTask();

	return 0;
}
