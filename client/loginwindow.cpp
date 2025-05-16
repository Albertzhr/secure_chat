#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow),
    auth(new AuthManager(this)) {
    ui->setupUi(this);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::on_btnLogin_clicked() {
    QString user = ui->lineEditUsername->text();
    QString pass = ui->lineEditPassword->text();
    if (auth->login(user, pass)) {
        emit loginSuccess(user);
        this->close();
    } else {
        ui->labelStatus->setText("登录失败：用户名或密码错误");
    }
}

void LoginWindow::on_btnRegister_clicked() {
    QString user = ui->lineEditUsername->text();
    QString pass = ui->lineEditPassword->text();
    if (auth->registerUser(user, pass)) {
        ui->labelStatus->setText("注册成功，请登录");
    } else {
        ui->labelStatus->setText("注册失败：用户已存在");
    }
}

