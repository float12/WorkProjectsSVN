#ifndef __RELATION_H
#define __RELATION_H
//***************************************************
//系统联系定义（不要去修改！！！)
//没有通讯就没有必要底层支持。


//一个扇区大小 感觉放在这个文件不太合理，但是又找不到合适的地方，需要放在event_api.h和freeze_api.h之前

#if ( (THIRD_MEM_CHIP == CHIP_GD25Q64C)||(THIRD_MEM_CHIP == CHIP_NO))//没有FLASH的时候也得有这个宏，要不然没法编译

	#define SECTOR_SIZE  		4096
#endif

#endif//对应文件开始的 #ifndef __RELATION_H

