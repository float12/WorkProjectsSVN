//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	
//��������	2016.9.3
//����		
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"

#if ( SEL_DLT698_2016_FUNC == YES )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
#define PPPGOODFCS16 0xf0b8 	    /* Good final FCS value */
BYTE FrameOverTime;					//��֡��ʱʱ��
__no_init BYTE RN_MAC[50];   		//�ݴ�RN_MAC  -- ���粻�����岻�󣬵��ǿ��ܻ�����λ���յ�ȷ�ϱ���Ҳ����֤��
__no_init BYTE RN_MAC_BAK[50];   	//RN_MAC����

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------


//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//FCS lookup table as calculated by the table generator.
static const WORD fcstab[256] = 
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};




//-----------------------------------------------
//				�ڲ���������
//-----------------------------------------------


//-----------------------------------------------
//				��������
//-----------------------------------------------

//-----------------------------------------------
//��������: ��ʼ����698ͨ����صı���
//
//����:		TSerial *p  //ͨ�ſڵ�ָ��
//						
//����ֵ:	��
//		   
//��ע:
//-----------------------------------------------
void InitPoint_Dlt698(TSerial *p)
{
	//DLT698_45��Լ���չ��̼���
	p->ProStepDlt698_45 = 0;
	//DLT698_45��Լ������Serial[].ProBuf�еĿ�ʼλ��
	p->BeginPosDlt698_45 = 0;
	p->wLen = 0;
	p->Addr_Len = 6;//ͨ�ű����з�������ַSA���ȣ���ַ����Ĭ��6�ֽ�
}

//-----------------------------------------------
//��������: fcsУ�麯��
//
//����:		BYTE *cp  
//			WORD len   
//����ֵ:	WORD
//		   
//��ע:
//-----------------------------------------------
WORD fcs16(BYTE *cp, WORD len)
{
	WORD cpLen;
	WORD fcs;
	
	fcs = PPPINITFCS16;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	fcs ^= 0xffff; // complement  //��ΪFCSֵ
	return fcs;
}
//-----------------------------------------------
//��������: ����֮ǰ��fcsУ��ֵ����������fscУ��
//
//����:		BYTE *cp[in]	����ָ��
//			WORD len[in] 	���ݳ���
//			LastFcs[in]		��һ�μ����У��ֵ�����ڱ��μ���ĳ�ʼֵ
//			IsEnd[in]		�Ƿ������һ������ TRUE/FALSE ��/��
//			
//����ֵ:	WORD
//		   
//��ע:
//-----------------------------------------------
WORD ContinueFcs16(BYTE *cp, WORD len, WORD LastFcs, BOOL IsEnd)
{
	WORD cpLen;
	WORD fcs;

	fcs = LastFcs;
	cpLen = len;
	while (cpLen--)
	{
		fcs = ( (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff]);
	}

	if(IsEnd)//�������һ��crcУ��ʱִ��
	{
		fcs ^= 0xffff; // complement  //��ΪFCSֵ
	}
	
	return fcs;
}

//-----------------------------------------------
//��������: 698У���ж�
//
//����:		  
//			 
//����ֵ:	
//		   
//��ע:
//----------------------------------------------- 
BYTE  Check_fcs16_698(BYTE *Buf, BYTE Addr_Len )
{
	TTwoByteUnion Temp16,CRC_Buf;
	BYTE Len;

	Len = Buf[1]; 
	memcpy( CRC_Buf.b, Buf+(Addr_Len+6) , 2 );
	Temp16.w = fcs16( Buf+1, Addr_Len+5 );
	if( CRC_Buf.w != Temp16.w)//У��HCS
	{
		return FALSE;
	}

	memcpy( CRC_Buf.b, Buf+Len-1, 2 );
	Temp16.w = fcs16( Buf+1, Len-2 );
	if( CRC_Buf.w != Temp16.w )//У��FCS
	{
		return FALSE;
	}
	
	return TRUE ;	
}


//---------------------------------------------------------------
//��������: �ж�ͨѶ��ַ
//
//����: 	pBuf[in] - ����ͨ�ŵ�ַ
//                   
//����ֵ:  TRUE/FALSE
//
//��ע:   Ҫ��pBuf[0--5]����Ϊ���ձ����е�ͨ�ŵ�ַ,pBuf[0]Ϊ���ֽ�
//---------------------------------------------------------------
WORD JudgeRecMeterNo(BYTE *pBuf)
{
	BYTE k;
	BYTE *pMeterNo;
	BYTE i,j;

	//�жϽ��ձ����е�ͨ�ŵ�ַ�Ƿ�ȫ���Ǹ���������,����99��88
	j = JudgeRecMeterNoOnlyOneData(0x88,pBuf);

	if(j!=TRUE)
	{
		j=TRUE;
		pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);//ͨѶ��ַBYTE CommAddr[6]��BCD [0]-������ֽ�

		k = (6-1);//SEL_DLT645_2007 �� k:�ݱ����һ��AA��λ��
		//������б��ַ�жϣ��ӵ��ֽڿ�ʼ�ж�
		for(i=0; i<6; i++)
		{			
			k = ( pBuf[i]&0xf );
			if( ( k != 0xa )&&(k != (pMeterNo[5-i]&0xf) ) )
			{
				j = FALSE;
				break;
			}
			k = ( pBuf[i]&0xf0 );
			if( ( k != 0xa0)&&(k != (pMeterNo[5-i]&0xf0) ) )
			{
				j = FALSE;
				break;
			}
		}

	}
	
	return j;
}


//-----------------------------------------------
//��������: �ж�ͨ�ŵ�ַ�Ƿ���ȷ
//
//����:		  
//			 
//����ֵ:	
//		   
//��ע:
//----------------------------------------------- 
WORD JudgeRecMeterNo_Dlt698_45(TSerial *p)
{
	BYTE i, ucData;
	BYTE Buf[6];
			
	//a)	bit0��bit3��Ϊ��ַ���ֽ�����ȡֵ��Χ��0��15����Ӧ��ʾ1��16���ֽڳ��ȣ�
	//b)	bit4��bit5���߼���ַ�� 
	//c)	bit6��bit7��Ϊ��������ַ�ĵ�ַ���ͣ�0��ʾ����ַ��1��ʾͨ���ַ��2��ʾ���ַ��3��ʾ�㲥��ַ��
	ucData = p->ProBuf[(p->BeginPosDlt698_45+4)%MAX_PRO_BUF_REAL];//ͨ�Ż����С�����  MAX_PRO_BUF 
	//�ݲ��ϸ��ж��߼���ַ�ǹ���о�����Ǽ���о��
	//˫о���ܱ����о�߼���ַΪ0������о�߼���ַΪ1������֮����߼���ַΪ�Ƿ�
	if( ((ucData>>4)&0x3) > 1 )
	{
		return FALSE;
	}	
	
	ucData = (ucData/0x40);//�õ���ַ����

	if( (ucData == 1) || ( ucData == 0 ))//����ǵ���ַ��ͨ���ַ
	{
		if( p->Addr_Len == 6 )
		{
			for( i=0; i<6; i++ )	
			{
				Buf[i] = p->ProBuf[(p->BeginPosDlt698_45+5+i)%MAX_PRO_BUF_REAL];
			}
			if( ucData == 0 )//����ַ
			{
				//������е���ַ�жϣ��ӵ��ֽڿ�ʼ�жϣ�������ַ��ΪA���򵥵�ַ��Ӧ��ΪA
				for(i=0; i<6; i++)
				{					
					if( ( (Buf[i]&0xf) == 0xa )&&(0xa != (FPara1->MeterSnPara.CommAddr[5-i]&0xf) ) )
					{
						return FALSE;
					}

					if( ( (Buf[i]&0xf0) == 0xa0)&&(0xa0 != (FPara1->MeterSnPara.CommAddr[5-i]&0xf0) ) )
					{
						return FALSE;
					}
				}
			}	
			if( JudgeRecMeterNo(Buf) != FALSE )
			{
				return TRUE;
			}	
		}
	}
	else if( ucData == 3 )//�㲥��ַ
	{
		if( (p->Addr_Len == 1)&&(p->ProBuf[(p->BeginPosDlt698_45+5)%MAX_PRO_BUF_REAL] == 0xaa) )
		{
			return TRUE;
		}	
	}
	
	return FALSE;
}


//-----------------------------------------------
//��������: �ж�ͨ�ŵ�ַ�Ƿ���ȷ
//
//����:		Type 0--HCS 1--FCS  
//			 
//����ֵ:	
//		   
//��ע:һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
//----------------------------------------------- 
static WORD JudgeDlt698_45_HCS_FCS(BYTE Type, TSerial *p)
{
	WORD i, wLen, wData;
	BYTE Buf[MAX_PRO_BUF_REAL+30];
	
	if( Type == 0 )
	{
		wLen = (4+p->Addr_Len+1);
	}
	else if( Type == 1 )
	{	
		wLen = p->wLen-2;
	}
	else
	{
		return FALSE;
	}	
	if( wLen >= (MAX_PRO_BUF_REAL-1) )
	{
		return FALSE;
	}			

	for( i=0; i<wLen; i++ )
	{
		 Buf[i] = p->ProBuf[(p->BeginPosDlt698_45+1+i)%MAX_PRO_BUF_REAL];
	}

	//68 L L 43 05 01 00 29 01 16 20 0A HCS_L HCS_H 10 00 08 05 01 01 40 01 02 00 00 00 85 01 02 03 06 12 34 56 78 90 12 04 12 34 56 78 FCS_L FCS_H 16 
	wData = p->ProBuf[(p->BeginPosDlt698_45+p->ProStepDlt698_45-1)%MAX_PRO_BUF_REAL]*0x100+p->ProBuf[(p->BeginPosDlt698_45+p->ProStepDlt698_45-2)%MAX_PRO_BUF_REAL]; 
	if( wData != fcs16( Buf , wLen ) ) //�Ա��Ľ���У��
	{		
		return FALSE;
	}
		
	return TRUE;
}
//--------------------------------------------------
//��������:  ����698����֡��ʼ��68����
//         
//����:      
//           TSerial * p[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void DoSearch_68_DLT698(TSerial * p)
{
	WORD i, wNum;
	wNum = p->ProStepDlt698_45;
	for( i=1; i<wNum; i++ )
	{
		if( p->ProBuf[(p->BeginPosDlt698_45+i)%MAX_PRO_BUF_REAL] == 0x68 )
		{
			if( p->ProStepDlt698_45 == 0 )
			{
				p->ProStepDlt698_45 = MAX_PRO_BUF_REAL - 1;
			}
			else
			{
				p->ProStepDlt698_45--;
			}
            p->BeginPosDlt698_45 += i;
            if( p->BeginPosDlt698_45 >= MAX_PRO_BUF_REAL )
            {
				p->BeginPosDlt698_45 = 0;
			}
            break;			
		}
	}
	if( i >= wNum )
	{		
		p->ProStepDlt698_45 = 0;
	}	
}
//--------------------------------------------------
//��������:  ͨ�ŵ�ַ���ж�
//         
//����:      
//           TSerial * p[in]
//         
//����ֵ:    TRUE FALSE   
//         
//��ע����:  ��
//--------------------------------------------------
BOOL rejudgeMailAddr(TSerial * p)
{
    BYTE control_byte  = 0;
    // ������ַ
    if (IsInSysMeters(&p->ProBuf[(p->BeginPosDlt698_45+5)%MAX_PRO_BUF_REAL]))
    {
        control_byte = p->ProBuf[(p->BeginPosDlt698_45+3)%MAX_PRO_BUF_REAL];
        //��������Ӧ֡���������մ���  //todo Ҫ���������ҵı��ĸ�ʽ�Ƿ���ϱ�׼
        // if ((control_byte & 0xC0) == 0xC0)
        // {
        //     if (p == &Serial[eCR]) //�ز��ڵĽ��շ���1376.2��Լ�д���
        //     {
        //         return FALSE;
        //     }
        // }
        // else //�ͻ�������֡����Ϊ��͸���а�����698֡���������մ���
        // {
        //     //return FALSE;
        // }
        return TRUE;
    }
	else if(MessageTransParaRam.AllowUnknownAddrTrans== TRUE)
	{
		return TRUE;
	}
    // else if( (p == &Serial[JLTxBuf.byDestNo])   //����͸�� ����������
    //         &&(JLTxBuf.byValid == JLS_RX)
    //         &&(JLTxBuf.proxy_choice == 7))
    // {
    //     return TRUE;
    // }
    else
    {
        return FALSE;
    }	
}
//--------------------------------------------------
//��������:  ����698��ʽ�ж�
//         
//����:      BYTE ReceByte[in]
//         
//           TSerial * p[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
WORD DoReceProc_DLT698(BYTE ReceByte, TSerial * p)
{
	//����ͬ��ͷ
	if( p->ProStepDlt698_45 == 0 )
	{
		if(ReceByte == 0x68)
		{
			//Dlt698.45��Լ������Serial[].ProBuf�еĿ�ʼλ��
			p->BeginPosDlt698_45 = p->RXRPoint;
			//����ָ��
			p->ProStepDlt698_45 ++;
		}
	}
	else if( p->ProBuf[p->BeginPosDlt698_45] == 0x68 ) 
	{
		//����ָ��
		p->ProStepDlt698_45 ++;
		if( p->ProStepDlt698_45 < 3 )//���û�յ�3�ֽڣ���������û��ȫ���򲻴���
		{
		}	
		else if( p->ProStepDlt698_45 == 3 )//�Ѿ��յ���3�����ˣ�������L
		{
			p->wLen = (ReceByte*0x100+p->ProBuf[p->BeginPosDlt698_45+1]);
			// if( p->wLen > (MAX_PRO_BUF-2) )//(MAX_PRO_BUF-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			if( p->wLen > (MAX_PRO_BUF_REAL-2) )//(MAX_PRO_BUF_REAL-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			if( p->wLen < (10+6) )//��������СΪ���٣�ͨ�ŵ�ַ��6�ֽ�
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if( p->ProStepDlt698_45 == 4 )//�Ѿ��յ���4�����ˣ���������C
		{
// 			if( ReceByte & 0x80 )//����ǵ�����ģ������� bit7:���䷽��λ��DIR=0��ʾ��֡���ɿͻ��������ģ�DIR=1��ʾ��֡���ɷ����������ġ�
// 			{
// 				DoSearch_68_DLT698(p);
// 				return FALSE;
// 			}
			//�������жϣ���û��
			//1	��·����	��·���ӹ�����¼���������˳���¼��
			//3	�û�����	Ӧ�����ӹ������ݽ�������
			if( (ReceByte & 0x07) != 0x03 )//���Ӧ�� ERROR-Response ������Ȫ���˶���Ӧ���Ų���������� 2017-7-29 wlk
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}			
		}
		else if( p->ProStepDlt698_45 == 5 )//�Ѿ��յ���5�����ˣ�����������ַSA�ĵ�1�ֽ�
		{
			//a)	bit0��bit3��Ϊ��ַ���ֽ�����ȡֵ��Χ��0��15����Ӧ��ʾ1��16���ֽڳ��ȣ�
			//b)	bit4��bit5���߼���ַ�� 
			//c)	bit6��bit7��Ϊ��������ַ�ĵ�ַ���ͣ�0��ʾ����ַ��1��ʾͨ���ַ��2��ʾ���ַ��3��ʾ�㲥��ַ��
			p->Addr_Len = (ReceByte&0xf)+1;
			if( (ReceByte/0x40) <= 1 )//����ǵ���ַ��ͨ���ַ
			{
				if( p->Addr_Len != 6 )//����ַ��6�ֽ�BCD����12λBCD��
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}	
			}	
		}
		else if( p->ProStepDlt698_45 == (8+p->Addr_Len) )//��ַ�����ˣ�HCS�����ˣ��ж�HCS�Ƿ���ȷ
		{
			if( p->Addr_Len > 6 )
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			//һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
			if( JudgeDlt698_45_HCS_FCS(0,p) == FALSE )	
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}

//			if( p == &Serial[eIR] )//�����ǰͨ��Ϊ����ͨ��
//			{
//                api_SetLcdCommunicationFlag( eIR );//��ͨѶ��־
//			}
			#if (SEL_SEARCH_METER == YES )
            if ( (p == &Serial[SEARCH_METER_CH]) 
              &&( SKMeter.byAutoCheck != 0 ) 
              &&( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
              ) // �ѱ�ͨ�����ж�
            {
                return FALSE;
            }
			else 
            #endif
			//�ж�ͨ�ŵ�ַ�Ƿ���ȷ
			if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			{
                //�ж��Ƿ�Ϊ����ģ���õ��ĵ�ַ
               if(rejudgeMailAddr(p) == FALSE)
               {
                    DoSearch_68_DLT698(p);
               }
                return FALSE;
			}
			else
			{
				BYTE control_byte = p->ProBuf[(p->BeginPosDlt698_45+3)%MAX_PRO_BUF_REAL];
                //����ǵ�����ģ������� bit7:���䷽��λ��DIR=0��ʾ��֡���ɿͻ��������ģ�DIR=1��ʾ��֡���ɷ����������ġ�
				if( control_byte & 0x80 )
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}
			}
		}
		else if( p->ProStepDlt698_45 == (1+p->wLen) )//�յ�֡������ 16H�ˣ��ж�FCS�Ƿ���ȷ
		{
			//һ�ڶ��Լ����£�����Ϊѭ�����壬��У��ǰҪ�Ȱѻ����������Ƶ�һ����ʱ����
			if( JudgeDlt698_45_HCS_FCS(1,p) == FALSE )	
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}	
		}	
		else if( p->ProStepDlt698_45 == (2+p->wLen) )//�յ�֡������ 16H��,��û�ж�16H�Ƿ������ȷ
		{
			if( ReceByte == 0x16 )
			{	
				BYTE byAck = 0;

				//��ֹ����
				api_DisableRece( p );
				
				//�����ݴӱ��Ŀ�ʼλ�� �� ��p->ProBuf[0]��ʼ����
				DoReceMoveData(PRO_DLT698_45, p);//��÷��� 698.45Э�鴦���� ��
	
			//	�˴�����698.45Э�鴦����//ProcDlt698App();//ProMessage( p );				

				if( Pre_Dlt698(p, &byAck) )
				{
					if (byAck == 0) 
					{
						api_EnableRece( p );
					}
				}
				else
				{
					ProcMessageDlt698(p);
				}
				TX_LED_FLASH;
				//�������
				InitPoint(p);
	
				return TRUE;
			}
			else
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}	
		}
		else
		{
			// if( p->ProStepDlt698_45 >= MAX_PRO_BUF )//(MAX_PRO_BUF-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			if( p->ProStepDlt698_45 >= MAX_PRO_BUF_REAL )//(MAX_PRO_BUF_REAL-2) )//�û����ݳ��ȣ���bit0��bit13��ɣ�����BIN���룬�Ǵ���֡�г���ʼ�ַ��ͽ����ַ�֮���֡�ֽ�����
			{
				p->ProStepDlt698_45 = 0;
				return FALSE;
			}
		}	
			
	}		
		
	return FALSE;
}
//--------------------------------------------------
//��������:  ���6981��·��ͷ����
//         
//����:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void AddDLT698LinkDataHead( BYTE Ch,BYTE *Buf, TTwoByteUnion* Lenth )
{	
	BYTE i;
	TTwoByteUnion *Len;
	
	Len = Lenth;
        
	Buf[0] =0x68;	//��ʼ�ַ�
					//����δ��д
	Buf[3] = (LinkData[Ch].ControlByte|0x80); //������
	Buf[4] = 0x05; //��ַ����
	for( i=0; i<LinkData[Ch].AddressLen; i++ )
	{
		Buf[5+i] = LinkData[Ch].ServerAddr[i];//��������ַ��������ַ(����Ϊ�������Ϊ�г��׮ҵ����)
	}
	Buf[5+LinkData[Ch].AddressLen] = LinkData[Ch].ClientAddress;
							      //У��δ��д
	Len->b[0] = (DLT_LINK_DATA_HEAD+LinkData[Ch].AddressLen);
}
//--------------------------------------------------
//��������:  ���6981��·��ͷ����
//         
//����:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void AddDLT698LinkDataHeadZD( BYTE Ch,BYTE *Buf, TTwoByteUnion* Lenth )
{	
	BYTE i;
	TTwoByteUnion *Len;
	
	Len = Lenth;
        
	Buf[0] =0x68;	//��ʼ�ַ�
					//����δ��д
	Buf[3] = (LinkData[Ch].ControlByte); //������
	Buf[4] = 0x05; //��ַ����
	for( i=0; i<LinkData[Ch].AddressLen; i++ )
	{
		Buf[5+i] = LinkData[Ch].ServerAddr[i];//��������ַ��������ַ(����Ϊ�������Ϊ�г��׮ҵ����)
	}
	Buf[5+LinkData[Ch].AddressLen] = LinkData[Ch].ClientAddress;
							      //У��δ��д
	Len->b[0] = (DLT_LINK_DATA_HEAD+LinkData[Ch].AddressLen);
}
//--------------------------------------------------
//��������:  ���698��·��β����
//         
//����:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static void AddDLT698LinkDataTail(BYTE Ch, BYTE *buf, TTwoByteUnion* Lenth)
{
    WORD i;
	TTwoByteUnion Temp16;
	TTwoByteUnion *Len;
	
	Len = Lenth;
	
	Len->w += (DLT_LINK_DATA_TAIL-2);//���� ��������ʼ�ַ�+�����ַ�
	buf[1] = Len->b[0]; //���ݳ���
	buf[2] = Len->b[1];
	Len->w += 2;//���������ݳ���
	
	Temp16.w = fcs16(&buf[1],(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_START_BYTE-DLT_CSLEN));	//��������ʼ��ַ��У����
	buf[(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_CSLEN)] = Temp16.b[0];  //���hrc
	buf[(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_CSLEN+1)] = Temp16.b[1];
	
	if( buf[3] & 0x08 )//LinkData[Ch].ControlByte bit3:�����־λSC
	{
		i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6ʱiΪ15
		i += 2;//��ǰ�У�Len->w += 2;//���������ݳ���
        if( Len->w > i )
		{
			Temp16.w = Len->w - i;
			for( i=0; i<Temp16.w; i++ )
			{
				buf[LinkData[Ch].AddressLen +DLT_LINK_DATA_HEAD+i] += 0x33;
			}
		}
	}

	Temp16.w = fcs16(&buf[1],(Len->w -DLT_CSLEN-DLT_START_BYTE-DLT_END_BYTE));	//���FCS
	buf[(Len->w-3)]= Temp16.b[0];
	buf[Len->w-2] = Temp16.b[1];
	buf[Len->w-1] = 0x16;

}


//-----------------------------------------------
//��������: ��·���֡������
//
//����:		BYTE *pControlByte[in]
//			 
//����ֵ:	BYTE
//		   
//��ע:
//-----------------------------------------------
static eFrameResultChoice Proc698LinkDataRequestFrame(BYTE Ch, BYTE ControlByte, BYTE *pAPDU )
{
	TTwoByteUnion Follow_Len;
	
	if( ControlByte & 0x20)//����Ǻ���֡
	{	
		if( (pAPDU[1]&0xc0) != 0x80 )//����ȷ��֡
		{

            if( (APDUBufFlag.Request.Flag == TRUE) && ( APDUBufFlag.Request.Ch != Ch ))//ͨ��buf��ռ����ͨ����һ��
            {
                return eLINKDATA_FAIL;
            }

            APDUBufFlag.Request.Ch = Ch;
            APDUBufFlag.Request.Flag = TRUE;

            LinkData[Ch].DownLinkFrame.pFrame = DLT698APDURequestBuf;//��ʼ��������·���ָ֡��

            if( (pAPDU[1]&0xc0) == 0x00 )//��֡��ʼ��־
            {
            	APPData[Ch].APPFlag = 0;//���Ӧ�ò��֡��־��ֻ�ܴ���һ�ַ�֡��ʽ
                LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//ֻ֧��1���ֽڵķ�֡����
                if( LinkData[Ch].DownLinkFrame.FrameType.b[0] == 0 ) //��ʼ��־��1֡����Ϊ��
                {	//Ӧ�ò����ݳ��� = �ܳ��� - ��·������ͷ - ��·������β -��ַ���� - ��֡������
                    Follow_Len.w = (LinkData[Ch].DataLen.w-DLT_LINK_DATA_HEAD-DLT_LINK_DATA_TAIL-LinkData[Ch].AddressLen-DLT_FRAME_BYTE+2);

                    if( Follow_Len.w > MAX_APDU_SIZE )//�������ֵ�ж�
                    {
                        return eLINKDATA_FAIL;
                    }

                    memcpy(LinkData[Ch].DownLinkFrame.pFrame,pAPDU+2,Follow_Len.w);//��������
                    LinkData[Ch].DownLinkFrame.FrameLen.w = Follow_Len.w;//������������ݳ���

                    LinkData[Ch].DownLinkFrame.FrameType.b[1] = 0x80;//�����Ӧ�����ֽڡ�1��
                    return eDOWN_FRAMEING;//���з�֡����
                }
                else
                {
                    return eLINKDATA_FAIL;
                }
            }
            else
            {
                if( LinkData[Ch].DownLinkFrame.FrameType.b[0] == (pAPDU[0]-1) ) //���������͵�֡���Ӧ�Ƚ��ܵ��ļ�1
                {
                    LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//ֻ֧��1���ֽڵķ�֡����
                    //Ӧ�ò����ݳ��� = �ܳ��� - ��·������ͷ - ��·������β -��ַ���� - ��֡������
                    Follow_Len.w = (LinkData[Ch].DataLen.w-DLT_LINK_DATA_HEAD-DLT_LINK_DATA_TAIL-LinkData[Ch].AddressLen-DLT_FRAME_BYTE+2);

                    if( (LinkData[Ch].DownLinkFrame.FrameLen.w+Follow_Len.w) > MAX_APDU_SIZE )//�������ֵ�ж�
                    {
                        return eLINKDATA_FAIL;
                    }

                    memcpy(&LinkData[Ch].DownLinkFrame.pFrame[LinkData[Ch].DownLinkFrame.FrameLen.w],pAPDU+2,Follow_Len.w);//��������
                    LinkData[Ch].DownLinkFrame.FrameLen.w += Follow_Len.w;//������������ݳ���

                    LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//�����Ӧ�����ֽڡ�0��
                    LinkData[Ch].DownLinkFrame.FrameType.b[1] = 0x80;//�����Ӧ�����ֽڡ�1��
                }
                else
                {
                    return eLINKDATA_FAIL;
                }
                
                if( (pAPDU[1]&0xc0) == 0x40 )
                {
                    LinkData[Ch].ControlByte &= 0xdf;
                    return eDOWN_FRAME_END;//���з�֡����
                }
                
                return eDOWN_FRAMEING;//���з�֡��
            }
		}
		else //��֡ȷ��֡-��ǰ�������⴦��
		{
        	if( pAPDU[0] == LinkData[Ch].UpLinkFrame.FrameType.b[0] )//ȷ��֡�����з�֡���
			{
				return eUP_FRAMEING;//���з�֡��
			}
			else
			{
				return eLINKDATA_FAIL;
			}
		}		
	}
	else
	{
		return eNO_FRAME;
	}

}

//--------------------------------------------------
//��������:  Ӧ����·���֡����
//         
//����:      BYTE Ch[in]
//         
//           BYTE* Buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//����ֵ:    static
//         
//��ע����:  ��
//--------------------------------------------------
static eFrameResultChoice Proc698LinkDataResponseFrame(BYTE Ch ,BYTE* Buf, TTwoByteUnion* Lenth)
{
	TTwoByteUnion Len1,TotalLen;
	TTwoByteUnion *Len;
	DWORD ConsultLen;
	
	Len = Lenth;
	
	if( (APPData[Ch].APPFlag & APP_LINK_FRAME) == APP_LINK_FRAME )//Ӧ�ò�������Ҫ��·���֡
	{
	    ConsultLen = GetConsultBufLen( Ch );//��ȡ��ǰͨ��Э�̳���
	    
		LinkData[Ch].UpLinkFrame.pFrame = &Buf[Len->w];//��ʼ��ָ��
		Buf[3] |= 0x20;//��λ��֡������ 
		if( LinkData[Ch].eLinkDataFlag == eAPP_OK )//��·���֡��֡
		{
			LinkData[Ch].UpLinkFrame.FrameType.b[0] = 0;//���͵�1֡
			LinkData[Ch].UpLinkFrame.FrameType.b[1] = 0;//��ʼ֡
			//�����֡���ݳ���
			LinkData[Ch].UpLinkFrame.FrameLen.w = ( ConsultLen - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -LinkData[Ch].AddressLen -2 );//-2�Ƿ�֡��ʽ��

            if( LinkData[Ch].UpLinkFrame.FrameLen.w > APPData[Ch].APPBufLen.w )//��һ֡buf���Ȳ�ӦС�������ܳ���
            {
               return eLINKDATA_FAIL;//��֡ʧ��
            }
			//��֡����
			LinkData[Ch].UpLinkFrame.pFrame[0] = LinkData[Ch].UpLinkFrame.FrameType.b[0];//��֡��ʽ��
			LinkData[Ch].UpLinkFrame.pFrame[1] = LinkData[Ch].UpLinkFrame.FrameType.b[1];

		    if( (Len->w+LinkData[Ch].UpLinkFrame.FrameLen.w) > MAX_PRO_BUF )//���ݳ��Ȳ��ܳ���MAX_PRO_BUF
	        {
                return eLINKDATA_FAIL;
    		}
    		
			memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf,LinkData[Ch].UpLinkFrame.FrameLen.w);
			Len->w += (LinkData[Ch].UpLinkFrame.FrameLen.w+2);//�������ݳ���
			return eUP_FRAMEING;//��֡��
		}
		else if( LinkData[Ch].eLinkDataFlag == eUP_FRAMEING)
		{
			LinkData[Ch].UpLinkFrame.FrameType.b[0] =LinkData[Ch].UpLinkFrame.FrameType.b[0]+1;//������һ֡
			LinkData[Ch].UpLinkFrame.FrameType.b[1] = 0xc0;//��ʼ֡

			//��֡����
			LinkData[Ch].UpLinkFrame.pFrame[0] = LinkData[Ch].UpLinkFrame.FrameType.b[0];//��֡��ʽ��
			LinkData[Ch].UpLinkFrame.pFrame[1] = LinkData[Ch].UpLinkFrame.FrameType.b[1];

            TotalLen.w = (LinkData[Ch].UpLinkFrame.FrameLen.w)*(LinkData[Ch].UpLinkFrame.FrameType.b[0]);

			if( APPData[Ch].APPBufLen.w < TotalLen.w )//�����ܳ��Ȳ�ӦС���Ѵ��������
			{
                return eLINKDATA_FAIL;//��֡ʧ��
			}
			
			if( ( APPData[Ch].APPBufLen.w-TotalLen.w ) > LinkData[Ch].UpLinkFrame.FrameLen.w)
			{			
    		    if( (Len->w+LinkData[Ch].UpLinkFrame.FrameLen.w) > MAX_PRO_BUF )//���ݳ��Ȳ��ܳ���MAX_PRO_BUF
    	        {
                    return eLINKDATA_FAIL;
        		}
        		
				memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf+TotalLen.w,LinkData[Ch].UpLinkFrame.FrameLen.w);
				Len->w += (LinkData[Ch].UpLinkFrame.FrameLen.w+2);//�������ݳ���
			}
			else
			{		
				if( Ch == eCR )//��֡���� �����֡��ʱ
	        	{
	        		FrameOverTime = 0;
	        	}
	        	
				LinkData[Ch].UpLinkFrame.pFrame[1] = 0x40;//����֡
				APPData[Ch].APPFlag = 0;//�����֡��־
				Len1.w = (APPData[Ch].APPBufLen.w -TotalLen.w);//��ȡbuf����

			    if( (Len->w+Len1.w) > MAX_PRO_BUF )//���ݳ��Ȳ��ܳ���MAX_PRO_BUF
    	        {
                    return eLINKDATA_FAIL;
        		}
        		
				memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf+TotalLen.w,Len1.w);//����ʣ��֡
				Len->w += Len1.w+2;//�������ݳ���
				return eUP_FRAME_END;//��֡����
			}
	
			return eUP_FRAMEING;//��֡��
		}
		else
		{
			return eLINKDATA_FAIL;//��֡ʧ��
		}
		
	}
	else if( LinkData[Ch].eLinkDataFlag == eDOWN_FRAMEING )
	{
		Buf[Len->w] = LinkData[Ch].DownLinkFrame.FrameType.b[0];
		Buf[Len->w+1] = LinkData[Ch].DownLinkFrame.FrameType.b[1];
		Len->w +=2;
		return eDOWN_FRAMEING;//��֡��
	}
	else
	{
		return eNO_FRAME;//�޷�֡
	}

}
//--------------------------------------------------
//��������:  ��·����������
//         
//����:      BYTE Ch[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void Proc698LinkDataRequest( BYTE Ch )
{	
	WORD i,j;
	TTwoByteUnion LEN;
	BYTE *pAPDU;
	BYTE GenericAddress[6];

    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
//	if( Ch != eIR )//����ͨ�����ж�֡��ʽʱ�ѽ�����λ
//	{
//	    api_SetLcdCommunicationFlag( Ch );//��ͨѶ��־
//	}
	
	LEN.b[0] = Serial[Ch].ProBuf[1];
	LEN.b[1] = Serial[Ch].ProBuf[2];

	if( LEN.w > MAX_PRO_BUF )
	{
        LinkData[Ch].eLinkDataFlag = eLINKDATA_FAIL;
	}
	else
	{
        LinkData[Ch].DataLen.w = LEN.w; //������ݳ���
        
        LinkData[Ch].ControlByte = Serial[Ch].ProBuf[3];//�������ֽ�
        LinkData[Ch].AddressType = (eAddressType)(Serial[Ch].ProBuf[4]>>6);
        LinkData[Ch].AddressLen = Serial[Ch].Addr_Len;//����ַ����
        memcpy(&LinkData[Ch].ServerAddr[0],&Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1],LinkData[Ch].AddressLen);

		//ȫ88��ַ
		memset(GenericAddress,0x88,sizeof(GenericAddress));
		if (memcmp(LinkData[Ch].ServerAddr,GenericAddress,sizeof(GenericAddress))==0)
		{
			lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
		}
		else//��ͨ������ָ�ϵͳ��ַ
		{
			for( i = 0; i < 12; i++ )
			{
				if( ( (LinkData[Ch].ServerAddr[i/2] >> ( (i % 2) * 4)) & 0x0F ) == 0x0A)	
				{
					lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
					break;
				}
			}
		}
		
        LinkData[Ch].ClientAddress = Serial[Ch].ProBuf[LinkData[Ch].AddressLen +DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1];
        pAPDU = (BYTE *)&(Serial[Ch].ProBuf[(LinkData[Ch].AddressLen +DLT_LINK_DATA_HEAD)]);
            
        if( LinkData[Ch].ControlByte & 0x08 )//bit3:�����־λSC
        {
            i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6ʱiΪ15
            if( LEN.w > i )
            {
                LEN.w = LEN.w - i;
                
                for( i=0; i<LEN.w; i++ )
                {
                    pAPDU[i] -= 0x33;
                }
            }
        }
        
        LinkData[Ch].eLinkDataFlag = Proc698LinkDataRequestFrame( Ch,LinkData[Ch].ControlByte , pAPDU);
    
        if( LinkData[Ch].eLinkDataFlag == eNO_FRAME )
        {
        	if( Ch == eCR )//������������ �����֡��ʱ
        	{
        		FrameOverTime = 0;
        	}

            LinkData[Ch].eLinkDataFlag = eAPP_OK;
            LinkData[Ch].pAPP = &Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN];
            LinkData[Ch].pAPPLen.w = (LinkData[Ch].DataLen.w -(DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_CSLEN) );
        }
        else if( LinkData[Ch].eLinkDataFlag == eDOWN_FRAME_END )//���з�֡����
        {
            if( Ch == eCR )//������������ �����֡��ʱ
        	{
        		FrameOverTime = 0;
        	}
        	
            LinkData[Ch].eLinkDataFlag = eAPP_OK;
            LinkData[Ch].pAPP = LinkData[Ch].DownLinkFrame.pFrame;
            LinkData[Ch].pAPPLen.w= LinkData[Ch].DownLinkFrame.FrameLen.w;
        }
        else
        {   
        }

	}       
}

//--------------------------------------------------
//��������:  ��·����������
//         
//����:      BYTE Ch[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void Proc698LinkDataRequestForZD( BYTE Ch, BYTE *pBuf, WORD wLen )
{	
	WORD i,j;
	TTwoByteUnion LEN;
	BYTE *pAPDU;
	BYTE GenericAddress[6];

    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return;
    }
    
	memcpy(Serial[Ch].ProBuf,pBuf,wLen);
	Serial[Ch].SendLength = wLen;
	
//	if( Ch != eIR )//����ͨ�����ж�֡��ʽʱ�ѽ�����λ
//	{
//	    api_SetLcdCommunicationFlag( Ch );//��ͨѶ��־
//	}
	
	LEN.b[0] = Serial[Ch].ProBuf[1];
	LEN.b[1] = Serial[Ch].ProBuf[2];

	//if( LEN.w > MAX_PRO_BUF )
	//{
    //    LinkData[Ch].eLinkDataFlag = eLINKDATA_FAIL;
	//}
	//else
	{
        LinkData[Ch].DataLen.w = LEN.w; //������ݳ���
        
        LinkData[Ch].ControlByte = Serial[Ch].ProBuf[3];//�������ֽ�
        LinkData[Ch].AddressType = (eAddressType)(Serial[Ch].ProBuf[4]>>6);
        LinkData[Ch].AddressLen = Serial[Ch].Addr_Len;//����ַ����
        memcpy(&LinkData[Ch].ServerAddr[0],&Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1],LinkData[Ch].AddressLen);

		//ȫ88��ַ
		memset(GenericAddress,0x88,sizeof(GenericAddress));
		if (memcmp(LinkData[Ch].ServerAddr,GenericAddress,sizeof(GenericAddress))==0)
		{
			lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
		}
		else//��ͨ������ָ�ϵͳ��ַ
		{
			for( i = 0; i < 12; i++ )
			{
				if( ( (LinkData[Ch].ServerAddr[i/2] >> ( (i % 2) * 4)) & 0x0F ) == 0x0A)	
				{
					lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
					break;
				}
			}
		}
		
        LinkData[Ch].ClientAddress = Serial[Ch].ProBuf[LinkData[Ch].AddressLen +DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1];
        pAPDU = (BYTE *)&(Serial[Ch].ProBuf[(LinkData[Ch].AddressLen +DLT_LINK_DATA_HEAD)]);
            
        if( LinkData[Ch].ControlByte & 0x08 )//bit3:�����־λSC
        {
            i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6ʱiΪ15
            if( LEN.w > i )
            {
                LEN.w = LEN.w - i;
                
                for( i=0; i<LEN.w; i++ )
                {
                    pAPDU[i] -= 0x33;
                }
            }
        }
        
        LinkData[Ch].eLinkDataFlag = eNO_FRAME;
    
    	if( Ch == eCR )//������������ �����֡��ʱ
    	{
    		FrameOverTime = 0;
    	}

        LinkData[Ch].eLinkDataFlag = eAPP_OK;
        APPData[Ch].APPFlag = APP_HAVE_DATA;
      	APPData[Ch].pAPPBuf = &pBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN];
        APPData[Ch].APPBufLen.w = (LinkData[Ch].DataLen.w -(DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_CSLEN) );
	}       
}

//--------------------------------------------------
//��������:  ��·��������Ӧ����
//         
//����:      BYTE Ch[in]
//         
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
BYTE Proc698LinkDataResponse(BYTE Ch)
{
	BYTE Buf[MAX_PRO_BUF+EXTRA_BUF];
	eFrameResultChoice eResultChoice;
	TTwoByteUnion Len;
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//����ͨ���ж�
    {
        return FALSE;
    }
    
	Len.w = 0; 
	if( ((APPData[Ch].APPFlag&APP_HAVE_DATA) != APP_HAVE_DATA) && (LinkData[Ch].eLinkDataFlag == eAPP_OK))//Ӧ�ò������ݷ���
	{
		api_InitSci( Ch );
		return FALSE;
	}
	if( (LinkData[Ch].eLinkDataFlag == eLINKDATA_FAIL) || (LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE) )//��·����ִ�����߹㲥��ַ��Ӧ��
	{
		api_InitSci( Ch );
		return FALSE;
	}
	
	AddDLT698LinkDataHead(Ch,Buf,&Len); //���֡ͷ
	eResultChoice = Proc698LinkDataResponseFrame( Ch ,Buf, &Len);//�жϺ���֡
	if( eResultChoice == eNO_FRAME )//�޺���֡
	{
	    if( (Len.w+APPData[Ch].APPBufLen.w) > MAX_PRO_BUF)//���ݴ���buf���� �쳣
	    {
            api_InitSci( Ch );
            return FALSE;
	    }
	    else
	    {
            memcpy( &Buf[Len.w],APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//��֡Ӧ�ò�buf
            Len.w += APPData[Ch].APPBufLen.w;
	    }
	}
	else if( eResultChoice == eLINKDATA_FAIL )
	{
		Serial[Ch].BroadCastFlag = 0xff;
		return FALSE;
	}
	else
	{
		if( Ch == eCR )//�ز�ͨ��
		{
			//��֡������ ��λ��֡��ʱ
			if( (eResultChoice == eDOWN_FRAMEING) || (eResultChoice == eUP_FRAMEING))
			{
				FrameOverTime = 60;//���ڷ�֡״̬���޷����з�֡�������з�֡������ˢ��1������ʱ
			}

		}
	}
	
	AddDLT698LinkDataTail(Ch, Buf,&Len);//���֡β

//	if(Ch == eBlueTooth)
//	{
//		api_mt_write_ble_ch_data(Buf,Len.sw,bBleChannel);	
//	}
//	else 
	{
		memset( (void *)Serial[Ch].ProBuf, 0xfe, 4 );
		memcpy( (Serial[Ch].ProBuf+4),Buf,Len.w);
		Serial[Ch].SendLength = (Len.w+4);
	}
	
	//ִ����ʱ
	Serial[Ch].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;

	SerialMap[Ch].SCIEnableSend(SerialMap[Ch].SCI_Ph_Num); 
    return TRUE;
}

//--------------------------------------------------
//��������:  �ϱ���·�����ݺ���
//         
//����:     pAPDU[IN]	 //apdu ָ��
//		  APDU_Len[IN]//apdu ���� ������м���ֵ�ж�
//        Type[IN]//���ͣ�0X00-����+MAC 0x55-����
//����ֵ:    
//         
//��ע����:  ��
//--------------------------------------------------
void api_ActiveReportResponse(BYTE* pAPDU, WORD APDU_Len ,BYTE Type)
{
	BYTE Buf[MAX_PRO_BUF+EXTRA_BUF],Buf1[MAX_PRO_BUF+EXTRA_BUF],i;
	TTwoByteUnion Len,Len1,Len2,Temp16;
	WORD Result;
	
	#if(SEL_ESAM_TYPE == 0)
	Type = 0x55;
	#endif
	
	if(Type != 0x55)
	{
		memcpy( RN_MAC_BAK, RN_MAC, sizeof(RN_MAC_BAK) );//���ݵ�ǰrn_mac
		Result = api_DealEsamSafeReportRN_MAC( APDU_Len, pAPDU, RN_MAC);//��ȡRN_MAC
		if( Result == 0x8000 )//ʧ�ܺ� �ϱ�ȫFF
		{
			memset( RN_MAC, 0xff, sizeof(RN_MAC) );
		}
		
		//��֡��ȫ����
		Len2.w = APDU_Len;//����Ӧ������-��ȫ���䴦��
		Buf1[0] = 0x10;//��ȫ����
		Buf1[1] = 0x00;
		Len1.w = 2;
		Len1.w += Deal_698DataLenth( &Buf1[2],Len2.b,eUNION_TYPE);;//��ȡ���������ݵ�ƫ�Ƴ���
		memcpy( &Buf1[Len1.w], pAPDU, APDU_Len );
		Len1.w += APDU_Len;//��䰲ȫ��������
		Buf1[Len1.w] = 0x02;//ѡ��rn_mac
		Buf1[Len1.w+1] = 12;//���������Ϊ12
		memcpy( &Buf1[Len1.w+2], RN_MAC, 12 );//��֡rn
		Len1.w +=14;
		Buf1[Len1.w] = 4;//mac����Ϊ4
		memcpy( &Buf1[Len1.w+1], RN_MAC+12, 4 );//��֡mac
		Len1.w +=5;
		

	}
	else
	{
		memcpy( (BYTE*)&Buf1, pAPDU, APDU_Len );//��֡apdu
		Len1.w = APDU_Len;
	}
	if( Len1.w > 450 )//����ֵ�ж�
	{
		return ;
	}
	Buf[0] =0x68;	//��ʼ�ַ�
					//����δ��д
	Buf[3] = 0x83; //������
	Buf[4] = 0x05; //��ַ����
	for( i=0; i<6; i++ )
	{
		Buf[5+i] = FPara1->MeterSnPara.CommAddr[5-i];//��������ַ��������ַ
	}
	Buf[5+6] = 0x00;//Э��涨 ���ͻ�����ַ��0x00
	Len.w = (DLT_LINK_DATA_HEAD+6);//֡ͷʵ�ʳ���
	
	memcpy( (BYTE*)&Buf[Len.w], Buf1, Len1.w );//��֡apdu
	Len.w += Len1.w;
	
	Len.w += (DLT_LINK_DATA_TAIL-2);//���� ��������ʼ�ַ�+�����ַ�
	Buf[1] = Len.b[0]; //���ݳ���
	Buf[2] = Len.b[1];
	Len.w += 2;//���������ݳ���
	Temp16.w = fcs16(&Buf[1],(6+DLT_LINK_DATA_HEAD-DLT_START_BYTE-DLT_CSLEN));	//��������ʼ��ַ��У����
	Buf[(6+DLT_LINK_DATA_HEAD-DLT_CSLEN)] = Temp16.b[0];  //���hrc
	Buf[(6+DLT_LINK_DATA_HEAD-DLT_CSLEN+1)] = Temp16.b[1];

	Temp16.w = fcs16(&Buf[1],(Len.w -DLT_CSLEN-DLT_START_BYTE-DLT_END_BYTE));	//���FCS
	Buf[(Len.w-3)]= Temp16.b[0];
	Buf[Len.w-2] = Temp16.b[1];
	Buf[Len.w-1] = 0x16;

	memset( (void *)Serial[eCR].ProBuf, 0xfe, 4 );
	memcpy( (Serial[eCR].ProBuf+4),Buf,Len.w);
	Serial[eCR].SendLength = (Len.w+4);
	//ִ����ʱ
	Serial[eCR].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	Serial[eCR].RxOverCount = 500;//����500ms������ʱ - ȡ�� ʹ���ֽڼ���ʱ�����ж� ����Ҫ��������ʱ ���ҿ��Խ����������ʱ���ϱ�������
	SerialMap[eCR].SCIEnableSend(SerialMap[eCR].SCI_Ph_Num); 
	
	//������
	/*memset( (void *)Serial[eRS485_I].ProBuf, 0xfe, 4 );
	memcpy( (Serial[eRS485_I].ProBuf+4),Buf,Len.w);
	Serial[eRS485_I].SendLength = (Len.w+4);
	//ִ����ʱ
	Serial[eRS485_I].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	Serial[eRS485_I].RxOverCount = 500;//����500ms������ʱ - ȡ�� ʹ���ֽڼ���ʱ�����ж� ����Ҫ��������ʱ ���ҿ��Խ����������ʱ���ϱ�������
	SerialMap[eRS485_I].SCIEnableSend(SerialMap[eRS485_I].SCI_Ph_Num);*/
}


#endif//(SEL_DLT698_2016_FUNC == YES )
