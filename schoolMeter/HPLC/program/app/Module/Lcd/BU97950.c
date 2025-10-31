//----------------------------------------------------------------------
//Copyright (C) 2003-2016 烟台东方威思顿电气股份有限公司电表软件研发部 
//创建人	马亮 张玉猛
//创建日期	2016-10-08
//描述		三相表液晶驱动芯片处理程序
//修改记录	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Lcd_GW3Phase09_M8.h"

#if( LCD_DRIVE_CHIP == LCD_CHIP_BU97950 )

//-----------------------------------------------
//			本文件使用的宏定义
//-----------------------------------------------
//倒序
#define SLAVEADDRESS	0x3e			//0b00111110	从设备地址，倒序
#define SOFTRST 		0xef			//0b11101111	显示开启状态下软件复位，倒序	下一个字节固定为命令字
#define ICSET_ON		0xaf			//0b10101111	显示使能命令字，倒序	下一个字节固定为命令字
#define ICSET_OFF		0x2f			//0b00101111	显示关闭命令字，倒序	下一个字节固定为命令字
#define DISCTL			0x07			//0b01110111	显示控制命令字，倒序  0x77	下一个字节固定为命令字
                                        //0b00110111	0x37	下一个字节固定为命令字
                                        //0b01110111	
                                        //0b01110111	
                                        //0b01110111	
                                        //0b01110111	
                                        //0b00000111	0x07--80HZ
#define APCTL			0x1F			//0b00011111	全显控制命令字初始化（D0，D1为0），避免出现全显和全不显	下一个字节固定为命令字
#define EVRSET			0x03			//0b00000011	电子可调电阻设置寄存器值设置	下一个字节固定为命令字
#define ADSET			0x00			//0b00000000	缓冲区地址设置命令字，倒序

//-----------------------------------------------
//		本文件使用的结构体，共用体，枚举
//-----------------------------------------------

//-----------------------------------------------
//				全局使用的变量，常量
//-----------------------------------------------


//-----------------------------------------------
//				本文件使用的变量，常量
//-----------------------------------------------

//发送开始条件
void SendStart( void )
{
	LCD_SDA_LOW;
//	Delayus( 1 );
	LCD_SCL_LOW;
}

//发送停止条件
void SendStop( void )
{
	LCD_SCL_HIGH;
//	Delayus( 1 );
	LCD_SDA_HIGH;
}

//发送一个字节数据
void SendByte(BYTE data )
{
	BYTE i;

	for(i=0; i<8; i++)
	{
		if( (data & 0x01) == 0 )
		{
			LCD_SDA_LOW;
		}
		else
		{
			LCD_SDA_HIGH;
		}

		LCD_SCL_HIGH;

		data >>= 1;

		LCD_SCL_LOW;
	}
	LCD_SCL_HIGH;
	LCD_SCL_LOW;

}

//开启显示输出
void EnableDisplay( void )
{
	SendStop();
  
	SendStart();

	SendByte( SLAVEADDRESS );

	SendByte( ICSET_ON );

	SendStop();
}
//关闭显示
void ShutOff_Lcd(void)
{
	SendStop();
  
	SendStart();

	SendByte( SLAVEADDRESS );

	SendByte( ICSET_OFF );

	SendStop();
}

//初始化驱动芯片
void InitLCDDriver( void )
{
	//api_Delay10us( 10 );

	SendStop();
	SendStart();

	SendByte( SLAVEADDRESS );		//从设备地址
        
//	//不能频繁软复位液晶驱动芯片，否则显示会晃动 @@@@@@
//	if( (RealTimer.Min==0) && (RealTimer.Sec==33) )
//	{
//	    SendByte( SOFTRST );
//	}	
        
	//有电的情况下，用大功耗方式  
	SendByte( DISCTL );		//或0x01表示下一个BYTE为命令字
	
	SendByte( (EVRSET | 0xe0) );		//等级7（0.810），电压4.58V*0.810=3.71V,     电压5V*0.810=4.050V,
	//SendByte( EVRSET | 0x80 );			//等级1（0.967），电压4.58V*0.967=4.43V,     电压5V*0.967=4.835V,
	//SendByte( EVRSET | 0x40 );			//等级2（0.937），电压4.58V*0.937=4.29V,     电压5V*0.937=4.685V,
	//SendByte( EVRSET | 0xc0 );			//等级3（0.909），电压4.58V*0.909=4.16V,     电压5V*0.909=4.545V,
	//SendByte( EVRSET | 0x20 );			//等级4（0.882），电压4.58V*0.882=4.04V,     电压5V*0.882=4.410V,
	//SendByte( EVRSET | 0xa0 );			//等级5（0.857），电压4.58V*0.857=3.93V,     电压5V*0.857=4.285V,
	//SendByte( EVRSET | 0x10 );			//等级8（0.789），电压4.58V*0.789=3.61V,     电压5V*0.789=3.945V,
	
	SendByte( APCTL );		//取消全显和全不显

	SendByte( ICSET_ON );			//使能显示

	SendStop();

	EnableDisplay();
}


void WriteLcdBufToDriver( BYTE Length )
{
	//经测试，44M下仍可以正常屏显，认为不需要加入延时
	BYTE i;

	SendStop();

	SendStart();

	SendByte( SLAVEADDRESS );		//从设备地址

	//写显示缓存前必须要加地址000000b并标明下一个字节为显示数据
	//即0b0(下一个BYTE为显示数据)0(该BYTE为地址设置)000000(地址)
	SendByte( 0x00 );	//进入缓冲写操作，之后无法再写命令字

	//无软复位液晶驱动芯片！！
	for(i=0; i<34; i++)// send Data
	{
		SendByte( lcd[i] );
	}

	SendStop();

	EnableDisplay();

}

#endif//#if( LCD_DRIVE_CHIP == LCD_CHIP_BU97950 )
