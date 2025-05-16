#include "database.h"
#include <stdio.h>
#include <string.h>

static sqlite3* global_db = NULL;

int init_database(sqlite3 **db) {
    if (sqlite3_open("users.db", db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return 0;
    }

    const char *create_users = "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, salt TEXT, hashed TEXT);";
    const char *create_friends = "CREATE TABLE IF NOT EXISTS friends (user TEXT, friend TEXT, PRIMARY KEY(user, friend));";

    char *errmsg = NULL;
    if (sqlite3_exec(*db, create_users, NULL, NULL, &errmsg) != SQLITE_OK ||
        sqlite3_exec(*db, create_friends, NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "DB init error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return 0;
    }

    global_db = *db;
    return 1;
}

int insert_user(const char *username, const char *salt, const char *hashed) {
    const char *sql = "INSERT INTO users (username, salt, hashed) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(global_db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashed, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

int get_user_salt_and_hash(const char *username, char *salt_out, char *hash_out) {
    const char *sql = "SELECT salt, hashed FROM users WHERE username=?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(global_db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(salt_out, (const char*)sqlite3_column_text(stmt, 0));
        strcpy(hash_out, (const char*)sqlite3_column_text(stmt, 1));
        found = 1;
    }

    sqlite3_finalize(stmt);
    return found;
}

int add_friend(sqlite3 *db, const char *user, const char *friend_name) {
    const char *sql = "INSERT OR IGNORE INTO friends (user, friend) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, friend_name, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

int is_friend(sqlite3 *db, const char *user, const char *friend_name) {
    const char *sql = "SELECT 1 FROM friends WHERE user=? AND friend=?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, friend_name, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_ROW;
}

int get_friend_list(sqlite3 *db, const char *user, char *result_text, int max_len) {
    const char *sql = "SELECT friend FROM friends WHERE user=?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

    result_text[0] = '\0';

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *f = (const char *)sqlite3_column_text(stmt, 0);
        if ((strlen(result_text) + strlen(f) + 2) < max_len) {
            strcat(result_text, f);
            strcat(result_text, "\n");
        } else break;
    }

    sqlite3_finalize(stmt);
    return 1;
}

