#include "online_users.h"
#include <string.h>
#include <unistd.h>

OnlineUser online_users[MAX_USERS];

void add_online_user(const char *username, int sock) {
    for (int i = 0; i < MAX_USERS; ++i) {
        if (online_users[i].sock == 0) {
            strncpy(online_users[i].username, username, USERNAME_LEN-1);
            online_users[i].sock = sock;
            return;
        }
    }
}

void remove_online_user_by_sock(int sock) {
    for (int i = 0; i < MAX_USERS; ++i) {
        if (online_users[i].sock == sock) {
            online_users[i].sock = 0;
            online_users[i].username[0] = 0;
        }
    }
}

int get_socket_by_user(const char *username) {
    for (int i = 0; i < MAX_USERS; ++i) {
        if (online_users[i].sock != 0 && strcmp(online_users[i].username, username) == 0)
            return online_users[i].sock;
    }
    return 0;
}
