//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.16
//����		��־�����ļ�������״̬��־�����Ʊ�־�������־�������־
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"

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
//��־����
static TFlagBytes FlagBytes;
__no_init TNoInitDataFlag NoInitDataFlag;
#if( SEL_DLT645_2007 == YES )
extern const BOOL  Meter645FollowStatusConst;
#endif

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ����ϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
	i = (StatusNo%eSYS_STATUS_NUM_T) / 8;
	j = StatusNo % 8;

	FlagBytes.SysStatus[i] |= (0x01<<j);
}


//-----------------------------------------------
//��������: ���ϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );
	
	i = (StatusNo%eSYS_STATUS_NUM_T) / 8;
	j = StatusNo % 8;

	FlagBytes.SysStatus[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//��������: ��ȡϵͳ״̬
//
//����: 
//			StatusNo[in]		ϵͳ״̬��
//                    
//����ֵ:  	TRUE:ϵͳ״̬��λ�� FALSE:ϵͳ״̬δ��λ
//
//��ע:   
//-----------------------------------------------
BOOL api_GetSysStatus(BYTE StatusNo)
{
	BYTE i, j;
	
	ASSERT( StatusNo < eSYS_STATUS_NUM_T, 0 );

	i = ((StatusNo%eSYS_STATUS_NUM_T)>>3);//�൱�ڳ���8
	j = (StatusNo&0x07);//�൱�ڶ�8��ģ

	if( (FlagBytes.SysStatus[i] & (0x01<<j)) != 0 )
	{
		// ״̬��Ч
		return TRUE;
	}

	// ״̬��Ч
	return FALSE;
}


//-----------------------------------------------
//��������: ����ϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:���óɹ�	FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_SetRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	i = (FlagNo%eRUN_HARD_NUM_T) / 8;
	j = FlagNo % 8;

	if( FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j) )//��λ�Ѿ���1������Ƶ��дEEPROM
	{
		return TRUE;
	}
	FlagBytes.RunHardFlagSafeRom.Flag[i] |= (0x01<<j);

	// ���¼���У��
	lib_CheckSafeMemVerify((BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC);

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// ��¼����
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ���ϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:����ɹ�	FALSE:���ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ClrRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	i = (FlagNo%eRUN_HARD_NUM_T) / 8;
	j = FlagNo % 8;

	if( (FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j)) == 0 )//��λ�Ѿ����㣬����Ƶ��дEEPROM
	{
		return TRUE;
	}

	FlagBytes.RunHardFlagSafeRom.Flag[i] &= ~(0x01<<j);

	// ���¼���У��
	lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC );

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// ��¼����
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ��ȡϵͳ���й̻���־
//
//����: 
//			FlagNo[in]		ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:��Ӧ�̻���־��λ��	FALSE:��Ӧ�̻���־δ��λ�����ݲ���
//
//��ע:   
//-----------------------------------------------
BOOL api_GetRunHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < eRUN_HARD_NUM_T, 0 );

	// ����ڴ��б�־У���Ƿ���ȷ
	if( lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.RunHardFlagSafeRom, sizeof(TRunHardFlagSafeRom), REPAIR_CRC ) != TRUE )
	{
		// ������ȷ��Ӵ洢���ж�ȡ
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
		{
			// ��¼����
			api_SetError(ERR_GET_HARD_FLAG);
			return FALSE;
		}
	}

	i = ((FlagNo%eRUN_HARD_NUM_T)>>3);//�൱�ڳ���8
	j = (FlagNo&0x07);//�൱�ڶ�8��ģ

	if( FlagBytes.RunHardFlagSafeRom.Flag[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//��������: ��λϵͳ���й̻���־
//
//����: 	��
//                    
//����ֵ:  	TRUE:��ʼ���ɹ�	FALSE:��ʼ��ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_FactoryInitRunHardFlag(void)
{
	memset(&FlagBytes.RunHardFlagSafeRom, 0, sizeof(TRunHardFlagSafeRom));
	
	#if( SEL_DLT645_2007 == YES )
    if( Meter645FollowStatusConst == TRUE )//645��������֡
    {
        api_SetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
    }
    #endif
    
	if( api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		// ��¼����
		api_SetError(ERR_SET_RUN_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: �ϵ����ϵͳ���й̻���־
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void RunHardFlagPowerOn(void)
{
	// ����Ӳ����־
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(RunHardFlagSafeRom), sizeof(TRunHardFlagSafeRom), (BYTE *)&FlagBytes.RunHardFlagSafeRom ) != TRUE )
	{
		memset( (BYTE *)&FlagBytes.RunHardFlagSafeRom, 0x00, sizeof(TRunHardFlagSafeRom) );
		// ��¼����
		api_SetError(ERR_GET_HARD_FLAG);
	}
}


//-----------------------------------------------
//��������: ����ϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		��Լ����ϵͳ�̻���־��
//                    
//����ֵ:  	TRUE:���óɹ�	FALSE:����ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_SetProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	i = (FlagNo%ePRO_HARD_NUM_T) / 8;
	j = FlagNo % 8;
	
	if( FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j) )//��λ�Ѿ���1������Ƶ��дEEPROM
	{
		return TRUE;
	}	

	FlagBytes.ProHardFlagSafeRom.Flag[i] |= (0x01<<j);

	// ���¼���У��
	lib_CheckSafeMemVerify((BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC);

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		// ��¼����
		api_SetError(ERR_SET_PRO_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ���ϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		ϵͳ��Լ���ù̻���־��
//                    
//����ֵ:  	TRUE:����ɹ�	FALSE:���ʧ��
//
//��ע:   
//-----------------------------------------------
BOOL api_ClrProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	i = (FlagNo%ePRO_HARD_NUM_T) / 8;
	j = FlagNo % 8;
	
	if( (FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j)) == 0 )//��λ�Ѿ����㣬����Ƶ��дEEPROM
	{
		return TRUE;
	}	
	FlagBytes.ProHardFlagSafeRom.Flag[i] &= ~(0x01<<j);

	// ���¼���У��
	lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC );

	if( api_VWriteSafeMem(  GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		// ��¼����
		api_SetError(ERR_SET_PRO_HARD_FLAG);
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------
//��������: ��ȡϵͳ��Լ���ù̻���־
//
//����: 
//			FlagNo[in]		ϵͳ��Լ���ù̻���־��
//                    
//����ֵ:  	TRUE:��Ӧ�̻���־��λ��	FALSE:��Ӧ�̻���־δ��λ�����ݲ���
//
//��ע:   
//-----------------------------------------------
BOOL api_GetProHardFlag(BYTE FlagNo)
{
	BYTE i, j;
	
	ASSERT( FlagNo < ePRO_HARD_NUM_T, 0 );

	// ����ڴ��б�־У���Ƿ���ȷ
	if( lib_CheckSafeMemVerify( (BYTE *)&FlagBytes.ProHardFlagSafeRom, sizeof(TProHardFlagSafeRom), REPAIR_CRC ) != TRUE )
	{
		// ������ȷ��Ӵ洢���ж�ȡ
		if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
		{
			// ��¼����
			api_SetError(ERR_GET_HARD_FLAG);
			return FALSE;
		}
	}

	i = ((FlagNo%ePRO_HARD_NUM_T)>>3);//�൱�ڳ���8
	j = (FlagNo&0x07);//�൱�ڶ�8��ģ

	if( FlagBytes.ProHardFlagSafeRom.Flag[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//��������: ��λϵͳ��Լ���ù̻���־
//
//����: 	��
//                    
//����ֵ:  	TRUE:��ʼ���ɹ�	FALSE:��ʼ��ʧ��
//
//��ע: �˺����Ƿ���Ҫ
//-----------------------------------------------
void api_FactoryInitProHardFlag(void)
{
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		memset(&FlagBytes.ProHardFlagSafeRom, 0, sizeof(TProHardFlagSafeRom));
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom );
	}
}


//-----------------------------------------------
//��������: �ϵ����ϵͳ��Լ���ù̻���־
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
static void ProHardFlagPowerOn(void)
{
	// ����Ӳ����־
	if( api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ProHardFlagSafeRom), sizeof(TProHardFlagSafeRom), (BYTE *)&FlagBytes.ProHardFlagSafeRom ) != TRUE )
	{
		memset( (BYTE *)&FlagBytes.ProHardFlagSafeRom, 0x00, sizeof(TProHardFlagSafeRom) );
		// ��¼����
		api_SetError(ERR_GET_HARD_FLAG);
	}
}

//-----------------------------------------------
//��������: ��ȡerr��Ϣ
//
//����: 
//			pErr[in]	����ָ��

//                    
//����ֵ:  	BYTE		���ݳ��� 
//
//��ע:   
//-----------------------------------------------
BYTE api_GetError( BYTE* pErr )
{
	memcpy( pErr, FlagBytes.ErrByte, (MAX_ERROR/8) );
	return (MAX_ERROR/8);
}


//-----------------------------------------------
//��������: ����ָ���Ĵ���
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//			BIT15		0  ������ϢҪд��EEPROM
//           			1  ������Ϣ��д��EEPROM
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetError(WORD Err)
{
	WORD i, j;
	WORD Temp;

	Temp = (Err&0x7fff);
	ASSERT( Temp < MAX_ERROR, 0 );
	//�������ڵ��ֽ�
	i = ((Temp%MAX_ERROR)>>3);
	//�������ڵ�bit
	j = (Temp&0x0007);
	
	//�����־����Ѿ����ڣ������ô����־����ֹ���дeeprom
	if( api_CheckError( Temp ) == TRUE )//ע��Ҫ��bit15ȥ�����ٴ�
	{
		return;
	}

	FlagBytes.ErrByte[i] |= (0x01<<j);

	if( (Err&0x8000) == 0 )
	{
		api_WriteSysUNMsg( Temp );
	}
}


//-----------------------------------------------
//��������: ��鵱ǰ�Ƿ����ָ���Ĵ���
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//                    
//����ֵ:  	TRUE:����ָ���Ĵ���  	FALSE:������ָ���Ĵ���
//
//��ע:   
//-----------------------------------------------
BOOL api_CheckError(WORD Err)
{
	WORD i, j;
	
	ASSERT( Err < MAX_ERROR, 0 );
	
	i = ((Err%MAX_ERROR)>>3);
	j = (Err&0x0007);
	
	if( FlagBytes.ErrByte[i] & (0x01<<j) )
	{
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------
//��������: ���ָ���Ĵ����־��
//
//����: 
//			Err[in]		ָ���Ĵ����־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrError(WORD Err)
{
	WORD i, j;

	ASSERT( Err < MAX_ERROR, 0 );
	
	i = ((Err%MAX_ERROR)>>3);
	j = (Err&0x0007);

	FlagBytes.ErrByte[i] &= ~(0x01<<j);
}


//-----------------------------------------------
//��������: ������������ͬһ����־
//
//����: 
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetAllTaskFlag( BYTE FlagID )
{
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	memset( &FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T], 0xff, sizeof(FlagBytes.TaskTimeFlag[FlagID]) );
}


//-----------------------------------------------
//��������: �������־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_SetTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T] |= ( (DWORD)1<<(TaskID) );
}


//-----------------------------------------------
//��������: ��ȡ�����־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	TRUE:��Ӧ��־��λ��  FALSE:��Ӧ��־δ��λ
//
//��ע:   
//-----------------------------------------------
BOOL api_GetTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	if( FlagBytes.TaskTimeFlag[FlagID] & ((DWORD)1<<TaskID) )
	{
		return TRUE;
	}
	
	return FALSE;
}


//-----------------------------------------------
//��������: ��������־
//
//����: 
//			TaskID[in]		�����
//			FlagID[in]		��־��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_ClrTaskFlag( BYTE TaskID, BYTE FlagID )
{
	ASSERT( TaskID < 8*sizeof(FlagBytes.TaskTimeFlag[0]), 0 );
	ASSERT( FlagID < eFLAG_TIMER_T, 0 );
	
	DISABLE_CLOCK_INT;//500ms��־λ��systick������ ������clock����
	DISABLE_SYSTICK_INT;
	FlagBytes.TaskTimeFlag[FlagID%eFLAG_TIMER_T] &= ~((DWORD)1<<(TaskID));
	ENABLE_SYSTICK_INT;
	ENABLE_CLOCK_INT;
}


//-----------------------------------------------
//��������: ��־�����ϵ紦��
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_PowerUpFlag( void )
{
	RunHardFlagPowerOn();
	ProHardFlagPowerOn();
	
	//�ж��ϵ粻��������Ƿ�ɿ���־
	if( NoInitDataFlag.CRC32 != lib_CheckCRC32( (BYTE*)&NoInitDataFlag, sizeof(NoInitDataFlag)-sizeof(DWORD) ) )
	{
		api_SetSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR);
	}
	else
	{
		api_ClrSysStatus(eSYS_STATUS_NO_INIT_DATA_ERR);
	}
}

//-----------------------------------------------
//��������: ��ʼ��_noinit_��־
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void api_InitNoInitFlag( void )
{
	NoInitDataFlag.Flag = 0x55aa88ee;
	NoInitDataFlag.CRC32 = lib_CheckCRC32( (BYTE*)&NoInitDataFlag, sizeof(NoInitDataFlag)-sizeof(DWORD) );
}

