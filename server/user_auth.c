// user_auth.c
#include "protocol.h"
#include "user_auth.h"
#include "database.h"
#include "crypto.h"
#include <string.h>
#include <stdio.h>

int handle_register(const AuthMessage *msg, ResultMessage *result) {
    if (user_exists(msg->username)) {
        result->type = MSG_RESULT;
        result->success = 0;
        strcpy(result->message, "用户名已存在");
        return 0;
    }
    unsigned char salt[SALT_LEN];
    unsigned char hash[32];
    char hash_hex[HASH_HEX_LEN], salt_hex[SALT_LEN*2+1];

    generate_salt(salt, SALT_LEN);
    sm3_hash_with_salt(msg->password, salt, SALT_LEN, hash);
    bin2hex(hash, 32, hash_hex);
    bin2hex(salt, SALT_LEN, salt_hex);

    if (!add_user(msg->username, hash_hex, salt_hex, msg->sm2_pubkey)) {
        result->type = MSG_RESULT;
        result->success = 0;
        strcpy(result->message, "数据库写入失败");
        return 0;
    }
    result->type = MSG_RESULT;
    result->success = 1;
    strcpy(result->message, "注册成功");
    return 1;
}

int handle_login(const AuthMessage *msg, ResultMessage *result) {
    char db_hash[HASH_HEX_LEN], db_salt[SALT_LEN*2+1], db_pubkey[PUBKEY_LEN] = {0};
    if (!get_user(msg->username, db_hash, db_salt, db_pubkey)) {
        result->type = MSG_RESULT;
        result->success = 0;
        strcpy(result->message, "用户名不存在");
        return 0;
    }
    unsigned char salt[SALT_LEN], hash[32];
    // hex to bin
    for (int i = 0; i < SALT_LEN; ++i) {
        sscanf(&db_salt[i*2], "%2hhx", &salt[i]);
    }
    sm3_hash_with_salt(msg->password, salt, SALT_LEN, hash);
    char hash_hex[HASH_HEX_LEN];
    bin2hex(hash, 32, hash_hex);

    if (strcmp(hash_hex, db_hash) == 0) {
        result->type = MSG_RESULT;
        result->success = 1;
        strcpy(result->message, "登录成功");
        return 1;
    } else {
        result->type = MSG_RESULT;
        result->success = 0;
        strcpy(result->message, "密码错误");
        return 0;
    }
}

