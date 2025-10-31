//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#include "wsd_dlt698.h"
#if( READMETER_PROTOCOL ==  PROTOCOL_645)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define UNSIGNED			0x00								//无符号
#define SIGNED				0x80								//有符号
#define PT_MULTI			0x01								//上传数据乘PT
#define CT_MULTI			0x02						
#define PT_CT_MULTI			0x03						


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
#pragma pack(1)
typedef struct _position{
	BYTE location;				//方位
	WORD degree;				//度  
	float min;					//分 
}position;
#pragma pack()

#pragma pack(1)
typedef struct _TOad40040200{
	position longitude;				//经度  
	position latitude;				//纬度   
	DWORD high;					//高度 cm
}TOad40040200;
#pragma pack()

typedef struct tMeterRead_t
{
	DWORD OI;				//645数据标识
	BYTE Num;				//一个数据块有几个数据项
	BYTE Slen;				//单个数据项长度
	BYTE Dot;				//接收数据小数位
	BYTE Symbol;			//是否需要处理符号位 bit7 符号判断 bit1 bit0 区分变比
	double* Buf;			//数据存放地址
}tMeterRead;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
char bAddr[6];				//电表通信地址
char reverAddr[6];			//地址倒叙方便发送
TSerial Serial;	
ServerParaRam_t	Serverpara[2];
char MeterVersion[35] = {0};
char LtuAddr[30] = {0};
DWORD PowerOnReadMeterFlag = 0;//上电抄表成功置bit位 目前七个
BYTE PublishInstantData[MAX_PHASE_NUM*INSTANT_DATA_LEN_PER_PHASE + 4];//4字节序号

BYTE bHisReadMeter[MAX_SAMPLE_CHIP] = {0}; // 上次抄表bit位
DWORD dwReadMeterFlag[MAX_SAMPLE_CHIP] = {0};
BYTE bReadMeterRetry = 0;
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
TOtherPara OtherPara;
static BYTE Loop = 0;
//扩展规约数据标识
const WORD  EventOI[]  = { 0x1700 };
const WORD  PtandCt[]  = { 0x1118,0x1119,0x111A,0x111B};//CT1,CT2,pt1,pt2
const WORD  Position[] = { 0x1300};
BYTE SampleStep = 1;		//瞬时量及电能抄读Step（秒任务）
BYTE ParaPowerOnStep = 0; 	//上电抄参数Step

#if(CYCLE_REPORT == YES)
double MeterData[MAX_SAMPLE_CHIP][SINGLE_LOOP_ITEM+MAX_RATIO*4];//电表采集的数据存放处
tMeterRead tMeterReadOIRam;
const tMeterRead  MeterReadOI[] = 
{     
	{0x0201FF00,3,2,1,PT_MULTI,&MeterData[0][0]  },											//电压数据块	
	{0x0211FF00,3,2,1,PT_MULTI,&MeterData[0][3]  },											//线电压数据块
	{0x02800001,1,3,3,SIGNED|CT_MULTI,&MeterData[0][6]  },									//零序电流
	{0x0202FF00,3,3,3,SIGNED|CT_MULTI,&MeterData[0][7]  },									//电流数据块
	{0x020CFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][10] },								//有功功率数据块 //标准645读取3个字节,扩展645抄读4个字节
	{0x020DFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][14] },								//无功功率数据块
	{0x020EFF00,4,4,4,SIGNED|PT_CT_MULTI,&MeterData[0][18] },								//视在功率数据块
	{0x0206FF00,4,2,3,SIGNED,&MeterData[0][22] },											//功率因数数据块
	{0x02800002,1,2,2,UNSIGNED,&MeterData[0][26] },											//频率
	{0x0001FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM] },				//正向有功总电能数据块
	{0x0002FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO] },		//反向有功总电能
	{0x0003FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO*2] },	//无功1总电能
	{0x0004FF00,MAX_RATIO,4,2,PT_CT_MULTI,&MeterData[0][SINGLE_LOOP_ITEM+MAX_RATIO*3] },	//无功2总电能
};
#endif //if(CYCLE_REPORT == YES)
#if(CYCLE_REPORT == YES)
const BYTE CycleReadMeterNum =  sizeof(MeterReadOI)/sizeof(tMeterRead);
#else
const BYTE CycleReadMeterNum =  0;
#endif
#if(EVENT_REPORT == YES)
static BYTE bHisDIStatus;							//当前遥信状态
static BYTE EventPowerOn = 0xAA;					//防止刚上电就上报
#endif
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//函数功能:	PT CT计算
//
//参数:	bType[in]: 获取类型
//
//返回值:	数据
//
//备注:
//-----------------------------------------------
double api_GetPtandCT(BYTE bType)
{
	double dbData = 1;
	#if(PT_CT == YES)
	switch(bType)
	{
		case eType_PT:
			if( OtherPara.bPTSecond == 0)
			{
				dbData = 1;
			}
			else
			{
	
				dbData = (double)OtherPara.dwPTPrim / OtherPara.bPTSecond;//PT变比，是分子除以分母后的值
			}
			break;
		case eType_CT:
			if( OtherPara.bCTSecond == 0 )
			{
				dbData = 1;
			}
			else
			{
				dbData = (double)OtherPara.dwCTPrim / OtherPara.bCTSecond;//CT变比，是分子除以分母后的值
			}	
			break;
		case eType_PT_CT:
			if( ( OtherPara.bCTSecond == 0 ) || ( OtherPara.bPTSecond == 0) )
			{
				dbData = 1;
			}
			else
			{
				dbData = (double)OtherPara.dwCTPrim * OtherPara.dwPTPrim / OtherPara.bCTSecond / OtherPara.bPTSecond;
			}	
			break;
		default:
			break;
	}
	#endif
	return dbData;
}
//-----------------------------------------------
//函数功能: 检查一个字节的数据是否为BCD码
//
//参数: 
//			In[in]					要判断的数据
//                    
//返回值:  	TRUE:数据符合BCD码		FALSE:数据不是BCD码
//
//备注:   
//-----------------------------------------------
BOOL lib_CheckBCD(BYTE In)
{
	if( (In&0x0f) > 0x09 )
	{
		return FALSE;
	}

	if( (In>>4) > 0x09 )
	{
		return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------
//函数功能: 一个字节的BCD码数据转换为HEX码数据
//
//参数: 
//			In[in]					要转换的数据
//                    
//返回值:  	转换后的HEX码数据
//
//备注:   
//-----------------------------------------------
BYTE lib_BBCDToBin(BYTE In)
{
	BYTE   Hex;
	
	if( lib_CheckBCD( In ) == FALSE )
	{
		
	}
    
    Hex = In & 0xF0;
    Hex >>= 1;
    Hex += (Hex>>2);
    Hex += In & 0x0F;
    return Hex;
}

WORD MW(BYTE Hi,BYTE Lo)
{
	return (WORD)((Hi<<8)+Lo);
}
DWORD MDW(BYTE HH,BYTE HL,BYTE LH,BYTE LL)
{
	return (DWORD)((HH<<24) | (HL<<16) | (LH<<8) | LL);
}
static void nwy_hex_to_string(int len, char *data ,char *buf)
{
	int i = 0;
	unsigned char highByte;
	unsigned char lowByte;
	
	for(i = 0; i < len; i++)
	{
		highByte = data[i] >> 4;
		lowByte = data[i] & 0x0f;

		highByte += 0x30;

		if(highByte > 0x39)
			buf[i * 2] = highByte + 0x07;
		else
			buf[i * 2] = highByte;

		lowByte += 0x30;
		if(lowByte > 0x39)
			buf[i * 2 + 1] = lowByte + 0x07;
		else
			buf[i * 2 + 1] = lowByte;
	}
}
//--------------------------------------------------
//功能描述:  把BufB的数据倒序给BufA
//         
//参数:      BufA[out] 		输出数据的缓冲
//         
//           BufB[in]		输入数据的缓冲
//         
//           Len[in]		数据长度
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void lib_ExchangeData(BYTE *BufA, BYTE *BufB, BYTE Len )
{
    BYTE n;
    BYTE Buf[255];
    
    memcpy( Buf, BufB, Len );
    
    for( n=0; n<Len; n++ )
    {
        BufA[n] = Buf[Len - 1 - n];
    }
}
//-----------------------------------------------
//函数功能: 计算单字节累加和，超出一个字节的部分舍弃
//
//参数: 
//	ptr[in]:	输入缓冲
//
//	Length[in]:	缓冲长度
//                    
//返回值:	单字节累加和
//
//备注: 
//-----------------------------------------------
BYTE lib_CheckSum(BYTE * ptr, WORD Length)
{
	WORD i;
	BYTE Sum = 0;

	for(i=0; i<Length; i++)
	{
		Sum += *ptr;
		ptr ++;
	}

	return Sum;
}
//--------------------------------------------------
//功能描述:  探测基表地址
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Adrr_Txd(void)
{
	BYTE reqAddr[]={0xFE,0xFE,0xFE,0xFE,0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x13,0x00,0xDF,0x16};
	nwy_uart_send_data(UART_HD,reqAddr,sizeof(reqAddr));
}
//--------------------------------------------------
//功能描述:  645 扩展规约读取 版本
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_Read_Version(void) //抄读OK
{
	BYTE Buf[] = {0xfe,0xfe,0xfe,0xfe,0x68,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x68,
				  0x11,0x08,0x0e,0x12,0x12,0x37,0x0e,0x3b,0x12,0x0e,0xb7,0x16};

	nwy_uart_send_data(UART_HD,Buf,sizeof(Buf));
}
//--------------------------------------------------
//功能描述:  645 通用规约读取 0x11
//         
//参数:      dwID 数据标识
//         	bAddress 基表地址
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_Read( DWORD dwID ) //抄读OK
{
	BYTE bLen,Buf[20];

	bLen = 12;
	memset(Buf,0,sizeof(Buf));
	//组前导帧
	memset( (BYTE *)&Buf[0], 0xFE, 4 );

	Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	// lib_ExchangeData((BYTE*)&Buf[5],(BYTE*)&bAddr[0],6);
	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;	
	Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);	
	Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);	
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,bLen);
}
//--------------------------------------------------
//功能描述:  645 通用规约读取 0x11	四回路使用
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_ReadMultiLoop( DWORD dwID, BYTE *bAddress )
{
	BYTE bLen,Buf[20];

	bLen = 12;
	memset(Buf,0,sizeof(Buf));
	//组前导帧
	memset( (BYTE *)&Buf[0], 0xFE, 4 );

	Buf[4] = 0x68;
	// memset(&Buf[5],0x88,6);
	lib_ExchangeData((BYTE*)&Buf[5],bAddress,6);
	Buf[5+6] = 0x68;
	Buf[bLen++] = 0x11;
	Buf[bLen++] = 0x04;	
	Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);	
	Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
	Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);	
	Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
	bLen++;
	Buf[bLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,bLen);
}
//--------------------------------------------------
//功能描述:  645 通用规约设置 0x14
//         
//参数:      dwDI 数据
//           bLen 
//           pBuf 设置数据内容
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  Dlt645_Tx_Write( DWORD dwID,BYTE bDataLen,BYTE *pBuf )//全8 地址有问题   写buf数据时不知道 要不要转格式
{
    BYTE i,bLen,Buf[128];

    bLen = 12;
    memset(Buf,0,sizeof(Buf));
    //组前导帧
	memset( (BYTE *)&Buf[0], 0xFE, 4 );
    Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	Buf[5+6] = 0x68;
    Buf[bLen++] = 0x14;
    Buf[bLen++] = 12+bDataLen;
    

    Buf[bLen++] = (BYTE)(LLBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(LHBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(HLBYTE(dwID)+0x33);
    Buf[bLen++] = (BYTE)(HHBYTE(dwID)+0x33);
    //密码 操作者代码
    Buf[bLen++] = 0x35;
    memset((BYTE *)&Buf[bLen],0x33,7);
    bLen = bLen + 7;
    //数据
    memcpy((BYTE*)&Buf[bLen],pBuf,bDataLen);
    for (i = 0; i < bDataLen; i++)
    {
       Buf[bLen] =Buf[bLen]  + 0x33;
       bLen++;
    }
    
    Buf[bLen] = lib_CheckSum(&Buf[4],bLen-4);
    bLen++;
    Buf[bLen++] = 0x16;
    // for (i = 0; i < bLen; i++)
    // {
    //     nwy_ext_echo("%c",Buf[i]);
    // }
    nwy_uart_send_data(UART_HD,Buf,bLen);
}
//--------------------------------------------------
//函数功能: 扩展规约DBDF 与LTU建立一次通信
//
//参数:		Type[int],  操作码，
// 								
//          ID[in],	数据标识
//					
//			bDataLen[in],设置参数的长度
//
//          pBuf[in],要设置的数据
//返回值: 	TRUE/FALSE
//
//备注:     0x11,0x14
//-----------------------------------------------
BOOL CommWithMeter_DBDF( BYTE Type, BYTE bDataLen, BYTE *ID ,BYTE*pBuf)//读取和设置OK
{
	BYTE i;
	WORD wLen = 12;
	BYTE Buf[128];
	//组同步帧 
	memset( (BYTE *)&Buf[0], 0x00, sizeof(Buf));
    
    //组前导帧
	memset( (BYTE *)&Buf[0], 0xFE, 4 );
	
	Buf[4] = 0x68;
	memset(&Buf[5],0x88,6);
	Buf[5+6] = 0x68;
	Buf[wLen++] = Type;
	
	if(Type == 0x11)//扩展规约抄读
	{
		Buf[wLen++] = 8;
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;	
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = ID[0] + 0x33;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12;//df
		Buf[wLen++] = 0x0E;//db
	}
	else if(Type == 0x14) //扩展规约写 
	{
		Buf[wLen++] = bDataLen+20; //长度可设置
		Buf[wLen++] = 0x0E;
		Buf[wLen++] = 0x12;	
		Buf[wLen++] = 0x12;
		Buf[wLen++] = 0x37;
		Buf[wLen++] = 0x35;
		memset((BYTE *)&Buf[wLen],0x33,7);
		Buf[wLen+7] = ID[0] + 0x33; 
	    wLen = wLen+8;
		Buf[wLen++] = ID[1] + 0x33;
		Buf[wLen++] = 0x12;//DF
		Buf[wLen++] = 0x0E;//DB
		memset((BYTE *)&Buf[wLen],0xBB,4);
		wLen = wLen+4;

		for ( i = 0; i <bDataLen; i++)
		{
			Buf[wLen++] = pBuf[i] + 0x33;
		}
	}
	Buf[wLen] = lib_CheckSum(&Buf[4],wLen-4);
    wLen++;
	Buf[wLen++] = 0x16;
	nwy_uart_send_data(UART_HD,Buf,wLen);
    return TRUE;
}
#if(CYCLE_REPORT == YES	)
//--------------------------------------------------
//功能描述:  事件检测任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_EventDetect( void )
{
	CommWithMeter_DBDF(0x11,0,(BYTE*)&EventOI[0],0);
}
#endif
#if(GPRS_POSITION == YES)
//--------------------------------------------------
//功能描述:  扩展规约检测或设置地理位置
//         
//参数:      bMode  1设置地理位置；0 检测地理位置
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  api_CheckorSetPosition( BYTE bMode )
{
	int result = 0;
	char message[2048] = {0};
	DWORD dwJWD[9] = {0};
	TOad40040200 TLocation;
	
	memset((BYTE*)&TLocation,0,sizeof(TOad40040200));
	// 获取经纬度位置
	result = nwy_loc_get_nmea_data(message);
	if (result) {
	    nwy_ext_echo("\r\n get nmea data success");
	} else {
	    nwy_ext_echo("\r\n get nmea data fail");
	}
	get_gps_position_info(message,dwJWD);
	TLocation.latitude.location = (BYTE)dwJWD[3];
	TLocation.latitude.degree = dwJWD[2];
	TLocation.latitude.min = (dwJWD[1]*100000 + dwJWD[0])/pow(10,5);
	TLocation.longitude.location = dwJWD[7];
	TLocation.longitude.degree = dwJWD[6];
	TLocation.longitude.min = (dwJWD[5]*100000 + dwJWD[4])/pow(10,5);
	TLocation.high = dwJWD[8]*100;
	nwy_ext_echo("\r\nTLocation.longitude.location is %d",TLocation.longitude.location);
	nwy_ext_echo("\r\nTLocation.longitude.degree is %d",TLocation.longitude.degree);
	nwy_ext_echo("\r\nTLocation.longitude.min is %f",TLocation.longitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.location is %d",TLocation.latitude.location);
	nwy_ext_echo("\r\nTLocation.latitude.degree is %d",TLocation.latitude.degree);
	nwy_ext_echo("\r\nTLocation.latitude.min is %f",TLocation.latitude.min);
	nwy_ext_echo("\r\nTLocation.latitude.high is %d",TLocation.high);
	if ((TLocation.longitude.degree == 0)&&(TLocation.latitude.degree == 0))
	{
		nwy_ext_echo("\r\nget location failed");
		if (bMode != 0)
		{
			nwy_gpio_set_value(LOCAL_LIGHT,1);
		}
		//本地灯亮
		return FALSE;
	}
	else
	{
		if (bMode == 1)
		{
			Dlt645_Tx_Write(0x0400041F,sizeof(TLocation),(BYTE*)&TLocation);
		}
		return TRUE;
	}
}
#endif
//--------------------------------------------------
//功能描述:  抄表任务
//         
//参数:      bBtep 传入标志bit位
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_ReadMeterTask645( BYTE bStep,BYTE* bAddr )
{
	BYTE bTempStep;

	bTempStep = (bStep-CycleReadMeterNum);
	if (bTempStep & 0x80)
	{
		bTempStep = 0xff;
	}
	nwy_ext_echo("\r\nreadMeter step is%d",bStep);
	// nwy_ext_echo("\r\nreadMeter bTempstep is%d",bTempStep);
	nwy_gpio_set_value(LOCAL_LIGHT,0);
	switch (bTempStep)
	{
		case eBIT_ADDR:
			Adrr_Txd();
			break;
		case eBIT_TIME:
			Dlt645_Tx_Read(DAY_TIME);
			break;
		case eBIT_TCPNET:
			Dlt645_Tx_Read(TCP_REQ_NET);
			break;
		case eBIT_MQTTNET:
			Dlt645_Tx_Read(MQTT_REQ_NET);
			break;
		case eBIT_TCPUSER:
			Dlt645_Tx_Read(TCP_REQ_USER_PD);
			break;
		case eBIT_MQTTUSER:
			Dlt645_Tx_Read(MQTT_REQ_USER_PD);
			break;
		case eBIT_VERSION:
			Dlt645_Tx_Read_Version();		
			break;
		#if(PT_CT == YES)
		case eBIT__CT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[1],0);
			break;
		case eBIT__CT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[2],0);
			break;
		case eBIT_PT1:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[3],0);
			break;
		case eBIT_PT2:
			CommWithMeter_DBDF(0x11,0,(BYTE*)&PtandCt[4],0);
			break;
		#endif
		case eBIT_POSITION:
#if (GPRS_POSITION == YES)
			api_CheckorSetPosition(1);
#endif
			break;
#if(CYCLE_REPORT == YES)
		case 0xFF:
			Dlt645_Tx_ReadMultiLoop((DWORD)MeterReadOI[bStep].OI,bAddr);
			break;
#endif
		default:
			break;
	}

	nwy_stop_timer(uart_timer);
	nwy_start_timer(uart_timer,1000);//1s定时
	api_SetSysStatus(eSYS_STASUS_START_COLLECT);
	
}
#if(CYCLE_REPORT == YES)
//-----------------------------------------------
//功能描述:  抄表数据解析并保存
//         
//参数:      pBuf[in]  dataBuf[in] 数据保存位置
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  DataAnalyseandSave( BYTE* pBuf ,double* dataBuf ,BYTE Len)
{
	BYTE ID[4];
	BYTE i,j,bStart,bEnd,bOffset,bSymbol,bDot;
	int symbol[4] = {1,1,1,1};		
	SDWORD temp[MAX_RATIO] = {0};//数组大小按最大费率数开
	char Buf[MAX_RATIO][10] = {0};
	int value = 0;
	double dwData = 0;
	j = 0;

	memcpy((BYTE*)&ID[0],(BYTE*)&tMeterReadOIRam.OI,sizeof(DWORD));
	bStart = tMeterReadOIRam.Slen -1;
	bOffset = tMeterReadOIRam.Slen;
	
	if (Len < (tMeterReadOIRam.Slen*tMeterReadOIRam.Num))	//用于区分单三相
	{
		bEnd = Len;
	}
	else
	{
		bEnd = tMeterReadOIRam.Slen*tMeterReadOIRam.Num;
	}
	bSymbol = tMeterReadOIRam.Symbol;
	bDot = tMeterReadOIRam.Dot;
	
	for (i = bStart; i < bEnd;)
	{
		if ((bSymbol & 0x80)==0x80)
		{	
			if ((pBuf[i] & 0x80)==0x80)//最高位的符号处理  
			{
				pBuf[i] &= 0x7F;//符号处理		
				symbol[j] = -1;
			}	
		}
		memcpy((BYTE*)&temp[j],(BYTE*)&pBuf[i-bStart],bOffset);
		sprintf((char*)Buf[j],"%lx",temp[j]);
		if ((bSymbol & 0x80)==0x80)
		{
			value =  atol(Buf[j])*symbol[j];
		}
		else
		{
			value =  atol(Buf[j]);
		}
		dwData = value / (1.0 * pow(10, bDot));
		//在这之后可能会乘上变比 存放double数组// 四回路无扩展规约读变比，单回路有
		if ((bSymbol & 0x0F) == PT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_PT);
		}
		else if ((bSymbol & 0x0F) == CT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_CT);
		}
		else if ((bSymbol & 0x0F) == PT_CT_MULTI)
		{
			dwData = dwData*api_GetPtandCT(eType_PT_CT);
		}

		//sprintf();乘上变比后 末位有无尽小数 统一传固定小数
		// nwy_ext_echo("\r\n %f",dwData);
		memcpy(dataBuf,&dwData,sizeof(double));
		dataBuf ++;
		i = i+bOffset;
		j++;
	}	
}
//--------------------------------------------------
//功能描述: 计量数据传送给MQTT任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void SampleDatatoMqtt( TRealTimer *pTime )
{
	BYTE i;    
	QWORD qwMs;
	CollectionDatas CollectValue;
	char heapinfo[100]={0};
	int iTmp;
	for (i = 0; i < MAX_SAMPLE_CHIP; i++)
    {
        memcpy((double*)&CollectValue.CollectMeter[i][0],(double*)&MeterData[i][0],(SINGLE_LOOP_ITEM+MAX_RATIO*4)*sizeof(double)); 
		for ( BYTE j = 0; j < (SINGLE_LOOP_ITEM+MAX_RATIO*4); j++)
		{
			nwy_ext_echo("\r\n %f",CollectValue.CollectMeter[i][j]);
		}
    }
    memcpy(CollectValue.Addr,LtuAddr,sizeof(LtuAddr));
    //时间戳
    pTime->Sec = 0;
    qwMs = getmktimems(pTime);
    CollectValue.TimeMs = qwMs;
	#if( EVENT_REPORT ==  YES)
	CollectValue.bDI = bHisDIStatus;
	#endif
    nwy_ext_echo("\r\n hour is %d",pTime->Hour);
    nwy_ext_echo("\r\n min is %d",pTime->Min);
	nwy_dm_get_device_heapinfo(heapinfo);
	iTmp = nwy_sdk_vfs_ls();
	nwy_ext_echo("RAM[%s]ROM[%d]",heapinfo,iTmp);
    if(nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
    {
        nwy_put_msg_que(CollectMessageQueue,&CollectValue,0xffffffff);
    }
}
#endif
//--------------------------------------------------
//功能描述:  数据接收成功清除相应bit位
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  ReceiveClearBitFlag(void)
{
	api_ClrSysStatus(eSYS_STASUS_START_COLLECT);
	// nwy_ext_echo("\r\ncurrent loop is %d ", Loop);
	dwReadMeterFlag[Loop] &= ~(1<<bHisReadMeter[Loop]);
	PowerOnReadMeterFlag |=(1<<bHisReadMeter[Loop]);
	bHisReadMeter[Loop] = 0xFF;
	bReadMeterRetry = 0;
	nwy_stop_timer(uart_timer);
	nwy_gpio_set_value(LOCAL_LIGHT,1);
}
//--------------------------------------------------
//功能描述:  645_07应用层函数
//         
//参数:      dwID[in] 要抄读的数据标识
//         		pBuf[in] 数据
//				len[in] 数据长度
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  DealDLT645_2007( DWORD dwID,BYTE* pBuf,BYTE Len )//直接拷贝多字节数据 无需倒叙 
{
	DWORD dwIP;
	TRealTimer tTime = {0};
	BYTE j;
	BYTE Sum = 0;
	BYTE bCorrect = 0;
	BYTE bReceiveBit = 0;
	#if(CYCLE_REPORT == YES)
	BYTE bOffset;
	WORD i;
	#endif

	if ((dwID == TCP_REQ_NET)||(dwID == MQTT_REQ_NET)||(dwID == TCP_REQ_USER_PD)||(dwID == MQTT_REQ_USER_PD))
	{
		for (j = 0; j < (Len-1); j++)
		{
			Sum += pBuf[j];
			if ((pBuf[j] != 0)&&(pBuf[j] != 0xff))//只要有不是0或ff的值就认为是合法值
			{
				bCorrect = 1;
			}
		}	
		if ((bCorrect !=1)||(Sum != pBuf[Len-1]))
		{
			return FALSE;
		}
	}
	if (dwID == DAY_TIME)
	{
		//对应数据标识接收函数
		tTime.Sec = lib_BBCDToBin(pBuf[0]);
		tTime.Min = lib_BBCDToBin(pBuf[1]);
		tTime.Hour = lib_BBCDToBin(pBuf[2]);
		tTime.Timezone = 8;
		tTime.Day = lib_BBCDToBin(pBuf[4]);
		tTime.Mon = lib_BBCDToBin(pBuf[5]);
		tTime.wYear = 2000+lib_BBCDToBin(pBuf[6]);
		if(api_CheckClock(&tTime) == TRUE)
		{
			set_N176_time(&tTime);
			nwy_ext_echo("\r\n enter set time ");
			bReceiveBit = 1;
		}
	}
	else if (dwID == TCP_REQ_NET)
	{		
		dwIP  = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		sprintf(Serverpara[0].ServerIP,"%d.%d.%d.%d",(int)LLBYTE(dwIP),(int)LHBYTE(dwIP),(int)HLBYTE(dwIP),(int)HHBYTE(dwIP));
		Serverpara[0].ServerPort = MW(pBuf[5],pBuf[4]);
		nwy_ext_echo("\r\ntcp ip%s",Serverpara[0].ServerIP);
		nwy_ext_echo("\r\ntcp port %d",Serverpara[0].ServerPort);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_NET)
	{	
		dwIP  = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
		sprintf(Serverpara[1].ServerIP,"%d.%d.%d.%d",(int)LLBYTE(dwIP),(int)LHBYTE(dwIP),(int)HLBYTE(dwIP),(int)HHBYTE(dwIP));
		Serverpara[1].ServerPort = MW(pBuf[5],pBuf[4]);
		nwy_ext_echo("\r\n mqtt ip%s",Serverpara[1].ServerIP);
		nwy_ext_echo("\r\n mqtt port%d",Serverpara[1].ServerPort);
		bReceiveBit = 1;	
	}
	else if (dwID == TCP_REQ_USER_PD)
	{
		memcpy((BYTE*)&Serverpara[0].ServerUserName[0],pBuf,32);
		memcpy((BYTE*)&Serverpara[0].ServerUserPwd[0],pBuf+32,32);
		nwy_ext_echo("\r\nTCP USER %s",Serverpara[0].ServerUserName);
		nwy_ext_echo("\r\nTCP PWD %s",Serverpara[0].ServerUserPwd);
		bReceiveBit = 1;
	}
	else if (dwID == MQTT_REQ_USER_PD)
	{	
		lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserName[0],pBuf,32);
		lib_ExchangeData((BYTE*)&Serverpara[1].ServerUserPwd[0],pBuf+32,32);
		nwy_ext_echo("\r\n MQTT USER %s",Serverpara[1].ServerUserName);
		nwy_ext_echo("\r\n MQTT PWD %s",Serverpara[1].ServerUserPwd);
		bReceiveBit = 1;
	}
	else
	{
		#if(CYCLE_REPORT == YES)
		for (i = 0; i < CycleReadMeterNum; i++)
		{
			if (dwID == MeterReadOI[i].OI)
			{
				memcpy((BYTE*)&tMeterReadOIRam,(BYTE*)&MeterReadOI[i],sizeof(tMeterRead));
				bOffset = Loop*(SINGLE_LOOP_ITEM+MAX_RATIO*4); 
				DataAnalyseandSave(pBuf,tMeterReadOIRam.Buf + bOffset,Len);
				//执行到这认为一帧抄读完毕
				bReceiveBit = 1;
				if (i == (CycleReadMeterNum-1))//表格遍历结束
				{
					nwy_ext_echo("\r\n %d loop is success",Loop);
					if (Loop == (MAX_SAMPLE_CHIP - 1))
					{	
						SampleDatatoMqtt(&tTimer);
					}
				}
				break;
			}
		}
		#endif
	}
	if (bReceiveBit == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  扩展规约应用层函数
//         
//参数:      pBuf[in] 扩展规约数据标识 DBDFXXXX
//        
//			bDataLen[in] 读到的数据长度 
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  DealDLT645_Factory( BYTE* pBuf,BYTE bDataLen)
{
	#if(EVENT_REPORT == YES)
	Eventmessage EventData;
	TRealTimer tTime = {0};
	QWORD qwMs;
	#endif
	BYTE bReceiveBit = 0;

	switch (pBuf[1])
	{
		case 0x13://每秒读取瞬时量
		if (pBuf[0] == 0x07)
		{
			if (Serial.ReadFollowFlag == 1) // 分帧处理
			{
				memcpy((BYTE *)&PublishInstantData[0], (BYTE *)&pBuf[2], MAX_DATA_LEN_PER_FRAME); // 194字节数据
			}
			else
			{
			}
			nwy_stop_timer(uart_timer);
		}
		break;
		case 0x17:
			#if(EVENT_REPORT == YES)
			if(EventPowerOn == 0)
			{
				if (pBuf[2] != bHisDIStatus)
				{
					nwy_ext_echo("report DI\r\n %d",pBuf[2]);
					get_N176_time(&tTime);
					qwMs = getmktimems(&tTime);
					EventData.TimeMs = qwMs;
					EventData.DIStatus = pBuf[2];
					memcpy((BYTE*)&EventData.Addr,(BYTE*)&LtuAddr,sizeof(LtuAddr));
					bReceiveBit = 1;
					if(nwy_get_queue_spaceevent_cnt(EventReportMessageQueue) != 0)
					{
					    nwy_put_msg_que(EventReportMessageQueue,&EventData,0xffffffff);
					}
				}
			}
			EventPowerOn = 0;
			bHisDIStatus = pBuf[2];
			#endif
			break;
		case 0x11://变比
			#if(PT_CT == YES)
			if (pBuf[0] == 0x18)
			{
				memcpy((BYTE*)&OtherPara.dwCTPrim,(BYTE*)&pBuf[2], 4);
				nwy_ext_echo("CT1\r\n %d",OtherPara.dwCTPrim);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x19)
			{
				OtherPara.bCTSecond = pBuf[2];
				nwy_ext_echo("CT2\r\n %d",OtherPara.bCTSecond);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x1A)
			{
				memcpy((BYTE*)&OtherPara.dwPTPrim,(BYTE*)&pBuf[2], 4);
				nwy_ext_echo("PT1\r\n %d",OtherPara.dwPTPrim);
				bReceiveBit = 1;
			}
			else if(pBuf[0] == 0x1B)
			{
				OtherPara.bPTSecond = pBuf[2];
				nwy_ext_echo("PT2\r\n %d",OtherPara.bPTSecond);
				bReceiveBit = 1;
			}
			#endif
			break;
		case 0x08://版本
			if (pBuf[0] == 0xDB)
			{	
				sprintf(MeterVersion, "version:%02X%02X,verification:%02X%02X", 
						pBuf[11], pBuf[10], pBuf[16], pBuf[17]);
				nwy_ext_echo("\r\nMeterVersion %s",MeterVersion);
				bReceiveBit = 1;
			}

			break;
		default:
			break;
	}	
	if (bReceiveBit == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  对报文进行减0x33处理
//         
//参数:      BYTE *pBuf[in]:输入缓冲pBuf[0]为68H
//         
//返回值:    发送的数据标识
//         
//备注内容:  无
//--------------------------------------------------
DWORD DataSubtract33H(BYTE *pBuf)
{
	WORD i;
	DWORD dwID;

	for(i=1;i<=pBuf[0];i++)
	{
		pBuf[i]=(pBuf[i]-0x33);
	}
	memcpy((BYTE*)&dwID,(BYTE*)&pBuf[1],4);

	return dwID;
	
}
void ReverseData(BYTE* buf,BYTE len)
{
	BYTE temp = 0;
	if (buf == NULL || len <= 1)
	{
		return; // 空指针或长度不足以反转
	}

	BYTE start = 0;		// 起始索引
	BYTE end = len - 1; // 结束索引
	while (start < end)
	{
		// 交换 buf[start] 和 buf[end]
		temp = buf[start];
		buf[start] = buf[end];
		buf[end] = temp;

		start++;
		end--;
	}
}
//--------------------------------------------------
//功能描述: int转float
//         
//参数:      起始字节地址
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
float BytesToFloat(BYTE *bytes)
{
	DWORD intValue = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
	return (float)intValue;
}
//--------------------------------------------------
//功能描述: 处理上传数据格式  
//         
//参数:     
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void processInstantData(void)
{
	BYTE i = 0, j = 0;
 	BYTE *buf;
    float Float;
	//逆序所有四字节数据
    for ( i = 0; i < sizeof(PublishInstantData) / 4; i++)
    {
        ReverseData(&PublishInstantData[4*i], 4);
    }
	//uipq小数点处理
    for (i = 0; i < MAX_PHASE_NUM; i++)
    {	
		for (j = 0; j < 4; j++)
		{
			buf = &PublishInstantData[i *4*DATA_ITEM_NUM_PER_PHASE + j * 4];
			Float = BytesToFloat(buf);	
			if(j == 1)
			{
				Float /=10000;	
			}
			else
			{
				Float /=10;	
			}
			memcpy(buf, &Float, sizeof(float));
			ReverseData(buf, 4);
		}
    }

}
				
//--------------------------------------------------
//功能描述:  4G模块 对接受的报文解析(07规约)
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL  RxUartMessage_Dlt645( TSerial *p )
{
	DWORD dwDataType;
	T645FmRx *pRxFm;
	BYTE *pBuf,bLen,i=0;
	BYTE *pTmp = &PublishInstantData[0];
	BYTE res = 0;
	
	pRxFm = (T645FmRx *)& p->ProBuf[0];
	
	if ((pRxFm->byCtrl == 0xB1) || (pRxFm->byCtrl == 0x92)) // 读取瞬时量
	{
		pBuf = (BYTE *)&pRxFm->byDataLen;
		dwDataType = DataSubtract33H(pBuf); // 对数据进行减0x33处理
		if (dwDataType == DLT645_FACTORY_ID)
		{	
			bLen = pRxFm->byDataLen - 4;
			return (DealDLT645_Factory(&pBuf[5], bLen));
		}
		else if (dwDataType == DLT645_FACTORY_FOLLOW_ID) // 后续帧
		{
			bLen = pRxFm->byDataLen - 5;
			nwy_stop_timer(uart_timer);
			api_SetSysStatus(eSYS_STASUS_UART_IDLE);
			memcpy(&pTmp[MAX_DATA_LEN_PER_FRAME], &(p->ProBuf[14]), bLen);
			if (nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
			{	
				// nwy_ext_echo("publishData:\r\n len:%d\r\n",sizeof(PublishInstantData));
				// for(WORD j = 0; j < sizeof(PublishInstantData); j++)
				// {
				// 	nwy_ext_echo("%02x ",pTmp[j]);
				// }
				processInstantData();
				Compose_InstantData_698();
				// nwy_ext_echo("\r\nfinish one 698");
				res = nwy_put_msg_que(CollectMessageQueue, &InstantData698Frame[0], 10);
				memset(PublishInstantData, 0, sizeof(PublishInstantData));
				if (res == TRUE)
				{
					// nwy_ext_echo("\r\nInstant Data put to collect queue suc");
				}
				else
				{
					nwy_ext_echo("\r\nInstant Data put to collect queue fail");
				}
			}
			else
			{
				nwy_ext_echo("\r\n collect queue full");
			}
			return FALSE;		
		}
		else
		{
			bLen = pRxFm->byDataLen - 4;
			return (DealDLT645_2007(dwDataType, &pBuf[5], bLen));
		}
	}
	else if (pRxFm->byCtrl == 0x91)
	{
		//判断第几回路数据	
		Loop = ((pRxFm->MAddr[0])-bAddr[5]);
		if (Loop>=MAX_SAMPLE_CHIP)
		{
			Loop = 0;//防止探测出末位地址 9，10这种 不符合四回路的地址
		}
		pBuf = (BYTE*)&pRxFm->byDataLen;
		dwDataType = DataSubtract33H( pBuf );//对数据进行减0x33处理

		if (dwDataType == DLT645_FACTORY_ID)
		{
			bLen = pRxFm->byDataLen - 6;
			return	(DealDLT645_Factory(&pBuf[5],bLen));
		}
		else
		{
			bLen = pRxFm->byDataLen - 4;
			return	(DealDLT645_2007(dwDataType,&pBuf[5],bLen));
		}
	}
	else if (pRxFm->byCtrl == 0x94)
	{
		return TRUE;
	}
	else if (pRxFm->byCtrl == 0x93)
	{
		pBuf = (BYTE*)&pRxFm->byDataLen;
		bLen = pRxFm->byDataLen;
		for(i=1;i<=pBuf[0];i++)
		{
			pBuf[i]=(pBuf[i]-0x33);
		}
		lib_ExchangeData((BYTE*)&bAddr[0],(BYTE*)&pBuf[1],bLen);
		memcpy((BYTE*)&reverAddr[0],(BYTE*)&pBuf[1],bLen);
		nwy_hex_to_string(6,bAddr,LtuAddr);
		nwy_ext_echo("\r\n meter addr:%s", LtuAddr);
		return TRUE;		
	}
	else
	{
		return FALSE;
	}
}
//--------------------------------------------------
//功能描述:  645规约处理主函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  DealUartMessage_Dlt645( TSerial *p  )
{
	if (RxUartMessage_Dlt645(p))
	{
		ReceiveClearBitFlag();
	}
}
//---------------------------------------------------------------
//函数功能: uart通信645格式判断
//
//参数: 	portType[in] - 通讯端口
//                   
//返回值:  FALSE: 报文没有收完
//		  TRUE : 报文已收完且InitPoint()
//
//备注:   
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_UART( BYTE ReceByte, TSerial * p )
{
	WORD i, j;
	BYTE *pBuf;
	TDlt645Head Dlt645Head;

	
	//搜索同步头
	if( p->ProStepDlt645 == 0 )
	{
		if(ReceByte == 0x68)
		{
			//写入数据
			//p->ProBuf[p->ProStepDlt645] = ReceByte;
			//Dlt645规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt645 = p->RXRPoint;

			//操作指针
			p->ProStepDlt645 ++;

			//开始接收定时
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
	}
	else if( p->ProBuf[p->BeginPosDlt645] == 0x68 )
	{
		//接收规约头、控制字、长度 68 1 2 3 4 5 6 68 01 02
		if( p->ProStepDlt645 < sizeof(TDlt645Head) )
		{
			//写入数据
			//p->ProBuf[p->ProStepDlt645] = ReceByte;

			//操作指针
			p->ProStepDlt645 ++;
			
			//是否可以处理数据
			if( p->ProStepDlt645 == sizeof(TDlt645Head) )
			{
				//把规约报文移到临时缓冲中 循环缓冲用此
				//Num：移动多少字节
				//BeginPos:从什么地址开始移
				pBuf = (BYTE *)&Dlt645Head;
				for( i=0; i<sizeof(Dlt645Head); i++ )
				{
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN];
				}
				//映射结构
				//pDlt645Head = (TDlt645Head *)&TmpArray;
                j = p->ProStepDlt645;
				if (Dlt645Head.Control == 0xB1) // 分帧
				{
					p->ReadFollowFlag = 1;
					// nwy_ext_echo("\r\nReadFollowFlag set to 1");
				}
				else
				{
					// nwy_ext_echo("\r\nReadFollowFlag set to 0,control:%02X", Dlt645Head.Control);
					p->ReadFollowFlag = 0;
				}
				//搜索第二同步字
				if( Dlt645Head.Sync2 != 0x68 )//如果没有第2个帧起始符68，则前面收到的第一68不对，从它后面搜索第一个68
				{
					for( i=1; i<p->ProStepDlt645; i++ )
					{
						if( p->ProBuf[(p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN] == 0x68 )
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645+i)%UART_BUFF_MAX_LEN;
							p->ProStepDlt645 -= i;
							break;
						}
						//改后68前面其它数据或68都能通信上，但这个通信不上，没防这种，不想太复杂：68 fe fe fe fe fe fe 68 88 88 88 88 88 88 68 11 04 33 33 34 33 e2 16
					}
					if( i >= j )//j 是 p->ProStepDlt645 的备份，所有收到的数据已找完，则不再找，直接ProStepDlt645赋0跳出
					{
						p->ProStepDlt645 = 0;//InitPoint;
						return FALSE;
					}
					//p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
				else
				{					
				}
				//取长度 645报文数据域数据长度
				p->Dlt645DataLen = Dlt645Head.Length;

				//判断长度
				//68 A0...A5 68 81 L D0..DL CS 16,除D0...DL外，其余共16字节
				if( p->Dlt645DataLen >= ( UART_BUFF_MAX_LEN - 12 ) )
				{
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}
		}
		//接收数据
		else if( p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2 ) )
		{
			//操作指针
			p->ProStepDlt645 ++;

			//处理校验字节
			if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 1) )
			{
				//把规约报文移到临时缓冲中 循环缓冲用此
				//Num：移动多少字节
				//BeginPos:从什么地址开始移
				if( ReceByte != api_CalRXD_CheckSum(0, p) )
				//if( ReceByte != ( lib_CheckSum(p->ProBuf+p->BeginPosDlt645,(10+p->Dlt645DataLen))&0xff) )
				{
					//校验不正确
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}

			//收到0x16 才进行程序处理
			else if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 2) )
			{
				//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				api_DoReceMoveData(p,PROTOCOL_645);

 				if (Pre_Dlt645(p))
				{
				}
				else
				{
					DealUartMessage_Dlt645(p);
				}
				//处理完成
				InitPoint(p);

				return TRUE;
			}
			else
			{

			}

		}
		else
		{
			p->ProStepDlt645 = 0;//InitPoint(p);
		}
	}
	return FALSE;
}
#endif//#if( READMETER_PROTOCOL ==  PROTOCOL_645)