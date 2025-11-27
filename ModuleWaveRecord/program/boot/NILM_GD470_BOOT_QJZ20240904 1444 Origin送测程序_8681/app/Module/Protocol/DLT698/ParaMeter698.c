//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部
//创建人	姜文浩
//创建日期	2016.12.27
//描述		DL/T 698.45面向对象协议电能读取C文件
//修改记录
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Dlt698_45.h"

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
// 索引信息在EEPROM中的地址范围

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------
//-----------------------------------------------
//				本文件使用的变量，常量

#define MAX_PARA_BUF_LENTH         4000//时段表源数据长度为最大数据长度

typedef struct TModulelist_t	//读取记录参数结构体
{
    char  Name[12];			//设备描述符
    BYTE  LogicAdrr;		//逻辑地址
    WORD  Class;			//模组类别
    BYTE  SerialNum[8];		//模组序列号
}TModulelistPara;
//不支持1字节对齐，不能直接使用
const TModulelistPara ModulelistPara=
{
	"PowerQuality",
	0x05,
	0x0300,//实际对应bit0和bit1为1
	{0,0,0,0,0,0,0,0 },
};

static const BYTE T_Modulelist[]		= //模组列表
{ 
	Array_698,			1,
	Structure_698, 	 	4, 
	Visible_string_698,	12, //模组描述符
	Unsigned_698,		1,  //逻辑地址
	Bit_string_698,		16, //模组类别
	Octet_string_698,	8,  //模组序列号
};


typedef struct TModuleVersion_t	//读取记录参数结构体
{
    BYTE  LogicAdrr;		//逻辑地址
    BYTE  Rsv1[4];			//厂商代码
	BYTE  Rsv2[16];			//软件版本号
	BYTE  Rsv3[6];			//软件版本日期
	BYTE  Rsv4[16];			//硬件版本号
	BYTE  Rsv5[6];			//硬件版本日期
	BYTE  Rsv6[16];			//厂商扩展信息
}TModuleVersion;
//不支持1字节对齐，不能直接使用
const TModuleVersion ModuleVersion=
{
	0x05,								//逻辑地址
	{0,0,0,0},                          //厂商代码
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //软件版本号
	{0,0,0,0,0,0},                      //软件版本日期
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //硬件版本号
	{0,0,0,0,0,0},                      //硬件版本日期
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //厂商扩展信息
};

static const BYTE T_ModuleVersion[]		= //模组列表
{ 
	Structure_698, 	 	7, 
	Unsigned_698,		1,  //逻辑地址
	Visible_string_698,	4,  //厂商代码
	Visible_string_698,	16, //软件版本号
	Visible_string_698,	6,  //软件版本日期
	Visible_string_698,	16, //硬件版本号
	Visible_string_698,	6,  //硬件版本日期
	Visible_string_698,	16, //厂商扩展信息
};

//增加数据类型请查看源数据和加TAG是否支持
static const TCommonObj ParaMeterObj[] =
{
	{  0x4031,  &SU_00,   	T_Modulelist  },//模组列表
	{  0x4810,	&SU_00, 	T_Special  	  },//电能模块

};	

//-----------------------------------------------

//-----------------------------------------------
//				内部函数声明
//-----------------------------------------------


//-----------------------------------------------
//				函数定义
//-----------------------------------------------

//--------------------------------------------------
//功能描述:  根据表格查找参变量OAD
//         
//参数:      OI[in]：OI
//         
//返回值:    参变量索引
//         
//备注内容:  无
//--------------------------------------------------
static BYTE SearchParaMeterOAD( WORD OI )
{
	BYTE i,Num;//数组个数不能超过255

	Num = (sizeof(ParaMeterObj)/sizeof(TCommonObj));
	if( Num >= 0x80 )//避免死循环
	{
		return 0x80;
	}
	
	for (i=0; i<Num; i++)
	{
		if( OI == ParaMeterObj[i].OI )
		{
			return i;
		}
	}

	if( i == Num )//未找到
	{
		return 0x80;
	}
	return 0x80;
}

//--------------------------------------------------
//功能描述:  获取参变量（class8）数据
//         
//参数:     
//          *pOAD[in]：	OAD         
//          Sch[in]：   ParaMeterObj 中的数据索引      
//          *pBuf[out]：输出数据
//         
//返回值:   数据长度
//         
//备注内容: 不含tag
//--------------------------------------------------
static WORD GetProMeterParaData( BYTE *pOAD, BYTE Sch, BYTE *pBuf )
{
	TTwoByteUnion OI;
	BYTE ClassAttribute,ClassIndex;//,Num,i,Type;
	WORD Len;
	BYTE *pBufBak = pBuf;
	const BYTE *ObjType;
    TModuleVersion tModuleVersion;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	ObjType = ParaMeterObj[Sch].Type2;
	Len = 0;
	
	switch( ClassAttribute )
	{
		case 2:
			switch( OI.w )
			{
				case 0x4031://模组列表
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}

					memcpy(pBuf, &ModulelistPara.Name, sizeof(ModulelistPara.Name) );
					pBuf += sizeof(ModulelistPara.Name);
	    			memcpy(pBuf, &ModulelistPara.LogicAdrr, sizeof(ModulelistPara.LogicAdrr) );
					pBuf += sizeof(ModulelistPara.LogicAdrr);	
					memcpy(pBuf, &ModulelistPara.Class, sizeof(ModulelistPara.Class) );
					pBuf += sizeof(ModulelistPara.Class);
					memcpy(pBuf, &ModulelistPara.SerialNum, sizeof(ModulelistPara.SerialNum) );
					pBuf += sizeof(ModulelistPara.SerialNum);
					//接收到管理芯命令后，开始读取管理芯信息
					ManageComInfo.DelayTime = 3;
				break;
				
				default:
					return 0x8000;
			}
			break;

		case 3:
			switch( OI.w )
			{
				case 0x4031://模组列表
					if( ClassIndex > 1 )
					{
						return 0x8000;
					}
					
					ObjType = (BYTE *)&T_ModuleVersion;
					Len = GetTypeLen( eData, ObjType );
	    			if( Len == 0x8000 )
	    			{
	                    return 0x8000;
	    			}

                    memcpy(&tModuleVersion, &ModuleVersion, sizeof(ModuleVersion));
                    memset(&tModuleVersion.Rsv6, 0, sizeof(ModuleVersion.Rsv6));
                    tModuleVersion.Rsv6[0] = (BYTE)(MODULE_VERSION);
                    tModuleVersion.Rsv6[1] = (BYTE)(MODULE_VERSION >> 8);
                    *(WORD *)&tModuleVersion.Rsv6[2] = api_CheckCpuFlashSum(0x01);

	    			memcpy(pBuf, &tModuleVersion.LogicAdrr, sizeof(tModuleVersion.LogicAdrr) );
					pBuf += sizeof(tModuleVersion.LogicAdrr);	
					memcpy(pBuf, &tModuleVersion.Rsv1, sizeof(tModuleVersion.Rsv1) );
					pBuf += sizeof(tModuleVersion.Rsv1);
					memcpy(pBuf, &tModuleVersion.Rsv2, sizeof(tModuleVersion.Rsv2) );
					pBuf += sizeof(tModuleVersion.Rsv2);
					memcpy(pBuf, &tModuleVersion.Rsv3, sizeof(tModuleVersion.Rsv3) );
					pBuf += sizeof(tModuleVersion.Rsv3);
					memcpy(pBuf, &tModuleVersion.Rsv4, sizeof(tModuleVersion.Rsv4) );
					pBuf += sizeof(tModuleVersion.Rsv4);
					memcpy(pBuf, &tModuleVersion.Rsv5, sizeof(tModuleVersion.Rsv5) );
					pBuf += sizeof(tModuleVersion.Rsv5);
					memcpy(pBuf, &tModuleVersion.Rsv6, sizeof(tModuleVersion.Rsv6) );
					pBuf += sizeof(tModuleVersion.Rsv6);
				break;
				
				default:
					return 0x8000;
			}
			break;
		default:
			return 0x8000;
	}

	return  (WORD)(pBuf-pBufBak);
}

//--------------------------------------------------
//功能描述:  对参变量（class8）数据添加Tag
//         
//参数:      
//         	*pOAD[in]：		OAD    
//          *InBuf[in]：	需要添加tag的数据         
//          OutBufLen[in]：	申请的缓冲长度         
//          *OutBuf[out]：	输出数据
//         
//返回值:   添加tag后的数据长度
//         
//备注内容: 参变量（class8）  OI=4XXX
//--------------------------------------------------
static WORD AddTagMeterPara(BYTE *pOAD, BYTE Sch, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{
	BYTE ClassAttribute,ClassIndex;
	const BYTE *Tag;
	TTwoByteUnion OI;
	WORD Result,Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Lenth = 0;
	
	switch( ClassAttribute )
	{
		case 2://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}
		
		case 3://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_ModuleVersion;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Lenth = GetTypeLen( eTagData, Tag );
                if( Lenth == 0x8000 )
                {
                    return 0x8000;
                }
        			
				if( OutBufLen < Lenth )
				{
					return 0;
				}
				Result = GetRequestAddTag( Tag, InBuf, OutBuf );
				if( Result == 0x8000 )
				{
					return 0x8000;
				}

				return Lenth;
			}			
		default:
			return 0x8000;
	}	
}

//--------------------------------------------------
//功能描述:  读取参变量（class8）数据 OI=4XXX
//         
//参数:  
//         	DataType[in]：	eData/eTagData/eAddTag
//         	Dot[in]:		小数点(暂不用)        
//         	*pOAD[in]：		OAD         
//         	*InBuf[in]：	需要添加tag的数据         
//         	OutBufLen[in]：	申请的缓冲长度         
//         	*OutBuf[out]：	输出数据
//         
//返回值:  	返回数据长度
//         
//备注内容: 
//--------------------------------------------------
WORD GetRequestMeterPara( BYTE DataType,BYTE Dot,BYTE *pOAD, BYTE *InBuf, WORD OutBufLen, BYTE *OutBuf )
{	
	BYTE Sch;
	BYTE Buf[MAX_PARA_BUF_LENTH];
	TTwoByteUnion OI;
	WORD Lenth;
	
	lib_ExchangeData(OI.b,pOAD,2);
	
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}

	if( (DataType==eData) || (DataType==eTagData) )
	{
		Lenth = GetProMeterParaData( pOAD, Sch, Buf );
		if( Lenth == 0x8000 )
		{
			return Lenth;
		}
		if( DataType == eData )
		{
			if( OutBufLen < Lenth )
			{
				return 0;
			}
			memcpy( OutBuf, Buf, Lenth );
		}
		else
		{
			Lenth = AddTagMeterPara(pOAD, Sch, Buf, OutBufLen, OutBuf);
		}
	}
	else if( DataType == eAddTag )
	{
		Lenth = AddTagMeterPara(pOAD, Sch, InBuf, OutBufLen, OutBuf);
	}
	else
	{
		return 0x8000;
	}

	return Lenth; 
}

//--------------------------------------------------
//功能描述: 读取参变量（class8）数据长度 OI=4XXX
//         
//参数:     
//         	DataType[in]：	eData/eTagData
//         	*pOAD[in]：		OAD   
//         
//返回值:   返回数据长度 
//         
//备注内容:  无
//--------------------------------------------------
WORD GetRequestMeterParaLen( BYTE DataType, BYTE* pOAD)
{
	BYTE ClassAttribute,Sch,ClassIndex;
	TTwoByteUnion OI,Len;	
	const BYTE *Tag;
	
	Len.w = 0;
	lib_ExchangeData(OI.b,pOAD,2);
	ClassAttribute = (pOAD[2]&0x0f);
	ClassIndex = pOAD[3];
	Sch = SearchParaMeterOAD( OI.w );
	if( Sch == 0x80 )
	{
		return 0x8000;
	}
	
	switch( ClassAttribute )
	{
		case 2://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = ParaMeterObj[Sch].Type2;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			break;
		
		case 3://属性2
			if( OI.w == 0x4031 )//模组列表
			{
				if( ClassIndex > 1 )
				{
					return 0x8000;
				}
				
				Tag = (BYTE *)&T_ModuleVersion;
				Tag +=GetTagOffsetAddr( ClassIndex, Tag);
				Len.w = GetTypeLen( eTagData, Tag );
			}
			break;			
		default:
			return 0x8000;
	}	

	return Len.w;
}

