#ifndef __RELATION_H
#define __RELATION_H
//***************************************************
//ϵͳ��ϵ���壨��Ҫȥ�޸ģ�����)
//û��ͨѶ��û�б�Ҫ�ײ�֧�֡�


//һ��������С �о���������ļ���̫�����������Ҳ������ʵĵط�����Ҫ����event_api.h��freeze_api.h֮ǰ

#if ( (THIRD_MEM_CHIP == CHIP_GD25Q64C)||(THIRD_MEM_CHIP == CHIP_NO))//û��FLASH��ʱ��Ҳ��������꣬Ҫ��Ȼû������

	#define SECTOR_SIZE  		4096
#endif

#endif//��Ӧ�ļ���ʼ�� #ifndef __RELATION_H

