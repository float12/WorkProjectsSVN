#include "AllHead.h"
#include "crc16.h"

#define ZONEISOK  (0x33)

unsigned char ReadParaFromSpiFlash(unsigned char *SaveBuf, unsigned short length)
{
#if(!WIN32)
    unsigned char  ParaZone0[4096];  //参数区0
    unsigned char  ParaZone1[4096];  //参数区1指针,两个区镜像,出现问题时至少有1个以上区数据完整
    unsigned int   i,k;
    unsigned char  SysCtrlByte;//见下面详细介绍
    unsigned char  SysCtrlVerifyByte;
    unsigned char  VerifyError;
    unsigned int   datalen;
    unsigned char  VerifyByte;
    unsigned short CrcByte;
    datalen = length + sizeof(unsigned short);//数据长度+校验字节长度
    if(CheckSpiFlashInit(C6746_Spi_Flash)!=1) //在这之前并未初始化过SpiFlash
    {
        SpiFlashInit(C6746_Spi_Flash);
        if(CheckSpiFlashInit(C6746_Spi_Flash)!=1)
        {
            return FALSE;
        }
    }
    SpiFlashRead(C6746_Spi_Flash, 0x03FFF, &VerifyByte, 1);//读最后1个字节是否为0X33
    if(VerifyByte!=ZONEISOK)
    {
        SpiFlashRead(C6746_Spi_Flash, 0x04000, ParaZone0, datalen);
        SpiFlashRead(C6746_Spi_Flash, 0x04FFE, &SysCtrlByte, 1);
        k=0;
        do
        {
            VerifyError = 0;
            SpiFlashEraseSector4K(C6746_Spi_Flash, 0 , 3);//Block0 Sector3--0x3000;
            SpiFlashWrite(C6746_Spi_Flash , 0x03000, ParaZone0, datalen);
            SpiFlashWriteOneByte(C6746_Spi_Flash , 0x03FFE, SysCtrlByte);
            SpiFlashRead(C6746_Spi_Flash, 0x03000, ParaZone1, datalen);
            SpiFlashRead(C6746_Spi_Flash, 0x03FFE, &SysCtrlVerifyByte, 1);
            for(i=0;i<datalen;i++)
            {
                if(ParaZone1[i]!=ParaZone0[i])
                    VerifyError = 1;//重写，一般重写的可能性不大，基本一次写成功
            }
            if(SysCtrlByte!=SysCtrlVerifyByte)
                VerifyError = 1;
            if(k++>3)
                return FALSE;
        }while(VerifyError);
        k=0;
        do
        {
            SpiFlashWriteOneByte(C6746_Spi_Flash , 0x03FFF, ZONEISOK);
            SpiFlashRead(C6746_Spi_Flash, 0x03FFF, &VerifyByte, 1);
            if(k++>3)
                return FALSE;
        }while(VerifyByte!=ZONEISOK);
    }
    else
    {
        SpiFlashRead(C6746_Spi_Flash, 0x04FFF, &VerifyByte, 1);//读参数区1最后1个字节是否为0X33
        if(VerifyByte!=ZONEISOK)
        {
            SpiFlashRead(C6746_Spi_Flash, 0x03000, ParaZone0, datalen);
            SpiFlashRead(C6746_Spi_Flash, 0x03FFE, &SysCtrlByte, 1);
            k=0;
            do
            {
                VerifyError = 0;
                SpiFlashEraseSector4K(C6746_Spi_Flash, 0 , 4);//Block1 Sector4--0x4000;
                SpiFlashWrite(C6746_Spi_Flash , 0x04000, ParaZone0, datalen);
                SpiFlashWriteOneByte(C6746_Spi_Flash , 0x04FFE, SysCtrlByte);
                SpiFlashRead(C6746_Spi_Flash, 0x04000, ParaZone1, datalen);
                SpiFlashRead(C6746_Spi_Flash, 0x04FFE, &SysCtrlVerifyByte, 1);
                for(i=0;i<datalen;i++)
                {
                    if(ParaZone1[i]!=ParaZone0[i])
                        VerifyError = 1;//重写，一般重写的可能性不大，基本一次写成功
                }
                if(SysCtrlByte!=SysCtrlVerifyByte)
                    VerifyError = 1;
                if(k++>3)
                    return FALSE;
            }while(VerifyError);
            k=0;
            do
            {
                SpiFlashWriteOneByte(C6746_Spi_Flash , 0x03FFF, ZONEISOK);
                SpiFlashRead(C6746_Spi_Flash, 0x03FFF, &VerifyByte, 1);
                if(k++>3)
                    return FALSE;
            }while(VerifyByte!=ZONEISOK);
        }
    }
    SpiFlashRead(C6746_Spi_Flash, 0x03000, ParaZone0, datalen);//参数区0和参数区1检验通关，开始读参数
    memcpy(&CrcByte, &ParaZone0[length], sizeof(unsigned short));
    if(crc16(ParaZone0, length)!=CrcByte)
    {
        SpiFlashRead(C6746_Spi_Flash, 0x04000, ParaZone0, datalen);//参数区0和参数区1检验通关，开始读参数
        memcpy(&CrcByte, &ParaZone0[length], sizeof(unsigned short));
        if(crc16(ParaZone0, length)!=CrcByte)
        {
            return FALSE;
        }
    }
    memcpy(SaveBuf, ParaZone0, length);
#endif
    return TRUE;
}

unsigned char WriteParaToSpiFlash(unsigned char *SaveBuf, unsigned short length)
{
#if(!WIN32)
    unsigned char  ParaZone0[4096];  //参数区0
    unsigned char  ParaZone1[4096];  //参数区1指针,两个区镜像,出现问题时至少有1个以上区数据完整
    unsigned int   i,k;
    unsigned char  SysCtrlByte;//见下面详细介绍
    unsigned char  SysCtrlVerifyByte;
    unsigned char  VerifyError;
    unsigned int   datalen;
    unsigned char  VerifyByte;
    unsigned short CrcByte;
    datalen = length + sizeof(unsigned short);//数据长度+校验字节长度
    if(CheckSpiFlashInit(C6746_Spi_Flash)!=1) //在这之前并未初始化过SpiFlash
    {
        SpiFlashInit(C6746_Spi_Flash);
        if(CheckSpiFlashInit(C6746_Spi_Flash)!=1)
        {
            return FALSE;
        }
    }
    memcpy(ParaZone0, SaveBuf, length);
    CrcByte = crc16(ParaZone0, length);
    memcpy(&ParaZone0[length], &CrcByte, sizeof(unsigned short));
    SpiFlashRead(C6746_Spi_Flash, 0x03FFE, &SysCtrlByte, 1);//以参数区0的SysCtrlByte为基准
    k=0;
    do
    {
        VerifyError=0;
        SpiFlashEraseSector4K(C6746_Spi_Flash, 0 , 4);//Block1 Sector4--0x4000;参数区1所在地址
        SpiFlashWrite(C6746_Spi_Flash , 0x04000, ParaZone0, datalen);
        SpiFlashWriteOneByte(C6746_Spi_Flash , 0x04FFE, SysCtrlByte);
        SpiFlashRead(C6746_Spi_Flash, 0x04000, ParaZone1, datalen);
        SpiFlashRead(C6746_Spi_Flash, 0x04FFE, &SysCtrlVerifyByte, 1);
        for(i=0;i<datalen;i++)
        {
            if(ParaZone1[i]!=ParaZone0[i])
                VerifyError = 1;//重写，一般重写的可能性不大，基本一次写成功
        }
        if(SysCtrlByte!=SysCtrlVerifyByte)
            VerifyError = 1;
        if(k++>3)
            return FALSE;
    }while(VerifyError);
    k=0;
    do
    {
        SpiFlashWriteOneByte(C6746_Spi_Flash , 0x04FFF, ZONEISOK);
        SpiFlashRead(C6746_Spi_Flash, 0x04FFF, &VerifyByte, 1);
        if(k++>3)
            return FALSE;
    }while(VerifyByte!=ZONEISOK);
    k=0;
    do
    {
        VerifyError=0;
        SpiFlashEraseSector4K(C6746_Spi_Flash, 0 , 3);//Block0 Sector3--0x3000;参数区0所在地址
        SpiFlashWrite(C6746_Spi_Flash , 0x03000, ParaZone0, datalen);
        SpiFlashWriteOneByte(C6746_Spi_Flash , 0x03FFE, SysCtrlByte);
        SpiFlashRead(C6746_Spi_Flash, 0x03000, ParaZone1, datalen);
        SpiFlashRead(C6746_Spi_Flash, 0x03FFE, &SysCtrlVerifyByte, 1);
        for(i=0;i<datalen;i++)
        {
            if(ParaZone1[i]!=ParaZone0[i])
                VerifyError=1;//重写，一般重写的可能性不大，基本一次写成功
        }
        if(SysCtrlByte!=SysCtrlVerifyByte)
            VerifyError = 1;
        if(k++>3)
            return FALSE;
    }while(VerifyError);
    k=0;
    do
    {
        SpiFlashWriteOneByte(C6746_Spi_Flash , 0x03FFF, ZONEISOK);
        SpiFlashRead(C6746_Spi_Flash, 0x03FFF, &VerifyByte, 1);
        if(k++>3)
            return FALSE;
    }while(VerifyByte!=ZONEISOK);

#endif
    return TRUE;
}
