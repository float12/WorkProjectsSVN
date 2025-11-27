//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	LiuZhenXing
//创建日期	2020.12.16
//描述		负荷识别模块EEPROM驱动程序
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "M24512.h"
#if(MASTER_MEMORY_CHIP==CHIP_M24512_RMN6TP)  
/* 64Kbytes  每页128bytes   总共有512页*/
    #define PAGE_SIZE                   128
    #define MEM_M24512_DEVADDR          0xa0
    #define I2CTIMES                    6
#endif
//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
// static BYTE g_byEEErrTimes[1];	//EEPROM存储器故障次数
//------------------------------------------------
//        函数声明
//-------------------------------------------------
WORD WriteX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf);

//函数功能: 获取IIC设备的片选地址，内部函数
//
//参数: 	
//			ChipNo[in]		选择哪个芯片
//          
//返回值: 	IIC设备的片选地址
//
//备注:   
//-----------------------------------------------
static BYTE GetM24512DevAddr( BYTE ChipNo )
{
	if( ChipNo == CS_SPI_EEPROM )
	{
		return MEM_M24512_DEVADDR;
	}
    else if(ChipNo == CS_SPI_EEPROM2)
    {
        return MEM_M24512_DEVADDR;
    }
	else
	{
		return 0xff;
	}
}

//-----------------------------------------------
//函数功能: 发IIC START的函数，内部函数
//
//参数: 	无
//                    
//返回值: 	无
//
//备注:   IIC启动信号 
//-----------------------------------------------
static void StartM24512IIC(void)
{
	SDA_WRITE_Direction;
	IIC_SCL_H;
    IIC_SDA_H;

    #if(MASTER_MEMORY_CHIP==CHIP_M24512_RMN6TP)
    api_Delayus(5);
    #endif

    IIC_SDA_H;

    #if(MASTER_MEMORY_CHIP==CHIP_M24512_RMN6TP)
    api_Delayus(5);
    #endif

    IIC_SDA_L;

    #if(MASTER_MEMORY_CHIP==CHIP_M24512_RMN6TP)
    api_Delayus(5);
    #endif

    IIC_SCL_L;

    #if(MASTER_MEMORY_CHIP==CHIP_M24512_RMN6TP)
    api_Delayus(5);
    #endif
}

//-----------------------------------------------
//函数功能: 发IIC STOP的函数，内部函数
//
//参数: 	无
//
//返回值: 	无
//
//备注:   IIC停止信号
//-----------------------------------------------
static void StopM24512IIC(void)
{
	SDA_WRITE_Direction;
	IIC_SDA_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif

    IIC_SCL_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif

    IIC_SDA_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
}
//-----------------------------------------------
//函数功能: 向IIC发送一个字节，内部函数
//
//参数: 	
//          SendData：要发送的数据
//          
//返回值: 	无
//
//备注:   
//-----------------------------------------------
static void SendM24512IICBYTE(BYTE SendData)
{
	WORD i;
	SDA_WRITE_Direction;
	for (i=0;i<8;i++)
	{
		if(SendData & 0x80)
		{
			IIC_SDA_H;
		}
		else
		{
			IIC_SDA_L;
		}

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif	

		IIC_SCL_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif	
	
		IIC_SCL_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif		
		SendData = SendData << 1;
	}
}
//-----------------------------------------------
//函数功能: 查询ACK，内部函数
//
//参数: 	无
//          
//返回值: 	TRUE:检测到了ACK	FALSE:没有检测到ACK
//
//备注:   接收从机ACK
//-----------------------------------------------
static WORD AckM24512IIC(void)
{
	WORD Ack_Flag;
	BYTE i=5;
	// BYTE xx=3;
	SDA_READ_Direction;
#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif	
	
	IIC_SCL_H;
	Ack_Flag = TRUE;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
	while ((i--) && IIC_ACK());
	if (i==0xff)
	{
		Ack_Flag = FALSE;
	}
	IIC_SCL_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
	SDA_WRITE_Direction;
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
static void NOAckM24512IIC(void)
{
	SDA_WRITE_Direction;
	IIC_SDA_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif	
	
	IIC_SCL_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif	
	
    IIC_SCL_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif	

    IIC_SDA_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif	
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
static void SendAckM24512IIC(void)
{
	SDA_WRITE_Direction;
	IIC_SDA_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
	
    IIC_SCL_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
	
    IIC_SCL_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
	
    IIC_SDA_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(5);
#endif
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
static WORD Receive_ByteM24512IIC(void)
{
	WORD R_word;
	WORD j;
	
	R_word = 0x00;

	SDA_READ_Direction;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif
	
	for(j=0;j<8;j++)
	{
		IIC_SCL_H;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif

		if (IIC_ACK())
		{
			R_word |= 0x01;
		}
		else
		{
			R_word &= 0xFE;
		}

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif
	
		IIC_SCL_L;

#if (MASTER_MEMORY_CHIP == CHIP_M24512_RMN6TP)
    api_Delayus(3);
#endif
	
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
//备注:   此时总线处于空闲状态
//-----------------------------------------------
static void i2c_start_setM24512IIC(void)
{
	SDA_WRITE_Direction;
	IIC_SDA_H;
	IIC_SCL_H;
    api_Delay10us(6);
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
static void i2c_stop_setM24512IIC(void)
{
	api_Delay10us(6);
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
//备注:   设备地址最后一个bit 0----写   1----读
//-----------------------------------------------
WORD I2CWriteDev(BYTE I2CNo, WORD Addr, WORD Num, BYTE *Buf)
{
	BYTE i,Flag, DevAddr;

	// 得到设备地址
	DevAddr = GetM24512DevAddr(I2CNo);

	if( DevAddr == 0xff )
	{
		return FALSE;
	}

	Flag = TRUE;
	i2c_start_setM24512IIC();

	for(i=0;i<I2CTIMES;i++)
	{
		StartM24512IIC();
        api_Delayus(5);

        // 操作地址
		SendM24512IICBYTE(DevAddr);
		if(AckM24512IIC()==FALSE)
		{
			continue;
		}


		SendM24512IICBYTE( (BYTE)((Addr>>8)&0xff) );
		if(AckM24512IIC()!=TRUE)
		{
			//处理地址高字节发送不应答
			continue;
		}

		SendM24512IICBYTE( (BYTE)(Addr&0xff) );
		if(AckM24512IIC()==TRUE)
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
		SendM24512IICBYTE(Buf[i]);
		if(AckM24512IIC()==FALSE)
		{
			Flag = FALSE;
		}
	}

	StopM24512IIC();
	i2c_stop_setM24512IIC();   
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
	DevAddr = GetM24512DevAddr(I2CNo);

	if( DevAddr == 0xff )
	{
		return FALSE;
	}

	i2c_start_setM24512IIC();

	for(i=0;i<I2CTIMES;i++)
	{
		CLEAR_WATCH_DOG;
		StartM24512IIC();

		// 写数据
		SendM24512IICBYTE(DevAddr);

		if(AckM24512IIC()==FALSE)
		{
			continue;
		}

		SendM24512IICBYTE( (BYTE)((Addr>>8)&0xff) );

		if(AckM24512IIC()==FALSE)
		{
			continue;
		}

		SendM24512IICBYTE( (BYTE)(Addr&0xff) );
		if(AckM24512IIC()==FALSE)
		{
			continue;
		}

		StartM24512IIC();

		// 读数据
		SendM24512IICBYTE(DevAddr + 1);
		if(AckM24512IIC()==TRUE)
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
			Buf[i] = Receive_ByteM24512IIC();

			if(i<(Num-1))
			{
				SendAckM24512IIC();
			}
		}

		NOAckM24512IIC();
	}

	StopM24512IIC();

	i2c_stop_setM24512IIC();

	return Result;
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
static WORD ReadX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
  	BYTE Result,i;

	Result = TRUE;

	// 判断写入的地址是否在24LC256的地址范围之内
	//ASSERT( (Addr+Len) <= HALFRMN_MEM_SPACE, 1 );
	if ((Addr + Len) > HALFRMN_MEM_SPACE)
	{
		return FALSE;
	}
	
	#if( SLAVE_MEM_CHIP == CHIP_NO )
	if( ChipNo == CS_SPI_EEPROM2 )
	{
		Addr += HALFRMN_MEM_SPACE;
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

	// 第一个页中数据长度   （剩余）
	PageLen = PAGE_SIZE - (WORD)(Addr - TrueAddr);

	// 要写入数据的长度
	WriteLen = (short)Len;

	// 是否所有数据都在一个扇区内  （能否写下）
	if( PageLen > Len )
	{
		PageLen = Len;
		WriteLen -= PageLen;
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
			if( WriteX24XXXPage(ChipNo, TrueAddr, PageLen, pBuf) == TRUE )
			{
				if( ChipNo == CS_SPI_EEPROM )
				{
					api_ClrError(ERR_WRITE_EEPROM1);
				}
				else
				{
					api_ClrError(ERR_WRITE_EEPROM2);
				}
				break;
			}
			CLEAR_WATCH_DOG;
		}
		// 重试次数是否已到？
		if( i >= 3 )
		{
			//置错误标志
			if( ChipNo == CS_SPI_EEPROM )
			{
				api_SetError( (ERR_WRITE_EEPROM1|0x8000) );
			}
			else
			{
				api_SetError( (ERR_WRITE_EEPROM2|0x8000) );
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
static WORD WriteX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE Result = FALSE;

	// 判断写入的地址是否在24LC256的地址范围之内
	//ASSERT( (Addr+Len) <= HALFRMN_MEM_SPACE, 1 );
	if( (Addr+Len) > HALFRMN_MEM_SPACE)
	{
		//记录异常事件
		api_WriteSysUNMsg( OVERWRITE_CONTINUE_SPACE );
		return FALSE;
	}
	
	#if( SLAVE_MEM_CHIP == CHIP_NO )
		if( ChipNo == CS_SPI_EEPROM2 )
		{
			Addr += HALFRMN_MEM_SPACE;
			ChipNo = CS_SPI_EEPROM;
		}
	#endif
	
	CLEAR_WATCH_DOG;

	Result = WriteMemWithPages(ChipNo, Addr, Len, pBuf);

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
WORD WriteX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	// 器件地址
	BYTE i,ReadBackBuf[PAGE_SIZE+30];
	BOOL Status;
//	WORD wLen;

	if( Len > PAGE_SIZE )//写页不能超过PAGE_SIZE
	{
		Len = PAGE_SIZE;
	}

	// 写入数据
	I2CWriteDev(ChipNo, Addr, Len, pBuf);

	// 延时等待
	api_Delayms(6);

	Status = FALSE;

	//等待回读
	for(i=0; i<5; i++)
	{
		if( i != 0 )
		{
			api_Delayms(1);
            CLEAR_WATCH_DOG;
		}

		if( I2CReadDev(ChipNo, Addr, Len, ReadBackBuf) != TRUE )
		{
			continue;
		}

		// 检测是否写入
		if( memcmp(pBuf, ReadBackBuf, Len) == 0 )
		{
			Status = TRUE;
			break;
		}
	}

	// 返回结果
	return Status;
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
	return ReadX24XXX(CS_SPI_EEPROM, (WORD)Addr, Length, Buf);
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
	return ReadX24XXX(CS_SPI_EEPROM2, (WORD)Addr, Length, Buf);
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
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 2) & 0x0001) == 0x0001)
	{
		rt_kprintf("%d\n",100);
		rt_kprintf("%d\n",Addr);
		rt_kprintf("%d\n",Length);
	}
	#endif
	
	return WriteX24XXX(CS_SPI_EEPROM, (WORD)Addr, Length, Buf);
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
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 2) & 0x0001) == 0x0001)
	{
		rt_kprintf("%d\n",110);
		rt_kprintf("%d\n",Addr);
		rt_kprintf("%d\n",Length);
	}
	#endif
	return WriteX24XXX(CS_SPI_EEPROM2, (WORD)Addr, Length, Buf);
}
//-----------------------------------------------
//函数功能: 同时读两片EEPROM函数，数据不取反，模块内函数
//
//参数: 
//			Addr[in]		要读出的起始地址
//			Length[in]		要读出数据的长度
//			Buf1[in/out]	要读出数据的缓冲1
//			Buf2[in/out]	要读出数据的缓冲2
//                    
//返回值:  	TRUE:读出正确	FALSE:读出错误
//
//备注:   
//-----------------------------------------------
WORD ReadDoubleEEPRom( WORD Addr, WORD Length, BYTE * Buf1, BYTE * Buf2 )
{
	ReadEEPRom1(Addr, Length, Buf1);
	ReadEEPRom2(Addr, Length, Buf2);
	return TRUE;
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
	if(Addr < UPFILE_DATA_ADDR) //由于连续空间也是写备份，那么不能超过特殊地址,所以第一片和第二片都不能超过这个地址
	{
		if( WriteEEPRom1(Addr, Length, Buf) != TRUE )
		{
			Result = FALSE;
		}


		if( WriteEEPRom2(Addr, Length, Buf) != TRUE )
		{
			Result = FALSE;
		}
	}
	else
	{
		Result = FALSE;
	}
	return Result;
}






