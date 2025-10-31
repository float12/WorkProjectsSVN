/*****************************************************************************/
/*文件名：dlt645_2007.c
 *Copyright (c) 2008 烟台东方威思顿电气有限公司计量产品开发组
 *All rights reserved
 *
 *创建人：魏灵坤
 *日  期：2008-9-21
 *修改人：魏灵坤
 *日  期：2009-3-26
 *描  述：DTSD/DSSD178全电子式多功能电能表通讯国标DL/T645-2007规约程序
 *
 *版  本：
 *说  明：ReadDlt645_2007Data()函数 可供LCD、事件处理、负荷曲线、冻结调用，只要数据是规约格式都可以调用此函数
 *		  如果函数入口参数是 BYTE *ProBuf，则ProBuf是规约缓冲TSerial中的ProBuf，
 *		  如果函数入口参数是 BYTE *pBuf，则 pBuf取决于调用时传输的数据
 */
/*****************************************************************************/

#include "AllHead.h"
#include "Dlt698_45.h"
#include "MCollect.h"

#if( SEL_DLT645_2007 == YES )
//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
#define	DLT645_DATA33H				(0x33)
#define	DLT645_ADDR_LEN				(6)
#define	DLT645_07_DATAID_LEN		(4)
#define	DLT645_07_PASSWORD_LEN		(3)	//4改为3--hy2017.11.14
#define	DLT645_07_OPERATORCODE_LEN	(4)	//3改为4--hy2017.11.14
#define DLT645_ADDR_STARTONFRAME	(1)
#define DLT645_CONTROL_STARTONFRAME	(8)
#define DLT645_LEN_STARTONFRAME		(9)
#define DLT645_DATA_STARTONFRAME	(10)


#define	DLT645_97_DATAID_LEN		(2)

//初始化 645_2007 一些变量，上电和从低功耗上电时都执行
void InitDlt645_2007(void)
{
	//Identitytimer = 0;	
}

//--------------------------------------------------
//功能描述: 获得645规约类型
//         
//参数:      BYTE *bBuf[in] 输入缓冲bBuf[0]为起始字节68
//         
//返回值:    static
//         
//备注内容:  只判断是07规约还是9
//--------------------------------------------------
static eTYPE_PROTOCOL GetDlt645ProtocalType( eSERIAL_TYPE PortType )
{
	TFourByteUnion DataID;
	eTYPE_PROTOCOL eResult;

	if( gPr645[PortType].byReceiveControl == eCONTROL_RECE_01 )//97规约只支持读命令
	{
		eResult = ePROTOCOL_DLT645_97;
	}
	else
	{
		memcpy( DataID.b, gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME, DLT645_07_DATAID_LEN );//获取数据标识

		if( DataID.d == DLT645_FACTORY_ID )
		{
			eResult = ePROTOCOL_DLT645_FACTORY;
		}
		else
		{
			eResult = ePROTOCOL_DLT645_07;
		}
	}
	return eResult;
}

//--------------------------------------------------
//功能描述:  对报文进行减0x33处理
//         
//参数:      BYTE *pBuf[in]:输入缓冲pBuf[0]为68H
//         
//返回值:    void
//         
//备注内容:  无
//--------------------------------------------------
void DataSubtract33H(BYTE *pBuf)
{
	WORD i;
	for(i=DLT645_DATA_STARTONFRAME;i<(DLT645_DATA_STARTONFRAME+pBuf[DLT645_LEN_STARTONFRAME]);i++)
	{
		pBuf[i]=(pBuf[i]-DLT645_DATA33H);
	}
}

//--------------------------------------------------
//功能描述:  对报文进行加0x33处理
//         
//参数:      BYTE *pBuf[in]:输入缓冲pBuf[0]为68H
//         
//返回值:   void 
//         
//备注内容:  无
//--------------------------------------------------
void DataPlus33H( BYTE *pBuf )
{
	WORD i;
	for(i=DLT645_DATA_STARTONFRAME;i<(DLT645_DATA_STARTONFRAME+pBuf[DLT645_LEN_STARTONFRAME]);i++)
	{
		pBuf[i]=(pBuf[i]+DLT645_DATA33H);
	}
}

//--------------------------------------------------
//功能描述:  判断通信地址是否为相同的输入数据
//         
//参数:      BYTE bySpecialData[in]
//         
//           BYTE *pBuf[in]
//         
//返回值:    
//         
//备注内容:  无
//--------------------------------------------------
WORD JudgeMeterNoIfEqually(BYTE bySpecialData,BYTE *pBuf)
{
	BYTE i;

	for(i=0; i<6; i++)
	{
		if(pBuf[i] != bySpecialData )
		{
			return FALSE;
		}
	}

	return TRUE;
}
//--------------------------------------------------
//功能描述:  判断是否是通配地址
//         
//参数:      BYTE *pBuf[in]
//         
//返回值:    
//         
//备注内容:  不能作为判断地址是否一样的依据
//--------------------------------------------------
WORD JudgeMeterNoAccessAddress(BYTE *pBuf)
{
	BYTE *pMeterNo;

	BYTE i;

	pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);

	for(i=0; i<6; i++)
	{
		if( pMeterNo[5-i] != 0xaa )//如果这个字节表地址不是AA
		{
			if( pBuf[i] == 0xaa )//但通信时这个字节用AA
			{
				return TRUE;
			}
		}

	}

	return FALSE;

}

//--------------------------------------------------
//功能描述:  获得645地址类型
//         
//参数:      BYTE *bBuf[in] 输入缓冲
//         
//返回值:    TYPE_ADDR_TYPE
//         
//备注内容:  无
//--------------------------------------------------
static eTYPE_ADDR_TYPE GetDlt645AddrType(BYTE *bBuf )
{
	eTYPE_ADDR_TYPE eResult;

	if( JudgeMeterNoIfEqually(0x88, bBuf) == TRUE )//判断是否为全8地址
	{
		return eTYPE_ADDR_TYPE_88;
	}
	else if( JudgeMeterNoIfEqually(0x99, bBuf) == TRUE )//判断是否为全9地址
	{
		return eTYPE_ADDR_TYPE_99;
	}
	else if( JudgeMeterNoIfEqually(0xAA, bBuf) == TRUE )//判断是否为全AA地址
	{
		return eTYPE_ADDR_TYPE_ALLAA;
	}
	else if( JudgeMeterNoAccessAddress( bBuf ) == TRUE )//判断是否为通配地址
	{
		return eTYPE_ADDR_TYPE_AA;
	}
	else
	{
		return eTYPE_ADDR_TYPE_XX;//普通地址
	}
}


//--------------------------------------------------
//功能描述:  将645的数据解析到结构体中
//         
//参数:      BYTE *bBuf[in]  输入缓冲bBuf[0]为起始字节68
//         
//           TYPE_PROTOCOL eProtocalType[in]  规约类型
//         
//           BYTE *Len[in]  数据标识长度
//         
//返回值:    DWORD 数据标识
//         
//备注内容:  无
//--------------------------------------------------
static void DealDlt645DataToStruct( eSERIAL_TYPE PortType, BYTE *pBuf, eTYPE_PROTOCOL eProtocalType )
{
	
	DWORD dwResult;

	if( (eProtocalType == ePROTOCOL_DLT645_07) || (eProtocalType == ePROTOCOL_DLT645_FACTORY) )//对07命令的数据域进行解析
	{
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_11://读命令
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//获取数据标识
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//数据域地址
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN);//数据区长度
				break;
			case eCONTROL_RECE_12://读后续帧
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//获取数据标识
				gPr645[PortType].bSEQ = pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//帧序号
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1];//数据域地址
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-1);//数据区长度		
				break;	
			case eCONTROL_RECE_14://写命令
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//获取数据标识 
				gPr645[PortType].bLevel = pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN];//密码等级
				memcpy( gPr645[PortType].bPassword,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1], DLT645_07_PASSWORD_LEN );//密码
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1+DLT645_07_PASSWORD_LEN], DLT645_07_OPERATORCODE_LEN);//操作者代码
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+1+DLT645_07_PASSWORD_LEN+DLT645_07_OPERATORCODE_LEN];//数据域地址
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-1-DLT645_07_PASSWORD_LEN-DLT645_07_OPERATORCODE_LEN);//数据区长度
				break;	

			case eCONTROL_RECE_03://预付费命令
				memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_07_DATAID_LEN );//获取数据标识 
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN], DLT645_07_OPERATORCODE_LEN);//操作者代码
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN+DLT645_07_OPERATORCODE_LEN];//数据域地址
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_07_DATAID_LEN-DLT645_07_OPERATORCODE_LEN);//数据区长度
			break;	

			case eCONTROL_RECE_1C://跳合闸命令
			case eCONTROL_RECE_19://需量清零
			case eCONTROL_RECE_1A://电表清零
			case eCONTROL_RECE_1B://事件清零
				gPr645[PortType].bLevel = pBuf[DLT645_DATA_STARTONFRAME];//密码等级
				memcpy( gPr645[PortType].bPassword,&pBuf[DLT645_DATA_STARTONFRAME+1], DLT645_07_PASSWORD_LEN );//密码
				memcpy( gPr645[PortType].byOperatorCode,&pBuf[DLT645_DATA_STARTONFRAME+1+DLT645_07_PASSWORD_LEN], DLT645_07_OPERATORCODE_LEN);//操作者代码
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+1+DLT645_07_PASSWORD_LEN+DLT645_07_OPERATORCODE_LEN];//数据域地址
				gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-1-DLT645_07_PASSWORD_LEN-DLT645_07_OPERATORCODE_LEN);//数据区长度
			break;

			default:
				gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME];//数据域地址
				gPr645[PortType].wDataLen = pBuf[DLT645_LEN_STARTONFRAME];//数据区长度
			break;
		}

		gPr645[PortType].bIDLen = 4;//数据标识长度与协议相关 有协议类型了 这个是否多余
	}
	else//97规约
	{
		memcpy( gPr645[PortType].dwDataID.b, &pBuf[DLT645_DATA_STARTONFRAME], DLT645_97_DATAID_LEN );//获取数据标识
		gPr645[PortType].pDataAddr = &pBuf[DLT645_DATA_STARTONFRAME+DLT645_97_DATAID_LEN];//数据域地址
		gPr645[PortType].wDataLen=(pBuf[DLT645_LEN_STARTONFRAME]-DLT645_97_DATAID_LEN);//数据区长度

		gPr645[PortType].bIDLen = 2;//数据标识长度与协议相关 有协议类型了 这个是否多余
	}
	
}

//--------------------------------------------------
//功能描述:  645报文解析函数
//         
//参数:      TYPE_PORT PortType[in]:协议类型
//         
//           WORD Len[in]:数据长度
//         
//           BYTE *pBuf[in]:输入buf
//         
//返回值:    WORD
//         
//备注内容:  无
//--------------------------------------------------
static WORD GetAndCheck645Pare( eSERIAL_TYPE PortType, BYTE *pBuf)
{
	if( PortType >= MAX_COM_CHANNEL_NUM )
	{
		return 0xffff;
	}
	
	gPr645[PortType].pMessageAddr = pBuf;
	
	gPr645[PortType].eAddrType= GetDlt645AddrType( pBuf+DLT645_ADDR_STARTONFRAME );
	
	gPr645[PortType].wSerialDataLen=pBuf[DLT645_07_PASSWORD_LEN];//串口数据长度 不传人serail结构体指针可能取不到串口数据长度!!!!!
	
	gPr645[PortType].byReceiveControl=pBuf[DLT645_CONTROL_STARTONFRAME];//接收报文控制码
	
	gPr645[PortType].eProtocolType = GetDlt645ProtocalType( PortType );
	
	DealDlt645DataToStruct( PortType, pBuf, gPr645[PortType].eProtocolType );//解析数据到结构体中

	if( gPr645[PortType].byReceiveControl != eCONTROL_RECE_12 )//控制码不是读后续帧命令进行清后续帧
	{
		memset( &DLT645APPFollow[PortType].FollowFlag, 0x00, sizeof(DLT645APPFollow[0]) );
	}

	return TRUE;
}

//---------------------------------------------------------------
//函数功能: 发送645数据
//
//参数: 	p[in]-数据区
//                   
//返回值:   无
//
//备注:   
//---------------------------------------------------------------
void SendDlt645(TSerial * p)
{
	WORD TDataLength;
	BYTE i;
    
	p->byLastRXDCon = p->ProBuf[8];//保存上次通信电表应答控制码，供后面读后续帧判断用

	TDataLength = p->ProBuf[9];
	
	//fe fe fe fe 68 A0...A5 68 81 L D0..DL CS 16,除D0...DL外，其余共16字节
	if( TDataLength > (MAX_PRO_BUF-16) )//最多或等于
	{//此举应可防把别的内存数据冲了
		p->BroadCastFlag = 0xff;
		return;
	}
	
	i=(11+TDataLength);
	for(;;)
	{
		p->ProBuf[i+4]=p->ProBuf[i];
		if(i==0)
		{
			break;
		}
		i--;
	}

	p->ProBuf[0]=0x0fe;
	p->ProBuf[1]=0x0fe;
	p->ProBuf[2]=0x0fe;
	p->ProBuf[3]=0x0fe;

	p->SendLength = (TDataLength + 16);//如有FE FE FE FE 则加16

	//执行延时
	p->ReceToSendDelay = (DWORD)RECE_TO_SEND_DELAY;


	for(i=0; i<MAX_COM_CHANNEL_NUM; i++)
	{
		if( p == &Serial[i] )
		{
			SerialMap[i].SCIEnableSend(SerialMap[i].SCI_Ph_Num);
			break;
		}
	}
}



//---------------------------------------------------------------
//函数功能: 判断下发命令是否广播地址，还有别的地方调用此函数，不能取消
//
//参数: 	pBuf[in] - 传入通信地址
//                   
//返回值:   TRUE/FALSE
//
//备注:   
//---------------------------------------------------------------
WORD JudgeBroadcastMeterNo(BYTE *pBuf)
{
	//判断接收报文中的通信地址是否全部是给定的数据,例如99或88
	return JudgeRecMeterNoOnlyOneData(0x99,pBuf);
}

//要求pBuf[0--5]依次为接收报文中的表号,如表号对返回TRUE,否则返回FALSE
WORD JudgeRecMeterNo_645(BYTE *pBuf)
{
	//#if( SEL_DLT645_2007 != YES )
	BYTE k;
	//#endif

	BYTE *pMeterNo;

	BYTE i,j;


	//判断下发命令是否广播地址
	//j = JudgeBroadcastMeterNo(pBuf);

	//if(j!=TRUE)
	{
		//判断接收报文中的通信地址是否全部是给定的数据,例如99或88
		j = JudgeRecMeterNoOnlyOneData(0x88,pBuf);
	}

	if(j!=TRUE)
	{
		j=TRUE;
		pMeterNo = (BYTE *)&(FPara1->MeterSnPara.CommAddr[0]);//通讯地址BYTE CommAddr[6]，BCD [0]-保存高字节
		
		k = 5;//(METER_ADDR_BYTE_NUM-1);//SEL_DLT645_2007 中 k:暂保存第一个AA的位置
		//下面进行表地址判断，从低字节开始判断
		for(i=0; i<6; i++)
		{
			if( pBuf[i] == pMeterNo[5-i] )
			{
				if( pMeterNo[5-i] != 0xaa )//如果这个字节表地址不是AA
				{
					//如果AA后出现不是AA的数据，645要求剩余高位补AAH作，正泰认为中间和低位也不能用AA
					if( i > k )
					{
						j = FALSE;
						break;
					}
				}
			}
			else if( pBuf[i] == 0xaa )
			{
				k = i;//k:暂保存第一个AA的位置
			}
			else
			{
				j = FALSE;
				break;
			}
		}
	}
	
	return j;
}

//---------------------------------------------------------------
//函数功能: 645格式判断
//
//参数: 	portType[in] - 通讯端口
//                   
//返回值:  FALSE: 报文没有收完
//		  TRUE : 报文已收完且InitPoint()
//
//备注:   
//---------------------------------------------------------------
WORD DoReceProc_Dlt645( BYTE ReceByte, TSerial * p )
{
	WORD i, j;
	WORD Result;
	BYTE *pBuf;
	TDlt645Head Dlt645Head;

	//搜索同步头
	if( p->ProStepDlt645 == 0 )
	{
		if(ReceByte == 0x68)
		{
			//写入数据
			//p->ProBuf[p->ProStepDlt645] = ReceByte;
			//Dlt645规约报文在Serial[].ProBuf中的开始位置
			p->BeginPosDlt645 = p->RXRPoint;

			//操作指针
			p->ProStepDlt645 ++;

			//开始接收定时
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
	}
	else if( p->ProBuf[p->BeginPosDlt645] == 0x68 )
	{
		//接收规约头、控制字、长度 68 1 2 3 4 5 6 68 01 02
		if( p->ProStepDlt645 < sizeof(TDlt645Head) )
		{
			//写入数据
			//p->ProBuf[p->ProStepDlt645] = ReceByte;

			//操作指针
			p->ProStepDlt645 ++;
			
			//是否可以处理数据
			if( p->ProStepDlt645 == sizeof(TDlt645Head) )
			{
				//把规约报文移到临时缓冲中 循环缓冲用此
				//Num：移动多少字节
				//BeginPos:从什么地址开始移
				pBuf = (BYTE *)&Dlt645Head;
				for( i=0; i<sizeof(Dlt645Head); i++ )
				{
					pBuf[i] = p->ProBuf[(p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL];
				}
				//映射结构
				//pDlt645Head = (TDlt645Head *)&TmpArray;
                j = p->ProStepDlt645;

				//搜索第二同步字
				if( Dlt645Head.Sync2 != 0x68 )//如果没有第2个帧起始符68，则前面收到的第一68不对，从它后面搜索第一个68
				{
					for( i=1; i<p->ProStepDlt645; i++ )
					{
						if( p->ProBuf[(p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL] == 0x68 )
						{
							p->BeginPosDlt645 = (p->BeginPosDlt645+i)%MAX_PRO_BUF_REAL;
							p->ProStepDlt645 -= i;
							break;
						}
						//改后68前面其它数据或68都能通信上，但这个通信不上，没防这种，不想太复杂：68 fe fe fe fe fe fe 68 88 88 88 88 88 88 68 11 04 33 33 34 33 e2 16
					}
					if( i >= j )//j 是 p->ProStepDlt645 的备份，所有收到的数据已找完，则不再找，直接ProStepDlt645赋0跳出
					{
						p->ProStepDlt645 = 0;//InitPoint;
						return FALSE;
					}
					//p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
/*				
				//if( p == &Serial[eIR] )//如果当前通道为红外通道
				//{
				//    api_SetLcdCommunicationFlag( eIR );//置通讯标志
				//}
				//判断下发命令是否广播地址
				if( JudgeBroadcastMeterNo(Dlt645Head.ID) == FALSE )
				{	
					if( JudgeRecMeterNo_645(Dlt645Head.ID) == FALSE )//如果判断两个68间的通信地址不对，则第2个68开始重新检测接收报文
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
				//接收到完整报文时，dlt645 Weisheng Modbus统一在DisableRece()中复位串口监视
				//复位串口监视
				//p->WatchSciTimer = WATCH_SCI_TIMER;
				
				//取控制字节
				if( Dlt645Head.Control >= 0x80 )//如果判断控制码不对，则第2个68开始重新检测接收报文
				{
					p->BeginPosDlt645 = p->BeginPosDlt645+7;
					p->ProStepDlt645 -= 7;
					return FALSE;
				}
 */             
 				#if (SEL_SEARCH_METER == YES )
                if ( (p == &Serial[SEARCH_METER_CH])
                  &&( SKMeter.byAutoCheck != 0 ) 
                  &&( (JLTxBuf.byValid == JLS_IDLE) || (JLTxBuf.byDestNo != SEARCH_METER_CH) )
                   ) // 搜表通道不判断
                {
                    
                }
                else 
                #endif
                if (JudgeBroadcastMeterNo(Dlt645Head.ID) || JudgeRecMeterNo_645(Dlt645Head.ID))
				{
					if( Dlt645Head.Control >= 0x80 )//如果判断控制码不对，则第2个68开始重新检测接收报文
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
                else
				{
					// 档案地址
					if (IsInSysMeters(Dlt645Head.ID))
					{
						
						if (Dlt645Head.Control >= 80) //从站应答帧，继续接收处理
						{
							// if (p == &Serial[eCR]) //载波口的接收放在1376.2规约中处理
							// {
							// 	p->BeginPosDlt645 = p->BeginPosDlt645+7;
							// 	p->ProStepDlt645 -= 7;
							// 	return FALSE;
							// }
						}
						else //主站命令帧，认为是透传中包含的645帧，不进接收处理
						{
							if( (p->ProStepDlt698_45 >= 40) || (p->ProStep3762 >= 30) )//698代理645时，645报文不直接进行处理，在698中处理							
							{
								p->BeginPosDlt645 = p->BeginPosDlt645+7;
								p->ProStepDlt645 -= 7;
								return FALSE;
							}
						}
					}
					else if(MessageTransParaRam.AllowUnknownAddrTrans == TRUE)
					{
					
					}
					else
					{
						p->BeginPosDlt645 = p->BeginPosDlt645+7;
						p->ProStepDlt645 -= 7;
						return FALSE;
					}
				}
                
				//取长度 645报文数据域数据长度
				p->Dlt645DataLen = Dlt645Head.Length;

				//判断长度
				//68 A0...A5 68 81 L D0..DL CS 16,除D0...DL外，其余共16字节
				if( p->Dlt645DataLen >= ( MAX_PRO_BUF - 12 ) )
				{
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}

			//处理校验
			//p->DltCheckSum += ReceByte;

			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		//接收数据
		else if( p->ProStepDlt645 < (sizeof(TDlt645Head) + p->Dlt645DataLen + 2 ) )
		{
			//接收数据
			//p->ProBuf[ p->ProStepDlt645 ] = ReceByte;

			//操作指针
			p->ProStepDlt645 ++;

			//处理校验字节
			if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 1) )
			{
				//把规约报文移到临时缓冲中 循环缓冲用此
				//Num：移动多少字节
				//BeginPos:从什么地址开始移
				if( ReceByte != CalRXD_CheckSum(PRO_NO_SPECIAL, p) )
				//if( ReceByte != ( lib_CheckSum(p->ProBuf+p->BeginPosDlt645,(10+p->Dlt645DataLen))&0xff) )
				{
					//校验不正确
					p->ProStepDlt645 = 0;//InitPoint(p);
					return FALSE;
				}
			}

			//收到0x16 才进行程序处理
			else if( p->ProStepDlt645 == (sizeof(TDlt645Head) + p->Dlt645DataLen + 2) )
			{
				//校验通过，可以处理数据

				//p->RXWPoint = MAX_PRO_BUF_REAL-1;
				//if( ReceByte != 0x16 )//如果16H不对，整帧报文放弃了
				//{
				//	p->ProStepDlt645 = 0;//InitPoint(p);
				//	return FALSE;
				//}

				//禁止接收
				api_DisableRece( p );

				//把数据从报文开始位置 向 以p->ProBuf[0]开始处移
				DoReceMoveData(PRO_NO_SPECIAL, p);
				
				if( Pre_Dlt645(p) )
				{
					api_EnableRece( p );
				}
				else
                {
					ProMessage_Dlt645( p );
				}
				TX_LED_FLASH;
				//处理完成
				InitPoint(p);
				return TRUE;
			}
			else
			{
				//处理校验
			//	p->DltCheckSum += ReceByte;
			}

			//处理接收定时
			//p->RxOverCount = ((DWORD)MAX_RX_OVER_TIME*1000)/LOOP_CYC_TIME;
		}
		else
		{
			p->ProStepDlt645 = 0;//InitPoint(p);
		}
	}
	return FALSE;
}



//////////////////////////////////////////////////
//--------------------------------------------------
//功能描述:  回填645通信地址
//         
//参数:      TYPE_PORT PortType[in]:串口类型
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void FillDlt645FrameAddr( eSERIAL_TYPE PortType )
{
	BYTE i;
	
	for(i=0; i<6; i++)
	{
		//确定数据是否正确
		gPr645[PortType].pMessageAddr[DLT645_ADDR_STARTONFRAME+i] =FPara1->MeterSnPara.CommAddr[5-i];
	}
}

//--------------------------------------------------
//功能描述:  回填645控制码
//         
//参数:      TYPE_PORT PortType[in]:串口类型
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void FillDlt645FrameSendControl( eSERIAL_TYPE PortType, WORD wResult )
{
	if( wResult < 0x8000 )//正常应答
	{
		if( DLT645APPFollow[PortType].FollowFlag != eNO_FOLLOW )//判断是否有后续帧
		{
			gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0xb0);
		}
		else
		{
			gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0x80);
		}		
	}
	else//错误应答
	{
		gPr645[PortType].pMessageAddr[DLT645_CONTROL_STARTONFRAME] = (gPr645[PortType].byReceiveControl | 0xC0);
	}
}

//--------------------------------------------------
//功能描述:  回填645后续帧数据
//         
//参数:      
//			 TYPE_PORT PortType[in]:串口类型
//			 Lenth - 规约数据长度
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
const BYTE ReportDI[4] = { 0x04, 0x00, 0x15, 0x01 }; //主动上报数据标识
static void FillDlt645FollowData( eSERIAL_TYPE PortType, WORD Lenth )
{
	WORD SendLen;//本帧发送数据长度
	WORD wSentLen;//已发送长度
	
	if( (DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW) 
	  ||(DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW_END))//如果是负荷记录后续帧
	{			
		gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = Lenth+DLT645_07_DATAID_LEN+1;//数据长度多4个字节的数据标识、帧序号
		memcpy( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
		gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + Lenth] = gPr645[PortType].bSEQ;
		
		if(DLT645APPFollow[PortType].FollowFlag == eLOAD_FOLLOW_END)
		{
			DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
		}
		DLT645APPFollow[PortType].bLastNum++;
	}
	else if( DLT645APPFollow[PortType].FollowFlag == eNORMAL_FOLLOW )//正常后续帧
	{
		DLT645APPFollow[PortType].bLastNum ++;
		
		//每帧数据固定 DLT645_FOLLOW_LEN 个字节 后续帧还有一字节帧序号(第一帧不算后续帧) 
		//所以: 已发送字节数 = DLT645_FOLLOW_LEN(第一帧数据，无帧序号) + (DLT645_FOLLOW_LEN-1)*后续帧个数 
		wSentLen = DLT645_FOLLOW_LEN + ( (gPr645[PortType].bSEQ - 1) * (DLT645_FOLLOW_LEN - 1) );
		
		if( DLT645Sendbuf[PortType].DataLen > wSentLen )
		{
			SendLen = DLT645Sendbuf[PortType].DataLen - wSentLen; //正常后续帧数据总长度使用缓存中的长度
		}
		else
		{
			SendLen = 0;
		}
		
		if( SendLen >= DLT645_FOLLOW_LEN )//剩余数据超过200个字节
		{
			gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = (DLT645_FOLLOW_LEN-1)+DLT645_07_DATAID_LEN+1;//数据长度多4个字节的数据标识+1个自己帧序号
			memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf+wSentLen, (DLT645_FOLLOW_LEN-1) );
			gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + DLT645_FOLLOW_LEN-1] = gPr645[PortType].bSEQ;
		}
		else
		{
			gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = SendLen+DLT645_07_DATAID_LEN+1;//数据长度多4个字节的数据标识+1个自己帧序号
			memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf+wSentLen, SendLen );
			gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + SendLen] = gPr645[PortType].bSEQ;
			
			DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
			DLT645APPFollow[PortType].bLastNum = 0;
		}
	}
	else if( DLT645APPFollow[PortType].FollowFlag == eREPORT_FOLLOW ) //主动上报后续帧处理
	{
		SendLen = api_ReadReportStatusByte( PortType,eREAD_STATUS_PROC, DLT645Sendbuf[PortType].DataBuf ); //!!!!!!!主动上报暂未支持
		
		gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = SendLen+DLT645_07_DATAID_LEN+1;
		memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, SendLen );
		gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN + SendLen] = gPr645[PortType].bSEQ; //帧序号固定为0x01
		memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME, ReportDI, 4 );
		lib_InverseData( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME, 4 );
		
		//不在此处清除主动上报标志 返回上一层再清 用于区分这次读取是否是读取主动上报状态字
		DLT645APPFollow[PortType].bLastNum = 0;
	}
}
//--------------------------------------------------
//功能描述:  回填645数据长度
//         
//参数:      TYPE_PORT PortType[in]:串口类型
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void FillDlt645FrameData( eSERIAL_TYPE PortType, WORD wResult )
{
	WORD Lenth;
	
	if( wResult < 0x8000 )//正常应答
	{
		Lenth = wResult;
		
		if( gPr645[PortType].byReceiveControl != eCONTROL_RECE_12 )
		{
			DLT645Sendbuf[PortType].DataLen = Lenth;
			gPr645[PortType].wDataLen = Lenth;
		}
		
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_01://读命令(07规约) 暂时未做

				break;
			
			case eCONTROL_RECE_11://读命令
				if( DLT645APPFollow[PortType].FollowFlag != eLOAD_FOLLOW )
				{
					if( gPr645[PortType].wDataLen <= DLT645_FOLLOW_LEN )
					{
						gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//数据长度多4个字节的数据标识
						memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
					}
					else //需要分帧处理
					{
						gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = DLT645_FOLLOW_LEN+DLT645_07_DATAID_LEN;//数据长度多4个字节的OAD
						memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, DLT645_FOLLOW_LEN );
						
						DLT645APPFollow[PortType].FollowFlag = eNORMAL_FOLLOW;
						DLT645APPFollow[PortType].bLastNum = 0;
					}
				}
				else//负荷记录需要特殊操作
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//数据长度多4个字节的数据标识
					memcpy( gPr645[PortType].pMessageAddr + DLT645_DATA_STARTONFRAME + DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
				
					DLT645APPFollow[PortType].bLastNum = 0;
				}
				//主动上报后续帧
				if( DLT645APPFollow[PortType].FollowFlag == eNO_FOLLOW )//!!!!!!主动上报跟随上报暂未处理
				{
					//如果本身是抄读主动上报 不置后续帧标志
					if(  gPr645[PortType].dwDataID.d != 0x04001501 )
					{
						if( api_GetReportReqFlag( PortType ) == TRUE )
						{
							//DLT645APPFollow[PortType].FollowFlag = eREPORT_FOLLOW;
						}
					}
				}
				break;
			case eCONTROL_RECE_12://读后续帧
				FillDlt645FollowData( PortType, Lenth );
				
				//主动上报后续帧 如果是读取主动上报 不应有后续帧标志
				if( DLT645APPFollow[PortType].FollowFlag == eREPORT_FOLLOW )
				{
					//如果已经是主动上报后续帧 此次读取需要返回状态字 并且不回复后续帧标志
					DLT645APPFollow[PortType].FollowFlag = eNO_FOLLOW;
				}
				else if( DLT645APPFollow[PortType].FollowFlag == eNO_FOLLOW )
				{
					//如果没有上报标志 说明数据已经读取结束 此时检查是否有主动上报 如果有 需要只后续帧标志
					if( api_GetReportReqFlag( PortType ) == TRUE )
					{
						//DLT645APPFollow[PortType].FollowFlag = eREPORT_FOLLOW;
					}
				}
				break;

			case eCONTROL_RECE_03://预付费命令
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen+DLT645_07_DATAID_LEN;//数据长度多4个字节的数据标识		
				memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME+DLT645_07_DATAID_LEN, DLT645Sendbuf[PortType].DataBuf, Lenth );
				break;
			
			case eCONTROL_RECE_14://写命令
			case eCONTROL_RECE_16://冻结数据
			case eCONTROL_RECE_19://需量清零
			case eCONTROL_RECE_1A://电表清零
			case eCONTROL_RECE_1B://事件清零
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 0;//数据长度为零
				break;

			case eCONTROL_RECE_1C://跳合闸命令
				if( gPr645[PortType].bPassword[2] != 0x01 )
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 0;//数据长度为零
				}
				else
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2;//数据长度2
					memcpy( (BYTE*)&gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], DLT645Sendbuf[PortType].DataBuf, 2 );
				}
				break;
				
			default:
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = gPr645[PortType].wDataLen;	
				memcpy( gPr645[PortType].pMessageAddr+DLT645_DATA_STARTONFRAME, DLT645Sendbuf[PortType].DataBuf, Lenth );
				break;
		}

	}
	else//异常应答
	{
		wResult = wResult&0x7FFF;
		
		switch( gPr645[PortType].byReceiveControl )
		{
			case eCONTROL_RECE_01://读命令(07规约) 暂时未做
				break;

			case eCONTROL_RECE_03://预付费命令
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2; //数据长度2
				memcpy( (BYTE*)&gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], (BYTE*)&wResult, 2 );
				break;
			
			case eCONTROL_RECE_1C://跳合闸命令
				if( gPr645[PortType].bPassword[2] != 0x01 )
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 1;//数据长度1
					gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME] = (BYTE)wResult;	
				}
				else
				{
					gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 2;//数据长度2
					memcpy( &gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME], (BYTE*)&wResult, 2 );
				}
				break;
			
			default:
				gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] = 1;//数据长度1
				gPr645[PortType].pMessageAddr[DLT645_DATA_STARTONFRAME] = (BYTE)wResult;
				break;
		}
	}
}

//--------------------------------------------------
//功能描述:   回填645帧头帧尾校验和
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
static void FillDlt645FrameHeadAndTail(eSERIAL_TYPE PortType )
{
	BYTE i;
	WORD wLen;
	
	wLen = gPr645[PortType].pMessageAddr[DLT645_LEN_STARTONFRAME] + DLT645_ADDR_LEN + 4; //4：两个68 一个控制码 一个长度
	
	gPr645[PortType].pMessageAddr[0] = 0x68;
	gPr645[PortType].pMessageAddr[DLT645_ADDR_STARTONFRAME+DLT645_ADDR_LEN] = 0x68;
	gPr645[PortType].pMessageAddr[wLen] = lib_CheckSum( gPr645[PortType].pMessageAddr, wLen );
	gPr645[PortType].pMessageAddr[wLen+1] = 0x16;
}

//--------------------------------------------------
//功能描述:  响应报文处理
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
void Proc645LinkDataRequest( eSERIAL_TYPE PortType, BYTE *pBuf )
{
    api_SetLcdCommunicationFlag( PortType );//置通讯标志

	DataSubtract33H( pBuf );//对数据进行减0x33处理
	
	GetAndCheck645Pare(PortType,pBuf);//数据解析到结构体
}

//--------------------------------------------------
//功能描述:  响应报文处理
//         
//参数:      TYPE_PORT PortType[in]
//         
//返回值:    static
//         
//备注内容:  无
//--------------------------------------------------
void Proc645LinkDataResponse( eSERIAL_TYPE PortType, WORD wResult)
{
	if( wResult == DLT645_NO_RESPONSE )//无需应答
	{
		api_InitSci( PortType );
		return;
	}
	
	FillDlt645FrameAddr( PortType );
	
	FillDlt645FrameData(PortType,wResult);
	
	//填充控制码在填充数据后 因为需要判断是否有后续帧
	FillDlt645FrameSendControl( PortType, wResult );
	
	DataPlus33H(gPr645[PortType].pMessageAddr);
	
	FillDlt645FrameHeadAndTail(PortType);
	
	SendDlt645( &Serial[PortType] );
}

#endif//#if( SEL_DLT645_2007 == YES )

