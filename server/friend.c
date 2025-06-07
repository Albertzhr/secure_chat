#include "friend.h"
#include "database.h"
#include <string.h>
#include <sqlite3.h>

extern sqlite3 *db;

int add_friend_request(const char *requester, const char *target) {
    if (is_already_friends(requester, target)) return 2;
    const char *sql_check = "SELECT status FROM friend_requests WHERE requester=? AND target=? AND status=0";
    sqlite3_stmt *stmt;
    int exists = 0;
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, target, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) exists = 1;
        sqlite3_finalize(stmt);
    }
    if (exists) return 1;
    const char *sql = "INSERT INTO friend_requests(requester, target, status) VALUES(?,?,0)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, target, -1, SQLITE_STATIC);
        int res = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return res == SQLITE_DONE ? 0 : -1;
    }
    return -1;
}

int confirm_friend(const char *requester, const char *target) {
    const char *sql = "UPDATE friend_requests SET status=1 WHERE requester=? AND target=? AND status=0";
    sqlite3_stmt *stmt;
    int ok = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, target, -1, SQLITE_STATIC);
        ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
    }
    if (!ok) return 0;
    const char *sql1 = "INSERT INTO friends(user1,user2) VALUES(?,?)";
    if (sqlite3_prepare_v2(db, sql1, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, target, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, target, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, requester, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    return 1;
}

int get_friend_list(const char *username, char friends[][USERNAME_LEN], int max_count) {
    const char *sql = "SELECT user2 FROM friends WHERE user1=?";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            strcpy(friends[idx++], (const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}

int is_already_friends(const char *user1, const char *user2) {
    const char *sql = "SELECT 1 FROM friends WHERE user1=? AND user2=?";
    sqlite3_stmt *stmt;
    int found = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user1, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user2, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) found = 1;
        sqlite3_finalize(stmt);
    }
    return found;
}

// 新增：拉取待处理好友请求
int get_pending_friend_requests(const char *username, char applicants[][USERNAME_LEN], int max_count) {
    const char *sql = "SELECT requester FROM friend_requests WHERE target=? AND status=0";
    sqlite3_stmt *stmt;
    int idx = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && idx < max_count) {
            strcpy(applicants[idx++], (const char*)sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return idx;
}

