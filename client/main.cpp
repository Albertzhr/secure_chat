#include <QApplication>
#include "loginwindow.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginWindow loginWin;
    MainWindow mainWin;

    // 连接登录成功信号，打开主窗口
    QObject::connect(&loginWin, &LoginWindow::loginSuccess, [&](QString username){
        mainWin.setUsername(username);  // 假设你在 MainWindow 中定义了这个方法
        mainWin.show();
    });

    loginWin.show();
    return app.exec();
}

