#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../common/protocol.h"
#include "user_auth.h"
#include "database.h"

sqlite3* db = NULL;

void handle_add_friend(AddFriendMessage* req, ResultMessage* res) {
    if (add_friend(db, req->requester, req->target)) {
        res->msg_type = MSG_RESULT;
        res->success = 1;
        snprintf(res->message, sizeof(res->message), "成功添加好友：%s", req->target);
    } else {
        res->msg_type = MSG_RESULT;
        res->success = 0;
        snprintf(res->message, sizeof(res->message), "添加失败：用户不存在或已是好友");
    }
}

void handle_friend_list(int client_sock, const char* username) {
    FriendListMessage list_msg;
    list_msg.msg_type = MSG_FRIEND_LIST_RET;
    strncpy(list_msg.username, username, sizeof(list_msg.username));
    get_friend_list(db, username, list_msg.list, sizeof(list_msg.list));
    send(client_sock, &list_msg, sizeof(FriendListMessage), 0);
}

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[sizeof(FriendListMessage)];
    char current_user[32] = "";

    while (1) {
        ssize_t bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;

        int msg_type = *(int*)buffer;

        if (msg_type == MSG_REGISTER || msg_type == MSG_LOGIN) {
            AuthMessage *req = (AuthMessage*)buffer;
            ResultMessage res;
            int ok = (msg_type == MSG_REGISTER) ? handle_register(req, &res)
                                                : handle_login(req, &res);
            if (ok && msg_type == MSG_LOGIN) {
                strncpy(current_user, req->username, sizeof(current_user));
            }
            send(client_sock, &res, sizeof(ResultMessage), 0);
        } else if (msg_type == MSG_CHAT) {
            ChatMessage *msg = (ChatMessage*)buffer;
            printf("[群聊] %s: %s\n", msg->sender, msg->message);
            send(client_sock, msg, sizeof(ChatMessage), 0);
        } else if (msg_type == MSG_PRIVATE_CHAT) {
            PrivateMessage *pm = (PrivateMessage*)buffer;
            printf("[私聊] %s -> %s: %s\n", pm->from, pm->to, pm->message);
            send(client_sock, pm, sizeof(PrivateMessage), 0); // TODO: 实现转发
        } else if (msg_type == MSG_ADD_FRIEND) {
            AddFriendMessage* req = (AddFriendMessage*)buffer;
            ResultMessage res;
            handle_add_friend(req, &res);
            send(client_sock, &res, sizeof(ResultMessage), 0);
        } else if (msg_type == MSG_FRIEND_LIST) {
            FriendListMessage* req = (FriendListMessage*)buffer;
            handle_friend_list(client_sock, req->username);
        }
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    if (!init_database(&db)) {
        fprintf(stderr, "数据库初始化失败\n");
        return 1;
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    printf("[Server] Listening on port %d...\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int* client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    close(server_sock);
    sqlite3_close(db);
    return 0;
}

