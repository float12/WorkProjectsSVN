#ifndef __CONST_CONFIG_H
#define __CONST_CONFIG_H

/*****************************************************************************/
/*�ļ�����config.h
 *Copyright (c) 2003 ��̨������˼�ٵ������޹�˾������Ʒ������
 *All rights reserved
 *
 *�����ˣ�������
 *��  �ڣ�2009-6-2
 *�޸��ˣ�κ����
 *��  �ڣ�2009-6-2
 *��  �����г������õĺ궨�壬���� sys.h �к궨��
 *
 *��  ����
 */
/*****************************************************************************/




//��������������
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
extern const BYTE SelZeroCurrentConst;//���ߵ������
extern const BYTE ZeroCTPositive;//�������ߵ��������������ӿ���

extern const WORD SoftVersionConst;
extern const BYTE WATCH_SCI_TIMER;// �����ֶ�������������� wlk 2009-7-16
//���ܳ�ʱʱ�䣬��λ�����룬Ҳ������100�������û�н��ܵ��µ����ݣ�������ܻ���
extern const WORD MAX_RX_OVER_TIME;
extern const WORD RECE_TO_SEND_DELAY;

#endif//#ifndef __CONST_CONFIG_H


