#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "../common/protocol.h"

int handle_register(const AuthMessage *req, ResultMessage *res);
int handle_login(const AuthMessage *req, ResultMessage *res);

#endif // USER_AUTH_H

