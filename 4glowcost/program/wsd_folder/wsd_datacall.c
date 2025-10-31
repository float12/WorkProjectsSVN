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
	for(i = 0; i < 8; i++)
	{
		if(ppp_state[i] == NWY_DATA_CALL_CONNECTED_STATE)
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
		if(ppp_state[pid - 1] == NWY_DATA_CALL_CONNECTED_STATE)
		{
			return 1;
		}
	}
	return 0;
}
void nwy_ext_echo(char *fmt, ...)
{
	static char echo_str[1024];
	static nwy_osi_mutex_t echo_mutex = NULL;
	va_list a;
	int i, size;

	if (NULL == echo_mutex) {
		nwy_sdk_mutex_create(&echo_mutex);
	}
	if (NULL == echo_mutex) {
		return;
	}
	nwy_sdk_mutex_lock(echo_mutex, NWY_OSA_SUSPEND);
	va_start(a, fmt);
	vsnprintf(echo_str, 1024, fmt, a);
	va_end(a);
	size = strlen((char *)echo_str);
	i = 0;
	while (1)
	{
		int tx_size;

		tx_size = nwy_usb_serial_send((char *)echo_str + i, size - i);
		if (tx_size <= 0)
		    break;
		i += tx_size;
		if ((i < size))
		    nwy_thread_sleep(10);
		else
		    break;
	}

	nwy_sdk_mutex_unlock(echo_mutex);
}
void nwy_data_cb_fun(int profile_idx, nwy_data_call_state_e ind_state)
{
	if (profile_idx > 0 && profile_idx < 8)
	{
		nwy_ext_echo("\r\nData call status update, profile_id:%d,state:%d\r\n", profile_idx, ind_state);
		if(ind_state == NWY_DATA_CALL_DISCONNECTED_STATE)
		{
			// if(is_trigger_by_app[profile_idx-1]==1)
			// {
				// if the data callback is trigger by current app , unreg the cb function
				// if(current_open_dial_action[profile_idx-1]==ind_state)
				// {
						// nwy_ext_echo("\r\nData call status NWY_DATA_CALL_DISCONNECTED_STATE\r\n");
					// nwy_data_unreg_cb(nwy_data_sim_id, profile_idx,nwy_test_cli_data_cb_fun);        
				// }
				// else
				// {
					// nwy_ext_echo("\r\nStart data call failed\r\n");
				// }
			// }
			// else
			{
				nwy_ext_echo("\r\nData call status NWY_DATA_CALL_DISCONNECTED_STATE\r\n");
			}
		}
		else if(ind_state == NWY_DATA_CALL_CONNECTED_STATE)
		{
			// if(is_trigger_by_app[profile_idx-1]==1)
			// {
				// nwy_ext_echo("\r\nStart data call success\r\n");
			// }
			ppp_state[profile_idx-1] = NWY_DATA_CALL_CONNECTED_STATE;
			nwy_ext_echo("\r\nData call status NWY_DATA_CALL_CONNECTED_STATE\r\n");
		}
	}
}

void DataCall_Task(void *profileID)
{
	int profile_id = *(int *)profileID;
	nwy_data_profile_info_t profile_info = {0};
	nwy_data_auto_connect_cfg_t cfg = {0};
	BOOL bisSetPara = FALSE;
	int ret = NWY_GEN_E_UNKNOWN;
	nwy_nw_get_csq_info_t csq_val;
	int s_NetRunStep = 0;
	nwy_data_start_call_t param_t;
	// nwy_data_addr_t_info addr_info;
	nwy_sim_status_e sim_status;
	int gb_csq_fail_count = 0;
	nwy_ext_echo("Start DateCall profileID : %d", profile_id);
	// nwy_set_sim_detect_mode(TRUE);忘记这个是不是热插拔 功能了！！！
	BYTE i = 0;
	static WORD wConnectFalseNum = 0;
	nwy_sim_id_e simid = 0;//！！！sim id 
	char imei[16] = {0};

	while(1)
	{
		// nwy_ext_echo("\r\ndataCall RunStep: %d", s_NetRunStep);
		switch(s_NetRunStep)
		{
		case 0:   // 0.延时 30
			nwy_thread_sleep(5000);
			nwy_sim_status_get(simid,&sim_status);
			if(sim_status == NWY_SIM_NOT_INSERTED)
			{
				nwy_ext_echo("\r\n no card insert");
			}
			else if(sim_status == NWY_SIM_READY)
			{
				nwy_ext_echo("\r\nsim is ready", sim_status);
				// nwy_nw_set_radio_st(1);//设置正常模式！！！ 不确定 影不影响
				s_NetRunStep++;
			}
			else
			{
				nwy_ext_echo("\r\n have card but not ready");
			}
			break;
		case 1:   // 1. 创建服务拨号句柄
			// hndl = nwy_data_get_srv_handle(nwy_data_cb_fun);
			// if((hndl > 0) && (hndl <= NWY_DATA_CALL_MAX_NUM))
			// {
			// 	nwy_ext_echo("\r\nCreate a Resource Handle id: %d success\r\n", hndl);
			// }
			// else
			// {
			// 	nwy_ext_echo("\r\nCreate a Resource Handle failed, result<%d>\r\n", hndl);
			// 	s_NetRunStep = 8;
			// 	break;
			// }
			ret = nwy_data_reg_cb(simid, profile_id, nwy_data_cb_fun);

  			if (ret != NWY_SUCCESS)
  			{
  				nwy_ext_echo("\r\nStart data call fail, result<%d>\r\n", ret);
  				s_NetRunStep = 8;
				break;
  			}
			s_NetRunStep++;
			break;
		case 2:   // 2.检查配置文件
			memset(&profile_info, 0, sizeof(nwy_data_profile_info_t));
			ret = nwy_data_profile_get(simid,profile_id,&profile_info);
			if(ret != NWY_SUCCESS)
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
				ret = NWY_GEN_E_UNKNOWN;
				memset(&profile_info, 0, sizeof(nwy_data_profile_info_t));
				profile_info.auth_proto = 0;   // auth_proto;
				profile_info.pdp_type = 1;
				memcpy(profile_info.apn, c_apn, sizeof(c_apn));
				memset(profile_info.user_name, 0, sizeof(profile_info.user_name));
				memset(profile_info.pwd, 0, sizeof(profile_info.pwd));
				ret = nwy_data_profile_set(simid,profile_id,&profile_info);
				if(ret != NWY_SUCCESS)
				{
					nwy_ext_echo("\r\nSet profile %d info fail, result<%d>\r\n", profile_id, ret);
					s_NetRunStep = 8;
				}
				else
				{
					nwy_ext_echo("\r\nSet profile %d info success\r\n", profile_id);
					s_NetRunStep++;
				}
				nwy_thread_sleep(5000);   // 如果设置参数，需要延迟
				break;
			}
			s_NetRunStep++;
			break;
		case 9://！！！后面再改吧
			cfg.interval[0] = 128;
			cfg.retry_max = 50;
			ret = nwy_data_auto_connect_set(profile_id,&cfg);
			if (ret != NWY_SUCCESS) {
				NWY_SDK_LOG_DEBUG("nwy auto connect set  fail ret = %d", ret,0, 0);
				s_NetRunStep = 7;
			}
			else
			{
				s_NetRunStep++;
			}
			break;	
		case 4:   // 4 触发拨号
			memset(&param_t, 0, sizeof(nwy_data_start_call_t));
			param_t.cid = profile_id;
			param_t.call_auto_type = NWY_DATA_CALL_AUTO_TYPE_ENABLE;
			param_t.action = NWY_DATA_CALL_ACT;
 			param_t.trigger_type = NWY_DATA_TRIGGER_OPEN;
 			param_t.if_internal_call = 1;
 			param_t.set_profile.pdp_type = NWY_DATA_PDP_TYPE_MIN ;
 			param_t.set_profile.auth_proto = NWY_DATA_AUTH_PROTO_MIN ;    
			ret = nwy_data_call_start(simid, &param_t);
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
			nwy_thread_sleep(1000);   // 如果设置参数，需要延迟
			break;
		case 5:
			// memset(&addr_info, 0, sizeof(nwy_data_addr_t_info));！！！ 看读出来没什么用 先不替换了
			// ret = nwy_data_get_ip_addr(hndl, &addr_info, &len);//判断标志
			// if(ret != NWY_RES_OK)
			// {
			// 	nwy_ext_echo("\r\nGet data info fail, result<%d>\r\n", ret);
			// 	s_NetRunStep = 7;
			// 	break;
			// }
			// nwy_ext_echo("\r\nGet data info success\r\nIface address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.iface_addr_s.ip_addr),
			// 			 nwy_ip6addr_ntoa(&addr_info.iface_addr_s.ip6_addr));
			// nwy_ext_echo("Dnsp address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.dnsp_addr_s.ip_addr), nwy_ip6addr_ntoa(&addr_info.dnsp_addr_s.ip6_addr));
			// nwy_ext_echo("Dnss address: %s,%s\r\n", nwy_ip4addr_ntoa(&addr_info.dnss_addr_s.ip_addr), nwy_ip6addr_ntoa(&addr_info.dnss_addr_s.ip6_addr));
			// IMEI
 			ret = nwy_dm_imei_get(NWY_SIM_ID_SLOT_1, imei, sizeof(imei));
 			if (NWY_SUCCESS != ret)
 			{
 				nwy_ext_echo("\r\n Get IMEI error \r\n");
				s_NetRunStep = 7;
 			}
 			nwy_ext_echo("\r\nIMEI:%s \r\n", imei);
			// CCID ismi  ！！！ 暂时没什么用 先不替换了
			// nwy_sim_result_type sim = {"", "", "", "", NWY_SIM_AUTH_NULL};
			// memset(&sim, 0, sizeof(nwy_sim_result_type));
			// nwy_sim_get_iccid(&sim);
			// nwy_thread_sleep(1000);
			// nwy_sim_get_imsi(&sim);
			// nwy_ext_echo("\r\n iccid:%s, imsi:%s)", sim.iccid, sim.imsi);
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
			nwy_nw_csq_get(simid,&csq_val);
			// nwy_ext_echo("\r\nCSQ: rssi_level:%d,ber:%d", csq_val.csq_rssi_level, csq_val.ber);
			if((csq_val.csq_rssi_level <= 5) || (csq_val.csq_rssi_level >= 99))
			{
				nwy_ext_echo("csq:%d \r\n", csq_val.csq_rssi_level);
				memset(&csq_val,0,sizeof(nwy_nw_get_csq_info_t));
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
			nwy_thread_sleep(10000);
			break;
		case 7:
			ret = nwy_data_call_stop(simid,&param_t);
			if(ret != NWY_SUCCESS)
			{
				nwy_ext_echo("\r\nStop data call fail, result<%d>\r\n", ret);
			}

			if (ret == NWY_GEN_E_AREADY_DISCONNECT)
			{
				nwy_ext_echo("\r\n Data call in cid %d already disconnected\r\n",profile_id);
			}
			nwy_ext_echo("\r\nStop data call ...\r\n");
			s_NetRunStep++;
			for ( i = 0; i < 49; i++)
			{
				nwy_gpio_value_set(REMOTE_LIGHT,(i%2));
				nwy_thread_sleep(100); 
			}
			break;
		case 8:
			ret = nwy_data_unreg_cb(simid, profile_id, nwy_data_cb_fun);
			nwy_ext_echo("\r\nRelease resource handle id, result<%d>\r\n", ret);
			api_WriteSysUNMsg(SYSUN_NETWORK_ABNORMAL);
			s_NetRunStep++;
			wConnectFalseNum ++;
			if( wConnectFalseNum >= 360 ) //0.5~2.5小时一直未拨号成功，复位（不同路径退出时间会有差异）
			{
				nwy_pm_ctrl(3);
			}
			break;
		default:
			s_NetRunStep = 0;
			break;
		}
	}
}