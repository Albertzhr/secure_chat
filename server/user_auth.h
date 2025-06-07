// user_auth.h
#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "protocol.h"

int handle_register(const AuthMessage *msg, ResultMessage *result);
int handle_login(const AuthMessage *msg, ResultMessage *result);

#endif

