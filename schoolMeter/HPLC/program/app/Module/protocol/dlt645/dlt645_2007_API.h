//------------------------------------------------------------------------------------------
//	��    �ƣ�dlt645_2007_API.h
//	��    ��:  DL/T645-2007 ��չ�ṹ�����ļ�
//	��    ��: ���ĺ�
//	����ʱ��:  2007-11-6 19:54
//	����ʱ��:
//	��    ע:	���ѽṹ�����
//	�޸ļ�¼:
//------------------------------------------------------------------------------------------
#ifndef __Dlt645_2007_API_H
#define __Dlt645_2007_API_H

#if( SEL_DLT645_2007 == YES )


//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define MAX_SIZE_FOR_IAP						(256 * 1024)									// ���������Ŀռ�  
#define NUM_OF_SECTOR_FOR_IAP					128												// һ������4096�ֽڣ���Ԥ��512K ��������
#define LEN_OF_ONE_FRAME						192												// һ֡���ĵĳ���
#define FRAMENUM_OF_ONE_SECTOR					(SECTOR_SIZE / LEN_OF_ONE_FRAME)				//һ�������ܹ���ı����� 21
#define FRAMESIZE_OF_ONE_SECTOR					(FRAMENUM_OF_ONE_SECTOR * LEN_OF_ONE_FRAME) 	//һ�������ܹ����ֽ���
#define SECTOR_NUM_FOR_IAP						((MAX_SIZE_FOR_IAP / FRAMESIZE_OF_ONE_SECTOR)+1)// ��MAX_SIZE_FOR_IAP���������ݣ���Ҫ���ٸ�����
#define IAP_CODE_ADDR							((2048 - SECTOR_NUM_FOR_IAP) * SECTOR_SIZE)		// ������ 0~2047
#define IAP_INFO_ADDR							((2048 - SECTOR_NUM_FOR_IAP- 1) * SECTOR_SIZE )	// ������ 0~2047 ���������Ϣ
#define MAX_FRAME_NUM							(MAX_SIZE_FOR_IAP / LEN_OF_ONE_FRAME)			// ֡������ֵ
#define IAP_FLAG								0xA55AA55A


//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------
typedef struct
{
	DWORD dwFileSize; //�����ļ���С��Byte��
	WORD wAllFrameNo; //�����ļ��ָ�����֡��
	WORD wCrc16;	  //���������ļ���CRC16У����
	DWORD dwIapFlag;  //������־λ  0xA55AA55A ��Ч��������Ч
	DWORD dwCrc32;	  //�ṹ��У��
} T_Iap;

#pragma pack(1)
typedef struct TResetDelayPara_t
{
	BYTE	ResetDelay;//��ʱ����ʱ��(s)
	DWORD	CRC32;
}TResetDelayPara;
#pragma pack()
//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------
extern TResetDelayPara ResetDelayPara;
//-----------------------------------------------

//-----------------------------------------------
//				���ļ�ʹ�õı���������


//-----------------------------------------------
//				api��������
//-----------------------------------------------
//--------------------------------------------------
//��������:�Ͽ������֤
//         
//����  :   ��
//
//����ֵ:    ��   
//         
//��ע����:  ��
//--------------------------------------------------
void api_Release645_Auth( void );
//--------------------------------------------------
//��������:  ����CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
//--------------------------------------------------
unsigned short CalCRC16( unsigned char *puchMsg, unsigned short usDataLen );
//--------------------------------------------------
//��������:  ���������ļ���CRC16У��
//
//����:
//
//����ֵ:    CRC16У��
//
//��ע����:
//--------------------------------------------------
unsigned short FileCalCRC16(unsigned char *puchMsg, unsigned short usDataLen);


#endif//#if( SEL_DLT645_2007 == YES )

#endif//��Ӧ�ļ���ʼ�� __DLT_645_2007_STRUCT



