//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	马亮
//日期：	2017-02-06
//描述		单相表液晶显示处理
//修改记录	
//----------------------------------------------------------------------

#ifndef __LCD_HT_SINGLE
#define __LCD_HT_SINGLE

// #if( (LCD_TYPE == LCD_HT_SINGLE_METER) && (BOARD_TYPE == BOARD_HT_SINGLE_78201602)\
// ||(LCD_TYPE == LCD_HT_SINGLE_METER) && (BOARD_TYPE == BOARD_HT_SINGLE_20250819) )
//待定!!!!!!
#if( (LCD_TYPE == LCD_HT_SINGLE_METER) && (BOARD_TYPE == BOARD_HT_SINGLE_78201602))
//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//定义显示缓冲区长度
#define LCD_BUF_LENGTH				31

//err 报警继电器 小铃铛 背光模式字对应编号
#define LCD_RELAY_ERR				0	//负荷开关误动
#define LCD_ESAM_ERR				1	//ESAM错误
#define LCD_INCARD_ERR				2	//内卡初始化错误
#define LCD_RTC_BATT_LOW			3	//时钟电池电压低
#define LCD_PROGRAM_ERR				4	//内部程序错误
#define LCD_EEPROM_ERR				5	//存储器故障或损坏
#define LCD_CTR_CURRENT_ERR			6	//原先的控制回路错误
#define LCD_RTC_ERR					7	//时钟故障

#define LCD_RUN_BATT_LOW			8	//停电抄表电池欠压
#define LCD_OVER_DRAFT				9	//透支状态
#define LCD_COVER_OPEN				10	//开表盖
#define LCD_BUTTON_COVER_OPEN		11	//开端钮盒	
#define LCD_MAGNETIC_ERROR			12	//恒定磁场干扰
#define LCD_POWER_ERROR				13	//电源异常
#define LCD_RELAY_OPEN				14	//跳闸	
#define LCD_RELAY_CLOSE				15	//合闸	

#define LCD_LOST_V					16	//失压		
#define LCD_WEAK_V					17	//欠压		
#define LCD_OVER_V					18	//过压		
#define LCD_LOST_I					19	//失流		
#define LCD_OVER_I					20	//过流		
#define LCD_OVERLOAD				21	//过载		
#define LCD_BACKPROP				22	//功率反向	
#define LCD_BREAK_V					23	//断相		

#define LCD_BREAK_I					24	//断流					
#define LCD_PHASE_COS_OVER			25	//分相功率因数超限		
#define LCD_RESERVE_26				26	//备用					
#define LCD_RESERVE_27				27	//备用					
#define LCD_RESERVE_28				28	//备用					
#define LCD_RESERVE_29				29	//备用					
#define LCD_RESERVE_30				30	//备用					
#define LCD_RESERVE_31				31	//备用					

#define LCD_REVERS_V				32	//电压逆相序			
#define LCD_REVERS_I				33	//电流逆相序			
#define LCD_UNBANLANCE_V			34	//电压不平衡			
#define LCD_UNBANLANCE_I			35	//电流不平衡			
#define LCD_LOST_SECOND_P			36	//辅助电源失电			
#define LCD_LOST_POWER				37	//掉电（有辅助电源时）	
#define LCD_P_DEMAND_OVER			38	//超有功需量报警事件	
#define LCD_COS_OVER				39	//总功率因数超限		

#define LCD_UNBANLANCE_I_S			40	//电流严重不平衡		
#define LCD_ALL_POWER_BACK			41	//总有功功率反向		
#define LCD_LOST_ALL_V				42	//全失压（能工作时）	
#define LCD_RESERVE_43				43	//备用					
#define LCD_P_ENERGY_REVERSE		44	//有功电能方向反向  ----标准上无此项，不要用,总功率反向用 LCD_ALL_POWER_BACK wlk
#define LCD_RESERVE_45				45	//备用					
#define LCD_RESERVE_46				46	//备用					
#define LCD_RESERVE_47				47	//备用					

#define LCD_PROMGRAM				48	//电表编程				
#define LCD_METER_EN_CLR			49	//电表清零				
#define LCD_METER_DM_CLR			50	//需量清零				
#define LCD_METER_EV_CLR			51	//事件清零				
#define LCD_METER_TM_SET			52	//校时					
#define LCD_METER_SG_SET			53	//时段表编程			
#define LCD_METER_TZ_SET			54	//时区表编程			
#define LCD_METER_WK_SET			55	//周休日编程			

#define LCD_METER_HD_SET			56	//节假日编程			
#define LCD_METER_PA_SET			57	//有功组合方式编程		
#define LCD_METER_RA1_SET			58	//无功组合方式1编程		
#define LCD_METER_RA2_SET			59	//无功组合方式2编程		
#define LCD_METER_JS_SET			60	//结算日编程			
#define LCD_METER_FEE_SET			61	//费率参数表编程		
#define LCD_METER_JT_SET			62	//阶梯表编程			
#define LCD_METER_KEY_CHG			63	//密钥更新	

#define	LCD_ERR_END_NO				64			

#define LCD_DIGITAL_TUBE_NUM		12	//数码管数量

#define LCD_SIGN_NUM				24	//符号数量

//新液晶屏新增符号标志
#define LCD_CURR_RATIO_T					lcd[13] |= 0x01			// 显示当前费率“T”
#define LCD_CURR_RATIO_1					lcd[13] |= 0x02			// 显示当前费率“1”
#define LCD_DATA_RATIO_T					lcd[ 5] |= 0x20			// 显示数据费率“费率”
#define LCD_DATA_RATIO_1					lcd[16] |= 0x20			// 显示数据费率“1”
#define LCD_CURR_LADDOR_L					lcd[15] |= 0x02			// 显示当前阶梯“L”
#define LCD_CURR_LADDOR_L_CLR				lcd[15] &= ~0x02		// 清除当前阶梯“L”
#define LCD_FORMER_MONTH_1					lcd[21] |= 0x20			// 显示上 月间“1”

// LCD状态显示
#define LCD_CPUCARD_READING					DoReadingCard()	// 新液晶没有‘读卡中’	显示8个'-'
#define LCD_CPUCARD_READ_OK					(lcd[10]|=	0x01)   	// 显示'成功'

#define LCD_CPUCARD_READ_BAD		        (lcd[12]|=	0x01)		// 显示'读卡失败'

#define LCD_CPUCARD_CLR						(lcd[10] &= ~0x01);\
											(lcd[12] &= ~0x01)		// 清除上述所有文字

#define LCD_STATUS_BUY_POWER				(lcd[12] |=	0x02)		// 显示'请购电标志'
#define LCD_STATUS_BUY_POWER_CLR			(lcd[12] &= ~0x02)		// 清除'请购电标志'
#define LCD_STATUS_CUT						(lcd[16] |=	0x01)		// 显示系统跳闸标志
#define LCD_STATUS_CUT_CLR					(lcd[16] &=~0x01)		// 清除系统跳闸标志
#define LCD_STATUS_HOARD					;						// 新液晶没有囤积标志			显示囤积标志
#define LCD_STATUS_HOARD_CLR				;						// 清除囤积标志

#define LCD_STATUS_COMMU_CR				(lcd[23] |= 0x10)		// 显示载波通讯标志
#define LCD_STATUS_COMMU_RF				(lcd[22] |= 0x10)		// 显示红外通讯标志
#define LCD_STATUS_COMMU_485			(lcd[22] |= 0x10)		// 显示485通讯标志1
#define LCD_STATUS_COMMU2_485			(lcd[22] |= 0x10)		// 显示485通讯标志2
#define LCD_STATUS_COMMU_485_RF_CR_CLR		(lcd[22] &= ~0x10);\
											(lcd[23] &= ~0x10)   // 清除红外、485、载波通讯标志

#define LCD_STATUS_LOW_BATT				(lcd[21] |= 0x01)	  // 显示电池1低电压
#define LCD_STATUS_LOW_BATT_CLR			(lcd[21] &= ~0x01)	  // 清除电池1低电压

#define LCD_STATUS_OP_POWER				(lcd[23] |= 0x01)			// 显示功率反向标志(箭头)
#define LCD_STATUS_OP_POWER_CLR			(lcd[23] &= ~0x01)			// 清除功率反向标志

#define LCD_STATUS_ALLOC_PRG			(lcd[17]|= 0x01)	// 显示允许编程标志
#define LCD_STATUS_ALLOC_PRG_CLR		(lcd[17]&= ~0x01)	// 清除允许编程标志
#define LCD_STATUS_SYS_LOCK				(lcd[0]|= 0x01)		// 显示系统锁定标志
#define LCD_STATUS_SYS_LOCK_CLR			(lcd[0]&= ~0x01)	// 清除系统锁定标志
#define LCD_STATUS_PUBLIC_KEY			(lcd[21] |= 0x10)		// 显示公匙状态标志
#define LCD_STATUS_PUBLIC_KEY_CLR		(lcd[21] &= ~0x10)		// 清除公匙状态标志
#define LCD_STATUS_ALARM				;	// 显示告警状态标志
#define LCD_STATUS_ALARM_CLR			;	// 清除告警状态标志???

#define LCD_STATUS_SEG_CLR				(lcd[14] &= ~0x02);\
										(lcd[14] &= ~0x01)		// 清除当前时段表号指示
#define LCD_STATUS_SEG_1				(lcd[14] |= 0x02)		// 当前运行在第一时段
#define LCD_STATUS_SEG_2				(lcd[14] |= 0x01)		// 当前运行在第二时段


#define LCD_DATA_RATIO_CLR				lcd[5] &= ~0x20;\
										lcd[16] &= ~0x20;\
										CLRDigTub(1)				// 清除下方“T ”及数码管显示和前面那个“1”

#define LCD_STATUS_RATIO_CLR			lcd[13] &= ~0x03;\
										CLRDigTub(11)				// 清除上方“费率”及数码管显示和前面那个“1”

#define LCD_STATUS_GRADE_TABLE_CLR		(lcd[16] &= ~0x02);\
										(lcd[15] &= ~0x01)	        // 清除当前阶梯电价方案指示
#define LCD_STATUS_GRADE_TABLE_1		(lcd[16] |= 0x02)		// 当前使用第一套阶梯电价
#define LCD_STATUS_GRADE_TABLE_2		(lcd[15] |= 0x01)		// 当前使用第二套阶梯电价

//数据显示区小数点
//从右到左1～7：	小数点
#define SET_POINT_1						(lcd[9] |= 0x01)						//第1小数点
#define SET_POINT_2						(lcd[6] |= 0x01)						//第2小数点
#define SET_POINT_3						(lcd[4] |= 0x01)						//第3小数点					
#define SET_POINT_4						(lcd[2] |= 0x01)						//第4小数点
#define SET_POINT_5						;						//第5小数点
#define SET_POINT_6						;						//第6小数点
#define SET_POINT_7						;						//第7小数点
#define SET_POINT_2_2					(lcd[6] |= 0x01);\
										(lcd[5] |= 0x10)		//两个小数点
#define SET_POINT_4_4					(lcd[2] |= 0x01);\
										(lcd[1] |= 0x10)		//两个小数点
//显示时间的格式(nnXX:XX:XX，时间，nn为空格)
#define SHOW_DOT_TIME			    		SET_POINT_4_4;\
                                      		SET_POINT_2_2
						
//显示日期的格式(nnXX.XX.XX，日期，n为空格)最大需量的时间(YY.MM.DD)
#define SHOW_DOT_DATE				        SET_POINT_4;\
                                    		SET_POINT_2

//显示公共假日格式(YY.MM.DD:NN)
#define SHOW_DOT_HOLIDAY_TIME				SET_POINT_6;\
											SET_POINT_4;\
											SET_POINT_2_2
											
//显示时区表(MM.DD.NN)
#define SHOW_DOT_AREA_ZONE					SET_POINT_4;\
											SET_POINT_2

//显示当前运行时段及费率号 HH:mm.NN
#define SHOW_DOT_CUR_TIME_RATIO		    	SET_POINT_4_4;\
							                SET_POINT_2		    

#define SHOW_UNIT_KWH			        	(lcd[9] |= 0x02);\
                          					(lcd[10] |= 0x02)	    //KWH
#define SHOW_UNIT_KW			  			(lcd[9] |= 0x02)		//KW
#define SHOW_UNIT_V				        	(lcd[9] |= 0x04)		//V
#define SHOW_UNIT_A				        	(lcd[10] |= 0x04)		//A
#define SHOW_UNIT_YUAN			        	(lcd[12] |= 0x04)	    //元

#define SHOW_DATA_NEGATIVE		     		(lcd[23] |= 0x04)		//负号

// 汉字提示显示
#define SHOW_CHINESE_ZONG		       		(lcd[4] |= 0x20)		// 汉字'总'

#define SHOW_CHINESE_DANGQIAN				(lcd[23] |= 0x20);	// '当前'
#define SHOW_CHINESE_SHANGYUE				(lcd[22] |= 0x20);\
											(lcd[17] |= 0x20)	//上月						    //上月
#define SHOW_CHINESE_SHANGCI				(lcd[22] |= 0x20)	// ‘上’
#define SHOW_CHINESE_YUE		      	    (lcd[17] |= 0x20)	// '月'

#define SHOW_CHINESE_ZUHE			        (lcd[30] |= 0x20)		// '组合'
#define SHOW_CHINESE_FANXIANG		        (lcd[2] |= 0x20);\
                                            (lcd[3] |= 0x20)		//反向

#define SHOW_CHINESE_ZHENGXIANG				(lcd[0] |= 0x20);\
                                            (lcd[3] |= 0x20)		//正向

#define SHOW_CHINESE_SHENGYU		        (lcd[10] |= 0x20)		// '剩余'
#define SHOW_CHINESE_COS					(lcd[10] |= 0x08)   	// 'COS'
#define SHOW_CHINESE_JIETI					(lcd[17] |= 0x10)		// '阶梯'
#define SHOW_CHINESE_TOUZHI             	(lcd[30] |= 0x10)		// '透支'

#define SHOW_CHINESE_DIANLIANG		    	(lcd[1] |= 0x20);\
							                (lcd[2] |= 0x10)		//电量
#define SHOW_CHINESE_DIANJIA		        (lcd[1] |= 0x20);\
							                (lcd[3] |= 0x10)		//电价

#define SHOW_CHINESE_SHIJIAN		        (lcd[6] |= 0x20);\
							                (lcd[6] |= 0x10)		//时间
#define SHOW_CHINESE_CHANGSHU				(lcd[12] |= 0x20)		//常数
#define SHOW_CHINESE_BIAOHAO				(lcd[12] |= 0x10);\
											(lcd[12] |= 0x08)		//表号
#define SHOW_CHINESE_JINE					(lcd[10] |= 0x10)		//金额
#define SHOW_CHINESE_YONG					(lcd[0] |= 0x10)		//用
#define SHOW_CHINESE_HUHAO					(lcd[4] |= 0x10);\
											(lcd[12] |= 0x08)		//户号

#define SHOW_Err_XX							DigTubCtrlShow(14,4);\
											DigTubCtrlShow(21,5);\
											DigTubCtrlShow(21,6);\
											DigTubCtrlShow(20,7)			//显示__Err-XX		__代表两个空格
	
#define SHOW_Err_01XX						DigTubCtrlShow(14,2);\
											DigTubCtrlShow(21,3);\
											DigTubCtrlShow(21,4);\
											DigTubCtrlShow(20,5)						//显示_Err-01XX		_代表一个空格		__代表空格

#define SHOW_NEGATIVE						(lcd[23] |= 0x04)		//显示最左侧负号‘-’

#define LCD_STATUS_L						(lcd[23] |= 0x08)		//L	  火线
#define LCD_STATUS_N						(lcd[23] |= 0x02)		//N	  零线


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
enum
{
	eLcdAlarm,
	eLcdHoard,	
	eLcdCommu485,	//这4个通信口顺序不能动
	eLcdCommuRf,	
	eLcdCommuCr,
	eLcdCommu2_485,
	eLcdCardSuccess,
	eLcdCardFailure,
	
	eLcdFlagT,
};
//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern BYTE lcd[LCD_BUF_LENGTH];

//-----------------------------------------------
// 				函数声明
//-----------------------------------------------

#endif	//#if( (LCD_TYPE == LCD_HT_SINGLE_METER)&&(BOARD_TYPE == BOARD_HT_SINGLE_78201602) )
#endif	// #ifndef

