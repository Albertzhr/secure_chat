#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QByteArray>
#include <string.h>
#include <gmssl/sm4.h>

static unsigned char sm4_key[16] = {'t','h','i','s','I','s','T','e','s','t','K','e','y','1','2','3'};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connectToServer();
    ui->stackWidget->setCurrentIndex(0);

    // 关联槽函数
    connect(ui->btnRegister,        &QPushButton::clicked, this, &MainWindow::on_btnRegister_clicked);
    connect(ui->btnLogin,           &QPushButton::clicked, this, &MainWindow::on_btnLogin_clicked);
    connect(ui->btnAddFriend,       &QPushButton::clicked, this, &MainWindow::on_btnAddFriend_clicked);
    connect(ui->btnRefreshFriend,   &QPushButton::clicked, this, &MainWindow::on_btnRefreshFriend_clicked);
    connect(ui->btnCheckRequests,   &QPushButton::clicked, this, &MainWindow::on_btnCheckRequests_clicked);
    connect(ui->btnSendMessage,     &QPushButton::clicked, this, &MainWindow::on_btnSendMessage_clicked);

    connect(ui->btnCreateGroup,     &QPushButton::clicked, this, &MainWindow::on_btnCreateGroup_clicked);
    connect(ui->btnJoinGroup,       &QPushButton::clicked, this, &MainWindow::on_btnJoinGroup_clicked);
    connect(ui->btnRefreshGroupList,&QPushButton::clicked, this, &MainWindow::on_btnRefreshGroupList_clicked);
    connect(ui->btnGroupSendMsg,    &QPushButton::clicked, this, &MainWindow::on_btnGroupSendMsg_clicked);
    connect(ui->btnFetchGroupMsg,   &QPushButton::clicked, this, &MainWindow::on_btnFetchGroupMsg_clicked);

    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::on_btnLogout_clicked);
    connect(ui->btnAuditLog, &QPushButton::clicked, this, &MainWindow::on_btnAuditLog_clicked);

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::connectToServer()
{
    if (socket->state() != QAbstractSocket::UnconnectedState) return;
    socket->connectToHost("127.0.0.1", 6666);
    if (!socket->waitForConnected(2000))
        QMessageBox::critical(this, "错误", "无法连接服务器");
}

// ========== 用户/好友区 ==========
void MainWindow::on_btnRegister_clicked()
{
    AuthMessage msg = {0};
    msg.type = MSG_REGISTER;
    QByteArray user = ui->editUser->text().toUtf8();
    QByteArray pwd = ui->editPass->text().toUtf8();
    strncpy(msg.username, user.constData(), sizeof(msg.username)-1);
    strncpy(msg.password, pwd.constData(), sizeof(msg.password)-1);
    memset(msg.sm2_pubkey, 0, sizeof(msg.sm2_pubkey));
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnLogin_clicked()
{
    AuthMessage msg = {0};
    msg.type = MSG_LOGIN;
    QByteArray user = ui->editUser->text().toUtf8();
    QByteArray pwd = ui->editPass->text().toUtf8();
    strncpy(msg.username, user.constData(), sizeof(msg.username)-1);
    strncpy(msg.password, pwd.constData(), sizeof(msg.password)-1);
    memset(msg.sm2_pubkey, 0, sizeof(msg.sm2_pubkey));
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
    currentUsername = ui->editUser->text();
}

void MainWindow::on_btnAddFriend_clicked()
{
    QString friendName = QInputDialog::getText(this, "添加好友", "输入好友用户名");
    if (friendName.isEmpty()) return;
    FriendRequestMessage msg = {0};
    msg.type = MSG_ADD_FRIEND_REQ;
    strncpy(msg.from, currentUsername.toUtf8().data(), sizeof(msg.from)-1);
    strncpy(msg.to, friendName.toUtf8().data(), sizeof(msg.to)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnRefreshFriend_clicked()
{
    FriendListMessage msg = {0};
    msg.type = MSG_FRIEND_LIST_REQ;
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnCheckRequests_clicked()
{
    AuthMessage msg = {0};
    msg.type = MSG_PENDING_FRIEND_REQ;
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnSendMessage_clicked()
{
    QListWidgetItem *item = ui->friendListWidget->currentItem();
    if (!item) return;
    QString target = item->text();
    QString text = QInputDialog::getText(this, "发送消息", QString("给 [%1] 发消息:").arg(target));
    if (text.isEmpty()) return;

    QByteArray plain = text.toUtf8();
    unsigned char cipher[CHAT_MAX_LEN];
    int cipher_len = 0;
    my_sm4_encrypt(plain, cipher, cipher_len);

    PrivateChatMessage msg = {0};
    msg.type = MSG_PRIVATE_CHAT;
    strncpy(msg.from, currentUsername.toUtf8().data(), sizeof(msg.from)-1);
    strncpy(msg.to, target.toUtf8().data(), sizeof(msg.to)-1);
    memcpy(msg.sm4_cipher, cipher, cipher_len);
    msg.cipher_len = cipher_len;

    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

// ========== 群聊区 ==========
void MainWindow::on_btnCreateGroup_clicked()
{
    QString gname = QInputDialog::getText(this, "新建群聊", "群名");
    if (gname.isEmpty()) return;
    GroupCreateMessage msg = {0};
    msg.type = MSG_GROUP_CREATE;
    strncpy(msg.groupname, gname.toUtf8().data(), sizeof(msg.groupname)-1);
    strncpy(msg.creator, currentUsername.toUtf8().data(), sizeof(msg.creator)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnJoinGroup_clicked()
{
    QString gname = QInputDialog::getText(this, "加入群聊", "群名");
    if (gname.isEmpty()) return;
    GroupJoinMessage msg = {0};
    msg.type = MSG_GROUP_JOIN;
    strncpy(msg.groupname, gname.toUtf8().data(), sizeof(msg.groupname)-1);
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnRefreshGroupList_clicked()
{
    AuthMessage msg = {0};
    msg.type = MSG_GROUP_LIST_REQ;
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnGroupSendMsg_clicked()
{
    QListWidgetItem *item = ui->groupListWidget->currentItem();
    if (!item) return;
    QString gname = item->text();
    QString text = QInputDialog::getText(this, "群聊发消息", "内容");
    if (text.isEmpty()) return;

    QByteArray plain = text.toUtf8();
    unsigned char cipher[CHAT_MAX_LEN];
    int cipher_len = 0;
    my_sm4_encrypt(plain, cipher, cipher_len);

    GroupChatMessage msg = {0};
    msg.type = MSG_GROUP_SEND_MSG;
    strncpy(msg.groupname, gname.toUtf8().data(), sizeof(msg.groupname)-1);
    strncpy(msg.sender, currentUsername.toUtf8().data(), sizeof(msg.sender)-1);
    memcpy(msg.sm4_cipher, cipher, cipher_len);
    msg.cipher_len = cipher_len;

    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

void MainWindow::on_btnFetchGroupMsg_clicked()
{
    QListWidgetItem *item = ui->groupListWidget->currentItem();
    if (!item) return;
    QString gname = item->text();

    GroupJoinMessage msg = {0};
    msg.type = MSG_FETCH_GROUP_MSG;
    strncpy(msg.groupname, gname.toUtf8().data(), sizeof(msg.groupname)-1);
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}

// ========== 加密/解密 ==========
void MainWindow::my_sm4_encrypt(const QByteArray &plain, unsigned char *cipher, int &cipher_len)
{
    SM4_KEY key;
    sm4_set_encrypt_key(&key, sm4_key);
    int inlen = plain.size();
    int blocks = (inlen + 15) / 16;
    cipher_len = blocks * 16;
    for (int i = 0; i < blocks; ++i) {
        unsigned char inbuf[16] = {0}, outbuf[16] = {0};
        int blen = (i == blocks - 1 && inlen % 16) ? inlen % 16 : 16;
        memcpy(inbuf, plain.data() + i * 16, blen);
        sm4_encrypt(&key, inbuf, outbuf);
        memcpy(cipher + i * 16, outbuf, 16);
    }
}
void MainWindow::my_sm4_decrypt(const unsigned char *cipher, int cipher_len, QByteArray &plain)
{
    SM4_KEY key;
    sm4_set_decrypt_key(&key, sm4_key);
    int blocks = cipher_len / 16;
    QByteArray buf(cipher_len, 0);
    for (int i = 0; i < blocks; ++i) {
        sm4_decrypt(&key, cipher + i * 16, (unsigned char*)buf.data() + i * 16);
    }
    plain = buf;
}

// ========== 网络消息处理 ==========
void MainWindow::onReadyRead()
{
    QByteArray data = socket->readAll();
    int type = *(int*)data.constData();

    if (type == MSG_RESULT) {
        const ResultMessage *res = reinterpret_cast<const ResultMessage*>(data.constData());
        showResult(*res);
    } else if (type == MSG_FRIEND_LIST_RESP) {
        const FriendListMessage *msg = reinterpret_cast<const FriendListMessage*>(data.constData());
        showFriendList(*msg);
    } else if (type == MSG_PENDING_FRIEND_LIST) {
        const PendingFriendListMessage *msg = reinterpret_cast<const PendingFriendListMessage*>(data.constData());
        showPendingRequests(*msg);
    } else if (type == MSG_CHAT_PUSH) {
        const ChatPushMessage *msg = reinterpret_cast<const ChatPushMessage*>(data.constData());
        QByteArray plain;
        my_sm4_decrypt(msg->sm4_cipher, msg->cipher_len, plain);
        showIncomingMessage(QString::fromUtf8(msg->from), QString::fromUtf8(plain));
    } else if (type == MSG_FETCH_CHAT_RESP) {
        const FetchChatRespMessage *resp = reinterpret_cast<const FetchChatRespMessage*>(data.constData());
        for (int i = 0; i < resp->count; ++i) {
            const PrivateChatMessage &msg = resp->messages[i];
            QByteArray plain;
            my_sm4_decrypt(msg.sm4_cipher, msg.cipher_len, plain);
            showIncomingMessage(QString::fromUtf8(msg.from), QString::fromUtf8(plain));
        }
    }
    // ===== 群聊相关 =====
    else if (type == MSG_GROUP_LIST_RESP) {
        const GroupListMessage *msg = reinterpret_cast<const GroupListMessage*>(data.constData());
        showGroupList(*msg);
    } else if (type == MSG_GROUP_PUSH_MSG) {
        const GroupChatMessage *msg = reinterpret_cast<const GroupChatMessage*>(data.constData());
        showGroupIncomingMessage(*msg);  // 传引用
    }else if (type == MSG_FETCH_GROUP_RESP) {
        const FetchGroupChatRespMessage *resp = reinterpret_cast<const FetchGroupChatRespMessage*>(data.constData());
        showGroupHistory(*resp);
    }else if (type == MSG_LOG_AUDIT_RESP) {
        const LogAuditRespMessage *resp = reinterpret_cast<const LogAuditRespMessage*>(data.constData());
        if (resp->success) {
            QMessageBox::information(this, "审计日志", QString::fromUtf8(resp->content));
        } else {
            QMessageBox::warning(this, "权限不足", QString::fromUtf8(resp->message));
        }
    }
}

void MainWindow::showResult(const ResultMessage &res)
{
    if (res.success) {
        QMessageBox::information(this, "结果", QString::fromUtf8(res.message));
        if (QString::fromUtf8(res.message).contains("登录") && res.success) {
            ui->stackWidget->setCurrentIndex(1);
            on_btnRefreshFriend_clicked();
            on_btnRefreshGroupList_clicked();
            // 登录后自动拉取所有未读私聊
            FetchChatMessage req = {0};
            req.type = MSG_FETCH_CHAT;
            strncpy(req.username, currentUsername.toUtf8().data(), sizeof(req.username)-1);
            socket->write(reinterpret_cast<char*>(&req), sizeof(req));
        }
    }
    else
        QMessageBox::warning(this, "失败", QString::fromUtf8(res.message));
}

void MainWindow::showFriendList(const FriendListMessage &msg)
{
    ui->friendListWidget->clear();
    for (int i = 0; i < msg.count; ++i)
        ui->friendListWidget->addItem(QString::fromUtf8(msg.friends[i]));
}

void MainWindow::showPendingRequests(const PendingFriendListMessage &msg)
{
    if (msg.count == 0) {
        QMessageBox::information(this, "好友请求", "暂无新的好友请求");
        return;
    }
    for (int i = 0; i < msg.count; ++i) {
        QString applicant = QString::fromUtf8(msg.applicants[i]);
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "好友请求",
            QString("用户 [%1] 请求添加你为好友，是否同意？").arg(applicant),
            QMessageBox::Yes | QMessageBox::No
        );
        FriendResponseMessage resp = {0};
        resp.type = MSG_ADD_FRIEND_RESP;
        strncpy(resp.from, currentUsername.toUtf8().data(), sizeof(resp.from)-1);
        strncpy(resp.to, applicant.toUtf8().data(), sizeof(resp.to)-1);
        resp.accept = (reply == QMessageBox::Yes) ? 1 : 0;
        socket->write(reinterpret_cast<char*>(&resp), sizeof(resp));
    }
}

void MainWindow::showIncomingMessage(const QString &from, const QString &plain)
{
    QMessageBox::information(this, QString("收到来自[%1]的消息").arg(from), plain);
}

// ========== 群聊UI辅助 ==========
void MainWindow::showGroupList(const GroupListMessage &msg)
{
    ui->groupListWidget->clear();
    for (int i = 0; i < msg.count; ++i)
        ui->groupListWidget->addItem(QString::fromUtf8(msg.groups[i]));
}

void MainWindow::showGroupIncomingMessage(const GroupChatMessage &msg)
{
    QByteArray plain;
    my_sm4_decrypt(msg.sm4_cipher, msg.cipher_len, plain);
    QMessageBox::information(this,
        QString("群[%1]消息").arg(msg.groupname),
        QString("[%1]: %2").arg(msg.sender, QString::fromUtf8(plain)));
}


void MainWindow::showGroupHistory(const FetchGroupChatRespMessage &resp)
{
    for (int i = 0; i < resp.count; ++i) {
        const GroupChatMessage &msg = resp.messages[i];
        QByteArray plain;
        my_sm4_decrypt(msg.sm4_cipher, msg.cipher_len, plain);
        QMessageBox::information(this,
            QString("群[%1]历史").arg(msg.groupname),
            QString("[%1]: %2").arg(msg.sender, QString::fromUtf8(plain)));
    }
}

void MainWindow::on_btnLogout_clicked()
{
    // 1. 可选：通知服务器退出（如有实现MSG_LOGOUT）
    /*
    LogoutMessage msg = {0};
    msg.type = MSG_LOGOUT;
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
    */

    // 2. 断开 socket
    socket->disconnectFromHost();
    socket->waitForDisconnected(1000); // 最多等1s

    // 3. 清空所有状态
    currentUsername.clear();
    ui->editUser->clear();
    ui->editPass->clear();
    ui->friendListWidget->clear();
    if (ui->groupListWidget)
        ui->groupListWidget->clear();

    // 4. 回到登录界面
    ui->stackWidget->setCurrentIndex(0);

    // 5. 重新连接服务端
    connectToServer();
}

void MainWindow::on_btnAuditLog_clicked()
{
    LogAuditReqMessage msg = {0};
    msg.type = MSG_LOG_AUDIT_REQ;
    strncpy(msg.username, currentUsername.toUtf8().data(), sizeof(msg.username)-1);
    socket->write(reinterpret_cast<char*>(&msg), sizeof(msg));
}


