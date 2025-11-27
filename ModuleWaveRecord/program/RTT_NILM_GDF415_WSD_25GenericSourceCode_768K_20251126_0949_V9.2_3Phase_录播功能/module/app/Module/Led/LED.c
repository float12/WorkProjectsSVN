//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	wangjunsheng
//创建日期	2020.10.28
//描述		电能质量模块LED驱动程序
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "LED.h"

/*********************************
 * 函数功能：设置LED指示灯状态
 * 参数：led--选择指示灯，LED_RED/LED_GREEN
 *       mode--指示灯状态，0-灭，1-亮，2-反转
 * 返回值：无
 * 备注：
 *********************************/
void api_SetUpLED(WORD led, BOOL mode)
{
    switch (led)
    {
    case LED_RED:
        if(mode==0)
        {
            LED_RED_CLOSE
        }
        else if(mode==1)
        {
            LED_RED_OPEN
        }
        else if(mode==2)
        {
            if(LED_RED_STATUS())
            {
                LED_RED_OPEN
            }
            else
            {
                LED_RED_CLOSE
            }
        }
        break;
    case LED_GREEN:
        if(mode==0)
        {
            LED_GREEN_CLOSE
        }
        else if(mode==1)
        {
            LED_GREEN_OPEN
        }
        else if(mode==2)
        {
            if(LED_GREEN_STATUS())
            {
                LED_GREEN_OPEN
            }
            else
            {
                LED_GREEN_CLOSE
            }
        }
        break;
    default:
        break;
    }
}
