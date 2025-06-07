#ifndef DATABASE_H
#define DATABASE_H

#include "protocol.h"
#include <sqlite3.h>
#include <stdbool.h>

// 数据库全局变量（friend.c等会用到）
extern sqlite3 *db;

// 初始化、关闭
int init_db(const char *db_path);
void close_db(void);

// 用户相关
bool user_exists(const char *username);
bool add_user(const char *username, const char *password_hash, const char *salt, const char *sm2_pubkey);
bool get_user(const char *username, char *password_hash, char *salt, char *sm2_pubkey);

// 好友相关：仅提供表结构由本文件建表，操作API建议由 friend.c 提供
// == 新增：私聊消息相关 ==
int save_private_message(const char *from, const char *to, const unsigned char *cipher, int cipher_len, int delivered);
int fetch_undelivered_messages(const char *username, PrivateChatMessage *out_msgs, int max_count);
int mark_messages_delivered(const char *username);

int create_group(const char *groupname, const char *creator);
int join_group(const char *groupname, const char *username);
int get_user_groups(const char *username, char groups[][GROUP_NAME_LEN], int max_count);
int get_group_members(const char *groupname, char members[][USERNAME_LEN], int max_count);
int save_group_message(const char *groupname, const char *sender, const unsigned char *cipher, int cipher_len);
int fetch_group_messages(const char *groupname, GroupChatMessage *out_msgs, int max_count);

int get_user_role(const char *username, int *role);

#endif

