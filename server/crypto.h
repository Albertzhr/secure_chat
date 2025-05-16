#ifndef CRYPTO_H
#define CRYPTO_H

#define SALT_LENGTH 16
#define HASH_HEX_LENGTH 65  // 64字节hash + '\0'

void generate_salt(char *salt_hex);
void hash_password(const char *password, const char *salt_hex, char *hash_hex);

#endif // CRYPTO_H

