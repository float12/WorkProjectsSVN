//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		��ȪCPU����ʱ�ӽӿ��ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

#if (RTC_CHIP == RTC_HT_INCLOCK)


//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define	C_Toff			0x0000			//�¶�ƫ�üĴ���
#define	C_MCON01		0x2000			//����ϵ��01
#define	C_MCON23		0x0588			//����ϵ��23
#define	C_MCON45		0x4488			//����ϵ��45
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------


//-----------------------------------------------
//��������: �ϵ罫info flash�е�ϵ���ᵽrtc�����Ĵ�����
//
//����: 	��
//			
//                    
//����ֵ:  	��
//
//��ע:�Ժ���ڷ������棬ÿ����ҲҪЧ��һ��!!!!!!   
//-----------------------------------------------
void api_Load_RtcInfoData(void)
{
	BYTE	i;
	DWORD	chksum = 0;
	DWORD	toff;
	short	temp, code1;
	
	EnWr_WPREG();
	
	for(i=0; i<14; i++)
	{
		chksum += HT_INFO->DataArry[i];
	}
       
    if(chksum == HT_INFO->DataArry[14])		//������
	{		
		if (HT_RTC->DFAH    != HT_INFO->Muster.iDFAH)   HT_RTC->DFAH    = HT_INFO->Muster.iDFAH;
        if (HT_RTC->DFAL    != HT_INFO->Muster.iDFAL)   HT_RTC->DFAL    = HT_INFO->Muster.iDFAL;
        if (HT_RTC->DFBH    != HT_INFO->Muster.iDFBH)   HT_RTC->DFBH    = HT_INFO->Muster.iDFBH;
        if (HT_RTC->DFBL    != HT_INFO->Muster.iDFBL)   HT_RTC->DFBL    = HT_INFO->Muster.iDFBL;
        if (HT_RTC->DFCH    != HT_INFO->Muster.iDFCH)   HT_RTC->DFCH    = HT_INFO->Muster.iDFCH;
        if (HT_RTC->DFCL    != HT_INFO->Muster.iDFCL)   HT_RTC->DFCL    = HT_INFO->Muster.iDFCL;
        if (HT_RTC->DFDH    != HT_INFO->Muster.iDFDH)   HT_RTC->DFDH    = HT_INFO->Muster.iDFDH;
        if (HT_RTC->DFDL    != HT_INFO->Muster.iDFDL)   HT_RTC->DFDL    = HT_INFO->Muster.iDFDL;
        if (HT_RTC->DFEH    != HT_INFO->Muster.iDFEH)   HT_RTC->DFEH    = HT_INFO->Muster.iDFEH;
        if (HT_RTC->DFEL    != HT_INFO->Muster.iDFEL)   HT_RTC->DFEL    = HT_INFO->Muster.iDFEL;
        if (HT_RTC->Toff    != HT_INFO->Muster.iToff)   HT_RTC->Toff    = HT_INFO->Muster.iToff;
        if (HT_RTC->MCON01  != HT_INFO->Muster.iMCON01) HT_RTC->MCON01  = HT_INFO->Muster.iMCON01;
        if (HT_RTC->MCON23  != HT_INFO->Muster.iMCON23) HT_RTC->MCON23  = HT_INFO->Muster.iMCON23;
        if (HT_RTC->MCON45  != HT_INFO->Muster.iMCON45) HT_RTC->MCON45  = HT_INFO->Muster.iMCON45;

	}
    else//����ϵ��δ���ã���д��Ĭ��ֵ
    {
		if (HT_RTC->DFAH	!= TAB_DFx_K[0])	HT_RTC->DFAH	= TAB_DFx_K[0];
		if (HT_RTC->DFAL	!= TAB_DFx_K[1])	HT_RTC->DFAL	= TAB_DFx_K[1];
		if (HT_RTC->DFBH	!= TAB_DFx_K[2])	HT_RTC->DFBH	= TAB_DFx_K[2];
		if (HT_RTC->DFBL	!= TAB_DFx_K[3])	HT_RTC->DFBL	= TAB_DFx_K[3];
		if (HT_RTC->DFCH	!= TAB_DFx_K[4])	HT_RTC->DFCH	= TAB_DFx_K[4];
		if (HT_RTC->DFCL	!= TAB_DFx_K[5])	HT_RTC->DFCL	= TAB_DFx_K[5];
		if (HT_RTC->DFDH	!= TAB_DFx_K[6])	HT_RTC->DFDH	= TAB_DFx_K[6];
		if (HT_RTC->DFDL	!= TAB_DFx_K[7])	HT_RTC->DFDL	= TAB_DFx_K[7];
		if (HT_RTC->DFEH	!= TAB_DFx_K[8])	HT_RTC->DFEH	= TAB_DFx_K[8];
		if (HT_RTC->DFEL	!= TAB_DFx_K[9])	HT_RTC->DFEL	= TAB_DFx_K[9];

        toff = *(DWORD*)0x4012C;				//Toff��λ
        temp = *(short*)0x4015E;				//�¶�
        code1 = *(short*)0x4015C;				//TPS code
        if( ( (toff>0x0BB8) && (toff<0xF448) )
        	|| (temp < 2000) 
        	|| (temp > 3000)
        	|| (code1 < -7000) 
        	|| (code1 > -1000) )
        {
            toff = C_Toff;
        }
        if (HT_RTC->Toff	!= toff)		HT_RTC->Toff	= toff;
        if (HT_RTC->MCON01	!= C_MCON01)	HT_RTC->MCON01	= C_MCON01;
        if (HT_RTC->MCON23	!= C_MCON23)	HT_RTC->MCON23	= C_MCON23;
        if (HT_RTC->MCON45	!= C_MCON45)	HT_RTC->MCON45	= C_MCON45;
    }
    
    DisWr_WPREG();
}


//-----------------------------------------------
//��������: дinfo flash�е�����
//
//����: 	pBuff[in]	Ҫд�������
//			addr[in]	ƫ�Ƶ�ַ
//          len[in]     д�����ݳ���    
//����ֵ:  	��
//
//��ע:   	*info��      ��������ָ��
//          pBuff[0]  = ϵ��A��7λ
//          pBuff[1]  = ϵ��A��16λ
//          pBuff[2]  = ϵ��B��7λ
//          pBuff[3]  = ϵ��B��16λ
//          pBuff[4]  = ϵ��C��7λ
//          pBuff[5]  = ϵ��C��16λ
//          pBuff[6]  = ϵ��D��7λ
//          pBuff[7]  = ϵ��D��16λ
//          pBuff[8]  = ϵ��E��7λ
//          pBuff[9]  = ϵ��E��16λ
//          pBuff[10] = Toff�¶�У׼
//          pBuff[11] = MCON01����ϵ��01
//          pBuff[12] = MCON23����ϵ��23
//          pBuff[13] = MCON45����ϵ��45
//          pBuff[14] = �ۼӺ�; 
//-----------------------------------------------
void Prog_InfoData(const BYTE *pBuff, WORD addr, BYTE len)
{
	WORD	i;
	BYTE	info[1024];//6015--256�ֽ�1ҳ	6025--1k�ֽ�1ҳ
	
	//��ҳ��ȡ
	for (i=0; i<1024; i++)
	{
		info[i] = *((BYTE*)(HT_INFO_BASE +i));								
	}
	//����RTC����
	for (i=0; i<len; i++)
	{
		info[(addr & 0x03FF) + i] = pBuff[i];								
	}
	//����InfoData
	EnWr_WPREG();
	HT_CMU->FLASHLOCK = 0x7A68;					//unlock flash memory
	HT_CMU->INFOLOCK  = 0xF998;					//unlock information flash memory
	HT_CMU->FLASHCON  = 0x02;					//page erase
	*((DWORD*)HT_INFO_BASE) = 0xFFFFFFFF;		//data
	while (HT_CMU->FLASHCON & 0x04)				//FLASH_BSY
		;
	HT_CMU->FLASHCON  = 0x00;					//read only
	
	api_Delayms(2);
	//����InfoData
	HT_CMU->FLASHCON  = 0x01;					//word write
	for (i=0; i<1024; i++)
	{
		*((BYTE*)(HT_INFO_BASE + i)) = info[i];	//program word
		while (HT_CMU->FLASHCON  & 0x04)		//FLASH_BSY
		{
			__NOP();
		}
	}
	HT_CMU->FLASHLOCK = ~0x7A68;				//lock flash memory
	HT_CMU->INFOLOCK  = ~0xF998;				//lock information flash memory
	HT_CMU->FLASHCON  = 0x00;					//read only
	DisWr_WPREG();
	
	api_Delayms(2);
	
	api_Load_RtcInfoData();
}


//-----------------------------------------------
//��������: ��ȡ�¶�
//
//����: 	��
//			
//                    
//����ֵ:  	�¶�ֵ
//
//��ע:   
//-----------------------------------------------
signed short  ADC_TempVolt(void)
{
	signed short 	temp, toff;
	
	toff = HT_RTC->Toff;
	temp = HT_TBS->TMPDAT;
	temp -= toff;
	return temp;
}



//-----------------------------------------------
//��������: RTC���У׼
//
//����:		err[in]   ���������(ÿ������룬��λ0.001)  ���λΪ1��ʾ��ֵ
//						
//����ֵ:	TRUE/FALSE
//		   
//��ע:
//-----------------------------------------------
BOOL api_CaliRtcError( WORD err )
{
	BYTE	i, j, num;
	BYTE	info[60];
	TRtcTemperature RtcTemperature;
	DWORD	chksum;
	signed long	ratio[5];				//��ǰ����ϵ��
	signed short error, tps[3];
	signed long long	tmp64s;			//�м����
	double	FN, tmp;
	double	add[3];						//��������ϵ��
    double	TT13, TT23, T13, T23;

	//RTC���¶Ȳ���
	if( err & 0x8000 )
    {
    	error = (err & ~0x8000);
    	error *= (-1);
    }
    else
    {
    	error = err;
    }
    
    //RTC���¶Ȳ��� -30.0s/d ~ +30.0s/d
    if( (error<-30000) || (error>30000) )                                
    {
        return FALSE;
    }
	
	tps[0] = ADC_TempVolt();							//��ǰTpsCode
	for (i=0; i<3; i++)
	{
		if ( (tps[0]<=TAB_Temperature[i*2+0])
			&& (tps[0]>=TAB_Temperature[i*2+1]) )		//-50C~0C; +15C~+35C; +50C~+100C
		{
			break;
		}
	}
	if (i >= 3)
	{
		return FALSE;
	}
	j = i;												//��ǰ������
	num = 1;											//1����Ч��
	//�Ѳ��������ռ� //��ȡ�¶Ȳ�������
	if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == FALSE )
	{
		return FALSE;
	}

	for (i=0; i<3; i++)
	{
		if (i != j)										//��ͬ��
		{
			tps[num] = (RtcTemperature.Temperature[i*2] | (RtcTemperature.Temperature[i*2+1]<<8) );
			if ( (tps[num]<=TAB_Temperature[i*2+0])
				&& (tps[num]>=TAB_Temperature[i*2+1]) )	//-50C~0C; +15C~+35C; +50C~+100C
			{
				num++;									//��Ч��
			}
		}
		else											//ͬһ��
		{
			RtcTemperature.Temperature[i*2]   = tps[0];
			RtcTemperature.Temperature[i*2+1] = tps[0]>>8;
		}
	}
	
	//�����¶Ȳ�������
	if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == FALSE )
	{
		return FALSE;
	}
	
	//����InfoData
	ratio[0] = HT_RTC->DFAH<<16 | HT_RTC->DFAL;
	ratio[1] = HT_RTC->DFBH<<16 | HT_RTC->DFBL;
	ratio[2] = HT_RTC->DFCH<<16 | HT_RTC->DFCL;
	ratio[3] = HT_RTC->DFDH<<16 | HT_RTC->DFDL;
	ratio[4] = HT_RTC->DFEH<<16 | HT_RTC->DFEL;

	for (i=0; i<5; i++)
	{
		if (ratio[i] & 0x00400000)
		{
			ratio[i] |= 0xFF800000;
		}
	}
	
	tmp64s  =  ratio[4];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[3];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[2];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[1];
	tmp64s *=  tps[0];
	tmp64s >>= 16;
	
	tmp64s +=  ratio[0];
	tmp64s +=  2;
	tmp64s >>= 2;
	
	FN  = (double)tmp64s/512.0 +32768;
	tmp = error / 1000.0;									//ת��Ϊdouble��
	FN  = FN*tmp/(86400-tmp);
	//�������
	if (num == 3)											//3�����
	{
		TT13 = (double)(tps[0]+tps[2]) * (tps[0]-tps[2]);	//dTps[0]^2 - dTps[2]^2
		TT23 = (double)(tps[1]+tps[2]) * (tps[1]-tps[2]);	//dTps[1]^2 - dTps[2]^2
		
		T13  = (double)(tps[0]-tps[2]);						//dTps[0] - dTps[2]
		T23  = (double)(tps[1]-tps[2]);						//dTps[0] - dTps[2]
		
		TT23 *= T13;
		TT23 /= T23;
		TT23 = TT13 - TT23;
		
		add[2] = FN/TT23;									//2��ϵ������
		add[1] = (FN-add[2]*TT13)/T13;						//1��ϵ������
		add[0] = FN -add[2]*tps[0]*tps[0] -add[1]*tps[0];	//0��ϵ������

		add[2] = add[2] *128 *16 *65536 *65536;
		add[1] = add[1] *128 *16 *65536;
		add[0] = add[0] *128 *16;
	}
	else if (num == 2)										//2�����
	{
		T23  = (double)(tps[1]-tps[0]);
		
		add[1] = -FN/T23;
		add[0] = -add[1]*tps[1];
		
		add[2] = 0;
		add[1] = add[1] *128 *16 *65536;
		add[0] = add[0] *128 *16;
	}
	else													//1�����
	{
		add[2] = 0;
		add[1] = 0;
		add[0] = FN*128*16;
	}
    ratio[0] += (signed long)add[0];						//0�β���ϵ��
    ratio[1] += (signed long)add[1];						//1�β���ϵ��
    ratio[2] += (signed long)add[2];						//2�β���ϵ��
	//д��InfoData����
	chksum = 0;
	for (i=0; i<5; i++)
	{
		chksum += (ratio[i]>>16) & 0x007F;
		chksum += ratio[i]       & 0xFFFF;
	}
	chksum += HT_RTC->Toff;
	chksum += HT_RTC->MCON01;
	chksum += HT_RTC->MCON23;
	chksum += HT_RTC->MCON45;
	
	for (i=0; i<5; i++)
	{
		info[i*8+0]  = (ratio[i]>>16) & 0x7F;
		info[i*8+1]  = 0x00;
		info[i*8+2]  = 0x00;
		info[i*8+3]  = 0x00;
		info[i*8+4]  = ratio[i]       & 0xFF;
		info[i*8+5]  = (ratio[i]>>8)  & 0xFF;
		info[i*8+6]  = 0x00;
		info[i*8+7]  = 0x00;
	}
	info[40]  = HT_RTC->Toff & 0xFF;
	info[41]  = (HT_RTC->Toff>>8) & 0xFF;
	info[42]  = 0x00;
	info[43]  = 0x00;
	info[44]  = HT_RTC->MCON01 & 0xFF;
	info[45]  = (HT_RTC->MCON01>>8) & 0xFF;
	info[46]  = 0x00;
	info[47]  = 0x00;
	info[48]  = HT_RTC->MCON23 & 0xFF;
	info[49]  = (HT_RTC->MCON23>>8) & 0xFF;
	info[50]  = 0x00;
	info[51]  = 0x00;
	info[52]  = HT_RTC->MCON45 & 0xFF;
	info[53]  = (HT_RTC->MCON45>>8) & 0xFF;
	info[54]  = 0x00;
	info[55]  = 0x00;
	
	info[56]  = chksum & 0xFF;
	info[57]  = (chksum>>8) & 0xFF;
	info[58]  = (chksum>>16) & 0xFF;
	info[59]  = (chksum>>24) & 0xFF;
	
	Prog_InfoData(&info[0], 0x0104, 60);
	
	return TRUE;
}


//-----------------------------------------------
//��������: RTCʱ�������������
//
//����:		Type[in]   ���������(ÿ������룬��λ0.001)  ���λΪ1��ʾ��ֵ
//				Type -- 0			1HZ�������
//				Type -- ��Ϊ��	��ֹ���������					
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void api_InitOutClockPuls(BYTE Type)
{
    if( Type == 0 )
    {
      	//�ɸ�Ƶ�����õ���1Hz
      	HT_RTC->RTCCON = 0x0006;
    }
    else
    {
      	//�ر����������
      	HT_RTC->RTCCON = 0;
    }
}


//-----------------------------------------------
//��������: д�ⲿʱ��
//
//����: 
//			t[in]		Ҫд���ʱ��ָ��,ʱ�ӽṹ�ڵ�����Ҫ����hex��
//                    
//����ֵ:  	TRUE:��ȷд��  FALSE:����д��
//
//��ע:   
//-----------------------------------------------
BOOL WriteOutClock(TRealTimer * t)
{
	DWORD tdw;
	
    tdw = api_CalcInTimeRelativeSec( t );
    if( tdw == ILLEGAL_VALUE_8F )
    {
    	return FALSE;
    }
    
    tdw = ((tdw/60/1440)+6)%7;//������Ϊ0ʱ����ʾ����
    if( tdw == 0 )//6025 HT_RTC->WEEKR�Ĵ�����������ֵ��Χ1~7����Ӧ��һ~����
    {
        tdw = 7;
    }

    
    EnWr_WPREG();
	HT_RTC->RTCWR = 0x0000;
	
	HT_RTC->YEARR = (t->wYear%100);//����20XX Hex�� �����ꡢ�¡��ա�ʱ���֡��롢�ܵ�˳��
	HT_RTC->MONTHR= (t->Mon);
	HT_RTC->DAYR  = (t->Day);                   
	HT_RTC->HOURR = (t->Hour);
	HT_RTC->MINR  = (t->Min);
	HT_RTC->SECR  = (t->Sec);
	HT_RTC->WEEKR = tdw; 
    
	HT_RTC->RTCWR = 0x0001;
//	while (HT_RTC->RTCWR & 0x0001)
//		;
	DisWr_WPREG();

	api_WriteFreeEvent( EVENT_WRITE_HARD_TIME, 0 );
	
	return TRUE;
}


//-----------------------------------------------
//��������: ���ⲿʱ�ӵĵײ㺯����ģ���ں���
//
//����: 
//			Type[in]		
//                    
//����ֵ:  	TRUE:��ȷд��  FALSE:����д��
//
//��ע:   
//-----------------------------------------------
BOOL ReadOutClockHara( WORD Type )
{
    TRealTimer TmpRealTimer;
    //������ͬ��
	BYTE SecSync;
	
    HT_RTC->RTCRD = 0x0001;
	while (HT_RTC->RTCRD & 0x0001)
		;
    
	if(Type == 1)
	{
        SecSync=HT_RTC->SECR;
		return TRUE;
	}
	
	TmpRealTimer.wYear = 2000+(HT_RTC->YEARR);
	TmpRealTimer.Mon = (HT_RTC->MONTHR);
	TmpRealTimer.Day = (HT_RTC->DAYR);
	TmpRealTimer.Hour = (HT_RTC->HOURR);
	TmpRealTimer.Min = (HT_RTC->MINR);
	TmpRealTimer.Sec = (HT_RTC->SECR);

    DISABLE_CLOCK_INT;
    memcpy((BYTE*)&RealTimer,(BYTE*)&TmpRealTimer, sizeof(TRealTimer) );
    ENABLE_CLOCK_INT;
    
    if( api_CheckClock( (TRealTimer*)&TmpRealTimer ) == TRUE )
	{	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//---------------------------------------------------------------
//��������: �ϵ縴λRTCģ��
//
//����: 	��
//                   
//����ֵ:   ��
//
//��ע:  HT6025H ���VRTC��Դ��0.7V���� �ϵ���ܵ���RTCģ�鸴λʧ��
//		���ֽϴ���ռ�ʱ��� ����ϵ�ǰ������Ƿѹ���߲����˵�����ϵ縴λ
// 		�ϵ���λһ��RTC 
// 		��λRTC��Ӱ��RTC��TBS��ؼĴ��� ��Ҫ�������ã�������
//---------------------------------------------------------------
void PowerOnResetRTC( void )
{
	//����ϵ�ǰ���Ƿѹ���߲�����LBOR����POR��λ
	if ((api_GetRunHardFlag( eRUN_HARD_CLOCK_BAT_LOW ) == TRUE)
		|| (HT_PMU->RSTSTA&0x0003))
	{
		HT_RTC->RTCRSTSET = 0xAAAA; //RTC ��λ
		HT_RTC->RTCRSTSET = 0x5555;
		api_Delayms( 2 ); //�ȴ���λ��� ��Ȫ������1ms��ʱ �˴��ŵ�2ms
	}
}

//-----------------------------------------------
//��������: �ⲿʱ�ӳ�ʼ����ģ���ں���
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void InitRtc( void )
{
	PowerOnResetRTC();	//��λ������������RTC��TBS��ؼĴ��� �ú��������������λ��
	
	api_Load_RtcInfoData();
	
	api_MCU_RTCInt( );		//���� �� ʱ�ж�
	
	api_InitOutClockPuls(0);	//��ʱ��оƬ�������
}

//-----------------------------------------------
//��������: ��Լ��дrtc��Уϵ��
//
//����: 	Type[in]	READ/WRITE
//			Buf[in/out] ����                  
//����ֵ:  	Buf����
//
//��ע:   
//-----------------------------------------------
BYTE api_ProcRtcPara(BYTE Type, BYTE *Buf)
{
	if( Type == READ )
	{
		if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), Buf) == FALSE )
		{
			return 0;
		}
	}
	else
	{
		if( api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), Buf) == FALSE )
		{
			return 0;
		}
	}
	
	return 6;
}

//---------------------------------------------------------------
//��������: ��дInfoFlash��RTC��A��B��C��D��EУ��ϵ��
//
//����: 	pBuff: У��ϵ������
//                   
//����ֵ:  	pBuff����
//
//��ע:   
//---------------------------------------------------------------
void ProcInfoRtcAdjustPara( BYTE Type, BYTE *pBuff )
{
	BYTE i = 0;
	BYTE Info[60];
	DWORD chksum = 0;
	
	if( Type == WRITE )
	{
		//����A��B��C��D��Eϵ��
		for(i = 0; i < 5; i++)
		{
			//��λ
			Info[i*8+0] = pBuff[i*4+2] & 0x7F;
			Info[i*8+1] = 0x00;
			Info[i*8+2] = 0x00;
			Info[i*8+3] = 0x00;
			//��λ
			Info[i*8+4] = pBuff[i*4+0] & 0xFF;
			Info[i*8+5] = pBuff[i*4+1] & 0xFF;
			Info[i*8+6] = 0x00;
			Info[i*8+7] = 0x00;
		}
		//�����¶�ƫ��Ϳ�����
		for(i = 0; i < 4; i++)
		{
			Info[i*4+40] = HT_INFO->DataArry[i+10] & 0xFF;
			Info[i*4+41] = (HT_INFO->DataArry[i+10] >> 8) & 0xFF;
			Info[i*4+42] = 0x00;
			Info[i*4+43] = 0x00;
		}
		
		//����CheckSumУ���
		for (i = 0; i < 5; i++)
		{
			//ϵ����λ
			chksum += Info[i*8+0];
			//ϵ����λ
			chksum += ((Info[i*8+5] << 8) + Info[i*8+4]);
		}
		chksum += HT_INFO->DataArry[10];//iToff
		chksum += HT_INFO->DataArry[11];//iMCON01
		chksum += HT_INFO->DataArry[12];//iMCON23
		chksum += HT_INFO->DataArry[13];//iMCON45
		
		Info[56] = chksum & 0xFF;
		Info[57] = (chksum>>8) & 0xFF;
		Info[58] = (chksum>>16) & 0xFF;
		Info[59] = (chksum>>24) & 0xFF;
		
		//д��InfoFlash
		Prog_InfoData( Info, 0x0104, sizeof(Info) );
	}
	else
	{
		//��ȡA��B��C��D��Eϵ��
		for (i = 0; i < 5; i++)
		{
			pBuff[i*4+0] = HT_INFO->DataArry[i*2+1] & 0xFF;
			pBuff[i*4+1] = (HT_INFO->DataArry[i*2+1] >> 8) & 0xFF;
			pBuff[i*4+2] = HT_INFO->DataArry[i*2+0] & 0xFF;
			pBuff[i*4+3] = (HT_INFO->DataArry[i*2+0] >> 8) & 0xFF;
		}
	}
}

//---------------------------------------------------------------
//��������: дInfoFlash��RTCУ����������У���
//
//����: 	��
//                   
//����ֵ:  	��
//
//��ע:   У��ʹ�������Ĭ�ϲ���, ��ʹ��InfoFlash�еĲ���
//---------------------------------------------------------------
void WriteInfoRTCErrorCheckSum( void )
{
	BYTE i = 0;
	BYTE Info[4];
	DWORD chksum = 0;
	
	//������ȷУ���
	for(i = 0; i < 14; i++)
	{
		chksum += HT_INFO->DataArry[i];
	}
	//У��ͼ�1
	chksum += 1;
	Info[0] = chksum & 0xFF;
	Info[1] = (chksum>>8) & 0xFF;
	Info[2] = (chksum>>16) & 0xFF;
	Info[3] = (chksum>>24) & 0xFF;
	//д�����У���
	Prog_InfoData( Info, 0x013C, sizeof(Info) );
}

//-----------------------------------------------
//��������: ʱ�Ӳ�������ʼ��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע: �岹��ϵ��  
//-----------------------------------------------
void api_FactoryInitRtc(void)
{
	TRtcTemperature RtcTemperature;
	
	if( api_VReadSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature) == TRUE )
	{
		return;
	}
	
	//��ʼ���¶�ֵΪ�Ƿ�ֵ, ��֤RTC��������Ϊ1�����
	memset( RtcTemperature.Temperature, 0x00, sizeof(RtcTemperature.Temperature) );
	
	api_VWriteSafeMem(GET_STRUCT_ADDR(TSafeMem,RtcSafeRom.RtcTemperature), sizeof(TRtcTemperature), RtcTemperature.Temperature);
}

#endif//#if (RTC_CHIP == RTC_HT_INCLOCK)
