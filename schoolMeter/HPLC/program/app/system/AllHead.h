//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		����ͷ�ļ�ͳһ������ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __ALLHEAD_H
#define __ALLHEAD_H

//#include <stdio.h>
#include "__def.h"
#include "sys.h"
#include "ConstConfig.h"
#include "board_API.h"
#include "Config_3Phase_Meter.h"
#include "Config_Single_Meter.h"
#include "Specialuser.h"
#include "cpu_API.h"
#include "relation.h"

#include "RTC_API.h"
#include "math.h"
#include "string.h"
#include "ctype.h"

#include "CommFunc_API.h"

#include "assert_API.h"
#include "lcd_API.h"
#include "relation.h"
#include "stdlib.h"
#include <string.h>
#include "syswatch_API.h"
#include "flag_API.h"
#include "SysDescribe.h"
#include "para_API.h"
#include "Protocol.h"
#include "SearchCollectMeter_API.h"
#include "Dlt698_45_API.h"
#include "dlt645_2007_API.h"
#include "MCollect_Api.h"
#include "demand_Api.h"
#include "demand_2022_Api.h"
#include "event_Api.h"
#include "Report645_Api.h"
#include "Report698_Api.h"
#include "sysEvent_API.h"
#include "Sample_API.h"
#include "PrePay_API.h"
#include "Energy_API.h"
#include "Relay_Local_API.h"
#include "Relay_Remote_API.h"
#include "Freeze_API.h"
#include "savemem_API.h"
#include "memory_API.h"

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
	        								
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
void MainTask(void);

#endif//#ifndef __ALLHEAD_H


