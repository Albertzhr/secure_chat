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
    msg.msg_type = 0;
    strncpy(msg.sender, username.toStdString().c_str(), sizeof(msg.sender));
    strncpy(msg.message, message.toStdString().c_str(), sizeof(msg.message));

    socket->write(reinterpret_cast<char*>(&msg), sizeof(ChatMessage));
}

void NetworkClient::onReadyRead() {
    ChatMessage msg;
    socket->read(reinterpret_cast<char*>(&msg), sizeof(ChatMessage));
    emit messageReceived(QString("[%1]: %2").arg(msg.sender).arg(msg.message));
}

