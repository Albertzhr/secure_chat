#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include "protocol.h"
#include "user_auth.h"
#include "database.h"
#include "friend.h"
#include "online_users.h"
#include "crypto.h"
#include "audit_log.h"
#define PORT 6666
unsigned char sm4_key[16] = "thisIsTestKey123";



void *client_handler(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buf[1024];
    char username[USERNAME_LEN] = {0};

    while (1) {
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) break;
        int type = *(int*)buf;

        if (type == MSG_REGISTER) {
            AuthMessage *msg = (AuthMessage*)buf;
            ResultMessage res;
            handle_register(msg, &res);
            send(sock, &res, sizeof(res), 0);
            audit_log(msg->username, "register", res.success ? "成功" : "失败");

        } else if (type == MSG_LOGIN) {
            AuthMessage *msg = (AuthMessage*)buf;
            ResultMessage res;
            handle_login(msg, &res);
            send(sock, &res, sizeof(res), 0);
            if (res.success) {
                strncpy(username, msg->username, USERNAME_LEN-1);
                add_online_user(username, sock);
                audit_log(username, "login", "登录成功");
            } else {
                audit_log(msg->username, "login", "登录失败");
            }

        } else if (type == MSG_ADD_FRIEND_REQ) {
            FriendRequestMessage *msg = (FriendRequestMessage*)buf;
            ResultMessage res = {0};
            int ret = add_friend_request(msg->from, msg->to);
            res.type = MSG_RESULT;
            if (ret == 0) {
                res.success = 1; strcpy(res.message, "好友申请已发送");
            } else if (ret == 1) {
                res.success = 0; strcpy(res.message, "你已发送过好友请求");
            } else if (ret == 2) {
                res.success = 0; strcpy(res.message, "你们已是好友");
            } else {
                res.success = 0; strcpy(res.message, "好友申请失败");
            }
            send(sock, &res, sizeof(res), 0);
            audit_log(msg->from, "add_friend", msg->to);

        } else if (type == MSG_ADD_FRIEND_RESP) {
            FriendResponseMessage *msg = (FriendResponseMessage*)buf;
            ResultMessage res = {0};
            if (msg->accept) {
                if (confirm_friend(msg->to, msg->from)) {
                    res.type = MSG_RESULT; res.success = 1; strcpy(res.message, "已同意好友请求");
                    audit_log(msg->to, "accept_friend", msg->from);
                } else {
                    res.type = MSG_RESULT; res.success = 0; strcpy(res.message, "同意好友失败");
                    audit_log(msg->to, "accept_friend_fail", msg->from);
                }
            } else {
                res.type = MSG_RESULT; res.success = 1; strcpy(res.message, "已拒绝好友请求");
                audit_log(msg->to, "reject_friend", msg->from);
            }
            send(sock, &res, sizeof(res), 0);

        } else if (type == MSG_FRIEND_LIST_REQ) {
            FriendListMessage *msg = (FriendListMessage*)buf;
            FriendListMessage resp = {0};
            resp.type = MSG_FRIEND_LIST_RESP;
            strcpy(resp.username, msg->username);
            resp.count = get_friend_list(msg->username, resp.friends, FRIEND_LIST_MAX);
            send(sock, &resp, sizeof(resp), 0);
            audit_log(msg->username, "get_friend_list", "");

        } else if (type == MSG_PENDING_FRIEND_REQ) {
            AuthMessage *msg = (AuthMessage*)buf;
            PendingFriendListMessage resp = {0};
            resp.type = MSG_PENDING_FRIEND_LIST;
            strcpy(resp.username, msg->username);
            resp.count = get_pending_friend_requests(msg->username, resp.applicants, FRIEND_LIST_MAX);
            send(sock, &resp, sizeof(resp), 0);
            audit_log(msg->username, "get_pending_requests", "");

        } else if (type == MSG_PRIVATE_CHAT) {
            PrivateChatMessage *msg = (PrivateChatMessage*)buf;
            save_private_message(msg->from, msg->to, msg->sm4_cipher, msg->cipher_len, 0);
            audit_log(msg->from, "private_chat_send", msg->to);
            int target_sock = get_socket_by_user(msg->to);
            if (target_sock > 0) {
                ChatPushMessage push = {MSG_CHAT_PUSH};
                strcpy(push.from, msg->from);
                strcpy(push.to, msg->to);
                memcpy(push.sm4_cipher, msg->sm4_cipher, msg->cipher_len);
                push.cipher_len = msg->cipher_len;
                send(target_sock, &push, sizeof(push), 0);
                mark_messages_delivered(msg->to);
                audit_log(msg->to, "private_chat_recv", msg->from);
            }

        } else if (type == MSG_FETCH_CHAT) {
            FetchChatMessage *req = (FetchChatMessage*)buf;
            FetchChatRespMessage resp = {MSG_FETCH_CHAT_RESP};
            resp.count = fetch_undelivered_messages(req->username, resp.messages, CHAT_HISTORY_MAX);
            send(sock, &resp, sizeof(resp), 0);
            mark_messages_delivered(req->username);
            audit_log(req->username, "fetch_private_msg", "");

        } else if (type == MSG_GROUP_CREATE) {
            GroupCreateMessage *msg = (GroupCreateMessage*)buf;
            GroupCreateRespMessage resp = {MSG_GROUP_CREATE_RESP, 0, ""};
            if (create_group(msg->groupname, msg->creator) == 0) {
                resp.success = 1;
                strcpy(resp.message, "群组创建成功");
                audit_log(msg->creator, "group_create", msg->groupname);
            } else {
                resp.success = 0;
                strcpy(resp.message, "群组创建失败或已存在");
                audit_log(msg->creator, "group_create_fail", msg->groupname);
            }
            send(sock, &resp, sizeof(resp), 0);

        } else if (type == MSG_GROUP_LIST_REQ) {
            AuthMessage *msg = (AuthMessage*)buf;
            GroupListMessage resp = {MSG_GROUP_LIST_RESP};
            resp.count = get_user_groups(msg->username, resp.groups, GROUP_LIST_MAX);
            send(sock, &resp, sizeof(resp), 0);
            audit_log(msg->username, "get_group_list", "");

        } else if (type == MSG_GROUP_JOIN) {
            GroupJoinMessage *msg = (GroupJoinMessage*)buf;
            GroupJoinRespMessage resp = {MSG_GROUP_JOIN_RESP, 0, ""};
            int r = join_group(msg->groupname, msg->username);
            if (r == 0) {
                resp.success = 1;
                strcpy(resp.message, "加入群聊成功");
                audit_log(msg->username, "group_join", msg->groupname);
            } else if (r == 1) {
                resp.success = 0;
                strcpy(resp.message, "你已是群成员");
                audit_log(msg->username, "group_join_repeat", msg->groupname);
            } else {
                resp.success = 0;
                strcpy(resp.message, "加入失败");
                audit_log(msg->username, "group_join_fail", msg->groupname);
            }
            send(sock, &resp, sizeof(resp), 0);

        } else if (type == MSG_GROUP_MEMBER_LIST) {
            GroupJoinMessage *msg = (GroupJoinMessage*)buf;
            GroupMemberListMessage resp = {MSG_GROUP_MEMBER_LIST};
            strcpy(resp.groupname, msg->groupname);
            resp.count = get_group_members(msg->groupname, resp.members, GROUP_MAX_MEMBERS);
            send(sock, &resp, sizeof(resp), 0);
            audit_log(msg->username, "get_group_members", msg->groupname);

        } else if (type == MSG_GROUP_SEND_MSG) {
            GroupChatMessage *msg = (GroupChatMessage*)buf;
            save_group_message(msg->groupname, msg->sender, msg->sm4_cipher, msg->cipher_len);
            audit_log(msg->sender, "group_send_msg", msg->groupname);
            char members[GROUP_MAX_MEMBERS][USERNAME_LEN];
            int count = get_group_members(msg->groupname, members, GROUP_MAX_MEMBERS);
            for (int i = 0; i < count; ++i) {
                if (strcmp(members[i], msg->sender) == 0) continue;
                int target_sock = get_socket_by_user(members[i]);
                if (target_sock > 0) {
                    send(target_sock, msg, sizeof(GroupChatMessage), 0);
                    audit_log(members[i], "group_msg_receive", msg->groupname);
                }
            }

        } else if (type == MSG_FETCH_GROUP_MSG) {
            GroupJoinMessage *msg = (GroupJoinMessage*)buf;
            FetchGroupChatRespMessage resp = {MSG_FETCH_GROUP_RESP};
            resp.count = fetch_group_messages(msg->groupname, resp.messages, GROUP_CHAT_MAX);
            send(sock, &resp, sizeof(resp), 0);
            audit_log(msg->username, "fetch_group_msg", msg->groupname);

        } else if (type == MSG_LOG_AUDIT_REQ) {
            LogAuditReqMessage *msg = (LogAuditReqMessage*)buf;
            LogAuditRespMessage resp = {0};
            resp.type = MSG_LOG_AUDIT_RESP;

            int role = 0;
            if (get_user_role(msg->username, &role) && role == 1) {
                FILE *fp = fopen("audit.log", "r");
                if (fp) {
                    fread(resp.content, 1, sizeof(resp.content)-1, fp);
                    fclose(fp);
                    resp.success = 1;
                    strcpy(resp.message, "获取成功");
                    audit_log(msg->username, "view_audit_log", "success");
                } else {
                    resp.success = 0;
                    strcpy(resp.message, "无法读取日志");
                    audit_log(msg->username, "view_audit_log", "fail");
                }
            } else {
                resp.success = 0;
                strcpy(resp.message, "权限不足，仅管理员可查看日志");
                audit_log(msg->username, "view_audit_log", "deny");
            }
            send(sock, &resp, sizeof(resp), 0);
        }
    }
    if (username[0]) remove_online_user_by_sock(sock);
    close(sock);
    return NULL;
}


int main() {
    if (!init_db("users.db")) {
        printf("数据库初始化失败\n");
        return 1;
    }
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listenfd, 10);
    printf("服务端监听 %d ...\n", PORT);

    while (1) {
        int *clientfd = malloc(sizeof(int));
        *clientfd = accept(listenfd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, clientfd);
        pthread_detach(tid);
    }
    close_db();
    return 0;
}
