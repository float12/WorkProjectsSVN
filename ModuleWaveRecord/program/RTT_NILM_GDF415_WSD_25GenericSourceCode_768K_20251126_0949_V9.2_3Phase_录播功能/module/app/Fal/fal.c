
#include "fal.h"

static uint8_t init_ok = 0;

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_init
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : void
///////////////////////////////////////////////////////////////
int fal_init(void)
{
    extern int fal_flash_init(void);
    extern int fal_partition_init(void);

    int result;

    /* initialize all flash device on FAL flash table */
    result = fal_flash_init();

    if (result < 0) {
        goto __exit;
    }

    /* initialize all flash partition on FAL partition table */
    result = fal_partition_init();

__exit:

    if ((result > 0) && (!init_ok))
    {
        init_ok = 1;
    }
    else if(result <= 0)
    {
        init_ok = 0;
    }

    return result;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_init_check
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : void
///////////////////////////////////////////////////////////////
int fal_init_check(void)
{
    return init_ok;
}
