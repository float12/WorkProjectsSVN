///////////////////////////////////////////////////////////////
//	�� �� �� :ABC_DF6203.h
//	�ļ����� :��������
////////////////////////////////////////////
#ifndef __ABC_DF6203_H__
#define __ABC_DF6203_H__


//EEPROM��ض���
#define EROM_OFT_NANDBADTAB_1	64//NANDFlash�����(��С TBadBlokTab 576�ֽ�)	
#define EROM_OFT_NANDBADTAB_2	(EROM_OFT_NANDBADTAB_1+576)//NANDFlash�����
//FLASHҳ�Ѳ�����ʶ������Ѳ������Ϳ���ֱ��д�����ڿ���д����СΪ6144�ֽ�(ÿλ����һ��2Kҳ�Ƿ��Ѳ�������96M)��
#define EROM_OFT_FLASHPAGE_OFT	(EROM_OFT_NANDBADTAB_2+576)

//���4K����FLASH����()
#define	EROM_OFT_INITPARA	0x2000//�������õĲ���				
#define	EROM_OFT_FLASHPARA	0x2800//FALSH��2Kϵͳ��������
		
//NANDFLASH ����
#define SIZE_PAGE			2048//ҳ��С		
#define SIZE_FLASH_BLOCK	0x20000	//FLASH���С					
#define FILE_SYSPARA		"SYSPARA.DAT"	//ϵͳ����
#define SYSCFG_OFFSET	    0x0

//4.1 ϵͳ������ (0~0x40000)
//128K������������256k����ʼƫ�� SYSCFG_OFFSET��
//FALSH�����ռ�(�����ռ�0x0~0x140000,0~0xC0000Ϊϵͳ��������0xC0000~0x140000Ϊ�����������)
//ϵͳ����ռ��512K��FLASH�ռ�(�������ͱ�����,ÿ2K�ռ��ɵײ�Ӳ����У��)

//ϵͳ����(���ܼӡ����塢������ 4K)
#define SYSCFG_ADDR			0x0 //�ڲ�ƫ��
//���ز���(4K)
#define FKCFG_ADDR          (SYSCFG_ADDR+0x1000)
//ϵͳ��������(2K)
#define SYSCFG_EX_ADDR		(FKCFG_ADDR+0x1000)
//�������(8K)
#define TASKCFG_ADDR        (SYSCFG_EX_ADDR +0x800)
//������������(336K)
#define MAX_SCH_GENERAL		16	//��ͨ����
#define MAX_SCH_EVENT		4
#define MAX_SCH_REPORT		4
#define MAX_SCH_TRANS		4
#define SYSCFG_SCHEME_ADDR	(TASKCFG_ADDR+0x2000)
//��������
#define SYSCFG_OFFSET_BAK	(SYSCFG_OFFSET+0x60000)
#if(SYSCFG_OFFSET_BAK < (SYSCFG_SCHEME_ADDR+0x54000))
ERROR "ϵͳ���������ռ��ص�!!!"
#endif

//4.2 ����������� (0xC0000~0x140000)
//192K������������384k����ʼƫ�� MPCFG_OFFSET��
#define MPCFG_OFFSET		(SYSCFG_OFFSET_BAK+0x60000)
#if(MPCFG_OFFSET != (0xC0000))
ERROR "ϵͳ���������ռ��ص�!!!"
#endif
#define FILE_MPPARA			"MPPARA.DAT"			//�������������
//���������ռ��512K��FLASH�ռ�(�������ͱ�����,ÿ2K�ռ��ɵײ�Ӳ����У��)
//�������������(256K)
#define MPCFG_ADDR          (0) 
//���������������
#define MPCFG_OFFSET_BAK	(MPCFG_OFFSET+0x40000)
//4.3 �澯(1.5M)
#define FILE_ALM			"SYSTEM.ALM"	//�ն˸澯����
#define ALM_OFFSET			(MPCFG_OFFSET_BAK+0x40000)	
//4.4 ������RAM��(128K),��λ��ÿ2Сʱ����
#define FILE_NVRAM			"NVRAM.RAM"		//�ڴ�����
#define NVRAM_OFFSET		(ALM_OFFSET+0x180000)
//4.5 ����(BLOCK_OF_CURV*24*16)(��0x3000000)(48M)
#define FILE_CUVFRZ			"CUVFRZ.CUV"	//���߶���		
#define CURV_OFFSET			(NVRAM_OFFSET+0x20000)
//4.6 �¶���(BLOCK_OF_MFRZ*12)(��6M)	
#define BLOCK_OF_MFRZ		(512*1024)	
#define FILE_MONFRZ			"MFRZ.MON"		//�¶���
#define MFRZ_OFFSET			(CURV_OFFSET+0x3000000)	
//4.7 �ն���(BLOCK_OF_DFRZ*62)(��31M)	
#define FILE_DAYFRZ			"DFRZ.DAY"		//�ն���
#define DFRZ_OFFSET			(MFRZ_OFFSET+0x600000)
//4.8 �ڵ�ע���ݴ滺����(��128K)
#define SNEWNODE_OFFSET		(DFRZ_OFFSET+0x1F00000)	
#define FILE_SNEWNODE		"SNODE.DAT"		//�ڵ�ע���ļ�
//4.9 ���������Ϣ(0x300000-128K-ÿ���2K�ռ�)
#define	METERRUNINFO_OFFSET	(SNEWNODE_OFFSET+0x20000)
#define FILE_METERINFO		"METINFO.DAT"	//���������Ϣ
//������Ϣ��0~2K һ�� 2~4K ��Ҫ,ʣ��Ԥ����
#define	CHINAINFO_OFFSET	(METERRUNINFO_OFFSET+(0x300000-0x20000)) //ʹ�����128K
#define FILE_CHINAINFO		"CHINAINFO.DAT"	//������Ϣ
//ʵʱ����ƫ��
#define	REAL_OFFSET			(CHINAINFO_OFFSET+0x20000)	//ʵʱ����
#define FILE_REAL			"REAL.DAT"		//ʵʱ����
#define BLOCK_OF_REAL		0x300000	
#define CATSIZE             16 //�����ֿ��С ���뱣֤���Ա�BLOCK_OF_REAL / SIZE_PAGE����
//�����ն���(BLOCK_OF_SETTLEDAY*12)(��1.5M)
#define BLOCK_OF_SETTLEDAY	(128*1024)	
#define SETTLEDAY_OFFSET    (REAL_OFFSET+BLOCK_OF_REAL)
#define FILE_SETTLEDAYFRZ   "SETLFRZ.MON"   //�����ն���
#define SIZE_OF_SETTLEDAY   0x180000
//���ݴ��������(�ͼ���������һ��)
#define	MAX_DATA_FLASH		(SETTLEDAY_OFFSET+SIZE_OF_SETTLEDAY)


//�����ļ���ʱ������CACH��(��128�������ݵ����ʱ��ȡCACH��)
#define CACHE_BLOCK_OFFSET	(0x7000000-0x1000000)	
//0x7000000��ַ�ռ俪ʼ���ڻ����滻
#define OFFSET_BLOCK_BADBAK	0x7000000

#if(MAX_DATA_FLASH >= CACHE_BLOCK_OFFSET)
ERROR "FLASH�����ռ��ص�!!!!!!!"
#endif

#endif

