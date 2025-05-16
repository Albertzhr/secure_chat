#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "../common/protocol.h"

class NetworkClient : public QObject {
    Q_OBJECT

public:
    explicit NetworkClient(QObject* parent = nullptr);
    void connectToServer(QString host, int port);

    void sendMessage(QString username, QString message);
    void sendAddFriendRequest(QString username, QString friendName);  // 添加好友
    void sendListFriendsRequest(QString username);                    // 查询好友列表
    void sendPrivateMessage(QString sender, QString receiver, QString message); // 私聊

signals:
    void messageReceived(QString message);    // 通用消息通知
    void systemMessage(QString message);      // 系统通知（好友列表等）
    void privateMessage(QString from, QString message); // 私聊消息

private slots:
    void onReadyRead();

private:
    QTcpSocket* socket;
};

#endif // NETWORK_CLIENT_H

