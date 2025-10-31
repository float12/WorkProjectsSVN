//----------------------------------------------------------------------
//Copyright (C) 2003-20XX 烟台东方威思顿电气有限公司低压台区产品部
//创建人	王泉
//创建日期	
//描述		
//修改记录
//----------------------------------------------------------------------
#include "wsd_def.h"
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------

//-----------------------------------------------
//				本文件使用的变量，常量		
//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------

//-----------------------------------------------
//				函数定义
//-----------------------------------------------
//--------------------------------------------------
//功能描述:  
//         
//参数:      
//         
//返回值:    
//         
//备注:  
//--------------------------------------------------
BYTE CollectStart = 0;
void  Collection_Task( void *param )
{
    while (1)
    {
        // if (CollectStart)
        // {   
            // CollectStart = 0;
            // for (i = 0; i < 4; i++)
            // {
                // memcpy((BYTE*)&CollectValue.CollectData[i].P,(BYTE*)&RemoteValue[i].P,sizeof(TRemoteValue));
                // memcpy((BYTE*)&CollectValue.CollectData[i].Eng[0],(BYTE*)&EnergyDBase[i].PActive[0],sizeof(TEnergyDBase));
            // }
            // strcpy( CollectValue.Addr,LtuAddr);
            //时间戳
            // get_N176_time(&tTime);
            // bTime = tTime.Min%15;
            // tTime.Min = tTime.Min -bTime;
            // tTime.Sec = 0;
            // d_save = getmktimems(&tTime);
            // CollectValue.TimeMs = d_save;
            // nwy_ext_echo("\r\n %d",tTime.Hour);
            // nwy_ext_echo("\r\n %d",tTime.Min);
            // if(nwy_get_queue_spaceevent_cnt(CollectMessageQueue) != 0)
            // {
                // nwy_put_msg_que(CollectMessageQueue,&CollectValue,0);
            // }
            // nwy_sleep(20000);
        // }
        // else
        // {
        //    nwy_sleep(20000);
        // }
    }      
    
}