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

//������� 0:��ʱ���� 1��ʱ�α��� 2 ��ʱ���� 3������ 4 ����������
const BYTE TimeSegMaxNum[] = {MAX_TIME_AREA,MAX_TIME_SEG_TABLE,MAX_TIME_SEG,MAX_RATIO,MAX_HOLIDAY};
//�±� 0--��ʱ������ 1--��ʱ�α����� 2--ʱ������ 3--�������� 4--������������
const WORD TimeSegParaOverError[] = { DLT645_ERR_OVER_AREA_10, DLT645_ERR_DATA_01, DLT645_ERR_OVER_SEG_20, DLT645_ERR_OVER_RATIO_40, DLT645_ERR_DATA_01 };

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
			//api_ResetBorrDotNum( 0 );
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
	//���
	else if( SubID0 == 0x02)
	{
		lib_ExchangeData( Buf, pBuf, 6 );
		if(api_ProcMeterTypePara( WRITE, eMeterMeterNo, Buf)== FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		
		return 6;
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
	else if(SubID0 == 0x1F)		// ��չ��Լ�����õ���λ�ã�4Gģ��ʹ��
	{
		// lib_InverseData(pBuf,18);

		if( (pBuf[0] > 1) || (pBuf[7] > 1))	//��γ�ȷ�λֻ����0��1
		{
			return DLT645_ERR_DATA_01;
		}
		
		if(api_ProcMeterTypePara( WRITE, eMeterMeterPosition, pBuf) == FALSE )
		{
			return DLT645_ERR_DATA_01;
		}
		return 18;
	}
	else if(SubID0 == 0x20)		// ��չ��Լ������͸�����Ĳ���
	{
		if( ( (pBuf[0]!=FALSE) && (pBuf[0]!=TRUE) )
		 || ( (pBuf[1]==5) || (pBuf[1]<2) || (pBuf[1]>10) ) // ��֧��7200������
		 || ( (pBuf[2]<5) || (pBuf[2]>100) )
		)
		{
			return DLT645_ERR_DATA_01;
		}

		MessageTransParaRam.AllowUnknownAddrTrans = pBuf[0];
		MessageTransParaRam.Baud = pBuf[1];
		MessageTransParaRam.Timeout = pBuf[2];
		if(api_VWriteSafeMem( GET_SAFE_SPACE_ADDR( ProSafeRom.MessageTransPara), sizeof(MessageTransPara_t), (BYTE*)&MessageTransParaRam ) == FALSE)
		{
			return DLT645_ERR_ID_02;
		}
		return 2;
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

WORD SetPara040013XX( BYTE SubID0, BYTE BufLen, BYTE *pBuf )
{
	BYTE Sum,i,Lenth;
	Sum = 0;
	if(SubID0 == 0xD1)
	{		
		for(i = 0; i < 22; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[22] = Sum;
		if( api_ProcGprsTypePara(WRITE, eTCPIP_IP_Port_APN, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 23;
	}
	else if(SubID0 == 0xD5)
	{		
		for(i = 0; i < 4; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[4] = Sum;
		if( api_ProcGprsTypePara(WRITE, eTCPIP_AreaCode_Ter, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 5;
	}
	else if(SubID0 == 0xD6)
	{
		for(i = 0; i < 64; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[64] = Sum;
		if( (api_ProcGprsTypePara(WRITE, eTCPIP_APN_UseName, pBuf) == FALSE) || (api_ProcGprsTypePara(WRITE, eTCPIP_APN_PassWord, pBuf+32) == FALSE))
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 65;
	}
	else if(SubID0 == 0xE1)
	{		
		for(i = 0; i < 22; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[22] = Sum;
		if( api_ProcGprsTypePara(WRITE, eMQTT_IP_Port_APN, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 23;
	}
	else if(SubID0 == 0xE5)
	{		
		for(i = 0; i < 4; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[4] = Sum;
		if( api_ProcGprsTypePara(WRITE, eMQTT_AreaCode_Ter, pBuf) == FALSE)
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 5;
	}
	else if(SubID0 == 0xE6)
	{
		for(i = 0; i < 64; i++)
		{
			Sum+=pBuf[i];
		}
		pBuf[64] = Sum;
		if( (api_ProcGprsTypePara(WRITE, eMQTT_APN_UseName, pBuf) == FALSE) || (api_ProcGprsTypePara(WRITE, eMQTT_APN_PassWord, pBuf+32) == FALSE))
		{
			return DLT645_ERR_DATA_01;
		}
		Lenth = 65;
	}
	else
	{
		return DLT645_ERR_ID_02;
	}
	return Lenth;
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
			wResult = SetPara040013XX( SubID0, Len, pBuf);
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

	// �͹�������ģʽ
	if(api_GetSysStatus(eSYS_STATUS_RUN_LOW_POWER) == TRUE)
	{
		return DLT645_ERR_ID_02;
	}

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

