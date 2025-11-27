//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司电表软件研发部
//创建人	低压台区软件-zhaohang
//创建日期	2023.02.07
//描述		
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define MEAN_COUNT	50

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------
extern SYS_PARA_1808 SysPara;
extern DWORD xStructQueue;

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
float V_data[MAX_PHASE][G_SAMPLE_NUM];  //电压报文解析数据
float I_data[MAX_PHASE][G_SAMPLE_NUM];  //电流报文解析数据
QWORD g_U50[MAX_PHASE] = {0};			//电压，50个平均
QWORD g_I50[MAX_PHASE] = {0};			//电流，50个平均
SQWORD g_P50[MAX_PHASE+1] = {0};		//有功功率，50个平均
BYTE g_Pcount = 0;						//50个平均计数
//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------
float EU_factor[MAX_PHASE];
float EI_factor[MAX_PHASE];
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
extern void SetPqData(BYTE type, void *val);/*稳态数据存放到数据库*/
//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  电压解析
//         
//参数:      *rawdate：原始数据指针， *voltagedatebuff：存储电压数据的指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void get_voltage(void *rawdate,float *voltagedatebuff)
{
	long ptemp;
	BYTE *r_date, temp1, i = 0,ph = 0;
	WORD temp2;

	r_date = (BYTE *)rawdate;
	temp1 = CHMAX*4;

	for (i = 0; i < G_SAMPLE_NUM; i++)
	{
		temp2 = temp1 * i;
		for(ph = 0;ph < MAX_PHASE; ph++)
		{
			
			ptemp = (r_date[temp2+3+ph*4]<<24)+(r_date[temp2+2+ph*4]<<16)+(r_date[temp2+1+ph*4]<<8)+(r_date[temp2+ph*4]);
			if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001) //开启了周波输入功能
			{
				*(voltagedatebuff+ph*G_SAMPLE_NUM+i) = (float)(ptemp * EU_factor[ph]);
			}
			else
			{
				*(voltagedatebuff+ph*G_SAMPLE_NUM+i) = (float)(ptemp * SysPara.big_adjust_para.U_factor[ph]);
			}

		}
	}
}

//--------------------------------------------------
//功能描述:  电流解析 
//         
//参数:      *rawdate：原始数据指针， *currentdatebuff：存储电流数据的指针
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void get_current(void *rawdate,float *currentdatebuff)
{
	long ptemp;
	BYTE *r_date, temp1, i = 0,ph = 0;
	WORD temp2;
	
	r_date = (BYTE *)rawdate;
	temp1 = CHMAX*4;
	for (i = 0; i < G_SAMPLE_NUM; i++)
	{
		temp2 = temp1 * i + 4*MAX_PHASE;
		for(ph = 0;ph < MAX_PHASE; ph++)
		{
			ptemp = (r_date[temp2+3+ph*4]<<24)+(r_date[temp2+2+ph*4]<<16)+(r_date[temp2+1+ph*4]<<8)+(r_date[temp2+ph*4]);
			if(((UsePrintfFlag >> 5) & 0x0001) == 0x0001) //开启了周波输入功能
			{
				*(currentdatebuff+ph*G_SAMPLE_NUM + i) = (float)(ptemp * EI_factor[ph]);
			}
			else
			{
				*(currentdatebuff+ph*G_SAMPLE_NUM + i) = (float)(ptemp * SysPara.big_adjust_para.I_factor[ph]);
			}
		}
	}
}
//--------------------------------------------------
//功能描述:  获取有功
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetActivePower(SDWORD *Act)
{
	FLOAT sum1 =0;
	BYTE i = 0,ph = 0;

	for(ph = 0; ph < MAX_PHASE; ph++)
	{
		sum1 = 0;
		for (i = 0; i < G_SAMPLE_NUM; i++)
		{
			sum1 += V_data[ph][i] * I_data[ph][i];
		}
		Act[ph+1] = (INT)((float)(sum1 / G_SAMPLE_NUM) * 10000);
	}
	#if(MAX_PHASE == 3)
	Act[0] = Act[1] + Act[2] + Act[3];
	#else
	Act[0] = Act[1];
	#endif
}

//--------------------------------------------------
//功能描述:  获取电压有效值
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetVrms(DWORD *Vrms)
{
	FLOAT sum2 = 0;
	BYTE i = 0,ph = 0;

	for(ph = 0; ph < MAX_PHASE; ph++)
	{
		sum2 = 0;
		for (i = 0; i < G_SAMPLE_NUM; i++)
		{
			sum2 += V_data[ph][i] * V_data[ph][i];
		}
		Vrms[ph] = (DWORD)((float)sqrt(sum2 / G_SAMPLE_NUM) * 10000);
	}
}

//--------------------------------------------------
//功能描述:  获取电流有效值
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void GetIrms(DWORD *Irms)
{
	FLOAT sum3 = 0;
	BYTE i = 0,ph = 0; 

	for(ph = 0; ph < MAX_PHASE; ph++)
	{
		sum3 = 0;
		for (i = 0; i < G_SAMPLE_NUM; i++)
		{
			sum3 += I_data[ph][i] * I_data[ph][i];
		}
		Irms[ph] = (DWORD)((float)sqrt(sum3 / G_SAMPLE_NUM) * 10000);
	}
}
//--------------------------------------------------
//功能描述:  电压电流有功的存储
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  SaveVIP( void )
{
	DWORD Vrms[MAX_PHASE],Irms[MAX_PHASE];
	SDWORD P[MAX_PHASE+1];//总abc
	BYTE i = 0;

	memset((BYTE*)&P[0],0,sizeof(P));

	GetVrms(Vrms);
	GetIrms(Irms);
	GetActivePower(P);

	for(i = 0;i<(MAX_PHASE);i++)
	{
		g_U50[i] += Vrms[i];//abc
		g_I50[i] += Irms[i];//abc
	}

	for(i = 0;i<(MAX_PHASE+1);i++)
	{
		g_P50[i] += P[i];//总abc
	}

	
	g_Pcount++;

	if(g_Pcount >= MEAN_COUNT)
	{
		g_Pcount = 0;
		for(i = 0;i<(MAX_PHASE+1);i++)
		{
			P[i] = (SDWORD)(g_P50[i]/MEAN_COUNT);//总abc
		}

		for(i = 0;i<(MAX_PHASE);i++)
		{
			Vrms[i] = (DWORD)(g_U50[i]/MEAN_COUNT);//abc
			Irms[i] = (DWORD)(g_I50[i]/MEAN_COUNT);//abc
		}
		memset(&g_P50[0],0,sizeof(g_P50));
		memset(&g_U50[0],0,sizeof(g_U50));
		memset(&g_I50[0],0,sizeof(g_I50));

		SetPqData(PQ_P, P);				//有功
		SetPqData(PQ_U_RMS, Vrms);		//电压
		SetPqData(PQ_I_RMS, Irms);		//电流
	}


}

//-------------------------------------------------
//功能描述： 获取芯片唯一ID
//
//参数: buf 读取到ID的指针
//
//返回值：
//
//备注：
//--------------------------------------------------
void get_uid(uint8_t *buf)
{
	for(int i = 0;i<12;i++)
	{ 
		buf[i] = *(uint8_t*)(0x1FFF7A10+i);
	}
}

//--------------------------------------------------
//功能描述:  读取指定位置存储的license
//         
//参数:      buf 保存接收到license的指针
//         
//返回值:    读取到license的长度
//         
//备注:  
//--------------------------------------------------
int get_license(uint8_t *buf)
{
	BYTE len = 0;

	len = api_ReadLicense(buf);

	return len;
}
//--------------------------------------------------
//功能描述:  指定位置存储的license
//         
//参数:      buf 保存接收到license的指针
//         
//返回值:    读取到license的长度
//         
//备注:  
//--------------------------------------------------
void  save_license( uint8_t *write_buf )
{
 	TLicenseSafeRom TLicense;

	//uint8_t write_buf[128]={0x37,0x5c,0x2d,0x02,0x07,0xea,0xf7,0xf0,0x52,0xd3,0xd9,0x36,0x27,0x72,0x5d,0xaf,0x23,0xe7,0xde,0x13,0x70,0x20,0x3d,0x59,0x10,0xab,0x4b,0x6e,0x6b,0xcc,0x83,0x7d,0xb7,0x1d,0xa1,0x6c,0xda,0x58,0xdc,0x1f,0xb9,0xaa,0x22,0x1f,0xe5,0xe8,0xf1,0xc7,0xdc,0x82,0x44,0x2b,0x66,0xef,0xc8,0xfe,0xea,0xb1,0x14,0xdb,0x0a,0x28,0x51,0x4c,0x6d,0x7f,0x81,0x2e,0xf3,0x6a,0xdf,0x05,0x62,0x65,0x2a,0xe5,0x3a,0xc0,0xb7,0x79,0x85,0xcc,0xac,0x76,0x09,0xf4,0x96,0x4c,0x3c,0xae,0xa4,0x6c,0x6b,0x7b,0x03,0x88,0x3a,0x47,0xdc,0xa1,0x14,0xb9,0x50,0xca,0x41,0x20,0xea,0x82,0xa7,0x4d,0x94,0x3d,0x1a,0xc8,0xe2,0x5c,0x67,0x2b,0x0e,0xf2,0xcd,0x58,0x19,0x77,0x06,0x9d,0xea,0x84};
	memset((BYTE*)&TLicense.License[0], 0xFF, sizeof(TLicense));
	memcpy((BYTE*)&TLicense.License[0],(BYTE*)&write_buf[0],128);
	
	//存储license
	api_SaveLicense((BYTE*)&TLicense.License[0]);
}
