//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	    ���ĺ�
//��������	    2018.08.09
//����        ����645��Լ�ϱ�
//�޸ļ�¼
//---------------------------------------------------------------------- 

#include "AllHead.h"

#if( SEL_DLT645_2007 == YES )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define FollowReportStatusNum  (7)
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
static __no_init TReport645 Report645;
static __no_init TReportTimes645 ReportTimes645;//�����ϱ�����
TReport645ResetTime Report645ResetTime;


//����698�����ϱ�����
extern TReportMode				ReportMode;					//�¼��ϱ���ʶ bit:�¼���Ч��ʶ
extern __no_init TReportData 	ReportData[MAX_COM_CHANNEL_NUM];//���������ͨ��
extern const BYTE  				Report645ResetTimeConst;   //645�ϱ���ʱ

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
//�¼�Sub���-�����ϱ����-�ϱ�ģʽ
//��֧���޹���Ϸ�ʽ���
//��ӷ��¼����͸����ϱ���Ҫͬ������FollowReportStatusNum�궨�壬�ҷ�����ǰ��
static const TReport645Table Report645Table[] =
{
    //esam����
    {eSTATUS_ESAM_Error,                eSUB_STATUS_ESAM_ERR,           eSTATUS_ESAM_Error,                 eSUB_MODE_ESAM_ERR              },

    //��ص�ѹ��
    {eSTATUS_ClockBatteryUnderVol,      eSUB_STATUS_CLOCK_BAT_LOW,      eSTATUS_ClockBatteryUnderVol,       eSUB_MODE_CLOCK_BAT_LOW         },

    //epprom����
    {eSTATUS_EEPROM_Error,              eSUB_STATUS_MEM_ERR,            eSTATUS_EEPROM_Error,               eSUB_MODE_MEM_ERR               },

    //ͣ�糭����Ƿѹ
    {eSTATUS_ReadMeterBatteryUnderVol,  eSUB_STATUS_READ_BAT_LOW,       eSTATUS_ReadMeterBatteryUnderVol,   eSUB_MODE_READ_BAT_LOW          },

    //͸֧״̬
    {eSTATUS_TickFlag,                  eSUB_STATUS_OVER_DRAFT,         eSTATUS_TickFlag,                   eSUB_MODE_OVER_DRAFT            },

    //��բ�ɹ�
    {eSTATUS_OpenRelaySucceed,          eSUB_STATUS_PRG_OPEN_RELAY,     eSTATUS_OpenRelaySucceed,           eSUB_MODE_PRG_OPEN_RELAY        },

    //��բ�ɹ�
    {eSTATUS_CloseRelaySucceed,         eSUB_STATUS_PRG_CLOSE_RELAY,    eSTATUS_CloseRelaySucceed,          eSUB_MODE_PRG_CLOSE_RELAY       },

	//���ɿ�������
    #if(SEL_EVENT_RELAY_ERR == YES) //45
    {eSUB_EVENT_RELAY_ERR,              eSUB_STATUS_RELAY_ERR,          eEVENT_RELAY_ERR_NO,                eSUB_MODE_RELAY_ERR             },
    #endif

    //ʱ�ӹ���
    #if(SEL_EVENT_RTC_ERR == YES)
    {eSUB_EVENT_RTC_ERR,                eSUB_STATUS_RTC_ERR,            eEVENT_RTC_ERR_NO,                  eSUB_MODE_RTC_ERR               },
    #endif
    
    //�����
    #if(SEL_EVENT_METERCOVER == YES)
    {eSUB_EVENT_METER_COVER,            eSUB_STATUS_METER_COVER,        eEVENT_METERCOVER_NO,               eSUB_MODE_METER_COVER           },
    #endif
    
    //����ť��
    #if(SEL_EVENT_BUTTONCOVER == YES)
    {eSUB_EVENT_BUTTON_COVER,           eSUB_STATUS_BUTTON_COVER,       eEVENT_BUTTONCOVER_NO,              eSUB_MODE_BUTTON_COVER          },
    #endif
    
    //�㶨�ų�����
    #if(SEL_EVENT_MAGNETIC_INT == YES)
    {eSUB_EVENT_MAGNETIC_INT,           eSUB_STATUS_MAGNETIC_INT,       eEVENT_MAGNETIC_INT_NO,             eSUB_MODE_MAGNETIC_INT          },
    #endif
    
    //��Դ�쳣
    #if(SEL_EVENT_POWER_ERR == YES)	
    {eSUB_EVENT_POWER_ERR,              eSUB_STATUS_POWER_ERR,          eEVENT_POWER_ERR_NO,                eSUB_MODE_POWER_ERR             },
    #endif

    //A��ʧѹ
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_A,                eSUB_STATUS_LOSTV_A,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //A��Ƿѹ
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_A,               eSUB_STATUS_WEAK_V_A,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //A���ѹ
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_A,               eSUB_STATUS_OVER_V_A,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //A��ʧ��
    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_A,                eSUB_STATUS_LOSTI_A,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
    
    //A�����
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_A,               eSUB_STATUS_OVER_I_A,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    
    //A�����
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_A,             eSUB_STATUS_OVERLOAD_A,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif
    
    //A�๦�ʷ���
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_A,             eSUB_STATUS_POW_BACK_A,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //A�����
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_A,                eSUB_STATUS_BREAK_A,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //A�����
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_A,              eSUB_STATUS_BREAK_I_A,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //B��ʧѹ
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_B,                eSUB_STATUS_LOSTV_B,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //B��Ƿѹ
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_B,               eSUB_STATUS_WEAK_V_B,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //B���ѹ
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_B,               eSUB_STATUS_OVER_V_B,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //B��ʧ��
    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_B,                eSUB_STATUS_LOSTI_B,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
    
    //B�����
    #if( MAX_PHASE == 3 )
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_B,               eSUB_STATUS_OVER_I_B,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    #endif
    
    //B�����
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_B,             eSUB_STATUS_OVERLOAD_B,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif

    //B�๦�ʷ���
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_B,             eSUB_STATUS_POW_BACK_B,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //B�����
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_B,                eSUB_STATUS_BREAK_B,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //B�����
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_B,              eSUB_STATUS_BREAK_I_B,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //C��ʧѹ
    #if ( SEL_EVENT_LOST_V == YES )
    {eSUB_EVENT_LOSTV_C,                eSUB_STATUS_LOSTV_C,            eEVENT_LOST_V_NO,                   eSUB_MODE_LOSTV                 },
    #endif
    
    //C��Ƿѹ
    #if( SEL_EVENT_WEAK_V == YES )
    {eSUB_EVENT_WEAK_V_C,               eSUB_STATUS_WEAK_V_C,           eEVENT_WEAK_V_NO,                   eSUB_MODE_WEAK_V                },
    #endif
    
    //C���ѹ
    #if( SEL_EVENT_OVER_V == YES )
    {eSUB_EVENT_OVER_V_C,               eSUB_STATUS_OVER_V_C,           eEVENT_OVER_V_NO,                   eSUB_MODE_OVER_V                },
    #endif
    
    //C��ʧ��

    #if( SEL_EVENT_LOST_I == YES )
    {eSUB_EVENT_LOSTI_C,                eSUB_STATUS_LOSTI_C,            eEVENT_LOST_I_NO,                   eSUB_MODE_LOSTI                 },
    #endif
 
    //C�����
    #if( MAX_PHASE == 3 )
    #if( SEL_EVENT_OVER_I == YES )
    {eSUB_EVENT_OVER_I_C,               eSUB_STATUS_OVER_I_C,           eEVENT_OVER_I_NO,                   eSUB_MODE_OVER_I                },
    #endif
    #endif
    
    //C�����
    #if( SEL_EVENT_OVERLOAD == YES )
    {eSUB_EVENT_OVERLOAD_C,             eSUB_STATUS_OVERLOAD_C,         eEVENT_OVERLOAD_NO,                 eSUB_MODE_OVERLOAD              },
    #endif
    
    //C�๦�ʷ���
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_C,             eSUB_STATUS_POW_BACK_C,         eEVENT_BACKPROP_NO,                 eSUB_MODE_POW_BACK              },
    #endif
    
    //C�����
    #if( SEL_EVENT_BREAK == YES )
    {eSUB_EVENT_BREAK_C,                eSUB_STATUS_BREAK_C,            eEVENT_BREAK_NO,                    eSUB_MODE_BREAK                 },
    #endif
    
    //C�����
    #if( SEL_EVENT_BREAK_I == YES )
    {eSUB_EVENT_BREAK_I_C,              eSUB_STATUS_BREAK_I_C,          eEVENT_BREAK_I_NO,                  eSUB_MODE_BREAK_I               },
    #endif
    
    //��ѹ������
    #if( SEL_EVENT_V_REVERSAL == YES )
    {eSUB_EVENT_V_REVERSAL,             eSUB_STATUS_V_REVERSAL,         eEVENT_V_REVERSAL_NO,               eSUB_MODE_V_REVERSAL            },
    #endif
    
    //����������
    #if( SEL_EVENT_I_REVERSAL == YES )
    {eSUB_EVENT_I_REVERSAL,             eSUB_STATUS_I_REVERSAL,         eEVENT_I_REVERSAL_NO,               eSUB_MODE_I_REVERSAL            },
    #endif
    
    //��ѹ��ƽ��
    #if(SEL_EVENT_V_UNBALANCE == YES)
    {eSUB_EVENT_V_UNBALANCE,            eSUB_STATUS_V_UNBALANCE,        eEVENT_V_UNBALANCE_NO,              eSUB_MODE_V_UNBALANCE           },
    #endif
    
    //������ƽ��
    #if(SEL_EVENT_I_UNBALANCE == YES)
    {eSUB_EVENT_I_UNBALANCE,            eSUB_STATUS_I_UNBALANCE,        eEVENT_I_UNBALANCE_NO,              eSUB_MODE_I_UNBALANCE           },
    #endif
    
    //������Դʧ��
    #if( SEL_EVENT_LOST_SECPOWER == YES )
    {eSUB_EVENT_LOST_SECPOWER,          eSUB_STATUS_LOST_SECPOWER,      eEVENT_LOST_SECPOWER_NO,            eSUB_MODE_LOST_SECPOWER         },
    #endif
    
    //����
    #if( SEL_EVENT_LOST_POWER == YES )
    {eSUB_EVENT_LOST_POWER,             eSUB_STATUS_LOST_POWER,         eEVENT_LOST_POWER_NO,               eSUB_MODE_LOST_POWER            },
    #endif
    
    //�������ޣ�����
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_PA_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_PA_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�������ޣ�����
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_NA_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_NA_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�������ޣ���һ���ޣ�
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QI_DEMAND_OVER,         eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�������ޣ��ڶ����ޣ�
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QII_DEMAND_OVER,        eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�������ޣ��������ޣ�
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QIII_DEMAND_OVER,       eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�������ޣ��������ޣ�
    #if(SEL_DEMAND_OVER == YES)
    {eSUB_EVENT_QIV_DEMAND_OVER,        eSUB_STATUS_DEMAND_OVER,        eEVENT_RE_DEMAND_OVER_NO,           eSUB_MODE_DEMAND_OVER           },
    #endif
    
    //�ܹ�����������
    #if( SEL_EVENT_COS_OVER == YES )
    {eSUB_EVENT_COS_OVER_ALL,           eSUB_STATUS_COS_OVER_ALL,       eEVENT_COS_OVER_NO,                 eSUB_MODE_COS_OVER_ALL          },
    #endif
    
    //�������ز�ƽ��
    #if(SEL_EVENT_I_S_UNBALANCE == YES)
    {eSUB_EVENT_S_I_UNBALANCE,          eSUB_STATUS_S_I_UNBALANCE,      eEVENT_I_S_UNBALANCE_NO,            eSUB_MODE_S_I_UNBALANCE         },
    #endif
    
    //���������ܹ��ʷ���
    #if( SEL_EVENT_BACKPROP == YES )
    {eSUB_EVENT_POW_BACK_ALL,           eSUB_STATUS_BACKPROPALL,        eEVENT_BACKPROP_NO,                 eSUB_MODE_BACKPROPALL           },
    #endif
    
    //ȫʧѹ
    #if( SEL_EVENT_LOST_ALL_V == YES )
    {eSUB_EVENT_LOST_ALL_V,             eSUB_STATUS_LOST_ALL_V,         eEVENT_LOST_POWER_NO,               eSUB_MODE_LOST_POWER            },
    #endif
    
    // �����¼�
    #if( SEL_TOPOLOGY == YES )
    {eSUB_EVENT_TOPU,           eSUB_STATUS_TOPO_IDENTI_RESULT,         eEVENT_TOPU_NO,               eSUB_MODE_TOPO_IDENTI_RESULT    },
    #endif
    //���
    {eSUB_EVENT_PRG_RECORD,             eSUB_STATUS_PRG_RECORD,         ePRG_RECORD_NO,                     eSUB_MODE_PRG_RECORD            },

    //�������
    #if( SEL_PRG_INFO_CLEAR_METER == YES )
    {eSUB_EVENT_PRG_CLEAR_METER,        eSUB_STATUS_PRG_CLEAR_METER,    ePRG_CLEAR_METER_NO,                eSUB_MODE_PRG_CLEAR_METER       },
    #endif
    
    //��������
    #if( SEL_PRG_INFO_CLEAR_MD == YES )
    {eSUB_EVENT_PRG_CLEAR_MD,           eSUB_STATUS_PRG_CLEAR_MD,       ePRG_CLEAR_MD_NO,                   eSUB_MODE_PRG_CLEAR_MD          },
    #endif
    
    //�¼�����
    #if( SEL_PRG_INFO_CLEAR_EVENT == YES )
    {eSUB_EVENT_PRG_CLEAR_EVENT,        eSUB_STATUS_PRG_CLEAR_EVENT,    ePRG_CLEAR_EVENT_NO,                eSUB_MODE_PRG_CLEAR_EVENT       },
    #endif
    
    //Уʱ
    #if( SEL_PRG_INFO_ADJUST_TIME == YES )
    {eSUB_EVENT_PRG_ADJUST_TIME,        eSUB_STATUS_PRG_ADJUST_TIME,    ePRG_ADJUST_TIME_NO,                eSUB_MODE_PRG_ADJUST_TIME       },
    #endif
    
    //ʱ�α���
    #if( SEL_PRG_INFO_TIME_TABLE == YES )
    {eSUB_EVENT_PRG_TIME_TABLE,         eSUB_STATUS_PRG_TIME_TABLE,     ePRG_TIME_TABLE_NO,                 eSUB_MODE_PRG_TIME_TABLE        },
    #endif
    
    //ʱ������
    #if( SEL_PRG_INFO_TIME_AREA == YES )
    {eSUB_EVENT_PRG_TIME_AREA,          eSUB_STATUS_PRG_TIME_AREA,      ePRG_TIME_AREA_NO,                  eSUB_MODE_PRG_TIME_AREA         },
    #endif
    
    //�����ձ��
    #if( SEL_PRG_INFO_WEEK == YES )
    {eSUB_EVENT_PRG_WEEKEND,            eSUB_STATUS_PRG_WEEKEND,        ePRG_WEEKEND_NO,                    eSUB_MODE_PRG_WEEKEND           },
    #endif
    
    //�ڼ��ձ��
    #if( SEL_PRG_INFO_HOLIDAY == YES )
    {eSUB_EVENT_PRG_HOLIDAY,            eSUB_STATUS_PRG_HOLIDAY,        ePRG_HOLIDAY_NO,                    eSUB_MODE_PRG_HOLIDAY           },
    #endif
    
    //�й���Ϸ�ʽ���
    #if( SEL_PRG_INFO_P_COMBO == YES )
    {eSUB_EVENT_ePRG_P_COMBO,           eSUB_STATUS_PRG_P_COMBO,        ePRG_P_COMBO_NO,                    eSUB_MODE_PRG_P_COMBO           },
    #endif
    //�޹���Ϸ�ʽ1���
    //{eSUB_EVENT_PRG_Q_COMBO,            eSUB_STATUS_PRG_Q_COMBO,        ePRG_Q_COMBO_NO,                    eSUB_MODE_PRG_Q_COMBO}
    //�޹���Ϸ�ʽ2���
    //{eSUB_EVENT_PRG_Q_COMBO,            eSUB_STATUS_PRG_Q2_COMBO,       ePRG_Q_COMBO_NO,                    eSUB_MODE_PRG_Q_COMBO}

    //�����ձ��
    #if( SEL_PRG_INFO_CLOSING_DAY == YES )
    {eSUB_EVENT_PRG_CLOSING_DAY,        eSUB_STATUS_PRG_CLOSING_DAY,    ePRG_CLOSING_DAY_NO,                eSUB_MODE_PRG_CLOSING_DAY       },
    #endif
    
    //��Կ����
    #if( SEL_PRG_UPDATE_KEY == YES )
    {eSUB_EVENT_PRG_UPDATE_KEY,         eSUB_STATUS_PRG_UPDATE_KEY,     ePRG_UPDATE_KEY_NO,                 eSUB_MODE_PRG_UPDATE_KEY        },
    #endif
};
//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ����״̬�����������RAM�е�CRCУ��ֵ
//
//����:	  
//			Type[in]		- ����ʽ��WRITE--д 		READ--��
//			pBuf[in/out]	- [in]--�������������1����				[out]--�������������1Bit2����
//
//����ֵ:		ReturnLen		- ���ݳ��� �� FALSE
//
//��ע:
//---------------------------------------------------------------	
static void UpdateReport645CRC32(void)
{
    Report645.CRC32 = lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) );
    ReportTimes645.CRC32 = lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) );

}

//-----------------------------------------------
//��������:     645�ϱ�crc���
//
//����:	    ��
//			
//
//����ֵ:		ReturnLen		- ���ݳ��� �� FALSE
//
//��ע:
//---------------------------------------------------------------	
static void CheckReport645(void)
{
    WORD Result;
    
    if( Report645.CRC32 != lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) ) )
	{
	    memset( Report645.BakFlag, 0x00,sizeof(TReport645) );  
        UpdateReport645CRC32();
	}

    if( ReportTimes645.CRC32 != lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) ) )
    {
        memset( ReportTimes645.Times, 0x00, sizeof(TReportTimes645) );//������� ��Ϊ��ͨ���޷�׼ȷ��1������ڶ�ȡʱ������1�ж�
        UpdateReport645CRC32();
    } 

    if( Report645ResetTime.CRC32 != lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) ) )
	{
        Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime.Time), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
        if( Result == FALSE )
        {
           Report645ResetTime.Time = 30;//Ĭ��Ϊ30����
           Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
        }    
	}
}

//-----------------------------------------------
//��������:      ���������ϱ���λʱ��
//
//����:        BYTE ResetTime[in]
//
//����ֵ:		
//
//��ע:
//---------------------------------------------------------------
WORD  api_SetReportResetTime( BYTE ResetTime )
{
    WORD Result;

    Report645ResetTime.Time = ResetTime;
    Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    if( Result == FALSE )
    {
        Report645ResetTime.Time = 30;
        Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
    }
    return Result;
}

//-----------------------------------------------
//��������:      ��ȡ�����ϱ���λʱ��
//
//����:        void
//
//����ֵ:		 BYTE ��λʱ��  
//
//��ע:
//---------------------------------------------------------------
BYTE  api_GetReportResetTime( void )
{
    return Report645ResetTime.Time; 
}

//-----------------------------------------------
//��������: �����ϱ�����
//
//����:      BYTE Ch[in]          ͨ��ѡ��
//
//         BYTE Mode[in]        0x00:�����ۼƼ�1
//                              0x55:��������Ϊ1
//
//         BYTE SubEventIndex   ���Ӵ������¼�����
//
//����ֵ:		Result		- TRUE������ 		FALSE��������
//
//��ע:
//---------------------------------------------------------------
WORD  api_AddReportTimes( BYTE Ch, BYTE Mode, BYTE SubEventIndex)
{
    BYTE i;

    if( SubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
    {
        return FALSE;
    }
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }
   
    if( Mode == 0x00 )
    {
        ReportTimes645.Times[Ch][SubEventIndex]++;
    }
    else
    {
        ReportTimes645.Times[Ch][SubEventIndex] = 1;
    }
    
    UpdateReport645CRC32();

    return TRUE;
}

//-----------------------------------------------
//��������: ���ϱ�����
//
//����:      BYTE Mode[in]        0x00:��ָ���¼�
//                              0x55:ȫ��
//         BYTE SubEventIndex   ���Ӵ������¼�����
//
//����ֵ:		Result		- TRUE������ 		FALSE��������
//
//��ע:
//---------------------------------------------------------------
WORD  ClearReport645Times( BYTE Ch, BYTE Mode, BYTE SubEventIndex)
{
    BYTE i;

    if( SubEventIndex >= SUB_EVENT_INDEX_MAX_NUM )
    {
        return FALSE;
    }
    
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }
    
    if( Mode == 0x00 )
    {
        ReportTimes645.Times[Ch][SubEventIndex] = 0;
    }
    else
    {
        memset( (BYTE*)&ReportTimes645.Times[0][0], 0x00, sizeof(TReportTimes645) );
    }
    
    UpdateReport645CRC32();

    return TRUE;
}

//-----------------------------------------------
//��������: �����ϱ�״̬�ֶ�ȡ
//
//����:		BYTE Ch[in]  - ͨ��ѡ��
//          Type[in]	 - ��ȡ��ʽ eREAD_STATUS_PROC--��Լ��ȡ	         	eREAD_STATUS_OTHER--�ǹ�Լ��ȡ
//			pBuf[out]	 - �����ϱ�״̬�ֻ���
//����ֵ:		ReturnLen	 - ���ݳ���		0������
//
//��ע:
//---------------------------------------------------------------
WORD api_ReadReportStatusByte( BYTE Ch, eREAD_STATUS_TYPE Type, BYTE *pBuf	)
{
	WORD ReturnLen = 0;
	BYTE i,j,Num;
    BYTE Status[RPT_STATUS_LEN];
	BYTE Times[eREPORT_STATUS_TOTAL];    //�����ϱ�״̬����������
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }

    memset( Status, 0x00, sizeof(Status) );
    memset( Times, 0x55, sizeof(Times) );//����0xff ��Ϊ���¼��ϱ���������0xff
    Num = (sizeof(Report645Table)/sizeof(TReport645Table));
    
    for( i=0; i<eSUB_EVENT_STATUS_TOTAL; i++ )//�¼��ϱ�����״̬��ת��
    {
        if( ReportData[Ch].Index[i] != 0xff )
        {
            for( j=FollowReportStatusNum; j<Num; j++ )
            {
                if( ReportData[Ch].Index[i] == Report645Table[j].Event_SubIndex )
                {
                    Status[(Report645Table[j].Status_Index/8)] |= (0x01<<(Report645Table[j].Status_Index%8));//��λ״̬��
                    Times[Report645Table[j].Status_Index] = ReportTimes645.Times[Ch][Report645Table[j].Event_SubIndex];//��ȡ����
                    break;
                }
            }
        }
    }

    for( i=0; i<eMAX_FOLLOW_REPORT_STATUS_NUM; i++ )//�����ϱ�״̬��ת��
    {
        if( (ReportData[Ch].Status[i/8]&(1<<i%8)) != 0 )
        {
            for( j=0; j<FollowReportStatusNum; j++ )
            {
                if( Report645Table[j].Event_SubIndex == i )
                {
                    Status[(Report645Table[j].Status_Index/8)] |= (0x01<<(Report645Table[j].Status_Index%8));//��λ״̬��
                    Times[Report645Table[j].Status_Index] = 0xff;//��ȡ����
                    break;
                }
            }
        }
    }
    
	memcpy( pBuf, Status, RPT_STATUS_LEN );	//��RAM�ж�ȡ״̬��

	ReturnLen = RPT_STATUS_LEN;
    
	if( Type == eREAD_STATUS_PROC )	//��Լ��ȡ
	{
		pBuf[ReturnLen] = 0xAA;

		ReturnLen += 1;

		//��RAM�ж����������������
		for( i = 0; i < eREPORT_STATUS_TOTAL; i++)
		{
			if( Times[i] != 0x55 )
			{
			    if( Times[i] == 0 )
			    {
				    pBuf[ReturnLen] = 1;
			    }
			    else
			    {
                    pBuf[ReturnLen] = Times[i];
			    }
				ReturnLen += 1;
			}
		}

        pBuf[ReturnLen] = 0xAA;

        ReturnLen += 1;
        
        Report645.BakFlag[Ch] = 0x55;
        memcpy( (BYTE*)&Report645.StatusBak[Ch][0], Status, RPT_STATUS_LEN );//���汸�� 
        Report645.ResetTime[Ch] = Report645ResetTime.Time;//��������ʱ
	}
	else if( Type == eREAD_STATUS_OTHER )//�ǹ�Լ��ȡ��������������
	{
		memcpy( pBuf, Status, RPT_STATUS_LEN );	//��RAM�ж�ȡ״̬��
		ReturnLen = RPT_STATUS_LEN;
	}
	
    UpdateReport645CRC32();

	return ReturnLen;
}

//-----------------------------------------------
//��������:     �����ϱ�״̬�ֶ�ȡ
//
//����:	    BYTE *pBuf[in]  ����buf
//
//��ע:
//---------------------------------------------------------------
WORD api_ReadReportStatusMode( BYTE *pBuf	)
{
    BYTE i,j,Num;
    BYTE Mode[RPT_MODE_LEN+20];
    
    Num = (sizeof(Report645Table)/sizeof(TReport645Table));

    memset( Mode, 0x00, sizeof(Mode) );
    
    for( i=0; i<eNUM_OF_EVENT_PRG; i++ )//�¼��ϱ�ģʽ��ת��
    {
        if( ReportMode.byReportMode[i] !=0 )
        {
            for( j=FollowReportStatusNum; j<Num; j++ ) 
            {
                if( Report645Table[j].Event_Index == i )
                {
                    Mode[(Report645Table[j].Mode_Index/8)] |= (0x01<<(Report645Table[j].Mode_Index%8));//��λ״̬��
                    break;
                }
            }
        }
    }

   for( i=0; i<eMAX_FOLLOW_REPORT_STATUS_NUM; i++ )//�����ϱ�ģʽ��ת��
   {
        if( (ReportMode.byFollowReportMode[i/8]&(1<<i%8)) != 0 )
        {
            for( j=0; j<FollowReportStatusNum; j++ )
            {
                if( Report645Table[j].Event_Index == i )
                {
                    Mode[(Report645Table[j].Mode_Index/8)] |= (0x01<<(Report645Table[j].Mode_Index%8));//��λ״̬��
                    break;
                }
            }

        }

   }

   memcpy( pBuf, Mode, RPT_MODE_LEN );

   return RPT_MODE_LEN;
   
}

//-----------------------------------------------
//��������: ��ȡ�����ϱ�״̬���Ƿ����ú���֡
//
//����:      BYTE Ch[in] ͨ��
//			
//
//����ֵ:	
//
//��ע:		
//---------------------------------------------------------------
WORD api_GetReportReqFlag( BYTE Ch )
{
    BYTE Buf[RPT_STATUS_LEN+20];
    BYTE i;
    WORD Result;

    Result = FALSE;
	if( api_GetRunHardFlag( eRUN_HARD_645_FOLLOW_STATUS ) == TRUE )//�ж��������Ƿ��������ϱ�
	{
        api_ReadReportStatusByte( Ch, eREAD_STATUS_OTHER, Buf	);
        for( i=0; i<RPT_STATUS_LEN; i++ )
        {
            if( Buf[i] != 0 )
            {
                break;
            }

        }

        if( i != RPT_STATUS_LEN )
        {
            Result = TRUE;
        }
	}

	return Result;
}

//-----------------------------------------------
//��������: �����ϱ�״̬�ָ�λ
//
//����:		Ch[in]      - ͨ��ѡ��
//          pBuf[in]	- ��λ����
//
//����ֵ:		Result	- TRUE����ȷ		FALSE������
//
//��ע:
//---------------------------------------------------------------
WORD api_ResetReportStatusByte( BYTE Ch, BYTE *pBuf )
{
	BYTE i,j,k,Num;
	BYTE TmpBuf[RPT_STATUS_LEN];
	BYTE StatusBuf[RPT_STATUS_LEN];
	
	if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }
    
    if( Report645.BakFlag[Ch] != 0x55 )
    {
        return FALSE;
    }

    api_ReadReportStatusByte( Ch, eREAD_STATUS_OTHER, StatusBuf );//��ȡ��ǰͨ���ϱ�״̬��
	
	for( i = 0; i < RPT_STATUS_LEN; i++)
	{
		TmpBuf[i] = ~pBuf[i];

		for( j = 0; j < 8 ; j++ )
		{
			//ĳ1λ״̬��δ������ �� ��λ �� �յ��Ը�λ��λ�ĸ�λ����
			if( ((Report645.StatusBak[Ch][i]&(0x01<<j))==0) && ((StatusBuf[i]&(0x01<<j))!=0) && ((TmpBuf[i]&(0x01<<j))!=0) )
			{
				break;
			}
		}

        if( j < 8 )
        {
            break;
        }
	}

	//��λ״̬����Чʱ����λ��������״̬��
	if( i == RPT_STATUS_LEN )
	{
		//��״̬��
		for( i = 0; i < RPT_STATUS_LEN; i++)
		{
			Report645.StatusBak[Ch][i] &= pBuf[i];
		}
		
        Num = (sizeof(Report645Table)/sizeof(TReport645Table));
		//����������
		for( i = 0; i < RPT_STATUS_LEN*8; i++)
		{
			if( (~pBuf[i/8]) & (0x01<<(i%8)) )//Ҫ��λ��λ
			{
                for( j=FollowReportStatusNum; j<Num; j++ )//�¼��ϱ���λ
                {
                    if( Report645Table[j].Status_Index == i )
                    {
                        ClrReportIndexChannel( Ch, Report645Table[j].Event_SubIndex );
                        ClearReport645Times(Ch,0x00, Report645Table[j].Event_SubIndex);
                        break;
                    }
                }
        
                for( j=0; j<FollowReportStatusNum; j++ )//�����ϱ���λ
                {
                    if( Report645Table[j].Status_Index == i )
                    {
					    ReportData[Ch].Status[Report645Table[j].Event_SubIndex/8] &= ~(1<<Report645Table[j].Event_SubIndex%8);
                        break;
                    }
                }       
			}
		}

		ReportData[Ch].CRC32 = lib_CheckCRC32((BYTE*)&ReportData[Ch],sizeof(TReportData)-sizeof(DWORD));		
	}
	else
	{
		return FALSE;
	}
	
    Report645.BakFlag[Ch] = 0;

	UpdateReport645CRC32();
	
	return TRUE;
}

//-----------------------------------------------
//��������: �����ϱ���������
//
//����:		���ޣ�
//
//����ֵ:		���ޣ�
//
//��ע:		״̬��ȫΪ0ʱ��EVENTOUT�������̬����������
//---------------------------------------------------------------
void api_Report645MinTask( void )
{
    BYTE i,j;
    BYTE Buf[RPT_STATUS_LEN+20];
    
    CheckReport645();//ȫ�ֱ������в���У��

    for( i=0; i<MAX_COM_CHANNEL_NUM; i++ )
    {
        if( Report645.ResetTime[i] !=0 )
        {
            Report645.ResetTime[i]--;
            
            if( Report645.ResetTime[i] == 0 )//���и�λ�����ϱ�״̬��
            {
                for( j=0; j<RPT_STATUS_LEN; j++ )
                {
                    Buf[j]  = ~Report645.StatusBak[i][j];
                }
                api_ResetReportStatusByte( i, Buf );
            }
        }
    }
    
    UpdateReport645CRC32();
}

//-----------------------------------------------
//��������: �����͹���ʱ��EEPROM�ָ������ϱ�״̬�֡�������������־λ����λ����ʱ�����ϱ�־λ
//
//����:		��
//			
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_PowerUpReport645( void )
{
	DWORD Addr;
	WORD Result;

	//��ȡ��λ�ϱ���ʱ
    Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime.Time), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    if( Result == FALSE )
    {
       Report645ResetTime.Time = 30;//Ĭ��Ϊ30����
       Report645ResetTime.CRC32 = lib_CheckCRC32( (BYTE*)&Report645ResetTime.Time, sizeof(TReport645ResetTime)-sizeof(DWORD) );
    }    

	if( Report645.CRC32 != lib_CheckCRC32( Report645.BakFlag, sizeof(TReport645)-sizeof(DWORD) ) )
	{
        Result = api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);
        if( Result == FALSE )
        {
	        memset( (BYTE*)&Report645.BakFlag, 0x00,sizeof(TReport645) );
        }
        
	}

    if( ReportTimes645.CRC32 != lib_CheckCRC32( (BYTE*)&ReportTimes645.Times[0][0], sizeof(TReportTimes645)-sizeof(DWORD) ) )
    {
        memset( (BYTE*)&ReportTimes645.Times[0][0], 0x00, sizeof(TReportTimes645) );//������� ��Ϊ��ͨ���޷�׼ȷ��1������ڶ�ȡʱ������1�ж�
    } 
    
    UpdateReport645CRC32();
}
//-----------------------------------------------
//��������: ���籣����ϱ�־
//
//����:		Type[in]	-
//			Index[in]	-
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_PowerDownReport645( void )
{
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);
}


//-----------------------------------------------
//��������:     ��ʼ�������ϱ��������������
//
//����:		��
//			
//����ֵ:		��
//
//��ע:
//---------------------------------------------------------------
void api_FactoryInitReport645( void )
{
    WORD Result;
    
    ClearReport645Times( 0, 0x55, 0 );
    memset( (BYTE*)&Report645.BakFlag, 0x00,sizeof(TReport645) );
    api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645.BakFlag), sizeof(TReport645), Report645.BakFlag);

    Report645ResetTime.Time = Report645ResetTimeConst;
    Result = api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ReportSafeRom.Report645ResetTime), sizeof(TReport645ResetTime), (BYTE*)&Report645ResetTime.Time);
    
    UpdateReport645CRC32();
}
#endif  //#if( SEL_DLT645_2007 == YES )
