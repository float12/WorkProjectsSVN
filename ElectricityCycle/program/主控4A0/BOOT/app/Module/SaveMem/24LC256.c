//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.11
//描述		eeprom 24LC256 的驱动文件，及模拟IIC驱动文件
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "SaveMem.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define I2CTIMES		6
#if( MASTER_MEMORY_CHIP == CHIP_24LC256 )
	#define PAGE_SIZE		64
#elif( MASTER_MEMORY_CHIP == CHIP_24LC512 )
	#define PAGE_SIZE		64
#endif

//IIC通信延时宏定义
#define IIC_DELAY	{__NOP();__NOP();__NOP();__NOP();__NOP();} //5个NOP，iic时钟频率约182K

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


// EEPROM 写入保护管脚 有管脚连接，但软件一直置为写保护无效
#define ENABLE_WRITE_PROTECT1			;
#define DISABLE_WRITE_PROTECT1			;
#define ENABLE_WRITE_PROTECT2			;
#define DISABLE_WRITE_PROTECT2			;
//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
static BYTE g_byEEErrTimes[2];	//EEPROM存储器故障次数
//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------
static BYTE Get24LC256DevAddr( BYTE ChipNo );
static WORD WriteXX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf);

//-----------------------------------------------
//				函数定义
//-----------------------------------------------

#if( SEL_24LC256_COMMUNICATION_MOD == IIC_SIM )
//-----------------------------------------------
//函数功能: IIC的延时函数，内部函数
//
//参数: 	
//          m:延时参数，没有标定具体延时是多少          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void Delay24LC256IIC( WORD m )
{  
	while((m) != 0)
  	{
    	m >>= 3;
  	}
}


//-----------------------------------------------
//函数功能: 发IIC START的函数，内部函数
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void Start24LC256IIC(void)
{
	EEPROM_SDA_H;
	EEPROM_SCL_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SDA_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SDA_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
}


//-----------------------------------------------
//函数功能: 发IIC STOP的函数，内部函数
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void Stop24LC256IIC(void)
{
	EEPROM_SDA_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SDA_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
}


//-----------------------------------------------
//函数功能: 向IIC发送一个字节，内部函数
//
//参数: 	
//          Byte：要发送的数据
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void Send_Byte24LC256IIC(BYTE Byte)
{
	WORD i;
	
	for (i=0;i<8;i++)
	{
		if(Byte & 0x80)
		{
			EEPROM_SDA_H;
		}
		else
		{
			EEPROM_SDA_L;
		}

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(2);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)
	
		EEPROM_SCL_H;

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(4);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)
	
		EEPROM_SCL_L;

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(2);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)
	
		Byte = Byte << 1;
	}
}


//-----------------------------------------------
//函数功能: 查询ACK，内部函数
//
//参数: 	无
//          
//返回值: 	TRUE:检测到了ACK	FALSE:没有检测到ACK
//
//备注:   
//-----------------------------------------------
static WORD Ack24LC256IIC(void)
{
	WORD Ack_Flag;
	BYTE i=5;
	
	EEPROM_I2C_SDA_PORT_IN;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_H;
	Ack_Flag = TRUE;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	while((i--)&&EEPROM_I2C_SDA_PORT_DATA_IN);
	if (i==0)
	{
		Ack_Flag = FALSE;
	}
	EEPROM_SCL_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	return Ack_Flag;
}


//-----------------------------------------------
//函数功能: 发送NOACK，内部函数
//
//参数: 	无
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void NOAck24LC256IIC(void)
{
	EEPROM_SDA_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)

	EEPROM_SDA_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
}


//-----------------------------------------------
//函数功能: 发送ACK，内部函数
//
//参数: 	无
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void SendAck24LC256IIC(void)
{
	EEPROM_SDA_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SCL_L;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	EEPROM_SDA_H;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(4);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
}


//-----------------------------------------------
//函数功能: IIC接收一个字节函数，内部函数
//
//参数: 	无
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static WORD Receive_Byte24LC256IIC(void)
{
	WORD R_word;
	WORD j;
	
	R_word = 0x00;

	EEPROM_I2C_SDA_PORT_IN;

	#if(SPD_MCU == SPD_22000K)
	Delay24LC256IIC(2);
	#elif(SPD_MCU == SPD_11000K)
	IIC_DELAY;
	#endif	//#if(SPD_MCU == SPD_22000K)
	
	for(j=0;j<8;j++)
	{
		EEPROM_SCL_H;

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(2);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)

		if (EEPROM_I2C_SDA_PORT_DATA_IN)
		{
			R_word |= 0x01;
		}
		else
		{
			R_word &= 0xFE;
		}

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(2);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)
	
		EEPROM_SCL_L;

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(4);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)
	
		R_word = R_word << 1;
	}
	R_word >>= 1;
	return R_word;
}


//-----------------------------------------------
//函数功能: IIC开始，内部函数
//
//参数: 	无
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void i2c_start_set24LC256IIC(void)
{
	EEPROM_SDA_H;
	EEPROM_SCL_H;
	Delay24LC256IIC(60);
}


//-----------------------------------------------
//函数功能: IIC结束，内部函数
//
//参数: 	无
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void i2c_stop_set24LC256IIC(void)
{
	Delay24LC256IIC(60);
}


//-----------------------------------------------
//函数功能: IIC写数据函数，内部函数
//
//参数: 	
//			I2CNo[in]		选择写入哪个芯片
//			Addr[in]		要写入的起始地址
//			Num[in]			要写入的数据长度
//			Buf[in]			要写入的数据
//          
//返回值: 	TRUE:写入正确	FALSE：写入错误
//
//备注:   
//-----------------------------------------------
static WORD I2CWriteDev(BYTE I2CNo, WORD Addr, WORD Num, BYTE *Buf)
{
	BYTE i,Flag, DevAddr;

	// 得到设备地址
	DevAddr = Get24LC256DevAddr(I2CNo);

	if( DevAddr == 0xff )
	{
		return FALSE;
	}

	Flag = TRUE;
	i2c_start_set24LC256IIC();

	for(i=0;i<I2CTIMES;i++)
	{
		Start24LC256IIC();

		#if(SPD_MCU == SPD_22000K)
		Delay24LC256IIC(4);
		#elif(SPD_MCU == SPD_11000K)
		IIC_DELAY;
		#endif	//#if(SPD_MCU == SPD_22000K)

		// 操作地址
		Send_Byte24LC256IIC(DevAddr);
		if(Ack24LC256IIC()==FALSE)
		{
			continue;
		}


		Send_Byte24LC256IIC( (BYTE)((Addr>>8)&0xff) );
		if(Ack24LC256IIC()!=TRUE)
		{
			//处理地址高字节发送不应答
			continue;
		}

		Send_Byte24LC256IIC( (BYTE)(Addr&0xff) );
		if(Ack24LC256IIC()==TRUE)
		{
			break;
		}
	}

	if(i >= (I2CTIMES-1))
	{
		Flag = FALSE;
	}

	for(i=0; i<Num; i++)
	{
		Send_Byte24LC256IIC(Buf[i]);
		if(Ack24LC256IIC()==FALSE)
		{
			Flag = FALSE;
		}
	}

	Stop24LC256IIC();
	i2c_stop_set24LC256IIC();
    
	return Flag;
}


//-----------------------------------------------
//函数功能: IIC读数据函数，内部函数
//
//参数: 	
//			I2CNo[in]		选择读出哪个芯片
//			Addr[in]		要读出的起始地址
//			Num[in]			要读出的数据长度
//			Buf[in/out]		要读出的数据
//          
//返回值: 	TRUE:读出正确	FALSE：读出错误
//
//备注:   
//-----------------------------------------------
static WORD I2CReadDev(BYTE I2CNo, WORD Addr, WORD Num,BYTE *Buf)
{
	BYTE DevAddr;
	BOOL Result;
	WORD i;

	Result = TRUE;

	// 得到设备地址
	DevAddr = Get24LC256DevAddr(I2CNo);

	if( DevAddr == 0xff )
	{
		return FALSE;
	}

	i2c_start_set24LC256IIC();

	for(i=0;i<I2CTIMES;i++)
	{
		CLEAR_WATCH_DOG;
		Start24LC256IIC();

		// 写数据
		Send_Byte24LC256IIC(DevAddr);

		if(Ack24LC256IIC()==FALSE)
		{
			continue;
		}

		Send_Byte24LC256IIC( (BYTE)((Addr>>8)&0xff) );

		if(Ack24LC256IIC()==FALSE)
		{
			continue;
		}

		Send_Byte24LC256IIC( (BYTE)(Addr&0xff) );
		if(Ack24LC256IIC()==FALSE)
		{
			continue;
		}

		Start24LC256IIC();

		// 读数据
		Send_Byte24LC256IIC(DevAddr + 1);
		if(Ack24LC256IIC()==TRUE)
		{
			break;
		}
 	}

 	if( i >= I2CTIMES )
 	{
 		Result = FALSE;
 	}

	if (Result == TRUE)
	{
		for(i=0; i<Num; i++)
		{
			// 取数据
			Buf[i] = Receive_Byte24LC256IIC();

			if(i<(Num-1))
			{
				SendAck24LC256IIC();
			}
		}

		NOAck24LC256IIC();
	}

	Stop24LC256IIC();

	i2c_stop_set24LC256IIC();

	return Result;
}

#endif//#if( SEL_24LC256_COMMUNICATION_MOD == IIC_SIM )


//--------------------------------------------------------------------------------
// 以下是24LC256的存储驱动程序
//--------------------------------------------------------------------------------
//-----------------------------------------------
//函数功能: 获取IIC设备的片选地址，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//          
//返回值: 	IIC设备的片选地址
//
//备注:   
//-----------------------------------------------
static BYTE Get24LC256DevAddr( BYTE ChipNo )
{
	if( ChipNo == CS_SPI_256401 )
	{
		return MEM_24LC25601_DEVADDR;
	}
	else if( ChipNo == CS_SPI_256402 )
	{
		return MEM_24LC25602_DEVADDR;
	}
	else
	{
		return 0xff;
	}
}


//-----------------------------------------------
//函数功能: 从IIC设备读数据，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//			Addr[in]		读数据的起始
//			Len[in]			读数据的长度
//			pBuf[in/out]	读数据的缓冲
//          
//返回值: 	TRUE:读成功		FALSE:读失败
//
//备注:   
//-----------------------------------------------
static WORD ReadXX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
  	BYTE Result,i;

	Result = TRUE;

	// 判断写入的地址是否在24LC256的地址范围之内
	ASSERT( (Addr+Len) <= SINGLE_CHIP_SIZE, 1 );
	if( (Addr+Len) > SINGLE_CHIP_SIZE )
	{
		return FALSE;
	}
	
	#if( SLAVE_MEM_CHIP == CHIP_NO )
	if( ChipNo == CS_SPI_256402 )
	{
		Addr += SINGLE_CHIP_SIZE;
		ChipNo = CS_SPI_256401;
	}
	#endif

	// 读数据时不再打开写允许功能确保在读数据的时候不会产生误写操作
	for( i=0; i<3; i++ )
	{
		CLEAR_WATCH_DOG;
		if( I2CReadDev(ChipNo, Addr, Len, pBuf) == TRUE )
		{
			break;
		}

        CLEAR_WATCH_DOG;
	}

	if( i >= 3 )
	{
		Result = FALSE;
	}

  	return Result;
}

//-----------------------------------------------
//函数功能: 判断要操作的地址是否进入了保护区
//
//参数:
//			ProtectStart[in]	保护区起始地址
//			ProtectEnd[in]		保护区结束地址
//			StartAddr[in]		要操作的起始地址
//			EndAddr[in]			要操作的结束地址
//
//返回值:  	TRUE:要操作的地址不在保护区范围内	FALSE:要操作的地址在保护区范围内
//
//备注:
//-----------------------------------------------
static BOOL CheckProtectArea( DWORD ProtectStart, DWORD ProtectEnd, DWORD StartAddr, DWORD EndAddr )
{
	//注意 都是带等号得
	if(EndAddr <= ProtectStart)
	{
		return TRUE;
	}

	if(StartAddr >= ProtectEnd)
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//函数功能: 向IIC设备进行分页写数据处理，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//			Addr[in]		写数据的起始
//			Len[in]			写数据的长度
//			pBuf[in/out]	写数据的缓冲
//          
//返回值: 	TRUE:写成功		FALSE:写失败
static WORD WriteMemWithPages(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	volatile WORD TrueAddr;
	short WriteLen;
	WORD PageLen;
	BYTE i;

	// 地址取整
	TrueAddr = (Addr / PAGE_SIZE) * PAGE_SIZE;

	// 第一个页中数据长度
	PageLen = PAGE_SIZE - (WORD)(Addr - TrueAddr);

	// 要写入数据的长度
	WriteLen = (short)Len;

	// 是否所有数据都在一个扇区内
	if( PageLen > Len )
	{
		PageLen = Len;
		WriteLen -= PageLen;
	}

	//对保护区域特殊处理
	if( api_GetSysStatus(eSYS_STATUS_EN_WRITE_ENERGY) == FALSE )
	{
		if(CheckProtectArea( GET_SAFE_SPACE_ADDR( EnergySafeRom ), GET_SAFE_SPACE_ADDR( EnergySafeRom ) + sizeof(TEnergySafeRom), Addr, Addr + Len ) != TRUE)
		{
			api_WriteSysUNMsg(SYSUN_EEPROM_PROTECT_11);
			return FALSE;
		}
	}

	if( api_GetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA) == FALSE )
	{
		if(CheckProtectArea( GET_SAFE_SPACE_ADDR( SampleSafeRom ), GET_SAFE_SPACE_ADDR( SampleSafeRom ) + sizeof(TSampleSafeRom), Addr, Addr + Len ) != TRUE)
		{
		    api_WriteSysUNMsg(SYSUN_EEPROM_PROTECT_33);
			return FALSE;
		}

	}

	// 写入数据的起始地址
	TrueAddr = Addr;
	// 写入所有页
	for(;;)
	{
		// 重复写入，若成功或者重试次数到则跳出循环
		for(i=0; i<3; i++)
		{
			// 写入一个页
			if( WriteXX24XXXPage(ChipNo, TrueAddr, PageLen, pBuf) == TRUE )
			{
				if( ChipNo == CS_SPI_256401 )
				{
					api_ClrError(ERR_WRITE_EEPROM1);
					g_byEEErrTimes[0] = 0;
				}
				else
				{
					api_ClrError(ERR_WRITE_EEPROM2);
					g_byEEErrTimes[1] = 0;
				}
				break;
			}

			CLEAR_WATCH_DOG;
		}

		// 重试次数是否已到？
		if( i >= 3 )
		{
			//置错误标志
			if( ChipNo == CS_SPI_256401 )
			{
				api_SetError( (ERR_WRITE_EEPROM1|0x8000) );

				if( g_byEEErrTimes[0] < 120 )
				{
					g_byEEErrTimes[0] ++;
					if( g_byEEErrTimes[0] == 120 )
					{
						api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
					}
				}
			}
			else
			{

				api_SetError( (ERR_WRITE_EEPROM2|0x8000) );
				if( g_byEEErrTimes[1] < 120 )
				{
					g_byEEErrTimes[1] ++;
					if( g_byEEErrTimes[1] == 120 )
					{
						api_SetFollowReportStatus(eSTATUS_EEPROM_Error);
					}
				}
			}
			// 大于最大重试次数，返回错误
			return FALSE;
		}

		// 观察是否写入完成
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			// 已经写完
			break;
		}

		// 操作地址
		TrueAddr += PageLen;
		pBuf += PageLen;

		// 计算长度
		if( WriteLen >= PAGE_SIZE )
		{
			PageLen = PAGE_SIZE;
		}
		else
		{
			PageLen = WriteLen;
		}
	}

	return TRUE;
}


//-----------------------------------------------
//函数功能: 写保护操作函数，指管脚控制的硬件写保护，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//			Do[in]			TRUE:使能写保护		FALSE:关闭写保护
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void DoEEPRomProtect( BYTE ChipNo, BYTE Do )
{
	#if( SEL_MEMORY_PROTECT==YES )
	
	if( ChipNo == CS_SPI_256401 )
	{
		if( Do == TRUE )
		{
			ENABLE_WRITE_PROTECT1;	// 打开第一片保护
		}
		else
		{
			DISABLE_WRITE_PROTECT1;
		}
	}
	else
	{
		if( Do == TRUE )
		{
			ENABLE_WRITE_PROTECT2;	// 打开第二片保护
		}
		else
		{
			DISABLE_WRITE_PROTECT2;
		}
	}

	api_Delay10us(5);
	
	#endif//#if( SEL_MEMORY_PROTECT==YES )
}


//-----------------------------------------------
//函数功能: 向IIC设备写数据，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//			Addr[in]		写数据的起始
//			Len[in]			写数据的长度
//			pBuf[in/out]	写数据的缓冲
//          
//返回值: 	TRUE:写成功		FALSE:写失败
//
//备注:   
//-----------------------------------------------
static WORD WriteXX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE Result = FALSE;

	// 判断写入的地址是否在24LC256的地址范围之内
	ASSERT( (Addr+Len) <= SINGLE_CHIP_SIZE, 1 );
	if( (Addr+Len) > SINGLE_CHIP_SIZE )
	{
		return FALSE;
	}
	
#if( SLAVE_MEM_CHIP == CHIP_NO )
	if( ChipNo == CS_SPI_256402 )
	{
		Addr += SINGLE_CHIP_SIZE;
		ChipNo = CS_SPI_256401;
	}
#endif
	
	CLEAR_WATCH_DOG;
	
	DoEEPRomProtect(ChipNo,FALSE);

	Result = WriteMemWithPages(ChipNo, Addr, Len, pBuf);

	DoEEPRomProtect(ChipNo,TRUE);

  	return Result;
}


//-----------------------------------------------
//函数功能: 向IIC设备某一页写数据，包括回读检测，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//			Addr[in]		写数据的起始，肯定是页大小的整数倍
//			Len[in]			写数据的长度（不会超过一页大小）
//			pBuf[in/out]	写数据的缓冲
//          
//返回值: 	TRUE:写成功		FALSE:写失败
static WORD WriteXX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	// 器件地址
	BYTE i;
	BYTE * pReadBackPtr;
	BOOL Status;
	WORD wLen;

	// 写入数据
	I2CWriteDev(ChipNo, Addr, Len, pBuf);

	// 延时等待
	api_Delayms(6);

	// 申请回读缓冲
	wLen = Len;
	pReadBackPtr = api_AllocBuf( (WORD*)&wLen );

	Status = FALSE;

	//等待回读
	for(i=0; i<5; i++)
	{
		if( i != 0 )
		{
			api_Delayms(1);
            CLEAR_WATCH_DOG;
		}

		if( I2CReadDev(ChipNo, Addr, Len, pReadBackPtr) != TRUE )
		{
			continue;
		}

		// 检测是否写入
		if( memcmp(pBuf, pReadBackPtr, Len) == 0 )
		{
			Status = TRUE;
			break;
		}
	}

	// 释放缓冲
	api_FreeBuf(pReadBackPtr);

	// 返回结果
	return Status;
}


//-----------------------------------------------
//函数功能: 写EEPROM函数，模块内函数
//
//参数: 
//			No[in]			指定写哪一片EEPROM
//							CS_SPI_256401：第一片
//							CS_SPI_256402：第二片
//			Addr[in]		要写入的起始地址
//			Length[in]		要写入数据的长度
//			Buf[in]			要写入数据的缓冲
//                    
//返回值:  	TRUE:写入正确	FALSE:写入错误
//
//备注:   
//-----------------------------------------------
BOOL UWriteAt25640(WORD No, DWORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX( (BYTE)No, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 读EEPROM函数，模块内函数
//
//参数: 
//			No[in]			指定读哪一片EEPROM
//							CS_SPI_256401：第一片
//							CS_SPI_256402：第二片
//			Addr[in]		要读出的起始地址
//			Length[in]		要读出数据的长度
//			Buf[out]		要读出数据的缓冲
//                    
//返回值:  	TRUE:读出正确	FALSE:读出错误
//
//备注:   
//-----------------------------------------------
BOOL UReadAt25640(WORD No, DWORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX((BYTE)No, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 读第一片EEPROM函数，模块内函数
//
//参数: 
//			Addr[in]		要读出的起始地址
//			Length[in]		要读出数据的长度
//			Buf[in/out]		要读出数据的缓冲
//                    
//返回值:  	TRUE:读出正确	FALSE:读出错误
//
//备注:   
//-----------------------------------------------
WORD ReadEEPRom1(WORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX(CS_SPI_256401, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 读第二片EEPROM函数，模块内函数
//
//参数: 
//			Addr[in]		要读出的起始地址
//			Length[in]		要读出数据的长度
//			Buf[in/out]		要读出数据的缓冲
//                    
//返回值:  	TRUE:读出正确	FALSE:读出错误
//
//备注:   
//-----------------------------------------------
WORD ReadEEPRom2(WORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX(CS_SPI_256402, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 写第一片EEPROM函数，模块内函数
//
//参数: 
//			Addr[in]		要写入的起始地址
//			Length[in]		要写入数据的长度
//			Buf[in]			要写入数据的缓冲
//                    
//返回值:  	TRUE:写入正确	FALSE:写入错误
//
//备注:   
//-----------------------------------------------
WORD WriteEEPRom1(WORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX(CS_SPI_256401, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 写第二片EEPROM函数，模块内函数
//
//参数: 
//			Addr[in]		要写入的起始地址
//			Length[in]		要写入数据的长度
//			Buf[in]			要写入数据的缓冲
//                    
//返回值:  	TRUE:写入正确	FALSE:写入错误
//
//备注:   
//-----------------------------------------------
WORD WriteEEPRom2(WORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX(CS_SPI_256402, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//函数功能: 同时写两片EEPROM函数，数据不取反，模块内函数
//
//参数: 
//			Addr[in]		要写入的起始地址
//			Length[in]		要写入数据的长度
//			Buf[in]			要写入数据的缓冲
//                    
//返回值:  	TRUE:写入正确	FALSE:写入错误
//
//备注:   
//-----------------------------------------------
WORD WriteDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf )
{
	WORD Result = TRUE;
	
	if( WriteEEPRom1(Addr, Length, Buf) != TRUE )
	{
		Result = FALSE;
	}

	if( WriteEEPRom2(Addr, Length, Buf) != TRUE )
	{
		Result = FALSE;
	}

	return Result;
}


//-----------------------------------------------
//函数功能: 初始化EEPROM函数，模块内函数
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   
//-----------------------------------------------
void InitEEPRom(void)
{
	DoEEPRomProtect( CS_SPI_256401, TRUE );
	DoEEPRomProtect( CS_SPI_256402, TRUE );
}


