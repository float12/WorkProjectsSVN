#ifndef __MID_PORT_H__
#define __MID_PORT_H__
#include "_defA.h"
WORD api_RequestBaseMeterA(BYTE *APDU, BYTE APDULen, WORD BufLen);
WORD api_GetNorm(OAD s_OAD,BYTE *pAPDU);//��ȡ����
void dataToBuff();//��������
void ModuleALoopTask(void);//ѭ������
void ModuleASecTask(void);//������
void ModuleAMinTask(void);//��������
void ModuleAHourTask(void);//Сʱ����
void api_Electric(BYTE *pAPDU);//��ȡ������
void ModuleADayTask(void);//������
void api_GetTime(BYTE *pAPDU);//��չģ���ȡ�����ʱ��
void api_Electricity(BYTE *pAPDU);//��չģ���ȡ�����ʱ����
void api_ApparentPower(BYTE *pAPDU);//��չģ���ȡ��������ڹ���
void api_ReactivePower(BYTE *pAPDU);//��չģ���ȡ������޹�����
void api_ActivePower(BYTE *pAPDU);//��չģ���ȡ������й�����
void api_Voltage(BYTE *pAPDU);//��չģ���ȡ������ѹ
void api_Record(BYTE *pAPDU);//��չģ����µ���
void api_Record2(BYTE *pAPDU);//���µڶ��׵���
void api_PowerFactor(BYTE *pAPDU);//��������
void api_SeriaNet(BYTE *pAPDU , OAD s_OAD ,BYTE a);//͸������
void api_OprationIncident(BYTE *pAPDU , SWORD OI , BYTE Happen,BYTE *DataBuff , BYTE DataLength );//��չģ�鴥�������һ���¼�
void api_SaveParameter(BYTE *pAPDU , BYTE *ExModelNameBuff ,  BYTE ExModelNameLength ,  SWDORD  Addr , SWORD  DataLength , BYTE *DataBuff);//��չӦ���Զ����������
void api_GetAdd(BYTE *APDU, BYTE APDULen, WORD BufLen);//��ȡ���ַ
#endif
