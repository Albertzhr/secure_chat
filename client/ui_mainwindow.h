/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowserChat;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEditMessage;
    QPushButton *btnSend;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnAddFriend;
    QPushButton *btnListFriends;
    QPushButton *btnSendPrivate;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(274, 320);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textBrowserChat = new QTextBrowser(centralwidget);
        textBrowserChat->setObjectName(QString::fromUtf8("textBrowserChat"));

        verticalLayout->addWidget(textBrowserChat);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEditMessage = new QLineEdit(centralwidget);
        lineEditMessage->setObjectName(QString::fromUtf8("lineEditMessage"));

        horizontalLayout->addWidget(lineEditMessage);

        btnSend = new QPushButton(centralwidget);
        btnSend->setObjectName(QString::fromUtf8("btnSend"));

        horizontalLayout->addWidget(btnSend);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btnAddFriend = new QPushButton(centralwidget);
        btnAddFriend->setObjectName(QString::fromUtf8("btnAddFriend"));

        horizontalLayout_2->addWidget(btnAddFriend);

        btnListFriends = new QPushButton(centralwidget);
        btnListFriends->setObjectName(QString::fromUtf8("btnListFriends"));

        horizontalLayout_2->addWidget(btnListFriends);

        btnSendPrivate = new QPushButton(centralwidget);
        btnSendPrivate->setObjectName(QString::fromUtf8("btnSendPrivate"));

        horizontalLayout_2->addWidget(btnSendPrivate);


        verticalLayout->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Secure Chat", nullptr));
        lineEditMessage->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\346\266\210\346\201\257...", nullptr));
        btnSend->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        btnAddFriend->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
        btnListFriends->setText(QCoreApplication::translate("MainWindow", "\345\245\275\345\217\213\345\210\227\350\241\250", nullptr));
        btnSendPrivate->setText(QCoreApplication::translate("MainWindow", "\347\247\201\350\201\212\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
