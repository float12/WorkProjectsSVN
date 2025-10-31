/**
 *******************************************************************************
 * @file  hc32_ll_gpio.h
 * @brief This file contains all the functions prototypes of the GPIO driver
 *        library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2022-06-30       CDT             Add API GPIO_AnalogCmd() and GPIO_ExIntCmd()
   2023-06-30       CDT             Rename GPIO_ExIntCmd() as GPIO_ExtIntCmd
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2023, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#ifndef __HC32_LL_GPIO_H__
#define __HC32_LL_GPIO_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/


#include "hc32_ll_def.h"
#include "hc32f4xx_conf.h"

/**
 * @addtogroup LL_Driver
 * @{
 */

/**
 * @addtogroup LL_GPIO
 * @{
 */

#if (LL_GPIO_ENABLE == DDL_ON)

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/**
 * @defgroup GPIO_Global_Types GPIO Global Types
 * @{
 */

/**
 * @brief  GPIO Pin Set and Reset enumeration
 */
typedef enum {
    PIN_RESET = 0U,           /*!< Pin reset    */
    PIN_SET   = 1U            /*!< Pin set      */
} en_pin_state_t;

/**
 * @brief  GPIO Init structure definition
 */
typedef struct {
    uint16_t u16PinState;       /*!< Set pin state to High or Low, @ref GPIO_PinState_Sel for details       */
    uint16_t u16PinDir;         /*!< Pin mode setting, @ref GPIO_PinDirection_Sel for details               */
    uint16_t u16PinOutputType;  /*!< Output type setting, @ref GPIO_PinOutType_Sel for details              */
    uint16_t u16PinDrv;         /*!< Pin drive capacity setting, @ref GPIO_PinDrv_Sel for details           */
    uint16_t u16Latch;          /*!< Pin latch setting, @ref GPIO_PinLatch_Sel for details                  */
    uint16_t u16PullUp;         /*!< Internal pull-up resistor setting, @ref GPIO_PinPU_Sel for details     */
    uint16_t u16Invert;         /*!< Pin input/output invert setting, @ref GPIO_PinInvert_Sel for details   */
    uint16_t u16ExtInt;         /*!< External interrupt pin setting, @ref GPIO_PinExtInt_Sel for details    */
    uint16_t u16PinInputType;   /*!< Input type setting, @ref GPIO_PinInType_Sel for detials                */
    uint16_t u16PinAttr;        /*!< Digital or analog attribute setting, @ref GPIO_PinMode_Sel for details */
} stc_gpio_init_t;
/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup GPIO_Global_Macros GPIO Global Macros
 * @{
 */

/**
 * @defgroup GPIO_Pins_Define GPIO Pin Source
 * @{
 */
#define GPIO_PIN_00                 (0x0001U)  /*!< Pin 00 selected   */
#define GPIO_PIN_01                 (0x0002U)  /*!< Pin 01 selected   */
#define GPIO_PIN_02                 (0x0004U)  /*!< Pin 02 selected   */
#define GPIO_PIN_03                 (0x0008U)  /*!< Pin 03 selected   */
#define GPIO_PIN_04                 (0x0010U)  /*!< Pin 04 selected   */
#define GPIO_PIN_05                 (0x0020U)  /*!< Pin 05 selected   */
#define GPIO_PIN_06                 (0x0040U)  /*!< Pin 06 selected   */
#define GPIO_PIN_07                 (0x0080U)  /*!< Pin 07 selected   */
#define GPIO_PIN_08                 (0x0100U)  /*!< Pin 08 selected   */
#define GPIO_PIN_09                 (0x0200U)  /*!< Pin 09 selected   */
#define GPIO_PIN_10                 (0x0400U)  /*!< Pin 10 selected   */
#define GPIO_PIN_11                 (0x0800U)  /*!< Pin 11 selected   */
#define GPIO_PIN_12                 (0x1000U)  /*!< Pin 12 selected   */
#define GPIO_PIN_13                 (0x2000U)  /*!< Pin 13 selected   */
#define GPIO_PIN_14                 (0x4000U)  /*!< Pin 14 selected   */
#define GPIO_PIN_15                 (0x8000U)  /*!< Pin 15 selected   */
#define GPIO_PIN_ALL                (0xFFFFU)  /*!< All pins selected */
/**
 * @}
 */

/**
 * @defgroup GPIO_All_Pins_Define GPIO All Pin Definition for Each Product
 * @{
 */
#define GPIO_PIN_A_ALL              (0xFFFFU)   /*!< Pin A all*/
#define GPIO_PIN_B_ALL              (0xFFFFU)   /*!< Pin B all*/
#define GPIO_PIN_C_ALL              (0xFFFFU)   /*!< Pin C all*/
#define GPIO_PIN_D_ALL              (0xFFFFU)   /*!< Pin D all*/
#define GPIO_PIN_E_ALL              (0xFFFFU)   /*!< Pin E all*/
#define GPIO_PIN_F_ALL              (0xFFFFU)   /*!< Pin F all*/
#define GPIO_PIN_G_ALL              (0xFFFFU)   /*!< Pin G all*/
#define GPIO_PIN_H_ALL              (0xFFFFU)   /*!< Pin H all*/
#define GPIO_PIN_I_ALL              (0x3FFFU)   /*!< Pin I all*/
/**
 * @}
 */

/**
 * @defgroup GPIO_Port_Source GPIO Port Source
 * @{
 */
#define GPIO_PORT_A                 (0x00U)     /*!< Port A selected  */
#define GPIO_PORT_B                 (0x01U)     /*!< Port B selected  */
#define GPIO_PORT_C                 (0x02U)     /*!< Port C selected  */
#define GPIO_PORT_D                 (0x03U)     /*!< Port D selected  */
#define GPIO_PORT_E                 (0x04U)     /*!< Port E selected  */
#define GPIO_PORT_F                 (0x05U)     /*!< Port F selected  */
#define GPIO_PORT_G                 (0x06U)     /*!< Port G selected  */
#define GPIO_PORT_H                 (0x07U)     /*!< Port H selected  */
#define GPIO_PORT_I                 (0x08U)     /*!< Port I selected  */
/**
 * @}
 */

/**
 * @defgroup GPIO_Function_Sel GPIO Function Selection
 * @{
 */
#define GPIO_FUNC_0               (0U)
#define GPIO_FUNC_1               (1U)
#define GPIO_FUNC_2               (2U)
#define GPIO_FUNC_3               (3U)
#define GPIO_FUNC_4               (4U)
#define GPIO_FUNC_5               (5U)
#define GPIO_FUNC_6               (6U)
#define GPIO_FUNC_7               (7U)
#define GPIO_FUNC_8               (8U)
#define GPIO_FUNC_9               (9U)
#define GPIO_FUNC_10              (10U)
#define GPIO_FUNC_11              (11U)
#define GPIO_FUNC_12              (12U)
#define GPIO_FUNC_13              (13U)
#define GPIO_FUNC_14              (14U)
#define GPIO_FUNC_15              (15U)
#define GPIO_FUNC_16              (16U)
#define GPIO_FUNC_17              (17U)
#define GPIO_FUNC_18              (18U)
#define GPIO_FUNC_19              (19U)
#define GPIO_FUNC_20              (20U)
#define GPIO_FUNC_32              (32U)
#define GPIO_FUNC_33              (33U)
#define GPIO_FUNC_34              (34U)
#define GPIO_FUNC_35              (35U)
#define GPIO_FUNC_36              (36U)
#define GPIO_FUNC_37              (37U)
#define GPIO_FUNC_38              (38U)
#define GPIO_FUNC_39              (39U)
#define GPIO_FUNC_40              (40U)
#define GPIO_FUNC_41              (41U)
#define GPIO_FUNC_42              (42U)
#define GPIO_FUNC_43              (43U)
#define GPIO_FUNC_44              (44U)
#define GPIO_FUNC_45              (45U)
#define GPIO_FUNC_46              (46U)
#define GPIO_FUNC_47              (47U)
#define GPIO_FUNC_48              (48U)
#define GPIO_FUNC_49              (49U)
#define GPIO_FUNC_50              (50U)
#define GPIO_FUNC_51              (51U)
#define GPIO_FUNC_52              (52U)
#define GPIO_FUNC_53              (53U)
#define GPIO_FUNC_54              (54U)
#define GPIO_FUNC_55              (55U)
#define GPIO_FUNC_56              (56U)
#define GPIO_FUNC_57              (57U)
#define GPIO_FUNC_58              (58U)
#define GPIO_FUNC_59              (59U)
#define GPIO_FUNC_60              (60U)
#define GPIO_FUNC_61              (61U)
#define GPIO_FUNC_62              (62U)
#define GPIO_FUNC_63              (63U)
/**
 * @}
 */
/**
 *******************************************************************************
 ** \brief GPIO Port Index enumeration
 ******************************************************************************/
typedef enum en_port
{
    PortA = 0,                          ///< port group A
    PortB = 1,                          ///< port group B
    PortC = 2,                          ///< port group C
    PortD = 3,                          ///< port group D
    PortE = 4,                          ///< port group E
    PortF = 5,                          ///< port group F
    PortG = 6,                          ///< port group F
    PortH = 7,                          ///< port group H
    PortI = 8,                          ///< port group F
}en_port_t;

/**
 *******************************************************************************
 ** \brief GPIO Pin Index enumeration
 ******************************************************************************/
typedef enum en_pin
{
    Pin00 = (1 <<  0),                  ///< Pin index 00 of each port group
    Pin01 = (1 <<  1),                  ///< Pin index 01 of each port group
    Pin02 = (1 <<  2),                  ///< Pin index 02 of each port group
    Pin03 = (1 <<  3),                  ///< Pin index 03 of each port group
    Pin04 = (1 <<  4),                  ///< Pin index 04 of each port group
    Pin05 = (1 <<  5),                  ///< Pin index 05 of each port group
    Pin06 = (1 <<  6),                  ///< Pin index 06 of each port group
    Pin07 = (1 <<  7),                  ///< Pin index 07 of each port group
    Pin08 = (1 <<  8),                  ///< Pin index 08 of each port group
    Pin09 = (1 <<  9),                  ///< Pin index 09 of each port group
    Pin10 = (1 << 10),                  ///< Pin index 10 of each port group
    Pin11 = (1 << 11),                  ///< Pin index 11 of each port group
    Pin12 = (1 << 12),                  ///< Pin index 12 of each port group
    Pin13 = (1 << 13),                  ///< Pin index 13 of each port group
    Pin14 = (1 << 14),                  ///< Pin index 14 of each port group
    Pin15 = (1 << 15),                  ///< Pin index 15 of each port group
    PinAll= 0xFFFF,                     ///< All pins selected
}en_pin_t;

/**
 *******************************************************************************
 ** \brief GPIO Function enumeration
 ******************************************************************************/
typedef enum en_port_func
{
    Func0_Gpio               = 0u,               ///< function set to gpio
    Func1_Fcmref             = 1u,               ///< function set to fcm reference
    Func1_Ctcref             = 1u,               ///< function set to ctc reference
    Func1_Rtcout             = 1u,               ///< function set to rtc output
    Func1_Mco                = 1u,               ///< function set to MCO
    Func1_Vcout              = 1u,               ///< function set to vc output
    Func1_Adtrg              = 1u,               ///< function set to adc trigger
    Func1_CAN_TST_SAMPLE     = 1u,               ///< function set to CAN sample
    Func1_CAN_TST_CLOCK      = 1u,               ///< function set to CAN clock
    Func1_Mclkout            = 1u,               ///< function set to mclk output//////////
    Func2_Tim4               = 2u,               ///< function set to timer4
    Func3_Tim6               = 3u,               ///< function set to timer6
    Func4_Tima               = 4u,               ///< function set to timerA
    Func5_Tima               = 5u,               ///< function set to timerA
    Func6_Tima               = 6u,               ///< function set to timerA
    Func6_Tim6               = 6u,               ///< function set to timer6
    Func6_Emb                = 6u,               ///< function set to emb
    Func7_Usart1_Ck          = 7u,               ///< function set to usart ck of ch.1
    Func7_Usart1_Cts         = 7u,               ///< function set to usart cts of ch.1
    Func7_Usart1_Rts         = 7u,               ///< function set to usart rts of ch.1
    Func7_Usart2_Ck          = 7u,               ///< function set to usart ck of ch.2
    Func7_Usart2_Cts         = 7u,               ///< function set to usart cts of ch.2
    Func7_Usart2_Rts         = 7u,               ///< function set to usart rts of ch.2
    Func7_Usart3_Ck          = 7u,               ///< function set to usart ck of ch.3
    Func7_Usart3_Cts         = 7u,               ///< function set to usart cts of ch.3
    Func7_Usart3_Rts         = 7u,               ///< function set to usart rts of ch.3
    Func7_Usart3_Tx          = 7u,               ///< function set to usart tx of ch.3
    Func7_Usart4_Ck          = 7u,               ///< function set to usart ck of ch.4
    Func7_Usart4_Cts         = 7u,               ///< function set to usart cts of ch.4
    Func7_Usart4_Rts         = 7u,               ///< function set to usart rts of ch.4
    Func7_Usart5_Ck          = 7u,               ///< function set to usart ck of ch.5
    Func7_Usart5_Cts         = 7u,               ///< function set to usart cts of ch.5
    Func7_Usart5_Rts         = 7u,               ///< function set to usart rts of ch.5
    Func7_Usart6_Ck          = 7u,               ///< function set to usart ck of ch.6
    Func7_Usart6_Cts         = 7u,               ///< function set to usart cts of ch.6
    Func7_Usart6_Rts         = 7u,               ///< function set to usart rts of ch.6
    Func7_Usart7_Ck          = 7u,               ///< function set to usart ck of ch.7
    Func7_Usart7_Cts         = 7u,               ///< function set to usart cts of ch.7
    Func7_Usart7_Rts         = 7u,               ///< function set to usart rts of ch.7
    Func7_Usart8_Ck          = 7u,               ///< function set to usart ck of ch.8
    Func7_Usart8_Cts         = 7u,               ///< function set to usart cts of ch.8
    Func7_Usart8_Rts         = 7u,               ///< function set to usart rts of ch.8
    Func7_Usart9_Ck          = 7u,               ///< function set to usart ck of ch.9
    Func7_Usart9_Cts         = 7u,               ///< function set to usart cts of ch.9
    Func7_Usart9_Rts         = 7u,               ///< function set to usart rts of ch.9
    Func7_Usart10_Ck         = 7u,               ///< function set to usart ck of ch.10
    Func7_Usart10_Cts        = 7u,               ///< function set to usart cts of ch.10
    Func7_Usart10_Rts        = 7u,               ///< function set to usart rts of ch.10
    Func8_Key                = 8u,               ///< function set to key
    Func9_Sdio               = 9u,               ///< function set to sdio
    Func10_Usbfs             = 10u,              ///< function set to usbfs
    Func10_Usbhs             = 10u,              ///< function set to usbhs
    Func10_Tim2              = 10u,              ///< function set to timer2
    Func11_Eth               = 11u,              ///< function set to ethmac
    Func12_Exmc              = 12u,              ///< function set to exmc
    Func12_Usbhs             = 12u,              ///< function set to usbhs
    Func13_Dvp               = 13u,              ///< function set to dvp
    Func14_Evnpt             = 14u,              ///< function set to event port
    Func15_Eventout          = 15u,              ///< function set to event out
    Func16_Tim2              = 16u,              ///< function set to timer2
    Func16_Tim4              = 16u,              ///< function set to timer4
    Func17_I2s               = 17u,              ///< function set to i2s
    Func18_Spi1_Nss1         = 18u,              ///< function set to spi nss1 of ch.1
    Func18_spi1_Nss2         = 18u,              ///< function set to spi nss2 of ch.1
    Func18_spi1_Nss3         = 18u,              ///< function set to spi nss3 of ch.1
    Func18_Spi2_Nss0_1       = 18u,              ///< function set to spi nss0 of ch.2
    Func18_Spi2_Nss1         = 18u,              ///< function set to spi nss1 of ch.2
    Func18_Spi2_Nss2         = 18u,              ///< function set to spi nss2 of ch.2
    Func18_Spi2_Nss3         = 18u,              ///< function set to spi nss3 of ch.2
    Func18_Spi3_Nss1         = 18u,              ///< function set to spi nss1 of ch.3
    Func18_spi3_Nss2         = 18u,              ///< function set to spi nss2 of ch.3
    Func18_spi3_Nss3         = 18u,              ///< function set to spi nss3 of ch.3
    Func18_Spi4_Nss1         = 18u,              ///< function set to spi nss1 of ch.4
    Func18_spi4_Nss2         = 18u,              ///< function set to spi nss2 of ch.4
    Func18_spi4_Nss3         = 18u,              ///< function set to spi nss3 of ch.4
    Func18_spi5_Nss0         = 18u,              ///< function set to spi nss0 of ch.5
    Func18_Spi5_Nss1         = 18u,              ///< function set to spi nss1 of ch.5
    Func18_spi5_Nss2         = 18u,              ///< function set to spi nss2 of ch.5
    Func18_spi5_Nss3         = 18u,              ///< function set to spi nss3 of ch.5
    Func18_Spi6_Nss1         = 18u,              ///< function set to spi nss1 of ch.6
    Func18_spi6_Nss2         = 18u,              ///< function set to spi nss2 of ch.6
    Func18_spi6_Nss3         = 18u,              ///< function set to spi nss3 of ch.6
    Func18_Qspi              = 18u,              ///< function set to qspi
    Func19_Spi1_Nss0         = 19u,              ///< function set to spi nss0 of ch.1
    Func19_Spi2_Nss0_2       = 19u,              ///< function set to spi nss0 of ch.2
    Func19_Spi2_Clk          = 19u,              ///< function set to spi sck of ch.2
    Func19_Spi2_Miso         = 19u,              ///< function set to spi miso of ch.2
    Func19_Spi2_Mosi         = 19u,              ///< function set to spi mosi of ch.2
    Func19_Spi3_Clk          = 19u,              ///< function set to spi sck of ch.3
    Func19_Spi3_Miso         = 19u,              ///< function set to spi miso of ch.3
    Func19_Spi3_Mosi         = 19u,              ///< function set to spi mosi of ch.3
    Func19_Spi4_Nss0         = 19u,              ///< function set to spi nss0 of ch.4
    Func19_Spi5_Nss0         = 19u,              ///< function set to spi nss0 of ch.5
    Func19_Spi5_Clk          = 19u,              ///< function set to spi sck of ch.5
    Func19_Spi5_Miso         = 19u,              ///< function set to spi miso of ch.5
    Func19_Spi5_Mosi         = 19u,              ///< function set to spi mosi of ch.5
    Func19_Spi6_Nss0         = 19u,              ///< function set to spi nss0 of ch.6
    Func20_Usart1_Ck         = 20u,              ///< function set to usart ck of ch.1
    Func20_Usart1_Cts        = 20u,              ///< function set to usart cts of ch.1
    Func20_Usart1_Rts        = 20u,              ///< function set to usart rts of ch.1
    Func20_Usart1_Tx         = 20u,              ///< function set to usart tx of ch.1
    Func20_Usart1_Rx         = 20u,              ///< function set to usart rx of ch.1
    Func20_Usart2_Ck         = 20u,              ///< function set to usart ck of ch.2
    Func20_Usart2_Cts        = 20u,              ///< function set to usart cts of ch.2
    Func20_Usart2_Rts        = 20u,              ///< function set to usart rts of ch.2
    Func20_Usart2_Tx         = 20u,              ///< function set to usart tx of ch.2
    Func20_Usart2_Rx         = 20u,              ///< function set to usart rx of ch.2
    Func20_Usart4_Tx         = 20u,              ///< function set to usart tx of ch.4
    Func20_Usart4_Rx         = 20u,              ///< function set to usart rx of ch.4
    Func20_Usart5_Tx         = 20u,              ///< function set to usart tx of ch.5
    Func20_Usart5_Rx         = 20u,              ///< function set to usart rx of ch.5
    Func20_Usart6_Ck         = 20u,              ///< function set to usart ck of ch.6
    Func20_Usart6_Cts        = 20u,              ///< function set to usart cts of ch.6
    Func20_Usart6_Rts        = 20u,              ///< function set to usart rts of ch.6
    Func20_Usart6_Tx         = 20u,              ///< function set to usart tx of ch.6
    Func20_Usart6_Rx         = 20u,              ///< function set to usart rx of ch.6
    Func20_Usart7_Tx         = 20u,              ///< function set to usart tx of ch.7
    Func20_Usart7_Rx         = 20u,              ///< function set to usart rx of ch.7
    Func20_Usart8_Tx         = 20u,              ///< function set to usart tx of ch.8
    Func20_Usart8_Rx         = 20u,              ///< function set to usart rx of ch.8
    Func32_Usart1_Tx         = 32u,              ///< function set to usart tx of ch.1
    Func32_Usart4_Tx         = 32u,              ///< function set to usart tx of ch.4
    Func32_Usart3_Tx         = 32u,              ///< function set to usart tx of ch.3
    Func33_Usart1_Rx         = 33u,              ///< function set to usart rx of ch.1
    Func33_Usart4_Rx         = 33u,              ///< function set to usart rx of ch.4
    Func33_Usart3_Rx         = 33u,              ///< function set to usart rx of ch.3
    Func34_Usart2_Tx         = 34u,              ///< function set to usart tx of ch.2
    Func34_Usart5_Tx         = 34u,              ///< function set to usart tx of ch.5
    Func34_Usart8_Tx         = 34u,              ///< function set to usart tx of ch.8
    Func35_Usart2_Rx         = 35u,              ///< function set to usart rx of ch.2
    Func35_Usart5_Rx         = 35u,              ///< function set to usart rx of ch.5
    Func35_Usart8_Rx         = 35u,              ///< function set to usart rx of ch.8
    Func36_Usart3_Tx         = 36u,              ///< function set to usart tx of ch.3
    Func36_Usart6_Tx         = 36u,              ///< function set to usart tx of ch.6
    Func36_Usart9_Tx         = 36u,              ///< function set to usart tx of ch.9
    Func37_Usart3_Rx         = 37u,              ///< function set to usart rx of ch.3
    Func37_Usart6_Rx         = 37u,              ///< function set to usart rx of ch.6
    Func37_Usart9_Rx         = 37u,              ///< function set to usart rx of ch.9
    Func38_Usart4_Tx         = 38u,              ///< function set to usart tx of ch.4
    Func38_Usart7_Tx         = 38u,              ///< function set to usart tx of ch.7
    Func38_Usart10_Tx        = 38u,              ///< function set to usart tx of ch.10
    Func39_Usart4_Rx         = 39u,              ///< function set to usart rx of ch.4
    Func39_Usart7_Rx         = 39u,              ///< function set to usart rx of ch.7
    Func39_Usart10_Rx        = 39u,              ///< function set to usart rx of ch.10
    Func40_Spi1_Sck          = 40u,              ///< function set to spi sck of ch.1
    Func40_Spi4_Sck          = 40u,              ///< function set to spi sck of ch.4
    Func41_Spi1_Mosi         = 41u,              ///< function set to spi mosi of ch.1
    Func41_Spi4_Mosi         = 41u,              ///< function set to spi mosi of ch.4
    Func42_Spi1_Miso         = 42u,              ///< function set to spi miso of ch.1
    Func42_Spi4_Miso         = 42u,              ///< function set to spi miso of ch.4
    Func43_Spi2_Sck          = 43u,              ///< function set to spi sck of ch.2
    Func43_Spi5_Sck          = 43u,              ///< function set to spi sck of ch.5
    Func43_Spi4_Sck          = 43u,              ///< function set to spi sck of ch.4
    Func44_Spi2_Mosi         = 44u,              ///< function set to spi mosi of ch.2
    Func44_Spi5_Mosi         = 44u,              ///< function set to spi mosi of ch.5
    Func44_Spi4_Mosi         = 44u,              ///< function set to spi mosi of ch.4
    Func45_Spi2_Miso         = 45u,              ///< function set to spi miso of ch.2
    Func45_Spi5_Mosi         = 45u,              ///< function set to spi miso of ch.5
    Func45_Spi4_Miso         = 45u,              ///< function set to spi miso of ch.4
    Func46_Spi3_Sck          = 46u,              ///< function set to spi sck of ch.3
    Func46_Spi6_Sck          = 46u,              ///< function set to spi sck of ch.6
    Func46_Spi4_Nss0         = 46u,              ///< function set to spi nss0 of ch.4
    Func47_Spi3_Mosi         = 47u,              ///< function set to spi mosi of ch.3
    Func47_Spi6_Mosi         = 47u,              ///< function set to spi mosi of ch.6
    Func47_Spi1_Nss0         = 47u,              ///< function set to spi nss0 of ch.1
    Func48_Spi3_Miso         = 48u,              ///< function set to spi miso of ch.3
    Func48_Spi6_Mosi         = 48u,              ///< function set to spi miso of ch.6
    Func48_I2c1_Sda          = 48u,              ///< function set to i2c sda of ch.1
    Func49_Spi3_Nss0         = 49u,              ///< function set to spi nss0 of ch.3
    Func49_Spi6_Nss0         = 49u,              ///< function set to spi nss0 of ch.6
    Func49_I2c1_Scl          = 49u,              ///< function set to i2c scl of ch.1
    Func50_I2c1_Sda          = 50u,              ///< function set to i2c sda of ch.1
    Func50_I2c2_Sda          = 50u,              ///< function set to i2c sda of ch.2
    Func51_I2c1_Scl          = 51u,              ///< function set to i2c scl of ch.1
    Func51_I2c2_Scl          = 51u,              ///< function set to i2c scl of ch.2
    Func52_I2c3_Sda          = 52u,              ///< function set to i2c sda of ch.3
    Func52_I2c4_Sda          = 52u,              ///< function set to i2c sda of ch.4
    Func52_I2c6_Sda          = 52u,              ///< function set to i2c sda of ch.6
    Func53_I2c3_Scl          = 53u,              ///< function set to i2c scl of ch.3
    Func53_I2c4_Scl          = 53u,              ///< function set to i2c scl of ch.4
    Func53_I2c6_Scl          = 53u,              ///< function set to i2c scl of ch.6
    Func54_I2s1_Ck           = 54u,              ///< function set to i2s ck of ch.1
    Func54_I2c5_Sda          = 54u,              ///< function set to i2c sda of ch.5
    Func54_I2s1_Wk           = 55u,              ///< function set to i2s wk of ch.1
    Func55_I2c5_Scl          = 55u,              ///< function set to i2c scl of ch.5
    Func56_I2s1_Sd           = 56u,              ///< function set to i2s sd of ch.1
    Func57_I2s2_Ck           = 57u,              ///< function set to i2s ck of ch.2
    Func57_I2s3_Ck           = 57u,              ///< function set to i2s ck of ch.3
    Func57_I2s4_Ck           = 57u,              ///< function set to i2s ck of ch.4
    Func58_I2s2_Wk           = 58u,              ///< function set to i2s wk of ch.2
    Func58_I2s3_Wk           = 58u,              ///< function set to i2s wk of ch.3
    Func58_I2s4_Wk           = 58u,              ///< function set to i2s wk of ch.4
    Func59_I2s2_Sd           = 59u,              ///< function set to i2s sd of ch.2
    Func59_I2s3_Sd           = 59u,              ///< function set to i2s sd of ch.3
    Func59_I2s4_Sd           = 59u,              ///< function set to i2s sd of ch.4
    Func60_Can1_Tx           = 60u,              ///< function set to can tx of ch.1
    Func61_Can1_Rx           = 61u,              ///< function set to can rx of ch.1
    Func62_Can2_Tx           = 62u,              ///< function set to can tx of ch.2
    Func63_Can2_Rx           = 63u,              ///< function set to can rx of ch.2
}en_port_func_t;

//×Ô¶¨Òå
typedef struct
{
	en_port_t Port;
	en_pin_t Pin;
	en_port_func_t IoFunc_x;
} MCU_IO;

/**
 * @defgroup GPIO_DebugPin_Sel GPIO Debug Pin Selection
 * @{
 */
#define GPIO_PIN_TCK                (0x01U)
#define GPIO_PIN_TMS                (0x02U)
#define GPIO_PIN_TDO                (0x04U)
#define GPIO_PIN_TDI                (0x08U)
#define GPIO_PIN_TRST               (0x10U)
#define GPIO_PIN_DEBUG_JTAG         (0x1FU)
#define GPIO_PIN_SWCLK              (0x01U)
#define GPIO_PIN_SWDIO              (0x02U)
#define GPIO_PIN_SWO                (0x04U)
#define GPIO_PIN_DEBUG_SWD          (0x07U)
#define GPIO_PIN_DEBUG              (0x1FU)
/**
 * @}
 */

/**
 * @defgroup GPIO_ReadCycle_Sel GPIO Pin Read Wait Cycle Selection
 * @{
 */
#define GPIO_RD_WAIT0              (0x00U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT1              (0x01U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT2              (0x02U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT3              (0x03U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT4              (0x04U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT5              (0x05U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT6              (0x06U << GPIO_PCCR_RDWT_POS)
#define GPIO_RD_WAIT7              (0x07U << GPIO_PCCR_RDWT_POS)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinState_Sel GPIO Pin Output State Selection
 * @{
 */
#define PIN_STAT_RST               (0U)
#define PIN_STAT_SET               (GPIO_PCR_POUT)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinDirection_Sel GPIO Pin Input/Output Direction Selection
 * @{
 */
#define PIN_DIR_IN                  (0U)
#define PIN_DIR_OUT                 (GPIO_PCR_POUTE)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinOutType_Sel GPIO Pin Output Type Selection
 * @{
 */
#define PIN_OUT_TYPE_CMOS           (0U)
#define PIN_OUT_TYPE_NMOS           (GPIO_PCR_NOD)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinDrv_Sel GPIO Pin Drive Capacity Selection
 * @{
 */
#define PIN_LOW_DRV                 (0U)
#define PIN_MID_DRV                 (GPIO_PCR_DRV_0)
#define PIN_HIGH_DRV                (GPIO_PCR_DRV_1)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinLatch_Sel GPIO Pin Output Latch Selection
 * @{
 */
#define PIN_LATCH_OFF               (0U)
#define PIN_LATCH_ON                (GPIO_PCR_LTE)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinPU_Sel GPIO Pin Internal Pull-Up Resistor Selection
 * @{
 */
#define PIN_PU_OFF                  (0U)
#define PIN_PU_ON                   (GPIO_PCR_PUU)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinInvert_Sel GPIO Pin I/O Invert Selection
 * @{
 */
#define PIN_INVT_OFF                (0U)
#define PIN_INVT_ON                 (GPIO_PCR_INVE)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinInType_Sel GPIO Pin Input Type Selection
 * @{
 */
#define PIN_IN_TYPE_SMT             (0U)
#define PIN_IN_TYPE_CMOS            (GPIO_PCR_CINSEL)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinExtInt_Sel GPIO Pin External Interrupt Selection
 * @{
 */
#define PIN_EXTINT_OFF              (0U)
#define PIN_EXTINT_ON               (GPIO_PCR_INTE)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinMode_Sel GPIO Pin Mode Selection
 * @{
 */
#define PIN_ATTR_DIGITAL            (0U)
#define PIN_ATTR_ANALOG             (GPIO_PCR_DDIS)
/**
 * @}
 */

/**
 * @defgroup GPIO_PinSubFuncSet_Sel GPIO Pin Sub-function ENABLE or DISABLE
 * @{
 */
#define PIN_SUBFUNC_DISABLE         (0U)
#define PIN_SUBFUNC_ENABLE          (GPIO_PFSR_BFE)
/**
 * @}
 */

/**
 * @defgroup GPIO_Register_Protect_Key GPIO Registers Protect Key
 * @{
 */
#define GPIO_REG_LOCK_KEY            (0xA500U)
#define GPIO_REG_UNLOCK_KEY          (0xA501U)
/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
/**
 * @addtogroup GPIO_Global_Functions
 * @{
 */
/**
 * @brief  GPIO lock. PSPCR, PCCR, PINAER, PCRxy, PFSRxy write disable
 * @param  None
 * @retval None
 */
__STATIC_INLINE void GPIO_REG_Lock(void)
{
    WRITE_REG16(CM_GPIO->PWPR, GPIO_REG_LOCK_KEY);
}

/**
 * @brief  GPIO unlock. PSPCR, PCCR, PINAER, PCRxy, PFSRxy write enable
 * @param  None
 * @retval None
 */
__STATIC_INLINE void GPIO_REG_Unlock(void)
{
    WRITE_REG16(CM_GPIO->PWPR, GPIO_REG_UNLOCK_KEY);
}

int32_t GPIO_Init(uint8_t u8Port, uint16_t u16Pin, const stc_gpio_init_t *pstcGpioInit);
void GPIO_DeInit(void);
int32_t GPIO_StructInit(stc_gpio_init_t *pstcGpioInit);
void GPIO_SetDebugPort(uint8_t u8DebugPort, en_functional_state_t enNewState);
void GPIO_SetFunc(uint8_t u8Port, uint16_t u16Pin, uint16_t u16Func);
void GPIO_SubFuncCmd(uint8_t u8Port, uint16_t u16Pin, en_functional_state_t enNewState);
void GPIO_SetSubFunc(uint8_t u8Func);
void GPIO_SetReadWaitCycle(uint16_t u16ReadWait);
void GPIO_InputMOSCmd(uint8_t u8Port, en_functional_state_t enNewState);
void GPIO_OutputCmd(uint8_t u8Port, uint16_t u16Pin, en_functional_state_t enNewState);
en_pin_state_t GPIO_ReadInputPins(uint8_t u8Port, uint16_t u16Pin);
uint16_t GPIO_ReadInputPort(uint8_t u8Port);
en_pin_state_t GPIO_ReadOutputPins(uint8_t u8Port, uint16_t u16Pin);
uint16_t GPIO_ReadOutputPort(uint8_t u8Port);
void GPIO_SetPins(uint8_t u8Port, uint16_t u16Pin);
void GPIO_ResetPins(uint8_t u8Port, uint16_t u16Pin);
void GPIO_WritePort(uint8_t u8Port, uint16_t u16PortVal);
void GPIO_TogglePins(uint8_t u8Port, uint16_t u16Pin);
void GPIO_ExtIntCmd(uint8_t u8Port, uint16_t u16Pin, en_functional_state_t enNewState);
void GPIO_AnalogCmd(uint8_t u8Port, uint16_t u16Pin, en_functional_state_t enNewState);
/**
 * @}
 */

#endif /* LL_GPIO_ENABLE */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __HC32_LL_GPIO_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
