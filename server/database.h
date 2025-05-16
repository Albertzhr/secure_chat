#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int init_database(sqlite3 **db);
int insert_user(const char *username, const char *salt, const char *hashed);
int get_user_salt_and_hash(const char *username, char *salt_out, char *hash_out);

int add_friend(sqlite3 *db, const char *user, const char *friend_name);
int is_friend(sqlite3 *db, const char *user, const char *friend_name);
int get_friend_list(sqlite3 *db, const char *user, char *result_text, int max_len);

#endif // DATABASE_H

