#include "AllHead.h"
#include "task_sample.h"
#include "main_task.h"


#define FramePosToTimPos(pos)   (pos/ONE_FRAME_LEN_R)
#pragma default_variable_attributes = @ ".sram"
static char SampBuf[SAMP_BUFF_MAX_SIZE];        //采样值内存内存
#pragma default_variable_attributes =

static volatile DWORD SampTime[SAMP_BUFF_MAX_SIZE/CHMAX/sizeof(int)];//用于定标由于计算导致时间滞后，采用采样点补偿

SAMPLEDATABUF	rxbuff;

volatile DWORD s_sv_wr_pos;//写指针
volatile DWORD s_sv_rd_pos;//读指针
volatile BOOL s_pass_tail;

DWORD bufferoverflow=0;

WORD Last_frame_num = 0xffff;
volatile SPICOUNT SPI_Count;

BOOL bInitSample=FALSE;

int m_SampleNum = G_SAMPLE_NUM;
int nChannel = CHMAX;

BYTE SPIRecError = 0;
DWORD SPIResetCount = 0;
BYTE SPIResetConvert = 0;

DWORD  g_dwSoftWatDog; 		//软件计数，防止算法死机
DWORD  g_dwSoftWatDogRoot; //软件计数，防止算法死机
BYTE  bySoftWatCount;
extern BYTE SPI3DMAFrameBuf[ONE_FRAME_LEN];

extern int get_license_status( void ); //算法提供的获取license状态函数
extern void MX_SPI3_Init(void);
extern void api_ResetCpu(void);
void Init_Sample()
{
	s_sv_wr_pos = 0;
	s_sv_rd_pos = 0;
	s_pass_tail = FALSE;

	bInitSample = TRUE;
}

float GetChaData(float sdwData1, float sdwData2, BYTE byType)
{  
    return (sdwData1 + sdwData2)/2;
}

/*************************************************
 * 函数功能: 将报文中的采样值放入采样缓冲区
 * 参数: val--采样值指针
 * 返回值: 无
 * 备注: 
 *************************************************/
void set_smp_points( void *val)
{
    DWORD i,j,pos,m;
    SDWORD *ptemp;
    DWORD tim;
    BYTE *sv8;
    
    SAMPLEDATABUF nFirstData;
    SAMPLEDATABUF nNewData;
    SAMPLEDATABUF nChaData;
    // 为提高运算速度的临时变量
    DWORD temp1,temp2;

    sv8 = (BYTE *)val;

    //报文解析 3字节有符号数转换为4字节有符号数 每次传输一帧
    temp1 = CHMAX * 3;
    m = 0;
    for (j = 0; j < m_SampleNum; j++)
    {
        if(m_SampleNum == 256)
        {
            if(j%2)
              continue;
        }
        ptemp = &rxbuff.data.VA;
        temp2 = temp1 * j;
        
        for (i = 0; i < CHMAX; i++)
        {
            *ptemp = (sv8[2 + 3 * i + temp2] << 24) + (sv8[1 + 3 * i + temp2] << 16) + (sv8[0 + 3 * i + temp2] << 8); //最高位有符号位，必须平移24位
            *ptemp >>= 8;
            ptemp++;
        }
        
        memcpy(&SampBuf[s_sv_wr_pos + m * ONE_FRAME_LEN_R], &rxbuff.data.VA, ONE_FRAME_LEN_R); //按照字节数处理
        
        if(m_SampleNum == 64)
        {
            if(j == 0)
            {
                memcpy(&nFirstData.data.VA, &rxbuff.data.VA, ONE_FRAME_LEN_R);
                m += 2;
            }
            else
            {
                memcpy(&nNewData.data.VA, &rxbuff.data.VA, ONE_FRAME_LEN_R);
                
                nChaData.data.VA = (SWORD)GetChaData(nFirstData.data.VA, nNewData.data.VA, 0);
                nChaData.data.VB = (SWORD)GetChaData(nFirstData.data.VB, nNewData.data.VB, 0);
                nChaData.data.VC = (SWORD)GetChaData(nFirstData.data.VC, nNewData.data.VC, 0);
                nChaData.data.IA = (SWORD)GetChaData(nFirstData.data.IA, nNewData.data.IA, 1);
                nChaData.data.IB = (SWORD)GetChaData(nFirstData.data.IB, nNewData.data.IB, 1);
                nChaData.data.IC = (SWORD)GetChaData(nFirstData.data.IC, nNewData.data.IC, 1);
                
                memcpy(&SampBuf[s_sv_wr_pos + (m-1) * ONE_FRAME_LEN_R], &nChaData.data.VA, ONE_FRAME_LEN_R);
                
                memcpy(&nFirstData.data.VA, &nNewData.data.VA, ONE_FRAME_LEN_R);
                
                m += 2;
                
            }
        }
        else
        {
            m++;
        }
        
    }

    //写指针 对应的 时间戳指针,由于5帧数据都是一起收到的,所以时间戳相同
    pos = FramePosToTimPos(s_sv_wr_pos);
    tim = GetPrivateTime();
  
    
    /*每次写5帧，这5帧时间一样 假定采样值一帧一包的话，时间分辨率20ms*/
    m = 0;
    for(i=0;i<m_SampleNum;i++)
    {
        if(m_SampleNum == 256)
        {
            if(i%2)
              continue;
        }
        SampTime[pos+m] = tim;//补偿时间时候采用，每个写指针都进行打时标处理
        m++;
    }
 
    s_sv_wr_pos+=ONE_FRAME_LEN_W;//加多少跟采样点数有关
    if(s_sv_wr_pos >= SAMP_BUFF_MAX_SIZE)
    {
        s_sv_wr_pos = 0;
        s_pass_tail = TRUE;
    }

    // 缓冲区覆盖计数
    if((s_sv_wr_pos>s_sv_rd_pos)&&(s_pass_tail))
    {
        bufferoverflow++;
    }
}

/*************************************************
 * 函数功能: 处理采样数据报文
 * 参数: ReceByte--接收到的字节
 *       p--报文指针信息
 * 返回值: 无
 * 备注: 
 *       起始符   68H
 *       命令符   31H/33H/34H     31H-单相，33H-三相，34H-单相电压+火线电流+零线电流
 *       长度域   L0L1            2字节，除起始符和结束符之外的帧字节数
 *       帧序号   
 *       数据域
 *       累加和                   命令符——数据域，四字节
 *       结束符   16H
 *************************************************/

void DoSPIReceProc(BYTE *pBuf)
{
	BYTE Result = TRUE;
	WORD Length, Frame_num;
	DWORD Sum1, Sum2;
	DWORD dwAskLen;

	dwAskLen = m_SampleNum*CHMAX*3 + 10;
	if (pBuf[0] == 0x68) // 检测到起始符后
	{
		// 判断命令符是否正确
		if ((pBuf[1] != 0x31) && (pBuf[1] != 0x33) && (pBuf[1] != 0x34))
		{
			Result = FALSE;
			SPI_Count.ModeError++;
		}

		// 判断长度域是否正确
		Length = (pBuf[3] << 8) + (pBuf[2]);
		if (Length != (dwAskLen - 2))
		{
			Result = FALSE;
			SPI_Count.LengthError++;
		}

		if(((UsePrintfFlag >> 5) & 0x0001) == 0x0000) //关闭了周波输入功能
		{
			// 判断帧序号是否正确
			Frame_num = (WORD)(pBuf[4]);
			if ((Frame_num != (BYTE)(Last_frame_num + 1)) && (Last_frame_num != 0xffff))
			{
				Result = FALSE;
				SPI_Count.FrameNumError++;
			}
		}

		Last_frame_num = Frame_num;

		// 判断校验码是否正确
		Sum1 = lib_DWORDCheckSum(&pBuf[1], Length - 4);
		Sum2 = (pBuf[Length] << 24);
		Sum2 += (pBuf[Length - 1] << 16);
		Sum2 += (pBuf[Length - 2] << 8);
		Sum2 += pBuf[Length - 3];
		if (Sum1 != Sum2)
		{
			Result = FALSE;
			SPI_Count.CheckError++;
		}

		// 判断结束符是否正确
		if (pBuf[Length+1] != 0x16)
		{
			Result = FALSE;
			SPI_Count.EndError++;
		}
	}
	else
	{
		Result = FALSE;
		SPI_Count.HeadError++;
	}

	#if(!WIN32)
	#if(USER_TEST_MODE)
	if(SPI_Count.HeadError>100)   
	#else
	if(SPI_Count.HeadError>500)
	#endif
	{
		SPIRecError = 1;
		SPI_Count.HeadError = 0;
	}
	#endif

	if(Result == TRUE)
	{
		// 转存采样值
		set_smp_points(&(pBuf[5]));
		SPI_Count.Success++;
		api_ClrSysStatus(eSYS_STATUS_METERING_FAULT);
		api_ClrError(ERR_CHECK_5460_1);
		SPIResetConvert = 0;
	}
	else
	{
		api_SetSysStatus(eSYS_STATUS_METERING_FAULT);
		LED_SPI3Working=0;
	}

}

void SPI_WatchDog(void)
{
	DWORD wSPICount=0;
	#if(!WIN32)
	if(SPIRecError == 1)
	{
		api_SetError(ERR_CHECK_5460_1);
		if(SPIResetConvert < 5)//主要机制
		{
			gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
			nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);
			syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN15);
			exti_init(EXTI_15, EXTI_INTERRUPT, EXTI_TRIG_RISING);
			exti_interrupt_flag_clear(EXTI_15);
		}
		else//备用机制
		{
			gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
			while (gpio_input_bit_get(GPIOA, GPIO_PIN_15) != TRUE)
			{
				CLEAR_WATCH_DOG;
				wSPICount++;
				api_Delayus(5);
				if(wSPICount > 1000)//最多750次,有点冗余
				{
					return;
				}
			}
			MX_SPI3_Init();
			SPIResetCount++;//复位次数，永远不会清除
			SPIResetConvert++;//复位次数，数据正常即清除
			api_Delayus(1000);
			SPIRecError = 0;
		}
	}
	#endif
}
/*获取采样数据*/
BOOL api_GetSmpPoints(void** new_data_ptr)
{
	/* 读指针位置小于写指针位置,说明有数据需要处理指导读指针跟踪 */
	if((s_sv_rd_pos<s_sv_wr_pos)||(s_pass_tail))//读小于写指针，或者写满当前缓冲区了，
		//写满缓冲区瞬间，写指针复位，此时不能通过读小于写指针方式进行判断
	{
		*new_data_ptr = &SampBuf[s_sv_rd_pos];

		/* 下一个待读取点位置 */
		s_sv_rd_pos += READ_LENGTH;
		if(s_sv_rd_pos >= SAMP_BUFF_MAX_SIZE)//读完当前缓冲区，要把写满标志清除
		{
			s_sv_rd_pos = 0;
			s_pass_tail = FALSE;
		}
		/* 有数据 */
		return TRUE;
	}
	else
	{/* 无数据需要处理 */
		return FALSE;
	}
}

//通过读指针查找采样缓冲区对应采样点位置   采样点打上时标，读指针访问到此点实际就是采样点时间
DWORD get_R_pos_Time()
{
    DWORD pos;
    pos = (s_sv_rd_pos) ? (FramePosToTimPos(s_sv_rd_pos)) : (FramePosToTimPos(SAMP_BUFF_MAX_SIZE));//零说明是读到缓冲区最后一个地址了
    return SampTime[pos-1];//采样中断每个写指针都有时标
}


void ClearSpiData(void)
{
		static int nCount = 0;
		static int nOldSpi=-1;
	
	   if(nOldSpi != SPI_Count.Success)
		 {
				nOldSpi = SPI_Count.Success;
				nCount = 0;
		 }
		 else
		 {
					nCount++;
					if(nCount == 2)
					{
							nOldSpi = SPI_Count.Success;
							SysParaInit();
					}
		 }
}
//--------------------------------------------------
//功能描述:  算法软看门狗
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void Algr_WatchDog(void)
{
	static BYTE by5sFlag = 0;

	//10秒到
	if( api_GetTaskFlag(eTASK_SYSWATCH_ID,eFLAG_SECOND) == TRUE )
	{
		api_ClrTaskFlag(eTASK_SYSWATCH_ID, eFLAG_SECOND);
		by5sFlag++;

		if(by5sFlag >= 5)//5s检查一次
		{
			by5sFlag = 0;

			if(g_dwSoftWatDogRoot != g_dwSoftWatDog)
			{
				g_dwSoftWatDogRoot = g_dwSoftWatDog; //更新
				bySoftWatCount = 0;
			}
			else
			{
				bySoftWatCount ++;
				if(bySoftWatCount == 6)//正好30s
				{
					bySoftWatCount = 0;
					
					// rt_kprintf("g_dwSoftWatDogRoot=%d\r\n",g_dwSoftWatDogRoot);
					// rt_kprintf("g_dwSoftWatDog=%d\r\n",g_dwSoftWatDog);
					// rt_kprintf("bySoftWatCount=%d\r\n",bySoftWatCount);
					api_WriteSysUNMsg(RT_RESET_ALGROTHM);
					api_ResetCpu();
				}
			}
		}
	}
}
//--------------------------------------------------
//功能描述:  看门狗任务
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void api_WatchTask(void)
{
	SPI_WatchDog();
	Algr_WatchDog();
}

//--------------------------------------------------
//功能描述:  清除缓冲内容
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
void  api_ClearSampBuf( void )
{
	memset(&SampBuf[0],0,sizeof(SampBuf));
	memset(&SPI3DMAFrameBuf[0],0,sizeof(SPI3DMAFrameBuf));
	Init_Sample();
}