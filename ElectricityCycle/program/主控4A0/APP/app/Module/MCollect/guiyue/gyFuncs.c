
#include "appcfg.h"
#include "cgy.h"
#include <string.h>

extern void Save698TaskData(TPort *pPort,WORD wMPNo);
extern void fresh_task_status(TPort *pPort,BYTE byNo,WORD wNo,BOOL bOK);

// void MPRxDeal(TPort *pPort,WORD wMPNo)
// {
// 	//һ�����������ٻ�����						
// 	if(pPort->GyRunInfo.wTxCount)
// 	{//�з���		
// 		if(pPort->GyRunInfo.wRxCount)
// 		{//�н���			
// 			if( pPort->GyRunInfo.byReqDataValid==1)
// 			{
// 				Save698TaskData(pPort,wMPNo);

// 				fresh_task_status(pPort,get_698_task_no(),wMPNo,TRUE);
// 			}
// 			else
// 				fresh_task_status(pPort,get_698_task_no(),wMPNo,FALSE);
// 		}
// 		else 
// 		{
// 			fresh_task_status(pPort,get_698_task_no(),wMPNo,FALSE);
// 		}
// 	}
// 	pPort->GyRunInfo.byReqDataValid	= 0;
// 	pPort->GyRunInfo.wTxCount		= 0; 
// }

HPARA GetMGyApp(void)
{
TPort *pPort=(TPort *)GethPort();
	
	return pPort->GyRunInfo.pApp;
}

