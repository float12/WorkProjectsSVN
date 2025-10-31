//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������	
//��������	
//����		
//�޸ļ�¼
//----------------------------------------------------------------------
#include "wsd_def.h"
#if( CYCLE_REPORT_PROTOCAL ==  PROTOCOL_212)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������		
//-----------------------------------------------
//����212Э���ϱ�����
TReportData ReportData[]  = 
{
	{"tn0110-Rtd,tn0110-Flag=N",eUa,4},				//A���ѹ
	{"tn0111-Rtd,tn0111-Flag=N",eUb,4},				//B���ѹ
	{"tn0112-Rtd,tn0112-Flag=N",eUc,4},				//C���ѹ
	{"tn0101-Rtd,tn0101-Flag=N",eIa,4},				//A�����
	{"tn0102-Rtd,tn0102-Flag=N",eIb,4},				//B�����	
	{"tn0103-Rtd,tn0103-Flag=N",eIc,4},				//C�����
	{"tn0105-Rtd,tn0105-Flag=N",eP1,4},				//���й�����
	{"tn0106-Rtd,tn0106-Flag=N",eQ1,4},				//���޹�����
	{"tn0109-Rtd,tn0109-Flag=N",ePF1,4},			//�ܹ�������
	{"tn0107-Rtd,tn0107-Flag=N",ePActiveAll,2},		//���й�����
	{"tn0108-Rtd,tn0108-Flag=N",eRActive1All,2},	//���޹�����
	{"tn0104-Rtd,tn0104-Flag=N",eIzs,4}				//�������  �Ȱ�����
};

//�ֳ�����λ	��
//��λ���ֳ�	��
char CN[][5] = 
{
	{"9011"}, //����Ӧ��	��
	{"9012"}, //ִ�н��	��
	{"9013"}, //֪ͨӦ��
	{"9014"}, //����Ӧ��
	{"2011"}, //��Ⱦ��ʵʱ����
};

char ST[][3] = 
{
	{"91"},	//���ݽ���
	{"80"},	//�õ���
};

char EnvirProtectFrame[]  = "##0087QN=20250517171850000;ST=91;CN=2011;PW=123456;MN=DFWSD0000000013011110222;Flag=5;CP=&&&&0500";
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  212Э�� ���� ����У��
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  ��212��ʽ���� ��� ��������ʽ
//         
//����:      frame_str �����ַ���
//			TParsedFrame* pf  c����ṹ������
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void ParseFrame_212(const char* frame_str,TParsedFrame* pf) 
{
		
	// 1. ȥ��Э��ͷ������## + 4�ֽ�Э����룬�ܳ�6�ַ���
	const char* kv_str = frame_str + START_FRAME; // ָ���ֵ�Բ���
		
	// 2. ���Ʋ��ָ��ֵ��
	char* str_copy = strdup(kv_str); //���� ԭ�ַ������� �������һ���¿ռ�
	if (!str_copy) 
	{
		free(pf);
		return ;
	}
		
	char* token = strtok(str_copy, ";");		//���� ";" �滻�� "\0" �����ַ�����ͷ������ ����null ������ϴηָ�λ��
	int idx = 0;
	while (token && idx < MAX_DATA_ITEM) 
	{
		char* eq = strchr(token, '=');
		if (!eq) 
		{
			token = strtok(NULL, ";");
			continue;
		}
		
		*eq = '\0'; // ���Ⱥ��滻Ϊ���ַ�
		
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
//��������:  �ͷ���Դ����
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  ��ȡָ������ֵ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  �滻ָ������ֵ
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  ׷�� �� ��ֵ ��Frame
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  212������֡����� =  �� ;
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
//��������:  	212��ʽ�ж�
//		 
//����:	  
//		 
//����ֵ:	
//		 
//��ע:  
//--------------------------------------------------
BOOL  DoReceProc_212( BYTE ReceByte ,char *pBuf)
{
	WORD wDataLen = 0;
	WORD wCrc;
	char buf[10] = {0};
	BYTE tempcrc[5] = {0};

	if (pBuf[0] == '#')//ȡ���ݳ���
	{
		if (pBuf[1] == '#')
		{
			memcpy(buf,&pBuf[2],4);
			wDataLen = atol(buf);
			nwy_ext_echo("data len is %d\n",wDataLen);
		}
	}
	if (pBuf[START_FRAME] == 'Q')//�������
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
//��������:  212����������֡
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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
// 	// ��������
// 	// nwy_ext_echo("PW: %s\n", GetFrameValue(pf, "PW")); // Ӧ��� 123456
// 	// nwy_ext_echo("ST: %s\n", GetFrameValue(pf, "ST")); // Ӧ��� 91

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
// 		memcpy(buf,ReportData[i].dataname,10);//���� ǰ��tn0105-Rtd
// 		buf[10] = '\0';
		
// 		dbtemp = Data[ReportData[i].ValueType];
// 		sprintf(strTemp, "%0.*f",ReportData[i].point,dbtemp);
// 		strcat(strTemp,&ReportData[i].dataname[10]);// ��,tn0106-Flag=N ������strTemp֮��

// 		AddFrameValue(pf,buf,strTemp);
// 	}
	

// 	Compose_Frame(&inbuf[START_FRAME],pf);

// 	//����������� && �������ܳ���
// 	wLen = strnlen(&inbuf[START_FRAME],1024);
// 	//׷��&&��������
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
//��������:  �黷��Э��֡
//         
//����:      pf[in] 			����Э����� ���ƺ�������
//           inbuf[in]			���뻺��
//           DataItem[in] 		�����ϱ���������
//           Data[in] 			�����ϱ�����
//           item[in] 			�����ϱ����������
//����ֵ:    
//         
//��ע:  
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
		memcpy(buf,DataItem[i].dataname,10);//���� ǰ��tn0105-Rtd
		buf[10] = '\0';
		
		dbtemp = Data[DataItem[i].ValueType];
		sprintf(strTemp, "%0.*f",DataItem[i].point,dbtemp);
		strcat(strTemp,&DataItem[i].dataname[10]);// ��,tn0106-Flag=N ������strTemp֮��

		AddFrameValue(pf,buf,strTemp);
	}
	

	Compose_Frame(&inbuf[START_FRAME],pf);

	//����������� && �������ܳ���
	wLen = strnlen(&inbuf[START_FRAME],1024);
	//׷��&&��������
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
//��������:  212Э�� ͣ�ϵ� �ϱ�֡
//         
//����:      bPower   1 ͣ�� 0 �ϵ�
//         
//����ֵ:    
//         
//��ע:  
//--------------------------------------------------
void  api_PowerSwitchReportFrame( double bPower , tTranData *transdata)
{
	//tn0250-Rtd=1 ͣ�� 0 �ϵ�
	TParsedFrame tempFrame =  {0};

	TReportData temp = {"tn0250-Rtd,tn0250-Flag=N",0,0};
	ParseFrame_212(EnvirProtectFrame,&tempFrame);
	transdata->len = api_ComposeEnvirProtectFrame(&tempFrame,(char*)&transdata->buf[0],&temp,&bPower,1);
	nwy_put_msg_que(TranDataToServerMessageQueue, transdata, 0xffffffff);
}
//--------------------------------------------------
//��������:  
//         
//����:      
//         
//����ֵ:    
//         
//��ע:  
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