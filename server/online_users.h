#ifndef ONLINE_USERS_H
#define ONLINE_USERS_H

#include "protocol.h"

typedef struct {
    char username[USERNAME_LEN];
    int sock;
} OnlineUser;

#define MAX_USERS 100
extern OnlineUser online_users[MAX_USERS];

void add_online_user(const char *username, int sock);
void remove_online_user_by_sock(int sock);
int get_socket_by_user(const char *username);

#endif
