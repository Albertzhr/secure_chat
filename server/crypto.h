// crypto.h
#ifndef CRYPTO_H
#define CRYPTO_H
#include <stddef.h>
void generate_salt(unsigned char *salt, int len);
void sm3_hash_with_salt(const char *password, const unsigned char *salt, int salt_len, unsigned char *out_digest);
void bin2hex(const unsigned char *in, int len, char *out);

void my_sm4_encrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen);
void my_sm4_decrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen);
#endif

