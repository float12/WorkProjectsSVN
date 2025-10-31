//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾��ѹ̨����Ʒ��
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define LEN_OF_ONE_FRAME						192						//��ԭ���������̱���һ��
#define ADDR_BITS								0x0F					//��ַ����
#undef  LOBYTE
#define LOBYTE(w)								(BYTE)(w)				//ȡһ��WORD�ĵ��ֽ�
#undef  HIBYTE
#define HIBYTE(w)								(BYTE)((WORD)(w) >> 8) //ȡһ��WORD�ĸ��ֽ�
#undef  LOWORD
#define LOWORD(l)								(WORD)(l)				//ȡһ��DWORD�ĵ�WORD
#undef  HIWORD
#define HIWORD(l)								(WORD)((DWORD)(l) >> 16)//ȡһ��DWORD�ĸ�WORD
#define OBJ_START_FLAG							0x68					//��ʼ�ַ�
#define APDU_DATA_MAX_LEN						500						// apdu������󳤶�
#define FILE_BLOCK_SIZE							400						//��֡�������ݲ��ɳ���MAX_FRAME_LEN,��ò�Ҫ��̫�������Ҫ��֡�������,Ҳ����̫С����ѯ�������֡�ֽ������ܳ���256
#define MAX_FRAME_LEN							512 
#define OMD_OFFSET								17
#define VERSION_LEN								4						// ���Ⱥ������ֵ��Ӧ
#define VERSION									"V1.1"
#define MAX_ERROR_FRAME_NUM 					256
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef enum TRANS_DIR
{
	TD12,
	TD21
} TRANS_DIR; //ת������: TD12:1�β�->2�β�; TD21:2->1
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
T_Iap_645 tIap_645;
T_Iap_698 tIap_698;
BYTE MeterUpgradeTimeUpFlag = 0; //������ȫ����1��ʾʱ�䵽
char upgrade_file_path_645[FILENAME_LEM] = {0};
char upgrade_file_path_698[FILENAME_LEM] = {0};

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
//645\698���ñ���
static BYTE	MoniIapNo = 0;//��ⵥԪ��ǰ��������
static WORD MoniFrameNo = 0;//��ⵥԪ�������ݰ�֡��� //������Ϊ�ļ�ƫ��ʹ��
int MeterUpgradeFD = -1;        //�ļ������� ���� ��������ļ�
BYTE TimeoutRetryNum = 0;         // �ط�����
//698ר�ñ���
static WORD errorFrameCount = 0;
static BOOL RetryPhaseFlag = 0; // ʧ���ط�֡�׶�,
static WORD errorFrames[MAX_ERROR_FRAME_NUM] = {0XFFFF};
static WORD retryIndex = 0;
static BYTE CheckResultRetryCount = 0;



static BYTE sCRCHi =0xFF; // high byte of CRC initialized 	//�˴�Ӧ���뱻�������һ�£�����֮ǰ��һ�µ���У�鲻һ��
static BYTE sCRCLo = 0xFF; // low byte of CRC initialized
// -----------------------------------------------------------------------------
// DESCRIPTION: CRCУ��ĸ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCHi[] = {
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
	0x81, 0x40};

// -----------------------------------------------------------------------------
// DESCRIPTION: CRCУ��ĵ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCLo[] = {
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
	0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

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
//				�ڲ���������
//-----------------------------------------------
extern	char	*strcasestr (const char *, const char *);
//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
// ��������: �˳�������645������������
//
// ����: ��
//
// ����ֵ:  ��
//
// ��ע:
//-----------------------------------------------
void InitUpgradeVariable645(void)
{
	MoniIapNo = 0;		 //��ⵥԪ��ǰ��������
	MoniFrameNo = 0;	 //��ⵥԪ�������ݰ�֡��� //������Ϊ�ļ�ƫ��ʹ��
	TimeoutRetryNum = 0; // �ط�����
	nwy_sdk_timer_stop(uart_timer);
	nwy_sdk_timer_stop(meter_upgrade_timer);
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
	nwy_ext_echo("\r\n init upgrade variable 645  success ");
}

// ��������: �˳�������698������������
//
// ����: ��
//
// ����ֵ:  ��
//
// ��ע:
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
	nwy_sdk_timer_stop(uart_timer);
	nwy_sdk_timer_stop(meter_upgrade_timer);
	TimeoutRetryNum = 0;
	RetryPhaseFlag = 0;
	MoniIapNo = 0;
	MoniFrameNo = 0;
	api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
	nwy_ext_echo("\r\n init upgrade variable 698 success ");
}
//
//-----------------------------------------------
// ��������: ����������������
//
// ����: 	��
//
// ����ֵ:  ��
//
// ��ע:
//-----------------------------------------------
void EnterUpgrade645(void)
{
	InitUpgradeVariable645();
	tIap_645.dwIapFlag = BASEMETER_UPGRADES; //�����ȷ���ñ�־ǰ�������
}
//-----------------------------------------------
// ��������: ����������������
//
// ����: 	��
//
// ����ֵ:  ��
//
// ��ע:
//-----------------------------------------------
void EnterUpgrade698(void)
{
	InitUpgradeVariable698();
	tIap_698.dwIapFlag = BASEMETER_UPGRADES; //�����ȷ���ñ�־ǰ�������
}
//-----------------------------------------------
// ��������: �˳�������������
//
// ����: 	��
//
// ����ֵ:  ��
//
// ��ע:
//-----------------------------------------------
void ExitUpgrade645(eUpgradeResult result)
{
	BYTE FullFilePath[10+FILENAME_LEM] = "/nwy/";

	InitUpgradeVariable645();
	tIap_645.dwIapFlag = 0;
	tIap_645.dwChip = 0;
	if (NWY_SUCCESS == nwy_file_close(MeterUpgradeFD))
	{
		// nwy_ext_echo("\r\n close file success");
	}
	else
	{
		nwy_ext_echo("\r\n close file err");
	}
	MeterUpgradeFD = -1;
	strcat(FullFilePath,upgrade_file_path_645);
	if(nwy_file_remove(FullFilePath) == NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n delete upgrade file success");
	}
	else
	{
		nwy_ext_echo("\r\n delete upgrade file err");
	}
	memset(upgrade_file_path_645, 0, sizeof(upgrade_file_path_645));
	nwy_ext_echo("\r\n exit 645 upgrade ");
	if (nwy_msg_queue_send(UpgradeResultMessageQueue, 1 ,&result,NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
	{
		nwy_ext_echo("\r\neUpgrade result put msg que success");
	}
	else
	{
		nwy_ext_echo("\r\neUpgrade result put msg que err");
	}
}

//-----------------------------------------------
// ��������: �˳�������������
//
// ����: 	��
//
// ����ֵ:  ��
//
// ��ע:
//-----------------------------------------------
void ExitUpgrade698(eUpgradeResult result)
{
	InitUpgradeVariable698();
	tIap_698.dwIapFlag = 0;
	if (NWY_SUCCESS == nwy_file_close(MeterUpgradeFD))
	{
		nwy_ext_echo("\r\n close file success");
	}
	MeterUpgradeFD = -1;
	if(nwy_file_remove(upgrade_file_path_698) == NWY_SUCCESS)
	{
		nwy_ext_echo("\r\n delete upgrade file success");
	}
	else
	{
		nwy_ext_echo("\r\n delete upgrade file err");
	}
	memset(upgrade_file_path_698, 0, sizeof(upgrade_file_path_698));
	nwy_ext_echo("\r\n exit 698 upgrade ");
	if (nwy_msg_queue_send(UpgradeResultMessageQueue, 1 ,&result,NWY_OSA_NO_SUSPEND) == NWY_SUCCESS)
	{
		nwy_ext_echo("\r\neUpgrade result put msg que success");
	}
	else
	{
		nwy_ext_echo("\r\neUpgrade result put msg que err");
	}
}
//-----------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void handleWriteFileSuccess(void)
{
	if (MoniIapNo == 1)
	{
		nwy_ext_echo("\r\n MoniIapNo = %d", MoniIapNo);
		if (MoniFrameNo < tIap_698.wAllFrameNo) // tIap_698.wAllFrameNo
		{	
			if (RetryPhaseFlag == 0) // ���ط��׶�
			{
				MoniFrameNo++;
				nwy_ext_echo("\r\n next FrameNo  is %d,all frame no:%d", MoniFrameNo, tIap_698.wAllFrameNo);
				if (MoniFrameNo == tIap_698.wAllFrameNo)
				{
					MoniIapNo = 2;
				}
			}
			else // �ط��׶�
			{
				if (errorFrameCount > 0)
				{
					MoniFrameNo = errorFrames[retryIndex++];
					errorFrameCount--;
				}
				else // ���д���֡�ط���
				{
					MoniIapNo = 2;
					nwy_ext_echo("\r\n all retry frames  over ");
				}
			}
		}
	}
}
//-----------------------------------------------
// ��������: ����֮ǰ��fcsУ��ֵ����������fscУ��
//
// ����:		BYTE *cp[in]	����ָ��
//			WORD len[in] 	���ݳ���
//			LastFcs[in]		��һ�μ����У��ֵ�����ڱ��μ���ĳ�ʼֵ
//			IsEnd[in]		�Ƿ������һ������ TRUE/FALSE ��/��
//
// ����ֵ:	WORD
//
// ��ע:
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

	if (IsEnd) // �������һ��crcУ��ʱִ��
	{
		fcs ^= 0xffff; // complement  //��ΪFCSֵ
	}

	return fcs;
}
//-----------------------------------------------
// ��������:
//
// ����:
//
// ����ֵ:
//
// ��ע:
//-----------------------------------------------
void handleCheckResult(BYTE *buf)
{
	WORD frameNo = 0;
	WORD byteIndex = 0; // buf �е�ǰ�����ֽ�λ��
	BYTE bitIndex = 0;  // buf[byteIndex] �еı���λ����

	BYTE CheckFrameResultOffest = 0; // ���ش���״̬֡״̬���ݵ�ƫ��λ��
	RetryPhaseFlag = 0;
	nwy_ext_echo("\r\n check result replied ");
	// �ж�״̬����ƫ��λ��
	if ((buf[OMD_OFFSET + 6]) == 0x82) // ���ؿ鳤����
	{
		CheckFrameResultOffest = OMD_OFFSET + 6 + 3;
	}
	else if ((buf[OMD_OFFSET + 6]) == 0x81) // �����ļ�������
	{
		CheckFrameResultOffest = OMD_OFFSET + 6 + 2;
	}
	else
	{
		CheckFrameResultOffest = OMD_OFFSET + 6 + 1;
	}
	nwy_ext_echo("\r\n CheckFrameResultOffest is %d", CheckFrameResultOffest);
	// if (tIap_698.wAllFrameNo % 8 == 0)
	// {
	for (frameNo = 0; frameNo < tIap_698.wAllFrameNo; frameNo++)
	{
		// ��ȡ��ǰ֡�Ľ��λ
		byteIndex = frameNo / 8; // ��ǰ֡���ڵ� buf ���ֽ�
		bitIndex = frameNo % 8;  // ��ǰ֡���ڵ��ֽڵı���λ��

		// ��鴫���������Ϊ0��ʾʧ��
		if (!(buf[CheckFrameResultOffest + byteIndex] & (1 << (7 - bitIndex))))
		{
			// ����ʧ��֡��
			errorFrames[errorFrameCount++] = frameNo; // ��¼֡�ţ���0��ʼ
			nwy_ext_echo("\r\n error frame no is %d", frameNo);
			RetryPhaseFlag = 1;
		}
	}

	if (RetryPhaseFlag == 0)
	{
		if (MoniIapNo == 2) // ���������յ��϶��ظ�
		{
			MoniIapNo = 3;
			nwy_ext_echo("\r\n enter step 3");
		}
	}
	else // �д���
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
			MoniIapNo = 1; // �ص���1���ط�
			errorFrameCount--;
			nwy_ext_echo("\r\nerror Frame Count left  is %d", errorFrameCount);
			retryIndex = 0;
			MoniFrameNo = errorFrames[retryIndex++];
			nwy_ext_echo("\r\nretry FrameNo  is %d,all frame no:%d", MoniFrameNo, tIap_698.wAllFrameNo);
		}
	}
}
//-----------------------------------------------
// ��������:����698������صĽ��ձ���
//
// ����:��
//
// ����ֵ:  true:�������������Ӧ��֡��false���յ�����698��Ӧ����
//
// ��ע:
//-----------------------------------------------
BOOL ProcUpgradeResponse698(void)
{
	BYTE *buf = Serial.ProBuf;
	WORD offset = 0;
	BYTE result = TRUE;

	offset = OMD_OFFSET ;         // omd�����ڶ��ֽ�λ��
	if ((*(DWORD *)(buf + offset)) == 0x000E01F0) //��������ظ�
	{
		if (buf[offset + 4] == 0x00)
		{
			MoniIapNo = 1;
			nwy_ext_echo("\r\n MoniIapNo = %d", MoniIapNo);
		}
		else //dar����
		{
			ExitUpgrade698(eUpgradeDARErr);
			nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
		}
	}
	else if ((*(DWORD *)(buf + offset)) == 0x000801F0) //д�ļ��ظ�
	{
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
	else if ((*(DWORD *)(buf + offset)) == 0x000401F0) //��ѯ����ظ�
	{
		handleCheckResult(buf);
	}
	else if ((*(DWORD *)(buf + offset)) == 0x000D01F0) //ִ�������ظ�
	{
		if (buf[offset + 4] == 0x00)
		{
			if (MoniIapNo == 3) // ���������յ��϶��ظ�
			{
				ExitUpgrade698(eUpgradeExeSuc);
				// ���۵�������ȫ��������ϣ�������վ�����ɹ���Ϣ
			}
		}
		else
		{
			ExitUpgrade698(eUpgradeDARErr);
			nwy_ext_echo("\r\n  698 RECEIVE DAR error exit upgrade \r\n");
		}
	}
	else
	{
		result = FALSE;
	}
	if (result == TRUE) //�յ�������ص���Ӧ����
	{
		nwy_sdk_timer_stop(uart_timer);
		TimeoutRetryNum = 0;
		api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
	}

	return result;
}
//-----------------------------------------------
// ��������: 645��Լ�������մ���
//
//����: 	��
//			
//����ֵ:  	Bool  
//
//��ע:   
//-----------------------------------------------
BOOL GyRxMonitor( void )
{
	TSerial *p = &Serial;
	T645FmRx *pRxFm=(T645FmRx *)p->ProBuf;

	if(pRxFm->byCtrl == 0x9D)//������ⵥԪ�յ��϶��ظ�
	{
		if(MoniIapNo == 0)
		{
			MoniIapNo = 1;
			nwy_ext_echo("\r\n MoniIapNo = %d",MoniIapNo);
		}
		else if(MoniIapNo == 1)
		{
			nwy_ext_echo("\r\n 2srMoniIapNo = %d",MoniIapNo);
			if(MoniFrameNo < tIap_645.wAllFrameNo)//tIap_645.wAllFrameNo
			{
				MoniFrameNo++;
				nwy_ext_echo("\r\n RECEIVE no is %d",MoniFrameNo);
				if(MoniFrameNo == tIap_645.wAllFrameNo)
				{
					MoniIapNo = 2;
				}
			}
		}
		else if(MoniIapNo == 2)//���������յ��϶��ظ�
		{
			ExitUpgrade645(eUpgradeExeSuc);
			nwy_ext_echo("\r\n 645 dbstart update");
			//���۵�������ȫ��������ϣ�������վ�����ɹ���Ϣ
		}
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

//-----------------------------------------------
//��������  : 645��ԼԤ����,ֻ�����ж�����
//
//����  :    TSerial *p[in]
//
//����ֵ:     BOOL ��TRUE-��Ҫ��ת�����߱������鴦��
//��ע����  :  ��
//-----------------------------------------------
BOOL Pre_Dlt645(TSerial *p)
{	
	if (GyRxMonitor() == TRUE)
	{
		nwy_sdk_timer_stop(uart_timer); //������ɹض�ʱ��
		api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}
//--------------------------------------------------
//��������:  ���������ļ���CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
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
//��������:  ����CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
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
//��������:  �Ա��Ľ��м�0x33����
//         
//����:      BYTE *pBuf[in]:���뻺��pBuf[0]Ϊ68H
//         
//����ֵ:   void 
//         
//��ע����:  ��
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
//��������:  �������������ļ���У��
//         
//����:      
//         
//����ֵ:    TRUE: CRCУ����ȷ��FALSE: CRCУ�����
//         
//��ע:  
//--------------------------------------------------
BYTE  ComputeFileCheckSum( void )
{
	WORD i;
	BYTE Buf[LEN_OF_ONE_FRAME] ={0},bLen;
	char crcStr[5];

	//��ʼ��CRC16
	sCRCHi =0xFF;
	sCRCLo =0xFF;
	for (i = 0; i < tIap_645.wAllFrameNo; i++)
	{
		nwy_file_seek(MeterUpgradeFD, LEN_OF_ONE_FRAME * i, NWY_SEEK_SET);
		bLen = nwy_file_read(MeterUpgradeFD, &Buf[0], LEN_OF_ONE_FRAME);
		if(bLen != LEN_OF_ONE_FRAME)
		{
			memset(&Buf[bLen],0xFF,LEN_OF_ONE_FRAME-bLen);
		}
		tIap_645.wCrc16 = FileCalCRC16(&Buf[0], LEN_OF_ONE_FRAME);
	}
	nwy_ext_echo("\r\ncal crc:%04x",tIap_645.wCrc16);
	nwy_ext_echo("\r\nfile path:%s", upgrade_file_path_645);
	sprintf(crcStr, "%04X", tIap_645.wCrc16); // �� CRC16 ת��Ϊ�ַ�����ʽ,��д��ʽ
	if (strcasestr(upgrade_file_path_645, crcStr) == NULL)
	{
		nwy_ext_echo("\r\n 645 CRC is error");
		return FALSE;
	}
	else
	{
		nwy_ext_echo("\r\n ftp file crc16 is ok");
		return TRUE;
	}

	return TRUE;
}
//---------------------------------------------------------------
//��������: ���������ļ�
//
//����: 	
//                   
//����ֵ:  eUpgradeResult �������
//
//��ע:   
//---------------------------------------------------------------
eUpgradeResult SendIapData645(void)
{
	TSerial *p = &Serial;
  	WORD wLen = 4, wCrc16;
	BYTE ret = FALSE,bLen;
	BYTE FullFilePath[10+FILENAME_LEM] = "/nwy/";

	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_meter_upgrade_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};

	p->TXPoint = 0;
	memset(p->ProBuf,0xFE,4);
	p->ProBuf[wLen++] = 0x68;
	//ģ���ַ
	memcpy(&p->ProBuf[wLen],reverAddr,6);   
	wLen += 6; 
	p->ProBuf[wLen++] = 0x68;
	p->ProBuf[wLen++] = 0x1d;

	if(MoniIapNo == 0)//�鱨�ķ����������󣬶�����������֡��
	{
		TimeoutRetryNum = 0;
		tIap_645.wAllFrameNo =  tIap_645.dwFileSize/LEN_OF_ONE_FRAME;	//������240k������  ��ȷ���Ժ󷢵�bin�ļ���С�Ƿ�̶�
		if ((tIap_645.dwFileSize % LEN_OF_ONE_FRAME) != 0)
		{
			tIap_645.wAllFrameNo++;
		}
		strcat(FullFilePath,upgrade_file_path_645);
		//���ļ����´δ���
		nwy_ext_echo("\r\n download pack is %s",FullFilePath);
		MeterUpgradeFD = nwy_file_open(FullFilePath, NWY_RDONLY);
		if (MeterUpgradeFD < 0)
		{
			nwy_ext_echo("\r\nopen apppack fail");
			return eUpgradeVeriErr;
		}
		ret = ComputeFileCheckSum();
		if (ret == FALSE)
		{
			return eUpgradeVeriErr;
		}	
		p->ProBuf[wLen++] = 0x14;
		//���ݱ�ʶ
		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			if (tIap_645.dwChip == CHIP_415_UPGRADE)
			{
				p->ProBuf[wLen++] = 0x04; 
			}
			else
			{
				p->ProBuf[wLen++] = 0x01; 
			}
			
		}
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f;  
		//���뼰�����ߴ���
		p->ProBuf[wLen++] = 0x02;
		memset(&p->ProBuf[wLen],0x00,7);
		wLen += 7;
		memcpy(&p->ProBuf[wLen], (BYTE *)&tIap_645.dwFileSize, 4);
		wLen += 4;
		memcpy(&p->ProBuf[wLen], (BYTE *)&tIap_645.wAllFrameNo, 2);
		wLen += 2;
		memcpy(&p->ProBuf[wLen], (BYTE *)&tIap_645.wCrc16, 2);
		wLen += 2;
		DataPlus33H( &p->ProBuf[4]);
		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 30 );
		p->ProBuf[wLen++] = 0x16;
		nwy_sdk_timer_stop(meter_upgrade_timer);
		OneTimeTImerPara.expired_time = 900000;
		nwy_sdk_timer_start(meter_upgrade_timer, &OneTimeTImerPara); //9600 ��������ʱ��ʵ��10����  ��1.5��Ԥ��
	}
	else if(MoniIapNo == 1)//�鱨�ķ���������
	{
		p->ProBuf[wLen++] = 0xd0;
		//���ݱ�ʶ
		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			if (tIap_645.dwChip == CHIP_415_UPGRADE)
			{
				p->ProBuf[wLen++] = 0x05; 
			}
			else
			{
				p->ProBuf[wLen++] = 0x02; 
			}
		}
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f;  
		//���뼰�����ߴ���
		p->ProBuf[wLen++] = 0x02;
		memset(&p->ProBuf[wLen],0x00,7);
		wLen += 7;
		p->ProBuf[wLen++] = (MoniFrameNo%0x0100);
		p->ProBuf[wLen++] = (MoniFrameNo/0x0100);
		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			nwy_file_seek(MeterUpgradeFD, LEN_OF_ONE_FRAME * MoniFrameNo, NWY_SEEK_SET);
			bLen = nwy_file_read(MeterUpgradeFD, &p->ProBuf[wLen], LEN_OF_ONE_FRAME);
			if (bLen != LEN_OF_ONE_FRAME)
			{
				memset(&p->ProBuf[wLen+bLen],0xFF,LEN_OF_ONE_FRAME-bLen);
			}
			nwy_ext_echo("\r\nsend pack%d", MoniFrameNo);
		}
		wLen += LEN_OF_ONE_FRAME;
		wCrc16 = CalCRC16(&p->ProBuf[28], LEN_OF_ONE_FRAME);
		memcpy(&p->ProBuf[wLen], (BYTE *)&wCrc16, sizeof(WORD));
		wLen += 2;
		DataPlus33H( &p->ProBuf[4]);
		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 218 );
		p->ProBuf[wLen++] = 0x16;
	}
	else if(MoniIapNo == 2)//�鱨�ķ��Ϳ�ʼ��������
	{
		p->ProBuf[wLen++] = 0x0d;
		//���ݱ�ʶ
		if(tIap_645.dwIapFlag == BASEMETER_UPGRADES)
		{
			if (tIap_645.dwChip == CHIP_415_UPGRADE)
			{
				p->ProBuf[wLen++] = 0x06; 
			}
			else
			{
				p->ProBuf[wLen++] = 0x03; 
			}
		}
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f; 
		p->ProBuf[wLen++] = 0x0f;  
		//���뼰�����ߴ���
		p->ProBuf[wLen++] = 0x02;
		memset(&p->ProBuf[wLen],0x00,7);
		wLen += 7;
		p->ProBuf[wLen++] = 0x55;
		DataPlus33H( &p->ProBuf[4]);
		p->ProBuf[wLen++] = lib_CheckSum( &p->ProBuf[4], 23 );
		p->ProBuf[wLen++] = 0x16;
	}

	p->SendLength = wLen;
	p->RXWPoint = 0;
	p->RXRPoint = 0;
	// nwy_ext_echo("\r\n ");
	// for (int i = 0; i < p->SendLength; i++)
	// {
	// 	nwy_ext_echo(" %02x", p->ProBuf[i]);
	// }
	nwy_uart_write(UART_HD,p->ProBuf,p->SendLength);
	return e645FrameNormal;
}
//--------------------------------------------------
//��������:  ģ��������������
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  MeterUpgrade645_Task( void  )
{
	BYTE ret = TRUE;
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	if (MeterUpgradeTimeUpFlag == 1) //15����δ���������˳�
	{
		ExitUpgrade645(eUpgradeWholeProTimeoutErr);
		return;
	}
	if ((api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE) && (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE))
	{	
		if (api_GetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME))
		{
			TimeoutRetryNum = 0;
			ret = SendIapData645();
			if (ret != e645FrameNormal)
			{
				ExitUpgrade645(ret);
				nwy_ext_echo("\r\nSendIapData645 false upgrade fail !!!");
				return;
			}
			else
			{
				api_ClrSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
				nwy_sdk_timer_stop(uart_timer);
				OneTimeTImerPara.expired_time = 1000;
				nwy_sdk_timer_start(uart_timer, &OneTimeTImerPara);
			}
		}
		else
		{
			if (api_GetSysStatus(eSYS_STASUS_TIMER_UP)) // 500ms��ʱ����û���յ�����
			{
				api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
				TimeoutRetryNum++; // ����ʧ��
				if (TimeoutRetryNum >= 2)
				{
					ExitUpgrade645(eUpgrdeFrameTimeoutErr);
					nwy_ext_echo("\r\n time out  upgrade fail !!!");
					return;
				}
				api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
			}
		}
	}
	else
	{
		nwy_ext_echo("\r\n start collect:%d, start tran:%d", api_GetSysStatus(eSYS_STASUS_START_COLLECT), api_GetSysStatus(eSYS_STASUS_START_TRAN));
	}
}

//-----------------------------------------------
//��������: fcsУ�麯��
//
//����:		BYTE *cp  
//			WORD len   
//����ֵ:	WORD
//		   
//��ע:
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

	fcs ^= 0xffff; // complement  //��ΪFCSֵ
	return fcs;
}

//-----------------------------------------------
// ��������: 
//
// ����:		
//		
//		
//		
//
// ����ֵ:	
//
// ��ע:
//-----------------------------------------------
WORD gdw698bufLink_TSA(BYTE *pBuf, TSA *psa, TRANS_DIR td)
{ //.��·����Ҫ��ת
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
// ��������: 
//
// ����:		
//		
// ����ֵ:	
//
// ��ע:
//-----------------------------------------------
void ComposeServerAddr(TSA *tSA)
{
	BYTE i = 0;
	tSA->AF = 0x05; // 00000110;����ַ�����߼���ַ����ַ����6�ֽ�
	for (i = 0; i < ((tSA->AF & ADDR_BITS) + 1); i++)
	{
		tSA->addr[i] = reverAddr[i];
	}
}
//-----------------------------------------------
// ��������: 
//
// ����:		
//		
// ����ֵ:	
//
// ��ע:
//-----------------------------------------------
WORD MakeFrameHead(TSA *sa, BYTE *pBuf, BYTE byClientAddr)
{
BYTE *pHead;
WORD wLen=0;

	
	pHead	= pBuf;
	pHead[wLen++]= OBJ_START_FLAG;
	wLen	+= 2;//����������
	wLen	++;	//������
	//��ַ��
	ComposeServerAddr(sa);
	wLen	+= gdw698bufLink_TSA(&pHead[wLen],sa,TD21);
	pHead[wLen++]=byClientAddr;//get_client_addr();	//�ͻ���ַ��
	//����֡ͷУ��
	wLen	+= 2;
	return wLen;
}
//-----------------------------------------------
// ��������: 
//
// ����:		
//		
// ����ֵ:	
//
// ��ע:
//-----------------------------------------------
WORD Add698FrameTail(BYTE byCtrl, WORD wFrameLen, BYTE *pBuf)
{
	WORD wLen=0,wTmp;

	pBuf[1]	= LOBYTE(wFrameLen+2);	
	pBuf[2]	= HIBYTE(wFrameLen+2);
	pBuf[3]	= byCtrl;
	//��ͷУ��
	wLen	= (pBuf[4]&ADDR_BITS)+1+2;
	//�̶�ͷ4�ֽ�
	wLen	+= 4;
	wTmp	= fcs16(&pBuf[1], (WORD)(((pBuf[4]&ADDR_BITS)+1)+5));	
	pBuf[wLen++]	= LOBYTE(wTmp);
	pBuf[wLen++]	= HIBYTE(wTmp);	
	//����У��
	wTmp	= fcs16(&pBuf[1],wFrameLen);
	//����68����1�ֽ�
	wFrameLen++;
	pBuf[wFrameLen++]	= LOBYTE(wTmp);
	pBuf[wFrameLen++]	= HIBYTE(wTmp);	
	pBuf[wFrameLen++]	= 0x16;	
	return wFrameLen;
}

//--------------------------------------------------
//��������:  ��APDU ������
//         
//����:      method ����
//			pBuf[in] apdu
//			DATA[in] 
//			data ����
//         
//����ֵ:    
//         
//��ע:  
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
	    buf[wLen++] = 0x05; // ��ѯ������
	}
	else if (MoniIapNo == 3)
	{
		buf[wLen++] = 0x07; // ִ������Ҳ��0x07
	}
	buf[wLen++] = 1;    // ActionRequest
	buf[wLen++] = 0;    // PPID
	buf[wLen++] = 0xF0; // OI
	buf[wLen++] = 0X01;
	buf[wLen++] = method;
	buf[wLen++] = 0; // ����ģʽ
	// DATA
	memcpy(&buf[wLen], Data, Len);
	wLen += Len;
	buf[wLen++] = 0; // ʱ���ǩ 0

	return wLen;
}

//-----------------------------------------------
// ��������: 
//
// ����:		
//		
// ����ֵ:	
//
// ��ע:
//-----------------------------------------------
WORD ComposeStartTranFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	char *version = VERSION;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	BYTE APDUDataLen = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));	
	wLen = MakeFrameHead(&tSA, pBuf, 0); // ������������ʾ�ͻ���ַ	
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x06;
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x06;
	APDUData[APDUDataLen++] = 0x00; // Դ�ļ�
	APDUData[APDUDataLen++] = 0x00; // Ŀ���ļ�	
	// �����ļ���С
	APDUData[APDUDataLen++] = 0x06;	
	APDUData[APDUDataLen++] = HIBYTE((HIWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = LOBYTE((HIWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = HIBYTE((LOWORD((DWORD)tIap_698.dwFileSize)));
	APDUData[APDUDataLen++] = LOBYTE((LOWORD((DWORD)tIap_698.dwFileSize)));
	// �ļ����� bit-string��size 3��
	APDUData[APDUDataLen++] = 0x04; // ����
	APDUData[APDUDataLen++] = 0x03; // ����
	APDUData[APDUDataLen++] = 0xe0; // ֵ	
	// �ļ��汾 visible-str
	APDUData[APDUDataLen++] = 0x0a;        // ����
	APDUData[APDUDataLen++] = VERSION_LEN; // ����	
	memcpy(APDUData + APDUDataLen, version, VERSION_LEN);	
	APDUDataLen += VERSION_LEN;
	// �ļ����
	APDUData[APDUDataLen++] = 0x16;
	APDUData[APDUDataLen++] = 0x00;
	// ������С
	APDUData[APDUDataLen++] = 0x12;
	APDUData[APDUDataLen++] = HIBYTE((WORD)tIap_698.wFrameSize);
	APDUData[APDUDataLen++] = LOBYTE((WORD)tIap_698.wFrameSize);
	// У��
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x16;
	APDUData[APDUDataLen++] = 0x00;
	// У��ֵ octet-str
	APDUData[APDUDataLen++] = 0x09;
	APDUData[APDUDataLen++] = 0x02;
	// У��ֵ
	APDUData[APDUDataLen++] = HIBYTE(tIap_698.wCrc16); // 0x05;
	APDUData[APDUDataLen++] = LOBYTE(tIap_698.wCrc16); // 0x83;	
	APDUData[APDUDataLen++] = 0x00; // ��������汾��
	APDUData[APDUDataLen++] = 0x00; // ����Ӳ���汾��
	APDUData[APDUDataLen++] = 0x00; // ���ط���ʶ
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
// ��������:  698���ļ�֡�����ֽ�
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void ComposeFileFrameLen(BYTE *pBuf, WORD *APDULen)
{
	WORD FrameSize = 0;

	if (MoniFrameNo < (tIap_698.wAllFrameNo - 1)) // ����֡
	{
		FrameSize = tIap_698.wFrameSize;
	}
	else if (MoniFrameNo == (tIap_698.wAllFrameNo - 1)) // ���һ֡
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
// ��������:  698��������֡
//
// ����:	pBuf ���뻺��
//
// ����ֵ: ֡����
//
// ��ע:
//--------------------------------------------------
WORD ComposeWriteFileFrame(BYTE *pBuf)
{
	WORD wLen = 0;
	TSA tSA;
	BYTE i = 0;
	BYTE APDUData[APDU_DATA_MAX_LEN];
	WORD APDUDataLen = 0;
	WORD ReadFileSize = 0;	
	memset(APDUData, 0, sizeof(APDUData));
	memset(&tSA, 0, sizeof(TSA));
	// ���ַ��
	tSA.AF = 0x05; // 00000110;����ַ�����߼���ַ����ַ����6�ֽ�
	for (i = 0; i < ((tSA.AF & ADDR_BITS) + 1); i++)
	{
		tSA.addr[i] = bAddr[i];
	}	
	wLen = MakeFrameHead(&tSA, pBuf, 0); // �ͻ���ַ	
	APDUData[APDUDataLen++] = 0x02;
	APDUData[APDUDataLen++] = 0x02;
	// �����
	APDUData[APDUDataLen++] = 0x12;	
	APDUData[APDUDataLen++] = HIBYTE(MoniFrameNo);
	APDUData[APDUDataLen++] = LOBYTE(MoniFrameNo);	
	// ������
	APDUData[APDUDataLen++] = 0x09;
	// ֡��С
	ComposeFileFrameLen(APDUData, &APDUDataLen);	
	// if(tIap_698.dwIapFlag == BASEMETER_UPGRADES)
	// {
	// �ļ�����
	if (MoniFrameNo < tIap_698.wAllFrameNo)
	{	
		// if(MoniFrameNo == 154)
		// {	
		// }
		nwy_file_seek(MeterUpgradeFD, tIap_698.wFrameSize * MoniFrameNo, NWY_SEEK_SET);
		if (MoniFrameNo < (tIap_698.wAllFrameNo - 1)) // ���һ֮֡ǰ������֡
		{
			ReadFileSize = tIap_698.wFrameSize;
		}
		else if (MoniFrameNo == (tIap_698.wAllFrameNo - 1)) // ���һ֡�����������ܲ�����
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
		nwy_file_read(MeterUpgradeFD, &APDUData[APDUDataLen], ReadFileSize);
		APDUDataLen += ReadFileSize;
		nwy_ext_echo("\r\nsend frame no:%d\r\n", MoniFrameNo);
	}
	wLen += ComposeApduPack(0x08, &pBuf[wLen], APDUData, APDUDataLen);
	wLen = Add698FrameTail(0x43, wLen - 1, pBuf);
	return wLen;
}
//--------------------------------------------------
// ��������:  698���Ͷ�ȡ������֡
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������:  698���Ͳ�ѯ�������֡
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
// ��������:  698����ִ������֡
//
// ����:
//
// ����ֵ:
//
// ��ע:
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
	// ���ֽ�ʱ��
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
// ��������:  698����������һ��//�������ݰ�//���Ϳ�ʼ������
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
void SendIapData698(void)
{
	WORD wLen = 0;
	BYTE Buf[MAX_FRAME_LEN] = {0};
	// WORD i = 0;
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_meter_upgrade_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	if (MoniIapNo == 0) // �鱨�ķ����������󣬶�����������֡��
	{
		if (GetUpGradeFileMessage() == FALSE)
		{
			ExitUpgrade698(eUpgradeVeriErr);
			return;
		}
		wLen = ComposeStartTranFrame(Buf);
		nwy_sdk_timer_stop(meter_upgrade_timer);
		OneTimeTImerPara.expired_time = 360000;
		nwy_sdk_timer_start(meter_upgrade_timer, &OneTimeTImerPara); //115200 ��������ʱ��ʵ��С��4����  ��1.5��Ԥ��
		MeterUpgradeTimeUpFlag = 0;
		nwy_ext_echo("\r\n send start frame\r\n");
	}
	else if (MoniIapNo == 1) // ����������ʼ��
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

		nwy_ext_echo("\r\n send execute upgrade frame\r\n");
		wLen = ComposeExecuteUpgradeFrame(Buf);
	}

	// for ( i = 0; i < wLen; i++)
	// {
	// 	nwy_ext_echo("%02x ", Buf[i]);
	// }
	 nwy_uart_write(UART_HD, Buf, wLen);
	
}
//--------------------------------------------------
// ��������:  ��ȡ�����ļ���Ϣ
//
// ����:
//
// ����ֵ:
//
// ��ע:
//--------------------------------------------------
BOOL GetUpGradeFileMessage(void)
{
	WORD w = 0, wCRC16 = 0, LastSize = 0;
	BYTE IapBuf[FILE_BLOCK_SIZE + 50] = {0};
	char crcStr[5] = {0};

	nwy_ext_echo("\r\nopen file:%s", upgrade_file_path_698);
	tIap_698.dwFileSize = nwy_file_path_size(upgrade_file_path_698);
	nwy_ext_echo("\r\nfile size  :%d", tIap_698.dwFileSize);
	tIap_698.wFrameSize = FILE_BLOCK_SIZE;
	
	if ((tIap_698.dwFileSize % tIap_698.wFrameSize) != 0)
	{
		tIap_698.wAllFrameNo = tIap_698.dwFileSize / tIap_698.wFrameSize + 1; // ������240k������  ��ȷ���Ժ󷢵�bin�ļ���С�Ƿ�̶�
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
		MeterUpgradeFD = nwy_file_open(upgrade_file_path_698, NWY_RDONLY);	
		nwy_ext_echo("\r\nMeterUpgradeFD:%d",MeterUpgradeFD);
	}
	
	//�ж�.bin֮ǰ��ΪУ�飬�ڴ˴�����bin�ļ����ļ���������У���Ƿ�һ�£���һ�����˳����������������־
	if (MeterUpgradeFD < 0)
	{
		nwy_ext_echo("\r\nopen app file fail");
		return FALSE;
	}
	wCRC16 = 0xffff; // ����ֵ
	for (w = 0; w < tIap_698.wAllFrameNo; w++)//�ж�У��
	{
		nwy_file_seek(MeterUpgradeFD, tIap_698.wFrameSize * w, NWY_SEEK_SET);
		nwy_file_read(MeterUpgradeFD, IapBuf, tIap_698.wFrameSize);
		if (w == (tIap_698.wAllFrameNo - 1))
		{
			wCRC16 = ContinueFcs16(IapBuf, LastSize, wCRC16, TRUE); // ����У��
			tIap_698.wCrc16 = wCRC16;
			nwy_ext_echo("\r\n calculated crc16:%04x", wCRC16);
			nwy_ext_echo("\r\nfile path:s", upgrade_file_path_698);
			sprintf(crcStr, "%04X", tIap_698.wCrc16); // �� CRC16 ת��Ϊ�ַ�����ʽ,��д��ʽ
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
			wCRC16 = ContinueFcs16(IapBuf, tIap_698.wFrameSize, wCRC16, FALSE); // ����У��
		}
	}

    return TRUE;
}

//--------------------------------------------------
//��������: 698��������
//
//����:��
//
//����ֵ:��
//
//��ע:
//--------------------------------------------------
void  MeterUpgrade698_Task( void  )
{
	nwy_timer_para_t OneTimeTImerPara =
	{
		.expired_time = 0,
		.type = NWY_TIMER_ONE_TIME,
		.cb = nwy_uart_timer_cb,
		.cb_para = NULL,
		.thread_hdl = Timer_thread,
	};
	if ((api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE)&&(api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE))
	{
		if (MeterUpgradeTimeUpFlag == 1) //15����δ���������˳�
		{
			ExitUpgrade698(eUpgradeWholeProTimeoutErr);
			return;
		}
		if (api_GetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME))
		{
			SendIapData698();
			api_ClrSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME);
			nwy_sdk_timer_stop(uart_timer);
			OneTimeTImerPara.expired_time = 1000;
			nwy_sdk_timer_start(uart_timer, &OneTimeTImerPara); // �����ش��޸�״̬λ��Ҫʱ��
		}
		else
		{
			if (api_GetSysStatus(eSYS_STASUS_TIMER_UP)) // 1s��ʱ����û���յ�����
			{
				nwy_ext_echo("\r\n upgrade timeout !!!");
				api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
				TimeoutRetryNum++; // ����ʧ��
				nwy_ext_echo("\r\n upgrade retry num:%d", TimeoutRetryNum);
				if (TimeoutRetryNum >= 3)
				{
					ExitUpgrade698(eUpgrdeFrameTimeoutErr);
					// ���Դ����ﵽ���ƣ��ж�����ʧ��
					nwy_ext_echo("\r\n exit upgrade  !!!");
				}
				else
				{
					// ��δ�ﵽ���Դ����������ط�����֡
					api_SetSysStatus(eSYS_STASUS_UPGRADE_REPLY_FRAME); // ����״̬��׼���ط�����
				}
			}
		}
	}
}