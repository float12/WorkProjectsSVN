//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������   
//�������� 
//����        ң���ļ�
//�޸ļ�¼  
//----------------------------------------------------------------------
#include "AllHead.h"

#if(SEL_EVENT_DI_CHANGE == YES)
//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define DI_BUFFER						64
#define MAX_DI_NUM						1
//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------
static BYTE bDIStatusBuf[DI_BUFFER];		//ң�Ż�����,��ң��״̬
static BYTE bDIWPoint;	  					//ң�Ż�����дָ��
static BYTE bDIRPoint[MAX_DI_NUM];			//ң�Ż�������ָ��
static BYTE bDIStatus;						//��ǰң��״̬(�����)
static BYTE bDIChange;						//��Լ��ȡ��ң��״̬�Ƿ��λ
TDIPara  DIPara;							//��ǰң�ŷ�����ʱ�估У��
static BYTE bDIEventFlag;					//�洢ң��698�¼���־

//�ն�״̬����λ�¼� 0x3104
static const DWORD DIChangeOadConst[] =
{
	0x00,//��������ĸ���
};

//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: У��ң�ŷ�����ʱ���Ƿ���ȷ
//
//����: 		
//			
// 			                  
//����ֵ:	��
//
//��ע:   
//-----------------------------------------------
void CheckDIPara(void)
{
	if(lib_CheckCRC32((BYTE *)&DIPara.AntiShakeTime,sizeof(DIPara.AntiShakeTime)) != DIPara.CRC32)
	{
		if(api_VReadSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara), (BYTE *)&DIPara) == FALSE)
		{
			memcpy((BYTE *)&DIPara.AntiShakeTime,(BYTE *)&DIParaconst,sizeof(TDIPara));
		}
	}
}

//-----------------------------------------------
//��������: ң���ϵ��ʼ��
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	
//-----------------------------------------------
void api_PowerUpDI(void)
{
	CheckDIPara();
	memset((void*)bDIStatusBuf,0,sizeof(bDIStatusBuf));
	bDIWPoint = 0;
	memset((void*)bDIRPoint,0,sizeof(bDIRPoint));
	bDIStatus = 0;
	bDIChange = 0;
	bDIEventFlag = 0;
}
//-----------------------------------------------
//��������: ң�Ŵ�ѭ������
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	
//-----------------------------------------------
void api_DIProcTask(void)
{
	BYTE uci,ucj;
	BYTE ucStatus,ucStatusN;
	BYTE ucStatus_old;
	DWORD ucCount;

	if(DIPara.bAccessState == 0)
	{
		bDIStatus = 0;
		bDIChange = 0;
		bDIEventFlag = 0;
		return;
	}

	ucStatus_old = bDIStatus;
	for(uci = 0;uci < MAX_DI_NUM;uci++)
	{
		ucStatus = (bDIStatusBuf[bDIRPoint[uci]]&(0x01<<uci));
		ucCount = 0;
		ucj = bDIRPoint[uci];
		while(ucj!=bDIWPoint)
		{
			ucStatusN = (bDIStatusBuf[ucj]&(0x01<<uci));
			if(ucStatus != ucStatusN)
			{//ң�ŷ����仯
				bDIRPoint[uci] = ucj;
				ucStatus = ucStatusN;
				ucCount = 0;
			}
			else
			{
				ucCount += 1;//�������������һ��
			}
			ucj++;
			if(ucj>=DI_BUFFER)
			{
				ucj = 0;
			}
		}
		if(ucCount >= DIPara.AntiShakeTime)
		{
			if(ucStatusN)
			{
				bDIStatus |= (0x01<<uci);			
			}
			else
			{
				bDIStatus &= (~(0x01<<uci));
			}
			bDIRPoint[uci] = bDIWPoint;
		}
	}
	
	if( ucStatus_old != bDIStatus )
	{
		bDIChange = 1;
		bDIEventFlag = 1;
	}
}
//-----------------------------------------------
//��������: ��ȡ�洢ң��698�¼���־
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	
//-----------------------------------------------
BYTE api_GetDIEventStatus( BYTE Phase )
{
	BYTE bTemp = bDIEventFlag;	
	
	bDIEventFlag = 0;	//��ȡ״̬�󣬱�λ״̬����
	
	return bTemp;
}
//-----------------------------------------------
//��������: ��ȡ��ǰң��״̬
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	
//-----------------------------------------------
BYTE api_ReadDIStatus( void )
{
	return bDIStatus;
}

//-----------------------------------------------
//��������: ��ȡ��ǰң�ű�λ״̬
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	
//-----------------------------------------------
BYTE api_ReadDIChange( void )
{
	BYTE bTemp = bDIChange;	
	
	bDIChange = 0;	//��Լ��ȡ�󣬱�λ״̬����
	
	return bTemp;
}

//-----------------------------------------------
//��������: ʵʱң��״̬��ȡ
//
//����: 		��
//			             
//����ֵ:	��
//
//��ע:   	10ms�жϵ���
//-----------------------------------------------
void api_DISample(void)
{
	BYTE ucTmp = 0xff;

	if(DIGITAL_INPUT0_STATUS)
	{
		ucTmp &= 0xfe;//����Bit0λ
	}
	
	bDIStatusBuf[bDIWPoint] = ucTmp;
	bDIWPoint++;
	if(bDIWPoint >= DI_BUFFER)
	{
		bDIWPoint = 0;
	}  
}

//--------------------------------------------------
//��������:  ��ȡң�ŷ�����ʱ��
//����	:	DINO:·��
//			pBuf[out]
//����ֵ:	Lenth
//ע:
//--------------------------------------------------
WORD api_ReadDIPara(BYTE bType,BYTE *pBuf )
{	
	if(bType == 0)
	{
		memcpy(pBuf, (BYTE *)&DIPara.AntiShakeTime,sizeof(DIPara.AntiShakeTime));
		return sizeof(DIPara.AntiShakeTime);	
	}
	else if(bType == 1)
	{
		memcpy(pBuf, (BYTE *)&DIPara.bAccessState,sizeof(DIPara.bAccessState));
		return sizeof(DIPara.bAccessState);			
	}
	else if(bType == 2)
	{
		memcpy(pBuf, (BYTE *)&DIPara.bAttributeState,sizeof(DIPara.bAttributeState));
		return sizeof(DIPara.bAttributeState);			
	}
	return FALSE;
}
//--------------------------------------------------
//��������:  ����ң�ŷ�����ʱ��
//����	:	DINO:·��
//			pBuf[in]
//����ֵ:	BOOL
//ע:
//--------------------------------------------------
BOOL api_WriteDIPara(BYTE bType, BYTE *pBuf )
{
	DWORD AntiShakeTime = 0;

	if(bType == 0)
	{
		memcpy((BYTE *)&AntiShakeTime, pBuf, 4);
		DIPara.AntiShakeTime = AntiShakeTime;
	}
	else if(bType == 1)
	{
		if(pBuf[0] > 1)
		{
			return FALSE;
		}
		DIPara.bAccessState = pBuf[0];
	}
	else if(bType == 2)
	{
		if(pBuf[0] > 1)
		{
			return FALSE;
		}
		DIPara.bAttributeState = pBuf[0];		
	}
	if(api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara),(BYTE *)&DIPara) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------
//��������: ��ʼ��ң�ŷ�����ʱ��
//
//����: 
//	��
//	
//  ����ֵ:	
//  ��
//
//��ע:	  ��ʼ������
//-----------------------------------------------
void api_FactoryInitDIPara(void)
{
	memcpy((BYTE *)&DIPara.AntiShakeTime,(BYTE *)&DIParaconst,sizeof(WORD)*MAX_DI_NUM);
	api_VWriteSafeMem(GET_SAFE_SPACE_ADDR(ParaSafeRom.DIPara),sizeof(TDIPara),(BYTE *)&DIPara);
}

//-----------------------------------------------
//��������: ��ʼ���ն�״̬����λ�¼�����
//
//����: 	��
//	
//  ����ֵ:	  ��
//
//��ע:	  ��ʼ������
//-----------------------------------------------
void api_FactoryInitDIChangePara(void)
{
	api_WriteEventAttribute( 0x3104, 0xff, (BYTE *)&DIChangeOadConst[0], sizeof(DIChangeOadConst)/sizeof(DWORD) );
}
#endif//#if(SEL_EVENT_DI_CHANGE == YES)