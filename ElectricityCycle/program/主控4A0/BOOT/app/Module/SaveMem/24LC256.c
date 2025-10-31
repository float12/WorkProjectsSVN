//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.11
//����		eeprom 24LC256 �������ļ�����ģ��IIC�����ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "SaveMem.h"

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define I2CTIMES		6
#if( MASTER_MEMORY_CHIP == CHIP_24LC256 )
	#define PAGE_SIZE		64
#elif( MASTER_MEMORY_CHIP == CHIP_24LC512 )
	#define PAGE_SIZE		64
#endif

//IICͨ����ʱ�궨��
#define IIC_DELAY	{__NOP();__NOP();__NOP();__NOP();__NOP();} //5��NOP��iicʱ��Ƶ��Լ182K

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


// EEPROM д�뱣���ܽ� �йܽ����ӣ������һֱ��Ϊд������Ч
#define ENABLE_WRITE_PROTECT1			;
#define DISABLE_WRITE_PROTECT1			;
#define ENABLE_WRITE_PROTECT2			;
#define DISABLE_WRITE_PROTECT2			;
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static BYTE g_byEEErrTimes[2];	//EEPROM�洢�����ϴ���
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------
static BYTE Get24LC256DevAddr( BYTE ChipNo );
static WORD WriteXX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf);

//-----------------------------------------------
//				��������
//-----------------------------------------------

#if( SEL_24LC256_COMMUNICATION_MOD == IIC_SIM )
//-----------------------------------------------
//��������: IIC����ʱ�������ڲ�����
//
//����: 	
//          m:��ʱ������û�б궨������ʱ�Ƕ���          
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
static void Delay24LC256IIC( WORD m )
{  
	while((m) != 0)
  	{
    	m >>= 3;
  	}
}


//-----------------------------------------------
//��������: ��IIC START�ĺ������ڲ�����
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
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
//��������: ��IIC STOP�ĺ������ڲ�����
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
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
//��������: ��IIC����һ���ֽڣ��ڲ�����
//
//����: 	
//          Byte��Ҫ���͵�����
//          
//����ֵ: 	��
//
//��ע:   
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
//��������: ��ѯACK���ڲ�����
//
//����: 	��
//          
//����ֵ: 	TRUE:��⵽��ACK	FALSE:û�м�⵽ACK
//
//��ע:   
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
//��������: ����NOACK���ڲ�����
//
//����: 	��
//          
//����ֵ: 	��
//
//��ע:   
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
//��������: ����ACK���ڲ�����
//
//����: 	��
//          
//����ֵ: 	��
//
//��ע:   
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
//��������: IIC����һ���ֽں������ڲ�����
//
//����: 	��
//          
//����ֵ: 	��
//
//��ע:   
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
//��������: IIC��ʼ���ڲ�����
//
//����: 	��
//          
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
static void i2c_start_set24LC256IIC(void)
{
	EEPROM_SDA_H;
	EEPROM_SCL_H;
	Delay24LC256IIC(60);
}


//-----------------------------------------------
//��������: IIC�������ڲ�����
//
//����: 	��
//          
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
static void i2c_stop_set24LC256IIC(void)
{
	Delay24LC256IIC(60);
}


//-----------------------------------------------
//��������: IICд���ݺ������ڲ�����
//
//����: 	
//			I2CNo[in]		ѡ��д���ĸ�оƬ
//			Addr[in]		Ҫд�����ʼ��ַ
//			Num[in]			Ҫд������ݳ���
//			Buf[in]			Ҫд�������
//          
//����ֵ: 	TRUE:д����ȷ	FALSE��д�����
//
//��ע:   
//-----------------------------------------------
static WORD I2CWriteDev(BYTE I2CNo, WORD Addr, WORD Num, BYTE *Buf)
{
	BYTE i,Flag, DevAddr;

	// �õ��豸��ַ
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

		// ������ַ
		Send_Byte24LC256IIC(DevAddr);
		if(Ack24LC256IIC()==FALSE)
		{
			continue;
		}


		Send_Byte24LC256IIC( (BYTE)((Addr>>8)&0xff) );
		if(Ack24LC256IIC()!=TRUE)
		{
			//�����ַ���ֽڷ��Ͳ�Ӧ��
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
//��������: IIC�����ݺ������ڲ�����
//
//����: 	
//			I2CNo[in]		ѡ������ĸ�оƬ
//			Addr[in]		Ҫ��������ʼ��ַ
//			Num[in]			Ҫ���������ݳ���
//			Buf[in/out]		Ҫ����������
//          
//����ֵ: 	TRUE:������ȷ	FALSE����������
//
//��ע:   
//-----------------------------------------------
static WORD I2CReadDev(BYTE I2CNo, WORD Addr, WORD Num,BYTE *Buf)
{
	BYTE DevAddr;
	BOOL Result;
	WORD i;

	Result = TRUE;

	// �õ��豸��ַ
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

		// д����
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

		// ������
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
			// ȡ����
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
// ������24LC256�Ĵ洢��������
//--------------------------------------------------------------------------------
//-----------------------------------------------
//��������: ��ȡIIC�豸��Ƭѡ��ַ���ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//          
//����ֵ: 	IIC�豸��Ƭѡ��ַ
//
//��ע:   
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
//��������: ��IIC�豸�����ݣ��ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//			Addr[in]		�����ݵ���ʼ
//			Len[in]			�����ݵĳ���
//			pBuf[in/out]	�����ݵĻ���
//          
//����ֵ: 	TRUE:���ɹ�		FALSE:��ʧ��
//
//��ע:   
//-----------------------------------------------
static WORD ReadXX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
  	BYTE Result,i;

	Result = TRUE;

	// �ж�д��ĵ�ַ�Ƿ���24LC256�ĵ�ַ��Χ֮��
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

	// ������ʱ���ٴ�д������ȷ���ڶ����ݵ�ʱ�򲻻������д����
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
//��������: �ж�Ҫ�����ĵ�ַ�Ƿ�����˱�����
//
//����:
//			ProtectStart[in]	��������ʼ��ַ
//			ProtectEnd[in]		������������ַ
//			StartAddr[in]		Ҫ��������ʼ��ַ
//			EndAddr[in]			Ҫ�����Ľ�����ַ
//
//����ֵ:  	TRUE:Ҫ�����ĵ�ַ���ڱ�������Χ��	FALSE:Ҫ�����ĵ�ַ�ڱ�������Χ��
//
//��ע:
//-----------------------------------------------
static BOOL CheckProtectArea( DWORD ProtectStart, DWORD ProtectEnd, DWORD StartAddr, DWORD EndAddr )
{
	//ע�� ���Ǵ��Ⱥŵ�
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
//��������: ��IIC�豸���з�ҳд���ݴ����ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//			Addr[in]		д���ݵ���ʼ
//			Len[in]			д���ݵĳ���
//			pBuf[in/out]	д���ݵĻ���
//          
//����ֵ: 	TRUE:д�ɹ�		FALSE:дʧ��
static WORD WriteMemWithPages(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	volatile WORD TrueAddr;
	short WriteLen;
	WORD PageLen;
	BYTE i;

	// ��ַȡ��
	TrueAddr = (Addr / PAGE_SIZE) * PAGE_SIZE;

	// ��һ��ҳ�����ݳ���
	PageLen = PAGE_SIZE - (WORD)(Addr - TrueAddr);

	// Ҫд�����ݵĳ���
	WriteLen = (short)Len;

	// �Ƿ��������ݶ���һ��������
	if( PageLen > Len )
	{
		PageLen = Len;
		WriteLen -= PageLen;
	}

	//�Ա����������⴦��
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

	// д�����ݵ���ʼ��ַ
	TrueAddr = Addr;
	// д������ҳ
	for(;;)
	{
		// �ظ�д�룬���ɹ��������Դ�����������ѭ��
		for(i=0; i<3; i++)
		{
			// д��һ��ҳ
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

		// ���Դ����Ƿ��ѵ���
		if( i >= 3 )
		{
			//�ô����־
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
			// ����������Դ��������ش���
			return FALSE;
		}

		// �۲��Ƿ�д�����
		WriteLen -= PageLen;
		if( WriteLen <= 0 )
		{
			// �Ѿ�д��
			break;
		}

		// ������ַ
		TrueAddr += PageLen;
		pBuf += PageLen;

		// ���㳤��
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
//��������: д��������������ָ�ܽſ��Ƶ�Ӳ��д�������ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//			Do[in]			TRUE:ʹ��д����		FALSE:�ر�д����
//          
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
static void DoEEPRomProtect( BYTE ChipNo, BYTE Do )
{
	#if( SEL_MEMORY_PROTECT==YES )
	
	if( ChipNo == CS_SPI_256401 )
	{
		if( Do == TRUE )
		{
			ENABLE_WRITE_PROTECT1;	// �򿪵�һƬ����
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
			ENABLE_WRITE_PROTECT2;	// �򿪵ڶ�Ƭ����
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
//��������: ��IIC�豸д���ݣ��ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//			Addr[in]		д���ݵ���ʼ
//			Len[in]			д���ݵĳ���
//			pBuf[in/out]	д���ݵĻ���
//          
//����ֵ: 	TRUE:д�ɹ�		FALSE:дʧ��
//
//��ע:   
//-----------------------------------------------
static WORD WriteXX24XXX(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	BYTE Result = FALSE;

	// �ж�д��ĵ�ַ�Ƿ���24LC256�ĵ�ַ��Χ֮��
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
//��������: ��IIC�豸ĳһҳд���ݣ������ض���⣬�ڲ�����
//
//����: 	
//			ChipNo[in]		ѡ���ĸ�оƬ
//			Addr[in]		д���ݵ���ʼ���϶���ҳ��С��������
//			Len[in]			д���ݵĳ��ȣ����ᳬ��һҳ��С��
//			pBuf[in/out]	д���ݵĻ���
//          
//����ֵ: 	TRUE:д�ɹ�		FALSE:дʧ��
static WORD WriteXX24XXXPage(BYTE ChipNo, WORD Addr, WORD Len, BYTE * pBuf)
{
	// ������ַ
	BYTE i;
	BYTE * pReadBackPtr;
	BOOL Status;
	WORD wLen;

	// д������
	I2CWriteDev(ChipNo, Addr, Len, pBuf);

	// ��ʱ�ȴ�
	api_Delayms(6);

	// ����ض�����
	wLen = Len;
	pReadBackPtr = api_AllocBuf( (WORD*)&wLen );

	Status = FALSE;

	//�ȴ��ض�
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

		// ����Ƿ�д��
		if( memcmp(pBuf, pReadBackPtr, Len) == 0 )
		{
			Status = TRUE;
			break;
		}
	}

	// �ͷŻ���
	api_FreeBuf(pReadBackPtr);

	// ���ؽ��
	return Status;
}


//-----------------------------------------------
//��������: дEEPROM������ģ���ں���
//
//����: 
//			No[in]			ָ��д��һƬEEPROM
//							CS_SPI_256401����һƬ
//							CS_SPI_256402���ڶ�Ƭ
//			Addr[in]		Ҫд�����ʼ��ַ
//			Length[in]		Ҫд�����ݵĳ���
//			Buf[in]			Ҫд�����ݵĻ���
//                    
//����ֵ:  	TRUE:д����ȷ	FALSE:д�����
//
//��ע:   
//-----------------------------------------------
BOOL UWriteAt25640(WORD No, DWORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX( (BYTE)No, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: ��EEPROM������ģ���ں���
//
//����: 
//			No[in]			ָ������һƬEEPROM
//							CS_SPI_256401����һƬ
//							CS_SPI_256402���ڶ�Ƭ
//			Addr[in]		Ҫ��������ʼ��ַ
//			Length[in]		Ҫ�������ݵĳ���
//			Buf[out]		Ҫ�������ݵĻ���
//                    
//����ֵ:  	TRUE:������ȷ	FALSE:��������
//
//��ע:   
//-----------------------------------------------
BOOL UReadAt25640(WORD No, DWORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX((BYTE)No, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: ����һƬEEPROM������ģ���ں���
//
//����: 
//			Addr[in]		Ҫ��������ʼ��ַ
//			Length[in]		Ҫ�������ݵĳ���
//			Buf[in/out]		Ҫ�������ݵĻ���
//                    
//����ֵ:  	TRUE:������ȷ	FALSE:��������
//
//��ע:   
//-----------------------------------------------
WORD ReadEEPRom1(WORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX(CS_SPI_256401, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: ���ڶ�ƬEEPROM������ģ���ں���
//
//����: 
//			Addr[in]		Ҫ��������ʼ��ַ
//			Length[in]		Ҫ�������ݵĳ���
//			Buf[in/out]		Ҫ�������ݵĻ���
//                    
//����ֵ:  	TRUE:������ȷ	FALSE:��������
//
//��ע:   
//-----------------------------------------------
WORD ReadEEPRom2(WORD Addr, WORD Length, BYTE * Buf)
{
	return ReadXX24XXX(CS_SPI_256402, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: д��һƬEEPROM������ģ���ں���
//
//����: 
//			Addr[in]		Ҫд�����ʼ��ַ
//			Length[in]		Ҫд�����ݵĳ���
//			Buf[in]			Ҫд�����ݵĻ���
//                    
//����ֵ:  	TRUE:д����ȷ	FALSE:д�����
//
//��ע:   
//-----------------------------------------------
WORD WriteEEPRom1(WORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX(CS_SPI_256401, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: д�ڶ�ƬEEPROM������ģ���ں���
//
//����: 
//			Addr[in]		Ҫд�����ʼ��ַ
//			Length[in]		Ҫд�����ݵĳ���
//			Buf[in]			Ҫд�����ݵĻ���
//                    
//����ֵ:  	TRUE:д����ȷ	FALSE:д�����
//
//��ע:   
//-----------------------------------------------
WORD WriteEEPRom2(WORD Addr, WORD Length, BYTE * Buf)
{
	return WriteXX24XXX(CS_SPI_256402, (WORD)Addr, Length, Buf);
}


//-----------------------------------------------
//��������: ͬʱд��ƬEEPROM���������ݲ�ȡ����ģ���ں���
//
//����: 
//			Addr[in]		Ҫд�����ʼ��ַ
//			Length[in]		Ҫд�����ݵĳ���
//			Buf[in]			Ҫд�����ݵĻ���
//                    
//����ֵ:  	TRUE:д����ȷ	FALSE:д�����
//
//��ע:   
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
//��������: ��ʼ��EEPROM������ģ���ں���
//
//����: 	��
//                    
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void InitEEPRom(void)
{
	DoEEPRomProtect( CS_SPI_256401, TRUE );
	DoEEPRomProtect( CS_SPI_256402, TRUE );
}


