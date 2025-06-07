// crypto.c
#include "crypto.h"
#include <gmssl/sm3.h>
#include <gmssl/sm4.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void generate_salt(unsigned char *salt, int len) {
    FILE *fp = fopen("/dev/urandom", "rb");
    fread(salt, 1, len, fp);
    fclose(fp);
}

void sm3_hash_with_salt(const char *password, const unsigned char *salt, int salt_len, unsigned char *out_digest) {
    SM3_CTX ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, salt, salt_len);
    sm3_update(&ctx, (unsigned char*)password, strlen(password));
    sm3_finish(&ctx, out_digest);
}

void bin2hex(const unsigned char *in, int len, char *out) {
    static const char hex[] = "0123456789abcdef";
    for (int i = 0; i < len; ++i) {
        out[2*i] = hex[(in[i] >> 4) & 0xf];
        out[2*i+1] = hex[in[i] & 0xf];
    }
    out[2*len] = 0;
}

// ECB模式，不带填充。若需填充可加零填充。
// key: 16字节
void my_sm4_encrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen)
{
    SM4_KEY sm4key;
    sm4_set_encrypt_key(&sm4key, key);

    size_t blocks = (inlen + 15) / 16; // 向上取整
    *outlen = blocks * 16;
    unsigned char inbuf[16], outbuf[16];
    for (size_t i = 0; i < blocks; ++i) {
        size_t blen = (i == blocks - 1) ? (inlen % 16 ? inlen % 16 : 16) : 16;
        memset(inbuf, 0, 16);
        memcpy(inbuf, in + i * 16, blen);
        sm4_encrypt(&sm4key, inbuf, outbuf);
        memcpy(out + i * 16, outbuf, 16);
    }
}

// ECB模式，不带填充。输出总是16字节倍数
void my_sm4_decrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen)
{
    SM4_KEY sm4key;
    sm4_set_decrypt_key(&sm4key, key);
    size_t blocks = inlen / 16;
    *outlen = inlen;
    for (size_t i = 0; i < blocks; ++i)
        sm4_decrypt(&sm4key, in + i * 16, out + i * 16);
}