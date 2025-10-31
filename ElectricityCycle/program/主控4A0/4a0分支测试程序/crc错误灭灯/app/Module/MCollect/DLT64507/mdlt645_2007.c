
#include "appcfg.h"
#if(MD_MDLT645_07)
#include "mdlt645.h"
#include "../GDW698/GDW698DataDef.h"

// extern BYTE GetReal2FrzFlg(void);
// extern BOOL GetCurvStartTimeEx(TTime *pTime,BYTE *pbyStepNum);

// static void _Tx_645_07_Read(DWORD dwDI,BYTE *pData,BYTE byDataLen)
// {
// TPort *pPort=(TPort*)GethPort();	
// TTx *pTx=GetTx();

// 	_Tx_Fm645Read(TRUE,dwDI,pData,byDataLen,pPort->GyRunInfo.Addr,pTx);
// 	SetSunitemFlag(TRUE,dwDI,TRUE);
// }

// static void Tx_645_07_Read(DWORD dwDI)
// {
// 	_Tx_645_07_Read(dwDI,NULL,0);
// }

// static BOOL MonDD_Tx(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_MON_APR))
// 	{//��1����������й�
// 		Tx_645_07_Read(0x0000ff01);
// 		return TRUE;				
// 	}
// 	if(CheckClearFlag(MSF_MON_AP))
// 	{//�����й�
// 		Tx_645_07_Read(0x0001FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_AR))
// 	{//�����й�
// 		Tx_645_07_Read(0x0002FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_RP))
// 	{//�����޹�
// 		Tx_645_07_Read(0x0003FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_RR))
// 	{//�����޹�
// 		Tx_645_07_Read(0x0004FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R1))
// 	{//һ�����޹�
// 		Tx_645_07_Read(0x0005FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R2))
// 	{//�������޹�
// 		Tx_645_07_Read(0x0006FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R3))
// 	{//�������޹�
// 		Tx_645_07_Read(0x0007FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_R4))
// 	{//�������޹�
// 		Tx_645_07_Read(0x0008FF01);
// 		return TRUE;
// 	}
// 	//����
// 	if(CheckClearFlag(TBD))
// 	{//ͭ�𲹳�����
// 		Tx_645_07_Read(0x00850001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(TBD))
// 	{//���𲹳�����
// 		Tx_645_07_Read(0x00860001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MON_A))
// 	{//A�������й�
// 		if(Sunitem(TRUE,0x00150001,MSF_MON_A,3,HL,0x16,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_MON_B))
// 	{//B��
// 		if(Sunitem(TRUE,0x00290001,MSF_MON_B,3,HL,0x2A,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_MON_C))
// 	{//C��
// 		if(Sunitem(TRUE,0x003D0001,MSF_MON_C,3,HL,0x3E,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

// static BOOL MonXL_Tx(void)
// {
// 	if(CheckClearFlag(MSF_MONXLAP))
// 	{//���������й�����
// 		Tx_645_07_Read(0x0101FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLAR))
// 	{//���·����й�����
// 		Tx_645_07_Read(0x0102FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLRP))
// 	{//���������޹�����
// 		Tx_645_07_Read(0x0103FF01);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_MONXLRR))
// 	{//���·����޹�����
// 		Tx_645_07_Read(0x0104FF01);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// static BOOL MeterTime_Tx(void)
// {
// 	if(CheckClearFlag(MSF_DATE))
// 	{//����		
// 		Tx_645_07_Read(0x04000101);		
// 		return TRUE;
// 	}	
// 	if(CheckClearFlag(MSF_TIME))
// 	{//ʱ�� 
// 		Tx_645_07_Read(0x04000102);
// 		return TRUE;
// 	}	
// 	return FALSE;
// }

// static BOOL MonData_Tx(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_AUTODAY))
// 	{//��һ�Զ�������/*ɽ���ڶ������Զ�������*/
// 		if(Sunitem(TRUE,0x04000B01,MSF_AUTODAY,2,LL,2,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	//�¶������
// 	if(MonDD_Tx())
// 		return TRUE;
// 	//�¶�������
// 	if(MonXL_Tx())
// 		return TRUE;
// 	return FALSE;
// }

// static BOOL DayData_Tx(void)
// {
// TPort *pPort=(TPort *)GethPort();
// BYTE byOffset=pPort->GyRunInfo.byHisDataOffset&0x7F;		

// 	if(CheckClearFlag(MSF_DAY_TM))
// 	{//��һ�ζ���ʱ��
// 		pPort->GyRunInfo.byTxIDFlag = MSF_DAY_TM;
// 		Tx_645_07_Read(0x05060001+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_AP))
// 	{//��1���ն��������й�
// 		Tx_645_07_Read(0x05060101+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_AR))
// 	{//��1���ն��ᷴ���й�
// 		Tx_645_07_Read(0x05060201+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_RP))
// 	{//��1���ն�������޹�1
// 		Tx_645_07_Read(0x05060301+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_RR))
// 	{//��1���ն�������޹�2
// 		Tx_645_07_Read(0x05060401+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R1))
// 	{//��1��һ�����޹�
// 		Tx_645_07_Read(0x05060501+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R2))
// 	{//��1�ζ������޹�
// 		Tx_645_07_Read(0x05060601+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R3))
// 	{//��1���������޹�
// 		Tx_645_07_Read(0x05060701+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAY_R4))
// 	{//��1���������޹�
// 		Tx_645_07_Read(0x05060801+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYXLAP))
// 	{//��һ���ն�����������
// 		Tx_645_07_Read(0x05060901+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYXLAR))
// 	{//��һ���ն��ᷴ������
// 		Tx_645_07_Read(0x05060A01+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYLEFTPAY))
// 	{
// 		Tx_645_07_Read(0x05080201+byOffset);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAYNUM))
// 	{//��1�ι�����ܹ������
// 		Tx_645_07_Read(0x03330201);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAYSUM))
// 	{//��1�ι�����ۼƹ�����
// 		Tx_645_07_Read(0x03330601);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_LEFTDD))
// 	{//ʣ����
// 		Tx_645_07_Read(0x00900200);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_OVERDRAFT))
// 	{//͸֧���
// 		Tx_645_07_Read(0x00900201);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_DAYP07))
// 	{
// 		Tx_645_07_Read(0x05061001+byOffset);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Cure_Tx
// //	�������� : ���������в�
// //	������� : 
// //	��    ע : 
// //	��    �� : �Ŷ�
// //	ʱ    �� : 2016��2��17��
// //	�� �� ֵ : BOOL
// //	����˵�� : void
// ///////////////////////////////////////////////////////////////
// static BOOL Cure_Tx(void)
// {//����
// BYTE bystep =1;	
// TPort *pPort=(TPort *)GethPort();
// 	if(CheckClearFlag(MSF_CURE))
// 	{
// 		TTime Time;		
// 		if(GetCurvStartTime(&Time))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= 1;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06000001,Buf,6);
// 			SetFlag(MSF_CURE);
// 			return TRUE;
// 		}		
// 	}
// 	if(pPort->GyRunInfo.byTxIDFlag == 0xFF)
// 		pPort->GyRunInfo.byTxIDFlag = MSF_CURE1;//�˴����ã���return FALSE�����˳�ǰ��� 
// 	if(CheckClearFlag(MSF_CURE1))
// 	{//��ѹ
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100101,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE2))
// 	{//����
// 		TTime Time;	
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100201,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE3))
// 	{//����
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100300,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE4))
// 	{//��������
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100500,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE5))
// 	{//����
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100601,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(CheckClearFlag(MSF_CURE6))
// 	{//����
// 		TTime Time;		
// 		if(GetCurvStartTimeEx(&Time,&bystep))
// 		{			
// 			BYTE Buf[6];
// 			Buf[0]	= bystep;
// 			Buf[1]	= Bin2Bcd(Time.Min);
// 			Buf[2]	= Bin2Bcd(Time.Hour);
// 			Buf[3]	= Bin2Bcd(Time.Day);
// 			Buf[4]	= Bin2Bcd(Time.Mon);
// 			Buf[5]	= Bin2Bcd((BYTE)(Time.wYear%100));
// 			_Tx_645_07_Read(0x06100602,Buf,6);
// 			return TRUE;
// 		}		
// 	}
// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_CURE1)
// 		pPort->GyRunInfo.byTxIDFlag = 0xFF;
// 	return FALSE;
// }

///////////////////////////////////////////////////////////////
//	�� �� �� : DD_Txd
//	�������� : �����������в�
//	������� : 
//	��    ע : OK
//	��    �� : �Ŷ�
//	ʱ    �� : 2016��2��17��
//	�� �� ֵ : BOOL
//	����˵�� : void
///////////////////////////////////////////////////////////////
// static BOOL DD_Txd(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_APR))
// 	{//����й�
// 		Tx_645_07_Read(0x0000ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_AP))
// 	{//�����й�	
// 		if(Sunitem(TRUE,0x0001ff00,MSF_AP,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_AR))
// 	{//�����й�	
// 		if(Sunitem(TRUE,0x0002ff00,MSF_AR,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_RP))
// 	{//�����޹�
// 		if(Sunitem(TRUE,0x0003ff00,MSF_RP,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_RR))
// 	{//�����޹�	
// 		if(Sunitem(TRUE,0x0004ff00,MSF_RR,1,LH,0,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}
// 	if(CheckClearFlag(MSF_R1))
// 	{//һ�����޹�	
// 		Tx_645_07_Read(0x0005ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R2))
// 	{//�������޹�	
// 		Tx_645_07_Read(0x0006ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R3))
// 	{//�������޹�	
// 		Tx_645_07_Read(0x0007ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_R4))
// 	{//�������޹�	
// 		Tx_645_07_Read(0x0008ff00);
// 		return TRUE;
// 	}

// 	if(CheckClearFlag(MSF_A_ARP))
// 	{//A�������
// 		if(Sunitem(TRUE,0x00150000,MSF_A_ARP,3,HL,0x16,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}		
// 	if(CheckClearFlag(MSF_B_ARP))
// 	{//B��
// 		if(Sunitem(TRUE,0x00290000,MSF_B_ARP,3,HL,0x2A,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	if(CheckClearFlag(MSF_C_ARP))
// 	{//C��
// 		if(Sunitem(TRUE,0x003D0000,MSF_C_ARP,3,HL,0x3E,&dwDI))
// 		{
// 			Tx_645_07_Read(dwDI);
// 			return TRUE;
// 		}
// 	}	
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : XL_Txd
// //	�������� : ���������в�
// //	������� : 
// //	��    ע : ok
// //	��    �� : �Ŷ�
// //	ʱ    �� : 2016��2��17��
// //	�� �� ֵ : BOOL
// //	����˵�� : void
// ///////////////////////////////////////////////////////////////
// static BOOL XL_Txd(void)
// {
// 	if(CheckClearFlag(MSF_APXL))
// 	{
// 		Tx_645_07_Read(0x0101ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_ARXL))
// 	{
// 		Tx_645_07_Read(0x0102ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_RPXL))
// 	{
// 		Tx_645_07_Read(0x0103ff00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_RRXL))
// 	{
// 		Tx_645_07_Read(0x0104ff00);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : SS_Txd
// //	�������� : ˲ʱ�������в�
// //	������� : 
// //	��    ע : ok
// //	��    �� : �Ŷ�
// //	ʱ    �� : 2016��2��17��
// //	�� �� ֵ : BOOL
// //	����˵�� : void
// ///////////////////////////////////////////////////////////////
// static BOOL SS_Txd(void)
// {
// DWORD dwDI;

// 	if(CheckClearFlag(MSF_VT))
// 	{//��ѹ
// 		if(Sunitem(TRUE,0x0201FF00,MSF_VT,3,LH,1,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_CT))
// 	{//����
// 		if(Sunitem(TRUE,0x0202ff00,MSF_CT,3,LH,1,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PA))
// 	{//�й�����
// 		if(Sunitem(TRUE,0x0203ff00,MSF_PA,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PR))
// 	{//�޹�����
// 		if(Sunitem(TRUE,0x0204ff00,MSF_PR,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PAP))
// 	{//���ڹ���
// 		if(Sunitem(TRUE,0x0205FF00,MSF_PAP,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_PE))
// 	{//��������
// 		if(Sunitem(TRUE,0x0206ff00,MSF_PE,4,LH,0,&dwDI))
// 			Tx_645_07_Read(dwDI);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_VIANGLE))
// 	{//��λ��
// 		Tx_645_07_Read(0x0207FF00);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_I0))
// 	{//�������
// 		Tx_645_07_Read(0x02800001);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_FREQ))
// 	{//Ƶ��
// 		Tx_645_07_Read(0x02800002);
// 		return TRUE;
// 	}	
// 	if(CheckClearFlag(MSF_STSMET))
// 	{//���״̬��
// 		Tx_645_07_Read(0x040005FF);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_STSMET1))
// 	{//���״̬��1
// 		Tx_645_07_Read(0x04000501);
// 		return TRUE;
// 	}
// 	if(CheckClearFlag(MSF_STSREST))
// 	{//������״̬��
// 		Tx_645_07_Read(0x04000801);
// 		return TRUE;
// 	}

// 	return FALSE;
// }


// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Rx_VCV 
// //	�������� : ��645��Լ��ѹ���մ���
// //	������� : 
// //	��    ע : ��ѹ(0.1V)
// //	��    �� : ����Ծ
// //	ʱ    �� : 2009��1��17��
// //	�� �� ֵ : void
// //	����˵�� : WORD wNo 
// ///////////////////////////////////////////////////////////////
// static void Rx_VCV(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// WORD wVCV;

// 	for(byNo=0;(byNo<(byLen/2))&&(byNo<3);byNo++)
// 	{//��BCDת��Ϊ������
// 		wVCV = MW(pBuf[1],pBuf[0]);
// 		if(IsBCD(wVCV))
// 		{
// 			wVCV = WBcd2Bin(wVCV)*10;
// 			//�����ѹ
// 			WriteYC(byNo+wNo,wVCV);						
// 		}
// 		pBuf += 2;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Rx_VCI 
// //	�������� : ��645��Լ�������մ���
// //	������� : 
// //	��    ע : ����(0.001A)
// //	��    �� : ����Ծ
// //	ʱ    �� : 2009��1��17��
// //	�� �� ֵ : void
// //	����˵�� : BYTE byNo
// ///////////////////////////////////////////////////////////////
// static void Rx_VCI(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {
// BYTE byI;
// DWORD dwVCI;
	
// 	for(byI=0;(byI<(byLen/3))&&(byI<3);byI++)
// 	{
// 		//��BCDת��Ϊ������
// 		dwVCI = MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwVCI))
// 		{
// 			dwVCI = DWBcd2Bin(dwVCI);			
// 			if(pBuf[2]&0x80)	//ȡ����					
// 				WriteYC(byNo+byI,(long)(0-dwVCI));
// 			else
// 				WriteYC(byNo+byI,dwVCI);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 3;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Rx_P07
// //	�������� : ��645��Լ���ʽ��մ���(�й����޹�������)
// //	������� : 
// //	��    ע : 0.0001kW
// //	��    �� : ����Ծ
// //	ʱ    �� : 2009��1��17��
// //	�� �� ֵ : void
// //	����˵�� : WORD wNo
// ///////////////////////////////////////////////////////////////
// static void Rx_P07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// DWORD dwData;
// BYTE byNo;

// 	for(byNo=0;(byNo<(byLen/3))&&(byNo<4);byNo++)
// 	{
// 		//��BCDת��Ϊ������
// 		dwData		= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwData))
// 		{
// 			dwData		= DWBcd2Bin(dwData);			
// 			//���湦��
// 			if(pBuf[2]&0x80)		//ȡ����
// 				WriteYC(byNo+wNo,(long)(0-dwData));
// 			else
// 				WriteYC(byNo+wNo,dwData);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 3;
// 	}
// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Rx_PE
// //	�������� : ��645��Լ�������ؽ��մ���
// //	������� : 
// //	��    ע : 
// //	��    �� : ����Ծ
// //	ʱ    �� : 2009��1��17��
// //	�� �� ֵ : void
// //	����˵�� : WORD wNo
// ///////////////////////////////////////////////////////////////
// static void Rx_PE07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// DWORD dwPEVal;//�ĳ�DWORD ��ֹԽ��
// BYTE byNo;
		
// 	for(byNo=0;(byNo<(byLen/2))&&(byNo<4);byNo++)
// 	{
// 		//��BCDת��Ϊ������
// 		dwPEVal = MDW(0,0,(BYTE)(pBuf[1]&0x7f),pBuf[0]);
// 		if(IsBCD(dwPEVal))
// 		{
// 			dwPEVal = DWBcd2Bin(dwPEVal)*10;			
// 			//���湦������
// 			if(pBuf[1]&0x80)	//ȡ����
// 				WriteYC(byNo+wNo,(long)(0-dwPEVal));
// 			else
// 				WriteYC(byNo+wNo,dwPEVal);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 2;
// 	}
// }

// static void Rx_MetStatus07(BYTE *pBuf,BYTE byLen,BYTE byNo)
// {

		
// }

// static void Rx_XB(BYTE *pBuf,BYTE byLen,WORD wNo)
// {

// }

// ///////////////////////////////////////////////////////////////
// //	�� �� �� : Rx_XL07
// //	�������� : ��645����������ʱ�����ݽ��մ���
// //	������� : 
// //	��    ע : 4λС����(0.0001kW)
// //	��    �� : ����Ծ
// //	ʱ    �� : 2009��1��17��
// //	�� �� ֵ : void
// //	����˵�� : WORD wNo  ����������ʼ���
// //				TXL *pXL ����ֵ��ָ��
// ///////////////////////////////////////////////////////////////
// static void Rx_XL07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{	

// 		if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//�޹�������Ϊ����
// 		{
// 			Data.dwVal  = MDW(0,pBuf[2]&0x7F,pBuf[1],pBuf[0]);
// 		}
// 		else
// 		{
// 			Data.dwVal	= MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		}
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//�޹�������Ϊ����
// 				Data.dwVal = 0-Data.dwVal;
// 			WriteXLVal((wNo+byNo),Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);			
// 			Data.Time.Buf[0]	= 0;
// 			WriteXLTime((wNo+byNo),&Data.Time);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 8;
// 	}
// }


// static void Rx_MonXL07(BYTE *pBuf,BYTE byLen,WORD wNo)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{	
// 		if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//�޹�������Ϊ����
// 		{
// 			Data.dwVal  = MDW(0,pBuf[2]&0x7F,pBuf[1],pBuf[0]);
// 		}
// 		else
// 		{
// 			Data.dwVal	= MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		}
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			if(((wNo == 10)||(wNo == 15))&&((pBuf[2]&0x80)==0x80))//�޹�������Ϊ����
// 				Data.dwVal = 0-Data.dwVal;
// 			WriteMonXLVal((wNo+byNo),Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);			
// 			Data.Time.Buf[0]	= 0;
// 			WriteMonXLTime((wNo+byNo),&Data.Time);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 8;
// 	}
// }
// static void Rx_DayP07(BYTE *pBuf,BYTE byLen,BYTE byStartNo,DWORD dwSeq)
// {
// 	DWORD dwData;
// 	BYTE byNo;
	
// 	for(byNo=0;(byNo<(byLen/3))&&(byNo<8);byNo++)
// 	{
// 		//��BCDת��Ϊ������
// 		dwData		= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 		if(IsBCD(dwData))
// 		{
// 			dwData		= DWBcd2Bin(dwData);			
// 			//���湦��
// 			if(pBuf[2]&0x80)		//ȡ����
// 				WriteYC(byNo+6,(long)(0-dwData));
// 			else
// 				WriteYC(byNo+6,dwData);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 3;
// 	}
// }
// static void Rx_DayDD(BYTE *pBuf,BYTE byLen,BYTE byStartNo,DWORD dwSeq)
// {
// BYTE byNo;//�����ݱ�ʶ
// DWORD dwDD;
// #if(MD_BASEUSER	== MD_GDW698)
// long CI;
// #endif

// 	for(byNo=0;(byNo<(byLen/4))&&(byNo<5);byNo++)
// 	{
// 		//��BCDת��Ϊ������
// 		dwDD = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 		if ((byStartNo==10) || (byStartNo==15))
// 		{
// 			if (pBuf[3]&BIT7)
// 				dwDD = MDW((BYTE)(pBuf[3]&0x7F),pBuf[2],pBuf[1],pBuf[0]);
// 		}
		
// 		if(IsBCD(dwDD))
// 		{
// 			dwDD = DWBcd2Bin(dwDD);
// #if(MD_BASEUSER	== MD_GDW698)
// 			if ((byStartNo==10) || (byStartNo==15))//����޹�1 ����޹�2���λΪ����λ
// 			{
// 				if (pBuf[3]&BIT7)
// 				{
// 					dwDD = (DWORD)(0-dwDD);
// 					CI = (long)dwDD;
// 					CI *= 10;
// 					dwDD = (DWORD)CI;
// 				}
// 				else
// 					dwDD *= 10;
// 			}
// 			else
// 				dwDD *= 10;
// #else
// 			dwDD *= 10;
// #endif		
// 			WriteDayDD((DWORD)(byStartNo+byNo),dwDD);
// 		}		
// 		//ָ����һ���ݿ�
// 		pBuf += 4;
// 	}
// }

// static void Rx_DayTM(BYTE *pBuf,BYTE byLen)
// {
// TTime Time,Time2;
// DWORD dwTime,dwTime2;
// TPort *pPort=(TPort *)GethPort();
// BYTE byOffset;
    
//     memset(&Time,0,sizeof(TTime));
//     Time.wYear= (WORD)(Bcd2Bin(pBuf[4])+2000);
// 	Time.Mon = Bcd2Bin(pBuf[3]);
// 	Time.Day = Bcd2Bin(pBuf[2]);
// 	Time.Hour= Bcd2Bin(pBuf[1]);
// 	Time.Min = Bcd2Bin(pBuf[0]);
// 	if(!TimeIsValid(&Time))
// 	{//����ʱ����Ч��ֱ�ӽ�����һ��
// 		if (GetReal2FrzFlg() == 0x5A)	//ʵʱת�ն���
// 			Fre2RealFlag();
// 		else
// 			ClearAllFlag();		
// 		pPort->GyRunInfo.byReqDataValid	= 0x55;		
//         return;
// 	}	
// 	byOffset	= pPort->GyRunInfo.byHisDataOffset&0x7F;	
// 	dwTime	= Time2Min(&Time);
// 	WriteOther(1,dwTime);
// 	dwTime = (dwTime/1440)*1440+byOffset*1440;
// 	if((pPort->GyRunInfo.byHisDataOffset & 0x80) == 0)
// 	{//��Ҫ�ж�ʱ��
// 		GetTime(&Time2);
// 		dwTime2 = (Time2Min(&Time2)/1440)*1440;
// 		if(dwTime != dwTime2)
// 		{//ʱ�䲻�Բ�������
// 			if (GetReal2FrzFlg() == 0x5A)	//ʵʱת�ն���
// 				Fre2RealFlag();
// 			else
// 				ClearAllFlag();
// 			pPort->GyRunInfo.byReqDataValid	= 0;	
// 		}
// 	}	
// }

// static void Rx_DayXL(BYTE *pBuf,BYTE byLen,WORD wNo,DWORD dwSeq)
// {
// BYTE byNo;
// UData Data;
	
// 	for(byNo=0;(byNo<(byLen/8))&&(byNo<5);byNo++)
// 	{
// 		Data.dwVal = MDW(0,pBuf[2],pBuf[1],pBuf[0]);
// 		if(IsBCD(Data.dwVal))
// 		{
// 			Data.dwVal = DWBcd2Bin(Data.dwVal);
// 			WriteDayXLVal(wNo+byNo,Data.dwVal);
// 			Data.Time.Buf[5]	= Bcd2Bin(pBuf[7]);
// 			Data.Time.Buf[4]	= Bcd2Bin(pBuf[6]);
// 			Data.Time.Buf[3]	= Bcd2Bin(pBuf[5]);
// 			Data.Time.Buf[2]	= Bcd2Bin(pBuf[4]);
// 			Data.Time.Buf[1]	= Bcd2Bin(pBuf[3]);
// 			Data.Time.Buf[0]	= 0;
// 			WriteDayXLTime(wNo+byNo,&Data.Time);
// 		}
// 		//ָ����һ���ݿ�
// 		pBuf += 8;
// 	}
// }

// static BYTE _SwitchToNextDataBlock(BYTE *pBuf)
// {
// BYTE byI;

// 	for(byI=0;byI<100;byI++)
// 	{
// 		if(pBuf[byI] == 0xAA)
// 		{
// 			break;
// 		}
// 	}
// 	return (BYTE)(byI+1);
// }

// static void Rx_Curve(BYTE *pBuf,BYTE byLen)
// {
// BYTE byInd=0,byCurvLen,byI,byCheckSum=0,byStep=0,byTmp;
// WORD wTmp;
// TTime TCTime;
// DWORD dwTmp;
// long nTmp;
// #if(MD_BASEUSER	== MD_GDW698)
// long CI;
// #endif

// 	ClearFlag(MSF_CURE1);ClearFlag(MSF_CURE2);ClearFlag(MSF_CURE3);
// 	ClearFlag(MSF_CURE4);ClearFlag(MSF_CURE5);ClearFlag(MSF_CURE6);
// 	//1����ʼ��ʶ�ж�
// 	if((pBuf[0] != 0xA0)||(pBuf[1] != 0xA0))
// 	{
// 		return;
// 	}
// 	byCurvLen	= pBuf[2];
// 	if(byCurvLen > byLen)
// 		return;
// 	//2������У��
// #if(!MOS_MSVC)
// 	for(byI=0;byI<(BYTE)(byCurvLen+3);byI++)
// 	{
// 		byCheckSum	+= pBuf[byI];
// 	}
// 	if(byCheckSum != pBuf[byI])
// 		return;
// #endif
// 	pBuf	+= 3;
// 	while(byInd<byCurvLen)
// 	{
// 		byTmp	= _SwitchToNextDataBlock(&pBuf[0]);
// 		switch(byStep)
// 		{
// 		case 0: //ʱ��			
// 			TCTime.wMSec = 0;
// 			TCTime.Sec	 = 0;
// 			TCTime.Min	 = Bcd2Bin(pBuf[0]);
// 			TCTime.Hour	 = Bcd2Bin(pBuf[1]);
// 			TCTime.Day	 = Bcd2Bin(pBuf[2]);
// 			TCTime.Mon	 = Bcd2Bin(pBuf[3]);
// 			TCTime.wYear = Bcd2Bin(pBuf[4])+2000;	
// 			//.if(!TimeIsValid(&TCTime))
// 			//.	return;
// 			byStep	= 1;
// 		case 1: //��ѹ��������Ƶ��
// 			if(byTmp == 23)
// 			{
// 				for( byI=0; byI<3; byI ++)
// 				{//��ѹ
// 					wTmp	= MW(pBuf[byI*2+1+5],pBuf[byI*2+5]);
// 					if(IsBCD(wTmp))
// 					{
// 						nTmp	= (long)(WBcd2Bin(wTmp)*10);					
// 						WriteCurvYC(&TCTime,byI,nTmp);
// 					}				
// 					dwTmp	= MDW(0,(BYTE)(pBuf[6+3*byI+2+5]&0x7f),pBuf[6+3*byI+1+5],pBuf[6+3*byI+5]);
// 					if(IsBCD(dwTmp))
// 					{
// 						nTmp	= (long)DWBcd2Bin(dwTmp);
// 						if(pBuf[6+3*byI+2+5] & 0x80)
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,byI+3,nTmp);
// 					}					
// 				}			
// 			}
// 			break;
// 		case 2: //���޹�����
// 			if(byTmp == 25)
// 			{
// 				for( byI=0; byI<8; byI ++)
// 				{
// 					dwTmp	= MDW(0,(BYTE)(pBuf[byI*3+2]&0x7f),pBuf[byI*3+1],pBuf[byI*3]);
// 					if(IsBCD(dwTmp))
// 					{				
// 						nTmp	= (long)DWBcd2Bin(dwTmp);
// 						if((pBuf[3*byI+2]&0x80)==0x80)//ȡ����
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,6+byI,nTmp);
// 					}
// 				}
// 			}
// 			break;
// 		case 3: //��������
// 			if(byTmp == 9)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp	= MW((BYTE)(pBuf[byI*2+1]&0x7f),pBuf[byI*2]);
// 					if(IsBCD(dwTmp))
// 					{
// 						nTmp	= (long)WBcd2Bin((WORD)dwTmp)*10;
// 						if((pBuf[byI*2+1]&0x80)==0x80)//ȡ����
// 						{
// 							nTmp	= 0-nTmp;
// 						}
// 						WriteCurvYC(&TCTime,byI+14,nTmp);
// 					}
// 				}
// 			}
// 			break;
// 		case 4: //���޹��ܵ���
// 			if(byTmp == 17)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp = MDW(pBuf[byI*4+3],pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4+0]);
// 					if ((byI==2) || (byI==3))				//����޹�1 ����޹�2���λΪ����λ
// 					{
// 						if (pBuf[byI*4+3]&BIT7)
// 							dwTmp = MDW((BYTE)(pBuf[byI*4+3]&0x7F),pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4+0]);
// 					}
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp);
// #if(MD_BASEUSER	== MD_GDW698)
// 						if ((byI==2) || (byI==3))			//����޹�1 ����޹�2���λΪ����λ
// 						{
// 							if (pBuf[byI*4+3]&BIT7)
// 							{
// 								dwTmp = (DWORD)(0-dwTmp);
// 								CI = (long)dwTmp;
// 								CI *= 10;
// 								dwTmp = (DWORD)CI;
// 							}
// 							else
// 								dwTmp *= 10;
// 						}
// 						else
// 							dwTmp *= 10;
// #else
// 						dwTmp *= 10;
// #endif
// 						WriteCurvDD(&TCTime,byI*5,dwTmp);
// 					}					
// 				}
// 			}
// 			break;
// 		case 5: //�������޹�����
// 			if(byTmp == 17)
// 			{
// 				for( byI=0; byI<4; byI ++)
// 				{
// 					dwTmp = MDW(pBuf[byI*4+3],pBuf[byI*4+2],pBuf[byI*4+1],pBuf[byI*4]);
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp)*10;
// 						WriteCurvDD(&TCTime,byI*5+20,dwTmp);
// 					}

// 				}
// 			}
// 			break;
// 		case 6: //��ǰ����
// 			if(byTmp == 7)
// 			{//���з���λ
// 				for( byI=0; byI<2; byI++)
// 				{
// 					dwTmp = MDW(0,(pBuf[byI*3+2]&0x7f),pBuf[byI*3+1],pBuf[byI*3]);			
// 					if(IsBCD(dwTmp))
// 					{
// 						dwTmp = DWBcd2Bin(dwTmp);
// #if(MD_BASEUSER == MD_GDW698)//376��Լ�ò�������ȥ����
// 						if((pBuf[byI*3+2]&0x80)==0x80)//����
// 							dwTmp = 0-dwTmp;							
// #endif
// 						WriteCurvXL(&TCTime,byI,dwTmp);
// 					}
// 				}
// 			}
// 			break;
// 		default:
// 			byTmp	= 100;
// 			break;
// 		}
// 		if(byTmp >= 100)
// 				{
// 			break;
// 		}	
// 		pBuf	+= byTmp;
// 		byInd	+= byTmp;
// 		byStep++;
// 	}
// }
// static void Rx_CurveEx(BYTE no,BYTE *pBuf,BYTE byLen)
// {
// BYTE byInd=0,byCurvLen,byStep=0,byTmp;
// WORD wTmp;
// TTime TCTime;
// DWORD dwTmp;
// long nTmp;
// 	byCurvLen	= byLen;
// 	TCTime.wMSec = 0;
// 	TCTime.Sec	 = 0;
// 	TCTime.Min	 = Bcd2Bin(pBuf[0]);
// 	TCTime.Hour	 = Bcd2Bin(pBuf[1]);
// 	TCTime.Day	 = Bcd2Bin(pBuf[2]);
// 	TCTime.Mon	 = Bcd2Bin(pBuf[3]);
// 	TCTime.wYear = Bcd2Bin(pBuf[4])+2000;
// 	pBuf += 5;
// 	byInd += 5;
// 	while(byInd<byCurvLen)
// 	{
// 		switch(no)
// 		{
// 		case 1: //��ѹ
// 			wTmp	= MW(pBuf[1],pBuf[0]);
// 			if(IsBCD(wTmp))
// 			{
// 				nTmp	= (long)(WBcd2Bin(wTmp)*10);					
// 				WriteCurvEx(&TCTime,byStep,0,nTmp);
// 			}	
// 			byTmp = 2;
// 			break;
// 		case 2: //����
// 			dwTmp	= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{				
// 				nTmp	= (long)DWBcd2Bin(dwTmp);
// 				if((pBuf[2]&0x80)==0x80)//ȡ����
// 				{
// 					nTmp	= 0-nTmp;
// 				}
// 				WriteCurvEx(&TCTime,byStep,3,nTmp);
// 			}
// 			byTmp = 3;
// 			break;
// 		case 3: //����
// 			dwTmp	= MDW(0,(BYTE)(pBuf[2]&0x7f),pBuf[1],pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{
// 				nTmp	= (long)WBcd2Bin((WORD)dwTmp);
// 				if((pBuf[2]&0x80)==0x80)//ȡ����
// 				{
// 					nTmp	= 0-nTmp;
// 				}
// 				WriteCurvEx(&TCTime,byStep,6,nTmp);
// 			}
// 			byTmp = 3;
// 			break;
// 		case 5: //��������
// 			dwTmp = MDW(0,0,(BYTE)(pBuf[1]&0x7f),pBuf[0]);
// 			if(IsBCD(dwTmp))
// 			{
// 				nTmp = ((long)DWBcd2Bin(dwTmp))*10;
// 				if((pBuf[1]&0x80)==0x80)//ȡ����
// 				{
// 					nTmp	= 0-nTmp;
// 				}				
// 				WriteCurvEx(&TCTime,byStep,14,nTmp);
// 			}	
// 			byTmp = 2;
// 			break;
// 		case 6: //�й��ܵ���
// 			dwTmp = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 			dwTmp = DWBcd2Bin(dwTmp)*10;
// 			WriteCurvEx(&TCTime,byStep,40,dwTmp);
// 			byTmp = 4;
// 			break;
// 		case 7: //�����ܵ���
// 			dwTmp = MDW(pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
// 			dwTmp = DWBcd2Bin(dwTmp)*10;
// 			WriteCurvEx(&TCTime,byStep,45,dwTmp);
// 			byTmp = 4;
// 			break;
// 		default:
// 			byTmp	= 100;
// 			break;
// 		}
// 		if(byTmp >= 100)
// 		{
// 			break;
// 		}	
// 		pBuf	+= byTmp;
// 		byInd	+= byTmp;
// 		byStep++;
// 	}
// }

// static void Rx_MetStatusUP(BYTE *pBuf,BYTE byLen)
// {//�����ϱ�״̬��12�ֽ�

// 	UData Data;
// 	Data.dwVal	= (DWORD)pBuf;
// //	WriteMeterRunInfo(DT_F31,byLen,Data);	
	
// }


void MDlt645_2007_RxMonitor(HPARA hPort)
{
	TPort *pPort=(TPort *)hPort;
#if(MD_BASEUSER	== MD_GDW698)
	TOad60170200 *pSch=(TOad60170200 *)pPort->GyRunInfo.psch698;
#endif
	BYTE *pBuf=(BYTE*)GetRxFm();
	TMDlt645Fm *pRxFm=(TMDlt645Fm *)pBuf;
	BYTE *pRxData=(BYTE *)&pRxFm[1];
	BYTE *pRead=&pRxData[4];
	DWORD dwDataType;
	BYTE byErr;
	BYTE byRLen;
	
	byRLen = MDlt645_RxMonitor(TRUE,pBuf,&dwDataType,&byErr);
	//�������������
	// if(pRxFm->byCtrl&NODATA)
	// {
	// 	if( GetFlag(MSF_CURE) || (pPort->GyRunInfo.byReqType == REQ_CURV) 
	// 		|| (pPort->GyRunInfo.byReqType == REQ_CURVEX))
	// 	{
	// 		ClearFlag(MSF_CURE);
	// 		if((pPort->GyRunInfo.byTxIDFlag != MSF_CURE1) && ((gVar.Cfg.byCurvSource == 0x55)||(pPort->GyRunInfo.byTxCurvSource == 0x55)))
	// 		{
	// 			if( (pPort->GyRunInfo.CurvCjCfg.wNo < pPort->GyRunInfo.CurvCjCfg.wNum 
	// 				&& pPort->GyRunInfo.CurvCjCfg.wNum > 1 ) 
	// 				|| ((pPort->GyRunInfo.CurvCjCfg.wNum==1) && (pPort->GyRunInfo.byReqType == REQ_CURV))  )
	// 			{
	// 				SetFlag(MSF_CURE1);SetFlag(MSF_CURE2);SetFlag(MSF_CURE3);
	// 				SetFlag(MSF_CURE4);SetFlag(MSF_CURE5);SetFlag(MSF_CURE6);
	// 			}
	// 		}
	// 	}
	// 	if((pPort->GyRunInfo.byReqType == REQ_DFRZ)
	// 		||(pPort->GyRunInfo.byReqType == REQ_CURV) 
	// 		|| (pPort->GyRunInfo.byReqType == REQ_CURVEX))
	// 	{//û�������������  
	// 		if(pPort->GyRunInfo.byReqDataValid == 0)
	// 		{
	// 			//.ClearAllFlag(); 
	// 			pPort->GyRunInfo.byReqDataValid	= 0xAA;	
	// 		}
	// 	}	
	// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_DAY_TM)
	// 	{//��һ���ն���ʱ��ط���ʱ
	// 		if (GetReal2FrzFlg() == 0x5A)	//ʵʱת�ն���
	// 			Fre2RealFlag();
	// 		else
	// 			ClearAllFlag();
	// 		pPort->GyRunInfo.byReqDataValid	= 0xAA;	//Ϊ��ʵ�ֲ������ӡ������ʵʱת�ն��ᣬ�˴�������Ӱ����Ϊ�ڴ洢����ʱ�Ὣvalid�ó�1
	// 	}

	// }
	// if((byErr != 0)||(byRLen == 0))
	// {
	// 	return;
	// }

	// SetSunitemFlag(TRUE,dwDataType,FALSE);
	// switch(dwDataType) 
	// {
	// case 0x04000101: Rx_Date(pRead);					return;//����
	// case 0x04000102: Rx_Time(pRead);					return;//ʱ��
	// case 0x0000ff00: Rx_DD(pRead,byRLen,52);			return;//����й�
	// case 0x0001ff00: //�����й�
	// case 0x00010000:
	// case 0x0002ff00:
	// case 0x00020000: Rx_DD_Ex(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;
	// case 0x0003ff00:
	// case 0x00030000:
	// case 0x0004ff00:
	// case 0x00040000:
	// case 0x0005ff00:
	// case 0x0006ff00:
	// case 0x0007ff00:
	// case 0x0008ff00: Rx_DD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;
	// //�����������
	// case 0x00150000:
	// case 0x00160000:
	// case 0x00170000:
	// case 0x00180000:
	// case 0x00290000:
	// case 0x002A0000:
	// case 0x002B0000:
	// case 0x002C0000:
	// case 0x003D0000:
	// case 0x003E0000:
	// case 0x003F0000:
	// case 0x00400000: Rx_DD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)%0x14+(HLBYTE(dwDataType)/0x14-1)*4+39)) ;return;
	
	// case 0x00850000: Rx_DD(pRead,byRLen,52);			return;
	// case 0x00860000: Rx_DD(pRead,byRLen,53);			return;

	// case 0x0201ff00: Rx_VCV(pRead,byRLen,0);			return;
	// case 0x02010100:
	// case 0x02010200:
	// case 0x02010300: Rx_VCV(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)-1));	return;
	// case 0x0202ff00: Rx_VCI(pRead,byRLen,3);			return;
	// case 0x02020100:
	// case 0x02020200:
	// case 0x02020300: Rx_VCI(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+2));	return;	//�������Ϊ3��4��5 
	// case 0x0203ff00: Rx_P07(pRead,byRLen,6);			return;
	// case 0x02030000: 
	// case 0x02030100:
	// case 0x02030200:
	// case 0x02030300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+6));	return;
	// case 0x0204ff00: Rx_P07(pRead,byRLen,10);			return;
	// case 0x02040000: 
	// case 0x02040100: 
	// case 0x02040200: 
	// case 0x02040300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+10));return;

	// case 0x0205FF00: Rx_P07(pRead,byRLen,25);			return;
	// case 0x02050000: 
	// case 0x02050100: 
	// case 0x02050200: 
	// case 0x02050300: Rx_P07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+25));return;
	// case 0x0206ff00: Rx_PE07(pRead,byRLen,14);			return;
	// case 0x02060000:
	// case 0x02060100:
	// case 0x02060200:
	// case 0x02060300: Rx_PE07(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)+14));return;
		
	// case 0x0207FF00: Rx_VCV(pRead,byRLen,19);			return;//MSF_VIANGLE
	// case 0x02800001: Rx_VCI(pRead,byRLen,18);			return;//MSF_I0
	// //case 0x02800002: Rx_FREQ(pRead,byRLen);				return;//MSF_FREQ
	// case 0x0208FF00: Rx_XB(pRead,byRLen,0);				return;//��ѹг��ʧ���
	// case 0x0209FF00: Rx_XB(pRead,byRLen,3);				return;//����г��ʧ���
	// case 0x04000501:
	// case 0x040005FF: Rx_MetStatus07(pRead,byRLen,0);	return;//MSF_STSMET
	// case 0x04000801: return;//MSF_STSREST

	// case 0x0101ff00: 
	// case 0x0102ff00: 
	// case 0x0103ff00: 
	// case 0x0104ff00: Rx_XL07(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));	return;//MSF_RRXL

	// //��
	// case 0x04000B01: 
	// case 0x04000B02: 
	// case 0x04000B03: Rx_AutoDay(pRead,byRLen,(BYTE)(LLBYTE(dwDataType)-1));	return;//�Զ���һ������
	// case 0x0000FF01: Rx_MonDD(pRead,byRLen,0);								return;//(����)��1����������й�
	// case 0x0001FF01: //���������й�
	// case 0x0002FF01: //���·����й�
	// case 0x0003FF01: //���������޹�
	// case 0x0004FF01: //���·����޹�
	// case 0x0005FF01: //һ�����޹�
	// case 0x0006FF01: //�������޹�	
	// case 0x0007FF01: //�������޹�
	// case 0x0008FF01: Rx_MonDD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));	return;//�������޹�
	// case 0x00150001:				
	// case 0x00160001:				
	// case 0x00170001:				
	// case 0x00180001:				
	// case 0x00290001:				
	// case 0x002A0001:				
	// case 0x002B0001:				
	// case 0x002C0001:				
	// case 0x003D0001:				
	// case 0x003E0001:				
	// case 0x003F0001:				
	// case 0x00400001: Rx_MonDD(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)%0x14+(HLBYTE(dwDataType)/0x14-1)*4+39)) ;return;

	// case 0x0101FF01: //���������й�����
	// case 0x0102FF01: //���·����й�����	
	// case 0x0103FF01: //���������޹�����
	// case 0x0104FF01: Rx_MonXL07(pRead,byRLen,(BYTE)(HLBYTE(dwDataType)*5-5));return;//���·����޹�����
	// case 0x0105FF01: return;//����һ�����޹�����
	// case 0x0106FF01: return;//���¶������޹�����	
	// case 0x0107FF01: return;//�����������޹�����
	// case 0x0108FF01: return;//�����������޹�����	
	// //����
	// case 0x06000001: Rx_Curve(pRead,byRLen);			return;//ָ��ʱ��һ����¼��

	// case 0x06100101: Rx_CurveEx(1,pRead,byRLen);			return;//A��ѹ
	// case 0x06100201: Rx_CurveEx(2,pRead,byRLen);			return;//A����
	// case 0x06100300: Rx_CurveEx(3,pRead,byRLen);			return;//�ܹ���
	// case 0x06100500: Rx_CurveEx(5,pRead,byRLen);			return;//�ܹ�������
	// case 0x06100601: Rx_CurveEx(6,pRead,byRLen);			return;//���е���
	// case 0x06100602: Rx_CurveEx(7,pRead,byRLen);			return;//���е���
	// }
	// switch((dwDataType&0xFFFFFF00)+1)
	// {//��
	// case 0x05060001: Rx_DayTM(pRead,byRLen);return;
	// case 0x05060101: 
	// case 0x05060201: 
	// case 0x05060301: 
	// case 0x05060401: 
	// case 0x05060501: 
	// case 0x05060601: 
	// case 0x05060701: 
	// case 0x05060801: Rx_DayDD(pRead,byRLen,(BYTE)(LHBYTE(dwDataType)*5-5),(dwDataType&0x0f));return;//MSF_DAY_R4
		
	// case 0x05060901: Rx_DayXL(pRead,byRLen,0,(dwDataType&0x0f));	return;//MSF_DAYXLAP
	// case 0x05060A01: Rx_DayXL(pRead,byRLen,5,(dwDataType&0x0f));	return;//MSF_DAYXLAR
	// //case 0x05080201: Rx_DayLeftPay(pRead, byRLen,0,(dwDataType&0x0f));return;
	// case 0x05061001: Rx_DayP07(pRead,byRLen,0,(dwDataType&0x0f));return;
	// }
}


// BOOL MDlt645_2007_TxMonitor(HPARA hPort)
// {
// TPort *pPort = (TPort*)hPort;
	
// 	if(pPort->GyRunInfo.wMPSeqNo==0xFFFF)
// 		return FALSE;
// 	if(pPort->GyRunInfo.byTxIDFlag == MSF_DAY_TM)
// 	{//��������ն���ʱ��û��Ӧ��ʱ���������ݲ��ٳ���
// 		if(pPort->GyRunInfo.wRxCount < pPort->GyRunInfo.wTxCount)
// 		{//�����ն���ʱ��û��Ӧ��
// 			if (GetReal2FrzFlg() == 0x5A)	//ʵʱת�ն���
// 				Fre2RealFlag();
// 			else
// 				ClearAllFlag();
// 			pPort->GyRunInfo.byReqDataValid	= 0xAA;	
// 			pPort->GyRunInfo.byTxIDFlag = 0xFF;
// 			return FALSE;	//���ڴ˴�return FALSE,���Զ�����1���ն���ʱ�䲻Ӧ��Ĳ��ٳ���
// 		}	
// 	}
// 	pPort->GyRunInfo.byTxIDFlag = 0xFF;
	
// 	//���ǰʱ��
// 	if(MeterTime_Tx())
// 		return TRUE;
// 	//����¶�������
// 	if(MonData_Tx())//ok
// 		return TRUE;
// 	//��Сʱ��������
// 	if(DayData_Tx())//OK
// 		return TRUE;
// 	if(Cure_Tx())
// 		return TRUE;
// 	//������Ƿ����
// 	if(DD_Txd())
// 		return TRUE;
// 	//���˲ʱ���Ƿ����
// 	if(SS_Txd())//OK
// 		return TRUE;
// 	//��������Ƿ����
// 	if(XL_Txd())//0k
// 		return TRUE;	
// 	return FALSE;
// }

#endif

