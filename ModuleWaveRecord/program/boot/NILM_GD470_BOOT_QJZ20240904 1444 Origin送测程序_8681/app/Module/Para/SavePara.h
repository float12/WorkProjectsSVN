#ifndef SavePara_H_
#define SavePara_H_

extern unsigned char ReadParaFromSpiFlash(unsigned char *SaveBuf, unsigned short length);
extern unsigned char WriteParaToSpiFlash(unsigned char *SaveBuf, unsigned short length);

#endif /* SavePara_H_ */
