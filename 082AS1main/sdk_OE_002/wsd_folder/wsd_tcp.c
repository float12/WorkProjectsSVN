//----------------------------------------------------------------------
// Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
// ������	��Ȫ
// ��������
// ����
// �޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#include<sys/time.h>
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define PRIVATE_HEART_INTERVAL 				(60*2)					//ά���� �������
#define USER_HEART_INTERVAL 				(30)					//�û��� �������
#define USER_RESET_INTERVAL					(60*3)					//�û��� ��ʱ���޷����� ��λ���// Լ��Сʱ
#define PORT     							 7194					//ά����  �˿�
#define PRIVATE_TCP_THREAD					0						//Ĭ��ά����tcp �̺߳�
#define TRANSPARENT_FLAG					0xAA					//͸�������־ AA ��־������ģ������ͨ��
#define CHECK_CONNECTED_NUM					10						//��������Ӵ���
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
int HeartSeconds[TCP_TOTAL_NUM] = {0};
TSerial Serial_698;
#if (TCP_COMM_MODULE == YES)
static BYTE Tcp_ComModuleFlag = TRANSPARENT_FLAG;
#endif
const BYTE TcpPrivateIp[] = {"218.201.129.20"};//ά���˹̶�IP
nwy_osiThread_t *tcp_recv_thread[TCP_TOTAL_NUM] = {NULL};
static int socket[TCP_TOTAL_NUM] = {0};

#if(MAX_USER_TRANS_NUM != 0) || (TCP_USER == YES)
static DWORD UserConnectfre[TCP_TOTAL_NUM] = {0};
#endif
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
extern int nwy_data_get_flowcalc_info(nwy_data_flowcalc_info_t *flowcalc_info);
//-----------------------------------------------
//				��������
//-----------------------------------------------
void Tcp_Commodule_timer_cb(void *type)
{
	Tcp_ComModuleFlag = TRANSPARENT_FLAG;
}
void Tcp_send(int Isocket, char *sendbuff, int sendlen, BYTE i)
{
	int s = Isocket;
	int send_len = 0;

	send_len = nwy_socket_send(s, sendbuff, sendlen, 0);
	if(send_len != sendlen)
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
//��������:  �ϱ���ͬ��ʽ��¼֡
//         
//����:      bChannel ����ͨ����
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Send_Loginframe( BYTE bChannel ,tTranData *transdata)
{
	if (bChannel == PRIVATE_TCP_THREAD)//ά���˷���698��¼֡
	{
		Send_Heartbeatframe_698(transdata,0);
	}
	else
	{
		// Send_Heartbeatframe_698(transdata,0);
	}
}
//--------------------------------------------------
//��������:  �ж� ͸��ģʽ�Ƿ���
//         
//����:      buff �������� 
//         	S  �׽���ID ��Ӧ͸��ͨ��
//          num ��Ӧ�߳�ID
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
BOOL JudgeTransMode(BYTE *buff,int s,BYTE num)
{
	BYTE buf[] = {0xFE,0xFE,0xFE,0xFE,0x68,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x68,0x14,0x15,0x0E,0x12,0x12,0x37,0x35,
	0x33,0x33,0x33,0x37,0x36,0x35,0x34,0x34,0x35,0x12,0x0E,0xBB,0xBB,0xBB,0xBB,0x34,0xA9,0x16};//��ȫff 645����� ��Ϊ ��������͸������
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
//��������:   �ж��Ƿ���������Ӧ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:   ����վ��������Ӧ��������� 
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
//��������:  ����˽��TCP����������������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Deal_PrivateServer( tTranData *tempTranData )
{
	#if (TCP_COMM_MODULE == YES)
	if (Tcp_ComModuleFlag != TRANSPARENT_FLAG)
	#endif
	{
		#if (TCP_COMM_MODULE == YES)
		nwy_stop_timer(Tcp_ComModule_timer);
		nwy_start_timer(Tcp_ComModule_timer, 60000);
		#endif
		tempTranData->TranType = eTRAN_TCP;

		if(nwy_put_msg_que(TranDataToUartMessageQueue, tempTranData, 0xffffffff))
		{
			nwy_ext_echo("\r\n tcp send messg success\r\n");
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
			//����������ظ� ��ȥ��� ���ඪ����
			if (JudgeHeartLinkResponse(tempTranData))
			{
				nwy_ext_echo("\r\n heart recv dont process \r\n");
			}
			else
			{
				if(nwy_put_msg_que(TcpDataToModuleMsgQue, tempTranData, 0xffffffff))
				{
					nwy_ext_echo("\r\n tcp send TO MODUEL success\r\n");
				}
			}
		}
	}
	#endif
}
//--------------------------------------------------
//��������:  �����û���TCP����������������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  Deal_UserServer( tTranData *tempTranData ,BYTE bChannel)
{
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
	Deal_UserServer_Ep212(tempTranData,api_GetTcpChannelTable(eMETER,bChannel-1));
	#else
	if(nwy_put_msg_que(TranDataToUartMessageQueue, tempTranData, 0xffffffff))
	{
		nwy_ext_echo("\r\n user_tcp success recv\r\n");
	}
	#endif
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
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	tTranData tcptouart;

	while(1)
	{
		FD_ZERO(&rd_fd);
		FD_ZERO(&ex_fd);
		FD_SET(s, &rd_fd);
		FD_SET(s, &ex_fd);
		result = nwy_socket_select(s + 1, &rd_fd, NULL, &ex_fd, &tv);
		if(result < 0)
		{
			nwy_socket_close(s);
			nwy_sleep(60000); 		//�ȴ�60s
			socket[bNum] = 0;
			nwy_exit_thread();
		}
		else if(result > 0)
		{
			nwy_ext_echo("\r\nstarttask2\r\n");
			if(FD_ISSET(s, &rd_fd))
			{
				memset(recv_buff, 0, NWY_UART_RECV_SINGLE_MAX + 1);
				recv_len = nwy_socket_recv(s, recv_buff, NWY_UART_RECV_SINGLE_MAX, 0);
				if(recv_len > 0)
				{
					memset(&tcptouart, 0, sizeof(tcptouart));
					memcpy(&tcptouart, recv_buff, sizeof(recv_buff));
					tcptouart.len = recv_len;
					tcptouart.socketid = s;
					tcptouart.threadid = bNum;
					HeartSeconds[bNum] = 0;
					if (bNum == PRIVATE_TCP_THREAD)//ά���˷����� �̶�һ��
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
					nwy_socket_close(s);
					nwy_sleep(60000); 		//�ȴ�60s
					socket[bNum] = 0;
					nwy_exit_thread();
				}
			}
			if(FD_ISSET(s, &ex_fd))
			{
				nwy_ext_echo("\r\ntcp select ex_fd:\r\n");
				nwy_socket_close(s);
				nwy_sleep(60000); 			//�ȴ�60s
				socket[bNum] = 0;
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
	if(strlen(hostname) > 15)
		return NWY_GEN_E_UNKNOWN;

	if((sscanf(hostname, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
		return NWY_GEN_E_UNKNOWN;

	if(!((a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0)))
		return NWY_GEN_E_UNKNOWN;

	sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
	// memset(hostname, 0, sizeof(hostname));
	strcpy(hostname, temp);
	return NWY_SUCESS;
}
//--------------------------------------------------
//��������:  tcp ���ӹ���
//         
//����:      bNum ��ͬ�̺߳�  0Ĭ��ά����  �����û���
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void Tcp_connet(tTranData *transdata,BYTE *bNum)
{
	char ip_buf[256] = {0};
	int af_inet_flag = AF_INET;
	int on = 1;
	int opt = 1, ret = 0;
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

	if(bChannel == PRIVATE_TCP_THREAD)// ���ӵĵ�һ��ά������վ ip�Ͷ˿ڹ̶�
	{
		memcpy(ip_buf,TcpPrivateIp, sizeof(TcpPrivateIp));
	}
	else
	{
		memcpy(ip_buf, Serverpara[bChannel-1].ServerIP, sizeof(Serverpara[bChannel-1].ServerIP));
		nwy_ext_echo("\r\nserver bNUM[%d]  ip %s",bChannel-1, Serverpara[bChannel-1].ServerIP);
	}
	ret = nwy_hostname_check(ip_buf);
	if(ret != NWY_SUCESS)
	{
		nwy_ext_echo("\r\ninput ip or url is invalid");
		return;
	}
	if(ip4addr_aton(ip_buf, &addr.u_addr.ip4) == 0)
	{
		nwy_ext_echo("\r\ninput ip error:\r\n");
		return;
	}
	inet_addr_from_ip4addr(&sa_v4.sin_addr, ip_2_ip4(&addr));
	sa_v4.sin_len = sizeof(struct sockaddr_in);
	sa_v4.sin_family = AF_INET;
	if (bChannel == PRIVATE_TCP_THREAD)
	{
		sa_v4.sin_port = htons(PORT);
	}
	else
	{
		sa_v4.sin_port = htons(Serverpara[bChannel-1].ServerPort);
	}

	af_inet_flag = AF_INET;
	bSocket = nwy_socket_open(af_inet_flag, SOCK_STREAM, IPPROTO_TCP);
	nwy_ext_echo("\r\ntcp open socket is[%d] num is [%d]\r\n",bSocket,bChannel);
	
	nwy_socket_setsockopt(bSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));

	nwy_socket_setsockopt(bSocket, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
	if(0 != nwy_socket_set_nonblock(bSocket))
	{
		nwy_socket_close(bSocket);
		socket[bChannel] = 0;
		nwy_ext_echo("\r\nsocket set err\r\n");
		return;
	}
	start = nwy_get_ms();

	while (1)
	{
		ret = nwy_socket_connect(bSocket, (struct sockaddr *)&sa_v4, sizeof(sa_v4));

		if(ret)
		{
			// nwy_ext_echo("\r\nsocket first connect ok\r\n");
		}
		if((nwy_get_ms() - start) >= 9000)
		{
			nwy_ext_echo("\r\nsocket connect timeout\r\n");
			nwy_ext_echo("\r\ntcp socket status is [%d]num is [%d]",nwy_socket_get_state(bSocket),bChannel);
			nwy_socket_shutdown(bSocket, SHUT_RD);
			nwy_socket_close(bSocket);
			bSocket = 0;
			break;
		}
		if(ret == -1)
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
		nwy_sleep(100);
	}	
	if(bSocket != 0)
	{
		{
			tcp_recv_thread[bChannel] = nwy_create_thread("tcp_recv_thread", nwy_tcp_recv_func, (void *)bNum, NWY_OSI_PRIORITY_NORMAL, 1024 * 10, 8);
			if(tcp_recv_thread[bChannel] == NULL)
			{
				nwy_ext_echo("\r\ncreate tcp recv thread failed, close connect");
				nwy_socket_close(bSocket);
				socket[bChannel] = 0;
			}
		}
	}
	return;
}
//--------------------------------------------------
//��������:  ת������tcp��վ���� ����Զ��ά��ģ���͸����������
//         
//����:      param �����߳�num
//         
//����ֵ:    
//         
//��ע:   �̶� �߳�0 ��ά�����߳�  ��ƽʱ����ģ������ ����͸��ģʽ�����͸������
//        ��վ������͸������ʱ���߳���Ŵ�1�����ۼ�
//--------------------------------------------------
void TCP_TranserTask(void *param)
{
	nwy_sleep(600);
	tTranData UploadTranData;
	BYTE bServerNum;
	bServerNum = *(int *)param;

	while(1)
	{
		if(0 != nwy_ext_check_data_connect())
		{
			if(socket[bServerNum] == 0)
			{
				Tcp_connet(&UploadTranData,&bServerNum);
				if(socket[bServerNum] == 0) 	//����ʧ��ʱ��ÿ����ִ��һ��
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
							nwy_power_off(2);
						}
					}
					#endif
					nwy_sleep(60000); 		//�ȴ�60s
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
					UploadTranData.threadid = bServerNum;
					Send_Heartbeatframe_698(&UploadTranData,1);
				}
				if(nwy_get_msg_que(UartDataToTcpMsgQue, &UploadTranData, 0xffffffff))//���û���Ҳ�Ǵ�͸��������ظ�������Ҳ�ڴ˴��ظ�
				{
					//test
					nwy_ext_echo("\r\n socket is [%d],thread is [%d]",socket[UploadTranData.threadid],UploadTranData.threadid);
					HeartSeconds[UploadTranData.threadid] = 0;
					Tcp_send(socket[UploadTranData.threadid], (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);//������ Ҫ����
				}
				#if (TCP_COMM_MODULE == YES)
				if (nwy_get_msg_que(ModuleDataToTcpMsgQue, &UploadTranData, 0xffffffff))
				{
					HeartSeconds[UploadTranData.threadid]  = 0;
					Tcp_send(socket[UploadTranData.threadid], (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);
				}
				#endif
			}
		}
		nwy_sleep(1000);
	}
}

#if(TCP_USER == YES)
//--------------------------------------------------
//��������:  ���� �û���վ
//         
//����:      param �����߳�num
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void TCP_User_Task(void *param)
{
	BYTE bServerNum;
	#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_104)
	CollectionDatas CollectData;
	#endif
	bServerNum = *(int *)param;
	tTranData UploadTranData;

	nwy_ext_echo("\r\n tcp user is %d",bServerNum);
	while (1)
	{
		if(0 != nwy_ext_check_data_connect())
		{
			if(socket[bServerNum] == 0)
			{
				Tcp_connet(&UploadTranData,&bServerNum);
				if(socket[bServerNum] == 0) 	//����ʧ��ʱ��ÿ����ִ��һ��
				{
					UserConnectfre[bServerNum]++;
					nwy_sleep(60000); 		//�ȴ�60s
					if (UserConnectfre[bServerNum] > USER_RESET_INTERVAL)
					{
						nwy_ext_echo("\r\n reset!!!");
						nwy_power_off(2);
					}
				}
			}
			else
			{
				UserConnectfre[bServerNum] = 0;

				#if(CYCLE_REPORT_PROTOCAL == PROTOCOL_104)
				if(nwy_get_msg_que(CollectMessageQueue, &CollectData, 0xffffffff))	// ��������tcp ���ϱ�
				{
					#if( CYCLE_METER_READING ==  YES)
					{
						nwy_ext_echo("TCP 104 CYCLE REPORT");
						CycleReportData_104(&CollectData.CollectMeter[0][0],socket[bServerNum],bServerNum);
					}
					#endif
				}
				#elif(CYCLE_REPORT_PROTOCAL == PROTOCOL_212)
				if(nwy_get_msg_que(UartToEp212MsgQue, &UploadTranData, 0xffffffff))
				{
					Tcp_send(socket[UploadTranData.threadid], (char *)&UploadTranData.buf[0], UploadTranData.len,UploadTranData.threadid);
					nwy_ext_echo("\r\n UartToEp212MsgQue success");
				}
				#endif
			}
		}
		nwy_sleep(1000);
	}
}
#endif
//--------------------------------------------------
//��������:  �����վ����״̬
//         
//����:      bNum ����tcp��վ���  0 ά����  1�������� �ݶ�
//         
//����ֵ:    
//         
//��ע:  
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
