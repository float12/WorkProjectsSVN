

#ifndef _FAL_H_
#define _FAL_H_

#include "fal_cfg.h"
#include "fal_def.h"

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_init
//	函数功能 : FAL (Flash Abstraction Layer) initialization.
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : void
///////////////////////////////////////////////////////////////
int fal_init(void);

///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_flash_device_find
//	函数功能 : find flash device by name
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_flash_dev      return != NULL: flash device ，NULL: not found
//	参数说明 : const char *name   flash device name
///////////////////////////////////////////////////////////////
const struct fal_flash_dev *fal_flash_device_find(const char *name);

///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_partition_find
//	函数功能 : find the partition by name
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_partition      return != NULL: partition,  NULL: not found
//	参数说明 : const char *name   partition name
///////////////////////////////////////////////////////////////
const struct fal_partition *fal_partition_find(const char *name);

///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_get_partition_table
//	函数功能 : get the partition table
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_partition	return partition table
//	参数说明 : size_t *len		return the partition table length
///////////////////////////////////////////////////////////////
const struct fal_partition *fal_get_partition_table(size_t *len);

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_set_partition_table_temp
//	函数功能 : set partition table temporarily
//	处理过程 : This setting will modify the partition table temporarily, the setting will be lost after restart.
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : void
//	参数说明 : struct fal_partition *table,		table partition table
//				 size_t len						len partition table length
///////////////////////////////////////////////////////////////
void fal_set_partition_table_temp(struct fal_partition *table, size_t len);

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_read
//	函数功能 : read data from partition
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int	return >= 0: successful read data size, -1: error
//	参数说明 : const struct fal_partition *part,		part partition
//				 uint32_t addr,					addr relative address for partition
//				 uint8_t *buf,					buf read buffer
//				 size_t size					size read size
///////////////////////////////////////////////////////////////
int fal_partition_read(const struct fal_partition *part, uint32_t addr, uint8_t *buf, size_t size);


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_write
//	函数功能 : write data to partition
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int		return >= 0: successful write data size, -1: error
//	参数说明 : const struct fal_partition *part,		part partition
//				 uint32_t addr,							addr relative address for partition
//				 const uint8_t *buf,					buf write buffer
//				 size_t size							size write size
///////////////////////////////////////////////////////////////
int fal_partition_write(const struct fal_partition *part, uint32_t addr, const uint8_t *buf, size_t size);


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_erase
//	函数功能 : erase partition data
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int				return >= 0: successful erased data size,  -1: error
//	参数说明 : const struct fal_partition *part,		part partition
//				 uint32_t addr,							addr relative address for partition
//				 size_t size							size erase size
///////////////////////////////////////////////////////////////
int fal_partition_erase(const struct fal_partition *part, uint32_t addr, size_t size);

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_erase_all
//	函数功能 : erase partition all data
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int			return >= 0: successful erased data size,	-1: error
//	参数说明 : const struct fal_partition *part		 part partition
///////////////////////////////////////////////////////////////
int fal_partition_erase_all(const struct fal_partition *part);

uint32_t  fal_partition_start(const char *name);
uint32_t  fal_partition_end(const char *name);

#endif /* _FAL_H_ */
