#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../common/protocol.h"

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    ChatMessage msg;
    while (recv(client_sock, &msg, sizeof(ChatMessage), 0) > 0) {
        printf("[Server] Message from %s: %s\n", msg.sender, msg.message);
        send(client_sock, &msg, sizeof(ChatMessage), 0);  // 回显
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
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
    return 0;
}

