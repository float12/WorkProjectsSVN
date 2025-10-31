//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
// 创建人
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
void nwy_cipgsmloc_cb(char *text)
{
	nwy_log_cipgsmloc_result_t *param = (nwy_log_cipgsmloc_result_t *)text;
	if (NULL == param)
		return;
	nwy_ext_echo("\r\n cipgsmloc info\r\n");
	if (0 == param->result)
	{
		nwy_ext_echo("lat %lf \r\n", param->info.data.lat);
		nwy_ext_echo("lng %lf \r\n", param->info.data.lng);
		nwy_ext_echo("accuracy %lf \r\n", param->info.data.alt);
	}
	else
	{
		nwy_ext_echo(" %s\r\n", param->info.errmsg);
	}
	return;
}
//--------------------------------------------------
// 功能描述:  地理位置信息解析
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL get_gps_position_info(char *c_data, DWORD *addr)
{
	char c_nmea[2048 + 16] = {0};
	int i_len = 0;
	DWORD dwValue[64] = {0};
	BYTE byValueNum = 0;
	BYTE byValuePos = 0;
	char *p_GPGGA = NULL;
	char c_GNGGA[] = "GNGGA";
	char c_GPGGA[] = "GPGGA";
	char c_temp;
	SDWORD dwData = 0;
	BYTE byData = 0;
	BOOL bRes = FALSE;

	i_len = strlen(c_data);
	memcpy(c_nmea, c_data, i_len);

	memset((BYTE *)&dwValue[0], 0xFF, sizeof(dwValue));
	p_GPGGA = strstr(c_nmea, c_GPGGA);
	if (p_GPGGA == NULL)
	{
		p_GPGGA = strstr(c_nmea, c_GNGGA);
	}
	if (p_GPGGA)
	{
		i_len = sizeof(c_GPGGA);
		p_GPGGA += i_len;
		byValueNum = 0;
		byValuePos = 0;
		dwData = 0;
		while ((byValueNum < 12) && (byValuePos < (64 + 16)))
		{
			c_temp = p_GPGGA[byValuePos++];
			if (c_temp == '.')
			{
			}
			else if ((c_temp >= '0') && (c_temp <= '9'))
			{
				byData = c_temp - '0';
				dwData *= 10;
				dwData += byData;
			}
			else if ((c_temp == 'S') || (c_temp == 'E') || (c_temp == 's') || (c_temp == 'e') || (c_temp == 'M') || (c_temp == 'm'))
			{
				dwValue[byValueNum++] = 0;
				byValuePos++; // 跳过,
			}
			else if ((c_temp == 'N') || (c_temp == 'W') || (c_temp == 'n') || (c_temp == 'w'))
			{
				dwValue[byValueNum++] = 1;
				byValuePos++; // 跳过,
			}
			else if (c_temp == ',')
			{
				dwValue[byValueNum++] = dwData;
				dwData = 0;
			}
			else
			{
				dwData = 0;
			}
		}
		if (byValueNum == 12)
		{
			dwData = dwValue[1];
			nwy_ext_echo("\r\n latitude is %d", dwData);
			addr[0] = dwData % 100000;
			dwData = dwData / 100000;
			addr[1] = dwData % 100;
			dwData = dwData / 100;
			addr[2] = dwData;
			addr[3] = dwValue[2];

			dwData = dwValue[3];
			nwy_ext_echo("\r\n longitude is %d", dwData);
			addr[4] = dwData % 100000;
			dwData = dwData / 100000;
			addr[5] = dwData % 100;
			dwData = dwData / 100;
			addr[6] = dwData;
			addr[7] = dwValue[4];
			addr[8] = dwValue[8];
		}
	}
	return bRes;
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void Location_Task(void *param)
{
	int LocationStep = 1;
	int ret = -1;
	int result = -1;
	int nmea_freq = 1000;

	nwy_loc_position_mode_t pos_mode;
	nwy_loc_startup_mode startup;

	nwy_loc_close_uart_nmea_data();
	ret = nwy_loc_start_navigation();
	if (ret)
	{
		nwy_ext_echo("\r\n open location success");
	}
	else
	{
		nwy_ext_echo("\r\n open location fail");
	}
	nwy_sleep(10000);

	while (1)
	{
		//    if(0 != nwy_ext_check_data_connect())
		{
			switch (LocationStep)
			{
			case 1:
			{
				nwy_ext_echo("\r\n set position(1-gps 3-gps+bd 5-gps+glo 7-gps+bd+glo):");

				pos_mode = 1;
				if (pos_mode > 7 || pos_mode < 1)
				{
					result = -1;
					nwy_ext_echo("\r\n set position mode fail,invalid param");
				}
				else
				{
					nwy_ext_echo("\r\n set position mode:%d", pos_mode);
					result = nwy_loc_set_position_mode(pos_mode);
				}
				if (result)
				{
					nwy_ext_echo("\r\n set position mode success");
				}
				else
				{
					nwy_ext_echo("\r\n set position mode fail");
				}
				LocationStep++;
				break;
			}
			case 2:
			{
				nwy_ext_echo("\r\n set location update rate(1000-1HZ 500-2HZ)ms:");
				nmea_freq = 1000;
				if (nmea_freq > 1000 || nmea_freq < 500)
				{
					result = -1;
					nwy_ext_echo("\r\n set location update rate fail,invalid param");
				}
				else
				{
					nwy_ext_echo("\r\n update rate:%d", nmea_freq);
					result = nwy_loc_nmea_format_mode(2, nmea_freq);
				}
				if (result)
				{
					nwy_ext_echo("\r\n set location update rate success");
				}
				else
				{
					nwy_ext_echo("\r\n set location update rate fail");
				}
				LocationStep++;
				break;
			}
			case 3:
			{
				nwy_ext_echo("\r\n set startup mode(0-hot 1-warm 2-cold 4-factory):");
				startup = 2;
				nwy_ext_echo("\r\n startup mode:%d", startup);
				result = nwy_loc_set_startup_mode(startup);
				if (result)
				{
					nwy_ext_echo("\r\n set nmea statements output format success");
				}
				else
				{
					nwy_ext_echo("\r\n set nmea statements output format fail");
				}
				nwy_sleep(500);
				LocationStep = 100;
				break;
			}
			default:
				nwy_exit_thread();
				break;
			}
		}
		nwy_sleep(1000);
	}
}
