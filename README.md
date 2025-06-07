# 安全聊天系统项目验收报告
## 1. 功能与代码仓库链接
### 1.1 GitHub仓库地址
完整项目已上传到github，下载编译即可运行：[https://github.com/Albertzhr/secure_chat](https://github.com/Albertzhr/secure_chat)  
### 1.2 代码结构说明  
```c
secure_chat/
├── client/                     # Qt 客户端部分
│   ├── client.pro              # Qt工程配置文件
│   ├── main.cpp                # 客户端入口
│   ├── mainwindow.cpp          # 主窗口逻辑（群聊、私聊、好友、UI）
│   ├── mainwindow.h
│   ├── mainwindow.ui           # 主窗口UI文件
│   ├── moc_mainwindow.cpp      # Qt自动生成moc文件
│   ├── ui_mainwindow.h         # Qt自动生成UI头文件
│   └── users.db                # 客户端用户本地数据库

├── common/
│   └── protocol.h              # 通信协议结构体和常量

├── server/                     # C语言多线程服务器端
│   ├── crypto.c
│   ├── crypto.h                # 哈希与加密工具（SM2/SM3/SM4等）
│   ├── database.c
│   ├── database.h              # SQLite用户/好友/群聊管理
│   ├── friend.c
│   ├── friend.h                # 好友管理功能
│   ├── main.c                  # 服务器主程序
│   ├── online_users.c
│   ├── online_users.h          # 在线用户管理（用户名-socket映射）
│   ├── user_auth.c
│   ├── user_auth.h             # 用户注册/登录/校验
│   └── users.db                # 服务器端数据库

├── Makefile                    # 总工程Makefile（客户端/服务端编译）
```
### 1.3 功能列表
本系统实现了基于C/S架构和国密算法的安全聊天平台，功能包括：
#### 1.3.1 用户注册/登录
（1）新用户注册（带密码加盐哈希与SM2公钥上传）
（2）用户名/密码身份认证
（3）登录后自动获取好友与群组信息

#### 1.3.2 好友管理
（1）添加好友（发送请求/双向同意）
（2）好友请求通知与同意/拒绝
（3）刷新好友列表，显示所有好友

#### 1.3.3 私聊（点对点聊天）
（1）选择好友后输入消息，消息自动用SM4加密后发送
（2）支持离线消息推送，用户上线后可自动拉取未读消息
#### 1.3.4 群聊功能
（1）创建群聊，自动成为群主并加入
（2）加入已存在群聊
（3）群聊列表刷新，显示所有已加入群组
（4）群聊消息发送/接收（SM4加密）
（5）拉取群聊历史消息
#### 1.3.5 退出登录
用户点击“退出登录”按钮，自动清空会话并重回登录界面

#### 1.3.6 系统安全基础设施
（1）全流程使用国密算法（SM2/SM3/SM4）
（2）权限分离、访问控制、消息加密
（3）完整审计日志与消息持久化

## 2. 密码与安全功能设计
### 2.1 注册与登录（密码哈希 + 盐存储）
#### 2.1.1 服务端注册逻辑（server/user_auth.c）
```c
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
```
#### 2.1.2密码加盐哈希实现（server/crypto.c）
```c
#include <gmssl/sm3.h>
void sm3_hash_with_salt(const char *password, const unsigned char *salt, int salt_len, unsigned char *out_digest) {
    SM3_CTX ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, salt, salt_len);
    sm3_update(&ctx, (unsigned char*)password, strlen(password));
    sm3_finish(&ctx, out_digest);
}
```
#### 2.1.3 用户信息插入数据库（server/database.c）
```c
bool add_user(const char *username, const char *password_hash, const char *salt, const char *sm2_pubkey) {
    const char *sql = "INSERT INTO users(username,password_hash,salt,sm2_pubkey) VALUES(?,?,?,?)";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password_hash, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, salt, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, sm2_pubkey, -1, SQLITE_STATIC);
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return res == SQLITE_DONE;
    }
    return 0;
}
```
#### 2.1.4 服务端登录校验（server/user_auth.c）
```c
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
```
### 2.2 消息加密传输（SM4）
#### 2.2.1 客户端加解密（client/mainwindow.cpp）
```c
// 加密
void MainWindow::my_sm4_encrypt(const QByteArray &plain, unsigned char *cipher, int &cipher_len)
{
    SM4_KEY key;
    sm4_set_encrypt_key(&key, sm4_key);
    int inlen = plain.size();
    int blocks = (inlen + 15) / 16;
    cipher_len = blocks * 16;
    for (int i = 0; i < blocks; ++i) {
        unsigned char inbuf[16] = {0}, outbuf[16] = {0};
        int blen = (i == blocks - 1 && inlen % 16) ? inlen % 16 : 16;
        memcpy(inbuf, plain.data() + i * 16, blen);
        sm4_encrypt(&key, inbuf, outbuf);
        memcpy(cipher + i * 16, outbuf, 16);
    }
}
// 解密
void MainWindow::my_sm4_decrypt(const unsigned char *cipher, int cipher_len, QByteArray &plain)
{
    SM4_KEY key;
    sm4_set_decrypt_key(&key, sm4_key);
    int blocks = cipher_len / 16;
    QByteArray buf(cipher_len, 0);
    for (int i = 0; i < blocks; ++i) {
        sm4_decrypt(&key, cipher + i * 16, (unsigned char*)buf.data() + i * 16);
    }
    plain = buf;
}
```
#### 2.2.2 服务端加解密（server/crypto.c）
```c
void my_sm4_encrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen)
{
    SM4_KEY sm4key;
    sm4_set_encrypt_key(&sm4key, key);

    size_t blocks = (inlen + 15) / 16;
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
void my_sm4_decrypt(const unsigned char *key, const unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen)
{
    SM4_KEY sm4key;
    sm4_set_decrypt_key(&sm4key, key);
    size_t blocks = inlen / 16;
    *outlen = inlen;
    for (size_t i = 0; i < blocks; ++i)
        sm4_decrypt(&sm4key, in + i * 16, out + i * 16);
}
```
### 2.3 身份认证
#### 2.3.1 客户端登录事件(client/mainwindow.cpp)
```c
void MainWindow::on_btnLogin_clicked()
{
    AuthMessage msg = {0};
    msg.type = MSG_LOGIN;
    QByteArray user = ui->editUser->text().toUtf8();
    QByteArray pwd = ui->editPass->text().toUtf8();
    strncpy(msg.username, user.constData(), sizeof(msg.username)-1);
    strncpy(msg.password, pwd.constData(), sizeof(msg.password)-1);
    memset(msg.sm2_pubkey, 0, sizeof(msg.sm2_pubkey));
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
    currentUsername = ui->editUser->text();
}

```
#### 2.3.2 服务端处理登录请求(server/main.c)

```c
else if (type == MSG_LOGIN) {
    AuthMessage *msg = (AuthMessage*)buf;
    ResultMessage res;
    handle_login(msg, &res);
    send(sock, &res, sizeof(res), 0);
    if (res.success) {
        strncpy(username, msg->username, USERNAME_LEN-1);
        add_online_user(username, sock);
        audit_log(username, "login", "登录成功");
    } else {
        audit_log(msg->username, "login", "登录失败");
    }
}
```
### 2.4 访问控制（管理员权限）
#### 2.4.1 数据库升级（添加管理员字段）
```c
ALTER TABLE users ADD COLUMN admin INTEGER DEFAULT 0;
-- 用于区分普通用户和管理员，admin=1为管理员
```
#### 2.4.2 服务端权限判断示例（server/main.c）
```c
else if (type == MSG_LOG_AUDIT_REQ) {
    LogAuditReqMessage *msg = (LogAuditReqMessage*)buf;
    LogAuditRespMessage resp = {0};
    resp.type = MSG_LOG_AUDIT_RESP;

    int role = 0;
    if (get_user_role(msg->username, &role) && role == 1) {
        FILE *fp = fopen("audit.log", "r");
        if (fp) {
            fread(resp.content, 1, sizeof(resp.content)-1, fp);
            fclose(fp);
            resp.success = 1;
            strcpy(resp.message, "获取成功");
        } else {
            resp.success = 0;
            strcpy(resp.message, "无法读取日志");
        }
    } else {
        resp.success = 0;
        strcpy(resp.message, "权限不足，仅管理员可查看日志");
    }
    send(sock, &resp, sizeof(resp), 0);
}
```
#### 2.4.3 管理员判断实现（server/database.c）
```c
int get_user_role(const char *username, int *role_out) {
    const char *sql = "SELECT admin FROM users WHERE username=?";
    sqlite3_stmt *stmt;
    int found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            *role_out = sqlite3_column_int(stmt, 0);
            found = 1;
        }
        sqlite3_finalize(stmt);
    }
    return found;
}
```
### 2.5 审计日志（audit_log.c）
#### 2.5.1 写入日志（server/audit_log.c）
```c
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
void audit_log(const char *username, const char *action, const char *target) {
    FILE *fp = fopen("audit.log", "a");
    if (!fp) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] 用户[%s] 操作[%s] 目标[%s]\n",
            t->tm_year+1900, t->tm_mon+1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            username, action, target ? target : "");
    fclose(fp);
}
```
#### 2.5.2 所有关键分支调用审计
```c
// 注册
audit_log(msg->username, "register", res.success ? "成功" : "失败");
// 登录
audit_log(username, "login", res.success ? "登录成功" : "登录失败");
// 加好友
audit_log(msg->from, "add_friend", msg->to);
// 发消息
audit_log(msg->from, "private_chat_send", msg->to);
// 群聊相关
audit_log(msg->sender, "group_send_msg", msg->groupname);
```
