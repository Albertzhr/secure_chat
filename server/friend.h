#ifndef FRIEND_H
#define FRIEND_H

#include "protocol.h"
int add_friend_request(const char *requester, const char *target);
int confirm_friend(const char *requester, const char *target);
int get_friend_list(const char *username, char friends[][USERNAME_LEN], int max_count);
int is_already_friends(const char *user1, const char *user2);
int get_pending_friend_requests(const char *username, char applicants[][USERNAME_LEN], int max_count);

#endif

