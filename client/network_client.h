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

signals:
    void messageReceived(QString message);

private slots:
    void onReadyRead();

private:
    QTcpSocket* socket;
};

#endif

