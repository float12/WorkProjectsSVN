//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述		ALLHEAD头文件
// 修改记录
//----------------------------------------------------------------------
#ifndef __ALLHEAD_H
#define __ALLHEAD_H
#include "nwy_config.h"
#include "nwy_osi_api.h"
#ifdef FEATURE_NWY_AT_HX_GNSS
	#include "nwy_loc_hx.h"
#else
	#include "nwy_loc.h"
#endif
#include "nwy_adc.h"
#include "nwy_audio_api.h"
#include "nwy_data.h"
#include "nwy_file.h"
#include "nwy_fota.h"
#include "nwy_fota_api.h"
#include "nwy_gpio.h"
#include "nwy_gpio_open.h"
#include "nwy_i2c.h"
#include "nwy_keypad.h"
#include "nwy_led.h"
#include "nwy_network.h"
#include "nwy_pm.h"
#include "nwy_sim.h"
#include "nwy_sms.h"
#include "nwy_socket.h"
#include "nwy_spi.h"
#include "nwy_uart.h"
#include "nwy_usb_serial.h"
#include "nwy_vir_at.h"
#include "nwy_voice.h"
#include "nwy_wifi.h"
#include "osi_log.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
// #include "nwy_alimqtt.h"
#include "MQTTClient.h"
#include "mqtt_api.h"
#include "nwy_ftp.h"
#include "nwy_http.h"
#include "nwy_mqtt.h"
#include "nwy_osi_api.h"
/* added by wangchen for N58 ussd api to test 20200826 begin */
#include "nwy_ussd.h"
/* added by wangchen for N58 ussd api to test 20200826 end */
#include "nwy_ble.h"
#include "nwy_dm.h"
#include "nwy_pwm.h"
#ifdef FEATURE_NWY_OPEN_ZZD_SDK
	#include "nwy_oem_zzd_api.h"
#endif
#include "nwy_ip_packet.h"
#include "nwy_poc_dsds.h"
/* added by wangchen for stk openapi 2021.08.26 begin */
#ifdef FEATURE_NWY_STK_COMMAND
	#include "nwy_sat.h"
#endif
/* added by wangchen for stk openapi 2021.08.26 end */
#include "nwy_camera.h"
#ifdef FEATURE_NEOWAY_PIPECLOUD
	#include "nwy_pipecloud.h"
#endif

// #define N716_TP_CST816_TEST
#ifdef N716_TP_CST816_TEST
	#include "nwy_tp.h"
#endif
#include "wsd_func_def.h"
#include "cJSON.h"
#include "RTC_API.h"
#include "wsd_dlt645.h"
#include "wsd_dlt698.h"
#include "wsd_Modbus.h"
#include "wsd_uart.h"
#include "wsd_flag.h"
#include "wsd_meter_update.h"
#include "wsd_TF.h"
#include "wsd_sysevent.h"
#include <math.h>

//-----------------------------------------------
//				宏定义
//-----------------------------------------------

//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				变量声明
//-----------------------------------------------

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

#endif   // #ifndef __ALLHEAD_H
