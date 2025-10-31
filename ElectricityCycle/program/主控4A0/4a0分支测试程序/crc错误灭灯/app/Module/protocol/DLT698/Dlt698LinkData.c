//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	2016.9.3
//描述		
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"

#if ( SEL_DLT698_2016_FUNC == YES )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define PPPGOODFCS16 0xf0b8 	    /* Good final FCS value */
BYTE FrameOverTime;					//分帧超时时间
__no_init BYTE RN_MAC[50];   		//暂存RN_MAC  -- 掉电不清意义不大，但是可能会解决表复位后收到确认报文也会验证过
__no_init BYTE RN_MAC_BAK[50];   	//RN_MAC备份

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
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
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//-----------------------------------------------
//函数功能: 初始化和698通信相关的变量
//
//参数:		TSerial *p  //通信口的指针
//						
//返回值:	无
//		   
//备注:
//-----------------------------------------------
void InitPoint_Dlt698(TSerial *p)
{
	//DLT698_45规约接收过程计数
	p->ProStepDlt698_45 = 0;
	//DLT698_45规约报文在Serial[].ProBuf中的开始位置
	p->BeginPosDlt698_45 = 0;
	p->wLen = 0;
	p->Addr_Len = 6;//通信报文中服务器地址SA长度，地址长度默认6字节
}

//-----------------------------------------------
//函数功能: fcs校验函数
//
//参数:		BYTE *cp  
//			WORD len   
//返回值:	WORD
//		   
//备注:
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

	fcs ^= 0xffff; // complement  //此为FCS值
	return fcs;
}
//-----------------------------------------------
//函数功能: 根据之前的fcs校验值，继续计算fsc校验
//
//参数:		BYTE *cp[in]	数据指针
//			WORD len[in] 	数据长度
//			LastFcs[in]		上一次计算的校验值，用于本次计算的初始值
//			IsEnd[in]		是否是最后一段数据 TRUE/FALSE 是/否
//			
//返回值:	WORD
//		   
//备注:
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

	if(IsEnd)//计算最后一段crc校验时执行
	{
		fcs ^= 0xffff; // complement  //此为FCS值
	}
	
	return fcs;
}

//-----------------------------------------------
//函数功能: 698校验判断
//
//参数:		  
//			 
//返回值:	
//		   
//备注:
//----------------------------------------------- 
BYTE  Check_fcs16_698(BYTE *Buf, BYTE Addr_Len )
{
	TTwoByteUnion Temp16,CRC_Buf;
	BYTE Len;

	Len = Buf[1]; 
	memcpy( CRC_Buf.b, Buf+(Addr_Len+6) , 2 );
	Temp16.w = fcs16( Buf+1, Addr_Len+5 );
	if( CRC_Buf.w != Temp16.w)//校验HCS
	{
		return FALSE;
	}

	memcpy( CRC_Buf.b, Buf+Len-1, 2 );
	Temp16.w = fcs16( Buf+1, Len-2 );
	if( CRC_Buf.w != Temp16.w )//校验FCS
	{
		return FALSE;
	}
	
	return TRUE ;	
}


//---------------------------------------------------------------
//函数功能: 判断通讯地址
//
//参数: 	pBuf[in] - 传入通信地址
//                   
//返回值:  TRUE/FALSE
//
//备注:   要求pBuf[0--5]依次为接收报文中的通信地址,pBuf[0]为低字节
//---------------------------------------------------------------
WORD JudgeRecMeterNo(BYTE *pBuf)
{
	BYTE k;
	BYTE *pMeterNo;
	BYTE i,j;

	//判断接收报文中的通信地址是否全部是给定的数据,例如99或88
	j = JudgeRecMeterNoOnlyOneData(0x88,pBuf);

	if(j!=TRUE)
	{
		j=TRUE;
		pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);//通讯地址BYTE CommAddr[6]，BCD [0]-保存高字节

		k = (6-1);//SEL_DLT645_2007 中 k:暂保存第一个AA的位置
		//下面进行表地址判断，从低字节开始判断
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
//函数功能: 判断通信地址是否正确
//
//参数:		  
//			 
//返回值:	
//		   
//备注:
//----------------------------------------------- 
WORD JudgeRecMeterNo_Dlt698_45(TSerial *p)
{
	BYTE i, ucData;
	BYTE Buf[6];
			
	//a)	bit0…bit3：为地址的字节数，取值范围：0…15，对应表示1…16个字节长度；
	//b)	bit4…bit5：逻辑地址； 
	//c)	bit6…bit7：为服务器地址的地址类型，0表示单地址，1表示通配地址，2表示组地址，3表示广播地址。
	ucData = p->ProBuf[(p->BeginPosDlt698_45+4)%MAX_PRO_BUF_REAL];//通信缓冲大小最后用  MAX_PRO_BUF 
	//暂不严格判断逻辑地址是管理芯，还是计量芯的
	//双芯电能表管理芯逻辑地址为0，计量芯逻辑地址为1，除此之外的逻辑地址为非法
	if( ((ucData>>4)&0x3) > 1 )
	{
		return FALSE;
	}	
	
	ucData = (ucData/0x40);//得到地址类型

	if( (ucData == 1) || ( ucData == 0 ))//如果是单地址、通配地址
	{
		if( p->Addr_Len == 6 )
		{
			for( i=0; i<6; i++ )	
			{
				Buf[i] = p->ProBuf[(p->BeginPosDlt698_45+5+i)%MAX_PRO_BUF_REAL];
			}
			if( ucData == 0 )//单地址
			{
				//下面进行单地址判断，从低字节开始判断，如果表地址不为A，则单地址不应该为A
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
	else if( ucData == 3 )//广播地址
	{
		if( (p->Addr_Len == 1)&&(p->ProBuf[(p->BeginPosDlt698_45+5)%MAX_PRO_BUF_REAL] == 0xaa) )
		{
			return TRUE;
		}	
	}
	
	return FALSE;
}


//-----------------------------------------------
//函数功能: 判断通信地址是否正确
//
//参数:		Type 0--HCS 1--FCS  
//			 
//返回值:	
//		   
//备注:一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个暂时缓冲
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
	if( wData != fcs16( Buf , wLen ) ) //对报文进行校验
	{		
		return FALSE;
	}
		
	return TRUE;
}
//--------------------------------------------------
//功能描述:  进行698报文帧起始符68搜索
//         
//参数:      
//           TSerial * p[in]
//         
//返回值:    
//         
//备注内容:  无
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
//功能描述:  通信地址再判断
//         
//参数:      
//           TSerial * p[in]
//         
//返回值:    TRUE FALSE   
//         
//备注内容:  无
//--------------------------------------------------
BOOL rejudgeMailAddr(TSerial * p)
{
    BYTE control_byte  = 0;
    // 档案地址
    if (IsInSysMeters(&p->ProBuf[(p->BeginPosDlt698_45+5)%MAX_PRO_BUF_REAL]))
    {
        control_byte = p->ProBuf[(p->BeginPosDlt698_45+3)%MAX_PRO_BUF_REAL];
        //服务器响应帧，继续接收处理  //todo 要测试其他家的报文格式是否符合标准
        // if ((control_byte & 0xC0) == 0xC0)
        // {
        //     if (p == &Serial[eCR]) //载波口的接收放在1376.2规约中处理
        //     {
        //         return FALSE;
        //     }
        // }
        // else //客户机请求帧，认为是透传中包含的698帧，不进接收处理
        // {
        //     //return FALSE;
        // }
        return TRUE;
    }
	else if(MessageTransParaRam.AllowUnknownAddrTrans== TRUE)
	{
		return TRUE;
	}
    // else if( (p == &Serial[JLTxBuf.byDestNo])   //代理透传 户表返回数据
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
//功能描述:  进行698格式判断
//         
//参数:      BYTE ReceByte[in]
//         
//           TSerial * p[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
WORD DoReceProc_DLT698(BYTE ReceByte, TSerial * p)
{
	//搜索同步头
	if( p->ProStepDlt698_45 == 0 )
	{
		if(ReceByte == 0x68)
		{
			//Dlt698.45规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt698_45 = p->RXRPoint;
			//操作指针
			p->ProStepDlt698_45 ++;
		}
	}
	else if( p->ProBuf[p->BeginPosDlt698_45] == 0x68 ) 
	{
		//操作指针
		p->ProStepDlt698_45 ++;
		if( p->ProStepDlt698_45 < 3 )//如果没收到3字节，即长度域没收全，则不处理
		{
		}	
		else if( p->ProStepDlt698_45 == 3 )//已经收到第3个数了，即长度L
		{
			p->wLen = (ReceByte*0x100+p->ProBuf[p->BeginPosDlt698_45+1]);
			// if( p->wLen > (MAX_PRO_BUF-2) )//(MAX_PRO_BUF-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			if( p->wLen > (MAX_PRO_BUF_REAL-2) )//(MAX_PRO_BUF_REAL-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			if( p->wLen < (10+6) )//长度域最小为多少，通信地址是6字节
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
		}
		else if( p->ProStepDlt698_45 == 4 )//已经收到第4个数了，即控制域C
		{
// 			if( ReceByte & 0x80 )//如果是电表发出的，控制域 bit7:传输方向位：DIR=0表示此帧是由客户机发出的；DIR=1表示此帧是由服务器发出的。
// 			{
// 				DoSearch_68_DLT698(p);
// 				return FALSE;
// 			}
			//功能码判断，暂没做
			//1	链路管理	链路连接管理（登录，心跳，退出登录）
			//3	用户数据	应用连接管理及数据交换服务
			if( (ReceByte & 0x07) != 0x03 )//最好应答 ERROR-Response ，但矩泉陈兴东不应答鼎信测试软件过了 2017-7-29 wlk
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}			
		}
		else if( p->ProStepDlt698_45 == 5 )//已经收到第5个数了，即服务器地址SA的第1字节
		{
			//a)	bit0…bit3：为地址的字节数，取值范围：0…15，对应表示1…16个字节长度；
			//b)	bit4…bit5：逻辑地址； 
			//c)	bit6…bit7：为服务器地址的地址类型，0表示单地址，1表示通配地址，2表示组地址，3表示广播地址。
			p->Addr_Len = (ReceByte&0xf)+1;
			if( (ReceByte/0x40) <= 1 )//如果是单地址、通配地址
			{
				if( p->Addr_Len != 6 )//电表地址是6字节BCD，即12位BCD码
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}	
			}	
		}
		else if( p->ProStepDlt698_45 == (8+p->Addr_Len) )//地址收完了，HCS收完了，判断HCS是否正确
		{
			if( p->Addr_Len > 6 )
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}
			//一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if( JudgeDlt698_45_HCS_FCS(0,p) == FALSE )	
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}

//			if( p == &Serial[eIR] )//如果当前通道为红外通道
//			{
//                api_SetLcdCommunicationFlag( eIR );//置通讯标志
//			}
			#if (SEL_SEARCH_METER == YES )
            if ( (p == &Serial[SEARCH_METER_CH]) 
              &&( SKMeter.byAutoCheck != 0 ) 
              &&( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
              ) // 搜表通道不判断
            {
                return FALSE;
            }
			else 
            #endif
			//判断通信地址是否正确
			if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			{
                //判断是否为其他模块用到的地址
               if(rejudgeMailAddr(p) == FALSE)
               {
                    DoSearch_68_DLT698(p);
               }
                return FALSE;
			}
			else
			{
				BYTE control_byte = p->ProBuf[(p->BeginPosDlt698_45+3)%MAX_PRO_BUF_REAL];
                //如果是电表发出的，控制域 bit7:传输方向位：DIR=0表示此帧是由客户机发出的；DIR=1表示此帧是由服务器发出的。
				if( control_byte & 0x80 )
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}
			}
		}
		else if( p->ProStepDlt698_45 == (1+p->wLen) )//收到帧结束符 16H了，判断FCS是否正确
		{
			//一口多规约情况下，缓冲为循环缓冲，算校验前要先把缓冲中数据移到一个昨时缓冲
			if( JudgeDlt698_45_HCS_FCS(1,p) == FALSE )	
			{
				DoSearch_68_DLT698(p);
				return FALSE;
			}	
		}	
		else if( p->ProStepDlt698_45 == (2+p->wLen) )//收到帧结束符 16H了,暂没判断16H是否接收正确
		{
			if( ReceByte == 0x16 )
			{	
				BYTE byAck = 0;

				//禁止接收
				api_DisableRece( p );
				
				//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				DoReceMoveData(PRO_DLT698_45, p);//最好放在 698.45协议处理函数 中
	
			//	此处调用698.45协议处理函数//ProcDlt698App();//ProMessage( p );				

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
				//处理完成
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
			// if( p->ProStepDlt698_45 >= MAX_PRO_BUF )//(MAX_PRO_BUF-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			if( p->ProStepDlt698_45 >= MAX_PRO_BUF_REAL )//(MAX_PRO_BUF_REAL-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
			{
				p->ProStepDlt698_45 = 0;
				return FALSE;
			}
		}	
			
	}		
		
	return FALSE;
}
//--------------------------------------------------
//功能描述:  填充6981链路层头函数
//         
//参数:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void AddDLT698LinkDataHead( BYTE Ch,BYTE *Buf, TTwoByteUnion* Lenth )
{	
	BYTE i;
	TTwoByteUnion *Len;
	
	Len = Lenth;
        
	Buf[0] =0x68;	//起始字符
					//长度未填写
	Buf[3] = (LinkData[Ch].ControlByte|0x80); //控制域
	Buf[4] = 0x05; //地址长度
	for( i=0; i<LinkData[Ch].AddressLen; i++ )
	{
		Buf[5+i] = LinkData[Ch].ServerAddr[i];//服务器地址，即电表地址(更改为这个是因为有充电桩业务报文)
	}
	Buf[5+LinkData[Ch].AddressLen] = LinkData[Ch].ClientAddress;
							      //校验未填写
	Len->b[0] = (DLT_LINK_DATA_HEAD+LinkData[Ch].AddressLen);
}
//--------------------------------------------------
//功能描述:  填充6981链路层头函数
//         
//参数:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void AddDLT698LinkDataHeadZD( BYTE Ch,BYTE *Buf, TTwoByteUnion* Lenth )
{	
	BYTE i;
	TTwoByteUnion *Len;
	
	Len = Lenth;
        
	Buf[0] =0x68;	//起始字符
					//长度未填写
	Buf[3] = (LinkData[Ch].ControlByte); //控制域
	Buf[4] = 0x05; //地址长度
	for( i=0; i<LinkData[Ch].AddressLen; i++ )
	{
		Buf[5+i] = LinkData[Ch].ServerAddr[i];//服务器地址，即电表地址(更改为这个是因为有充电桩业务报文)
	}
	Buf[5+LinkData[Ch].AddressLen] = LinkData[Ch].ClientAddress;
							      //校验未填写
	Len->b[0] = (DLT_LINK_DATA_HEAD+LinkData[Ch].AddressLen);
}
//--------------------------------------------------
//功能描述:  添加698链路层尾函数
//         
//参数:      BYTE Ch[in]
//         
//           BYTE *buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void AddDLT698LinkDataTail(BYTE Ch, BYTE *buf, TTwoByteUnion* Lenth)
{
    WORD i;
	TTwoByteUnion Temp16;
	TTwoByteUnion *Len;
	
	Len = Lenth;
	
	Len->w += (DLT_LINK_DATA_TAIL-2);//长度 不包括起始字符+结束字符
	buf[1] = Len->b[0]; //数据长度
	buf[2] = Len->b[1];
	Len->w += 2;//更新总数据长度
	
	Temp16.w = fcs16(&buf[1],(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_START_BYTE-DLT_CSLEN));	//不包括起始地址和校验码
	buf[(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_CSLEN)] = Temp16.b[0];  //添加hrc
	buf[(LinkData[Ch].AddressLen+DLT_LINK_DATA_HEAD-DLT_CSLEN+1)] = Temp16.b[1];
	
	if( buf[3] & 0x08 )//LinkData[Ch].ControlByte bit3:扰码标志位SC
	{
		i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6时i为15
		i += 2;//因前有：Len->w += 2;//更新总数据长度
        if( Len->w > i )
		{
			Temp16.w = Len->w - i;
			for( i=0; i<Temp16.w; i++ )
			{
				buf[LinkData[Ch].AddressLen +DLT_LINK_DATA_HEAD+i] += 0x33;
			}
		}
	}

	Temp16.w = fcs16(&buf[1],(Len->w -DLT_CSLEN-DLT_START_BYTE-DLT_END_BYTE));	//添加FCS
	buf[(Len->w-3)]= Temp16.b[0];
	buf[Len->w-2] = Temp16.b[1];
	buf[Len->w-1] = 0x16;

}


//-----------------------------------------------
//函数功能: 链路层分帧处理函数
//
//参数:		BYTE *pControlByte[in]
//			 
//返回值:	BYTE
//		   
//备注:
//-----------------------------------------------
static eFrameResultChoice Proc698LinkDataRequestFrame(BYTE Ch, BYTE ControlByte, BYTE *pAPDU )
{
	TTwoByteUnion Follow_Len;
	
	if( ControlByte & 0x20)//如果是后续帧
	{	
		if( (pAPDU[1]&0xc0) != 0x80 )//不是确认帧
		{

            if( (APDUBufFlag.Request.Flag == TRUE) && ( APDUBufFlag.Request.Ch != Ch ))//通道buf被占用且通道不一致
            {
                return eLINKDATA_FAIL;
            }

            APDUBufFlag.Request.Ch = Ch;
            APDUBufFlag.Request.Flag = TRUE;

            LinkData[Ch].DownLinkFrame.pFrame = DLT698APDURequestBuf;//初始化下行链路层分帧指针

            if( (pAPDU[1]&0xc0) == 0x00 )//分帧起始标志
            {
            	APPData[Ch].APPFlag = 0;//清除应用层分帧标志，只能存在一种分帧格式
                LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//只支持1个字节的分帧次数
                if( LinkData[Ch].DownLinkFrame.FrameType.b[0] == 0 ) //起始标志第1帧必须为零
                {	//应用层数据长度 = 总长度 - 链路层数据头 - 链路层数据尾 -地址长度 - 分帧控制字
                    Follow_Len.w = (LinkData[Ch].DataLen.w-DLT_LINK_DATA_HEAD-DLT_LINK_DATA_TAIL-LinkData[Ch].AddressLen-DLT_FRAME_BYTE+2);

                    if( Follow_Len.w > MAX_APDU_SIZE )//进行最大值判断
                    {
                        return eLINKDATA_FAIL;
                    }

                    memcpy(LinkData[Ch].DownLinkFrame.pFrame,pAPDU+2,Follow_Len.w);//保存数据
                    LinkData[Ch].DownLinkFrame.FrameLen.w = Follow_Len.w;//保存已填充数据长度

                    LinkData[Ch].DownLinkFrame.FrameType.b[1] = 0x80;//添加响应控制字节【1】
                    return eDOWN_FRAMEING;//下行分帧结束
                }
                else
                {
                    return eLINKDATA_FAIL;
                }
            }
            else
            {
                if( LinkData[Ch].DownLinkFrame.FrameType.b[0] == (pAPDU[0]-1) ) //服务器发送的帧序号应比接受到的加1
                {
                    LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//只支持1个字节的分帧次数
                    //应用层数据长度 = 总长度 - 链路层数据头 - 链路层数据尾 -地址长度 - 分帧控制字
                    Follow_Len.w = (LinkData[Ch].DataLen.w-DLT_LINK_DATA_HEAD-DLT_LINK_DATA_TAIL-LinkData[Ch].AddressLen-DLT_FRAME_BYTE+2);

                    if( (LinkData[Ch].DownLinkFrame.FrameLen.w+Follow_Len.w) > MAX_APDU_SIZE )//进行最大值判断
                    {
                        return eLINKDATA_FAIL;
                    }

                    memcpy(&LinkData[Ch].DownLinkFrame.pFrame[LinkData[Ch].DownLinkFrame.FrameLen.w],pAPDU+2,Follow_Len.w);//保存数据
                    LinkData[Ch].DownLinkFrame.FrameLen.w += Follow_Len.w;//保存已填充数据长度

                    LinkData[Ch].DownLinkFrame.FrameType.b[0] = pAPDU[0];//添加响应控制字节【0】
                    LinkData[Ch].DownLinkFrame.FrameType.b[1] = 0x80;//添加响应控制字节【1】
                }
                else
                {
                    return eLINKDATA_FAIL;
                }
                
                if( (pAPDU[1]&0xc0) == 0x40 )
                {
                    LinkData[Ch].ControlByte &= 0xdf;
                    return eDOWN_FRAME_END;//下行分帧结束
                }
                
                return eDOWN_FRAMEING;//下行分帧中
            }
		}
		else //分帧确认帧-提前处理，避免处理
		{
        	if( pAPDU[0] == LinkData[Ch].UpLinkFrame.FrameType.b[0] )//确认帧与上行分帧相等
			{
				return eUP_FRAMEING;//上行分帧中
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
//功能描述:  应答链路层分帧函数
//         
//参数:      BYTE Ch[in]
//         
//           BYTE* Buf[in]
//         
//           TTwoByteUnion* Lenth[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static eFrameResultChoice Proc698LinkDataResponseFrame(BYTE Ch ,BYTE* Buf, TTwoByteUnion* Lenth)
{
	TTwoByteUnion Len1,TotalLen;
	TTwoByteUnion *Len;
	DWORD ConsultLen;
	
	Len = Lenth;
	
	if( (APPData[Ch].APPFlag & APP_LINK_FRAME) == APP_LINK_FRAME )//应用层数据需要链路层分帧
	{
	    ConsultLen = GetConsultBufLen( Ch );//获取当前通道协商长度
	    
		LinkData[Ch].UpLinkFrame.pFrame = &Buf[Len->w];//初始化指针
		Buf[3] |= 0x20;//置位分帧控制域 
		if( LinkData[Ch].eLinkDataFlag == eAPP_OK )//链路层分帧首帧
		{
			LinkData[Ch].UpLinkFrame.FrameType.b[0] = 0;//发送第1帧
			LinkData[Ch].UpLinkFrame.FrameType.b[1] = 0;//起始帧
			//计算分帧数据长度
			LinkData[Ch].UpLinkFrame.FrameLen.w = ( ConsultLen - DLT_LINK_DATA_HEAD -DLT_LINK_DATA_TAIL -LinkData[Ch].AddressLen -2 );//-2是分帧格式域

            if( LinkData[Ch].UpLinkFrame.FrameLen.w > APPData[Ch].APPBufLen.w )//第一帧buf长度不应小于数据总长度
            {
               return eLINKDATA_FAIL;//分帧失败
            }
			//组帧数据
			LinkData[Ch].UpLinkFrame.pFrame[0] = LinkData[Ch].UpLinkFrame.FrameType.b[0];//分帧格式域
			LinkData[Ch].UpLinkFrame.pFrame[1] = LinkData[Ch].UpLinkFrame.FrameType.b[1];

		    if( (Len->w+LinkData[Ch].UpLinkFrame.FrameLen.w) > MAX_PRO_BUF )//数据长度不能超过MAX_PRO_BUF
	        {
                return eLINKDATA_FAIL;
    		}
    		
			memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf,LinkData[Ch].UpLinkFrame.FrameLen.w);
			Len->w += (LinkData[Ch].UpLinkFrame.FrameLen.w+2);//返回数据长度
			return eUP_FRAMEING;//分帧中
		}
		else if( LinkData[Ch].eLinkDataFlag == eUP_FRAMEING)
		{
			LinkData[Ch].UpLinkFrame.FrameType.b[0] =LinkData[Ch].UpLinkFrame.FrameType.b[0]+1;//发送下一帧
			LinkData[Ch].UpLinkFrame.FrameType.b[1] = 0xc0;//起始帧

			//组帧数据
			LinkData[Ch].UpLinkFrame.pFrame[0] = LinkData[Ch].UpLinkFrame.FrameType.b[0];//分帧格式域
			LinkData[Ch].UpLinkFrame.pFrame[1] = LinkData[Ch].UpLinkFrame.FrameType.b[1];

            TotalLen.w = (LinkData[Ch].UpLinkFrame.FrameLen.w)*(LinkData[Ch].UpLinkFrame.FrameType.b[0]);

			if( APPData[Ch].APPBufLen.w < TotalLen.w )//数据总长度不应小于已传输的数据
			{
                return eLINKDATA_FAIL;//分帧失败
			}
			
			if( ( APPData[Ch].APPBufLen.w-TotalLen.w ) > LinkData[Ch].UpLinkFrame.FrameLen.w)
			{			
    		    if( (Len->w+LinkData[Ch].UpLinkFrame.FrameLen.w) > MAX_PRO_BUF )//数据长度不能超过MAX_PRO_BUF
    	        {
                    return eLINKDATA_FAIL;
        		}
        		
				memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf+TotalLen.w,LinkData[Ch].UpLinkFrame.FrameLen.w);
				Len->w += (LinkData[Ch].UpLinkFrame.FrameLen.w+2);//返回数据长度
			}
			else
			{		
				if( Ch == eCR )//分帧结束 清零分帧延时
	        	{
	        		FrameOverTime = 0;
	        	}
	        	
				LinkData[Ch].UpLinkFrame.pFrame[1] = 0x40;//结束帧
				APPData[Ch].APPFlag = 0;//清除分帧标志
				Len1.w = (APPData[Ch].APPBufLen.w -TotalLen.w);//获取buf长度

			    if( (Len->w+Len1.w) > MAX_PRO_BUF )//数据长度不能超过MAX_PRO_BUF
    	        {
                    return eLINKDATA_FAIL;
        		}
        		
				memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf+TotalLen.w,Len1.w);//发送剩余帧
				Len->w += Len1.w+2;//返回数据长度
				return eUP_FRAME_END;//分帧结束
			}
	
			return eUP_FRAMEING;//分帧中
		}
		else
		{
			return eLINKDATA_FAIL;//分帧失败
		}
		
	}
	else if( LinkData[Ch].eLinkDataFlag == eDOWN_FRAMEING )
	{
		Buf[Len->w] = LinkData[Ch].DownLinkFrame.FrameType.b[0];
		Buf[Len->w+1] = LinkData[Ch].DownLinkFrame.FrameType.b[1];
		Len->w +=2;
		return eDOWN_FRAMEING;//分帧中
	}
	else
	{
		return eNO_FRAME;//无分帧
	}

}
//--------------------------------------------------
//功能描述:  链路层请求处理函数
//         
//参数:      BYTE Ch[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void Proc698LinkDataRequest( BYTE Ch )
{	
	WORD i,j;
	TTwoByteUnion LEN;
	BYTE *pAPDU;
	BYTE GenericAddress[6];

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
//	if( Ch != eIR )//红外通道在判断帧格式时已进行置位
//	{
//	    api_SetLcdCommunicationFlag( Ch );//置通讯标志
//	}
	
	LEN.b[0] = Serial[Ch].ProBuf[1];
	LEN.b[1] = Serial[Ch].ProBuf[2];

	if( LEN.w > MAX_PRO_BUF )
	{
        LinkData[Ch].eLinkDataFlag = eLINKDATA_FAIL;
	}
	else
	{
        LinkData[Ch].DataLen.w = LEN.w; //填充数据长度
        
        LinkData[Ch].ControlByte = Serial[Ch].ProBuf[3];//填充控制字节
        LinkData[Ch].AddressType = (eAddressType)(Serial[Ch].ProBuf[4]>>6);
        LinkData[Ch].AddressLen = Serial[Ch].Addr_Len;//填充地址长度
        memcpy(&LinkData[Ch].ServerAddr[0],&Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1],LinkData[Ch].AddressLen);

		//全88地址
		memset(GenericAddress,0x88,sizeof(GenericAddress));
		if (memcmp(LinkData[Ch].ServerAddr,GenericAddress,sizeof(GenericAddress))==0)
		{
			lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
		}
		else//有通缩符则恢复系统地址
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
            
        if( LinkData[Ch].ControlByte & 0x08 )//bit3:扰码标志位SC
        {
            i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6时i为15
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
        	if( Ch == eCR )//接受完整数据 清零分帧延时
        	{
        		FrameOverTime = 0;
        	}

            LinkData[Ch].eLinkDataFlag = eAPP_OK;
            LinkData[Ch].pAPP = &Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN];
            LinkData[Ch].pAPPLen.w = (LinkData[Ch].DataLen.w -(DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_CSLEN) );
        }
        else if( LinkData[Ch].eLinkDataFlag == eDOWN_FRAME_END )//下行分帧结束
        {
            if( Ch == eCR )//接受完整数据 清零分帧延时
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
//功能描述:  链路层请求处理函数
//         
//参数:      BYTE Ch[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void Proc698LinkDataRequestForZD( BYTE Ch, BYTE *pBuf, WORD wLen )
{	
	WORD i,j;
	TTwoByteUnion LEN;
	BYTE *pAPDU;
	BYTE GenericAddress[6];

    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return;
    }
    
	memcpy(Serial[Ch].ProBuf,pBuf,wLen);
	Serial[Ch].SendLength = wLen;
	
//	if( Ch != eIR )//红外通道在判断帧格式时已进行置位
//	{
//	    api_SetLcdCommunicationFlag( Ch );//置通讯标志
//	}
	
	LEN.b[0] = Serial[Ch].ProBuf[1];
	LEN.b[1] = Serial[Ch].ProBuf[2];

	//if( LEN.w > MAX_PRO_BUF )
	//{
    //    LinkData[Ch].eLinkDataFlag = eLINKDATA_FAIL;
	//}
	//else
	{
        LinkData[Ch].DataLen.w = LEN.w; //填充数据长度
        
        LinkData[Ch].ControlByte = Serial[Ch].ProBuf[3];//填充控制字节
        LinkData[Ch].AddressType = (eAddressType)(Serial[Ch].ProBuf[4]>>6);
        LinkData[Ch].AddressLen = Serial[Ch].Addr_Len;//填充地址长度
        memcpy(&LinkData[Ch].ServerAddr[0],&Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+1],LinkData[Ch].AddressLen);

		//全88地址
		memset(GenericAddress,0x88,sizeof(GenericAddress));
		if (memcmp(LinkData[Ch].ServerAddr,GenericAddress,sizeof(GenericAddress))==0)
		{
			lib_ExchangeData(LinkData[Ch].ServerAddr,FPara1->MeterSnPara.CommAddr,sizeof(FPara1->MeterSnPara.CommAddr));
		}
		else//有通缩符则恢复系统地址
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
            
        if( LinkData[Ch].ControlByte & 0x08 )//bit3:扰码标志位SC
        {
            i = (DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_FRAME_BYTE);//AddressLen=6时i为15
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
    
    	if( Ch == eCR )//接受完整数据 清零分帧延时
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
//功能描述:  链路层数据响应函数
//         
//参数:      BYTE Ch[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
BYTE Proc698LinkDataResponse(BYTE Ch)
{
	BYTE Buf[MAX_PRO_BUF+EXTRA_BUF];
	eFrameResultChoice eResultChoice;
	TTwoByteUnion Len;
	
    if( Ch >= MAX_COM_CHANNEL_NUM )//进行通道判断
    {
        return FALSE;
    }
    
	Len.w = 0; 
	if( ((APPData[Ch].APPFlag&APP_HAVE_DATA) != APP_HAVE_DATA) && (LinkData[Ch].eLinkDataFlag == eAPP_OK))//应用层无数据返回
	{
		api_InitSci( Ch );
		return FALSE;
	}
	if( (LinkData[Ch].eLinkDataFlag == eLINKDATA_FAIL) || (LinkData[Ch].AddressType == eBROADCAST_ADDRESS_MODE) )//链路层出现错误或者广播地址不应答
	{
		api_InitSci( Ch );
		return FALSE;
	}
	
	AddDLT698LinkDataHead(Ch,Buf,&Len); //填充帧头
	eResultChoice = Proc698LinkDataResponseFrame( Ch ,Buf, &Len);//判断后续帧
	if( eResultChoice == eNO_FRAME )//无后续帧
	{
	    if( (Len.w+APPData[Ch].APPBufLen.w) > MAX_PRO_BUF)//数据大于buf长度 异常
	    {
            api_InitSci( Ch );
            return FALSE;
	    }
	    else
	    {
            memcpy( &Buf[Len.w],APPData[Ch].pAPPBuf,APPData[Ch].APPBufLen.w);//组帧应用层buf
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
		if( Ch == eCR )//载波通道
		{
			//分帧进行中 置位分帧延时
			if( (eResultChoice == eDOWN_FRAMEING) || (eResultChoice == eUP_FRAMEING))
			{
				FrameOverTime = 60;//处于分帧状态，无法下行分帧还是上行分帧都重新刷新1分钟延时
			}

		}
	}
	
	AddDLT698LinkDataTail(Ch, Buf,&Len);//填充帧尾

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
	
	//执行延时
	Serial[Ch].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;

	SerialMap[Ch].SCIEnableSend(SerialMap[Ch].SCI_Ph_Num); 
    return TRUE;
}

//--------------------------------------------------
//功能描述:  上报链路层数据函数
//         
//参数:     pAPDU[IN]	 //apdu 指针
//		  APDU_Len[IN]//apdu 长度 无需进行极限值判断
//        Type[IN]//类型：0X00-明文+MAC 0x55-明文
//返回值:    
//         
//备注内容:  无
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
		memcpy( RN_MAC_BAK, RN_MAC, sizeof(RN_MAC_BAK) );//备份当前rn_mac
		Result = api_DealEsamSafeReportRN_MAC( APDU_Len, pAPDU, RN_MAC);//获取RN_MAC
		if( Result == 0x8000 )//失败后 上报全FF
		{
			memset( RN_MAC, 0xff, sizeof(RN_MAC) );
		}
		
		//组帧安全传输
		Len2.w = APDU_Len;//用来应用数据-安全传输处理
		Buf1[0] = 0x10;//安全传输
		Buf1[1] = 0x00;
		Len1.w = 2;
		Len1.w += Deal_698DataLenth( &Buf1[2],Len2.b,eUNION_TYPE);;//获取明密文数据的偏移长度
		memcpy( &Buf1[Len1.w], pAPDU, APDU_Len );
		Len1.w += APDU_Len;//填充安全传输内容
		Buf1[Len1.w] = 0x02;//选择rn_mac
		Buf1[Len1.w+1] = 12;//随机数长度为12
		memcpy( &Buf1[Len1.w+2], RN_MAC, 12 );//组帧rn
		Len1.w +=14;
		Buf1[Len1.w] = 4;//mac长度为4
		memcpy( &Buf1[Len1.w+1], RN_MAC+12, 4 );//组帧mac
		Len1.w +=5;
		

	}
	else
	{
		memcpy( (BYTE*)&Buf1, pAPDU, APDU_Len );//组帧apdu
		Len1.w = APDU_Len;
	}
	if( Len1.w > 450 )//极限值判断
	{
		return ;
	}
	Buf[0] =0x68;	//起始字符
					//长度未填写
	Buf[3] = 0x83; //控制域
	Buf[4] = 0x05; //地址长度
	for( i=0; i<6; i++ )
	{
		Buf[5+i] = FPara1->MeterSnPara.CommAddr[5-i];//服务器地址，即电表地址
	}
	Buf[5+6] = 0x00;//协议规定 电表客户机地址回0x00
	Len.w = (DLT_LINK_DATA_HEAD+6);//帧头实际长度
	
	memcpy( (BYTE*)&Buf[Len.w], Buf1, Len1.w );//组帧apdu
	Len.w += Len1.w;
	
	Len.w += (DLT_LINK_DATA_TAIL-2);//长度 不包括起始字符+结束字符
	Buf[1] = Len.b[0]; //数据长度
	Buf[2] = Len.b[1];
	Len.w += 2;//更新总数据长度
	Temp16.w = fcs16(&Buf[1],(6+DLT_LINK_DATA_HEAD-DLT_START_BYTE-DLT_CSLEN));	//不包括起始地址和校验码
	Buf[(6+DLT_LINK_DATA_HEAD-DLT_CSLEN)] = Temp16.b[0];  //添加hrc
	Buf[(6+DLT_LINK_DATA_HEAD-DLT_CSLEN+1)] = Temp16.b[1];

	Temp16.w = fcs16(&Buf[1],(Len.w -DLT_CSLEN-DLT_START_BYTE-DLT_END_BYTE));	//添加FCS
	Buf[(Len.w-3)]= Temp16.b[0];
	Buf[Len.w-2] = Temp16.b[1];
	Buf[Len.w-1] = 0x16;

	memset( (void *)Serial[eCR].ProBuf, 0xfe, 4 );
	memcpy( (Serial[eCR].ProBuf+4),Buf,Len.w);
	Serial[eCR].SendLength = (Len.w+4);
	//执行延时
	Serial[eCR].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	Serial[eCR].RxOverCount = 500;//启动500ms接受延时 - 取巧 使用字节间延时进行判断 不需要新增加延时 而且可以解决接受数据时会上报的问题
	SerialMap[eCR].SCIEnableSend(SerialMap[eCR].SCI_Ph_Num); 
	
	//测试用
	/*memset( (void *)Serial[eRS485_I].ProBuf, 0xfe, 4 );
	memcpy( (Serial[eRS485_I].ProBuf+4),Buf,Len.w);
	Serial[eRS485_I].SendLength = (Len.w+4);
	//执行延时
	Serial[eRS485_I].ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;
	Serial[eRS485_I].RxOverCount = 500;//启动500ms接受延时 - 取巧 使用字节间延时进行判断 不需要新增加延时 而且可以解决接受数据时会上报的问题
	SerialMap[eRS485_I].SCIEnableSend(SerialMap[eRS485_I].SCI_Ph_Num);*/
}


#endif//(SEL_DLT698_2016_FUNC == YES )
