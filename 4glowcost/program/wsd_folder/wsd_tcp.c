//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include<sys/time.h>
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define PRIVATE_HEART_INTERVAL 				(60*2)					//维护端 心跳间隔
#define USER_HEART_INTERVAL 				(30)					//用户端 心跳间隔
#define USER_RESET_INTERVAL					(60*3)					//用户端 长时间无法连接 复位间隔// 约两小时
#define PORT     							 7197					//维护端  端口
#define PRIVATE_TCP_THREAD					0						//默认维护端tcp 线程号
#define TRANSPARENT_FLAG					0xAA					//透传基表标志 AA 标志代表与模块自身通信
#define CHECK_CONNECTED_NUM					10						//检测已连接次数
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
int HeartSeconds[TCP_TOTAL_NUM] = {0};
TSerial Serial_698;
#if (TCP_COMM_MODULE == YES)
static BYTE Tcp_ComModuleFlag = TRANSPARENT_FLAG;
#endif
const BYTE TcpPrivateIp[] = {"218.201.129.20"};//维护端固定IP
nwy_osi_thread_t tcp_recv_thread[TCP_TOTAL_NUM] = {NULL};
static int socket[TCP_TOTAL_NUM] = {0};

#if(MAX_USER_TRANS_NUM != 0) || (TCP_USER == YES)
static DWORD UserConnectfre[TCP_TOTAL_NUM] = {0};
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
#if (TCP_COMM_MODULE == YES)
void Tcp_Commodule_timer_cb(void *type)
{
	Tcp_ComModuleFlag = TRANSPARENT_FLAG;
}
#endif
void Tcp_send(int Isocket, char *sendbuff, int sendlen, BYTE i)
{
	int s = Isocket;
	int send_len = 0;
	
	send_len = nwy_socket_send(s, sendbuff, sendlen, 0);
	if (send_len != sendlen)
	{
		nwy_ext_echo("\r\nsend len=%d, return len=%d", strlen(sendbuff), send_len);
	}
	else
	{
		nwy_ext_echo("\r\nsend ok");
	}
	return;
}
//--------------------------------------------------
//功能描述:  上报不同格式登录帧
//         
//参数:      bChannel 连接通道数
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Send_Loginframe( BYTE bChannel ,tTranData *transdata)
{
	if (bChannel == PRIVATE_TCP_THREAD)//维护端发送698登录帧
	{
		Send_Heartbeatframe_698(transdata,0);
	}
	else
	{
		// Send_Heartbeatframe_698(transdata,0);
	}
}
//--------------------------------------------------
//功能描述:  判断 透传模式是否开启
//
//参数:      buff 接收数据
//         	S  套接字ID
//          num 对应线程ID
//返回值:
//
//备注:
//--------------------------------------------------
BOOL JudgeTransMode(BYTE *buff,int s,BYTE num)
{
	BYTE buf[] = {0xFE,0xFE,0xFE,0xFE,0x68,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,0x14,0x15,0x0E,0x12,0x12,0x37,0x35,
	0x33,0x33,0x33,0x37,0x36,0x35,0x34,0x34,0x35,0x12,0x0E,0xBB,0xBB,0xBB,0xBB,0x34,0xA9,0x16};//以全ff 645开编程 作为 开启基表透传命令
	char respond[] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x94,0x00,0x60,0x16};

	if (memcmp(buff,buf,sizeof(buf)) == 0)
	{
		Tcp_send(s, respond, sizeof(respond),num);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:   判断是否是心跳响应
//         
//参数:      
//         
//返回值:    
//         
//备注:   若主站是心跳响应，则不予理会 
//--------------------------------------------------
BOOL JudgeHeartLinkResponse( tTranData *tempTran )
{
	BYTE i;

	for (i = 0; i < tempTran->len; i++)
	{
		if (tempTran->buf[i] == 0x68)
		{
			if (tempTran->buf[i+14] == 0x81)
			{
				if (tempTran->buf[i+15] != 0x16)
				{
					return TRUE;
				}	
			}
		}
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  处理私有TCP服务器发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Deal_PrivateServer( tTranData *tempTranData )
{
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = Tcp_Commodule_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	#if (TCP_COMM_MODULE == YES)
	if (Tcp_ComModuleFlag != 0xAA)
	#endif
	{
		#if (TCP_COMM_MODULE == YES)
		nwy_sdk_timer_stop(Tcp_ComModule_timer);
		OneTimeTImerPara.expired_time = 60000;
		nwy_sdk_timer_start(Tcp_ComModule_timer, &OneTimeTImerPara);
		#endif
		tempTranData->TranType = eTRAN_TCP;

		if (QueueWrite(&MessageQueueInfoArr[eQUEUE_TRAN_TO_UART], tempTranData, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
		{
			nwy_ext_echo("\r\n tcp send messg success\r\n");
		}
		else
		{
			nwy_ext_echo("\r\n tcp send messg fail\r\n");
		}
	}
	#if (TCP_COMM_MODULE == YES)
	else
	{
		if (JudgeTransMode(tempTranData->buf,tempTranData->socketid,tempTranData->threadid))
		{
			Tcp_ComModuleFlag = 0;
		}
		else
		{
			if (JudgeHeartLinkResponse(tempTranData))
			{
				/* code */
			}
			else
			{
				if (QueueWrite(&MessageQueueInfoArr[eQUEUE_TCP_TO_MODULE], tempTranData, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
				{
					nwy_ext_echo("\r\n tcp send TO MODUEL success\r\n");
				}
				else
				{
					nwy_ext_echo("\r\n tcp send TO MODUEL fail\r\n");
				}
			}
			
		}
	}
	#endif
}
//--------------------------------------------------
//功能描述:  处理用户端TCP服务器发来的数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Deal_UserServer( tTranData *tempTranData ,BYTE bChannel)
{
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	Deal_UserServer_Ep212(tempTranData,api_GetTcpChannelTable(eMETER,bChannel-1));
	#else
	if (QueueWrite(&MessageQueueInfoArr[eQUEUE_TRAN_TO_UART], tempTranData, sizeof(tTranData), NWY_OSA_NO_SUSPEND) == TRUE)
	{
		nwy_ext_echo("\r\n user_tcp success recv\r\n");
	}
	#endif
}
static void nwy_tcp_recv_func(void *param)
{
	char recv_buff[NWY_UART_RECV_SINGLE_MAX + 1] = {0};
	int recv_len = 0, result = 0;
	BYTE bNum = (BYTE )param;
	BYTE s = socket[bNum];
	fd_set rd_fd;
	struct nwy_timeval_t tv = {0};
	tv.tv_sec = 1;//此处 会释放CPU 切换到别的线程
	tv.tv_usec = 0;
	tTranData tcptouart;
	nwy_ext_echo("\r\nrecv socketid is [%d]bNum is [%d]", s,bNum);

	while (1)
	{
		FD_ZERO(&rd_fd);
		FD_SET(s, &rd_fd);

		result = nwy_socket_select(s + 1, &rd_fd, NULL, NULL, &tv);
		if (result < 0)
		{
			break;
		}
		else if (result > 0)
		{
			if (FD_ISSET(s, &rd_fd))
			{
				memset(recv_buff, 0, NWY_UART_RECV_SINGLE_MAX + 1);
				recv_len = nwy_socket_recv(s, recv_buff, NWY_UART_RECV_SINGLE_MAX, 0);
				if (recv_len > 0)
				{
					memset(&tcptouart, 0, sizeof(tcptouart));
					memcpy(&tcptouart, recv_buff, sizeof(recv_buff));
					tcptouart.len = recv_len;
					tcptouart.socketid = s;
					tcptouart.threadid = bNum;
					HeartSeconds[bNum] = 0;
					if (bNum == PRIVATE_TCP_THREAD)//维护端服务器 固定一个
					{
						nwy_ext_echo("\r\n private tcp socket is[%d]thread is[%d]\r\n",s,bNum);
						Deal_PrivateServer(&tcptouart);
					}
					else
					{
						Deal_UserServer(&tcptouart,bNum);
					}
				}
				else
				{
					if (recv_len == 0)
					{
						nwy_ext_echo("\r\ntcp id[%d] close by server\r\n",bNum);		
					}
					else
					{
						nwy_ext_echo("\r\ntcp id[%d] connection error\r\n",bNum);
					}
					break;
				}
			}
			else
			{
				//不知道这个算不算异常路径
				break;
			}
		}
	}
	nwy_socket_close(s);
	// nwy_sleep(60000); 		//等待60s
	socket[bNum] = 0;
	nwy_thread_exit(tcp_recv_thread[bNum]);
}

static int nwy_hostname_check(char *hostname)
{
	int a, b, c, d;
	char temp[32] = {0};
	if(strlen(hostname) > 15)
		return NWY_GEN_E_UNKNOWN;

	if((sscanf(hostname, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
		return NWY_GEN_E_UNKNOWN;

	if(!((a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0)))
		return NWY_GEN_E_UNKNOWN;

	sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
	// memset(hostname, 0, sizeof(hostname));
	strcpy(hostname, temp);
	return NWY_SUCCESS;
}
//--------------------------------------------------
//功能描述:  connect 函数直接挪用N58
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void Tcp_connet(tTranData *transdata,BYTE *bNum)
{
	char ip_buf[256] = {0};
	int  ret = 0;
	uint64_t start;
	ip_addr_t addr = {0};
	BYTE bChannel = *bNum;
	BYTE bSocket = 0;
	BYTE bConnected = 0;

	struct sockaddr_in sa_v4;

	if(socket[bChannel] != 0)
	{
		nwy_ext_echo("\r\ntcp alreay connect");
		return;
	}
	memset(ip_buf, 0, sizeof(ip_buf));

	if(bChannel == PRIVATE_TCP_THREAD)// 连接的第一个维护端主站 ip和端口固定
	{
		memcpy(ip_buf,TcpPrivateIp, sizeof(TcpPrivateIp));
	}
	else
	{
		memcpy(ip_buf, Serverpara[bChannel-1].ServerIP, sizeof(Serverpara[bChannel-1].ServerIP));
		nwy_ext_echo("\r\nserver bNUM[%d]  ip %s",bChannel-1, Serverpara[bChannel-1].ServerIP);
	}
	ret = nwy_hostname_check(ip_buf);
	if(ret != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\ninput ip or url is invalid");
		return;
	}
	// 转换IP字符串为网络地址
	if (nwy_socket_inet_pton(AF_INET, ip_buf, &addr.u_addr.ip4) != NWY_SUCCESS)
	{
		nwy_ext_echo("\r\ninput ip or url is invalid");
		return;
	}

	inet_addr_from_ip4addr(&sa_v4.sin_addr, ip_2_ip4(&addr));
	sa_v4.sin_len = sizeof(struct sockaddr_in);
	sa_v4.sin_family = AF_INET;
	if (bChannel == 0)
	{
		sa_v4.sin_port = htons(PORT);
	}
	else
	{
		sa_v4.sin_port = htons(Serverpara[bChannel-1].ServerPort);
	}
	bSocket = nwy_socket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP, NWY_DEFAULT_NETIF_ID);//！！！ 不确认默认cid 有什么作用

	nwy_ext_echo("\r\ntcp open socket is[%d] num is [%d]\r\n",bSocket,bChannel);
	
	if (bSocket <= 0)
	{
		nwy_ext_echo("\r\n nwy_socket_open error");
		return;
	}

	// 设置socket属性
	if ((NWY_SUCCESS != nwy_socket_set_reuseaddr(bSocket)) || (NWY_SUCCESS != nwy_socket_set_nodelay(bSocket)))
	{
		nwy_ext_echo("\r\nsocket set err");
		ret = nwy_socket_close(bSocket);
		if (ret != NWY_SUCCESS)
		{
			nwy_ext_echo("\r\nSocket close fail");
		}
		bSocket = 0;
		return;
	}

	start = nwy_uptime_get();

	while (1)
	{
		ret = nwy_socket_connect(bSocket, (struct sockaddr *)&sa_v4, sizeof(sa_v4));
		
		if (ret == NWY_SUCCESS)
		{
		}
		if((nwy_uptime_get() - start) >= 9000)
		{
			nwy_ext_echo("\r\nsocket connect timeout\r\n");
			nwy_ext_echo("tcp socket status is %d",nwy_socket_get_state(bSocket));
			nwy_socket_shutdown(bSocket, SHUT_RD);
			nwy_socket_close(bSocket);
			bSocket = 0;
			break;
		}
		if(ret == -501)
		{
			if(NWY_ESTABLISHED == nwy_socket_get_state(bSocket))
			{
				bConnected++;
				if (bConnected == CHECK_CONNECTED_NUM)
				{
					nwy_ext_echo("\r\nnwy_net_connect_tcp connect ok..");
					transdata->socketid = bSocket;
					transdata->threadid = bChannel;
					socket[bChannel] = bSocket;
					Send_Loginframe(bChannel,transdata);
					break;
				}		
			}
			else
			{
				bConnected = 0;
			}
		}
		nwy_thread_sleep(100);
	}	
	if(bSocket != 0)
	{
		{
			ret = nwy_thread_create(&tcp_recv_thread[bChannel], "tcp_recv_thread", NWY_OSI_PRIORITY_NORMAL, nwy_tcp_recv_func, (void*)bChannel, 8, 1024 * 10, NULL);
			if ((ret != NWY_SUCCESS) || (tcp_recv_thread[bChannel] == NULL))
			{
				nwy_ext_echo("\r\ncreate tcp recv thread failed, close connect,ret:%d", ret);
				nwy_socket_close(bSocket);
				bSocket = 0;
			}

		}
	}

	return;
}
//--------------------------------------------------
//功能描述:  厂内维护端tcp 主站 用于远程维护模块和透传基表数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void TCP_PrivateTask(void *param)
{
	nwy_thread_sleep(600);
	tTranData UploadTranData;
	BYTE bServerNum = 0;
	bServerNum = (BYTE )param;

	BYTE *UploadTranDataPtr = NULL;
	nwy_ext_echo("\r\nTCP_PrivateTask,bServerNum:%d", bServerNum);
	while(1)
	{
		if(0 != nwy_ext_check_data_connect())
		{
			if (socket[bServerNum] == 0)
			{
				Tcp_connet(&UploadTranData, &bServerNum);
				if (socket[bServerNum] == 0) //连接失败时，每分钟执行一次
				{
					#if(MAX_USER_TRANS_NUM != 0)
					if (bServerNum !=PRIVATE_TCP_THREAD)
					{
						UserConnectfre[bServerNum]++;
						nwy_ext_echo("\r\n reconnect is [%d][%d]",bServerNum,UserConnectfre[bServerNum]);
						if (UserConnectfre[bServerNum] > USER_RESET_INTERVAL)
						{
							nwy_ext_echo("\r\n reset!!!");
							api_WriteSysUNMsg(TCP_DISCONNECT_RESET);
							nwy_pm_ctrl(3);
						}
					}
					#endif
				nwy_thread_sleep(60000); 		//等待60s
				}
			}
			else
			{
				#if(MAX_USER_TRANS_NUM != 0)
				if (bServerNum !=PRIVATE_TCP_THREAD)
				{
					UserConnectfre[bServerNum] = 0;
				}
				#endif
				HeartSeconds[bServerNum]++;
				if (HeartSeconds[bServerNum] == PRIVATE_HEART_INTERVAL)
				{
					HeartSeconds[bServerNum] = 0;
					Send_Heartbeatframe_698(&UploadTranData, 1);
				}
				if (nwy_msg_queue_recv(UartDataToTcpMsgQue, sizeof(char *), &UploadTranDataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
				{
					//test
					memcpy(&UploadTranData, UploadTranDataPtr, sizeof(tTranData));
					nwy_ext_echo("\r\n UploadTranData socket is [%d],thread is [%d]", UploadTranData.socketid, UploadTranData.threadid);
					HeartSeconds[UploadTranData.threadid] = 0;
					Tcp_send(UploadTranData.socketid, (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);
				}
				#if (TCP_COMM_MODULE == YES)
				if (nwy_msg_queue_recv(ModuleDataToTcpMsgQue, sizeof(char *), &UploadTranDataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
				{
					memcpy(&UploadTranData, UploadTranDataPtr, sizeof(tTranData));
					HeartSeconds[UploadTranData.threadid]  = 0;
					Tcp_send(UploadTranData.socketid, (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);
				}
				#endif
			}
		}
		// nwy_ext_echo("\r\nstart run");
		nwy_thread_sleep(1000);
	}
}
#if(TCP_USER == YES)
//--------------------------------------------------
//功能描述:  连接 用户主站
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void TCP_User_Task(void *param)
{
	BYTE bServerNum;
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_104)
	CollectionDatas CollectData;
	#endif
	bServerNum = (BYTE )param;
	tTranData UploadTranData;
	BYTE *UploadTranDataPtr = NULL;

	nwy_ext_echo("\r\n tcp user is %d",bServerNum);
	while (1)
	{
		if(0 != nwy_ext_check_data_connect())
		{
			if(socket[bServerNum] == 0)
			{
				Tcp_connet(&UploadTranData,&bServerNum);
				if(socket[bServerNum] == 0) 	//连接失败时，每分钟执行一次
				{
					UserConnectfre[bServerNum]++;
					nwy_thread_sleep(60000); 		//等待60s
					if (UserConnectfre[bServerNum] > USER_RESET_INTERVAL)
					{
						nwy_ext_echo("\r\n reset!!!");
						nwy_pm_ctrl(3);
					}
				}
			}
			else
			{
				UserConnectfre[bServerNum] = 0;
				#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_104)
				if(nwy_msg_queue_recv(CollectMessageQueue, sizeof(char *), &UploadTranDataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)	// 其他基于tcp 的上报
				{
					#if( CYCLE_METER_READING ==  YES)
					{
						nwy_ext_echo("TCP 104 CYCLE REPORT");
						memcpy(&UploadTranData, UploadTranDataPtr, sizeof(tTranData));
						CycleReportData_104(&CollectData.CollectMeter[0][0],socket[bServerNum],bServerNum);
					}
					#endif
				}
				#elif(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
				if(nwy_msg_queue_recv(UartToEp212MsgQue, sizeof(char *), &UploadTranDataPtr, NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
				{
					memcpy(&UploadTranData, UploadTranDataPtr, sizeof(tTranData));
					Tcp_send(socket[UploadTranData.threadid], (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);
					nwy_ext_echo("\r\n UartToEp212MsgQue success");
				}
				#endif
			}
		}
		nwy_thread_sleep(1000);
	}
}
#endif
//--------------------------------------------------
//功能描述:  检测主站连接状态
//         
//参数:      bNum 连接tcp主站序号  0 维护端  1——其他 暂定
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_GetTcpConStatus( BYTE bNum )
{
	if (socket[bNum] != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
