//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_212)
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
//环保212协议上报数据
TReportData ReportData[]  = 
{
	{"tn0110-Rtd,tn0110-Flag=N",eUa,4},				//A相电压
	{"tn0111-Rtd,tn0111-Flag=N",eUb,4},				//B相电压
	{"tn0112-Rtd,tn0112-Flag=N",eUc,4},				//C相电压
	{"tn0101-Rtd,tn0101-Flag=N",eIa,4},				//A相电流
	{"tn0102-Rtd,tn0102-Flag=N",eIb,4},				//B相电流	
	{"tn0103-Rtd,tn0103-Flag=N",eIc,4},				//C相电流
	{"tn0105-Rtd,tn0105-Flag=N",eP1,4},				//总有功功率
	{"tn0106-Rtd,tn0106-Flag=N",eQ1,4},				//总无功功率
	{"tn0109-Rtd,tn0109-Flag=N",ePF1,4},			//总功率因数
	{"tn0107-Rtd,tn0107-Flag=N",ePActiveAll,2},		//总有功电能
	{"tn0108-Rtd,tn0108-Flag=N",eRActive1All,2},	//总无功电能
	{"tn0104-Rtd,tn0104-Flag=N",eIzs,4}				//单相电流  先按零序
};

//现场向上位	↑
//上位向现场	↓
char CN[][5] = 
{
	{"9011"}, //请求应答	↑
	{"9012"}, //执行结果	↑
	{"9013"}, //通知应答
	{"9014"}, //数据应答
	{"2011"}, //污染物实时数据
};

char ST[][3] = 
{
	{"91"},	//数据交互
	{"80"},	//用电监控
};

char EnvirProtectFrame[]  = "##0087QN=20250517171850000;ST=91;CN=2011;PW=123456;MN=DFWSD0000000013011110222;Flag=5;CP=&&&&0500";
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  212协议 计算 报文校验
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
unsigned int CRC16_Checkout(BYTE *puchMsg,unsigned int usDataLen)
{
	unsigned int i,j,crc_reg,check;
	crc_reg = 0xFFFF;

	for( i=0; i < usDataLen; i++)
	{
		crc_reg=(crc_reg>>8)^puchMsg[i];
		for(j=0;j<8;j++)
		{
			check=crc_reg&0x0001;
			crc_reg>>=1;
			if(check==0x0001)
			{
				crc_reg^=0xA001;
			}
		}	
	}								
	return crc_reg;	
}
//--------------------------------------------------
//功能描述:  将212格式报文 拆解 成数组形式
//         
//参数:      frame_str 传入字符串
//			TParsedFrame* pf  c传入结构体数组
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void ParseFrame_212(const char* frame_str,TParsedFrame* pf) 
{
		
	// 1. 去除协议头（假设## + 4字节协议代码，总长6字符）
	const char* kv_str = frame_str + START_FRAME; // 指向键值对部分
		
	// 2. 复制并分割键值对
	char* str_copy = strdup(kv_str); //复制 原字符串数据 到申请的一块新空间
	if (!str_copy) 
	{
		free(pf);
		return ;
	}
		
	char* token = strtok(str_copy, ";");		//查找 ";" 替换成 "\0" 返回字符串开头，后续 传入null 会继续上次分割位置
	int idx = 0;
	while (token && idx < MAX_DATA_ITEM) 
	{
		char* eq = strchr(token, '=');
		if (!eq) 
		{
			token = strtok(NULL, ";");
			continue;
		}
		
		*eq = '\0'; // 将等号替换为空字符
		
		TKeyValue* kv = &pf->items[idx++];
		kv->key = strdup(token);
		kv->value = strdup(eq + 1);
		
		token = strtok(NULL, ";");
	}
		
	pf->count = idx;
	free(str_copy);
	nwy_ext_echo("success prase");
}
//--------------------------------------------------
//功能描述:  释放资源函数
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void free_frame(TParsedFrame* pf) 
{
	for (int i=0; i<pf->count; ++i) 
	{
		free(pf->items[i].key);
		free(pf->items[i].value);
	}
}
//--------------------------------------------------
//功能描述:  获取指定键的值
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
const char* GetFrameValue(const TParsedFrame* pf, const char* key) 
{
	for (int i=0; i<pf->count; ++i) 
	{
		if (strcmp(pf->items[i].key, key) == 0) 
		{
			return pf->items[i].value;
		}
	}
	return NULL;
}
//--------------------------------------------------
//功能描述:  替换指定键的值
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL ReplaceFrameValue(TParsedFrame* pf, char* key, char* value) 
{
	for (int i=0; i<pf->count; ++i) 
	{
		if (strcmp(pf->items[i].key, key) == 0) 
		{
			// nwy_ext_echo("\r\n%s",pf->items[i].key);
			free(pf->items[i].value);
			pf->items[i].value = strdup(value);
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  追加 键 和值 到Frame
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BOOL AddFrameValue(TParsedFrame* pf, char* key, char* value) 
{
	if (pf->count >= (MAX_DATA_ITEM-1))
	{
		return FALSE;
	}
	else
	{
		pf->items[pf->count].key = strdup(key);
		pf->items[pf->count].value = strdup(value);
		pf->count++;
		return TRUE;
	}
}
//--------------------------------------------------
//功能描述:  212数据组帧，添加 =  和 ;
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void Compose_Frame(char *buf,TParsedFrame* pf)
{
	WORD addr = 0;
	for (int i = 0; i < pf->count; i++)
	{
		memcpy(&buf[addr],pf->items[i].key,strnlen(pf->items[i].key,100));
		addr = addr + strnlen(pf->items[i].key,100);	
		buf[addr++] = '=';
		memcpy(&buf[addr],pf->items[i].value,strnlen(pf->items[i].value,100));
		addr = addr + strnlen(pf->items[i].value,100);	
		buf[addr++] = ';';
	}
	addr--;
	buf[addr] = '\0';
}

//--------------------------------------------------
//功能描述:  	212格式判断
//		 
//参数:	  
//		 
//返回值:	
//		 
//备注:  
//--------------------------------------------------
BOOL  DoReceProc_212( BYTE ReceByte ,char *pBuf)
{
	WORD wDataLen = 0;
	WORD wCrc;
	char buf[10] = {0};
	BYTE tempcrc[5] = {0};

	if (pBuf[0] == '#')//取数据长度
	{
		if (pBuf[1] == '#')
		{
			memcpy(buf,&pBuf[2],4);
			wDataLen = atol(buf);
			nwy_ext_echo("data len is %d\n",wDataLen);
		}
	}
	if (pBuf[START_FRAME] == 'Q')//计算检验
	{
		wCrc = CRC16_Checkout((BYTE*)&pBuf[START_FRAME],wDataLen);
		sprintf(buf,"%04x",wCrc);
		nwy_ext_echo("crc is %s\n",buf);

		memcpy(tempcrc,&pBuf[START_FRAME+wDataLen],4);
		tempcrc[4] = '\0';

		nwy_ext_echo("last1 crc is %s\n",&pBuf[START_FRAME+wDataLen]);
		nwy_ext_echo("last2 crc is %s\n",tempcrc);

		for (BYTE i = 0; i < 4; i++)
		{
			if (buf[i] != tempcrc[i])
			{
				return FALSE;
			}	
		}
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------
//功能描述:  212上行数据组帧
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
// WORD  api_Compose211ReportFrame( TParsedFrame *pf ,char * inbuf ,double *Data)
// {
// 	char buf[11] = {0};
// 	char time[30] =  {0};
// 	char reporttime[26] = {"&&DataTime="};
// 	char buf2[START_FRAME+5] = {0};//+'0'
// 	WORD wCrc,wLen;
// 	nwy_time_t julian_time = {0};
// 	char timezone = 0;
// 	// 检索数据
// 	// nwy_ext_echo("PW: %s\n", GetFrameValue(pf, "PW")); // 应输出 123456
// 	// nwy_ext_echo("ST: %s\n", GetFrameValue(pf, "ST")); // 应输出 91

// 	nwy_get_time(&julian_time, &timezone);
// 	sprintf(time,"%04d%02d%02d%02d%02d%02d000", julian_time.year, julian_time.mon, julian_time.day, julian_time.hour, julian_time.min, julian_time.sec);
	
// 	ReplaceFrameValue(pf,"QN",time);

// 	memcpy(&reporttime[11],time,strlen(time)-3);
// 	reporttime[strlen(time)+9] = '\0';
// 	ReplaceFrameValue(pf,"CP",reporttime);

// 	for (BYTE i = 0; i < sizeof(ReportData)/sizeof(TReportData); i++)
// 	{
// 		char strTemp[64] = {0};
// 		double dbtemp = 0;
// 		memcpy(buf,ReportData[i].dataname,10);//拷贝 前段tn0105-Rtd
// 		buf[10] = '\0';
		
// 		dbtemp = Data[ReportData[i].ValueType];
// 		sprintf(strTemp, "%0.*f",ReportData[i].point,dbtemp);
// 		strcat(strTemp,&ReportData[i].dataname[10]);// 将,tn0106-Flag=N 拷贝置strTemp之后

// 		AddFrameValue(pf,buf,strTemp);
// 	}
	

// 	Compose_Frame(&inbuf[START_FRAME],pf);

// 	//计算除结束符 && 的数据总长度
// 	wLen = strnlen(&inbuf[START_FRAME],1024);
// 	//追加&&结束符号
// 	inbuf[START_FRAME+(wLen++)] = '&';
// 	inbuf[START_FRAME+(wLen++)] = '&';
	
// 	sprintf(&buf2[2],"%04d",wLen);
// 	buf2[0] = '#';
// 	buf2[1] = '#';
// 	memcpy(inbuf,buf2,START_FRAME);

// 	wCrc = CRC16_Checkout((BYTE*)&inbuf[START_FRAME],wLen);
// 	sprintf(&inbuf[START_FRAME+wLen],"%04x",wCrc);
// 	inbuf[START_FRAME+(wLen+4)] = '\r';
// 	inbuf[START_FRAME+(wLen+5)] = '\n';
// 	inbuf[START_FRAME+(wLen+6)] = '\0';

// 	nwy_ext_echo("\r\nout : %s\n",inbuf);

// 	wLen = START_FRAME+(wLen+6);
// 	free_frame(pf);
// 	return wLen;
// }

//--------------------------------------------------
//功能描述:  组环保协议帧
//         
//参数:      pf[in] 			传入协议外壳 名称和数据项
//           inbuf[in]			传入缓存
//           DataItem[in] 		传入上报数据名称
//           Data[in] 			传入上报数据
//           item[in] 			传入上报数据项个数
//返回值:    
//         
//备注:  
//--------------------------------------------------
WORD  api_ComposeEnvirProtectFrame( TParsedFrame *pf ,char * inbuf ,TReportData *DataItem, double *Data ,BYTE item )
{
	char buf[11] = {0};
	char time[30] =  {0};
	char reporttime[26] = {"&&DataTime="};
	char buf2[START_FRAME+5] = {0};//+'0'
	WORD wCrc,wLen;
	nwy_time_t julian_time = {0};
	char timezone = 0;
	

	nwy_get_time(&julian_time, &timezone);
	sprintf(time,"%04d%02d%02d%02d%02d%02d000", julian_time.year, julian_time.mon, julian_time.day, julian_time.hour, julian_time.min, julian_time.sec);
	
	ReplaceFrameValue(pf,"QN",time);

	memcpy(&reporttime[11],time,strlen(time)-3);
	reporttime[strlen(time)+9] = '\0';
	ReplaceFrameValue(pf,"CP",reporttime);

	for (BYTE i = 0; i < item; i++)
	{
		char strTemp[64] = {0};
		double dbtemp = 0;
		memcpy(buf,DataItem[i].dataname,10);//拷贝 前段tn0105-Rtd
		buf[10] = '\0';
		
		dbtemp = Data[DataItem[i].ValueType];
		sprintf(strTemp, "%0.*f",DataItem[i].point,dbtemp);
		strcat(strTemp,&DataItem[i].dataname[10]);// 将,tn0106-Flag=N 拷贝置strTemp之后

		AddFrameValue(pf,buf,strTemp);
	}
	

	Compose_Frame(&inbuf[START_FRAME],pf);

	//计算除结束符 && 的数据总长度
	wLen = strnlen(&inbuf[START_FRAME],1024);
	//追加&&结束符号
	inbuf[START_FRAME+(wLen++)] = '&';
	inbuf[START_FRAME+(wLen++)] = '&';
	
	sprintf(&buf2[2],"%04d",wLen);
	buf2[0] = '#';
	buf2[1] = '#';
	memcpy(inbuf,buf2,START_FRAME);

	wCrc = CRC16_Checkout((BYTE*)&inbuf[START_FRAME],wLen);
	sprintf(&inbuf[START_FRAME+wLen],"%04x",wCrc);
	inbuf[START_FRAME+(wLen+4)] = '\r';
	inbuf[START_FRAME+(wLen+5)] = '\n';
	inbuf[START_FRAME+(wLen+6)] = '\0';

	nwy_ext_echo("\r\nout : %s\n",inbuf);

	wLen = START_FRAME+(wLen+6);
	free_frame(pf);
	return wLen;
}
//--------------------------------------------------
//功能描述:  212协议 停上电 上报帧
//         
//参数:      bPower   1 停电 0 上电
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_PowerSwitchReportFrame( double bPower , tTranData *transdata)
{
	//tn0250-Rtd=1 停电 0 上电
	TParsedFrame tempFrame =  {0};

	TReportData temp = {"tn0250-Rtd,tn0250-Flag=N",0,0};
	ParseFrame_212(EnvirProtectFrame,&tempFrame);
	transdata->len = api_ComposeEnvirProtectFrame(&tempFrame,(char*)&transdata->buf[0],&temp,&bPower,1);
	nwy_put_msg_que(TranDataToServerMessageQueue, transdata, 0xffffffff);
}
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_CycleReportData( tTranData *transdata ,double *Data)
{
	BYTE bLen;
	TParsedFrame tempFrame =  {0};

	bLen = sizeof(ReportData)/sizeof(TReportData);

	for(BYTE i = 0; i < MAX_SAMPLE_CHIP; i++)
	{
		ParseFrame_212(EnvirProtectFrame,&tempFrame);
		transdata->len = api_ComposeEnvirProtectFrame(&tempFrame,(char*)&transdata->buf[0],ReportData,&Data[MAX_SAMPLE_CHIP*i],bLen);
	}
	
	nwy_put_msg_que(TranDataToServerMessageQueue, transdata, 0xffffffff);
}
#endif//#if( CYCLE_REPORT_PROTOCAL ==  eMQTT)