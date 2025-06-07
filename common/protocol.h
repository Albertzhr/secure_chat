#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MSG_REGISTER 1
#define MSG_LOGIN    2
#define MSG_RESULT   3

#define MSG_ADD_FRIEND_REQ   10
#define MSG_ADD_FRIEND_RESP  11
#define MSG_FRIEND_LIST_REQ  12
#define MSG_FRIEND_LIST_RESP 13

#define MSG_PENDING_FRIEND_REQ 14
#define MSG_PENDING_FRIEND_LIST 15
#define SALT_LEN     16
#define MSG_PRIVATE_CHAT      20
#define MSG_CHAT_PUSH         21
#define MSG_FETCH_CHAT        22
#define MSG_FETCH_CHAT_RESP   23

#define USERNAME_LEN 32
#define PASSWORD_LEN 64
#define HASH_HEX_LEN 65
#define PUBKEY_LEN   132
#define FRIEND_LIST_MAX 50

#define CHAT_MAX_LEN 256
#define CHAT_HISTORY_MAX 100

#define MSG_GROUP_CREATE        100
#define MSG_GROUP_CREATE_RESP  101
#define MSG_GROUP_LIST_REQ     102
#define MSG_GROUP_LIST_RESP    103
#define MSG_GROUP_JOIN         104
#define MSG_GROUP_JOIN_RESP    105
#define MSG_GROUP_MEMBER_LIST  106
#define MSG_GROUP_SEND_MSG     110
#define MSG_GROUP_PUSH_MSG     111
#define MSG_FETCH_GROUP_MSG    112
#define MSG_FETCH_GROUP_RESP   113

#define GROUP_NAME_LEN     32
#define GROUP_MAX_MEMBERS  50
#define GROUP_LIST_MAX     20
#define GROUP_CHAT_MAX     100

#define MSG_LOG_AUDIT_REQ 200
#define MSG_LOG_AUDIT_RESP 201

#define LOG_CONTENT_MAX 2048
typedef struct {
    int type;
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char sm2_pubkey[PUBKEY_LEN];
} AuthMessage;

typedef struct {
    int type;
    int success;
    char message[128];
} ResultMessage;

typedef struct {
    int type;
    char from[USERNAME_LEN];
    char to[USERNAME_LEN];
} FriendRequestMessage;

typedef struct {
    int type;
    char from[USERNAME_LEN];
    char to[USERNAME_LEN];
    int accept;
} FriendResponseMessage;

typedef struct {
    int type;
    char username[USERNAME_LEN];
    char friends[FRIEND_LIST_MAX][USERNAME_LEN];
    int count;
} FriendListMessage;

// 好友请求列表
typedef struct {
    int type;
    char username[USERNAME_LEN];
    char applicants[FRIEND_LIST_MAX][USERNAME_LEN];
    int count;
} PendingFriendListMessage;

// 私聊：加密消息传递
typedef struct {
    int type;
    char from[USERNAME_LEN];
    char to[USERNAME_LEN];
    unsigned char sm4_cipher[CHAT_MAX_LEN];
    int cipher_len;
} PrivateChatMessage;

// 消息推送
typedef struct {
    int type;
    char from[USERNAME_LEN];
    char to[USERNAME_LEN];
    unsigned char sm4_cipher[CHAT_MAX_LEN];
    int cipher_len;
} ChatPushMessage;

// 拉取所有未读消息
typedef struct {
    int type;
    char username[USERNAME_LEN];
} FetchChatMessage;

typedef struct {
    int type;
    int count;
    PrivateChatMessage messages[CHAT_HISTORY_MAX];
} FetchChatRespMessage;

typedef struct {
    int type;
    char groupname[GROUP_NAME_LEN];
    char creator[USERNAME_LEN];
} GroupCreateMessage;

typedef struct {
    int type;
    int success;
    char message[128];
} GroupCreateRespMessage;

typedef struct {
    int type;
    char username[USERNAME_LEN];
    char groups[GROUP_LIST_MAX][GROUP_NAME_LEN];
    int count;
} GroupListMessage;

typedef struct {
    int type;
    char groupname[GROUP_NAME_LEN];
    char username[USERNAME_LEN];
} GroupJoinMessage;

typedef struct {
    int type;
    int success;
    char message[128];
} GroupJoinRespMessage;

typedef struct {
    int type;
    char groupname[GROUP_NAME_LEN];
    char members[GROUP_MAX_MEMBERS][USERNAME_LEN];
    int count;
} GroupMemberListMessage;

// 群聊消息
typedef struct {
    int type;
    char groupname[GROUP_NAME_LEN];
    char sender[USERNAME_LEN];
    unsigned char sm4_cipher[CHAT_MAX_LEN];
    int cipher_len;
} GroupChatMessage;

typedef struct {
    int type;
    char groupname[GROUP_NAME_LEN];
    GroupChatMessage messages[GROUP_CHAT_MAX];
    int count;
} FetchGroupChatRespMessage;





typedef struct {
    int type;
    char username[USERNAME_LEN];
} LogAuditReqMessage;

typedef struct {
    int type;
    int success;             // 0=失败 1=成功
    char message[128];       // 错误/提示
    char content[LOG_CONTENT_MAX]; // 日志内容
} LogAuditRespMessage;

#endif
