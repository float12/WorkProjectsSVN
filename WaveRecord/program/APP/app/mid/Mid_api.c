//----------------------------------------------------------------------
//Copyright (C) 2003-2023 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	    
//��������	    2023.04.04
//����         ��������м������չ���ܵ�ַC�ļ�
//�޸ļ�¼
//---------------------------------------------------------------------- 
#ifdef __cplusplus
 extern "C" {
#endif

#include "mid.h"

//--------------------------------------------�м����ض���-----------------------------------------------------//

const MidEnterFunction MidEnterFunctionConst  = MidFunction;

//--------------------------------------------��չ������ض���-----------------------------------------------------//

const ExpandOneEnterFunction ExpandFunctionList[ Expand_functin_Max_Num + 1 ] =
{
	(ExpandOneEnterFunction)0,					//��չ���ܸ���
	//(ExpandOneEnterFunction)ExpandFunction,		//��չ����
	//ExpandFunction2,							//��չ����
};

#ifdef __cplusplus
}
#endif
































































