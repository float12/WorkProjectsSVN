//----------------------------------------------------------------------
//Copyright (C) 2003-2016 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	���� ������
//��������	2016-10-08
//����		�����Һ������оƬ�������
//�޸ļ�¼	
//----------------------------------------------------------------------
#include "AllHead.h"
#include "Lcd_GW3Phase09_M8.h"

#if( LCD_DRIVE_CHIP == LCD_CHIP_BU97950 )

//-----------------------------------------------
//			���ļ�ʹ�õĺ궨��
//-----------------------------------------------
//����
#define SLAVEADDRESS	0x3e			//0b00111110	���豸��ַ������
#define SOFTRST 		0xef			//0b11101111	��ʾ����״̬�������λ������	��һ���ֽڹ̶�Ϊ������
#define ICSET_ON		0xaf			//0b10101111	��ʾʹ�������֣�����	��һ���ֽڹ̶�Ϊ������
#define ICSET_OFF		0x2f			//0b00101111	��ʾ�ر������֣�����	��һ���ֽڹ̶�Ϊ������
#define DISCTL			0x07			//0b01110111	��ʾ���������֣�����  0x77	��һ���ֽڹ̶�Ϊ������
                                        //0b00110111	0x37	��һ���ֽڹ̶�Ϊ������
                                        //0b01110111	
                                        //0b01110111	
                                        //0b01110111	
                                        //0b01110111	
                                        //0b00000111	0x07--80HZ
#define APCTL			0x1F			//0b00011111	ȫ�Կ��������ֳ�ʼ����D0��D1Ϊ0�����������ȫ�Ժ�ȫ����	��һ���ֽڹ̶�Ϊ������
#define EVRSET			0x03			//0b00000011	���ӿɵ��������üĴ���ֵ����	��һ���ֽڹ̶�Ϊ������
#define ADSET			0x00			//0b00000000	��������ַ���������֣�����

//-----------------------------------------------
//		���ļ�ʹ�õĽṹ�壬�����壬ö��
//-----------------------------------------------

//-----------------------------------------------
//				ȫ��ʹ�õı���������
//-----------------------------------------------


//-----------------------------------------------
//				���ļ�ʹ�õı���������
//-----------------------------------------------

//���Ϳ�ʼ����
void SendStart( void )
{
	LCD_SDA_LOW;
//	Delayus( 1 );
	LCD_SCL_LOW;
}

//����ֹͣ����
void SendStop( void )
{
	LCD_SCL_HIGH;
//	Delayus( 1 );
	LCD_SDA_HIGH;
}

//����һ���ֽ�����
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

//������ʾ���
void EnableDisplay( void )
{
	SendStop();
  
	SendStart();

	SendByte( SLAVEADDRESS );

	SendByte( ICSET_ON );

	SendStop();
}
//�ر���ʾ
void ShutOff_Lcd(void)
{
	SendStop();
  
	SendStart();

	SendByte( SLAVEADDRESS );

	SendByte( ICSET_OFF );

	SendStop();
}

//��ʼ������оƬ
void InitLCDDriver( void )
{
	//api_Delay10us( 10 );

	SendStop();
	SendStart();

	SendByte( SLAVEADDRESS );		//���豸��ַ
        
//	//����Ƶ����λҺ������оƬ��������ʾ��ζ� @@@@@@
//	if( (RealTimer.Min==0) && (RealTimer.Sec==33) )
//	{
//	    SendByte( SOFTRST );
//	}	
        
	//�е������£��ô󹦺ķ�ʽ  
	SendByte( DISCTL );		//��0x01��ʾ��һ��BYTEΪ������
	
	SendByte( (EVRSET | 0xe0) );		//�ȼ�7��0.810������ѹ4.58V*0.810=3.71V,     ��ѹ5V*0.810=4.050V,
	//SendByte( EVRSET | 0x80 );			//�ȼ�1��0.967������ѹ4.58V*0.967=4.43V,     ��ѹ5V*0.967=4.835V,
	//SendByte( EVRSET | 0x40 );			//�ȼ�2��0.937������ѹ4.58V*0.937=4.29V,     ��ѹ5V*0.937=4.685V,
	//SendByte( EVRSET | 0xc0 );			//�ȼ�3��0.909������ѹ4.58V*0.909=4.16V,     ��ѹ5V*0.909=4.545V,
	//SendByte( EVRSET | 0x20 );			//�ȼ�4��0.882������ѹ4.58V*0.882=4.04V,     ��ѹ5V*0.882=4.410V,
	//SendByte( EVRSET | 0xa0 );			//�ȼ�5��0.857������ѹ4.58V*0.857=3.93V,     ��ѹ5V*0.857=4.285V,
	//SendByte( EVRSET | 0x10 );			//�ȼ�8��0.789������ѹ4.58V*0.789=3.61V,     ��ѹ5V*0.789=3.945V,
	
	SendByte( APCTL );		//ȡ��ȫ�Ժ�ȫ����

	SendByte( ICSET_ON );			//ʹ����ʾ

	SendStop();

	EnableDisplay();
}


void WriteLcdBufToDriver( BYTE Length )
{
	//�����ԣ�44M���Կ����������ԣ���Ϊ����Ҫ������ʱ
	BYTE i;

	SendStop();

	SendStart();

	SendByte( SLAVEADDRESS );		//���豸��ַ

	//д��ʾ����ǰ����Ҫ�ӵ�ַ000000b��������һ���ֽ�Ϊ��ʾ����
	//��0b0(��һ��BYTEΪ��ʾ����)0(��BYTEΪ��ַ����)000000(��ַ)
	SendByte( 0x00 );	//���뻺��д������֮���޷���д������

	//����λҺ������оƬ����
	for(i=0; i<34; i++)// send Data
	{
		SendByte( lcd[i] );
	}

	SendStop();

	EnableDisplay();

}

#endif//#if( LCD_DRIVE_CHIP == LCD_CHIP_BU97950 )
