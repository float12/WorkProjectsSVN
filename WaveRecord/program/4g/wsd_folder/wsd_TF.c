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
#define WriteOneFileFullTime    1//一直写写满一个文件的时长（分钟）
#define MAX_WAVE_DATA_FILE_SIZE     WAVE_DATA_FRAME_SIZE * 50 * WriteOneFileFullTime * 60 // 一个文件的最大大小
#define INFO_FILE_PATH "/sdcard0/Info.bin"                          // 保存读写文件名称的文件的路径
#define PATH_PREFIX "/sdcard0/"                                     // 保存读写文件名称的文件的路径前缀
#define MAX_FILE_NAME_LEN 30
#define MAX_CHARGE_SAVE_NUM 4 // 保存的最大充电次数为4
#define DATA_FILE_NAME_SUFFIX "-1p.bin"
#define TF_REQUEST_TIME 1000 * 60 // 请求上传间隔时间 ms
#define FRAME_TYPE_OFFSET 790     // 帧类型字节位置：从后往前偏移790个字节
#define CHARGE_START_FRAME_TYPE 0x01
#define CHARGE_MIDDLE_FRAME_TYPE 0x02
#define CHARGE_END_FRAME_TYPE 0x03

//-----------------------------------------------
// 文件使用的结构体、共用体、枚举
//-----------------------------------------------

typedef enum
{
    FirstFrame = 1,
    MiddleFrame,
    LastFrame
} FrameType; // 波形数据的帧类型

typedef enum
{
    STATE_CHECK_FILE_TO_READ = 0,       // 0
    STATE_REQUEST_UPLOAD_FILE,      // 1
    STATE_WAIT_REQUEST_RESULT,      // 2
    STATE_READ_DATA,                // 3
    STATE_SEND_TO_QUEUE,            // 4
    STATE_ONE_FILE_FINISH,          // 一个文件结束
    STATE_ONE_CHARGE_UPLOAD_FINISH, // 一次充电波形结束
} UploadFileState;

typedef struct
{
    BYTE FirstFileName[MAX_FILE_NAME_LEN];
    WORD CNT;
} FileInfoOfOneCharge; // 1次充电行为的起始文件名和文件数

typedef struct // 信息文件结构体
{
    BYTE WriteDataFileName[MAX_FILE_NAME_LEN];                  // 上一个波形数据文件的文件名
    FileInfoOfOneCharge FileInfoOfCharges[MAX_CHARGE_SAVE_NUM]; // 暂存待上传的充电行为，读文件名为全0表示无某次充电波形的数据文件，
                                                                // 否则发生了某次充电行为，读到结束帧后将该文件名置为全0表示已上传完成
    BYTE ReadOffset;                                            // 读偏移量
    BYTE WriteOffset;                                           // 写偏移量
    WORD crc16;
} TInfoFile;

typedef struct 
{
    BYTE isFinalFrame;
    TInfoFile infoFile;
    UploadFileState state;
    DWORD SendCnt;
    int ReadFileFd;
    BYTE ReadFileName[MAX_FILE_NAME_LEN];
    WORD WaitCnt;
    FileInfoOfOneCharge *pInfoOfOneCharge;
    BYTE isSuccess;
}TReadAndUploadFile;
//-----------------------------------------------
// 全局使用的变量、常量
//-----------------------------------------------

//-----------------------------------------------
// 文件使用的变量、常量
//-----------------------------------------------
static BYTE RequestTimeUpFlag = 1;
static WaveOrRequestData DataToTcp;
static WaveOrRequestData DataToTF;
static DWORD TotalBytesWritten = 0;                 // 一个文件已写入的总字节数

//-----------------------------------------------
// 内部函数声明
//-----------------------------------------------

//-----------------------------------------------
// 函数定义
//-----------------------------------------------
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
void UpdateInfoFile(BYTE type, BYTE *FirstFileName, WORD cnt)
{
    int fd = -1;
    TInfoFile infoFile;
    WORD crc16;
    memset(&infoFile, 0, sizeof(infoFile));

    fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_CREAT | NWY_RDWR);
    if (fd == -1)
    {
        nwy_ext_echo("\r\n open InfoFile fail");
        return;
    }
    nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
    nwy_sdk_fread(fd, &infoFile, sizeof(infoFile));
    //   nwy_ext_echo("\r\n read:");
    // print_infoFile(&infoFile);
    if (CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD)) != infoFile.crc16)
    {
        nwy_ext_echo("\r\n read InfoFile, crc error");
        return;
    }
    if (type == 0)
    {
        memcpy(infoFile.FileInfoOfCharges[infoFile.WriteOffset].FirstFileName, FirstFileName, MAX_FILE_NAME_LEN);
        infoFile.FileInfoOfCharges[infoFile.WriteOffset].CNT = cnt;
        infoFile.WriteOffset = (infoFile.WriteOffset + 1) % MAX_CHARGE_SAVE_NUM;
    }
    else
    {
        memset(&infoFile.FileInfoOfCharges[infoFile.ReadOffset], 0, sizeof(FileInfoOfOneCharge));
        infoFile.ReadOffset = (infoFile.ReadOffset + 1) % MAX_CHARGE_SAVE_NUM;
    }
    infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD));
    nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
    nwy_sdk_fwrite(fd, &infoFile, sizeof(infoFile));
    nwy_sdk_fclose(fd);
    nwy_ext_echo("\r\n after Update,InfoFile:");
    print_infoFile(&infoFile);
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
BYTE CheckaAndMountSDCard(void)
{
    if (nwy_read_sdcart_status() == FALSE) // 卸载状态
    {
        // nwy_ext_echo("\r\n sdcard is not mounted");
        if (nwy_sdk_sdcard_mount() == FALSE)
        {
            // nwy_ext_echo("\r\n nwy_sdk_sdcard_mount fail");
            return FALSE;
        }
    }
    // nwy_ext_echo("\r\n sdcard is mounted");
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
BYTE Check698Format(BYTE *data, DWORD length)
{
    // for (size_t i = 0; i < length; i++)
    // {
    //     nwy_ext_echo("%02X ", data[i]);
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
        return FirstFrame;
    }
    else if (data[25] == 0x02)
    {
        return MiddleFrame;
    }
    else if (data[25] == 0x03)
    {
        return LastFrame;
    }
    else
    {
        nwy_ext_echo("\r\n 698 frame type err");
        return FALSE;
    }
}
void TF_Request_timer_cb(void *param)
{
    RequestTimeUpFlag = 1;
}

void print_infoFile(const TInfoFile *infoFile)
{
    // 打印写入数据文件�?
    nwy_ext_echo("\r\nWriteDataFileName: %s\n", infoFile->WriteDataFileName);

    // 打印每�?�充电的文件信息
    for (int i = 0; i < MAX_CHARGE_SAVE_NUM; i++)
    {
        nwy_ext_echo("FileInfoOfCharges[%d].FirstFileName: %s\n",
                     i, infoFile->FileInfoOfCharges[i].FirstFileName);
        nwy_ext_echo("FileInfoOfCharges[%d].CNT: %d\n",
                     i, infoFile->FileInfoOfCharges[i].CNT);
    }

    // 打印读写偏移量和 crc16 校验�?
    nwy_ext_echo("ReadOffset: %d\n", infoFile->ReadOffset);
    nwy_ext_echo("WriteOffset: %d\n", infoFile->WriteOffset);
    nwy_ext_echo("crc16: %04x\n", infoFile->crc16);
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
    BYTE CompletePath[MAX_FILE_NAME_LEN] = "/sdcard0/";  
    WORD i = 0;
    WORD FileNo = 0;
    DWORD lastReadNumber = 0;
    BYTE TmpFileName[MAX_FILE_NAME_LEN] = {0};
    BYTE AllZero[sizeof(FileInfoOfOneCharge)] = {0};
    static TReadAndUploadFile ReadAndUploadFile = {0};

    // 检查是否有存满的文?
    if ((tcp_connect_flag == 1))
    {
        // nwy_ext_echo("\r\n tf task state:%d", ReadAndUploadFile.state);
        switch (ReadAndUploadFile.state)
        {
        case STATE_CHECK_FILE_TO_READ:
            if (nwy_sdk_fexist(INFO_FILE_PATH) == TRUE)
            {
                fd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDONLY);
                if (fd == -1)
                {
                    nwy_ext_echo("\r\n check file to read ,Open InfoFile fail");
                    break;
                }
                nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
                nwy_sdk_fread(fd, &ReadAndUploadFile.infoFile, sizeof(ReadAndUploadFile.infoFile));
                // nwy_ext_echo("\r\n check file to read, InfoFile:\r\n");
                // print_infoFile(&ReadAndUploadFile.infoFile);
                if (CalCRC16(&ReadAndUploadFile.infoFile, sizeof(ReadAndUploadFile.infoFile) - 2) != ReadAndUploadFile.infoFile.crc16)
                {
                    nwy_ext_echo("\r\n read InfoFile, crc error");
                    nwy_sdk_fclose(fd);
                    nwy_sdk_file_unlink(INFO_FILE_PATH);
                    return;
                }
                nwy_sdk_fclose(fd);
                ReadAndUploadFile.pInfoOfOneCharge = &ReadAndUploadFile.infoFile.FileInfoOfCharges[ReadAndUploadFile.infoFile.ReadOffset];
                if (memcmp(ReadAndUploadFile.pInfoOfOneCharge, AllZero, sizeof(FileInfoOfOneCharge)) != 0) //
                {
                    nwy_ext_echo("\r\n read InfoFile, has data to upload");
                    strcpy(ReadAndUploadFile.ReadFileName, "/sdcard0/");
                    strcat(ReadAndUploadFile.ReadFileName,ReadAndUploadFile.pInfoOfOneCharge->FirstFileName);
                    ReadAndUploadFile.ReadFileFd = nwy_sdk_fopen(ReadAndUploadFile.ReadFileName, NWY_RDONLY);
                    if (ReadAndUploadFile.ReadFileFd != -1)
                    {
                        nwy_ext_echo("\r\n Open read  File fd success,read fd:%d", ReadAndUploadFile.ReadFileFd);
                        ReadAndUploadFile.state = STATE_REQUEST_UPLOAD_FILE;
                    }
                    else
                    {
                        nwy_ext_echo("\r\n Open read  File fd fail");
                        ReadAndUploadFile.state = STATE_CHECK_FILE_TO_READ; // 文件打开失败，直接退?
                    }
                }
                break;
            }
        case STATE_REQUEST_UPLOAD_FILE:
            if (RequestTimeUpFlag == 1)
            {
                DataToTcp.type = 1; //请求上传文件
                if (nwy_put_msg_que(WaveDataUploadMessageQueue, &DataToTcp, 100) == TRUE)
                {
                    nwy_ext_echo("\r\n send request ok");
                    ReadAndUploadFile.state = STATE_WAIT_REQUEST_RESULT;
                    RequestTimeUpFlag = 0;
                }
                else
                {
                    nwy_ext_echo("\r\n send request fail");
                }
            }
            break;
        case STATE_WAIT_REQUEST_RESULT:
            if (nwy_get_msg_que(TCP_RESULT_TO_TF_MessageQueue, &result, 0xffffffff) == TRUE) //立即返回
            {
                if (result == FALSE)
                {
                    nwy_ext_echo("\r\n request result fail");
                    nwy_start_timer(TF_REQUEST_TIMER, TF_REQUEST_TIME);
                    ReadAndUploadFile.state = STATE_REQUEST_UPLOAD_FILE;
                }
                else if (result == TRUE)
                {
                    nwy_ext_echo("\r\n request result ok");
                    ReadAndUploadFile.state = STATE_READ_DATA; // 发送成功，直接退?
                }
            }
            else
            {
                ReadAndUploadFile.WaitCnt++;
                if (ReadAndUploadFile.WaitCnt > 10) //200ms*10=2s
                {
                    nwy_ext_echo("\r\n request result timeout");
                    ReadAndUploadFile.state = STATE_ONE_CHARGE_UPLOAD_FINISH;
                }
            }
            break;
        case STATE_READ_DATA:
            ReadBytes = nwy_sdk_fread(ReadAndUploadFile.ReadFileFd, &DataToTcp, WAVE_DATA_LEN);
            if (ReadBytes == WAVE_DATA_LEN)
            {
                // nwy_ext_echo("\r\n Read File ok");
                // nwy_ext_echo("\r\n790,frame type:%02X ", DataToTcp.buf2[WAVE_DATA_LEN - 4096 - FRAME_TYPE_OFFSET]);
                if (DataToTcp.buf[WAVE_DATA_LEN - FRAME_TYPE_OFFSET] == CHARGE_END_FRAME_TYPE) //一次充电?取结束
                {
                    nwy_ext_echo("\r\n charge end frame,one charge upload finish");
                    ReadAndUploadFile.isFinalFrame = 1;
                }
                ReadAndUploadFile.state = STATE_SEND_TO_QUEUE;
            }
            else if (ReadBytes == 0) //读取文件结束
            {
                nwy_ext_echo("\r\n Read one File end,read next file");
                nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
                ReadAndUploadFile.ReadFileFd = -1;
                nwy_ext_echo("\r\nlast ReadFileName:%s", ReadAndUploadFile.ReadFileName);
                sscanf(ReadAndUploadFile.ReadFileName, "/sdcard0/%d-", &lastReadNumber);
                lastReadNumber++;
                memset(ReadAndUploadFile.ReadFileName, 0, sizeof(ReadAndUploadFile.ReadFileName));
                sprintf(ReadAndUploadFile.ReadFileName, "/sdcard0/%d-1p.bin", lastReadNumber);
                nwy_ext_echo("\r\nnew ReadFileName:%s", ReadAndUploadFile.ReadFileName);
                nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
                ReadAndUploadFile.ReadFileFd = nwy_sdk_fopen(ReadAndUploadFile.ReadFileName, NWY_RDONLY);
                if (ReadAndUploadFile.ReadFileFd == -1)
                {
                    nwy_ext_echo("\r\n Open read  File fd fail");
                    ReadAndUploadFile.state = STATE_ONE_CHARGE_UPLOAD_FINISH;
                }
                else
                {
                    ReadAndUploadFile.state = STATE_READ_DATA;
                }
            }
            else
            {
                nwy_ext_echo("\r\n Read File fail,ReadBytes=%d,ReadFileFd:%d", ReadBytes, ReadAndUploadFile.ReadFileFd);
                ReadAndUploadFile.state = STATE_ONE_CHARGE_UPLOAD_FINISH;
            }
            break;
        case STATE_SEND_TO_QUEUE:
            if (nwy_get_queue_spaceevent_cnt(WaveDataUploadMessageQueue) != 0)
            {
                DataToTcp.type = 0;
                if (nwy_put_msg_que(WaveDataUploadMessageQueue, &DataToTcp, 100) == TRUE)
                {
                    ReadAndUploadFile.SendCnt++;
                    if (ReadAndUploadFile.SendCnt % 100 == 0)
                    {
                        nwy_ext_echo("\r\ntf send wave data to tcp ok,SendCnt:%d", ReadAndUploadFile.SendCnt);
                    }
                    if (ReadAndUploadFile.isFinalFrame == 1)
                    {
                        nwy_ext_echo("\r\n one charge upload finish");
                        ReadAndUploadFile.isFinalFrame = 0;
                        ReadAndUploadFile.isSuccess = 1;
                        ReadAndUploadFile.state = STATE_ONE_CHARGE_UPLOAD_FINISH;
                    }
                    else
                    {
                        ReadAndUploadFile.state = STATE_READ_DATA;
                    }
                }
                else
                {
                    nwy_ext_echo("\r\n send data to queue fail,try again");
                }
            }
            else
            {
                nwy_ext_echo("\r\ndata queue is full,wait");
            }
            break;
        case STATE_ONE_CHARGE_UPLOAD_FINISH: //成功或失败后初始化变量.
            if (ReadAndUploadFile.isSuccess == 1)
            {
                UpdateInfoFile(1, NULL, 0);
            }
            nwy_sdk_fclose(ReadAndUploadFile.ReadFileFd);
            memset(&ReadAndUploadFile, 0, sizeof(ReadAndUploadFile));
            ReadAndUploadFile.ReadFileFd = -1;
            RequestTimeUpFlag = 1;
            break;
        }
    }
}

//--------------------------------------------------
// 功能描述:
//
// 参数:
//
//
//
// 返回�?:
//
// 备注内�??:
//--------------------------------------------------
void WriteFileTask(void)
{
    BYTE result = 0;
    BYTE i = 0;
    BYTE LastFrameSign = 0;
    static int WriteFileFd = -1;
    static WORD FileCntOfOneCharge = 0;
    static BYTE FirstFileNameOfOneCharge[MAX_FILE_NAME_LEN];
    static DWORD PrintCnt = 0;
    BYTE NameOfWriteFile[MAX_FILE_NAME_LEN] = {0}; // 文件名
    long WriteBytes = 0;
    if (nwy_get_msg_que(WaveDataToTFMessageQueue, &DataToTF, 100) == TRUE)
    { //判断格式
        // nwy_ext_echo("\r\n write file task,get wave data from queue ok");
        for (i = 0; i < WAVE_CYCLE_CNT; i++)
        {
            result = Check698Format(&DataToTF.buf[i * WAVE_DATA_FRAME_SIZE], WAVE_DATA_FRAME_SIZE);
            if (result == FALSE)
            {
                nwy_ext_echo("\r\n 698 frame format err");
                return;
            }
            else if (result == FirstFrame) //写入该帧数据后创建新文件
            {
                nwy_ext_echo("\r\n get first frame");
                FileCntOfOneCharge = 0;
            }
            else if (result == MiddleFrame) // 中间帧数据
            {
                // nwy_ext_echo("\r\n get middle frame");
            }
            else if (result == LastFrame) // 最后一??数据
            {
                nwy_ext_echo("\r\n get last frame");
                LastFrameSign = 1;
            }
        }
        // 写入数据到文件
        if (WriteFileFd == -1)
        {
            WriteFileFd = CreateNextWaveFile(NameOfWriteFile);
            if (WriteFileFd < 0)
            {
                nwy_ext_echo("\r\n Create new wave file fail");
                return;
            }
            FileCntOfOneCharge++;
            nwy_ext_echo("\r\n new file name:%s,FileCntOfOneCharge:%d", NameOfWriteFile, FileCntOfOneCharge);
            if (FileCntOfOneCharge == 1) //保存一次充电的�?一�?文件�?
            {
                memcpy(FirstFileNameOfOneCharge, NameOfWriteFile, sizeof(FirstFileNameOfOneCharge));
            }
        }
        WriteBytes = nwy_sdk_fwrite(WriteFileFd, DataToTF.buf, WAVE_DATA_LEN);
        if (WriteBytes != WAVE_DATA_LEN)
        {
            nwy_ext_echo("\r\n Write full data len fail,WriteBytes:%d", WriteBytes);
            nwy_sdk_fclose(WriteFileFd);
            WriteFileFd = -1;
            return;
        }
        else
        {
            PrintCnt++;
            if (PrintCnt % 100 == 0)
            {
                nwy_sdk_fsync(WriteFileFd);
                nwy_ext_echo("\r\ntf write wave data to file ok,write Cnt:%d ", PrintCnt);
            }
            TotalBytesWritten += WAVE_DATA_LEN;
            if (LastFrameSign == 1)//最后一帧数据
            {
                LastFrameSign = 0;
                nwy_ext_echo("\r\n last frame,one charge upload finish");
                nwy_sdk_fclose(WriteFileFd);
                
                nwy_ext_echo("\r\n Updating info file: %s, Count: %d", FirstFileNameOfOneCharge, FileCntOfOneCharge);
                UpdateInfoFile(0, FirstFileNameOfOneCharge, FileCntOfOneCharge);
                WriteFileFd = -1;
                TotalBytesWritten = 0;
                FileCntOfOneCharge = 0;
                memset(FirstFileNameOfOneCharge, 0, sizeof(FirstFileNameOfOneCharge));
            }
            else
            {
                // 检查是否需要创建新文件
                if (TotalBytesWritten >= MAX_WAVE_DATA_FILE_SIZE)
                {
                    nwy_sdk_fclose(WriteFileFd);
                    WriteFileFd = -1;
                    // 创建新文�?
                    WriteFileFd = CreateNextWaveFile(NameOfWriteFile);
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
void WriteInfoFileTest(void)
{
    TInfoFile infoFile;
    memset(&infoFile, 0, sizeof(infoFile));
}

void PowerOnCreateInfoFile(void)
{
    TInfoFile infoFile;
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
            infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - 2);
            nwy_sdk_fseek(fd, 0, NWY_SEEK_SET);
            nwy_sdk_fwrite(fd, &infoFile, sizeof(infoFile));
            nwy_sdk_fclose(fd);
            nwy_ext_echo("\r\n power on Create InfoFile success");
            print_infoFile(&infoFile);
        }
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
int CreateNextWaveFile(BYTE *NameOfFile)
{
    // 尝试打开保存文件名的文件
    int infoFd = -1;
    int fd = -1;
    BYTE AllZero[MAX_FILE_NAME_LEN] = {0};
    DWORD lastWriteNumber;
    DWORD lastReadNumber;
    TInfoFile infoFile;
    BYTE tmp[MAX_FILE_NAME_LEN] = "/sdcard0/";

    memset(&infoFile,0,sizeof(infoFile));
    infoFd = nwy_sdk_fopen(INFO_FILE_PATH, NWY_RDWR);
    if (infoFd < 0)
    {
        nwy_ext_echo("\r\ncreate next wave file open InfoFile fail");
        return -1;
    }
    // 读取上一次的文件名
    nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);
    nwy_sdk_fread(infoFd, &infoFile, sizeof(infoFile));
    // nwy_ext_echo("\r\n before create");
    // print_infoFile(&infoFile);
    if (strncmp(infoFile.WriteDataFileName, AllZero,sizeof(infoFile.WriteDataFileName)) == 0)
    {
        // 全0表示这是第一个波形文件
        memcpy(infoFile.WriteDataFileName, "1-1p.bin",sizeof(infoFile.WriteDataFileName));
        memset(&infoFile.WriteDataFileName[8], 0, sizeof(infoFile.WriteDataFileName)-8);
        infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD));
        // nwy_ext_echo("\r\n after create");
        // print_infoFile(&infoFile);
        // 创建并保存文件名
        nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);
        int WriteBytes = nwy_sdk_fwrite(infoFd, &infoFile,sizeof(infoFile));
        nwy_ext_echo("\r\n file name :%s",infoFile.WriteDataFileName);
        nwy_ext_echo("\r\n WriteBytes:%d", WriteBytes);
        nwy_sdk_fclose(infoFd);
    }
    else
    {
        // 解析文件编号并加1
        if (sscanf(infoFile.WriteDataFileName, "%d-", &lastWriteNumber) == 1)
        {
            lastWriteNumber++;
            sprintf(infoFile.WriteDataFileName, "%d-1p.bin", lastWriteNumber);
            nwy_ext_echo("\r\nnew file name:%s", infoFile.WriteDataFileName);
            infoFile.crc16 = CalCRC16(&infoFile, sizeof(infoFile) - sizeof(WORD));
            // 保存新文件名
            nwy_sdk_fseek(infoFd, 0, NWY_SEEK_SET);
            nwy_sdk_fwrite(infoFd, &infoFile, sizeof(infoFile));
            nwy_sdk_fclose(infoFd);
        }
        else
        {
            nwy_sdk_fclose(infoFd);
            nwy_ext_echo("\r\n sscanf No error");
        }
    }
    strcat(tmp,infoFile.WriteDataFileName);
    nwy_ext_echo("\r\n data file name:%s",tmp);
    // 打开新文件
    fd = nwy_sdk_fopen(tmp, NWY_CREAT | NWY_RDWR);
    if (fd >= 0)
    {
        // 重置已写入的总字节数
        TotalBytesWritten = 0;
        memcpy(NameOfFile, infoFile.WriteDataFileName, sizeof(infoFile.WriteDataFileName));
        return fd;
    }
    else
    {
        nwy_ext_echo("\r\n Create new wave file fail");
        return -1;
    }
}
//-----------------------------------------------
// 功能描述: TF卡任�?
//
// 参数: param - 参数
//
// 返回�?: �?
//
// 备注内�??: �?
//-----------------------------------------------
BYTE TFready = 0;
void TF_Task(void *param)
{

    WORD WriteLen = 0;
    BYTE ReadFlag = 0;
    DWORD lastReadNumber = 0;
    BYTE i = 0;
    if (CheckaAndMountSDCard() == TRUE)
    {
        PowerOnCreateInfoFile();
    }
    TFready = 1;
    while (1)
    {
        nwy_sleep(10);
        if (CheckaAndMountSDCard() == FALSE)
        {
            nwy_ext_echo("\r\n check or Mount TF Card fail");
            nwy_sleep(5000);
            continue;
        }
        else
        {
            WriteFileTask();
            i++;
            if(i == 10)//300ms左右发送一次波形数据 等待TCP发送
            {
                i = 0;
                ReadFileTask();
            }
            
        }
    }
}