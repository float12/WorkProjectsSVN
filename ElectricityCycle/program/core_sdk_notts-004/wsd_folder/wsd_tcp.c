//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建人	王泉
// 创建日期
// 描述
// 修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include <sys/time.h>
#include "wsd_uart.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define USER_HEART_INTERVAL			12000// 用户端 心跳四分钟间隔 延时5ms 延时不准确   每次20ms 触发		Sleep只能粗略延时
#define PRIVATE_HEART_INTERVAL 		(60*5)//私有端 心跳间隔
#define RESEND_CNT_MAX 				10	// 重发次数最大值
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum TcpState_e
{
	eTCP_DISCONNECTED,
	eTCP_CONNECTING,
	eTCP_CONNECTED,
	eTCP_GET_DATA_FROM_QUEUE, // 3
	eTCP_SEND_DATA_TO_SERVER,
	eTCP_LOGIN_WAIT,
} TcpState;

typedef struct
{
	BYTE WaveDataTmpBuf[SEND_WAVE_DATA_LEN * TCP_RESEND_QUEUE_SIZE];//和TCPResendTmpMsgQue队列大小一致
	BYTE WaveDataTmpBufLen;
} TWaveDataTmpBuf;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
int tcp_connect_flag[2] = {0};
int socket[2] = {0};
nwy_osiThread_t *tcp_recv_thread[2] = {NULL};

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
TWaveDataTmpBuf WaveDataTmpBuf;//重发成功后清零结构体
WaveDatatoTcp WaveDataToServer;
int HeartSeconds = 0;
static BYTE Tcp_ComModuleFlag = 0xAA;
static BYTE Tcp_PowerOnFlag = 0xAA;
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
void Tcp_Commodule_timer_cb(void *type)
{
	Tcp_ComModuleFlag = 0xAA;
}
BYTE Tcp_send(BYTE SocketNum, char *sendbuff, int sendlen)
{
	int s = socket[SocketNum];
	int send_len = 0;

	if (!tcp_connect_flag[SocketNum])
	{
		nwy_ext_echo("\r\ntcp not setup");
		return FALSE;
	}
	send_len = nwy_socket_send(s, sendbuff, sendlen, 0);
	if (send_len != sendlen)
	{
		nwy_ext_echo("\r\ntcp send err  send len=%d, return len=%d", sendlen, send_len);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
void ParseRec698(char *buff, WORD recv_len)
{
	BYTE  btemp;

	if((buff[17] == 0xee)&&(buff[18] == 0xee))
	{
		if (buff[23] == 1)
		{
			btemp = 1;
			if (nwy_put_msg_que(RecvTcpUserServerMsgQue, &btemp, 0xffffffff) == TRUE)
			{
				nwy_ext_echo("\r\nrecv request success");
			} 
		}
	}
}
//判断 透传模式是否开启
BOOL JudgeTransMode(char *buff)
{
	char buf[] = {0xFE,0xFE,0xFE,0xFE,0x68,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,0x14,0x15,0x0E,0x12,0x12,0x37,0x35,
	0x33,0x33,0x33,0x37,0x36,0x35,0x34,0x34,0x35,0x12,0x0E,0xBB,0xBB,0xBB,0xBB,0x34,0xA9,0x16};//以全ff 645开编程 作为 开启基表透传命令
	char respond[] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x94,0x00,0x60,0x16};

	if (memcmp(buff,buf,sizeof(buf)) == 0)
	{
		Tcp_send(1, respond, sizeof(respond));
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL JudgeLtoAdrr_698(char *buff)
{
	BYTE bAddrLen = 0;
	BYTE i ;

	bAddrLen = (buff[0]&0x0f)+1;

	
	if ((bAddrLen == 6)||(bAddrLen == 7))
	{
		for (i = (bAddrLen -6); i < bAddrLen; i++)
		{
			if (buff[i+1] != 0xAA)
			{
				if (buff[i+1] != bLtoAddr[bAddrLen-1-i])
				{
					return FALSE;			
				}
			}
			nwy_ext_echo("\r\njudge is%x",buff[i+1]);
		}
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

static void nwy_tcp_recv_func(void *param)
{
	char recv_buff[NWY_UART_RECV_SINGLE_MAX + 1] = {0};
	int recv_len = 0, result = 0;
	BYTE bNum = *(BYTE *)param;
	int s = socket[bNum];
	fd_set rd_fd;
	fd_set ex_fd;
	FD_ZERO(&rd_fd);
	FD_ZERO(&ex_fd);
	FD_SET(s, &rd_fd);
	FD_SET(s, &ex_fd);
	struct timeval tv = {0};
	tv.tv_sec = 20;
	tv.tv_usec = 0;
	tTranData tcptouart;
	while (1)
	{
		FD_ZERO(&rd_fd);
		FD_ZERO(&ex_fd);
		FD_SET(s, &rd_fd);
		FD_SET(s, &ex_fd);
		result = nwy_socket_select(s + 1, &rd_fd, NULL, &ex_fd, &tv);
		// nwy_ext_echo("\r\nlisten is ing\r\n");
		if (result < 0)
		{
			nwy_ext_echo("\r\nstarttask2\r\n");
			nwy_socket_close(s);
			socket[bNum] = 0;
			// nwy_sleep(60000); 
			tcp_connect_flag[bNum] = 0;
			nwy_exit_thread();
		}
		else if (result > 0)
		{
			nwy_ext_echo("\r\nsocket select > 0 \r\n");
			if (FD_ISSET(s, &rd_fd))
			{
				memset(recv_buff, 0, NWY_UART_RECV_SINGLE_MAX + 1);
				recv_len = nwy_socket_recv(s, recv_buff, NWY_UART_RECV_SINGLE_MAX, 0);
				if (recv_len > 0)
				{
					// nwy_ext_echo("\r\nbThread is %d,s is %d\r\n",bNum,s);
					// for (int i = 0; i < recv_len; i++)
					// {
					// 	nwy_ext_echo("%02X ", recv_buff[i]);
					// }
					HeartSeconds = 0;
					if (bNum == 0)		//录波主站 只判断请求帧的回复
					{
						ParseRec698(recv_buff, recv_len);
					}
					else					//维护主站
					{
						memset(&tcptouart, 0, sizeof(tcptouart));
						memcpy(&tcptouart, recv_buff, sizeof(tcptouart.buf));
						tcptouart.len = recv_len;
						
						if (Tcp_ComModuleFlag != 0xAA)
						{	
							nwy_stop_timer(Tcp_ComModule_timer);
							nwy_start_timer(Tcp_ComModule_timer, 60000);
							if(JudgeLtoAdrr_698(&recv_buff[4]) == TRUE)
							{
								tcptouart.Uart = eUART_LTO;			//透传物联网表
							}
							else
							{
								tcptouart.Uart = eUART_WHWAVE;		//透传录波表
							}
							tcptouart.TranType = SERVER_TRAN;
							if(nwy_put_msg_que(TranDataToUartMessageQueue, &tcptouart, 0xffffffff))
							{
								nwy_ext_echo("\r\n tcp send messg success\r\n");
							}
						}
						else
						{
							if (JudgeTransMode(recv_buff))
							{
								Tcp_ComModuleFlag = 0;
							}
							else
							{
								if(nwy_put_msg_que(TcpDataToModuleMsgQue, &tcptouart, 0xffffffff))
								{
									nwy_ext_echo("\r\n tcp send TO MODUEL success\r\n");
								}
							}											
						}
					}
				}
				else if (recv_len == 0)
				{
					nwy_ext_echo("\r\ntcp closed by server\r\n");
					nwy_socket_close(s);
					socket[bNum] = 0;
					// nwy_sleep(60000); 		
					tcp_connect_flag[bNum] = 0;
					nwy_exit_thread();
				}
				else
				{
					nwy_ext_echo("\r\ntcp connection error,err code:%d\r\n", recv_len);
					nwy_socket_close(s);
					socket[bNum] = 0;
					// nwy_sleep(60000);
					tcp_connect_flag[bNum] = 0;
					nwy_exit_thread();
				}
			}
			if (FD_ISSET(s, &ex_fd))
			{
				nwy_ext_echo("\r\ntcp select ex_fd:\r\n");
				nwy_socket_close(s);
				socket[bNum] = 0;
				// nwy_sleep(60000); 
				tcp_connect_flag[bNum] = 0;
				nwy_exit_thread();
			}
		}
		else
			OSI_LOGI(0, "\r\ntcp select timeout:\r\n");

		nwy_sleep(1000);
	}
}

static int nwy_hostname_check(char *hostname)
{
	int a, b, c, d;
	char temp[32] = {0};
	if (strlen(hostname) > 15)
		return NWY_GEN_E_UNKNOWN;

	if ((sscanf(hostname, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
		return NWY_GEN_E_UNKNOWN;

	if (!((a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0)))
		return NWY_GEN_E_UNKNOWN;

	sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
	// memset(hostname, 0, sizeof(hostname));
	strcpy(hostname, temp);
	return NWY_SUCESS;
}

void Tcp_connet(BYTE *bNum)
{
	char ip_buf[256] = {0};
	int af_inet_flag = AF_INET;
	int on = 1;
	int opt = 1, ret = 0;
	uint64_t start;
	ip_addr_t addr = {0};

	struct sockaddr_in sa_v4;
	BYTE Channel = *bNum;

	if(tcp_connect_flag[Channel])
	{
		nwy_ext_echo("\r\ntcp already connect");
		return;
	}
	memset(ip_buf, 0, sizeof(ip_buf));

	memcpy(ip_buf, Serverpara[Channel].ServerIP, sizeof(Serverpara[Channel].ServerIP));

	ret = nwy_hostname_check(ip_buf);
	if (ret != NWY_SUCESS)
	{
		nwy_ext_echo("\r\ninput ip or url is invalid");
		return;
	}
	if (ip4addr_aton(ip_buf, &addr.u_addr.ip4) == 0)
	{
		nwy_ext_echo("\r\ninput ip error:\r\n");
		return;
	}
	inet_addr_from_ip4addr(&sa_v4.sin_addr, ip_2_ip4(&addr));
	sa_v4.sin_len = sizeof(struct sockaddr_in);
	sa_v4.sin_family = AF_INET;
	sa_v4.sin_port = htons(Serverpara[Channel].ServerPort);
	af_inet_flag = AF_INET;

	if(socket[Channel] == 0)
	{
		socket[Channel] = nwy_socket_open(af_inet_flag, SOCK_STREAM, IPPROTO_TCP);
		{
			if (Tcp_PowerOnFlag != 0xAA)
			{
				if (socket[Channel] == 4)
				{
					api_WriteSysUNMsg(TCP_NOFREE_SOCKET);
				}
			}
			else
			{
				Tcp_PowerOnFlag = 0;
			}
		}
		nwy_ext_echo("\r\n tcp socketid  is [%d][%d]",socket[Channel],Channel);
	}
	nwy_socket_setsockopt(socket[Channel], SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));

	nwy_socket_setsockopt(socket[Channel], IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
	if(0 != nwy_socket_set_nonblock(socket[Channel]))
	{
		nwy_ext_echo("\r\nsocket set err\r\n");
		return;
	}
	start = nwy_get_ms();
	do
	{
		ret = nwy_socket_connect(socket[Channel], (struct sockaddr *)&sa_v4, sizeof(sa_v4));

		if (ret)
		{
			// nwy_ext_echo("\r\nsocket first connect ok\r\n");
		}
		if ((nwy_get_ms() - start) >= 9000)
		{
			nwy_ext_echo("\r\nsocket connect timeout Channel is %d\r\n",Channel);
			nwy_socket_shutdown(socket[Channel], SHUT_RD);
			nwy_socket_close(socket[Channel]);
			socket[Channel] = 0;
			break;
		}
		if (ret == -1)
		{
			// nwy_ext_echo("errno=%d\r\n", nwy_socket_errno());
			if (EISCONN == nwy_socket_errno())
			{
				nwy_ext_echo("\r\nnwy_net_connect_tcp connect ok..,Num:%d",Channel);
				tcp_connect_flag[Channel] = 1;
				memset(ip_buf,0,sizeof(ip_buf));
				Compose_Heartbeatframe_698((BYTE*)&ip_buf, 0);
				Tcp_send(Channel, (char*)&ip_buf, 60);
				break;
			}
			if (EINPROGRESS != nwy_socket_errno() && EALREADY != nwy_socket_errno())
			{
				nwy_ext_echo("\r\nconnect errno = %d", nwy_socket_errno());
				nwy_ext_echo("\r\nsocket is %d",socket[Channel]);
				nwy_ext_echo("\r\ni is %d",Channel);
				nwy_socket_close(socket[Channel]);
				socket[Channel] = 0;
				tcp_connect_flag[Channel] = 0;
				break;
			}
		}
		nwy_sleep(100);
	} while(1);
	if(tcp_connect_flag[Channel])
	{
		{
			tcp_recv_thread[Channel] = nwy_create_thread("tcp_recv_thread", nwy_tcp_recv_func, (void *)bNum, NWY_OSI_PRIORITY_NORMAL, 1024 * 20, 8);//感觉这块传入的参数 必须为全局变量？
			if(tcp_recv_thread[Channel] == NULL)
			{
				nwy_ext_echo("\r\ncreate tcp recv thread failed, close connect");
				nwy_socket_close(socket[Channel]);
				socket[Channel] = 0;
				tcp_connect_flag[Channel] = 0;
			}
		}
	}
	return;
}

//-----------------------------------------------
// 函数功能:发送波形数??
//
// 参数:
//
// 返回??: true 发送成功，false 发送失??
//
// 备注:
//-----------------------------------------------
BOOL TCPSendWaveData(void)
{
	BYTE i = 0;
	for (i = 0; i < 3; i++)
	{
		if (Tcp_send(0, (char *)&WaveDataToServer, SEND_WAVE_DATA_LEN) == TRUE)
		{
			return TRUE;
		}
		else
		{
			nwy_ext_echo("\r\n tcp send err,retry:%d", i + 1);
			nwy_sleep(1000); // 延时1秒后重试
		}
	}
	// if (i == 3)
	{
		nwy_ext_echo("\r\n tcp 3 times send err ");
		return FALSE;
	}
}
//--------------------------------------------------
// 功能描述:  读取所有缓存数据到WaveDataTmpBuf
//           
// 参数:      无
//           
// 返回值:    无
//           
// 备注:      
//--------------------------------------------------
void ReadAllWaveDataFromQueue(void)
{
    DWORD cnt = 0;
    WORD i = 0;

	if(WaveDataTmpBuf.WaveDataTmpBufLen > 0)
	{
		nwy_ext_echo("\r\n wave data tmp buf len is not 0");
		return;
	}
	memset(&WaveDataTmpBuf, 0, sizeof(WaveDataTmpBuf));
	cnt = nwy_get_queue_pendingevent_cnt(TCPResendTmpMsgQue);
	nwy_ext_echo("\r\n resend tmp msg que cnt is %d", cnt);
    for(i = 0; i < cnt; i++)
    {
        if(nwy_get_msg_que(TCPResendTmpMsgQue, &WaveDataTmpBuf.WaveDataTmpBuf[i * SEND_WAVE_DATA_LEN], 0) == TRUE)
        {
            WaveDataTmpBuf.WaveDataTmpBufLen += 1;
			// nwy_ext_echo("\r\n read wave data len is %d", WaveDataTmpBuf.WaveDataTmpBufLen);
        }
        else
        {
			nwy_ext_echo("\r\n  read all wave data from queue err");
            break;
        }
    }
}

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
	nwy_sleep(15000);
	tTranData UploadTranData;
	WORD wHeart = 0;
	TcpState tcpState = eTCP_DISCONNECTED;
	BYTE ReconAndReSendFlag = 0; // 重连后重发波形数
	BYTE Result = 0;
	static BYTE bLoop = 0;
	static DWORD SendCnt = 0;
	static BYTE ResendCnt = 0;
	BYTE LogInBuf[60] = {0};
	DWORD Connect = 0;
	BYTE tmp_buf[SEND_WAVE_DATA_LEN],bchannel = 0;
	WORD i = 0;
	BYTE TCPResendErrFlag = 0;
	memset(tmp_buf,0,sizeof(tmp_buf));
	while (1)
	{
		ThreadRunCnt[eTcpThread]++;
		nwy_sleep(65);
		// nwy_ext_echo("\r\ntcp task state:%d", tcpState);
		if ((tcp_connect_flag[0] == 1))
		{
			if (nwy_get_msg_que(TranDataToTcpUserServerMsgQue, &UploadTranData, 0xffffffff))
			{
				wHeart = 0;
				if(Tcp_send(0, (char *)&UploadTranData.buf[0], UploadTranData.len) == TRUE)
				{
					nwy_ext_echo("\r\ntcp send freeze suc ");
				}
			}
		}
		switch (tcpState)
		{
		case eTCP_DISCONNECTED:
			if (0 != nwy_ext_check_data_connect())
			{
				tcpState = eTCP_CONNECTING;
			}
			else
			{
				nwy_sleep(5000);
			}
			nwy_gpio_set_value(ONLINE,0);
			break;
		case eTCP_CONNECTING:
			if ((tcp_connect_flag[0] == 0)||(0 == nwy_ext_check_data_connect()))
			{ 
				Tcp_connet(&bchannel);
				if (tcp_connect_flag[0] == 0)
				{
					Connect++;
					nwy_ext_echo("\r\nconnect is %d",Connect);
					if (Connect > 480)//约两小时连接不上 模块复位一次
					{
						ResetFlag = 1;
					}
					nwy_sleep(5000);
				}
				else
				{
					Connect = 0;
				}
				tcpState = eTCP_DISCONNECTED;
			}
			else
			{
				Compose_Heartbeatframe_698(LogInBuf, 1);
				if (Tcp_send(0, (char*)&LogInBuf, sizeof(LogInBuf)) == TRUE)
				{
					nwy_ext_echo("\r\nsend heartbeat frame suc");
					tcpState = eTCP_CONNECTED;
					nwy_sleep(1000);
				}
			}
			break;
		case eTCP_LOGIN_WAIT:
			if ((tcp_connect_flag[0] == 0) || (0 == nwy_ext_check_data_connect()))
			{
				tcpState = eTCP_DISCONNECTED;
				Result = 0;
				nwy_put_msg_que(TcpUserServertoTFMsgQue, &Result, 0xffffffff);
			}
			else
			{
				if (nwy_get_msg_que(RecvTcpUserServerMsgQue, &Result, 5000))
				{
					if (Result == 1)
					{
						nwy_ext_echo("\r\nrequset suc\r\n");

						nwy_put_msg_que(TcpUserServertoTFMsgQue, &Result, 0xffffffff);
						tcpState = eTCP_CONNECTED;
					}
					else
					{
						nwy_ext_echo("\r\nrequset fail\r\n");
						nwy_put_msg_que(TcpUserServertoTFMsgQue, &Result, 0xffffffff);
						tcpState = eTCP_CONNECTED;
						//主站资源不足
					}
				}
				else
				{
					nwy_ext_echo("\r\nrequset response queue time out,request again");
					nwy_sleep(20000);
					memset(&LogInBuf[0],0,sizeof(LogInBuf));
					Compose_SendRequestfactor_698(UK[bLoop-1],IK[bLoop-1],LogInBuf,bLoop);
					if (Tcp_send(0, (char*)&LogInBuf, sizeof(LogInBuf)) == TRUE)
					{
						tcpState = eTCP_LOGIN_WAIT;
					}
					else
					{
						api_WriteSysUNMsg(SEND_REQUEST_FRAME_ERR);
						nwy_ext_echo("\r\nrequset send err\r\n");
					}
				}
			}
			break;
		case eTCP_CONNECTED: 
			if ((tcp_connect_flag[0] == 0) || (0 == nwy_ext_check_data_connect()))
			{
				tcpState = eTCP_DISCONNECTED;
			}
			else
			{
				if (ReconAndReSendFlag == 1)
				{
					tcpState = eTCP_SEND_DATA_TO_SERVER;
				}
				else
				{
					tcpState = eTCP_GET_DATA_FROM_QUEUE;
					nwy_gpio_set_value(ONLINE, 1);
				}
			}
			break;
		case eTCP_GET_DATA_FROM_QUEUE:
			if (tcp_connect_flag[0] == 0)
			{
				tcpState = eTCP_DISCONNECTED;
				nwy_ext_echo("\r\n get data tcp disconn");
				ReconAndReSendFlag = 1;
				ReadAllWaveDataFromQueue();
			}
			else
			{
				if (nwy_get_msg_que(WaveDataUploadMessageQueue, &WaveDataToServer, 0xffffffff) == TRUE)
				{
					if (WaveDataToServer.type == 1) // 请求传输
					{
						//传输新数据前先清空波形缓存队列
						memset(&WaveDataTmpBuf, 0, sizeof(WaveDataTmpBuf));
						ReadAllWaveDataFromQueue();
						memset(&WaveDataTmpBuf, 0, sizeof(WaveDataTmpBuf));
						nwy_ext_echo("\r\n tcp get request from queue");
						Result = 1;
						SendCnt = 0;
						memset(&LogInBuf[0],0,sizeof(LogInBuf));
						bLoop = WaveDataToServer.LoopNum; //查找 系数表
						Compose_SendRequestfactor_698(UK[bLoop-1],IK[bLoop-1],LogInBuf,bLoop);
						if (Tcp_send(0, (char*)&LogInBuf, sizeof(LogInBuf)) == TRUE)
						{
							tcpState = eTCP_LOGIN_WAIT;
						}
						else
						{
							api_WriteSysUNMsg(SEND_REQUEST_FRAME_ERR);
							nwy_ext_echo("\r\nrequset send err\r\n");
						}
					}
					else if (WaveDataToServer.type == 0)
					{
						tcpState = eTCP_SEND_DATA_TO_SERVER;
						wHeart = 0;
						//保存最近的480ms波形
						if (nwy_get_queue_spaceevent_cnt(TCPResendTmpMsgQue) > 0)
						{
							if(nwy_put_msg_que(TCPResendTmpMsgQue, &WaveDataToServer, 0xffffffff) == TRUE)
							{
								// nwy_ext_echo("\r\n tcp put resendmsg que suc");
							}
							else
							{
								nwy_ext_echo("\r\n tcp put resendmsg que err");
							}
						}
						else
						{
							if (nwy_get_msg_que(TCPResendTmpMsgQue,tmp_buf,0xffffffff))
							{
								// nwy_ext_echo("\r\n tcp get resendmsg que suc");
							}
							else
							{
								nwy_ext_echo("\r\n tcp get resendmsg que err");
							}
							if(nwy_put_msg_que(TCPResendTmpMsgQue, &WaveDataToServer, 0xffffffff) == TRUE)
							{
								// nwy_ext_echo("\r\n tcp put resendmsg que suc");
							}
							else
							{
								nwy_ext_echo("\r\n tcp put resendmsg que err");
							}
						}
					}
				}
				else
				{
					wHeart++;
					if(wHeart == USER_HEART_INTERVAL) //发送心跳
					{
						wHeart = 0;
						memset(&LogInBuf[0],0,sizeof(LogInBuf));
						Compose_Heartbeatframe_698(LogInBuf, 1);
						Tcp_send(0, (char*)&LogInBuf, sizeof(LogInBuf));
					}
				}
			}
			break;
		case eTCP_SEND_DATA_TO_SERVER:
			if (tcp_connect_flag[0] == 0)
			{
				tcpState = eTCP_DISCONNECTED;
				nwy_ext_echo("\r\n send data tcp disconn");
				ReconAndReSendFlag = 1;
				ReadAllWaveDataFromQueue();
			}
			else
			{
				if(ReconAndReSendFlag == 1)//先发送缓存波形
				{
					if(WaveDataTmpBuf.WaveDataTmpBufLen > 0)
					{
						nwy_ext_echo("\r\n wave data tmp buf len:%d", WaveDataTmpBuf.WaveDataTmpBufLen);
						for(i = 0; i < WaveDataTmpBuf.WaveDataTmpBufLen; i++)
						{
							memcpy(&WaveDataToServer, &WaveDataTmpBuf.WaveDataTmpBuf[i * SEND_WAVE_DATA_LEN], SEND_WAVE_DATA_LEN);
							if(TCPSendWaveData() == FALSE)
							{
								nwy_ext_echo("\r\n tcp reSend wave data err，check tcp connect");
								api_WriteSysUNMsg(RECON_SEND_DATA_ERR);
								tcpState = eTCP_DISCONNECTED;
								TCPResendErrFlag = 1;
								break;
							}
							else
							{
								TCPResendErrFlag = 0;
							}
						}
						//退出case
						if(TCPResendErrFlag == 1)
						{
							break;
						}
						//可能重发过程断开连接，等待20秒确定没有断开连接认为重发成功，否则重连后继续重发
						nwy_sleep(20000);
						if(tcp_connect_flag[0] == 0)
						{
							ResendCnt++;
							if(ResendCnt > RESEND_CNT_MAX)
							{
								ResetFlag = 1;
							}
							nwy_ext_echo("\r\n tcp connect flag is 0,resend  data again");
							tcpState = eTCP_DISCONNECTED;
							break;
						}
						else
						{
							ResendCnt = 0;
							nwy_ext_echo("\r\n resend all data success");
							memset(&WaveDataTmpBuf, 0, sizeof(WaveDataTmpBuf));
							ReconAndReSendFlag = 0;
							tcpState = eTCP_GET_DATA_FROM_QUEUE;
						}
					}
					else
					{
						nwy_ext_echo("\r\n tmp buf len is 0");
						ReconAndReSendFlag = 0;
						memset(&WaveDataTmpBuf, 0, sizeof(WaveDataTmpBuf));
						tcpState = eTCP_GET_DATA_FROM_QUEUE;
					}
				}
				else
				{
#if (UPLOAD_WAVE_DATA_SWITCH == YES)
					if (TCPSendWaveData() == TRUE)
					{
						SendCnt++;
						if (SendCnt % 1000 == 0)
						{
							nwy_ext_echo("\r\ntcp send wave data suc,SendCnt:%d", SendCnt);
						}
						tcpState = eTCP_GET_DATA_FROM_QUEUE;
					}
					else
					{
						nwy_ext_echo("\r\n TCPSendWaveData  err");
						if ((tcp_connect_flag[0] == 0) || (0 == nwy_ext_check_data_connect()))
						{
							nwy_ext_echo("\r\n tcp send err and tcp is disconnected,reconnect");
							tcpState = eTCP_DISCONNECTED;
							ReconAndReSendFlag = 1;
							ReadAllWaveDataFromQueue();
						}
						else
						{
							api_WriteSysUNMsg(TCP_RETRY_SEND_DATA_ERR);
							// 	nwy_ext_echo("\r\n tcp send err and tcp is connected,skip wave data");
							// 	tcpState = eTCP_GET_DATA_FROM_QUEUE;
						}
					}
#else
					if (nwy_get_msg_que(TranDataToTcpUserServerMsgQue, &UploadTranData, 0xffffffff))
					{
						HeartSeconds = 0;
						Tcp_send(socket, (char *)&UploadTranData.buf[0], UploadTranData.len);
					}
					if (nwy_get_msg_que(CollectMessageQueue, &InstantData698Frame[0], 100))
					{
						Tcp_send(socket, (char *)&InstantData698Frame[0], sizeof(InstantData698Frame));
					}
#endif
				}
			}
			break;
		default:
			break;
		}
	}
}
//--------------------------------------------------
//功能描述:  tcp 维护线程 维护主站透传基表
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void TCP_PrivateTask(void *param)
{
	nwy_sleep(600);
	BYTE LogInBuf[60] = {0},bchannel = 1;
	tTranData UploadTranData;

	while(1)
	{
		ThreadRunCnt[eTcpUserThread]++;
		if(0 != nwy_ext_check_data_connect())
		{
			if(tcp_connect_flag[1] == 0)
			{
				Tcp_connet(&bchannel);
				if(tcp_connect_flag[1] == 0)
				{
					nwy_sleep(60000); 		//等待60s
				}
			}
			else
			{
				HeartSeconds++;
				if (HeartSeconds == PRIVATE_HEART_INTERVAL)
				{
					HeartSeconds = 0;
					memset(&LogInBuf[0],0,sizeof(LogInBuf));
					Compose_Heartbeatframe_698(LogInBuf, 1);
					Tcp_send(1, (char*)&LogInBuf, sizeof(LogInBuf));
				}
				if(nwy_get_msg_que(TranDataToTcpPrivateServerMsgQue, &UploadTranData, 0xffffffff))
				{
					Tcp_send(1, (char *)&UploadTranData.buf[0], UploadTranData.len);
				}
				if (nwy_get_msg_que(ModuleDataToTcpMsgQue, &UploadTranData, 0xffffffff))
				{
					Tcp_send(1, (char *)&UploadTranData.buf[0], UploadTranData.len);
				}
			}
		}
		nwy_sleep(1000);
	}
}