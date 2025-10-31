//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з���
//������	���ĺ�
//��������	2016.12.27
//����		DL/T 698.45�������Э����ܶ�ȡC�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#if ( SEL_DLT698_2016_FUNC == YES)
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

static const BYTE T_MeterStatus[]		= { Array_698, 0x07, Bit_string_698,         16  };		//�з����������ݱ�ʶ
static const BYTE T_AH[]				= { Array_698, 0x04, Double_long_unsigned_698, 4       };		//�з����������ݱ�ʶ
static const BYTE T_RunTime[]			= //��ѹ�ϸ���
{ 
	Structure_698, 				0x05, 
	Double_long_unsigned_698,	4, 
	Long_unsigned_698,			2, 
	Long_unsigned_698,			2, 
	Double_long_unsigned_698,	4, 
	Double_long_unsigned_698,	4
};	

static const BYTE T_EventNowData[]		= //�¼���ǰֵ
{ 
	Structure_698, 				0x02, 
	Double_long_unsigned_698, 	4, 
	Double_long_unsigned_698, 	4
};	

static const BYTE T_MoneyBag[]      = //Ǯ���ļ�
{ 
    Structure_698,              0x02, 
    Double_long_unsigned_698,   4, 
    Double_long_unsigned_698,   4
};  

static const BYTE T_NI[]				= { Double_long_698, 	4 	};						//���ߵ������ݱ�ʶ
#if(MAX_PHASE == 1)
static const BYTE T_U[]					= { Array_698, 0x01, Long_unsigned_698,	2	};		//��ѹ���ݱ�ʶ
static const BYTE T_I[]					= { Array_698, 0x01, Double_long_698, 	4 	};		//�������ݱ�ʶ
static const BYTE T_P[]					= { Array_698, 0x02, Double_long_698, 	4 	};		//�������ݱ�ʶ
static const BYTE T_COS[]				= { Array_698, 0x02, Long_698, 			2 	};		//�������ݱ�ʶ
#else
static const BYTE T_U[]					= { Array_698, 0x03, Long_unsigned_698,	2 	};		//��ѹ���ݱ�ʶ
static const BYTE T_I[]					= { Array_698, 0x03, Double_long_698, 	4 	};		//�������ݱ�ʶ
static const BYTE T_P[]					= { Array_698, 0x04, Double_long_698, 	4 	};		//�������ݱ�ʶ
static const BYTE T_COS[]				= { Array_698, 0x04, Long_698, 			2 	};		//�������ݱ�ʶ
static const BYTE T_PHASE[]				= { Array_698, 0x03, Long_unsigned_698,	2 	};		//����-��ѹ����������ݱ�ʶ
#endif
static const WORD T_RemotePara[] = 
{	
	PHASE_A,	//��ѹ
	PHASE_A,	//����
	PHASE_A,	//��ѹ���
	PHASE_A,	//��ѹ�������
	PHASE_ALL,	//�й�����
	PHASE_ALL,	//�޹�����
	PHASE_ALL,	//���ڹ���
	PHASE_ALL,	//һ����ƽ���й�����
	PHASE_ALL,	//һ����ƽ���޹�����
	PHASE_ALL,	//һ����ƽ�����ڹ���
	PHASE_ALL,	//��������
	PHASE_ALL,	//��ѹ����ʧ���
	PHASE_ALL,	//��������ʧ���
	PHASE_ALL,	//��ѹг��������
	PHASE_ALL,	//����г��������
	PHASE_ALL,	//����Ƶ��
};
//��������������鿴Դ���ݺͼ�TAG�Ƿ�֧��
static const TCommonObj VariableObj[] =
{
	{  0x2000,  &SU_1V,   	T_U				},//��ѹ
	{  0x2001,  &SU_3A,   	T_I				},//����
#if( MAX_PHASE != 1 )	
	{  0x2002,  &SU_10,  	T_PHASE			},//��ѹ���
	{  0x2003,  &SU_10,  	T_PHASE			},//��ѹ�������
	{  0x2005,  &SU_1VAR,  	T_P		 		},//�޹�����
	{  0x2008,  &SU_1VAR,  	T_P		 		},//һ����ƽ���޹�����
	{  0x2009,  &SU_1VA,  	T_P		 		},//һ����ƽ�����ڹ���	
	{  0x2017,  &SU_4KW, 	T_DoubleLong	},//��ǰ�й�����
	{  0x2018,  &SU_4KVAR, 	T_DoubleLong	},//��ǰ�޹�����
	#if( SEL_DEMAND_2022 == NO )
	{  0x2019,  &SU_4KVA, 	T_DoubleLong	},//��ǰ��������
	#endif
	{  0x2026,  &SU_2PCT, 	T_UNLong 		},//��ѹ��ƽ����
	{  0x2027,  &SU_2PCT, 	T_UNLong		},//������ƽ����
	{  0x2028,  &SU_2PCT, 	T_UNLong 		},//������
	{  0x2029,  &SU_2AH, 	T_AH 			},//��ʱֵ
#endif
	{  0x2004,  &SU_1W,  	T_P				},//�й�����
	{  0x2006,  &SU_1VA,  	T_P 			},//���ڹ���
	{  0x2007,  &SU_1W,  	T_P 			},//һ����ƽ���й�����
	{  0x200A,  &SU_3,		T_COS			},//��������
	{  0x200F,  &SU_2HZ,  	T_UNLong		},//����Ƶ��
	{  0x2010,  &SU_10C, 	T_Long			},//�����¶�
	{  0x2011,  &SU_2v, 	T_UNLong		},//ʱ�ӵ�ص�ѹ
	{  0x2012,  &SU_2v, 	T_UNLong		},//ͣ�糭���ص�ѹ
	{  0x2013,  &SU_0MIN, 	T_UNDoubleLong	},//ʱ�ӵ�ع���ʱ��
	{  0x2014,  &SU_00, 	T_MeterStatus 	},//�������״̬��
	{  0x2015,  &SU_00, 	T_BitString32 	},//�������״̬��
	{  0x2031,  &SU_2kwh, 	T_UNDoubleLong	},//�¶��õ���
	{  0x2040,  &SU_00, 	T_BitString16	},//��������ִ��״̬��
	{  0x2041,  &SU_00, 	T_BitString16 	},//�����������״̬��
    #if( PREPAY_MODE == PREPAY_LOCAL )
    {  0x201a,  &SU_4Yuan, 	T_UNDoubleLong 	},//��ǰ���
    {  0x201b,  &SU_4Yuan, 	T_UNDoubleLong 	},//��ǰ���ʵ��
    {  0x201c,  &SU_4Yuan, 	T_UNDoubleLong 	},//��ǰ���ݵ��
    {  0x202c,  &SU_2Yuan, 	T_MoneyBag 	    },//Ǯ���ļ�
    {  0x202d,  &SU_2Yuan, 	T_UNDoubleLong 	},//͸֧���
    {  0x202e,  &SU_2Yuan,  T_UNDoubleLong  },//�ۼƹ�����
    {  0x2032,  &SU_2kwh,   T_UNDoubleLong  },//���ݽ����õ���
	
    #endif
	{  0x2700,  &SU_1V,  	T_UNLong		},
	{  0x2701,  &SU_3A,  	T_DoubleLong	},
	{  0x2702,  &SU_1V,  	T_UNLong		},
	{  0x2704,  &SU_10C,  	T_UNLong 		},
	{  0x2705,  &SU_00,  	T_UNLong 		},
	{  0x2706,  &SU_10C, 	T_Long			},
	{  0x2707,  &SU_10C,  	T_Long	 		},
	{  0x2708,  &SU_1V, 	T_UNLong		},
	{  0x2709,  &SU_00,  	T_UNLong 		},
    #if( SEL_STAT_V_RUN == YES )	
	{  0x2131,  &SU_00, 	T_RunTime 		},//����A���ѹ�ϸ���
	{  0x2132,  &SU_00, 	T_RunTime 		},//����B���ѹ�ϸ���
	{  0x2133,  &SU_00, 	T_RunTime 		},//����C���ѹ�ϸ���
    #endif	
//��TAGר��
	{  0x201e,  &SU_00, 	T_DateTimS		},//�¼�����ʱ��
	{  0x2020,  &SU_00, 	T_DateTimS		},//�¼�����ʱ��
	{  0x2021,  &SU_00, 	T_DateTimS		},//���ݶ���ʱ��
	{  0x2022,  &SU_00, 	T_UNDoubleLong	},//�¼���¼���
	{  0x2023,  &SU_00, 	T_UNDoubleLong	},//�����¼���
	{  0x2024,  &SU_00, 	T_Enum			},//�¼�����Դ
	{  0x2025,  &SU_00, 	T_EventNowData	},//�¼���ǰֵ
	{  0x2051,  &SU_00, 	T_EventNowData	},
};								
//-----------------------------------------------

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  ����OI���Ҷ�Ӧ��������
//         
//����:      OI[in]:	OI
//         
//����ֵ:    ��������������
//         
//��ע:  
//--------------------------------------------------
static BYTE SearchVariableOAD( WORD OI )
{
	BYTE i,Num;//����������ܳ���255

	Num = (sizeof(VariableObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//������ѭ��
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == VariableObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//δ�ҵ�
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//��������:  ��ȡ��������
//         
//����:      
//			Dot[in]:		��ȡ���ݵ�С����λ����oxff--��ʾ��ԼĬ�ϵ�С��λ������Լ���ã�
//			*pOAD[in]:		OAD         
//			VariableIndex[in]:VariableObj�е�����         
//			*OutBuf[in]:	�������
//         
//����ֵ:   ���ر������ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע: ����tag
//--------------------------------------------------
static WORD GetVariableData( BYTE Ch, BYTE Dot, BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex,Num,i;
	WORD VariableType,Result,Len,IType;
	BYTE *pBufBak = InBuf;
	#if( PREPAY_MODE != PREPAY_NO )
	TFourByteUnion Money;
	#endif
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Len =0;
	switch( ClassAttribute )
	{
		case 2://�ܼ����������������
			switch( OI.w )
			{
				case 0x2000://��ѹ
				case 0x2001://����
				case 0x2002://��ѹ���
				case 0x2003://��ѹ�������
				case 0x2004://�й�����
				case 0x2005://�޹�����
				case 0x2006://���ڹ���
				case 0x2007://һ����ƽ���й�����
				case 0x2008://һ����ƽ���޹�����
				case 0x2009://һ����ƽ�����ڹ���
				case 0x200A://��������
					if( ClassIndex > VariableObj[VariableIndex].Type2[1])
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, (BYTE*)&VariableObj[VariableIndex].Type2[2] );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Num = ((ClassIndex == 0) ? (VariableObj[VariableIndex].Type2[1]) : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					for( ; i<Num ; i++ )
					{
						VariableType = (T_RemotePara[(OI.w-0x2000)]+i*0x1000+(OI.w-0x2000));					
						Result = api_GetRemoteData( VariableType, DATA_HEXCOMP, Dot, Len, InBuf );
						if( Result == FALSE )
						{
							return 0x8000;
						}
						InBuf += Len;
					}
					break;
				case 0x200F://����Ƶ��
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
    			    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetRemoteData( 0x0f, DATA_HEXCOMP, Dot, Len, InBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;

				case 0x2010://�����¶�
				case 0x2011://ʱ�ӵ�ص�ѹ
				//case 0x2012://ͣ�糭���ص�ѹ
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					
					if( OI.w == 0x2010 )
					{
						VariableType = SYS_TEMPERATURE_AD;
					}
					else if( OI.w == 0x2011 )
					{
						VariableType = SYS_CLOCK_VBAT_AD;
					}
					else
					{
						VariableType = SYS_READ_VBAT_AD;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
                    if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetBatAndTempValue( DATA_HEX, VariableType);
					if(Result == 0x8000 )
					{
						return 0x8000;
					}
					memcpy( InBuf,(BYTE*)&Result,Len);
					InBuf += Len;
					break;

				case 0x2013://ʱ�ӵ�ع���ʱ�� 
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
					Result = api_ReadFromContinueEEPRom(GET_CONTINUE_ADDR( BatteryTime ), sizeof(DWORD), InBuf);
					if(Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;

				case 0x2014://�������״̬��
					Result = api_GetMeterStatus( ePROTOCO_698, ClassIndex, 100, InBuf);
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					InBuf += Result;		
					break;

                case 0x2015://�������ϱ���
                    if( ClassIndex != 0 )
					{
						return 0x8000;
					}
                    Result = api_ReadFollowReportStatus_Mode( Ch, 0, InBuf );
                    if( Result == 0x8000 )
                    {
                        return 0x8000;
                    }
                    InBuf += Result;		
                break;	
				
				#if( MAX_PHASE == 3 )//ֻ�������Ŵ���������
				case 0x2017://��ǰ�й�����
				case 0x2018://��ǰ�޹�����
				case 0x2019://��ǰ��������
				
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					api_GetCurrDemandData( (OI.w-0x2016), DATA_HEXCOMP, Dot, 4, InBuf );
					InBuf += Len;
					break;
				#endif

				case 0x2026://��ѹ��ƽ����
				case 0x2027://������ƽ����
				case 0x2028://������
				    if( ClassIndex != 0 )
				    {
                        return 0x8000;
				    }
				    
					Result = api_GetSampleStatus( OI.w-0x2026, 0 );
					if(Result == 0xffff)
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					memcpy( InBuf, (BYTE*)&Result,Len );
					InBuf += Len;
					break;

				#if( SEL_AHOUR_FUNC == YES)
				case 0x2029://��ʱֵ				
					if( ClassIndex > 4 )
					{
						return 0x8000;
					}

					Num = ((ClassIndex == 0) ? 4 : 1);
					i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
					Num = Num+i;
					
					for( ; i<Num; i++ )
					{
						Result = api_GetAHourData( i, DATA_HEXCOMP, InBuf );
						if(Result == FALSE)
						{
							return 0x8000;
						}
						InBuf += 4;
					}
					break;
				#endif
				#if( PREPAY_MODE == PREPAY_LOCAL )
				case 0x201a://��ǰ���
				    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurPice, InBuf );
                    InBuf += Len;
				    break;

				case 0x201b://��ǰ���ʵ��
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurRatePrice, InBuf );
                    InBuf += Len;
				    break;

				case 0x201c://��ǰ���ݵ��
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eCurLadderPrice, InBuf );
                    InBuf += Len;
				    break;
                case 0x202c://��ǰǮ���ļ�
                    if( ClassIndex > 2 )
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eRemainMoney, (BYTE*)Money.b );
					api_ReadPrePayPara( eBuyTimes, InBuf + 4 );
					if( Money.l < 0 )
					{
						memset( &Money, 0x00, sizeof(Money) );
					}
					memcpy( InBuf, (BYTE*)Money.b, 4 );

                    if( ClassIndex != 0 )
                    {
                        memmove( InBuf, InBuf+(ClassIndex-1)*4, 4 );
                        InBuf += 4;
                    }
                    else
                    {
                        InBuf += 8;
                    }
                    break;

                case 0x202d://͸֧���
                    if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadPrePayPara( eRemainMoney, (BYTE*)Money.b );
                    if( Money.l > 0 )//��͸֧���
                    {
                        memset( InBuf, 0x00, 4 );
                    }
                    else
                    {
                        Money.l = ~Money.l;
                        Money.l += 1;
                        memcpy( InBuf, (BYTE*)Money.b, 4 );
                    }

                    InBuf += 4;
                    break;
				case 0x202e://�ۼƹ�����
					if( ClassIndex != 0 )
                    {
                        return 0x8000;
                    }
     
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 ) 
                    {
                        return 0x8000;
                    }
                    
                    api_ReadTotalBuyMoney( InBuf );

                    InBuf += 4;
                    break;
                case 0x2032://���ݽ����õ���
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_ReadPrePayPara( eLadderUseEnergy, InBuf );
					if( Result == 0 )
					{
						return 0x8000;
					}
					InBuf += Len;
                    break;

				#endif
				case 0x2031://�¶��õ���
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}

					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = api_GetAccuEnergyData( 0, DATA_HEXCOMP, 2, InBuf );
					if( Result == FALSE )
					{
						return 0x8000;
					}
					InBuf += Len;
					break;
				case 0x2040://��������ִ��״̬��
				case 0x2041://�����������״̬��
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, VariableObj[VariableIndex].Type2 );
					if( Len == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					Result = GetRelayReturnStatus((OI.w-0x2040));
					lib_InverseData((BYTE *)&Result, 2);//���ں�����ȡword�ĵ��ֽ�����Ƚ��е���
					memcpy( InBuf, (BYTE*)&Result,Len );
					InBuf += Len;
					break;
				#if( SEL_STAT_V_RUN == YES )
				case 0x2131://����A���ѹ�ϸ���
				case 0x2132://����B���ѹ�ϸ���
				case 0x2133://����C���ѹ�ϸ���				
					if( ClassIndex > 2 )
					{
						return 0x8000;
					}
					VariableType = ((ClassIndex==0) ? (BIT0+BIT1) : ClassIndex);
					Len = ReadVRunTime( (OI.w-0x2131+1), VariableType, InBuf );
					if(Len == 0 )
					{
						Len = 0x8000;
						break;
					}
					InBuf += Len;
					break;
				#endif
				case 0x2051:
					InBuf[Len++]=0;
					break;
				// case 0x2700:
				// case 0x2701:
				// case 0x2702:
				// case 0x2704:
				// case 0x2705:
				// case 0x2706:
				// case 0x2707:
				// case 0x2708:
				// case 0x2709:
				// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
				// 	InBuf += Len;
				// 	break;
				default:
					return 0x8000;
			}
			break;
		case 3://���㼰��λ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			InBuf[0] = VariableObj[VariableIndex].Scaler_Unit3->Scaler;
			InBuf[1] = VariableObj[VariableIndex].Scaler_Unit3->Unit;
			InBuf += 2;
			break;

		case 4://���ߵ����͵������ϱ�״̬��
		case 6://�������
			// if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
			// {
			// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
			// 	InBuf += Len;
			// 	break;
			// }else 
			if( (OI.w != 0x2001) && (OI.w != 0x2015) )
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OI.w == 0x2001 )
			{
				if( ClassAttribute == 4 )//���ߵ���
				{
					return 0x8000;
				}
				else
				{
					IType = PHASE_N2+REMOTE_I;
				}
				
                Len = GetTypeLen( eData, T_NI );
                if( Len == 0x8000 )
                {
                    return 0x8000;
                }               
                Result = api_GetRemoteData( IType , DATA_HEXCOMP, Dot, Len, InBuf );
                if( Result == FALSE )
                {
                    return 0x8000;
                }
                
                InBuf += Len;
			}
			else if( ClassAttribute == 4 )//2015ֻ֧������4
			{
                Result = api_ReadFollowReportStatus_Mode( Ch, 1, InBuf );
                if( Result == 0x8000 )
                {
                    return 0x8000;
                }
                InBuf += Result;		
			}
			else
			{
				return 0x8000;
			}

			break;
		case 5://�������ϱ����ϱ���ʽ
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Result = api_ReadEventReportMode( eReportMethod, OI.w, InBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				InBuf += 1;	
			}
			// else if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
			// {
			// 	Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
			// 	InBuf += Len;
			// }
			else
			{
				return 0x8000;
			}
            break;	
		// case 7:
		// 	if ((OI.w >= 0x2700) && (OI.w <= 0x2709))
		// 	{
		// 		Len = Charging698(eChargingGet,InBuf,OI.w,ClassAttribute,api_GetChargingPileCanID(LinkData[Ch].ServerAddr),ClassIndex);
		// 		InBuf += Len;
		// 	}
		// 	else
		// 	{
		// 		return 0x8000;
		// 	}
		// 	break;
		default:
			return 0x8000;
	}

	return  (WORD)(InBuf-pBufBak);
}

//--------------------------------------------------
//��������:  �Ա������ݽ��м�TAG
//         
//����:      *pOAD[in]��		OAD         
//           VariableIndex[in]:	VariableObj�е�����         
//           *InBuf[in]��		����Ҫ���tag������         
//           OutBufLen[in]��	OutBuf����       
//           *OutBuf[out]��		�������
//         
//����ֵ:    �������tag�����ݳ��� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע:  	��
//--------------------------------------------------
static WORD GetVariableAddTag(BYTE *pOAD, BYTE VariableIndex, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex,Num,i;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth,Lenth1;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	Lenth = 0x8000;
	Num = ((ClassIndex == 0) ? (FPara1->TimeZoneSegPara.Ratio+1) : 1);
	
	switch( ClassAttribute )
	{
		case 2://����2 array struct����
			if( 	( (OI.w>=0x2000) && (OI.w<=0x200A) ) 
				|| 	( OI.w == 0x2029 ) 		//��ʱֵ
				|| 	( OI.w == 0x2014 ) 		//�������״̬��
				|| 	( OI.w == 0x2025 )  	//�¼���ǰֵ
                #if( PREPAY_MODE == PREPAY_LOCAL )
				||  ( OI.w == 0x202c )//Ǯ���ļ�
				#endif
                  )
			{
				if( ClassIndex > VariableObj[VariableIndex].Type2[1])
				{
					return 0x8000;
				}
				
				Tag = VariableObj[VariableIndex].Type2;
				Tag += GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0	;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;		
			}	
			else if( OI.w <= 0x2041 )//��ͨ��������
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				Tag = VariableObj[VariableIndex].Type2;
				Lenth = GetTypeLen( eTagData, Tag) ;
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;		
			}
			else if ((OI.w <= 0x2709)&&(OI.w >= 0x2700))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else if (OI.w==0x2051)
			{
				Lenth=0;
				OutBuf[Lenth++]=0x01;
				OutBuf[Lenth++]=0x03;
				for ( i = 0; i < 3; i++)
				{
					OutBuf[Lenth++]=0x02;
					OutBuf[Lenth++]=0x02;
					OutBuf[Lenth++]=0x09;
					OutBuf[Lenth++]=0x00;
					OutBuf[Lenth++]=0x04;
					OutBuf[Lenth++]=0x10;
					OutBuf[Lenth++]=0x00;
					OutBuf[Lenth++]=0x00;
				}
				return Lenth;
			}
			else//��ѹ�ϸ��� ���⴦��
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				Tag = VariableObj[VariableIndex].Type2;
				
				Lenth = ((ClassIndex == 0) ? ((GetTypeLen( eTagData, Tag)*2)+2) : (GetTypeLen( eTagData, Tag)));
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Num = ((ClassIndex == 0) ? 2 : 1);
				i = ( (ClassIndex ==0) ? 0 : (ClassIndex-1));
				Num = Num+i;
				if( ClassIndex == 0 )
				{
					*(OutBuf++) = Structure_698;
					*(OutBuf++) = 02;
				}
				
				for( ; i < Num; i++ )
				{
					Result = GetRequestAddTag( Tag, InBuf, OutBuf );
					if( Result == 0x8000 )
					{
						return 0x8000;
					}
					
					Lenth1 = GetTypeLen( eData, Tag);
					if( Lenth1 == 0x8000 )
                    {
                        return 0x8000;
                    }
                    
					OutBuf += Result;
					InBuf += Lenth1;

				}
				
				return Lenth;		
			}
						
			break;

		case 3://���㼰��λ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( OutBufLen < 3 )//��������С�������򲻽��в���
			{
				return 0;
			}
			
			Result = GetRequestAddTag( T_ScalerUnit, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			return 3;
			break;

		case 4://���ߵ����͵������ϱ�״̬��
		case 6://�������
			if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else if( (OI.w != 0x2001) && (OI.w != 0x2015) )
			{
				return 0x8000;
			}
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			
			if( OI.w == 0x2001 )
			{
                Tag = T_NI;
			}
			else if( ClassAttribute == 4)//�������ϱ�״̬�� ����4����ͬ����2
			{
               Tag = VariableObj[VariableIndex].Type2;
			}
			else
			{
				return 0x8000;
			}
			
			Lenth = GetTypeLen( eTagData, Tag) ;
            if( Lenth == 0x8000 )
            {
                return 0x8000;
            }
            
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			
			Result = GetRequestAddTag( Tag, InBuf, OutBuf );
			if( Result == 0x8000 )
			{
				return 0x8000;
			}
			
			return Lenth;		
			break;
		case 5://�������ϱ����ϱ���ʽ
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Tag = T_Enum;
				Lenth = GetTypeLen( eTagData, Tag) ;
				if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}
				return Lenth;	
			}else if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			break;
		case 7:
			if ((OI.w <= 0x2709)&&(OI.w >= 0x2701))
			{
				Lenth=Charging698AddTag(ClassAttribute,OI.w,InBuf,OutBuf,ClassIndex);
				return Lenth;
			}
			else
			{
				return 0x8000;
			}
			
		default:
			return 0x8000;
	}
    return 0x8000;
}

//--------------------------------------------------
//��������:  ��ȡ����
//         
//����:      DataType[in]��	eData/eTagData/eAddTag        
//           Dot[in] :		С����        
//           *pOAD[in]��	OAD         
//           *InBuf[in]��	��Ҫ��ӱ�ǩ������         
//           OutBufLen[in]������OutBuf�����ݳ���         
//           *OutBuf[in]��	���صĵ�������
//         
//����ֵ:    ���ر������� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  
//--------------------------------------------------
WORD GetRequestVariable( BYTE Ch, BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	TTwoByteUnion OI;
	BYTE VariableIndex;
	BYTE Buf[32+30];//��ѹ�ϸ���Դ���ݳ���Ϊ32
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	Lenth = 0;
	
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType == eData) || (DataType == eTagData) )
	{
		Lenth = GetVariableData( Ch, Dot, pOAD, VariableIndex, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			Lenth = GetVariableAddTag(pOAD, VariableIndex, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		if( InBuf == NULL )
		{
			return 0x8000;
		}
		
		Lenth = GetVariableAddTag(pOAD, VariableIndex, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth;
}
//--------------------------------------------------
//��������:  ��ȡ��չģ�����
//         
//����:      	DataType[in]��	eData/eTagData/eAddTag
//         	Dot[in]:		С����(�ݲ���)        
//         	*pOAD[in]��		OAD         
//         	*InBuf[in]��	��Ҫ���tag������         
//         	OutBufLen[in]��	����Ļ��峤��         
//         	*OutBuf[out]��	�������
//         
//����ֵ:    ���ر������� 0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  
//--------------------------------------------------
WORD GetModuleRequestVariable(BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
    BYTE len=0;
	BYTE GetCmd[] = {0x05, 0x01, 0x00};
	BYTE APDUbufs[200];

	if (DataType==eAddTag)
	{
		return 0x8000;
	}
    memcpy(APDUbufs,GetCmd,sizeof(GetCmd));
	memcpy((APDUbufs+3), pOAD, 4);
    APDUbufs[7]=0;
	len=api_RequestModule(APDUbufs,8,OutBufLen);//050100+OAD+ʱ���ǩ00
	
	memcpy(OutBuf,APDUbufs+8,len);

	return len;

}

//--------------------------------------------------
//��������:  ����TAG��ȡ��������
//         
//����:      
//			DataType[in]��	eData/eTagData
//			*pOAD[in]��		OAD
//         
//����ֵ:   ����OAD��Ӧ���ݵĳ���  0x0000:���ݳ��Ȳ���  0x8000: ���ִ���
//         
//��ע����:  ��
//--------------------------------------------------
WORD GetRequestVariableLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,VariableIndex,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Type;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x1f);//bit0��bit4�����ʾ�������Ա�ţ�
	ClassIndex = pOAD[3];
	VariableIndex = SearchVariableOAD( OI.w );
	if( VariableIndex == 0x80 )
	{
		return 0x8000;
	}
	
	Type = VariableObj[VariableIndex].Type2;
	
	switch( ClassAttribute )
	{
		case 2:
			if( (OI.w>=0x2131) && (OI.w<=0x2133) )//��ѹ�ϸ���
			{
				if( ClassIndex > 2 )
				{
					return 0x8000;
				}
				Len.w = GetTypeLen( DataType, Type );
				if( Len.w == 0x8000 )
                {
                    return 0x8000;
                }
                
				if( DataType == eData )
				{
					Len.w = ((ClassIndex==0) ? (Len.w*2) : Len.w);
				}
				else
				{
					Len.w = ((ClassIndex==0) ? (Len.w*2+2) : Len.w);
				}				
			}
			else
			{
				if( 	((OI.w>=0x2000) && (OI.w<=0x200A)) 
					|| 	( OI.w == 0x2029 ) 
					|| 	( OI.w == 0x2014 ) 
					||	( OI.w == 0x2025 ) 
					#if( PREPAY_MODE == PREPAY_LOCAL )
					||  ( OI.w == 0x202c )//Ǯ���ļ�
					#endif
					)
				{
					if( ClassIndex > Type[1] )
					{
						return 0x8000;
					}
					
					Type +=GetTagOffsetAddr( ClassIndex, Type );
				}
				else
				{
					if( ClassIndex != 0 )
					{
						return 0x8000;
					}
				}
				
				Len.w = GetTypeLen(DataType, Type);
			}				
			break;

		case 3://���㼰��λ
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}
			if( DataType == eData )//Դ���ݳ���
			{
				Len.w = 2;
			}
			else
			{
				Len.w = 3;
			}
			break;
		
		case 4://���ߵ����͵������״̬��
		case 6://�������
			if( (OI.w != 0x2001) && (OI.w != 0x2015))
			{
				return 0x8000;
			}
			
			if( ClassIndex != 0 )
			{
				return 0x8000;
			}

			if( OI.w == 0x2001 )
			{
                Len.w = GetTypeLen( DataType, T_NI);    
			}
			else if( ClassAttribute == 4 )//�������״̬��
			{
               Len.w = GetTypeLen( DataType, Type);     
			}
			else
			{
				return 0x8000;
			}
				
			break;
		case 5://�����ϱ�״̬�� �ϱ���ʽ	
			if( OI.w == 0x2015 )
			{
				if( ClassIndex != 0 )
				{
					return 0x8000;
				}
				
				Type = T_Enum;
				Len.w = GetTypeLen( DataType, Type) ;
				if( Len.w == 0x8000 )
                {
                    return 0x8000;
                }
			}
			
			break;
		default:		
			return 0x8000;
	}

	return Len.w;
}

#endif//#if ( SEL_DLT698_2016_FUNC == YES)

