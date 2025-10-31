//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	马亮 张玉猛
//创建日期	2016-10-08
//描述		三相表液晶显示处理
//修改记录	
//----------------------------------------------------------------------

#ifndef __LCD_GW3PAHSE09_M8
#define __LCD_GW3PAHSE09_M8

#if( LCD_TYPE == LCD_GW3PHASE_09 )	//09年8月国网三相表液晶

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//定义显示缓冲区长度
#define LCD_BUF_LENGTH					34

//数码管个数
#define LCD_DIGITAL_TUBE_NUM			24
//符号个数
#define LCD_SIGN_NUM					24

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

//新液晶屏新增符号标志
#define LCD_CURR_RATIO_T					lcd[19] |= 0x02			// 显示当前费率“T”
#define LCD_CURR_RATIO_1					lcd[20] |= 0x10			// 显示当前费率“1”
#define LCD_DATA_RATIO_T					lcd[23] |= 0x02			// 显示数据费率“费率”
#define LCD_DATA_RATIO_1					lcd[22] |= 0x08			// 显示数据费率“1”
#define LCD_CURR_LADDOR_L					lcd[16] |= 0x01			// 显示当前阶梯“L”
#define LCD_CURR_LADDOR_L_CLR				lcd[16] &= ~0x01		// 清除当前阶梯“L”


// LCD状态显示
#define LCD_CPUCARD_READING				DoReadingCard()	// 新液晶没有‘读卡中’	显示8个'-'
#define LCD_CPUCARD_READ_OK				(lcd[20] |= 0x01);	// 显示‘读卡成功’
#define LCD_CPUCARD_READ_BAD			(lcd[20] |= 0x02); 	// 显示‘读卡失败’
#define LCD_CPUCARD_CLR					(lcd[20] &= ~0x03);	// 清除上述所有文字

#define LCD_STATUS_BUY_POWER			(lcd[20] |= 0x04)	// 显示‘请购电标志’
#define LCD_STATUS_BUY_POWER_CLR		(lcd[20] &= ~0x04)	// 清除‘请购电标志’
#define LCD_STATUS_CUT					(lcd[20] |= 0x08)	// 显示系统跳闸标志
#define LCD_STATUS_CUT_CLR				(lcd[20] &= ~0x08)	// 清除系统跳闸标志
#define LCD_STATUS_HOARD				;					// 显示囤积标志
#define LCD_STATUS_HOARD_CLR			;					// 清除囤积标志

#define SHOW_CHINESE_JINE				;						//金额

#define LCD_STATUS_COMMU_CR				(lcd[4] |= 0x08)	// 显示载波通讯标志
#define LCD_STATUS_COMMU_RF				(lcd[4] |= 0x04)	// 显示红外通讯标志
#define LCD_STATUS_COMMU_485			(lcd[4] |= 0x06)	// 显示485通讯标志1
#define LCD_STATUS_COMMU2_485			(lcd[4] |= 0x04);\
										(lcd[6] |= 0x01)	// 显示485通讯标志2
#define LCD_STATUS_COMMU_485_RF_CR_CLR	(lcd[4] &= ~0x0E);\
										(lcd[6] &= ~0x01)// 清除红外、485、载波通讯标志

#define LCD_STATUS_LOW_BATT				(lcd[15] |= 0x01)	// 显示电池1低电压
#define LCD_STATUS_LOW_BATT_CLR			(lcd[15] &= ~0x01)	// 清除电池1低电压
#define LCD_STATUS_LOW_BATT2			(lcd[17] |= 0x01)	// 显示电池2低电压
#define LCD_STATUS_LOW_BATT2_CLR		(lcd[17] &= ~0x01)	// 清除电池2低电压

#define LCD_STATUS_OP_POWER				(_NOP())			// 显示功率反向标志
#define LCD_STATUS_OP_POWER_CLR			(_NOP())			// 清除功率反向标志

#define LCD_STATUS_ALLOC_PRG			(lcd[9] |= 0x01)	// 显示允许编程标志
#define LCD_STATUS_ALLOC_PRG_CLR		(lcd[9] &= ~0x01)	// 清除允许编程标志
#define LCD_STATUS_SYS_LOCK				(lcd[10] |= 0x01)	// 显示系统锁定标志
#define LCD_STATUS_SYS_LOCK_CLR			(lcd[10] &= ~0x01)	// 清除系统锁定标志
#define LCD_STATUS_PUBLIC_KEY			(lcd[13] |= 0x01)	// 显示公匙状态标志
#define LCD_STATUS_PUBLIC_KEY_CLR		(lcd[13] &= ~0x01)	// 清除公匙状态标志
#define LCD_STATUS_ALARM				(lcd[14] |= 0x01)	// 显示告警状态标志
#define LCD_STATUS_ALARM_CLR			(lcd[14] &= ~0x01)	// 清除告警状态标志???

#define LCD_STATUS_SEG_CLR				(lcd[19] &= ~0x0C)	// 清除当前时段表号指示
#define LCD_STATUS_SEG_1				(lcd[19] |= 0x08)	// 当前运行在第一时段
#define LCD_STATUS_SEG_2				(lcd[19] |= 0x04)	// 当前运行在第二时段
//

#define LCD_DATA_RATIO_CLR				lcd[23] &= ~0x02;\
										lcd[22] &= ~0x08;\
										CLRDigTub(1)			// 清除下方“T ”及数码管显示和前面那个“1”

#define LCD_STATUS_RATIO_CLR			lcd[19] &= ~0x02;\
										lcd[20] &= ~0x10;\
										CLRDigTub(21)			// 清除所有费率显示

#define LCD_STATUS_GRADE_TABLE_CLR		(lcd[16] &= ~0x06);		// 清除当前阶梯电价方案指示
#define LCD_STATUS_GRADE_TABLE_1		(lcd[16] |= 0x04)		// 当前使用第一套阶梯电价
#define LCD_STATUS_GRADE_TABLE_2		(lcd[16] |= 0x02)		// 当前使用第二套阶梯电价

//当前运行象限
#define CURR_QUADRANT_CLR				(lcd[32] &= ~0x06);\
										(lcd[33] &= ~0x0E)
#define CURR_QUADRANT_AXES				(lcd[33] |= 0x04)
#define CURR_QUADRANT_I					(lcd[32] |= 0x02)
#define CURR_QUADRANT_II				(lcd[33] |= 0x02)
#define CURR_QUADRANT_III				(lcd[33] |= 0x08)
#define CURR_QUADRANT_IV				(lcd[32] |= 0x04)

//电压状态
#define SHOW_UA							(lcd[13] |= 0x02)
#define SHOW_UB							(lcd[13] |= 0x04)
#define SHOW_UC							(lcd[13] |= 0x08)
#define HIDE_UA							(lcd[13] &= ~0x02)
#define HIDE_UB							(lcd[13] &= ~0x04)
#define HIDE_UC							(lcd[13] &= ~0x08)
#define HIDE_U_ALL						(lcd[13] &= ~0x0E)

//电流状态及方向，P为正向，N为反向
#define SHOW_IA_P						(lcd[14] |= 0x04)
#define SHOW_IA_N						(lcd[14] |= 0x04);\
										(lcd[14] |= 0x08)
#define SHOW_IB_P						(lcd[15] |= 0x02)
#define SHOW_IB_N						(lcd[15] |= 0x02);\
										(lcd[14] |= 0x02)
#define SHOW_IC_P						(lcd[15] |= 0x08)
#define SHOW_IC_N						(lcd[15] |= 0x08);\
										(lcd[15] |= 0x04)
										
#define HIDE_IA							(lcd[14] &= ~0x0C);
#define HIDE_IB							(lcd[15] &= ~0x02);\
										(lcd[14] &= ~0x02)
#define HIDE_IC							(lcd[15] &= ~0x0C);
#define HIDE_I_ALL						(lcd[14] &= ~0x0E);\
										(lcd[15] &= ~0x0E)

//无线信号强度
#define SIGNAL_NO						(lcd[7] &= ~0x10);\
										(lcd[4] &= ~0xf0)
#define SIGNAL_INTENSITY_0				(lcd[7] |= 0x10)
#define SIGNAL_INTENSITY_1				(lcd[4] |= 0x10)
#define SIGNAL_INTENSITY_2				(lcd[4] |= 0x30)
#define SIGNAL_INTENSITY_3				(lcd[4] |= 0x70)
#define SIGNAL_INTENSITY_4				(lcd[4] |= 0xf0)

//数据显示区小数点
//1～7：	小数点
#define SET_POINT_1						(lcd[18] |= 0x80)	//P6
#define SET_POINT_2						(lcd[16] |= 0x08)	//P5
#define SET_POINT_3						(lcd[14] |= 0x80)	//P4
#define SET_POINT_4						(lcd[12] |= 0x08)	//P3
#define SET_POINT_5						(lcd[10] |= 0x80)	//P2
#define SET_POINT_6						(lcd[25] |= 0x80)	//P1
#define SET_POINT_7						;

//显示时间的格式(nnXX:XX:XX，时间，nn为空格)
#define SHOW_DOT_TIME					(lcd[16] |= 0x08);\
										(lcd[16] |= 0x80);\
										(lcd[12] |= 0x08);\
										(lcd[12] |= 0x80)

//显示日期的格式(nnXX.XX.XX，日期，n为空格)最大需量的时间(YY.MM.DD)
#define SHOW_DOT_DATE					(lcd[16] |= 0x08);\
										(lcd[12] |= 0x08)

//显示公共假日格式(YY.MM.DD:NN)
#define SHOW_DOT_HOLIDAY_TIME			(lcd[16] |= 0x80);\
										(lcd[16] |= 0x08);\
										(lcd[12] |= 0x08);\
										(lcd[25] |= 0x80)

//显示时区表(MM.DD.NN)
#define SHOW_DOT_AREA_ZONE				(lcd[16] |= 0x08);\
										(lcd[12] |= 0x08)

//显示当前运行时段及费率号 HH:mm.NN
#define SHOW_DOT_CUR_TIME_RATIO			(lcd[12] |= 0x80);\
										(lcd[16] |= 0x08);\
										(lcd[12] |= 0x08)

//索引码显示区小数点XX.XX.XX.XX
#define SHOW_INDEX_POINT				(lcd[3] |= 0x10);\
										(lcd[5] |= 0x10);\
										(lcd[8] |= 0x01)
//显示当前需量时分 HH:mm
#define SHOW_DOT_Demand					(lcd[16] |= 0x08);\
										(lcd[16] |= 0x80)
										
//单位显示，液晶不支持的定义为NULL
#define SHOW_UNIT_KWH					(lcd[22] |= 0xC0);\
										(lcd[21] |= 0x10)
#define SHOW_UNIT_KVARH					(lcd[21] |= 0xC0)
#define SHOW_UNIT_KW					(lcd[22] |= 0xC0)	//有功
#define SHOW_UNIT_KVAR					(lcd[21] |= 0x80)	//无功
#define SHOW_UNIT_VA					(lcd[22] |= 0x80);\
										(lcd[21] |= 0x20)//是在功率
#define SHOW_UNIT_V						(lcd[22] |= 0x80)
#define SHOW_UNIT_A						(lcd[21] |= 0x20)
#define SHOW_UNIT_KVAH					(lcd[22] |= 0x40);\
										(lcd[21] |= 0x30)
#define SHOW_UNIT_KVA					(lcd[22] |= 0x40);\
										(lcd[21] |= 0x20)
#define SHOW_UNIT_AH					(lcd[21] |= 0x30)
#define SHOW_UNIT_WAN					;
#define SHOW_UNIT_YUAN					(lcd[22] |= 0x20)
#define SHOW_UNIT_WANYUAN				;

//汉字提示象限
#define TEXT_QUADRANT_I					(lcd[24] |= 0x01)
#define TEXT_QUADRANT_II				(lcd[25] |= 0x01)
#define TEXT_QUADRANT_III				(lcd[24] |= 0x01);\
										(lcd[25] |= 0x01)
#define TEXT_QUADRANT_IV				(lcd[24] |= 0x03)
										

//汉字提示相别
#define TEXT_PAHSE_A					(lcd[32] |= 0x08)	// A
#define TEXT_PAHSE_B					(lcd[31] |= 0x10)	// B
#define TEXT_PAHSE_C					(lcd[29] |= 0x10)	// C
#define TEXT_PAHSE_N					(lcd[29] |= 0x08)	// N

//数据负号显示
#define SHOW_DATA_NEGATIVE				(lcd[33] |= 0x10)
// 汉字提示显示
#define SHOW_CHINESE_ZONG				(lcd[23] |= 0x01)	// 汉字‘总’

#define SHOW_CHINESE_DANGQIAN			(lcd[33] |= 0x01)	// ‘当前’
#define SHOW_CHINESE_SHANGYUE			(lcd[32] |= 0x01);\
										(lcd[29] |= 0x01) 	// ‘上月’
#define SHOW_CHINESE_YUE				(lcd[29] |= 0x01)	// ‘月’
#define SHOW_CHINESE_SHANGCI			(lcd[32] |= 0x01)	// ‘上’
//显示月次
#define SHOW_CHINESE_MON_NUM(Mon)		if(Mon>9)\
										{\
											lcd[31] |= 0x08;\
											Mon -= 10;\
										}\
										DigTubCtrlShow(Mon,0)

#define SHOW_CHINESE_ZUHE				(lcd[28] |= 0x01)	// ‘组合’
#define SHOW_CHINESE_FANXIANG			(lcd[28] |= 0x02);\
										(lcd[27] |= 0x02) 	// ‘反向’
#define SHOW_CHINESE_ZHENGXIANG			(lcd[27] |= 0x03)	// ‘正向’
#define SHOW_CHINESE_WUGONG				(lcd[26] |= 0x01);\
										(lcd[25] |= 0x02)	// ‘无功’
#define SHOW_CHINESE_YOUGONG			(lcd[26] |= 0x02);\
										(lcd[25] |= 0x02)	// ‘有功’
#define SHOW_CHINESE_SHENGYU			(lcd[28] |= 0x08)	// ‘剩余’
#define SHOW_CHINESE_COS				(lcd[29] |= 0x06)	// ‘COSΦ’
#define SHOW_CHINESE_ANGLE				(lcd[29] |= 0x02)	// ‘Φ’相角
#define SHOW_CHINESE_JIETI				(lcd[28] |= 0x04)	// ‘阶梯’


#define SHOW_CHINESE_XULIANG			(lcd[27] |= 0x14);	// ‘需量’
#define SHOW_CHINESE_DIANLIANG			(lcd[27] |= 0x0C)	// ‘电量’
#define SHOW_CHINESE_DIANFEI			(lcd[27] |= 0x08);\
										(lcd[26] |= 0x04)	// ‘电费’
#define SHOW_CHINESE_DIANJIA			(lcd[27] |= 0x08);\
										(lcd[26] |= 0x08)	// ‘电价’
#define SHOW_CHINESE_DIANYA				(lcd[27] |= 0x08);\
										(lcd[25] |= 0x04)	// ‘电压’
#define SHOW_CHINESE_DIANLIU			(lcd[27] |= 0x08);\
										(lcd[24] |= 0x08)	// ‘电流’
#define SHOW_CHINESE_SHIYA				(lcd[25] |= 0x0C)	// ‘失压’
#define SHOW_CHINESE_SHILIU				(lcd[25] |= 0x08);\
										(lcd[24] |= 0x08)	// ‘失流’
#define SHOW_CHINESE_GONGLV				(lcd[24] |= 0x04)	// ‘功率’
#define SHOW_CHINESE_SHIJIAN			(lcd[23] |= 0x0C)	// ‘时间’

#define LCD_STATUS_PHASE_REVERSE		(lcd[19] |= 0x01)	//显示逆相序状态标志
#define LCD_STATUS_PHASE_REVERSE_CLR	(lcd[19] &= ~0x01)	//清除逆相序状态标志

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
void WriteLcdBufToDriver( BYTE Length );

#endif//#if( LCD_TYPE == LCD_GW3PHASE_09 )
#endif//#ifndef

