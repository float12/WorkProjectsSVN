///////////////////////////////////////////////////////////////
//	文 件 名 :sysdef.h
//	文件功能 :系统级宏定义
//	作    者 : jiangjy
//	创建时间 : 2015年9月2日
//	项目名称 ：DF6203
//	操作系统 : 
//	备    注 :
//	历史记录： : 
///////////////////////////////////////////////////////////////
#ifndef __SYSDEFS_H__
#define __SYSDEFS_H__

#define MT_DF6203_JZQ_I		0xDF620301	//集中器I型
#define MT_DF6203_JZQ_II	0xDF620302	//集中器II型
#define MT_DF6203_FK_III	0xDF620303	//专变III型	

#define HT_DF6203_A			0x20620300  //I型集中器\III型负控 
#define HT_DF6203_B			0x20620301	//II型集中器 Q20平台
#define HT_DF6203_C			0x30620300  //I型集中器\III型负控 (ARM9平台)
#define HT_DF6203_D			0x20620302	//II型集中器 F407平台

#define MSYSGY_WEIHU		1		//维护规约
#define MD_GDW2013			2		//国网2013规范(protab.c中的规约号)
#define MD_CSG2013			3		//南网2013规范(protab.c中的规约号)
#define MGY_SDLT645_07		4		//标准从站645-2007规约
#define MD_GDW698			9		//面向对象的698规约
#define MSYSGY_TRANS		7		//透传规约
#define MSYSGY_YK			8		//遥控规约
#define MAX_SLAVEGY_NUM		9		//最大从规约个数
#define MGY_DLT645_07		15		//标准645-2007规约

#define LSM_NULL			0		//无液晶界面
#define LSM_STD_GDW_JZQ_I	1		//国网集中器标准界面
#define LSM_STD_GDW_FK_III	2		//国网负控标准界面

#define MJC_ATT7022			0x7022	//芯片ATT7022E
#define MJC_RN8302			0x8302	//芯片RN8302
#define MESAM_376			0x376	//376用
#define MESAM_698			0x698	//698用

#define NO_START_MET_PRO	10		//电表规约的起始协议号
//大类号
#define MET_KHB				1		//考核表
#define MET_DGN				2		//三相多功能表（工商用户）
#define MET_SINGLE			3		//单相表


#define IS_SAMPLE			1		//交流采样
#define IS_MET_KHB			2		//考核表
#define IS_MET_RS485		3		//RS485表
#define IS_MET_DGN			4		//三相用户多功能表
#define IS_MET_ZB			5	    //载波/无线用户
#define IS_MET_PAY			6		//预付费电表
#define IS_GRP				7		//总加组
#define IS_MET_VIP			8		//重点用户
#define IS_SAMPLE_STATIC	9		//表计统计数据
#define IS_MET_KHB_STATIC	10      //交采统计数据
#define IS_RTU_STATIC		11      //终端统计数据

#define	MT_YK				1
#define	MT_MEK				2
#define	MT_BEK				3
#define	MT_PDPK				4
#define	MT_BTPK				5
#define	MT_FRPK				6
#define	MT_TMPK				7

#define	IMAGIC_START		0x12345678
#define	IMAGIC_END			0x87654321

//以下变量需和上位机配置一致
#define MAR_STANDARD		1		//标准版本
#define MAR_SHAN_DONG		2   	//山东省
#define MAR_HE_NAN			3		//河南
#define MAR_SHAN_XI			4  		//山西
#define MAR_BEI_JING		5  		//北京
#define MAR_HU_BEI			6  		//湖北
#define MAR_HU_NAN			7  		//湖南
#define MAR_JIANG_SU		8  		//江苏
#define MAR_ZHE_JIANG		9  		//浙江
#define MAR_AN_HUI			10  	//安徽
#define MAR_JIANG_XI		11  	//江西
#define MAR_GUANG_DONG		12  	//广东
#define MAR_SI_CHUAN		13  	//四川
#define MAR_CHONG_QING		14  	//重庆
#define MAR_GAN_SU			15		//甘肃
#define MAR_NING_XIA		16		//宁夏
#define MAR_QING_HAI		17		//青海
#define MAR_JI_LIN			18		//吉林
#define MAR_HEI_LONG_JIANG	19		//黑龙江
#define MAR_JI_BEI			20		//冀北
#define MAR_FU_JIAN			21		//福建
#define MAR_XB_SHAN_XI      22      //陕西

//一、规约接口级数据类型定义
//数据类型定义(最大可到31)
#define	MTP_AI				1 		//遥测（瞬时量）
#define	MTP_CI				2 		//电度 (电量)
#define MTP_STA				3 		//统计数据
#define MTP_MM				4 		//极值
#define	MTP_CJTIME			5		//数据采集时间
#define	MTP_XL				6 		//需量
#define	MTP_XB				7		//谐波
#define MTP_PARACJ			8 		//参数参数时间
#define MTP_DX				9 		//断相
#define	MTP_UCI				10 		//用电度(用电量)
#define MTP_TDREC			11 		//停电记录
#define MTP_PAYINFO			12 		//购用电信息
#define MTP_PAYCJTIME		13		//预付费采集时间
#define MTP_MULTI			14      //多表集抄数据
#define DATATYPE(x)			(x&0x1F)//冻结的数据类型
//冻结类型(最大可到7)
#define MTP_REAL			0		//实时
#define MTP_CURV			1		//曲线
#define MTP_DAYFRZ			2		//日冻结
#define MTP_DAYFRZ_CB		3		//抄表日冻结
#define MTP_MONFRZ			4		//月冻结
#define FRZTYPE(x)			((x>>5)&0x7)//冻结类型	

#define GetDataType(x,y)	(x | (y<<5))

//二、数据库级数据类型定义
//数据类型定义(最大可到31)
#define DBDATATYPE(x)		(x&0x1F)//数据库结构类型(见具体数据库冻结结构)	
//冻结类型(最大可到7)
#define MTP_SYS				1		//系统
#define MTP_MP				2		//测量点
#define MTP_GRP				3		//总加组
#define DBASETYPE(x)		((x>>5)&0x7)//冻结类型	

//接线方式(三相三线，三相四线)
#define WIRE_33				0x33
#define WIRE_34				0x34
#define WIRE_S				0x11
#define WIRE_BAK			0xFF

//正序升级
#define UPDATE_DATA_POS_SEQ	0x5A

//测量点的类型
#define	MP_METER			(1<<0)
#define	MP_SAMPLE			(1<<1)
#define	MP_PULSE			(1<<2)
#define	MP_ANALOG			(1<<3)
#define	MP_CALCUL			(1<<4)
#endif		
		
		
		