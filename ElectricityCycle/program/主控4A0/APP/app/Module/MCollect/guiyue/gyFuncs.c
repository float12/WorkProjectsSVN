
#include "appcfg.h"
#include "cgy.h"
#include <string.h>

extern void Save698TaskData(TPort *pPort,WORD wMPNo);
extern void fresh_task_status(TPort *pPort,BYTE byNo,WORD wNo,BOOL bOK);

// void MPRxDeal(TPort *pPort,WORD wMPNo)
// {
// 	//一块表的数据已召唤结束						
// 	if(pPort->GyRunInfo.wTxCount)
// 	{//有发送		
// 		if(pPort->GyRunInfo.wRxCount)
// 		{//有接收			
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

