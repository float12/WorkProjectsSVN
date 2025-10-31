#ifndef __RELATION_H
#define __RELATION_H
//***************************************************
//系统联系定义（不要去修改！！！)
//没有通讯就没有必要底层支持。
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
"串口数目配置不对"
#endif

#if( BOARD_TYPE < 30000 )
	#if( cMETER_TYPES != METER_ZT )
	单相配置错误
	#endif
	
	#if(cMETER_VOLTAGE != METER_220V)
	电压配置不对
	#endif
	
	#if( (cCURR_TYPES==CURR_1A) || (cCURR_TYPES==CURR_6A) )
	单相配置错误
	#endif	
	
	#if( (PPRECISION_TYPE == PPRECISION_C) || (PPRECISION_TYPE == PPRECISION_D) )
	单相配置错误
	#endif
	
#else

#endif

#if( cCURR_TYPES != CURR_60A )
	#if( cRELAY_TYPE == RELAY_INSIDE )
	继电器配置错误
	#endif
#endif

#if(cMETER_TYPES == METER_ZT)
	#if(cMETER_VOLTAGE != METER_220V)
	电压配置不对
	#endif
#elif(cMETER_TYPES == METER_3P4W)
	#if( (cMETER_VOLTAGE!=METER_220V) && (cMETER_VOLTAGE!=METER_57V) )
	电压配置不对
	#endif
	
	#if( (cCURR_TYPES==CURR_60A) || (cCURR_TYPES==CURR_100A))
	配置不对
	#endif
#else
	#if( (cMETER_VOLTAGE!=METER_220V))
	电压配置不对
	#endif
	
	#if(cCURR_TYPES!=CURR_100A)
	配置不对
	#endif
#endif

#if( SEL_DLT645_2007 == NO )
	#if(SEL_PRG_RECORD645 == YES)
	配置错误
	#endif
#endif



//一个扇区大小 感觉放在这个文件不太合理，但是又找不到合适的地方，需要放在event_api.h和freeze_api.h之前
#if(( THIRD_MEM_CHIP == CHIP_GD25Q256C ) ||( THIRD_MEM_CHIP == CHIP_GD25Q64C ) ||(THIRD_MEM_CHIP == CHIP_GD25Q32C)||(THIRD_MEM_CHIP == CHIP_GD25Q16C))
	#define SECTOR_SIZE  		4096
#endif

#endif//对应文件开始的 #ifndef __RELATION_H

