/*******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 */
/******************************************************************************/
/** \file main.c
 **
 ** \brief This example demonstrates how to use SDIOC to read/write SDCard by
 **        high-speed(50MHz) && 4 bits && multiple blocks mode.
 **
 **   - 2021-04-16 CDT First version for Device Driver Library of SDIOC
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "sdcard.h"
#include "AllHead.h"
#include "SDIO_MSD_Driver.h"



/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/



/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
int MSD_Init(void)
{
    sd_error_enum status = sd_init();
    if (SD_OK != status)
    {
        return -1;
    }
    return 0;
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
