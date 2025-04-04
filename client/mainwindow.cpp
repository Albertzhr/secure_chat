#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    client = new NetworkClient(this);
    client->connectToServer("127.0.0.1", 6666);

    connect(client, &NetworkClient::messageReceived,
            this, &MainWindow::onMessageReceived);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btnSend_clicked() {
    QString user = ui->lineEditUsername->text();
    QString msg = ui->lineEditMessage->text();
    if (user.isEmpty() || msg.isEmpty()) return;

    client->sendMessage(user, msg);
    ui->lineEditMessage->clear();
}

void MainWindow::onMessageReceived(QString message) {
    ui->textBrowserChat->append(message);
}

