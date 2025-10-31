//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.10
//����		at45db161��at45db321��ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __AT45DB161_H
#define __AT45DB161_H

#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//�������д�����Դ���
#define FLASH_MAX_WRITE_COUNT	2
//һ��������С
#define AT45DB161B_PAGE_SIZE	528

//Continuous Array Read
#define AT45DB161_CMD_CAR		0xe8
//Main Memory Page Read
#define AT45DB161_CMD_MMPR		0xd2
//Buffer 1 Read
#define AT45DB161_CMD_B1R		0xd4
//Buffer 2 Read
#define AT45DB161_CMD_B2R		0xd6
//Status Register Read
#define AT45DB161_CMD_SRR		0xd7

//Buffer 1 Write
#define AT45DB161_CMD_B1W		0x84
//Buffer 2 Write
#define AT45DB161_CMD_B2W		0x87
//Buffer 1 to Main Memory Page Program with Built-in Erase
#define AT45DB161_CMD_B1MPBE	0x83
//Buffer 2 to Main Memory Page Program with Built-in Erase
#define AT45DB161_CMD_B2MPBE	0x86
//Buffer 1 to Main Memory Page Program without Built-in Erase
#define AT45DB161_CMD_B1MPOBE	0x88
//Buffer 2 to Main Memory Page Program without Built-in Erase
#define AT45DB161_CMD_B2MPOBE	0x89
//Page Erase
#define AT45DB161_CMD_PE		0x81
//Block Erase
#define AT45DB161_CMD_BE		0x50
//Main Memory Page Prgram through Buffer 1
#define AT45DB161_CMD_MPTB1		0x82
//Main Memory Page Prgram through Buffer 2
#define AT45DB161_CMD_MPTB2		0x85

//Main Memory Page to Buffer 1 Transter
#define AT45DB161_CMD_MPB1T		0x53
//Main Memory Page to Buffer 2 Transter
#define AT45DB161_CMD_MPB2T		0x55
//Main Memory Page to Buffer 1 compare
#define AT45DB161_CMD_MPB1C		0x60
//Main Memory Page to Buffer 2 compare
#define AT45DB161_CMD_MPB2C		0x61
//Auto Page Rewrite through Buffer 1
#define AT45DB161_CMD_APRB1		0x58
//Auto Page Rewrite through Buffer 2
#define AT45DB161_CMD_APRB2		0x59

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
// 				��������
//-----------------------------------------------
//-----------------------------------------------
//��������: ��FlashоƬ�ڶ�ȡ���ݣ�ģ���ں���
//
//����: 
//			No[in]			оƬ��ţ�����Ƭѡѡ��
//			Addr[in]		�����ݵ���ʼ��ַ
//			Len[in]			��������
//			pBuf[in/out]	�������ݴ洢����
//                    
//����ֵ:  	TRUE:��ȷ����   FALSE:��������  ʵ��û���жϣ�����������ȷ
//
//��ע:   
//-----------------------------------------------
BOOL ReadExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);

//-----------------------------------------------
//��������: дFlash������ģ���ڲ�����
//
//����: 	
//			No[in]			д��оƬ���
//			Addr[in]		д�����ݵ���ʼ��ַ
//			Len[in]			д�����ݳ���
//			pBuf[in]		�洢Ҫд�������
//
//����ֵ:  	TRUE:�ɹ�		FALSE��ʧ��
//
//��ע:
//-----------------------------------------------
BOOL WriteExtFlash(WORD No, DWORD Addr, WORD Len, BYTE * pBuf);

//-----------------------------------------------
//��������: ��FlashоƬ���жϵ�ǿ�Ƹ�λ��ģ���ں���
//
//����: 	
//          Type	��λ����  0���ϵ�     ���������ϵ���
//          
//����ֵ:  	��
//
//��ע:
//-----------------------------------------------
void PowerUpResetExtFlash( BYTE Type );


#endif//#if( (THIRD_MEM_CHIP == CHIP_AT45DB161) || (THIRD_MEM_CHIP == CHIP_AT45DB321) )

#endif//#ifndef __AT45DB161_H


