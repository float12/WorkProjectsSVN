//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	������
//��������	2018.10.30
//����		��ʱֵ���㴦�����ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"

#if( SEL_AHOUR_FUNC == YES )

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

static __no_init TAHour	AhourRam;                           // RAM��İ�ʱֵ����λ0.01As

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: �ۼư�ʱֵ
//
//����:  ��
//
//����ֵ:	��
//
//��ע: һ�����ۼ�һ��
//-----------------------------------------------
void AccumulateAHour( void )
{
	DWORD	dwCurrent;
	BYTE i;

	// ���RAMֵ
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );
	}

	for(i = 0; i < MAX_PHASE; i++)
	{
		api_GetRemoteData( PHASE_A + REMOTE_I + 0x1000 * i, DATA_HEX, 2, 4, (BYTE *)&dwCurrent );   // ��λС��

		AhourRam.AHour[1 + i] += dwCurrent;
		AhourRam.AHour[0] += dwCurrent; // ��
	}

	// ����У��
	AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
}

//-----------------------------------------------
//��������: ת�氲ʱֵ
//
//����:  ��
//
//����ֵ:	��
//
//��ע:��1������ת�� ��2��1Сʱת��һ��
//-----------------------------------------------
void SwapAHour( void )
{
	// ���RAMֵ
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );
		AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
	}

	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam );

}

//-----------------------------------------------
//��������: ��鰲ʱֵ
//
//����:  ��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void PowerUpAHour( void )
{
	// ���
	if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam.AHour, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
	{
		// ��EEP�ָ�
		if(api_ReadFromContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam ) == FALSE)
		{
			if(lib_CheckSafeMemVerify( (BYTE *)&AhourRam.AHour, sizeof(TAHour), UN_REPAIR_CRC ) == FALSE)
			{
				// EEP��Ҳ����,������
				memset( (BYTE *)&AhourRam.AHour, 0x00, sizeof(TAHour) );
				// ����У��
				AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
			}
		}
	}
}

//-----------------------------------------------
//��������: �尲ʱֵ
//
//����:  ��
//
//����ֵ:	��
//
//��ע:
//-----------------------------------------------
void api_ClrAHour( void )
{
	memset( (BYTE *)&AhourRam, 0x00, sizeof(TAHour) );

	// ����У��
	AhourRam.CRC32 = lib_CheckCRC32( (BYTE *)&AhourRam, sizeof(TAHour) - sizeof(DWORD) );
	// ��0.01Ah
	api_WriteToContinueEEPRom( GET_CONTINUE_ADDR( AHourConRom ), sizeof(TAHour), (BYTE *)&AhourRam );
}

//-----------------------------------------------
//��������: ����ʱֵ
//
//����:  Type[in]: ePHASE_TYPE ��ABC
//		DataType[in]	DATA_BCD/DATA_HEX(ԭ���ʽ)/DATA_HEXCOMP(�����ʽ)
// 		pBuf[out]: ָ�򷵻����ݵ�ָ��
//
//����ֵ:	��
//
//��ע:   TRUE   ��ȷ
//		   FALSE  ����
//-----------------------------------------------
BOOL api_GetAHourData( BYTE Type, BYTE DataType, BYTE *pBuf )
{
	DWORD dw;

	dw = AhourRam.AHour[Type] / 3600;

	if(DataType == DATA_BCD)
	{
		dw = lib_DWBinToBCD( dw );
	}

	memcpy( pBuf, (BYTE *)&dw, sizeof(DWORD) );

	return TRUE;
}


#endif//#if( SEL_AHOUR_FUNC == YES )

