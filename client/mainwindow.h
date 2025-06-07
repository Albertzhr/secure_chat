#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 用户/好友相关
    void on_btnRegister_clicked();
    void on_btnLogin_clicked();
    void on_btnAddFriend_clicked();
    void on_btnRefreshFriend_clicked();
    void on_btnCheckRequests_clicked();
    void on_btnSendMessage_clicked();

    // 群聊相关
    void on_btnCreateGroup_clicked();
    void on_btnJoinGroup_clicked();
    void on_btnRefreshGroupList_clicked();
    void on_btnGroupSendMsg_clicked();
    void on_btnFetchGroupMsg_clicked();
    
    void on_btnLogout_clicked();
    void on_btnAuditLog_clicked();

    void onReadyRead();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString currentUsername;

    void connectToServer();
    void showResult(const ResultMessage &res);
    void showFriendList(const FriendListMessage &msg);
    void showPendingRequests(const PendingFriendListMessage &msg);
    void showIncomingMessage(const QString &from, const QString &plain);

    // 群聊相关
    void showGroupList(const GroupListMessage &msg);
    void showGroupIncomingMessage(const GroupChatMessage &msg);
    void showGroupHistory(const FetchGroupChatRespMessage &resp);

    // 加密/解密
    void my_sm4_encrypt(const QByteArray &plain, unsigned char *cipher, int &cipher_len);
    void my_sm4_decrypt(const unsigned char *cipher, int cipher_len, QByteArray &plain);
};

#endif // MAINWINDOW_H
