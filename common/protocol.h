#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_PORT 6666
#define MAX_MSG_LEN 1024

typedef struct {
    int msg_type;            // 0: 普通消息，1: 登录请求，2: 登出
    char sender[32];         // 用户名
    char message[MAX_MSG_LEN]; // 消息内容
} ChatMessage;

#endif

