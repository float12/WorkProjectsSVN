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
#define MAX_BLE_PACKET_SIZE 		244
#define FACTORY_TF_TEST				"/sdcard0/test.bin"		//厂内tf卡读写测试 路径
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
// File_txt File;
char PrintLogSign = 0;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
DWORD ReadTfInfoOffset = 0;
BYTE BleRecvByteTimeoutFlag = 0;
T_DLT645 gPr645;
tTranData bleTranDataToUart;
BYTE IsBLETranData = 0; // 1表示收到蓝牙透传数据
TSerial ParaSerial;
BYTE DataBuf[UART_BUFF_MAX_LEN];
const char filepara_name[64] = {"/para.txt"};
const ServerParaRam_t ServerparaConst[2] = 
{
	[0] = 
	{// tcp
		.ServerPort = 7214,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0
	},
	[1] = 
	{// mqtt
		.ServerPort = 6211,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0
	}
};

const TSafeMem SafeConst = 
{
	.Serverpara[0] = 
	{
		.ServerPort = 8042,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0
	},
	.Serverpara[1] = 
	{// mqtt
		.ServerPort = 7214,
		.ServerIP = "218.201.129.20",
		.ServerUserName = "wisdom",
		.ServerUserPwd = "wisdom",
		.wCrc = 0
	},
	.ReportPara = 
	{
		.QueSpace = 10,
		.WriteFileTime = 20,
	}
};
int UartWh = 0;
BYTE WhFlag = 0;	  // 蓝牙bit0 串口bit1
int BleLogSwitch = 0; // 蓝牙打印log开关
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
// 打印log
static nwy_osiMutex_t *fact_mutex = NULL;
void nwy_ext_factory(char *fmt, ...)
{
	static char echo_str[NWY_EXT_SIO_RX_MAX];
	va_list a;
	int i, size;
	if (NULL == fact_mutex)
		fact_mutex = nwy_create_mutex();
	if (NULL == fact_mutex)
		return;
	nwy_lock_mutex(fact_mutex, 0);
	va_start(a, fmt);
	vsnprintf(echo_str, NWY_EXT_SIO_RX_MAX, fmt, a);
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
			nwy_sleep(10);
		else
			break;
	}
	nwy_unlock_mutex(fact_mutex);
}

void nwy_ext_sio_data_cb(const char *data, uint32 length)
{
	WORD wLen, wLen1;
	wLen = ParaSerial.RXWPoint + length;
	if (length >= UART_BUFF_MAX_LEN)
	{
		length = UART_BUFF_MAX_LEN;
	}
	if (wLen < UART_BUFF_MAX_LEN)
	{
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], data, length);
		ParaSerial.RXWPoint += length;
	}
	else
	{
		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], data, wLen1);
		memcpy((char *)&ParaSerial.ProBuf[0], data + wLen1, length - wLen1);
		ParaSerial.RXWPoint = length - wLen1;
	}
	return;
}
void api_GetSoftBuildTime(TRealTimer *pT)
{
	int tmp[5] = {1};
	BYTE strTemp[4] = {0}, i = 0;
	BYTE strMonth[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	sscanf(__DATE__, "%s %2d %4d", strTemp, &tmp[0], &tmp[1]);
	sscanf(__TIME__, "%2d:%2d:%2d", &tmp[2], &tmp[3], &tmp[4]);
	for (i = 0; i < 12; i++)
	{
		if (strTemp[0] == strMonth[i][0] && strTemp[1] == strMonth[i][1] && strTemp[2] == strMonth[i][2])
		{
			pT->Mon = i + 1;
			break;
		}
	}
	if (i >= 12)
	{
		pT->Mon = 1;
	}
	pT->Day = (BYTE)tmp[0];
	pT->wYear = (WORD)tmp[1];
	pT->Hour = (BYTE)tmp[2];
	pT->Min = (BYTE)tmp[3];
	pT->Sec = (BYTE)tmp[4];
}
//---------------------------------------------------------------
// 函数功能: 发送645数据
//
// 参数: 	p[in]-数据区
//
// 返回值:   无
//
// 备注:
//---------------------------------------------------------------
void SendDlt645(TSerial *p)
{
	WORD TDataLength;
	BYTE i;

	TDataLength = p->ProBuf[9];

	// fe fe fe fe 68 A0...A5 68 81 L D0..DL CS 16,除D0...DL外，其余共16字节
	if (TDataLength > (UART_BUFF_MAX_LEN - 16)) // 最多或等于
	{											// 此举应可防把别的内存数据冲了
		return;
	}

	i = (11 + TDataLength);
	for (;;)
	{
		p->ProBuf[i + 4] = p->ProBuf[i];
		if (i == 0)
		{
			break;
		}
		i--;
	}

	p->ProBuf[0] = 0x0fe;
	p->ProBuf[1] = 0x0fe;
	p->ProBuf[2] = 0x0fe;
	p->ProBuf[3] = 0x0fe;

	p->SendLength = (TDataLength + 16); // 如有FE FE FE FE 则加16

	#if (BLE_UART_WH == YES)
	if (WhFlag & BIT1)
	{
		nwy_uart_send_data(UartWh, (BYTE *)&p->ProBuf[0], p->SendLength);
		WhFlag &= ~(BIT1);
		nwy_ext_echo("uart  send");
	}
	if (WhFlag & (BIT0))
	{
		if (nwy_ble_get_conn_status() == TRUE)
		{
			nwy_ble_send_data(p->SendLength, (char *)&p->ProBuf[0]);
		}
		WhFlag &= ~(BIT0);
		nwy_ext_echo("ble   send");
	}
	#endif
	if (WhFlag & BIT2)
	{
		WhFlag &= ~(BIT2);

		memcpy(bleTranDataToUart.buf,(char *)&p->ProBuf[0],p->SendLength);
		bleTranDataToUart.len = p->SendLength;
		if (nwy_put_msg_que(ModuleDataToTcpMsgQue, &bleTranDataToUart, 0xffffffff))
		{
			nwy_ext_echo("\r\n module to tcp success");
		}
	}
	
	nwy_usb_serial_send((char *)&p->ProBuf[0], p->SendLength);
}
//--------------------------------------------------
// 功能描述:  回填645数据长度
//
// 参数:      TYPE_PORT PortType[in]:串口类型
//
// 返回值:    static
//
// 备注内容:  无
//--------------------------------------------------
static void FillDlt645FrameData(WORD wResult)
{
	WORD Lenth;

	if (wResult < 0x8000) // 正常应答
	{
		Lenth = wResult;
		gPr645.wDataLen = Lenth;
		switch (gPr645.byReceiveControl)
		{
		case 0x11:
			if (gPr645.wDataLen <= 196)
			{
				gPr645.pMessageAddr[9] = gPr645.wDataLen + 4; // 数据长度多4个字节的数据标识
				memcpy(gPr645.pMessageAddr + 10 + 4, DataBuf, Lenth);
			}
			else // 需要分帧处理
			{
			}
			break;
		case 0x14:
			gPr645.pMessageAddr[9] = 0; // 数据长度为零
			break;
		}
	}
	else // 异常应答
	{
	}
}
//--------------------------------------------------
// 功能描述:  发送698协议透传数据
//
// 参数:     p
//
// 返回值:    无
//
// 备注内容:  无
//--------------------------------------------------
void Deal_BLETran698(TSerial *p)
{
	nwy_ext_echo("\r\n 698 whole received");

	// 把数据从报文开始位置 向 以p->ProBuf[0]开始处移最好放在 698.45协议处理函数 中
	api_DoReceMoveData(p, PROTOCOL_698);

	nwy_ext_echo("\r\n698 FrameLength:%d", p->ProStepDlt698_45);

	memcpy(bleTranDataToUart.buf, &(p->ProBuf[0]), p->ProStepDlt698_45);
	bleTranDataToUart.len = p->ProStepDlt698_45;
	bleTranDataToUart.TranType = BLUETOOTH_TRAN;
	bleTranDataToUart.Uart = eUART_LTO;

	if (nwy_put_msg_que(TranDataToUartMessageQueue, &bleTranDataToUart, 0xffffffff) == TRUE)
	{
		nwy_ext_echo("\r\nput blueTran data to uart queue success");
	}
	else
	{
		nwy_ext_echo("\r\nput blueTran data to uart queue fail");
	}
}
//--------------------------------------------------
// 功能描述:  响应报文处理
//
// 参数:      TYPE_PORT PortType[in]
//
// 返回值:    static
//
// 备注内容:  无
//--------------------------------------------------
void Proc645LinkDataResponse(WORD wResult)
{
	BYTE i;
	BYTE *pBuf;
	WORD wLen;
	pBuf = gPr645.pMessageAddr;

	for (i = 0; i < 6; i++)
	{
		// 确定数据是否正确
		gPr645.pMessageAddr[1 + i] = 0x88;
	}

	FillDlt645FrameData(wResult);

	// 填充控制码在填充数据后 因为需要判断是否有后续帧
	if (wResult < 0x8000)
	{
		gPr645.pMessageAddr[8] = (gPr645.byReceiveControl | 0x80);
	}
	else
	{
		gPr645.pMessageAddr[8] = (gPr645.byReceiveControl | 0xC0);
	}

	for (i = 10; i < (10 + pBuf[9]); i++)
	{
		pBuf[i] = (pBuf[i] + 0x33);
	}
	wLen = gPr645.pMessageAddr[9] + 6 + 4; // 4：两个68 一个控制码 一个长度
	gPr645.pMessageAddr[0] = 0x68;
	gPr645.pMessageAddr[1 + 6] = 0x68;
	gPr645.pMessageAddr[wLen] = lib_CheckSum(gPr645.pMessageAddr, wLen);
	gPr645.pMessageAddr[wLen + 1] = 0x16;

	SendDlt645(&ParaSerial);
}
//-----------------------------------------------
// 函数功能: 扩展规约读函数
//
// 参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB
//
// 返回值:	返回读取数据长度
//
// 备注:规约见《电表软件平台扩展规约》:抄读时在04dfdfdb后面扩展4个字节，设置时在数据域扩展8个字节（扩展字节以下简称扩展标识）。
//	第1,2字节为厂内扩展数据标识，分别对应DI0，DI1.
//	第3,4字节固定为0xdf，0xdb。
//	第5,6,7,8字节为当前时间与00年1月1日0时0分的相对分钟数（hex码，传输时高在后，低在前）（抄读时无此数据）。
//-----------------------------------------------
WORD ReadFactoryExtPro(BYTE *pBuf)
{
	BYTE *p;
	WORD wReturnLen;
	char BuilTime[20] = {0};
	TRealTimer Time = {0};
	// char ServerConnectFlag = 0;
	char *mac = NULL;
	char buf[12] = {0};
	char buf1[15] = {0};
	char buf2[100] = {0};
	BYTE i = 0;
	BYTE csq;
	DWORD offset = 0;
	WORD tw,wLen;
	int fd = -1;
	FileInfoOfOneCharge infoOfOneCharge = {0};
	tw = pBuf[0];
	wLen = 0;

	if ((pBuf[2] != 0xdf) || (pBuf[3] != 0xdb))
	{
		IsBLETranData = 1;
		return 0;
	}
	if ((pBuf[1] != 0xFF) && (pBuf[1] != 0xF0) &&(pBuf[1] != 0xFD))	//蓝牙透传时 FD 区分基表与 模块的自由异常事件
	{
		IsBLETranData = 1;
		return 0;
	}

	p = pBuf + 2;
	wReturnLen = 0;

	switch (pBuf[1])
	{
	case 0xFD://异常事件记录			
		if(tw == 0)
		{
			return 0;
		}
		
		for(i=1;i<=6;i++)
		{
			wLen = api_ReadSysUNMsg((tw-1)*6+i,p);
			p += wLen;
			wReturnLen += wLen;
		}
		
		wReturnLen += 2;
		break;
	case 0xff:
		if (pBuf[0] == 0x00)
		{
			api_GetSoftBuildTime(&Time);
			sprintf((char *)BuilTime, "%04d%02d%02d%02d%02d", Time.wYear, Time.Mon, Time.Day, Time.Hour, Time.Min);
			lib_ExchangeData(p, (BYTE *)&BuilTime[0], sizeof(BuilTime));
			wReturnLen = sizeof(BuilTime);
		}
		else if (pBuf[0] == 0x01)
		{
			lib_ExchangeData(p, imei.nImei, sizeof(imei.nImei));
			wReturnLen = sizeof(imei.nImei);
		}
		else if (pBuf[0] == 0x02) // 抄表状态
		{
			for (i = CycleReadMeterNum; i < (CycleReadMeterNum + POWERON_READRPARA_NUM); i++) // 置上电要抄读的bit
			{
				if ((PowerOnReadMeterFlag & (1 << i)) != (1 << i))
				{
					p[0] = 0;
					return (wReturnLen += 3);
				}
			}
			p[0] = 0x0F; // 兼容qdlt
			wReturnLen = 1;
		}
		else if (pBuf[0] == 0x03) // 与主站状态
		{
			// if (api_GetMqttConStatus() == TRUE)
			// {
				// ServerConnectFlag |= (BIT0);
			// }
			// else
			// {
				// ServerConnectFlag &= ~(BIT0);
			// }
			// if (tcp_connect_flag == TRUE)
			// {
				// ServerConnectFlag |= (BIT1);
			// }
			// else
			// {
				// ServerConnectFlag &= ~(BIT1);
			// }
			// p[0] = ServerConnectFlag;
			// wReturnLen = 1;
		}
		else if (pBuf[0] == 0x04) // 当前设备的蓝牙mac地址
		{
			mac = nwy_ble_get_mac_addr();

			lib_ExchangeData((BYTE *)&buf[0], (BYTE *)mac, 6);
			sprintf((char *)&buf1[0], "%02x%02x%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			nwy_ext_echo("(%s) \r\n", buf1);
			for (i = 0; i < 12; i++)
			{
				if ((buf1[i] > 96) && (buf1[i] < 123))
				{
					buf1[i] = (buf1[i] - 32);
				}
			}
			memcpy(p, (BYTE *)&buf1[0], 12);
			wReturnLen = 12;
		} // 05 是关闭蓝牙 车间调试
		else if (pBuf[0] == 0x06) // 检查地理位置
		{
			#if (GPRS_POSITION == YES)
			if (api_CheckorSetPosition(0) == TRUE)
			{
				p[0] = 1;
			}
			else
			{
				p[0] = 0;
			}
			wReturnLen = 1;
			#endif
		}
		else if (pBuf[0] == 0x07) // 检测底层 版本
		{
			nwy_dm_get_inner_version(buf2, 100);
			nwy_ext_echo("\r\nble is %s", buf2);
			lib_ExchangeData(p, (BYTE *)&buf2[0], 100);
			wReturnLen = 100;
		}
		else if (pBuf[0] == 0x08) // 读取信号强度 cqs
		{
			nwy_nw_get_signal_csq(&csq); // 降低频率
			p[0] = csq;
			wReturnLen = 1;
		}
		else if (pBuf[0] == 0x09) // 读取拨号连接状态
		{
			p[0] = nwy_ext_check_data_connect();
			wReturnLen = 1;
		}
		// 录播工装 车间检测维护
		else if (pBuf[0] == 0x0A) // 读写测试TF卡
		{
			if (api_CheckaAndMountSDCard() == TRUE)
			{
				int fd = nwy_sdk_fopen(FACTORY_TF_TEST, NWY_CREAT | NWY_RDWR);
				if (fd>= 0)
				{
					memset(buf1,0xAA,sizeof(buf1));
					nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
					if(nwy_sdk_fwrite(fd,buf1,sizeof(buf1)) == sizeof(buf1))
					{
						memset(buf1,0x00,sizeof(buf1));
 						nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
						if(nwy_sdk_fread(fd,buf1,sizeof(buf1)) == sizeof(buf1))
						{
							for (i = 0; i < sizeof(buf1); i++)
							{
								// nwy_ext_echo("\r\ntest is %x", buf1[i]);
								if(buf1[i] != 0XAA)
								{
									p[0] = 0;
									wReturnLen = 3;
									return wReturnLen;
								}
							}
						}
					}
				}
			}
			p[0] = 0;
			if (i == sizeof(buf1))
			{
				p[0] = 1;
			}
			wReturnLen = 1;
		}
		// else if (pBuf[0] == 0x0B) // 读取个回路UI系数
		// {
			// p[0] = nwy_ext_check_data_connect();
			// wReturnLen = 1;
		// }
		else if (pBuf[0] == 0x0C) // 读取 波形info文件
		{
			TInfoFileHead info = {0};
			int fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDWR);
			if (fd >= 0)
			{
				nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
				nwy_sdk_fread(fd, &info, sizeof(TInfoFileHead));
				nwy_sdk_fclose(fd);
				memcpy((BYTE*)&p[0],(BYTE*)&info,sizeof(info));
				wReturnLen = sizeof(info);
			}	
		}
		else if (pBuf[0] == 0x0E)
		{
			if (api_OperateFileSystem(READ, GET_SAFE_SPACE_ADDR(ReportPara.QueSpace), p, sizeof(ReportPara.QueSpace)) == TRUE)
			{
				wReturnLen = sizeof(ReportPara.QueSpace);
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x0F)
		{
			if (api_OperateFileSystem(READ, GET_SAFE_SPACE_ADDR(ReportPara.WriteFileTime), p, sizeof(ReportPara.WriteFileTime)) == TRUE)
			{
				wReturnLen = sizeof(ReportPara.WriteFileTime);
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x10)//读tf卡的info信息的偏移
		{
			memcpy(p, (BYTE *)&ReadTfInfoOffset, sizeof(ReadTfInfoOffset));
			wReturnLen = sizeof(ReadTfInfoOffset);
		}
		else if (pBuf[0] == 0x11)//读tf卡的info信息
		{
			fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDONLY);
			offset = sizeof(TInfoFileHead) + ReadTfInfoOffset * sizeof(FileInfoOfOneCharge);
			nwy_sdk_fseek(fd, offset, NWY_SEEK_SET);
			nwy_sdk_fread(fd, &infoOfOneCharge, sizeof(infoOfOneCharge));
			nwy_sdk_fclose(fd);
			//消除结构体字节对齐影响单独赋值
			memcpy(p, (BYTE *)&infoOfOneCharge.FirstFileName, sizeof(infoOfOneCharge.FirstFileName));
			memcpy(p+sizeof(infoOfOneCharge.FirstFileName), (BYTE *)&infoOfOneCharge.CNT, sizeof(infoOfOneCharge.CNT));
			memcpy(p+sizeof(infoOfOneCharge.FirstFileName)+sizeof(infoOfOneCharge.CNT), (BYTE *)&infoOfOneCharge.Loop, sizeof(infoOfOneCharge.Loop));
			wReturnLen = sizeof(infoOfOneCharge.FirstFileName) + sizeof(infoOfOneCharge.CNT) + sizeof(infoOfOneCharge.Loop);
			ReadTfInfoOffset++;
			ReadTfInfoOffset %= MAX_CHARGE_SAVE_NUM;
		}
		if (wReturnLen)
		{
			wReturnLen += 2;
		}
		break;
	case 0xF0:
		if (pBuf[0] == 0x01) // 端口号
		{
			memcpy(p, (BYTE *)&Serverpara[ePARA_TCP].ServerPort, sizeof(Serverpara[ePARA_TCP].ServerPort));
			wReturnLen = 4;
		}
		else if (pBuf[0] == 0x02) // IP
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_TCP].ServerIP[0], sizeof(Serverpara[ePARA_TCP].ServerIP));
			wReturnLen = sizeof(Serverpara[ePARA_TCP].ServerIP) + 2;
		}
		else if (pBuf[0] == 0x03) // 账号
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_TCP].ServerUserName[0], sizeof(Serverpara[ePARA_TCP].ServerUserName));
			wReturnLen = sizeof(Serverpara[ePARA_TCP].ServerUserName) + 2;
		}
		else if (pBuf[0] == 0x04) // 密码
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_TCP].ServerUserPwd[0], sizeof(Serverpara[ePARA_TCP].ServerUserPwd));
			wReturnLen = sizeof(Serverpara[ePARA_TCP].ServerUserPwd) + 2;
		}
		else if (pBuf[0] == 0x11) // 端口号
		{
			memcpy(p, (BYTE *)&Serverpara[ePARA_MQTT].ServerPort, sizeof(Serverpara[ePARA_MQTT].ServerPort));
			wReturnLen = 4;
		}
		else if (pBuf[0] == 0x12) // IP
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_MQTT].ServerIP[0], sizeof(Serverpara[ePARA_MQTT].ServerIP));
			wReturnLen = sizeof(Serverpara[ePARA_MQTT].ServerIP) + 2;
		}
		else if (pBuf[0] == 0x13) // 账号
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_MQTT].ServerUserName[0], sizeof(Serverpara[ePARA_MQTT].ServerUserName));
			wReturnLen = sizeof(Serverpara[ePARA_MQTT].ServerUserName) + 2;
		}
		else if (pBuf[0] == 0x14) // 密码
		{
			lib_ExchangeData(p, (BYTE *)&Serverpara[ePARA_MQTT].ServerUserPwd[0], sizeof(Serverpara[ePARA_MQTT].ServerUserPwd));
			wReturnLen = sizeof(Serverpara[ePARA_MQTT].ServerUserPwd) + 2;
		}
		break;
	default:
		break;
	}
	return wReturnLen;
}
//-----------------------------------------------
// 函数功能: 扩展规约写函数
//
// 参数:		pBuf[in]	0--扩展DI0 1--DI1 2--0XDF 3--0XDB
//						4,5,6,7--相对分钟数
// 返回值:	返回值bit15为1为错误，其他为正确
//
// 备注:规约见《电表软件平台扩展规约》
//-----------------------------------------------
WORD WriteFactoryExtPro(BYTE *pBuf)
{
	WORD wReturnLen;
	int fd = -1;
	BYTE k;
	wReturnLen = 0;
	#if (CYCLE_REPORT == YES)
	BYTE i, j;
	#endif

	if ((pBuf[2] != 0xdf) || (pBuf[3] != 0xdb))
	{
		IsBLETranData = 1;
		return 0;
	}
	if ((pBuf[1] != 0xFF) && (pBuf[1] != 0xF0)) // F1后期该标识
	{
		IsBLETranData = 1;
		return 0;
	}
	switch (pBuf[1])
	{
	case 0xFF:
		if (pBuf[0] == 0xAA)
		{
			if (pBuf[8] == 1)
			{
				PrintLogSign = 0;
			}
			else if (pBuf[8] == 0)
			{
				PrintLogSign = 0xAA;
			}
		}
		else if (pBuf[0] == 0x05) // 关闭蓝牙
		{
			if (pBuf[8] == 1)
			{
				nwy_ble_disable();
			}
		}
		else if (pBuf[0] == 0x0A) // 打印蓝牙打印输出
		{
			if (pBuf[8] == 1)
			{
				BleLogSwitch = 0xAA;
			}
			else if (pBuf[8] == 0)
			{
				BleLogSwitch = 0;
			}
			else
			{
				return 0;
			}
		}
		else if (pBuf[0] == 0x0B)// 删除TF路径下所有文件
		{
			if (pBuf[8] == 1)
			{
				// if(api_DeleteAllWaveFile() != TRUE)
				// {
					// return 0;
				// }
				nwy_format_sdcard();
			}
			else
			{
				return 0;
			}
		}
		else if (pBuf[0] == 0x0C)//修改 info 文件参数
		{
			TInfoFileHead info = {0};
			memcpy((BYTE*)&info,&pBuf[8],sizeof(info));
			int fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDWR);
			if (fd >= 0)
			{	
				nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
				if(nwy_sdk_fwrite(fd, &info, sizeof(TInfoFileHead)) == sizeof(info))
				{
					nwy_sdk_fclose(fd);
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}									
		}
		else if (pBuf[0] == 0x0D)//删除event 文件  作为事件初始化 
		{
			if (pBuf[8] == 1)
			{
  				if(api_DeleteSysEventFile() != TRUE)
				{
					return 0;
				}
			}
		}
		else if (pBuf[0] == 0x0E)
		{
			if (api_OperateFileSystem(WRITE, GET_SAFE_SPACE_ADDR(ReportPara.QueSpace), &pBuf[8], sizeof(ReportPara.QueSpace)) == TRUE)
			{
				wReturnLen = sizeof(ReportPara.QueSpace);
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x0F)
		{
			if (api_OperateFileSystem(WRITE, GET_SAFE_SPACE_ADDR(ReportPara.WriteFileTime), &pBuf[8], sizeof(ReportPara.WriteFileTime)) == TRUE)
			{
				wReturnLen = sizeof(ReportPara.WriteFileTime);
			}
			else
			{
				wReturnLen = 0;
			}
		}
		else if (pBuf[0] == 0x10)//tf卡的info信息的偏移
		{
			memcpy((BYTE *)&ReadTfInfoOffset,&pBuf[8],sizeof(ReadTfInfoOffset));
			wReturnLen = sizeof(ReadTfInfoOffset);
		}
		wReturnLen = 1;
		break;
	case 0xF0:
		if (pBuf[0] == 0x01) // 端口号			//tcp 参数
		{
			memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerPort, &pBuf[8], sizeof(Serverpara[ePARA_TCP].ServerPort));
		}
		else if (pBuf[0] == 0x02) // IP
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_TCP].ServerIP[0], &pBuf[8], sizeof(Serverpara[ePARA_TCP].ServerIP));
		}
		else if (pBuf[0] == 0x03) // 用户名
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_TCP].ServerUserName[0], &pBuf[8], sizeof(Serverpara[ePARA_TCP].ServerUserName));
		}
		else if (pBuf[0] == 0x04) // 密码
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_TCP].ServerUserPwd[0], &pBuf[8], sizeof(Serverpara[ePARA_TCP].ServerUserPwd));
		}
		else if (pBuf[0] == 0x11) // 端口号			//tcp 参数
		{
			memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerPort, &pBuf[8], sizeof(Serverpara[ePARA_MQTT].ServerPort));
		}
		else if (pBuf[0] == 0x12) // IP
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_MQTT].ServerIP[0], &pBuf[8], sizeof(Serverpara[ePARA_MQTT].ServerIP));
		}
		else if (pBuf[0] == 0x13) // 用户名
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_MQTT].ServerUserName[0], &pBuf[8], sizeof(Serverpara[ePARA_MQTT].ServerUserName));
		}
		else if (pBuf[0] == 0x14) // 密码
		{
			lib_ExchangeData((BYTE *)&Serverpara[ePARA_MQTT].ServerUserPwd[0], &pBuf[8], sizeof(Serverpara[ePARA_TCP].ServerUserPwd));
		}
		else if (pBuf[0] == 0xAA) // 将所设置参数全部写到文件里
		{
			if (pBuf[8] == 1)
			{
				Serverpara[ePARA_TCP].wCrc = CalCRC16((BYTE *)&Serverpara[ePARA_TCP].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[ePARA_TCP].wCrc)));

				Serverpara[ePARA_MQTT].wCrc = CalCRC16((BYTE *)&Serverpara[ePARA_MQTT].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[ePARA_MQTT].wCrc)));

				for (k = 0; k < 2; k++)
				{
					nwy_ext_echo("\r\nServerpara[%d].wCrc is %04x", k, Serverpara[k].wCrc);
					nwy_ext_echo("\r\nServerpara[%d].ServerPort is %u", k, Serverpara[k].ServerPort);
					nwy_ext_echo("\r\nServerpara[%d].ServerIP is %s", k, Serverpara[k].ServerIP);
					nwy_ext_echo("\r\nServerpara[%d].ServerUserName is %s", k, Serverpara[k].ServerUserName);
					nwy_ext_echo("\r\nServerpara[%d].ServerUserPwd is %s", k, Serverpara[k].ServerUserPwd);
				}
				nwy_ext_echo("\r\nwrite para file Serverpara[ePARA_TCP].wCrc is%04x", Serverpara[ePARA_TCP].wCrc);
				nwy_ext_echo("\r\nwrite para file Serverpara[ePARA_MQTT].wCrc is%04x", Serverpara[ePARA_MQTT].wCrc);
				fd = nwy_sdk_fopen(filepara_name, NWY_CREAT | NWY_RDWR);
				if (nwy_sdk_fwrite(fd, (char *)&Serverpara[ePARA_TCP].ServerPort, (2 * sizeof(ServerParaRam_t))) == (2 * sizeof(ServerParaRam_t)))
				{
					{
						nwy_sdk_fclose(fd);
					}
				}
				else
				{
					nwy_sdk_fclose(fd);
					return 0;
				}
			}
		}
		wReturnLen = 1;
		break;
	case 0xF1:
		#if (CYCLE_REPORT == YES)
		if (pBuf[0] == 0x01)
		{
			for (i = 0; i < MAX_SAMPLE_CHIP; i++)
			{
				for (j = 0; j < CycleReadMeterNum; j++)
				{
					dwReadMeterFlag[i] |= (1 << j);
				}
			}
			wReturnLen = 1;
		}
		#endif
		break;
	default:
		break;
	}
	return wReturnLen;
}
//--------------------------------------------------
// 功能描述:  645应用层函数
//
// 参数:      TYPE_PORT PortType[in]
//
// 返回值:
// 			DLT645_ERR_DATA_01 ~ DLT645_ERR_PRG_KEY_80  -  错误代码
// 			DLT645_NO_RESPONSE - 无需回复
// 			其他 - 数据长度
//
// 备注内容:  无
//--------------------------------------------------
WORD DLT645APPPro(void)
{
	WORD wResult = 0;

	switch (gPr645.byReceiveControl)
	{
	case 0x11:
		wResult = ReadFactoryExtPro(gPr645.pMessageAddr + 14);
		if (wResult != 0)
		{
			memcpy(DataBuf, gPr645.pMessageAddr + 14, wResult);
		}
		else
		{
			wResult = 0x8001;
		}
		break;
	case 0x14:
		wResult = WriteFactoryExtPro(gPr645.pMessageAddr + 22);
		if (wResult == 0)
		{
			wResult = 0x8001;
		}
		else if (wResult > 0x8000)
		{
			return wResult; // 得到失败原因
		}
		else
		{
			DataBuf[0] = 0x00;
			wResult = 1;
		}
		break;
	default:
		break;
	}
	return wResult;
}

//--------------------------------------------------
// 功能描述:  响应报文处理
//
// 参数:      TYPE_PORT PortType[in]
//
// 返回值:    static
//
// 备注内容:  无
//--------------------------------------------------
void Proc645LinkDataRequest(BYTE *pBuf)
{
	WORD i;

	for (i = 10; i < (10 + pBuf[9]); i++)
	{
		pBuf[i] = (pBuf[i] - 0x33);
	}

	gPr645.pMessageAddr = pBuf;

	gPr645.wSerialDataLen = pBuf[3]; // 串口数据长度 不传人serail结构体指针可能取不到串口数据长度!!!!!

	gPr645.byReceiveControl = pBuf[8]; // 接收报文控制码

	// gPr645.eProtocolType = ePROTOCOL_DLT645_FACTORY;

	switch (gPr645.byReceiveControl)
	{
	case 0x11: // 读命令
		// memcpy( gPr645.dwDataID.b, &pBuf[10], 4 );//获取数据标识
		gPr645.pDataAddr = &pBuf[14];	 // 数据域地址
		gPr645.wDataLen = (pBuf[9] - 4); // 数据区长度
		break;
	case 0x14: // 写命令
		// memcpy( gPr645.dwDataID.b, &pBuf[10], 4 );//获取数据标识
		gPr645.bLevel = pBuf[14];					 // 密码等级
		gPr645.pDataAddr = &pBuf[22];				 // 数据域地址
		gPr645.wDataLen = (pBuf[9] - 4 - 1 - 3 - 4); // 数据区长度
		break;
	default:
		break;
	}
}
//--------------------------------------------------
// 功能描述:  645规约处理主函数
//
// 参数:      TYPE_PORT PortType[in]
//
//           BYTE *pBuf[in]
//
// 返回值:    WORD
//
// 备注内容:  无
//--------------------------------------------------
void ProMessage_Dlt645(TSerial *p)
{
	WORD wResult = 0;
	WORD i = 0;

	memset(&bleTranDataToUart, 0, sizeof(tTranData));
	Proc645LinkDataRequest(ParaSerial.ProBuf); // 链路层数据处理
	wResult = DLT645APPPro();				   // 应用层数据解析
	if (IsBLETranData == 0)
	{
		Proc645LinkDataResponse(wResult); // 链路层数据组帧
	}
	else // 数据标识与模块不匹配，透传给表
	{
		IsBLETranData = 0;
		nwy_ext_echo("\r\nFrameLength %d", p->ProStepDlt645);

		for (i = 10; i < (10 + gPr645.pMessageAddr[9]); i++)
		{
			gPr645.pMessageAddr[i] = (gPr645.pMessageAddr[i] + 0x33);
		}

		// nwy_ext_echo("\r\n ble tran data:");

		memcpy(bleTranDataToUart.buf, gPr645.pMessageAddr, p->ProStepDlt645);
		bleTranDataToUart.len = p->ProStepDlt645;
		bleTranDataToUart.TranType = BLUETOOTH_TRAN;
		bleTranDataToUart.Uart = eUART_WHWAVE;
		
		if (nwy_put_msg_que(TranDataToUartMessageQueue, &bleTranDataToUart, 0xffffffff) == TRUE)
		{
			nwy_ext_echo("\r\nput blueTran data to uart queue success");
		}
		else
		{
			nwy_ext_echo("\r\nput blueTran data to uart queue fail");
		}
		// for (i = 0; i < FrameLength; i++)
		// {
		// 		nwy_ext_echo(" %02x ", gPr645.pMessageAddr[i]);
		// }
	}
}
//---------------------------------------------------------------
// 函数功能: 645格式判断
//
// 参数: 	portType[in] - 通讯端口
//
// 返回值:  FALSE: 报文没有收完
//		  TRUE : 报文已收完且InitPoint()
//
// 备注:
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_WH(BYTE ReceByte, TSerial *p)
{
	WORD i, j;
	BYTE *pBuf;
	TDlt645Head Dlt645Head;

	// 搜索同步头
	if (p->ProStepDlt645 == 0)
	{
		if (ReceByte == 0x68)
		{
			// 写入数据
			// p->ProBuf[p->ProStepDlt645] = ReceByte;
			// Dlt645规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt645 = p->RXRPoint;

			// 操作指针
			p->ProStepDlt645++;

			// 开始接收定时
			// p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
	}
	else if (p->ProBuf[p->BeginPosDlt645] == 0x68)
	{
		// 接收规约头、控制字、长度 68 1 2 3 4 5 6 68 01 02
		if (p->ProStepDlt645 < sizeof(TDlt645Head))
		{
			// 写入数据
			// p->ProBuf[p->ProStepDlt645] = ReceByte;

			// 操作指针
			p->ProStepDlt645++;

			// 是否可以处理数据
			if (p->ProStepDlt645 == sizeof(TDlt645Head))
			{
				// 把规约报文移到临时缓冲中 循环缓冲用此
				// Num：移动多少字节
				// BeginPos:从什么地址开始移
				pBuf = (BYTE *)&Dlt645Head;
				for (i = 0; i < sizeof(Dlt645Head); i++)
				{
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN];
				}
				// 映射结构
				// pDlt645Head = (TDlt645Head *)&TmpArray;
				j = p->ProStepDlt645;

				// 搜索第二同步字
				if (Dlt645Head.Sync2 != 0x68) // 如果没有第2个帧起始符68，则前面收到的第一68不对，从它后面搜索第一个68
				{
					for (i = 1; i < p->ProStepDlt645; i++)
					{
						if (p->ProBuf[(p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN] == 0x68)
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645 + i) % UART_BUFF_MAX_LEN;
							p->ProStepDlt645 -= i;
							break;
						}
						// 改后68前面其它数据或68都能通信上，但这个通信不上，没防这种，不想太复杂：68 fe fe fe fe fe fe 68 88 88 88 88 88 88 68 11 04 33 33 34 33 e2 16
					}
					if (i >= j) // j 是 p->ProStepDlt645 的备份，所有收到的数据已找完，则不再找，直接ProStepDlt645赋0跳出
					{
						p->ProStepDlt645 = 0; // InitPoint;
						return FALSE;
					}
					// p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
				else
				{
				}

				// 取控制字节
				if (Dlt645Head.Control >= 0x80) // 如果判断控制码不对，则第2个68开始重新检测接收报文
				{
					p->BeginPosDlt645 = p->BeginPosDlt645 + 7;
					p->ProStepDlt645 -= 7;
					return FALSE;
				}

				// 取长度 645报文数据域数据长度
				p->Dlt645DataLen = Dlt645Head.Length;

				// 判断长度
				// 68 A0...A5 68 81 L D0..DL CS 16,除D0...DL外，其余共16字节
				if (p->Dlt645DataLen >= (UART_BUFF_MAX_LEN - 12))
				{
					p->ProStepDlt645 = 0; // InitPoint(p);
					return FALSE;
				}
			}

			// 处理校验
			// p->DltCheckSum += ReceByte;

			// p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		// 接收数据
		else if (p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2))
		{
			// 接收数据
			// p->ProBuf[ p->ProStepDlt645 ] = ReceByte;

			// 操作指针
			p->ProStepDlt645++;

			// 处理校验字节
			if (p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 1))
			{
				// 把规约报文移到临时缓冲中 循环缓冲用此
				// Num：移动多少字节
				// BeginPos:从什么地址开始移
				if (ReceByte != api_CalRXD_CheckSum(0, p))
				// if( ReceByte != ( lib_CheckSum(p->ProBuf+p->BeginPosDlt645,(10+p->Dlt645DataLen))&0xff) )
				{
					// 校验不正确
					p->ProStepDlt645 = 0; // InitPoint(p);
					return FALSE;
				}
			}

			// 收到0x16 才进行程序处理
			else if (p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 2))
			{
				// 校验通过，可以处理数据

				// 禁止接收
				// api_DisableRece( p );
				// 把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				api_DoReceMoveData(p, PROTOCOL_645);

				ProMessage_Dlt645(p);

				// 处理完成
				InitPoint(p);

				return TRUE;
			}
			else
			{
				// 处理校验
				//	p->DltCheckSum += ReceByte;
			}

			// 处理接收定时
			// p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		else
		{
			p->ProStepDlt645 = 0; // InitPoint(p);
		}
	}
	return FALSE;
}
//---------------------------------------------------------------
// 函数功能: 透传698格式判断
//
// 参数: 	ReceByte：接收字节
//
// 返回值:  FALSE: 报文没有收完
//		  TRUE : 报文已收完且InitPoint()
//
// 备注:
//---------------------------------------------------------------
WORD DoReceProc_Dlt698_WH(BYTE ReceByte, TSerial *p)
{
	// 搜索同步头
	if (p->ProStepDlt698_45 == 0)
	{
		if (ReceByte == 0x68)
		{
			// Dlt698.45规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt698_45 = p->RXRPoint;

			// 操作指针
			p->ProStepDlt698_45++;
		}
	}
	else if (p->ProBuf[p->BeginPosDlt698_45] == 0x68)
	{
		// 操作指针
		p->ProStepDlt698_45++;
		if (p->ProStepDlt698_45 < 3) // 如果没收到3字节，即长度域没收全，则不处理
		{
		}
		else if (p->ProStepDlt698_45 == 3) // 已经收到第3个数了，即长度L
		{
			p->wLen = (ReceByte * 0x100 + p->ProBuf[p->BeginPosDlt698_45 + 1]);
			if (p->wLen > (UART_BUFF_MAX_LEN - 2)) //(UART_BUFF_MAX_LEN-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			if (p->wLen < (10 + 6)) // 长度域最小为多少，通信地址是6字节
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if (p->ProStepDlt698_45 == 4) // 已经收到第4个数了，即控制域C
		{
			if (!(ReceByte & 0x80)) // 如果不是服务器回复报文重搜起始符
			{
				// DoSearch_68_DLT698(p);

				return FALSE;
			}
			else if (ReceByte == 0x01)
			{
				// 只判断是否 是链路管理 其余均返回false
			}
			else
			{
				return FALSE;
			}
			// //功能码判断，暂没做
			// //1	链路管理	链路连接管理（登录，心跳，退出登录）
			// //3	用户数据	应用连接管理及数据交换服务
			// if( (ReceByte & 0x07) != 0x03 )//最好应答 ERROR-Response ，但矩泉陈兴东不应答鼎信测试软件过了 2017-7-29 wlk
			// {
			// 	DoSearch_68_DLT698(p);
			// 	return FALSE;
			// }
		}
		else if (p->ProStepDlt698_45 == 5) // 已经收到第5个数了，即服务器地址SA的第1字节
		{
			// a)	bit0…bit3：为地址的字节数，取值范围：0…15，对应表示1…16个字节长度；
			// b)	bit4…bit5：逻辑地址；
			// c)	bit6…bit7：为服务器地址的地址类型，0表示单地址，1表示通配地址，2表示组地址，3表示广播地址。
			p->Addr_Len = (ReceByte & 0xf) + 1;
			if ((ReceByte / 0x40) <= 1) // 如果是单地址、通配地址
			{
				if (p->Addr_Len != 6) // 电表地址是6字节BCD，即12位BCD码
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}
			}
		}
		else if (p->ProStepDlt698_45 == (8 + p->Addr_Len)) // 地址收完了，HCS收完了，判断HCS是否正确
		{
			if (p->Addr_Len > 6)
			{
				DoSearch_68_DLT698(p);

				return FALSE;
			}
			// //一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if (JudgeDlt698_45_HCS_FCS(0, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			// for (BYTE i = 0; i < p->Addr_Len; i++)不判断地址
			// {
			// 	if (reverAddr[i] != p->ProBuf[p->BeginPosDlt698_45 + 5 + i])
			// 	{
			// 		// DoSearch_68_DLT698(p);
			// 		nwy_ext_echo("\r\n addr is error \r\n");
			// 		return FALSE;
			// 	}
			// }
			// nwy_ext_echo("\r\n addr is correct \r\n");
			// //判断通信地址是否正确
			// if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			// {

			// }
		}
		else if (p->ProStepDlt698_45 == (1 + p->wLen)) // 收到帧结束符 16H了，判断FCS是否正确
		{
			// 一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if (JudgeDlt698_45_HCS_FCS(1, p) == FALSE)
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if (p->ProStepDlt698_45 == (2 + p->wLen)) // 收到帧结束符 16H了,暂没判断16H是否接收正确
		{
			if (ReceByte == 0x16)
			{
				Deal_BLETran698(p);
				// 处理完成
				InitPoint(p);
				// nwy_ext_echo("\r\n  698 RECEIVE process over \r\n");

				return TRUE;
			}
			else
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else
		{
			if (p->ProStepDlt698_45 >= UART_BUFF_MAX_LEN) //(UART_BUFF_MAX_LEN-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				nwy_ext_echo("\r\n3 pro setp is set to 0 ");
				p->ProStepDlt698_45 = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}
//--------------------------------------------------
// 功能描述:  维护口规约接收处理
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_ReceData_WHTask(void)
{
	BYTE Result = FALSE;
	WORD ReceNum = 0;

	ReceNum = ParaSerial.RXWPoint;
	while (ReceNum != ParaSerial.RXRPoint)
	{
		nwy_start_timer(Ble_Recv_ByteTimeout_timer, 500);
		Result = DoReceProc_Dlt645_WH(ParaSerial.ProBuf[ParaSerial.RXRPoint], &ParaSerial);
		if (Result == FALSE)
		{
			Result = DoReceProc_Dlt698_WH(ParaSerial.ProBuf[ParaSerial.RXRPoint], &ParaSerial);
		}
		if (Result == FALSE)
		{
			ParaSerial.RXRPoint++;
			if (ParaSerial.RXRPoint >= UART_BUFF_MAX_LEN)
			{
				ParaSerial.RXRPoint = 0;
			}
		}
		else // 接受到完整报文
		{
			nwy_stop_timer(Ble_Recv_ByteTimeout_timer);
			break;
		}
	}
	if (BleRecvByteTimeoutFlag == 1)
	{
		BleRecvByteTimeoutFlag = 0;
		InitPoint(&ParaSerial);
	}
}
//--------------------------------------------------
//功能描述:  操作文件系统
//         
//参数:      Operation 1写操作 0 读操作
//           addr 要操作的文件地址
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_OperateFileSystem( BYTE Operation,int addr,BYTE*Buf,WORD bLen)//此函数测试没问题
{
	int fd;

	fd = nwy_sdk_fopen(filepara_name, NWY_RDWR);
	if (fd >= 0)
	{
		if(nwy_sdk_fseek(fd,addr,NWY_SEEK_SET) == addr)
		{
			if (Operation == WRITE)//写操作
			{
				if(nwy_sdk_fwrite(fd,Buf,bLen) == bLen )
				{
					nwy_sdk_fclose(fd);
					nwy_ext_echo("\r\n write file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
			else
			{
				if (nwy_sdk_fread(fd,Buf,bLen) == bLen)
				{
					nwy_sdk_fclose(fd);
					nwy_ext_echo("\r\n read file success!!!");
					return TRUE;
				}
				else
				{
					nwy_sdk_fclose(fd);
					return FALSE;
				}
			}
		}
		else
		{
			nwy_sdk_fclose(fd);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  上电创建或打开时间para文件
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_PowerOnCreatParaTable( void )
{
	int fd;

	fd = nwy_sdk_fopen(filepara_name, NWY_RDWR);
	if(fd < 0)
	{
		fd = nwy_sdk_fopen(filepara_name, NWY_CREAT | NWY_RDWR);
		if (fd >= 0)
		{
			api_OperateFileSystem(WRITE,0,(BYTE*)&SafeConst,(sizeof(TSafeMem))); 
		}
		return TRUE;
	}
	else
	{
		nwy_sdk_fclose(fd);
		return FALSE;
	}
}
//--------------------------------------------------
// 功能描述:根据底层版本设置芯片类型
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void api_Get4gChipType(void)
{
	char buf2[100] = {0};

	nwy_dm_get_inner_version(buf2, 100);
	if (strstr(buf2, "BLE") != NULL)
	{
		chipType = NWY_082AS1;
		nwy_ext_echo("\r\nchip type is NWY_082AS1");
	}
	else 
	{
		chipType = NWY_092AS1;
		nwy_ext_echo("\r\nchip type is NWY_092AS1");
	}
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
void api_ReadSystemFiletoRam(eREAD_METER_BIT eBit)
{
	int fd;
	ServerParaRam_t TempServerpara[2];
	WORD crc16_1 = 0;
	WORD crc16_2 = 0;

	memset((BYTE *)&TempServerpara[0].ServerPort, 0, sizeof(ServerParaRam_t) * 2);
	fd = nwy_sdk_fopen(filepara_name, NWY_RDONLY);
	if (nwy_sdk_fread(fd, (char *)&TempServerpara[0].ServerPort, 2 * sizeof(ServerParaRam_t)) == (2 * sizeof(ServerParaRam_t)))
	{
		crc16_1 = CalCRC16((BYTE *)&TempServerpara[0].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[ePARA_TCP].wCrc)));
		crc16_2 = CalCRC16((BYTE *)&TempServerpara[1].ServerPort, (sizeof(ServerParaRam_t) - sizeof(Serverpara[ePARA_MQTT].wCrc)));
		nwy_ext_echo("\r\n2 calculated serverpara[0].wCrc is %04x", crc16_1);
		nwy_ext_echo("\r\n2 calculated Serverpara[ePARA_MQTT].wCrc is %04x", crc16_2);

		if (crc16_1 == (TempServerpara[0].wCrc))
		{
			if (eBit == eBIT_TCPNET)
			{
				memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerPort, (BYTE *)&TempServerpara[0].ServerPort, (sizeof(TempServerpara[0].ServerPort) + sizeof(TempServerpara[0].ServerIP)));
				nwy_ext_factory("\r\n%d", TempServerpara[0].ServerPort);
				nwy_ext_factory("\r\n%s", TempServerpara[0].ServerIP);
			}
			else if (eBit == eBIT_TCPUSER)
			{
				memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerUserName[0], (BYTE *)&TempServerpara[0].ServerUserName[0], (sizeof(TempServerpara[0].ServerUserName) + sizeof(TempServerpara[0].ServerUserPwd)));
				nwy_ext_factory("\r\n%s", TempServerpara[0].ServerUserName);
				nwy_ext_factory("\r\n%s", TempServerpara[0].ServerUserPwd);
			}
		}
		else
		{
			if (eBit == eBIT_TCPNET)
			{
				memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerPort, (BYTE *)&ServerparaConst[0].ServerPort, (sizeof(TempServerpara[0].ServerPort) + sizeof(TempServerpara[0].ServerIP)));
			}
			else if (eBit == eBIT_TCPUSER)
			{
				memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerUserName[0], (BYTE *)&ServerparaConst[0].ServerUserName[0], (sizeof(TempServerpara[0].ServerUserName) + sizeof(TempServerpara[0].ServerUserPwd)));
			}
		}
		if (crc16_2 == (TempServerpara[1].wCrc))
		{
			if (eBit == eBIT_MQTTNET)
			{
				memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerPort, (BYTE *)&TempServerpara[1].ServerPort, (sizeof(TempServerpara[1].ServerPort) + sizeof(TempServerpara[1].ServerIP)));
				nwy_ext_factory("\r\n%d", TempServerpara[1].ServerPort);
				nwy_ext_factory("\r\n%s", TempServerpara[1].ServerIP);
			}
			else if (eBit == eBIT_MQTTUSER)
			{
				memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerUserName[0], (BYTE *)&TempServerpara[1].ServerUserName[0], (sizeof(TempServerpara[1].ServerUserName) + sizeof(TempServerpara[1].ServerUserPwd)));
				nwy_ext_factory("\r\n%s", TempServerpara[1].ServerUserName);
				nwy_ext_factory("\r\n%s", TempServerpara[1].ServerUserPwd);
			}
		}
		else
		{
			if (eBit == eBIT_MQTTNET)
			{
				memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerPort, (BYTE *)&ServerparaConst[1].ServerPort, (sizeof(TempServerpara[1].ServerPort) + sizeof(TempServerpara[1].ServerIP)));
			}
			else if (eBit == eBIT_MQTTUSER)
			{
				memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerUserName[0], (BYTE *)&ServerparaConst[1].ServerUserName[0], (sizeof(TempServerpara[1].ServerUserName) + sizeof(TempServerpara[1].ServerUserPwd)));
			}
		}
	}
	else // 读取失败 或没有文件 也要用const
	{
		if (eBit == eBIT_TCPNET)
		{
			memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerPort, (BYTE *)&ServerparaConst[0].ServerPort, (sizeof(TempServerpara[0].ServerPort) + sizeof(TempServerpara[0].ServerIP)));
		}
		else if (eBit == eBIT_TCPUSER)
		{
			memcpy((BYTE *)&Serverpara[ePARA_TCP].ServerUserName[0], (BYTE *)&ServerparaConst[0].ServerUserName[0], (sizeof(TempServerpara[0].ServerUserName) + sizeof(TempServerpara[0].ServerUserPwd)));
		}
		else if (eBit == eBIT_MQTTNET)
		{
			memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerPort, (BYTE *)&ServerparaConst[1].ServerPort, (sizeof(TempServerpara[1].ServerPort) + sizeof(TempServerpara[1].ServerIP)));
		}
		else if (eBit == eBIT_MQTTUSER)
		{
			memcpy((BYTE *)&Serverpara[ePARA_MQTT].ServerUserName[0], (BYTE *)&ServerparaConst[1].ServerUserName[0], (sizeof(TempServerpara[1].ServerUserName) + sizeof(TempServerpara[1].ServerUserPwd)));
		}
	}
	nwy_sdk_fclose(fd);
}

#if (BLE_UART_WH == YES)
void nwy_ble_conn_status_func(void)
{
	int conn_status = 0;

	conn_status = nwy_ble_get_conn_status();
	if (conn_status != 0)
	{
		nwy_ext_echo("\r\n Ble connect,status:%d", conn_status);
	}
	else
	{
		nwy_ext_echo("\r\n Ble disconnect,status:%d", conn_status);
	}

	return;
}

void nwy_Ble_Recv_timer_cb(void *type)
{
	nwy_ext_echo("\r\n Ble_Recv_time out ");
	BleRecvByteTimeoutFlag = 1;
}

void nwy_ble_recv_data_func(void)
{
	DWORD length = 0;
	BYTE *precv = NULL;
	WORD wLen = 0, wLen1 = 0;
	// WORD i = 0;
	nwy_osiEvent_t event;

	memset(&event, 0, sizeof(event));

	length = (DWORD)nwy_ble_receive_data(0);
	precv = (BYTE *)nwy_ble_receive_data(1);
	if ((NULL != precv) && (0 != length))
	{
		nwy_ext_echo("\r\n nwy_bel_receive_data:%d\r\n", length);
		// for (i = 0; i < length; i++)
		// {
		// 	nwy_ext_echo(" 0x%02x ", precv[i]);
		// }
	}
	else
	{
		nwy_ext_echo("\r\nnwy_ble receive data is null.");
	}
	wLen = ParaSerial.RXWPoint + length;
	if (length >= UART_BUFF_MAX_LEN)
	{
		length = UART_BUFF_MAX_LEN;
	}
	if (wLen < UART_BUFF_MAX_LEN)
	{
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], precv, length);
		ParaSerial.RXWPoint += length;
	}
	else
	{
		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], precv, wLen1);
		memcpy((char *)&ParaSerial.ProBuf[0], precv + wLen1, length - wLen1);
		ParaSerial.RXWPoint = length - wLen1;
	}

	nwy_ble_receive_data(2);
	WhFlag |= BIT0;
	return;
}

// static void nwy_uart_wh_handle(const char *str, uint32_t length)
// {
// 	WORD wLen, wLen1;

// 	wLen = ParaSerial.RXWPoint + length;
// 	if (length >= UART_BUFF_MAX_LEN)
// 	{
// 		length = UART_BUFF_MAX_LEN;
// 	}
// 	if (wLen < UART_BUFF_MAX_LEN)
// 	{
// 		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], str, length);
// 		ParaSerial.RXWPoint += length;
// 	}
// 	else
// 	{
// 		wLen1 = UART_BUFF_MAX_LEN - ParaSerial.RXWPoint;
// 		memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], str, wLen1);
// 		memcpy((char *)&ParaSerial.ProBuf[0], str + wLen1, length - wLen1);
// 		ParaSerial.RXWPoint = length - wLen1;
// 	}
// 	WhFlag |= (BIT1);
// }

// void InitUartWH(void)
// {
// 	UartWh = nwy_uart_init(NWY_NAME_UART2, 1); // 初始化
// 	nwy_uart_set_baud(UartWh, 9600);		   // 设置默认波特率
// 	nwy_uart_set_para(UartWh, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
// 	nwy_uart_reg_recv_cb(UartWh, nwy_uart_wh_handle);
// }
//--------------------------------------------------
// 功能描述:  开通蓝牙口 用户设置参数用
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void BuleToothWH(void)
{
	char dev_name[30] = "wsd";
	char *ble_name = NULL;
	char bleflag = 0;
	char bFailcount = 0;

	strcat(dev_name, (char *)&LtuAddr[5]);
	nwy_ble_set_device_name(dev_name);
	nwy_ext_echo("nwy_ble_enable(%d) \r\n", nwy_ble_enable());
	nwy_sleep(3000);
	// nwy_ext_echo("nwy_ble_set_adv(%d) \r\n", nwy_ble_set_adv(1));
	while (bleflag == 0)
	{
		bleflag = nwy_ble_set_adv(1);
		if (bleflag == 1)
		{
			nwy_ext_echo("BLE broadcast \r\n");
			break;
		}
		else
		{
			bFailcount++;
			nwy_sleep(200);
		}
		if (bFailcount > 10)
		{
			break;
		}
	}
	ble_name = nwy_ble_get_device_name();
	nwy_ext_echo("\n new_set_dev_name:(%s) \r\n", ble_name);
	nwy_ble_register_callback(nwy_ble_recv_data_func);
	nwy_ble_conn_status_cb(nwy_ble_conn_status_func);
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
BYTE SendBleDataInPiece(char *data, WORD total_len)
{
	WORD BytesSent = 0; // 已发送的字节数
	WORD ret = 0;		// 存储每次发送的返回值
	WORD ChunkSize = 0;

	while (BytesSent < total_len)
	{
		// 计算剩余数据是否超过 MAX_BLE_PACKET_SIZE
		if ((total_len - BytesSent) > MAX_BLE_PACKET_SIZE)
		{
			ChunkSize = MAX_BLE_PACKET_SIZE;
		}
		else
		{
			ChunkSize = (total_len - BytesSent);
		}
		// 发送当前块数据
		ret = nwy_ble_send_data(ChunkSize, data + BytesSent);
		nwy_sleep(100);
		if (ret != 1)
		{
			nwy_ext_echo(" \r\nsending ble data Error, ret = %d", ret);
			return FALSE;
		}
		// 更新已发送的字节数
		BytesSent += ChunkSize;
	}
	return TRUE; // 全部发送成功
}
#endif
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void Factory_Task(void *param)
{
	tTranData TransDataToBle;
	#if( BLE_UART_WH == YES)
	WORD i = 0;
	BYTE ret = 0;
	#endif
	nwy_osiEvent_t event;
	memset(&TransDataToBle, 0, sizeof(tTranData));
	InitPoint(&ParaSerial);
	memset((char *)&ParaSerial.ProBuf[0], 0, UART_BUFF_MAX_LEN);
	nwy_usb_serial_reg_recv_cb((nwy_sio_recv_cb_t)nwy_ext_sio_data_cb);
	#if (BLE_UART_WH == YES)
	nwy_sleep(10000);
	// InitUartWH();
	BuleToothWH();
	#endif

	while (1)
	{
		ThreadRunCnt[eFactoryParaThread]++;
		api_ReceData_WHTask();
		#if( BLE_UART_WH == YES)
		if (nwy_get_msg_que(TranDataToFactoryMessageQueue, &TransDataToBle, 0xffffffff) == TRUE)
		{
			nwy_ext_echo("\r\nble send data leng :%d", TransDataToBle.len);
			ret = SendBleDataInPiece((char *)TransDataToBle.buf, TransDataToBle.len);
			nwy_ext_echo("\r\nret %d ", ret);
			nwy_ext_echo("\r\nuart to factory data: ");
			for (i = 0; i < TransDataToBle.len; i++)
			{
				nwy_ext_echo(" %02X ", TransDataToBle.buf[i]);
			}
		}
		else
		{
		}
		#endif
		if (nwy_get_msg_que(TcpDataToModuleMsgQue, &TransDataToBle, 0xffffffff))
		{
			memcpy((char *)&ParaSerial.ProBuf[ParaSerial.RXWPoint], TransDataToBle.buf, TransDataToBle.len);
			ParaSerial.RXWPoint += TransDataToBle.len;
			WhFlag |= BIT2;
		}
		nwy_wait_thead_event(factorypara_thread, &event, 5); // 可以删掉 等待事件
	}
}