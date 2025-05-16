#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "auth_manager.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccess(QString username);

private slots:
    void on_btnLogin_clicked();
    void on_btnRegister_clicked();

private:
    Ui::LoginWindow *ui;
    AuthManager *auth;
};

#endif // LOGINWINDOW_H

