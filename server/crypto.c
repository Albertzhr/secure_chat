#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>

// 生成十六进制格式的盐（16字节 -> 32字符）
void generate_salt(char *salt_hex) {
    unsigned char salt_raw[SALT_LENGTH];
    RAND_bytes(salt_raw, SALT_LENGTH);

    for (int i = 0; i < SALT_LENGTH; ++i) {
        sprintf(salt_hex + i * 2, "%02x", salt_raw[i]);
    }
    salt_hex[SALT_LENGTH * 2] = '\0';
}

// 使用 SHA3-256 计算 hash(salt + password)，输出 hex 字符串
void hash_password(const char *password, const char *salt_hex, char *hash_hex) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt_hex, password);

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha3_256(), NULL);
    EVP_DigestUpdate(ctx, combined, strlen(combined));
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);

    for (unsigned int i = 0; i < digest_len; ++i) {
        sprintf(hash_hex + i * 2, "%02x", digest[i]);
    }
    hash_hex[digest_len * 2] = '\0';
}

