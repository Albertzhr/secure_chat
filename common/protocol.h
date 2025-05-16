#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_PORT 6666
#define MAX_MSG_LEN 1024

// 消息类型常量
#define MSG_CHAT             0   // 群聊消息
#define MSG_LOGIN            1   // 登录请求
#define MSG_LOGOUT           2   // 登出请求
#define MSG_REGISTER         3   // 注册请求
#define MSG_RESULT           4   // 登录/注册反馈
#define MSG_PRIVATE_CHAT     5   // 私聊消息
#define MSG_ADD_FRIEND       6   // 添加好友
#define MSG_FRIEND_LIST      7   // 请求好友列表
#define MSG_FRIEND_LIST_RET  8   // 返回好友列表

// ================= 原有结构 ===================

// 群聊消息结构体
typedef struct {
    int msg_type;              // MSG_CHAT
    char sender[32];           // 用户名
    char message[MAX_MSG_LEN]; // 内容
} ChatMessage;

// 注册 / 登录 请求结构体
typedef struct {
    int msg_type;              // MSG_LOGIN or MSG_REGISTER
    char username[32];
    char password[64];         // 明文密码（服务端会做加盐 SM3）
} AuthMessage;

// 登录/注册反馈
typedef struct {
    int msg_type;              // MSG_RESULT
    int success;               // 1 成功 / 0 失败
    char message[128];         // 反馈信息
} ResultMessage;

// ================ 新增结构 ===================

// 私聊消息结构体
typedef struct {
    int msg_type;              // MSG_PRIVATE_CHAT
    char from[32];             // 发送方
    char to[32];               // 接收方
    char message[MAX_MSG_LEN]; // 消息内容
} PrivateMessage;

// 添加好友请求
typedef struct {
    int msg_type;              // MSG_ADD_FRIEND
    char requester[32];        // 谁添加
    char target[32];           // 添加谁
} AddFriendMessage;

// 获取好友列表请求（发出用 MSG_FRIEND_LIST，返回用 MSG_FRIEND_LIST_RET）
typedef struct {
    int msg_type;              // MSG_FRIEND_LIST or MSG_FRIEND_LIST_RET
    char username[32];         // 谁在查
    char list[MAX_MSG_LEN];    // 好友列表（MSG_FRIEND_LIST_RET中使用，\n 分隔）
} FriendListMessage;

#endif // PROTOCOL_H

