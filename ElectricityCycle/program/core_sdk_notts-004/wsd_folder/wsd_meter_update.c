//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_meter_update.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define LEN_OF_ONE_FRAME						192						//与原先升级流程保持一致
#define ADDR_BITS								0x0F					//地址掩码
#undef  LOBYTE
#define LOBYTE(w)								(BYTE)(w)				//取一个WORD的低字节
#undef  HIBYTE
#define HIBYTE(w)								(BYTE)((WORD)(w) >> 8) //取一个WORD的高字节
#undef  LOWORD
#define LOWORD(l)								(WORD)(l)				//取一个DWORD的低WORD
#undef  HIWORD
#define HIWORD(l)								(WORD)((DWORD)(l) >> 16)//取一个DWORD的高WORD
#define OBJ_START_FLAG							0x68					//起始字符
#define APDU_DATA_MAX_LEN						500						// apdu数据最大长度
#define FILE_BLOCK_SIZE							400						//和帧其他内容不可超过MAX_FRAME_LEN,最好不要设太大否则需要分帧传输机制,也不能太小，查询结果返回帧字节数不能超过256
#define MAX_FRAME_LEN							512 
#define OMD_OFFSET								17						//无逻辑地址时的偏移
#define VERSION_LEN								4						// 长度和下面的值对应
#define VERSION									"V1.1"
#define MAX_ERROR_FRAME_NUM 					256
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
typedef enum TRANS_DIR
{
	TD12,
	TD21
} TRANS_DIR; //转换方向: TD12:1形参->2形参; TD21:2->1
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
T_Iap_645 tIap_645;
T_Iap_698 tIap_698;
BYTE MeterUpgradeTimeUpFlag = 0; //表升级全过程1表示时间到
char upgrade_file_path_645[FILENAME_LEM] = {0};
char upgrade_file_path_698[FILENAME_LEM] = {0};
BYTE OpenFactoryStep = eIDLE;
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
static BYTE	MoniIapNo = 0;//监测单元当前升级步骤
static WORD MoniFrameNo = 0;//监测单元升级数据包帧序号 //可以作为文件偏移使用
static WORD errorFrameCount = 0;
static BOOL RetryPhaseFlag = 0; // 失败重发帧阶段,
static WORD errorFrames[MAX_ERROR_FRAME_NUM] = {0XFFFF};
static WORD retryIndex = 0;
static BYTE CheckResultRetryCount = 0;
int MeterUpgradeFD = -1;		//文件描述符 链接 电表升级文件
BYTE TimeoutRetryNum = 0;		 // 重发次数
static BYTE sCRCHi =0xFF; // high byte of CRC initialized 	//此处应与与被升级电表一致！！！之前不一致导致校验不一样
static BYTE sCRCLo = 0xFF; // low byte of CRC initialized
// -----------------------------------------------------------------------------
// DESCRIPTION: CRC校验的高位字节表
// -----------------------------------------------------------------------------
static const unsigned char auchCRCHi[] = 
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40
};
// -----------------------------------------------------------------------------
// DESCRIPTION: CRC校验的低位字节表
// -----------------------------------------------------------------------------
static const unsigned char auchCRCLo[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5,
	0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B,
	0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
	0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
	0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2,
	0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F,
	0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB,
	0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25,
	0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C,
	0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8,
	0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D,
	0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73,
	0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57,
	0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A,
	0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E,
	0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
	0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
//FCS lookup table as calculated by the table generator.
static const WORD fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
// 函数功能: 退出、进入698升级变量处理
//
// 参数: 无
//
// 返回值:  无
//
// 备注:
//-----------------------------------------------
void InitUpgradeVariable698(void)
{
	WORD i = 0;

	for (i = 0; i < MAX_ERROR_FRAME_NUM; i++)
	{
		errorFrames[i] = 0xFFFF;
	}
	errorFrameCount = 0;
	MeterUpgradeTimeUpFlag = 0;
	retryIndex = 0;
	CheckResultRetryCount = 0;
	nwy_stop_timer(uart_timer);
	nwy_stop_timer(meter_upgrade_timer);
	TimeoutRetryNum = 0;
	RetryPhaseFlag = 0;
	MoniIapNo = 0;
	MoniFrameNo = 0;
	OpenFactoryStep = eIDLE;
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
	nwy_ext_echo("\r\n init upgrade variable success ");
}
//-----------------------------------------------
// 函数功能: 进入升级变量处理
//
// 参数: 	无
//
// 返回值:  无
//
// 备注:
//-----------------------------------------------
void EnterUpgrade698(void)
{
	InitUpgradeVariable698();
	tIap_698.dwIapFlag = BASEMETER_UPGRADES; //放最后，确保置标志前清完变量
}
//-----------------------------------------------
// 函数功能: 退出升级变量处理
//
// 参数: 	无
//
// 返回值:  无
//
// 备注:
//-----------------------------------------------
void ExitUpgrade698(eUpgradeResult result)
{
	InitUpgradeVariable698();
	memset(&tIap_698,0,sizeof(tIap_698));
	if (NWY_SUCESS == nwy_sdk_fclose(MeterUpgradeFD))
	{
		nwy_ext_echo("\r\n close file success");
	}
	MeterUpgradeFD = -1;
	if(nwy_sdk_file_unlink(upgrade_file_path_698) == NWY_SUCESS)
	{
		nwy_ext_echo("\r\n delete upgrade file success");
	}
	else
	{
		nwy_ext_echo("\r\n delete upgrade file err");
	}
	memset(upgrade_file_path_698, 0, sizeof(upgrade_file_path_698));
	nwy_ext_echo("\r\n exit 698 upgrade ");
	if (nwy_put_msg_que(UpgradeResultMessageQueue, &result, 0xffffffff) == TRUE)
	{
		nwy_ext_echo("\r\neUpgrade result put msg que success");
	}
	else
	{
		nwy_ext_echo("\r\neUpgrade result put msg que err");
	}
}
//-----------------------------------------------
// 函数功能:
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void handleWriteFileSuccess(void)
{
	if (MoniIapNo == 1)
	{
		nwy_ext_echo("\r\n MoniIapNo = %d", MoniIapNo);
		if (MoniFrameNo < tIap_698.wAllFrameNo) // tIap_698.wAllFrameNo
		{	
			if (RetryPhaseFlag == 0) // 非重发阶段
			{
				MoniFrameNo++;
				nwy_ext_echo("\r\n next FrameNo  is %d,all frame no:%d", MoniFrameNo, tIap_698.wAllFrameNo);
				if (MoniFrameNo == tIap_698.wAllFrameNo)
				{
					MoniIapNo = 2;
				}
			}
			else // 重发阶段
			{
				if (errorFrameCount > 0)
				{
					MoniFrameNo = errorFrames[retryIndex++];
					errorFrameCount--;
				}
				else // 所有错误帧重发完
				{
					MoniIapNo = 2;
					nwy_ext_echo("\r\n all retry frames  over ");
				}
			}
		}
	}
}
//-----------------------------------------------
// 函数功能:
//
// 参数:
//
// 返回值:
//
// 备注:
//-----------------------------------------------
void handleCheckResult(BYTE *buf)
{
	WORD frameNo = 0;
	WORD byteIndex = 0; // buf 中当前检查的字节位置
	BYTE bitIndex = 0;  // buf[byteIndex] 中的比特位索引
	WORD offset = 0;
	BYTE CheckFrameResultOffest = 0; // 返回传输状态帧状态数据的偏移位置
	RetryPhaseFlag = 0;
	nwy_ext_echo("\r\n check result replied ");
	if(tIap_698.IsModuleUpgrade == 0)
	{
		offset = OMD_OFFSET ;		 // omd位置
	}
	else if(tIap_698.IsModuleUpgrade == 1)
	{
		offset = OMD_OFFSET + 1 ;		 // omd位置
	}
	// 判断状态数据偏移位置
	if ((buf[offset + 6]) == 0x82) // 返回块长度域
	{
		CheckFrameResultOffest = offset + 6 + 3;
	}
	else if ((buf[offset + 6]) == 0x81) // 返回文件长度域
	{
		CheckFrameResultOffest = offset + 6 + 2;
	}
	else
	{
		CheckFrameResultOffest = offset + 6 + 1;
	}
	nwy_ext_echo("\r\n CheckFrameResultOffest is %d", CheckFrameResultOffest);
	// if (tIap_698.wAllFrameNo % 8 == 0)
	// {
	for (frameNo = 0; frameNo < tIap_698.wAllFrameNo; frameNo++)
	{
		// 获取当前帧的结果位
		byteIndex = frameNo / 8; // 当前帧所在的 buf 的字节
		bitIndex = frameNo % 8;  // 当前帧所在的字节的比特位置

		// 检查传输结果，如果为0表示失败
		if (!(buf[CheckFrameResultOffest + byteIndex] & (1 << (7 - bitIndex))))
		{
			// 保存失败帧号
			errorFrames[errorFrameCount++] = frameNo; // 记录帧号，从0开始
			nwy_ext_echo("\r\n error frame no is %d", frameNo);
			RetryPhaseFlag = 1;
		}
	}

	if (RetryPhaseFlag == 0)
	{
		if (MoniIapNo == 2) // 升级命令收到肯定回复
		{
			MoniIapNo = 3;
			nwy_ext_echo("\r\n enter step 3");
		}
	}
	else // 有错误
	{
		CheckResultRetryCount++;
		nwy_ext_echo("\r\n retry count is %d", CheckResultRetryCount);
		if (CheckResultRetryCount >= 3)
		{
			ExitUpgrade698(eUpgradeCheckResultErr);

			nwy_ext_echo("\r\n retry failed exit upgrade process");
		}
		else
		{
			MoniIapNo = 1; // 回到第1步重发
			errorFrameCount--;
			nwy_ext_echo("\r\nerror Frame Count left  is %d", errorFrameCount);
			retryIndex = 0;
			MoniFrameNo = errorFrames[retryIndex++];
			nwy_ext_echo("\r\nretry FrameNo  is %d,all frame no:%d", MoniFrameNo, tIap_698.wAllFrameNo);
		}
	}
}
//-----------------------------------------------
// 函数功能:处理698升级相关的接收报文
//
// 参数:无
//
// 返回值:  true:处理完升级相关应答帧，false：收到其他698响应报文
//
// 备注:
//-----------------------------------------------
BOOL ProcUpgradeResponse698(TSerial *p)
{
	BYTE *buf = p->ProBuf;
	WORD offset = 0;
	BYTE result = TRUE;

	if(tIap_698.IsModuleUpgrade == 0)
	{
		offset = OMD_OFFSET ;		 // omd位置
	}
	else if(tIap_698.IsModuleUpgrade == 1)
	{
		offset = OMD_OFFSET + 1 ;		 // omd位置
	}
	if ((*(DWORD *)(buf + offset)) == 0x000E01F0) //启动传输回复
	{
		if (buf[offset + 4] == 0x00)
		{
			MoniIapNo = 1;
			nwy_ext_echo("\r\n MoniIapNo = %d", MoniIapNo);
		}
		else //dar错误
		{
			ExitUpgrade698(eUpgradeDARErr);
			nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
		}
	}
	else if ((*(DWORD *)(buf + offset)) == 0x000801F0) //写文件回复
	{
		nwy_ext_echo("\r\nrec write file result");
		if (buf[offset + 4] == 0x00)
		{
			handleWriteFileSuccess();
		}
		else //
		{
			ExitUpgrade698(eUpgradeDARErr);
			nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
		}
	}
	else if ((*(DWORD *)(buf + offset)) == 0x000401F0) //查询结果回复
	{
		tIap_698.CheckResultCnt++;
		nwy_ext_echo("\r\nrec check result,cnt:%d",tIap_698.CheckResultCnt);
		if(tIap_698.CheckResultCnt >= 3)
		{
			handleCheckResult(buf);
		}
		
	}
	else if ((*(DWORD *)(buf + offset)) == 0x000D01F0) //执行升级回复
	{
		nwy_ext_echo("\r\nrec exe upgrade ");
		if (buf[offset + 4] == 0x00)
		{
			if (MoniIapNo == 3) // 升级命令收到肯定回复
			{
				ExitUpgrade698(eUpgradeExeSuc);
				nwy_ext_echo("\r\nUpgradeExeSuc");
				// 理论到这数据全部传输完毕，传给主站升级成功信息
			}
		}
		else
		{
			ExitUpgrade698(eUpgradeDARErr);
			nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
		}
	}
	else if ((*(DWORD *)(buf + offset )) == 0x00022033) //模组上报
	{
		// nwy_ext_echo("\r\nrec 0x00022033 ");
		// ExitUpgrade698(eUpgradeExeSuc);
		// nwy_ext_echo("\r\nUpgradeExeSuc");//退出升级后，后续三帧的模组重启事件变更上报将进入下面分支
	}
	else
	{
		nwy_ext_echo("\r\nupgrade response other omd"); 
		result = FALSE;
	}
	if (result == TRUE) //收到升级相关的响应报文
	{
		nwy_stop_timer(uart_timer);
		TimeoutRetryNum = 0;
		api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	}

	return result;
}
//-----------------------------------------------
// 函数功能: 645规约升级接收处理
//
//参数: 	无
//			
//返回值:  	BOOL
//
//备注:   
//-----------------------------------------------
// BOOL GyRxMonitor( void )
// {
// 	TSerial *p = &Serial;
// 	T645FmRx *pRxFm=(T645FmRx *)p->ProBuf;

// 	if(pRxFm->byCtrl == 0x9D)//升级监测单元收到肯定回复
// 	{
// 		if(MoniIapNo == 0)
// 		{
// 			MoniIapNo = 1;
// 			nwy_ext_echo("\r\n MoniIapNo = %d",MoniIapNo);
// 		}
// 		else if(MoniIapNo == 1)
// 		{
// 			nwy_ext_echo("\r\n 2srMoniIapNo = %d",MoniIapNo);
// 			if(MoniFrameNo < tIap_645.wAllFrameNo)//tIap_645.wAllFrameNo
// 			{
// 				MoniFrameNo++;
// 				nwy_ext_echo("\r\n RECEIVE no is %d",MoniFrameNo);
// 				if(MoniFrameNo == tIap_645.wAllFrameNo)
// 				{
// 					MoniIapNo = 2;
// 				}
// 			}
// 		}
// 		else if(MoniIapNo == 2)//升级命令收到肯定回复
// 		{
// 			MoniIapNo = 0;
// 			MoniFrameNo = 0;
// 			api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME); // 升级完应该置为可发送报文状态
// 			nwy_ext_echo("\r\n dbstart update");
// 			tIap_645.dwIapFlag = 0;
// 			nwy_sdk_file_unlink(upgrade_file_path_645);
// 			//理论到这数据全部传输完毕，传给主站升级成功信息
// 			return TRUE;
// 		}
// 			return TRUE;
// 	}
// 	else 
// 	{
// 		return FALSE;
// 	}
// }

//-----------------------------------------------
//功能描述  : 645规约预处理,只用来判断升级
//
//参数  :    TSerial *p[in]
//
//返回值:     BOOL （TRUE-需要做转发或者报文重组处理）
//备注内容  :  无
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p)
{	
	// if (GyRxMonitor() == TRUE)
	// {
		// nwy_stop_timer(uart_timer); //处理完成关定时器
		// api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
		// return TRUE;
	// }
	// else
	{
		return FALSE;
	}	
}
//--------------------------------------------------
//功能描述:  计算整个文件的CRC16校验
//
//参数:
//
//返回值:    CRC16校验
//
//备注内容:
//--------------------------------------------------
unsigned short FileCalCRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	unsigned uIndex;		// will index into CRC lookup table

	while (usDataLen--)
	{
		// calculate the CRC
		uIndex = sCRCHi ^ (unsigned char)(*puchMsg++);
		sCRCHi = sCRCLo ^ auchCRCHi[uIndex];
		sCRCLo = auchCRCLo[uIndex];
	}
	return (sCRCHi << 8 | sCRCLo);
}
//--------------------------------------------------
//功能描述:  计算CRC16校验
//
//参数:
//
//返回值:    CRC16校验
//
//备注内容:
//--------------------------------------------------
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen )
{
	unsigned char uchCRCHi;// high byte of CRC initialized
	unsigned char uchCRCLo;// low byte of CRC initialized
	unsigned uIndex;// will index into CRC lookup table

	uchCRCHi = 0xFF;
	uchCRCLo = 0xFF;

	while ( usDataLen-- )
	{
		// calculate the CRC
		uIndex = uchCRCHi^(unsigned char)(*puchMsg++);
		uchCRCHi = uchCRCLo^auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}
//--------------------------------------------------
//功能描述:  对报文进行加0x33处理
//         
//参数:      BYTE *pBuf[in]:输入缓冲pBuf[0]为68H
//         
//返回值:   void 
//         
//备注内容:  无
//--------------------------------------------------
void DataPlus33H( BYTE *pBuf )
{
	WORD i;
	for(i=10;i<(10+pBuf[9]);i++)
	{
		pBuf[i]=(pBuf[i]+0x33);
	}
}
//--------------------------------------------------
//功能描述:  计算整个升级文件的校验
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  ComputeFileCheckSum( void )
{
	WORD i;

	BYTE Buf[LEN_OF_ONE_FRAME] ={0};
	for (i = 0; i < tIap_645.wAllFrameNo; i++)
	{
		nwy_sdk_fseek(MeterUpgradeFD, LEN_OF_ONE_FRAME * i, NWY_SEEK_SET);
		nwy_sdk_fread(MeterUpgradeFD, &Buf[0], LEN_OF_ONE_FRAME);
		tIap_645.wCrc16 = FileCalCRC16(&Buf[0], LEN_OF_ONE_FRAME);
	}
	nwy_ext_echo("\r\n%x",tIap_645.wCrc16);
}
//---------------------------------------------------------------
//函数功能: 发送升级文件
//
//参数: 	
//                   
//返回值:  
//
//备注:   
//---------------------------------------------------------------
// void SendIapData645(void)
// {
// 	TSerial *p = &Serial;
//   	WORD wLen = 4, wCrc16;

// 	p->TXPoint = 0;
// 	memset(p->ProBuf,0xFE,4);
// 	p->ProBuf[wLen++] = 0x68;
// 	//模块地址
// 	memcpy(&p->ProBuf[wLen],reverAddr,6);   
// 	wLen += 6; 
// 	p->ProBuf[wLen++] = 0x68;
// 	p->ProBuf[wLen++] = 0x1d;
// 	if(MoniIapNo == 0)//组报文发送升级请求，读出升级包总帧数
// 	{
//		 TimeoutRetryNum = 0;
// 		tIap_645.wAllFrameNo =  tIap_645.dwFileSize/LEN_OF_ONE_FRAME;	//？？？240k能整除  不确定以后发的bin文件大小是否固定
// 		//打开文件，下次传输
// 		nwy_ext_echo("\r\n download pack is %s",upgrade_file_path_645);
// 		MeterUpgradeFD = nwy_sdk_fopen(upgrade_file_path_645, NWY_RDONLY);
// 		if (MeterUpgradeFD < 0)
// 		{
// 			nwy_ext_echo("\r\nopen apppack fail");
// 			return;
// 		}
// 		ComputeFileCheckSum();
// 		p->ProBuf[wLen++] = 0x14;
// 		//数据标识
// 		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
// 		{
// 			p->ProBuf[wLen++] = 0x01; 
// 		}
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f;  
// 		//密码及操作者代码
// 		p->ProBuf[wLen++] = 0x02;
// 		memset(&p->ProBuf[wLen],0x00,7);
// 		wLen += 7;
// 		memcpy(&p->ProBuf[wLen], (BYTE *)&tIap_645.dwFileSize, 8);
// 		wLen += 8;
// 		DataPlus33H( &p->ProBuf[4]);
// 		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 30 );
// 		p->ProBuf[wLen++] = 0x16;
// 	}
// 	else if(MoniIapNo == 1)//组报文发送升级包
// 	{
// 		p->ProBuf[wLen++] = 0xd0;
// 		//数据标识
// 		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
// 		{
// 			p->ProBuf[wLen++] = 0x02; 
// 		}
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f;  
// 		//密码及操作者代码
// 		p->ProBuf[wLen++] = 0x02;
// 		memset(&p->ProBuf[wLen],0x00,7);
// 		wLen += 7;
// 		p->ProBuf[wLen++] = (MoniFrameNo%0x0100);
// 		p->ProBuf[wLen++] = (MoniFrameNo/0x0100);
// 		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
// 		{
// 			nwy_sdk_fseek(MeterUpgradeFD, LEN_OF_ONE_FRAME * MoniFrameNo, NWY_SEEK_SET);
// 			nwy_sdk_fread(MeterUpgradeFD, &p->ProBuf[wLen], LEN_OF_ONE_FRAME);
// 			nwy_ext_echo("\r\nsend pack%d", MoniFrameNo);
// 		}
// 		wLen += LEN_OF_ONE_FRAME;
// 		wCrc16 = CalCRC16(&p->ProBuf[28], LEN_OF_ONE_FRAME);
// 		memcpy(&p->ProBuf[wLen], (BYTE *)&wCrc16, sizeof(WORD));
// 		wLen += 2;
// 		DataPlus33H( &p->ProBuf[4]);
// 		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 218 );
// 		p->ProBuf[wLen++] = 0x16;
// 	}
// 	else if(MoniIapNo == 2)//组报文发送开始升级命令
// 	{
// 		p->ProBuf[wLen++] = 0x0d;
// 		//数据标识
// 		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
// 		{
// 			p->ProBuf[wLen++] = 0x03; 
// 		}
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f; 
// 		p->ProBuf[wLen++] = 0x0f;  
// 		//密码及操作者代码
// 		p->ProBuf[wLen++] = 0x02;
// 		memset(&p->ProBuf[wLen],0x00,7);
// 		wLen += 7;
// 		p->ProBuf[wLen++] = 0x55;
// 		DataPlus33H( &p->ProBuf[4]);
// 		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 23 );
// 		p->ProBuf[wLen++] = 0x16;
// 	}

// 	p->SendLength = wLen;
// 	p->RXWPoint = 0;
// 	p->RXRPoint = 0;
// 	nwy_uart_send_data(UART_HD_LTO,p->ProBuf,p->SendLength);
// }
//--------------------------------------------------
//功能描述:  模块给电表升级任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
// void  MeterUpgrade645_Task( void  )
// {
// 	if ((api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE)&&(api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE))
// 	{	
// 		if (api_GetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME))
// 		{
// 			TimeoutRetryNum = 0;
// 			SendIapData645();
// 			api_ClrSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
// 			nwy_stop_timer(uart_timer);
// 			nwy_start_timer(uart_timer, 600);
// 		}
// 		else
// 		{
// 			if (api_GetSysStatus(eSYS_STASUS_TIMER_UP)) // 500ms延时到都没有收到数据
// 			{
// 				api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
// 				TimeoutRetryNum++; // 升级失败
// 				if (TimeoutRetryNum >= 2)
// 				{
// 					TimeoutRetryNum = 0;
// 					MoniIapNo = 0;
// 					MoniFrameNo = 0;
// 					tIap_645.dwIapFlag = 0; // 清除标志  可以给主站发一帧升级失败
// 					nwy_ext_echo("\r\n upgrade fail !!!");
// 				}
// 				api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
// 			}
// 		}
// 	}
// }

//-----------------------------------------------
//函数功能: fcs校验函数
//
//参数:		BYTE *cp  
//			WORD len   
//返回值:	WORD
//		   
//备注:
//-----------------------------------------------
WORD fcs16(BYTE *cp, WORD len)
{
	WORD cpLen;
	WORD fcs;
	
	fcs = 0xFFFF;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	fcs ^= 0xffff; // complement  //此为FCS值
	return fcs;
}
//-----------------------------------------------
// 函数功能: 根据之前的fcs校验值，继续计算fsc校验
//
// 参数:		BYTE *cp[in]	数据指针
//			WORD len[in] 	数据长度
//			LastFcs[in]		上一次计算的校验值，用于本次计算的初始值
//			IsEnd[in]		是否是最后一段数据 TRUE/FALSE 是/否
//
// 返回值:	WORD
//
// 备注:
//-----------------------------------------------
WORD ContinueFcs16(BYTE *cp, WORD len, WORD LastFcs, BOOL IsEnd)
{
	WORD cpLen;
	WORD fcs;

	fcs = LastFcs;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ((fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	if (IsEnd) // 计算最后一段crc校验时执行
	{
		fcs ^= 0xffff; // complement  //此为FCS值
	}

	return fcs;
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
//		
//		
//
// 返回值:	
//
// 备注:
//-----------------------------------------------
WORD gdw698bufLink_TSA(BYTE *pBuf, TSA *psa, TRANS_DIR td)
{ //.链路层需要反转
	WORD tmp = 0, i, num = 0;

	switch(td)
	{
	case TD12:
		memset(psa,0,sizeof(TSA));
		num	= (BYTE)(pBuf[tmp++]&ADDR_BITS)+1;
		if(num)	
		{
			psa->AF = pBuf[tmp-1];
			if(num >= 1)
			{
				for(i=0;i<num;i++)
					psa->addr[i]	= pBuf[tmp++];
			}			
		}
		break;
	case TD21:
		pBuf[tmp++]	= psa->AF;
		for(i=0;i<((psa->AF&ADDR_BITS)+1);i++)
			pBuf[tmp++]	= psa->addr[i];	
		break;
	}	
	return tmp;
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
// 返回值:	
//
// 备注:
//-----------------------------------------------
void ComposeServerAddr(TSA *tSA)
{
	BYTE i = 0;
	if (tIap_698.IsModuleUpgrade == 0)
	{
		tSA->AF = 0x05; // 00000110;单地址，无逻辑地址，地址长度6字节
		for (i = 0; i < ((tSA->AF & ADDR_BITS) + 1); i++)
		{
			tSA->addr[i] = MeterReverAddr[i];
		}
	}
	else if (tIap_698.IsModuleUpgrade == 1)
	{
		tSA->AF = 0x26; // 00100110;单地址，有逻辑地址，地址长度7字节
		tSA->addr[0] = 0x03;//逻辑地址
		for (i = 1; i < ((tSA->AF & ADDR_BITS) + 1); i++)
		{
			tSA->addr[i] = ModuleReverAddr[i-1];
		}
	}
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
// 返回值:	
//
// 备注:
//-----------------------------------------------
WORD MakeFrameHead(TSA *sa, BYTE *pBuf, BYTE byClientAddr)
{
BYTE *pHead;
WORD wLen=0;

	
	pHead	= pBuf;
	pHead[wLen++]= OBJ_START_FLAG;
	wLen	+= 2;//跳过长度域
	wLen	++;	//控制域
	//地址域
	ComposeServerAddr(sa);
	wLen += gdw698bufLink_TSA(&pHead[wLen], sa, TD21);
	if(tIap_698.IsModuleUpgrade == 0)
	{
		pHead[wLen++]=byClientAddr;//get_client_addr();	//客户地址区
	}
	else if (tIap_698.IsModuleUpgrade == 1)
	{
		pHead[wLen++]= 0xa0;
	}
	
	//跳过帧头校验
	wLen	+= 2;
	return wLen;
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
// 返回值:	
//
// 备注:
//-----------------------------------------------
WORD Add698FrameTail(BYTE byCtrl, WORD wFrameLen, BYTE *pBuf)
{
	WORD wLen=0,wTmp;

	pBuf[1]	= LOBYTE(wFrameLen+2);	
	pBuf[2]	= HIBYTE(wFrameLen+2);
	pBuf[3]	= byCtrl;
	//包头校验
	wLen	= (pBuf[4]&ADDR_BITS)+1+2;
	//固定头4字节
	wLen	+= 4;
	wTmp	= fcs16(&pBuf[1], (WORD)(((pBuf[4]&ADDR_BITS)+1)+5));	
	pBuf[wLen++]	= LOBYTE(wTmp);
	pBuf[wLen++]	= HIBYTE(wTmp);	
	//整包校验
	wTmp	= fcs16(&pBuf[1],wFrameLen);
	//加入68长度1字节
	wFrameLen++;
	pBuf[wFrameLen++]	= LOBYTE(wTmp);
	pBuf[wFrameLen++]	= HIBYTE(wTmp);	
	pBuf[wFrameLen++]	= 0x16;	
	return wFrameLen;
}

//--------------------------------------------------
//功能描述:  组APDU 升级包
//         
//参数:      method 方法
//			pBuf[in] apdu
//			DATA[in] 
//			data 长度
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
WORD  ComposeApduPack( BYTE method,BYTE* pBuf,BYTE *Data,WORD Len)
{
	BYTE *buf;
	WORD wLen=0;

	buf = pBuf;
	if (MoniIapNo == 0 || MoniIapNo == 1)
	{
		buf[wLen++] = 0x07; // ActionRequest
	}
	else if (MoniIapNo == 2)
	{
		buf[wLen++] = 0x05; // 查询传输结果
	}
	else if (MoniIapNo == 3)
	{
		buf[wLen++] = 0x07; // 执行升级也是0x07
	}
	buf[wLen++] = 1;    // ActionRequest
	buf[wLen++] = 0;    // PPID
	buf[wLen++] = 0xF0; // OI
	buf[wLen++] = 0X01;
	buf[wLen++] = method;
	buf[wLen++] = 0; // 操作模式
	// DATA
	memcpy(&buf[wLen], Data, Len);
	wLen += Len;
	buf[wLen++] = 0; // 时间标签 0

	return wLen;
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
// 返回值:	
//
// 备注:
//-----------------------------------------------
WORD ComposeOpenFactoryFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	BYTE APDUData[] = {
	0x07, 0x01, 0x00, 0x20, 0xeb, 0x03, 0x00,
	0x09, 0x21, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x68, 0x14, 0x15, 0x0e, 0x12, 0x12, 0x37, 0x35, 0x33,
	0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12,
	0x0e, 0xbb, 0xbb, 0xbb, 0xbb, 0x34, 0xaf, 0x16, 0x00};

	memset(&tSA, 0, sizeof(TSA));	

	wLen = MakeFrameHead(&tSA, pBuf, 0); // 第三个参数表示客户地址	
	memcpy(&pBuf[wLen],APDUData,sizeof(APDUData));
	wLen += sizeof(APDUData);


	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	// 	nwy_ext_echo("\r\n");
	// for(i = 0; i < wLen; i++)
	// {
	// 	nwy_ext_echo("%02x ", pBuf[i]);
	// }	
	return wLen;
}
//-----------------------------------------------
// 函数功能: 
//
// 参数:		
//		
// 返回值:	
//
// 备注:
//-----------------------------------------------
WORD ComposeStartTranFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	char *version = VERSION;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	BYTE APDUDataLen = 0;	
	char timezone = 0;
	BYTE timtStr[20] = {0};
	nwy_time_t time;
	BYTE FileInfo[46] = {
	0x01, 0x01, 0x0A, 0x10, 0x56, 0x31, 0x2E, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x01, 0x0A, 0x10, 0x56, 0x31, 0x2E, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0A, 0x04, 0x31, 0x32, 0x33, 0x34};
	
	memset(&time, 0, sizeof(time));	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));	
	
	wLen = MakeFrameHead(&tSA, pBuf, 0); // 第三个参数表示客户地址	
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x06;
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x06;
	if(tIap_698.IsModuleUpgrade == 0)
	{
		APDUData[APDUDataLen++] = 0x00; // 源文件
		APDUData[APDUDataLen++] = 0x00; // 目标文件	
	}
	else if(tIap_698.IsModuleUpgrade == 1)
	{
		APDUData[APDUDataLen++] = 0x0a; // 源文件
		APDUData[APDUDataLen++] = strlen(upgrade_file_path_698);
		memcpy(&APDUData[APDUDataLen],upgrade_file_path_698,strlen(upgrade_file_path_698));
		APDUDataLen += strlen(upgrade_file_path_698);

		APDUData[APDUDataLen++] = 0x0a; //目标文件	
		APDUData[APDUDataLen++] = strlen(upgrade_file_path_698);
		memcpy(&APDUData[APDUDataLen],upgrade_file_path_698,strlen(upgrade_file_path_698));
		APDUDataLen += strlen(upgrade_file_path_698);
	}

	// 传输文件大小
	APDUData[APDUDataLen++] = 0x06;	
	APDUData[APDUDataLen++] = HIBYTE((HIWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = LOBYTE((HIWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = HIBYTE((LOWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = LOBYTE((LOWORD((DWORD)tIap_698.dwFileSize)));
	// 文件属性 bit-string（size 3）
	APDUData[APDUDataLen++] = 0x04; // 类型
	APDUData[APDUDataLen++] = 0x03; // 长度
	APDUData[APDUDataLen++] = 0xe0; // 值	
	// 文件版本 visible-str
	APDUData[APDUDataLen++] = 0x0a;		// 类型
	if(tIap_698.IsModuleUpgrade == 0)
	{

	APDUData[APDUDataLen++] = VERSION_LEN; // 长度	
	memcpy(APDUData + APDUDataLen, version, VERSION_LEN);	
	APDUDataLen += VERSION_LEN;
	}
	else if(tIap_698.IsModuleUpgrade == 1)
	{
		nwy_get_time(&time, &timezone);
		sprintf((char*)&timtStr,"%04d%02d%02d",time.year,time.mon,time.day);
		nwy_ext_echo("\r\ntime 年月日: %d年%d月%d日 字符串:%s\r\n", time.year, time.mon, time.day, timtStr);
		APDUData[APDUDataLen++] = strlen((char*)&timtStr); // 长度	
		memcpy(APDUData + APDUDataLen, timtStr, strlen((char*)&timtStr));	
		APDUDataLen += strlen((char*)&timtStr);
	}

	// 文件类别
	APDUData[APDUDataLen++] = 0x16;
	APDUData[APDUDataLen++] = 0x00;
	// 传输块大小
	APDUData[APDUDataLen++] = 0x12;
	APDUData[APDUDataLen++] = HIBYTE((WORD)tIap_698.wFrameSize);
	APDUData[APDUDataLen++] = LOBYTE((WORD)tIap_698.wFrameSize);
	// 校验
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x16;
	APDUData[APDUDataLen++] = 0x00;
	// 校验值 octet-str
	APDUData[APDUDataLen++] = 0x09;
	APDUData[APDUDataLen++] = 0x02;
	// 校验值
	APDUData[APDUDataLen++] = HIBYTE(tIap_698.wCrc16); // 0x05;
	APDUData[APDUDataLen++] = LOBYTE(tIap_698.wCrc16); // 0x83;	
	if(tIap_698.IsModuleUpgrade == 0)
	{
	APDUData[APDUDataLen++] = 0x00; // 兼容软件版本号
	APDUData[APDUDataLen++] = 0x00; // 兼容硬件版本号
	APDUData[APDUDataLen++] = 0x00; // 下载方标识
	}
	else if(tIap_698.IsModuleUpgrade == 1)
	{
		memcpy(APDUData + APDUDataLen,FileInfo,sizeof(FileInfo));
		APDUDataLen += sizeof(FileInfo);
	}

	wLen += ComposeApduPack(0x0e, &pBuf[wLen], APDUData, APDUDataLen);
	// nwy_ext_echo("\r\n");
	// for(i = 0; i < wLen; i++)
	// {
	// 	nwy_ext_echo("%02x ", pBuf[i]);
	// }	
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}
//--------------------------------------------------
// 功能描述:  698组文件帧长度字节
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void ComposeFileFrameLen(BYTE *pBuf, WORD *APDULen)
{
	WORD FrameSize = 0;

	if (MoniFrameNo < (tIap_698.wAllFrameNo - 1)) // 完整帧
	{
		FrameSize = tIap_698.wFrameSize;
	}
	else if (MoniFrameNo == (tIap_698.wAllFrameNo - 1)) // 最后一帧
	{
		if (tIap_698.LastFrameSize == 0)
		{
			FrameSize = tIap_698.wFrameSize;
		}
		else
		{
			FrameSize = tIap_698.LastFrameSize;
		}
	}
	nwy_ext_echo("\r\nFrameSize:%d", FrameSize);
	if ((FrameSize > 0) && (FrameSize < 128))
	{
		pBuf[(*APDULen)++] = FrameSize;
	}
	else if ((FrameSize > 127) && (FrameSize <= 255))
	{
		pBuf[(*APDULen)++] = 0x81;
		pBuf[(*APDULen)++] = FrameSize;
	}
	else if (FrameSize >= 256)
	{
		pBuf[(*APDULen)++] = 0x82;
		pBuf[(*APDULen)++] = HIBYTE((WORD)FrameSize);
		pBuf[(*APDULen)++] = LOBYTE((WORD)FrameSize);
	}
	else
	{
	}
}

//--------------------------------------------------
// 功能描述:  698发送数据帧
//
// 参数:	pBuf 输入缓冲
//
// 返回值: 帧长度
//
// 备注:
//--------------------------------------------------
WORD ComposeWriteFileFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	// BYTE i = 0;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	WORD APDUDataLen = 0;
	WORD ReadFileSize = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));
	// 组地址域
	tSA.AF = 0x05; // 00000110;单地址，无逻辑地址，地址长度6字节
	// for (i = 0; i < ((tSA.AF & ADDR_BITS) + 1); i++)
	// {
	// 	tSA.addr[i] = meter[i];
	// }	
	wLen = MakeFrameHead(&tSA, pBuf, 0); // 客户地址	
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x02;
	// 块序号
	APDUData[APDUDataLen++] = 0x12;	
	APDUData[APDUDataLen++] = HIBYTE(MoniFrameNo);
	APDUData[APDUDataLen++] = LOBYTE(MoniFrameNo);	
	// 块数据
	APDUData[APDUDataLen++] = 0x09;
	// 帧大小
	ComposeFileFrameLen(APDUData, &APDUDataLen);	
	// if(tIap_698.dwIapFlag == BASEMETER_UPGRADES)
	// {
	// 文件数据
	if (MoniFrameNo < tIap_698.wAllFrameNo)
	{	
		// if(MoniFrameNo == 154)
		// {	
		// }
		nwy_sdk_fseek(MeterUpgradeFD, tIap_698.wFrameSize * MoniFrameNo, NWY_SEEK_SET);
		if (MoniFrameNo < (tIap_698.wAllFrameNo - 1)) // 最后一帧之前的完整帧
		{
			ReadFileSize = tIap_698.wFrameSize;
		}
		else if (MoniFrameNo == (tIap_698.wAllFrameNo - 1)) // 最后一帧可能完整可能不完整
		{
			if (tIap_698.LastFrameSize == 0)
			{
				ReadFileSize = tIap_698.wFrameSize;
			}
			else
			{
				ReadFileSize = tIap_698.LastFrameSize;
			}
		}
		nwy_sdk_fread(MeterUpgradeFD, &APDUData[APDUDataLen], ReadFileSize);
		APDUDataLen += ReadFileSize;
		nwy_ext_echo("\r\nsend frame no:%d\r\n", MoniFrameNo);
	}
	wLen += ComposeApduPack(0x08, &pBuf[wLen], APDUData, APDUDataLen);
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}
//--------------------------------------------------
// 功能描述:  698发送读取传输结果帧
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD ComposeReadResultFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	BYTE APDUDataLen = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));	
	wLen = MakeFrameHead(&tSA, pBuf, 0);
	wLen += ComposeApduPack(0x04, &pBuf[wLen], APDUData, APDUDataLen);
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}

//--------------------------------------------------
// 功能描述:  698发送查询传输块结果帧
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD ComposeCheckResultFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	BYTE APDUDataLen = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));	
	wLen = MakeFrameHead(&tSA, pBuf, 0);
	wLen += ComposeApduPack(0x04, &pBuf[wLen], APDUData, APDUDataLen);
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}
//--------------------------------------------------
// 功能描述:  698发送执行升级帧
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
WORD ComposeExecuteUpgradeFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	BYTE APDUDataLen = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));	
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x1c;
	// 七字节时间
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;
	APDUData[APDUDataLen++] = 0xff;	
	APDUData[APDUDataLen++] = 0x00;	
	wLen = MakeFrameHead(&tSA, pBuf, 0);
	wLen += ComposeApduPack(0x0D, &pBuf[wLen], APDUData, APDUDataLen);
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}
//--------------------------------------------------
// 功能描述:  698升级模组前先打开厂内
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void SendOpenFactoryFrame(void)
{
	WORD wLen = 0;
	BYTE Buf[MAX_FRAME_LEN] = {0};

	wLen = ComposeOpenFactoryFrame(Buf);


	for ( int i = 0; i < wLen; i++)
	{
		nwy_ext_echo("%02x ", Buf[i]);
	}
	ENABLE_HARD_SCI_SEND;
	SendDataToModule(Buf, wLen);
	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer, 5000); //10s定时
}

//--------------------------------------------------
// 功能描述:  698升级函数第一包//发送数据包//发送开始升级包
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void SendIapData698(void)
{
	WORD wLen = 0;
	BYTE Buf[MAX_FRAME_LEN] = {0};
	// WORD i = 0;
	if (MoniIapNo == 0) // 组报文发送升级请求，读出升级包总帧数
	{
		if (GetUpGradeFileMessage() == FALSE)
		{
			ExitUpgrade698(eUpgradeVeriErr);
			nwy_ext_echo("\r\neUpgradeVeriErr");
			return;
		}
		wLen = ComposeStartTranFrame(Buf);
		nwy_stop_timer(meter_upgrade_timer);
		nwy_start_timer(meter_upgrade_timer, 900000);//115200 基表升级时间实测小于4分钟  留1.5倍预留,模组升级慢
		MeterUpgradeTimeUpFlag = 0;
		nwy_ext_echo("\r\n send start frame\r\n");
	}
	else if (MoniIapNo == 1) // 发送升级开始包
	{
		nwy_ext_echo("\r\n send file frame\r\n");
		wLen = ComposeWriteFileFrame(Buf);
	}
	else if (MoniIapNo == 2)
	{

		nwy_ext_echo("\r\n send check result frame\r\n");
		wLen = ComposeCheckResultFrame(Buf);
	}
	else if (MoniIapNo == 3)
	{
		// nwy_ext_echo("\r\n before send execute frame");
		// if(tIap_698.IsModuleUpgrade == 1)
		// {
		// 	nwy_sleep(15000);
		// }
		if((tIap_698.IsModuleUpgrade == 1) && (tIap_698.AlreadySendExeFrame == 1))
		{
			ExitUpgrade698(eUpgrdeFrameTimeoutErr);//到这里表示发送执行帧20s后没收到上报事件，退出模组升级
		}
		else
		{
			tIap_698.AlreadySendExeFrame = 1;//只供模组升级判断
			nwy_ext_echo("\r\n send execute upgrade frame\r\n");
			wLen = ComposeExecuteUpgradeFrame(Buf);
		}
		
	}

	for ( int i = 0; i < wLen; i++)
	{
		nwy_ext_echo("%02x ", Buf[i]);
	}
	if (tIap_698.IsModuleUpgrade == 1)
	{
		ENABLE_HARD_SCI_SEND;
		SendDataToModule(Buf, wLen);
	}
	else
	{
		nwy_uart_send_data(UART_HD_BASEMETER, Buf, wLen);
	}
}
//--------------------------------------------------
// 功能描述:  获取升级文件信息
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
BOOL GetUpGradeFileMessage(void)
{
	WORD w = 0, wCRC16 = 0, LastSize = 0;
	BYTE IapBuf[FILE_BLOCK_SIZE + 50] = {0};
	char crcStr[5] = {0};

	nwy_ext_echo("\r\nopen file:%s", upgrade_file_path_698);
	tIap_698.dwFileSize = nwy_sdk_fsize(upgrade_file_path_698);
	nwy_ext_echo("\r\nfile size  :%d", tIap_698.dwFileSize);
	tIap_698.wFrameSize = FILE_BLOCK_SIZE;
	
	if ((tIap_698.dwFileSize % tIap_698.wFrameSize) != 0)
	{
		tIap_698.wAllFrameNo = tIap_698.dwFileSize / tIap_698.wFrameSize + 1; // ？？？240k能整除  不确定以后发的bin文件大小是否固定
		tIap_698.LastFrameSize = tIap_698.dwFileSize % tIap_698.wFrameSize;
		LastSize = tIap_698.dwFileSize % tIap_698.wFrameSize;
	}
	else
	{
		tIap_698.wAllFrameNo = tIap_698.dwFileSize / tIap_698.wFrameSize;
		tIap_698.LastFrameSize = 0;
		LastSize = tIap_698.wFrameSize;
	}
	nwy_ext_echo("\r\nall frame no:%d lastFrameSize:%d", tIap_698.wAllFrameNo, tIap_698.LastFrameSize);
	if(MeterUpgradeFD == -1)
	{
		MeterUpgradeFD = nwy_sdk_fopen(upgrade_file_path_698, NWY_RDONLY);	
		nwy_ext_echo("\r\nMeterUpgradeFD:%d",MeterUpgradeFD);
	}
	
	//判断.bin之前作为校验，在此处计算bin文件与文件名所带的校验是否一致，不一致则退出升级，清除升级标志
	if (MeterUpgradeFD < 0)
	{
		nwy_ext_echo("\r\nopen apppack fail");
		return FALSE;
	}
	wCRC16 = 0xffff; // 赋初值
	for (w = 0; w < tIap_698.wAllFrameNo; w++)//判断校验
	{
		nwy_sdk_fseek(MeterUpgradeFD, tIap_698.wFrameSize * w, NWY_SEEK_SET);
		nwy_sdk_fread(MeterUpgradeFD, IapBuf, tIap_698.wFrameSize);
		if (w == (tIap_698.wAllFrameNo - 1))
		{
			wCRC16 = ContinueFcs16(IapBuf, LastSize, wCRC16, TRUE); // 计算校验
			tIap_698.wCrc16 = wCRC16;
			nwy_ext_echo("\r\n crc16:%04x", wCRC16);
			nwy_ext_echo("\r\nfile path:%s", upgrade_file_path_698);
			sprintf(crcStr, "%04X", tIap_698.wCrc16); // 将 CRC16 转换为字符串形式,大写形式
			if (strstr(upgrade_file_path_698, crcStr) == NULL)
			{
				nwy_ext_echo("\r\n 698 CRC is error");
				return FALSE;
			}
			else
			{
				nwy_ext_echo("\r\n ftp file crc16 is ok");
				return TRUE;
			}
		}
		else
		{
			wCRC16 = ContinueFcs16(IapBuf, tIap_698.wFrameSize, wCRC16, FALSE); // 计算校验
		}
	}

	return TRUE;
}

//--------------------------------------------------
//功能描述: 698升级任务
//
//参数:无
//
//返回值:无
//
//备注:
//--------------------------------------------------
void  MeterUpgrade698_Task( void  )
{
	if ((api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE)&&(api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE))
	{	
		if (MeterUpgradeTimeUpFlag == 1) //15分钟未升级完则退出
		{
			ExitUpgrade698(eUpgradeWholeProTimeoutErr);
			nwy_ext_echo("\r\neUpgradeWholeProTimeoutErr");
			return;
		}
		if (api_GetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME))
		{
			SendIapData698();
			api_ClrSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
			if((tIap_698.IsModuleUpgrade == 1) &&( MoniIapNo == 3))//模组发送执行升级帧不会回复，多等一会直到回复主动上报事件变更帧
			{
				// nwy_stop_timer(uart_timer);
				// nwy_start_timer(uart_timer, 20000);//8s后收到上报帧，退出升级
				ExitUpgrade698(eUpgradeExeSuc);
			}
			else
			{
				nwy_stop_timer(uart_timer);
				nwy_start_timer(uart_timer, 3000); // 调试重传修改状态位需要时间
			}
		}
		else
		{
			if (api_GetSysStatus(eSYS_STASUS_TIMER_UP)) // 1s延时到都没有收到数据
			{
				nwy_ext_echo("\r\n upgrade timeout !!!");
				api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
				TimeoutRetryNum++; // 升级失败
				nwy_ext_echo("\r\n upgrade retry num:%d", TimeoutRetryNum);
				if (TimeoutRetryNum >= 3)
				{
					ExitUpgrade698(eUpgrdeFrameTimeoutErr);
					// 重试次数达到限制，判定升级失败
					nwy_ext_echo("\r\n exit upgrade  !!!");
				}
				else
				{
					// 尚未达到重试次数，继续重发数据帧
					api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME); // 设置状态，准备重发数据
				}
			}
		}
	}
}