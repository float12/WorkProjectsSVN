
/******************************************************************************/
/** \file light_sm.h
 ** \date 2020-04-20
 ** \ver. 1.4.2
 ******************************************************************************/

#ifndef __LIGHT_SM_H__
#define __LIGHT_SM_H__

#if(!WIN32)
#include <stdint.h>
#endif

//typedef unsigned char uint8_t;
//typedef unsigned int  uint32_t;

typedef uint8_t boolean_t;

// #define TRUE    0x01
// #define FALSE   0x00

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define MIN_STORAGE 1024//256

#define ID_LEN      8

#define TEST_STAT	0x00
#define NORM_STAT	0x01

#define ERR_AUTH    0x0a
#define ERR_PAD     0x05
#define ERR_WRITE   0xFF

#define VERSION			0x47

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

#ifdef Dll_EXPORTS  
#define DllAPI __declspec(dllexport)
#else
#define DllAPI
#endif

#ifdef _WIN32

#ifdef CALL_BACK_EXPORT
# define CALL_BACK_FUNC __declspec(dllexport)
#else
# define CALL_BACK_FUNC __declspec(dllimport)
#endif

#else 
# define CALL_BACK_FUNC extern
#endif

typedef boolean_t (*flash_set_zeros)(uint32_t addr, uint32_t len);
typedef boolean_t (*flash_set_words)(uint32_t addr, uint32_t len, uint32_t *arr);
typedef void (*flash_get_words)(uint32_t addr, uint32_t len, uint32_t *arr);

typedef boolean_t (*flash_set_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);
typedef void (*flash_get_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);

typedef boolean_t (*flash_clean_t)(uint32_t addr, uint32_t len);

typedef boolean_t (*random_gen_t)(uint32_t *ranL, uint32_t len);

CALL_BACK_FUNC flash_set_zeros callback_flash_setWords_zero;
CALL_BACK_FUNC flash_set_words callback_flash_setWords;
CALL_BACK_FUNC flash_get_words callback_flash_getWords;
CALL_BACK_FUNC flash_clean_t callback_flash_cleanWords;

CALL_BACK_FUNC flash_set_bytes callback_flash_setBytes;
CALL_BACK_FUNC flash_get_bytes callback_flash_getBytes;
CALL_BACK_FUNC flash_clean_t callback_flash_cleanBytes;

CALL_BACK_FUNC random_gen_t callback_getRandomWords;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

DllAPI boolean_t SetStorageSector(uint32_t bgn, uint32_t bytelen);
DllAPI boolean_t SetID(uint8_t *devid, uint32_t idlen);
DllAPI uint8_t getStatus(uint8_t* devid, uint32_t idlen);
DllAPI uint8_t SM4SetIK(uint8_t* keySets);

DllAPI uint8_t SM4UpdateCheck(uint8_t* cRanA, uint8_t* cRanCat);
DllAPI boolean_t SM4UpdateGen(void);
DllAPI uint8_t SM4UpdateFinish(uint8_t *ciphB);

DllAPI uint8_t SM4InitSIK(uint8_t* keyData);

//auth:  false: enc/dec   true: enc+mac / dec+verifymac
DllAPI uint8_t SM4Encrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen); // EK1 enc & SIK auth
DllAPI uint8_t SM4Decrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen); // EK2 dec & SIK auth

//plaintext + mac
DllAPI uint8_t SM4Mac(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t *outLen); // EK1 plaintext + mac
DllAPI uint8_t SM4MacVerify(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t outLen); // EK2 plaintext + mac verify



DllAPI boolean_t statReset(void);
DllAPI boolean_t ivecReset(boolean_t rmEK);

// New interface:
DllAPI uint8_t initConnection(uint8_t *info, uint32_t infolen, uint8_t *outData, uint32_t *outLen);

DllAPI uint8_t version(void);

#endif /* __LIGHT_SM_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/



