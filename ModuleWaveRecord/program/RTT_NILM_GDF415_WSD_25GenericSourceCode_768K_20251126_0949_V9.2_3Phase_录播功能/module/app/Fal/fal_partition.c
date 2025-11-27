
#include "fal.h"
#include <string.h>
#include <stdlib.h>

/* partition magic word */
#define FAL_PART_MAGIC_WORD         0x45503130
#define FAL_PART_MAGIC_WORD_H       0x4550L
#define FAL_PART_MAGIC_WORD_L       0x3130L
#define FAL_PART_MAGIC_WROD         0x45503130

/**
 * FAL partition table config has defined on 'fal_cfg.h'.
 * When this option is disable, it will auto find the partition table on a specified location in flash partition.
 */
#ifdef FAL_PART_HAS_TABLE_CFG

/* check partition table definition */
#if !defined(FAL_PART_TABLE)
#error "You must defined FAL_PART_TABLE on 'fal_cfg.h'"
#endif

#ifdef __CC_ARM                        /* ARM Compiler */
    #define SECTION(x)                 __attribute__((section(x)))
    #define USED                       __attribute__((used))
#elif defined (__IAR_SYSTEMS_ICC__)    /* for IAR Compiler */
    #define SECTION(x)                 @ x
    #define USED                       __root
#elif defined (__GNUC__)               /* GNU GCC Compiler */
    #define SECTION(x)                 __attribute__((section(x)))
    #define USED                       __attribute__((used))
#else
	#define SECTION(x)                 
    #define USED     
//    #error not supported tool chain
#endif /* __CC_ARM */

static const struct fal_partition partition_table_def[] SECTION("FalPartTable") = FAL_PART_TABLE;
static const struct fal_partition *partition_table = NULL;

#else /* FAL_PART_HAS_TABLE_CFG */

#if !defined(FAL_PART_TABLE_FLASH_DEV_NAME)
#error "You must defined FAL_PART_TABLE_FLASH_DEV_NAME on 'fal_cfg.h'"
#endif

/* check partition table end offset address definition */
#if !defined(FAL_PART_TABLE_END_OFFSET)
#error "You must defined FAL_PART_TABLE_END_OFFSET on 'fal_cfg.h'"
#endif

static struct fal_partition *partition_table = NULL;
#endif /* FAL_PART_HAS_TABLE_CFG */

static uint8_t init_ok = 0;
static size_t partition_table_len = 0;


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_init
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : void
///////////////////////////////////////////////////////////////
int fal_partition_init(void)
{
    size_t i;
    const struct fal_flash_dev *flash_dev = NULL;

    if (init_ok)
    {
        return partition_table_len;
    }

#ifdef FAL_PART_HAS_TABLE_CFG
    partition_table = &partition_table_def[0];
    partition_table_len = sizeof(partition_table_def) / sizeof(partition_table_def[0]);
#else
    /* load partition table from the end address FAL_PART_TABLE_END_OFFSET, error return 0 */
    long part_table_offset = FAL_PART_TABLE_END_OFFSET;
    size_t table_num = 0, table_item_size = 0;
    uint8_t part_table_find_ok = 0;
    uint32_t read_magic_word;
    fal_partition_t new_part = NULL;

    flash_dev = fal_flash_device_find(FAL_PART_TABLE_FLASH_DEV_NAME);
    if (flash_dev == NULL)
    {
         goto _exit;
    }

    /* check partition table offset address */
    if (part_table_offset < 0 || part_table_offset >= (long) flash_dev->len)
    {
        goto _exit;
    }

    table_item_size = sizeof(struct fal_partition);
    new_part = (fal_partition_t)FAL_MALLOC(table_item_size);
    if (new_part == NULL)
    {
        goto _exit;
    }

    /* find partition table location */
    {
        uint8_t read_buf[64];

        part_table_offset -= sizeof(read_buf);
        while (part_table_offset >= 0)
        {
            if (flash_dev->ops.read(part_table_offset, read_buf, sizeof(read_buf)) > 0)
            {
                /* find magic word in read buf */
                for (i = 0; i < sizeof(read_buf) - sizeof(read_magic_word) + 1; i++)
                {
                    read_magic_word = read_buf[0 + i] + (read_buf[1 + i] << 8) + (read_buf[2 + i] << 16) + (read_buf[3 + i] << 24);
                    if (read_magic_word == ((FAL_PART_MAGIC_WORD_H << 16) + FAL_PART_MAGIC_WORD_L))
                    {
                        part_table_find_ok = 1;
                        part_table_offset += i;
  
                        break;
                    }
                }
            }
            else
            {
                /* read failed */
                break;
            }

            if (part_table_find_ok)
            {
                break;
            }
            else
            {
                /* calculate next read buf position */
                if (part_table_offset >= (long)sizeof(read_buf))
                {
                    part_table_offset -= sizeof(read_buf);
                    part_table_offset += (sizeof(read_magic_word) - 1);
                }
                else if (part_table_offset != 0)
                {
                    part_table_offset = 0;
                }
                else
                {
                    /* find failed */
                    break;
                }
            }
        }
    }

    /* load partition table */
    while (part_table_find_ok)
    {
        memset(new_part, 0x00, table_num);
        if (flash_dev->ops.read(part_table_offset - table_item_size * (table_num), (uint8_t *) new_part,
                table_item_size) < 0)
        {
            table_num = 0;
            break;
        }

        if (new_part->magic_word != ((FAL_PART_MAGIC_WORD_H << 16) + FAL_PART_MAGIC_WORD_L))
        {
            break;
        }

        partition_table = (fal_partition_t) FAL_REALLOC(partition_table, table_item_size * (table_num + 1));
        if (partition_table == NULL)
        {
            table_num = 0;
            break;
        }

        memcpy(partition_table + table_num, new_part, table_item_size);

        table_num++;
    };

    if (table_num == 0)
    {
          goto _exit;
    }
    else
    {
        partition_table_len = table_num;
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

    /* check the partition table device exists */

    for (i = 0; i < partition_table_len; i++)
    {
        flash_dev = fal_flash_device_find(partition_table[i].flash_name);
        if (flash_dev == NULL)
        {
            continue;
        }

        if (partition_table[i].offset >= (long)flash_dev->len)
        {
            partition_table_len = 0;
            goto _exit;
        }
    }

    init_ok = 1;

_exit:


#ifndef FAL_PART_HAS_TABLE_CFG
    if (new_part)
    {
        FAL_FREE(new_part);
    }
#endif /* !FAL_PART_HAS_TABLE_CFG */

    return partition_table_len;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_partition_find
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_partition
//	参数说明 : const char *name
///////////////////////////////////////////////////////////////
const struct fal_partition *fal_partition_find(const char *name)
{
	size_t i;

    for (i = 0; i < partition_table_len; i++)
    {
        if (!strcmp(name, partition_table[i].name))
        {
            return &partition_table[i];
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_get_partition_table
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_partition
//	参数说明 : size_t *len
///////////////////////////////////////////////////////////////
const struct fal_partition *fal_get_partition_table(size_t *len)
{
    *len = partition_table_len;

    return partition_table;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_set_partition_table_temp
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : void
//	参数说明 : struct fal_partition *table,
//				 size_t len
///////////////////////////////////////////////////////////////
void fal_set_partition_table_temp(struct fal_partition *table, size_t len)
{
    partition_table_len = len;
    partition_table = table;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_read
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const struct fal_partition *part,
//				 uint32_t addr,
//				 uint8_t *buf,
//				 size_t size
///////////////////////////////////////////////////////////////
int fal_partition_read(const struct fal_partition *part, uint32_t addr, uint8_t *buf, size_t size)
{
    int ret = 0;
    const struct fal_flash_dev *flash_dev = NULL;

    if (addr + size > part->len)
    {
        return -1;
    }

    flash_dev = fal_flash_device_find(part->flash_name);
    if (flash_dev == NULL)
    {
        return -1;
    }

	if(flash_dev->ops.read == NULL)
		return -1;

    ret = flash_dev->ops.read(part->offset + addr, buf, size);
    if (ret < 0)
    {
		return -1;
    }

    return ret;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_write
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const struct fal_partition *part,
//				 uint32_t addr,
//				 const uint8_t *buf,
//				 size_t size
///////////////////////////////////////////////////////////////
int fal_partition_write(const struct fal_partition *part, uint32_t addr, const uint8_t *buf, size_t size)
{
    int ret = 0;
    const struct fal_flash_dev *flash_dev = NULL;

    if (addr + size > part->len)
    {
        return -1;
    }

    flash_dev = fal_flash_device_find(part->flash_name);
    if (flash_dev == NULL)
    {
        return -1;
    }

	if(flash_dev->ops.write == NULL)
		return -1;

    ret = flash_dev->ops.write(part->offset + addr, buf, size);
    if (ret < 0)
    {
		return -1;
    }

    return ret;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_erase
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const struct fal_partition *part,
//				 uint32_t addr,
//				 size_t size
///////////////////////////////////////////////////////////////
int fal_partition_erase(const struct fal_partition *part, uint32_t addr, size_t size)
{
    int ret = 0;
    const struct fal_flash_dev *flash_dev = NULL;

    if (addr + size > part->len)
    {
        return -1;
    }

    flash_dev = fal_flash_device_find(part->flash_name);
    if (flash_dev == NULL)
    {
        return -1;
    }

    ret = flash_dev->ops.erase(part->offset + addr, size);
    if (ret < 0)
    {
    }

    return ret;
}

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_erase_all
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const struct fal_partition *part
///////////////////////////////////////////////////////////////
int fal_partition_erase_all(const struct fal_partition *part)
{
    return fal_partition_erase(part, 0, part->len);
}

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_write
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const char *name,
//				 uint32_t addr,
//				 const uint8_t *buf,
//				 size_t size
///////////////////////////////////////////////////////////////
int  fal_write(const char *name, uint32_t addr, const uint8_t *buf, size_t size)
{
	const struct fal_partition *part = NULL;
	
	if (!name)
	{
		return 0;
	}
	
	part = fal_partition_find(name);
	if (!part)
	{
		return 0;
	}

	return fal_partition_write(part, addr, buf, size);

}

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_read
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : const char *name,
//				 uint32_t addr,
//				 const uint8_t *buf,
//				 size_t size
///////////////////////////////////////////////////////////////
int  fal_read(const char *name, uint32_t addr, const uint8_t *buf, size_t size)
{
	const struct fal_partition *part = NULL;
	
	if (!name)
	{
		return 0;
	}
	
	part = fal_partition_find(name);
	if (!part)
	{
		return 0;
	}
	
	return fal_partition_read(part, addr, (uint8_t *)buf, size);
}

///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_start
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月26日
//	返 回 值 : uint32_t
//	参数说明 : const char *name
///////////////////////////////////////////////////////////////
uint32_t  fal_partition_start(const char *name)
{
	const struct fal_partition *part = NULL;
	
	if (!name)
	{
		return 0;
	}
	
	part = fal_partition_find(name);
	if (!part)
	{
		return 0;
	}
	
	return part->offset;
}
///////////////////////////////////////////////////////////////
//	函 数 名 : fal_partition_end
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月26日
//	返 回 值 : uint32_t
//	参数说明 : const char *name
///////////////////////////////////////////////////////////////
uint32_t  fal_partition_end(const char *name)
{
	const struct fal_partition *part = NULL;
	
	if (!name)
	{
		return 0;
	}
	
	part = fal_partition_find(name);
	if (!part)
	{
		return 0;
	}
	
	return ( part->offset + part->len );
}
// int i;
// BYTE Buf[256];
// 	fal_init();
// 
// 	for(i=0;i<256;i++)
// 		Buf[i]=i;
// 
// 	fal_write("minFrz", 0, Buf, 256);
// 	
// 	memset(Buf,0,256);
// 	
// 	fal_read("minFrz", 0, Buf, 256);
// 	
// 	i = fal_partition_start("minFrz");
// 	
// 	i = fal_partition_end("minFrz");

// 
// for(i=0;i<256;i++)
// Buf[i]=i;
// 
// fal_write("efrom", 0, Buf, 256);
// 
// memset(Buf,0,256);
// 
// fal_read("efrom", 0, Buf, 256);
// 
// i = fal_partition_start("efrom");
// 
// i = fal_partition_end("efrom");
