//----------------------------------------------------------------------
//Copyright (C) 2016-2026 ��̨������˼�ٵ����ɷ����޹�˾�������з��� 
//������	������
//��������	2016.8.30
//����		֧��698.45������ӡ�ư�ͷ�ļ�
//�޸ļ�¼	
//----------------------------------------------------------------------
#ifndef __BOARD_HT_698_THREE_H
#define __BOARD_HT_698_THREE_H

#if( ( BOARD_TYPE == BOARD_HT_THREE_0134566 ) || (BOARD_TYPE == BOARD_HT_THREE_0131699)\
 || (BOARD_TYPE == BOARD_HT_THREE_0130347)  || (BOARD_TYPE == BOARD_HT_THREE_0132515))


//-----------------------------------------------
//				�궨��
//-----------------------------------------------
//ϵͳ֧�ֵ�����
//�����(�����������߻����������߻�ֱͨ����3)  �������1
#define MAX_PHASE		3

//����CPU����
#define CPU_TYPE		CPU_HT6025
//#define CPU_HT6025              	6

//����Դ
#define SAMPLE_CHIP			CHIP_HT7038
//#define CHIP_HT7017			6
//#define CHIP_HT7038			7
//#define CHIP_HT7026			8

//ʱ��оƬ����
#define RTC_CHIP_TYPE		SOC_CLOCK
//SOC_CLOCK					SOC����ʱ��
//OUT_CLOCK					�ⲿ����ʱ��

//ʱ��оƬ
#define RTC_CHIP			RTC_HT_INCLOCK
//RTC_CHIP_8025T			֧�ִ��¶Ȳ�����8025
//RTC_HT_INCLOCK			��Ȫ����RTC

//Һ������ѡ��
#define LCD_TYPE			LCD_GW3PHASE_09
//LCD_GW3PHASE_09			09��8�¹��������Һ��
//LCD_HT_SINGLE_METER		��Ȫ��������Һ��

//Һ������оƬ��ַ�߿��
#define LCD_DRIVE_CHIP			LCD_CHIP_BU97950
//#define LCD_CHIP_1623					7
//#define LCD_CHIP_1622					6
//#define LCD_CHIP_BU97950				5
//#define LCD_CHIP_BU9792               4
//#define LCD_CHIP_PCF8576              3
//#define LCD_CHIP_BU9794               2
//#define LCD_CHIP_BU9799               1
//#define LCD_CHIP_NO                   0

//ͨѶ����ѡ�񣬣������⡢Զ���⣩
#define COMM_TYPE				COMM_FAR
//COMM_NEAR		������
//COMM_FAR		Զ����

#define MASTER_MEMORY_CHIP			CHIP_24LC256
//CHIP_24LC256
//CHIP_24LC512
//CHIP_NO							û�к��洢оƬ  ֻ�е���Ƭ����ѡû��

//�ڶ�Ƭ�洢оƬ(D4)
#define SLAVE_MEM_CHIP				CHIP_24LC256

//����Ƭ�洢������  ֻ���������ռ�����ݴ洢
#define THIRD_MEM_CHIP				CHIP_GD25Q64C //�ڲ���RAM�׾�Flash

//����Ƭ�洢���Ƿ������ݱ��ݹ���
#define SLE_THIRD_MEM_BACKUP		NO

//--------------------------------------------

//Ԥ���ѱ�׼
#define PREPAY_STANDARD		PREPAY_GUOWANG_698
//PREPAY_GUOWANG_698	 	����698��Լ
//PREPAY_GUOWANG_13			����13�淶
//PREPAY_NANWANG_15			����15�淶

//���ܷ�ʽ
#define ENCRYPT_MODE		ENCRYPT_ESAM
//ENCRYPT_ESAM				Ӳ����
//ENCRYPT_SOFTWARE			�����

//��Ƭ����
#define CARD_COM_TYPE		CARD_NO
//CARD_NO
//CARD_CONTACT
//CARD_RADIO

//ESAMͨ��ģʽ
#define ESAM_COM_TYPE		TYPE_SPI
//TYPE_SPI
//TYPE_7816

//ѡ��·���� ֻ��ѡ��4(Ӳ�����ں�ģ�⴮�ڵ�����)
#define MAX_COM_CHANNEL_NUM			4

//IIC��ģ��Ļ���Ӳ����
#define SEL_24LC256_COMMUNICATION_MOD	IIC_SIM
//IIC_SIM
//IIC_HARD

//�Ƿ�ѡ�����ⱨ�Ľ��е͹��Ļ���
#define SEL_CONTINUS_FRAM_WAKEUP		IR_WAKEUP_PHOTO_SWITCH

//-----------------------------------------------------------
//�����ж�
//-----------------------------------------------------------
	

//////////////////////////////////////////////////////////////////////////
//							IIC�ܽŶ���
//////////////////////////////////////////////////////////////////////////
//EEPROM��IIC����
#define EEPROM_I2C_SDA_PORT_IN			HT_GPIOD->PTDIR &=~(1<<1)
#define EEPROM_I2C_SDA_PORT_DATA_IN		(HT_GPIOD->PTDAT & (1<<1))
#define EEPROM_SCL_L					HT_GPIOD->PTCLR |= (1<<2)				
#define EEPROM_SCL_H					HT_GPIOD->PTSET |= (1<<2);\
										HT_GPIOD->PTDIR |= (1<<2)
#define EEPROM_SDA_L					HT_GPIOD->PTCLR |= (1<<1);\
										HT_GPIOD->PTDIR |= (1<<1)
#define EEPROM_SDA_H					EEPROM_I2C_SDA_PORT_IN
                      
// EEPROM д�뱣���ܽ� �йܽ����ӣ������һֱ��Ϊд������Ч
#define ENABLE_WRITE_PROTECT1		    ;

#define ENABLE_WRITE_PROTECT1_FOR_ADDR	HT_GPIOD->PTDIR |= (1<<0);\
										HT_GPIOD->PTOD |= (1<<0);\
										HT_GPIOD->PTSET |= (1<<0)

#define DISABLE_WRITE_PROTECT1		    HT_GPIOD->PTDIR |= (1<<0);\
										HT_GPIOD->PTOD |= (1<<0);\
										HT_GPIOD->PTCLR |= (1<<0)
#define ENABLE_WRITE_PROTECT2		    ;
#define ENABLE_WRITE_PROTECT2_FOR_ADDR	HT_GPIOD->PTDIR |= (1<<3);\
										HT_GPIOD->PTOD |= (1<<3);\
										HT_GPIOD->PTSET |= (1<<3)

#define DISABLE_WRITE_PROTECT2		    HT_GPIOD->PTDIR |= (1<<3);\
										HT_GPIOD->PTOD |= (1<<3);\
										HT_GPIOD->PTCLR |= (1<<3)

//�͹���������ǻ����EE����Ҫ���ܽ���Ϊ����̬����ֹ�����쳣
#define EEPROM_INIT_LOW_POWER				HT_GPIOD->PTDIR |= ((1<<0)|(1<<1)|(1<<2)|(1<<3));\
											HT_GPIOD->PTOD &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));\
											HT_GPIOD->PTSET |= ((1<<0)|(1<<1)|(1<<2)|(1<<3))
											
//EEPROM �ĵ�ַ
#define MEM_24LC25601_DEVADDR		    0xa0
#define MEM_24LC25602_DEVADDR		    0xae
#define MEM_24LC25601_CONFIG_DEVADDR	0xb0
#define MEM_24LC25602_CONFIG_DEVADDR	0xbe


//////////////////////////////////////////////////////////////////////////
// Һ������оƬIIC����
#define LCD_I2C_SDA_PORT_IN             HT_GPIOC->PTDIR &= ~(1<<14)
#define LCD_I2C_SDA_PORT_DATA_IN        (HT_GPIOC->PTDAT & (1<<14))
#define LCD_SCL_LOW                     HT_GPIOC->PTCLR |= (1<<13)
#define LCD_SCL_HIGH                    HT_GPIOC->PTSET |= (1<<13);\
										HT_GPIOC->PTDIR |= (1<<13)
#define LCD_SDA_LOW                     HT_GPIOC->PTCLR |= (1<<14);\
										HT_GPIOC->PTDIR |= (1<<14)
#define LCD_SDA_HIGH                    LCD_I2C_SDA_PORT_IN

/////////////////////////////////////////////////////////////////////////
//ʱ��оƬ��IIC����
#define CLOCK_I2C_SDA_PORT_IN             	HT_GPIOB->PTDIR &= ~(1<<5)
#define CLOCK_I2C_SDA_PORT_DATA_IN        	(HT_GPIOB->PTDAT & (1<<5))
#define CLOCK_SCL_L                     	HT_GPIOB->PTCLR |= (1<<4)
#define CLOCK_SCL_H                    		HT_GPIOB->PTSET |= (1<<4);\
											HT_GPIOB->PTDIR |= (1<<4)
#define CLOCK_SDA_L                     	HT_GPIOB->PTCLR |= (1<<5);\
											HT_GPIOB->PTDIR |= (1<<5)
#define CLOCK_SDA_H                    		CLOCK_I2C_SDA_PORT_IN

//////////////////////////////////////////////////////////////////////////
//SPI CS����
// FLASHƬѡ�ܽ�    
#define FLASH_CS_ENABLE				    HT_GPIOB->PTCLR |= (1<<10)
#define FLASH_CS_DISABLE			    HT_GPIOB->PTSET |= (1<<10)
// ����оƬƬѡ�ܽ� 
#define SAMPLE_CS_ENABLE			    HT_GPIOC->PTCLR |= (1<<7)
#define SAMPLE_CS_DISABLE			    HT_GPIOC->PTSET |= (1<<7)
// ESAMоƬƬѡ�ܽ� 
#define ESAM_CS_ENABLE			    	HT_GPIOD->PTCLR |= (1<<14)
#define ESAM_CS_DISABLE			    	HT_GPIOD->PTSET |= (1<<14)

//����ģ��                     
#define EMU_UARTSTA						;
#define EMU_UARTBUF						;

// �ز�ģ����ƹܽŶ���
#define ENABLE_PLC_PROGRAMM         	HT_GPIOA->PTDIR |= (1<<2);\
										HT_GPIOA->PTCLR |= (1<<2)
#define DISABLE_PLC_PROGRAMM        	HT_GPIOA->PTDIR |= (1<<2);\
										HT_GPIOA->PTSET |= (1<<2)

//�ز�����״̬��� �ߵ�ƽ��ʾ�ز�����
#define CHECK_CARRRWAVE_SENDING     	(HT_GPIOA->PTDAT & (1<<3))
#define RESET_PLC_MODUAL            	HT_GPIOA->PTDIR |= (1<<1);\
										HT_GPIOA->PTCLR |= (1<<1);\
										api_Delayms( 250 );\
										HT_GPIOA->PTSET |= (1<<1)

//ģ���¼�֪ͨ �ߵ�ƽ��ʾ���¼����� �͵�ƽ��ʾû���¼�����
#define CANCEL_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<0);\
									HT_GPIOA->PTCLR |= (1<<0)
#define NOTICE_EVENTOUT			    HT_GPIOA->PTDIR |= (1<<0);\
									HT_GPIOA->PTSET |= (1<<0)

//�๦�ܶ��������
#define MULTI_FUN_PORT_OUT_L			HT_GPIOC->IOCFG &= ~(1<<8);\
										HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTCLR |= (1<<8)
										
//�๦�ܶ��������
#define MULTI_FUN_PORT_OUT_H			HT_GPIOC->IOCFG &= ~(1<<8);\
										HT_GPIOC->PTDIR |= (1<<8);\
										HT_GPIOC->PTSET |= (1<<8)
										
//�л���ʱ���������
#define SWITCH_TO_CLOCK_PULS_OUTPUT		HT_GPIOC->IOCFG |= (1<<8)

//��������     
#define	OPEN_BACKLIGHT			        if( SelThreeBoard == BOARD_HT_THREE_0134566 )\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTCLR |= (1<<3);\
										}\
										else\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTSET |= (1<<3);\
										}
	
#define	CLOSE_BACKLIGHT			       	if( SelThreeBoard == BOARD_HT_THREE_0134566 )\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTSET |= (1<<3);\
										}\
										else\
										{\
											HT_GPIOB->PTDIR |= (1<<3);\
											HT_GPIOB->PTCLR |= (1<<3);\
										}	
//��բָʾ��     
#define RELAY_LED_ON			       	 HT_GPIOD->PTCLR |= (1<<12)                                          
#define RELAY_LED_OFF			         HT_GPIOD->PTSET |= (1<<12)
//����оƬ��������
#define EMU_ACTIVE						;
//�й��������ָʾ��
#define PULSE_LED_ON			        ;
#define PULSE_LED_OFF			        ;
//����ָʾ��        
#define WARN_ALARM_LED_ON		        ;//HT_GPIOD->PTCLR |= (1<<7)
#define WARN_ALARM_LED_OFF		        ;//HT_GPIOD->PTSET |= (1<<7)
//������
#if(PREPAY_MODE == PREPAY_LOCAL)
//��ͨ�˿� ��©�����
#define BEEP_OFF						HT_GPIOE->IOCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD &= ~(1<<0);\
										HT_GPIOE->PTSET |= (1<<0);
//��һ���ù���(PWM���)
#define BEEP_ON							HT_GPIOE->IOCFG |= (1<<0);\
										HT_GPIOE->AFCFG &= ~(1<<0);\
										HT_GPIOE->PTDIR |= (1<<0);\
										HT_GPIOE->PTOD |= (1<<0);
#endif

//������Դ�Ƿ������
#define SECPOWER_IS_DOWN	        	(HT_GPIOD->PTDAT & (1<<9))

//�����̵�������
#define WARN_RELAY_OPEN_4566					HT_GPIOB->PTSET |= (1<<1);\
												HT_GPIOB->PTDIR |= (1<<1);
//�����̵���ȡ��
#define WARN_RELAY_CLOSE_4566					HT_GPIOB->PTCLR |= (1<<1);\
												HT_GPIOB->PTDIR |= (1<<1);

//��բ �͵�ƽ��Ч������ͬʱΪ��--ʵ����248v������ӵ�ʱ30ms�������̵���
#define CLOSE_POWER_RELAY_PLUS_INSIDE_4566		HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(120);\
												HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTSET|=(1<<10);
//��բ �͵�ƽ��Ч������ͬʱΪ��
#define OPEN_POWER_RELAY_PLUS_INSIDE_4566		HT_GPIOC->PTSET|=(1<<10);HT_GPIOC->PTCLR|=(1<<9);\
												HT_GPIOC->PTDIR|=(1<<9);HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(120);\
												HT_GPIOC->PTSET|=(1<<9);HT_GPIOC->PTSET|=(1<<10);
//���ü̵�����բ
#define CLOSE_POWER_RELAY_LEVEL_OUTSIDE_4566 	HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);
//���ü̵�����բ
#define OPEN_POWER_RELAY_LEVEL_OUTSIDE_4566		HT_GPIOC->PTSET|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);
//���ü̵�����բ(���巽ʽ)
#define OPEN_POWER_RELAY_PLUS_OUTSIDE_4566		HT_GPIOC->PTCLR|=(1<<10);\
												HT_GPIOC->PTDIR|=(1<<10);\
												api_Delayms(400);\
												HT_GPIOC->PTSET|=(1<<9);\
												HT_GPIOC->PTSET|=(1<<10);  
//���ڷ�ֹ�͹��ĳ�ʼ���ܽ�ʱͻȻ�ϵ絼�¼̵�����բ
#define LOWPOWER_INVALID_RELAY_CTRL_4566		HT_GPIOC->PTOD |= 0x0600;\
												HT_GPIOC->PTCLR |= 0x0600;\
												HT_GPIOC->PTDIR |= 0x0600 

//�����̵�������
#define WARN_RELAY_OPEN				    		WARN_RELAY_OPEN_4566

//�����̵���ȡ��
#define WARN_RELAY_CLOSE						WARN_RELAY_CLOSE_4566
//��բ �ߵ�ƽ��Ч������ͬʱΪ��
#define CLOSE_POWER_RELAY_PLUS_INSIDE			CLOSE_POWER_RELAY_PLUS_INSIDE_4566

//��բ �ߵ�ƽ��Ч������ͬʱΪ��
#define OPEN_POWER_RELAY_PLUS_INSIDE			OPEN_POWER_RELAY_PLUS_INSIDE_4566

//���ü̵�����բ
#define CLOSE_POWER_RELAY_LEVEL_OUTSIDE			CLOSE_POWER_RELAY_LEVEL_OUTSIDE_4566

//���ü̵�����բ
#define OPEN_POWER_RELAY_LEVEL_OUTSIDE			OPEN_POWER_RELAY_LEVEL_OUTSIDE_4566

//���ü̵�����բ(���巽ʽ)
#define OPEN_POWER_RELAY_PLUS_OUTSIDE			OPEN_POWER_RELAY_PLUS_OUTSIDE_4566

//���ڷ�ֹ�͹��ĳ�ʼ���ܽ�ʱͻȻ�ϵ絼�¼̵�����բ
#define LOWPOWER_INVALID_RELAY_CTRL				LOWPOWER_INVALID_RELAY_CTRL_4566


//���ü̵���״̬���
#define RELAY_STATUS_OPEN	        (!(HT_GPIOD->PTDAT & (1<<9)))


//////////////////////////////////////////////////////////////////////////
//							������عܽŲ���
//////////////////////////////////////////////////////////////////////////
//��ȡ������״̬ �����Ǹߵ�ƽ
#define UP_KEY_PRESSED          ((HT_GPIOA->PTDAT & (1<<10)))//�Ϸ�����
#define DOWN_KEY_PRESSED        ((HT_GPIOA->PTDAT & (1<<11)))
#define PROG_KEY_PRESSED        0//��̰���
// ǿ�ų���� �͵�ƽ��ʾ��⵽
#define CHECK_MAGNETIC_STATUS   (!(HT_GPIOD->PTDAT & (1<<6)))
#define MAGNETIC_IS_OCCUR       CHECK_MAGNETIC_STATUS
// ���ϸǸ����� �ߵ�ƽ��⵽����
#define UP_COVER_IS_OPEN        (!(HT_GPIOA->PTDAT & (1<<7)))
#define UP_COVER_IS_CLOSE       ((HT_GPIOA->PTDAT & (1<<7)))
#define ENABLE_UP_COVER_INT     ;
#define DIS_UP_COVER_INT        ;
// ��β������ �ߵ�ƽ��⵽����
#define END_COVER_IS_OPEN       (!(HT_GPIOA->PTDAT & (1<<8)))
#define END_COVER_IS_CLOSE      ((HT_GPIOA->PTDAT & (1<<8)))
#define ENABLE_END_COVER_INT    ;
#define DIS_END_COVER_INT       ;

#if(PREPAY_MODE == PREPAY_LOCAL)
#define CARD_IN_SLOT       		(!(HT_GPIOE->PTDAT &0x0100))//���ڿ����У�������IC_Key
#endif  

///////////////////////////////////////////////////////////////////////////
// 485���ڷ�����ƶ���
//////////////////////////////////////////////////////////////////////////
#define ENABLE_HARD_SCI_0_RECE		    ;
#define ENABLE_HARD_SCI_0_SEND		    ;
#define ENABLE_HARD_SCI_1_RECE		    ;
#define ENABLE_HARD_SCI_1_SEND		    ;
#define ENABLE_HARD_SCI_2_RECE		    HT_GPIOB->PTSET=(1<<0)
#define ENABLE_HARD_SCI_2_SEND		    HT_GPIOB->PTCLR=(1<<0)
#define ENABLE_HARD_SCI_3_RECE		    HT_GPIOE->PTSET=(1<<6)
#define ENABLE_HARD_SCI_3_SEND		    HT_GPIOE->PTCLR=(1<<6)
#define ENABLE_HARD_SCI_4_RECE		    ;
#define ENABLE_HARD_SCI_4_SEND		    ;
#define ENABLE_HARD_SCI_5_RECE		    ;
#define ENABLE_HARD_SCI_5_SEND		    ;
#define ENABLE_HARD_SCI_6_RECE		    ;
#define ENABLE_HARD_SCI_6_SEND		    ;
#define ENABLE_HARD_SCI_7_RECE		    ;
#define ENABLE_HARD_SCI_7_SEND		    ;
#define ENABLE_HARD_SCI_8_RECE		    ;
#define ENABLE_HARD_SCI_8_SEND		    ;

//////////////////////////////////////////////////////////////////////////
// ��Դ���ƶ���
//////////////////////////////////////////////////////////////////////////
// ����оƬ��Դ���� �͵�ƽ�򿪵�Դ
#define POWER_SAMPLE_OPEN				HT_GPIOA->PTDIR |= (1<<4);\
										HT_GPIOA->PTOD |= (1<<4);\
										HT_GPIOA->PTCLR |= (1<<4)
#define POWER_SAMPLE_CLOSE				HT_GPIOA->PTDIR |= (1<<4);\
										HT_GPIOA->PTOD |= (1<<4);\
										HT_GPIOA->PTSET |= (1<<4)	

//eeprom��Դ���� �͵�ƽ�򿪵�Դ
#define POWER_FLASH_OPEN				HT_GPIOD->PTDIR |= (1<<5);\
										HT_GPIOD->PTOD |= (1<<5);\
										HT_GPIOD->PTCLR |= (1<<5)	
#define POWER_FLASH_CLOSE				HT_GPIOD->PTDIR |= (1<<5);\
										HT_GPIOD->PTOD |= (1<<5);\
										HT_GPIOD->PTSET |= (1<<5)
										
#define POWER_EEPROM_OPEN				if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											POWER_FLASH_OPEN;\
										}
#define POWER_EEPROM_CLOSE				if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											POWER_FLASH_CLOSE;\
										}

//������յ�Դ���� �͵�ƽ�򿪵�Դ
#define POWER_HONGWAI_REC_OPEN			HT_GPIOD->PTDIR |= (1<<10);\
										HT_GPIOD->PTOD |= (1<<10);\
										HT_GPIOD->PTCLR |= (1<<10)	
#define POWER_HONGWAI_REC_CLOSE			HT_GPIOD->PTDIR |= (1<<10);\
										HT_GPIOD->PTOD |= (1<<10);\
										HT_GPIOD->PTSET |= (1<<10)		

//���ⷢ�͵�Դ���� �͵�ƽ�򿪵�Դ
#define POWER_HONGWAI_TXD_OPEN			;//HT_GPIOE->PTDIR |= (1<<8);\
										//HT_GPIOE->PTOD |= (1<<8);\
										//HT_GPIOE->PTCLR |= (1<<8)	
#define POWER_HONGWAI_TXD_CLOSE			;//HT_GPIOE->PTDIR |= (1<<8);\
										//HT_GPIOE->PTOD |= (1<<8);\
										//HT_GPIOE->PTSET |= (1<<8)		

//lcd��Դ���� �ߵ�ƽ�򿪵�Դ
#define POWER_LCD_OPEN				    HT_GPIOD->PTDIR |= (1<<15);\
										HT_GPIOD->PTOD |= (1<<15);\
										HT_GPIOD->PTSET |= (1<<15)	
#define POWER_LCD_CLOSE				    HT_GPIOD->PTDIR |= (1<<15);\
										HT_GPIOD->PTOD |= (1<<15);\
										HT_GPIOD->PTCLR |= (1<<15)	
// ESAM��Դ���� �͵�ƽ�򿪵�Դ
#define POWER_ESAM_OPEN				    HT_GPIOD->PTDIR |= (1<<11);\
										HT_GPIOD->PTOD |= (1<<11);\
										HT_GPIOD->PTCLR |= (1<<11)
#define POWER_ESAM_CLOSE			    HT_GPIOD->PTDIR |= (1<<11);\
										HT_GPIOD->PTOD |= (1<<11);\
										HT_GPIOD->PTSET |= (1<<11)

#define ESAM_POWER_IS_CLOSE             (HT_GPIOD->PTDAT & (1<<11))

//������������Դ �͵�ƽ�򿪵�Դ
#define POWER_HALL_OPEN				    ;//HT_GPIOD->PTDIR |= (1<<8);\
										//HT_GPIOD->PTOD |= (1<<8);\
										//HT_GPIOD->PTCLR |= (1<<8)
#define POWER_HALL_CLOSE	            ;//HT_GPIOD->PTDIR |= (1<<8);\
										//HT_GPIOD->PTOD |= (1<<8);\
										//HT_GPIOD->PTSET |= (1<<8)
										
#define POWER_CV485_OPEN				HT_GPIOB->PTDIR |= (1<<2);\
										HT_GPIOB->PTOD |= (1<<2);\
										HT_GPIOB->PTCLR |= (1<<2)
#define POWER_CV485_CLOSE				HT_GPIOB->PTDIR |= (1<<2);\
										HT_GPIOB->PTOD |= (1<<2);\
										HT_GPIOB->PTSET |= (1<<2)
										
#if( PREPAY_MODE == PREPAY_LOCAL )
//���͸�λ����5ms����λCPU���������Ŀǰ��Ӳ�������������
#define RESET_CARD						HT_GPIOB->PTDIR |= (1<<12);\
										HT_GPIOB->PTOD |= (1<<12);\
										HT_GPIOB->PTCLR |= (1<<12);\
										api_Delayms(5);\
										HT_GPIOB->PTDIR |= (1<<12);\
										HT_GPIOB->PTOD |= (1<<12);\
										HT_GPIOB->PTSET |= (1<<12)
										
//CARD��Դ���� �͵�ƽ�򿪵�Դ ֻ��ESAM��Դ�򿪺� CARD��Դ����Ч
#define POWER_CARD_OPEN					HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3);\
										HT_GPIOE->PTCLR |= (1<<3)  										

#define POWER_CARD_CLOSE				HT_GPIOE->PTDIR |= (1<<3);\
										HT_GPIOE->PTOD |= (1<<3);\
										HT_GPIOE->PTSET |= (1<<3) 
										
#endif//if( PREPAY_MODE == PREPAY_LOCAL )

//�͹����ϱ�ͨ����Դ����
#define	POWER_REPORT_CR_OPEN			if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13);\
											HT_GPIOD->PTSET |= (1<<13);\
										}
										
#define	POWER_REPORT_CR_CLOSE			if( SelThreeBoard != BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD &= ~(1<<13);\
											HT_GPIOD->PTUP |= (1<<13);\
										}
							
//���Ǽ���Դ �ߵ�ƽ�򿪵�Դ
#define POWER_COVER_OPEN				if( SelThreeBoard == BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13); \
											HT_GPIOD->PTSET = (1<<13);\
										}  										

#define POWER_COVER_CLOSE				if( SelThreeBoard == BOARD_HT_THREE_0132515 )\
										{\
											HT_GPIOD->PTDIR |= (1<<13);\
											HT_GPIOD->PTOD |= (1<<13);\
											HT_GPIOD->PTCLR =  (1<<13);\
										}
#define POWER_COVER_INT					(HT_GPIOD->PTDAT & (1<<13))
						

//-----------------------------------------------
//				�ṹ�壬�����壬ö��
//-----------------------------------------------
// ���������ṹ����
typedef struct
{
	BYTE SerialType;//�������� 0: 485_I 1:IR 2:CR 3:485_II
	BYTE SCI_Ph_Num;								// �����ں�
	BYTE (*SCIInit)( BYTE SCI_Ph_Num );				// ��ʼ������
	BYTE (*SCIEnableRcv)( BYTE SCI_Ph_Num );		// �������  
	BYTE (*SCIDisableRcv)( BYTE SCI_Ph_Num );		// ��ֹ���ս���
	BYTE (*SCIEnableSend)( BYTE SCI_Ph_Num );		// ������
	BYTE (*SCIBeginSend)( BYTE SCI_Ph_Num );		// ��ʼ����
}TypeDef_Pulic_SCI;

//GPIO��������
typedef enum
{
	eGPIO_TYPE_COMMON = 0,			//��ͨIO
	eGPIO_TYPE_AF_FIRST,			//��һ���ù���
	eGPIO_TYPE_AF_SECOND,			//�ڶ����ù���
	eGPIO_TYPE_NONE					//��
}eGPIO_TYPE;

//GPIO��������
typedef enum
{
	eGPIO_DIRECTION_IN = 0,			//����
	eGPIO_DIRECTION_OUT,			//���
	eGPIO_DIRECTION_NONE			//��
}eGPIO_DIRECTION;

//GPIOģʽ����
typedef enum
{
	eGPIO_MODE_PP = 0,				//����
	eGPIO_MODE_OD,					//��©
	eGPIO_MODE_IPU,					//����
	eGPIO_MODE_IPD,					//����
	eGPIO_MODE_FLOATING,			//����
	eGPIO_MODE_NONE					//��
}eGPIO_MODE;

//GPIO�������
typedef enum
{
	eGPIO_OUTPUT_HIGH = 0,			//��
	eGPIO_OUTPUT_LOW,				//��
	eGPIO_OUTPUT_HI,				//����̬
	eGPIO_OUTPUT_NONE				//��
}eGPIO_OUTPUT;

//GPIO��ʼ���ṹ����
typedef struct
{
	eGPIO_TYPE eGPIO_TYPE;				//GPIO������(����ȡ�� GPIO_TYPE �ж����ֵ)
	eGPIO_DIRECTION eGPIO_DIRECTION;	//GPIO�ķ���(����ȡ�� GPIO_DIRECTION �ж����ֵ)
	eGPIO_MODE eGPIO_MODE;				//GPIO��ģʽ(����ȡ�� GPIO_MODE �еĶ���ֵ)
	eGPIO_OUTPUT eGPIO_OUTPUT;			//GPIO�����(����ȡ�� GPIO_OUTPUT �еĶ���ֵ)
}GPIO_InitTypeDef;

//-----------------------------------------------
//				��������
//-----------------------------------------------
extern BYTE IsExitLowPower;//ģ���ڱ���
extern const TypeDef_Pulic_SCI SerialMap[MAX_COM_CHANNEL_NUM];
extern WORD WakeUpTimer;
extern WORD MaxWakeupTimer;
extern DWORD LowPowerSecCount;     //�͹����ۼ�����

//-----------------------------------------------
// 				��������
//-----------------------------------------------


//-----------------------------------------------
//��������: CPU�ܽų�ʼ����ͳһ����
//
//����: 	
//          Type[in]:	ePowerOnMode		�����ϵ���еĹܽų�ʼ��
//						ePowerDownMode		�͹��Ľ��еĹܽų�ʼ��
//						ePowerWakeUpMode	�͹��Ļ��ѽ��еĹܽų�ʼ��
//����ֵ: 	��
//
//��ע:   
//-----------------------------------------------
void InitPort( ePOWER_MODE Type );

//--------------------------------------------------
//��������:  �뵥�ຯ����Ӧ��������SPI���ã��޷��ر�esam��Դ���պ���
//         
//����  : ��
//
//����ֵ:    
//         
//��ע����:  �պ��� ������
//--------------------------------------------------
void ESAMSPIPowerDown( ePOWER_MODE PowerMode );
//--------------------------------------------------
//��������:  ��SPI�¹��ص��豸���и�λ
//
//����:     eCOMPONENT_TYPE  Type[in]  eCOMPONENT_SPI_SAMPLE/eCOMPONENT_SPI_FLASH/eCOMPONENT_SPI_LCD/eCOMPONENT_SPI_ESAM/eCOMPONENT_SPI_CPU
//         BYTE  Time[in]  ��ʱʱ�䣨MS��
//
//����ֵ:    BOOL  TRUE: �ɹ� FALSE: ʧ��
//
//��ע:	  ��������ĸ�λesam spi ���⸴λ�ᵼ�½���Ӧ������ʧЧ
//--------------------------------------------------
BOOL ResetSPIDevice( eCOMPONENT_TYPE Type, BYTE Time );


//-----------------------------------------------
//��������: ��ʼ��Һ������оƬ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void InitLCDDriver(void);

//-----------------------------------------------
//��������: �ر���ʾ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void ShutOff_Lcd(void);

//-----------------------------------------------
//��������: ���ڼ��ά��cpu�ļĴ���ֵ
//
//����: 	��
//                    
//����ֵ:  	��
//
//��ע:   
//-----------------------------------------------
void Maintain_MCU( void );

//-----------------------------------------------
//��������: ��ȡADת���Ľ����С����λ���ǹ̶��ģ����С���㲻���ʣ���Ҫ�������Լ�ת��
//����:
//				BYTE Mode[in]		��׼��ѹ�Ƿ�У׼ 0x00 У׼ 0x55��У׼
//				Type[in]			��ȡAD���ݵ�����
//				SYS_POWER_AD		    		0//ϵͳ��ѹ ������λС�� ��λv
//				SYS_CLOCK_VBAT_AD		    	1//ʱ�ӵ�ص�ѹ ������λС�� ��λv
//				SYS_READ_VBAT_AD	    		2//�����صĵ�ѹ ������λС�� ��λv
//				SYS_TEMPERATURE_AD		    	3//�¶�AD���� ����һλС��
//                    
//����ֵ:  	ADת���Ľ����ֻ���¶�֧�ָ�ֵ��������֧�ָ�ֵ���¶ȵĸ�ֵ���ò����ʾ
//

//��ע:   
//-----------------------------------------------
short GetADValue( BYTE Mode, BYTE Type);


//-----------------------------------------------
//��������: ����оƬURAT��ȡ����оƬһ���ֽ�
//
//����:		�� 
//                    
//����ֵ: 
//		   ���ض�ȡ�����ݣ���ʱ����0
//��ע:   
//-----------------------------------------------
BYTE Drv_SampleReadByte(void);

//-----------------------------------------------
//��������: ����оƬURATд�����оƬһ���ֽ�
//
//����:		Data[in] Ҫд������оƬ������ 
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void Drv_SampleWriteByte(BYTE Data);

//-----------------------------------------------
//��������: ����Χ�豸�ĵ�Դ����
//
//����:		Type[in] 0:�ϵ�ʱ���ⲿ��Դ�Ŀ���  1:����
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void PowerCtrl( BYTE Type );

//-----------------------------------------------
//��������: �����ʼ������
//
//����:		��
//                    
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void InitBoard( void );

//-----------------------------------------------
//��������: SPIƬѡ��������
//
//����:		No[in]		��Ҫѡ���оƬ
//          Do[in]		TRUE��ʹ��Ƭѡ   FALSE����ֹƬѡ
//
//����ֵ: ��
//		   
//��ע:   
//-----------------------------------------------
void DoSPICS(WORD No, WORD Do);


//-----------------------------------------------
//��������:     ���ö๦�ܶ��Ӻ���
//
//����:	    Type[in] �๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
//          
//
//����ֵ: 	    ��
//		   
//��ע:   
//-----------------------------------------------
BYTE api_MultiFunPortSet( BYTE Type );
//-----------------------------------------------
//��������:     �๦�ܶ��ӿ��ƺ���
//
//����:	    Type[in] �๦�ܶ��ӵ�������ͣ�ʱ�����塢�������ڡ�ʱ���л���
//          
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void api_MultiFunPortCtrl( BYTE Type );




//-----------------------------------------------
//��������: systick�жϷ������10ms�ж�һ��
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void SysTickIRQ_Service(void);
//-----------------------------------------------
//��������: PMU�жϴ�����
//
//����:		��
//
//����ֵ: 	��
//
//��ע:
//-----------------------------------------------
void PMUIRQ_Service( void );
//-----------------------------------------------
//��������: �ⲿ�ж�0�������Ŀǰ��������ɼ�
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void EXTI0IRQ_Service( void );

//Ŀǰû����
void EXTI1IRQ_Service( void );

//Ŀǰû����
void EXTI2IRQ_Service( void );

//Ŀǰû����
void EXTI3IRQ_Service( void );

//Ŀǰû����
void EXTI4IRQ_Service( void );

//Ŀǰû����
void EXTI5IRQ_Service( void );

//Ŀǰû����
void EXTI6IRQ_Service( void );

//-----------------------------------------------
//��������: ������0�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART0IRQ_Service( void );

//-----------------------------------------------
//��������: ������1�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART1IRQ_Service( void );

//-----------------------------------------------
//��������: ������2�жϷ�����򣬾����Ӧ�ĸ��߼����ڣ��ο�SerialMap����
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void UART2IRQ_Service( void );

//Ŀǰû����
void UART3IRQ_Service( void );

//Ŀǰû����
void UART4IRQ_Service( void );

//Ŀǰû����
void UART5IRQ_Service( void );

//Ŀǰû����
void TIMER0IRQ_Service( void );

//Ŀǰû����
void TIMER1IRQ_Service( void );

//Ŀǰû����
void TIMER2IRQ_Service( void );

//Ŀǰû����
void TIMER3IRQ_Service( void );

//-----------------------------------------------
//��������: ʱ���жϷ���������ڲ����롢�֡�ʱ�����־
//
//����:		��
//
//����ֵ: 	��
//		   
//��ע:   
//-----------------------------------------------
void RTCIRQ_Service( void );

//Ŀǰû����
void TIMER4IRQ_Service( void );

//Ŀǰû����
void TIMER5IRQ_Service( void );

//Ŀǰû����
void UART6IRQ_Service( void );

//Ŀǰû����
void EXTI7IRQ_Service( void );

//Ŀǰû����
void EXTI8IRQ_Service( void );

//Ŀǰû����
void EXTI9IRQ_Service( void );

void HTMCU_GoToLowSpeed(void);
#endif//#if( (BOARD_TYPE == BOARD_HT_THREE_0134566) )

#endif//#ifndef __BOARD_HT_698_THREE_H


