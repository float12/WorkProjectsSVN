///////////////////////////////////////////////////////////////
//	�� �� �� :DF6203Cfg.h
//	�ļ����� : DF6203 ����
///////////////////////////////////////////////////////////////
#ifndef _DF6203CFG_H_
#define _DF6203CFG_H_
#include "__define.h"
#include "module.h"

//����Ӳ������
#include "ABC_DF6203.h"

#define	MAX_VPORT_NUM		5		//(����ܳ���32)

#define MAX_DWNPORT_NUM		5	    //485���ز���can������

#define	MAX_MP_NUM			(32+1)	

	
//�ڴ�������������
#define	MAX_698_TASK_NUM	16		//(4096/sizeof(TOad60130200))

#define MAX_TASK_TIMEREG_NUM	4					

#ifndef MD_MDLT645_97
#define MD_MDLT645_97		YES
#endif
#ifndef MD_MDLT645_07
#define MD_MDLT645_07		YES
#endif	


#endif//����
