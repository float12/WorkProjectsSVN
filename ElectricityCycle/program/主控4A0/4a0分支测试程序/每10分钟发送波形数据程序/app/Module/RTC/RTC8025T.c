#include "AllHead.h"


#if (RTC_CHIP == RTC_CHIP_8025T)

//用于秒同步
BYTE SecSync;

#define I2CTIMES	10

void Delay( WORD m )
{
	while(m-- > 0);
}


void Start(void)
{
	CLOCK_SCL_H;
	Delay(10);
	CLOCK_SDA_H;
	Delay(10);
	CLOCK_SDA_L;
	Delay(10);
	CLOCK_SCL_L;
	Delay(10);
}

void Stop(void)
{
	CLOCK_SDA_L;
	Delay(10);
	CLOCK_SCL_H;
	Delay(10);
	CLOCK_SDA_H;
	Delay(10);
}

void Send_Byte(BYTE Byte)
{
	int i;
	for (i=0;i<8;i++)
	{
		if(Byte & 0x80)
		{
			CLOCK_SDA_H;
		}
		else
		{
			CLOCK_SDA_L; 
		}
		Delay(5);
		CLOCK_SCL_H;
		Delay(10);
		CLOCK_SCL_L;
		Delay(5);
		Byte = Byte << 1;
	}
	
	CLOCK_I2C_SDA_PORT_IN;
}

WORD Ack(void)
{
	WORD Ack_Flag;
	
	CLOCK_SDA_H;
	Delay(10);
	CLOCK_SCL_H;
	Ack_Flag = TRUE;
	Delay(10);
	if(CLOCK_I2C_SDA_PORT_DATA_IN)
	{
			Ack_Flag = FALSE;
	}
	CLOCK_SCL_L;
	Delay(10);
	return Ack_Flag;
}

void NOAck(void)
{
	CLOCK_SDA_H;
	Delay(10);
	CLOCK_SCL_H;
	Delay(10);
	CLOCK_SCL_L;
	Delay(10);
	CLOCK_SDA_L;
	Delay(10);
}

void SendAck(void)
{
	CLOCK_SDA_L;
	Delay(10);
	CLOCK_SCL_H;
	Delay(10);
	CLOCK_SCL_L;
	Delay(10);
	CLOCK_SDA_H;
	Delay(10);
	
}

WORD  Receive_Byte(void)
{
	WORD R_word;
	int j;
	R_word = 0x00;
	
	CLOCK_SDA_H;
	Delay(5);
	for(j=0;j<8;j++)
	{
		CLOCK_SCL_H;
		Delay(5);
		
		if(CLOCK_I2C_SDA_PORT_DATA_IN)
		{
			R_word |= 0x01;
		}
		else
		{
			R_word &= 0xFE;
		}
		Delay(5);
		CLOCK_SCL_L;
		Delay(10);
		R_word = R_word << 1;
	}
	
	R_word >>= 1; 
	return R_word;
}

void i2c_start_set(void)
{
	CLOCK_SDA_H;
	CLOCK_SCL_H;
	Delay(150);
	
}
void i2c_stop_set(void)
{
	CLOCK_SCL_L;
	
	Delay(150);
}
// Num---要写入数据的个数
// Buf---写入数据的起始地址
// Addr ---RTC起始寄存器地址
// 返回 TRUE   1-----成功
//      FALSE  0-----失败
WORD i2c_write_RTC(BYTE Addr,BYTE Num,BYTE *Buf)
{
	BYTE i,Flag;
	//BYTE IntStatus;
	
	//IntStatus = api_splx(0);
	Flag = TRUE;
	i2c_start_set();
	for(i=0;i<I2CTIMES;i++)
	{
    
		Start();
		Delay(20);
		Send_Byte(0x64);//write a byte :slave device
		if(Ack()==FALSE)
			continue;
		Send_Byte(Addr);// write a byte :address
		if(Ack()==TRUE)
			break;
    		
	}
	
	if(i >= (I2CTIMES-1))
	{
		Flag = FALSE;
	}
	
	for(i = 0;i<Num;i++)
	{
		Send_Byte(Buf[i]);
		if(Ack()==FALSE)
		{
			Flag = FALSE;
		}
			
	}
		
	Stop();
	i2c_stop_set();
	
	//api_splx(IntStatus);
	
	return Flag;
}

// Num---要读取数据的个数
// Buf---读取数据的起始地址
// Addr ---RTC起始寄存器地址
void i2c_read_RTC(BYTE Addr,BYTE Num,BYTE *Buf)
{
	BYTE i;
	//BYTE IntStatus;
	
	//IntStatus = api_splx(0);
	i2c_start_set();
	for(i=0;i<I2CTIMES;i++)
	{
		Start();
		Send_Byte(0x64);
		
		if(Ack()==FALSE)
			continue;
		Send_Byte(Addr);
		if(Ack()==FALSE)
			continue;
		Start();
		Send_Byte(0x65);
		if(Ack()==TRUE)
			break;
		}
 	
 	for(i=0;i<Num;i++)
 	{
		Buf[i] = Receive_Byte();
		
		if(i<(Num-1))
		{
			SendAck();
		}
	}
	NOAck();
	Stop();
	i2c_stop_set();
	//api_splx(IntStatus);
}


//初始化秒脉冲输出方式
//Type -- 0			1HZ脉冲输出
//Type -- 不为零	禁止秒脉冲输出
void api_InitOutClockPuls(BYTE Type)
{
	BYTE temp;
	BYTE Status;
	i2c_read_RTC(0x0f,1,&temp);
	if(temp == 0xff)
	{
		temp = 0x60;
	}
	if( Type == 0 )		//打开秒脉冲输出
	{
		temp |= 0x20;
	}
	else				//关闭秒脉冲输出
	{
		temp &= ~(0x20);
	}
	Status = i2c_write_RTC(0x0f,1,&temp);
	
	api_Delayms(10);
	i2c_read_RTC(0x0d,1,&temp);

	temp &= 0xF3;
	i2c_write_RTC(0x0d,1,&temp);
}

//要一次性写入，不用中断方式
//输入t为BCD码方式
BOOL WriteOutClock(TRealTimer * t)
{
	WORD Flag;
	BYTE tempbuf[16];
	DWORD tdw;
	
    tdw = api_CalcInTimeRelativeSec( t );
    if( tdw == ILLEGAL_VALUE_8F )
    {
    	return FALSE;
    }

	tempbuf[13] = 0x00; 
	tempbuf[14] = 0x00;
	
	if( GlobalVariable.g_byMultiFunPortType == eCLOCK_PULS_OUTPUT )
	{
		tempbuf[15] = 0x60;//0110 0000 bit7，bit6 温补间隔2S，bit5允许秒中断产生  
	}
	else
	{
		tempbuf[15] = 0x40;//0110 0000 bit7，bit6 温补间隔2S，bit5允许秒中断产生  
	}
	
	//memcpy((BYTE*)&tempbuf[0],(BYTE*)t,7 );
	tempbuf[0] = lib_BBinToBCD( t->Sec );
	tempbuf[1] = lib_BBinToBCD( t->Min );
	tempbuf[2] = lib_BBinToBCD( t->Hour );
	tempbuf[3] = ((tdw/60/1440)+6)%7;
	tempbuf[4] = lib_BBinToBCD( t->Day );
	tempbuf[5] = lib_BBinToBCD( t->Mon );
	tempbuf[6] = lib_BBinToBCD( t->wYear%100 );
	
	if( tempbuf[3] < 0x07 )
	{
		tempbuf[3] = 0x01 << tempbuf[3];
	}

	Flag = i2c_write_RTC(0x00,16,tempbuf);

	api_WriteFreeEvent( EVENT_WRITE_HARD_TIME, 0 );
		
	return Flag;
}


BOOL ReadOutClockHara( WORD Type )
{
	BYTE TempBuf[7];
	TRealTimer TmpRealTimer;
	
	if(Type == 1)
	{
		i2c_read_RTC(0x00,1,&SecSync);
		return TRUE;
	}
	
	i2c_read_RTC(0x00,7,TempBuf);
	
	switch( TempBuf[3] )
	{
	case 0x01:
		TempBuf[3] = 0x00;
		break;
	case 0x02:
		TempBuf[3] = 0x01;
		break;
	case 0x04:
		TempBuf[3] = 0x02;
		break;
	case 0x08:
		TempBuf[3] = 0x03;
		break;
	case 0x10:
		TempBuf[3] = 0x04;
		break;
	case 0x20:
		TempBuf[3] = 0x05;
		break;
	case 0x40:
		TempBuf[3] = 0x06;
		break;
	default:
		break;
	}
	
	TmpRealTimer.Sec = lib_BBCDToBin( TempBuf[0] );
	TmpRealTimer.Min = lib_BBCDToBin( TempBuf[1] );
	TmpRealTimer.Hour = lib_BBCDToBin( TempBuf[2] );
	TmpRealTimer.Day = lib_BBCDToBin( TempBuf[4] );
	TmpRealTimer.Mon = lib_BBCDToBin( TempBuf[5] );
	TmpRealTimer.wYear = lib_BBCDToBin( TempBuf[6] );
	TmpRealTimer.wYear += 2000;
	
    DISABLE_CLOCK_INT;
    memcpy( (BYTE*)&RealTimer, (BYTE*)&TmpRealTimer, sizeof(TRealTimer) );
    ENABLE_CLOCK_INT;
    
	if( api_CheckClock( &TmpRealTimer ) == TRUE )
	{		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void InitRtc( void )
{
	api_InitOutClockPuls( 0 );
}


//空函数，没有用
BOOL api_CaliRtcError( WORD err )
{
	return FALSE;
}


//-----------------------------------------------
//函数功能: 时钟补偿电表初始化
//
//参数: 	无
//                    
//返回值:  	无
//
//备注: 清补偿系数  
//-----------------------------------------------
void api_FactoryInitRtc(void)
{
	
}

#endif//#if (RTC_CHIP == RTC_CHIP_8025T)
