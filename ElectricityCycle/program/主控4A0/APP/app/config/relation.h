#ifndef __RELATION_H
#define __RELATION_H
//***************************************************
//ϵͳ��ϵ���壨��Ҫȥ�޸ģ�����)
//û��ͨѶ��û�б�Ҫ�ײ�֧�֡�
#if ( SEL_SEPARATE_ENERGY == YES )
	#define NUM_OF_SEPARATE_ENERGY					3
	#if( SEL_SEPARATE_RATIO_ENERGY == YES )
		#define NUM_OF_SEPARATE_ENERGY_RATIO		NUM_OF_SEPARATE_ENERGY
	#else
		#define NUM_OF_SEPARATE_ENERGY_RATIO		0
	#endif
#else
	#define NUM_OF_SEPARATE_ENERGY					0
	#define NUM_OF_SEPARATE_ENERGY_RATIO			NUM_OF_SEPARATE_ENERGY
#endif


#if ( (MAX_COM_CHANNEL_NUM > 6) || (MAX_COM_CHANNEL_NUM < 2) )
"������Ŀ���ò���"
#endif

#if( BOARD_TYPE < 30000 )
	#if( cMETER_TYPES != METER_ZT )
	�������ô���
	#endif
	
	#if(cMETER_VOLTAGE != METER_220V)
	��ѹ���ò���
	#endif
	
	#if( (cCURR_TYPES==CURR_1A) || (cCURR_TYPES==CURR_6A) )
	�������ô���
	#endif	
	
	#if( (PPRECISION_TYPE == PPRECISION_C) || (PPRECISION_TYPE == PPRECISION_D) )
	�������ô���
	#endif
	
#else

#endif

#if( cCURR_TYPES != CURR_60A )
	#if( cRELAY_TYPE == RELAY_INSIDE )
	�̵������ô���
	#endif
#endif

#if(cMETER_TYPES == METER_ZT)
	#if(cMETER_VOLTAGE != METER_220V)
	��ѹ���ò���
	#endif
#elif(cMETER_TYPES == METER_3P4W)
	#if( (cMETER_VOLTAGE!=METER_220V) && (cMETER_VOLTAGE!=METER_57V) )
	��ѹ���ò���
	#endif
	
	#if( (cCURR_TYPES==CURR_60A) || (cCURR_TYPES==CURR_100A))
	���ò���
	#endif
#else
	#if( (cMETER_VOLTAGE!=METER_220V))
	��ѹ���ò���
	#endif
	
	#if(cCURR_TYPES!=CURR_100A)
	���ò���
	#endif
#endif

#if( SEL_DLT645_2007 == NO )
	#if(SEL_PRG_RECORD645 == YES)
	���ô���
	#endif
#endif



//һ��������С �о���������ļ���̫�����������Ҳ������ʵĵط�����Ҫ����event_api.h��freeze_api.h֮ǰ
#if(( THIRD_MEM_CHIP == CHIP_GD25Q256C ) ||( THIRD_MEM_CHIP == CHIP_GD25Q64C ) ||(THIRD_MEM_CHIP == CHIP_GD25Q32C)||(THIRD_MEM_CHIP == CHIP_GD25Q16C))
	#define SECTOR_SIZE  		4096
#endif

#endif//��Ӧ�ļ���ʼ�� #ifndef __RELATION_H

