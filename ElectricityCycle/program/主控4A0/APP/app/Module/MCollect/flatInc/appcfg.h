
#ifndef _APPCFGS_H_
#define _APPCFGS_H_

#include "configs/DF6203Cfg.h"
#include "paradef.h"
#include "appbsp.h"

//��׼�ڲ���ʱ����ʶ(ʹ�� OS_SetTimer���� )
#define MTID_DRIVER			BIT0	//����
#define	MTID_USER			BIT1				
//ϵͳ��PV��������
#define MPV_PARA_TK			1		//�ն���ʱ�л������ڴ棨����
#define MPV_PARA_MPB		2		//�ն���ʱ�л������ڴ棨�����������
#define MPV_PARA_FK			3		//�ն���ʱ�л������ڴ棨���ز�����
#define MPV_PARA_F38		4		//������ɼ���ʶF3839
#define MPV_PARA_MP			5		//�����㵵������2K����
#define MPV_PARA_SYS		6		//ϵͳ����������ͬ��д
#define MPV_HIS_BUF			7		//��ʷ���ݻ�����
#define MPV_MPREAL			8		//������ʵʱ����
#define MPV_MPCURV			9		//��������������
#define MPV_FRZ				10		//�����㶳������
#define MPV_EVENT			11		//�ն��¼�����
#define MPV_EVENTUP			12		//�ն������ϱ��¼�����
#define MPV_PARA_ESAM		13		//ESAM����������ͬ��д
#define MPV_PARA_USER		14		//�û��������������ͬ��д
#define MPV_698_MEM			15		//698�����ڴ�

//ͨ�ù���
#include "usros.h"
#include "port.h"
#include "usrtime.h"
#include "usrfun.h"
#include <string.h>

//ר��Ӧ��ͷ�ļ�
#include "mpruninfo.h"
#include "sysCfg.h"
#include "dbase.h"
#include "sysdata.h"
#include "mp.h"
#include "cgy.h"
#include "__def.h"


#endif



