//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	κ����
//��������	2016.9.2
//����		����DL/T645-2007��ԼԴ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if( SEL_DLT645_2007 == YES )

// -----------------------------------------------------------------------------
// DESCRIPTION: CRCУ��ĸ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
	0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40};

// -----------------------------------------------------------------------------
// DESCRIPTION: CRCУ��ĵ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5,
	0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B,
	0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
	0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
	0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2,
	0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F,
	0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB,
	0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25,
	0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C,
	0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8,
	0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D,
	0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73,
	0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57,
	0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A,
	0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E,
	0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
	0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};
//������� 0:��ʱ���� 1��ʱ�α��� 2 ��ʱ���� 3������ 4 ����������
const BYTE TimeSegMaxNum[] = {MAX_TIME_AREA,MAX_TIME_SEG_TABLE,MAX_TIME_SEG,MAX_RATIO,MAX_HOLIDAY};
//�±� 0--��ʱ������ 1--��ʱ�α����� 2--ʱ������ 3--�������� 4--������������
const WORD TimeSegParaOverError[] = { DLT645_ERR_OVER_AREA_10, DLT645_ERR_DATA_01, DLT645_ERR_OVER_SEG_20, DLT645_ERR_OVER_RATIO_40, DLT645_ERR_DATA_01 };
BYTE sCRCHi; // high byte of CRC initialized
BYTE sCRCLo; // low byte of CRC initialized
T_Iap tIap;
TResetDelayPara ResetDelayPara;
//-----------------------------------------------
//��������: ��ʱ�����ʱ�α��������
//
//����: 
//			AreaAndSegmentNum[in]	ʱ������ʱ������ �� ʱ�α���ʱ�θ���
//          pAreaAndSegment[in]		ʱ�����ʱ�α�
//         
//����ֵ:  	��
//��ע: 
//-----------------------------------------------
void api_SortAreaAndSegment( BYTE AreaAndSegmentNum, BYTE *pAreaAndSegment )
{
	BYTE i,j,i0,i1,i2,j0,j1,j2;
	BYTE Temp;

	for(i=0; i<AreaAndSegmentNum; i++)
	{
		for(j=i; j<AreaAndSegmentNum; j++)
		{
			i0 = i*3+0;
			j0 = j*3+0;
			i1 = i*3+1;
			j1 = j*3+1;
			i2 = i*3+2;
			j2 = j*3+2;

			//����մ��ڣ����յ��ڵ�ʱ����
			if( (pAreaAndSegment[i0] > pAreaAndSegment[j0])||
				( (pAreaAndSegment[i0] == pAreaAndSegment[j0])&&(pAreaAndSegment[i1] > pAreaAndSegment[j1]) ) )
			{
				Temp = pAreaAndSegment[i0];
				pAreaAndSegment[i0] = pAreaAndSegment[j0];
				pAreaAndSegment[j0] = Temp;

				Temp = pAreaAndSegment[i1];
				pAreaAndSegment[i1] = pAreaAndSegment[j1];
				pAreaAndSegment[j1] = Temp;

				Temp = pAreaAndSegment[i2];
				pAreaAndSegment[i2] = pAreaAndSegment[j2];
				pAreaAndSegment[j2] = Temp;
			}
		}
	}
}

//DealSetPassword(): �ɹ����� DLT_PRG_OK_00�����ɹ����� DLT_PRG_ERR_PASSWORD_04
WORD DealSetPassword( BYTE byLevel, BYTE *pBuf )
{
	BYTE byUseLevel;
	TMuchPassword TmpMuchPassword;

	TSerial *pSerial = GetSerial(pBuf);
	if( pSerial == FALSE )
	{
		//Ϊ���ͳ������,һ��û�����������������ǹ�Լ������Ĭ��ʹ�õ����뼶����02
		byUseLevel = MINGWEN_H_PASSWORD_LEVEL;
	}
	else
	{
		byUseLevel = pSerial->ProBuf[MAX_PRO_BUF-1];//����ʹ�õ����뼶��
	}

	if( ( byLevel != MINGWEN_H_PASSWORD_LEVEL )&&( byLevel != 0x04 ) )
	{
		return DLT645_ERR_ID_02;//���ܹ淶����Ҫ���֧��02��04������ 
	}

	if( ( (byLevel >= byUseLevel)||(byUseLevel == CLASS_2_PASSWORD_LEVEL) )
		&&( byLevel <= (MAX_645PASSWORD_LEVEL - 1) ) )
	{
		// ��дʱֻд���룬��д�ܼ����ܼ����±�����

		//����EEPROM�б�������� ���ܶ����Ƿ�ɹ����涼д������ֻ��һ��д���ɹ�
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
		
		pBuf[3] = pBuf[2];
		memcpy( (void*)&(TmpMuchPassword.Password645[byLevel][0]), (void*)&(pBuf[0]), 4 );

		//д�����룬�Լ�����У��
		api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.MuchPassword ), sizeof(TMuchPassword), (BYTE*)&TmpMuchPassword );
		
		return DLT645_OK_00;
	}
	else
	{
		//DL/T745_2007�����ļ���Err��D2: 1--�����/δ��Ȩ 0--�������/δ��Ȩ
		//�û�����������룬Ӧ��"�����/δ��Ȩ"
		return DLT645_ERR_PASSWORD_04;
	}
}

//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:040001XX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err

//04000101	YYMMDDWW	 4	 ����������	 ���ڼ�����(����0����������)
//04000102  hhmmss       3   ʱ����      ʱ��
//04000103  NN           1   ��          �����������
//04000104  NN           1   ��          ����ʱ��
//04000105  XXXX         2   ����        У�������� (ֻ��)
//04000106  YYMMDDhhmm   5   ������ʱ��  ����ʱ�����л�ʱ��
//04000107	YYMMDDhhmm	 5	 ������ʱ��	 ������ʱ���л�ʱ��
//04000108  YYMMDDhhmm 5	 ���׷�ʱ�����л�ʱ��
//04000109	YYMMDDhhmm 5 * * �����ݶ��л�ʱ��
//0400010A	YYMMDDhhmm 5 * * ���׽���ʱ���л�ʱ��
//0400010B	YYMMDDhhmm 5 * * ���׷ѿ�ģʽ�л�ʱ��
//0400010C  YYMMDDWWhhmmss 7 ͬʱд���ڡ����ڡ�ʱ��
//----------------------------------------------------------
const BYTE Para040001XXLen[] = {4,3,1,1,2,5,5,5,5,5,5,7};
WORD SetPara040001XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i, j;
	BYTE Buf[6];
	WORD Result;
	TRealTimer TmpRealTimer;//ʱ��ṹ��
	DWORD RelativeTime;
	TSwitchTimePara TmpSwitchTimePara;
	
	//04000101	YYMMDDWW	   4	 ����������	 ���ڼ�����(����0����������)
	//04000102  hhmmss         3     ʱ����      ʱ��
	//0400010C  YYMMDDWWhhmmss 7     ͬʱд���ڡ����ڡ�ʱ��
	if( ( SubID0 == 0x01 )||( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
	{
		//ȡ�û�����TSerial�е��ĸ�����
		//����: FALSE--���Ǵ��ڻ��� ����--���ص��Ǵ���Serial[]�ĵ�ַ
		if( GetSerial(pBuf) == FALSE )
		{
			//Ҫ��pBuf�����ǹ�Լ���壬����ҪΪ����Уʱ��¼ SaveAdjustTimeInfo()ʱ��ʡ��ջ
			return DLT645_ERR_DATA_01;
		}
	}

	if( ( SubID0 == 0x01 )||( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
	{
		api_ReadMeterTime( 0 );//�ȶ�һ�µ������ʱ��
		memcpy( (void *)&TmpRealTimer, (void *)&RealTimer, sizeof(TRealTimer) );
     
        if( ( SubID0 == 0x02 )||( SubID0 == 0x0c ) )
		{
			TmpRealTimer.Sec = lib_BBCDToBin(pBuf[0]);
            TmpRealTimer.Min = lib_BBCDToBin(pBuf[1]);
            TmpRealTimer.Hour = lib_BBCDToBin(pBuf[2]);
			
			//ʱ���֡���Ϸ����ж�
			if( (TmpRealTimer.Hour > 23) && (TmpRealTimer.Hour != 0xff) )
			{
				return DLT645_ERR_DATA_01;
			}
			if( (TmpRealTimer.Min > 59) && (TmpRealTimer.Min != 0xff) )	
			{
				return DLT645_ERR_DATA_01;
			}
			if( (TmpRealTimer.Sec> 59) && (TmpRealTimer.Sec != 0xff) )	
			{
				return DLT645_ERR_DATA_01;
			}
			if( SubID0 == 0x0c )
			{
				pBuf += 3;
			}				
		}
		if( ( SubID0 == 0x01 )||( SubID0 == 0x0c ) )
		{
			TmpRealTimer.Day = lib_BBCDToBin(pBuf[1]);
			TmpRealTimer.Mon = lib_BBCDToBin(pBuf[2]);
			TmpRealTimer.wYear = 2000+lib_BBCDToBin(pBuf[3]);
			
			//�ꡢ�¡��ա��Ϸ����ж�
			if( api_CheckMonDay( TmpRealTimer.wYear, TmpRealTimer.Mon, TmpRealTimer.Day ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		}
		api_WritePreProgramData( 0, 0x00020040 );  //Уʱ��¼ OAD = 40000200
        api_WriteMeterTime(&TmpRealTimer);
		api_SetAllTaskFlag( eFLAG_TIME_CHANGE );
		api_SavePrgOperationRecord( ePRG_ADJUST_TIME_NO );  //Уʱ��¼
		#if( SEL_DEMAND_2022 == NO )
		#if( MAX_PHASE != 1)		
		api_InitDemand( );//���óɹ������½��������ļ���
		#endif
		#endif

		#if(SEL_F415 == YES)
		SetCJFlag_m(0,MSF_TIME_PARA);
		#endif
		
		return Para040001XXLen[ SubID0-1 ];
	}
	//04000103  NN           1   ��          �����������
	//04000104  NN           1   ��          ����ʱ��
	#if( MAX_PHASE == 3 )
	else if( (SubID0 == 0x03) || (SubID0 == 0x04) )
	{
		pBuf[0] = lib_BBCDToBin(pBuf[0]);
		if( api_WritePara( 1,  GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.DemandPeriod)+(SubID0-0x03), 1, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		else
		{
			#if( MAX_PHASE != 1)
            api_InitDemand();//���óɹ������½��������ļ���	-���ʺ͵��������������¼���
            #endif
            
			return Para040001XXLen[ SubID0-1 ];
		}
	}
	#endif//#if( MAX_PHASE == 3 )
	else if( SubID0 == 0x05 )
	{
		return DLT645_ERR_PASSWORD_04;
	}

	//04000106	YYMMDDhhmm	 5	 ������ʱ��  ����ʱ�����л�ʱ��
	//04000107	YYMMDDhhmm	 5	 ������ʱ��  ������ʱ���л�ʱ��
	//04000108  YYMMDDhhmm   5	 ���׷�ʱ�����л�ʱ��
	//04000109	YYMMDDhhmm   5   �����ݶ��л�ʱ��
	#if( PREPAY_MODE == PREPAY_LOCAL )//���طѿر�
	else if( (SubID0 >= 0x06) && (SubID0 <= 0x09) )  
	#else
	else if( (SubID0 == 0x06) || (SubID0 == 0x07) )
	#endif	
	{
		if( lib_IsAllAssignNum(pBuf, 0x00, 5) == TRUE ) //�ж��Ƿ�Ϊ00
		{
			RelativeTime = 0;
		}
		else if( lib_IsAllAssignNum(pBuf, 0x99, 5) == TRUE ) //�ж��Ƿ�Ϊ99
		{
			RelativeTime = 0x99999999;
		}
		else
		{
			api_ConvertYYMMDDhhmm_TRealTimer( (TRealTimer *)&TmpRealTimer, pBuf );
			RelativeTime = api_CalcInTimeRelativeMin( &TmpRealTimer );//�������ʱ��
		}

		if( RelativeTime == ILLEGAL_VALUE_8F )//�Ƿ�����
		{
			return DLT645_ERR_DATA_01;
		}

		if( (RelativeTime == 0) || (RelativeTime == 0x99999999) )
        {
        	//���������ʱ����Ч��־
        	if( SubID0 == 0x06 )
        	{
        	    api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG1);
        	}
       	 	else if( SubID0 == 0x07 )
       	 	{
       	     	api_ClrRunHardFlag(eRUN_HARD_SWITCH_FLAG2);
       	 	}
        	else
        	{

        	}
		}

		Result = api_VReadSafeMem( GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof(TmpSwitchTimePara), (BYTE *)&TmpSwitchTimePara);
		TmpSwitchTimePara.dwSwitchTime[ SubID0 - 0x06 ] = RelativeTime;
		Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.SwitchTimePara),sizeof( TmpSwitchTimePara ), (BYTE *)&TmpSwitchTimePara );

		if( Result == TRUE )
		{
			api_SetAllTaskFlag( eFLAG_SWITCH_JUDGE );
			return Para040001XXLen[ SubID0-1 ];
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
	
	}	
	else
	{
		return DLT645_ERR_ID_02;
	}
}

//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:040002XX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000201	NN	  1	   ��	   ��ʱ����p��14
//04000202	NN    1    ��      ��ʱ�α���q��8
//04000203	NN    1    ��      ��ʱ����(ÿ���л���)m��14
//04000204	NN    1    ��      ������k��63
//04000205	NNNN  2    ��      ����������n��254
//04000206	NN	  1	   ��	   г����������
//04000207	NN	  1	   ��	*  �ݶ��� ������	---��������ʱ����֧��--songchen
//04000208	NN	  1	   ��	*  ��Կ������ HEX	---��������ʱ����֧��--songchen
//----------------------------------------------------------
WORD SetPara040002XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	    
    if( SubID0 != 0x08 )
    {	
	    if( lib_CheckBCD( pBuf[0] ) == FALSE )
	    {
			return DLT645_ERR_DATA_01;
		}
	}
	
	i = SubID0 - 1;

	if( i <= 4 )
	{
		if( pBuf[0] != 0xff )
		{
			pBuf[0] = lib_BBCDToBin(pBuf[0]);	
		}
		if( ( i == 4 )&&( pBuf[0] == 0xff ) )//����Ҫ�󣺹�������������ΪFF����ʾ������
		{
		}
		//��ʱ������BCD ��ʱ�α�����BCD ��ʱ������BCD ��������BCD
		//��ʱ��������������,�����ݳ���Ҫ��
		else if( pBuf[0] > TimeSegMaxNum[i] )
		{
			//����ԭ��
			return TimeSegParaOverError[i];
		}
		#if( MAX_HOLIDAY < 100 )
		else if( ( i == 4 )&&( pBuf[1] != 0x00 ) )//���ù���������ʱ, if( MAX_HOLIDAY < 100 ) ���ֽڱ�����0
		{
			return TimeSegParaOverError[4];
		}
		#endif

		if( api_WritePara( 0, (GET_STRUCT_ADDR(TFPara1, TimeZoneSegPara.TimeZoneNum) +i), 1, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	//���أ�0--û�����ý����� ����--��ʾ���ý���������
	return 1;
}

//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:040003XX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000301	NN	 1	��	�Զ�ѭ����ʾ����
//04000302	NN   1  ��  ÿ����ʾʱ��
//04000303	NN   1  λ  ��ʾ����С��λ��
//04000304	NN   1  λ  ��ʾ���ʣ����������С��λ��
//04000305	NN	 1	��	����ѭ����ʾ����
//04000306  NNNNNNNN	CT���
//04000307  NNNNNNNN	PT���
//04000308  NN 	  1 �� * * �ϵ�ȫ��ʱ��
//----------------------------------------------------------
const BYTE Para040003XXLen[] = {1,1,1,1,1,3,3,1};
WORD SetPara040003XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	WORD wAddr;
	DWORD dwData;

	i = SubID0 - 1;

	//���pBuf���岻������Ҫ�������ݣ����˳�
	if( Para040003XXLen[i] > Len )
	{
		return DLT645_ERR_DATA_01;
	}	
	//PT CT ���ݴ���������������ͬ--��ʱ��֧������ɾ��
	pBuf[0] = lib_BBCDToBin(pBuf[0]);	

	if( (i < 5)||(i == 7) )
	{
		if( i == 0 )
		{	
			if( pBuf[0] > MAX_LCD_LOOP_ITEM )//04000301 �Զ�ѭ����ʾ����
			{
				return DLT645_ERR_DATA_01;
			}
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DispItemNum[0]);
		}
		else if( i == 1 )
		{
		    if( (pBuf[0] > 20) || (pBuf[0] < 5) ) //ѭ����ʾʱ����5-20s
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.LoopDisplayTime);
		}
		else if( i == 2 )
		{
			#if( MAX_PHASE == 1 )
			if( pBuf[0] > 4 ) //����������ʾλ�����֧��4λ��ɽ��Ҫ��0-4���裬
			{
				return DLT645_ERR_DATA_01;
			}
			#endif//#if( MAX_PHASE == 1 )
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.EnergyFloat);
		}
		else if( i == 3 )
		{
		    if( pBuf[0] > 5 ) //����С��λ���֧��5λ
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DemandFloat);
		}
		else if( i == 4 )
		{	
			if( pBuf[0] > MAX_LCD_KEY_ITEM )
			{
				return DLT645_ERR_DATA_01;
			}
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.DispItemNum[1]);
		}
		else if( i == 7 )
		{
		    if( (pBuf[0] < 5) || (pBuf[0] > 30) )  //�ϵ�ȫ��ʱ��5~30����
		    {
                return DLT645_ERR_DATA_01;
		    }
			wAddr = GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer);
		}	
		else
		{
            return DLT645_ERR_DATA_01;
		}
		
		if( api_WritePara( 0,  wAddr, 1, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		if( i == 2 )
		{
			api_ResetBorrDotNum( 0 );
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	return Para040003XXLen[i];
}
//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:040004XX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000301	NN	 ͨ�ŵ�ַ
//040003XX	NN   �������ò���--�ݲ�������
//----------------------------------------------------------
WORD SetPara040004XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;
	BYTE Buf[6];

	if( SubID0 == 0x01 )
	{
		lib_ExchangeData( Buf, pBuf, 6 );
		if(api_ProcMeterTypePara( WRITE, eMeterCommAddr, Buf)== FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		
		return 6;
	}
	//��Ų�֧������
	else if( SubID0 == 0x02)
	{
		return DLT645_ERR_PASSWORD_04;
	}
	//�ʲ���������ݲ�֧������
	else if( SubID0 == 0x03)
	{
		return DLT645_ERR_PASSWORD_04;
	}
	//ֻ��������֧������
	else if(( SubID0 >= 0x02 ) && (SubID0 <= 0x0D))
	{
		return DLT645_ERR_PASSWORD_04;
	}
	else//��֧��������ݱ�ʶ���
	{
		return DLT645_ERR_ID_02;
	}

}

//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:040006XX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000601 �й���Ϸ�ʽ������
//04000602 04000603 �޹���Ϸ�ʽ1��2������
//----------------------------------------------------------
WORD SetPara040006XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE i;

	i = SubID0 - 1;
	
	#if(SEL_RACTIVE_ENERGY == YES)
	if( i > 2 )// 0 1 2
	{
		return DLT645_ERR_ID_02;
	}
	#else
	if( i > 0 )
	{
		return DLT645_ERR_ID_02;
	}
	#endif

	//04000601 �й���Ϸ�ʽ������ ����й�
	//Bit7	Bit6		Bit5	Bit4	Bit3		Bit2		Bit1		Bit0
	//����	����		����	����	�����й�	�����й�	�����й�	�����й�
	//									0����1��	0����1��	0����1��	0����1��
	if( api_WritePara(1, GET_STRUCT_ADDR(TFPara2, EnereyDemandMode.byActiveCalMode)+i, 1, pBuf) == FALSE )
	{
		return DLT645_ERR_DATA_01;
	}

	#if( PREPAY_MODE == PREPAY_LOCAL )
	//�����й���Ϸ�ʽ�����֣���Ҫˢ��һ�½���ֵ�ͽ��ݵ��
	//api_SetUpdatePriceFlag( ePriceChangeEnergy );
	#endif

	return 1;
}

//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:04000BXX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000B01		DDhh	2	��ʱ	ÿ�µ�1������
//04000B02      DDhh    2   ��ʱ    ÿ�µ�2������
//04000B03	    DDhh	2	��ʱ    ÿ�µ�3������
//----------------------------------------------------------
WORD SetPara04000BXX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE byNeedSwitchSave;
	BYTE i,Status;
	BYTE Buf[6];
	DWORD dwID;
	
	TBillingPara TmpBillingPara;

	i = SubID0 - 1;


	//���pBuf���岻������Ҫ�������ݣ����˳�
	if( 2 > Len )
	{
		return DLT645_ERR_DATA_01;
	}

	if( i < 3 )
	{
		pBuf[0] = lib_BBCDToBin(pBuf[0]);
		pBuf[1] = lib_BBCDToBin(pBuf[1]);
		//04000B01	DDhh	ÿ�µ�1������
		//04000B02  DDhh    ÿ�µ�2������
		//04000B03  DDhh    ÿ�µ�3������
		api_VReadSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		
		Buf[0] = TmpBillingPara.MonSavePara[0].Hour;
		Buf[1] = TmpBillingPara.MonSavePara[0].Day;
		
		
		byNeedSwitchSave = 0;
		if( i == 0 )
		{									
			if( (TmpBillingPara.MonSavePara[0].Hour != pBuf[0])||(TmpBillingPara.MonSavePara[0].Day != pBuf[1]) )
			{
				byNeedSwitchSave = 1;//����ı��һ�����գ���Ҫת���¶��õ����������յ���
			}
		}
		TmpBillingPara.MonSavePara[i].Hour = pBuf[0];
		TmpBillingPara.MonSavePara[i].Day = pBuf[1];

		if( api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE )//�ǳ���ģʽ�� 
		{	
			//����ȷ����һ������������Ч��
			Status = TRUE;
	
			//��ʱ���붼Ϊ99 �����ж�ʱ���Ƿ�Ϸ�
			if( (TmpBillingPara.MonSavePara[i].Day == 99)&&(TmpBillingPara.MonSavePara[i].Hour == 99) )
			{
				
			}
			else
			{
				if( (TmpBillingPara.MonSavePara[i].Day > 28)||(TmpBillingPara.MonSavePara[i].Day == 0) )
				{
					Status = FALSE;
				}
				
				if( TmpBillingPara.MonSavePara[i].Hour > 23  )
				{
					Status = FALSE;
				}
			}
	
			if( Status == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
		}

		api_WritePreProgramData( 0, 0x00021641 );  //41160200 ������
		Status = api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ParaSafeRom.BillingPara ),sizeof(TBillingPara), (BYTE *)&TmpBillingPara);
		if( Status == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		else
		{
			api_SavePrgOperationRecord( ePRG_CLOSING_DAY_NO ); 
			if( byNeedSwitchSave != 0 )//����ı��һ�����գ���Ҫת���¶��õ����������յ��� 
			{
				api_SetFreezeFlag( eFREEZE_CLOSING, 0 ); //���㶳��
			}
		}
		return 2;
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
}


//04000C01		NNNNNNNN	4		0������
//		...     ...    		...     ...
//04000C0A	    NNNNNNNN	4		9������
//���òα�����ʶ�뷶Χ:04000CXX
WORD SetPara04000CXX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	WORD wResult;
	if( Len < 4 )
	{
		return DLT645_ERR_DATA_01;
	}

	//SubID0-1��õ������뼶��
	//����ֻ��3���ֽڣ���Э������14Hд������4���ֽڣ���Э�鱨��ϰ�ߣ���Ϊ��һ�ֽ������뼶��ȡ��3���ֽڣ�2010-1-9
	//DealSetPassword(): �ɹ����� DLT_PRG_OK_00�����ɹ����� DLT_PRG_ERR_PASSWORD_04
	wResult = DealSetPassword( (SubID0-1), (BYTE *)&(pBuf[1]) );
	if( wResult == DLT645_OK_00 )
	{
		return 4;//���ݳ���
	}
	else
	{
		return wResult;//0x8000+ʧ��ԭ��
	}

}
//--------------------------------------------------
//��������:  ��Լ���ú���:���òα�����ʶ�뷶Χ:04000BXX
//         
//����:
//	 SubID0[in]:	��Լ��ʶDL/T645-2007������ֽ�DI0���ĸ��ֽ�0xDI3DI2DI1DI0��
//
//	 Len[in] :	pBuf������Ҫ���õ����ݵĳ���
//
//	 pBuf[in]��	����Ҫ���õ�����
//����ֵ���ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//
//��ע����:������ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//04000F00		���õ�ѹ����ϵ��
//----------------------------------------------------------
WORD SetPara04000FXX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	TSampleUIK Tsetbuf;
	DWORD dCrc;
	BYTE Result;

	#if(SEL_F415 == YES)
	if( SubID0 == 0x00 )
	{
		//04000F00���õ�����ѹϵ��
		lib_ExchangeData((BYTE*)&Tsetbuf.Uk[0], pBuf, sizeof(TSampleUIK) -sizeof(DWORD));
		if( TF415UIK.CRC32 != lib_CheckCRC32((BYTE*)&Tsetbuf,sizeof(TSampleUIK)-sizeof(DWORD)))
		{
			api_SetSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);//����������Ͳ�����ֵ�һƬû��дֻд�ڶ�ƬEEP�����⣬����д�ɹ�����True��д���ɹ�����False
			Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(SampleSafeRom.SampleUIKSafeRom.Uk),sizeof(TSampleUIK),(BYTE*)&Tsetbuf);
			api_ClrSysStatus(eSYS_STATUS_EN_WRITE_SAMPLEPARA);
			if(Result == FALSE)
			{
				return DLT645_ERR_DATA_01;
			}
			else
			{
				memcpy((BYTE*)&TF415UIK.Uk,(BYTE*)&Tsetbuf.Uk,sizeof(TSampleUIK));//дeep���Զ�����Tsetbuf��crc
				SetCJFlag_m(0,MSF_VOL_CUR_PARA);
				return DLT645_OK_00;
			}
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else if ( SubID0 == 0xB0 )
	{
		//04000fB0----6025_to_415�ɼ�����״̬
		Result = pBuf[0];
		if(( Result == 0) ||( Result == 1))
		{
			api_SetCollectionTaskStatus415(Result);
			return DLT645_OK_00;
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else if ( SubID0 == 0xB3 )
	{
		//04000fB3----�Ƿ�������ΪͨѶ�쳣��415����cpu��λ��0��Ĭ������1��������415��λ
		Result = pBuf[0];
		if(( Result == 0) ||( Result == 1))
		{
			ControlResetStatus = Result;
			return DLT645_OK_00;
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	#else	
	return DLT645_ERR_DATA_01;
	#endif//#if(SEL_F415 == YES)
}
//���òα�����ʶ�뷶Χ:040011XX
WORD SetPara040011XX( BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE Buf[30];
	WORD wAddr,Result;
	// 04001101	 NN	�������������1
	//Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	Bit0
	//����	����	����	����	����	����	����	���ÿ��ؿ��Ʒ�ʽ
	//														��0��ƽ��1���壩
	if( SubID0 == 0x01 )
	{
		//Bit2�������ϱ�ģʽ
		//Bit1��Һ��(1)(2)��������
		//Bit0�����ÿ��ؿ��Ʒ�ʽ
		if( pBuf[0] & BIT2 )//���������ϱ�
		{
            api_SetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
		}
		else
		{
            api_ClrRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS );
		}

		if( pBuf[0] & BIT1 )//Һ����ʾ����
		{
		    Buf[0] = 1;
            Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+9, 1, Buf);
		}
		else
		{
            Buf[0] = 0;
            Result = api_WritePara( 0,  GET_STRUCT_ADDR(TFPara1, LCDPara.PowerOnDispTimer)+9, 1, Buf);
		}

        if( Result == FALSE )
        {
            return DLT645_ERR_DATA_01;
        }
        
		if( pBuf[0] & BIT0 )//���ÿ��ؿ��Ʒ�ʽ ��ƽ
		{
		    Buf[0] = 0;
            Result = WriteRelayPara( 3, Buf );
		}
		else//����
		{
            Buf[0] = 1;
            Result = WriteRelayPara( 3, Buf );
		}
		
		if( Result == FALSE )
        {
            return DLT645_ERR_DATA_01;
        }
        
		return 1;
	}
    else
    {
		return DLT645_ERR_ID_02;
    }
}
WORD SetPara040014XX( BYTE SubID0, BYTE BufLen, BYTE *pBuf )
{
	BYTE Type;
	WORD wDataLen;
	TTwoByteUnion DataBuf;

	wDataLen = 0;

	if( SubID0 == 0x01 )
	{
		memcpy( DataBuf.b, pBuf, 2 );
		DataBuf.w = lib_WBCDToBin( DataBuf.w );
		WriteRelayPara( 2, DataBuf.b );
		
		wDataLen = 2;
	}
	else if( SubID0 == 0x05 )
	{
        pBuf[0] = lib_BBCDToBin( pBuf[0] );  
        api_SetReportResetTime(pBuf[0]);

		wDataLen = 1;	
	}	
	else
	{
		return DLT645_ERR_ID_02;
	}

	return wDataLen;
}

//���òα�����ʶ�뷶Χ:0400XXXX
WORD SetPara0400XXXX( eSERIAL_TYPE PortType, BYTE SubID1, BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	WORD wResult;

	wResult = DLT645_ERR_ID_02;

	switch( SubID1 )
	{
		case 0x01:
			wResult = SetPara040001XX(SubID0,  Len, pBuf);
			break;
		case 0x02:
			wResult = SetPara040002XX(SubID0,  Len, pBuf);
			break;
		case 0x03:
			wResult = SetPara040003XX(SubID0,  Len, pBuf);
			break;
		case 0x04:
			wResult = SetPara040004XX(SubID0,  Len, pBuf);
			break;
		case 0x05://�������״̬��ֻ��
			wResult = DLT645_ERR_PASSWORD_04;
			break;
		case 0x06:
			wResult = SetPara040006XX(SubID0,  Len, pBuf);
			break;
		case 0x07: //������������ ��ר�õ��������������Ҳ�ã������������һ���ڸı�Ŀ�����
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			wResult = SetPara04000BXX(SubID0,  Len, pBuf);
			break;
		case 0x0c://04000CXX ��0--9�����룬���ɶ�
			wResult = SetPara04000CXX(SubID0,  Len, pBuf);
			break;
		case 0x0e:
			break;
		#if( PREPAY_MODE == PREPAY_LOCAL )//���طѿر�
		case 0x10:
			break;
		#endif		
		case 0x11:
			wResult = SetPara040011XX(SubID0,  Len, pBuf);
			break;
		case 0x12:
			break;
		case 0x13:
			break;
		case 0x14:
			wResult = SetPara040014XX( SubID0, Len, pBuf);
			break;		
		case 0x15://04001501	XX��XX	12+1+1+..+1		*		�����ϱ�״̬��,�����ϱ��¼�1��������,�����ϱ��¼�2��������...�����ϱ��¼�N��������
			if( SubID0 == 0x01 )
			{
				wResult = (DLT645_ERR_PASSWORD_04);
			}
			//0x04001503 ��λ�����ϱ�״̬��
			else if( SubID0 == 0x03 )
			{
				wResult = api_ResetReportStatusByte( PortType, pBuf);

				if( wResult == FALSE)
				{
					wResult = DLT645_ERR_DATA_01;
				}
				else
				{
					wResult = DLT645_OK_00;
				}
			}	
			break;
		default:
			break;
	}
	//�������ý��������ݳ���
	return wResult;
}


//-----------------------------------------------
//��������: ����645-2007�е����ݱ�ʶΪ040100XX��ʱ����ʱ�α�
//
//����: 
//	SubID2[in]		645-2007�е����ݱ�ʶ��DI2
//	SubID0[in]		645-2007�е����ݱ�ʶ��DI0
//	Len[in]			Э�鴫���������ݳ���
//  pBuf[in]		�������ݵĻ���
//                    
//����ֵ:    �ɹ����������ݳ��ȣ��ֽڣ��� ʧ�ܣ�0x8000+Err
//��ע: 
//04010000:��һ��ʱ��������  04020000:�ڶ���ʱ��������
//04010001:��һ�׵�1��ʱ�α����� 04020001:�ڶ��׵�1��ʱ�α�����
//04010008:��һ�׵�8��ʱ�α����� 04020008:�ڶ��׵�8��ʱ�α�����
//--------------------------------------------------
WORD SetTimeSegPara040100XX( BYTE SubID2, BYTE SubID0, BYTE Len, BYTE *pBuf )
{
	BYTE TimeTableType, i;
	WORD wAddr;
	WORD wDataLen;
	DWORD dwID;
	BYTE Buf[MAX_TIME_AREA*3*2];
	BYTE TmpBuf[3];

	wDataLen = 0;

	// ���ó��ȼ�ȥ��3��������ÿ��ʱ����ʱ�ζ���3�ֽ� �������ݳ��� Len Ҫ��3��������

	if( Len < 3 )
	{
		return DLT645_ERR_DATA_01;
	}
	for( i=0; i<Len; i++ )
	{
		if( lib_CheckBCD(pBuf[i]) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}	
		pBuf[i] = lib_BBCDToBin(pBuf[i]);//��645��λ��������BCD��תΪHEX��698.45���EEPROM������HEX
	}

	//�������:3���ֽ�Ϊһ�飬���ڽ��е���
	for(i = 0; i < Len ; i += 3)
	{
		lib_InverseData(&pBuf[i], 3);
	}

	//У��ʱ��ʱ�α��Ƿ���ȷ���Ҷ���������FlashBuf��
	//Type
	//D7:	0--��һ��ʱ��ʱ�α� 1--�ڶ���ʱ��ʱ�α�  ���¹�Լ( SEL_DLT645_2007 ) ʱ�Ķ� 2008-7-19
	//D6--D0:
	//		1 -- TSubTimeTable1
	//		2 -- TSubTimeTable3
	//		3 - MAX_TIME_SEG_TABLE+2 -- TSubTimeTable2
	//WORD DbReadTimeTable(WORD Type, WORD Addr, WORD Length, BYTE * Buf)

	if( SubID0 == 0x00 )//04010000:��һ��ʱ��������  04020000:�ڶ���ʱ��������
	{
		if( Len > (MAX_TIME_AREA*3) )
		{
			return DLT645_ERR_OVER_AREA_10;
		}

		// ���ó��ȼ�ȥ��3��������ÿ��ʱ����ʱ�ζ���3�ֽ� �������ݳ��� Len Ҫ��3��������
		//Len -= (Len % 3 );
		if( (Len % 3 ) != 0 )
		{
			return DLT645_ERR_DATA_01;
		}

		wDataLen = ( MAX_TIME_AREA *3 );
		wAddr = GET_STRUCT_ADDR(TTimeAreaTable,TimeArea[0][0]);
		TimeTableType= 1;


		for( i=0; i<Len; i+=3 )
		{
			//698.45��ʱ���£��գ�ʱ�α�ţ�0��1��2����ʱ��ʱ���֣����ʺţ�0��1��2��,ע�����ǰ˳���෴
			if( api_CheckMonDay( RealTimer.wYear, pBuf[i], pBuf[i+1] ) == FALSE )
			{
				return DLT645_ERR_DATA_01;
			}
			else if( ( pBuf[i+2] == 0x00 )||( pBuf[i+2] > FPara1->TimeZoneSegPara.TimeSegTableNum ) )//MAX_TIME_SEG_TABLE )
			{
				return DLT645_ERR_DATA_01;
			}
		}

	}
	// 04010001:��һ�׵�1��ʱ�α����� 04020001:�ڶ��׵�1��ʱ�α�����
	// 04010008:��һ�׵�8��ʱ�α����� 04020008:�ڶ��׵�8��ʱ�α�����
	// �¹�Լ�����8����ʱ�α�
	else if( SubID0 <= MAX_TIME_SEG_TABLE )
	{
		if( Len > (MAX_TIME_SEG*3) )
		{
			return DLT645_ERR_OVER_SEG_20;
		}

		// ���ó��ȼ�ȥ��3��������ÿ��ʱ����ʱ�ζ���3�ֽ� �������ݳ��� Len Ҫ��3��������
		//Len -= (Len % 3 );
		if( (Len % 3 ) != 0 )
		{
			return DLT645_ERR_DATA_01;
		}

		wDataLen = ( MAX_TIME_SEG * 3 );
		wAddr = GET_STRUCT_ADDR(TTimeSegTable,TimeSeg[0][0]);
		TimeTableType = ( 2 + SubID0 );//ʱ�α�


		memcpy( (void *)Buf, (void *)pBuf, Len );
		api_SortAreaAndSegment( (Len/3), Buf);			
		Buf[Len+2]=Buf[2];
		Buf[Len+1]=Buf[1];//���油�ϵ�1ʱ�Σ���Сʱ��24�������ж����һ��ʱ�κ͵�һ��ʱ�εļ��
		Buf[Len]=Buf[0]+24;
						
        for( i=0; i<Len; i+=3 )
		{
			//���ÿ��ʱ�����ݵĺϷ���
			{
				if( i <= (Len-3) )//���һ��ʱ�κ͵�һ��ʱ�εļ��ҲҪ�ж� 2014-5-8
				{
					if(( ( api_JudgeTimeSegInterval((BYTE *)&(Buf[i])) == FALSE ))
						&&(api_GetSysStatus(eSYS_STATUS_INSIDE_FACTORY) == FALSE) )
					{
						return DLT645_ERR_DATA_01;
					}
				}
				//698.45��ʱ���£��գ�ʱ�α�ţ�0��1��2����ʱ��ʱ���֣����ʺţ�0��1��2��,ע�����ǰ˳���෴
				if( pBuf[i] > 23 )//ʱ
				{
					return DLT645_ERR_DATA_01;
				}
				else if( pBuf[i+1] > 59 )//��
				{
					return DLT645_ERR_DATA_01;
				}
				else if( ( pBuf[i+2] == 0x00 )||( pBuf[i+2] > FPara1->TimeZoneSegPara.Ratio ) )//MAX_RATIO )
				{
					return DLT645_ERR_OVER_RATIO_40;
				}
			}
		}
	}
	else
	{
		return DLT645_ERR_ID_02;
	}

	// ���Ҫ�������ݳ������֧�����ʱ��ʱ�γ��ȣ�ȡ���֧�����ʱ��ʱ�γ���
	if( Len > wDataLen )
	{
		Len = wDataLen;
	}
	
	if( SubID2 == 0x02 )
	{
		TimeTableType |= 0x80;
	}

	if( wDataLen > Len )
	{
		//��ʱ�β��������ʱ����ʱ�������һ����Ч�Ĳ���
		api_AddTimeSegParaExtDataWithLastValid( Len, wDataLen, pBuf );
		Len = wDataLen;
	}

	api_WriteTimeTable(TimeTableType, wAddr, wDataLen, pBuf);

	return wDataLen;
}                             
//--------------------------------------------------
//��������:  ��Լ���ú���DL/T645-2007SetDlt645_2007Data()
//         
//����:
//	 dwID:	��Լ��ʶDL/T645-2007���ĸ��ֽ�0xDI3DI2DI1DI0��
//	 Len :	pBuf������Ҫ���õ����ݵĳ���
//	 pBuf��	����Ҫ���õ�����
//����ֵ���ɹ�����ȡ���ݳ��ȣ��ֽڣ��� ʧ�ܣ�0
//��ע����:SetDlt645_2007Data()�� ��Լ��Ԥ���ѡ�CPU������Ҫ���ò����ĳ��򶼿��Ե��ô˺���
//		   :SetDlt645_2007Data()��ɹ��������ݳ���,��ʧ��,���� 0x8000+Err
//----------------------------------------------------------
WORD SetDlt645_2007Data( eSERIAL_TYPE PortType, BYTE Len, BYTE *pBuf )
{
	BYTE SubID2,SubID1, SubID0;
	WORD wDataLen;
	TFourByteUnion EventID;

	wDataLen = DLT645_ERR_ID_02;

	SubID0 = gPr645[PortType].dwDataID.b[0];
	SubID1 = gPr645[PortType].dwDataID.b[1];
	SubID2 = gPr645[PortType].dwDataID.b[2];
	//�������ú��������óɹ����������ý��������ݳ��ȣ�������ò��ɹ�������0��ʾû���������õ������ȥ
	switch( SubID2 )
	{
		case 0x00:
			wDataLen = SetPara0400XXXX( PortType, SubID1,  SubID0,  Len, pBuf);
			break;

		case 0x01://��һ��ʱ������һ�׵�1--8��ʱ�α�
			wDataLen = DLT645_ERR_PASSWORD_04;
			break;
		case 0x02://�ڶ���ʱ�����ڶ��׵�1--8��ʱ�α�
			wDataLen = SetTimeSegPara040100XX( SubID2, SubID0,  Len, pBuf );
			break;
			
		#if( PREPAY_MODE == PREPAY_LOCAL )//���طѿر�
		case 0x05:
			break;
		case 0x06:
			break;
		#endif//#if( PREPAY_MODE == PREPAY_LOCAL )//���طѿر�
		case 0x09://09-09-06��κ�����Ĺ��������ļ��й涨�¼�������ֵ
			break;
		case 0x80:
			break;
		case 0xdf://������չ���� wlk 2008-8-22
			break;
			
		default:
			break;
	}

	return wDataLen;
}

//--------------------------------------------------
//��������:  ����CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
//--------------------------------------------------
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen )
{
	unsigned char uchCRCHi;// high byte of CRC initialized
	unsigned char uchCRCLo;// low byte of CRC initialized
	unsigned uIndex;// will index into CRC lookup table

	uchCRCHi = 0xFF;
	uchCRCLo = 0xFF;

	while ( usDataLen-- )
	{
		// calculate the CRC
		uIndex = uchCRCHi^(unsigned char)(*puchMsg++);
		uchCRCHi = uchCRCLo^auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}

	return (uchCRCHi << 8 | uchCRCLo);
}

//--------------------------------------------------
//��������:  ���������ļ���CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
//--------------------------------------------------
unsigned short FileCalCRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	unsigned uIndex;		// will index into CRC lookup table

	while (usDataLen--)
	{
		// calculate the CRC
		uIndex = sCRCHi ^ (unsigned char)(*puchMsg++);
		sCRCHi = sCRCLo ^ auchCRCHi[uIndex];
		sCRCLo = auchCRCLo[uIndex];
	}

	return (sCRCHi << 8 | sCRCLo);
}
//--------------------------------------------------
//��������:  ����������������
//         
//����:
//         
//����ֵ:    DLT645_OK_00--�ɹ� ����--����ʧ������
//         
//��ע����: 
//--------------------------------------------------
WORD ProIap( eSERIAL_TYPE PortType )
{
	DWORD dwAddr;
	WORD wFrameNo,wCrc16=0,wTemp;
	BYTE *ProBuf;
	BOOL boResult;
	static WORD TotalCrc16 = 0,LastFrameNo = 0XFFFF;

	ProBuf = (BYTE *)&gPr645[PortType].pMessageAddr[22];

	if ((gPr645[PortType].dwDataID.d == 0x0f0f0f01) || (gPr645[PortType].dwDataID.d == 0x0f0f0f04) || (gPr645[PortType].dwDataID.d == 0x0f0f0f0a))
	{
		//��վ�������ݷ�������
		memset((BYTE *)&tIap.dwFileSize, 0x00, sizeof(tIap));
		memcpy((BYTE *)&tIap.dwFileSize, ProBuf, sizeof(tIap)-sizeof(DWORD)*2);
		//��ʼ��У����Ϣ
		TotalCrc16 = 0;
		LastFrameNo = 0XFFFF;
		sCRCHi = 0xFF;
		sCRCLo = 0xFF;
	}
	else if ((gPr645[PortType].dwDataID.d == 0x0f0f0f02) || (gPr645[PortType].dwDataID.d == 0x0f0f0f05) || (gPr645[PortType].dwDataID.d == 0x0f0f0f0b) )
	{
		// ��վ�·�����֡
		// ��ǰ���͵�֡��ţ�0Ϊ��һ֡ 2�ֽ�
		// �̶�����(���һ֡����Ĳ�����FFH����) 192 �ֽ�
		// ��֡�������ݵ�CRC16У�� 2�ֽ�

		// ���У��
		wCrc16 = CalCRC16(&ProBuf[2], LEN_OF_ONE_FRAME);
		memcpy((BYTE *)&wTemp, &ProBuf[2+192], sizeof(WORD));

		if( wTemp != wCrc16 )
		{
			return DLT645_ERR_DATA_01;
		}

		// ֡��ż��
		memcpy((BYTE *)&wFrameNo, ProBuf, sizeof(WORD));
        // testFrameNo = wFrameNo;//!!!!!!
		if ((wFrameNo >= MAX_FRAME_NUM) || (wFrameNo >= tIap.wAllFrameNo))
		{
			return DLT645_ERR_DATA_01;
		}

		// if ((testFrameNo == (tIap.wAllFrameNo - 2)) || (testFrameNo == (tIap.wAllFrameNo - 1)))
		// if( testFrameNo == (tIap.wAllFrameNo - 1) )
		// {
		// 	ttt = testFrameNo;
		// }
		dwAddr = IAP_CODE_ADDR;
		dwAddr += (wFrameNo / FRAMENUM_OF_ONE_SECTOR) * SECTOR_SIZE + (wFrameNo%FRAMENUM_OF_ONE_SECTOR)*LEN_OF_ONE_FRAME;
		boResult = api_WriteMemRecordData(dwAddr, LEN_OF_ONE_FRAME, &ProBuf[2]);

		if(LastFrameNo != wFrameNo)//���Բ�����У��
		{
			TotalCrc16 = FileCalCRC16(&ProBuf[2], LEN_OF_ONE_FRAME);
			LastFrameNo++;
		}

		if( boResult == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
	}
	else if (gPr645[PortType].dwDataID.d == 0x0f0f0f03)
	{
		// ��վ�������֡��DATA = 55H����ʾȷ��������DATA = AAH����ʾ����������
		if( ProBuf[0] == 0x55 )
		{
			if (TotalCrc16 != tIap.wCrc16)
			{
				return DLT645_ERR_DATA_01;
			}

			// for (wTemp = 0; wTemp < tIap.wAllFrameNo; wTemp++)
			// {
			// 	dwAddr = IAP_CODE_ADDR;
			// 	dwAddr += (wTemp / FRAMENUM_OF_ONE_SECTOR) * SECTOR_SIZE + (wTemp % FRAMENUM_OF_ONE_SECTOR) * LEN_OF_ONE_FRAME;
			// 	api_ReadMemRecordData(dwAddr, LEN_OF_ONE_FRAME, &ProBuf[2]);

			// 	wCrc16 = FileCalCRC16(&ProBuf[2], LEN_OF_ONE_FRAME);

			// 	CLEAR_WATCH_DOG;
			// }

			// if (wCrc16 != tIap.wCrc16)
			// {
			// 	return DLT645_ERR_DATA_01;
			// }

			//�������������ļ���CRC16У���룬�����·��Ľ��бȽ� !!!!!!
			tIap.dwIapFlag = IAP_FLAG;
			tIap.dwCrc32 = lib_CheckCRC32((BYTE *)&tIap.dwFileSize, sizeof(tIap) - sizeof(DWORD)); // ����У��

			api_WriteMemRecordData(IAP_INFO_ADDR, sizeof(tIap), (BYTE *)&tIap.dwFileSize);

			ResetDelayPara.ResetDelay = 3;//��ʱ3s��ʼ����,�ȴ��ظ��������֡
			ResetDelayPara.CRC32 = lib_CheckCRC32((BYTE *)&ResetDelayPara.ResetDelay, sizeof(ResetDelayPara.ResetDelay));
		}
		else
		{
			return DLT645_ERR_DATA_01;
		}
		
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	// else if (gPr645[PortType].dwDataID.d == 0x0f0f0f06)
	// {
	// 	// ��վ�������֡��DATA = 55H����ʾȷ��������DATA = AAH����ʾ����������
	// 	if (ProBuf[0] == 0x55)
	// 	{
	// 		// ��֤У����ȷ��
	// 		sCRCHi = 0xFF;
	// 		sCRCLo = 0xFF;

	// 		for (wTemp = 0; wTemp < tIap.wAllFrameNo; wTemp++)
	// 		{
	// 			dwAddr = IAP_CODE_ADDR;
	// 			dwAddr += (wTemp / FRAMENUM_OF_ONE_SECTOR) * SECTOR_SIZE + (wTemp % FRAMENUM_OF_ONE_SECTOR) * LEN_OF_ONE_FRAME;
	// 			api_ReadMemRecordData(dwAddr, LEN_OF_ONE_FRAME, &ProBuf[2]);

	// 			wCrc16 = FileCalCRC16(&ProBuf[2], LEN_OF_ONE_FRAME);

	// 			CLEAR_WATCH_DOG;
	// 		}

	// 		if (wCrc16 != tIap.wCrc16)
	// 		{
	// 			return DLT645_ERR_DATA_01;
	// 		}

	// 		// ��2026����
	// 		ApiStartRN2026Iap();
				
	// 	}
	// 	else if (ProBuf[0] == 0xAA)
	// 	{

	// 	}
	// 	else
	// 	{
	// 		return DLT645_ERR_DATA_01;
	// 	}
	// }
	
	return DLT645_OK_00;
}
//--------------------------------------------------
//��������:  645-2007Э����������ܱ�ͨ�ŵ�ַ����֧�ֵ�Ե�ͨ�š�
//         
//����:      BYTE *ProBuf[in]
//         		ProBuf: ����ͨ�Ż���
//         
//����ֵ:    DLT645_OK_00--�ɹ� ����--����ʧ������
//         
//��ע����: 
//--------------------------------------------------
WORD DealSetDlt645_2007Data0X14( eSERIAL_TYPE PortType )
{
	WORD i;
	WORD wDataLen,ReturnStatus;
	BYTE *ProBuf;
	WORD wResult;

	ProBuf = (BYTE *)gPr645[PortType].pMessageAddr;
	
	wDataLen = DLT645_ERR_ID_02;
	ReturnStatus = 0;//����������벻��98���� Inverse ʱ������0��������ڴ� wlk
	
	//#define CLASS_2_PASSWORD_LEVEL				0x98//�������ݼ��ܲ������뼶�����Ϊ98H
	//#define CLASS_1_PASSWORD_LEVEL				0x99//һ�����ݼ��ܲ������뼶�����Ϊ99H
	
	if( (ProBuf[14] == CLASS_2_PASSWORD_LEVEL)||(ProBuf[14] == CLASS_1_PASSWORD_LEVEL) )
	{
		if( ProBuf[14] == CLASS_2_PASSWORD_LEVEL )
		{
			if( ProBuf[9] < 16 )
			{
				return DLT645_ERR_DATA_01;
			}
			//����
			lib_InverseData( ProBuf+22,ProBuf[9]-16 );
			//MAC
			lib_InverseData( ProBuf+22+ProBuf[9]-16, 4 );
			ReturnStatus = api_DecipherSecretData(0x83,ProBuf[9]-12,ProBuf+22, ProBuf+22 );//Ϊ����02�����룬���ݻ��ǻָ���02�����ֵ���ʽ
			if(ReturnStatus & 0x8000)
			{
				return DLT645_ERR_PASSWORD_04;
			}	
			
			//ʱ����ʱ�α� ���ݿ���Ҫ������ر��� 04010000-04010008   04020000-04020008
			if(((gPr645[PortType].dwDataID.d>=0x04010000)&&(gPr645[PortType].dwDataID.d<=0x04010008))||((gPr645[PortType].dwDataID.d>=0x04020000)&&(gPr645[PortType].dwDataID.d<=0x04020008)))
			{
				lib_InverseData(ProBuf+22,ReturnStatus);
				memmove( ProBuf+22, ProBuf+22+4, ReturnStatus-4 );//�����ݱ�ʶȥ��
				
				for( i=0; i<((ReturnStatus-4)/3); i++)
				{
					lib_InverseData(ProBuf+22+(i*3),3);//698.45��ʱ���£��գ�ʱ�α�ţ�0��1��2����ʱ��ʱ���֣����ʺţ�0��1��2��,ע�����ǰ˳���෴
				}
			}

		}
		else
		{	
			return DLT645_ERR_PASSWORD_04;
		}
	}
	
	//68 A0...A5 68 14 L DIODI1DI2DI3+PAP0P1P2+C0C1C2C3+DATA
	//wDataLen:���ý��������ݳ��ȣ����==0����ʾû���������õ������ȥ�����ش�������������
	if( wDataLen == DLT645_ERR_ID_02 )
	{
		if(ProBuf[14] == CLASS_2_PASSWORD_LEVEL)
		{	
			wDataLen = SetDlt645_2007Data( PortType, (ReturnStatus-4), (ProBuf+22) );
		}
		else
		{
			wDataLen = SetDlt645_2007Data( PortType, (ProBuf[9] - 12), (ProBuf+22) );
		}
	}
	if( wDataLen < 0xff )
    {
		#if(SEL_PRG_RECORD645 == YES)
		//��¼645��̼�¼��645����������Դ���洢��eeprom��
		api_SaveProgramRecord645( EVENT_START, gPr645[PortType].byOperatorCode, gPr645[PortType].dwDataID.b );
		#endif
		return DLT645_OK_00;        
    }   
	
	return wDataLen;
}

#endif

