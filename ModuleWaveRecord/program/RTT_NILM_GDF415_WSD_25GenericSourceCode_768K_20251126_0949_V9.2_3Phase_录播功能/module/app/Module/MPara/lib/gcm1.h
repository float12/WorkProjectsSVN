#ifndef MBEDTLS_GCM_H
#define MBEDTLS_GCM_H

#include "cipher.h"

#include <stdint.h>

#define MBEDTLS_GCM_ENCRYPT     1
#define MBEDTLS_GCM_DECRYPT     0

#define MBEDTLS_ERR_GCM_AUTH_FAILED                       -0x0012  

#define MBEDTLS_ERR_GCM_HW_ACCEL_FAILED                   -0x0013  

#define MBEDTLS_ERR_GCM_BAD_INPUT                         -0x0014  

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MBEDTLS_GCM_ALT)

typedef struct mbedtls_gcm_context
{
    mbedtls_cipher_context_t cipher_ctx;  
    uint64_t HL[16];                      
    uint64_t HH[16];                      
    uint64_t len;                         
    uint64_t add_len;                     
    unsigned char base_ectr[16];          
    unsigned char y[16];                  
    unsigned char buf[16];                
    int mode;                             
                                          
                                          
}
mbedtls_gcm_context;

#else
#include "gcm_alt.h"
#endif

#ifdef __cplusplus
}
#endif


#endif
