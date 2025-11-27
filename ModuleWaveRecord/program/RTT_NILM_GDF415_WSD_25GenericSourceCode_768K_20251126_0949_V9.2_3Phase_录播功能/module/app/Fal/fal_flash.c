/*
 * File      : fal_flash.c
 * This file is part of FAL (Flash Abstraction Layer) package
 * COPYRIGHT (C) 2006 - 2018, ZX-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */
#include "AllHead.h"
#include "fal.h"
#include <string.h>

/* flash device table, must defined by user */
#if !defined(FAL_FLASH_DEV_TABLE)
#error "You must defined flash device table (FAL_FLASH_DEV_TABLE) on 'fal_cfg.h'"
#endif

static const struct fal_flash_dev * const device_table[] = FAL_FLASH_DEV_TABLE;
static const size_t device_table_len = sizeof(device_table) / sizeof(device_table[0]);
static uint8_t init_ok = 0;

extern BOOL WriteExtFlash( BYTE No, DWORD Addr, WORD Len, BYTE *pBuf );
extern BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);

extern WORD WriteEEPRom2(WORD Addr, WORD Length, BYTE * Buf);
extern WORD ReadEEPRom2(WORD Addr, WORD Length, BYTE * Buf);

int flash_read(long offset, uint8_t *buf, size_t size)
{
    return ReadExtFlash(0,offset, size, buf);//spi_flash_read(EXT_FLASH_BASE + offset, buf, size);
}

int flash_write(long offset, const uint8_t *buf, size_t size)
{
    return WriteExtFlash(0,offset, size, (uint8_t *)buf);//spi_flash_write(EXT_FLASH_BASE + offset, buf, size);
}

int flash_erase(long offset, size_t size)
{
    return 0;//spi_flash_erase(EXT_FLASH_BASE + offset, size);
}

int efrom_read(long offset, uint8_t *buf, size_t size)
{
    return ReadEEPRom2(offset, size, buf);//spi_flash_read(EXT_FLASH_BASE + offset, buf, size);
}

int efrom_write(long offset, const uint8_t *buf, size_t size)
{
    return WriteEEPRom2(offset, size, (uint8_t *)buf);//spi_flash_write(EXT_FLASH_BASE + offset, buf, size);
}

const struct fal_flash_dev ext_flash =
{
    "EXT_FL",
	0,						//	EXT_FLASH_BASE,
	(8192UL*1024UL),		//	EXT_FLASH_SIZE,
	(4096UL),				//	EXT_SECTOR_SIZE,

    {
        NULL,				//	spi_flash_init,
		flash_read,			//	flash_read,
		flash_write,		//	flash_write,
		NULL,				//	flash_erase,
    }
};
const struct fal_flash_dev scm_onchip_flash =
{
    "E2F_FL",
	0,						//	EXT_FLASH_BASE,
	(64UL*1024UL),		//	EXT_FLASH_SIZE,
	(128UL),				//	EXT_SECTOR_SIZE,
		
    {
        NULL,				//	spi_flash_init,
		efrom_read,			//	flash_read,
		efrom_write,		//	flash_write,
		NULL,				//	flash_erase,
    }
};




///////////////////////////////////////////////////////////////
//	函 数 名 : fal_flash_init
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : int
//	参数说明 : void
///////////////////////////////////////////////////////////////
int fal_flash_init(void)
{
    size_t i;

    if (init_ok)
    {
        return 0;
    }

    for (i = 0; i < device_table_len; i++)
    {
        /* init flash device on flash table */
        if (device_table[i]->ops.init)
        {
            device_table[i]->ops.init();
        }
        //Trace("Flash device | %s | addr: 0x%08x | len: 0x%08x | blk_size: 0x%08x |initialized finish.\r\n",
        //        device_table[i]->name, device_table[i]->addr, device_table[i]->len,device_table[i]->blk_size);
    }

    init_ok = 1;
    return 0;
}


///////////////////////////////////////////////////////////////
//	函 数 名 : *fal_flash_device_find
//	函数功能 : 
//	处理过程 : 
//	备    注 : 
//	作    者 : 智能电表组
//	时    间 : 2021年6月25日
//	返 回 值 : fal_flash_dev
//	参数说明 : const char *name
///////////////////////////////////////////////////////////////
const struct fal_flash_dev *fal_flash_device_find(const char *name)
{
	int i;

    for (i = 0; i < device_table_len; i++)
    {
        if (!strncmp(name, device_table[i]->name, FAL_DEV_NAME_MAX)) {
            return device_table[i];
        }
    }

    return NULL;
}
