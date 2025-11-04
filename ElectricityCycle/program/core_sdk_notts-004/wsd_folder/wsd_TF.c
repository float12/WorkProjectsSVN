//----------------------------------------------------------------------
// Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
// 创建者:
// 创建日期:
// 描述:
// 修改记录:
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_uart.h"

//-----------------------------------------------
// 文件使用的宏定义
//-----------------------------------------------
#define WriteOneFileFullTime 		35														//一直写写满一个文件的时长（分钟），目前由电表控制发送最后一帧表示结束
#define MAX_WAVE_DATA_FILE_SIZE 	(WAVE_DATA_FRAME_SIZE * 50 * WriteOneFileFullTime * 60) 	// 一个文件的最大大小
#define FRAME_TYPE_OFFSET 			790	 // 帧类型字节位置：从后往前偏移790个字节
#define CHARGE_START_FRAME_TYPE 	0x01
#define CHARGE_MIDDLE_FRAME_TYPE 	0x02
#define CHARGE_END_FRAME_TYPE 		0x03
#define TF_REQUEST_TIME 			(1000 * 60) // 请求上传间隔时间 ms
#define TF_RECV_UART_QUEUE_REMAIN	1			//tf接收波形队列 剩余空间
#define FACTORY_TF_FILE				"/sdcard0/"				//厂内初始化TF卡 路径
//-----------------------------------------------
// 文件使用的结构体、共用体、枚举
//-----------------------------------------------

typedef enum
{
	eFirstFrame = 1,
	eMiddleFrame,
	eLastFrame
} eFrameType; // 波形数据的帧类型

typedef enum
{
	eSTATE_CHECK_FILE_TO_READ = 0,		// 0
	eSTATE_REQUEST_UPLOAD_FILE,			// 1
	eSTATE_WAIT_REQUEST_RESULT,			// 2
	eSTATE_READ_DATA,					// 3
	eSTATE_SEND_TO_QUEUE,				// 4
	eSTATE_ONE_CHARGE_UPLOAD_FINISH, 	// 一次充电波形结束
} eUploadFileState;



typedef struct
{
	BYTE isFinalFrame;
	TInfoFileHead infoFileHead;
	eUploadFileState state;
	DWORD SendCnt;
	int ReadFileFd;
	BYTE ReadFileName[MAX_FILE_NAME_LEN];
	WORD WaitCnt;
	FileInfoOfOneCharge *pInfoOfOneCharge;
	BYTE isSuccess;
} TReadAndUploadFile;
//-----------------------------------------------
// 全局使用的变量、常量
//-----------------------------------------------
BYTE RequestLoop = 0;
TInfoFile infoFile;
BYTE WaveQueueFullFlag = 0;	// 波形数据队列是否满,满置1，用于长期监视
BYTE RecLenExceed4096Flag = 0; // 接收波形数据长度超过4096字节标志
BYTE RecLenNot815Flag = 0; // 接收波形数据长度不是815字节标志
//-----------------------------------------------
// 文件使用的变量、常量
//-----------------------------------------------
static BYTE RequestTimeUpFlag = 1;		//请求主站失败重发标志
static WaveDatatoTcp DataToTcp;			//TF卡 上传数据到tcp的缓存
static BYTE WriteBuf[WAVE_DATA_FRAME_SIZE * WAVE_UART_QUEUE_SPACE];
static DWORD TotalBytesWritten = 0; 	// 一个文件已写入的总字节数

static DWORD WriteMaxCycleOneTime = 0;
static DWORD QueueSpace = 5;
static DWORD WriteTFmaxtime = 1;
static BYTE	 PowerDownFlag = 0;
//-----------------------------------------------
// 内部函数声明
//-----------------------------------------------

//-----------------------------------------------
// 函数定义
//-----------------------------------------------
void TF_Request_timer_cb(void *param)
{
	RequestTimeUpFlag = 1;
}
//--------------------------------------------------
//功能描述:  删除TF卡内所有波形数据
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_DeleteAllWaveFile( void )
{
	int fd = -1;
	DWORD offset;
	FileInfoOfOneCharge fileToDelete;
	BYTE AllZero[sizeof(FileInfoOfOneCharge)] = {0};
	BYTE fullPath[MAX_FILE_NAME_LEN + 10] = "/sdcard0/";

	fd = nwy_sdk_fopen(INFO_FILE_PATH,NWY_RDWR);

	if (fd >=0 )
	{
		for (BYTE i = 0; i < MAX_CHARGE_SAVE_NUM; i++)
		{
			offset = i*sizeof(FileInfoOfOneCharge) + sizeof(TInfoFileHead);
			nwy_sdk_fseek(fd, offset, NWY_SEEK_SET);
			// 获取要删除的文件名
			if(nwy_sdk_fread(fd, &fileToDelete, sizeof(FileInfoOfOneCharge)) == sizeof(FileInfoOfOneCharge))
			{
				// nwy_ext_echo("\r\n dele file is %s",fileToDelete.FirstFileName);
				if (memcmp(&fileToDelete, AllZero, sizeof(FileInfoOfOneCharge)) != 0) //
				{
					strcat((char*)&fullPath, (char*)&fileToDelete.FirstFileName);
					if (nwy_sdk_fexist((char*)&fullPath) == TRUE)
					{
						nwy_sdk_file_unlink((char*)&fullPath);
						// nwy_ext_echo("\r\n Deleted file: %s", fullPath);
					}		
				}	
			}
			memset(AllZero,0,sizeof(AllZero));
			memset(fullPath,0,sizeof(AllZero));
			memcpy(fullPath,FACTORY_TF_FILE,sizeof(FACTORY_TF_FILE));
			nwy_sleep(100);
		}		
	}
	if(nwy_sdk_file_unlink(INFO_FILE_PATH) != NWY_SUCESS)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
//--------------------------------------------------
// 功能描述: 更新一次充电波形后的信息文件
//
// 参数:
//   type: 0-写完一次充电波形后更新，1-读完一次充电波形文件后更新，不需要传入文件名和数量
//   FirstFileName: 第一个文件名
//   cnt: 文件数量
//
// 返回值: 无
//
// 备注内容: 无
//--------------------------------------------------
void UpdateInfoFile(BYTE type, BYTE *FirstFileName, WORD cnt,BYTE LoopNum)
{
	int fd = -1;
	TInfoFileHead infoFileHead;
	FileInfoOfOneCharge OneChargeToUpdate;
	// WORD crc16 = 0;
	DWORD UpdateOffset = 0;
	memset(&infoFileHead, 0, sizeof(infoFileHead));
	memset(&OneChargeToUpdate, 0, sizeof(OneChargeToUpdate));
	BYTE AllZero[sizeof(FileInfoOfOneCharge)] = {0};
	FileInfoOfOneCharge fileToDelete;
	BYTE fullPath[MAX_FILE_NAME_LEN + 10] = "/sdcard0/";

	memset(&infoFileHead, 0, sizeof(infoFileHead));
	memset(&OneChargeToUpdate, 0, sizeof(OneChargeToUpdate));
	fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_CREAT | NWY_RDWR);
	if (fd == -1)
	{
		nwy_ext_echo("\r\n open InfoFile fail");
		return;
	}
	nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
	nwy_sdk_fread(fd, &infoFileHead, sizeof(infoFileHead)); //   nwy_ext_echo("\r\n read:");

	// print_infoFile(&infoFile);
	// if (CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD)) != infoFile.crc16)
	// {
	//	 nwy_ext_echo("\r\n read InfoFile, crc error");
	//	 return;
	// }
	if (type == 0)//写完一次后检查卡是否满
	{
		nwy_ext_echo("\r\n infoFileHead.WriteOffset is %d",infoFileHead.WriteOffset);
		UpdateOffset = infoFileHead.WriteOffset * sizeof(FileInfoOfOneCharge) + sizeof(TInfoFileHead);
		nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
		nwy_sdk_fread(fd, &OneChargeToUpdate, sizeof(OneChargeToUpdate));
		memcpy(OneChargeToUpdate.FirstFileName, FirstFileName, MAX_FILE_NAME_LEN);
		OneChargeToUpdate.CNT = cnt;
		OneChargeToUpdate.Loop = LoopNum;
		nwy_ext_echo("\r\n first loop is %d",LoopNum);
		nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
	 	nwy_sdk_fwrite(fd, &OneChargeToUpdate, sizeof(OneChargeToUpdate));
		infoFileHead.WriteOffset = (infoFileHead.WriteOffset + 1) % MAX_CHARGE_SAVE_NUM;
		infoFileHead.ChargeCnt++;
		// 检查卡是否已满
		if(infoFileHead.ChargeCnt == MAX_CHARGE_SAVE_NUM)
		{
				nwy_ext_echo("\r\n Card is full, WriteOffset:%d, ReadOffset:%d",
							 infoFileHead.WriteOffset, infoFileHead.ReadOffset);
			}
	}
	else
	{
		UpdateOffset = infoFileHead.ReadOffset * sizeof(FileInfoOfOneCharge) + sizeof(TInfoFileHead);
		nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
		nwy_sdk_fread(fd, &OneChargeToUpdate, sizeof(OneChargeToUpdate));
		memset(&OneChargeToUpdate, 0, sizeof(FileInfoOfOneCharge));
		infoFileHead.ReadOffset = (infoFileHead.ReadOffset + 1) % MAX_CHARGE_SAVE_NUM;
		// 读完一次后判断卡是否满，如果满了则删除deleteoffset处的信息并更新偏移
		if (infoFileHead.ChargeCnt == MAX_CHARGE_SAVE_NUM)
		{
			// 清除DeleteOffset位置的FileInfoOfOneCharge信息
			UpdateOffset = infoFileHead.DeleteOffset * sizeof(FileInfoOfOneCharge) + sizeof(TInfoFileHead);
			nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
			// 获取要删除的文件名

			nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
			nwy_sdk_fread(fd, &fileToDelete, sizeof(FileInfoOfOneCharge));
			  // 删除文件
			if (memcmp(&fileToDelete, AllZero, sizeof(FileInfoOfOneCharge)) != 0) //
			{
				strcat((char*)&fullPath, (char*)&fileToDelete.FirstFileName);
				if (nwy_sdk_fexist((char*)&fullPath) == TRUE)
				{
					nwy_sdk_file_unlink((char*)&fullPath);
					nwy_ext_echo("\r\n Deleted file: %s", fullPath);
				}
			}
			memset(&OneChargeToUpdate, 0, sizeof(FileInfoOfOneCharge));
			nwy_sdk_fseek(fd, UpdateOffset, NWY_SEEK_SET);
			nwy_sdk_fwrite(fd, &OneChargeToUpdate, sizeof(FileInfoOfOneCharge));

			// 环形更新DeleteOffset
			infoFileHead.DeleteOffset = (infoFileHead.DeleteOffset + 1) % MAX_CHARGE_SAVE_NUM;
			infoFileHead.ChargeCnt--;
			nwy_ext_echo("\r\n Card is not full now, WriteOffset:%d, ReadOffset:%d, DeleteOffset:%d",
						 infoFileHead.WriteOffset, infoFileHead.ReadOffset, infoFileHead.DeleteOffset);
		}
	}
	// infoFileHead.crc16 = CalCRC16(&infoFileHead, sizeof(infoFileHead) - sizeof(WORD));
	nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
	nwy_sdk_fwrite(fd, &infoFileHead, sizeof(infoFileHead));
	nwy_sdk_fclose(fd);
	nwy_ext_echo("\r\n after Update,InfoFile:");
	// print_infoFile(&infoFileHead);
}

//--------------------------------------------------
// 功能描述: 挂载TF卡
//
// 参数: 无
//
// 返回值: TRUE - 成功，FALSE - 失败
//
// 备注内容: 无
//--------------------------------------------------
BYTE api_CheckaAndMountSDCard(void)
{
	if (nwy_read_sdcart_status() == FALSE) // 卸载状态
	{
		// nwy_ext_echo("\r\n nwy_read_sdcart_status fail");
		if (nwy_sdk_sdcard_mount() == FALSE)
		{
			// nwy_ext_echo("\r\n nwy_sdk_sdcard_mount fail");
			return FALSE;
		}
	}
	// nwy_ext_echo("\r\n api_CheckaAndMountSDCard success");
	return TRUE;
}

//--------------------------------------------------
// 功能描述:  698格式判断
//
// 参数:
// data: 数据指针
// length: 数据长度
//
// 返回值: 0 错误，1 起始帧 2 中间数帧，3 最后一帧
//
// 备注内容: 串口格式判断需要TSerial结构体，此函数简单判断下
//--------------------------------------------------
BYTE Check698Format(BYTE *data, DWORD length,BYTE *Loop)
{
	// for (size_t i = 0; i < length; i++)
	// {
	//	 nwy_ext_echo("start is %02X ", data[0]);
	// 	nwy_ext_echo("end is%02X ", data[814]);
	// }

	if (data[0] != 0x68 || data[length - 1] != 0x16)
	{
		nwy_ext_echo("\r\n 698 head or tail error");
		return FALSE;
	}
	// 计算帧头校验值
	if (api_fcs16(&data[1], 11) != (data[13] << 8 | data[12]))
	{
		nwy_ext_echo("\r\n 698 head crc error,data crc:%04x,calc:%04x", (data[12] << 8 | data[13]), api_fcs16(&data[1], 11));
		return FALSE;
	}
	// 计算整帧校验值
	if (api_fcs16(&data[1], WAVE_DATA_FRAME_SIZE - 4) != (data[WAVE_DATA_FRAME_SIZE - 2] << 8 | data[WAVE_DATA_FRAME_SIZE - 3]))
	{
		nwy_ext_echo("\r\n 698 tail crc error,data crc:%04x,calc:%04x", (data[WAVE_DATA_FRAME_SIZE - 1] << 8 | data[WAVE_DATA_FRAME_SIZE]), api_fcs16(&data[1], WAVE_DATA_FRAME_SIZE - 4));
		return FALSE;
	}
	// 计算OAD值
	if (*(DWORD *)&data[17] != 0x4444eeee)
	{
		nwy_ext_echo("\r\n oad err", *(DWORD *)&data[17]);
		return FALSE;
	}
	// 判断帧类型
	if (data[25] == 0x01)
	{
		*Loop = data[27];
		return eFirstFrame;
	}
	else if (data[25] == 0x02)
	{
		return eMiddleFrame;
	}
	else if (data[25] == 0x03)
	{
		return eLastFrame;
	}
	else
	{
		nwy_ext_echo("\r\n 698 frame type err");
		return FALSE;
	}
}
void print_infoFile(const TInfoFileHead *infoFileHead)
{
	// 打印写入数据文件??
	nwy_ext_echo("\r\nWriteDataFileName: %s\n", infoFileHead->WriteDataFileName);

	// 打印每???充电的文件信息
	// for (int i = 0; i < MAX_CHARGE_SAVE_NUM; i++)
	{
		// nwy_ext_echo("FileInfoOfCharges[%d].FirstFileName: %s\n",
					//  i, infoFile->FileInfoOfCharges[i].FirstFileName);
		// nwy_ext_echo("FileInfoOfCharges[%d].CNT: %d\n",
					//  i, infoFile->FileInfoOfCharges[i].CNT);
	}

	// 打印读写偏移量和 crc16 校验??
	nwy_ext_echo("ReadOffset: %d\n", infoFileHead->ReadOffset);
	nwy_ext_echo("WriteOffset: %d\n", infoFileHead->WriteOffset);
	nwy_ext_echo("DeleteOffset: %d\n", infoFileHead->DeleteOffset);
	// nwy_ext_echo("crc16: %04x\n", infoFile->crc16);
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
//
// 返回值:
// 备注信息:
//--------------------------------------------------

void ReadFileTask(void)
{
	BYTE result = 0;
	DWORD ReadBytes = 0;
	int fd = -1;
	DWORD lastReadNumber = 0,RecordOffset = 0;
	BYTE AllZero[sizeof(FileInfoOfOneCharge)] = {0};
	static TReadAndUploadFile ReadAndUploadFile = {0};
	FileInfoOfOneCharge TmpInfo;

	memset(&TmpInfo,0,sizeof(TmpInfo));
	// 检查是否有存满的文?
	if ((tcp_connect_flag[0] == 1))
	{
		// nwy_ext_echo("\r\n tf task state:%d,DataToTcp.typeis %d", ReadAndUploadFile.state,DataToTcp.type);
		
		switch (ReadAndUploadFile.state)
		{
		case eSTATE_CHECK_FILE_TO_READ:
			if (nwy_sdk_fexist(INFO_FILE_PATH) == TRUE)
			{
				fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDONLY);
				if (fd == -1)
				{
					nwy_ext_echo("\r\n check file to read ,Open InfoFile fail");
					break;
				}
				nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
				nwy_sdk_fread(fd,&ReadAndUploadFile.infoFileHead, sizeof(TInfoFileHead)); //只读头部大小);
				// 计算单条记录在文件中的偏移：
				RecordOffset =sizeof(TInfoFileHead) + ReadAndUploadFile.infoFileHead.ReadOffset * sizeof(FileInfoOfOneCharge);
				nwy_sdk_fseek(fd, RecordOffset, NWY_SEEK_SET);
				nwy_sdk_fread(fd,&TmpInfo,sizeof(FileInfoOfOneCharge));
				nwy_sdk_fclose(fd);
				ReadAndUploadFile.pInfoOfOneCharge = &TmpInfo;
				if (memcmp(ReadAndUploadFile.pInfoOfOneCharge, AllZero, sizeof(FileInfoOfOneCharge)) != 0) //
				{
					nwy_ext_echo("\r\n read InfoFile, has data to upload");
					print_infoFile(&ReadAndUploadFile.infoFileHead);
					strcpy((char*)&ReadAndUploadFile.ReadFileName, "/sdcard0/");
					strcat((char*)&ReadAndUploadFile.ReadFileName, (char*)&ReadAndUploadFile.pInfoOfOneCharge->FirstFileName);
					ReadAndUploadFile.ReadFileFd = nwy_sdk_fopen((char*)&ReadAndUploadFile.ReadFileName, NWY_RDONLY);
					if (ReadAndUploadFile.ReadFileFd != -1)
					{
						nwy_ext_echo("\r\n Open read  File fd success,read fd:%d", ReadAndUploadFile.ReadFileFd);
						ReadAndUploadFile.state = eSTATE_REQUEST_UPLOAD_FILE;
						DataToTcp.LoopNum = ReadAndUploadFile.pInfoOfOneCharge->Loop;
					}
					else
					{
						nwy_ext_echo("\r\n Open read  File fd fail");
						ReadAndUploadFile.state = eSTATE_CHECK_FILE_TO_READ; // 文件打开失败，直接退?
					}
				}
				break;
			}
		case eSTATE_REQUEST_UPLOAD_FILE:
			if (RequestTimeUpFlag == 1)
			{
				DataToTcp.type = 1; //请求上传文件
				RequestLoop = DataToTcp.LoopNum;
				if (nwy_put_msg_que(WaveDataUploadMessageQueue, &DataToTcp, 0xffffffff) == TRUE)
				{
					nwy_ext_echo("\r\n send request ok");
					ReadAndUploadFile.state = eSTATE_WAIT_REQUEST_RESULT;
					RequestTimeUpFlag = 0;
				}
				else
				{
					nwy_ext_echo("\r\n send request fail");
				}
			}
			break;
		case eSTATE_WAIT_REQUEST_RESULT:
			if (nwy_get_msg_que(TcpUserServertoTFMsgQue, &result, 0xffffffff) == TRUE) //立即返回
			{
				if (result == FALSE)
				{
					nwy_ext_echo("\r\n request result fail");	//主站资源有限 ？
					ReadAndUploadFile.state = eSTATE_REQUEST_UPLOAD_FILE;
					nwy_start_timer(TF_Requset_timer, TF_REQUEST_TIME);
				}
				else if (result == TRUE)
				{
					nwy_ext_echo("\r\n request result ok");
					ReadAndUploadFile.state = eSTATE_READ_DATA; // 发送成功，直接退?
				}
			}
			else
			{
				ReadAndUploadFile.state = eSTATE_WAIT_REQUEST_RESULT;
			}
			break;
		case eSTATE_READ_DATA:
			ReadBytes = nwy_sdk_fread(ReadAndUploadFile.ReadFileFd, &DataToTcp, SEND_WAVE_DATA_LEN);
			if ((ReadBytes == SEND_WAVE_DATA_LEN)
			||(ReadBytes == (WAVE_DATA_FRAME_SIZE*2))
			||(ReadBytes == WAVE_DATA_FRAME_SIZE))
			{
				if (DataToTcp.buf[ReadBytes - FRAME_TYPE_OFFSET] == CHARGE_END_FRAME_TYPE) //一次充电?取结束
				{
					nwy_ext_echo("\r\n charge end frame,one charge upload finish");
					memset(&DataToTcp.buf[ReadBytes],0,SEND_WAVE_DATA_LEN-ReadBytes);
					ReadAndUploadFile.isFinalFrame = 1;
				}
				ReadAndUploadFile.state = eSTATE_SEND_TO_QUEUE;
			}
			else if(ReadBytes == 0)//读取文件结束
			{
				nwy_ext_echo("\r\n Read one File end,read next file");
				nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
				ReadAndUploadFile.ReadFileFd = -1;
				nwy_ext_echo("\r\nlast ReadFileName:%s", ReadAndUploadFile.ReadFileName);
				sscanf((char*)&ReadAndUploadFile.ReadFileName, "/sdcard0/%ld-", &lastReadNumber);
				lastReadNumber++;
				memset(ReadAndUploadFile.ReadFileName, 0, sizeof(ReadAndUploadFile.ReadFileName));
				sprintf((char*)&ReadAndUploadFile.ReadFileName, "/sdcard0/%ld-1p.bin", lastReadNumber);
				nwy_ext_echo("\r\nnew ReadFileName:%s", ReadAndUploadFile.ReadFileName);
				nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
				ReadAndUploadFile.ReadFileFd = nwy_sdk_fopen((char*)&ReadAndUploadFile.ReadFileName, NWY_RDONLY);
				if (ReadAndUploadFile.ReadFileFd == -1)
				{
					nwy_ext_echo("\r\n Open read  File fd fail");
					ReadAndUploadFile.state = eSTATE_ONE_CHARGE_UPLOAD_FINISH;
				}
				else
				{
					ReadAndUploadFile.state = eSTATE_READ_DATA;
				}
			}
			else
			{
				nwy_ext_echo("\r\n Read File fail,ReadBytes=%d,ReadFileFd:%d", ReadBytes, ReadAndUploadFile.ReadFileFd);
				ReadAndUploadFile.state = eSTATE_ONE_CHARGE_UPLOAD_FINISH;
			}
			break;
		case eSTATE_SEND_TO_QUEUE:
			if (nwy_get_queue_spaceevent_cnt(WaveDataUploadMessageQueue) != 0)
			{
				DataToTcp.type = 0;
				if (nwy_put_msg_que(WaveDataUploadMessageQueue, &DataToTcp, 100) == TRUE)
				{
					ReadAndUploadFile.SendCnt++;
					if (ReadAndUploadFile.SendCnt % 100 == 0)
					{
						// nwy_ext_echo("\r\ntf send wave data to tcp ok,SendCnt:%d", ReadAndUploadFile.SendCnt);
					}
					if (ReadAndUploadFile.isFinalFrame == 1)
					{
						nwy_ext_echo("\r\n one charge upload finish");
						ReadAndUploadFile.isFinalFrame = 0;
						ReadAndUploadFile.isSuccess = 1;
						ReadAndUploadFile.state = eSTATE_ONE_CHARGE_UPLOAD_FINISH;
					}
					else
					{
						ReadAndUploadFile.state = eSTATE_READ_DATA;
					}
				}
				else
				{
					nwy_ext_echo("\r\n send data to queue fail,try again");
				}
			}
			else
			{
				// nwy_ext_echo("\r\ndata queue is full,wait");
			}
			break;
		case eSTATE_ONE_CHARGE_UPLOAD_FINISH: //成功或失败后初始化变量.
			if (ReadAndUploadFile.isSuccess == 1)
			{
				nwy_ext_echo("\r\nfinish one charge upload");
				UpdateInfoFile(1, NULL, 0,0);
			}
			nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
			memset(&ReadAndUploadFile, 0, sizeof(ReadAndUploadFile));
			ReadAndUploadFile.ReadFileFd = -1;
			RequestTimeUpFlag = 1;
			break;
		}
	}
}

BYTE IsCardFull(void)
{
	int fd = -1;
	TInfoFileHead infoFileHead;
	
	memset(&infoFileHead, 0, sizeof(TInfoFileHead));
	fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDWR);
	if (fd == -1)
	{
		nwy_ext_echo("\r\n open InfoFile fail");
		return FALSE;
	}
	nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
	nwy_sdk_fread(fd, &infoFileHead, sizeof(infoFileHead));
	nwy_sdk_fclose(fd);
	if(infoFileHead.ChargeCnt == MAX_CHARGE_SAVE_NUM)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//-----------------------------------------------
// 函数功能: 创建新文件
//
// 参数: NameOfFile - 创建的额文件名
//
// 返回值:
//
// 备注: 创建新文件，并更新文件名和序号
//-----------------------------------------------
int CreateNextWaveFile(BYTE *NameOfFile,BYTE loop)
{
	// 尝试打开保存文件名的文件
	int infoFd = -1;
	int fd = -1;
	BYTE AllZero[MAX_FILE_NAME_LEN] = {0};
	DWORD lastWriteNumber;
	BYTE OldDataFileName[MAX_FILE_NAME_LEN] = {0};
	BYTE NewDataFileName[MAX_FILE_NAME_LEN] = {0};
	BYTE tmp[MAX_FILE_NAME_LEN + 20] = "/sdcard0/";
	nwy_time_t julian_time = {0};
    char timezone = 0;
                    
	nwy_get_time(&julian_time, &timezone);
	infoFd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDWR);
	if (infoFd < 0)
	{
		nwy_ext_echo("\r\ncreate next wave file open InfoFile fail");
		return -1;
	}
	// 读取上一次的文件名
	nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);//上一次的文件名的偏移位置为0
	nwy_sdk_fread(infoFd, OldDataFileName, sizeof(OldDataFileName));
	// nwy_ext_echo("\r\n before create");
	// print_infoFile(&infoFile);
	if (strncmp((char*)&OldDataFileName, (char*)&AllZero, sizeof(OldDataFileName)) == 0)
	{
		sprintf((char*)NewDataFileName, "1-L%d-%d-%d-%d-%d.bin",loop ,julian_time.mon,julian_time.day,julian_time.hour,julian_time.min);
		// 全0表示这是第一个波形文件
		// memcpy(WriteDataFileName, "1-1p.bin", sizeof(WriteDataFileName));
		// memset(&WriteDataFileName[8], 0, sizeof(WriteDataFileName) - 8);
		// infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD));
		// nwy_ext_echo("\r\n after create");
		// print_infoFile(&infoFile);
		// 创建并保存文件名
		nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);
		int WriteBytes = nwy_sdk_fwrite(infoFd, NewDataFileName, sizeof(NewDataFileName));
		nwy_ext_echo("\r\n file name :%s", NewDataFileName);
		nwy_ext_echo("\r\n WriteBytes:%d", WriteBytes);
		nwy_sdk_fclose(infoFd);
	}
	else
	{
		// 解析文件编号并加1
		if (sscanf((char*)OldDataFileName, "%ld-", &lastWriteNumber) == 1)
		{
			lastWriteNumber = lastWriteNumber % MAX_CHARGE_SAVE_NUM + 1;
			sprintf((char*)NewDataFileName, "%ld-L%d-%d-%d-%d-%d.bin",lastWriteNumber,loop ,julian_time.mon,julian_time.day,julian_time.hour,julian_time.min);
			nwy_ext_echo("\r\nnew file name:%s", NewDataFileName);
			// infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD));
			// 保存新文件名
			nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);
			nwy_sdk_fwrite(infoFd, NewDataFileName, sizeof(NewDataFileName));
			nwy_sdk_fclose(infoFd);
		}
		else
		{
			nwy_sdk_fclose(infoFd);
			nwy_ext_echo("\r\n sscanf No error");
		}
	}
	strcat((char*)&tmp, (char*)NewDataFileName);
	nwy_ext_echo("\r\n data file name:%s", tmp);
	// 打开新文件
	fd = nwy_sdk_fopen((char*)&tmp, NWY_CREAT | NWY_RDWR);
	if (fd >= 0)
	{
		// 重置已写入的总字节数
		TotalBytesWritten = 0;
		memcpy(NameOfFile, NewDataFileName, sizeof(NewDataFileName));
		return fd;
	}
	else
	{
		nwy_ext_echo("\r\n Create new wave file fail");
		return -1;
	}
}
//--------------------------------------------------
// 功能描述:
//
// 参数:
//
//
//
// 返回??:
//
// 备注内???:
//--------------------------------------------------
void WriteFileTask(void)
{	
	int addr;
	long WriteBytes = 0;
	static int WriteFileFd = -1;
	DWORD dwTimeStart,dwTimeEnd;
	static WORD FileCntOfOneCharge = 0;
	static DWORD PrintCnt = 0,Hisnumber = 1,Newnumber = 0;
	BYTE result = 0,i = 0,LastFrameSign = 0,NameOfWriteFile[MAX_FILE_NAME_LEN] = {0};
	static BYTE bLoop = 0,FirstFileNameOfOneCharge[MAX_FILE_NAME_LEN] = {0},StartSaveData = 0;
	BYTE LeftQueueSpace = 0;

	if (nwy_get_msg_que(WaveDataToTFMessageQueue, &WriteBuf[0], 5) == TRUE) //	确保大部分时间在这里等待数据，有数据后立马读取
	{
		LeftQueueSpace  =  nwy_get_queue_pendingevent_cnt(WaveDataToTFMessageQueue);
		if(LeftQueueSpace >= 1)
		{
			nwy_ext_echo("\r\nhas more cycle data, LeftQueueSpace:%d",LeftQueueSpace);
		}
		for(i = 0; i < LeftQueueSpace; i++)
		{
			if(nwy_get_msg_que(WaveDataToTFMessageQueue, &WriteBuf[WAVE_DATA_FRAME_SIZE*(i+1)], 0xffffffff) == FALSE)
			{
				nwy_ext_echo("\r\n get left  msg from queue fail");
			}
			else
			{
				// nwy_ext_echo("\r\n get left  msg from queue suc");
			}
		}
		for (i = 0; i < LeftQueueSpace + 1; i++)
		{
			result = Check698Format(&WriteBuf[i * WAVE_DATA_FRAME_SIZE], WAVE_DATA_FRAME_SIZE,&bLoop);
			if (result == FALSE)
			{
				nwy_ext_echo("\r\n 698 frame format err");
				return;
			}
			else if (result == eFirstFrame) //写入该帧数据后创建新文件
			{
				nwy_ext_echo("\r\n get first frame");
				PrintCnt = 0;
				if (IsCardFull() == TRUE)
				{
					StartSaveData = 0;
				}
				else
				{
					StartSaveData = 1;
				}
				FileCntOfOneCharge = 0;
			}
			else if (result == eMiddleFrame) // 中间帧数据
			{
				if(StartSaveData == 1)
				{
					lib_ExchangeData((BYTE*)&Newnumber,(BYTE*)&WriteBuf[34+i * WAVE_DATA_FRAME_SIZE],4);
					// nwy_ext_echo("\r\n err number is %d",Newnumber);
					if ((Newnumber - Hisnumber) !=1)
					{
						nwy_ext_echo("\r\n err number is %d %d",Newnumber,Hisnumber);
						api_WriteSysUNMsg(DATA_LOST);
					}
					Hisnumber  = Newnumber;
				}
			}
			else if (result == eLastFrame) // 最后一??数据
			{
				nwy_ext_echo("\r\n get last frame");
				Hisnumber = 1;
				LastFrameSign = 1;
				i++;
				break;
			}
		}
		if (StartSaveData == 1)
		{
			// 写入数据到文件
			if (WriteFileFd == -1)
			{
				WriteFileFd = CreateNextWaveFile(NameOfWriteFile,bLoop);
				if (WriteFileFd < 0)
				{
					nwy_ext_echo("\r\n Create new wave file fail");
					return;
				}
				FileCntOfOneCharge++;
				nwy_ext_echo("\r\n new file name:%s,FileCntOfOneCharge:%d", NameOfWriteFile, FileCntOfOneCharge);
				if (FileCntOfOneCharge == 1) //保存一次充电的??一??文件??
				{
					memcpy(FirstFileNameOfOneCharge, NameOfWriteFile, sizeof(FirstFileNameOfOneCharge));
				}
			}
			dwTimeStart = nwy_get_ms();
			WriteBytes = nwy_sdk_fwrite(WriteFileFd, &WriteBuf[0], (WAVE_DATA_FRAME_SIZE*(1 + LeftQueueSpace)));	
			dwTimeEnd = nwy_get_ms();

			if ((dwTimeEnd - dwTimeStart) > WriteTFmaxtime)
			{
				WriteTFmaxtime = (dwTimeEnd - dwTimeStart);
				nwy_ext_echo("\r\n write file time %d,write cycles:%d,WriteMaxCycleOneTime:%d",(dwTimeEnd - dwTimeStart),1 + LeftQueueSpace,WriteMaxCycleOneTime);
				addr = GET_SAFE_SPACE_ADDR( ReportPara.WriteFileTime );
				api_OperateFileSystem(WRITE,addr,(BYTE *)&WriteTFmaxtime,sizeof(WriteTFmaxtime));
			}
			if(WriteMaxCycleOneTime > QueueSpace)
			{
				QueueSpace = WriteMaxCycleOneTime;
				api_WriteSysUNMsg(WAVE_QUEUE_SPACE_CHANGE);
				api_OperateFileSystem(WRITE,GET_SAFE_SPACE_ADDR( ReportPara.QueSpace ),(BYTE *)&QueueSpace,sizeof(QueueSpace));
			}
			if( WriteMaxCycleOneTime < (1 + LeftQueueSpace))
			{
				WriteMaxCycleOneTime = 1 + LeftQueueSpace;
				nwy_ext_echo("\r\n WriteOneTimeMaxCycle:%d",WriteMaxCycleOneTime);
			}
			
			if(LeftQueueSpace > 0)
			{
				nwy_ext_echo("\r\nwrite cycles %d",LeftQueueSpace + 1);
			}
			if (WriteBytes != (WAVE_DATA_FRAME_SIZE*(1 + LeftQueueSpace)))
			{
				nwy_ext_echo("\r\n Write full data len fail,WriteBytes:%d", WriteBytes);
				nwy_sdk_fclose(WriteFileFd);
				WriteFileFd = -1;
				return;
			}
			else
			{
				if(WaveQueueFullFlag == 1)
				{
					api_WriteSysUNMsg(WAVE_QUEUE_FULL);
					WaveQueueFullFlag = 0;
				}
				if(RecLenExceed4096Flag == 1)
				{
					api_WriteSysUNMsg(UART_RECV_EXCEED_4096);
					RecLenExceed4096Flag = 0;
				}
				if(RecLenNot815Flag == 1)
				{
					api_WriteSysUNMsg(UART_RECV_NO_815);
					RecLenNot815Flag = 0;
				}
				PrintCnt++;
				if ((PrintCnt % 5000) == 0)
				{
					nwy_sdk_fsync(WriteFileFd);
					nwy_ext_echo("\r\ntf write wave data to file ok,write Cnt:%d ,WaveQueueFullFlag:%d", PrintCnt, WaveQueueFullFlag);
				}
				TotalBytesWritten += (WAVE_DATA_FRAME_SIZE*i);
				if (LastFrameSign == 1) //最后一帧数据
				{
					LastFrameSign = 0;
					StartSaveData = 0;
					nwy_ext_echo("\r\n last frame");
					nwy_sdk_fclose(WriteFileFd);

					nwy_ext_echo("\r\n Updating info file: %s, Count: %d", FirstFileNameOfOneCharge, FileCntOfOneCharge);
					UpdateInfoFile(0, FirstFileNameOfOneCharge, FileCntOfOneCharge,bLoop);
					WriteFileFd = -1;
					TotalBytesWritten = 0;
					FileCntOfOneCharge = 0;
					memset(FirstFileNameOfOneCharge, 0, sizeof(FirstFileNameOfOneCharge));
				}
				else
				{
					// 检查是否需要创建新文件，目前用不上，只需要一个文件
					if (TotalBytesWritten >= MAX_WAVE_DATA_FILE_SIZE)
					{
						nwy_sdk_fclose(WriteFileFd);
						WriteFileFd = -1;
						// 创建新文??
						WriteFileFd = CreateNextWaveFile(NameOfWriteFile,bLoop);
						if (WriteFileFd == -1)
						{
						// nwy_ext_echo("\r\n Create new wave file fail");
						return;
						}
						nwy_ext_echo("\r\n Create new wave file ok");
						FileCntOfOneCharge++;
					}
				}
			}
		}
	}
}

void CheckInfoFile(void)
{
	int fd = -1;
	memset(&infoFile, 0, sizeof(infoFile));

	if (nwy_sdk_fexist(INFO_FILE_PATH) == FALSE)
	{
		nwy_ext_echo("\r\n InfoFile not exist");
		fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_CREAT | NWY_WRONLY);
		if (fd == -1)
		{
			nwy_ext_echo("\r\n open InfoFile fail");
			return;
		}
		else
		{
			// infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - 2);
			nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
			nwy_sdk_fwrite(fd, &infoFile, sizeof(infoFile));
			nwy_sdk_fclose(fd);
			nwy_ext_echo("\r\n power on Create InfoFile success");
			print_infoFile((TInfoFileHead *)&infoFile);
		}
	}
}

//-----------------------------------------------
// 功能描述: TF卡任??
//
// 参数: param - 参数
//
// 返回??: ??
//
// 备注内???: ??
//-----------------------------------------------
void TF_Task(void *param)
{
	if (api_CheckaAndMountSDCard() == TRUE)
	{
		CheckInfoFile();
	}

	if (api_PowerOnCreatParaTable() == FALSE)
	{
		if (api_OperateFileSystem(READ, GET_SAFE_SPACE_ADDR(ReportPara.QueSpace), (BYTE*)&QueueSpace, sizeof(ReportPara.QueSpace)) == FALSE)
		{
			QueueSpace = 10;
		}
		
		if(api_OperateFileSystem(READ, GET_SAFE_SPACE_ADDR(ReportPara.WriteFileTime), (BYTE*)&WriteTFmaxtime, sizeof(ReportPara.WriteFileTime)) == FALSE)
		{
			WriteTFmaxtime = 10;
		}
	}
	while (1)
	{
		ThreadRunCnt[eTfThread]++;
		if (api_CheckaAndMountSDCard() == FALSE)
		{
			nwy_ext_echo("\r\n check or Mount TF Card fail");
			nwy_sleep(5000);
			continue;
		}
		else
		{
			if (nwy_adc_get(NWY_ADC_CHANNEL3, NWY_ADC_SCALE_5V000) > 1200)
			{
				CheckInfoFile();
				WriteFileTask();
				ReadFileTask();
			}
			else
			{
				if (PowerDownFlag == 0)
				{
					nwy_ext_echo("\r\n power down");
					api_WriteSysUNMsg(POWER_DOWN);
					PowerDownFlag = 0xAA;
				}
			}
		}
	}
}