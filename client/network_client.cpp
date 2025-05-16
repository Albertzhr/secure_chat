#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include "network_client.h"

NetworkClient::NetworkClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
}

void NetworkClient::connectToServer(QString host, int port) {
    socket->connectToHost(QHostAddress(host), port);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "[Client] Failed to connect!";
    } else {
        qDebug() << "[Client] Connected to server!";
    }
}

void NetworkClient::sendMessage(QString username, QString message) {
    ChatMessage msg;
    msg.msg_type = MSG_CHAT;
    strncpy(msg.sender, username.toStdString().c_str(), sizeof(msg.sender));
    strncpy(msg.message, message.toStdString().c_str(), sizeof(msg.message));

    socket->write(reinterpret_cast<char*>(&msg), sizeof(ChatMessage));
}

void NetworkClient::sendAddFriendRequest(QString requester, QString target) {
    AddFriendMessage req;
    req.msg_type = MSG_ADD_FRIEND;
    strncpy(req.requester, requester.toStdString().c_str(), sizeof(req.requester));
    strncpy(req.target, target.toStdString().c_str(), sizeof(req.target));

    socket->write(reinterpret_cast<char*>(&req), sizeof(AddFriendMessage));
}


void NetworkClient::sendListFriendsRequest(QString username) {
    FriendListMessage req;
    req.msg_type = MSG_FRIEND_LIST;
    strncpy(req.username, username.toStdString().c_str(), sizeof(req.username));

    socket->write(reinterpret_cast<char*>(&req), sizeof(FriendListMessage));
}

void NetworkClient::sendPrivateMessage(QString sender, QString receiver, QString message) {
    PrivateMessage msg;
    msg.msg_type = MSG_PRIVATE_CHAT;
    strncpy(msg.from, sender.toStdString().c_str(), sizeof(msg.from));
    strncpy(msg.to, receiver.toStdString().c_str(), sizeof(msg.to));
    strncpy(msg.message, message.toStdString().c_str(), sizeof(msg.message));

    socket->write(reinterpret_cast<char*>(&msg), sizeof(PrivateMessage));
}


void NetworkClient::onReadyRead() {
    char buffer[sizeof(FriendListMessage)];
    socket->read(buffer, sizeof(buffer));

    int msg_type = *(int*)buffer;

    switch (msg_type) {
        case MSG_CHAT: {
            ChatMessage* msg = reinterpret_cast<ChatMessage*>(buffer);
            emit messageReceived(QString("[%1]: %2").arg(msg->sender, msg->message));
            break;
        }
        case MSG_PRIVATE_CHAT: {
            PrivateMessage* msg = reinterpret_cast<PrivateMessage*>(buffer);
            emit messageReceived(QString("[私聊] %1 -> 我: %2").arg(msg->from, msg->message));
            break;
        }
        case MSG_RESULT: {
            ResultMessage* res = reinterpret_cast<ResultMessage*>(buffer);
            emit messageReceived(QString("[系统] %1").arg(res->message));
            break;
        }
        case MSG_FRIEND_LIST_RET: {
            FriendListMessage* list = reinterpret_cast<FriendListMessage*>(buffer);
            emit messageReceived(QString("[好友列表]\n%1").arg(list->list));
            break;
        }
        default:
            emit messageReceived("[未知消息类型]");
            break;
    }
}


