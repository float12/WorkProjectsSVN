//----------------------------------------------------------------------
//Copyright (C) 2003-20XX ��̨������˼�ٵ������޹�˾�������з���
//������
//��������
//����		DLT645ͷ�ļ�
//�޸ļ�¼
//----------------------------------------------------------------------
#ifndef __DLT645_H
#define __DLT645_H

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
#define TCP_REQ_NET 		0x040013D1		//TCP IP��ַ�Ͷ˿ں�
#define TCP_REQ_USER_PD 	0x040013D6  	//TCP �û���������
#define MQTT_REQ_NET 		0x040013E1		//MQTT IP��ַ�Ͷ˿ں�
#define MQTT_REQ_USER_PD 	0x040013E6 		//MQTT �û���������
#define DAY_TIME 			0x0400010C		//���ں�ʱ��
#define USED_CHANNEL_NUM 	0x04A00100 		//������ʹ�û�·��

#if( EVENT_REPORT ==  YES)
#define WARNING_REPORT		0x04A10001		//�����澯״̬��
#endif

#define DLT645_FACTORY_ID (0x04dfdfdb)
#define LLBYTE(w) ((BYTE)(w))				 //ȡһ��DWORD������ֽ�
#define LHBYTE(w) ((BYTE)((WORD)(w) >> 8))   //ȡһ��DWORD�Ĵε��ֽ�
#define HLBYTE(w) ((BYTE)((DWORD)(w) >> 16)) //ȡһ��DWORD�Ĵθ��ֽ�
#define HHBYTE(w) ((BYTE)((DWORD)(w) >> 24)) //ȡһ��DWORD������ֽ�

#define MAX_READMETER (CycleReadMeterNum + eBIT_TOTAL) //��󳭱���,������bitλ��
//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

typedef struct
{
	BYTE *pMessageAddr;		//���Ĵ����ַ
	WORD wSerialDataLen;	//�������ݳ���
	BYTE bIDLen;			//���ݱ�ʶ����
	BYTE byReceiveControl;  //���ձ��Ŀ�����
	BYTE *pDataAddr;		//��������ַ
	WORD wDataLen;			//����������
	BYTE bSEQ;				//����֡��� ������������֡��Ӱ�� �ں���֡����֡��֡�����Ϊ0
	BYTE byOperatorCode[4]; //�����ߴ���
	BYTE bLevel;			//����ȼ�
	BYTE bPassword[3];		//�·�������
	WORD wErrType;			//��������
	DWORD dwSpecialFlag;	//�����־������BIT0��1������ģʽ
} T_DLT645;

//645�ṹ
typedef struct TDlt645Head_t
{
	BYTE Sync1;
	BYTE ID[6];
	BYTE Sync2;
	BYTE Control;
	BYTE Length;
} TDlt645Head;

typedef struct
{
	BYTE byS68;		//��ʼ68�ַ�
	BYTE MAddr[6];  //��ַ
	BYTE byE68;		//����68�ַ�
	BYTE byCtrl;	//������
	BYTE byDataLen; //�����򳤶�
	DWORD dwDI;
} T645FmRx;

typedef struct TOtherPara_t
{
	DWORD dwCTPrim; // CTһ��
	DWORD dwPTPrim; // PTһ��
	BYTE bPTSecond; // PT����
	BYTE bCTSecond; // CT����
} TOtherPara;

typedef struct TSerial_t
{

	//�������壬��RAM�б�����ż��ַ
	BYTE ProBuf[UART_BUFF_MAX_LEN];

	WORD ProStepDlt645;
	//Dlt645��Լ������Serial[].ProBuf�еĿ�ʼλ��
	WORD BeginPosDlt645;

	//645�������������ݳ���
	WORD Dlt645DataLen;

	//DLT698_45��Լ���չ��̼���
	WORD ProStepDlt698_45;
	//DLT698_45��Լ������Serial[].ProBuf�еĿ�ʼλ��
	WORD BeginPosDlt698_45;
	WORD wLen;
	BYTE Addr_Len; //ͨ�ű����з�������ַSA���ȣ���ַ����

	WORD RXWPoint;
	WORD RXRPoint;

	WORD TXPoint;
	WORD SendLength; //��Ҫ���͵����ݳ���

	//���ڼ���ʱ��
	BYTE WatchSciTimer;

	//��485ͨѶ�ļ���ʱ�䴦��ȫ�����ô�ѭ��������ʽ
	//���ܳ�ʱʱ�䣬���յ����һ���ֽں󣬵ȴ���ʱ��û���µ������յ��������ǽ��ܵ��������ݣ���Ҫ��ʼ��ָ�롣
	WORD RxOverCount;

	BYTE byLastRXDCon; //�ϴ�ͨ�ŵ��Ӧ�������

	//bit0 ��Ӧͨ�����޸Ĳ����ʵĲ���
	BYTE OperationFlag;

} TSerial;

// PT CT
typedef enum
{
	eType_PT = 0, //pt
	eType_CT,	 //ct
	eType_PT_CT,  //pt_ct
} eTYPE_PT_CT_DATA;

//-----------------------------------------------
//				��������
//-----------------------------------------------

extern char bAddr[6]; //���ͨ�ŵ�ַ
extern char reverAddr[6];
extern DWORD PowerOnReadMeterFlag; //�ϵ糭��ɹ���bitλ Ŀǰ�߸�
extern char LtuAddr[30];
extern char MeterVersion[35];
extern TSerial Serial;
extern BYTE bHisReadMeter[MAX_SAMPLE_CHIP]; //�ϵ��ʼ����FF
extern QWORD qwReadMeterFlag[MAX_SAMPLE_CHIP];
extern BYTE bReadMeterRetry;
extern BYTE bUsedChannelNum; //��ʹ��ͨ������ �ϵ���̽��
//-----------------------------------------------
// 				��������
//-----------------------------------------------
//--------------------------------------------------
//��������:  645 ͨ�ù�Լ��ȡ
//
//����:      dwID ���ݱ�ʶ
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_Read(DWORD dwID);
//--------------------------------------------------
//��������:  645 ͨ�ù�Լ����
//
//����:      dwDI ����
//           bLen
//           pBuf ������������
//����ֵ:
//
//��ע:
//--------------------------------------------------
void Dlt645_Tx_Write(DWORD dwID, BYTE bDataLen, BYTE *pBuf);
//--------------------------------------------------
//��������: ����������������
//
//����:
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
void api_ParaPowerOnTask(void);
//-----------------------------------------------
//��������: ���㵥�ֽ��ۼӺͣ�����һ���ֽڵĲ�������
//
//����:
//	ptr[in]:	���뻺��
//
//	Length[in]:	���峤��
//
//����ֵ:	���ֽ��ۼӺ�
//
//��ע:
//-----------------------------------------------
BYTE lib_CheckSum(BYTE *ptr, WORD Length);
//-----------------------------------------------
//��������:	PT CT����
//
//����:	bType[in]: ��ȡ����
//
//����ֵ:	����
//
//��ע:
//-----------------------------------------------
double api_GetPtandCT(BYTE bType);
//---------------------------------------------------------------
//��������: uartͨ��645��ʽ�ж�
//
//����: 	portType[in] - ͨѶ�˿�
//
//����ֵ:  FALSE: ����û������
//		  TRUE : ������������InitPoint()
//
//��ע:
//---------------------------------------------------------------
WORD DoReceProc_Dlt645_UART(BYTE ReceByte, TSerial *p);
//--------------------------------------------------
//��������:  ��������
//
//����:      bBtep �����־bitλ
//
//����ֵ:
//
//��ע:
//--------------------------------------------------
// void api_ReadMeterTask645(BYTE bStep, BYTE *bAddr);
#if (GPRS_POSITION == YES)
//----------------------------------------
//��������:  ��չ��Լ�������õ���λ��
//
//����:      bMode  1���õ���λ�ã�0 ���
//
//����ֵ:
//
//��ע:
//----------------------------------------
BOOL api_CheckorSetPosition(BYTE bMode);
#endif
void Adrr_Txd(void);
void Dlt645_Tx_Read_Version(void);
void  OpenSystemProgram(void);
#endif //#ifndef __DLT645_H
