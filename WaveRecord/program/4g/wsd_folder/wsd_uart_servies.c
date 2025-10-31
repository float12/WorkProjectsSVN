//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
//创建人	王泉
//创建日期
//描述
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define BAUDRATE 115200       //波特率
#define BYTE_DANCE_TIMEOUT 50 //字节间超时 ms		115200 间隔 默认50ms
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
int UART_HD = 0;
int IntervalTime = 15; //采集间隔时间

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
nwy_time_t poweronTimer;
WaveDataReceiver gWaveReceiver = {0}; // 全局接收状态
BYTE UartRecTimeout = 0;              //接收超时标志
static WORD SysTickCounter;
BYTE gTranType = 0;
TRealTimer tTimer = {0}; //防止 在58秒处开启标志，下一次进行后分钟数 不对，将time设为全局变量 开始抄读的时间作为上报时间？？？

tTranData Transdata;
BYTE PowerOnReadMeter = 0; //上电抄表成功标志
uint64_t dwCurTicks = 0, dwCurTicksHis = 0;
//-----------------------------------------------
//				函数定义
//-----------------------------------------------
extern int nwy_set_rx_frame_timeout(uint8_t hd, int time);
extern int nwy_data_get_flowcalc_info(nwy_data_flowcalc_info_t *flowcalc_info);
//初始化规约读写指针,初始化、接收超时、规约处理完三种情况调用，
void InitPoint(TSerial *p)
{
    p->ProStepDlt645 = 0;
    p->BeginPosDlt645 = 0;
    p->Dlt645DataLen = 0;
    p->RXWPoint = 0;
    p->RXRPoint = 0;
    p->RxOverCount = 0;
    // 698初始化
    p->ProStepDlt698_45 = 0;
    // DLT698_45规约报文在Serial[].ProBuf中的开始位置
    p->BeginPosDlt698_45 = 0;
    p->wLen = 0;
    p->Addr_Len = 6; // 通信报文中服务器地址SA长度，地址长度默认6字节
}

void nwy_meter_upgrade_timer_cb(void *type)
{
    MeterUpgradeTimeUpFlag = 1;
}
void nwy_general_timer_cb(void *type)
{
    OSI_LOGI(0, "nwy_app_timer_cb");
    SysTickCounter++;
    if (SysTickCounter >= 3600)
    {
        SysTickCounter = 0;
    }
    if ((SysTickCounter % 3600) == 0)
    {
        api_SetAllTaskFlagRam(eFLAG_HOUR); //小时任务
    }
    if ((SysTickCounter % 60) == 0)
    {
        api_SetAllTaskFlagRam(eFLAG_MINUTE); //分钟任务
    }
    if ((SysTickCounter % 30) == 0)
    {
        api_SetAllTaskFlagRam(eFLAG_HALF_MINUTE); //半分钟任务
    }
    api_SetAllTaskFlagRam(eFLAG_SECOND); //秒任务
    nwy_gpio_set_value(RUN_LIGHT, (SysTickCounter % 2));
    if (nwy_get_queue_spaceevent_cnt(TimerMessageQueue) != 0)
    {
        nwy_put_msg_que(TimerMessageQueue, &FlagBytesRam, 0xffffffff);
        api_ClrAllTaskFlagRam();
    }
    dwCurTicksHis = dwCurTicks;
}
BYTE api_CalRXD_CheckSum(WORD ProtocolType, TSerial *p)
{
    WORD i;
    BYTE bySum = 0;

    WORD ProStep, BeginPos;
    //国标645规约
    if (ProtocolType == 0)
    {
        ProStep = p->ProStepDlt645;
        BeginPos = p->BeginPosDlt645;
    }
    else
    {
        return 0;
    }
    if (ProStep <= 0)
    {
        return 0;
    }

    for (i = 0; i < (ProStep - 1); i++)
    {
        bySum += p->ProBuf[(BeginPos + i) % UART_BUFF_MAX_LEN];
    }
    return bySum;
}

//--------------------------------------------------
//功能描述:  遍历抄表bit位
//
//参数:      bLoop  当前回路数
//
//返回值:
//
//备注:  遍历标志位，有标志则占用串口进行发送
//--------------------------------------------------
BOOL TraverseMeterReadingSigns(BYTE bLoop, BYTE *bAddr)
{
    BYTE i;

    // nwy_ext_echo("\r\nsqReadMeterFlag IS %lx",&dwReadMeterFlag[0]);
    for (i = 0; i < MAX_READMETER; i++)
    {
        if (dwReadMeterFlag[bLoop] & (0x0001 << i))
        {
            // nwy_ext_echo("\r\nFlag bit is%d",i);
            // nwy_ext_echo("\r\nbHisReadMeter[bLoop] is%d",bHisReadMeter[bLoop]);
            if ((bHisReadMeter[bLoop] == i))
            {
                if (bReadMeterRetry >= 3)
                {
                    dwReadMeterFlag[bLoop] &= ~(1 << i);
                    bReadMeterRetry = 0;
                    //重试三次失败 如果是服务端参数从sys回复
                    if ((i >= (CycleReadMeterNum + eBIT_TCPNET)) && (i <= (CycleReadMeterNum + eBIT_MQTTUSER)))
                    {
                        api_ReadSystemFiletoRam(i - CycleReadMeterNum);
                        //从文件系统恢复代表表内数据异常，闪灯提示？
                    }
                    return FALSE;
                }
                bReadMeterRetry++;
            }
            else
            {
                bHisReadMeter[bLoop] = i;
            }
            api_ReadMeterTask645(i, bAddr);
            return FALSE;
        }
    }
    return TRUE;
}
//--------------------------------------------------
//功能描述: 多回路采集任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void CollectMeterTask(void)
{
    BYTE i;
    BYTE bTempAddr[6] = {0};

    if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE)
    {
        memcpy((BYTE *)&bTempAddr[0], (BYTE *)&bAddr[0], 6);
        for (i = 0; i < MAX_SAMPLE_CHIP; i++)
        {
            bTempAddr[5] += i;
            if (TraverseMeterReadingSigns(i, bTempAddr) != TRUE)
            {
                break;
            }
        }
    }
}
#if (CYCLE_REPORT == YES)
//--------------------------------------------------
//功能描述:  周期性与mqtt任务交互，目前是60分钟交互一次
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void ToMqttByCycle(void)
{
    BYTE bTime, i, j;

    get_N176_time(&tTimer);
    bTime = tTimer.Min % IntervalTime; //此函数目前只支持 2分钟  ，其他版本未合并！！！
    if (bTime == 0)
    {
        if (api_GetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD) == TRUE)
        {
            for (i = 0; i < MAX_SAMPLE_CHIP; i++)
            {
                for (j = 0; j < CycleReadMeterNum; j++)
                {
                    dwReadMeterFlag[i] |= (1 << j);
                }
            }
            api_ClrSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
            nwy_ext_echo("\r\ntime up");
        }
    }
    else
    {
        api_SetSysStatus(eSYS_STASUS_SAMPLE_UPLOAD);
    }
}
#endif
//--------------------------------------------------
//功能描述:
//
//参数:
//
//返回值:
//
//备注:  一次最大接收4096字节，波形数据长度变化需要调整
//--------------------------------------------------

void nwy_uart_recv_handle(const char *str, uint32_t length)
{
    WORD wLen, wLen1;
    WORD i;
    static int Handlecnt = 300;
    static BYTE errflag = 0;
    // nwy_ext_echo("\r\nenter uart,len:%d\r\n",length);

    if (api_GetSysStatus(eSYS_STASUS_START_TRAN))
    {
        memset(&Transdata, 0, sizeof(Transdata));
        if (length > sizeof(Transdata.buf))
        {
            memcpy(Transdata.buf, str, sizeof(Transdata.buf));
            Transdata.len = sizeof(Transdata.buf);
        }
        else
        {
            memcpy(Transdata.buf, str, length);
            Transdata.len = length;
        }
        if (gTranType == BLUETOOTH_TRAN)
        {
            if (nwy_put_msg_que(TranDataToFactoryMessageQueue, &Transdata, 0xffffffff) == TRUE)
            {
                nwy_ext_echo("\r\nuart to factory ok,length");
            }
            else
            {
                nwy_ext_echo("\r\nuart to factory err");
            }
        }
        else if (gTranType == SERVER_TRAN)
        {
            if (nwy_put_msg_que(TranDataToServerMessageQueue, &Transdata, 0xffffffff))
            {
                nwy_ext_echo("\r\nuart to mqtt/tcp queue ok,length:%d", length);
            }
            else
            {
                nwy_ext_echo("\r\nto mqtt queue err");
            }
        }
        gTranType = 0;
        nwy_stop_timer(uart_timer);
        api_ClrSysStatus(eSYS_STASUS_START_TRAN); // 亮灯 远程
    }
    else
    {
        wLen = Serial.RXWPoint + length;
        if (length >= UART_BUFF_MAX_LEN)
        {
            length = UART_BUFF_MAX_LEN;
        }
        if (wLen < UART_BUFF_MAX_LEN)
        {
            memcpy((char *)&Serial.ProBuf[Serial.RXWPoint], str, length);
            Serial.RXWPoint += length;
        }
        else
        {
            wLen1 = UART_BUFF_MAX_LEN - Serial.RXWPoint;
            memcpy((char *)&Serial.ProBuf[Serial.RXWPoint], str, wLen1);
            memcpy((char *)&Serial.ProBuf[0], str + wLen1, length - wLen1);
            Serial.RXWPoint = length - wLen1;
        }
    }
    // nwy_ext_echo("\r\n %s",Serial.ProBuf[0]);
}

//--------------------------------------------------
// 功能描述:  发送透传数据
//
// 参数:
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void SendTranData(BYTE *buf, int len)
{
    int RetLen = 0;

    RetLen = nwy_uart_send_data(UART_HD, buf, len);
    nwy_ext_echo("uart send len is \r\n %d", RetLen);
    nwy_stop_timer(uart_timer);
    nwy_start_timer(uart_timer, 10000); //10s定时
}
//--------------------------------------------------
//功能描述:  串口初始化
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void Init_UartPara_DL645(void)
{
    BYTE j;

    int hd = nwy_uart_init(NWY_NAME_UART1, 1); // 初始化
    nwy_ext_echo("\r\ninit uart1 hd:%d", hd);
    nwy_uart_set_baud(hd, BAUDRATE);           // 设置默认波特率
    nwy_uart_set_para(hd, NWY_UART_EVEN_PARITY, NWY_UART_DATA_BITS_8, NWY_UART_STOP_BITS_1, 0);
    nwy_set_rx_frame_timeout(hd, BYTE_DANCE_TIMEOUT);
    InitPoint(&Serial);
    memset((char *)&Serial.ProBuf[0], 0, UART_BUFF_MAX_LEN);
    memset(bHisReadMeter, 0xFF, sizeof(bHisReadMeter));
    UART_HD = hd;
    nwy_uart_reg_recv_cb(hd, nwy_uart_recv_handle);
    for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //置上电要抄读的bit
    {
        dwReadMeterFlag[0] |= (1 << j);
    }
    if ((eBIT_TOTAL + CycleReadMeterNum) > (sizeof(DWORD) * 8)) //标志不能超过DWORD*8
    {
        for (;;)
        {
            nwy_sleep(1000);
            nwy_ext_echo("error ,Bit overrun!!!");
        }
    }
}
//--------------------------------------------------
//功能描述:  把数据从报文开始位置 向 以p->ProBuf[0]开始处移
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_DoReceMoveData(TSerial *p, BYTE ProtocolType)
{
    WORD ProStep, BeginPos;
    WORD i;
    BYTE Buf[UART_BUFF_MAX_LEN + 30];

    if (ProtocolType == PROTOCOL_645)
    {
        ProStep = p->ProStepDlt645;
        BeginPos = p->BeginPosDlt645;
    }
    else if (ProtocolType == PROTOCOL_698)
    {
        ProStep = p->ProStepDlt698_45;
        BeginPos = p->BeginPosDlt698_45;
    }
    else
    {
        return;
    }

    if (BeginPos == 0)
    {
        return;
    }
    if (ProStep >= UART_BUFF_MAX_LEN)
    {
        return;
    }
    if (BeginPos <= (UART_BUFF_MAX_LEN - ProStep))
    {
        for (i = 0; i < ProStep; i++)
        {
            p->ProBuf[i] = p->ProBuf[BeginPos + i];
        }
    }
    else
    {
        for (i = 0; i < ProStep; i++)
        {
            Buf[i] = p->ProBuf[(BeginPos + i) % UART_BUFF_MAX_LEN];
        }
        memcpy((void *)p->ProBuf, (void *)Buf, ProStep);
    }
}
//--------------------------------------------------
//功能描述:  数据处理任务
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void api_ReceData_UartTask(void)
{
    BYTE Result = FALSE;

    while (Serial.RXWPoint != Serial.RXRPoint)
    {
        nwy_start_timer(Uart_Recv_timer, 500);
        Result = DoReceProc_Dlt645_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
        if (Result == FALSE)
        {
            Result = DoReceProc_Dlt698_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
        }
#if (READMETER_PROTOCOL == PROTOCOL_MODBUS)
        Result = DoReceProc_Modbus_UART(Serial.ProBuf[Serial.RXRPoint], &Serial);
#endif
        if (Result == FALSE)
        {
            Serial.RXRPoint++;
            if (Serial.RXRPoint >= UART_BUFF_MAX_LEN)
            {
                Serial.RXRPoint = 0;
            }
        }
        else //接受到完整报文
        {
            nwy_stop_timer(Uart_Recv_timer);
            break;
        }
    }

    if (UartRecTimeout == 1)
    {
        UartRecTimeout = 0;
        InitPoint(&Serial);
    }
}
//--------------------------------------------------
//功能描述:  uart接收字节间超时定时器
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void Uart_Recv_timer_cb(void *type)
{
    UartRecTimeout = 1;
}
//--------------------------------------------------
//功能描述:  上电检测检测抄表状态
//
//参数:
//
//返回值:
//
//备注:
//--------------------------------------------------
void DetectReadMeterStasus(void)
{
    BYTE j;

    for (j = CycleReadMeterNum; j < (CycleReadMeterNum + POWERON_READRPARA_NUM); j++) //置上电要抄读的bit
    {
        if ((dwReadMeterFlag[0] & (1 << j)) == (1 << j))
        {
            return;
        }
    }
    PowerOnReadMeter = 1;
}
//--------------------------------------------------
// 函数功能: 扩展规约DBDF 与表建立一次通信
//
// 参数:		Type[int],  操作码，
//
//          ID[in],	数据标识
//
//			bDataLen[in],设置参数的长度
//
//          pBuf[in],要设置的数据
// 返回值: 	TRUE/FALSE
//
// 备注:     0x11,0x14
//-----------------------------------------------
BOOL api_CommWithMeter_DBDF(BYTE Type, BYTE bDataLen, BYTE *ID, BYTE *pBuf) // 读取和设置OK
{
    BYTE i;
    WORD wLen = 12;
    BYTE Buf[128];
    // 组同步帧
    memset((BYTE *)&Buf, 0x00, sizeof(Buf));

    // 组前导帧
    memset((BYTE *)&Buf, 0xFE, 4);

    Buf[4] = 0x68;
    memcpy(&Buf[5], reverAddr, 6);
    Buf[5 + 6] = 0x68;
    Buf[wLen++] = Type;

    if (Type == 0x11) // 扩展规约抄读
    {
        Buf[wLen++] = 8;
        Buf[wLen++] = 0x0E;
        Buf[wLen++] = 0x12;
        Buf[wLen++] = 0x12;
        Buf[wLen++] = 0x37;
        Buf[wLen++] = ID[0] + 0x33;
        Buf[wLen++] = ID[1] + 0x33;
        Buf[wLen++] = 0x12; // df
        Buf[wLen++] = 0x0E; // db
    }
    else if (Type == 0x14) // 扩展规约写
    {
        Buf[wLen++] = bDataLen + 20; // 长度可设置
        Buf[wLen++] = 0x0E;
        Buf[wLen++] = 0x12;
        Buf[wLen++] = 0x12;
        Buf[wLen++] = 0x37;
        Buf[wLen++] = 0x35;
        memset((BYTE *)&Buf[wLen], 0x33, 7);
        Buf[wLen + 7] = ID[0] + 0x33;
        wLen = wLen + 8;
        Buf[wLen++] = ID[1] + 0x33;
        Buf[wLen++] = 0x12; // DF
        Buf[wLen++] = 0x0E; // DB
        memset((BYTE *)&Buf[wLen], 0xBB, 4);
        wLen = wLen + 4;

        for (i = 0; i < bDataLen; i++)
        {
            Buf[wLen++] = pBuf[i] + 0x33;
        }
    }
    Buf[wLen] = lib_CheckSum(&Buf[4], wLen - 4);
    wLen++;
    Buf[wLen++] = 0x16;
    nwy_uart_send_data(UART_HD, Buf, wLen);
    // nwy_ext_echo("\r\n");
    // for (uint32_t i = 0; i < wLen; i++)
    // {
    // 	nwy_ext_echo("%02X ", Buf[i]); // 按十六进制格式打印
    // }

    return TRUE;
}
//--------------------------------------------------
// 功能描述:  瞬时量读取后续帧
//
// 参数:
//
// 返回值:
//
// 备注:  可为每个uart创建，禁止使用静态变量
//--------------------------------------------------
void Dlt645_Tx_Read_UIP_Follow(void)
{
    BYTE buf[] = {0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x12, 0x05, 0x35,
                  0x21, 0x21, 0x21, 0x34, 0xE3, 0x16};
    nwy_uart_send_data(UART_HD, buf, sizeof(buf));
    // nwy_ext_echo("\r\n\r\n");
    // for (uint32_t i = 0; i < sizeof(buf); i++)
    // {
    // 	nwy_ext_echo("%02X ", buf[i]); // 按十六进制格式打印
    // }
}

//--------------------------------------------------
// 功能描述:  采集瞬时量任务
//
// 参数: FlagBytes 时间标志
//
// 返回值:
//
// 备注:
//--------------------------------------------------
void CollectInstantDataTask(TFlagBytes *FlagBytes)
{
    BYTE ID[2] = {0x07, 0x13};

    if ((PowerOnReadMeter == 1))
    {
        // nwy_ext_echo("\r\nenter collect instant data task");
        if (api_GetSysStatus(eSYS_STASUS_TIMER_UP))
        {
            api_ClrSysStatus(eSYS_STASUS_TIMER_UP);
            api_SetSysStatus(eSYS_STASUS_UART_IDLE);
        }
        if (Serial.ReadFollowFlag == 1)
        {
            Serial.ReadFollowFlag = 0;
            Dlt645_Tx_Read_UIP_Follow();
            nwy_start_timer(uart_timer, 5000); // 5s定时
        }
        if (api_GetTaskFlag(FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND) == TRUE)
        {
            api_ClrTaskFlag(FlagBytes, eTASK_SAMPLE_ID, eFLAG_SECOND);
            // 表示多帧抄读完成
            if (api_GetSysStatus(eSYS_STASUS_UART_IDLE))
            {
                api_CommWithMeter_DBDF(0x11, 0, ID, 0); // 读取瞬时量
                api_ClrSysStatus(eSYS_STASUS_UART_IDLE);
                nwy_start_timer(uart_timer, 5000); // 5s定时
            }
#if (EVENT_REPORT == YES)
            dwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_EVENT));
#endif
        }
    }
}
// 流量统计功能
void CalculateFlow(void)
{
    static WORD i = 0;
    i++;
    char rsp[128] = {0};
    QWORD total_bytes = 0;
    nwy_data_flowcalc_info_t flowcalc_info = {0};
    nwy_time_t tTimer;
    // 计算时间差(秒)
    int total_seconds = 0;
    float flow_per_second = 0;
    float flow_per_day = 0;
    BYTE timezone = 8;
    if (i >= 2000)
    {
        i = 0;
        nwy_data_get_flowcalc_info(&flowcalc_info);
        total_bytes = flowcalc_info.tx_bytes + flowcalc_info.rx_bytes;
        snprintf(rsp, sizeof(rsp),
                 "tx_bytes:%llu,rx_bytes:%llu,total_bytes:%llu\r\n"
                 "tx_packets:%lu,rx_packets:%lu",
                 flowcalc_info.tx_bytes, flowcalc_info.rx_bytes, total_bytes,
                 flowcalc_info.tx_packets, flowcalc_info.rx_packets);
        //  nwy_ext_echo("\r\n%s", rsp);
        nwy_get_time(&tTimer, &timezone);
        // 转换为总秒数
        total_seconds = (tTimer.hour - poweronTimer.hour) * 3600 +
                        (tTimer.min - poweronTimer.min) * 60 +
                        (tTimer.sec - poweronTimer.sec);
        nwy_ext_echo("\r\ntotal_bytes:%llu\r\n", total_bytes);
        if (total_seconds > 0)
        {
            // 计算每秒流量(字节/秒)
            flow_per_second = (float)total_bytes / total_seconds;
            // 计算每日流量(MB/天)
            flow_per_day = flow_per_second * 86400 / (1024 * 1024);
            nwy_ext_echo("\r\nflow per second: %.2f bytes/s", flow_per_second);
            nwy_ext_echo("\r\nflow per 30 days: %.2f MB", flow_per_day * 30);
        }
    }
}

//备注:  可为每个uart创建，禁止使用静态变量
//--------------------------------------------------
void Uart_Task(void *parameter)
{
    //该路串口初始化
    nwy_osiEvent_t event;
    TFlagBytes FlagBytes;
    tTranData TranDataToMeter;
#if (GPRS_POSITION == YES)
    BYTE bmin = 0;
#endif
    char heapinfo[100] = {0};
    int iTmp;
#if (FLOW_CALCULATE == YES)
    BYTE timezone = 8;
    nwy_get_time(&poweronTimer, &timezone);
#endif

    Init_UartPara_DL645();
    memset(&TranDataToMeter, 0, sizeof(TranDataToMeter));
    memset((BYTE *)&FlagBytesRam, 0, sizeof(TFlagBytes));
    memset((BYTE *)&FlagBytes, 0, sizeof(TFlagBytes));
    dwCurTicks = nwy_get_ms();
    dwCurTicksHis = dwCurTicks;
    nwy_start_timer_periodic(general_timer, 1000); //周期性定时 一秒一次
    api_SetSysStatus(eSYS_STASUS_UART_IDLE);
    nwy_stop_timer(uart_timer);

    while (1)
    {
        dwCurTicks = nwy_get_ms();

        if ((dwCurTicks - dwCurTicksHis) > 10000) //如超过10s未进定时器。重新打开定时器
        {
            dwCurTicksHis = dwCurTicks;
            nwy_stop_timer(general_timer); //关闭定时器，重新打开
            nwy_sleep(50);
            nwy_start_timer_periodic(general_timer, 1000); //周期性定时 一秒一次
        }

        if (PowerOnReadMeter == 0)
        {
            DetectReadMeterStasus();
        }
        else
        {
#if (CYCLE_REPORT == YES)
            ToMqttByCycle();
#endif
        }
        api_ReceData_UartTask();
        nwy_get_msg_que(TimerMessageQueue, &FlagBytes, 0xffffffff);

        if (tIap_645.dwIapFlag == BASEMETER_UPGRADES)
        {
            MeterUpgrade645_Task();
        }
        else if (tIap_698.dwIapFlag == BASEMETER_UPGRADES)
        {
            MeterUpgrade698_Task();
        }
        else
        {
            if (api_GetSysStatus(eSYS_STASUS_START_COLLECT) != TRUE) //判断当前处于上传模式则不会进行透传
            {
                //消息队列可能同时攒了多条，所以加上判断接收后发下一帧透传数据，判断顺序不能变
                if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE)
                {
                    if (nwy_get_msg_que(TranDataToUartMessageQueue, &TranDataToMeter, 0xffffffff))
                    {
                        api_SetSysStatus(eSYS_STASUS_START_TRAN);
                        gTranType = TranDataToMeter.TranType;
                        SendTranData(TranDataToMeter.buf, TranDataToMeter.len);
                        nwy_ext_echo("\r\ntran data send ok");
                    }
                }
            }
            if (api_GetSysStatus(eSYS_STASUS_START_TRAN) != TRUE) //透传结束,才允许进行抄表
            {

                // if(api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR) == TRUE)
                // {
                // 	dwReadMeterFlag[0] |= (1<<(CycleReadMeterNum + eBIT_TIME));
                // 	api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_HOUR);
                // }
                if (api_GetTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE) == TRUE)
                {
                    nwy_dm_get_device_heapinfo(heapinfo);
                    iTmp = nwy_sdk_vfs_ls();
                    // nwy_ext_echo("RAM[%s]ROM[%d]", heapinfo, iTmp);
#if (GPRS_POSITION == YES)
                    if (bmin >= 5)
                    {
                    }
                    else
                    {
                        bmin++;
                        dwReadMeterFlag[0] |= (1 << (CycleReadMeterNum + eBIT_POSITION));
                    }
#endif
                    api_ClrTaskFlag(&FlagBytes, eTASK_PARA_ID, eFLAG_MINUTE);
                }

#if (COLLECT_UIP == YES)
                CollectInstantDataTask(&FlagBytes);
#endif
                // CollectMeterTask();
            }
        }
#if (FLOW_CALCULATE == YES)
        CalculateFlow();
#endif
        nwy_wait_thead_event(uart_thread, &event, 50);
    }
}

// 关于透传 如果正在抄表时，突然来透传报文则透传失败