//----------------------------------------------------------------------
//Copyright (C) 2016-2026 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	胡春华
//创建日期	2016.8.9
//描述		自由事件异常事件Api头文件
//修改记录	
//----------------------------------------------------------------------
#ifndef __SYSEVENT_API_H
#define __SYSEVENT_API_H

//-----------------------------------------------
//				宏定义
//-----------------------------------------------
//自由时间记录次数
#define MAX_FREE_EVENT      128
//最大记录的自由异常事件
#define MAX_SYS_FREE_MSG	256
//最大记录的系统异常信息
#define MAX_SYS_UN_MSG		64



// 自由事件记录
//系统启动事件
#define EVENT_SYS_START					1
//系统掉电，进入低功耗
#define EVENT_ENTER_LOW_POWER			2
//系统上电电压事件
#define EVENT_SYS_START_VOLTAGE			3

//系统进行了初始化操作
//清电能操作成功
//#define EVENT_CLR_ENERGY_SUCCEED		4
//清电能操作失败
//#define EVENT_CLR_ENERGY_FALSE		5
//有清需量的操作
//#define EVENT_CLR_DEMAND				6
//有修改密码的操作
#define EVENT_CHG_PSW					7
//有修改循显操作
//#define EVENT_SET_LOOP_DISPLAY		8
//有修改键显操作
//#define EVENT_SET_KEY_DISPLAY			9
//有精度修正的操作
#define EVENT_PRECISION_CORRECT			10
//清最大最小电压
//#define EVENT_CLR_MAXMIN_VOL			11
//清最大最小功率因数
//#define EVENT_CLR_MAXMIN_COS			12
//设置功率因数上下限
//#define EVENT_COS_LEVEL				13
//清功率因数合格率
//#define EVENT_CLR_COS_TIME			14
//清过压记录
//#define EVENT_CLR_OVER_VOL			15
//清电流不平衡率
//#define EVENT_CLR_CURRENT_UN			16
//写入时区数、时段表数、费率数、日时段数、公共假日数
//#define EVENT_TIMETABLE_1				17
//写入时区描述
//#define EVENT_TIMETABLE_2				18
//写入时段表描述
//#define EVENT_TIMETABLE_3				19
//写入周修日时段表号
//#define EVENT_WEEKEND_NO				20
//写入公共假日、周修日时段表号
//#define EVENT_HOLIDAY_STATUS			21
//写入其他设置，包括需量控制、LCD参数、自动抄表日、符合代表日等。
//#define EVENT_OTHER_SET				22
//写周修日状态字
//#define EVENT_WEEKEND_STATUS			23
//写入表号
//#define EVENT_METER_NO_ONLY			24
//写入表号、用户号、设备号
//#define EVENT_METER_NO				25
//写电压合格界限
//#define EVENT_WRITE_VOL_LIMIT			26
//写入无功控制方式、过压界限、电压零点、电流不平衡
//#define EVENT_WRITE_OTHER_CONTROL		27
//有整定直流电压偏移系数的操作
#define EVENT_ADJUST_DC_VOL_OFF			28
//有整定直流电流偏移系数的操作
#define EVENT_ADJUST_DC_CUR_OFF			29
//有整定交流电压偏移系数的操作
#define EVENT_ADJUST_AC_VOL_OFF			30
//有整定交流电流偏移系数的操作
#define EVENT_ADJUST_AC_CUR_OFF			31
//有整定电压增益的操作
#define EVENT_ADJUST_VOL_GAIN			32
//有整定电流增益的操作
#define EVENT_ADJUST_CUR_GAIN			33
//有电压微调的操作
#define EVENT_ADJUST_VOL_SMALL			34
//有整定相角的操作
#define EVENT_ADJUST_ANGLE				35
//清电池时间
//#define EVENT_CLEAR_BATTERY_TIME		36
//清电压合格率
//#define EVENT_CLR_VOL_TIME			37
//清断相记录
//#define EVENT_CLR_BREAK				38
//清失压记录
//#define EVENT_CLR_LOST_V				39
//系统初始化开始
#define EVENT_SYS_INIT_BEGIN			40

//规约编程事件记录
//规约中标准编程（符合标准规约）
//#define EVENT_PROTOCOL_STD_WRITE		41
//规约中编程更改波特率
//#define EVENT_PROTOCOL_CHG_BPS		42
//规约中编程修改密码
#define EVENT_PROTOCOL_CHG_PSW			43
//规约中编程清电能
//#define EVENT_PROTOCOL_CLR_ENG		44
//规约中编程清断相记录
//#define EVENT_PROTOCOL_CLR_BREAK		45
//规约中编程设置循显
//#define EVENT_PROTOCOL_SET_LOOP_DIS	46
//规约中编程设置键显
//#define EVENT_PROTOCOL_SET_KEY_DIS	47
//规约中编程精度修正
#define EVENT_PROTOCOL_PRECISION		48
//规约中编程设置系统控制位
#define EVENT_PROTOCOL_SET_SYS_BIT		49
//清失流记录
//#define EVENT_CLR_LOST_I				50
//写广播时间成功
#define EVENT_BROADCAST_WRITE_TIME		51
//写时钟芯片
#define EVENT_WRITE_HARD_TIME			52//0x34
//命令复位
#define EVENT_COMMAND_RESET				53
//异常复位（非上电复位）
#define EVENT_UNNORMAL_RESET			54
//设置通信地址 2010-2-24
#define EVENT_SET_COMM_ADDR				55
//有远程报警操作
#define EVENT_SET_WARN_RELAY			56
//有解除远程报警操作
#define EVENT_CLR_WARN_RELAY			57
//有保电操作
#define EVENT_SET_SAFETY				58
//有解除保电操作
#define EVENT_CLR_SAFETY				59
//MAC错误超，挂起
#define EVENT_MAC_ERR_LOCK				60
//上电从esam中恢复表号,密钥状态
#define EVENT_RESTORE_INFO_FROM_ESAM	61
//打开厂内模式
#define EVENT_OPEN_FAC_MODE				62
//实际设置费率数小于时段费率数
#define EVENT_TIME_TABLE_NUM_ERR		63
//电表密码错误锁定
#define EVENT_PROGRAMKEY_LOCK			64

//698有参数设置操作
#define EVENT_PROGRAM_698				65//0x41
//698有方法操作
#define EVENT_METHOD_698				66//0x42
//698有数据初始化操作
#define EVENT_DATA_INIT_698				67//0x43
//698有恢复出厂参数操作
#define EVENT_PARA_INIT_698				68//0x44
//698有清事件操作
#define EVENT_EVENT_INIT_698			69//0x45
//698有清需量操作
#define EVENT_DEMAND_INIT_698			70//0x46
//有校表操作
#define EVENT_ADJUST_METER				71//0x47
//有设置寄存器和校表系数操作
#define EVENT_WRITE_REG					72//0x48
//厂内规约扩展写操作
#define EVENT_FACTORY_WRITE				73//0x49
//远程继电器操作失败
#define EVENT_RELAY_ERR					74//0x4A

//系统异常信息记录
//最高位为 00(0)		为SetError存储的异常事件  序号为程序中定义的错误号，和显示一致
//最高位为 01(4)		保留
//最高位为 10(8)		普通的异常事件
//最高位为 11(c)		为断言存储的异常事件  序号为断言所在的行号

//有修复时区时段表的操作
#define SYSUN_FIX_TIME_TABLE			0x8001
//有从EEPROM1中恢复参数区的操作
#define SYSUN_PARA_READ_EEPROM1			0x8002
//有从EEPROM2中恢复参数区的操作
#define SYSUN_PARA_READ_EEPROM2			0x8003
//使用了电能备份区（第一次读电能不成功）
#define SYSUN_USE_ENERGY_BACK			0x8004
//转存写入时，检查到RAM被破坏
#define SYSUN_SWAP_BUF_ERR				0x8005
//转存时写入第一备份区错误
#define SYSUN_SWAP_PART1_1				0x8006
//转存时写入第一备份区错误
#define SYSUN_SWAP_PART1_2				0x8007
//转存电能前检查到RAM数据库被破坏
#define SYSUN_SWAP_E_RAMDBASE_ERR		0x8008
//转存前检查到EEPROM数据库损坏，不转存
#define SYSUN_SWAP_EEPROM_ERR			0x8009
//转存时检查到暂存RAM被破坏（重试）
#define SYSUN_SWAP_MID_RAM_ERR			0x800a
//转存前检查到5460错误。
#define SYSUN_SWAP_5460_ERR				0x800b
//写入EEPROM前检测到电能逻辑保护错误
#define SYSUN_EEPROM_PROTECT_11			0x800c
//写入EEPROM前检测到关键参数逻辑保护错误
#define SYSUN_EEPROM_PROTECT_33			0x800d
//写入EEPROM时错误重试
#define SYSUN_EEPROM_REPET_WRITE		0x800e
//写入FLASH时错误重试
#define SYSUN_FALSE_REPET_ERR			0x800f
//启动时读EEPROM中余数错误
#define SYSUN_READ_REMANDER_ERR			0x8010
//矫正系数时，电压微调超出范围
#define SYSUN_ADJUST_VOL				0x8011
//转存需量前检查到RAM数据库被破坏
#define SYSUN_SWAP_D_RAMDBASE_ERR		0x8012
//电能正确性检测时，检测到FSTA2
#define SYSUN_CHECK_ENERGY_FSTA2		0x8013
//修改过5460参数
#define MODIFIED_5460_PARA				0x8014
//手动修正过增益和角度
//#define MANUAL_MODIFIED_PARA			0x8015
//修复过第一片的5460参数
#define REPAIRED_FIRST_5460_PARA		0x8016
//修复过第二片的5460参数
#define REPAIRED_SECOND_5460_PARA		0x8017
//7022内部参数校表和改变了
#define CHECKSUM_CHANGE_7022			0x8018
//写数据超出连续空间地址
#define OVERWRITE_CONTINUE_SPACE		0x8019
//内存分配指针和备份的不一致
#define ALLOCBUF_POINT_ERROR			0x801a
//初始化采样芯片失败过
#define INIT_SAMPLE_CHIP_FAIL			0x801b
//两次读7026E电能时出现丢弃脉冲情况
#define EVENT_7026E_PULS_ERR			0x8021
//规约层接收到校表命令，只用来进行记录以供日后问题排查
#define EVENT_CALIBRATE					0x8022
//校准参数错误
#define SAMPLE_CALI_PARA_ERR			0x8023
//电能检查发现RAM校验错误
#define ENERGY_CHECK_RAM_ERR			0x8024
//检查到内存电能超门限清零
#define WATCH_ENERGY_OVERRIDE_ERR		0x8025
//存在非正常转存操作
#define WATCH_ENERGY_SWAP_ERR			0x8026
//ESAM第一次复位失败
#define ESAM_FIRST_RESET_ERR			0x8027
//写入EEPROM前检测到金额逻辑保护错误
#define SYSUN_EEPROM_PROTECT_44			0x8028
//检查到内存金额校验错误
#define  WATCH_MONEY_RAM_ERR			0x8029
//发生金额从ESAM恢复事件
#define SYSUN_FIX_MONEY					0x802A
//发生上电修改RTC时间
#define SYSUN_POWERON_FIX_TIME			0x802B
//发生切换时间CRC校验错误的问题
#define SWITCH_TIME_CRC_ERR				0x802C
//冻结数据CRC校验错误
#define GET_FREEZE_RECORD_ERR			0x802D
//sysytick 不运行
#define SYSERR_SYSTICK_ERR				0x802E
//看门狗复位
#define SYSERR_WDTRST_ERR				0x802F
//电能EE通过FLASH恢复
#define SYSUN_WRITE_ENERGY_EE			0x8030
//冻结EE通过FLASH恢复
#define SYSUN_WRITE_FREEZE_EE			0x8031
//tf卡缓冲区满，波形数据丢失
#define SYSUN_TF_BUF_FULL				0x8032
//收波形数据crc错误
#define SYSUN_WAVE_DATA1_CRC_ERR			0x8033
#define SYSUN_WAVE_DATA2_CRC_ERR			0x8034
#define SYSUN_WAVE_DATA3_CRC_ERR			0x8035
#define SYSUN_WAVE_DATA4_CRC_ERR			0x8036

//写采样芯片寄存器8100+寄存器地址(一个字节)1
#define MANUAL_MODIFIED_PARA			0x8100
//flash写入错误
#define SYSUN_FALSE_WRITE_ERR			0x2000 //bit0-bit13记录扇区数
//ESAM获取随机数失败
#define ESAM_GET_RN_ERR				    0x8800
//ESAM验证MAC失败
#define ESAM_VERIFY_MAC_ERR				0x8801
//生成发现未知电能表事件过程异常
#define FIND_UNKNOWN_METER_EVENT_ERR    0x8900

//hardfault或者NMI导致中断不响应
#define SYSERR_SCB_INFO					0x4000 //bit0-bit11记录异常信息
//CR超时
#define SYSERR_CRTIME_ERR				0x8CFF
//assert异常事件 改为0xc000+行号
#define ASSERT_EVENT					0xc000


//-----------------------------------------------
//				结构体，共用体，枚举
//-----------------------------------------------
//自由事件记录格式
typedef struct TFreeEvent_t
{
	//事件类型
	WORD EventType;
	//附加记录
	WORD SubEvent;
	//发生时间，要记录到年：月：日：时：分：秒（5、4、3、2、1、0）
	BYTE EventTime[6];

}TFreeEvent;

//事件记录，自由事件记录
typedef struct TEventSave_t
{
	WORD FreeEventPtr;
	//缓冲
	TFreeEvent FreeEvent[MAX_FREE_EVENT];
}TSysFreeEventConRom;

//异常事件记录格式
typedef struct TSysUNMsg_t
{
	//事件类型
	WORD EventType;
	//发生时间，要记录到年：月：日：时：分：秒（5、4、3、2、1、0）
	BYTE EventTime[6];

}TSysUNMsg;

typedef struct TSaveSysMsg_t
{
	WORD SysUnMsgPoint;
	TSysUNMsg SysUNMsg[MAX_SYS_UN_MSG];
}TSysAbrEventConRom;

//-----------------------------------------------
//				变量声明
//-----------------------------------------------
extern WORD		SysFREEMsgCounter;		//一天自由事件记录记录最大次数
extern BYTE		SysUNMsgCounter;		//一天异常事件记录记录最大次数
//-----------------------------------------------
// 				函数声明
//-----------------------------------------------
//-----------------------------------------------
//函数功能: 记录自由事件
//
//参数: 
//			EventType[in]		自由事件类型
//        	EventSub[in]		附属数据
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_WriteFreeEvent(WORD EventType, WORD EventSub);


//-----------------------------------------------
//函数功能: 读自由事件
//
//参数: 
//			Num[in]		上Num次自由事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	无
//
//备注:   
//-----------------------------------------------
WORD api_ReadFreeEvent(WORD Num,BYTE *Buf);

//-----------------------------------------------
//函数功能: 记录异常事件
//
//参数: 
//			Msg[in]		异常事件类型
//                    
//返回值:  	无
//
//备注:   
//-----------------------------------------------
void api_WriteSysUNMsg(WORD Msg);


//-----------------------------------------------
//函数功能: 读异常事件
//
//参数: 
//			Num[in]		上Num次异常事件记录
//          Buf[out]	输出缓冲	          
//返回值:  	无
//
//备注:   
//-----------------------------------------------
WORD api_ReadSysUNMsg(WORD Num,BYTE *Buf);


#endif//#ifndef __SYSEVENT_API_H

