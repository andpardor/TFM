/*
 * File  : aes.h
 * Autor : Adaptado por Andres Pardo a partir de KOKKE/tiny-AES-C
 *          libreria publica de GITHUB. Varios 
 * 
 * Solo contempla cifrado en modo bloque.
 * 
 * Revision history: 
 *      Primera version : 05/08/2021.
 */


#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include <stddef.h>

// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// ECB enables the basic ECB 16-byte block algorithm. 

#ifndef ECB
  #define ECB 1
#endif

#define AES128 1
#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only
#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 176


struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);

#if defined(ECB) && (ECB == 1)
// buffer size is exactly AES_BLOCKLEN bytes; 
// you need only AES_init_ctx 
// NB: ECB is considered insecure for most uses
void AES_ECB_encrypt(const struct AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(const struct AES_ctx* ctx, uint8_t* buf);

#endif // #if defined(ECB) && (ECB == !)

#endif // _AES_H_
