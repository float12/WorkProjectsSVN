///////////////////////////////////////////////////////////////
//	�� �� �� :sysdef.h
//	�ļ����� :ϵͳ���궨��
//	��    �� : jiangjy
//	����ʱ�� : 2015��9��2��
//	��Ŀ���� ��DF6203
//	����ϵͳ : 
//	��    ע :
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#ifndef __SYSDEFS_H__
#define __SYSDEFS_H__

#define MT_DF6203_JZQ_I		0xDF620301	//������I��
#define MT_DF6203_JZQ_II	0xDF620302	//������II��
#define MT_DF6203_FK_III	0xDF620303	//ר��III��	

#define HT_DF6203_A			0x20620300  //I�ͼ�����\III�͸��� 
#define HT_DF6203_B			0x20620301	//II�ͼ����� Q20ƽ̨
#define HT_DF6203_C			0x30620300  //I�ͼ�����\III�͸��� (ARM9ƽ̨)
#define HT_DF6203_D			0x20620302	//II�ͼ����� F407ƽ̨

#define MSYSGY_WEIHU		1		//ά����Լ
#define MD_GDW2013			2		//����2013�淶(protab.c�еĹ�Լ��)
#define MD_CSG2013			3		//����2013�淶(protab.c�еĹ�Լ��)
#define MGY_SDLT645_07		4		//��׼��վ645-2007��Լ
#define MD_GDW698			9		//��������698��Լ
#define MSYSGY_TRANS		7		//͸����Լ
#define MSYSGY_YK			8		//ң�ع�Լ
#define MAX_SLAVEGY_NUM		9		//���ӹ�Լ����
#define MGY_DLT645_07		15		//��׼645-2007��Լ

#define LSM_NULL			0		//��Һ������
#define LSM_STD_GDW_JZQ_I	1		//������������׼����
#define LSM_STD_GDW_FK_III	2		//�������ر�׼����

#define MJC_ATT7022			0x7022	//оƬATT7022E
#define MJC_RN8302			0x8302	//оƬRN8302
#define MESAM_376			0x376	//376��
#define MESAM_698			0x698	//698��

#define NO_START_MET_PRO	10		//����Լ����ʼЭ���
//�����
#define MET_KHB				1		//���˱�
#define MET_DGN				2		//����๦�ܱ������û���
#define MET_SINGLE			3		//�����


#define IS_SAMPLE			1		//��������
#define IS_MET_KHB			2		//���˱�
#define IS_MET_RS485		3		//RS485��
#define IS_MET_DGN			4		//�����û��๦�ܱ�
#define IS_MET_ZB			5	    //�ز�/�����û�
#define IS_MET_PAY			6		//Ԥ���ѵ��
#define IS_GRP				7		//�ܼ���
#define IS_MET_VIP			8		//�ص��û�
#define IS_SAMPLE_STATIC	9		//���ͳ������
#define IS_MET_KHB_STATIC	10      //����ͳ������
#define IS_RTU_STATIC		11      //�ն�ͳ������

#define	MT_YK				1
#define	MT_MEK				2
#define	MT_BEK				3
#define	MT_PDPK				4
#define	MT_BTPK				5
#define	MT_FRPK				6
#define	MT_TMPK				7

#define	IMAGIC_START		0x12345678
#define	IMAGIC_END			0x87654321

//���±��������λ������һ��
#define MAR_STANDARD		1		//��׼�汾
#define MAR_SHAN_DONG		2   	//ɽ��ʡ
#define MAR_HE_NAN			3		//����
#define MAR_SHAN_XI			4  		//ɽ��
#define MAR_BEI_JING		5  		//����
#define MAR_HU_BEI			6  		//����
#define MAR_HU_NAN			7  		//����
#define MAR_JIANG_SU		8  		//����
#define MAR_ZHE_JIANG		9  		//�㽭
#define MAR_AN_HUI			10  	//����
#define MAR_JIANG_XI		11  	//����
#define MAR_GUANG_DONG		12  	//�㶫
#define MAR_SI_CHUAN		13  	//�Ĵ�
#define MAR_CHONG_QING		14  	//����
#define MAR_GAN_SU			15		//����
#define MAR_NING_XIA		16		//����
#define MAR_QING_HAI		17		//�ຣ
#define MAR_JI_LIN			18		//����
#define MAR_HEI_LONG_JIANG	19		//������
#define MAR_JI_BEI			20		//����
#define MAR_FU_JIAN			21		//����
#define MAR_XB_SHAN_XI      22      //����

//һ����Լ�ӿڼ��������Ͷ���
//�������Ͷ���(���ɵ�31)
#define	MTP_AI				1 		//ң�⣨˲ʱ����
#define	MTP_CI				2 		//��� (����)
#define MTP_STA				3 		//ͳ������
#define MTP_MM				4 		//��ֵ
#define	MTP_CJTIME			5		//���ݲɼ�ʱ��
#define	MTP_XL				6 		//����
#define	MTP_XB				7		//г��
#define MTP_PARACJ			8 		//��������ʱ��
#define MTP_DX				9 		//����
#define	MTP_UCI				10 		//�õ��(�õ���)
#define MTP_TDREC			11 		//ͣ���¼
#define MTP_PAYINFO			12 		//���õ���Ϣ
#define MTP_PAYCJTIME		13		//Ԥ���Ѳɼ�ʱ��
#define MTP_MULTI			14      //���������
#define DATATYPE(x)			(x&0x1F)//�������������
//��������(���ɵ�7)
#define MTP_REAL			0		//ʵʱ
#define MTP_CURV			1		//����
#define MTP_DAYFRZ			2		//�ն���
#define MTP_DAYFRZ_CB		3		//�����ն���
#define MTP_MONFRZ			4		//�¶���
#define FRZTYPE(x)			((x>>5)&0x7)//��������	

#define GetDataType(x,y)	(x | (y<<5))

//�������ݿ⼶�������Ͷ���
//�������Ͷ���(���ɵ�31)
#define DBDATATYPE(x)		(x&0x1F)//���ݿ�ṹ����(���������ݿⶳ��ṹ)	
//��������(���ɵ�7)
#define MTP_SYS				1		//ϵͳ
#define MTP_MP				2		//������
#define MTP_GRP				3		//�ܼ���
#define DBASETYPE(x)		((x>>5)&0x7)//��������	

//���߷�ʽ(�������ߣ���������)
#define WIRE_33				0x33
#define WIRE_34				0x34
#define WIRE_S				0x11
#define WIRE_BAK			0xFF

//��������
#define UPDATE_DATA_POS_SEQ	0x5A

//�����������
#define	MP_METER			(1<<0)
#define	MP_SAMPLE			(1<<1)
#define	MP_PULSE			(1<<2)
#define	MP_ANALOG			(1<<3)
#define	MP_CALCUL			(1<<4)
#endif		
		
		
		