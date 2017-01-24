/*
 * hmac.h
 *
 *  Created on: 04-Feb-2014
 *      Author: ranjit
 */

#ifndef HMAC_H_
#define HMAC_H_


#include <stdio.h>
#include <stdlib.h>


//check if I need to use typedef
#define uint8_t uint8
#define uint32_t uint32


typedef struct {
	uint8_t hash[32];
	uint32_t buffer[16];
	uint32_t state[8];
	uint8_t length[8];
} sha256;

typedef struct _hmac_sha256 {
	uint8_t digest[32];
	uint8_t key[64];
	sha256 sha;
} hmac_sha256;

void sha256_initialize(sha256 *sha);

void sha256_update(sha256 *sha, const uint8_t *message, uint32_t length);

void sha256_finalize(sha256 *sha, const uint8_t *message, uint32_t length);

void sha256_get(uint8_t hash[32], const uint8_t *message, int length);


void hmac_sha256_initialize(hmac_sha256 *hmac, const uint8_t *key, int length);

void hmac_sha256_update(hmac_sha256 *hmac, const uint8_t *message, int length) ;

void hmac_sha256_finalize(hmac_sha256 *hmac, const uint8_t *message, int length);

void hmac_sha256_get(uint8_t digest[32], const uint8_t *message,
		int message_length, const uint8_t *key, int key_length) ;

#endif /* HMAC_H_ */
