//------------------------------------------------------------------------------------------------------------
// ���������ߵ�����ƽ���¼���� 
//------------------------------------------------------------------------------------------------------------

#include "AllHead.h"
#if( SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )
static const DWORD NeutralCurrentErrOad[] =
	#if(MAX_PHASE == 1)
	{
		0x02,				// ����
		0x20012201, // �¼�����ʱ��A�����
		0x20012400, // �¼�����ʱ�����ߵ���
	};
	#else
	{
		0x02,				// ����
		0x20012400, // �¼�����ʱ�����ߵ���
		0x20012600, // �¼�����ʱ������������������ʸ���ͣ�
	};
	#endif
#endif

#if( SEL_EVENT_SP_I_UNBALANCE == YES )
// �жϵ��������ߵ�����ƽ�� 
BYTE GetSPIUnbalanceStatus(void)
{
	/*
	WORD w;
	BYTE b, BigChn;
	DWORD dI[2];
  DWORD d;

	dI[0] = RamDBase.RemoteData.I[0];
	dI[1] = GetZeroCurr();
	
	//-----------------------
	// �õ�����ͨ����ֵ��ͬʱ�ҵ������ϴ�ͨ�� 
	if( dI[0] > dI[1] )
	{
		d = dI[0] - dI[1];
		BigChn = 0;
	}
	else
	{
		d = dI[1] - dI[0];
		BigChn = 1;
	}
	
	//-----------------------
	// �жϵ����Ƿ񳬹�����ֵ 
	DealThresholdPara(0, 0xda, 2, (BYTE *)&w);
	
	// ������С���������� 
	if( dI[BigChn] <= w )
	{
		// �򲻽��в�ƽ���ж� 

		// ��״̬�� 
		api_ClrSysStatus(SYS_STATUS_SPI_UN_B);
		return 0;
	}
	
	//-----------------------
	// ȡ��ֵ 
	d = (d*100) / dI[BigChn];
	
	//-----------------------
	#if( SEL_PARA_SYS_THRESHOLD == YES )
	// ��ȡ��ƽ������ֵ	
	DealThresholdPara(0, 0xDD, 1, (BYTE *)&b);
	#else
	// �粻����,��Ĭ��Ϊ50%
	b = 50;
	#endif
	
	// �Ƿ񳬹����� 	
	if(d > b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	return 0;
}

// ���������ߵ�����ƽ�⿪ʼ 
void DealSPIUnbalanceBegin(void)
{
	/*
	BYTE i, Point;
	WORD TempI[2];
	DWORD Addr;
	
	//-------------------------------------
	// ȡ��ָ��
	Point = DealEventPoint(0, EVENT_SP_I_UNBALANCE_NO, 0, 0);
	
	// ���¼���¼�洢�ṹ�Ŀ�ʼ��ַ
	Addr = GetEventDataExtAddr(EVENT_SP_I_UNBALANCE_NO, 0, Point);

	// ��¼����ʱ�̵���ֵ 
	for(i=0; i<2; i++)
	{
		// ����i 
		TempI[i] = (WORD)(CS5464_I[i]/10);
		
		// ת��BCD�� 
		TempI[i] = lib_WBinToBCD(TempI[i]);
	}
	
	// д��
	api_WriteToContinueEEPRom(Addr, sizeof(TempI), (BYTE*)&(TempI[0]));
	*/
}

// ���������ߵ�����ƽ����� 
void DealSPIUnbalanceEnd(void)
{
	// û�и������ݣ�����Ҫ����
}

// ��ȡ����������ƽ���¼  
// No -- 0���ܴ������ۼ�ʱ�� 
//     1-A����n�� 
// pBfu --- �洢���������� 
// ���� --- ʵ�ʶ��������ݳ��� 
BYTE ReadSPIUnbalance(BYTE No, BYTE BufLen, BYTE *pBuf)
{
	/*
	BYTE Len, Point;
	DWORD Addr;
	
	if(No > EVENT_MAX_TIMES)
	{
		return 0xff;
	}
	
	if(No == 0)
	{
		return ReadEventCommon(EVENT_SP_I_UNBALANCE_NO, 0, 0, 1, 0, BufLen, pBuf);
	}
	else
	{
		// ȡ��ָ��
		Point = DealEventPoint(2, EVENT_SP_I_UNBALANCE_NO, 0, No);
		
		// ����־λ
		if(DealEventFlag(0, EVENT_SP_I_UNBALANCE_NO, 0, Point) == FALSE)
		{
			// ���ݳ��� 
			Len = 12 + sizeof(TSPIUnbalanceData);
		
			// ������
			memset(pBuf, 0, Len);
		}
		else
		{
			// ��ȡʱ��
			Len = ReadEventCommon(EVENT_SP_I_UNBALANCE_NO, 1, 0, 1, No, 12, pBuf);
			
			// ��������ݿ��ַ
			Addr = GetEventDataExtAddr(EVENT_SP_I_UNBALANCE_NO, 0, Point);
			
			// ��ȡ����ֵ
			Len += api_ReadFromContinueEEPRom(Addr, sizeof(TSPIUnbalanceData), (pBuf + Len));	
		}	
	
		// ����
		ASSERT( Len <= BufLen, 1 );
		
		return Len;
	}
	*/
	return 0;
}

#endif

#if(SEL_EVENT_NEUTRAL_CURRENT_ERR == YES )

//-----------------------------------------------
//��������: ��ʼ���¼�����ֵ,�����������Ա�
//
//����: ��
//                    
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void FactoryInitNeutralCurrentErrPara( void )
{	
	api_WriteEventAttribute( 0x3040, 0xFF, (BYTE *)&NeutralCurrentErrOad[0], sizeof(NeutralCurrentErrOad)/sizeof(DWORD) );// ���ߵ����쳣
	
}
//-----------------------------------------------
//��������: �ж��Ƿ����ߵ�����ƽ��
//
//����:  ��
//                    
//����ֵ:	����״̬		FALSE������
//					TRUE�����ߵ�����ƽ��״̬
//
//��ע: 
//-----------------------------------------------
BYTE GetNeutralCurrentErrStatus(BYTE Phase)
{
	DWORD dwRemoteI_A,dwRemoteI_N,dwRemoteI_Max,dwRemoteI_Dis,dwLimitI;
	WORD wRatioI;//��ȡ�Ĵ�������

	//ӵ�����ߵ�����������
	if(SelZeroCurrentConst == NO )
	{
		return FALSE;
	}
	//���ߵ����쳣��ƽ���ʴ�����ֵ
	DealEventParaByEventOrder( READ, eEVENT_NEUTRAL_CURRENT_ERR_NO, eTYPE_WORD, (BYTE *)&wRatioI );
	if(wRatioI == 0)//��ƽ���ʴ�����ֵΪ0��ʾ������
	{
		return FALSE;
	}
	//���ߵ����쳣������������
	DealEventParaByEventOrder( READ, eEVENT_NEUTRAL_CURRENT_ERR_NO, eTYPE_DWORD, (BYTE *)&dwLimitI );
	
	#if(MAX_PHASE == 1)
	//��A�����
	api_GetRemoteData(PHASE_A+REMOTE_I, DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_A);
	#else
	//���������
	api_GetRemoteData(PHASE_N2+REMOTE_I,DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_A);
	#endif
	//��N�����
	api_GetRemoteData(PHASE_N+REMOTE_I, DATA_HEX, 4, 4, (BYTE *)&dwRemoteI_N);
	if(dwRemoteI_A > dwRemoteI_N)//�������ݸ�ʽΪDATA_HEX��ȷ���˵���Ϊ��ֵ     
	{
		dwRemoteI_Max = dwRemoteI_A;
		dwRemoteI_Dis =	dwRemoteI_A - dwRemoteI_N;
	}
	else
	{
		dwRemoteI_Max = dwRemoteI_N;
		dwRemoteI_Dis =	dwRemoteI_N - dwRemoteI_A;
	}
	if(dwRemoteI_Max <= dwLimitI)//���ߵ����ͻ��ߵ�������һ�������ڵ�����������
	{
		return FALSE;
	}

	//�������ߵ�������ߵ����ı�ֵ��
    if((WORD)((QWORD)dwRemoteI_Dis * 10000 / dwRemoteI_Max ) > wRatioI) 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#endif
