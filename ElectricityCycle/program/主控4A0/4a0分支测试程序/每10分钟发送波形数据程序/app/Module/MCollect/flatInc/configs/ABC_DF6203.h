///////////////////////////////////////////////////////////////
//	文 件 名 :ABC_DF6203.h
//	文件功能 :公用配置
////////////////////////////////////////////
#ifndef __ABC_DF6203_H__
#define __ABC_DF6203_H__


//EEPROM相关定义
#define EROM_OFT_NANDBADTAB_1	64//NANDFlash坏块表(大小 TBadBlokTab 576字节)	
#define EROM_OFT_NANDBADTAB_2	(EROM_OFT_NANDBADTAB_1+576)//NANDFlash坏块表
//FLASH页已擦除标识（如果已擦除，就可以直接写，用于快速写，大小为6144字节(每位代表一个2K页是否已擦除，共96M)）
#define EROM_OFT_FLASHPAGE_OFT	(EROM_OFT_NANDBADTAB_2+576)

//最后4K用作FLASH参数()
#define	EROM_OFT_INITPARA	0x2000//出厂配置的参数				
#define	EROM_OFT_FLASHPARA	0x2800//FALSH中2K系统基本参数
		
//NANDFLASH 定义
#define SIZE_PAGE			2048//页大小		
#define SIZE_FLASH_BLOCK	0x20000	//FLASH块大小					
#define FILE_SYSPARA		"SYSPARA.DAT"	//系统参数
#define SYSCFG_OFFSET	    0x0

//4.1 系统参数区 (0~0x40000)
//128K带备份区，共256k（起始偏移 SYSCFG_OFFSET）
//FALSH存贮空间(参数空间0x0~0x140000,0~0xC0000为系统参数区，0xC0000~0x140000为测量点参数区)
//系统参数占用512K的FLASH空间(分主区和备份区,每2K空间由底层硬件做校验)

//系统参数(含总加、脉冲、运算量 4K)
#define SYSCFG_ADDR			0x0 //内部偏移
//负控参数(4K)
#define FKCFG_ADDR          (SYSCFG_ADDR+0x1000)
//系统扩充配置(2K)
#define SYSCFG_EX_ADDR		(FKCFG_ADDR+0x1000)
//任务参数(8K)
#define TASKCFG_ADDR        (SYSCFG_EX_ADDR +0x800)
//方案参数配置(336K)
#define MAX_SCH_GENERAL		16	//普通方案
#define MAX_SCH_EVENT		4
#define MAX_SCH_REPORT		4
#define MAX_SCH_TRANS		4
#define SYSCFG_SCHEME_ADDR	(TASKCFG_ADDR+0x2000)
//备份区间
#define SYSCFG_OFFSET_BAK	(SYSCFG_OFFSET+0x60000)
#if(SYSCFG_OFFSET_BAK < (SYSCFG_SCHEME_ADDR+0x54000))
ERROR "系统参数存贮空间重叠!!!"
#endif

//4.2 测量点参数区 (0xC0000~0x140000)
//192K带备份区，共384k（起始偏移 MPCFG_OFFSET）
#define MPCFG_OFFSET		(SYSCFG_OFFSET_BAK+0x60000)
#if(MPCFG_OFFSET != (0xC0000))
ERROR "系统参数存贮空间重叠!!!"
#endif
#define FILE_MPPARA			"MPPARA.DAT"			//测量点基本参数
//测量点参数占用512K的FLASH空间(分主区和备份区,每2K空间由底层硬件做校验)
//测量点基本参数(256K)
#define MPCFG_ADDR          (0) 
//测量点参数备份区
#define MPCFG_OFFSET_BAK	(MPCFG_OFFSET+0x40000)
//4.3 告警(1.5M)
#define FILE_ALM			"SYSTEM.ALM"	//终端告警数据
#define ALM_OFFSET			(MPCFG_OFFSET_BAK+0x40000)	
//4.4 不掉电RAM区(128K),复位、每2小时存贮
#define FILE_NVRAM			"NVRAM.RAM"		//内存数据
#define NVRAM_OFFSET		(ALM_OFFSET+0x180000)
//4.5 曲线(BLOCK_OF_CURV*24*16)(共0x3000000)(48M)
#define FILE_CUVFRZ			"CUVFRZ.CUV"	//曲线冻结		
#define CURV_OFFSET			(NVRAM_OFFSET+0x20000)
//4.6 月冻结(BLOCK_OF_MFRZ*12)(共6M)	
#define BLOCK_OF_MFRZ		(512*1024)	
#define FILE_MONFRZ			"MFRZ.MON"		//月冻结
#define MFRZ_OFFSET			(CURV_OFFSET+0x3000000)	
//4.7 日冻结(BLOCK_OF_DFRZ*62)(共31M)	
#define FILE_DAYFRZ			"DFRZ.DAY"		//日冻结
#define DFRZ_OFFSET			(MFRZ_OFFSET+0x600000)
//4.8 节点注册暂存缓冲区(共128K)
#define SNEWNODE_OFFSET		(DFRZ_OFFSET+0x1F00000)	
#define FILE_SNEWNODE		"SNODE.DAT"		//节点注册文件
//4.9 电表运行信息(0x300000-128K-每块表2K空间)
#define	METERRUNINFO_OFFSET	(SNEWNODE_OFFSET+0x20000)
#define FILE_METERINFO		"METINFO.DAT"	//电表运行信息
//中文信息（0~2K 一般 2~4K 重要,剩下预留）
#define	CHINAINFO_OFFSET	(METERRUNINFO_OFFSET+(0x300000-0x20000)) //使用最后128K
#define FILE_CHINAINFO		"CHINAINFO.DAT"	//中文信息
//实时数据偏移
#define	REAL_OFFSET			(CHINAINFO_OFFSET+0x20000)	//实时数据
#define FILE_REAL			"REAL.DAT"		//实时数据
#define BLOCK_OF_REAL		0x300000	
#define CATSIZE             16 //索引分块大小 必须保证可以被BLOCK_OF_REAL / SIZE_PAGE整除
//结算日冻结(BLOCK_OF_SETTLEDAY*12)(共1.5M)
#define BLOCK_OF_SETTLEDAY	(128*1024)	
#define SETTLEDAY_OFFSET    (REAL_OFFSET+BLOCK_OF_REAL)
#define FILE_SETTLEDAYFRZ   "SETLFRZ.MON"   //结算日冻结
#define SIZE_OF_SETTLEDAY   0x180000
//数据存贮区最大(和集中器保持一致)
#define	MAX_DATA_FLASH		(SETTLEDAY_OFFSET+SIZE_OF_SETTLEDAY)


//用于文件临时存贮的CACH块(共128个，根据当天的时间取CACH块)
#define CACHE_BLOCK_OFFSET	(0x7000000-0x1000000)	
//0x7000000地址空间开始用于坏块替换
#define OFFSET_BLOCK_BADBAK	0x7000000

#if(MAX_DATA_FLASH >= CACHE_BLOCK_OFFSET)
ERROR "FLASH存贮空间重叠!!!!!!!"
#endif

#endif

