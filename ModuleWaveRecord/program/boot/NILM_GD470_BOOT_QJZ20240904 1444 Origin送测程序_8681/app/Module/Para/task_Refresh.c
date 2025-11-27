#include "AllHead.h"
#include "task_Refresh.h"

/*当发生计量模组修改参数时,需刷新一部分文件内部的变量等*/
void WPARA_Refresh()
{

    RefreshFlucPara();
    RefreshEvtPara();
    RefreshHarmPara();
    RefreshMmxuPara();
    RefreshFreqPara();
    RefreshFlickPara();
}



