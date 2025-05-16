/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineEditUsername;
    QLabel *label_2;
    QLineEdit *lineEditPassword;
    QLabel *labelStatus;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnLogin;
    QPushButton *btnRegister;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName(QString::fromUtf8("LoginWindow"));
        verticalLayout = new QVBoxLayout(LoginWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(LoginWindow);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        lineEditUsername = new QLineEdit(LoginWindow);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));

        verticalLayout->addWidget(lineEditUsername);

        label_2 = new QLabel(LoginWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        lineEditPassword = new QLineEdit(LoginWindow);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(lineEditPassword);

        labelStatus = new QLabel(LoginWindow);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        labelStatus->setStyleSheet(QString::fromUtf8("color: red;"));

        verticalLayout->addWidget(labelStatus);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnLogin = new QPushButton(LoginWindow);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));

        horizontalLayout->addWidget(btnLogin);

        btnRegister = new QPushButton(LoginWindow);
        btnRegister->setObjectName(QString::fromUtf8("btnRegister"));

        horizontalLayout->addWidget(btnRegister);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "\347\231\273\345\275\225 - Secure Chat", nullptr));
        label->setText(QCoreApplication::translate("LoginWindow", "\347\224\250\346\210\267\345\220\215", nullptr));
        label_2->setText(QCoreApplication::translate("LoginWindow", "\345\257\206\347\240\201", nullptr));
        labelStatus->setText(QString());
        btnLogin->setText(QCoreApplication::translate("LoginWindow", "\347\231\273\345\275\225", nullptr));
        btnRegister->setText(QCoreApplication::translate("LoginWindow", "\346\263\250\345\206\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
