/*
 *  main_task.h
 *
 *
 * */
 
#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

//6个通道测量值缓存Buffer，分别对应IA,VA,IB,VB,IC,VC IN;
typedef struct RecDataBuf{


	SDWORD VA;
	SDWORD VB;
	SDWORD VC;

	SDWORD IA;
	SDWORD IB;
	SDWORD IC;


}RECDATABUF;

typedef struct TimeOut{
	DWORD SampleTimeOut;		//采样时间，在进行采样中断时清零，判断采样是否正常，如果超过时间阀值，说明采样异常
	DWORD ComTimeOut;			//通讯时间，再进行通讯时清零，判断通讯是否正常，如果超过时间阀值，说明通讯异常

}TIME_OUT;

//采样AD接收buffer链表定义
typedef struct SampleDataBuf{

	RECDATABUF	  data;

} SAMPLEDATABUF;


void InitApp(void);/*初始化应用程序参数*/
#endif
