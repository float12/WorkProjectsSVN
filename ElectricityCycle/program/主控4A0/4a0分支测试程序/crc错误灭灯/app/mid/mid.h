/**
 * �м��ͷ�ļ�
 * @���� jwh
 * @��Ȩ 
 * @�汾 1.0
 * @ʱ�� 2023��03��08��
**/
#ifdef __cplusplus
 extern "C" {
#endif

#ifndef __Mid_H__
#define __Mid_H__
#include <string.h>

//�������Ͷ���
#define Mid_BYTE	unsigned char
#define Mid_WORD    unsigned short
#define Mid_SWORD   short
#define Mid_DWORD	unsigned long
#define Mid_BOOL	unsigned char
#define	Mid_QWORD   unsigned long long
#define Mid_SQWORD	long long

#define True		1
#define False		(!True)
//�м��֧�ֵ������չ������չ���ܣ�
#define Expand_functin_Max_Num	3

//��һ�м�����ע��oi��������չ���ܣ�
#define Expand_Logon_OI_Max_Num	30

//��һ�м��Ȩ��������󳤶ȣ���չ���ܣ�
#define Expand_DataInfo_Max_Len	500

//�м��֧�ֵ����oi��������չ���ܣ�
#define Mid_OI_Data_Num	(Expand_Logon_OI_Max_Num+Expand_functin_Max_Num)

//���Ķ�ʱ�������
#define Max_Time_Task_Num	(Max_Expand_Time_Task_Num*Expand_functin_Max_Num)

//��һ��չ�������ʱ�����ܳ���3��
#define Max_Expand_Time_Task_Num	5


typedef enum
{
	eMidLoopMode = 0,	//ѭ������
   	eMidSecMode, 		//������
   	eMidMinMode, 		//��������
}eMidTimeTaskType;

//�м����ʱ����ṹ��
typedef struct TMidTimeTask_t   
{
	eMidTimeTaskType  TimeTaskType;	//��ʱ��������
	void (*MidTimeTask)();			//��ʱ������
}TMidTimeTask;

typedef struct TMidAndExpandTimeTask_t   
{
	Mid_BYTE Num;								//��ʱ�������
	TMidTimeTask TimeTask[Max_Time_Task_Num];	//��ʱ�������б�
}TMidAndExpandTimeTask;

typedef struct TOIData_t   	
{
	Mid_WORD 	OI;				//OI
	Mid_BYTE	OIClass;		//OI����
	Mid_BYTE 	OIAuth;			//OIȨ��
}TOIData;

typedef struct TMidOIData_t   	
{
	Mid_WORD 	OINum;						//OI����
	TOIData 	OIList[Mid_OI_Data_Num];	//OI�����б�
}TMidOIData;

typedef struct TMidInfoData_t   	
{
	TMidOIData OIData;								//֧�ֵĻ�ȡOI����
}TMidInfoData;

typedef struct TMidParaData_t   	
{
	Mid_BYTE ControlByte;				//�����ֽ�
	Mid_BYTE AddressLen;				//��ַ����
	Mid_BYTE ClientAddress; 			//�ͻ�����ַ
	Mid_BYTE AddressType;				//��ַ����
	Mid_BYTE* Address;					//��ַ(��󳤶�10�ֽ�)
	Mid_WORD OI;						//��չӦ�õ�ΨһOI������͸��ת��ʱʹ�ã�����������
	Mid_WORD ProxyFlag;					//�����־ 0x00 �Ǵ����� 0x55 ������
	Mid_BYTE* pAPDU; 					//Ӧ�ò���ʼָ��
	Mid_WORD pAPDULen;					//Ӧ�ò����ݳ���
	Mid_WORD  APDUMAXLen; 				//Ӧ�ò�������󳤶�
	void* pReserve; 					//Ԥ��ָ��
}TMidParaData;

//ͨ�����ݻ�ȡ��������-��������չ���ܶ�Ҫʵ�ִ˺���
typedef Mid_WORD (*CommonDataFunction)( TMidParaData* ExpandParaData );

typedef struct TMidFunction_t   	
{
	//�м���ӿں���-��ʱ����
	TMidAndExpandTimeTask MidAndExpandTimeTask;
	//�м���ӿں���-��ȡָ��OAD����
	CommonDataFunction MidDataFunction;
}TMidFunction;

typedef struct TMeterFun_t//������ṩ�Ļص�����   	
{
	//�������ʵ�ִ˺����ķ�ʽΪMid_WORD *MeterGetDataFun( eDataType DataType, Mid_BYTE *Type, Mid_BYTE *pOAD, Mid_WORD BufLen, Mid_BYTE *Buf )
	CommonDataFunction MeterDataFun;
}TMeterFun;

//MeterFun �����ص������ṹ��
//MidOIData �м�����ص����ݽṹ��
//MidFunction �м�����صĺ����ṹ��
typedef Mid_BOOL (*MidEnterFunction)( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction *MidFunction);

//--------------------------------------------��չ������ض���-----------------------------------------------------//

typedef struct TExpandData_t   	
{
	TMidOIData  ExpandOIData;
	CommonDataFunction Function;
}TExpandData;

typedef struct TExpandTimeTask_t   
{
	Mid_BYTE Num;												//��ʱ�������
	TMidTimeTask TimeTask[Max_Expand_Time_Task_Num];			//��ʱ�����б�
}TExpandTimeTask;

typedef struct TExpandFunction_t   	
{
	Mid_BYTE ExpandNum;											//��չ���ܸ���
	
	TMidOIData ExpandOIDataList[Expand_functin_Max_Num];		//��չ���ܼ��������б�

	TExpandTimeTask ExpandTimeTask[Expand_functin_Max_Num];		//��չ���ܶ�ʱ�����б�
	//�м���ӿں���-��ȡָ��OAD��
	CommonDataFunction ExpandFunction[Expand_functin_Max_Num];	//��չ���ܺ����б�
}TExpandFunction;

typedef struct TExpandOneFunction_t   	
{
	TMidOIData ExpandOIData;

	TExpandTimeTask ExpandTimeTask;
	//�м���ӿں���-��ȡָ��OAD����
	//DataType 
	//pOADΪ4�ֽ�698OAD
	//TypeΪ��ȡ������������� 
	//bufΪԴ����
	//return �������ݳ���
	CommonDataFunction ExpandOnceFunction;
}TExpandOneFunction;

//��չ������ں�������
typedef void (*ExpandOneEnterFunction)( CommonDataFunction MibFun, TExpandOneFunction *ExpandFunction);

//--------------------------------------------�м����ض���------------------------------------------------------//

extern Mid_BOOL MidFunction( TMeterFun MeterFun,TMidInfoData *MidInfoData, TMidFunction * MidFunction);

//--------------------------------------------��չ������ض���-----------------------------------------------------//

extern const MidEnterFunction MidEnterFunctionConst;
extern void ExpandFunction( CommonDataFunction MidFun, TExpandOneFunction *ExpandOneFunction);

//--------------------------------------------�������ӿ�-------------------------------------------------------//

//�м����ں����궨����ô˺궨������м����ں���
#define	LoadMidEnterFunction( MeterFun, MidInfoData, MidFunction) ((MidEnterFunction)MidEnterFunctionConst)( MeterFun, MidInfoData, MidFunction)

#endif

#ifdef __cplusplus
}
#endif
