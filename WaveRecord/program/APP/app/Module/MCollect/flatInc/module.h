///////////////////////////////////////////////////////////////
//	�� �� �� :module.h
//	�ļ����� :�õ���Ϣ�ն�ģ������ѡ��
//	��    �� : jiangjy
//	����ʱ�� : 2015��9��2��
//	��Ŀ���� ��DF6203
//	����ϵͳ : 
//	��    ע : ��ѡ��ģ�鶨���sysdefs.h
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#ifndef __MODULE_H__
#define __MODULE_H__
#include "sysdefs.h"

//1��ģ�����Ͷ���

#define MT_MODULE	MT_DF6203_JZQ_II

//Ӳ������ HT_DF6203_A (����\������1) HT_DF6203_B(������2)  
#define MT_HARDTYPE		HT_DF6203_D


//2��ģ������Լ����
#define MD_BASEUSER		MGY_SDLT645_07//MD_GDW2013//

//ѡ��ESAM����(��ʵ��Ӳ�����)
#if(MD_BASEUSER	== MD_GDW2013)
#define MESAM_TYPE		MESAM_376
#elif(MD_BASEUSER	== MD_GDW698)
#define LT_NET			BIT4			//��������Ϊ����
#define MESAM_TYPE		MESAM_698
#endif

#ifndef LT_NET
#define LT_NET			0
#endif
#ifndef LT_GPRS
#define LT_GPRS			0
#endif	

#endif