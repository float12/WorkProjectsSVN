
#include "fal_def.h"
#include "AllHead.h"

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_PART_HAS_TABLE_CFG

#define E2F_FLASH_DEV_NAME			"E2F_FL"
#define EXT_FLASH_DEV_NAME          "EXT_FL"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev scm_onchip_flash;
extern const struct fal_flash_dev ext_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
	&scm_onchip_flash,                                                      \
    &ext_flash,                                                      \
}
/* ====================== Partition Configuration ========================== */


#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
	//magic						//分块名	 //flash 名称			  //偏移量      //长度      //保留
// #define FAL_PART_TABLE                                                               \
// {                                                                                    \
// 	{FAL_PART_MAGIC_WORD,       "efrom",	 E2F_FLASH_DEV_NAME,	  0x00000000,   0x00010000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "app1",		 EXT_FLASH_DEV_NAME,      0x00000000,   0x00040000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "app2",		 EXT_FLASH_DEV_NAME,      0x00040000,   0x00040000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "minFrz",	 EXT_FLASH_DEV_NAME,      0x00737000,   0x000C8000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "dayFrz",	 EXT_FLASH_DEV_NAME,      0x006D3000,   0x00064000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "evtFrz",    EXT_FLASH_DEV_NAME,      0x0065B800,   0x0004E000, 0}, \
//     {FAL_PART_MAGIC_WORD,       "sm4",       EXT_FLASH_DEV_NAME,      0x007FF000,   0x00001000, 0}, \
// }
// #endif /* FAL_PART_HAS_TABLE_CFG */

#define FAL_PART_TABLE                                                               \
{                                                                                    \
		{FAL_PART_MAGIC_WORD,       "efrom",	 E2F_FLASH_DEV_NAME,	  0x00000000,   		0x00010000, 0}, \
		{FAL_PART_MAGIC_WORD,       "app1",		 EXT_FLASH_DEV_NAME,      0x00000000,   		CODEBUFFER_SIZE, 0}, \
		{FAL_PART_MAGIC_WORD,       "app2",		 EXT_FLASH_DEV_NAME,      CODEBUFFER_SIZE,   	CODEBUFFER_SIZE, 0}, \
		{FAL_PART_MAGIC_WORD,       "app3",		 EXT_FLASH_DEV_NAME,      CODEBUFFER_SIZE*2,   	CODEBUFFER_SIZE, 0}, \
		{FAL_PART_MAGIC_WORD,       "evtFrz",    EXT_FLASH_DEV_NAME,      0x00130000,   		0x000A0000, 0}, \
		{FAL_PART_MAGIC_WORD,       "dayFrz",	 EXT_FLASH_DEV_NAME,      0x001D0000,   		0x00030000, 0}, \
		{FAL_PART_MAGIC_WORD,       "minFrz",	 EXT_FLASH_DEV_NAME,      0x00200000,   		0x005A0000, 0}, \
		{FAL_PART_MAGIC_WORD,       "monFrz",    EXT_FLASH_DEV_NAME,      0x007A0000,   		0x00002000, 0}, \
		{FAL_PART_MAGIC_WORD,       "sm4",       EXT_FLASH_DEV_NAME,      0x007FF000,   		0x00001000, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// minFrz -----> 长度为0x005FF000 按当前配置可存储1535个点  0x005A0000 按当前配置10分钟间隔 可存储10天数据
// dayFrz -----> 长度为0x00060000 按当前配置可存储48个点    0x005A0000 按当前配置可存储
// evtFrz -----> 按当前事件的个数42个存储需要长度为0x54000  留有空闲可存储80种事件，每种事件10个
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* _FAL_CFG_H_ */
//0x000C8000
