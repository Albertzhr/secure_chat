#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "network_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 设置当前用户名
    void setUsername(const QString &uname);

private slots:
    void on_btnSend_clicked();
    void onMessageReceived(QString message);

    // ✅ 新增：三个操作按钮的槽函数
    void on_btnAddFriend_clicked();      // 添加好友
    void on_btnListFriends_clicked();    // 查看好友列表
    void on_btnSendPrivate_clicked();    // 发送私聊消息

private:
    Ui::MainWindow *ui;
    NetworkClient *client;
    QString username;
};

#endif // MAINWINDOW_H

