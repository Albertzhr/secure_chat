#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 初始化客户端并连接服务器
    client = new NetworkClient(this);
    client->connectToServer("127.0.0.1", 6666);

    // 接收消息连接
    connect(client, &NetworkClient::messageReceived,
            this, &MainWindow::onMessageReceived);

    // 新增：连接好友按钮
    connect(ui->btnAddFriend, &QPushButton::clicked, this, &MainWindow::on_btnAddFriend_clicked);
    connect(ui->btnListFriends, &QPushButton::clicked, this, &MainWindow::on_btnListFriends_clicked);
    connect(ui->btnSendPrivate, &QPushButton::clicked, this, &MainWindow::on_btnSendPrivate_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setUsername(const QString &uname) {
    username = uname;
    setWindowTitle("Secure Chat - " + uname);
}

void MainWindow::on_btnSend_clicked() {
    QString msg = ui->lineEditMessage->text();
    if (msg.isEmpty()) return;

    client->sendMessage(username, msg);
    ui->textBrowserChat->append("我: " + msg);
    ui->lineEditMessage->clear();
}

void MainWindow::onMessageReceived(QString message) {
    ui->textBrowserChat->append(message);
}

// ✅ 新增：添加好友
void MainWindow::on_btnAddFriend_clicked() {
    QString friendName = QInputDialog::getText(this, "添加好友", "好友用户名：");
    if (friendName.isEmpty()) return;

    client->sendAddFriendRequest(username, friendName);
}

// ✅ 新增：查看好友列表
void MainWindow::on_btnListFriends_clicked() {
    client->sendListFriendsRequest(username);
}

// ✅ 新增：私聊功能
void MainWindow::on_btnSendPrivate_clicked() {
    QString friendName = QInputDialog::getText(this, "发送私聊", "接收人用户名：");
    if (friendName.isEmpty()) return;

    QString msg = ui->lineEditMessage->text();
    if (msg.isEmpty()) return;

    client->sendPrivateMessage(username, friendName, msg);
    ui->textBrowserChat->append(QString("我 -> %1: %2").arg(friendName, msg));
    ui->lineEditMessage->clear();
}

