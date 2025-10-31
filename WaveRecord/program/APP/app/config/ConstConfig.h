#ifndef __CONST_CONFIG_H
#define __CONST_CONFIG_H

/*****************************************************************************/
/*文件名：config.h
 *Copyright (c) 2003 烟台东方威思顿电气有限公司计量产品开发组
 *All rights reserved
 *
 *创建人：胡春华
 *日  期：2009-6-2
 *修改人：魏灵坤
 *日  期：2009-6-2
 *描  述：列出不常用的宏定义，减少 sys.h 中宏定义
 *
 *版  本：
 */
/*****************************************************************************/




//保护区常数声明
extern const Fun FunctionConst;
extern const BYTE MeterTypesConst;
extern const BYTE MeterCurrentTypesConst;
extern const BYTE SelSecPowerConst;
extern const BYTE SelOscTypeConst;
extern const BYTE RelayTypeConst;
extern const BYTE SpecialClientsConst;
extern const BYTE SelDivAdjust;
extern const BYTE SelMChipVrefgain;
extern const BYTE SelEESoftAddrConst;
extern const BYTE SelEEDoubleErrReportConst;
extern const BYTE MeterVoltageConst;
extern const BYTE SmallCurrentCorrection;

//extern const BYTE SelSwitchPowerSupplyConst;
extern const BYTE SelVolCorrectConst;
extern const BYTE SelVolUnbalanceGain;
extern const WORD SelThreeBoard;
extern const BYTE a_byDefaultData[];
extern const BYTE ESAMPowerControlConst;
extern const BYTE SelZeroCurrentConst;//零线电流检测
extern const BYTE ZeroCTPositive;//单相零线电流互感器正反接开关

extern const WORD SoftVersionConst;
extern const BYTE WATCH_SCI_TIMER;// 考虑手动组包读后续数据 wlk 2009-7-16
//接受超时时间，单位，毫秒，也就是在100毫秒后若没有接受到新的数据，则清接受缓冲
extern const WORD MAX_RX_OVER_TIME;
extern const WORD RECE_TO_SEND_DELAY;

#endif//#ifndef __CONST_CONFIG_H


