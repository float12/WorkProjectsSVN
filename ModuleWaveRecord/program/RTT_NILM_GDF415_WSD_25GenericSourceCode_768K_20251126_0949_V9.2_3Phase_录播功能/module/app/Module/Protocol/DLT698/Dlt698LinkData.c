//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	
//创建日期	2016.9.3
//描述		
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------


//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------


//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
extern __no_init TNAddr_t NAddr;
//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------
const DWORD LegalOITable[1] = {0x000FEB20}; 

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


//-----------------------------------------------
//函数功能: 判断通信地址是否正确
//
//参数:		  
//			 
//返回值:	
//		   
//备注:
//----------------------------------------------- 
static WORD JudgeRecMeterNo_Dlt698_45(TSerial *p)
{
	BYTE ucData;
			
	//a)	bit0…bit3：为地址的字节数，取值范围：0…15，对应表示1…16个字节长度；
	//b)	bit4…bit5：逻辑地址； 
	//c)	bit6…bit7：为服务器地址的地址类型，0表示单地址，1表示通配地址，2表示组地址，3表示广播地址。
	ucData = p->ProBuf[(p->BeginPosDlt698_45+4)%MAX_PRO_BUF];//通信缓冲大小最后用  MAX_PRO_BUF 
	//暂不严格判断逻辑地址是管理芯，还是计量芯的
	//双芯电能表管理芯逻辑地址为0，计量芯逻辑地址为1，除此之外的逻辑地址为非法
	//if( ((ucData>>4)&0x3) > 1 )
	//{
	//	return FALSE;
	//}
	
	ucData = (ucData/0x40);//得到地址类型

	if( (ucData == 1) || ( ucData == 0 ))//如果是单地址、通配地址
	{
		if( (p->Addr_Len == 6) || (p->Addr_Len == 7) )
		{
			return TRUE;
		}
	}
	else if( ucData == 3 )//广播地址
	{
		if( (p->Addr_Len == 1)&&(p->ProBuf[(p->BeginPosDlt698_45+5)%MAX_PRO_BUF] == 0xaa) )
		{
			return TRUE;
		}
                else if( (p->Addr_Len == 2)&&(p->ProBuf[(p->BeginPosDlt698_45+6)%MAX_PRO_BUF] == 0xaa) )
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
	BYTE pAllocBuf[MAX_PRO_BUF+30];
	
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
	if( wLen >= (MAX_PRO_BUF-1) )
	{
		return FALSE;
	}			

	for( i=0; i<wLen; i++ )
	{
		 pAllocBuf[i] = p->ProBuf[(p->BeginPosDlt698_45+1+i)%MAX_PRO_BUF];
	}

	//68 L L 43 05 01 00 29 01 16 20 0A HCS_L HCS_H 10 00 08 05 01 01 40 01 02 00 00 00 85 01 02 03 06 12 34 56 78 90 12 04 12 34 56 78 FCS_L FCS_H 16 
	wData = p->ProBuf[(p->BeginPosDlt698_45+p->ProStepDlt698_45-1)%MAX_PRO_BUF]*0x100+p->ProBuf[(p->BeginPosDlt698_45+p->ProStepDlt698_45-2)%MAX_PRO_BUF]; 
	if( wData != fcs16( pAllocBuf , wLen ) ) //对报文进行校验
	{		
		return FALSE;
	}	// 释放缓冲
	
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
		if( p->ProBuf[(p->BeginPosDlt698_45+i)%MAX_PRO_BUF] == 0x68 )
		{
			if( p->ProStepDlt698_45 == 0 )
			{
				p->ProStepDlt698_45 = MAX_PRO_BUF - 1;
			}
			else
			{
				p->ProStepDlt698_45--;
			}
            p->BeginPosDlt698_45 += i;
            if( p->BeginPosDlt698_45 >= MAX_PRO_BUF )
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
    BYTE DefAddrLen = 6;//通信地址默认长度
	WORD i,j;
//    BOOL Res;
	
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
			if( p->wLen > (MAX_PRO_BUF-2) )//(MAX_PRO_BUF-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
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
			//@@@@存在解析应答帧的工况--lzq
//			if( ReceByte & 0x80 )//如果是电表发出的，控制域 bit7:传输方向位：DIR=0表示此帧是由客户机发出的；DIR=1表示此帧是由服务器发出的。
//			{
//				DoSearch_68_DLT698(p);
//				return FALSE;
//			}
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
			    if( (ReceByte & 0x30) > 0x10 )
			    {
			        //有扩展逻辑地址
			        DefAddrLen = 7;
			    }

				if( p->Addr_Len != DefAddrLen )//电表地址是6或7字节（有扩展逻辑地址时为7）BCD，即12位BCD码
				{
					DoSearch_68_DLT698(p);
					return FALSE;
				}	
			}	
		}
		else if( p->ProStepDlt698_45 == (8+p->Addr_Len) )//地址收完了，HCS收完了，判断HCS是否正确
		{
			if( p->Addr_Len > 7 )
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
			
			//判断通信地址是否正确
			if( JudgeRecMeterNo_Dlt698_45(p) == FALSE )
			{
				DoSearch_68_DLT698(p);
				return FALSE;
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
			
				//禁止接收
                USART_Count.RxCount++;
				api_DisableRece( p );
				
				//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				DoReceMoveData(PRO_DLT698_45, p);//最好放在 698.45协议处理函数 中
	
			//	此处调用698.45协议处理函数//ProcDlt698App();//ProMessage( p );
				
				//测试用：报文打印
				#if(USE_PRINTF == 1)
				if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
				{
					rt_kprintf("\nRX: ");
					j = 0;
					for(i = 0;i<Serial->ProStepDlt698_45;i++)
					{
						rt_kprintf("%02X ",Serial->ProBuf[i]);
						j++;
						if(j>100)
						{
							rt_kprintf("\n");
							j = 0;
						}
					}
					rt_kprintf("\n");
				}
				#endif
				ProcMessageDlt698(p);

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
			if( p->ProStepDlt698_45 >= MAX_PRO_BUF )//(MAX_PRO_BUF-2) )//用户数据长度：由bit0～bit13组成，采用BIN编码，是传输帧中除起始字符和结束字符之外的帧字节数。
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
	BYTE i,Addr,Loop;
	TTwoByteUnion *Len;
	
	Len = Lenth;
        
	Buf[0] =0x68;	//起始字符
					//长度未填写
	Buf[3] = (LinkData[Ch].ControlByte | 0x80); //控制域

	if( LinkData[Ch].LogicAddr == LOGIC_ADDR_JL )
	{
		Buf[4] = 0x10; //逻辑地址
		Addr = 5;
		Loop = LinkData[Ch].AddressLen;
	}
	else if( LinkData[Ch].LogicAddr == LOGIC_ADDR_GL )
	{
		Buf[4] = 0x00; //逻辑地址
		Addr = 5;
		Loop = LinkData[Ch].AddressLen;
	}
	else
	{
		Buf[4] = 0x20; //扩展逻辑地址
		Buf[5] =  LinkData[Ch].LogicAddr;
		Addr = 6;
		Loop = LinkData[Ch].AddressLen - 1;
	}
	Buf[4] |= (eSINGLE_ADDRESS_TYPE << 6);			//地址类型:单地址
	Buf[4] |= ((LinkData[Ch].AddressLen-1)&0x0F); 	//地址长度

	for( i = 0; i < Loop; i++ )
	{
		Buf[Addr+i] = ManageComInfo.Address[Loop-1-i];//服务器地址，即电表地址
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
			APPData[Ch].APPFlag = 0;//清除应用层分帧标志，只能存在一种分帧格式
            if( (APDUBufFlag.Request.Flag == TRUE) && ( APDUBufFlag.Request.Ch != Ch ))//通道buf被占用且通道不一致
            {
                return eLINKDATA_FAIL;
            }

            APDUBufFlag.Request.Ch = Ch;
            APDUBufFlag.Request.Flag = TRUE;

            LinkData[Ch].DownLinkFrame.pFrame = DLT698APDURequestBuf;//初始化下行链路层分帧指针

            if( (pAPDU[1]&0xc0) == 0x00 )//分帧起始标志
            {
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
				LinkData[Ch].UpLinkFrame.pFrame[1] = 0x40;//结束帧
				APPData[Ch].APPFlag = 0;//清除分帧标志
				Len1.w = (APPData[Ch].APPBufLen.w -TotalLen.w);//获取buf长度
				
			    if( (Len->w+Len1.w) > MAX_PRO_BUF )//数据长度不能超过MAX_PRO_BUF
    	        {
                    return eLINKDATA_FAIL;
        		}
        		
				memcpy( &LinkData[Ch].UpLinkFrame.pFrame[2],APPData[Ch].pAPPBuf+TotalLen.w,Len1.w);//发送剩余帧
				Len->w += Len1.w+2;//返回数据长度
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
	WORD i;
	TTwoByteUnion LEN;
	BYTE *pAPDU;
	BYTE LogicAddr;
	BYTE AddrIndex,MeterAddrLen;
	
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

		//逻辑地址
		LogicAddr = (Serial[Ch].ProBuf[4]&(3U << 4)) >> 4;
		if( LogicAddr == LOGIC_ADDR_EXT )
		{
			//扩展逻辑地址
			LinkData[Ch].LogicAddr = Serial[Ch].ProBuf[5];
			AddrIndex = 6;
			MeterAddrLen = LinkData[Ch].AddressLen - 1;
		}
		else
		{
			//逻辑地址
			LinkData[Ch].LogicAddr = LogicAddr;
			AddrIndex = 5;
			MeterAddrLen = LinkData[Ch].AddressLen;
		}

		//wxy 地址按收到地址的返回 if(( LinkData[Ch].AddressType == eSINGLE_ADDRESS_TYPE )&&( ManageComInfo.AddressType == 0x00 ))
		{
			//@@@@未获取地址时，模块回复的地址？？
			for( i = 0; i < MeterAddrLen; i++ )
			{
				ManageComInfo.Address[MeterAddrLen-1-i] = Serial[Ch].ProBuf[AddrIndex+i];//电表地址
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
            LinkData[Ch].eLinkDataFlag = eAPP_OK;
            LinkData[Ch].pAPP = &Serial[Ch].ProBuf[DLT_START_BYTE+DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN];
            LinkData[Ch].pAPPLen.w = (LinkData[Ch].DataLen.w -(DLT_LENTH+DLT_CONTROL_BYTE+DLT_ADDRESS_ELSE_LEN+LinkData[Ch].AddressLen+DLT_CSLEN+DLT_CSLEN) );
        }
        else if( LinkData[Ch].eLinkDataFlag == eDOWN_FRAME_END )//下行分帧结束
        {
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
	
	AddDLT698LinkDataTail(Ch, Buf,&Len);//填充帧尾
#if 0
	memset( (void *)Serial[Ch].ProBuf, 0xfe, 4 );
	memcpy( (Serial[Ch].ProBuf+4),Buf,Len.w);
	Serial[Ch].SendLength = (Len.w+4);
#else
	//memset( (void *)Serial[Ch].ProBuf, 0xfe, 4 );
	memcpy( (Serial[Ch].ProBuf+0),Buf,Len.w);
	Serial[Ch].SendLength = (Len.w+0);
#endif
	//执行延时
	Serial[Ch].ReceToSendDelay = 100;

//	SerialMap[0].SCIBeginSend(0);
    return TRUE;
}

//--------------------------------------------------
//功能描述:  链路层数据响应函数
//         
//参数:     pAPDU[IN]	 //apdu 指针
//		  APDU_Len[IN]//apdu 长度 无需进行极限值判断
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
void api_ActiveReportResponse(BYTE* pAPDU, BYTE APDU_Len, BYTE Type )
{
	BYTE Buf[MAX_PRO_BUF+EXTRA_BUF],Att;
	TTwoByteUnion Len,Temp16;
	WORD i,j,OI;
	int nLen = 0;
	BYTE sbyClientAddr = 0x30;//默认值是30

	Buf[0] =0x68;	//起始字符
					//长度未填写

	if( Type == 0x55 )
	{
		Buf[3] = 0x83;
	}
	else
	{
	    Buf[3] = 0x43; //控制域@@@@
	}

	nLen = 4;
	if( Type == 0x00 )
	{
		//if( ManageComInfo.AddressType == 0x00 )//未获取电表地址时，使用通配地址
		if(lib_CheckCRC32(&NAddr.NAddressType,sizeof(TNAddr_t)-sizeof(DWORD)) != NAddr.dwCRC )//未获取电表地址时，使用通配地址
		{
			Buf[nLen++] = 0x45; //地址长度
			
			for( i=0; i<6; i++ )
			{
				Buf[nLen+i] = 0xAA;//服务器地址，即通配地址
			}
			nLen += 6;
		}
		else
		{
			Buf[nLen] = 0x05; //地址长度
			if((ManageComInfo.Status == eGET_SAMPLEINFO) || (ManageComInfo.Status == eGET_ALL)
				|| (ManageComInfo.Status == eGET_V) || (ManageComInfo.Status == eGET_I))
			{
				Buf[nLen] |= 0x10;//读计量芯配置参数，逻辑地址为1
			}
			nLen++;
			for( i=0; i<6; i++ )
			{
				//Buf[nLen+i] = ManageComInfo.Address[5-i];//服务器地址，即电表地址
				Buf[nLen+i] = NAddr.NAddress[5-i];//服务器地址，即电表地址
			}
			nLen += 6;
		}
	}
	
	if( Type == 0x55 )
	{
		if(lib_CheckCRC32(&NAddr.NAddressType,sizeof(TNAddr_t)-sizeof(DWORD)) != NAddr.dwCRC )//未获取电表地址时，使用通配地址
		{
			Buf[nLen++] = 0x66; //地址长度
			Buf[nLen++] = LOGICADDR;//LinkData[0].LogicAddr;
		
			for( i=0; i<6; i++ )
			{
				Buf[nLen+i] = 0xAA;//服务器地址，即通配地址
			}
			nLen += 6;
		}
		else
		{
			Buf[nLen] = 0x06; //地址长度
			Buf[nLen++] |= 0x20;//读计量芯配置参数，逻辑地址为1
		
			Buf[nLen++] = LOGICADDR;//LinkData[0].LogicAddr; 

			for( i=0; i<6; i++ )
			{
				Buf[nLen+i] = ManageComInfo.Address[5-i];//服务器地址，即电表地址
			}
			nLen += 6;
		}
	}
	
	if( Type == 0x55 )
	{
		Buf[nLen++] = 0x00;//0x10;//客户机地址
	}
	else
	{
		lib_ExchangeData((BYTE*)&OI,(BYTE*)&pAPDU[3],2);
		Att = pAPDU[5];

		if((OI == 0x4031) && (Att == 0xFE))
		{
			Buf[nLen] = 0x30; //方法254固定用30
		}
		else
		{
			Buf[nLen] = sbyClientAddr;
			for(i = 0; i < sizeof(ErrpassNoAut)/sizeof(TErrPassNotAut); i++) //针对模组主动抄读一些参数时，老版物联表和新版物联表客户机地址不一致的兼容
			{
				if((OI == ErrpassNoAut[i].OI) && (Att == ErrpassNoAut[i].Attr))
				{
					ErrpassNoAut[i].SendCount ++;
					if(ErrpassNoAut[i].RecErrPassFlag == 0x77)//说明模组有回复，但是回复错误
					{
						//客户机地址，主动抄读,NILM为30,这如果是30的话，很多老表不通过，只有新表才通过，需要做兼容！！！！！！
						//默认用30，如果30经过3次不通过，改为A0再试，如果A0经过3次不通过，改为30再试，如果还是不行，再该A0，再试3次，以此类推
						if(((((ErrpassNoAut[i].RecErrPassCount - 3) / 3) % 2) == 0) && (ErrpassNoAut[i].RecErrPassCount >= 3) )//代表重试三次
						{
							sbyClientAddr = 0xA0;//老表用A0
						}
						else
						{
							sbyClientAddr = 0x30;//新表用30
						}	
					}
					else //说明模组发送报文后，物联表没有回复。或者是某个地址成功
					{

						if((ErrpassNoAut[i].SendCount >= 3) && (ErrpassNoAut[i].RecErrPassFlag == 0x00))//物联表没有回复
						{
							if(((((ErrpassNoAut[i].SendCount - 3) / 3) % 2) == 0) && (ErrpassNoAut[i].SendCount >= 3) )//代表重试三次
							{
								sbyClientAddr = 0xA0;
							}
							else
							{
								sbyClientAddr = 0x30;
							}
							
						}
					}
					Buf[nLen] = sbyClientAddr;
					break;
				}
			}
		}

		nLen ++;
	}

	nLen -= 6;

	Len.w = (DLT_LINK_DATA_HEAD+nLen);//帧头实际长度
	
	memcpy( (BYTE*)&Buf[Len.w], pAPDU, APDU_Len );//组帧apdu
	Len.w += APDU_Len;
	
	Len.w += (DLT_LINK_DATA_TAIL-2);//长度 不包括起始字符+结束字符
	Buf[1] = Len.b[0]; //数据长度
	Buf[2] = Len.b[1];
	Len.w += 2;//更新总数据长度
	Temp16.w = fcs16(&Buf[1],(nLen+DLT_LINK_DATA_HEAD-DLT_START_BYTE-DLT_CSLEN));	//不包括起始地址和校验码
	Buf[(nLen+DLT_LINK_DATA_HEAD-DLT_CSLEN)] = Temp16.b[0];  //添加hrc
	Buf[(nLen+DLT_LINK_DATA_HEAD-DLT_CSLEN+1)] = Temp16.b[1];

	Temp16.w = fcs16(&Buf[1],(Len.w -DLT_CSLEN-DLT_START_BYTE-DLT_END_BYTE));	//添加FCS
	Buf[(Len.w-3)]= Temp16.b[0];
	Buf[Len.w-2] = Temp16.b[1];
	Buf[Len.w-1] = 0x16;
	


	memset( (void *)Serial[0].ProBuf, 0xfe, 4 );
	memcpy( (Serial[0].ProBuf+4),Buf,Len.w);
	Serial[0].SendLength = (Len.w+4);
	//执行延时
	Serial[0].ReceToSendDelay = 100;
	Serial[0].RxOverCount = 500;//启动500ms接受延时 - 取巧 使用字节间延时进行判断 不需要新增加延时 而且可以解决接受数据时会上报的问题
	SerialMap[0].SCIBeginSend(0);//uart0

	//测试用：报文打印
	#if(USE_PRINTF == 1)
	if(((UsePrintfFlag >> 0) & 0x0001) == 0x0001)
	{
		rt_kprintf("\nTX: ");
		j = 0;
		for(i = 0;i<Serial->SendLength;i++)
		{
			rt_kprintf("%02X ",Serial->ProBuf[i]);
			j++;
			if(j>100)
			{
				rt_kprintf("\n");
				j = 0;
			}
		}
		rt_kprintf("\n");
	}
	#endif
}

