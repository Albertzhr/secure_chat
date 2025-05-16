#include "user_auth.h"
#include "database.h"
#include "crypto.h"
#include <string.h>
#include <stdio.h>

int handle_register(const AuthMessage *req, ResultMessage *res) {
    char salt[33];
    char hash[HASH_HEX_LENGTH];

    char dummy_salt[33], dummy_hash[HASH_HEX_LENGTH];
    if (get_user_salt_and_hash(req->username, dummy_salt, dummy_hash)) {
        res->msg_type = MSG_RESULT;
        res->success = 0;
        snprintf(res->message, sizeof(res->message), "注册失败：用户已存在");
        return 0;
    }

    generate_salt(salt);
    hash_password(req->password, salt, hash);

    if (!insert_user(req->username, salt, hash)) {
        res->msg_type = MSG_RESULT;
        res->success = 0;
        snprintf(res->message, sizeof(res->message), "注册失败：写入数据库失败");
        return 0;
    }

    res->msg_type = MSG_RESULT;
    res->success = 1;
    snprintf(res->message, sizeof(res->message), "注册成功");
    return 1;
}

int handle_login(const AuthMessage *req, ResultMessage *res) {
    char salt[33];
    char stored_hash[HASH_HEX_LENGTH];
    char input_hash[HASH_HEX_LENGTH];

    if (!get_user_salt_and_hash(req->username, salt, stored_hash)) {
        res->msg_type = MSG_RESULT;
        res->success = 0;
        snprintf(res->message, sizeof(res->message), "登录失败：用户不存在");
        return 0;
    }

    hash_password(req->password, salt, input_hash);
    if (strcmp(input_hash, stored_hash) != 0) {
        res->msg_type = MSG_RESULT;
        res->success = 0;
        snprintf(res->message, sizeof(res->message), "登录失败：密码错误");
        return 0;
    }

    res->msg_type = MSG_RESULT;
    res->success = 1;
    snprintf(res->message, sizeof(res->message), "登录成功，欢迎 %s", req->username);
    return 1;
}

