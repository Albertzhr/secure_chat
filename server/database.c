#include "database.h"
#include <stdio.h>
#include <string.h>

sqlite3 *db = NULL;

int init_db(const char *db_path) {
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        printf("数据库打开失败: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    // === 1. 用户表 ===
    const char *sql_users = "CREATE TABLE IF NOT EXISTS users ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "username TEXT UNIQUE NOT NULL,"
                            "password_hash TEXT NOT NULL,"
                            "salt TEXT NOT NULL,"
                            "sm2_pubkey TEXT"
                            ");";
    if (sqlite3_exec(db, sql_users, 0, 0, 0) != SQLITE_OK)
        return 0;

    // === 2. 好友表 ===
    const char *sql_friends = "CREATE TABLE IF NOT EXISTS friends ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "user1 TEXT NOT NULL,"
                              "user2 TEXT NOT NULL"
                              ");";
    if (sqlite3_exec(db, sql_friends, 0, 0, 0) != SQLITE_OK)
        return 0;

    // === 3. 好友请求表 ===
    const char *sql_requests = "CREATE TABLE IF NOT EXISTS friend_requests ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "requester TEXT NOT NULL,"
                               "target TEXT NOT NULL,"
                               "status INTEGER DEFAULT 0,"   // 0=待确认 1=已同意 2=已拒绝
                               "time DATETIME DEFAULT CURRENT_TIMESTAMP"
                               ");";
    if (sqlite3_exec(db, sql_requests, 0, 0, 0) != SQLITE_OK)
        return 0;
    // 新增私聊表
    const char *sql_chat =
        "CREATE TABLE IF NOT EXISTS private_chat ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sender TEXT,"
        "receiver TEXT,"
        "sm4_cipher BLOB,"
        "cipher_len INT,"
        "delivered INT DEFAULT 0"
        ");";
    if (sqlite3_exec(db, sql_requests, 0, 0, 0) != SQLITE_OK)
        return 0;
    // === 4. 群组表 ===
    const char *sql_group =
        "CREATE TABLE IF NOT EXISTS groups ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "groupname TEXT UNIQUE NOT NULL,"
        "creator TEXT NOT NULL"
        ");";
    if (sqlite3_exec(db, sql_group, 0, 0, 0) != SQLITE_OK) return 0;

    // === 5. 群成员表 ===
    const char *sql_group_member =
        "CREATE TABLE IF NOT EXISTS group_members ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "groupname TEXT NOT NULL,"
        "username TEXT NOT NULL"
        ");";
    if (sqlite3_exec(db, sql_group_member, 0, 0, 0) != SQLITE_OK) return 0;

    // === 6. 群消息表 ===
    const char *sql_group_chat =
        "CREATE TABLE IF NOT EXISTS group_chat ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "groupname TEXT,"
        "sender TEXT,"
        "sm4_cipher BLOB,"
        "cipher_len INT"
        ");";
    if (sqlite3_exec(db, sql_group_chat, 0, 0, 0) != SQLITE_OK) return 0;
    // === 7. 审计日志表 ===
    const char *sql_audit =
        "CREATE TABLE IF NOT EXISTS audit_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT,"
        "action TEXT,"
        "detail TEXT,"
        "time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    if (sqlite3_exec(db, sql_audit, 0, 0, 0) != SQLITE_OK) return 0;

    return 1;
}

void close_db(void) {
    if (db) sqlite3_close(db);
}

// === 用户操作 ===

bool user_exists(const char *username) {
    const char *sql = "SELECT 1 FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    int found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) found = 1;
        sqlite3_finalize(stmt);
    }
    return found;
}

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

bool get_user(const char *username, char *password_hash, char *salt, char *sm2_pubkey) {
    const char *sql = "SELECT password_hash, salt, sm2_pubkey FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    int found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            strcpy(password_hash, (const char*)sqlite3_column_text(stmt, 0));
            strcpy(salt, (const char*)sqlite3_column_text(stmt, 1));
            if (sm2_pubkey) strcpy(sm2_pubkey, (const char*)sqlite3_column_text(stmt, 2));
            found = 1;
        }
        sqlite3_finalize(stmt);
    }
    return found;
}

// == 私聊消息操作 ==
int save_private_message(const char *from, const char *to, const unsigned char *cipher, int cipher_len, int delivered) {
    const char *sql =
        "INSERT INTO private_chat(sender,receiver,sm4_cipher,cipher_len,delivered) VALUES(?,?,?,?,?)";
    sqlite3_stmt *stmt;
    int ok = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, from, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, to, -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 3, cipher, cipher_len, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, cipher_len);
        sqlite3_bind_int(stmt, 5, delivered);
        ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
    }
    return ok;
}

int fetch_undelivered_messages(const char *username, PrivateChatMessage *out_msgs, int max_count) {
    const char *sql = "SELECT sender, receiver, sm4_cipher, cipher_len FROM private_chat WHERE receiver=? AND delivered=0";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            PrivateChatMessage *msg = &out_msgs[idx++];
            msg->type = MSG_PRIVATE_CHAT;
            strcpy(msg->from, (const char*)sqlite3_column_text(stmt, 0));
            strcpy(msg->to, (const char*)sqlite3_column_text(stmt, 1));
            msg->cipher_len = sqlite3_column_int(stmt, 3);
            int len = msg->cipher_len;
            const void *blob = sqlite3_column_blob(stmt, 2);
            memcpy(msg->sm4_cipher, blob, len > CHAT_MAX_LEN ? CHAT_MAX_LEN : len);
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}

int mark_messages_delivered(const char *username) {
    const char *sql = "UPDATE private_chat SET delivered=1 WHERE receiver=?";
    sqlite3_stmt *stmt;
    int ok = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
    }
    return ok;
}

int create_group(const char *groupname, const char *creator) {
    const char *sql = "INSERT INTO groups(groupname,creator) VALUES(?,?)";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, creator, -1, SQLITE_STATIC);
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (res == SQLITE_DONE) {
            // 创建者自动加入群
            return join_group(groupname, creator);
        }
    }
    return 0;
}

int join_group(const char *groupname, const char *username) {
    // 检查是否已加入
    const char *sql_check = "SELECT 1 FROM group_members WHERE groupname=? AND username=?";
    sqlite3_stmt *stmt;
    int exists = 0;
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) exists = 1;
        sqlite3_finalize(stmt);
    }
    if (exists) return 1;
    // 插入
    const char *sql = "INSERT INTO group_members(groupname,username) VALUES(?,?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return res == SQLITE_DONE ? 0 : -1;
    }
    return -1;
}

int get_user_groups(const char *username, char groups[][GROUP_NAME_LEN], int max_count) {
    const char *sql = "SELECT groupname FROM group_members WHERE username=?";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            strcpy(groups[idx++], (const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}

int get_group_members(const char *groupname, char members[][USERNAME_LEN], int max_count) {
    const char *sql = "SELECT username FROM group_members WHERE groupname=?";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            strcpy(members[idx++], (const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}

int save_group_message(const char *groupname, const char *sender, const unsigned char *cipher, int cipher_len) {
    const char *sql =
        "INSERT INTO group_chat(groupname,sender,sm4_cipher,cipher_len) VALUES(?,?,?,?)";
    sqlite3_stmt *stmt;
    int ok = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, sender, -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 3, cipher, cipher_len, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, cipher_len);
        ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
    }
    return ok;
}

int fetch_group_messages(const char *groupname, GroupChatMessage *out_msgs, int max_count) {
    const char *sql = "SELECT sender, sm4_cipher, cipher_len FROM group_chat WHERE groupname=?";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            GroupChatMessage *msg = &out_msgs[idx++];
            msg->type = MSG_GROUP_PUSH_MSG;
            strcpy(msg->groupname, groupname);
            strcpy(msg->sender, (const char*)sqlite3_column_text(stmt, 0));
            msg->cipher_len = sqlite3_column_int(stmt, 2);
            memcpy(msg->sm4_cipher, sqlite3_column_blob(stmt, 1), msg->cipher_len);
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}


int get_user_role(const char *username, int *role) {
    const char *sql = "SELECT role FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    int found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            *role = sqlite3_column_int(stmt, 0);
            found = 1;
        }
        sqlite3_finalize(stmt);
    }
    return found;
}
