//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
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
static int ppp_state[10] = {0};
char c_apn[] = "CMNET";
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

int nwy_ext_check_data_connect()//检测拨号成功
{
	int i = 0;
	for(i = 0; i < NWY_DATA_CALL_MAX_NUM; i++)
	{
		if(ppp_state[i] == NWY_DATA_CALL_CONNECTED)
		{
			return 1;
		}
	}
	return 0;
}
int check_data_connect_for_pid(BYTE pid)
{
	if(pid > 0 && pid <= 8)
	{
		if(ppp_state[pid - 1] == NWY_DATA_CALL_CONNECTED)
		{
			return 1;
		}
	}
	return 0;
}
void nwy_ext_send_sig(nwy_osiThread_t *thread, uint16 sig)
{
	nwy_osiEvent_t event;

	memset(&event, 0, sizeof(event));
	event.id = sig;
	nwy_send_thead_event(thread, &event, 0);
}
static nwy_osiMutex_t *echo_mutex = NULL;
void nwy_ext_echo(char *fmt, ...)
{
	static char echo_str[NWY_EXT_SIO_RX_MAX];
	if (PrintLogSign == 0)
	{
		int tx_size;
		va_list a;
		int i, size;
		if(NULL == echo_mutex)
			echo_mutex = nwy_create_mutex();
		if(NULL == echo_mutex)
			return;
		nwy_lock_mutex(echo_mutex, 0);
		va_start(a, fmt);
		vsnprintf(echo_str, NWY_EXT_SIO_RX_MAX, fmt, a);
		va_end(a);
		size = strlen((char *)echo_str);
		i = 0;
		tx_size = 0;
		while(1)
		{
			if (BleLogSwitch == 0)
			{
				tx_size = nwy_usb_serial_send((char *)echo_str + i, size - i);
			}
			#if( BLE_WH== YES)
			else
			{
				SendBleDataInPiece((char *)echo_str + i,size - i);
				tx_size = (size - i);
			}
			#endif
			if(tx_size <= 0)
				break;
			i += tx_size;
			if((i < size))
				nwy_sleep(10);
			else
				break;
		}
		nwy_unlock_mutex(echo_mutex);
	}
}

void nwy_data_cb_fun(int hndl, nwy_data_call_state_t ind_state)
{
	OSI_LOGI(0, "=DATA= hndl=%d,ind_state=%d", hndl, ind_state);
	if(hndl > 0 && hndl <= 8)
	{
		ppp_state[hndl - 1] = ind_state;
		nwy_ext_echo("\r\nData call status update, handle_id:%d,state:%d\r\n", hndl, ind_state);
	}
}
int Get_data_srv()
{
	int hndl = nwy_data_get_srv_handle(nwy_data_cb_fun);
	OSI_LOGI(0, "=DATA=  hndl= %d", hndl);
	/* Begin: Add by yjj for TD83000 in 2021.05.12*/
	if((hndl > 0) && (hndl <= NWY_DATA_CALL_MAX_NUM))
	{
		nwy_ext_echo("\r\nCreate a Resource Handle id: %d success\r\n", hndl);
		return hndl;
	}
	else
	{
		nwy_ext_echo("\r\nCreate a Resource Handle failed, result<%d>\r\n", hndl);
		return 0;
	}
}

void DataCall_Task(void *profileID)
{
	int profile_id = *(int *)profileID;
	nwy_data_profile_info_t profile_info = {0};
	BOOL bisSetPara = FALSE;
	int ret;
	unsigned char csq = 0;
	int s_NetRunStep = 0;
	int hndl = 0;
	int len = 0;
	nwy_data_start_call_v02_t param_t;
	nwy_data_addr_t_info addr_info;
	nwy_sim_status sim_status;
	int gb_csq_fail_count = 0;
	nwy_ext_echo("Start DateCall profileID : %d", profile_id);
	nwy_set_sim_detect_mode(TRUE);
	static WORD wConnectFalseNum = 0;

	while(1)
	{
		switch(s_NetRunStep)
		{
		case 0:   // 0.延时 30
			nwy_sleep(2000);
			sim_status = nwy_sim_get_card_status();
			if(sim_status == NWY_SIM_STATUS_NOT_INSERT)
			{
				nwy_ext_echo("\r\n no card insert");
			}
			else if(sim_status == NWY_SIM_STATUS_READY)
			{
				nwy_ext_echo("\r\nsim is ready", sim_status);
				nwy_nw_set_radio_st(1);//设置正常模式
				s_NetRunStep++;
			}
			else
			{
				nwy_ext_echo("\r\n have card but not ready");
			}
			break;
		case 1:   // 1. 创建服务拨号句柄
			hndl = nwy_data_get_srv_handle(nwy_data_cb_fun);
			if((hndl > 0) && (hndl <= NWY_DATA_CALL_MAX_NUM))
			{
				nwy_ext_echo("\r\nCreate a Resource Handle id: %d success\r\n", hndl);
			}
			else
			{
				nwy_ext_echo("\r\nCreate a Resource Handle failed, result<%d>\r\n", hndl);
				s_NetRunStep = 8;
				break;
			}
			s_NetRunStep++;
			break;
		case 2:   // 2.检查配置文件
			memset(&profile_info, 0, sizeof(nwy_data_profile_info_t));
			ret = nwy_data_get_profile(profile_id, NWY_DATA_PROFILE_3GPP, &profile_info);
			if(ret != 0)
			{
				nwy_ext_echo("\r\nRead profile %d info fail, result%d\r\n", profile_id, ret);
			}
			else
			{
				nwy_ext_echo("\r\nProfile %d info: <pdp_type>,<auth_proto>,<apn>,<user_name>,<password>\r\n%d,%d,%s,%s,%s\r\n", profile_id, profile_info.pdp_type,
				profile_info.auth_proto, profile_info.apn, profile_info.user_name, profile_info.pwd);
			}
			if((strcmp(c_apn, profile_info.apn) == 0)
			   //&& (strcmp(gVar.d_Para.c_User, profile_info.user_name) == 0)
			   //&& (strcmp(gVar.d_Para.c_Psw, profile_info.pwd) == 0)
			)
			{   // 比较APN.USR.PWSD
				bisSetPara = TRUE;
			}
			else
			{
				bisSetPara = FALSE;
			}
			s_NetRunStep++;
			break;
		case 3:   // 3. 设置配置文件
			if(!bisSetPara)
			{
				memset(&profile_info, 0, sizeof(nwy_data_profile_info_t));
				profile_info.auth_proto = 0;   // auth_proto;
				profile_info.pdp_type = 1;
				memcpy(profile_info.apn, c_apn, sizeof(c_apn));
				memset(profile_info.user_name, 0, sizeof(profile_info.user_name));
				memset(profile_info.pwd, 0, sizeof(profile_info.pwd));
				ret = nwy_data_set_profile(profile_id, NWY_DATA_PROFILE_3GPP, &profile_info);
				if(ret != 0)
				{
					nwy_ext_echo("\r\nSet profile %d info fail, result<%d>\r\n", profile_id, ret);
					s_NetRunStep = 8;
				}
				else
				{
					nwy_ext_echo("\r\nSet profile %d info success\r\n", profile_id);
					s_NetRunStep++;
				}
				nwy_sleep(2000);   // 如果设置参数，需要延迟
				break;
			}
			s_NetRunStep++;
			break;
		case 4:   // 4 触发拨号
			memset(&param_t, 0, sizeof(nwy_data_start_call_v02_t));
			param_t.profile_idx = profile_id;
			param_t.is_auto_recon = 1;
			param_t.auto_re_max = 50;
			param_t.auto_interval_ms = 10000;
			ret = nwy_data_start_call(hndl, &param_t);
			if(ret != 0)
			{
				nwy_ext_echo("\r\nStart data call fail, result<%d>\r\n", ret);
				
				s_NetRunStep = 7;
			}
			else
			{
				nwy_ext_echo("\r\nStart data call ...\r\n");
				s_NetRunStep++;
			}
			nwy_sleep(1000);   // 如果设置参数，需要延迟
			break;
		case 5:
			memset(&addr_info, 0, sizeof(nwy_data_addr_t_info));
			ret = nwy_data_get_ip_addr(hndl, &addr_info, &len);//判断标志
			if(ret != NWY_RES_OK)
			{
				nwy_ext_echo("\r\nGet data info fail, result<%d>\r\n", ret);
				s_NetRunStep = 7;
				break;
			}
			nwy_ext_echo("\r\nGet data info success\r\nIface address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.iface_addr_s.ip_addr),
						 nwy_ip6addr_ntoa(&addr_info.iface_addr_s.ip6_addr));
			nwy_ext_echo("Dnsp address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.dnsp_addr_s.ip_addr), nwy_ip6addr_ntoa(&addr_info.dnsp_addr_s.ip6_addr));
			nwy_ext_echo("Dnss address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.dnss_addr_s.ip_addr), nwy_ip6addr_ntoa(&addr_info.dnss_addr_s.ip6_addr));
			// IMEI
			nwy_sim_result_type imei = {0};
			ret = nwy_sim_get_imei(&imei);
			if(ret != 0)
			{
				nwy_ext_echo(0, "=UIM= nwy get imei fail");
			}
			nwy_ext_echo("\r\n imei:%s \r\n", imei.nImei);
			// CCID ismi
			nwy_sim_result_type sim = {"", "", "", "", NWY_SIM_AUTH_NULL};
			memset(&sim, 0, sizeof(nwy_sim_result_type));
			nwy_sim_get_iccid(&sim);
			nwy_sleep(1000);
			nwy_sim_get_imsi(&sim);
			nwy_ext_echo("\r\n iccid:%s, imsi:%s)", sim.iccid, sim.imsi);
			api_UpdateForInte();
			s_NetRunStep++;
			break;
		case 6:   // 6. 连接成功
			if(!check_data_connect_for_pid(profile_id))
			{
				gb_csq_fail_count = 0;
				s_NetRunStep = 7;   // 异常
				break;
			}
			wConnectFalseNum = 0;
			nwy_nw_get_signal_csq(&csq);   // 降低频率
			if((csq <= 5) || (csq >= 99))
			{
				nwy_ext_echo("csq:%d \r\n", csq);
				csq = 0;
				gb_csq_fail_count++;
				if(gb_csq_fail_count > 10)
				{
					gb_csq_fail_count = 0;
					s_NetRunStep = 7;   // 异常
					break;
				}
			}
			else
			{
				gb_csq_fail_count = 0;
			}
			nwy_sleep(10000);
			break;
		case 7:
			ret = nwy_data_stop_call(hndl);
			if(ret != NWY_RES_OK)
				nwy_ext_echo("\r\nStop data call fail, result<%d>\r\n", ret);
			else
				nwy_ext_echo("\r\nStop data call ...\r\n");
			s_NetRunStep++;
			break;
		case 8:
			nwy_data_relealse_srv_handle(hndl);
			nwy_ext_echo("\r\nRelease resource handle id %d\r\n", hndl);
			api_WriteSysUNMsg(SYSUN_NETWORK_ABNORMAL);
			s_NetRunStep++;
			wConnectFalseNum ++;
			if( wConnectFalseNum >= 360 ) //0.5~2.5小时一直未拨号成功，复位（不同路径退出时间会有差异）
			{
				api_WriteSysUNMsg(DATACALL_RESET);
				nwy_power_off(2);
			}
			break;
		default:
			s_NetRunStep = 0;
			break;
		}
	}
}