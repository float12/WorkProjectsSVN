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
#include "sd_card.h"
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
stc_sd_handle_t stcSdhandle;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
 static en_result_t SdiocInitPins(void);

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief Initialize SDIO pins
 **
 ** \param [in] None
 **
 ** \retval Ok  SDIO pins initialized successfully
 **
 ******************************************************************************/
 static en_result_t SdiocInitPins(void)
{
  
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);
    
    stcPortInit.enPinDrv = Pin_Drv_M;
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType =  Pin_OType_Cmos ;
    #if (HARDWARE_TYPE == THREE_PHASE_BOARD)
    PORT_Init(SDIOC_PWR_EN_PORT, SDIOC_PWR_EN_PIN, &stcPortInit);
    PORT_ResetPortData(SDIOC_PWR_EN_PORT, SDIOC_PWR_EN_PIN);
    #elif (HARDWARE_TYPE == SINGLE_PHASE_BOARD)
    // PORT_Init(SDIOC_PWR_EN_PORT, SDIOC_PWR_EN_PIN, &stcPortInit);
    // PORT_SetPortData(SDIOC_PWR_EN_PORT, SDIOC_PWR_EN_PIN);
    #endif

    PORT_SetFunc(SDIOC_D0_PORT, SDIOC_D0_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D1_PORT, SDIOC_D1_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D2_PORT, SDIOC_D2_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D3_PORT, SDIOC_D3_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CD_PORT, SDIOC_CD_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CK_PORT, SDIOC_CK_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CMD_PORT, SDIOC_CMD_PIN, Func_Sdio, Disable);
    return Ok;
}
int MSD_Init(void)
{
    en_result_t enResult = Ok;
    stc_sdcard_init_t stcCardInitCfg =
    {
        SdiocBusWidth4Bit,
        SdiocClk50M,
        SdiocHighSpeedMode,
        NULL,
    };
    SdiocInitPins();
    stcSdhandle.SDIOCx = SDIOC_UNIT;
    if (Ok != SDCARD_Init(&stcSdhandle, &stcCardInitCfg))
    {
        enResult = Error;
    }
    return (int)enResult;
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
